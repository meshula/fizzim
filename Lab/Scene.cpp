

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#include <fltk/run.h>
#include "Scene.h"


// we make a vanilla stock material always available
DiffuseMaterial gStockMaterial;


Scene* gScene = 0;

void Renderer :: SetViewport(float left, float right, float width, float height) {
	m_Left = left;
	m_Right = right;
	m_Width = width;
	m_Height = height;
	glViewport((int)left, (int)right, (int)width, (int)height);
}

// cribbed from a flipcode COTD
void Renderer :: ProjectToScreen(PMath::Vec3f& ret, const PMath::Vec3f v) {
	double modelView[16];
	double projection[16];
	double viewport[4];
	double depthRange[2];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_VIEWPORT, viewport);
	glGetDoublev(GL_DEPTH_RANGE, depthRange);

	// Compose the matrices into a single row-major transformation
	PMath::Vec4f T[4];
	int r, c, i;
	for (r = 0; r < 4; ++r) {
		for (c = 0; c < 4; ++c) {
			T[r][c] = k0;
			for (i = 0; i < 4; ++i) {
				// OpenGL matrices are column major
				T[r][c] += (float) (projection[r + i * 4] * modelView[i + c * 4]);
			}
		}
	}

	// Transform the vertex
	PMath::Vec4f result;
	PMath::Vec4f temp;
	temp[0] = v[0]; temp[1] = v[1]; temp[2] = v[2]; temp[3] = k1;
	for (r = 0; r < 4; ++r) {
		result[r] = PMath::Vec4fDot(T[r], temp);
	}

	// Homogeneous divide
	// it might be interesting to return rhw at some point
	const double rhw = 1.0 / result[3];

	ret[0] = (float)((k1 + result[0] * rhw) * viewport[2] * kHalf + viewport[0]);
	ret[1] = (float)((k1 - result[1] * rhw) * viewport[3] * kHalf + viewport[1]);
	ret[2] = (float)((     result[2] * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0]);
} 

void Renderer :: Render(DrawObject* pObject) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pObject->LoadMatrix();
		pObject->BindMaterial();
		pObject->Render();
		pObject->UnbindMaterial();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Renderer :: Render(std::vector<DrawObject*>& objects) {
	for (int i = 0; i < (int) objects.size(); ++i) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			objects[i]->LoadMatrix();
			objects[i]->BindMaterial();
			objects[i]->Render();
			objects[i]->UnbindMaterial();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}

void Renderer :: BeginScene() {
	glClearColor(m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], m_BackgroundColor[3]);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_LESS);					
	glEnable(GL_DEPTH_TEST);				
	glDepthMask(GL_TRUE); 
	mp_Camera->SetCameraMatrices(m_Width, m_Height);
}

Scene :: Scene() : mp_Renderer(0), mp_Window(0),
	m_LightsGroup(0), m_ActorsGroup(0), 
	m_Browser(0), m_CameraProperties(0), m_PropProperties(0),
	m_CameraModel(0), m_ArrowheadModel(0), m_IcosaModel(0),
	m_PrimarySelect(&m_Deselected), m_SecondarySelect(&m_Deselected), m_PotentialSelect(&m_Deselected)
{
}

Scene :: ~Scene() {
	delete m_GridMaterial;
	delete m_DrawGrid;
}

static char timeBuff[128];

void Scene :: Idle() {
	TimeVal newTime;
	int i;

	m_Clock->update();
	if (!m_Clock->getPausedState()) {
		m_Clock->update();
		newTime = m_Clock->getSimulationTime();
		Real dt = (Real) (newTime - m_PrevTime);
		m_PrevTime = newTime;

		dt = PMath::Clamp(dt, k1 / Real(60.0f), k1 / Real(20.0f));
		m_Phys.Simulate(dt);

		for (i = 0; i < m_PropList.size(); ++i) {
			WOT* pObj = m_PropList.m_Objects[i];
			int body = pObj->m_Physics;
			if (body != 0) {
				m_Phys.GetRigidBodyTransformMatrix(body, pObj->mp_Drawn->GetLocalToWorldPtr());
			}
		}

		static TimeVal prevDisplay = 0;

		// only display to 1/10 of a second
		if (newTime - prevDisplay > 0.09) {
			prevDisplay = newTime;
			// update time display
			int nt = (int) newTime;
			int subseconds = (int) (10.0 * (newTime - (TimeVal) nt));
			int seconds = nt % 60;
			int minutes = nt / 60;
			int hours = nt / (60 * 60);
			sprintf(timeBuff, "%02d:%02d:%02d.%d", hours, minutes, seconds, subseconds);
			m_TimeDisplay->value(timeBuff);
		}
	}
}

