/** @file ML_Camera.h
	Camera class
	*/

#ifndef _ML_CAMERA_H_
#define _ML_CAMERA_H_

#include "PMath.h"

class Camera {
public:
	Camera() : m_Left(0.0f), m_Right(0.0f), m_Width(1.0f), m_Height(1.0f) {
		PMath::Mat44Identity(m_Camera);
	}
	~Camera() { }

	void Set(const Camera& camera) {
		PMath::Mat44Set(m_Camera, camera.m_Camera);
		PMath::Vec3fSet(m_Position, camera.m_Position);
		PMath::Vec3fSet(m_Interest, camera.m_Interest);
		PMath::Vec3fSet(m_Up, camera.m_Up);
	}

	void Interest(PMath::Vec3f pos) {
		PMath::Vec3fSet(m_Interest, pos);
		LookAt(m_Position, m_Interest, m_Up);
	}

	void SetViewport(float left, float right, float width, float height);

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

	void SetCameraMatrices(float width, float height);

	void Track(float mouseMotionX, float mouseMotionY) {
		PMath::Vec3f temp;
		temp[0] = m_Camera[1];
		temp[1] = m_Camera[5];
		temp[2] = m_Camera[9];
		PMath::Vec3fScale(temp, k10 * mouseMotionY, temp);
		PMath::Vec3fAdd(m_Position, m_Position, temp);
		PMath::Vec3fAdd(m_Interest, m_Interest, temp);
		temp[0] = m_Camera[0];
		temp[1] = m_Camera[4];
		temp[2] = m_Camera[8];
		PMath::Vec3fScale(temp, -k10 * mouseMotionX, temp);
		PMath::Vec3fAdd(m_Position, m_Position, temp);
		PMath::Vec3fAdd(m_Interest, m_Interest, temp);
		LookAt(m_Position, m_Interest, m_Up);
	}

	void Dolly(float mouseMotionX, float mouseMotionY) {
		PMath::Vec3f temp;
		temp[0] = m_Camera[2];
		temp[1] = m_Camera[6];
		temp[2] = m_Camera[10];
		PMath::Vec3fScale(temp, k10 * mouseMotionY, temp);
		PMath::Vec3fAdd(m_Position, m_Position, temp);
		PMath::Vec3fAdd(m_Interest, m_Interest, temp);
		LookAt(m_Position, m_Interest, m_Up);
	}

	void Tumble(float mouseMotionX, float mouseMotionY) {
		PMath::Vec3f temp;
		PMath::Vec3fSubtract(temp, m_Position, m_Interest);
		float tempM[16];
		PMath::Mat44Identity(tempM);
		PMath::Vec3f rot;
		rot[0] = k0; rot[1] = k0; rot[2] = mouseMotionX * k4;
		PMath::Mat44Rotate(tempM, tempM, rot);

		rot[0] = m_Camera[0];
		rot[1] = m_Camera[4];
		rot[2] = m_Camera[8];
		PMath::Vec3fScale(rot, mouseMotionY * k4, rot);
		PMath::Mat44Rotate(tempM, tempM, rot);

		PMath::Mat44Transform3x3(temp, tempM, temp);
		PMath::Vec3fAdd(m_Position, temp, m_Interest);
		LookAt(m_Position, m_Interest, m_Up);
	}

	PMath::Vec3f	m_Position;
	PMath::Vec3f	m_Interest;
	PMath::Vec3f	m_Up;

private:
	float			m_Left, m_Right, m_Width, m_Height;
	float			m_Camera[16];
};

#endif
