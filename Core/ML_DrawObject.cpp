
#include "ML_DrawObject.h"
#include "ML_File.h"
#include "ModelReader.h"

void DrawMesh :: CreateFromOBJFile(char const*const pName) {
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
