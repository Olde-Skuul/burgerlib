/***************************************

    4D Floating point matrix manager

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmatrix4d.h"
#include "brfixedmatrix4d.h"

/*! ************************************

    \struct Burger::Matrix4D_t
    \brief 4 dimensional floating point matrix

    This 64 byte matrix contains x,y,z and w 32 bit floating point rows and
    columns. A set of common functions for simple 4 dimensional math are part of
    the structure.

    It is expected to be 4 byte aligned and use scalar math. The members are
    hard coded to be "x", "y", "z" and "w" for maximum compatibility

    \note Since this is a structure, there is no constructor or destructor, so
    assume the data is uninitialized when creating this data type.

    \sa Burger::Vector4D_t, Burger::Matrix3D_t or Burger::FixedMatrix4D_t

***************************************/

/*! ************************************

    \brief Clear out a 4D matrix

    Set all of the entries to zero

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|0|0|0|0|
    |y|0|0|0|0|
    |z|0|0|0|0|
    |w|0|0|0|0|

    \sa Identity(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Zero(void) BURGER_NOEXCEPT
{
    const float fTemp = 0.0f; // Force the compiler to use a register
    x.x = fTemp;              // Fill all the elements with zero
    x.y = fTemp;
    x.z = fTemp;
    x.w = fTemp;
    y.x = fTemp;
    y.y = fTemp;
    y.z = fTemp;
    y.w = fTemp;
    z.x = fTemp;
    z.y = fTemp;
    z.z = fTemp;
    z.w = fTemp;
    w.x = fTemp;
    w.y = fTemp;
    w.z = fTemp;
    w.w = fTemp;
}

/*! ************************************

    \brief Initialize a 4D matrix so that it is inert

    Sets the x.x, y.y, z.z and w.w components to 1.0f, all others to 0.0f

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1|0|0|0|
    |y|0|1|0|0|
    |z|0|0|1|0|
    |w|0|0|0|1|

    \sa Zero(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Identity(void) BURGER_NOEXCEPT
{
    x.x = 1.0f; // Leave X alone
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;
    y.x = 0.0f;
    y.y = 1.0f; // Leave Y alone
    y.z = 0.0f;
    y.w = 0.0f;
    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 1.0f; // Leave Z alone
    z.w = 0.0f;
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f; // Leave W alone
}

/*! ************************************

    \brief Copy the matrix

    Copy a matrix into this one

    \param pInput Pointer to a valid Matrix4D_t for copying
    \sa Identity() or Zero()

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(
    const Matrix4D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x.x;
    x.y = pInput->x.y;
    x.z = pInput->x.z;
    x.w = pInput->x.w;
    y.x = pInput->y.x;
    y.y = pInput->y.y;
    y.z = pInput->y.z;
    y.w = pInput->y.w;
    z.x = pInput->z.x;
    z.y = pInput->z.y;
    z.z = pInput->z.z;
    z.w = pInput->z.w;
    w.x = pInput->w.x;
    w.y = pInput->w.y;
    w.z = pInput->w.z;
    w.w = pInput->w.w;
}

/*! ************************************

    \brief Convert a fixed point matrix into a floating point matrix

    Using fixed_to_float(float *,Fixed32), convert all of the entries from a
    4x4 16.16 fixed point matrix into a floating point matrix

    \param pInput Pointer to a valid FixedMatrix4D_t for conversion
    \sa fixed_to_float(float *,Fixed32)

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(
    const FixedMatrix4D_t* pInput) BURGER_NOEXCEPT
{
    fixed_to_float(&x.x, &pInput->x.x);
    fixed_to_float(&x.y, &pInput->x.y);
    fixed_to_float(&x.z, &pInput->x.z);
    fixed_to_float(&x.w, &pInput->x.w);
    fixed_to_float(&y.x, &pInput->y.x);
    fixed_to_float(&y.y, &pInput->y.y);
    fixed_to_float(&y.z, &pInput->y.z);
    fixed_to_float(&y.w, &pInput->y.w);
    fixed_to_float(&z.x, &pInput->z.x);
    fixed_to_float(&z.y, &pInput->z.y);
    fixed_to_float(&z.z, &pInput->z.z);
    fixed_to_float(&z.w, &pInput->z.w);
    fixed_to_float(&w.x, &pInput->w.x);
    fixed_to_float(&w.y, &pInput->w.y);
    fixed_to_float(&w.z, &pInput->w.z);
    fixed_to_float(&w.w, &pInput->w.w);
}

/*! ************************************

    \brief Copy a Matrix3D_t to a Matrix4D_t

    Copy a 3x3 matrix into this one.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->x.x|pInput->x.y|pInput->x.z|0|
    |y|pInput->y.x|pInput->y.y|pInput->y.z|0|
    |z|pInput->z.x|pInput->z.y|pInput->z.z|0|
    |w|0|0|0|1|

    \param pInput Pointer to a valid Matrix3D_t for copying
    \sa Set(const Matrix4D_t *) or Matrix3D_t::Set(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(
    const Matrix3D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x.x;
    x.y = pInput->x.y;
    x.z = pInput->x.z;
    x.w = 0.0f;
    y.x = pInput->y.x;
    y.y = pInput->y.y;
    y.z = pInput->y.z;
    y.w = 0.0f;
    z.x = pInput->z.x;
    z.y = pInput->z.y;
    z.z = pInput->z.z;
    z.w = 0.0f;
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for yaw (Y)

    \code
    float sy = get_sine(fYaw);
    float cy = get_cosine(fYaw);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy|0|-sy|0|
    |y|0|1|0|0|
    |z|sy|0|cy|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \sa TransposeSetYaw(float), SetXYZ(float,float,float), SetPitch(float) and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYaw(float fYaw) BURGER_NOEXCEPT
{
    const float fSY = get_sine(fYaw); // Get the sine/cosine
    const float fCY = get_cosine(fYaw);

    x.x = fCY;
    x.y = 0.0f;
    x.z = -fSY;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = 1.0f;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = fSY;
    z.y = 0.0f;
    z.z = fCY;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for pitch (X)

    \code
    float sx = get_sine(fPitch);
    float cx = get_cosine(fPitch);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1|0|0|0|
    |y|0|cx|-sx|0|
    |z|0|sx|cx|0|
    |w|0|0|0|1|

    \param fPitch Pitch of the X in radians
    \sa TransposeSetPitch(float), SetXYZ(float,float,float), SetYaw(float) and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetPitch(float fPitch) BURGER_NOEXCEPT
{
    const float fSX = get_sine(fPitch); // Get the sine/cosine
    const float fCX = get_cosine(fPitch);

    x.x = 1.0f;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fCX;
    y.z = -fSX;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = fSX;
    z.z = fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for roll (Z)

    \code
    float sz = get_sine(fRoll);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cz|-sz|0|0|
    |y|sz|cz|0|0|
    |z|0|0|1|0|
    |w|0|0|0|1|

    \param fRoll Roll of the Z in radians
    \sa TransposeSetRoll(float), SetXYZ(float,float,float), SetYaw(float) and
        SetPitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetRoll(float fRoll) BURGER_NOEXCEPT
{
    const float fSZ = get_sine(fRoll); // Get the sine/cosine
    const float fCZ = get_cosine(fRoll);

    x.x = fCZ;
    x.y = -fSZ;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = fSZ;
    y.y = fCZ;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 1.0f;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of Y, X and Z

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of Y, X and then Z.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(cy*cz)+(sy*-sx*-sz)|cx*-sz|(-sy*cz)+(cy*-sx*-sz)|0|
    |y|(sy*-sx*cz)+(cy*sz)|cx*cz|(cy*-sx*cz)+(-sy*sz)|0|
    |z|sy*cx|sx|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetYXZ(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYXZ(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSXSZ = fSX * fSZ;     // sx*sz
    const float fNegSXCZ = -fSX * fCZ; // -sx*cz

    // Code for YXZ
    x.x = (fCY * fCZ) + (fSY * fSXSZ);
    x.y = fCX * -fSZ;
    x.z = (fCY * fSXSZ) - (fSY * fCZ);
    x.w = 0.0f;

    y.x = (fSY * fNegSXCZ) + (fCY * fSZ);
    y.y = fCX * fCZ;
    y.z = (fCY * fNegSXCZ) - (fSY * fSZ);
    y.w = 0.0f;

    z.x = fSY * fCX;
    z.y = fSX;
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of Y, Z and X

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of Y, Z and then X.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|-sz|-sy*cz|0|
    |y|(cy*sz*cx)+(sy*-sx)|cx*cz|(-sy*sz*cx)+(cy*-sx)|0|
    |z|(sy*cx)+(cy*sx*sz)|sx*cz|(cy*cx)+(-sy*sx*sz)|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetYZX(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYZX(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSZCX = fSZ * fCX; // sz*cx
    const float fSXSZ = fSX * fSZ; // sx*sz

    // Code for YZX
    x.x = fCY * fCZ;
    x.y = -fSZ;
    x.z = -fSY * fCZ;
    x.w = 0.0f;

    y.x = (fCY * fSZCX) - (fSY * fSX);
    y.y = fCX * fCZ;
    y.z = (-fSY * fSZCX) - (fCY * fSX);
    y.w = 0.0f;

    z.x = (fSY * fCX) + (fCY * fSXSZ);
    z.y = fSX * fCZ;
    z.z = (fCY * fCX) - (fSY * fSXSZ);
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of X, Y and Z

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of X, Y and then Z.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|(-sy*sx*cz)+(cx*-sz)|(-sy*cx*cz)+(-sx*-sz)|0|
    |y|cy*sz|(cx*cz)+(-sy*sx*sz)|(-sx*cx)+(cy*sx*-sz)|0|
    |z|sy|cy*sx|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetXYZ(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXYZ(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSXSZ = fSX * fSZ;     // sx*sz
    const float fNegSYCZ = -fSY * fCZ; // sy*cz

    // Code for XYZ
    x.x = fCY * fCZ;
    x.y = (fNegSYCZ * fSX) - (fCX * fSZ);
    x.z = (fNegSYCZ * fCX) + (fSX * fSZ);
    x.w = 0.0f;

    y.x = fCY * fSZ;
    y.y = (fCX * fCZ) - (fSY * fSXSZ);
    y.z = (-fSX * fCX) - (fCY * fSXSZ);
    y.w = 0.0f;

    z.x = fSY;
    z.y = fCY * fSX;
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of X, Z and Y

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of X, Z and then Y.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|(cx*-sz*cy)+(-sy*sx)|(-sx*-sz*cy)+(-sy*cx)|0|
    |y|sz|cx*cz|-sx*cz|0|
    |z|sy*cz|(sx*cy)+(sy*cx*-sz)|(cx*cy)+(sy*-sx*-sz)|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetXZY(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXZY(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSZCY = fSZ * fCY; // sz*cy
    const float fSYSZ = fSY * fSZ; // sy*sz

    // Code for XZY
    x.x = fCY * fCZ;
    x.y = (fCX * -fSZCY) - (fSY * fSX);
    x.z = (fSX * fSZCY) - (fSY * fCX);
    x.w = 0.0f;

    y.x = fSZ;
    y.y = fCX * fCZ;
    y.z = -fSX * fCZ;
    y.w = 0.0f;

    z.x = fSY * fCZ;
    z.y = (fSX * fCY) - (fSYSZ * fCX);
    z.z = (fCX * fCY) + (fSYSZ * fSX);
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of Z, Y and X

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of Z, Y and then X.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|cy*-sz|-sy|0|
    |y|(sz*cx)+(sy*-sx*cz)|(cz*cx)+(sy*-sx*-sz)|cy*-sx|0|
    |z|(sy*cz*cx)+(sx*sz)|(sy*-sz*cx)+(sx*cz)|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetZYX(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZYX(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSYSX = fSY * fSX; // sy*sx
    const float fSYCX = fSY * fCX; // sy*cx

    // Code for ZYX
    x.x = fCY * fCZ;
    x.y = fCY * -fSZ;
    x.z = -fSY;
    x.w = 0.0f;

    y.x = (fSZ * fCX) - (fSYSX * fCZ);
    y.y = (fCZ * fCX) + (fSYSX * fSZ);
    y.z = fCY * -fSX;
    y.w = 0.0f;

    z.x = (fSYCX * fCZ) + (fSX * fSZ);
    z.y = (fSX * fCZ) - (fSYCX * fSZ);
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix in the order of Z, X and Y

    Initialize a rotation matrix with radians for Yaw (Y), Pitch (X), and Roll
    (Z) in the order of Z, X and then Y.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(cz*cy)+(-sy*sx*sz)|(-sz*cy)+(-sy*sx*cz)|-sy*cx|0|
    |y|cx*sz|cx*cz|-sx|0|
    |z|(sx*sz*cy)+(sy*cz)|(sx*cz*cy)+(sy*-sz)|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa TransposeSetZXY(float,float,float), SetYaw(float), SetPitch(float), and
        SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZXY(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSYSX = fSY * fSX; // sy*sx
    const float fSXCY = fSX * fCY; // sx*cy

    // Code for ZXY
    x.x = (fCZ * fCY) - (fSYSX * fSZ);
    x.y = (-fSZ * fCY) - (fSYSX * fCZ);
    x.z = -fSY * fCX;
    x.w = 0.0f;

    y.x = fCX * fSZ;
    y.y = fCX * fCZ;
    y.z = -fSX;
    y.w = 0.0f;

    z.x = (fSXCY * fSZ) + (fSY * fCZ);
    z.y = (fSXCY * fCZ) - (fSY * fSZ);
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Convert a quaternion to a 4x4 matrix

    Here's the resulting matrix of math used using terms from the input
    quaternion

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1-(2yy+2zz)|2xy+2wz|2xz-2wy|0|
    |y|2xy-2wz|1-(2xx+2zz)|2yz+2wx|0|
    |z|2xz+2wy|2yz-2wx|1-(2xx+2yy)|0|
    |w|0|0|0|1|

    \param pInput Pointer to a normalized quaternion
    \sa Matrix3D_t::SetFromQuaternion(const Vector4D_t *pInput) or
        TransposeSetFromQuaternion(const Vector4D_t *pInput)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetFromQuaternion(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    // Create the constants for the conversion
    // First the 2x constants

    float q_2x = pInput->x; // Perform an addition to fake a multiply by 2.0f
    float q_2y = pInput->y; // to remove a loading of the 2.0f constant
    float q_2z = pInput->z;
    q_2x += q_2x;
    q_2y += q_2y;
    q_2z += q_2z;

    // Now the matrix multiply constants

    const float q_2xw = q_2x * pInput->w;
    const float q_2yw = q_2y * pInput->w;
    const float q_2zw = q_2z * pInput->w;

    const float q_2xx = q_2x * pInput->x;
    const float q_2xy = q_2x * pInput->y;
    const float q_2yy = q_2y * pInput->y;

    const float q_2xz = q_2x * pInput->z;
    const float q_2yz = q_2y * pInput->z;
    const float q_2zz = q_2z * pInput->z;

    // x.x = 1 - 2y^2 - 2z^2

    x.x = 1.0f - q_2yy - q_2zz;

    // x.y = 2xy - 2wz

    x.y = q_2xy - q_2zw;

    // x.z = 2xz + 2wy

    x.z = q_2xz + q_2yw;
    x.w = 0.0f;

    // y.x = 2xy + 2wz

    y.x = q_2xy + q_2zw;

    // y.y = 1 - 2x^2 - 2z^2

    y.y = 1.0f - q_2xx - q_2zz;

    // y.z = 2yz - 2wx

    y.z = q_2yz - q_2xw;
    y.w = 0.0f;

    // z.x = 2xz - 2wy

    z.x = q_2xz - q_2yw;

    // z.y = 2yz + 2wx

    z.y = q_2yz + q_2xw;

    // z.z = 1 - 2x^2 - 2y^2

    z.z = 1.0f - q_2xx - q_2yy;
    z.w = 0.0f;

    // Scale is 1.0f
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for yaw (Y)

    \code
    float sy = get_sine(fYaw);
    float cy = get_cosine(fYaw);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy|0|sy|0|
    |y|0|1|0|0|
    |z|-sy|0|cy|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \sa SetYaw(float), TransposeSetXYZ(float,float,float),
TransposeSetPitch(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYaw(float fYaw) BURGER_NOEXCEPT
{
    const float fSY = get_sine(fYaw); // Get the sine/cosine
    const float fCY = get_cosine(fYaw);

    x.x = fCY;
    x.y = 0.0f;
    x.z = fSY;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = 1.0f;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = -fSY;
    z.y = 0.0f;
    z.z = fCY;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for pitch (X)

    \code
    float sx = get_sine(fPitch);
    float cx = get_cosine(fPitch);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1|0|0|0|
    |y|0|cx|sx|0|
    |z|0|-sx|cx|0|
    |w|0|0|0|1|

    \param fPitch Pitch of the X in radians
    \sa SetPitch(float), TransposeSetXYZ(float,float,float),
TransposeSetYaw(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetPitch(
    float fPitch) BURGER_NOEXCEPT
{
    const float fSX = get_sine(fPitch); // Get the sine/cosine
    const float fCX = get_cosine(fPitch);

    x.x = 1.0f;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fCX;
    y.z = fSX;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = -fSX;
    z.z = fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a rotation matrix with radians for roll (Z)

    \code
    float sz = get_sine(fRoll);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cz|sz|0|0|
    |y|-sz|cz|0|0|
    |z|0|0|1|0|
    |w|0|0|0|1|

    \param fRoll Roll of the Z in radians
    \sa SetRoll(float), TransposeSetXYZ(float,float,float),
TransposeSetYaw(float) and TransposeSetPitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetRoll(
    float fRoll) BURGER_NOEXCEPT
{
    const float fSZ = get_sine(fRoll); // Get the sine/cosine
    const float fCZ = get_cosine(fRoll);

    x.x = fCZ;
    x.y = fSZ;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = fSZ;
    y.y = -fCZ;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 1.0f;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of Y, X and Z.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of Y, X and then Z.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(cy*cz)+(-sy*sx*sz)|cx*sz|(sy*cz)+(cy*sx*sz)|0|
    |y|(-sy*sx*cz)+(cy*-sz)|cx*cz|(cy*sx*cz)+(sy*-sz)|0|
    |z|-sy*cx|-sx|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetYXZ(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYXZ(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSXSZ = fSX * fSZ; // sx*sz
    const float fSXCZ = fSX * fCZ; // sx*cz

    // Code for YXZ
    x.x = (fCY * fCZ) - (fSY * fSXSZ);
    x.y = fCX * fSZ;
    x.z = (fSY * fCZ) + (fCY * fSXSZ);
    x.w = 0.0f;

    y.x = (-fSY * fSXCZ) - (fCY * fSZ);
    y.y = fCX * fCZ;
    y.z = (fCY * fSXCZ) - (fSY * fSZ);
    y.w = 0.0f;

    z.x = -fSY * fCX;
    z.y = -fSX;
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of Y, Z and X.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of Y, Z and then X.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|sz|sy*cz|0|
    |y|(cy*-sz*cx)+(-sy*sx)|cx*cz|(sy*-sz*cx)+(cy*sx)|0|
    |z|(-sy*cx)+(cy*-sx*-sz)|-sx*cz|(cy*cx)+(sy*-sx*-sz)|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetYZX(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYZX(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSXSZ = fSX * fSZ;     // sx*sz
    const float fNegSZCX = -fSZ * fCX; // -sz*cx

    // Code for YZX
    x.x = fCY * fCZ;
    x.y = fSZ;
    x.z = fSY * fCZ;
    x.w = 0.0f;

    y.x = (fCY * fNegSZCX) - (fSY * fSX);
    y.y = fCX * fCZ;
    y.z = (fSY * fNegSZCX) + (fCY * fSX);
    y.w = 0.0f;

    z.x = (fCY * fSXSZ) - (fSY * fCX);
    z.y = -fSX * fCZ;
    z.z = (fCY * fCX) + (fSY * fSXSZ);
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of X, Y and Z.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of X, Y and then Z.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|(sy*-sx*cz)+(cx*sz)|(sy*cx*cz)+(sx*sz)|0|
    |y|cy*-sz|(cx*cz)+(sy*-sx*-sz)|(sx*cz)+(sy*cx*-sz)|0|
    |z|-sy|cy*-sx|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetXYZ(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetXYZ(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSYSX = fSY * fSX; // sy*sx
    const float fSYCX = fSY * fCX; // sy*cx

    // Code for XYZ
    x.x = fCY * fCZ;
    x.y = (fCX * fSZ) - (fSYSX * fCZ);
    x.z = (fSYCX * fCZ) + (fSX * fSZ);
    x.w = 0.0f;

    y.x = fCY * -fSZ;
    y.y = (fCX * fCZ) + (fSYSX * fSZ);
    y.z = (fSX * fCZ) - (fSYCX * fSZ);
    y.w = 0.0f;

    z.x = -fSY;
    z.y = fCY * -fSX;
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of X, Z and Y.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of X, Z and then Y.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|(cx*sz*cy)+(sy*-sx)|(sx*sz*cy)+(sy*cx)|0|
    |y|-sz|cx*cz|sx*cz|0|
    |z|-sy*cz|(-sx*cy)+(-sy*cx*sz)|(cx*cy)+(-sy*sx*sz)|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetXZY(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetXZY(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSZCY = fSZ * fCY; // sz*cy
    const float fSYSZ = fSY * fSZ; // sy*sz

    // Code for XZY
    x.x = fCY * fCZ;
    x.y = (fCX * fSZCY) - (fSY * fSX);
    x.z = (fSX * fSZCY) + (fSY * fCX);
    x.w = 0.0f;

    y.x = -fSZ;
    y.y = fCX * fCZ;
    y.z = fSX * fCZ;
    y.w = 0.0f;

    z.x = -fSY * fCZ;
    z.y = (-fSX * fCY) - (fSYSZ * fCX);
    z.z = (fCX * fCY) - (fSYSZ * fSX);
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of Z, Y and X.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of Z, Y and then X.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|cy*cz|cy*sz|sy|0|
    |y|(-sz*cx)+(-sy*sx*cz)|(cz*cx)+(-sy*sx*sz)|cy*sx|0|
    |z|(-sy*cz*cx)+(-sx*-sz)|(-sy*sz*cx)+(-sx*cz)|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetZYX(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetZYX(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSYSX = fSY * fSX;     // sy*sx
    const float fNegSYCX = -fSY * fCX; // -sy*cx

    // Code for ZYX
    x.x = fCY * fCZ;
    x.y = fCY * fSZ;
    x.z = fSY;
    x.w = 0.0f;

    y.x = (-fSZ * fCX) - (fSYSX * fCZ);
    y.y = (fCZ * fCX) - (fSYSX * fSZ);
    y.z = fCY * fSX;
    y.w = 0.0f;

    z.x = (fSX * fSZ) + (fNegSYCX * fCZ);
    z.y = (fNegSYCX * fSZ) - (fSX * fCZ);
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Initialize a transposed rotation matrix in the order of Z, X and Y.

    Initialize a transposed rotation matrix with radians for Yaw (Y), Pitch (X),
    and Roll (Z) in the order of Z, X and then Y.

    \code
    float sx = get_sine(fPitch);
    float sy = get_sine(fYaw);
    float sz = get_sine(fRoll);
    float cx = get_cosine(fPitch);
    float cy = get_cosine(fYaw);
    float cz = get_cosine(fRoll);
    \endcode

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(cz*cy)+(sy*-sx*-sz)|(sz*cy)+(sy*-sx*cz)|sy*cx|0|
    |y|cx*-sz|cx*cz|sx|0|
    |z|(-sx*-sz*cy)+(-sy*cz)|(-sx*cz*cy)+(-sy*sz)|cy*cx|0|
    |w|0|0|0|1|

    \param fYaw Yaw of the Y in radians
    \param fPitch Pitch of the X in radians
    \param fRoll Roll of the Z in radians
    \sa SetZXY(float,float,float), TransposeSetYaw(float),
        TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetZXY(
    float fYaw, float fPitch, float fRoll) BURGER_NOEXCEPT
{
    // Create the sines and cosines
    const float fSX = get_sine(fPitch);
    const float fSY = get_sine(fYaw);
    const float fSZ = get_sine(fRoll);
    const float fCX = get_cosine(fPitch);
    const float fCY = get_cosine(fYaw);
    const float fCZ = get_cosine(fRoll);

    // Cache this value
    const float fSXSZ = fSX * fSZ; // sx*sz
    const float fSXCZ = fSX * fCZ; // sx*cz

    // Code for ZXY
    x.x = (fCZ * fCY) + (fSXSZ * fSY);
    x.y = (fSZ * fCY) - (fSXCZ * fSY);
    x.z = fSY * fCX;
    x.w = 0.0f;

    y.x = fCX * -fSZ;
    y.y = fCX * fCZ;
    y.z = fSX;
    y.w = 0.0f;

    z.x = (fSXSZ * fCY) - (fSY * fCZ);
    z.y = (-fSZ * fSY) - (fSXCZ * fCY);
    z.z = fCY * fCX;
    z.w = 0.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Convert a quaternion to a transposed 4x4 matrix

    Here's the resulting matrix of math used using terms from the input
    quaternion

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1-(2yy+2zz)|2xy-2wz|2xz+2wy|0|
    |y|2xy+2wz|1-(2xx+2zz)|2yz-2wx|0|
    |z|2xz-2wy|2yz+2wx|1-(2xx+2yy)|0|
    |w|0|0|0|1|

    \param pInput Pointer to a normalized quaternion
    \sa Matrix3D_t::TransposeSetFromQuaternion(const Vector4D_t *pInput) or
        SetFromQuaternion(const Vector4D_t *pInput)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetFromQuaternion(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    // Create the constants for the conversion
    // First the 2x constants

    float q_2x = pInput->x; // Perform an addition to fake a multiply by 2.0f
    float q_2y = pInput->y; // to remove a loading of the 2.0f constant
    float q_2z = pInput->z;
    q_2x += q_2x;
    q_2y += q_2y;
    q_2z += q_2z;

    // Now the matrix multiply constants

    const float q_2xw = q_2x * pInput->w;
    const float q_2yw = q_2y * pInput->w;
    const float q_2zw = q_2z * pInput->w;

    const float q_2xx = q_2x * pInput->x;
    const float q_2xy = q_2x * pInput->y;
    const float q_2yy = q_2y * pInput->y;

    const float q_2xz = q_2x * pInput->z;
    const float q_2yz = q_2y * pInput->z;
    const float q_2zz = q_2z * pInput->z;

    // x.x = 1 - 2y^2 - 2z^2

    x.x = 1.0f - q_2yy - q_2zz;

    // x.y = 2xy - 2wz

    y.x = q_2xy - q_2zw;

    // x.z = 2xz + 2wy

    z.x = q_2xz + q_2yw;
    w.x = 0.0f;

    // y.x = 2xy + 2wz

    x.y = q_2xy + q_2zw;

    // y.y = 1 - 2x^2 - 2z^2

    y.y = 1.0f - q_2xx - q_2zz;

    // y.z = 2yz - 2wx

    z.y = q_2yz - q_2xw;
    w.y = 0.0f;

    // z.x = 2xz - 2wy

    x.z = q_2xz - q_2yw;

    // z.y = 2yz + 2wx

    y.z = q_2yz + q_2xw;

    // z.z = 1 - 2x^2 - 2y^2

    z.z = 1.0f - q_2xx - q_2yy;
    w.z = 0.0f;

    // Scale is 1.0f
    x.w = 0.0f;
    y.w = 0.0f;
    z.w = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Create a 4D translation matrix

    Sets the w row to fX, fY and fZ and the rest of the values to that of an
    identity matrix.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|1|0|0|0|
    |y|0|1|0|0|
    |z|0|0|1|0|
    |w|fX|fY|fZ|1|

    \param fX new w.x component
    \param fY new w.y component
    \param fZ new w.z component
    \sa Identity(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetTranslate(
    float fX, float fY, float fZ) BURGER_NOEXCEPT
{
    w.x = fX;
    w.y = fY;
    w.z = fZ;
    w.w = 1.0f;
    x.x = 1.0f;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;
    y.x = 0.0f;
    y.y = 1.0f;
    y.z = 0.0f;
    y.w = 0.0f;
    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 1.0f;
    z.w = 0.0f;
}

/*! ************************************

    \brief Create a 4D scale matrix

    Sets the x.x, y.y and z.z components to the input values, w.w is set to 1.0f
    and all others are set to 0.0f

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|fX|0|0|0|
    |y|0|fY|0|0|
    |z|0|0|fZ|0|
    |w|0|0|0|1|

    \param fX new x.x component
    \param fY new y.y component
    \param fZ new z.z component
    \sa Identity(void) and SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetScale(
    float fX, float fY, float fZ) BURGER_NOEXCEPT
{
    x.x = fX;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;
    y.x = 0.0f;
    y.y = fY;
    y.z = 0.0f;
    y.w = 0.0f;
    z.x = 0.0f;
    z.y = 0.0f;
    z.z = fZ;
    z.w = 0.0f;
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Create a 4D scale matrix

    Sets the x.x, y.y, z.z and w.w components to the input values and all others
    are set to 0.0f

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|fX|0|0|0|
    |y|0|fY|0|0|
    |z|0|0|fZ|0|
    |w|0|0|0|fW|

    \param fX new x.x component
    \param fY new y.y component
    \param fZ new z.z component
    \param fW new w.w component
    \sa Identity(void) and SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetScale(
    float fX, float fY, float fZ, float fW) BURGER_NOEXCEPT
{
    x.x = fX;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;
    y.x = 0.0f;
    y.y = fY;
    y.z = 0.0f;
    y.w = 0.0f;
    z.x = 0.0f;
    z.y = 0.0f;
    z.z = fZ;
    z.w = 0.0f;
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = fW;
}

// clang-format off
/*! ************************************

    \brief Create a transposed 4D projection matrix

    Using a frustum, create a projection matrix in a format that OpenGL can use
    directly. If a DirectX version is desired, call TransposeSetFrustum()
    instead.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(fNear * 2.0f) / (fRight - fLeft)|0|0|0|
    |y|0|(fNear * 2.0f) / (fTop - fBottom)|0|0|
    |z|(fRight + fLeft) / (fRight - fLeft)|(fTop + fBottom) / (fTop - fBottom)|-(fFar+fNear) / (fFar - fNear)|-1|
    |w|0|0|-(2.0f * fFar * fNear) / (fFar -fNear)|0|

    \note This is functional duplicate of glFrustum()

    \param fLeft Leftmost X coordinate
    \param fRight Rightmost X coordinate
    \param fBottom Bottommost Y coordinate
    \param fTop Topmost Y coordinate
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa SetOrtho(), SetPerspective() or TransposeSetFrustum()

***************************************/
// clang-format on

