// Lab.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab.h"
#define MAX_LOADSTRING 100

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "fltk_config.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/gl.h>
#include <FL/math.h>

#include <fltk/Browser.h>
#include <fltk/CheckButton.h>
#include <fltk/ColorChooser.h>
#include <fltk/file_chooser.h>
#include <fltk/GLWindow.h>
#include <fltk/Input.h>
#include <fltk/Item.h>
#include <fltk/ItemGroup.h>
#include <fltk/PopupMenu.h>
#include <fltk/Run.h>
#include <fltk/ScrollGroup.h>
#include <fltk/Window.h>
#include <fltk/xpmImage.h>

#include "ColorButton.h"	// not really part of FLTK
#include "PMath.h"
#include "GraphObj.h"
#include "ML_DrawObject.h"
#include "ML_Camera.h"
#include "ML_Material.h"
#include "ML_File.h"
#include "Scene.h"

#include "folder_small.xpm"
#include "file_small.xpm"
#include "camera.xpm"
#include "teapot.xpm"
#include "set.xpm"

using namespace PMath;

fltk::xpmImage* folderSmall;
fltk::xpmImage* fileSmall;

#define PLAY_LABEL "@+4>"
#define PAUSE_LABEL "@+9||"
#define ZEROTIME_LABEL "00:00:00:00"

class RenderWindow : public fltk::GlWindow {
	class RenderArea : public Fl_Box {
		void	draw() { }							// override the standard Box behavior
		int		idle() { }
		int		handle(int event);

	public:
		RenderArea(Scene* pScene, int x,int y,int w,int h) : Fl_Box(x,y,w,h) {
		}
	};

public:
	RenderWindow(Scene* pScene, int x,int y,int w,int h,const char *l=0) :
		fltk::GlWindow(x, y, w, h), mp_Scene(pScene)  {
		mode(Fl_Mode(FL_DOUBLE | FL_RGB));								// set double buffered, RGB
		begin();
		mp_InteractiveArea = new RenderArea(pScene, x, y, w, h);
		end();
		resizable(this);												// indicates that the RenderArea is resized whenever the RenderWindow is resized
	}

	virtual ~RenderWindow() { delete mp_InteractiveArea; }

private:
	RenderArea*		mp_InteractiveArea;

	void draw();
	Scene*		mp_Scene;
};

class WorkspaceWindow : public fltk::Window {
public:
	WorkspaceWindow(Scene* pScene, int w,int h, const char *l=0);
	virtual ~WorkspaceWindow();

	Scene*				GetScene() const { return mp_Scene; }
	Renderer*			GetRenderer() const { return mp_Scene->mp_Renderer; }
	fltk::Input*		mp_StatusLabel;

	RenderWindow*		mp_RenderWindow;

private:
	Scene*				mp_Scene;
};

WorkspaceWindow::WorkspaceWindow(Scene* pScene, int w, int h, const char *l) :
	mp_Scene(pScene), 
	fltk::Window(w,h, l) 
{
}

WorkspaceWindow :: ~WorkspaceWindow() {
}

static WorkspaceWindow* sWorkspace = 0;


static void DrawObjectCues(DrawObject* pProp, PMath::Vec3f boxColor) {
	float temp[16];
	PMath::Vec3f corners[8];

	// draw box
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pProp->LoadMatrix();
		glColor3f(boxColor[0], boxColor[1], boxColor[2]);
		pProp->DrawBoundingBox();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// draw corners
	DiffuseMaterial* pMat = (DiffuseMaterial*) gScene->m_IcosaModel->GetMaterial();
	pMat->m_Diffuse[0] = 0.7f;
	pMat->m_Diffuse[1] = 0.7f;
	pMat->m_Diffuse[2] = 0.1f;
	gScene->m_IcosaModel->BindMaterial();

	pProp->GetWorldBoundingCorners(corners);
	PMath::Mat44Identity(temp);
	PMath::Mat44Scale3x3(temp, 0.1f);
	for (int i = 0; i < 8; ++i) {
		temp[12] = corners[i][0];
		temp[13] = corners[i][1];
		temp[14] = corners[i][2];
		gScene->m_IcosaModel->SetMatrix(temp);
		gScene->mp_Renderer->Render(gScene->m_IcosaModel);
	}
	gScene->m_IcosaModel->UnbindMaterial();
}

