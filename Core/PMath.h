/** @file PMath.h

	Necessary types and utilities for the physics engine
*/
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

#ifndef _PMATH_H_
#define _PMATH_H_

#include <math.h>
#include <stdlib.h>

typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef float			Real;		///< changing this definition would adapt all of PMath to a different float representation

// common constants

#define kEps	Real(0.0001f)
#define kN4	Real(-4.0f)
#define kN3	Real(-3.0f)
#define kN2	Real(-2.0f)
#define kN1	Real(-1.0f)
#define k0		Real(0.0f)
#define kHalf	Real(0.5f)
#define k1		Real(1.0f)
#define k2		Real(2.0f)
#define k3		Real(3.0f)
#define kPi	Real(3.1415926539f)
#define k4		Real(4.0f)
#define k5		Real(5.0f)
#define k6		Real(6.0f)
#define k7		Real(7.0f)
#define k8		Real(8.0f)
#define k9		Real(9.0f)
#define k10	Real(10.0f)
#define k11	Real(11.0f)
#define k12	Real(12.0f)
#define k1000	Real(1000.0f)

/** @namespace PMath
	@brief Physics Math namespace

	Only as much math as is required by the physics engine is included here

	No SIMD implementation is provided, but this API is designed to map naturally onto
	SSE and VU0 intrinsics

	@todo - modify API to return result, not put result in referenced parameter
 */



namespace PMath {
	typedef Real Vec2f[2];			///< 2 component float vector
	typedef Real Vec3f[3];			///< A 3 float vector; SIMD implementation would need 4 components
	typedef Real Vec4f[4];			///< 4 component float vector
	typedef Real Quaternion[4];		///< Stored as xi, yj, zk, w; w is the real component

	/// return a random number between zero and one
	inline Real randf() {
		Real retval = (Real) (rand() & 0x7fff);
		return retval * (1.0f / 32768.0f);
	}

	/// return a random number within the specified range (inclusive)
	inline Real randf(Real minR, Real maxR) {
		Real retval = randf() * (maxR - minR);
		return retval + minR;
	}

	/// Rotation angle in rads
	inline void Vec2fRotate(Vec2f a, Real angle) {
		Real cost = cosf(angle);
		Real sint = sinf(angle);
		Real x = cost * a[0] - sint * a[1];	//  |  cost sint |
		a[1] = sint * a[0] + cost * a[1];		//  | -sint cost |
		a[0] = x;
	}

	template <class Type> Type Min(Type a, Type b)								{ return (a < b) ? a : b; }
	template <class Type> Type Max(Type a, Type b)								{ return (a > b) ? a : b; }
	template <class Type> Type Clamp(Type a, Type mini, Type maxi)				{ return Max(Min(a,maxi),mini); }

	/// Square a real number
	inline	Real Sqr(Real a)													{ return a * a; }

	/// Square root
	inline	Real Sqrt(Real a)													{ return sqrtf(a); }

	/// Absolute value
	inline	Real Abs(Real a)													{ return (a > 0) ? a : -a; }

	/// Reciprocal square root
	inline	Real RecipSqrt(Real a)												{ return (k1 / sqrtf(a)); }

	/// Sine
	inline	Real Sin(Real a)													{ return sinf(a); }

	/// cosine
	inline	Real Cos(Real a)													{ return cosf(a); }

	/// compare one vector to another
	inline	bool Vec3fEqual(const Vec3f a, const Vec3f b, Real eps)				{ return (Abs(a[0]-b[0]) + Abs(a[1]-b[1]) + Abs(a[2]-b[2])) < eps; } 

	/// Copy one vector into another
	inline	void Vec2fSet(Vec2f& a, const Vec2f b)								{ a[0] = b[0]; a[1] = b[1]; }

	/// Copy one vector into another
	inline	void Vec3fSet(Vec3f& a, const Vec3f b)								{ a[0] = b[0]; a[1] = b[1]; a[2] = b[2]; }