void BURGER_API Burger::Matrix4D_t::SetFrustum(float fLeft, float fRight,
    float fBottom, float fTop, float fNear, float fFar) BURGER_NOEXCEPT
{
    const float fWidth = fRight - fLeft;
    const float fHeight = fTop - fBottom;
    const float fNegativeDepth = -(fFar - fNear);
    const float fNear2 = fNear * 2.0f;

    x.x = fNear2 / fWidth;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fNear2 / fHeight;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = (fRight + fLeft) / fWidth;
    z.y = (fTop + fBottom) / fHeight;
    z.z = (fFar + fNear) / fNegativeDepth;
    z.w = -1.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = (2.0f * fFar * fNear) / fNegativeDepth;
    w.w = 0.0f;
}

// clang-format off
/*! ************************************

    \brief Create a 4D projection matrix

    Using a frustum, create a projection matrix in a format that DirectX can use
    directly. If a OpenGL version is desired, call SetFrustum() instead.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(fNear * 2.0f) / (fRight - fLeft)|0|(fRight + fLeft) / (fRight - fLeft)|0|
    |y|0|(fNear * 2.0f) / (fTop - fBottom)|(fTop + fBottom) / (fTop - fBottom)|0|
    |z|0|0|-(fFar+fNear) / (fFar - fNear)|-(2.0f * fFar * fNear) / (fFar - fNear)|
    |w|0|0|-1|0|

    \param fLeft Leftmost X coordinate
    \param fRight Rightmost X coordinate
    \param fBottom Bottommost Y coordinate
    \param fTop Topmost Y coordinate
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa TransposeSetOrtho(), TransposeSetPerspective() or SetFrustum()

***************************************/
// clang-format on

