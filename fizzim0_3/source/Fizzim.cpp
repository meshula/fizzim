// Fizzim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "basictypes.h"

#include <vector>

// TestParser.cpp : Defines the entry point for the console application.
//

#include <expat.h>

#include <string>

int Depth;

FILE* outFile;

class Group
{
public:
	Group() :	m_BoxID(-1),
				m_TextID(-1),
				m_ArcID(-1),
				m_From(-1),
				m_To(-1)
	{
	}

	void Render(FILE* outFile)
	{
		if (m_BoxID != -1)
		{
			fprintf(outFile, "box %ld, text %ld\n", m_BoxID, m_TextID);
		}
		else if (m_ArcID != -1)
		{
			fprintf(outFile, "arc %ld, text %ld from %ld to %ld\n", m_ArcID, m_TextID, m_From, m_To);
		}
	}

	int16 m_BoxID;
	int16 m_TextID;
	int16 m_ArcID;
	int16 m_From;
	int16 m_To;
};

#define kBox 0
#define kArc 1
#define kText 2

static std::vector<Group*> s_Groups;

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
				if (!strcmp(attr[i+1], "Standard - Box"))
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
		case kBox:	s_pCurrGroup->m_BoxID = id; break;
		case kText:	s_pCurrGroup->m_TextID = id; break;
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

	Depth--;
}  /* End of end handler */


static void FizzimCharacterDataHandler(void *userData, const XML_Char *s, int len)
{
	if (s_pCurrGroup && s[0] == '#')
	{
		char buff[2048];
		strncpy(buff, s, len);
		buff[len] = '\0';

		if (s[len-1] != '#')
		{
			printf("Error - Found whitespace in name. %s", buff);
		}

		if (s_pCurrGroup->m_ArcID != -1)
		{
			printf("Arc: textid:%ld %s\n", s_pCurrGroup->m_TextID, buff);
		}
		else if (s_pCurrGroup->m_BoxID != -1)
		{
			printf("Node: textid:%ld %s\n", s_pCurrGroup->m_TextID, buff);
		}
		else
		{
			printf("??? %s\n", buff);
		}
	}
}

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

	outFile = fopen("test.fsm", "wt");

	fprintf(outFile, "nodes: \n");

	printf("Found %ld groups\n", s_Groups.size());

	int i;
	for (i = 0; i < s_Groups.size(); ++i)
		s_Groups[i]->Render(outFile);

	fclose(outFile);

	// deallocate the groups
	for (i = 0; i < s_Groups.size(); ++i)
		delete s_Groups[i];

	delete [] buffer;

	return 0;
}
