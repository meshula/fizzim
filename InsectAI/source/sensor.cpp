

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <gl/gl.h>
#include <math.h>

#include "PMath.h"
#include "InsectAI.h"

using PMath::randf;

namespace InsectAI {
	void Sensor::Reset() {
		if (mbClearEachFrame) {
			mActivation = 0.0f;
			mSteeringActivation = 0.0f;
		}
		mClosestDistance = 1.0e6f;
	}

	CollisionSensor::CollisionSensor() : Sensor() {
		m_Kind = GetStaticKind();
		m_SensedAgent = Vehicle::GetStaticKind();
		mbDirectional = true;
		mbClearEachFrame = true;
		mbInternalSensor = false;
		mbChooseClosest = true;
	}


	void CollisionSensor::Sense(DynamicState* pFrom, DynamicState* pTo) {
		PMath::Vec3f temp;
		PMath::Vec3fSet(temp, pTo->GetPosition());
		PMath::Vec3fSubtract(temp, pFrom->GetPosition());

		float distance = PMath::Vec2fLength(temp);

		// if close enough to collide
		if (distance < 0.1f) {
			float activation = PMath::Max(0.0f, 1.0f - distance);
			activation *= activation;
			activation = PMath::Min(1.0f, activation);

			// if closer than something else we're avoiding
			if (distance < mClosestDistance) {
				float steeringActivation;

				PMath::Vec2fRotate(temp, pFrom->GetHeading());

				// if the possible collidee is in front of us, or simply very very close
				if ((distance < 0.05f) || (temp[1] > 0.0f)) {
					PMath::Vec3fScale(temp, k1 / distance);
					steeringActivation = -temp[0] + randf(0.0f, 0.1f);	// a tiny bit of noise

					temp[0] = 0.0f;
					temp[1] = 1.0f;
					PMath::Vec2fRotate(temp, pFrom->GetHeading());

					PMath::Vec2f temp2;
					temp2[0] = 0.0f;
					temp2[1] = 1.0f;
					PMath::Vec2fRotate(temp2, pTo->GetHeading());

					// if the collidee is heading towards us or simply very very close
					float dot = PMath::Vec2fDot(temp2, temp);
					if ((distance < 0.05f) || (dot < 0.0f)) {
						mClosestDistance = distance;
						mActivation = activation;
						mSteeringActivation = steeringActivation;
					}
				}
			}
		}
	}

void LightSensor::Sense(DynamicState* pFrom, DynamicState* pTo) {
	PMath::Vec3f temp;
	PMath::Vec3fSet(temp, pTo->GetPosition());
	PMath::Vec3fSubtract(temp, pFrom->GetPosition());

	float distance = PMath::Vec2fLength(temp);
	float activation = PMath::Max(0.0f, 1.0f - distance);
	activation *= activation;
	activation = PMath::Min(1.0f, activation);

	float steeringActivation;

	if (mbDirectional) {
		PMath::Vec3fScale(temp, k1 / distance);
		PMath::Vec2fRotate(temp, pFrom->GetHeading());		// already normalized, so length won't change
		steeringActivation = temp[0] + randf(0.0f, 0.1f);	// a tiny bit of noise
	}
	else {
		steeringActivation = 0.0f;
	}

	if (mbChooseClosest) {
		if (distance < mClosestDistance) {
			mClosestDistance = distance;
			mActivation = activation;
			mSteeringActivation = steeringActivation;
		}
	}
	else {
		mSteeringActivation = steeringActivation;
		mActivation += activation;
	}
}



} // end namespace InsectAI