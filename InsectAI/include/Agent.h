
/** @file	Agent.h
	@brief	Most of the AI types are defined here
	*/

#ifndef _AGENT_H_
#define _AGENT_H_

#include "PMath.h"

void DrawCircle();
void DrawFilledCircle();

/// @class	Moveable
/// @brief	Encodes the position and orientation of the Agent

/*
todo:
modify this class so that rotation and position are not exposed, but their effects are exposed
then, the resulting functions can be implemented on the DynamicState in the physics engine
then, Agent can be parameterized Agent<RigidState> with Moveable or DynamicState
then, make a 2D physics engine to make this demo work as is
then, means the InsectAI class can be parameterized with the 3d physics engine as well
*/


namespace InsectAI {

	class DynamicState {
	public:
		DynamicState() : mRotation(0.0f) { }
		virtual ~DynamicState() { }

		float DistanceSquared(float x, float y) {
			float dx = x - mPosition[0];
			float dy = y - mPosition[1];
			float distSquared = dx * dx + dy * dy;
			return distSquared;
		}

		float DistanceSquared(DynamicState* pRHS) {
			return DistanceSquared(pRHS->mPosition[0], pRHS->mPosition[1]);
		}

		float			mRotation;
		PMath::Vec2f	mPosition;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Agent
/// @brief	Agent base class
	class Agent {
	public:
		Agent(DynamicState* pState);
		virtual ~Agent();

		virtual void			Update(float dt)							= 0;
		virtual bool			Sense(Agent* pA)							= 0;
		virtual void			ClearSenses(float dt)						= 0;
		virtual	uint32			GetKind() const								= 0;

		DynamicState*			m_pDynamicState;

	protected:
		uint32					m_Kind;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Light
/// @brief	A sense-able Light source
///			Lights are implemented as agents because Sensors are sensitive to agents
	class Light : public Agent {
	public:
		Light(DynamicState* pState);
		virtual ~Light();
		void Init(float phase, float x, float y);

		void	Update(float dt);
		bool	Sense(Agent*) { return false; }
		void	ClearSenses(float) { }
		uint32	GetKind() const { return 'Lght'; }
		static uint32 GetStaticKind() { return 'Lght'; }
	};

}	// end namespace InsectAI

#endif
