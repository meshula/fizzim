/*
---------------------------------------------------------------------------------------------------
Meshula Physics Demo
Created for Games Programming Gems IV
Copyright (c) 2003 Nick Porcino, http://meshula.net

The MIT License: http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------------------
*/

#include "PMath.h"

using namespace PMath;




void PMath::Mat44Rotate(Mat44& result, const Mat44 src, const Vec3f args)
{
	Real sinx, cosx, siny, cosy, sinz, cosz;
	sinx = Sin(args[0]);
	cosx = Cos(args[0]);
	siny = Sin(args[1]);
	cosy = Cos(args[1]);
	sinz = Sin(args[2]);
	cosz = Cos(args[2]);
	Real cosycosz = cosy * cosz;
	Real sinysinz = siny * sinz;
	Real cosysinz = cosy * sinz;
	Real sinycosz = siny * cosz;
	Real m1 = src[0];
	Real m2 = src[1];
	Real m3 = src[2];
	Real m4 = src[3];
	Real m5 = src[4];
	Real m6 = src[5];
	Real m7 = src[6];
	Real m8 = src[7];

	Real coeff1 = cosycosz + sinx * sinysinz;
	Real coeff2 = sinycosz * sinx - cosysinz;
	Real cosxsiny = cosx * siny;

	result[0] = m1 * coeff1 + m5 * coeff2 + src[8 ]  * cosxsiny;
	result[1] = m2 * coeff1 + m6 * coeff2 + src[9 ]  * cosxsiny;
	result[2] = m3 * coeff1 + m7 * coeff2 + src[10]  * cosxsiny;
	result[3] = m4 * coeff1 + m8 * coeff2 + src[11]  * cosxsiny;

	Real cosxsinz = cosx * sinz;
	Real cosxcosz = cosx * cosz;
	result[4] = m1 * cosxsinz + m5 * cosxcosz - src[8 ]  * sinx;
	result[5] = m2 * cosxsinz + m6 * cosxcosz - src[9 ]  * sinx;
	result[6] = m3 * cosxsinz + m7 * cosxcosz - src[10]  * sinx;
	result[7] = m4 * cosxsinz + m8 * cosxcosz - src[11]  * sinx;

	coeff1 = cosysinz * sinx - sinycosz;
	coeff2 = sinysinz + cosycosz * sinx;
	Real cosxcosy = cosx * cosy; 
	result[8 ] = m1 * coeff1 + m5 * coeff2 + result[8 ] * cosxcosy;
	result[9 ] = m2 * coeff1 + m6 * coeff2 + result[9 ] * cosxcosy;
	result[10] = m3 * coeff1 + m7 * coeff2 + result[10] * cosxcosy;
	result[11] = m4 * coeff1 + m8 * coeff2 + result[11] * cosxcosy;
}


void PMath::QuatToBasis(Mat44& pResult, const Quaternion a)
{
	Real x = a[0];
	Real y = a[1];
	Real z = a[2];
	Real w = a[3];

	Real x2 = x + x; 
	Real y2 = y + y; 
	Real z2 = z + z;

	Real xx = x * x2;
	Real xy = x * y2;
	Real xz = x * z2;
	Real yy = y * y2;
	Real yz = y * z2;
	Real zz = z * z2;
	Real wx = w * x2;
	Real wy = w * y2;
	Real wz = w * z2;

	pResult[ 0] = k1 - (yy + zz);
	pResult[ 1] = xy - wz;
	pResult[ 2] = xz + wy;

	pResult[ 4] = xy + wz;
	pResult[ 5] = k1 - (xx + zz);
	pResult[ 6] = yz - wx;

	pResult[ 8] = xz - wy;
	pResult[ 9] = yz + wx;
	pResult[10] = k1 - (xx +yy);
}

// implements v = dw/dt

void PMath::QuatInputAngularVelocity(Quaternion& result, Real dt, const Quaternion input, const Vec3f a)
{
	Quaternion velquat;
	velquat[0] = a[0];	// x
	velquat[1] = a[1];	// y
	velquat[2] = a[2];	// z
	velquat[3] = k0;	// w

	QuatMultiply(velquat, velquat, input);
	Real scale = dt * kHalf;
	result[0] = input[0] + velquat[0] * scale;
	result[1] = input[1] + velquat[1] * scale;
	result[2] = input[2] + velquat[2] * scale;
	result[3] = input[3] + velquat[3] * scale;

	QuatNormalize(result, result);
}

// 	roll	- rotation around x
//	pitch	- rotation around y
//	yaw		- rotation around z

void PMath::QuatFromEuler(Quaternion& result, Real roll, Real pitch, Real yaw) {
	Real cr, cp, cy, sr, sp, sy, cpcy, spsy;

	Real ti = roll * kHalf;
	Real tj = pitch * kHalf;
	Real th = yaw * kHalf;

	cr = Cos(ti);
	cp = Cos(tj);
	cy = Cos(th);

	sr = Sin(ti);
	sp = Sin(tj);
	sy = Sin(th);

	cpcy = cp * cy;
	spsy = sp * sy;

	result[0] = sr * cpcy - cr * spsy;
	result[1] = cr * sp * cy + sr * cp * sy;
	result[2] = cr * cp * sy - sr * sp * cy;
	result[3] = cr * cpcy + sr * spsy;	// w
}


