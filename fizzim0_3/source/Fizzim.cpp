// Fizzim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>

#include <expat.h>

#include "basictypes.h"
#include "Fizzim.h"
#include "StringTable.h"

///////////////////// LANDRU EXPORT ////////////////////////////////////

void FizzimLandruExport(FILE* pOutFile, Fizzim::MachineSet& machines)
{
	fprintf(pOutFile, "Landru {\n");
	for (int i = 0; i < machines.m_Nodes.size(); ++i)
	{
		int machineName = machines.m_Nodes[i]->m_Name;
		fprintf(pOutFile, "   %s {\n", s_StringTable[machineName]);

		for (int j = 0; j < machines.m_Nodes[i]->m_Transitions.size(); ++j)
		{
			int gotoID = machines.m_Nodes[machines.m_Nodes[i]->m_Transitions[j]->m_To]->m_Name;
			int conditionID = machines.m_Nodes[i]->m_Transitions[j]->m_Condition;
			fprintf(pOutFile, "      on (%s) { goto %s }\n", s_StringTable[conditionID], s_StringTable[gotoID]);
		}
		fprintf(pOutFile, "   }\n");
	}
	fprintf(pOutFile, "}\n");
}

///////////////////// XML IMPORT  /////////////////////////////////////

int Depth;

FILE* outFile;
class Group;
static std::vector<Group*> s_Groups;

static StringTable s_StringTable;

class Group
{
public:
	Group() :	m_NodeID(-1),
				m_TextID(-1),
				m_ArcID(-1),
				m_From(-1),
				m_To(-1)
	{
	}

	void Render(FILE* outFile)
	{
		if (m_NodeID != -1)
		{
			fprintf(outFile, "box %ld, %s\n", m_NodeID, s_StringTable[m_TextID]);
		}
		else if (m_ArcID != -1)
		{
			fprintf(outFile, "arc %ld, %s from %ld to %ld\n", m_ArcID, s_StringTable[m_TextID], m_From, m_To);
		}
	}

	int16 m_NodeID;
	int16 m_TextID;
	int16 m_ArcID;
	int16 m_From;
	int16 m_To;
};

#define kBox 0
#define kArc 1
#define kText 2

static Group* s_pCurrGroup = 0;

static int groupNest = 0;

static void start(void *data, const char *el, const char **attr) 
{
	// parse group
	if (!strcmp(el, "dia:group"))
	{
		if (s_pCurrGroup)
			printf("Error - nested groups not yet supported\n");

		++groupNest;
	}

	// else parse object and its attributes
	// currently, all objects have to be contained within a group
	else if (!strcmp(el, "dia:object"))
	{
		if (!s_pCurrGroup)
		{
			s_pCurrGroup = new Group();
		}

		int16 kind = -1;
		int16 id;
		for (int i = 0; attr[i]; i += 2)
		{
			if (!strcmp(attr[i], "type"))
			{
				if (!strcmp(attr[i+1], "UML - State"))
				{
					kind = kBox;
				}
				else if (!strcmp(attr[i+1], "Standard - Box"))
				{
					kind = kBox;
				}
				else if (!strcmp(attr[i+1], "Standard - Text"))
				{
					kind = kText;
				}
				else if (!strcmp(attr[i+1], "FS - Orthflow"))
				{
					kind = kArc;
				}
				else if (!strcmp(attr[i+1], "Standard - ZigZagLine"))
				{
					kind = kArc;
				}
			}
			else if (!strcmp(attr[i], "id"))
			{
				const char* sId = attr[i+1];
				id = atoi(&sId[1]);
			}
		}

		switch (kind)
		{
		case kBox:	s_pCurrGroup->m_NodeID = id; break;

		// don't record text, text id's are actually the names
		//case kText:	s_pCurrGroup->m_TextID = id; break;

		case kArc:	s_pCurrGroup->m_ArcID = id; break;
		}
	}

	// we might actually be a connection
	else if (s_pCurrGroup && !strcmp(el, "dia:connection"))
	{
		int16 handle = -1;
		int16 to;
		for (int i = 0; attr[i]; i += 2)
		{
			if (!strcmp(attr[i], "handle"))
			{
				handle = atoi(attr[i+1]);
			}
			else if (!strcmp(attr[i], "to"))
			{
				const char* sId = attr[i+1];
				to = atoi(&sId[1]);
			}
		}

		if (handle == 0)
			s_pCurrGroup->m_From = to;
		else
			s_pCurrGroup->m_To = to;
	}

  int i;

  for (i = 0; i < Depth; i++)
    fprintf(outFile, "  ");

  fprintf(outFile, "%s", el);

  for (i = 0; attr[i]; i += 2) {
    fprintf(outFile, " %s='%s'", attr[i], attr[i + 1]);
  }

  fprintf(outFile, "\n");
  Depth++;
}  /* End of start handler */