void BURGER_API Burger::Matrix4D_t::TransposeSetFrustum(float fLeft,
    float fRight, float fBottom, float fTop, float fNear,
    float fFar) BURGER_NOEXCEPT
{
    const float fWidth = fRight - fLeft;
    const float fHeight = fTop - fBottom;
    const float fNegativeDepth = -(fFar - fNear);
    const float fNear2 = fNear * 2.0f;

    x.x = fNear2 / fWidth;
    x.y = 0.0f;
    x.z = (fRight + fLeft) / fWidth;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fNear2 / fHeight;
    y.z = (fTop + fBottom) / fHeight;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = (fFar + fNear) / fNegativeDepth;
    z.w = (2.0f * fFar * fNear) / fNegativeDepth;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = -1.0f;
    w.w = 0.0f;
}

// clang-format off
/*! ************************************

    \brief Create a transposed orthogonal 4D projection matrix

    Using a frustum, create an orthogonal matrix in a format that OpenGL can use
    directly. If a DirectX version is desired, call TransposeSetOrtho() instead.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|2.0f / (fRight - fLeft)|0|0|0|
    |y|0|2.0f / (fTop - fBottom)|0|0|
    |z|0|0|-2.0f / (fFar - fNear)|0|
    |w|-(fRight + fLeft) / (fRight - fLeft)|-(fTop + fBottom) / (fTop - fBottom)|-(Far + fNear) / (fFar - fNear)|1|

    \param fLeft Leftmost X coordinate
    \param fRight Rightmost X coordinate
    \param fBottom Bottommost Y coordinate
    \param fTop Topmost Y coordinate
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa SetFrustum() or TransposeSetOrtho()

***************************************/
// clang-format on

void BURGER_API Burger::Matrix4D_t::SetOrtho(float fLeft, float fRight,
    float fBottom, float fTop, float fNear, float fFar) BURGER_NOEXCEPT
{
    const float fWidth = fRight - fLeft;
    const float fHeight = fTop - fBottom;
    const float fNegativeDepth = -(fFar - fNear);

    x.x = 2.0f / fWidth;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = 2.0f / fHeight;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 2.0f / fNegativeDepth;
    z.w = 0.0f;

    w.x = -(fRight + fLeft) / fWidth;
    w.y = -(fTop + fBottom) / fHeight;
    w.z = (fFar + fNear) / fNegativeDepth;
    w.w = 1.0f;
}

/*! ************************************

    \brief Create an orthogonal 4D projection matrix

    Using a frustum, create an orthogonal matrix in a format that DirectX can
    use directly. If an OpenGL version is desired, call SetOrtho() instead.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|2.0f / (fRight - fLeft)|0|0|-(fRight + fLeft) / (fRight - fLeft)|
    |y|0|2.0f / (fTop - fBottom)|0|-(fTop + fBottom) / (fTop - fBottom)|
    |z|0|0|-2.0f / (fFar - fNear)|-(Far + fNear) / (fFar - fNear)|
    |w|0|0|0|1|

    \note This is functional duplicate of glOrtho()

    \param fLeft Leftmost X coordinate
    \param fRight Rightmost X coordinate
    \param fBottom Bottommost Y coordinate
    \param fTop Topmost Y coordinate
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa TransposeSetFrustum() or SetOrtho()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetOrtho(float fLeft, float fRight,
    float fBottom, float fTop, float fNear, float fFar) BURGER_NOEXCEPT
{
    const float fWidth = fRight - fLeft;
    const float fHeight = fTop - fBottom;
    const float fNegativeDepth = -(fFar - fNear);

    x.x = 2.0f / fWidth;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = -(fRight + fLeft) / fWidth;

    y.x = 0.0f;
    y.y = 2.0f / fHeight;
    y.z = 0.0f;
    y.w = -(fTop + fBottom) / fHeight;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 2.0f / fNegativeDepth;
    z.w = (fFar + fNear) / fNegativeDepth;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = 0.0f;
    w.w = 1.0f;
}

/*! ************************************

    \brief Create a 4D perspective matrix

    Using a field of view and an aspect ratio (x/y), create a projection matrix
    in a format that OpenGL can use directly. If a DirectX version is desired,
    call TransposeSetPerspective() instead.

    f = 1.0f / tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|f/fAspect|0|0|0|
    |y|0|f|0|0|
    |z|0|0|(fFar+fNear) / (fNear-fFar)|-1|
    |w|0|0|(2.0f * fFar * fNear) / (fNear-fFar)|0|

    \note This is functional duplicate of gluPerspective()

    \param fFieldOfViewY Angle of the field of view in degrees
    \param fAspect Aspect ratio of the screen
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa SetOrtho(), SetFrustum() or TransposeSetPerspective()

***************************************/

void BURGER_API Burger::Matrix4D_t::SetPerspective(
    float fFieldOfViewY, float fAspect, float fNear, float fFar) BURGER_NOEXCEPT
{
    // Calculate the cotangent of Field of View * 2.0f
    const float f = 1.0f / get_tangent(fFieldOfViewY * static_cast<float>(g_fDegreesToRadians) * 0.5f);

    const float fDepth = (fNear - fFar);

    x.x = f / fAspect;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = f;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = (fFar + fNear) / fDepth;
    z.w = -1.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = (2.0f * fFar * fNear) / fDepth;
    w.w = 0.0f;
}