void PMath::Vec2fNormalize(Vec2f& result, const Vec2f source) {
	Real square = Sqr(source[0]) + Sqr(source[1]);

	if (square > k0) {	  // if vector is zero, don't do anything
		Real mag = RecipSqrt(square);
		result[0] = source[0] * mag; 
		result[1] = source[1] * mag; 
	}
}

void PMath::Vec3fNormalize(Vec3f& result, const Vec3f source)
{
	Real square = Sqr(source[0]) + Sqr(source[1]) + Sqr(source[2]);

	if (square > k0) {	  // if vector is zero, don't do anything
		Real mag = RecipSqrt(square);
		result[0] = source[0] * mag; 
		result[1] = source[1] * mag; 
		result[2] = source[2] * mag; 
	}
}

void PMath::QuatNormalize(Quaternion& result, const Quaternion source)
{
	Real square = Sqr(source[0]) + Sqr(source[1]) + Sqr(source[2]) + Sqr(source[3]);
	if (square > k0) {	  // if Quaternion is zero, don't do anything
		Real mag = RecipSqrt(square);
		result[0] = source[0] * mag; 
		result[1] = source[1] * mag; 
		result[2] = source[2] * mag; 
		result[3] = source[3] * mag; 
	}
}



/*=================================================================================================
 Vec3fPointOnUnitSphere
 Project a 2D point on a circle to a 3D point on the +z hemisphere of a unit sphere. 
 If the 2D point is outside the circle, it is first mapped to the nearest point on 
 the circle before projection.
 Orthographic projection is used, though technically the field of view of the camera
 should be taken into account.  However, the discrepancy is neglegible.
 -------------------------------------------------------------------------------------------------*/

void PMath::Vec3fPointOnUnitSphere (Vec3f& v, const Vec3f p, const Vec3f cueCenter, Real cueRadius) {
	Real    length;
	Real    lengthSquared;

	/* Turn the mouse points into vectors relative to the center of the circle	* and normalize them.  Note we need to flip the y value since the 3D coordinate	* has positive y going up. */
	v[0] = (p[0] - cueCenter[0]) / cueRadius;
	v[1] = -(p[1] - cueCenter[1]) / cueRadius;

	lengthSquared = v[0] * v[0] + v[1] * v[1];

	/* Project the point onto the sphere, assuming orthographic projection.	* Points beyond the virtual sphere are normalized onto 	* edge of the sphere (where z = 0). */
	if (lengthSquared < k1) {
		v[2] = Sqrt(k1 - lengthSquared);
	} else {
		length = Sqrt(lengthSquared);
		v[0] /= length;
		v[1] /= length;
		v[2] = k0;
	}
}

/*=================================================================================================
 Mat44SetRotateVectorToVector
 Computes a rotation matrix that would map (rotate) vectors op onto oq.
  The rotation is about an axis perpendicular to op and oq.
   Note this routine won't work if op or oq are zero vectors, or if they
    are parallel or antiparallel to each other.
	 Modification of Michael Pique's formula in 
	  Graphics Gems Vol. 1.  Andrew Glassner, Ed.  Addison-Wesley.
-------------------------------------------------------------------------------------------------*/

void PMath::Mat44SetRotateVectorToVector (Mat44& result, const Vec3f theop, const Vec3f theoq) {
	Vec3f   op; Vec3fSet(op, theop); PMath::Vec3fNormalize(op, op);
	Vec3f   oq; Vec3fSet(oq, theoq); PMath::Vec3fNormalize(oq, oq);
	Vec3f   a;
	Vec3fCross(a, op, oq);

	Real s = Vec3fLength(a);
	Real c = Vec3fDot(op, oq);
	Real t = k1 - c;

	if (s > k0) {
		Vec3fScale(a, k1 / s);
	}

	Real ax2 = a[0] * a[0];
	Real ay2 = a[1] * a[1];
	Real az2 = a[2] * a[2];

	PMath::Mat44Identity(result);

	result[0] = t * ax2 + c;					// [0][0]
	result[1] = t * a[0] * a[1] + s * a[2];		// [0][1]
	result[2] = t * a[0] * a[2] - s * a[1];		// [0][2]

	result[4] = t * a[0] * a[1] - s * a[2];		// [1][0]
	result[5] = t * ay2 + c;					// [1][1]
	result[6] = t * a[1] * a[2] + s * a[0];		// [1][2]

	result[8] = t * a[0] * a[2] + s * a[1];		// [2][0]
	result[9] = t * a[1] * a[2] - s * a[0];		// [2][1]
	result[10] = t * az2 + c;					// [2][2]
}

/*=================================================================================================
TrackBall
Determine the axis and angle of rotation from the last 2 locations of the mouse
relative to the Virtual Sphere cue circle.  
-------------------------------------------------------------------------------------------------*/

void PMath::Mat44TrackBall(Mat44& result, const Vec3f p, const Vec3f q, const Vec3f cueCenter, Real cueRadius) {
	Vec3f op, oq;

	// Project mouse points to 3-D points on the +z hemisphere of a unit sphere.
	Vec3fPointOnUnitSphere (op, p, cueCenter, cueRadius);
	Vec3fPointOnUnitSphere (oq, q, cueCenter, cueRadius);

	// Consider the two projected points as vectors from the center of the 
	// unit sphere. Compute the rotation matrix that will transform vector op to oq
	Mat44SetRotateVectorToVector(result, op, oq);
}


