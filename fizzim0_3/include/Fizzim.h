
#ifndef FIZZIM_H
#define FIZZIM_H

#include <vector>

namespace Fizzim
{
	class Transition
	{
	public:
		Transition(int condition, int to) : m_Condition(condition), m_To(to) { }
		~Transition() { }

		int m_To;
		int m_Condition;
	};

	class Node
	{
	public:
		Node(int name) : m_Name(name) { }
		~Node()
		{
			for (int i = 0; i < m_Transitions.size(); ++i)
				delete m_Transitions[i];
		}

		int							m_Name;
		std::vector <Transition*>	m_Transitions;
	};

	class MachineSet
	{
	public:
		MachineSet() { }
		~MachineSet()
		{
			for (int i = 0; i < m_Nodes.size(); ++i)
				delete m_Nodes[i];
		}

		std::vector <int>	m_StartNodes;
		std::vector <Node*>	m_Nodes;
	};
}

#endif
