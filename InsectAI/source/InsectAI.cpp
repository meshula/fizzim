#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <map>

#include "InsectAI.h"

namespace InsectAI {
static uint32 UniqueID()
{
	static uint32 id = 0;
	return ++id;
}

typedef std::map<int, Entity*> EntityMap;		//!< maps unique IDs to RigidBody pointers.

/// @class	EngineAux
/// @brief	Extra data for the agent manager, not exposed in the header file
class EngineAux {
public:
	EngineAux() { }
	~EngineAux() { }

	EntityMap mEntities;
};

Engine::Engine()
{
	m_pAux = new EngineAux();
}

Engine::~Engine()
{
	RemoveAllEntities();
	delete m_pAux;
}

int Engine::AddEntity(Entity* pEntity)
{
	uint32 id				= UniqueID();
	m_pAux->mEntities[id]		= pEntity;				// add it to the sim
	return id;
}

void Engine::RemoveEntity(int id)
{
	if (m_pAux->mEntities.count(id) != 0) {
		m_pAux->mEntities.erase(id);
	}
}

void Engine::RemoveAllEntities()
{
	m_pAux->mEntities.clear();
}

void Engine::UpdateEntities(float dt, EntityDatabase* pDB)
{
	Agent* pAgent;
	EntityMap::iterator aIter;
	EntityMap::iterator aIter2;

	// clear senses
	for (aIter = m_pAux->mEntities.begin(); aIter != m_pAux->mEntities.end(); ++aIter) {
		Entity* pEntity = aIter->second;
		if (pEntity->GetKind() & kKindAgent) {
			pAgent = (Agent*) pEntity;
			pAgent->ClearSenses(dt);
		}
	}

	// publish stimuli to senses
	for (aIter = m_pAux->mEntities.begin(); aIter != m_pAux->mEntities.end(); ++aIter) {
		Entity* pAi = aIter->second;
		if (pAi->GetKind() & kKindAgent) {
			pAgent = (Agent*) pAi;
			pAgent->Sense(pDB);
		}
	}

	// update agents
	for (aIter = m_pAux->mEntities.begin(); aIter != m_pAux->mEntities.end(); ++aIter) {
		Entity* pEntity = aIter->second;
		pEntity->Update(dt);
	}
}

int Engine::GetEntityCount() {
	return (int) m_pAux->mEntities.size();
}



} // end namespace InsectAI
