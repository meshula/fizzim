
/** @file	Vehicle.h
	@brief	An agent, embodying a Vehicle
	*/

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

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
				Vehicle();
		virtual ~Vehicle();

				uint32		GetKind() const { return kKindVehicle; }
		static	uint32		GetStaticKind() { return kKindVehicle; }

				void		Update(float dt);
				bool		Sense(EntityDatabase*);
				void		ClearSenses(float dt);

				float		mMaxSpeed;
	};

}	// end namespace InsectAI

#endif
