/***************************************

	3D Floating point matrix manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
struct Matrix3D_t {
	Vector3D_t x;	///< 32 bit floating point X row for the 3D Matrix
	Vector3D_t y;	///< 32 bit floating point Y row for the 3D Matrix
	Vector3D_t z;	///< 32 bit floating point Z row for the 3D Matrix
	void BURGER_API Zero(void);
	void BURGER_API Identity(void);
	void BURGER_API Set(const Matrix3D_t *pInput);
	void BURGER_API Set(const FixedMatrix3D_t *pInput);
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
	void BURGER_API Transpose(void);
	void BURGER_API Transpose(const Matrix3D_t *pInput);
	void BURGER_API GetXRow(Vector3D_t *pOutput) const;
	void BURGER_API GetYRow(Vector3D_t *pOutput) const;
	void BURGER_API GetZRow(Vector3D_t *pOutput) const;
	void BURGER_API GetXColumn(Vector3D_t *pOutput) const;
	void BURGER_API GetYColumn(Vector3D_t *pOutput) const;
	void BURGER_API GetZColumn(Vector3D_t *pOutput) const;
	void BURGER_API SetXRow(const Vector3D_t *pInput);
	void BURGER_API SetYRow(const Vector3D_t *pInput);
	void BURGER_API SetZRow(const Vector3D_t *pInput);
	void BURGER_API SetXColumn(const Vector3D_t *pInput);
	void BURGER_API SetYColumn(const Vector3D_t *pInput);
	void BURGER_API SetZColumn(const Vector3D_t *pInput);
	void BURGER_API Multiply(const Matrix3D_t *pInput);
	void BURGER_API Multiply(const Matrix3D_t *pInput1,const Matrix3D_t *pInput2);
	void BURGER_API Multiply(float fScale);
	void BURGER_API Multiply(const Matrix3D_t *pInput,float fScale);
	void BURGER_API Transform(Vector3D_t *pInput) const;
	void BURGER_API Transform(Vector3D_t *pOutput,const Vector3D_t *pInput) const;
	void BURGER_API TransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransposeTransform(Vector3D_t *pInput) const;
	void BURGER_API TransposeTransform(Vector3D_t *pOutput,const Vector3D_t *pInput) const;
	void BURGER_API TransposeTransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
	void BURGER_API TransposeTransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const;
};
extern const Matrix3D_t s_Matrix3DIdentity;
}
/* END */

#endif
