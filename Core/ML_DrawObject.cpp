
#include "ML_DrawObject.h"
#include "ML_File.h"
#include "ModelReader.h"

void DrawMesh::CreateFromOBJFile(char const*const pName) {
	MeshulaLabs::File modelFile(pName);
	ModelReader reader;
	reader.ReadOBJ(modelFile.GetContents(), modelFile.GetLength());

	GraphObj::Mesh* pRet = new GraphObj::Mesh();

	int temp = (int) reader.normals.size();		
	pRet->m_pNormals = new float[temp];
	memcpy(pRet->m_pNormals, &reader.normals[0], sizeof(float) * temp);

	temp = (int) reader.vertices.size();
	pRet->m_pPositions = new float[temp];
	memcpy(pRet->m_pPositions, &reader.vertices[0], sizeof(float) * temp);

	pRet->m_IndexCount = (int) reader.faceIndices.size();
	pRet->m_Indices =	new uint16[reader.faceIndices.size()];
	for (int i = 0; i < (int) reader.faceIndices.size(); ++i)
		pRet->m_Indices[i] = reader.faceIndices[i];

	SetMesh(pRet);
}

