
/** @file ModelReader.h
 */

#ifndef MODELREADER_H
#define MODELREADER_H

#include <vector>

/// A model reading class

class ModelReader
{
public:
	class VertexPool {
	public:
		VertexPool() { }
		~VertexPool() { }

		std::vector <float>				vertices;		///< 3 tuple
		std::vector <float>				normals;		///< 3 tuple
		std::vector <float>				mapping;		///< 2 tuple
	};

	class SubModel {
	public:
		SubModel() : meshType(kTriangulated), componentType(kIndexedVerts) { }
		~SubModel() { }

		enum MeshType		{ kTriangulated, kTriQuad, kPolygon };
		enum ComponentType	{	kIndexedComponents,					///< each component is indexed individually per vertex
								kIndexedVerts						///< each component is indexed indentically per vertex
							};

		std::vector <int>				edgeIndices;	///< 8 tuple .. vertb, verta, leftface, rightface, succleft, succright, predright, predleft
		std::vector <int>				faceIndices;	///< these currently refer to triangles, not edges
		std::vector <int>				normalIndices;	///< these currently refer to triangles, not edges
		std::vector <int>				uvIndices;		///< these currently refer to triangles, not edges

		VertexPool*						vertexPool;
		MeshType						meshType;
		ComponentType					componentType;
	};

	class Model {
	public:
		Model() { }
		~Model() { 
			int i;
			for (i = 0; i < (int) subModels.size(); ++i) {
				delete subModels[i];
			}
		}

		std::vector<SubModel*> subModels;
	};

	ModelReader() { }
	~ModelReader() {
		int i;
		for (i = 0; i < (int) models.size(); ++i) {
			delete models[i];
		}

		for (i = 0; i < (int) vertexPools.size(); ++i) {
			delete vertexPools[i];
		}
	}

	void ReadOBJ(char const*const pSource, int sourceSize);				///< Read an OBJ format 3d file
	void ReadNDO(unsigned char const*const pSource, int sourceSize);	///< Read an NDO format 3d file

	std::vector <VertexPool*>			vertexPools;
	std::vector <Model*>				models;
};

#endif
