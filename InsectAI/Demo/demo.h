#ifndef __DEMO_H_
#define __DEMO_H_

#include "InsectAI.h"
#include "application.h"
#include "NearestNeighbours.h"

#define MAX_AI 200

//draw connections in brains
//make AI query for k nearest neighbours, don't do it in AI
//derive DynamicState from Physics or Physics2D, make position and rotation derived values

class PhysState : public InsectAI::DynamicState {
public:
			PhysState() : m_Rotation(0.0f) { m_Position[0] = k0; m_Position[1] = k0; }
	virtual ~PhysState() { }

			float			DistanceSquared(float x, float y) {
				float dx = x - m_Position[0];
				float dy = y - m_Position[1];
				float distSquared = dx * dx + dy * dy;
				return distSquared;
			}

			float			DistanceSquared(PhysState* pRHS) {
				return DistanceSquared(pRHS->m_Position[0], pRHS->m_Position[1]);
			}

			Real const*const	GetPosition() const { return &m_Position[0]; }
			Real const			GetHeading() const { return m_Rotation; }
			Real const			GetPitch() const { return k0; }

			float			m_Rotation;
			PMath::Vec3f	m_Position;
};

/** @class	Demo
	@brief	Demonstrates the InsectAI library
	*/

class Demo : public VOpenGLMain, InsectAI::EntityDatabase {
public:
			Demo();
	virtual ~Demo();

			enum { kVehicle = 1, kLight = 2 };	// bit masks, so we can or them together for searches

			void	Reset();
			void	Update(float dt);

			void	ClearAll();
			void	BuildTestBrain(InsectAI::Vehicle* pVehicle, uint32 brainType);
			void	CreateDefaultDemo();
			void	ChoosePotentialPick();
			void	DragEntity(int id);
			void	RenderEntities();
			int		FindClosestEntity(float x, float y, float maxDistance);
			void	HighlightEntity(int id, float radius, float red, float green, float blue);
			void	MoveEntities();

			InsectAI::DynamicState* GetState(InsectAI::Entity*);
			InsectAI::DynamicState* GetNearest(InsectAI::Entity*, uint32 filter);

			bool	HandleKey(int key);
			void	MouseMotion(int x, int y);
			void	MouseClick(eMouseButton button);
			void	MouseUnclick(eMouseButton button);
	virtual void	SetWindowSize(int width, int height, bool fullScreen);

	/// Wrap all the agents around a rectangular planar universe
			void	WrapAround(float left, float right, float bottom, float top);

protected:

	void	CreateDemoZero_Zero();
	void	CreateDemoZero_One();
	void	CreateDemoZero_Two();
	void	CreateDemoZero_Three();
	void	CreateDemoOne();
	void	CreateDemoTwo_Zero();
	void	CreateDemoTwo();
	void	CreateDemoThree();
	void	CreateLightSeekingAvoider();

	NearestNeighbours*		m_pNN;

	int						m_AICount;
	uint32					m_AI[MAX_AI];
	PhysState				m_State[MAX_AI];
	uint32					m_Kind[MAX_AI];
	InsectAI::Vehicle*		m_Vehicles[MAX_AI];		///< vehicles are tracked here, so we can render their brains

	char*					m_DemoName;
	int						mDemoMode;
	int						mCurrentPick;
	int						mPotentialPick;
	InsectAI::Engine		m_Engine;
	int						mCurrentDemo;
	float					mMousex, mMousey;
};


#endif