static void DrawCameraCues(Camera* pCamera, DrawObject* pMesh, PMath::Vec3f boxColor) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pMesh->LoadMatrix();
		glColor3f(boxColor[0], boxColor[1], boxColor[2]);
		pMesh->DrawBoundingBox();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	Real distance = PMath::Vec3fDistance(pCamera->m_Position, pCamera->m_Interest);
	int segs = (int) distance;
	float remainder = distance - (float) segs;
	PMath::Vec3f seg;
	PMath::Vec3fSubtract(seg, pCamera->m_Interest, pCamera->m_Position);
	PMath::Vec3fSetLength(seg, seg, kHalf);
	PMath::Vec3f point;
	PMath::Vec3fSet(point, pCamera->m_Position);
	glColor3f(1.0f, 0.0f, 0.1f);
	glBegin(GL_LINES);
		for (int i = 0; i < segs; ++i) {
			glVertex3fv(point);
			PMath::Vec3fAdd(point, point, seg);
			glVertex3fv(point);
			PMath::Vec3fAdd(point, point, seg);
		}
	glEnd();
}

void RenderWindow::draw() {
	int i;
	float temp[16];
	PMath::Vec3f boxColor;

	// has the window been resized?
	if (!valid()) {
		valid(1);
		mp_Scene->mp_Renderer->SetViewport(0.0f, 0.0f, (float) w(), (float) h());
		mp_InteractiveArea->resize(0, 0, w(), h());
	}

	mp_Scene->mp_Renderer->BeginScene();
	mp_Scene->m_SetList.Render();
	mp_Scene->m_PropList.Render();

	Scene::WOT* pPrimary = mp_Scene->PrimarySelect();
	if (pPrimary->Kind() == Scene::kSelectProp) {
		boxColor[0] = 0.1f;
		boxColor[1] = 1.0f;
		boxColor[2] = 0.1f;
		DrawObjectCues(pPrimary->mp_Drawn, boxColor);
	}

	Scene::WOT* pSecondary = mp_Scene->SecondarySelect();
	if (pSecondary->Kind() == Scene::kSelectProp) {
		boxColor[0] = 1.0f;
		boxColor[1] = 1.0f;
		boxColor[2] = 0.1f;
		DrawObjectCues(pSecondary->mp_Drawn, boxColor);
	}

	Scene::WOT* pPotential = mp_Scene->PotentialSelect();
	if (pPrimary != pPotential && pSecondary != pPotential && pPotential->Kind() != Scene::kSelectNone) {
		boxColor[0] = 1.0f;
		boxColor[1] = 0.1f;
		boxColor[2] = 0.1f;
		DrawObjectCues(pPotential->mp_Drawn, boxColor);
	}

	for (i = 0; i < mp_Scene->m_CameraList.size(); ++i) {
		Scene::CameraWOT* pCWOT = static_cast<Scene::CameraWOT*>(mp_Scene->m_CameraList.m_Objects[i]);
		Camera* pCamera = &pCWOT->m_Camera;
		if (pCamera != mp_Scene->mp_Renderer->CurrentCamera()) {
			pCamera->GetCameraMatrix(temp);
			temp[12] = pCamera->m_Position[0];
			temp[13] = pCamera->m_Position[1];
			temp[14] = pCamera->m_Position[2];
			PMath::Mat44Transpose3x3(temp);
			PMath::Mat44Scale3x3(temp, 0.5f);
			mp_Scene->m_CameraModel->SetMatrix(temp);
			mp_Scene->mp_Renderer->Render(mp_Scene->m_CameraModel);

			if (pCWOT == mp_Scene->PrimarySelect()) {
				boxColor[0] = 0.1f;
				boxColor[1] = 1.0f;
				boxColor[2] = 0.1f;
				DrawCameraCues(pCamera, mp_Scene->m_CameraModel, boxColor);
			}
		}
	}

	if (mp_Scene->m_SpringData.size() > 0) {
		glBegin(GL_LINES);
		for (i = 0; i < (int) mp_Scene->m_SpringData.size(); ++i) {
			PMath::Vec3f posA, posB;
			mp_Scene->m_SpringData[i]->m_BodyA->mp_Drawn->GetWorldPosition(posA);
			mp_Scene->m_SpringData[i]->m_BodyB->mp_Drawn->GetWorldPosition(posB);
			if (mp_Scene->m_SpringData[i]->m_ResistCompress) {
				glColor3f(1.0f, 0.0f, 0.0f);
			}
			else {
				glColor3f(0.0f, 1.0f, 0.0f);
			}
			glVertex3f(posA[0], posA[1], posA[2]);
			glVertex3f(posB[0], posB[1], posB[2]);
		}
		glEnd();
	}

	mp_Scene->mp_Renderer->EndScene();
}


