
#ifndef _ML_DRAWOBJECT_H_
#define _ML_DRAWOBJECT_H_

#include "PMath.h"
#include "GraphObj.h"
#include "ML_Material.h"

/// base object for drawing

class DrawObject {
public:
	DrawObject(Material* pMaterial) : mp_Material(pMaterial), m_Visible(true) {
		PMath::Mat44Identity(m_LocalToWorld);
	}

	virtual ~DrawObject() { }

	void SetMatrix(float* pMatrix) {
		PMath::Mat44Set(m_LocalToWorld, pMatrix);
	}

	void LoadMatrix() {
		glMatrixMode(GL_MODELVIEW);
		glMultMatrixf(m_LocalToWorld);
	}

	void BindMaterial() {
		mp_Material->Bind();
	}

	void UnbindMaterial() {
		mp_Material->Unbind();
	}

	void Visible(bool v) { 	m_Visible = v; 	}
	bool Visible() const { return m_Visible; }

	void SetMaterial(Material* pMat) {
		mp_Material = pMat;
	}

	Material* GetMaterial() const { 
		return mp_Material;
	}

	void GetPosition(PMath::Vec3f& pos) {
		PMath::Vec3fSet(pos, m_Center);
	}

	void GetWorldPosition(PMath::Vec3f& pos) {
		PMath::Mat44Transform(pos, m_LocalToWorld, m_Center);
	}

	void GetLocalToWorld(float*const pMatrix) {
		for (int i = 0; i < 16; ++i) pMatrix[i] = m_LocalToWorld[i];
	}

	float* GetLocalToWorldPtr() {
		return &m_LocalToWorld[0];
	}

	void GetBoundingCorners(PMath::Vec3f corners[8]) {
		int i;
		float x = k1, y = k1, z = k1;
		for (i = 0; i < 8; ++i) {
			PMath::Vec3fSet(corners[i], m_Center);
			corners[i][0] +=        (i & 1) ? -m_Extent[0] : +m_Extent[0];
			corners[i][1] += ((i >> 1) & 1) ? -m_Extent[1] : +m_Extent[1];
			corners[i][2] += ((i >> 2) & 1) ? -m_Extent[2] : +m_Extent[2];
		}
	}

	void GetWorldBoundingCorners(PMath::Vec3f corners[8]) {
		int i;
		float x = k1, y = k1, z = k1;
		for (i = 0; i < 8; ++i) {
			PMath::Vec3fSet(corners[i], m_Center);
			corners[i][0] +=        (i & 1) ? -m_Extent[0] : +m_Extent[0];
			corners[i][1] += ((i >> 1) & 1) ? -m_Extent[1] : +m_Extent[1];
			corners[i][2] += ((i >> 2) & 1) ? -m_Extent[2] : +m_Extent[2];
			PMath::Mat44Transform(corners[i], m_LocalToWorld, corners[i]);
		}
	}

	void DrawBoundingBox();

	virtual void Render() = 0;
	virtual void CalcBoundingBox() = 0;

	PMath::Vec3f	m_Center;
	PMath::Vec3f	m_Extent;

private:
	bool		m_Visible;
	float		m_LocalToWorld[16];
	Material*	mp_Material;
};

/// an infinite plane in XY

class DrawPlane : public DrawObject {
public:
	DrawPlane(Material* pMaterial) : DrawObject(pMaterial) { CalcBoundingBox(); }
	virtual ~DrawPlane() { }

	virtual void Render() {
		if (Visible())
			m_Plane.Render();
	}

	virtual void CalcBoundingBox() {
		PMath::Vec3fZero(m_Center);
		m_Extent[0] = 1.0e6f;
		m_Extent[1] = 1.0e6f;
		m_Extent[2] = k0;
	}

	GraphObj::Plane m_Plane;
};

/// a mesh

class DrawMesh : public DrawObject {
public:
	DrawMesh(Material* pMaterial) : DrawObject(pMaterial) { }
	virtual ~DrawMesh() { }

	virtual void Render() {
		if (Visible())
			mp_Mesh->Render();
	}

	virtual void CalcBoundingBox() {
		mp_Mesh->CalcBoundingBox(m_Center, m_Extent);
	}

	void SetMesh(GraphObj::Mesh* pMesh) { mp_Mesh = pMesh; CalcBoundingBox(); }

	void CreateFromOBJFile(char const*const pFilename);

private:
	GraphObj::Mesh* mp_Mesh;
};

/// a sphere

class DrawSphere : public DrawObject {
public:
	DrawSphere(Material* pMaterial) : DrawObject(pMaterial) { CalcBoundingBox(); }
	virtual ~DrawSphere() { }

	virtual void Render() {
		if (Visible())
			m_Sphere.Render();
	}

	virtual void CalcBoundingBox() {
		PMath::Vec3fZero(m_Center);
		m_Extent[0] = m_Sphere.m_Radius;
		m_Extent[1] = m_Sphere.m_Radius;
		m_Extent[2] = m_Sphere.m_Radius;
	}

	GraphObj::Sphere m_Sphere;
};

class DrawGrid : public DrawObject {
public:
	DrawGrid(Material* pMaterial, float extent, int divisions) 
		: DrawObject(pMaterial), m_LinearExtent(extent), m_Divisions(divisions) 
	{ 
		CalcBoundingBox(); 
	}

	virtual void Render();

	virtual void CalcBoundingBox() {
		PMath::Vec3fZero(m_Center);
		m_Extent[0] = m_LinearExtent;
		m_Extent[1] = m_LinearExtent;
		m_Extent[2] = k0;
	} 

	float m_LinearExtent;
	int m_Divisions;
};

#endif
