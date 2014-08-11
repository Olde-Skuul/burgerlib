/***************************************

	3D Floating point vector manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVECTOR3D_H__
#define __BRVECTOR3D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector3D_t;
struct Vector3D_t {
	float x;			///< 32 bit floating point X value for the 3D Vector
	float y;			///< 32 bit floating point Y value for the 3D Vector
	float z;			///< 32 bit floating point Z value for the 3D Vector
	BURGER_INLINE void Zero(void) { x = 0.0f; y = 0.0f; z = 0.0f; }
	BURGER_INLINE void One(void) { x = 1.0f; y = 1.0f; z = 1.0f; }
	BURGER_INLINE float GetX(void) const { return x; }
	BURGER_INLINE float GetY(void) const { return y; }
	BURGER_INLINE float GetZ(void) const { return z; }
	BURGER_INLINE void SetX(float fX) { x=fX; }
	BURGER_INLINE void SetY(float fY) { y=fY; }
	BURGER_INLINE void SetZ(float fZ) { z=fZ; }
	BURGER_INLINE void Set(float fInput) { x = fInput; y = fInput; z = fInput; };
	BURGER_INLINE void Set(float fX,float fY) { x = fX; y = fY; z = 0.0f; };
	BURGER_INLINE void Set(float fX,float fY,float fZ) { x = fX; y = fY; z = fZ; };
	BURGER_INLINE void Set(const Vector3D_t *pInput) { *this = *pInput; }
	void BURGER_API Set(const FixedVector3D_t *pInput);
	BURGER_INLINE void Negate(void) { x = -x; y = -y; z = -z; }
	BURGER_INLINE void Negate(float fX,float fY,float fZ) { x = -fX; y = -fY; z = -fZ; }
	BURGER_INLINE void Negate(const Vector3D_t *pInput) { x = -pInput->x; y = -pInput->y; z = -pInput->z; }
	void BURGER_API Interpolate(const Vector3D_t *pFrom,const Vector3D_t *pTo,float fFactor);
	BURGER_INLINE float Dot(float fX,float fY,float fZ) const { return (x*fX) + (y*fY) + (z*fZ); }
	BURGER_INLINE float Dot(const Vector3D_t *pInput) const { return (x*pInput->x) + (y*pInput->y) + (z*pInput->z); }
	BURGER_INLINE void Add(const Vector3D_t *pInput) { x += pInput->x; y += pInput->y; z += pInput->z; }
	BURGER_INLINE void Add(float fInput) { x += fInput; y += fInput; z += fInput; }
	BURGER_INLINE void Add(const Vector3D_t *pInput1,const Vector3D_t *pInput2) { x = pInput1->x+pInput2->x; y = pInput1->y+pInput2->y; z = pInput1->z+pInput2->z; }
	BURGER_INLINE void Add(const Vector3D_t *pInput,float fInput) { x = pInput->x+fInput; y = pInput->y+fInput; z = pInput->z+fInput; }
	BURGER_INLINE void Sub(const Vector3D_t *pInput) { x -= pInput->x; y -= pInput->y; z -= pInput->z; }
	BURGER_INLINE void Sub(float fInput) { x -= fInput; y -= fInput; z -= fInput; }
	BURGER_INLINE void Sub(const Vector3D_t *pInput1,const Vector3D_t *pInput2) { x = pInput1->x-pInput2->x; y = pInput1->y-pInput2->y; z = pInput1->z-pInput2->z; }
	BURGER_INLINE void Sub(const Vector3D_t *pInput,float fInput) { x = pInput->x-fInput; y = pInput->y-fInput; z = pInput->z-fInput; }
	BURGER_INLINE void Mul(const Vector3D_t *pInput) { x *= pInput->x; y *= pInput->y; z *= pInput->z; }
	BURGER_INLINE void Mul(float fInput) { x *= fInput; y *= fInput; z *= fInput; }
	BURGER_INLINE void Mul(const Vector3D_t *pInput1,const Vector3D_t *pInput2) { x = pInput1->x*pInput2->x; y = pInput1->y*pInput2->y; z = pInput1->z*pInput2->z; }
	BURGER_INLINE void Mul(const Vector3D_t *pInput,float fInput) { x = pInput->x*fInput; y = pInput->y*fInput; z = pInput->z*fInput; }
	BURGER_INLINE void MulAdd(const Vector3D_t *pMul,const Vector3D_t *pAdd) { x = (x*pMul->x)+pAdd->x; y = (y*pMul->y)+pAdd->y; z = (z*pMul->z)+pAdd->z; }
	BURGER_INLINE void MulAdd(const Vector3D_t *pMul1,const Vector3D_t *pMul2,const Vector3D_t *pAdd) { x = (pMul1->x*pMul2->x)+pAdd->x; y = (pMul1->y*pMul2->y)+pAdd->y; z = (pMul1->z*pMul2->z)+pAdd->z; }
	BURGER_INLINE void MulAdd(float fInput,const Vector3D_t *pAdd) { x = (x*fInput)+pAdd->x; y = (y*fInput)+pAdd->y; z = (z*fInput)+pAdd->z; }
	BURGER_INLINE void MulAdd(const Vector3D_t *pMul,float fInput,const Vector3D_t *pAdd) { x = (pMul->x*fInput)+pAdd->x; y = (pMul->y*fInput)+pAdd->y; z = (pMul->z*fInput)+pAdd->z; }
	void BURGER_API Cross(const Vector3D_t *pInput);
	void BURGER_API Cross(const Vector3D_t *pInput1,const Vector3D_t *pInput2);
	BURGER_INLINE float GetLengthSquared(void) const { return (x*x) + (y*y) + (z*z); }
	BURGER_INLINE float GetLength(void) const { return Sqrt((x*x) + (y*y) + (z*z)); }
	BURGER_INLINE float GetLengthFast(void) const { return Sqrt((x*x) + (y*y) + (z*z)); }
	void BURGER_API SetLength(float fInput);
	void BURGER_API SetLengthFast(float fInput);
	float BURGER_API GetDistanceSquared(float fX,float fY,float fZ) const;
	float BURGER_API GetDistanceSquared(const Vector3D_t *pInput) const;
	float BURGER_API GetDistance(float fX,float fY,float fZ) const;
	float BURGER_API GetDistance(const Vector3D_t *pInput) const;
	float BURGER_API GetDistanceFast(float fX,float fY,float fZ) const;
	float BURGER_API GetDistanceFast(const Vector3D_t *pInput) const;
	void BURGER_API Normalize(void);
	void BURGER_API Normalize(float fX,float fY,float fZ);
	void BURGER_API Normalize(const Vector3D_t *pInput);
	void BURGER_API PlaneNormal(const Vector3D_t *pOrigin,const Vector3D_t *pPoint1,const Vector3D_t *pPoint2);
	void BURGER_API NormalizeFast(void);
	void BURGER_API NormalizeFast(float fX,float fY,float fZ);
	void BURGER_API NormalizeFast(const Vector3D_t *pInput);
	Word BURGER_API Equal(const Vector3D_t *pInput) const;
	Word BURGER_API Equal(const Vector3D_t *pInput,float fRange) const;
	BURGER_INLINE float & operator[](Word uInput) { return (&x)[uInput]; }
	BURGER_INLINE const float & operator[](Word uInput) const { return (&x)[uInput]; }
	BURGER_INLINE Word operator == (const Vector3D_t& rInput) const { return (x == rInput.x) && (y == rInput.y) && (z == rInput.z); }
	BURGER_INLINE Word operator != (const Vector3D_t& rInput) const { return (x != rInput.x) || (y != rInput.y) || (z != rInput.z); }
};

struct Word32ToVector3D_t {
	union {
		Word32 x[3];		///< Value as three 32 bit unsigned integers
		Vector3D_t v;		///< Value as three 32 bit floats
	};
	BURGER_INLINE operator const Vector3D_t &() const { return v; }
};

extern const Vector3D_t g_Vector3DZero;
extern const Vector3D_t g_Vector3DOne;
extern const Vector3D_t g_Vector3DOneX;
extern const Vector3D_t g_Vector3DOneY;
extern const Vector3D_t g_Vector3DOneZ;
extern const Word32ToVector3D_t g_Vector3DMax;
extern const Word32ToVector3D_t g_Vector3DInfinity;
}
/* END */

#endif
