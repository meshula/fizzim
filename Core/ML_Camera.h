/** @file ML_Camera.h
	Camera class
	*/

#ifndef _ML_CAMERA_H_
#define _ML_CAMERA_H_

#include "PMath.h"

class Camera {
public:
	Camera() {
		PMath::Mat44Identity(m_Camera);
	}
	~Camera() { }

	void Set(const Camera& camera) {
		PMath::Mat44Set(m_Camera, camera.m_Camera);
		PMath::Vec3fSet(m_Position, camera.m_Position);
		PMath::Vec3fSet(m_Interest, camera.m_Interest);
		PMath::Vec3fSet(m_Up, camera.m_Up);
	}

	/*
Vector4 glToScreen(const Vector4& v) {
    
    // Get the matrices and viewport

    double modelView[16];
    double projection[16];
    double viewport[4];
    double depthRange[2];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_VIEWPORT, viewport);
    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    // Compose the matrices into a single row-major transformation


    Vector4 T[4];
    int r, c, i;
    for (r = 0; r < 4; ++r) {
        for (c = 0; c < 4; ++c) {
            T[r][c] = 0;
            for (i = 0; i < 4; ++i) {
                // OpenGL matrices are column major


                T[r][c] += projection[r + i * 4] * modelView[i + c * 4];
            }
        }
    }

    // Transform the vertex

    Vector4 result;
    for (r = 0; r < 4; ++r) {
        result[r] = T[r].dot(v);
    }

    // Homogeneous divide

    const double rhw = 1 / result.w;

    return Vector4(
        (1 + result.x * rhw) * viewport[2] / 2 + viewport[0],
        (1 - result.y * rhw) * viewport[3] / 2 + viewport[1],
        (result.z * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0],
        rhw);
} 
	*/
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

	void SetCameraMatrices(float width, float height) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();           
		gluPerspective(45.0f, width / height, 1.0f, 1000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m_Camera);
	}

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
		rot[0] = k0; rot[1] = k0; rot[2] = mouseMotionX;
		PMath::Mat44Rotate(tempM, tempM, rot);
		PMath::Mat44Transform3x3(temp, tempM, temp);
		PMath::Vec3fAdd(m_Position, temp, m_Interest);
		LookAt(m_Position, m_Interest, m_Up);
	}

	PMath::Vec3f	m_Position;
	PMath::Vec3f	m_Interest;
	PMath::Vec3f	m_Up;

private:
	float			m_Camera[16];
};

#endif
