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

using namespace PMath;

class Scene;
fltk::xpmImage* folderSmall;
fltk::xpmImage* fileSmall;

// we make a vanilla stock material always available
DiffuseMaterial gStockMaterial;



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
	RenderWindow(Scene* pScene, Renderer* pRenderer, int x,int y,int w,int h,const char *l=0) :
		fltk::GlWindow(x, y, w, h), mp_Scene(pScene)  {
		mp_Renderer = pRenderer;
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
	Renderer*	mp_Renderer;
};

class WorkspaceWindow : public fltk::Window {
public:
	WorkspaceWindow(Scene* pScene, Renderer* pRenderer, int w,int h, const char *l=0);
	virtual ~WorkspaceWindow() { }

	Scene*				GetScene() const { return mp_Scene; }
	Renderer*			GetRenderer() const { return mp_Renderer; }
	fltk::Input*		mp_StatusLabel;

	RenderWindow*		mp_RenderWindow;

private:
	Scene*				mp_Scene;
	Renderer*			mp_Renderer;
};

WorkspaceWindow::WorkspaceWindow(Scene* pScene, Renderer* pRenderer, int w, int h, const char *l) :
	mp_Renderer(pRenderer),
	mp_Scene(pScene), 
	fltk::Window(w,h, l) 
{
}

static WorkspaceWindow* sWorkspace = 0;


