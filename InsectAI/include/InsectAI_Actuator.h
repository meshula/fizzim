#ifndef __ACTUATOR_H_
#define __ACTUATOR_H_

namespace InsectAI {

	class Sensor;
	#define kSteeringSpeed 0.25f

	/// @class	Actuator
	/// @brief	Actuators are things like steering wheels and motors
	class Actuator {
	public:
		enum { kMotor = 'Motr', kSteering = 'Ster' };

		Actuator(uint32 kind);
		virtual ~Actuator();

		void	Update(float dt);
		void	SetInput(Sensor* pSensor)	{ mpInput = pSensor; }
		uint32	GetKind() const				{ return mKind; }

		Sensor*					mpInput;
		float					mActivation, mSteeringActivation;

	protected:
		uint32					mKind;
	};

} // end namespace InsectAI

#endif