/*! ************************************

    \brief Create a 4D perspective matrix

    Using a field of view and an aspect ratio (x/y), create a projection matrix
    in a format that DirectX can use directly. If an OpenGL version is desired,
    call SetPerspective() instead.

    f = 1.0f / tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|f/fAspect|0|0|0|
    |y|0|f|0|0|
    |z|0|0|(fFar+fNear) / (fNear-fFar)|(2.0f * fFar * fNear) / (fNear-fFar)|
    |w|0|0|-1|0|

    \param fFieldOfViewY Angle of the field of view in degrees
    \param fAspect Aspect ratio of the screen
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa TransposeSetOrtho(), TransposeSetFrustum() or SetPerspective()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetPerspective(
    float fFieldOfViewY, float fAspect, float fNear, float fFar) BURGER_NOEXCEPT
{
    // Calculate the cotangent of Field of View * 2.0f
    const float f = 1.0f / get_tangent(fFieldOfViewY * static_cast<float>(g_fDegreesToRadians) * 0.5f);

    const float fDepth = (fNear - fFar);

    x.x = f / fAspect;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = f;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = (fFar + fNear) / fDepth;
    z.w = (2.0f * fFar * fNear) / fDepth;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = -1.0f;
    w.w = 0.0f;
}

/*! ************************************

    \brief Perform a matrix transposition.

    Swap the entries to convert a right handed matrix to a left handed one and
    vice versa. The matrix is assumed to be already initialized.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|yx|zx|wx|
    |y|xy|yy|zy|wy|
    |z|xz|yz|zz|wz|
    |w|xw|yw|zw|ww|

    \sa Transpose(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Transpose(void) BURGER_NOEXCEPT
{
    float fTemp1 = x.y; // Swap x.y and y.x
    float fTemp2 = y.x;
    x.y = fTemp2;
    y.x = fTemp1;

    fTemp1 = x.z; // Swap x.z and z.x
    fTemp2 = z.x;
    x.z = fTemp2;
    z.x = fTemp1;

    fTemp1 = x.w; // Swap x.w and w.x
    fTemp2 = w.x;
    x.w = fTemp2;
    w.x = fTemp1;

    fTemp1 = y.z; // Swap y.z and z.y
    fTemp2 = z.y;
    y.z = fTemp2;
    z.y = fTemp1;

    fTemp1 = y.w; // Swap y.w and w.y
    fTemp2 = w.y;
    y.w = fTemp2;
    w.y = fTemp1;

    fTemp1 = z.w; // Swap z.w and w.z
    fTemp2 = w.z;
    z.w = fTemp2;
    w.z = fTemp1;
}

/*! ************************************

    \brief Perform a matrix transposition.

    Swap the entries to convert a right handed matrix to a left handed one and
    vice versa. The other three entries are copied. The destination matrix is
    assumed to be uninitialized.

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|yx|zx|wx|
    |y|xy|yy|zy|wy|
    |z|xz|yz|zz|wz|
    |w|xw|yw|zw|ww|

    \note This function is optimized to write the new matrix in sequential order
    in the event this function is writing to write combined memory.

    \sa Transpose(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Transpose(
    const Matrix4D_t* pInput) BURGER_NOEXCEPT
{
    float fTemp1 = pInput->x.x; // I am copying the matrix, this
    float fTemp2 = pInput->y.x; // is why I am copying the x.x, y.y
    x.x = fTemp1;               // and z.z entries
    x.y = fTemp2;

    fTemp1 = pInput->z.x;
    fTemp2 = pInput->w.x;
    x.z = fTemp1;
    x.w = fTemp2;

    fTemp1 = pInput->x.y;
    fTemp2 = pInput->y.y;
    y.x = fTemp1;
    y.y = fTemp2;

    fTemp1 = pInput->z.y;
    fTemp2 = pInput->w.y;
    y.z = fTemp1;
    y.w = fTemp2;

    fTemp1 = pInput->x.z;
    fTemp2 = pInput->y.z;
    z.x = fTemp1;
    z.y = fTemp2;

    fTemp1 = pInput->z.z;
    fTemp2 = pInput->w.z;
    z.z = fTemp1;
    z.w = fTemp2;

    fTemp1 = pInput->x.w;
    fTemp2 = pInput->y.w;
    w.x = fTemp1;
    w.y = fTemp2;

    fTemp1 = pInput->z.w;
    fTemp2 = pInput->w.w;
    w.z = fTemp1;
    w.w = fTemp2;
}

/*! ************************************

    \brief Return the X row from a matrix

    Copy the x row of a matrix into a user supplied \ref Vector3D_t
    buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |xx|xy|xz|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXRow(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.x;
    pOutput->y = x.y;
    pOutput->z = x.z;
}

/*! ************************************

    \brief Return the X row from a matrix

    Copy the x row of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |xx|xy|xz|xw|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXRow(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.x;
    pOutput->y = x.y;
    pOutput->z = x.z;
    pOutput->w = x.w;
}

/*! ************************************

    \brief Return the Y row from a matrix

    Copy the y row of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |yx|yy|yz|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYRow(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = y.x;
    pOutput->y = y.y;
    pOutput->z = y.z;
}

/*! ************************************

    \brief Return the Y row from a matrix

    Copy the y row of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |yx|yy|yz|yw|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYRow(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = y.x;
    pOutput->y = y.y;
    pOutput->z = y.z;
    pOutput->w = y.w;
}

/*! ************************************

    \brief Return the Z row from a matrix

    Copy the z row of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |zx|zy|zz|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZRow(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = z.x;
    pOutput->y = z.y;
    pOutput->z = z.z;
}

/*! ************************************

    \brief Return the Z row from a matrix

    Copy the z row of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |zx|zy|zz|zw|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZRow(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = z.x;
    pOutput->y = z.y;
    pOutput->z = z.z;
    pOutput->w = z.w;
}

/*! ************************************

    \brief Return the W row from a matrix

    Copy the w row of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |wx|wy|wz|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWRow(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = w.x;
    pOutput->y = w.y;
    pOutput->z = w.z;
}

/*! ************************************

    \brief Return the W row from a matrix

    Copy the w row of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |wx|wy|wz|ww|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWRow(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = w.x;
    pOutput->y = w.y;
    pOutput->z = w.z;
    pOutput->w = w.w;
}

/*! ************************************

    \brief Return the X column from a matrix

    Copy the x column of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |xx|yx|zx|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXColumn(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.x;
    pOutput->y = y.x;
    pOutput->z = z.x;
}

/*! ************************************

    \brief Return the X column from a matrix

    Copy the x column of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |xx|yx|zx|wx|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXColumn(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.x;
    pOutput->y = y.x;
    pOutput->z = z.x;
    pOutput->w = w.x;
}

/*! ************************************

    \brief Return the Y column from a matrix

    Copy the y column of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |xy|yy|zy|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYColumn(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.y;
    pOutput->y = y.y;
    pOutput->z = z.y;
}

/*! ************************************

    \brief Return the Y column from a matrix

    Copy the y column of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |xy|yy|zy|wy|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYColumn(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.y;
    pOutput->y = y.y;
    pOutput->z = z.y;
    pOutput->w = w.y;
}

/*! ************************************

    \brief Return the Z column from a matrix

    Copy the z column of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |xz|yz|zz|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZColumn(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.z;
    pOutput->y = y.z;
    pOutput->z = z.z;
}

/*! ************************************

    \brief Return the Z column from a matrix

    Copy the z column of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |xz|yz|zz|wz|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZColumn(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.z;
    pOutput->y = y.z;
    pOutput->z = z.z;
    pOutput->w = w.z;
}

/*! ************************************

    \brief Return the W column from a matrix

    Copy the w column of a matrix into a user supplied \ref Vector3D_t buffer.

    |x|y|z|
    |:--:|:--:|:--:|
    |xw|yw|zw|

    \param pOutput Pointer to an uninitialized Vector3D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWColumn(
    Vector3D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.w;
    pOutput->y = y.w;
    pOutput->z = z.w;
}

/*! ************************************

    \brief Return the W column from a matrix

    Copy the w column of a matrix into a user supplied \ref Vector4D_t buffer.

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |xw|yw|zw|ww|

    \param pOutput Pointer to an uninitialized Vector4D_t structure to receive
        the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWColumn(
    Vector4D_t* pOutput) const BURGER_NOEXCEPT
{
    pOutput->x = x.w;
    pOutput->y = y.w;
    pOutput->z = z.w;
    pOutput->w = w.w;
}

/*! ************************************

    \brief Set the X row of a matrix

    Overwrite the X row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->x|pInput->y|pInput->z|xw|
    |y|yx|yy|yz|yw|
    |z|zx|zy|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new X row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXRow(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x;
    x.y = pInput->y;
    x.z = pInput->z;
}

/*! ************************************

    \brief Set the X row of a matrix

    Overwrite the X row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->x|pInput->y|pInput->z|pInput->w|
    |y|yx|yy|yz|yw|
    |z|zx|zy|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new X row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXRow(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x;
    x.y = pInput->y;
    x.z = pInput->z;
    x.w = pInput->w;
}

/*! ************************************

    \brief Set the Y row of a matrix

    Overwrite the Y row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|pInput->x|pInput->y|pInput->z|yw|
    |z|zx|zy|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new Y row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYRow(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    y.x = pInput->x;
    y.y = pInput->y;
    y.z = pInput->z;
}

/*! ************************************

    \brief Set the Y row of a matrix

    Overwrite the Y row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|pInput->x|pInput->y|pInput->z|pInput->w|
    |z|zx|zy|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new Y row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYRow(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    y.x = pInput->x;
    y.y = pInput->y;
    y.z = pInput->z;
    y.w = pInput->w;
}

/*! ************************************

    \brief Set the Z row of a matrix

    Overwrite the Z row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|yx|yy|yz|yw|
    |z|pInput->x|pInput->y|pInput->z|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new Z row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZRow(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    z.x = pInput->x;
    z.y = pInput->y;
    z.z = pInput->z;
}

/*! ************************************

    \brief Set the Z row of a matrix

    Overwrite the Z row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|yx|yy|yz|yw|
    |z|pInput->x|pInput->y|pInput->z|pInput->w|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new Z row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZRow(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    z.x = pInput->x;
    z.y = pInput->y;
    z.z = pInput->z;
    z.w = pInput->w;
}

/*! ************************************

    \brief Set the W row of a matrix

    Overwrite the W row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|yx|yy|yz|yw|
    |z|zx|zy|zz|zw|
    |w|pInput->x|pInput->y|pInput->z|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new W row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWRow(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    w.x = pInput->x;
    w.y = pInput->y;
    w.z = pInput->z;
}

/*! ************************************

    \brief Set the W row of a matrix

    Overwrite the W row of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|xw|
    |y|yx|yy|yz|yw|
    |z|zx|zy|zz|zw|
    |w|pInput->x|pInput->y|pInput->z|pInput->w|

    \param pInput Pointer to a Vector4D_t structure that has the new W row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWRow(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    w.x = pInput->x;
    w.y = pInput->y;
    w.z = pInput->z;
    w.w = pInput->w;
}

/*! ************************************

    \brief Set the X column of a matrix

    Overwrite the X column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->x|xy|xz|xw|
    |y|pInput->y|yy|yz|yw|
    |z|pInput->z|zy|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new X column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXColumn(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x;
    y.x = pInput->y;
    z.x = pInput->z;
}

/*! ************************************

    \brief Set the X column of a matrix

    Overwrite the X column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->x|xy|xz|xw|
    |y|pInput->y|yy|yz|yw|
    |z|pInput->z|zy|zz|zw|
    |w|pInput->w|wy|wz|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new X column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXColumn(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    x.x = pInput->x;
    y.x = pInput->y;
    z.x = pInput->z;
    w.x = pInput->w;
}

/*! ************************************

    \brief Set the Y column of a matrix

    Overwrite the Y column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|pInput->x|xz|xw|
    |y|yx|pInput->y|yz|yw|
    |z|zx|pInput->z|zz|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new Y column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYColumn(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    x.y = pInput->x;
    y.y = pInput->y;
    z.y = pInput->z;
}

/*! ************************************

    \brief Set the Y column of a matrix

    Overwrite the Y column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|pInput->x|xz|xw|
    |y|yx|pInput->y|yz|yw|
    |z|zx|pInput->z|zz|zw|
    |w|wx|pInput->w|wz|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new Y column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYColumn(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    x.y = pInput->x;
    y.y = pInput->y;
    z.y = pInput->z;
    w.y = pInput->w;
}

/*! ************************************

    \brief Set the Z column of a matrix

    Overwrite the Z column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|pInput->x|xw|
    |y|yx|yy|pInput->y|yw|
    |z|zx|zy|pInput->z|zw|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new Z column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZColumn(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    x.z = pInput->x;
    y.z = pInput->y;
    z.z = pInput->z;
}

/*! ************************************

    \brief Set the Z column of a matrix

    Overwrite the Z column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|pInput->x|xw|
    |y|yx|yy|pInput->y|yw|
    |z|zx|zy|pInput->z|zw|
    |w|wx|wy|pInput->w|ww|

    \param pInput Pointer to a Vector4D_t structure that has the new Z column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZColumn(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    x.z = pInput->x;
    y.z = pInput->y;
    z.z = pInput->z;
    w.z = pInput->w;
}

/*! ************************************

    \brief Set the W column of a matrix

    Overwrite the W column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|pInput->x|
    |y|yx|yy|yz|pInput->y|
    |z|zx|zy|zz|pInput->z|
    |w|wx|wy|wz|ww|

    \param pInput Pointer to a Vector3D_t structure that has the new W column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWColumn(
    const Vector3D_t* pInput) BURGER_NOEXCEPT
{
    x.w = pInput->x;
    y.w = pInput->y;
    z.w = pInput->z;
}

/*! ************************************

    \brief Set the W column of a matrix

    Overwrite the W column of the matrix with the input vector

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx|xy|xz|pInput->x|
    |y|yx|yy|yz|pInput->y|
    |z|zx|zy|zz|pInput->z|
    |w|wx|wy|wz|pInput->w|

    \param pInput Pointer to a Vector4D_t structure that has the new W column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWColumn(
    const Vector4D_t* pInput) BURGER_NOEXCEPT
{
    x.w = pInput->x;
    y.w = pInput->y;
    z.w = pInput->z;
    w.w = pInput->w;
}

// clang-format off
/*! ************************************

    \brief Perform a matrix multiply against this matrix

    Multiply this matrix against another one

    Use this formula to create the final matrix, this matrix is matrix #1 and
    the input matrix is matrix #2

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(x.x*pInput->x.x)+(y.x*pInput->x.y)+(z.x*pInput->x.z)+(w.x*pInput->x.w)|(x.y*pInput->x.x)+(y.y*pInput->x.y)+(z.y*pInput->x.z)+(w.y*pInput->x.w)|(x.z*pInput->x.x)+(y.z*pInput->x.y)+(z.z*pInput->x.z)+(w.z*pInput->x.w)|(x.w*pInput->x.x)+(y.w*pInput->x.y)+(z.w*pInput->x.z)+(w.w*pInput->x.w)|
    |y|(x.x*pInput->y.x)+(y.x*pInput->y.y)+(z.x*pInput->y.z)+(w.x*pInput->y.w)|(x.y*pInput->y.x)+(y.y*pInput->y.y)+(z.y*pInput->y.z)+(w.y*pInput->y.w)|(x.z*pInput->y.x)+(y.z*pInput->y.y)+(z.z*pInput->y.z)+(w.z*pInput->y.w)|(x.w*pInput->y.x)+(y.w*pInput->y.y)+(z.w*pInput->y.z)+(w.w*pInput->y.w)|
    |z|(x.x*pInput->z.x)+(y.x*pInput->z.y)+(z.x*pInput->z.z)+(w.x*pInput->z.w)|(x.y*pInput->z.x)+(y.y*pInput->z.y)+(z.y*pInput->z.z)+(w.y*pInput->z.w)|(x.z*pInput->z.x)+(y.z*pInput->z.y)+(z.z*pInput->z.z)+(w.z*pInput->z.w)|(x.w*pInput->z.x)+(y.w*pInput->z.y)+(z.w*pInput->z.z)+(w.w*pInput->z.w)|
    |w|(x.x*pInput->w.x)+(y.x*pInput->w.y)+(z.x*pInput->z.z)+(w.x*pInput->w.w)|(x.y*pInput->w.x)+(y.y*pInput->w.y)+(z.y*pInput->z.z)+(w.y*pInput->w.w)|(x.z*pInput->w.x)+(y.z*pInput->w.y)+(z.z*pInput->z.z)+(w.z*pInput->w.w)|(x.w*pInput->w.x)+(y.w*pInput->w.y)+(z.w*pInput->z.z)+(w.w*pInput->w.w)|

    \param pInput Matrix to multiply against
    \sa Multiply(const Matrix4D_t *,const Matrix4D_t *)

***************************************/
// clang-format on

void BURGER_API Burger::Matrix4D_t::Multiply(
    const Matrix4D_t* pInput) BURGER_NOEXCEPT
{
    const float fXX = (x.x * pInput->x.x) + (y.x * pInput->x.y) +
        (z.x * pInput->x.z) + (w.x * pInput->x.w);
    const float fXY = (x.y * pInput->x.x) + (y.y * pInput->x.y) +
        (z.y * pInput->x.z) + (w.y * pInput->x.w);
    const float fXZ = (x.z * pInput->x.x) + (y.z * pInput->x.y) +
        (z.z * pInput->x.z) + (w.z * pInput->x.w);
    const float fXW = (x.w * pInput->x.x) + (y.w * pInput->x.y) +
        (z.w * pInput->x.z) + (w.w * pInput->x.w);

    const float fYX = (x.x * pInput->y.x) + (y.x * pInput->y.y) +
        (z.x * pInput->y.z) + (w.x * pInput->y.w);
    const float fYY = (x.y * pInput->y.x) + (y.y * pInput->y.y) +
        (z.y * pInput->y.z) + (w.y * pInput->y.w);
    const float fYZ = (x.z * pInput->y.x) + (y.z * pInput->y.y) +
        (z.z * pInput->y.z) + (w.z * pInput->y.w);
    const float fYW = (x.w * pInput->y.x) + (y.w * pInput->y.y) +
        (z.w * pInput->y.z) + (w.w * pInput->y.w);

    const float fZX = (x.x * pInput->z.x) + (y.x * pInput->z.y) +
        (z.x * pInput->z.z) + (w.x * pInput->z.w);
    const float fZY = (x.y * pInput->z.x) + (y.y * pInput->z.y) +
        (z.y * pInput->z.z) + (w.y * pInput->z.w);
    const float fZZ = (x.z * pInput->z.x) + (y.z * pInput->z.y) +
        (z.z * pInput->z.z) + (w.z * pInput->z.w);
    const float fZW = (x.w * pInput->z.x) + (y.w * pInput->z.y) +
        (z.w * pInput->z.z) + (w.w * pInput->z.w);

    const float fWX = (x.x * pInput->w.x) + (y.x * pInput->w.y) +
        (z.x * pInput->w.z) + (w.x * pInput->w.w);
    const float fWY = (x.y * pInput->w.x) + (y.y * pInput->w.y) +
        (z.y * pInput->w.z) + (w.y * pInput->w.w);
    const float fWZ = (x.z * pInput->w.x) + (y.z * pInput->w.y) +
        (z.z * pInput->w.z) + (w.z * pInput->w.w);
    const float fWW = (x.w * pInput->w.x) + (y.w * pInput->w.y) +
        (z.w * pInput->w.z) + (w.w * pInput->w.w);
    x.x = fXX;
    x.y = fXY;
    x.z = fXZ;
    x.w = fXW;
    y.x = fYX;
    y.y = fYY;
    y.z = fYZ;
    y.w = fYW;
    z.x = fZX;
    z.y = fZY;
    z.z = fZZ;
    z.w = fZW;
    w.x = fWX;
    w.y = fWY;
    w.z = fWZ;
    w.w = fWW;
}