static void ChangePrimaryName(fltk::Widget* pWidget, void* pWindow) {
	fltk::Input* pInput = (fltk::Input*) pWidget;
	Scene::WOT* pPrimary = gScene->PrimarySelect();
	if (pPrimary->Kind() != Scene::kSelectNone) {
		strncpy(pPrimary->m_Name, pInput->value(), 31);
		pPrimary->m_Name[31] = '\0';
		pPrimary->mp_Widget->label(pPrimary->m_Name);
		gScene->m_Browser->redraw();
	}
}

static void Rewind_cb(fltk::Widget* pWidget, void* pWindow) {
	gScene->m_Play->label(PLAY_LABEL);
	gScene->m_Play->redraw();
	gScene->m_Clock->setPausedState(true);
	gScene->m_Clock->reset();
	gScene->m_TimeDisplay->value(ZEROTIME_LABEL);
}

static void Play_cb(fltk::Widget* pWidget, void* pWindow) {
	if (gScene->m_Clock->getPausedState()) {
		gScene->m_Play->label(PAUSE_LABEL);
		gScene->m_Clock->setPausedState(false);
	}
	else {
		gScene->m_Play->label(PLAY_LABEL);
		gScene->m_Clock->setPausedState(true);
	}
}

static void ActivateCamera_cb(fltk::Widget* pWidget, void* pWindow) {
	if (gScene->PrimarySelect()->Kind() == Scene::kSelectCamera) {
		Scene::CameraWOT* pCameraWOT = static_cast<Scene::CameraWOT*>(gScene->PrimarySelect());
		gScene->mp_Renderer->CurrentCamera(&pCameraWOT->m_Camera);
	}
}

static void SetIsCollider_cb(fltk::Widget* pWidget, void* pWindow) {
	if (gScene->PrimarySelect()->Kind() == Scene::kSelectSet) {
		Scene::SetWOT* pSetWOT = static_cast<Scene::SetWOT*>(gScene->PrimarySelect());
		gScene->m_Phys.SetRigidBodyBool(pSetWOT->m_Physics, Physics::Engine::propCollidable, pWidget->value());
	}
}

static void PropSimulate_cb(fltk::Widget* pWidget, void* pWindow) {
	if (gScene->PrimarySelect()->Kind() == Scene::kSelectProp) {
		Scene::SetWOT* pSetWOT = static_cast<Scene::SetWOT*>(gScene->PrimarySelect());
		gScene->m_Phys.SetRigidBodyBool(pSetWOT->m_Physics, Physics::Engine::propActive, pWidget->value());
	}
}

