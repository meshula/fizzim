
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <gl/gl.h>
#include <math.h>

#include "InsectAI.h"

namespace InsectAI {


Vehicle::Vehicle() {
	m_MaxSensor = 0;
	m_MaxActuator = 0;
	m_Actuators = 0;
	m_Sensors = 0;
}

Vehicle::~Vehicle() {
	int i;
	for (i = 0; i < m_MaxActuator; ++i) delete m_Actuators[i];
	for (i = 0; i < m_MaxSensor; ++i)   delete m_Sensors[i];
	delete [] m_Actuators;
	delete [] m_Sensors;
}

void Vehicle::ClearSenses(float dt) {
	int i;
	for (i = 0; i < m_MaxSensor; ++i) {
		m_Sensors[i]->Reset();
	}
}

bool Vehicle::Sense(EntityDatabase* pDB) {
	bool sensed = true;

	for (int i = 0; i < m_MaxSensor; ++i) {
		if (m_Sensors[i]->GetSensorWidth() == Sensor::kNearest) {
			DynamicState* pNearest = pDB->GetNearest(this, m_Sensors[i]->GetSensedAgentKind());
			if (pNearest) {
				m_Sensors[i]->Sense(GetDynamicState(), pNearest);
			}
		}
		else {
		}
	}

	return sensed;
}


void Vehicle::Update(float dt) {
	int i;
	// run functions
	for (i = 0; i < m_MaxSensor; ++i) {
		if (m_Sensors[i]->mbInternalSensor) {
			m_Sensors[i]->Update(dt);
		}
	}

	// update actuators
	for (i = 0; i < m_MaxActuator; ++i) {
		m_Actuators[i]->Update(dt);
	}
}


}
// end namespace InsectAI
