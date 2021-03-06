
/** @file	ML_Material.h
	@brief	Meshula Labs materials
 */

#ifndef _ML_MATERIAL_H_
#define _ML_MATERIAL_H_

/// Base Material
/// Base state:
/// lighting disabled, all lights disabled
/// Color is unspecified
/// GL_COLOR_MATERIAL is off

class Material {
public:
	Material() { }
	virtual ~Material() { }
	virtual void Bind() = 0;		///< activates a material
	virtual void Unbind() = 0;		///< returns state to base state
};

/// Standard Diffuse Material

class DiffuseMaterial : public Material {
public:
	DiffuseMaterial() : m_Lit(true) { 
		for (int i = 0; i < 3; ++i) m_Diffuse[i] = 1.0f;
	}
	~DiffuseMaterial() { }

	virtual void Bind() {
		if (m_Lit) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
		}
		glColor3fv(m_Diffuse);
		glEnable(GL_COLOR_MATERIAL);
	}

	virtual void Unbind() {
		if (m_Lit) {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}
		glColor3f(1.0f, 1.0f, 1.0f);
		glDisable(GL_COLOR_MATERIAL);
	}

	bool	m_Lit;
	float	m_Diffuse[4];
};

#endif
