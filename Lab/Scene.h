
#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <fltk/Browser.h>
#include <fltk/CheckButton.h>
#include <fltk/Group.h>
#include <fltk/Input.h>
#include <fltk/Item.h>
#include <fltk/PopupMenu.h>
#include <fltk/ScrollGroup.h>
#include <fltk/Widget.h>
#include <fltk/Window.h>
#include <fltk/xpmImage.h>

#include "Clock.h"
#include "ML_Camera.h"
#include "ML_DrawObject.h"
#include "PhysicsEngine.h"

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

	void CurrentCamera(Camera* pCamera) { mp_Camera = pCamera; }
	Camera* CurrentCamera() const { return mp_Camera; }

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


class SpringData;
class Scene;
extern Scene* gScene;

class Scene {
public:
	Scene();
	~Scene();

	bool Init();

	enum eSelect {
		kSelectNone, kSelectProp, kSelectCamera, kSelectSet
	};

	enum eSetType {
		kInfinitePlane
	};

	enum ePropType {
		kSphere, kMesh
	};

	/// Widget Object Tuple
	class WOT
	{
	public:
		WOT() : m_Kind(kSelectNone), mp_Drawn(0), m_Physics(0) { }
		WOT(eSelect kind) : m_Kind(kind), mp_Drawn(0), m_Physics(0) { }
		WOT(eSelect kind, DrawObject* pOb) : m_Kind(kind), mp_Drawn(pOb), m_Physics(0) { }

		virtual ~WOT() { }

		eSelect			Kind() const { return m_Kind; }

		fltk::Widget*	mp_Widget;
		DrawObject*		mp_Drawn;
		char			m_Name[32];
		uint32			m_Physics;
	private:
		eSelect			m_Kind;
	};

	class SetWOT : public WOT
	{
	public:
		SetWOT() : WOT(kSelectSet) { }
		SetWOT(DrawObject* pOb) : WOT(kSelectSet, pOb) { }
		virtual ~SetWOT() { }
	};

	class CameraWOT : public WOT
	{
	public:
		CameraWOT() : WOT(kSelectCamera) { }
		virtual ~CameraWOT() { }
		Camera	m_Camera;
	};

	//------------------------------------------------------------------------------------------------
	class SceneList {
	public:
		SceneList() : m_Group(0), m_UniqueID(1), m_Icon(0) { }
		~SceneList() { for (int i = 0; i < (int)m_Objects.size(); ++i) delete m_Objects[i]; }

		int		size()		const	{ return (int) m_Objects.size(); }		///< number of objects in the list
		int		unique()			{ int retval = m_UniqueID; ++m_UniqueID; return retval; }

		int		FindObject(Renderer*, float x, float y);					///< find the object under the cursor
		int		FindWidget(fltk::Widget* pWidget);
		void	Render();

		fltk::Group*		m_Group;
		fltk::xpmImage*		m_Icon;			///< This memory is not owned by the SceneList
		std::vector<WOT*>	m_Objects;

	private:
		int					m_UniqueID;
	};
	//------------------------------------------------------------------------------------------------

	static void IdleCallback(void* pV);
	void Idle();

	void SetIcons(fltk::xpmImage* pCameraIcon, fltk::xpmImage* pPropIcon, fltk::xpmImage* pSetIcon) { 
		m_CameraList.m_Icon = pCameraIcon; m_PropList.m_Icon = pPropIcon; m_SetList.m_Icon = pSetIcon;
	}

	void HideAllProperties() {
		m_CameraProperties->hide();
		m_PropProperties->hide();
		m_SetProperties->hide();
	}

	void AddSet(DrawObject* pObject, eSetType setType);
	void AddProp(DrawObject* pObject, ePropType propType);

	void Deselect();
	void PrimarySelect(WOT*);
	void SecondarySelect(WOT*);
	void PotentialSelect(WOT*);

	WOT* PrimarySelect()		const { return m_PrimarySelect; }
	WOT* SecondarySelect()		const { return m_SecondarySelect; }
	WOT* PotentialSelect()		const { return m_PotentialSelect; }

	static void SelectCameraFromList(fltk::Widget* pWidget, void* pWindow);
	static void SelectPropFromList(fltk::Widget* pWidget, void* pWindow);
	static void SelectSetFromList(fltk::Widget* pWidget, void* pWindow);

	void CreateCamera(char* pName);
	void DupCamera();
	Camera* CurrentCamera() const { return mp_Renderer->CurrentCamera(); }

	Material*					m_GridMaterial;
	DrawGrid*					m_DrawGrid;

	SceneList					m_CameraList;
	SceneList					m_PropList;

	fltk::Group*				m_LayersGroup;
	SceneList					m_SetList;
	fltk::Group*				m_LightsGroup; 
	fltk::Group*				m_ActorsGroup;

	fltk::ScrollGroup*			m_PropProperties;
	fltk::Input*				m_PropPropertyName;
	fltk::CheckButton*			m_PropActive;
	fltk::Button*				m_PropLink;
	fltk::Button*				m_PropSpring;

	fltk::ScrollGroup*			m_CameraProperties;
	fltk::Input*				m_CameraPropertyName;

	fltk::ScrollGroup*			m_SetProperties;
	fltk::Input*				m_SetPropertyName;
	fltk::CheckButton*			m_SetCollider;

	fltk::PopupMenu*			m_RMBMenu;

	fltk::Window*				mp_Window;

	fltk::Button*				m_Rewind;
	fltk::Button*				m_Play;

	fltk::Browser*				m_Browser;
	Renderer*					mp_Renderer;

	fltk::Input*				m_TimeDisplay;

	DrawMesh*					m_CameraModel;
	DrawMesh*					m_ArrowheadModel;
	DrawMesh*					m_IcosaModel;

	Physics::Engine				m_Phys;
	Clock*						m_Clock;
	TimeVal						m_PrevTime;

	std::vector<SpringData*>	m_SpringData;

private:
	WOT							m_Deselected;

	WOT*						m_PrimarySelect;
	WOT*						m_SecondarySelect;
	WOT*						m_PotentialSelect;

	static fltk::Widget* AddItem(fltk::Group* parent, const char* name, int, fltk::Image* image) {
		parent->begin();
		fltk::Item* o = new fltk::Item(name);
		o->image(image);
		return o;
	}
};

class SpringData {
public:
	Scene::WOT*	m_BodyA;
	Scene::WOT*	m_BodyB;
	uint32	m_Physics;
	bool m_ResistCompress;
};

#endif