	/// return inner product of two vectors
	inline	Real Vec2fDot(const Vec2f a, const Vec2f b)							{ return a[0] * b[0] + a[1] * b[1]; }

	/// return inner product of two vectors
	inline	Real Vec3fDot(const Vec3f a, const Vec3f b)							{ return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }

	/// Zero a vector
	inline	void Vec3fZero(Vec3f& a)											{ a[0] = Real(0.0f); a[1] = Real(0.0f); a[2] = Real(0.0f); }

	/// Return true if vector is zero
	inline bool Vec3fIsZero(const Vec3f a)										{ return Vec3fDot(a, a) <= kEps; }

	/// Get length of vector
	inline	Real Vec2fLength(const Vec2f a)										{ return Sqrt(a[0]*a[0] + a[1]*a[1]); }

	/// Get length of vector
	inline	Real Vec3fLength(const Vec3f a)										{ return Sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]); }

	/// Normalize a vector
			void Vec2fNormalize(Vec2f& result, const Vec2f source);

	/// Normalize a vector
			void Vec3fNormalize(Vec3f& result, const Vec3f source);

	/// Add two vectors
	inline	void Vec3fAdd(Vec3f& a, const Vec3f b)								{ a[0] += b[0]; a[1] += b[1]; a[2] += b[2]; }

	/// Add two vectors
	inline	void Vec3fAdd(Vec3f& result, const Vec3f a, const Vec3f b)			{ result[0] = a[0]+b[0]; result[1] = a[1]+b[1]; result[2] = a[2]+b[2]; }
			void Vec3fPointOnUnitSphere (Vec3f& v, const Vec3f p, const Vec3f cueCenter, Real cueRadius);
	/// Subtract two vectors
	inline	void Vec2fSubtract(Vec2f& a, const Vec2f b)							{ a[0] -= b[0]; a[1] -= b[1]; }

	/// Subtract two vectors
	inline	void Vec3fSubtract(Vec3f& a, const Vec3f b)							{ a[0] -= b[0]; a[1] -= b[1]; a[2] -= b[2]; }

	/// Subtract two vectors
	inline	void Vec3fSubtract(Vec3f& result, const Vec3f a, const Vec3f b)		{ result[0] = a[0]-b[0]; result[1] = a[1]-b[1]; result[2] = a[2]-b[2]; }

	inline	Real Vec3fDistance(const Vec3f a, const Vec3f b)					{ Vec3f temp; Vec3fSubtract(temp, a, b); return Vec3fLength(temp); }

	/// Multiply and accumulate two vectors
	inline	void Vec3fMultiplyAccumulate(Vec3f& a, Real scale, const Vec3f b)	{ a[0] += scale * b[0]; a[1] += scale * b[1]; a[2] += scale * b[2]; }

	/// linearly interpolate between a and b
	inline	void Vec3fLerp(Vec3f& result, Real t, const Vec3f a, const Vec3f b)	{ result[0] = t * a[0] + (k1-t) * b[0]; result[1] = t * a[1] + (k1-t) * b[1]; result[2] = t * a[2] + (k1-t) * b[2]; }

	/// Multiply two vectors, element-wise
	inline	void Vec3fMultiply(Vec3f&a, const Vec3f b, const Vec3f c)			{ a[0] = b[0] * c[0]; a[1] = b[1] * c[1]; a[2] = b[2] * c[2]; }

	/// calculate outer product of two vectors
	inline	void Vec3fCross(Vec3f& result, const Vec3f a, const Vec3f b)		{ result[0] = a[1]*b[2] - a[2]*b[1]; result[1] = a[2]*b[0] - a[0]*b[2]; result[2] = a[0]*b[1] - a[1]*b[0]; }

	/// negate a vector in place
	inline  void Vec3fNegate(Vec3f& a)											{ a[0] *= kN1; a[1] *= kN1; a[2] *= kN1; }

	inline	void Vec2fScale(Vec2f& a, Real scale)								{ a[0] *= scale; a[1] *= scale; }
	inline	void Vec3fScale(Vec3f& a, Real scale)								{ a[0] *= scale; a[1] *= scale; a[2] *= scale; }
	inline	void Vec3fScale(Vec3f& a, Real scale, const Vec3f b)				{ a[0] = b[0] * scale; a[1] = b[1] * scale; a[2] = b[2] * scale; }

	/// Set a vector to a scale copy of the other
	inline	void Vec3fSetScaled(Vec3f& a, Real scale, const Vec3f b)			{ a[0] = scale * b[0]; a[1] = scale * b[1]; a[2] = scale * b[2]; }

	/// Set a vector to copy of another vector and setting a particular length
	inline	void Vec3fSetLength(Vec3f& a, const Vec3f b, Real length)
	{
		Real scale = length / Vec3fLength(a);
		a[0] *= scale; a[1] *= scale; a[2] *= scale;
	}

			Real Vec3fDistancePointToSegment(const Vec3f point, const Vec3f point0, const Vec3f point1);

	inline	Real Vec4fDot(const Vec4f a, const Vec4f b)							{ return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3]; }

	/// Create a basis matrix from a quaternion. Only sets upper left 3x3 portion
			void QuatToBasis(Real* pMatrixResult, const Quaternion a);

	/// Copy one quaternion to another
	inline	void QuatSet(Quaternion& a, const Quaternion b)						{ a[0] = b[0]; a[1] = b[1]; a[2] = b[2]; a[3] = b[3]; }

			void QuatFromEuler(Quaternion& a, Real roll, Real pitch, Real yaw);
	
	inline	void Mat44Set(Real *const pResult, Real const*const pMatrix)			{ for (int i = 0; i < 16; ++i) { pResult[i] = pMatrix[i]; } }


	/// Set the first 3 elements of the translation column of a matrix
	inline	void Mat44SetTranslation(Real *const pResult, const Vec3f a)				{ pResult[12] = a[0]; pResult[13] = a[1]; pResult[14] = a[2]; }

	/// Set a matrix to identity
	inline	void Mat44Identity(Real *const pResult)									{ for (int i = 1; i < 15; ++i) pResult[i] = k0; 	pResult[0] = pResult[5] = pResult[10] = pResult[15] = k1; }

	inline	void Mat44LoadTransposed(Real *const pResult, const Vec3f x, const Vec3f y, const Vec3f z) {
		pResult[0] = x[0]; pResult[4] = x[1]; pResult[8] = x[2];
		pResult[1] = y[0]; pResult[5] = y[1]; pResult[9] = y[2];
		pResult[2] = z[0]; pResult[6] = z[1]; pResult[10] = z[2];
	}

	inline void Mat44Scale3x3(Real *const pResult, const Real scale) {
		pResult[0] *= scale; pResult[1] *= scale; pResult[2 ] *= scale;
		pResult[4] *= scale; pResult[5] *= scale; pResult[6 ] *= scale;
		pResult[8] *= scale; pResult[9] *= scale; pResult[10] *= scale;
	}

	inline void Mat44Transpose3x3(Real *const pResult)			{ 
		Real temp = pResult[1]; pResult[1] = pResult[4]; pResult[4] = temp;
		temp = pResult[2]; pResult[2] = pResult[8]; pResult[8] = temp;
		temp = pResult[6]; pResult[6] = pResult[9]; pResult[9] = temp;
	}

	inline	void Mat44Transform3x3(Real *const pResult, Real const*const pMatrix, const Vec3f a) {
		Vec3f temp;																			// a may be pResult, so copy it
		Vec3fSet(temp, a);
		pResult[0] = temp[0] * pMatrix[0] + temp[1] * pMatrix[4] + temp[2] * pMatrix[8];
		pResult[1] = temp[0] * pMatrix[1] + temp[1] * pMatrix[5] + temp[2] * pMatrix[9];
		pResult[2] = temp[0] * pMatrix[2] + temp[1] * pMatrix[6] + temp[2] * pMatrix[10];
	}

	inline	void Mat44Transform(Real *const pResult, Real const*const pMatrix, const Vec3f a) {
		Vec3f temp;																			// a may be pResult, so copy it
		Vec3fSet(temp, a);
		pResult[0] = temp[0] * pMatrix[0] + temp[1] * pMatrix[4] + temp[2] * pMatrix[8]  + pMatrix[12];
		pResult[1] = temp[0] * pMatrix[1] + temp[1] * pMatrix[5] + temp[2] * pMatrix[9]  + pMatrix[13];
		pResult[2] = temp[0] * pMatrix[2] + temp[1] * pMatrix[6] + temp[2] * pMatrix[10] + pMatrix[14];
	}
			void Mat44SetRotateVectorToVector (Real *const pResult, const Vec3f theop, const Vec3f theoq);			void Mat44Rotate(Real *const pResult, Real const*const pMatrix, const Vec3f args);
			void Mat44TrackBall(Real *const pResult, const Vec3f p, const Vec3f q, const Vec3f cueCenter, Real cueRadius);