// clang-format off
/*! ************************************

    \brief Perform a matrix multiply

    Multiply two matrices together and store the result in this matrix.

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(pInput1->x.x*pInput2->x.x)+(pInput1->y.x*pInput2->x.y)+(pInput1->z.x*pInput2->x.z)+(pInput1->w.x*pInput2->x.w)|(pInput1->x.y*pInput2->x.x)+(pInput1->y.y*pInput2->x.y)+(pInput1->z.y*pInput2->x.z)+(pInput1->w.y*pInput2->x.w)|(pInput1->x.z*pInput2->x.x)+(pInput1->y.z*pInput2->x.y)+(pInput1->z.z*pInput2->x.z)+(pInput1->w.z*pInput2->x.w)|(pInput1->x.w*pInput2->x.x)+(pInput1->y.w*pInput2->x.y)+(pInput1->z.w*pInput2->x.z)+(pInput1->w.w*pInput2->x.w)|
    |y|(pInput1->x.x*pInput2->y.x)+(pInput1->y.x*pInput2->y.y)+(pInput1->z.x*pInput2->y.z)+(pInput1->w.x*pInput2->y.w)|(pInput1->x.y*pInput2->y.x)+(pInput1->y.y*pInput2->y.y)+(pInput1->z.y*pInput2->y.z)+(pInput1->w.y*pInput2->y.w)|(pInput1->x.z*pInput2->y.x)+(pInput1->y.z*pInput2->y.y)+(pInput1->z.z*pInput2->y.z)+(pInput1->w.z*pInput2->y.w)|(pInput1->x.w*pInput2->y.x)+(pInput1->y.w*pInput2->y.y)+(pInput1->z.w*pInput2->y.z)+(pInput1->w.w*pInput2->y.w)|
    |z|(pInput1->x.x*pInput2->z.x)+(pInput1->y.x*pInput2->z.y)+(pInput1->z.x*pInput2->z.z)+(pInput1->w.x*pInput2->z.w)|(pInput1->x.y*pInput2->z.x)+(pInput1->y.y*pInput2->z.y)+(pInput1->z.y*pInput2->z.z)+(pInput1->w.y*pInput2->z.w)|(pInput1->x.z*pInput2->z.x)+(pInput1->y.z*pInput2->z.y)+(pInput1->z.z*pInput2->z.z)+(pInput1->w.z*pInput2->z.w)|(pInput1->x.w*pInput2->z.x)+(pInput1->y.w*pInput2->z.y)+(pInput1->z.w*pInput2->z.z)+(pInput1->w.w*pInput2->z.w)|
    |w|(pInput1->x.x*pInput2->w.x)+(pInput1->y.x*pInput2->w.y)+(pInput1->z.x*pInput2->z.z)+(pInput1->w.x*pInput2->w.w)|(pInput1->x.y*pInput2->w.x)+(pInput1->y.y*pInput2->w.y)+(pInput1->z.y*pInput2->z.z)+(pInput1->w.y*pInput2->w.w)|(pInput1->x.z*pInput2->w.x)+(pInput1->y.z*pInput2->w.y)+(pInput1->z.z*pInput2->z.z)+(pInput1->w.z*pInput2->w.w)|(pInput1->x.w*pInput2->w.x)+(pInput1->y.w*pInput2->w.y)+(pInput1->z.w*pInput2->z.z)+(pInput1->w.w*pInput2->w.w)|

    \note This function is optimized to reduce variable usage by storing into
        the output without buffering. Do not use the output matrix as an input matrix.

    \param pInput1 Matrix to multiply from
    \param pInput2 Matrix to multiply against
    \sa Multiply(const Matrix4D_t *)

***************************************/
// clang-format on

void BURGER_API Burger::Matrix4D_t::Multiply(
    const Matrix4D_t* pInput1, const Matrix4D_t* pInput2) BURGER_NOEXCEPT
{
    x.x = (pInput1->x.x * pInput2->x.x) + (pInput1->y.x * pInput2->x.y) +
        (pInput1->z.x * pInput2->x.z) + (pInput1->w.x * pInput2->x.w);
    x.y = (pInput1->x.y * pInput2->x.x) + (pInput1->y.y * pInput2->x.y) +
        (pInput1->z.y * pInput2->x.z) + (pInput1->w.y * pInput2->x.w);
    x.z = (pInput1->x.z * pInput2->x.x) + (pInput1->y.z * pInput2->x.y) +
        (pInput1->z.z * pInput2->x.z) + (pInput1->w.z * pInput2->x.w);
    x.w = (pInput1->x.w * pInput2->x.x) + (pInput1->y.w * pInput2->x.y) +
        (pInput1->z.w * pInput2->x.z) + (pInput1->w.w * pInput2->x.w);

    y.x = (pInput1->x.x * pInput2->y.x) + (pInput1->y.x * pInput2->y.y) +
        (pInput1->z.x * pInput2->y.z) + (pInput1->w.x * pInput2->y.w);
    y.y = (pInput1->x.y * pInput2->y.x) + (pInput1->y.y * pInput2->y.y) +
        (pInput1->z.y * pInput2->y.z) + (pInput1->w.y * pInput2->y.w);
    y.z = (pInput1->x.z * pInput2->y.x) + (pInput1->y.z * pInput2->y.y) +
        (pInput1->z.z * pInput2->y.z) + (pInput1->w.z * pInput2->y.w);
    y.w = (pInput1->x.w * pInput2->y.x) + (pInput1->y.w * pInput2->y.y) +
        (pInput1->z.w * pInput2->y.z) + (pInput1->w.w * pInput2->y.w);

    z.x = (pInput1->x.x * pInput2->z.x) + (pInput1->y.x * pInput2->z.y) +
        (pInput1->z.x * pInput2->z.z) + (pInput1->w.x * pInput2->z.w);
    z.y = (pInput1->x.y * pInput2->z.x) + (pInput1->y.y * pInput2->z.y) +
        (pInput1->z.y * pInput2->z.z) + (pInput1->w.y * pInput2->z.w);
    z.z = (pInput1->x.z * pInput2->z.x) + (pInput1->y.z * pInput2->z.y) +
        (pInput1->z.z * pInput2->z.z) + (pInput1->w.z * pInput2->z.w);
    z.w = (pInput1->x.w * pInput2->z.x) + (pInput1->y.w * pInput2->z.y) +
        (pInput1->z.w * pInput2->z.z) + (pInput1->w.w * pInput2->z.w);

    w.x = (pInput1->x.x * pInput2->w.x) + (pInput1->y.x * pInput2->w.y) +
        (pInput1->z.x * pInput2->w.z) + (pInput1->w.x * pInput2->w.w);
    w.y = (pInput1->x.y * pInput2->w.x) + (pInput1->y.y * pInput2->w.y) +
        (pInput1->z.y * pInput2->w.z) + (pInput1->w.y * pInput2->w.w);
    w.z = (pInput1->x.z * pInput2->w.x) + (pInput1->y.z * pInput2->w.y) +
        (pInput1->z.z * pInput2->w.z) + (pInput1->w.z * pInput2->w.w);
    w.w = (pInput1->x.w * pInput2->w.x) + (pInput1->y.w * pInput2->w.y) +
        (pInput1->z.w * pInput2->w.z) + (pInput1->w.w * pInput2->w.w);
}