static void DrawObjectCues(DrawObject* pProp) {
	float temp[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pProp->LoadMatrix();

		glColor3f(0.1f, 1.0f, 0.1f);

		PMath::Vec3f corners[8];
		pProp->DrawBoundingBox();

		pProp->GetBoundingCorners(corners);
		pProp->GetLocalToWorld(temp);
		PMath::Mat44Scale3x3(temp, 0.1f);
		PMath::Vec3f tempPos;
		tempPos[0] = temp[12]; tempPos[1] = temp[13]; tempPos[2] = temp[14];
		DiffuseMaterial* pMat = (DiffuseMaterial*) gScene->m_IcosaModel->GetMaterial();
		pMat->m_Diffuse[0] = 0.7f;
		pMat->m_Diffuse[1] = 0.7f;
		pMat->m_Diffuse[2] = 0.1f;
		gScene->m_IcosaModel->BindMaterial();
		for (int i = 0; i < 8; ++i) {
			temp[12] = tempPos[0] + corners[i][0];
			temp[13] = tempPos[1] + corners[i][1];
			temp[14] = tempPos[2] + corners[i][2];
			gScene->m_IcosaModel->SetMatrix(temp);
			gScene->mp_Renderer->Render(gScene->m_IcosaModel);
		}
		gScene->m_IcosaModel->UnbindMaterial();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static void DrawCameraCues(Camera* pCamera, DrawObject* pMesh) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pMesh->LoadMatrix();
		glColor3f(0.1f, 1.0f, 0.1f);
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

	// has the window been resized?
	if (!valid()) {
		valid(1);
		mp_Renderer->SetViewport(0.0f, 0.0f, (float) w(), (float) h());
		mp_InteractiveArea->resize(0, 0, w(), h());
	}

	mp_Renderer->BeginScene();
	mp_Scene->m_SetList.Render();
	mp_Scene->m_PropList.Render();

	i = mp_Scene->m_PropList.selected();
	if (i != -1) {
		DrawObjectCues(mp_Scene->m_PropList.m_Objects[i]->mp_Drawn);
	}

	for (i = 0; i < mp_Scene->m_CameraList.size(); ++i) {
		if (i != mp_Scene->m_CameraList.current()) {
			CameraWOT* pCWOT = static_cast<CameraWOT*>(mp_Scene->m_CameraList.m_Objects[i]);
			Camera* pCamera = &pCWOT->m_Camera;
			pCamera->GetCameraMatrix(temp);
			temp[12] = pCamera->m_Position[0];
			temp[13] = pCamera->m_Position[1];
			temp[14] = pCamera->m_Position[2];
			PMath::Mat44Transpose3x3(temp);
			PMath::Mat44Scale3x3(temp, 0.5f);
			mp_Scene->m_CameraModel->SetMatrix(temp);
			mp_Renderer->Render(mp_Scene->m_CameraModel);

			if (i == mp_Scene->m_CameraList.selected()) {
				DrawCameraCues(pCamera, mp_Scene->m_CameraModel);
			}
		}
	}

	mp_Renderer->EndScene();
}



static void ChangeCameraName(fltk::Widget* pWidget, void* pWindow) {
	fltk::Input* pInput = (fltk::Input*) pWidget;
	Scene* pScene = sWorkspace->GetScene();
	if (pScene->m_CameraList.selected() != -1) {
		CameraWOT* pCameraWOT = static_cast<CameraWOT*>(pScene->m_CameraList.m_Objects[pScene->m_CameraList.selected()]);
		Camera* pCamera = &pCameraWOT->m_Camera;
		strncpy(pCameraWOT->m_Name, pInput->value(), 31);
		pCameraWOT->m_Name[31] = '\0';
		pCameraWOT->mp_Widget->label(pCameraWOT->m_Name);
		pScene->m_Browser->redraw();
	}
}

static void ActivateCamera_cb(fltk::Widget* pWidget, void* pWindow) {
	Scene* pScene = sWorkspace->GetScene();
	if (pScene->m_CameraList.selected() != -1) {
		pScene->m_CameraList.current(pScene->m_CameraList.selected());
		pScene->mp_Renderer->BindCamera(pScene->GetCurrentCamera());
	}
}

void CreateInfinitePlane(Fl_Widget *o, void *p) {
	DrawPlane* pObject = new DrawPlane(&gStockMaterial);
	sWorkspace->GetScene()->AddSet(pObject);
}

void CreateSphere(Fl_Widget *o, void *p) {
	DrawSphere* pObject = new DrawSphere(&gStockMaterial);
	sWorkspace->GetScene()->AddProp(pObject);
}

void CreateOBJProp(Fl_Widget* o, void* pWindow) {
	const char *p = fltk::file_chooser("Choose an OBJ mesh file", "*.obj", "");
	if (p != 0) {
		DrawMesh* pObject = new DrawMesh(&gStockMaterial);
		pObject->CreateFromOBJFile(p);
		sWorkspace->GetScene()->AddProp(pObject);
	}
}

void CreateCamera(Fl_Widget *o, void *p) {
	sWorkspace->GetScene()->DupCamera();
	sWorkspace->redraw();
}


fltk::Color c = fltk::BLACK;
#define fullcolor_cell ((fltk::Color)16)
/*
void cb1(fltk::Widget *, void *v) {
  c = fltk::show_colormap(c);
  fltk::Widget* b = (fltk::Widget*)v;
  b->color(c);
  b->parent()->redraw();
}
*/

void ColorButtonColorChooserCB(fltk::Widget *pWidget, void *v) {
	uchar r,g,b;
	//  Fl::get_color(c,r,g,b);
	fltk::split_color(c,r,g,b);
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
	int retval = 0;
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

	char* eventS = "?";
	switch (event) {

		case FL_ENTER:
			mouseInWindow = true;
			retval = 1; 
			eventS = "enter";	
			break;

		case FL_LEAVE:		
			mouseInWindow = false;
			retval = 1; 
			eventS = "leave";	
			break;

		case FL_FOCUS:		
			eventS = "focus";	
			retval = 1;
			break;

		case FL_UNFOCUS:
			eventS = "focus";	
			retval = 1;
			break;

		case FL_PUSH:		retval = 1; eventS = "push";	break;

		case FL_MOVE:		
			retval = 1; 
			eventS = "move";	
	{
		char temp[128];
		PMath::Vec3f world;
		PMath::Vec3fZero(world);
		PMath::Vec3f proj;
		sWorkspace->GetRenderer()->ProjectToScreen(proj, world);
		sprintf(temp, "%f %f %f", proj[0], proj[1], proj[2]);
		sWorkspace->mp_StatusLabel->value(temp);
	}
			break;

		case FL_SHORTCUT:
		case FL_KEYDOWN:
			switch (key) {
				case FL_Control_L:
				case FL_Control_R:
					controlKey = true;
					retval = 1;
					break;

				case FL_Shift_L:
				case FL_Shift_R:
					shiftKey = true;
					retval = 1;
					break;
			}
			//retval = mouseInWindow ? 1 : 0;
			eventS = "keydown";	
			break;

	case FL_KEYUP:
		switch (key) {
			case 'f':
			case 'F':
				if (gScene->m_PropList.selected() != -1)
				{
					Vec3f pos;
					WOT* prop = gScene->m_PropList.m_Objects[gScene->m_PropList.selected()];
					prop->mp_Drawn->GetPosition(pos);
					gScene->GetCurrentCamera()->Interest(pos);
				}
				break;

			case FL_Control_L:
			case FL_Control_R:
				controlKey = false;
				retval = 1;
				break;

			case FL_Shift_L:
			case FL_Shift_R:
				shiftKey = false;
				retval = 1;
				break;
		}
		break;

	case FL_DRAG:
		if (controlKey && shiftKey) {
			retval = 1; 
			eventS = "tumble";	
			Scene* pScene = sWorkspace->GetScene();
			Camera* pCamera = pScene->GetCurrentCamera();
			pCamera->Tumble(newfX - oldfX, newfY - oldfY);
		}
		else if (controlKey) {
			retval = 1; 
			eventS = "track";	
			Scene* pScene = sWorkspace->GetScene();
			Camera* pCamera = pScene->GetCurrentCamera();
			pCamera->Track(newfX - oldfX, newfY - oldfY);
		}
		else if (shiftKey) {
			retval = 1; 
			eventS = "dolly";	
			Scene* pScene = sWorkspace->GetScene();
			Camera* pCamera = pScene->GetCurrentCamera();
			pCamera->Dolly(newfX - oldfX, newfY - oldfY);
		}
		break;

	case FL_RELEASE:	retval = 1; eventS = "release";	break;
	default:			retval = 0; eventS = "?";		break;
	}

	sWorkspace->mp_RenderWindow->redraw();

	oldfX = newfX;
	oldfY = newfY;

	return retval;
}



Fl_Menu_Item MenuNothingSelected[] = {

	{"&Edit",0,0,0,FL_SUBMENU},
		{"Undo",	FL_ALT+'z',	0, 0, FL_MENU_INACTIVE},						// nothing to undo, therefore inactive
		{"Redo",	FL_ALT+'r',	0, 0, FL_MENU_INACTIVE + FL_MENU_DIVIDER},		// nothing to do again, therefore inactive
		{"Cut",		FL_ALT+'x',	0, 0, FL_MENU_INACTIVE},						// nothing selected, therefore inactive
		{"Copy",	FL_ALT+'c',	0, 0, FL_MENU_INACTIVE},						// nothing selected, therefore inactive
		{"Paste",	FL_ALT+'v',	0, 0, FL_MENU_INACTIVE},						// nothing on clipboard, therefore inactive
		{0},

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
int NUMB = 5;
Fl_Button *border_button;

void border_cb(Fl_Widget *o, void *p) {
//	Fl_Window *w = (Fl_Window *)p;
//	int d = ((Fl_Button *)o)->value();
//	w->border(d);
}

void RightMenu_cb(Fl_Widget* w, void*) {
	Fl_Menu_* mw = (Fl_Menu_*)w;
/*	const Fl_Menu_Item* m = mw->mvalue();
	if (!m) {
		//printf("NULL\n");
	}
	else if (m->shortcut()) {
		//printf("%s - shortcut\n", m->label());
	}
	else {
		//printf("%s\n", m->label());
	}
*/
}

/*
void fullscreen_cb(Fl_Widget *o, void *p) {
	static int px,py,pw,ph;
	//fltk::Window *w = (fltk::Window *)p;
	int d = ((Fl_Button *)o)->value();
	if (d) {
		px = sWorkspace->x();
		py = sWorkspace->y();
		pw = sWorkspace->w();
		ph = sWorkspace->h();
		sWorkspace->fullscreen();
	} else {
		sWorkspace->fullscreen_off(px,py,pw,ph);
	}
}
*/







int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	Renderer renderer;
	Scene scene(&renderer);
	gScene = &scene;

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

	WorkspaceWindow window(&scene, &renderer, 1024,600+30*NUMB, "Meshula Lab"); 
	window.end();
	sWorkspace = &window;
	scene.mp_Window = &window;

	RenderWindow sw(&scene, &renderer, 215,10,window.w()-(10 + 235),window.h()-30*NUMB-20);
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
			scene.m_CameraPropertyName->callback(ChangeCameraName);
			scene.m_CameraPropertyName->when(fltk::WHEN_ENTER_KEY);
			fltk::Button * b = new fltk::Button(2,60,100,23,"Activate");
			b->callback(ActivateCamera_cb);
		scene.m_CameraProperties->end();
//		scene.m_CameraProperties->hide();

		fltk::PopupMenu RightMouseButton(0,0,600,400,"&Main");
		RightMouseButton.type(fltk::PopupMenu::POPUP3);
		RightMouseButton.box(FL_NO_BOX);
		RightMouseButton.menu(MenuNothingSelected);

		// note: in fltk 2, setting this callback means RightMenu_cb has to do the callbacks manually
		// in fltk 1, that is not the case
//		RightMouseButton.callback(RightMenu_cb);

		window.resizable(&RightMouseButton);

		int y = window.h()-30*NUMB;

		Fl_Toggle_Button b2(50,y,window.w()-60,30,"Border");
		b2.callback(border_cb, &sw);
		b2.set();
		border_button = &b2;
		y+=30;

		window.mp_StatusLabel = new fltk::Input(2, window.h()-26, window.w()-4, 24);
		window.mp_StatusLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		y+=30;

	window.end();

	scene.m_CameraModel		= new DrawMesh(&gStockMaterial);
	scene.m_CameraModel->CreateFromOBJFile("data/motioncamera.obj");

	scene.m_ArrowheadModel	= new DrawMesh(&gStockMaterial);
	scene.m_ArrowheadModel->CreateFromOBJFile("data/arrowhead.obj");

	scene.m_IcosaModel		= new DrawMesh(new DiffuseMaterial());
	scene.m_IcosaModel->CreateFromOBJFile("data/icosahedron.obj");

	folderSmall		= new fltk::xpmImage(folder_small);
	fileSmall		= new fltk::xpmImage(file_small);
	scene.SetIcons(new fltk::xpmImage(camera_icon), new fltk::xpmImage(teapot_icon), new fltk::xpmImage(teapot_icon));

	scene.m_LayersGroup		= AddFolder(scene.m_Browser, "Layers", 1, folderSmall);

	scene.m_LayersGroup->begin();
		fltk::ColorButton colorButton(50, y, 150, 30, "Background Color");
		colorButton.color(0,0,0,1);
		colorButton.callback(BackgroundCB);
	scene.m_LayersGroup->end();

	scene.m_CameraList.m_Group	= AddFolder(scene.m_Browser, "Cameras", 1, folderSmall);
	scene.m_SetList.m_Group		= AddFolder(scene.m_Browser, "Sets", 1, folderSmall);
	scene.m_PropList.m_Group	= AddFolder(scene.m_Browser, "Props", 1, folderSmall);
	scene.m_LightsGroup			= AddFolder(scene.m_Browser, "Lights", 1, folderSmall);
	scene.m_ActorsGroup			= AddFolder(scene.m_Browser, "Actors", 1, folderSmall);

	window.show();//argc,argv);

	scene.CreateCamera("Interactive");
	Camera* pCamera = scene.GetCurrentCamera();
	pCamera->LookAt(viewpoint, interest, up);
	renderer.BindCamera(pCamera);

	for (;window.visible();) {
		if (window.iconic())// || !speed->value())
			fltk::wait();	// waits until something happens
		else
			fltk::check();	// returns immediately

		sw.redraw();
	}

	return 0;
}

