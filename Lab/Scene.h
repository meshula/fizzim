
#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <fltk/Browser.h>
#include <fltk/Group.h>
#include <fltk/Input.h>
#include <fltk/Item.h>
#include <fltk/ScrollGroup.h>
#include <fltk/Widget.h>
#include <fltk/Window.h>
#include <fltk/xpmImage.h>
#include "ML_Camera.h"
#include "ML_DrawObject.h"

class Renderer {
public:
	Renderer() : m_Left(0.0f), m_Right(0.0f), m_Width(1.0f), m_Height(1.0f) {
		m_BackgroundColor[0] = 0.1f;
		m_BackgroundColor[1] = 0.1f;
		m_BackgroundColor[2] = 0.1f;
		m_BackgroundColor[3] = 1.0f;
	}

	~Renderer() {
	}

	void BindCamera(Camera* pCamera) {
		mp_Camera = pCamera;
	}

	void SetViewport(float left, float right, float width, float height);
	void Render(DrawObject* pObject);
	void Render(std::vector<DrawObject*>& objects);
	void BeginScene();
	void EndScene() { }
	void ProjectToScreen(PMath::Vec3f& result, const PMath::Vec3f v);

	float						m_BackgroundColor[4];

private:
	Camera*						mp_Camera;
	float						m_Left, m_Right, m_Width, m_Height;
};

/// Widget Object Tuple
class WOT
{
public:
	WOT() : mp_Drawn(0) { }
	WOT(DrawObject* pOb) : mp_Drawn(pOb) { }

	virtual ~WOT() { }

	fltk::Widget*	mp_Widget;
	DrawObject*		mp_Drawn;
	char			m_Name[32];
};

class CameraWOT : public WOT
{
public:
	CameraWOT() { }
	virtual ~CameraWOT() { }
	Camera	m_Camera;
};

class Scene;
extern Scene* gScene;

class Scene {
public:
	Scene(Renderer* pRenderer) : mp_Renderer(pRenderer), mp_Window(0),
		m_LightsGroup(0), m_ActorsGroup(0), m_Mode(kGlobal),
		m_Browser(0), m_CameraProperties(0), 
		m_CameraModel(0), m_ArrowheadModel(0), m_IcosaModel(0) {
	}

	~Scene() {
	}

	enum eMode {	kGlobal,		// nothing selected
					kPropSelected,
					kCameraSelected
	};

	class SceneList {
	public:
		SceneList() : m_Group(0), m_Curr(-1), m_Selected(-1), m_UniqueID(1), m_Icon(0) { }
		~SceneList() { for (int i = 0; i < (int)m_Objects.size(); ++i) delete m_Objects[i]; }

		int		size()		const	{ return (int) m_Objects.size(); }		///< number of objects in the list
		int		unique()			{ int retval = m_UniqueID; ++m_UniqueID; return retval; }
		int		current()	const	{ return m_Curr; }						///< the currently active object
		void	current(int i)		{ m_Curr = i; }							///< set the currently active object
		int		selected()	const	{ return m_Selected; }					///< the object selected in the UI list
		void	selected(int s)		{ m_Selected = s; }						///< set the object selected in the UI list

		int		FindWidget(fltk::Widget* pWidget) {
			int ret = -1;
			for (int i = 0; i < (int) m_Objects.size(); ++i) {
				if (pWidget == m_Objects[i]->mp_Widget) {					// found the widget?
					ret = i;
					break;
				}
			}
			return ret;
		}

		void	Render();

		fltk::Group*		m_Group;
		fltk::xpmImage*		m_Icon;			///< This memory is not owned by the SceneList
		std::vector<WOT*>	m_Objects;

	private:
		int					m_Curr;
		int					m_Selected;
		int					m_UniqueID;
	};

	void SetIcons(fltk::xpmImage* pCameraIcon, fltk::xpmImage* pPropIcon, fltk::xpmImage* pSetIcon) { 
		m_CameraList.m_Icon = pCameraIcon; m_PropList.m_Icon = pPropIcon; m_SetList.m_Icon = pSetIcon;
	}

	void HideAllProperties() {
		m_CameraList.selected(-1);
		m_PropList.selected(-1);
		m_CameraProperties->hide();
	}

	void AddSet(DrawObject* pObject);
	void AddProp(DrawObject* pObject);

	void DisplayCameraProperties() {
		if (m_CameraList.selected() != -1) {
			m_CameraPropertyName->value(m_CameraList.m_Objects[m_CameraList.selected()]->mp_Widget->label());
			m_CameraProperties->show();
		}
		else {
			m_CameraProperties->hide();
		}
	}

	static void SelectCameraFromList(fltk::Widget* pWidget, void* pWindow) {
		int i = gScene->m_CameraList.FindWidget(pWidget);
		if (i != -1) {
			if (gScene->m_Mode != kCameraSelected) {
				gScene->HideAllProperties();
			}
			gScene->m_Mode = kCameraSelected;
			gScene->m_CameraList.selected(i);
			gScene->DisplayCameraProperties();
			gScene->mp_Window->redraw();
		}
	}

	static void SelectPropFromList(fltk::Widget* pWidget, void* pWindow) {
		int i = gScene->m_PropList.FindWidget(pWidget);
		if (i != -1) {
			if (gScene->m_Mode != kPropSelected) {
				gScene->HideAllProperties();
			}
			gScene->m_Mode = kPropSelected;
			gScene->m_PropList.selected(i);
			//gScene->DisplayPropProperties();
			gScene->mp_Window->redraw();
		}
	}

	static void SelectSetFromList(fltk::Widget* pWidget, void* pWindow) {
		int i = gScene->m_SetList.FindWidget(pWidget);
		if (i != -1) {
			gScene->m_SetList.selected(i);
			//gScene->DisplayCameraProperties();
			gScene->mp_Window->redraw();
		}
	}

	int CreateCamera(char* pName);

	int DupCamera() {
		CameraWOT* pCurrCamera = static_cast<CameraWOT*>(m_CameraList.m_Objects[m_CameraList.current()]);
		char temp[32];
		sprintf(temp, "Camera %d", m_CameraList.unique());
		int ret = CreateCamera(temp);
		CameraWOT* pNewCamera = static_cast<CameraWOT*>(m_CameraList.m_Objects[m_CameraList.current()]);
		pNewCamera->m_Camera.Set(pCurrCamera->m_Camera);	// copy settings of original camera
		return ret;
	}

	Camera* GetCurrentCamera() {
		CameraWOT* pCurrCamera = static_cast<CameraWOT*>(m_CameraList.m_Objects[m_CameraList.current()]);
		return &pCurrCamera->m_Camera;
	}

	SceneList					m_CameraList;
	SceneList					m_PropList;

	fltk::Group*				m_LayersGroup;
	SceneList					m_SetList;
	fltk::Group*				m_LightsGroup; 
	fltk::Group*				m_ActorsGroup;

	fltk::ScrollGroup*			m_CameraProperties;
	fltk::Input*				m_CameraPropertyName;

	fltk::Window*				mp_Window;

	fltk::Browser*				m_Browser;
	Renderer*					mp_Renderer;

	DrawMesh*					m_CameraModel;
	DrawMesh*					m_ArrowheadModel;
	DrawMesh*					m_IcosaModel;

	eMode						m_Mode;

private:

	static fltk::Widget* AddItem(fltk::Group* parent, const char* name, int, fltk::Image* image) {
		parent->begin();
		fltk::Item* o = new fltk::Item(name);
		o->image(image);
		return o;
	}
};

#endif
