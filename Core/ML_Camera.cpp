

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

void Camera::SetCameraMatrices(float width, float height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();           
	gluPerspective(45.0f, width / height, 1.0f, 1000.0f);
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

