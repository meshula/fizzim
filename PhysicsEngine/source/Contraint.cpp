
#include "Constraint.h"
#include "RigidBody.h"
#include "PMath.h"

using namespace Physics;
using PMath::Vec3f;

DistanceConstraint :: DistanceConstraint(uint32 a, RigidBody* pBodyA, uint32 b, RigidBody* pBodyB, Real distance, Real tolerance) 
	: m_BodyA(a), mp_BodyA(pBodyA), m_BodyB(b), mp_BodyB(pBodyB), 
	m_Distance(distance), m_PrevLength(distance), m_Tolerance(tolerance) 
{
}

bool DistanceConstraint :: Apply() {
	bool retval = m_Active;
	if (retval) {
		Vec3f* pPosA = &mp_BodyA->m_StateT1.m_Position;
		Vec3f* pPosB = &mp_BodyB->m_StateT1.m_Position;

		Vec3f direction;
		PMath::Vec3fSubtract(direction, *pPosA, *pPosB);
		Real length = PMath::Vec3fLength(direction);

		Real x = length - m_Distance;

		if (1) {
			Real force = 1000.0f /*pSpring->m_Stiffness*/ * x;

			// damping -b * difference in length between this frame and previous
			Real v = (m_PrevLength - x);
			force += v * 0.95f; //pSpring->m_Damping;

			m_PrevLength = length;

			PMath::Vec3fMultiplyAccumulate(mp_BodyA->m_Acc.m_Force, -force, direction);
			PMath::Vec3fMultiplyAccumulate(mp_BodyB->m_Acc.m_Force,  force, direction);
		}
	}

	return retval;
}

/*
bool DistanceConstraint :: Apply() {
	bool retval = m_Active;
	if (m_Active) {
		Vec3f distance;
		Vec3f* pPosA = &mp_BodyA->m_StateT1.m_Position;
		Vec3f* pPosB = &mp_BodyB->m_StateT1.m_Position;
		PMath::Vec3fSubtract(distance, *pPosA, *pPosB);
		Real distSquared = PMath::Vec3fDot(distance, distance);
		Real desiredDistSquared = m_Distance * m_Distance;
		Real tolSquared = m_Tolerance * m_Tolerance;

		// if length is within tolerance, done
		if (PMath::Abs(desiredDistSquared - distSquared) < tolSquared) {
			retval = true;
		}
		else {
			// else move the bodies towards their correct mutual distance, proportionately to their masses
			Real dist = PMath::Sqrt(distSquared) + 1.0e-4f;	// add a teeny bit to avoid divide by zero problems
			Real delta = (dist - m_Distance) / (dist * (mp_BodyA->GetOOMass() + mp_BodyB->GetOOMass()));

			PMath::Vec3fMultiplyAccumulate(*pPosA, -mp_BodyA->GetOOMass() * delta, distance);
			PMath::Vec3fMultiplyAccumulate(*pPosB,  mp_BodyB->GetOOMass() * delta, distance);

			retval = PMath::Abs((dist - m_Distance) / m_Distance) < m_Tolerance;
		}
	}

	return retval;
}
*/

/*
	bool applyConstraint()
	{
	// if the particles are in the same place, dislocate one slightly
	  if (p1->x == p2->x)
	    p2->x.x+=0.001;
	
	  Vector3 delta(p2->x);
	  // delta is the difference vector between the 2 particles
	  delta -= p1->x;
	  // length of the distance
    float32 deltaLength = delta.length();

    float32 diff = (deltaLength - length) / (deltaLength * (p1->invMass + p2->invMass));
    delta *= diff;
    Vector3 delta2(delta);
    delta *= p1->invMass;
    delta2 *= p2->invMass;
    p1->x += delta;
    p2->x -= delta2;
	      	
	  return ((abs(deltaLength - length)/length) <= satisfied);
	}	
*/
