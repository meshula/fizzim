
/** @file PhysicsEngineDef.h
	@brief This file contains end-user accessible data structures

	This file contains end-user accessible data structures.
	Many classes and structures are declared here to avoid circular
	dependencies between header files.
*/

#ifndef _PHYSICSENGINEDEF_H_
#define _PHYSICSENGINEDEF_H_

#include "PMath.h"


namespace Physics {
	enum	EInertialKind { kI_Immobile, kI_Box, 
							kI_Sphere, kI_Ellipsoid, kI_HollowSphere, kI_Hemisphere, kI_HemisphereBottomHeavy,
							kI_Cylinder, kI_CylinderBottomHeavy, kI_CylinderThinShell, kI_CylinderThinShellBottomHeavy,
							kI_Torus, kI_Hoop };

	class RigidBody;
	class Engine;

	enum	ECollisionKind	{ kC_Plane = 0, kC_Sphere, kC_BoundedPlane, kC_Box, kC_ConvexHull, kC_Mesh }; 

	/**	@class	ICallback
		@brief	An application receives collision callbacks via this class
	*/

	class ICallback {
	public:
		ICallback() { }
		virtual ~ICallback() { }

		// Override this function to respond to collisions as a they occur
		/// @return true if the collision should be resolved, false if it should be ignored
		virtual bool CollisionOccurred(
			uint32 bodyA,					///< The first participant in the collision
			uint32 bodyB,					///< The second participant in the collision
			PMath::Vec3f pos,				///< The position of the contact
			PMath::Vec3f normal,			///< The normal of the contact, relative to bodyA
			PMath::Vec3f forceA,			///< The force that was applied to bodyA to resolve the collision
			PMath::Vec3f forceB				///< The force that was applied to bodyB to resolve the collision
			) = 0;
	};


	/** @class ICollide
	*	Collision data baseclass
	*/
	class IGeometry
	{
	public:
		IGeometry() { }
		virtual ~IGeometry() { }

		virtual uint32 GetKind() = 0;							//!< For cheap RTTI
	};

	/** @class InfPlane
	 *  plane for collision purposes
	 */
	class Plane : public IGeometry
	{
	public:
		Plane(const PMath::Plane& plane) : m_Plane(plane) { }
		virtual ~Plane() { }
		PMath::Plane m_Plane;									//!< The plane representation

		uint32 GetKind() { return kC_Plane; }
	};

	/** @class Sphere
	* Sphere for collision purposes
	*/
	class Sphere : public IGeometry
	{
	public:
		Sphere(const Real radius) : m_Radius(radius) { }
		virtual ~Sphere() { }
		Real m_Radius;

		uint32 GetKind() { return kC_Sphere; }
	};


}

#define APILOG printf

#endif
