
/** @file	ML_Material.h
	@brief	Meshula Labs materials
 */

#ifndef _ML_MATERIAL_H_
#define _ML_MATERIAL_H_

/// Base Material
/// Base state:
/// lighting disabled, all lights disabled

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
	DiffuseMaterial() { }
	~DiffuseMaterial() { }

	virtual void Bind() {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	virtual void Unbind() {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
};

#endif
