
/** @file	DynamicState.h
	@brief	Instantaneous rigid body dynamic state
*/


/** The state of a rigid body
 */

class DynamicState
{
public:
	DynamicState()
	{
		SetAtRest();
	}

	~DynamicState() { }

	DynamicState& operator=(const DynamicState& right)
	{	// assign _Right
		if (this != &right) {
			PMath::QuatSet(m_Orientation,		right.m_Orientation);
			PMath::Vec3fSet(m_Position,			right.m_Position);
			PMath::Vec3fSet(m_Velocity,			right.m_Velocity);
			PMath::Vec3fSet(m_AngularVelocity,	right.m_AngularVelocity);
			PMath::Vec3fSet(m_AngularMomentum,	right.m_AngularMomentum);
		}
		return (*this);
	}

	void	SetAtRest()
	{
		PMath::QuatFromEuler(m_Orientation, 0, 0, 0);
		PMath::Vec3fZero(m_Position);
		PMath::Vec3fZero(m_Velocity);
		PMath::Vec3fZero(m_AngularVelocity);
		PMath::Vec3fZero(m_AngularMomentum);
	}

	PMath::Quaternion	m_Orientation;
	PMath::Vec3f		m_Position;
	PMath::Vec3f		m_Velocity;
	PMath::Vec3f		m_AngularVelocity;
	PMath::Vec3f		m_AngularMomentum;
};
