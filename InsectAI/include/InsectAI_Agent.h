
/** @file	Agent.h
	@brief	Most of the AI types are defined here
	*/

#ifndef _AGENT_H_
#define _AGENT_H_

#include "PMath.h"
#include "InsectAI.h"

void DrawCircle();
void DrawFilledCircle();

/// @class	Moveable
/// @brief	Encodes the position and orientation of the Agent

/*
todo:
modify this class so that rotation and position are not exposed, but their effects are exposed
then, the resulting functions can be implemented on the DynamicState in the physics engine
then, Agent can be parameterized Agent<RigidState> with Moveable or DynamicState
then, make a 2D physics engine to make this demo work as is
then, means the InsectAI class can be parameterized with the 3d physics engine as well
*/


namespace InsectAI {
	enum SensingMask { kKindAgent = 1, kKindLight = 2, kKindVehicle = 4 | kKindAgent };

	class Sensor;
	class Actuator;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Entity
/// @brief	Agents and other things which can be sensed derive from this
/// @todo	who owns the state? does it get it deleted? do we template Entity?

	class Entity {
	public:
				Entity() { }
		virtual ~Entity() { }

		virtual void			Update(float dt)						= 0;
		virtual	uint32			GetKind() const							= 0;
		virtual DynamicState*	GetDynamicState()						= 0;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Agent
/// @brief	Agent base class
	class Agent : public Entity {
	public:
				Agent();
		virtual ~Agent();

				void			AllocBrain(int sensorCount, int actuatorCount);
				int				AddSensor(Sensor* pSensor);
				int				AddActuator(Actuator* pActuator);

		virtual bool			Sense(EntityDatabase*)	= 0;
		virtual void			ClearSenses(float dt)										= 0;

				int				GetSensorCount() const										{ return m_MaxSensor; }
				int				GetActuatorCount() const									{ return m_MaxActuator; }

				Sensor*			GetSensor(int i) const										{ return m_Sensors[i]; }
				Actuator*		GetActuator(int i) const									{ return m_Actuators[i]; }

	protected:
				int				m_MaxSensor;
				int				m_MaxActuator;

				Actuator**		m_Actuators;
				Sensor**		m_Sensors;
				uint32			m_Kind;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Light
/// @brief	A sense-able Light source
///			Lights are implemented as agents because Sensors are sensitive to agents
	class Light : public Entity {
	public:
				Light();
		virtual ~Light();

				void			Update(float dt);
				uint32			GetKind() const							{ return kKindLight; }
		static	uint32			GetStaticKind()							{ return kKindLight; }
	};

}	// end namespace InsectAI

#endif