static void end(void *data, const char *el) 
{
	if (s_pCurrGroup)
	{
		// terminating a group?
		if (!strcmp(el, "dia:group"))
		{
			--groupNest;

			// if completely denested...
			if (!groupNest)
			{
				s_Groups.push_back(s_pCurrGroup);
				s_pCurrGroup = 0;
			}
		}

		// terminating an object?
		else if (!strcmp(el, "dia:object"))
		{
			// if not grouped, push back the current group
			if (0 == groupNest)
			{
				s_Groups.push_back(s_pCurrGroup);
				s_pCurrGroup = 0;
			}
		}
	}

	--Depth;
}  /* End of end handler */


static void FizzimCharacterDataHandler(void *userData, const XML_Char *s, int len)
{
	if (s_pCurrGroup && s[0] == '#')
	{
		char buff[2048];
		strncpy(buff, s+1, len-1);

		if (s[len-1] != '#')
		{
			printf("Error - Found whitespace in name. %s", buff);
		}
		else
		{
			buff[len-2] = '\0';	// discard the trailing #
		}

		if (s_pCurrGroup->m_ArcID != -1)
		{
			char* pString = new char[len+1];
			strcpy(pString, buff);
			s_pCurrGroup->m_TextID = s_StringTable.Add(pString);
		}
		else if (s_pCurrGroup->m_NodeID != -1)
		{
			char* pString = new char[len+1];
			strcpy(pString, buff);
			s_pCurrGroup->m_TextID = s_StringTable.Add(pString);
		}
		else
		{
			printf("??? %s\n", buff);
		}
	}
}

void SanityCheck()
{
}

class NodeDescriptor
{
public:
	NodeDescriptor(int id, int name) : m_ID(id), m_TextID(name), m_HasIncomingArc(false) { }

	int		m_ID;
	int		m_TextID;
	bool	m_HasIncomingArc;
};

class ArcDescriptor
{
public:
	ArcDescriptor(int id, int name, int from, int to) : m_ID(id), m_TextID(name), m_From(from), m_To(to) { }

	int		m_ID;
	int		m_TextID;
	int		m_From;
	int		m_To;
};


