/** @file ML_Camera.h
	Camera class
	*/

#ifndef _ML_CAMERA_H_
#define _ML_CAMERA_H_

class Camera {
public:
	Camera() {
		PMath::Mat44Identity(m_Camera);
	}
	~Camera() { }

	void LookAt(Vec3f viewpoint, Vec3f interest, Vec3f up) {
		glMatrixMode(GL_MODELVIEW);

		Vec3f resultX, resultY, resultZ;
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

		glLoadMatrixf(m_Camera);
	}

	void SetCameraMatrices(float width, float height) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();           
		gluPerspective(45.0f, width / height, 1.0f, 1000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m_Camera);
	}

	void Track(float mouseMotionX, float mouseMotionY) {
		Vec3f temp;
		temp[0] = k4 * mouseMotionX;
		temp[1] = k0;
		temp[2] = k4 * -mouseMotionY;
		PMath::Mat44Transform(temp, m_Camera, temp);
		m_Camera[12] += temp[0];
		m_Camera[13] += temp[1];
		m_Camera[14] += temp[2];
	}

	void Dolly(float mouseMotionX, float mouseMotionY) {
		Vec3f temp;
		temp[0] = k4 * mouseMotionX;
		temp[1] = k4 * -mouseMotionY;
		temp[2] = k0;
		PMath::Mat44Transform(temp, m_Camera, temp);
		m_Camera[12] += temp[0];
		m_Camera[13] += temp[1];
		m_Camera[14] += temp[2];
	}

	void Tumble(float mouseMotionX, float mouseMotionY) {
		Vec3f temp;
		temp[0] = -mouseMotionY;
		temp[1] = k0;
		temp[2] = k0;
		PMath::Mat44Rotate(m_Camera, m_Camera, temp);
	}

private:
	float	m_Camera[16];
};

#endif
