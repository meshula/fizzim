
#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include "CRC32.h"

class StringTable
{
public:
	StringTable()
	{
	}

	~StringTable()
	{
		int i;
		for (i = 0; i < m_Strings.size(); ++i)
			delete [] m_Strings[i];
	}

	// returns new index
	uint32	Add(char const*const pString)
	{
		uint32 hash = m_CRC32.Calculate(pString, strlen(pString));
		int i;
		for (i = 0; i < m_Strings.size(); ++i)
		{
			if (m_Hashes[i] == hash)
				return i;
		}

		m_Hashes.push_back(hash);

		char* pNewString = new char[strlen(pString) + 1];
		strcpy(pNewString, pString);

		m_Strings.push_back(pNewString);
	}

	int32	Exists(char const*const pString)
	{
		uint32 hash = m_CRC32.Calculate(pString, strlen(pString));

		int i;
		for (i = 0; i < m_Strings.size(); ++i)
		{
			if (m_Hashes[i] == hash)
				return i;
		}

		return -1;
	}

	std::vector <uint32>	m_Hashes;
	std::vector <char*>		m_Strings;

	CRC32					m_CRC32;
};

#endif