void CreateLink_cb(Fl_Widget *o, void *p) {
	Scene::WOT* pPrimary = gScene->PrimarySelect();
	Scene::WOT* pSecondary = gScene->SecondarySelect();
	if (pPrimary->Kind() == Scene::kSelectProp && pSecondary->Kind() == Scene::kSelectProp) {
		PMath::Vec3f posA, posB;
		pPrimary->mp_Drawn->GetWorldPosition(posA);
		pSecondary->mp_Drawn->GetWorldPosition(posB);
		Real length = PMath::Vec3fDistance(posA, posB);
		int spring = gScene->m_Phys.AddSpring();
		gScene->m_Phys.SetSpringUInt32(spring,			Physics::Engine::propBodyA,				pPrimary->m_Physics);
		gScene->m_Phys.SetSpringUInt32(spring,			Physics::Engine::propBodyB,				pSecondary->m_Physics);
		gScene->m_Phys.SetSpringBool(spring,			Physics::Engine::propResistCompression, true);
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringRestLength,	length);
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringDamping,		Real(0.9));		// 0 means no damping, 1 means critical damping

		SpringData* pData = new SpringData();
		pData->m_BodyA = pPrimary;
		pData->m_BodyB = pSecondary;
		pData->m_Physics = spring;
		pData->m_ResistCompress = true;
		gScene->m_SpringData.push_back(pData);
	}
}

void CreateSpring_cb(Fl_Widget *o, void *p) {
	Scene::WOT* pPrimary = gScene->PrimarySelect();
	Scene::WOT* pSecondary = gScene->SecondarySelect();
	if (pPrimary->Kind() == Scene::kSelectProp && pSecondary->Kind() == Scene::kSelectProp) {
		PMath::Vec3f posA, posB;
		pPrimary->mp_Drawn->GetWorldPosition(posA);
		pSecondary->mp_Drawn->GetWorldPosition(posB);
		Real length = PMath::Vec3fDistance(posA, posB);
		int spring = gScene->m_Phys.AddSpring();
		gScene->m_Phys.SetSpringUInt32(spring,			Physics::Engine::propBodyA,				pPrimary->m_Physics);
		gScene->m_Phys.SetSpringUInt32(spring,			Physics::Engine::propBodyB,				pSecondary->m_Physics);
		gScene->m_Phys.SetSpringBool(spring,			Physics::Engine::propResistCompression, false);
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringRestLength,	length);
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
		gScene->m_Phys.SetSpringScalar(spring,			Physics::Engine::propSpringDamping,		Real(0.9));		// 0 means no damping, 1 means critical damping

		SpringData* pData = new SpringData();
		pData->m_BodyA = pPrimary;
		pData->m_BodyB = pSecondary;
		pData->m_Physics = spring;
		pData->m_ResistCompress = false;
		gScene->m_SpringData.push_back(pData);
	}
}

void CreateInfinitePlane(Fl_Widget *o, void *p) {
	DiffuseMaterial* pMat = new DiffuseMaterial();
	pMat->m_Lit = false;
	DrawPlane* pObject = new DrawPlane(pMat);
	sWorkspace->GetScene()->AddSet(pObject, Scene::kInfinitePlane);
}

void CreateSphere(Fl_Widget *o, void *p) {
	DiffuseMaterial* pMat = new DiffuseMaterial();
	pMat->m_Lit = true;
	DrawSphere* pObject = new DrawSphere(pMat);
	Real* pMatrix = pObject->GetLocalToWorldPtr();
	pMatrix[12]  = PMath::randf(1, 10);
	pMatrix[13]  = PMath::randf(1, 10);
	pMatrix[14] = PMath::randf(1, 10);
	sWorkspace->GetScene()->AddProp(pObject, Scene::kSphere);
}

void CreateOBJProp(Fl_Widget* o, void* pWindow) {
	const char *p = fltk::file_chooser("Choose an OBJ mesh file", "*.obj", "");
	if (p != 0) {
		DiffuseMaterial* pMat = new DiffuseMaterial();
		pMat->m_Lit = true;
		DrawMesh* pObject = new DrawMesh(pMat);
		pObject->CreateFromOBJFile(p);
		sWorkspace->GetScene()->AddProp(pObject, Scene::kMesh);
	}
}