/* mat44 multiply
			for( i = 0; i < 4; i++ )			{				for( j = 0; j < 4; j++ )				{					tmp = kZero;					for( k = 0; k < 4; k++ )					{						//tmp += mat1[i][k] * mat2[k][j];						tmp += mat1.m_Array[i*4 + k] * mat2.m_Array[k*4 + j];					}					result.m_Array[i*4 + j] = tmp;				}			}*/

	/// Update a quaternion's orientation with an angular velocity
			void QuatInputAngularVelocity(Quaternion& result, Real dt, const Quaternion input, const Vec3f velocity);

	/// Normalize a quaternion
			void QuatNormalize(Quaternion& result, const Quaternion source);

	/// Multiply two quaternions together
	inline 	void QuatMultiply(Quaternion& result, const Quaternion a, const Quaternion b) {
		Real ax = a[0]; Real ay = a[1]; Real az = a[2]; Real aw = a[3];
		Real bx = b[0]; Real by = b[1]; Real bz = b[2]; Real bw = b[3];
		Real tempx = aw * bx  +  ax * bw  +  ay * bz  -  az * by;
		Real tempy = aw * by  +  ay * bw  +  az * bx  -  ax * bz;
		Real tempz = aw * bz  +  az * bw  +  ax * by  -  ay * bx;
		Real tempw = aw * bw  -  ax * bx  -  ay * by  -  az * bz;
		result[0] = tempx; result[1] = tempy; result[2] = tempz; result[3] = tempw;	// done through temps to allow in-place multiply
	}

	// compound types

	/** @class Plane
	*	Plane encoded as a normal, and the distance of the plane from the origin, along the normal
	*	@todo - vectorize and take functions out of class
	*	for documentation on the math, cf http://www.gamasutra.com/features/19991018/Gomez_1.htm
	*/
	class Plane
	{
	public:
		Plane(const Plane& plane) {
			m_D = plane.m_D;
			Vec3fSet(m_Normal, plane.m_Normal);
		}

		Plane(const Vec3f point, const Vec3f normal) {
			Vec3fSet(m_Normal, normal);
			//Vec3fNormalize(m_Normal, normal);				// would someone ever pass in a non-normalized normal?
			m_D = -Vec3fDot(normal, point);
		}

		Plane(const Vec3f p0, const Vec3f p1, const Vec3f p2) {
			Vec3f t1;
			Vec3fSubtract(t1, p1, p0);
			Vec3f t2;
			Vec3fSubtract(t2, p2, p0);
			Vec3fCross(m_Normal, t1, t2);
			Vec3fNormalize(m_Normal, m_Normal);
			m_D = - Vec3fDot(m_Normal, p0);
		}

		~Plane() { }

		Real DistanceToPoint(const Vec3f a) const {
			return m_D + Vec3fDot(m_Normal, a);
		}

		Vec3f	m_Normal;		///< plane normal
		Real	m_D;			///< distance of plane from origin, D = -(point dot normal)
	};
}

// no SIMD implementation included with this version of PMath

#ifdef Vec3fSIMD
#undef Vec3fSIMD
#endif

#endif
