

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

#include "ML_Camera.h"

void Camera::SetCameraMatrices(float width, float height, float znear, float zfar) {
	m_Near = znear;
	m_Far = zfar;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();           
	gluPerspective(45.0f, width / height, znear, zfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_Camera);
}

void Camera::SetViewport(float left, float right, float width, float height) {
	m_Left = left;
	m_Right = right;
	m_Width = width;
	m_Height = height;
	glViewport((int)left, (int)right, (int)width, (int)height);
}

void Camera::Set(const Camera& camera) {
	PMath::Mat44Set(m_Camera, camera.m_Camera);
	PMath::Vec3fSet(m_Position, camera.m_Position);
	PMath::Vec3fSet(m_Interest, camera.m_Interest);
	PMath::Vec3fSet(m_Up, camera.m_Up);
}

void Camera::Interest(PMath::Vec3f pos) {
	PMath::Vec3fSet(m_Interest, pos);
	LookAt(m_Position, m_Interest, m_Up);
}

void Camera::Track(float mouseMotionX, float mouseMotionY) {
	PMath::Vec3f temp;
	temp[0] = m_Camera[1];
	temp[1] = m_Camera[5];
	temp[2] = m_Camera[9];
	PMath::Vec3fScale(temp, k10 * mouseMotionY, temp);
	PMath::Vec3fAdd(m_Position, m_Position, temp);
	PMath::Vec3fAdd(m_Interest, m_Interest, temp);
	temp[0] = m_Camera[0];
	temp[1] = m_Camera[4];
	temp[2] = m_Camera[8];
	PMath::Vec3fScale(temp, -k10 * mouseMotionX, temp);
	PMath::Vec3fAdd(m_Position, m_Position, temp);
	PMath::Vec3fAdd(m_Interest, m_Interest, temp);
	LookAt(m_Position, m_Interest, m_Up);
}

void Camera::Dolly(float mouseMotionX, float mouseMotionY) {
	PMath::Vec3f temp;
	temp[0] = m_Camera[2];
	temp[1] = m_Camera[6];
	temp[2] = m_Camera[10];
	PMath::Vec3fScale(temp, k10 * mouseMotionY, temp);
	PMath::Vec3fAdd(m_Position, m_Position, temp);
	PMath::Vec3fAdd(m_Interest, m_Interest, temp);
	LookAt(m_Position, m_Interest, m_Up);
}

void Camera::Tumble(float mouseMotionX, float mouseMotionY) {
	PMath::Vec3f temp;
	PMath::Vec3fSubtract(temp, m_Position, m_Interest);
	float tempM[16];
	PMath::Mat44Identity(tempM);
	PMath::Vec3f rot;
	rot[0] = k0; rot[1] = k0; rot[2] = mouseMotionX * k4;
	PMath::Mat44Rotate(tempM, tempM, rot);

	rot[0] = m_Camera[0];
	rot[1] = m_Camera[4];
	rot[2] = m_Camera[8];
	PMath::Vec3fScale(rot, mouseMotionY * k4, rot);
	PMath::Mat44Rotate(tempM, tempM, rot);

	PMath::Mat44Transform3x3(temp, tempM, temp);
	PMath::Vec3fAdd(m_Position, temp, m_Interest);
	LookAt(m_Position, m_Interest, m_Up);
}