void Scene :: IdleCallback(void* pV) {
	Scene* pScene = (Scene*) pV;
	pScene->Idle();
}

bool Scene :: Init() {
	m_GridMaterial		= new DiffuseMaterial();
	m_DrawGrid			= new DrawGrid(m_GridMaterial, 10, 10);
	m_CameraModel		= new DrawMesh(&gStockMaterial);
	m_ArrowheadModel	= new DrawMesh(&gStockMaterial);
	m_IcosaModel		= new DrawMesh(new DiffuseMaterial());

	m_CameraModel->CreateFromOBJFile("data/motioncamera.obj");
	m_ArrowheadModel->CreateFromOBJFile("data/arrowhead.obj");
	m_IcosaModel->CreateFromOBJFile("data/icosahedron.obj");

	mp_Renderer = new Renderer();

	m_Clock = new Clock();
	m_Clock->update();
	m_PrevTime = m_Clock->getSimulationTime();
	m_Clock->setPausedState(true);

	m_Phys.SetMinTimeStep(1.0f/50.0f);
	PMath::Vec3f gravity = {k0, k0, Real(-9.8f)};
	m_Phys.SetGravity(gravity);

	fltk::add_idle(IdleCallback, this);
	return true;													///< @todo check if models successfully loaded
}

void Scene :: CreateCamera(char* pName) {
	// create a new camera and push it on the list
	CameraWOT* pNewCamera = new CameraWOT();
	strcpy(pNewCamera->m_Name, pName);
	m_CameraList.m_Objects.push_back(pNewCamera);

	// add a widget to the browser
	pNewCamera->mp_Widget = AddItem(m_CameraList.m_Group, pNewCamera->m_Name, 1, m_CameraList.m_Icon);
	pNewCamera->mp_Widget->callback(SelectCameraFromList);
	m_Browser->goto_focus();

	PrimarySelect(pNewCamera);

	// and set the camera to be the current camera
	mp_Renderer->CurrentCamera(&pNewCamera->m_Camera);
}

void Scene :: DupCamera() {
	char temp[32];
	// remember existing camera settings
	Camera* pCamera = mp_Renderer->CurrentCamera();

	// create the new camera
	sprintf(temp, "Camera %d", m_CameraList.unique());
	CreateCamera(temp);

	// copy the business from the previos camera into the new one
	mp_Renderer->CurrentCamera()->Set(*pCamera);
}

void Scene :: AddProp(DrawObject* pObject, ePropType propType) {
	WOT* pProp = new WOT(kSelectProp, pObject);
	m_PropList.m_Objects.push_back(pProp);

	sprintf(pProp->m_Name, "Prop %d", m_PropList.unique());
	pProp->mp_Widget = AddItem(m_PropList.m_Group, pProp->m_Name, 1, m_PropList.m_Icon);
	pProp->mp_Widget->callback(SelectPropFromList);

	if (propType == kSphere) {
		PMath::Vec3f spherePos;
		pObject->GetWorldPosition(spherePos);
		pProp->m_Physics = m_Phys.AddRigidBodySphere(1.0f);
		m_Phys.SetRigidBodyVec3f(pProp->m_Physics,	Physics::Engine::propPosition,		spherePos);
		m_Phys.SetRigidBodyBool(pProp->m_Physics,	Physics::Engine::propCollidable,	true);
		m_Phys.SetRigidBodyBool(pProp->m_Physics,	Physics::Engine::propTranslatable,	true);
	}
}

void Scene :: AddSet(DrawObject* pObject, eSetType setType) {
	SetWOT* pSet = new SetWOT(pObject);
	m_SetList.m_Objects.push_back(pSet);

	sprintf(pSet->m_Name, "Set %d", m_SetList.unique());
	pSet->mp_Widget = AddItem(m_SetList.m_Group, pSet->m_Name, 1, m_SetList.m_Icon);
	pSet->mp_Widget->callback(SelectSetFromList);

	if (setType == kInfinitePlane) {
		PMath::Vec3f origin;
		pObject->GetPosition(origin);
		PMath::Vec3f up;
		Real* pMatrix = pObject->GetLocalToWorldPtr();
		up[0] = pMatrix[2];
		up[1] = pMatrix[6];
		up[2] = pMatrix[10];
		PMath::Plane plane(origin, up);
		pSet->m_Physics = m_Phys.AddRigidBodyPlane(plane);
		m_Phys.SetRigidBodyBool(pSet->m_Physics,	Physics::Engine::propCollidable,	true);
		m_Phys.SetRigidBodyBool(pSet->m_Physics,	Physics::Engine::propTranslatable,	false);
	}
}

