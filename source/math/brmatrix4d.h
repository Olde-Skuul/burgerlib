/***************************************

    4D Floating point matrix manager

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMATRIX4D_H__
#define __BRMATRIX4D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

#ifndef __BRMATRIX3D_H__
#include "brmatrix3d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedMatrix4D_t;
struct Matrix4D_t {
    Vector4D_t x; ///< 32 bit floating point X row for the 4D Matrix
    Vector4D_t y; ///< 32 bit floating point Y row for the 4D Matrix
    Vector4D_t z; ///< 32 bit floating point Z row for the 4D Matrix
    Vector4D_t w; ///< 32 bit floating point W row for the 4D Matrix

    void BURGER_API Zero(void) BURGER_NOEXCEPT;
    void BURGER_API Identity(void) BURGER_NOEXCEPT;
    void BURGER_API Set(const Matrix4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API Set(const FixedMatrix4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API Set(const Matrix3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetYaw(float fYaw) BURGER_NOEXCEPT;
    void BURGER_API SetPitch(float fPitch) BURGER_NOEXCEPT;
    void BURGER_API SetRoll(float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetYXZ(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetYZX(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetXYZ(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetXZY(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetZYX(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetZXY(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API SetTranslate(float fX, float fY, float fZ) BURGER_NOEXCEPT;
    void BURGER_API SetFromQuaternion(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetYaw(float fYaw) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetPitch(float fPitch) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetRoll(float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetYXZ(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetYZX(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetXYZ(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetXZY(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetZYX(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetZXY(
        float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetFromQuaternion(
        const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetScale(float fX, float fY, float fZ) BURGER_NOEXCEPT;
    void BURGER_API SetScale(
        float fX, float fY, float fZ, float fW) BURGER_NOEXCEPT;
    void BURGER_API SetFrustum(float fLeft, float fRight, float fBottom,
        float fTop, float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetFrustum(float fLeft, float fRight,
        float fBottom, float fTop, float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API SetOrtho(float fLeft, float fRight, float fBottom,
        float fTop, float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetOrtho(float fLeft, float fRight, float fBottom,
        float fTop, float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API SetPerspective(float fFieldOfViewY, float fAspect,
        float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API TransposeSetPerspective(float fFieldOfViewY, float fAspect,
        float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API Transpose(void) BURGER_NOEXCEPT;
    void BURGER_API Transpose(const Matrix4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API GetXRow(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetXRow(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetYRow(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetYRow(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetZRow(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetZRow(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetWRow(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetWRow(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetXColumn(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetXColumn(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetYColumn(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetYColumn(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetZColumn(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetZColumn(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetWColumn(Vector3D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API GetWColumn(Vector4D_t* pOutput) const BURGER_NOEXCEPT;
    void BURGER_API SetXRow(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetXRow(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetYRow(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetYRow(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetZRow(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetZRow(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetWRow(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetWRow(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetXColumn(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetXColumn(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetYColumn(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetYColumn(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetZColumn(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetZColumn(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetWColumn(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API SetWColumn(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API Multiply(const Matrix4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API Multiply(
        const Matrix4D_t* pInput1, const Matrix4D_t* pInput2) BURGER_NOEXCEPT;
    void BURGER_API Multiply(float fScale) BURGER_NOEXCEPT;
    void BURGER_API Multiply(
        const Matrix4D_t* pInput, float fScale) BURGER_NOEXCEPT;
    void BURGER_API Multiply(
        float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT;
    void BURGER_API Multiply(const Matrix4D_t* pInput, float fScaleX,
        float fScaleY, float fScaleZ) BURGER_NOEXCEPT;
    void BURGER_API Multiply(float fScaleX, float fScaleY, float fScaleZ,
        float fScaleW) BURGER_NOEXCEPT;
    void BURGER_API Multiply(const Matrix4D_t* pInput, float fScaleX,
        float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT;
    void BURGER_API Multiply3x3(float fInput) BURGER_NOEXCEPT;
    void BURGER_API Multiply3x3(
        const Matrix4D_t* pInput, float fInput) BURGER_NOEXCEPT;
    void BURGER_API TransposeMultiply(
        float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT;
    void BURGER_API TransposeMultiply(const Matrix4D_t* pInput, float fScaleX,
        float fScaleY, float fScaleZ) BURGER_NOEXCEPT;
    void BURGER_API TransposeMultiply(float fScaleX, float fScaleY,
        float fScaleZ, float fScaleW) BURGER_NOEXCEPT;
    void BURGER_API TransposeMultiply(const Matrix4D_t* pInput, float fScaleX,
        float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT;
    void BURGER_API Transform(Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Transform(Vector4D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Transform(
        Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Transform(
        Vector4D_t* pOutput, const Vector4D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform(
        Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform(
        Vector4D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform(
        Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform(
        Vector4D_t* pOutput, const Vector4D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Transform3x3(Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Transform3x3(
        Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform3x3(
        Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API TransposeTransform3x3(
        Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Yaw(float fYaw) BURGER_NOEXCEPT;
    void BURGER_API Pitch(float fPitch) BURGER_NOEXCEPT;
    void BURGER_API Roll(float fRoll) BURGER_NOEXCEPT;
    void BURGER_API Rotate(
        float fRadians, float fX, float fY, float fZ) BURGER_NOEXCEPT;
    void BURGER_API Translate(float fX, float fY, float fZ) BURGER_NOEXCEPT;
    void BURGER_API TransposeTranslate(
        float fX, float fY, float fZ) BURGER_NOEXCEPT;
    uint_t BURGER_API AffineInverse(const Matrix4D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API PerspectiveFovLH(float fFieldOfViewY, float fAspect,
        float fNear, float fFar) BURGER_NOEXCEPT;
    void BURGER_API PerspectiveFovRH(float fFieldOfViewY, float fAspect,
        float fNear, float fFar) BURGER_NOEXCEPT;
    BURGER_INLINE operator const float *() const BURGER_NOEXCEPT
    {
        return &x.x;
    }
};

extern const Matrix4D_t s_Matrix4DIdentity;

}
/* END */

#endif