void CreateCamera(Fl_Widget *o, void *p) {
	sWorkspace->GetScene()->DupCamera();
	sWorkspace->redraw();
}


void ColorButtonColorChooserCB(fltk::Widget *pWidget, void *v) {
	fltk::ColorButton* pB = (fltk::ColorButton*) pWidget;
	float red = pB->red();
	float green = pB->green();
	float blue = pB->blue();
	if (fltk::color_chooser(pB->label(),red, green, blue)) {
		pB->red(red);
		pB->green(green);
		pB->blue(blue);
		pWidget->redraw();
	}
}

void BackgroundCB(fltk::Widget *pWidget, void *v) {
	ColorButtonColorChooserCB(pWidget, v);
	fltk::ColorButton* pB = (fltk::ColorButton*) pWidget;
	Renderer* pRender = sWorkspace->GetRenderer();
	pRender->m_BackgroundColor[0] = pB->red();
	pRender->m_BackgroundColor[1] = pB->green();
	pRender->m_BackgroundColor[2] = pB->blue();
}

int RenderWindow::RenderArea::handle(int event)
{
	fltk::Widget* pFocus = fltk::focus();

	int retval = 1;
	static bool controlKey = false;
	static bool shiftKey = false;

	static bool		mouseInWindow = false;
	static float	oldfX = k0;
	static float	oldfY = k0;

	int newX = Fl::event_x(); 
	if (newX < x()) newX = x(); 
	if (newX >= x() + w()) newX = x() + w() - 1;

	int newY = Fl::event_y(); if (newY < y()) newY = y(); if (newY >= y() + h()) newY = y() + h() - 1;

	float scale = k2 / w();
	float newfX = (float) newX * scale - k1;
	float newfY = (float) newY * scale - ((float) h() / (float) w());

	unsigned int key = Fl::event_key();

	switch (event) {
		case FL_ENTER:
			mouseInWindow = true;
			break;

		case FL_LEAVE:		
			mouseInWindow = false;
			break;

		case FL_FOCUS:		
			break;

		case FL_UNFOCUS:
			break;

		case FL_PUSH:		
			break;

		case FL_MOVE:
			{
				Scene::WOT* pPotential = 0;
				Scene::eSelect potentialSelect = Scene::kSelectProp;
				int index = gScene->m_PropList.FindObject(gScene->mp_Renderer, (float) newX, (float) newY);
				if (index != -1) {
					pPotential = gScene->m_PropList.m_Objects[index];
				}

				if (index == -1) {
					potentialSelect = Scene::kSelectCamera;
					index = gScene->m_CameraList.FindObject(gScene->mp_Renderer, (float) newX, (float) newY);
					if (index != -1) {
						pPotential = gScene->m_CameraList.m_Objects[index];
					}
				}

				if (index == -1) {
					potentialSelect = Scene::kSelectSet;
					index = gScene->m_SetList.FindObject(gScene->mp_Renderer, (float) newX, (float) newY);
					if (index != -1) {
						pPotential = gScene->m_SetList.m_Objects[index];
					}
				}
				gScene->PotentialSelect(pPotential);
			}
			break;

		case FL_SHORTCUT:
		case FL_KEYDOWN:
			switch (key) {
				case FL_Control_L:
				case FL_Control_R:
					controlKey = true;
					break;

				case FL_Shift_L:
				case FL_Shift_R:
					shiftKey = true;
					break;
			}
			//retval = mouseInWindow ? 1 : 0;
			break;

	case FL_KEYUP:
		if (pFocus != gScene->m_PropPropertyName && pFocus != gScene->m_CameraPropertyName && pFocus != gScene->m_SetPropertyName) {
			switch (key) {
				case ' ':
					gScene->Deselect();
					break;

				case 'f':
				case 'F':
					// if a prop is the primary select, focus on it
					if (gScene->PrimarySelect()->Kind() == Scene::kSelectProp) {
						Vec3f pos;
						gScene->PrimarySelect()->mp_Drawn->GetWorldPosition(pos);
						gScene->CurrentCamera()->Interest(pos);
					}
					// else if a camera is primary, focus it on the secondary
					else if (gScene->PrimarySelect()->Kind() == Scene::kSelectCamera) {
						if (gScene->SecondarySelect()->Kind() != Scene::kSelectNone) {
							Vec3f pos;
							Scene::WOT* prop = gScene->SecondarySelect();
							prop->mp_Drawn->GetWorldPosition(pos);
							Scene::CameraWOT* pCamera = static_cast<Scene::CameraWOT*>(gScene->PrimarySelect());
							pCamera->m_Camera.Interest(pos);
						}
					}
					break;
			}
		}

		// treat control and shift keys separately since mouse maneuvering could still validly occur
		switch (key) {
			case FL_Control_L:
			case FL_Control_R:
				controlKey = false;
				break;

			case FL_Shift_L:
			case FL_Shift_R:
				shiftKey = false;
				break;
		}
		break;

	case FL_MOUSEWHEEL:
		{
			Camera* pCamera = gScene->CurrentCamera();
			pCamera->Dolly(0, (float) fltk::event_dy());
		}
		break;

	case FL_DRAG:
		if (controlKey && shiftKey) {
			Camera* pCamera = gScene->CurrentCamera();
			pCamera->Tumble(newfX - oldfX, newfY - oldfY);
		}
		else if (controlKey) {
			Camera* pCamera = gScene->CurrentCamera();
			pCamera->Track(newfX - oldfX, newfY - oldfY);
		}
		else if (shiftKey) {
			Camera* pCamera = gScene->CurrentCamera();
			pCamera->Dolly(newfX - oldfX, newfY - oldfY);
		}
		break;

	case FL_RELEASE:
		if (gScene->PotentialSelect()->Kind() != Scene::kSelectNone) {
			// if the potential is the secondary, promote it to primary
			if (gScene->PotentialSelect() == gScene->SecondarySelect()) {
				gScene->PrimarySelect(gScene->PotentialSelect());
				gScene->SecondarySelect(0);										// clear secondary select
				fltk::Widget* pWidget = gScene->PrimarySelect()->mp_Widget;
				Group* pGroup = gScene->m_Browser;
				int groupIndex = pGroup->find(pWidget);							// this finds the group the widget is contained in
				int widgetIndex = gScene->m_PropList.m_Group->find(pWidget);	// this finds the index within the group
				gScene->m_Browser->goto_index(groupIndex, widgetIndex);
				gScene->m_Browser->set_focus();
			}
			else {
				// promote the potential to the secondary
				gScene->SecondarySelect(gScene->PotentialSelect()); 
			}
		}
		break;

	default:			
		retval = 0; 	
		break;
	}

	sWorkspace->mp_RenderWindow->redraw();

	oldfX = newfX;
	oldfY = newfY;

	return retval;
}



