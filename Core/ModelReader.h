
/** @file ModelReader.h
 */

#ifndef MODELREADER_H
#define MODELREADER_H

#include <vector>

/// A model reading class

class ModelReader
{
public:
	ModelReader() { }
	~ModelReader();

	void ReadOBJ(char const*const pSource, int sourceSize);	///< Read an OBJ format 3d file

	std::vector <float>				vertices;		///< 3 tuple
	std::vector <float>				normals;		///< 3 tuple
	std::vector <float>				mapping;		///< 2 tuple
	std::vector <int>				faceIndices;
	std::vector <int>				normalIndices;
	std::vector <int>				uvIndices;
};

#endif
