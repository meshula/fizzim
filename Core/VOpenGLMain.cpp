
#include "SDL.h"
#include "GraphObj.h"
#include "Application.h"

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

void VOpenGLMain::ClearWindow() {
	glDrawBuffer(GL_BACK);
	glViewport ( 0, 0, mWindowWidth, mWindowHeight);
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();

	float wWidth = (float) mWindowWidth;
	float wHeight = (float) mWindowHeight;
	float right, top;

	if (mWindowWidth > mWindowHeight) {
		right = wWidth / wHeight;
		top = 1.0f;
	}
	else {
		right = 1.0f;
		top = wHeight / wWidth;
	}

	mRight = right;
	mTop = top;

	gluOrtho2D (0.0f, right, 0.0f, top);					// left right, bottom top
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);									// The Type Of Depth Test To Do
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

GLvoid KillGLWindow(GLvoid)									// Properly Kill The Window
{
}

bool InitGL(int Width, int Height) 
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	
	glClearDepth(1.0);					
	glDepthFunc(GL_LESS);					
	glEnable(GL_DEPTH_TEST);				
	glDepthMask(GL_TRUE); 
	glShadeModel(GL_SMOOTH);	

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	return true;
}

void VOpenGLMain::ConvertWindowCoordsToOrthoGL(float wX, float wY, float& x, float& y) {
	x = (mRight * (float) wX) / (float) mWindowWidth;
	y = (mTop * ((float) mWindowHeight - (float) wY)) / (float) mWindowHeight;
}

void VOpenGLMain::SetWindowSize(int width, int height, bool fullScreen) {

	VMain::SetWindowSize(width, height, fullScreen);

	uint32 flags = 0;
	if (fullScreen) {
		flags |= SDL_FULLSCREEN;
	}

	flags |= SDL_DOUBLEBUF | SDL_RESIZABLE;

	static bool initted = false;
	if (initted)
		return;

	initted = true;

	int bits = 0;	// natural bit depth
	if ( SDL_SetVideoMode(width, height, bits, SDL_OPENGL | flags) == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// Set the title bar in environments that support it
	SDL_WM_SetCaption(mName.c_str(), NULL);

	if (!InitGL(width, height))						// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
	}
}
