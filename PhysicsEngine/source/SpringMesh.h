
/** @file	SpringMesh.h
	@brief	A mesh of points connected by springs
	*/

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

#ifndef _SPRINGMESH_H_
#define _SPRINGMESH_H_

#include	"RigidBody.h"
#include	"PMath.h"

namespace Physics {

	/// A single spring mesh spring; much simplified compared to Spring

	class SpringMeshSpring {
	public:
		Real				m_RestLength, m_PrevLength;
		uint32				m_BodyA, m_BodyB;
	};

	/// A single spring mesh body; much simplifed compared to RigidBody

	class SpringMeshBody {
	public:
		PMath::Vec3f				m_Pos0;
		PMath::Vec3f				m_Pos1;
		PMath::Vec3f				m_Vel0;
		PMath::Vec3f				m_Vel1;
		PMath::Vec3f				m_AccelPrev;
		PMath::Vec3f				m_AccForce;
	};

	/// SpringMesh efficiently executes a spring based mesh

	class SpringMesh : public RigidBody {
	public:
		SpringMesh();
		virtual ~SpringMesh();

		void SetPoints (
			int count, 
			PMath::Vec3f* pPoints);

		/// Set an array of links ab, ab, ab, etc
		void SetSprings(
			int count,					///< number of links
			uint32* pPoints				///< the point numbers
			);

		/// Calculate rest lengths after springs and bodies have been set
		void SetRestLengths();

		/// reset body for next time step. @return true fell asleep, false = didn't fall asleep
		virtual bool ResetForNextTimeStep();	

		/// integrate from t0 to t0 + 1/2 dt
		virtual	void Integrate1(
			Real dt,					///< time step, seconds
			PMath::Vec3f gravity		///< acceleration due to gravity length/sec
			);

		/// integrate from t0 + 1/2 dt  to  t0+dt
		virtual	void 	Integrate2(
			Real dt,					///< time step, seconds
			PMath::Vec3f gravity		///< acceleration due to gravity length/sec
			);

		virtual	void					Render();														//!< render physics body, for debugging only

		SpringMeshSpring*				m_Springs;
		SpringMeshBody*					m_Points;

		int								m_NumPoints;
		int								m_NumSprings;

		Real							m_Stiffness;		//!< k in Hooke's law, applies to all
		Real							m_Damping;			//!< b in Hooke's law, applies to all

		bool							m_ResistCompression;
	};
}

#endif
