
#include "GraphObj.h"
#include "PMath.h"

#include <gl/gl.h>

using PMath::Vec3f;

namespace GraphObj {

// ------------------------------------------------------------------------
// Draw a single OpenGL quadrangle given four Vec3 vertices, and color.
// lifted from OpenSteer

inline void iDrawQuadrangle (const Vec3f& a,
                             const Vec3f& b,
                             const Vec3f& c,
                             const Vec3f& d,
                             const Vec3f& color)
{
    glColor3f (color[0], color[1], color[2]);
    glBegin (GL_QUADS);
    {
        glVertex3f(a[0], a[1], a[2]);
        glVertex3f(b[0], b[1], b[2]);
        glVertex3f(c[0], c[1], c[2]);
        glVertex3f(d[0], d[1], d[2]);
    }
    glEnd ();
}
	

// ------------------------------------------------------------------------
// draw a (filled-in, polygon-based) square checkerboard grid on the XZ
// (horizontal) plane.
//
// ("size" is the length of a side of the overall grid, "subsquares" is the
// number of subsquares along each edge (for example a standard checkboard
// has eight), "center" is the 3d position of the center of the grid,
// color1 and color2 are used for alternating subsquares.)
// lifted from OpenSteer

void drawXYCheckerboardGrid (const float size,
                             const int subsquares,
                             const Vec3f& center,
                             const Vec3f& color1,
                             const Vec3f& color2)
{
    const float half = size/2;
    const float spacing = size / subsquares;

	glDisable(GL_LIGHTING);
    //beginDoubleSidedDrawing ();
    {
        bool flag1 = false;
        float p = -half;
        Vec3f corner;
        for (int i = 0; i < subsquares; i++)
        {
            bool flag2 = flag1;
            float q = -half;
            for (int j = 0; j < subsquares; j++)
            {
                corner[0] = p; corner[1] = q; corner[2] = k0;
				PMath::Vec3fAdd(corner, center);
				PMath::Vec3f b, c, d;
				PMath::Vec3fSet(b, corner); b[0] += spacing;
				PMath::Vec3fSet(c, corner); c[0] += spacing; c[1] += spacing;
				PMath::Vec3fSet(d, corner); d[1] += spacing;
//                iDrawQuadrangle (corner,
  //                               corner + Vec3 (spacing,       0, 0 ),
    //                             corner + Vec3 (spacing, spacing, 0 ),
      //                           corner + Vec3 (0,       spacing, 0 ),
        //                         flag2 ? color1 : color2);
				Vec3f color;
				if (flag2) {
					PMath::Vec3fSet(color, color1);
				}
				else {
					PMath::Vec3fSet(color, color2);
				}
				iDrawQuadrangle(corner, b, c, d, color);
                flag2 = !flag2;
                q += spacing;
            }
            flag1 = !flag1;
            p += spacing;
        }
    }
	glEnable(GL_LIGHTING);
    //endDoubleSidedDrawing ();
}


// x-y plane

void Plane::Render()
{
	Vec3f color1; color1[0] = 0.35f;  color1[1] = 0.35f;  color1[2] = 0.35f; 
	Vec3f color2; color2[0] = 0.5f;   color2[1] = 0.5f;   color2[2] = 0.5f; 
	Vec3f center; center[0] = k0; center[1] = k0; center[2] = k0;
	drawXYCheckerboardGrid(100, 20, center, color1, color2);
/*	const float kPlaneSize = 10.0f;
	glBegin(GL_LINES);
	glVertex3f(-kPlaneSize, 0.0f, 0.0f); glVertex3f(kPlaneSize, 0.0f, 0.0f);	// cross
	glVertex3f(0.0f, -kPlaneSize, 0.0f); glVertex3f(0.0f, kPlaneSize, 0.0f);	// cross
	glVertex3f(-0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f); glVertex3f(-0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f);  // box
	glVertex3f(-0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f); glVertex3f(0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f);
	glVertex3f(0.5f * kPlaneSize, 0.5f * kPlaneSize, 0.0f); glVertex3f(0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f);
	glVertex3f(0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f); glVertex3f(-0.5f * kPlaneSize, -0.5f * kPlaneSize, 0.0f);
	glEnd();
	*/
}

void Sphere::Render()
{
	auxSolidSphere(m_Radius);
}



void Mesh::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glVertexPointer (3, GL_FLOAT, 3 * sizeof(float), m_pPositions);
	glNormalPointer (GL_FLOAT, 3 * sizeof(float), m_pNormals);
	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_SHORT, m_Indices);
}

void Mesh::CalcBoundingBox(Vec3f& center, Vec3f& extent)
{
	PMath::Vec3f minPt, maxPt;
	minPt[0] =  1.0e6f; minPt[1] =  1.0e6f; minPt[2] =  1.0e6f;
	maxPt[0] = -1.0e6f; maxPt[1] = -1.0e6f; maxPt[2] = -1.0e6f;
	int i;
	for (i = 0; i < m_IndexCount; ++i) {
		int index = m_Indices[i];
		if (m_pPositions[index*3    ] < minPt[0]) minPt[0] = m_pPositions[index*3    ];
		if (m_pPositions[index*3 + 1] < minPt[1]) minPt[1] = m_pPositions[index*3 + 1];
		if (m_pPositions[index*3 + 2] < minPt[2]) minPt[2] = m_pPositions[index*3 + 2];
		if (m_pPositions[index*3    ] > maxPt[0]) maxPt[0] = m_pPositions[index*3    ];
		if (m_pPositions[index*3 + 1] > maxPt[1]) maxPt[1] = m_pPositions[index*3 + 1];
		if (m_pPositions[index*3 + 2] > maxPt[2]) maxPt[2] = m_pPositions[index*3 + 2];
	}

	for (i = 0; i < 3; ++i) {
		extent[i] = (maxPt[i] - minPt[i]) * kHalf;
		center[i] = minPt[i] + extent[i]; 
	}
}


} // end namespace GraphObj