/*! ************************************

    \brief Perform a matrix multiply by a scalar.

    Multiply a scalar value to every element in the matrix.

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx*fInput|xy*fInput|xz*fInput|xw*fInput|
    |y|yx*fInput|yy*fInput|yz*fInput|yw*fInput|
    |z|zx*fInput|zy*fInput|zz*fInput|zw*fInput|
    |w|wx*fInput|wy*fInput|wz*fInput|ww*fInput|

    \param fScale Scalar value to multiply all values by
    \sa Multiply(const Matrix4D_t *,float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(float fScale) BURGER_NOEXCEPT
{
    x.x *= fScale;
    x.y *= fScale;
    x.z *= fScale;
    x.w *= fScale;

    y.x *= fScale;
    y.y *= fScale;
    y.z *= fScale;
    y.w *= fScale;

    z.x *= fScale;
    z.y *= fScale;
    z.z *= fScale;
    z.w *= fScale;

    w.x *= fScale;
    w.y *= fScale;
    w.z *= fScale;
    w.w *= fScale;
}

/*! ************************************

    \brief Initialize with a matrix multiplied by a scalar

    Multiply all values of the matrix by a scalar constant and store the result
    in this matrix

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->xx*fInput|pInput->xy*fInput|pInput->xz*fInput|pInput->xw*fInput|
    |y|pInput->yx*fInput|pInput->yy*fInput|pInput->yz*fInput|pInput->yw*fInput|
    |z|pInput->zx*fInput|pInput->zy*fInput|pInput->zz*fInput|pInput->zw*fInput|
    |w|pInput->wx*fInput|pInput->wy*fInput|pInput->wz*fInput|pInput->ww*fInput|

    \param pInput Pointer to Matrix to multiply
    \param fScale Scalar value to multiply all values by

    \sa Multiply(float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(
    const Matrix4D_t* pInput, float fScale) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fScale;
    x.y = pInput->x.y * fScale;
    x.z = pInput->x.z * fScale;
    x.w = pInput->x.w * fScale;

    y.x = pInput->y.x * fScale;
    y.y = pInput->y.y * fScale;
    y.z = pInput->y.z * fScale;
    y.w = pInput->y.w * fScale;

    z.x = pInput->z.x * fScale;
    z.y = pInput->z.y * fScale;
    z.z = pInput->z.z * fScale;
    z.w = pInput->z.w * fScale;

    w.x = pInput->w.x * fScale;
    w.y = pInput->w.y * fScale;
    w.z = pInput->w.z * fScale;
    w.w = pInput->w.w * fScale;
}

/*! ************************************

    \brief Multiply by an X,Y and Z scale

    Using only the x.x, y.y, and z.z components of a simulated matrix, perform a
    matrix multiply quickly that would yield a multiplication vs a scale matrix

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(x.x*fScaleX)|(x.y*fScaleX)|(x.z*fScaleX)|(x.w*fScaleX)|
    |y|(y.x*fScaleY)|(y.y*fScaleY)|(y.z*fScaleY)|(y.w*fScaleY)|
    |z|(z.x*fScaleZ)|(z.y*fScaleZ)|(z.z*fScaleZ)|(z.w*fScaleZ)|
    |w|(w.x*1.0f)|(w.y*1.0f)|(w.z*1.0f)|(w.w*1.0f)|

    \note Terms with a multiply by 1.0f perform no action in the implementation.

    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \sa Multiply(const Matrix4D_t *,float,float,float),
        TransposeMultiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(
    float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT
{
    x.x = x.x * fScaleX;
    x.y = x.y * fScaleX;
    x.z = x.z * fScaleX;
    x.w = x.w * fScaleX;
    y.x = y.x * fScaleY;
    y.y = y.y * fScaleY;
    y.z = y.z * fScaleY;
    y.w = y.w * fScaleY;
    z.x = z.x * fScaleZ;
    z.y = z.y * fScaleZ;
    z.z = z.z * fScaleZ;
    z.w = z.w * fScaleZ;
    //	w.x = w.x * 1.0f;
    //	w.y = w.y * 1.0f;
    //	w.z = w.z * 1.0f;
    //	w.w = w.w * 1.0f;
}

/*! ************************************

    \brief Multiply by an X,Y and Z scale into a copy

    Using only the x.x, y.y, and z.z components of a simulated matrix, perform a
    matrix multiply quickly that would yield a multiplication vs a scale matrix
    and store the result in this Matrix4D_t

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(pInput->x.x*fScaleX)|(pInput->x.y*fScaleX)|(pInput->x.z*fScaleX)|(pInput->x.w*fScaleX)|
    |y|(pInput->y.x*fScaleY)|(pInput->y.y*fScaleY)|(pInput->y.z*fScaleY)|(pInput->y.w*fScaleY)|
    |z|(pInput->z.x*fScaleZ)|(pInput->z.y*fScaleZ)|(pInput->z.z*fScaleZ)|(pInput->z.w*fScaleZ)|
    |w|(pInput->w.x*1.0f)|(pInput->w.y*1.0f)|pInput->w.z*1.0f)|(pInput->w.w*1.0f)|

    \note Terms with a multiply by 1.0f perform a copy operation instead of a
        multiply in the implementation.

    \param pInput Pointer to a valid Matrix4D_t to apply the scale to
    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \sa Multiply(float,float,float), TransposeMultiply(const
        Matrix4D_t*,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t* pInput,
    float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fScaleX;
    x.y = pInput->x.y * fScaleX;
    x.z = pInput->x.z * fScaleX;
    x.w = pInput->x.w * fScaleX;
    y.x = pInput->y.x * fScaleY;
    y.y = pInput->y.y * fScaleY;
    y.z = pInput->y.z * fScaleY;
    y.w = pInput->y.w * fScaleY;
    z.x = pInput->z.x * fScaleZ;
    z.y = pInput->z.y * fScaleZ;
    z.z = pInput->z.z * fScaleZ;
    z.w = pInput->z.w * fScaleZ;
    w.x = pInput->w.x; // * 1.0f;
    w.y = pInput->w.y; // * 1.0f;
    w.z = pInput->w.z; // * 1.0f;
    w.w = pInput->w.w; // * 1.0f;
}

/*! ************************************

    \brief Multiply by an X, Y, Z and W scale

    Using only the x.x, y.y, z.z and w.w components of a simulated matrix,
    perform a matrix multiply quickly that would yield a multiplication vs a
    scale matrix

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(x.x*fScaleX)|(x.y*fScaleX)|(x.z*fScaleX)|(x.w*fScaleX)|
    |y|(y.x*fScaleY)|(y.y*fScaleY)|(y.z*fScaleY)|(y.w*fScaleY)|
    |z|(z.x*fScaleZ)|(z.y*fScaleZ)|(z.z*fScaleZ)|(z.w*fScaleZ)|
    |w|(w.x*fScaleW)|(w.y*fScaleW)|(w.z*fScaleW)|(w.w*fScaleW)|

    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \param fScaleW W scale value
    \sa Multiply(const Matrix4D_t *,float,float,float,float),
        TransposeMultiply(float,float,float,float) or
        SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(
    float fScaleX, float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT
{
    x.x = x.x * fScaleX;
    x.y = x.y * fScaleX;
    x.z = x.z * fScaleX;
    x.w = x.w * fScaleX;
    y.x = y.x * fScaleY;
    y.y = y.y * fScaleY;
    y.z = y.z * fScaleY;
    y.w = y.w * fScaleY;
    z.x = z.x * fScaleZ;
    z.y = z.y * fScaleZ;
    z.z = z.z * fScaleZ;
    z.w = z.w * fScaleZ;
    w.x = w.x * fScaleW;
    w.y = w.y * fScaleW;
    w.z = w.z * fScaleW;
    w.w = w.w * fScaleW;
}

/*! ************************************

    \brief Multiply by an X, Y, Z and W scale

    Using only the x.x, y.y, z.z and w.w components of a simulated matrix,
    perform a matrix multiply quickly that would yield a multiplication vs a
    scale matrix and store the result in this Matrix4D_t

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(pInput->x.x*fScaleX)|(pInput->x.y*fScaleX)|(pInput->x.z*fScaleX)|(pInput->x.w*fScaleX)|
    |y|(pInput->y.x*fScaleY)|(pInput->y.y*fScaleY)|(pInput->y.z*fScaleY)|(pInput->y.w*fScaleY)|
    |z|(pInput->z.x*fScaleZ)|(pInput->z.y*fScaleZ)|(pInput->z.z*fScaleZ)|(pInput->z.w*fScaleZ)|
    |w|(pInput->w.x*fScaleW)|(pInput->w.y*fScaleW)|(pInput->w.z*fScaleW)|(pInput->w.w*fScaleW)|

    \param pInput Pointer to a valid Matrix4D_t to apply the scale to
    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \param fScaleW W scale value
    \sa Multiply(float,float,float,float), TransposeMultiply(const
        Matrix4D_t*,float,float,float,float) or
        SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t* pInput,
    float fScaleX, float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fScaleX;
    x.y = pInput->x.y * fScaleX;
    x.z = pInput->x.z * fScaleX;
    x.w = pInput->x.w * fScaleX;
    y.x = pInput->y.x * fScaleY;
    y.y = pInput->y.y * fScaleY;
    y.z = pInput->y.z * fScaleY;
    y.w = pInput->y.w * fScaleY;
    z.x = pInput->z.x * fScaleZ;
    z.y = pInput->z.y * fScaleZ;
    z.z = pInput->z.z * fScaleZ;
    z.w = pInput->z.w * fScaleZ;
    w.x = pInput->w.x * fScaleW;
    w.y = pInput->w.y * fScaleW;
    w.z = pInput->w.z * fScaleW;
    w.w = pInput->w.w * fScaleW;
}

/*! ************************************

    \brief Perform a 3x3 matrix multiply by a scalar.

    Multiply a scalar value to the 3x3 subset of a matrix (The equivalent of W
    equaling 1.0f)

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|xx*fInput|xy*fInput|xz*fInput|xw*1.0f|
    |y|yx*fInput|yy*fInput|yz*fInput|yw*1.0f|
    |z|zx*fInput|zy*fInput|zz*fInput|zw*1.0f|
    |w|wx*1.0f|wy*1.0f|wz*1.0f|ww*1.0f|

    \note The terms where the value is multiplied by 1.0f are actually not
    performed and are replaced with a value copy since mathematically they
    perform the same action.

    \param fInput Scalar value to multiply all values by
    \sa Multiply3x3(const Matrix4D_t *,float) or Multiply(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply3x3(float fInput) BURGER_NOEXCEPT
{
    x.x *= fInput;
    x.y *= fInput;
    x.z *= fInput;

    y.x *= fInput;
    y.y *= fInput;
    y.z *= fInput;

    z.x *= fInput;
    z.y *= fInput;
    z.z *= fInput;
}

/*! ************************************

    \brief Perform a 3x3 matrix multiply by a scalar.

    Multiply a scalar value to the 3x3 subset of a matrix (The equivalent of W
    equaling 1.0f)

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|pInput->xx*fInput|pInput->xy*fInput|pInput->xz*fInput|pInput->xw*1.0f|
    |y|pInput->yx*fInput|pInput->yy*fInput|pInput->yz*fInput|pInput->yw*1.0f|
    |z|pInput->zx*fInput|pInput->zy*fInput|pInput->zz*fInput|pInput->zw*1.0f|
    |w|pInput->wx*1.0f|pInput->wy*1.0f|pInput->wz*1.0f|pInput->ww*1.0f|

    \note The terms where the value is multiplied by 1.0f are actually not
    performed and are replaced with a value copy since mathematically they
    perform the same action.

    \param pInput Pointer to Matrix to multiply
    \param fInput Scalar value to multiply all values by
    \sa Multiply3x3(float) or Multiply(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply3x3(
    const Matrix4D_t* pInput, float fInput) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fInput;
    x.y = pInput->x.y * fInput;
    x.z = pInput->x.z * fInput;
    x.w = pInput->x.w;

    y.x = pInput->y.x * fInput;
    y.y = pInput->y.y * fInput;
    y.z = pInput->y.z * fInput;
    y.w = pInput->y.w;

    z.x = pInput->z.x * fInput;
    z.y = pInput->z.y * fInput;
    z.z = pInput->z.z * fInput;
    z.w = pInput->z.w;

    w.x = pInput->w.x;
    w.y = pInput->w.y;
    w.z = pInput->w.z;
    w.w = pInput->w.w;
}

/*! ************************************

    \brief Multiply by an X,Y and Z scale

    Using only the x.x, y.y, and z.z components of a simulated matrix, perform a
    matrix multiply quickly that would yield a multiplication vs a scale matrix

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(x.x*fScaleX)|(x.y*fScaleY)|(x.z*fScaleZ)|(x.w*1.0f)|
    |y|(y.x*fScaleX)|(y.y*fScaleY)|(y.z*fScaleZ)|(y.w*1.0f)|
    |z|(z.x*fScaleX)|(z.y*fScaleY)|(z.z*fScaleZ)|(z.w*1.0f)|
    |w|(w.x*fScaleX)|(w.y*fScaleY)|(w.z*fScaleZ)|(w.w*1.0f)|

    \note Terms with a multiply by 1.0f perform no action in the implementation.

    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \sa TransposeMultiply(const Matrix4D_t *,float,float,float),
        Multiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(
    float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT
{
    x.x = x.x * fScaleX;
    x.y = x.y * fScaleY;
    x.z = x.z * fScaleZ;
    //	x.w = x.w * 1.0f;
    y.x = y.x * fScaleX;
    y.y = y.y * fScaleY;
    y.z = y.z * fScaleZ;
    //	y.w = y.w * 1.0f;
    z.x = z.x * fScaleX;
    z.y = z.y * fScaleY;
    z.z = z.z * fScaleZ;
    //	z.w = z.w * 1.0f;
    w.x = w.x * fScaleX;
    w.y = w.y * fScaleY;
    w.z = w.z * fScaleZ;
    //	w.w = w.w * 1.0f;
}

/*! ************************************

    \brief Multiply by an X,Y and Z scale into a copy

    Using only the x.x, y.y, and z.z components of a simulated matrix, perform a
    matrix multiply quickly that would yield a multiplication vs a scale matrix
    and store the result in this Matrix4D_t

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(pInput->x.x*fScaleX)|(pInput->x.y*fScaleY)|(pInput->x.z*fScaleZ)|(pInput->x.w*1.0f)|
    |y|(pInput->y.x*fScaleX)|(pInput->y.y*fScaleY)|(pInput->y.z*fScaleZ)|(pInput->y.w*1.0f)|
    |z|(pInput->z.x*fScaleX)|(pInput->z.y*fScaleY)|(pInput->z.z*fScaleZ)|(pInput->z.w*1.0f)|
    |w|(pInput->w.x*fScaleX)|(pInput->w.y*fScaleY)|(pInput->w.z*fScaleZ)|(pInput->w.w*1.0f)|

    \note Terms with a multiply by 1.0f perform a copy operation instead of a
        multiply in the implementation.

    \param pInput Pointer to a valid Matrix4D_t to apply the scale to
    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \sa TransposeMultiply(float,float,float), Multiply(const
        Matrix4D_t*,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(const Matrix4D_t* pInput,
    float fScaleX, float fScaleY, float fScaleZ) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fScaleX;
    x.y = pInput->x.y * fScaleY;
    x.z = pInput->x.z * fScaleZ;
    x.w = pInput->x.w; // * 1.0f;
    y.x = pInput->y.x * fScaleX;
    y.y = pInput->y.y * fScaleY;
    y.z = pInput->y.z * fScaleZ;
    y.w = pInput->y.w; // * 1.0f;
    z.x = pInput->z.x * fScaleX;
    z.y = pInput->z.y * fScaleY;
    z.z = pInput->z.z * fScaleZ;
    z.w = pInput->z.w; // * 1.0f;
    w.x = pInput->w.x * fScaleX;
    w.y = pInput->w.y * fScaleY;
    w.z = pInput->w.z * fScaleZ;
    w.w = pInput->w.w; // * 1.0f;
}

/*! ************************************

    \brief Multiply by an X, Y, Z, and W scale

    Using only the x.x, y.y, z.z and w.w components of a simulated matrix,
    perform a matrix multiply quickly that would yield a multiplication vs a
    scale matrix

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(x.x*fScaleX)|(x.y*fScaleY)|(x.z*fScaleZ)|(x.w*fScaleW)|
    |y|(y.x*fScaleX)|(y.y*fScaleY)|(y.z*fScaleZ)|(y.w*fScaleW)|
    |z|(z.x*fScaleX)|(z.y*fScaleY)|(z.z*fScaleZ)|(z.w*fScaleW)|
    |w|(w.x*fScaleX)|(w.y*fScaleY)|(w.z*fScaleZ)|(w.w*fScaleW)|

    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \param fScaleW W scale value
    \sa TransposeMultiply(const Matrix4D_t *,float,float,float,float),
        Multiply(float,float,float,float) or SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(
    float fScaleX, float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT
{
    x.x = x.x * fScaleX;
    x.y = x.y * fScaleY;
    x.z = x.z * fScaleZ;
    x.w = x.w * fScaleW;
    y.x = y.x * fScaleX;
    y.y = y.y * fScaleY;
    y.z = y.z * fScaleZ;
    y.w = y.w * fScaleW;
    z.x = z.x * fScaleX;
    z.y = z.y * fScaleY;
    z.z = z.z * fScaleZ;
    z.w = z.w * fScaleW;
    w.x = w.x * fScaleX;
    w.y = w.y * fScaleY;
    w.z = w.z * fScaleZ;
    w.w = w.w * fScaleW;
}

/*! ************************************

    \brief Multiply by an X, Y, Z, and W scale

    Using only the x.x, y.y, z.z and w.w components of a simulated matrix,
    perform a matrix multiply quickly that would yield a multiplication vs a
    scale matrix and store the result in this Matrix4D_t

    Use this formula to create the final matrix

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(pInput->x.x*fScaleX)|(pInput->x.y*fScaleY)|(pInput->x.z*fScaleZ)|(pInput->x.w*fScaleW)|
    |y|(pInput->y.x*fScaleX)|(pInput->y.y*fScaleY)|(pInput->y.z*fScaleZ)|(pInput->y.w*fScaleW)|
    |z|(pInput->z.x*fScaleX)|(pInput->z.y*fScaleY)|(pInput->z.z*fScaleZ)|(pInput->z.w*fScaleW)|
    |w|(pInput->w.x*fScaleX)|(pInput->w.y*fScaleY)|(pInput->w.z*fScaleZ)|(pInput->w.w*fScaleW)|

    \param pInput Pointer to a valid Matrix4D_t to apply the scale to
    \param fScaleX X scale value
    \param fScaleY Y scale value
    \param fScaleZ Z scale value
    \param fScaleW W scale value
    \sa TransposeMultiply(float,float,float,float), Multiply(const
        Matrix4D_t*,float,float,float,float) or
        SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(const Matrix4D_t* pInput,
    float fScaleX, float fScaleY, float fScaleZ, float fScaleW) BURGER_NOEXCEPT
{
    x.x = pInput->x.x * fScaleX;
    x.y = pInput->x.y * fScaleY;
    x.z = pInput->x.z * fScaleZ;
    x.w = pInput->x.w * fScaleW;
    y.x = pInput->y.x * fScaleX;
    y.y = pInput->y.y * fScaleY;
    y.z = pInput->y.z * fScaleZ;
    y.w = pInput->y.w * fScaleW;
    z.x = pInput->z.x * fScaleX;
    z.y = pInput->z.y * fScaleY;
    z.z = pInput->z.z * fScaleZ;
    z.w = pInput->z.w * fScaleW;
    w.x = pInput->w.x * fScaleX;
    w.y = pInput->w.y * fScaleY;
    w.z = pInput->w.z * fScaleZ;
    w.w = pInput->w.w * fScaleW;
}

/*! ************************************

    \brief Multiply a vector by a matrix

    Transform the point by the matrix

    |x|y|z|
    |:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)+(xw)|(yx*x)+(yy*y)+(yz*z)+(yw)|(zx*x)+(zy*y)+(zz*z)+(zw)|

    \param pInput Pointer to a Vector3D_t to transform against this matrix
    \sa Transform(Vector3D_t *,const Vector3D_t *) const or
        TransposeTransform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(
    Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pInput->x = x.x * fX + x.y * fY + x.z * fZ + x.w;
    pInput->y = y.x * fX + y.y * fY + y.z * fZ + y.w;
    pInput->z = z.x * fX + z.y * fY + z.z * fZ + z.w;
}

/*! ************************************

    \brief Multiply a vector by a matrix

    Transform the point by the matrix

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)+(xw*w)|(yx*x)+(yy*y)+(yz*z)+(yw*w)|(zx*x)+(zy*y)+(zz*z)+(zw*w)|(wx*x)+(wy*y)+(wz*z)+(ww*w)|

    \param pInput Pointer to a Vector4D_t to transform against this matrix
    \sa Transform(Vector4D_t *,const Vector4D_t *) const or
        TransposeTransform(Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(
    Vector4D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    const float fW = pInput->w;
    pInput->x = x.x * fX + x.y * fY + x.z * fZ + x.w * fW;
    pInput->y = y.x * fX + y.y * fY + y.z * fZ + y.w * fW;
    pInput->z = z.x * fX + z.y * fY + z.z * fZ + z.w * fW;
    pInput->w = w.x * fX + w.y * fY + w.z * fZ + w.w * fW;
}

/*! ************************************

    \brief Multiply a vector by a matrix

    Transform the point by the matrix and store the result in an uninitialized
    Vector3D_t

    |x|y|z|
    |:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)+(xw)|(yx*x)+(yy*y)+(yz*z)+(yw)|(zx*x)+(zy*y)+(zz*z)+(zw)|

    \param pInput Pointer to a Vector3D_t to transform against this matrix
    \param pOutput Pointer to an uninitialized Vector3D_t to store the result
    \sa Transform(Vector3D_t *) const or TransposeTransform(Vector3D_t *,const
        Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(
    Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pOutput->x = x.x * fX + x.y * fY + x.z * fZ + x.w;
    pOutput->y = y.x * fX + y.y * fY + y.z * fZ + y.w;
    pOutput->z = z.x * fX + z.y * fY + z.z * fZ + z.w;
}

/*! ************************************

    \brief Multiply a vector by a matrix

    Transform the point by the matrix and store the result in an
    uninitialized Vector4D_t

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)+(xw*w)|(yx*x)+(yy*y)+(yz*z)+(yw*w)|(zx*x)+(zy*y)+(zz*z)+(zw*w)|(wx*x)+(wy*y)+(wz*z)+(ww*w)|

    \param pInput Pointer to a Vector4D_t to transform against this matrix
    \param pOutput Pointer to an uninitialized Vector4D_t to store the result

    \sa Transform(Vector4D_t *) const or TransposeTransform(Vector4D_t *,const
        Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(
    Vector4D_t* pOutput, const Vector4D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    const float fW = pInput->w;
    pOutput->x = x.x * fX + x.y * fY + x.z * fZ + x.w * fW;
    pOutput->y = y.x * fX + y.y * fY + y.z * fZ + y.w * fW;
    pOutput->z = z.x * fX + z.y * fY + z.z * fZ + z.w * fW;
    pOutput->w = w.x * fX + w.y * fY + w.z * fZ + w.w * fW;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix

    Transform the point by the transposed matrix

    |x|y|z|
    |:--:|:--:|:--:|
    |(xx*x)+(yx*y)+(zx*z)+(wx)|(xy*x)+(yy*y)+(zy*z)+(wy)|(xz*x)+(yz*y)+(zz*z)+(wz)|

    \param pInput Pointer to a Vector3D_t to transform against this transposed
        matrix
    \sa TransposeTransform(Vector3D_t *,const Vector3D_t *) const or
        Transform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(
    Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pInput->x = x.x * fX + y.x * fY + z.x * fZ + w.x;
    pInput->y = x.y * fX + y.y * fY + z.y * fZ + w.y;
    pInput->z = x.z * fX + y.z * fY + z.z * fZ + w.z;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix

    Transform the point by the transposed matrix

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(yx*y)+(zx*z)+(wx*w)|(xy*x)+(yy*y)+(zy*z)+(wy*w)|(xz*x)+(yz*y)+(zz*z)+(wz*w)|(xw*x)+(yw*y)+(zw*z)+(ww*w)|

    \param pInput Pointer to a Vector4D_t to transform against this transposed
        matrix
    \sa TransposeTransform(Vector4D_t *,const Vector4D_t *) const or
        Transform(Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(
    Vector4D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    const float fW = pInput->w;
    pInput->x = x.x * fX + y.x * fY + z.x * fZ + w.x * fW;
    pInput->y = x.y * fX + y.y * fY + z.y * fZ + w.y * fW;
    pInput->z = x.z * fX + y.z * fY + z.z * fZ + w.z * fW;
    pInput->w = x.w * fX + y.w * fY + z.w * fZ + w.w * fW;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix

    Transform the point by the transposed matrix and store the result in an
    uninitialized Vector3D_t

    |x|y|z|
    |:--:|:--:|:--:|
    |(xx*x)+(yx*y)+(zx*z)+(wx)|(xy*x)+(yy*y)+(zy*z)+(wy)|(xz*x)+(yz*y)+(zz*z)+(wz)|

    \param pInput Pointer to a Vector3D_t to transform against this transposed
        matrix
    \param pOutput Pointer to an uninitialized Vector3D_t to store the result
    \sa TransposeTransform(Vector3D_t *) const or Transform(Vector3D_t *,const
        Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(
    Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pOutput->x = x.x * fX + y.x * fY + z.x * fZ + w.x;
    pOutput->y = x.y * fX + y.y * fY + z.y * fZ + w.y;
    pOutput->z = x.z * fX + y.z * fY + z.z * fZ + w.z;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix

    Transform the point by the transposed matrix and store the result in an
    uninitialized Vector4D_t

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(yx*y)+(zx*z)+(wx*w)|(xy*x)+(yy*y)+(zy*z)+(wy*w)|(xz*x)+(yz*y)+(zz*z)+(wz*w)|(xw*x)+(yw*y)+(zw*z)+(ww*w)|

    \param pInput Pointer to a Vector4D_t to transform against this transposed
        matrix
    \param pOutput Pointer to an uninitialized Vector4D_t to store the result
    \sa TransposeTransform(Vector4D_t *) const or Transform(Vector4D_t *,const
        Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(
    Vector4D_t* pOutput, const Vector4D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    const float fW = pInput->w;
    pOutput->x = x.x * fX + y.x * fY + z.x * fZ + w.x * fW;
    pOutput->y = x.y * fX + y.y * fY + z.y * fZ + w.y * fW;
    pOutput->z = x.z * fX + y.z * fY + z.z * fZ + w.z * fW;
    pOutput->w = x.w * fX + y.w * fY + z.w * fZ + w.w * fW;
}

/*! ************************************

    \brief Multiply a vector by a matrix without adding W

    Transform the point by the matrix only using the x, y and z terms

    |x|y|z|
    |:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)|(yx*x)+(yy*y)+(yz*z)|(zx*x)+(zy*y)+(zz*z)|

    \param pInput Pointer to a Vector3D_t to transform against this matrix
    \sa Transform3x3(Vector3D_t *,const Vector3D_t *) const or
        TransposeTransform3x3(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform3x3(
    Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pInput->x = x.x * fX + x.y * fY + x.z * fZ;
    pInput->y = y.x * fX + y.y * fY + y.z * fZ;
    pInput->z = z.x * fX + z.y * fY + z.z * fZ;
}

/*! ************************************

    \brief Multiply a vector by a matrix without adding W

    Transform the point by the matrix only using the x, y and z terms and store
    the result in an uninitialized Vector3D_t

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(xy*y)+(xz*z)|(yx*x)+(yy*y)+(yz*z)|(zx*x)+(zy*y)+(zz*z)|

    \param pInput Pointer to a Vector3D_t to transform against this matrix
    \param pOutput Pointer to an uninitialized Vector3D_t to store the result

    \sa Transform3x3(Vector3D_t *) const or
        TransposeTransform3x3(Vector3D_t*,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform3x3(
    Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pOutput->x = x.x * fX + x.y * fY + x.z * fZ;
    pOutput->y = y.x * fX + y.y * fY + y.z * fZ;
    pOutput->z = z.x * fX + z.y * fY + z.z * fZ;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix without adding W

    Transform the point by the transposed matrix only using the x, y and z terms

    |x|y|z|
    |:--:|:--:|:--:|
    |x|(xx*x)+(yx*y)+(zx*z)|(xy*x)+(yy*y)+(zy*z)|(xz*x)+(yz*y)+(zz*z)|

    \param pInput Pointer to a Vector3D_t to transform against this transposed
        matrix
    \sa TransposeTransform3x3(Vector3D_t *,const Vector3D_t *) const or
        Transform3x3(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform3x3(
    Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pInput->x = x.x * fX + y.x * fY + z.x * fZ;
    pInput->y = x.y * fX + y.y * fY + z.y * fZ;
    pInput->z = x.z * fX + y.z * fY + z.z * fZ;
}

/*! ************************************

    \brief Multiply a vector by a transposed matrix without adding W

    Transform the point by the transposed matrix only using the x, y and z terms
    and store the result in an uninitialized Vector3D_t

    |x|y|z|w|
    |:--:|:--:|:--:|:--:|
    |(xx*x)+(yx*y)+(zx*z)|(xy*x)+(yy*y)+(zy*z)|(xz*x)+(yz*y)+(zz*z)|

    \param pInput Pointer to a Vector3D_t to transform against this transposed
        matrix
    \param pOutput Pointer to an uninitialized Vector3D_t to store the result

    \sa TransposeTransform3x3(Vector3D_t *) const or
        Transform3x3(Vector3D_t*,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform3x3(
    Vector3D_t* pOutput, const Vector3D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = pInput->x;
    const float fY = pInput->y;
    const float fZ = pInput->z;
    pOutput->x = x.x * fX + y.x * fY + z.x * fZ;
    pOutput->y = x.y * fX + y.y * fY + z.y * fZ;
    pOutput->z = x.z * fX + y.z * fY + z.z * fZ;
}

/*! ************************************

    \brief Rotate a matrix in the Y axis (Yaw)

    Given a Y angle in radians, rotate the matrix accordingly

    fCos = get_cosine(fYaw); fSin = get_sine(fYaw);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(z.x*fSin)+(x.x*fCos)|(z.y*fSin)+(x.y*fCos)|(z.z*fSin)+(x.z*fCos)|(z.w*fSin)+(x.w*fCos)|
    |y|y.x|y.y|y.z|y.w|
    |z|(z.x*fCos)-(x.x*fSin)|(z.y*fCos)-(x,y*fSin)|(z.z*fCos)-(x.z*fSin)|(z.w*fCos)-(x.w*fSin)|
    |w|w.x|w.y|w.z|w.w|

    \param fYaw Angle in radians to yaw the matrix
    \sa Pitch(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Yaw(float fYaw) BURGER_NOEXCEPT
{
    const float fCos = get_cosine(fYaw);
    const float fSin = get_sine(fYaw);

    const float fXX = x.x;
    const float fXY = x.y;
    const float fXZ = x.z;
    const float fXW = x.w;

    x.x = (z.x * fSin) + (fXX * fCos);
    x.y = (z.y * fSin) + (fXY * fCos);
    x.z = (z.z * fSin) + (fXZ * fCos);
    x.w = (z.w * fSin) + (fXW * fCos);

    z.x = (z.x * fCos) - (fXX * fSin);
    z.y = (z.y * fCos) - (fXY * fSin);
    z.z = (z.z * fCos) - (fXZ * fSin);
    z.w = (z.w * fCos) - (fXW * fSin);
}

/*! ************************************

    \brief Rotate a matrix in the X axis (Pitch)

    Given a X angle in radians, rotate the matrix accordingly

    fCos = get_cosine(fPitch); fSin = get_sine(fPitch);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|x.x|x.y|x.z|x.w|
    |y|(z.x*fSin)+(y.x*fCos)|(z.y*fSin)+(y.y*fCos)|(z.z*fSin)+(y.z*fCos)|(z.w*fSin)+(y.w*fCos)|
    |z|(z.x*fCos)-(y.x*fSin)|(z.y*fCos)-(y.y*fSin)|(z.z*fCos)-(y.z*fSin)|(z.w*fCos)-(y.w*fSin)|
    |w|w.x|w.y|w.z|w.w|

    \param fPitch Angle in radians to pitch the matrix
    \sa Yaw(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Pitch(float fPitch) BURGER_NOEXCEPT
{
    const float fCos = get_cosine(fPitch);
    const float fSin = get_sine(fPitch);

    const float fYX = y.x;
    const float fYY = y.y;
    const float fYZ = y.z;
    const float fYW = y.w;

    y.x = (z.x * fSin) + (fYX * fCos);
    y.y = (z.y * fSin) + (fYY * fCos);
    y.z = (z.z * fSin) + (fYZ * fCos);
    y.w = (z.w * fSin) + (fYW * fCos);

    z.x = (z.x * fCos) - (fYX * fSin);
    z.y = (z.y * fCos) - (fYY * fSin);
    z.z = (z.z * fCos) - (fYZ * fSin);
    z.w = (z.w * fCos) - (fYW * fSin);
}

/*! ************************************

    \brief Rotate a matrix in the Z axis (Roll)

    Given a Z angle in radians, rotate the matrix accordingly

    fCos = get_cosine(fRoll); fSin = get_sine(fRoll);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|(y.x*fSin)+(x.x*fCos)|(y.y*fSin)+(x.y*fCos)|(y.z*fSin)+(x.z*fCos)|(y.w*fSin)+(x.w*fCos)|
    |y|(y.x*fCos)-(x.x*fSin)|(y.y*fCos)-(x.y*fSin)|(y.z*fCos)-(x.z*fSin)|(y.w*fCos)-(x.w*fSin)|
    |z|z.x|z.y|z.z|z.w|
    |w|w.x|w.y|w.z|w.w|

    \param fRoll Angle in radians to roll the matrix
    \sa Yaw(float) or Pitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Roll(float fRoll) BURGER_NOEXCEPT
{
    const float fCos = get_cosine(fRoll);
    const float fSin = get_sine(fRoll);

    const float fXX = x.x;
    const float fXY = x.y;
    const float fXZ = x.z;
    const float fXW = x.w;

    x.x = (y.x * fSin) + (fXX * fCos);
    x.y = (y.y * fSin) + (fXY * fCos);
    x.z = (y.z * fSin) + (fXZ * fCos);
    x.w = (y.w * fSin) + (fXW * fCos);

    y.x = (y.x * fCos) - (fXX * fSin);
    y.y = (y.y * fCos) - (fXY * fSin);
    y.z = (y.z * fCos) - (fXZ * fSin);
    y.w = (y.w * fCos) - (fXW * fSin);
}

/*! ************************************

    \brief Rotate a matrix in an arbitrary axis

    Given a vector to determine direction and an angle in radians, rotate the
    matrix accordingly

    \note This is a replacement for glRotate()

    \param fRadians Angle in radians to roll the matrix
    \param fX X value of the vector
    \param fY Y value of the vector
    \param fZ Z value of the vector
    \sa Yaw(float), Pitch(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Rotate(
    float fRadians, float fX, float fY, float fZ) BURGER_NOEXCEPT
{
    if ((fY == 0.0f) && (fZ == 0.0f)) {
        Pitch(fRadians);
    } else if ((fX == 0.0f) && (fZ == 0.0f)) {
        Yaw(fRadians);
    } else if ((fX == 0.0f) && (fY == 0.0f)) {
        Roll(fRadians);
    } else {
        const float fSin = get_sine(fRadians);
        const float fCos = get_cosine(fRadians);

        // Get the length of the vector
        const float fLength = square_root((fX * fX) + (fY * fY) + (fZ * fZ));

        const float fReciprocalLength = 1.0f / fLength;
        const float fOneLessCosine = 1.0f - fCos;

        float fXNormal = fX * fReciprocalLength;
        float fYNormal = fY * fReciprocalLength;
        float fZNormal = fZ * fReciprocalLength;

        const float fXNormalSquared = fXNormal * fXNormal;
        const float fYNormalSquared = fYNormal * fYNormal;
        const float fZNormalSquared = fZNormal * fZNormal;

        const float fXYNegCosine = fXNormal * fYNormal * fOneLessCosine;
        const float fYZNegCosine = fYNormal * fZNormal * fOneLessCosine;
        const float fZXNegCosine = fZNormal * fXNormal * fOneLessCosine;

        fXNormal *= fSin;
        fYNormal *= fSin;
        fZNormal *= fSin;

        // Create the constants
        const float fXXAdj = fXNormalSquared + fCos * (1.0f - fXNormalSquared);
        const float fXYAdj = fXYNegCosine + fZNormal;
        const float fXZAdj = fZXNegCosine - fYNormal;
        const float fYXAdj = fXYNegCosine - fZNormal;
        const float fYYAdj = fYNormalSquared + fCos * (1.0f - fYNormalSquared);
        const float fYZAdj = fYZNegCosine + fXNormal;
        const float fZXAdj = fZXNegCosine + fYNormal;
        const float fZYAdj = fYZNegCosine - fXNormal;
        const float fZZAdj = fZNormalSquared + fCos * (1.0f - fZNormalSquared);

        // Apply rotation
        float fTemp1 = x.x;
        float fTemp2 = y.x;
        float fTemp3 = z.x;
        x.x = fTemp1 * fXXAdj + fTemp2 * fXYAdj + fTemp3 * fXZAdj;
        y.x = fTemp1 * fYXAdj + fTemp2 * fYYAdj + fTemp3 * fYZAdj;
        z.x = fTemp1 * fZXAdj + fTemp2 * fZYAdj + fTemp3 * fZZAdj;

        fTemp1 = x.y;
        fTemp2 = y.y;
        fTemp3 = z.y;
        x.y = fTemp1 * fXXAdj + fTemp2 * fXYAdj + fTemp3 * fXZAdj;
        y.y = fTemp1 * fYXAdj + fTemp2 * fYYAdj + fTemp3 * fYZAdj;
        z.y = fTemp1 * fZXAdj + fTemp2 * fZYAdj + fTemp3 * fZZAdj;

        fTemp1 = x.z;
        fTemp2 = y.z;
        fTemp3 = z.z;
        x.z = fTemp1 * fXXAdj + fTemp2 * fXYAdj + fTemp3 * fXZAdj;
        y.z = fTemp1 * fYXAdj + fTemp2 * fYYAdj + fTemp3 * fYZAdj;
        z.z = fTemp1 * fZXAdj + fTemp2 * fZYAdj + fTemp3 * fZZAdj;

        fTemp1 = x.w;
        fTemp2 = y.w;
        fTemp3 = z.w;
        x.w = fTemp1 * fXXAdj + fTemp2 * fXYAdj + fTemp3 * fXZAdj;
        y.w = fTemp1 * fYXAdj + fTemp2 * fYYAdj + fTemp3 * fYZAdj;
        z.w = fTemp1 * fZXAdj + fTemp2 * fZYAdj + fTemp3 * fZZAdj;
    }
}

/*! ************************************

    \brief Multiply the matrix by a generated translation matrix

    With an X, Y and Z for translation, apply a matrix multiply if a
    matrix was generated with SetTranslate(float,float,float).

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|x.x|x.y|x.z|x.w|
    |y|y.x|y.y|y.z|y.w|
    |z|z.x|z.y|z.z|z.w|
    |w|(x.x*fX)+(y.x*fY)+(z.x*fZ)+w.x|(x.y*fX)+(y.y*fY)+(z.y*fZ)+w.y|(x.z*fX)+(y.z*fY)+(z.z*fZ)+w.z|(x.w*fX)+(y.w*fY)+(z.w*fZ)+w.w|

    \param fX X value of the temp translation matrix
    \param fY Y value of the temp translation matrix
    \param fZ Z value of the temp translation matrix
    \sa TransposeTranslate(float,float,float) or SetTranslate(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Translate(
    float fX, float fY, float fZ) BURGER_NOEXCEPT
{
    w.x += x.x * fX + y.x * fY + z.x * fZ;
    w.y += x.y * fX + y.y * fY + z.y * fZ;
    w.z += x.z * fX + y.z * fY + z.z * fZ;
    w.w += x.w * fX + y.w * fY + z.w * fZ;
}

/*! ************************************

    \brief Multiply the matrix by a generated transposed translation matrix

    With an X, Y and Z for translation, apply a matrix multiply if a
    matrix was generated with SetTranslate(float,float,float) and then
    Transpose()

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|x.x|x.y|x.z|(x.x*fX)+(x.y*fY)+(x.z*fZ)+x.w|
    |y|y.x|y.y|y.z|(y.x*fX)+(y.y*fY)+(y.z*fZ)+y.w|
    |z|z.x|z.y|z.z|(z.x*fX)+(z.y*fY)+(z.z*fZ)+z.w|
    |w|w.x|w.y|w.z|(w.x*fX)+(w.y*fY)+(w.z*fZ)+w.w|

    \param fX X value of the temp translation matrix
    \param fY Y value of the temp translation matrix
    \param fZ Z value of the temp translation matrix
    \sa Translate(float,float,float) or SetTranslate(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTranslate(
    float fX, float fY, float fZ) BURGER_NOEXCEPT
{
    x.w += x.x * fX + x.y * fY + x.z * fZ;
    y.w += y.x * fX + y.y * fY + y.z * fZ;
    z.w += z.x * fX + z.y * fY + z.z * fZ;
    w.w += w.x * fX + w.y * fY + w.z * fZ;
}

/*! ************************************

    \brief Generate an affine inverse of a matrix

    Using the 3x3 sub-matrix, generate a determinate and use it to calculate the
    inverse of the 3x3 matrix. Adjust the translate component and then clear out
    the scale.

    If the matrix cannot be inverted, \ref FALSE is returned and the original
    matrix is copied as is.

    \param pInput Pointer to a matrix to affine invert.
    \return \ref TRUE if the inversion was successful, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::Matrix4D_t::AffineInverse(
    const Matrix4D_t* pInput) BURGER_NOEXCEPT
{
    const float fPrecisionLimit = (1.0e-15f);

    // Calculate the determinant of the 3x3 section of the matrix
    // while keeping the negative and positive components separate
    // It's needed to determine floating point error
    // to see if floating point precision is enough to
    // calculate the inverse of the matrix

    float fPositive = 0.0f;
    float fNegative = 0.0f;

    float fTemp = pInput->x.x * pInput->y.y * pInput->z.z;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    fTemp = pInput->x.y * pInput->y.z * pInput->z.x;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    fTemp = pInput->x.z * pInput->y.x * pInput->z.y;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    fTemp = -pInput->x.z * pInput->y.y * pInput->z.x;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    fTemp = -pInput->x.y * pInput->y.x * pInput->z.z;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    fTemp = -pInput->x.x * pInput->y.z * pInput->z.y;
    if (fTemp >= 0.0f) {
        fPositive += fTemp;
    } else {
        fNegative += fTemp;
    }

    // The determinant is the sum
    float fDeterminant = fPositive + fNegative;

    // Is the 3x3 matrix divisible with floating point precision?

    uint_t bResult;
    if ((fDeterminant == 0.0f) ||
        (absolute(fDeterminant / (fPositive - fNegative)) < fPrecisionLimit)) {
        Set(pInput);
        bResult = FALSE;
    } else {
        //  Calculate inverse(A) = original(A) / determinant(A)

        // Reciprocal multiply for speed
        fDeterminant = 1.0F / fDeterminant;

        x.x = (pInput->y.y * pInput->z.z - pInput->y.z * pInput->z.y) *
            fDeterminant;
        y.x = -(pInput->y.x * pInput->z.z - pInput->y.z * pInput->z.x) *
            fDeterminant;
        z.x = (pInput->y.x * pInput->z.y - pInput->y.y * pInput->z.x) *
            fDeterminant;
        x.y = -(pInput->x.y * pInput->z.z - pInput->x.z * pInput->z.y) *
            fDeterminant;
        y.y = (pInput->x.x * pInput->z.z - pInput->x.z * pInput->z.x) *
            fDeterminant;
        z.y = -(pInput->x.x * pInput->z.y - pInput->x.y * pInput->z.x) *
            fDeterminant;
        x.z = (pInput->x.y * pInput->y.z - pInput->x.z * pInput->y.y) *
            fDeterminant;
        y.z = -(pInput->x.x * pInput->y.z - pInput->x.z * pInput->y.x) *
            fDeterminant;
        z.z = (pInput->x.x * pInput->y.y - pInput->x.y * pInput->y.x) *
            fDeterminant;

        // Calculate -C * inverse(A)

        const float fX = pInput->w.x;
        const float fY = pInput->w.y;
        const float fZ = pInput->w.z;

        w.x = -(fX * x.x + fY * y.x + fZ * z.x);
        w.y = -(fX * x.y + fY * y.y + fZ * z.y);
        w.z = -(fX * x.z + fY * y.z + fZ * z.z);

        // Fill in last column

        x.w = 0.0f;
        y.w = 0.0f;
        z.w = 0.0f;
        w.w = 1.0f;
        bResult = TRUE;
    }
    return bResult;
}

/*! ************************************

    \brief Create a 4D perspective matrix

    Using a field of view, an aspect ratio (width/height) and a near/far range,
    create a left handed projection matrix.

    fYScale = 1.0f / tan(fFieldOfViewY*0.5f);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|fYScale/fAspect|0|0|0|
    |y|0|fYScale|0|0|
    |z|0|0|fFar/(fFar-fNear)|1|
    |w|0|0|(-fNear * fFar) / (fFar-fNear)|0|

    \note This is a drop in replacement for the Windows function
[D3DXMatrixPerspectiveFovLH()]
        (http://msdn.microsoft.com/en-us/library/bb205350(VS.85).aspx)

    \param fFieldOfViewY Angle of the field of view in radians
    \param fAspect Aspect ratio of the screen
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa PerspectiveFovRH()

***************************************/

