
#include "ML_DrawObject.h"
#include "ML_File.h"
#include "ModelReader.h"

#include <assert.h>

void DrawMesh :: CreateFromOBJFile(char const*const pName) {
	MeshulaLabs::File modelFile(pName);
	ModelReader reader;
	reader.ReadOBJ(modelFile.GetContents(), modelFile.GetLength());

	GraphObj::Mesh* pRet = new GraphObj::Mesh();

	ModelReader::Model* pModel = reader.models[0];

	ModelReader::VertexPool* pPool = reader.vertexPools[0];

	int temp = (int) pPool->normals.size();		

	pRet->m_pNormals = new float[temp];
	memcpy(pRet->m_pNormals, &pPool->normals[0], sizeof(float) * temp);

	temp = (int) pPool->vertices.size();
	pRet->m_pPositions = new float[temp];
	memcpy(pRet->m_pPositions, &pPool->vertices[0], sizeof(float) * temp);

	size_t i;
	size_t indices = 0;
	for (i = 0; i < pModel->subModels.size(); ++i) {
		indices += pModel->subModels[i]->faceIndices.size();
	}

	pRet->m_IndexCount = indices;
	pRet->m_Indices =	new uint16[indices];

	size_t currIndex = 0;

	// consolidate indices of all submodels into one model
	for (i = 0; i < pModel->subModels.size(); ++i)
		for (size_t j = 0; j < pModel->subModels[i]->faceIndices.size(); ++j)
			pRet->m_Indices[currIndex++] = pModel->subModels[i]->faceIndices[j];

	SetMesh(pRet);
}

void DrawMesh :: CreateFromNDOFile(char const*const pName) {
	assert(false); // NDO not currently supported

#if 0
	MeshulaLabs::File modelFile(pName);
	ModelReader reader;
	reader.ReadNDO((const unsigned char*) modelFile.GetContents(), modelFile.GetLength());

	GraphObj::Mesh* pRet = new GraphObj::Mesh();

	ModelReader::Model* pModel = reader.models[0];

	int temp = (int) pModel->normals.size();		
	pRet->m_pNormals = new float[temp];
	memcpy(pRet->m_pNormals, &pModel->normals[0], sizeof(float) * temp);

	temp = (int) pModel->vertices.size();
	pRet->m_pPositions = new float[temp];
	memcpy(pRet->m_pPositions, &pModel->vertices[0], sizeof(float) * temp);

	pRet->m_IndexCount = (int) pModel->faceIndices.size();
	pRet->m_Indices =	new uint16[pModel->faceIndices.size()];
	for (int i = 0; i < (int) pModel->faceIndices.size(); ++i)
		pRet->m_Indices[i] = pModel->faceIndices[i];

	SetMesh(pRet);
#endif
}

void DrawObject :: DrawBoundingBox() {
	glBegin(GL_LINES);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);

		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);

		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] - m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] - m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] - m_Extent[2]);
		glVertex3f(m_Center[0] + m_Extent[0], m_Center[1] + m_Extent[1], m_Center[2] + m_Extent[2]);
	glEnd();
}

void DrawGrid :: Render() {
	if (Visible()) {
		int i;
		float startX = m_Center[0] - m_Extent[0];
		float startY = m_Center[1] - m_Extent[1];
		float step = (m_Extent[0] * 2.0f) / (float) m_Divisions;

		glBegin(GL_LINES);
			for (i = 0; i <= m_Divisions; ++i) {
				glVertex3f(startX, startY, 0.0f);
				glVertex3f(startX + m_Extent[0] * 2.0f, startY, 0.0f);
				startY += step;
			}
			startY = m_Center[1] - m_Extent[1];
			for (i = 0; i <= m_Divisions; ++i) {
				glVertex3f(startX, startY, 0.0f);
				glVertex3f(startX, startY + m_Extent[0] * 2.0f, 0.0f);
				startX += step;
			}
		glEnd();
	}
}
