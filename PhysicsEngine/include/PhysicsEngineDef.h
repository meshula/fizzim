
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
	enum	EInertialKind { kI_Immobile, kI_SpringMesh, kI_Box, 
							kI_Sphere, kI_Ellipsoid, kI_HollowSphere, kI_Hemisphere, kI_HemisphereBottomHeavy,
							kI_Cylinder, kI_CylinderBottomHeavy, kI_CylinderThinShell, kI_CylinderThinShellBottomHeavy,
							kI_Torus, kI_Hoop };

	class RigidBody;
	class Engine;
}

#define APILOG printf

#endif