Fl_Menu_Item MenuNothingSelected[] = {
/*
	{"&Edit",0,0,0,FL_SUBMENU},
		{"Undo",	FL_ALT+'z',	0, 0, FL_MENU_INACTIVE},						// nothing to undo, therefore inactive
		{"Redo",	FL_ALT+'r',	0, 0, FL_MENU_INACTIVE + FL_MENU_DIVIDER},		// nothing to do again, therefore inactive
		{"Cut",		FL_ALT+'x',	0, 0, FL_MENU_INACTIVE},						// nothing selected, therefore inactive
		{"Copy",	FL_ALT+'c',	0, 0, FL_MENU_INACTIVE},						// nothing selected, therefore inactive
		{"Paste",	FL_ALT+'v',	0, 0, FL_MENU_INACTIVE},						// nothing on clipboard, therefore inactive
		{0},
*/
	{"&Create",0,0,0,FL_SUBMENU},
		{"Camera",			0,	CreateCamera },
		{"Sets", 0,0,0,FL_SUBMENU},
			{"Infinite Plane",	0,	CreateInfinitePlane},
			{0},
		{"Props",0,0,0,FL_SUBMENU},
			{"Sphere",			0,	CreateSphere },
			{"OBJ Mesh...", 0, CreateOBJProp },
			{0},
		{0},
	{0},
};


