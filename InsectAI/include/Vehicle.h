
/** @file	Vehicle.h
	@brief	An agent, embodying a Vehicle
	*/

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "agent.h"

namespace InsectAI {

#define MAXSENSORS 10
#define MAXACTUATORS 5

	class Actuator;
	class Sensor;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Vehicle
/// @brief	A Vehicle, as found in Braitenberg's work

	class Vehicle : public Agent {
	public:
		Vehicle(DynamicState* pState);
		virtual ~Vehicle();

		void				Init(float rot, float x, float y);
		void				AllocBrain(int sensorCount, int actuatorCount);

		uint32				GetKind() const { return 'Vhcl'; }
		static uint32		GetStaticKind() { return 'Vhcl'; }

		void				Update(float dt);
		bool				Sense(Agent*);
		void				ClearSenses(float dt);

		int					AddSensor(Sensor* pSensor);
		int					AddActuator(Actuator* pActuator);

		float				mMaxSpeed;

		int			m_MaxSensor;
		int			m_MaxActuator;

		Actuator**	m_Actuators;
		Sensor**	m_Sensors;
	};

}	// end namespace InsectAI

#endif
