
/** @file	Engine.h
	@brief	The AI simulator
*/

#ifndef _INSECTAI_ENGINE_H_
#define _INSECTAI_ENGINE_H_

namespace InsectAI {

	class EngineAux;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Engine
/// @brief	The AI simulator
	class Engine {
	public:
		Engine();
		~Engine();

		/// Add an Entity to the simulation
		/// @return the ID of the Entity
		int		AddEntity(Entity* pEntity);

		void	RemoveEntity(int id);
		void	RemoveAllEntities();
		int		GetEntityCount();
		void	UpdateEntities(float dt, EntityDatabase* pDB);

	private:
		EngineAux* m_pAux;
	};

}	// end namespace InsectAI

#endif
