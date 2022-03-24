/***************************************

    3D Floating point matrix manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMATRIX3D_H__
#define __BRMATRIX3D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedMatrix3D_t;
struct Matrix4D_t;
struct Matrix3D_t {
	Vector3D_t x;	///< 32 bit floating point X row for the 3D Matrix
	Vector3D_t y;	///< 32 bit floating point Y row for the 3D Matrix
	Vector3D_t z;	///< 32 bit floating point Z row for the 3D Matrix
	void BURGER_API Zero(void);
	void BURGER_API Identity(void);
	void BURGER_API Set(const Matrix3D_t *pInput);
	void BURGER_API Set(const FixedMatrix3D_t *pInput);
	void BURGER_API Set(const Matrix4D_t *pInput);
	void BURGER_API SetYaw(float fYaw);
	void BURGER_API SetPitch(float fPitch);
	void BURGER_API SetRoll(float fRoll);
	void BURGER_API SetYXZ(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetYZX(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetXYZ(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetXZY(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetZYX(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetZXY(float fYaw,float fPitch,float fRoll);
	void BURGER_API SetTranslate(float fX,float fY);
	void BURGER_API SetFromQuaternion(const Vector4D_t *pInput);
	void BURGER_API TransposeSetYaw(float fYaw);
	void BURGER_API TransposeSetPitch(float fPitch);
	void BURGER_API TransposeSetRoll(float fRoll);
	void BURGER_API TransposeSetYXZ(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetYZX(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetXYZ(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetXZY(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetZYX(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetZXY(float fYaw,float fPitch,float fRoll);
	void BURGER_API TransposeSetFromQuaternion(const Vector4D_t *pInput);
	void BURGER_API SetScale(float fX,float fY,float fZ) BURGER_NOEXCEPT;
	void BURGER_API Transpose(void);
	void BURGER_API Transpose(const Matrix3D_t *pInput);
	void BURGER_API GetXRow(Vector3D_t *pOutput) const;
	void BURGER_API GetYRow(Vector3D_t *pOutput) const;
	void BURGER_API GetZRow(Vector3D_t *pOutput) const;
	void BURGER_API GetXColumn(Vector3D_t *pOutput) const;
	void BURGER_API GetYColumn(Vector3D_t *pOutput) const;
	void BURGER_API GetZColumn(Vector3D_t *pOutput) const;
	BURGER_INLINE void SetXRow(const Vector3D_t *pInput) { x.x = pInput->x; x.y = pInput->y; x.z = pInput->z; }
	BURGER_INLINE void SetXRow(float fX,float fY,float fZ) { x.x = fX; x.y = fY; x.z = fZ; }
	BURGER_INLINE void SetYRow(const Vector3D_t *pInput) { y.x = pInput->x; y.y = pInput->y; y.z = pInput->z; }
	BURGER_INLINE void SetYRow(float fX,float fY,float fZ) { y.x = fX; y.y = fY; y.z = fZ; }
	BURGER_INLINE void SetZRow(const Vector3D_t *pInput) { z.x = pInput->x; z.y = pInput->y; z.z = pInput->z; }
	BURGER_INLINE void SetZRow(float fX,float fY,float fZ) { z.x = fX; z.y = fY; z.z = fZ; }
	BURGER_INLINE void SetXColumn(const Vector3D_t *pInput) { x.x = pInput->x; y.x = pInput->y; z.x = pInput->z; }
	BURGER_INLINE void SetXColumn(float fX,float fY,float fZ) { x.x = fX; y.x = fY; z.x = fZ; }
	BURGER_INLINE void SetYColumn(const Vector3D_t *pInput) { x.y = pInput->x; y.y = pInput->y; z.y = pInput->z; }
	BURGER_INLINE void SetYColumn(float fX,float fY,float fZ) { x.y = fX; y.y = fY; z.y = fZ; }
	BURGER_INLINE void SetZColumn(const Vector3D_t *pInput) { x.z = pInput->x; y.z = pInput->y; z.z = pInput->z; }
	BURGER_INLINE void SetZColumn(float fX,float fY,float fZ) { x.z = fX; y.z = fY; z.z = fZ; }
	void BURGER_API Multiply(const Matrix3D_t *pInput);
	void BURGER_API Multiply(const Matrix3D_t *pInput1,const Matrix3D_t *pInput2) BURGER_NOEXCEPT;
	void BURGER_API Multiply(float fScale);
	void BURGER_API Multiply(const Matrix3D_t *pInput,float fScale);
	void BURGER_API Multiply(float fScaleX,float fScaleY,float fScaleZ);
	void BURGER_API Multiply(const Matrix3D_t *pInput,float fScaleX,float fScaleY,float fScaleZ) BURGER_NOEXCEPT;
	void BURGER_API TransposeMultiply(float fScaleX,float fScaleY,float fScaleZ);
	void BURGER_API TransposeMultiply(const Matrix3D_t *pInput,float fScaleX,float fScaleY,float fScaleZ)BURGER_NOEXCEPT;
	void BURGER_API Transform(Vector3D_t *pInput) const;
	void BURGER_API Transform(Vector3D_t *pOutput,const Vector3D_t *pInput) const;
	void BURGER_API TransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransposeTransform(Vector3D_t *pInput) const;
	void BURGER_API TransposeTransform(Vector3D_t *pOutput,const Vector3D_t *pInput) const;
	void BURGER_API TransposeTransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransposeTransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	uint_t BURGER_API AffineInverse(const Matrix3D_t *pInput);
	void BURGER_API Yaw(float fYaw);
	void BURGER_API Pitch(float fPitch);
	void BURGER_API Roll(float fRoll);
	BURGER_INLINE operator const float *() const { return &x.x; }
};
extern const Matrix3D_t g_Matrix3DIdentity;
}
/* END */

#endif
