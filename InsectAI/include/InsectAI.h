
/** @file	InsectAI.h
	@brief	The main public interface to the InsectAI system
*/

#include "PMath.h"

/// @namespace	InsectAI
/// @brief		The main public interface is contained in the InsectAI namespace
namespace InsectAI {

class Agent;
class EngineAux;
class DynamicState;

/// @class	Engine
/// @brief	The AI simulator
class Engine {
public:
	Engine();
	~Engine();

	/// Add an agent to the simulation
	/// @return the ID of the agent
	int		AddAgent(Agent* pAgent);

	void	RemoveAgent(int id);
	void	RemoveAllAgents();
	int		GetAgentCount();
	void	UpdateAgents(float dt);

	// All the following are intended for the simple demo and will be removed.

	/// Pick an agent
	int		FindClosestAgent(float x, float y, float maxDistance);

	/// Teleport an agent to a particular spot
	void	SetAgentPosition(int id, PMath::Vec3f pos);

	/// Highlights a particular agent; this is meant for for the simple demo
	void	HighlightAgent(int id, float radius, float red, float green, float blue);

private:
	EngineAux* m_pAux;
};

}	// end namespace InsectAI
