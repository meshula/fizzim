#ifndef _CONSTRAINT_H_
#define _CONSTRAINT_H_

#include "PhysicsEngineDef.h"

namespace Physics {

	/// Pure virtual constraint base class

	class Constraint {
	public:
		Constraint() : m_Active(true) { }
		virtual ~Constraint() { }
		virtual bool Apply() = 0;		///< applies constraint, @return true if the constraint is satisified
		virtual uint32 GetKind() const = 0;
		bool m_Active;
	};

	class DistanceConstraint : public Constraint {
	public:
		DistanceConstraint(uint32 a, RigidBody* pBodyA, uint32 b, RigidBody* pBodyB, Real distance, Real tolerance);
		virtual ~DistanceConstraint() { }

		virtual bool Apply();
		virtual uint32 GetKind() const { return GetStaticKind(); }
		static uint32 GetStaticKind() { return 'dist'; }

		uint32 m_BodyA, m_BodyB;
		RigidBody *mp_BodyA, *mp_BodyB;
		Real	m_Distance, m_Tolerance;

	private:
		Real	m_PrevLength;
	};
}

#endif
