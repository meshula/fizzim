
/** @file	InsectAI.h
	@brief	The main public interface to the InsectAI system
*/

#ifndef _INSECTAI_H_
#define _INSECTAI_H_

#include "PMath.h"



/// @namespace	InsectAI
/// @brief		The main public interface is contained in the InsectAI namespace
namespace InsectAI {

	class Actuator;
	class Agent;
	class Engine;
	class Entity;
	class Sensor;
	class Vehicle;

	class DynamicState {
	public:
		DynamicState() { }
		virtual ~DynamicState() { }

		virtual Real const*const	GetPosition() const = 0;
		virtual Real const			GetHeading() const = 0;
		virtual Real const			GetPitch() const = 0;
	};

	class EntityDatabase {
	public:
		EntityDatabase() { }
		virtual ~EntityDatabase() { }

		virtual DynamicState* GetState(Entity*) = 0;
		virtual DynamicState* GetNearest(Entity*, uint32 filter) = 0;
	};

}	// end namespace InsectAI


#include "InsectAI_Actuator.h"
#include "InsectAI_Agent.h"
#include "InsectAI_Engine.h"
#include "InsectAI_Sensor.h"
#include "InsectAI_Vehicle.h"


#endif
