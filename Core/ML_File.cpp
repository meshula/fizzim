
#include <cstdio>
#include <cstring>

#include "ML_File.h"


MeshulaLabs :: File :: File(char* pName) {

#ifdef WIN32
	int length = (int) std::strlen(pName);
	char* _pName = new char[length + 1];
	for (int i = 0; i < length; ++i) {
		if (pName[i] == '/')
			_pName[i] = '\\';
		else
			_pName[i] = pName[i];
	}
	_pName[i] = '\0';
#else
	char* _pName = pName;
#endif

	FILE* pFile = fopen(_pName, "rb");
	fseek(pFile, 0, SEEK_END);
	m_BuffSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	
	m_pBuf = new char[m_BuffSize + 1];
	fread(m_pBuf, 1, m_BuffSize, pFile);
	fclose(pFile);
	m_pBuf[m_BuffSize] = '\0';

#ifdef WIN32
	delete [] _pName;
#endif
}

MeshulaLabs :: File :: ~File() {
	delete [] m_pBuf;
}
