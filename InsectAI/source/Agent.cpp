
#include "InsectAI.h"

namespace InsectAI {
	Agent::Agent() {
	}

	Agent::~Agent() {
	}

	void Agent::AllocBrain(int maxSensors, int maxActuators) {
		m_Actuators = new Actuator*[maxActuators];
		m_Sensors = new Sensor*[maxSensors];
		int i;
		for (i = 0; i < m_MaxActuator; ++i) m_Actuators[i] = 0;
		for (i = 0; i < m_MaxSensor; ++i)   m_Sensors[i] = 0;
	}

	int Agent::AddSensor(Sensor* pSensor) {
		int newId = m_MaxSensor;
		++m_MaxSensor;
		m_Sensors[newId] = pSensor;
		return newId;
	}

	int Agent::AddActuator(Actuator* pActuator) {
		int newId = m_MaxActuator;
		++m_MaxActuator;
		m_Actuators[newId] = pActuator;
		return newId;
	}

}
