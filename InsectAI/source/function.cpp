

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <gl/gl.h>
#include <math.h>

#include "InsectAI.h"

namespace InsectAI {

	Function::Function(uint32 func) : Sensor(), mFunction(func) {
		m_Kind = GetStaticKind();
		m_SensedAgent = 0;
		mbClearEachFrame = false;
		mbInternalSensor = true;
		mbDirectional = false;
	}

	Function::~Function() { 
	}

void Function::Update(float dt) {
	int count = (int) mInputs.size();
	if (count >= 0) {
		float input = mInputs[0]->mActivation;
		float steeringInput = mInputs[0]->mSteeringActivation;

		switch (mFunction) {
			case kSigmoid:
				if (input <= 0.0f) mActivation = 0.0f;
				else if (input >= 1.0f) mActivation = 1.0f;
				else mActivation = (1.0f / (1.0f + expf(-(input-0.5f)*24.0f)));	// 24 controls the speed of the slope. -0.5f shifts the range to be centered about 0.5 instead of 0
				break;

			case kInvert:
				mActivation = 1.0f - input;
				break;

			case kBuffer:
			default:
				mActivation = mActivation + (input - mActivation) * 4.0f * dt;	// hysteresis of 0.25 seconds
				break;
		}

		mSteeringActivation = steeringInput;
	}
	else {
		mActivation = 0.0f;
		mSteeringActivation = 0.0f;
	}
}


Switch::Switch() : mpA(0), mpB(0), mpSwitch(0) {
		m_Kind = GetStaticKind();
		m_SensedAgent = 0;
		mbClearEachFrame = true;
		mbInternalSensor = true;
		mbDirectional = false;
	}

	Switch::~Switch() { 
	}


} // end namespace InsectAI
