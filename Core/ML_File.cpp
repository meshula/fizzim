
#include <cstdio>
#include <cstring>

#include "ML_File.h"

#ifdef WIN32
#define WRONGSLASH '/'
#define GOODSLASH '\\'
#else
#define WRONGSLASH '\\'
#define GOODSLASH '/'
#endif

MeshulaLabs :: File :: File(char const*const pName) {
	int length = (int) std::strlen(pName);
	char* _pName = new char[length + 1];
	for (int i = 0; i < length; ++i) {
		if (pName[i] == WRONGSLASH)
			_pName[i] = GOODSLASH;
		else
			_pName[i] = pName[i];
	}
	_pName[i] = '\0';

	FILE* pFile = fopen(_pName, "rb");
	if (pFile) {
		fseek(pFile, 0, SEEK_END);
		m_BuffSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		
		m_pBuf = new char[m_BuffSize + 1];
		fread(m_pBuf, 1, m_BuffSize, pFile);
		fclose(pFile);
	}
	else {
		m_BuffSize = 0;
		m_pBuf = new char[m_BuffSize + 1];
	}

	m_pBuf[m_BuffSize] = '\0';

	delete [] _pName;
}

MeshulaLabs :: File :: ~File() {
	delete [] m_pBuf;
}
