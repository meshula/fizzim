

#include "InsectAI.h"

namespace InsectAI {

	Actuator::Actuator(uint32 kind) : mKind(kind) {
	}

	Actuator::~Actuator() {
	}


void Actuator::Update(float dt) {
	mActivation = mpInput->mActivation;		// to do, time integration or weighting
	mSteeringActivation = mpInput->mSteeringActivation;
}

} // end namespace InsectAI