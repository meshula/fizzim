
/// SpringMesh.cpp

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

#include "stdio.h"
#include "SpringMesh.h"
#include "PMath.h"

using PMath::Vec3f;

namespace Physics {

///////////////////////////////////////////////////////////////////////////////////////////////

	SpringMesh :: SpringMesh() : m_Springs(0), m_Points(0), m_ResistCompression(true)
	{
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	SpringMesh :: ~SpringMesh()
	{
		delete [] m_Springs;
		delete [] m_Points;
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: SetPoints(int numPoints, Vec3f* pPoints)
	{
		m_Points	= new SpringMeshBody[numPoints];
		m_NumPoints = numPoints;

		for (int i = 0; i < m_NumPoints; ++i) {
			PMath::Vec3fSet(m_Points[i].m_Pos0, pPoints[i]);
			PMath::Vec3fSet(m_Points[i].m_Pos1, pPoints[i]);
			PMath::Vec3fZero(m_Points[i].m_Vel0);
			PMath::Vec3fZero(m_Points[i].m_Vel1);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: SetSprings(int numSprings, uint32* pBodies)
	{
		m_Springs 	 = new SpringMeshSpring[numSprings];
		m_NumSprings = numSprings;
		
		for (int i = 0; i < m_NumSprings; ++i) {
			m_Springs[i].m_BodyA = pBodies[i * 2];
			m_Springs[i].m_BodyB = pBodies[i * 2 + 1];
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: SetRestLengths()
	{
		if (m_Springs != 0 && m_Points != 0) {
			for (int i = 0; i < m_NumSprings; ++i) {
				Vec3f dx;
				m_Springs[i].m_RestLength = PMath::Vec3fLength(dx);
				m_Springs[i].m_PrevLength = m_Springs[i].m_RestLength;
			}
		}
		else {
			APILOG("Must set springs and points before calculating rest lengths\n");
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: Render()
	{
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	bool SpringMesh :: ResetForNextTimeStep()
	{
		// first, reset the overall state of the sim
		// this is needed for the overall application of friction

		bool fellAsleep = RigidBody::ResetForNextTimeStep();

		// now, update all the points within the spring mesh for iteration
		for (int i = 0; i < m_NumPoints; ++i) {
			PMath::Vec3fSet(m_Points[i].m_Pos0, m_Points[i].m_Pos1);
			PMath::Vec3fSet(m_Points[i].m_Vel0, m_Points[i].m_Vel1);
		}
		return fellAsleep;
	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: Integrate1(Real dt, PMath::Vec3f gravity)
	{
		int i;

		for (i = 0; i < m_NumPoints; ++i) {
			PMath::Vec3fMultiplyAccumulate(m_Points[i].m_Vel1, dt * kHalf,		m_Points[i].m_AccelPrev);					// v += 1/2 a * t
			PMath::Vec3fMultiplyAccumulate(m_Points[i].m_Pos1, dt,				m_Points[i].m_Vel1);			// pos += v * dt
			PMath::Vec3fMultiplyAccumulate(m_Points[i].m_Pos1, kHalf * dt * dt,	m_Points[i].m_AccelPrev);					// pos += 1/2 a * t * t
		}

		// now calculate all springs
		// loop over all springs
		//		add forces to appropriate bodies

		for (i = 0; i < m_NumSprings; ++i) {
			Vec3f direction;
			PMath::Vec3fSubtract(direction, m_Points[m_Springs[i].m_BodyA].m_Pos1, m_Points[m_Springs[i].m_BodyB].m_Pos1);
			Real length = PMath::Vec3fLength(direction);

			Real x = length - m_Springs[i].m_RestLength;

			if (m_ResistCompression || (x > k0)) {
				Real force = m_Stiffness * x;

				// damping -b * difference in length between this frame and previous
				Real v = (m_Springs[i].m_PrevLength - x);
				force += v * m_Damping;

				m_Springs[i].m_PrevLength = length;

				PMath::Vec3fMultiplyAccumulate(m_Points[m_Springs[i].m_BodyA].m_AccForce, -force, direction);
				PMath::Vec3fMultiplyAccumulate(m_Points[m_Springs[i].m_BodyB].m_AccForce,  force, direction);
			}
		}

	}

///////////////////////////////////////////////////////////////////////////////////////////////

	void SpringMesh :: Integrate2(Real dt, PMath::Vec3f gravity)
	{
		for (int i = 0; i < m_NumPoints; ++i) {
			PMath::Vec3fSetScaled(m_Points[i].m_AccelPrev, m_OOMass, m_Acc.m_Force);									// dvdt = f / m     works for gravity? yes:  f = mg    a = mg / m = g
			if (m_Gravity) {
				PMath::Vec3fAdd(m_Points[i].m_AccelPrev, gravity);
			}
			PMath::Vec3fMultiplyAccumulate(m_Points[i].m_Vel1, dt * kHalf, m_Points[i].m_AccelPrev);					// v += 1/2 a * t
		}

		// calculate average velocity and place it in the velocity, this is used for friction force

		// finally, clear out the accumulator for next time
		PMath::Vec3fZero(m_Acc.m_Force);																// clear out the force accumulator
	}

///////////////////////////////////////////////////////////////////////////////////////////////

}	// end namespace Physics