void CreateMachines(Fizzim::MachineSet& machines)
{
	int i;

	std::vector <NodeDescriptor*>	allNodes;
	std::vector <ArcDescriptor*>	allArcs;

	// find all nodes and arcs
	for (i = 0; i < s_Groups.size(); ++i)
	{
		if (s_Groups[i]->m_NodeID != -1)
		{
			NodeDescriptor* pND = new NodeDescriptor(s_Groups[i]->m_NodeID, s_Groups[i]->m_TextID);
			allNodes.push_back(pND);
		}
		else if (s_Groups[i]->m_ArcID != -1)
		{
			ArcDescriptor* pAD = new ArcDescriptor(s_Groups[i]->m_NodeID, s_Groups[i]->m_TextID, s_Groups[i]->m_From, s_Groups[i]->m_To);
			allArcs.push_back(pAD);
		}
	}

	// now mark all nodes that are transitioned into
	for (i = 0; i < allArcs.size(); ++i)
	{
		int to = allArcs[i]->m_To;
		for (int j = 0; j < allNodes.size(); ++j)
		{
			if (allNodes[j]->m_ID == to)
				allNodes[j]->m_HasIncomingArc = true;
		}
	}

	std::vector <int> nodeIDs;

	// loop over all nodes
	for (i = 0; i < allNodes.size(); ++i)
	{
		// record all start nodes
		if (!allNodes[i]->m_HasIncomingArc)
		{
			machines.m_StartNodes.push_back(machines.m_Nodes.size());
		}

		int id = allNodes[i]->m_ID;

		// record all states
		Fizzim::Node* pNode = new Fizzim::Node(allNodes[i]->m_TextID);
		nodeIDs.push_back(allNodes[i]->m_ID);

		// add all transitions from this state
		for (int j = 0; j < allArcs.size(); ++j)
		{
			if (allArcs[j]->m_From == id)
			{
				Fizzim::Transition* pTrans = new Fizzim::Transition(allArcs[j]->m_TextID, allArcs[j]->m_To);
				pNode->m_Transitions.push_back(pTrans);
			}
		}

		machines.m_Nodes.push_back(pNode);
	}

	// now resolve all transitions to refer to the proper nodes

	for (i = 0; i < machines.m_Nodes.size(); ++i)
	{
		// loop over all transitions within a state
		for (int j = 0; j < machines.m_Nodes[i]->m_Transitions.size(); ++j)
		{
			// search all nodeIDs for the one to transition to
			for (int k = 0; k < nodeIDs.size(); ++k)
			{
				// if the cached nodeID refers to the node we want to transition to,
				if (machines.m_Nodes[i]->m_Transitions[j]->m_To == nodeIDs[k])
				{
					// then k is the index of the node we are transitioning to
					machines.m_Nodes[i]->m_Transitions[j]->m_To = k;
					
					// stop the search and continue with the next transition.
					break;
				}
			}
		}
	}

	// clean up
	for (i = 0; i < allNodes.size(); ++i)
		delete allNodes[i];
	for (i = 0; i < allArcs.size(); ++i)
		delete allArcs[i];
}

///////////////////// MAIN /////////////////////////////////////////////

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("No filename specified.\n");
		exit(-1);
	}

	outFile = fopen("test.txt", "wt");

	std::string filename(argv[1]);

	FILE* inFile = fopen(filename.c_str(), "rb");
	int result = fseek(inFile, 0, SEEK_END);
	int fileLength = ftell(inFile);
	result = fseek(inFile, 0, SEEK_SET);

	char* buffer = new char[fileLength+2];

	fileLength = fread(buffer, 1, fileLength, inFile);
	buffer[fileLength] = 0;

	printf("read %ld bytes\n", fileLength);

	XML_Parser p = XML_ParserCreate(NULL);
	if (! p) {
		fprintf(stderr, "Couldn't allocate memory for parser\n");
		exit(-1);
	}
	
	XML_SetElementHandler(p, start, end);
	XML_SetCharacterDataHandler(p, FizzimCharacterDataHandler);
	
	bool done = false;

	if (! XML_Parse(p, buffer, fileLength-1, done)) 
	{
		printf("Parse error at line %d:\n%s\n",
			XML_GetCurrentLineNumber(p),
			XML_ErrorString(XML_GetErrorCode(p)));
		done = true;
	}
	
	fclose(outFile);

	Fizzim::MachineSet machines;
	CreateMachines(machines);

	outFile = fopen("test.landru", "wt");
	FizzimLandruExport(outFile, machines);
	fclose(outFile);

	// now sanity check the read-in data
	SanityCheck();

	// deallocate the groups
	for (int i = 0; i < s_Groups.size(); ++i)
		delete s_Groups[i];

	delete [] buffer;

	return 0;
}