void BURGER_API Burger::Matrix4D_t::PerspectiveFovLH(
    float fFieldOfViewY, float fAspect, float fNear, float fFar) BURGER_NOEXCEPT
{
    // Calculate the cotangent of Field of View * 2.0f
    const float fYScale = 1.0f / get_tangent(fFieldOfViewY * 0.5f);
    const float fDepth = (fFar - fNear);

    x.x = fYScale / fAspect;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fYScale;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = fFar / fDepth;
    z.w = 1.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = (-fNear * fFar) / fDepth;
    w.w = 0.0f;
}

/*! ************************************

    \brief Create a 4D perspective matrix

    Using a field of view, an aspect ratio (width/height) and a near/far range,
    create a right handed projection matrix.

    fYScale = 1.0f / tan(fFieldOfViewY*0.5f);

    ||x|y|z|w|
    |:--:|:--:|:--:|:--:|:--:|
    |x|fYScale/fAspect|0|0|0|
    |y|0|fYScale|0|0|
    |z|0|0|fFar/(fNear-fFar)|-1|
    |w|0|0|(fNear * fFar) / (fNear-fFar)|0|

    \note This is a drop in replacement for the Windows function
[D3DXMatrixPerspectiveFovRH()]
    (http://msdn.microsoft.com/en-us/library/bb205351(v=vs.85).aspx)

    \param fFieldOfViewY Angle of the field of view in radians
    \param fAspect Aspect ratio of the screen
    \param fNear Near clip plane
    \param fFar Far clip plane
    \sa PerspectiveFovLH()

***************************************/

