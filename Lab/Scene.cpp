

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

#include "Scene.h"

Scene* gScene = 0;

void Renderer::SetViewport(float left, float right, float width, float height) {
	m_Left = left;
	m_Right = right;
	m_Width = width;
	m_Height = height;
	glViewport((int)left, (int)right, (int)width, (int)height);
}

// cribbed from a flipcode COTD
void Renderer::ProjectToScreen(PMath::Vec3f& ret, const PMath::Vec3f v) {
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

void Renderer::Render(DrawObject* pObject) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		pObject->LoadMatrix();
		pObject->BindMaterial();
		pObject->Render();
		pObject->UnbindMaterial();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Renderer::Render(std::vector<DrawObject*>& objects) {
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

void Renderer::BeginScene() {
	glClearColor(m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], m_BackgroundColor[3]);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_LESS);					
	glEnable(GL_DEPTH_TEST);				
	glDepthMask(GL_TRUE); 
	mp_Camera->SetCameraMatrices(m_Width, m_Height);
}

int Scene::CreateCamera(char* pName) {
	m_CameraList.current((int) m_CameraList.m_Objects.size());
	CameraWOT* pNewCamera = new CameraWOT();
	strcpy(pNewCamera->m_Name, pName);
	m_CameraList.m_Objects.push_back(pNewCamera);

	pNewCamera->mp_Widget = AddItem(m_CameraList.m_Group, pNewCamera->m_Name, 1, m_CameraList.m_Icon);
	pNewCamera->mp_Widget->callback(SelectCameraFromList);
	m_Browser->goto_focus();
	DisplayCameraProperties();
	mp_Renderer->BindCamera(&pNewCamera->m_Camera);
	return m_CameraList.current();
}

void Scene::AddProp(DrawObject* pObject) {
	WOT* pProp = new WOT(pObject);
	m_PropList.m_Objects.push_back(pProp);

	sprintf(pProp->m_Name, "Prop %d", m_PropList.unique());
	pProp->mp_Widget = AddItem(m_PropList.m_Group, pProp->m_Name, 1, m_PropList.m_Icon);
	pProp->mp_Widget->callback(SelectPropFromList);
}

void Scene::AddSet(DrawObject* pObject) {
	WOT* pSet = new WOT(pObject);
	m_SetList.m_Objects.push_back(pSet);

	sprintf(pSet->m_Name, "Set %d", m_SetList.unique());
	pSet->mp_Widget = AddItem(m_SetList.m_Group, pSet->m_Name, 1, m_SetList.m_Icon);
	pSet->mp_Widget->callback(SelectSetFromList);
}

void Scene::SceneList::Render() {
	for (int i = 0; i < size(); ++i) {
		DrawObject* pDraw = m_Objects[i]->mp_Drawn;
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

