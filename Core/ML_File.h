/** @file ML_File.h
 */

/// ultra-simple File class

#ifndef _ML_FILE_H_
#define _ML_FILE_H_

namespace MeshulaLabs {

	class File
	{
	public:
		File(char* pName);
		~File();

		char const* GetContents() const { return m_pBuf; }
		int GetLength() const { return m_BuffSize; }

	protected:
		char*	m_pBuf;
		int		m_BuffSize;
	};

}

#endif
