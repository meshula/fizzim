
/** @file GraphObj.h */
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

#ifndef _GRAPHOBJ_H_
#define _GRAPHOBJ_H_

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <gl/glaux.h>            // Allows us to draw a sphere using
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#include "PMath.h"

/// @namespace	GraphObj
/// @brief		drawable things
namespace GraphObj {

class Base {
public:
	Base () { }
	virtual ~Base() { }

	virtual void Render() = 0;
};

class Plane : public Base {
public:
	Plane() { }
	virtual ~Plane() { }

	void Render();
};

class Sphere : public Base {
public:
	Sphere() : m_Radius(1.0f) { }
	virtual ~Sphere() { }

	void Render();
	float m_Radius;
};

class Mesh : public Base {
public:
	Mesh() { }
	~Mesh() { }

	void Render();

	void CalcBoundingBox(PMath::Vec3f& center, PMath::Vec3f& extent);

	int		m_IndexCount;
	float*	m_pNormals;			///< memory not owned by Mesh object
	float*	m_pPositions;		///< memory not owned by Mesh object
	uint16*	m_Indices;			///< memory not owned by Mesh object
};

}	// end namespace GraphObj

#endif
