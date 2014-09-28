/***************************************

	4D Floating point vector manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVECTOR4D_H__
#define __BRVECTOR4D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector4D_t;
struct Vector4D_t {
	float x;			///< 32 bit floating point X value for the 4D Vector
	float y;			///< 32 bit floating point Y value for the 4D Vector
	float z;			///< 32 bit floating point Z value for the 4D Vector
	float w;			///< 32 bit floating point W value for the 4D Vector
	BURGER_INLINE void Zero(void) { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }
	BURGER_INLINE void One(void) { x = 1.0f; y = 1.0f; z = 1.0f; w = 1.0f; }
	BURGER_INLINE void Identity(void) { x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f; }
	BURGER_INLINE float GetX(void) const { return x; }
	BURGER_INLINE float GetY(void) const { return y; }
	BURGER_INLINE float GetZ(void) const { return z; }
	BURGER_INLINE float GetW(void) const { return w; }
	BURGER_INLINE void SetX(float fX) { x=fX; }
	BURGER_INLINE void SetY(float fY) { y=fY; }
	BURGER_INLINE void SetZ(float fZ) { z=fZ; }
	BURGER_INLINE void SetW(float fW) { z=fW; }
	BURGER_INLINE void Set(float fInput) { x = fInput; y = fInput; z = fInput; w = fInput; };
	BURGER_INLINE void Set(float fX,float fY) { x = fX; y = fY; z = 0.0f; w = 1.0f; };
	BURGER_INLINE void Set(float fX,float fY,float fZ) { x = fX; y = fY; z = fZ; w = 1.0f; };
	BURGER_INLINE void Set(float fX,float fY,float fZ,float fW) { x = fX; y = fY; z = fZ; w = fW; };
	BURGER_INLINE void Set(const Vector4D_t *pInput) { *this = *pInput; }
	void BURGER_API Set(const FixedVector4D_t *pInput);
	BURGER_INLINE void Negate(void) { x = -x; y = -y; z = -z; w = -w; }
	BURGER_INLINE void Negate(float fX,float fY,float fZ,float fW) { x = -fX; y = -fY; z = -fZ; w = -fW; }
	BURGER_INLINE void Negate(const Vector4D_t *pInput) { x = -pInput->x; y = -pInput->y; z = -pInput->z; w = -pInput->w; }
	void BURGER_API Interpolate(const Vector4D_t *pFrom,const Vector4D_t *pTo,float fFactor);
	BURGER_INLINE float Dot(float fX,float fY,float fZ,float fW) const { return (x*fX) + (y*fY) + (z*fZ) + (w*fW); }
	BURGER_INLINE float Dot(const Vector4D_t *pInput) const { return (x*pInput->x) + (y*pInput->y) + (z*pInput->z) + (w*pInput->w); }
	BURGER_INLINE void Add(const Vector4D_t *pInput) { x += pInput->x; y += pInput->y; z += pInput->z; w += pInput->w; }
	BURGER_INLINE void Add(float fInput) { x += fInput; y += fInput; z += fInput; w += fInput; }
	BURGER_INLINE void Add(const Vector4D_t *pInput1,const Vector4D_t *pInput2) { x = pInput1->x+pInput2->x; y = pInput1->y+pInput2->y; z = pInput1->z+pInput2->z; w = pInput1->w+pInput2->w; }
	BURGER_INLINE void Add(const Vector4D_t *pInput,float fInput) { x = pInput->x+fInput; y = pInput->y+fInput; z = pInput->z+fInput; w = pInput->w+fInput; }
	BURGER_INLINE void Sub(const Vector4D_t *pInput) { x -= pInput->x; y -= pInput->y; z -= pInput->z; w -= pInput->w; }
	BURGER_INLINE void Sub(float fInput) { x -= fInput; y -= fInput; z -= fInput; w -= fInput; }
	BURGER_INLINE void Sub(const Vector4D_t *pInput1,const Vector4D_t *pInput2) { x = pInput1->x-pInput2->x; y = pInput1->y-pInput2->y; z = pInput1->z-pInput2->z; w = pInput1->w-pInput2->w; }
	BURGER_INLINE void Sub(const Vector4D_t *pInput,float fInput) { x = pInput->x-fInput; y = pInput->y-fInput; z = pInput->z-fInput; w = pInput->w-fInput; }
	BURGER_INLINE void Mul(const Vector4D_t *pInput) { x *= pInput->x; y *= pInput->y; z *= pInput->z; w *= pInput->w; }
	BURGER_INLINE void Mul(float fInput) { x *= fInput; y *= fInput; z *= fInput; w *= fInput; }
	BURGER_INLINE void Mul(const Vector4D_t *pInput1,const Vector4D_t *pInput2) { x = pInput1->x*pInput2->x; y = pInput1->y*pInput2->y; z = pInput1->z*pInput2->z; w = pInput1->w*pInput2->w; }
	BURGER_INLINE void Mul(const Vector4D_t *pInput,float fInput) { x = pInput->x*fInput; y = pInput->y*fInput; z = pInput->z*fInput; w = pInput->w*fInput; }
	BURGER_INLINE float GetLengthSquared(void) const { return (x*x) + (y*y) + (z*z) + (w*w); }
	BURGER_INLINE float GetLength(void) const { return Sqrt((x*x) + (y*y) + (z*z) + (w*w)); }
	BURGER_INLINE float GetLengthFast(void) const { return Sqrt((x*x) + (y*y) + (z*z) + (w*w)); }
	void BURGER_API SetLength(float fInput);
	void BURGER_API SetLengthFast(float fInput);
	float BURGER_API GetDistanceSquared(float fX,float fY,float fZ,float fW) const;
	float BURGER_API GetDistanceSquared(const Vector4D_t *pInput) const;
	float BURGER_API GetDistance(float fX,float fY,float fZ,float fW) const;
	float BURGER_API GetDistance(const Vector4D_t *pInput) const;
	float BURGER_API GetDistanceFast(float fX,float fY,float fZ,float fW) const;
	float BURGER_API GetDistanceFast(const Vector4D_t *pInput) const;
	void BURGER_API Normalize(void);
	void BURGER_API Normalize(float fX,float fY,float fZ,float fW);
	void BURGER_API Normalize(const Vector4D_t *pInput);
	void BURGER_API NormalizeFast(void);
	void BURGER_API NormalizeFast(float fX,float fY,float fZ,float fW);
	void BURGER_API NormalizeFast(const Vector4D_t *pInput);
	BURGER_INLINE float Dot3(float fX,float fY,float fZ) const { return (x*fX) + (y*fY) + (z*fZ); }
	BURGER_INLINE float Dot3(const Vector4D_t *pInput) const { return (x*pInput->x) + (y*pInput->y) + (z*pInput->z); }
	BURGER_INLINE void QuaternionNegate(void) { x = -x; y = -y; z = -z; }
	BURGER_INLINE void QuaternionNegate(float fX,float fY,float fZ,float fW) { x = -fX; y = -fY; z = -fZ; w = fW; }
	BURGER_INLINE void QuaternionNegate(const Vector4D_t *pInput) { x = -pInput->x; y = -pInput->y; z = -pInput->z; w = pInput->w; }
	void BURGER_API QuaternionMul(const Vector4D_t *pInput);
	void BURGER_API QuaternionMul(const Vector4D_t *pInput1,const Vector4D_t *pInput2);
	void BURGER_API QuaternionMulNormalize(const Vector4D_t *pInput);
	void BURGER_API QuaternionMulNormalize(const Vector4D_t *pInput1,const Vector4D_t *pInput2);
	Word BURGER_API BitwiseEqual(const Vector4D_t *pInput) const;
	BURGER_INLINE float & operator[](Word uInput) { return (&x)[uInput]; }
	BURGER_INLINE const float & operator[](Word uInput) const { return (&x)[uInput]; }
	BURGER_INLINE Word operator == (const Vector4D_t& rInput) const { return (x == rInput.x) && (y == rInput.y) && (z == rInput.z) && (w == rInput.z); }
	BURGER_INLINE Word operator != (const Vector4D_t& rInput) const { return (x != rInput.x) || (y != rInput.y) || (z != rInput.z) || (w != rInput.z); }
};
extern const Vector4D_t g_Vector4DZero;
extern const Vector4D_t g_Vector4DOne;
extern const Vector4D_t g_Vector4DOneX;
extern const Vector4D_t g_Vector4DOneY;
extern const Vector4D_t g_Vector4DOneZ;
extern const Vector4D_t g_Vector4DOneW;
}
/* END */

#endif