void Scene :: Deselect() {
	m_PrimarySelect		= &m_Deselected;
	m_SecondarySelect	= &m_Deselected;
	m_PotentialSelect	= &m_Deselected;
	HideAllProperties();
}

void Scene :: SelectCameraFromList(fltk::Widget* pWidget, void* pWindow) {
	int i = gScene->m_CameraList.FindWidget(pWidget);
	if (i != -1) {
		gScene->PrimarySelect(gScene->m_CameraList.m_Objects[i]);
	}
}

void Scene :: SelectPropFromList(fltk::Widget* pWidget, void* pWindow) {
	int i = gScene->m_PropList.FindWidget(pWidget);
	if (i != -1) {
		gScene->PrimarySelect(gScene->m_PropList.m_Objects[i]);
	}
}

void Scene :: PrimarySelect(WOT* pWOT) {
	HideAllProperties();
	m_PrimarySelect = (pWOT != 0) ? pWOT : &m_Deselected;

	if (pWOT->Kind() == kSelectCamera) {
		m_CameraPropertyName->value(pWOT->mp_Widget->label());
		m_CameraProperties->show();
	}
	else if (pWOT->Kind() == kSelectProp) {
		m_PropPropertyName->value(pWOT->mp_Widget->label());
		m_PropProperties->show();
		bool active = m_Phys.GetRigidBodyBool(pWOT->m_Physics, Physics::Engine::propActive);
		m_PropActive->value(active);
	}
	else if (pWOT->Kind() == kSelectSet) {
		m_SetPropertyName->value(pWOT->mp_Widget->label());
		m_SetProperties->show();
		bool collider = m_Phys.GetRigidBodyBool(pWOT->m_Physics, Physics::Engine::propCollidable);
		m_SetCollider->value(collider);
	}
}

void Scene :: SecondarySelect(WOT* pWOT) {
	m_SecondarySelect = (pWOT != 0) ? pWOT : &m_Deselected;
}

void Scene :: PotentialSelect(WOT* pWOT) {
	m_PotentialSelect = (pWOT != 0) ? pWOT : &m_Deselected;
}

void Scene :: SelectSetFromList(fltk::Widget* pWidget, void* pWindow) {
	int i = gScene->m_SetList.FindWidget(pWidget);
	if (i != -1) {
		gScene->PrimarySelect(gScene->m_SetList.m_Objects[i]);
		gScene->mp_Window->redraw();
	}
}


void Scene::SceneList :: Render() {
	for (int i = 0; i < size(); ++i) {
		DrawObject* pDraw = m_Objects[i]->mp_Drawn;
		if (pDraw->Visible()) {
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				pDraw->LoadMatrix();
				pDraw->BindMaterial();
				pDraw->Render();
				pDraw->UnbindMaterial();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}
}

int Scene::SceneList :: FindWidget(fltk::Widget* pWidget) {
	int ret = -1;
	for (int i = 0; i < (int) m_Objects.size(); ++i) {
		if (pWidget == m_Objects[i]->mp_Widget) {					// found the widget?
			ret = i;
			break;
		}
	}
	return ret;
}

int Scene::SceneList :: FindObject(Renderer* pRenderer, float x, float y) {
	int ret = -1;
	float distance = 1.0e6f;
	PMath::Vec3f position, objectPosition;
	position[0] = x;
	position[1] = y;
	for (int i = 0; i < (int) m_Objects.size(); ++i) {
		DrawObject* pDraw = m_Objects[i]->mp_Drawn;
		if (pDraw && pDraw->Visible()) {
			pDraw->GetPosition(objectPosition);
			PMath::Mat44Transform3x3(objectPosition, (Real const*const) pDraw->GetLocalToWorldPtr(), objectPosition);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			pDraw->LoadMatrix();

			pRenderer->ProjectToScreen(objectPosition, objectPosition);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			position[2] = objectPosition[2];
			float dist = PMath::Vec3fDistance(position, objectPosition);
			if (dist < 40.0f) {
				if (dist < distance) {
					ret = i;
				}
			}
		}
	}
	return ret;
}
