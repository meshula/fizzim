
#include "GraphObj.h"

namespace GraphObj {
// x-y plane

void Plane::Render()
{
	const float kPlaneSize = 10.0f;
	glBegin(GL_LINES);
	glVertex3f(-kPlaneSize, 0.0f, 0.0f); glVertex3f(kPlaneSize, 0.0f, 0.0f);	// cross
	glVertex3f(0.0f, -kPlaneSize, 0.0f); glVertex3f(0.0f, kPlaneSize, 0.0f);	// cross
	glVertex3f(-0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f); glVertex3f(-0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f);  // box
	glVertex3f(-0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f); glVertex3f(0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f);
	glVertex3f(0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f); glVertex3f(0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f);
	glVertex3f(0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f); glVertex3f(-0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f);
	glEnd();
}

void Sphere::Render()
{
	auxSolidSphere(m_Radius);
}

} // end namespace GraphObj
