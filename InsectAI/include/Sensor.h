
#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <vector>
#include "agent.h"
#include "vehicle.h"

namespace InsectAI {

/// @class	Sensor
/// @brief	Virtual base class for all sensors
///			Sensors are sensitive to particular kinds of agents
class Sensor {
public:
	Sensor() : m_Kind(0), m_SensedAgent(0), mbChooseClosest(false), mActivation(0.0f), mSteeringActivation(0.0f) {
	}

	virtual ~Sensor() { }

	void Reset();

	virtual void			Update(float dt) { }

	virtual void			Sense(float fromRotation, PMath::Vec2f& mFrom, float toRotation, PMath::Vec2f& mTo) = 0;

			uint32			GetSensedAgentKind() const { return m_SensedAgent; }
			uint32			GetKind() const { return m_Kind; }

	bool					mbDirectional;
	bool					mbInternalSensor;
	bool					mbClearEachFrame;
	bool					mbChooseClosest;						///< if false, accumulate. if true, choose closest
	float					mClosestDistance;
	float					mActivation;
	float					mSteeringActivation;

protected:
	uint32					m_Kind;									///< RTTI
	uint32					m_SensedAgent;							///< the kind of agent that can be sensed
};

/// @class	LightSensor
/// @brief	The LightSensor can sense Light Agents
class LightSensor : public Sensor {
public:
	LightSensor(bool directional) : Sensor() {
		m_Kind = GetStaticKind();
		m_SensedAgent = Light::GetStaticKind();
		mbDirectional = directional;
		mbClearEachFrame = true;
		mbInternalSensor = false;
	}

	virtual ~LightSensor() { }

	static uint32 GetStaticKind() { return 'Lght'; }

	virtual void Sense(float fromRotation, PMath::Vec2f& mFrom, float toRotation, PMath::Vec2f& mTo);
};

/// @class	CollisionSensor
/// @brief	Sensitive to collisions versus Vehicles
class CollisionSensor : public Sensor {
public:
	CollisionSensor() : Sensor() {
		m_Kind = GetStaticKind();
		m_SensedAgent = Vehicle::GetStaticKind();
		mbDirectional = true;
		mbClearEachFrame = true;
		mbInternalSensor = false;
		mbChooseClosest = true;
	}

	virtual ~CollisionSensor() { }
	static uint32 GetStaticKind() { return 'Clld'; }

	virtual void Sense(float fromRotation, PMath::Vec2f& mFrom, float toRotation, PMath::Vec2f& mTo);
};


/// @class	Switch
/// @brief	A switch toggles to on when it's activation level exceeds 1/2
class Switch : public Sensor {
public:
	Switch();
	virtual ~Switch();
	static uint32 GetStaticKind() { return 'Swch'; }

	void SetControl(Sensor* pS) { mpSwitch = pS; }
	void SetInputs(Sensor* pA, Sensor* pB) { mpA = pA; mpB = pB; }

	virtual void Sense(float fromRotation, PMath::Vec2f& mFrom, float toRotation, PMath::Vec2f& mTo) { }
	void Update(float dt) {
		bool useA = mpSwitch->mActivation <= 0.5f;
		mActivation = useA ? mpA->mActivation : mpB->mActivation;
		mSteeringActivation = useA ? mpA->mSteeringActivation : mpB->mSteeringActivation;
	}

	Sensor* mpSwitch;
	Sensor* mpA;
	Sensor* mpB;
};

/// @class	Function
/// @brief	a function is a sensor that senses one or more other sensors
class Function : public Sensor {
public:
	enum {
		kBuffer	= 'buff',
		kInvert	= 'nvrt',
		kSigmoid = 'sigm'
	};

	Function(uint32 func);
	virtual ~Function();
	static uint32 GetStaticKind() { return 'Func'; }

	void AddInput(Sensor* pSensor) {
		mInputs.push_back(pSensor);
	}

	void Update(float dt);

	virtual void Sense(float fromRotation, PMath::Vec2f& mFrom, float toRotation, PMath::Vec2f& mTo) { }
	uint32 mFunction;
	std::vector <Sensor*> mInputs;
};

} // end namespace InsectAI

#endif
