/** @file ML_Camera.h
	Camera class
	*/

#ifndef _ML_CAMERA_H_
#define _ML_CAMERA_H_

#include "PMath.h"

class Camera {
public:
	Camera() : m_Left(0.0f), m_Right(0.0f), m_Width(1.0f), m_Height(1.0f), m_Near(1.0f), m_Far(1000.0f) {
		PMath::Mat44Identity(m_Camera);
	}

	~Camera() { }

	void Set(const Camera& camera);
	void Interest(PMath::Vec3f pos);
	void SetViewport(float left, float right, float width, float height);
	float Near() const { return m_Near; }
	float Far() const { return m_Far; }

	void LookAt(PMath::Vec3f viewpoint, PMath::Vec3f interest, PMath::Vec3f up) {
		PMath::Vec3fSet(m_Position, viewpoint);
		PMath::Vec3fSet(m_Interest, interest);
		PMath::Vec3fSet(m_Up,		up);

		PMath::Vec3f resultX, resultY, resultZ;
		PMath::Vec3fSubtract(resultZ, interest, viewpoint);
		PMath::Vec3fNegate(resultZ);
		PMath::Vec3fCross(resultX, up, resultZ);
		PMath::Vec3fCross(resultY, resultZ, resultX);
		PMath::Vec3fNormalize(resultX, resultX);
		PMath::Vec3fNormalize(resultY, resultY);
		PMath::Vec3fNormalize(resultZ, resultZ);
		PMath::Mat44LoadTransposed(m_Camera, resultX, resultY, resultZ);
		m_Camera[3] = k0; m_Camera[7] = k0; m_Camera[11] = k0; m_Camera[15] = k1;
		m_Camera[12] = -(viewpoint[0] * m_Camera[0] + viewpoint[1] * m_Camera[4] + viewpoint[2] * m_Camera[8]);
		m_Camera[13] = -(viewpoint[0] * m_Camera[1] + viewpoint[1] * m_Camera[5] + viewpoint[2] * m_Camera[9]);
		m_Camera[14] = -(viewpoint[0] * m_Camera[2] + viewpoint[1] * m_Camera[6] + viewpoint[2] * m_Camera[10]);
	}

	void GetCameraMatrix(float *const pMatrix) {
		for (int i = 0; i < 16; ++i) pMatrix[i] = m_Camera[i];
	}

	float const* GetCameraMatrixPtr() { return &m_Camera[0]; }

	void SetCameraMatrices(float width, float height, float near, float far);

	void Track(float mouseMotionX, float mouseMotionY);
	void Dolly(float mouseMotionX, float mouseMotionY);
	void Tumble(float mouseMotionX, float mouseMotionY);

	PMath::Vec3f	m_Position;
	PMath::Vec3f	m_Interest;
	PMath::Vec3f	m_Up;

private:
	float			m_Left, m_Right, m_Width, m_Height;
	float			m_Near, m_Far;
	float			m_Camera[16];
};

#endif
