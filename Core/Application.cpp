
/*
---------------------------------------------------------------------------------------------------
Meshula Physics Demo
Created for Games Programming Gems IV
Copyright (c) 2003 Nick Porcino, http://meshula.net

The MIT License: http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------------------
*/

#include "GraphObj.h"
#include "SDL.h"
#include "application.h"

int MeshulaApp::PollEvents(VMain& handler)
{
	int done = 0;
	int key;

	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		switch (event.type) {
			case SDL_QUIT:			
				done = 1; 
				break;

			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if (!handler.HandleKey(key)) {
					if (key == SDLK_ESCAPE) {
						done = 1;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button) {
					case SDL_BUTTON_WHEELUP:	handler.MouseWheelRollUp();					break;
					case SDL_BUTTON_WHEELDOWN:	handler.MouseWheelRollDown();				break;
					case SDL_BUTTON_LEFT:		handler.MouseClick(VEventHandler::eLeft);	break;
					case SDL_BUTTON_MIDDLE:		handler.MouseClick(VEventHandler::eMiddle);	break;
					case SDL_BUTTON_RIGHT:		handler.MouseClick(VEventHandler::eRight);	break;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				switch (event.button.button) {
					//case SDL_BUTTON_WHEELUP:	handler.MouseWheelRollUp();						break;
					//case SDL_BUTTON_WHEELDOWN:	handler.MouseWheelRollDown();				break;
					case SDL_BUTTON_LEFT:		handler.MouseUnclick(VEventHandler::eLeft);		break;
					case SDL_BUTTON_MIDDLE:		handler.MouseUnclick(VEventHandler::eMiddle);	break;
					case SDL_BUTTON_RIGHT:		handler.MouseUnclick(VEventHandler::eRight);	break;
				}
				break;

			case SDL_VIDEORESIZE:
				handler.SetWindowSize(event.resize.w, event.resize.h, false);
				break;
		}
	}

	int x, y;
	int buttons = SDL_GetMouseState(&x, &y);
	handler.MouseMotion(x, y);
	return done;
}

void VEventHandler::SetWindowSize(int width, int height, bool fullScreen) {
	mWindowWidth = width;
	mWindowHeight = height;
	mbFullScreen = fullScreen;
}