static fltk::Group* AddFolder(fltk::Group* parent, const char* name, int open, fltk::Image* image) {
	parent->begin();
	fltk::ItemGroup* o = new fltk::ItemGroup(name);
	o->image(image);
	if (open) 
		o->set_flag(fltk::VALUE);
	return o;
}

int initfull = 0;


void RightMenu_cb(Fl_Widget* w, void* p) {
	Fl_Menu_* mw = (Fl_Menu_*)w;
	fltk::Widget* pWidget = mw->mvalue();
	fltk::Callback_p pCallback = pWidget->callback();
	pCallback(w, p);
	mw->value(0);					// reset to showing the root
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	Scene scene;
	gScene = &scene;
	scene.Init();

	Vec3f viewpoint;
	viewpoint[0] = k5;
	viewpoint[1] = -20.0f;
	viewpoint[2] = 5.0f;

	Vec3f interest;
	interest[0] = k0;
	interest[1] = k0;
	interest[2] = 3.5f;

	Vec3f up;
	up[0] = k0;
	up[1] = k0;
	up[2] = k1;

	WorkspaceWindow window(&scene, 1024,800, "Meshula Lab"); 
	window.end();
	sWorkspace = &window;
	scene.mp_Window = &window;

	RenderWindow sw(&scene, 215,10,window.w()-(10 + 235), 600);
	window.mp_RenderWindow = &sw;

	window.add(sw);
	window.resizable(&sw);

	window.begin();
		scene.m_Browser = new fltk::Browser(10, 10, 200, 280);
		scene.m_Browser->indented(1);
  
		scene.m_CameraProperties = new fltk::ScrollGroup(10, 300, 200, 100);
		scene.m_CameraProperties->box(FL_ENGRAVED_FRAME);
		scene.m_CameraProperties->begin();
			fltk::InvisibleBox* pBox = new fltk::InvisibleBox(fltk::FLAT_BOX, 2, 2, 190, 24, "Camera Properties");
			pBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
			scene.m_CameraPropertyName = new fltk::Input(2, 30, 190, 24, "Name");
			scene.m_CameraPropertyName->callback(ChangePrimaryName);
			scene.m_CameraPropertyName->when(fltk::WHEN_ENTER_KEY);
			fltk::Button * b = new fltk::Button(2,60,100,23,"Activate");
			b->callback(ActivateCamera_cb);
		scene.m_CameraProperties->end();
		scene.m_CameraProperties->hide();

		scene.m_PropProperties = new fltk::ScrollGroup(10, 300, 200, 200);
		scene.m_PropProperties->box(FL_ENGRAVED_FRAME);
		scene.m_PropProperties->begin();
			pBox = new fltk::InvisibleBox(fltk::FLAT_BOX, 2, 2, 190, 24, "Prop Properties");
			pBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
			scene.m_PropPropertyName = new fltk::Input(2, 30, 190, 24, "Name");
			scene.m_PropPropertyName->callback(ChangePrimaryName);
			scene.m_PropPropertyName->when(fltk::WHEN_ENTER_KEY);
			scene.m_PropActive = new fltk::CheckButton(2, 60, 100, 23, "Simulate");
			scene.m_PropActive->callback(PropSimulate_cb);
			scene.m_PropActive->value(true);
			scene.m_PropLink = new fltk::Button(2, 90, 100, 23, "Link");
			//scene.m_PropLink->deactivate();
			scene.m_PropLink->callback(CreateLink_cb);
			scene.m_PropSpring = new fltk::Button(2, 120, 100, 23, "Spring");
			//scene.m_PropSpring->deactivate();
			scene.m_PropSpring->callback(CreateSpring_cb);
		scene.m_PropProperties->end();
		scene.m_PropProperties->hide();

		scene.m_SetProperties = new fltk::ScrollGroup(10, 300, 200, 100);
		scene.m_SetProperties->box(FL_ENGRAVED_FRAME);
		scene.m_SetProperties->begin();
			pBox = new fltk::InvisibleBox(fltk::FLAT_BOX, 2, 2, 190, 24, "Set Properties");
			pBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
			scene.m_SetPropertyName = new fltk::Input(2, 30, 190, 24, "Name");
			scene.m_SetPropertyName->callback(ChangePrimaryName);
			scene.m_SetPropertyName->when(fltk::WHEN_ENTER_KEY);
			scene.m_SetCollider = new fltk::CheckButton(2,60,100,23,"Collider");
			scene.m_SetCollider->callback(SetIsCollider_cb);
		scene.m_SetProperties->end();
		scene.m_SetProperties->hide();

		scene.m_RMBMenu = new fltk::PopupMenu(0,0,600,400,"&Main");
		scene.m_RMBMenu->type(fltk::PopupMenu::POPUP3);
		scene.m_RMBMenu->box(FL_NO_BOX);
		scene.m_RMBMenu->menu(MenuNothingSelected);

		// note: in fltk 2, setting this callback means RightMenu_cb has to do the callbacks manually
		// in fltk 1, that is not the case
		scene.m_RMBMenu->callback(RightMenu_cb);

		window.resizable(scene.m_RMBMenu);

		int y = window.h() - 64;

		scene.m_Rewind	= new fltk::Button(5, y, 32, 32, "@|<");
		scene.m_Rewind->callback(Rewind_cb);
		scene.m_Play	= new fltk::Button(40, y, 32, 32, PLAY_LABEL);
		scene.m_Play->callback(Play_cb);

		scene.m_TimeDisplay = new fltk::Input(75, y, 70, 24);
		scene.m_TimeDisplay->value(ZEROTIME_LABEL);

		window.mp_StatusLabel = new fltk::Input(2, window.h()-26, window.w()-4, 24);
		window.mp_StatusLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		y+=30;

	window.end();

	folderSmall		= new fltk::xpmImage(folder_small);
	fileSmall		= new fltk::xpmImage(file_small);
	scene.SetIcons(new fltk::xpmImage(camera_icon), new fltk::xpmImage(teapot_icon), new fltk::xpmImage(set_icon));

	scene.m_LayersGroup		= AddFolder(scene.m_Browser, "Layers", 1, folderSmall);

	scene.m_LayersGroup->begin();
		fltk::ColorButton colorButton(50, y, 150, 24, "Background Color");
		colorButton.color(0,0,0,1);
		colorButton.callback(BackgroundCB);
	scene.m_LayersGroup->end();

	scene.m_CameraList.m_Group	= AddFolder(scene.m_Browser, "Cameras",		1, folderSmall);
	scene.m_SetList.m_Group		= AddFolder(scene.m_Browser, "Sets",		1, folderSmall);
	scene.m_PropList.m_Group	= AddFolder(scene.m_Browser, "Props",		1, folderSmall);
	scene.m_LightsGroup			= AddFolder(scene.m_Browser, "Lights",		1, folderSmall);
	scene.m_ActorsGroup			= AddFolder(scene.m_Browser, "Actors",		1, folderSmall);

	window.show();//argc,argv);

	scene.CreateCamera("Interactive");
	Camera* pCamera = scene.CurrentCamera();
	pCamera->LookAt(viewpoint, interest, up);
	scene.mp_Renderer->CurrentCamera(pCamera);

	for (;window.visible();) {
		if (window.iconic())// || !speed->value())
			fltk::wait();	// waits until something happens
		else
			fltk::check();	// returns immediately

		sw.redraw();
	}

	return 0;
}