void BURGER_API Burger::Matrix4D_t::PerspectiveFovRH(
    float fFieldOfViewY, float fAspect, float fNear, float fFar) BURGER_NOEXCEPT
{
    // Calculate the cotangent of Field of View * 2.0f
    const float fYScale = 1.0f / get_tangent(fFieldOfViewY * 0.5f);
    const float fDepth = (fNear - fFar);

    x.x = fYScale / fAspect;
    x.y = 0.0f;
    x.z = 0.0f;
    x.w = 0.0f;

    y.x = 0.0f;
    y.y = fYScale;
    y.z = 0.0f;
    y.w = 0.0f;

    z.x = 0.0f;
    z.y = 0.0f;
    z.z = fFar / fDepth;
    z.w = -1.0f;

    w.x = 0.0f;
    w.y = 0.0f;
    w.z = (fNear * fFar) / fDepth;
    w.w = 0.0f;
}

/*! ************************************

    \fn Burger::Matrix4D_t::operator const float *() const
    \brief Convert to a const float pointer

    This convenience operator converts the Matrix4D_t into a float pointer to
    pass to other APIs that treat this as an array of 32 bit floats.

    \return The pointer to the object typecast as a (const float *)

***************************************/

/** Constant 4x4 identity matrix */

const Burger::Matrix4D_t Burger::s_Matrix4DIdentity = {{1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
