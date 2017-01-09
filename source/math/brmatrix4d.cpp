/***************************************

	4D Floating point matrix manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmatrix4d.h"
#include "brfixedmatrix4d.h"

/*! ************************************

	\struct Burger::Matrix4D_t
	\brief 4 dimensional floating point matrix

	This 64 byte matrix contains x,y,z and w 32 bit floating point rows and columns. A
	set of common functions for simple 4 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y", "z" and "w" for maximum compatibility

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::Vector4D_t, Burger::Matrix3D_t or Burger::FixedMatrix4D_t
	
***************************************/

/*! ************************************

	\brief Clear out a 4D matrix

	Set all of the entries to zero

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	</table>

	\sa Identity(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Zero(void)
{
	float fTemp = 0.0f;		// Force the compiler to use a register
	x.x = fTemp;		// Fill all the elements with zero
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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>1</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\sa Zero(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Identity(void)
{
	x.x = 1.0f;	// Leave X alone
	x.y = 0.0f;
	x.z = 0.0f;
	x.w = 0.0f;
	y.x = 0.0f;
	y.y = 1.0f;	// Leave Y alone
	y.z = 0.0f;
	y.w = 0.0f;
	z.x = 0.0f;
	z.y = 0.0f;
	z.z = 1.0f;	// Leave Z alone
	z.w = 0.0f;
	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;	
	w.w = 1.0f;	// Leave W alone
}

/*! ************************************

	\brief Copy the matrix

	Copy a matrix into this one

	\param pInput Pointer to a valid Matrix4D_t for copying
	\sa Identity() or Zero()

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(const Matrix4D_t *pInput)
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

	Using FixedToFloat(float *,Fixed32), convert all of the entries
	from a 4x4 16.16 fixed point matrix into a floating point matrix

	\param pInput Pointer to a valid FixedMatrix4D_t for conversion
	\sa FixedToFloat(float *,Fixed32)

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(const FixedMatrix4D_t *pInput)
{
	FixedToFloat(&x.x,&pInput->x.x);
	FixedToFloat(&x.y,&pInput->x.y);
	FixedToFloat(&x.z,&pInput->x.z);
	FixedToFloat(&x.w,&pInput->x.w);
	FixedToFloat(&y.x,&pInput->y.x);
	FixedToFloat(&y.y,&pInput->y.y);
	FixedToFloat(&y.z,&pInput->y.z);
	FixedToFloat(&y.w,&pInput->y.w);
	FixedToFloat(&z.x,&pInput->z.x);
	FixedToFloat(&z.y,&pInput->z.y);
	FixedToFloat(&z.z,&pInput->z.z);
	FixedToFloat(&z.w,&pInput->z.w);
	FixedToFloat(&w.x,&pInput->w.x);
	FixedToFloat(&w.y,&pInput->w.y);
	FixedToFloat(&w.z,&pInput->w.z);
	FixedToFloat(&w.w,&pInput->w.w);
}

/*! ************************************

	\brief Copy a Matrix3D_t to a Matrix4D_t

	Copy a 3x3 matrix into this one.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>pInput->x.x</td>	<td>pInput->x.y</td>	<td>pInput->x.z</td>	<td>0</td></tr>
	<tr><th>y</th><td>pInput->y.x</td>	<td>pInput->y.y</td>	<td>pInput->y.z</td>	<td>0</td></tr>
	<tr><th>z</th><td>pInput->z.x</td>	<td>pInput->z.y</td>	<td>pInput->z.z</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param pInput Pointer to a valid Matrix3D_t for copying
	\sa Set(const Matrix4D_t *) or Matrix3D_t::Set(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Set(const Matrix3D_t *pInput)
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
	float sy = Sin(fYaw);
	float cy = Cos(fYaw);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy</td>	<td>0</td>	<td>-sy</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</th><td>sy</td>	<td>0</td>	<td>cy</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\sa TransposeSetYaw(float), Set(float,float,float), SetPitch(float) and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYaw(float fYaw)
{
	float fSY = Sin(fYaw);		// Get the sine/cosine
	float fCY = Cos(fYaw);

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
	float sx = Sin(fPitch);
	float cx = Cos(fPitch);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>cx</td>	<td>-sx</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>sx</td><td>cx</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fPitch Pitch of the X in radians
	\sa TransposeSetPitch(float), Set(float,float,float), SetYaw(float) and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetPitch(float fPitch)
{
	float fSX = Sin(fPitch);		// Get the sine/cosine
	float fCX = Cos(fPitch);

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
	float sz = Sin(fRoll);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cz</td>	<td>-sz</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>sz</td>	<td>cz</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fRoll Roll of the Z in radians
	\sa TransposeSetRoll(float), Set(float,float,float), SetYaw(float) and SetPitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetRoll(float fRoll)
{
	float fSZ = Sin(fRoll);		// Get the sine/cosine
	float fCZ = Cos(fRoll);

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
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of Y, X and
	then Z.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(cy*cz)+(sy*-sx*-sz)</td><td>cx*-sz</td><td>(-sy*cz)+(cy*-sx*-sz)</td><td>0</td></tr>
	<tr><th>y</th><td>(sy*-sx*cz)+(cy*sz)</td><td>cx*cz</td><td>(cy*-sx*cz)+(-sy*sz)</td><td>0</td></tr>
	<tr><th>z</th><td>sy*cx</td><td>sx</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetYXZ(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYXZ(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSXSZ = fSX*fSZ;	// sx*sz
	float fNegSXCZ = -fSX*fCZ;	// -sx*cz

	// Code for YXZ
	x.x = (fCY*fCZ)+(fSY*fSXSZ);
	x.y = fCX*-fSZ;
	x.z = (fCY*fSXSZ)-(fSY*fCZ);
	x.w = 0.0f;

	y.x = (fSY*fNegSXCZ)+(fCY*fSZ);
	y.y = fCX*fCZ;
	y.z = (fCY*fNegSXCZ)-(fSY*fSZ);
	y.w = 0.0f;

	z.x = fSY*fCX;
	z.y = fSX;
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a rotation matrix in the order of Y, Z and X
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of Y, Z and
	then X.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>-sz</td><td>-sy*cz</td><td>0</td></tr>
	<tr><th>y</th><td>(cy*sz*cx)+(sy*-sx)</td><td>cx*cz</td><td>(-sy*sz*cx)+(cy*-sx)</td><td>0</td></tr>
	<tr><th>z</th><td>(sy*cx)+(cy*sx*sz)</td><td>sx*cz</td><td>(cy*cx)+(-sy*sx*sz)</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetYZX(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYZX(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSZCX = fSZ*fCX;	// sz*cx
	float fSXSZ = fSX*fSZ;	// sx*sz

	// Code for YZX
	x.x = fCY*fCZ;
	x.y = -fSZ;
	x.z = -fSY*fCZ;
	x.w = 0.0f;

	y.x = (fCY*fSZCX)-(fSY*fSX);
	y.y = fCX*fCZ;
	y.z = (-fSY*fSZCX)-(fCY*fSX);
	y.w = 0.0f;

	z.x = (fSY*fCX)+(fCY*fSXSZ);
	z.y = fSX*fCZ;
	z.z = (fCY*fCX)-(fSY*fSXSZ);
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a rotation matrix in the order of X, Y and Z
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of X, Y and
	then Z.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(-sy*sx*cz)+(cx*-sz)</td><td>(-sy*cx*cz)+(-sx*-sz)</td><td>0</td></tr>
	<tr><th>y</th><td>cy*sz</td><td>(cx*cz)+(-sy*sx*sz)</td><td>(-sx*cx)+(cy*sx*-sz)</td><td>0</td></tr>
	<tr><th>z</th><td>sy</td><td>cy*sx</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetXYZ(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXYZ(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSXSZ = fSX*fSZ;	// sx*sz
	float fNegSYCZ = -fSY*fCZ;	// sy*cz

	// Code for XYZ
	x.x = fCY*fCZ;
	x.y = (fNegSYCZ*fSX)-(fCX*fSZ);
	x.z = (fNegSYCZ*fCX)+(fSX*fSZ);
	x.w = 0.0f;

	y.x = fCY*fSZ;
	y.y = (fCX*fCZ)-(fSY*fSXSZ);
	y.z = (-fSX*fCX)-(fCY*fSXSZ);
	y.w = 0.0f;

	z.x = fSY;
	z.y = fCY*fSX;
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a rotation matrix in the order of X, Z and Y
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of X, Z and
	then Y.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(cx*-sz*cy)+(-sy*sx)</td><td>(-sx*-sz*cy)+(-sy*cx)</td><td>0</td></tr>
	<tr><th>y</th><td>sz</td><td>cx*cz</td><td>-sx*cz</td><td>0</td></tr>
	<tr><th>z</th><td>sy*cz</td><td>(sx*cy)+(sy*cx*-sz)</td><td>(cx*cy)+(sy*-sx*-sz)</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetXZY(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXZY(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSZCY = fSZ*fCY;	// sz*cy
	float fSYSZ = fSY*fSZ;	// sy*sz

	// Code for XZY
	x.x = fCY*fCZ;
	x.y = (fCX*-fSZCY)-(fSY*fSX);
	x.z = (fSX*fSZCY)-(fSY*fCX);
	x.w = 0.0f;

	y.x = fSZ;
	y.y = fCX*fCZ;
	y.z = -fSX*fCZ;
	y.w = 0.0f;

	z.x = fSY*fCZ;
	z.y = (fSX*fCY)-(fSYSZ*fCX);
	z.z = (fCX*fCY)+(fSYSZ*fSX);
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a rotation matrix in the order of Z, Y and X
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of Z, Y and
	then X.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>cy*-sz</td><td>-sy</td><td>0</td></tr>
	<tr><th>y</th><td>(sz*cx)+(sy*-sx*cz)</td><td>(cz*cx)+(sy*-sx*-sz)</td><td>cy*-sx</td><td>0</td></tr>
	<tr><th>z</th><td>(sy*cz*cx)+(sx*sz)</td><td>(sy*-sz*cx)+(sx*cz)</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetZYX(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZYX(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSYSX = fSY*fSX;	// sy*sx
	float fSYCX = fSY*fCX;	// sy*cx

	// Code for ZYX
	x.x = fCY*fCZ;
	x.y = fCY*-fSZ;
	x.z = -fSY;
	x.w = 0.0f;

	y.x = (fSZ*fCX)-(fSYSX*fCZ);
	y.y = (fCZ*fCX)+(fSYSX*fSZ);
	y.z = fCY*-fSX;
	y.w = 0.0f;

	z.x = (fSYCX*fCZ)+(fSX*fSZ);
	z.y = (fSX*fCZ)-(fSYCX*fSZ);
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a rotation matrix in the order of Z, X and Y
	
	Initialize a rotation matrix with radians for
	Yaw (Y), Pitch (X), and Roll (Z) in the order of Z, X and
	then Y.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(cz*cy)+(-sy*sx*sz)</td><td>(-sz*cy)+(-sy*sx*cz)</td><td>-sy*cx</td><td>0</td></tr>
	<tr><th>y</th><td>cx*sz</td><td>cx*cz</td><td>-sx</td><td>0</td></tr>
	<tr><th>z</th><td>(sx*sz*cy)+(sy*cz)</td><td>(sx*cz*cy)+(sy*-sz)</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetZXY(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZXY(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSYSX = fSY*fSX;	// sy*sx
	float fSXCY = fSX*fCY;	// sx*cy

	// Code for ZXY
	x.x = (fCZ*fCY)-(fSYSX*fSZ);
	x.y = (-fSZ*fCY)-(fSYSX*fCZ);
	x.z = -fSY*fCX;
	x.w = 0.0f;

	y.x = fCX*fSZ;
	y.y = fCX*fCZ;
	y.z = -fSX;
	y.w = 0.0f;

	z.x = (fSXCY*fSZ)+(fSY*fCZ);
	z.y = (fSXCY*fCZ)-(fSY*fSZ);
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Convert a quaternion to a 4x4 matrix

	Here's the resulting matrix of math used using terms from the input quaternion

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
		<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
		<tr><th>x</th>	<td>1-(2yy+2zz)</td>	<td>2xy+2wz</td>		<td>2xz-2wy</td>		<td>0</td></tr>
		<tr><th>y</th>	<td>2xy-2wz</td>		<td>1-(2xx+2zz)</td>	<td>2yz+2wx</td>		<td>0</td></tr>
		<tr><th>z</th>	<td>2xz+2wy</td>		<td>2yz-2wx</td>		<td>1-(2xx+2yy)</td>	<td>0</td></tr>
		<tr><th>w</th>	<td>0</td>				<td>0</td>				<td>0</td>				<td>1</td></tr>
	</table>

	\param pInput Pointer to a normalized quaternion
	\sa Matrix3D_t::SetFromQuaternion(const Vector4D_t *pInput) or TransposeSetFromQuaternion(const Vector4D_t *pInput)
	
***************************************/

void BURGER_API Burger::Matrix4D_t::SetFromQuaternion(const Vector4D_t *pInput)
{
	// Create the constants for the conversion
	// First the 2x constants

	float q_2x=pInput->x;	// Perform an addition to fake a multiply by 2.0f
	float q_2y=pInput->y;	// to remove a loading of the 2.0f constant
	float q_2z=pInput->z;
	q_2x+=q_2x;
	q_2y+=q_2y;
	q_2z+=q_2z;

	// Now the matrix multiply constants

	float q_2xw=q_2x*pInput->w;
	float q_2yw=q_2y*pInput->w;
	float q_2zw=q_2z*pInput->w;

	float q_2xx=q_2x*pInput->x;
	float q_2xy=q_2x*pInput->y;
	float q_2yy=q_2y*pInput->y;

	float q_2xz=q_2x*pInput->z;
	float q_2yz=q_2y*pInput->z;
	float q_2zz=q_2z*pInput->z;

	// x.x = 1 - 2y^2 - 2z^2

	x.x=1.0f-q_2yy-q_2zz;

	// x.y = 2xy - 2wz

	x.y=q_2xy-q_2zw;

	// x.z = 2xz + 2wy

	x.z=q_2xz+q_2yw;
	x.w = 0.0f;

	// y.x = 2xy + 2wz

	y.x=q_2xy+q_2zw;

	// y.y = 1 - 2x^2 - 2z^2

	y.y=1.0f-q_2xx-q_2zz;

	// y.z = 2yz - 2wx

	y.z=q_2yz-q_2xw;
	y.w = 0.0f;

	// z.x = 2xz - 2wy

	z.x=q_2xz-q_2yw;

	// z.y = 2yz + 2wx

	z.y=q_2yz+q_2xw;

	// z.z = 1 - 2x^2 - 2y^2

	z.z=1.0f-q_2xx-q_2yy;
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
	float sy = Sin(fYaw);
	float cy = Cos(fYaw);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy</td>	<td>0</td>	<td>sy</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</th><td>-sy</td>	<td>0</td>	<td>cy</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\sa SetYaw(float), TransposeSet(float,float,float), TransposeSetPitch(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYaw(float fYaw)
{
	float fSY = Sin(fYaw);		// Get the sine/cosine
	float fCY = Cos(fYaw);

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
	float sx = Sin(fPitch);
	float cx = Cos(fPitch);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>cx</td>	<td>sx</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>-sx</td><td>cx</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fPitch Pitch of the X in radians
	\sa SetPitch(float), TransposeSet(float,float,float), TransposeSetYaw(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetPitch(float fPitch)
{
	float fSX = Sin(fPitch);		// Get the sine/cosine
	float fCX = Cos(fPitch);

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
	float sz = Sin(fRoll);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cz</td>	<td>sz</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>-sz</td>	<td>cz</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>w</th><td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fRoll Roll of the Z in radians
	\sa SetRoll(float), TransposeSet(float,float,float), TransposeSetYaw(float) and TransposeSetPitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetRoll(float fRoll)
{
	float fSZ = Sin(fRoll);		// Get the sine/cosine
	float fCZ = Cos(fRoll);

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
	
	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of Y, X and then Z.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(cy*cz)+(-sy*sx*sz)</td><td>cx*sz</td><td>(sy*cz)+(cy*sx*sz)</td><td>0</td></tr>
	<tr><th>y</th><td>(-sy*sx*cz)+(cy*-sz)</td><td>cx*cz</td><td>(cy*sx*cz)+(sy*-sz)</td><td>0</td></tr>
	<tr><th>z</th><td>-sy*cx</td><td>-sx</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetYXZ(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYXZ(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSXSZ = fSX*fSZ;	// sx*sz
	float fSXCZ = fSX*fCZ;	// sx*cz

	// Code for YXZ
	x.x = (fCY*fCZ)-(fSY*fSXSZ);
	x.y = fCX*fSZ;
	x.z = (fSY*fCZ)+(fCY*fSXSZ);
	x.w = 0.0f;

	y.x = (-fSY*fSXCZ)-(fCY*fSZ);
	y.y = fCX*fCZ;
	y.z = (fCY*fSXCZ)-(fSY*fSZ);
	y.w = 0.0f;

	z.x = -fSY*fCX;
	z.y = -fSX;
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a transposed rotation matrix in the order of Y, Z and X.
	
	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of Y, Z and then X.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>sz</td><td>sy*cz</td><td>0</td></tr>
	<tr><th>y</th><td>(cy*-sz*cx)+(-sy*sx)</td><td>cx*cz</td><td>(sy*-sz*cx)+(cy*sx)</td><td>0</td></tr>
	<tr><th>z</th><td>(-sy*cx)+(cy*-sx*-sz)</td><td>-sx*cz</td><td>(cy*cx)+(sy*-sx*-sz)</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetYZX(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetYZX(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSXSZ = fSX*fSZ;		// sx*sz
	float fNegSZCX = -fSZ*fCX;	// -sz*cx

	// Code for YZX
	x.x = fCY*fCZ;
	x.y = fSZ;
	x.z = fSY*fCZ;
	x.w = 0.0f;

	y.x = (fCY*fNegSZCX)-(fSY*fSX);
	y.y = fCX*fCZ;
	y.z = (fSY*fNegSZCX)+(fCY*fSX);
	y.w = 0.0f;

	z.x = (fCY*fSXSZ)-(fSY*fCX);
	z.y = -fSX*fCZ;
	z.z = (fCY*fCX)+(fSY*fSXSZ);
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a transposed rotation matrix in the order of X, Y and Z.
	
	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of X, Y and then Z.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(sy*-sx*cz)+(cx*sz)</td><td>(sy*cx*cz)+(sx*sz)</td><td>0</td></tr>
	<tr><th>y</th><td>cy*-sz</td><td>(cx*cz)+(sy*-sx*-sz)</td><td>(sx*cz)+(sy*cx*-sz)</td><td>0</td></tr>
	<tr><th>z</th><td>-sy</td><td>cy*-sx</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetXYZ(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetXYZ(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSYSX = fSY*fSX;	// sy*sx
	float fSYCX = fSY*fCX;	// sy*cx

	// Code for XYZ
	x.x = fCY*fCZ;
	x.y = (fCX*fSZ)-(fSYSX*fCZ);
	x.z = (fSYCX*fCZ)+(fSX*fSZ);
	x.w = 0.0f;

	y.x = fCY*-fSZ;
	y.y = (fCX*fCZ)+(fSYSX*fSZ);
	y.z = (fSX*fCZ)-(fSYCX*fSZ);
	y.w = 0.0f;

	z.x = -fSY;
	z.y = fCY*-fSX;
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a transposed rotation matrix in the order of X, Z and Y.
	
	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of X, Z and then Y.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(cx*sz*cy)+(sy*-sx)</td><td>(sx*sz*cy)+(sy*cx)</td><td>0</td></tr>
	<tr><th>y</th><td>-sz</td><td>cx*cz</td><td>sx*cz</td><td>0</td></tr>
	<tr><th>z</th><td>-sy*cz</td><td>(-sx*cy)+(-sy*cx*sz)</td><td>(cx*cy)+(-sy*sx*sz)</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetXZY(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetXZY(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSZCY = fSZ*fCY;	// sz*cy
	float fSYSZ = fSY*fSZ;	// sy*sz

	// Code for XZY
	x.x = fCY*fCZ;
	x.y = (fCX*fSZCY)-(fSY*fSX);
	x.z = (fSX*fSZCY)+(fSY*fCX);
	x.w = 0.0f;

	y.x = -fSZ;
	y.y = fCX*fCZ;
	y.z = fSX*fCZ;
	y.w = 0.0f;

	z.x = -fSY*fCZ;
	z.y = (-fSX*fCY)-(fSYSZ*fCX);
	z.z = (fCX*fCY)-(fSYSZ*fSX);
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a transposed rotation matrix in the order of Z, Y and X.
	
	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of Z, Y and then X.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>cy*sz</td><td>sy</td><td>0</td></tr>
	<tr><th>y</th><td>(-sz*cx)+(-sy*sx*cz)</td><td>(cz*cx)+(-sy*sx*sz)</td><td>cy*sx</td><td>0</td></tr>
	<tr><th>z</th><td>(-sy*cz*cx)+(-sx*-sz)</td><td>(-sy*sz*cx)+(-sx*cz)</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetZYX(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetZYX(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSYSX = fSY*fSX;	// sy*sx
	float fNegSYCX = -fSY*fCX;	// -sy*cx

	// Code for ZYX
	x.x = fCY*fCZ;
	x.y = fCY*fSZ;
	x.z = fSY;
	x.w = 0.0f;

	y.x = (-fSZ*fCX)-(fSYSX*fCZ);
	y.y = (fCZ*fCX)-(fSYSX*fSZ);
	y.z = fCY*fSX;
	y.w = 0.0f;

	z.x = (fSX*fSZ)+(fNegSYCX*fCZ);
	z.y = (fNegSYCX*fSZ)-(fSX*fCZ);
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Initialize a transposed rotation matrix in the order of Z, X and Y.

	Initialize a transposed rotation matrix with radians for Yaw (Y), 
	Pitch (X), and Roll (Z) in the order of Z, X and then Y.

	\code
	float sx = Sin(fPitch);
	float sy = Sin(fYaw);
	float sz = Sin(fRoll);
	float cx = Cos(fPitch);
	float cy = Cos(fYaw);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(cz*cy)+(sy*-sx*-sz)</td><td>(sz*cy)+(sy*-sx*cz)</td><td>sy*cx</td><td>0</td></tr>
	<tr><th>y</th><td>cx*-sz</td><td>cx*cz</td><td>sx</td><td>0</td></tr>
	<tr><th>z</th><td>(-sx*-sz*cy)+(-sy*cz)</td><td>(-sx*cz*cy)+(-sy*sz)</td><td>cy*cx</td><td>0</td></tr>
	<tr><th>w</th><td>0</td><td>0</td><td>0</td><td>1</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetZXY(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetZXY(float fYaw,float fPitch,float fRoll)
{
	// Create the sines and cosines
	float fSX = Sin(fPitch);
	float fSY = Sin(fYaw);
	float fSZ = Sin(fRoll);
	float fCX = Cos(fPitch);
	float fCY = Cos(fYaw);
	float fCZ = Cos(fRoll);

	// Cache this value
	float fSXSZ = fSX*fSZ;	// sx*sz
	float fSXCZ = fSX*fCZ;	// sx*cz

	// Code for ZXY
	x.x = (fCZ*fCY)+(fSXSZ*fSY);
	x.y = (fSZ*fCY)-(fSXCZ*fSY);
	x.z = fSY*fCX;
	x.w = 0.0f;

	y.x = fCX*-fSZ;
	y.y = fCX*fCZ;
	y.z = fSX;
	y.w = 0.0f;

	z.x = (fSXSZ*fCY)-(fSY*fCZ);
	z.y = (-fSZ*fSY)-(fSXCZ*fCY);
	z.z = fCY*fCX;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Convert a quaternion to a transposed 4x4 matrix

	Here's the resulting matrix of math used using terms from the input quaternion

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
		<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
		<tr><th>x</th>	<td>1-(2yy+2zz)</td>	<td>2xy-2wz</td>		<td>2xz+2wy</td>		<td>0</td></tr>
		<tr><th>y</th>	<td>2xy+2wz</td>		<td>1-(2xx+2zz)</td>	<td>2yz-2wx</td>		<td>0</td></tr>
		<tr><th>z</th>	<td>2xz-2wy</td>		<td>2yz+2wx</td>		<td>1-(2xx+2yy)</td>	<td>0</td></tr>
		<tr><th>w</th>	<td>0</td>				<td>0</td>				<td>0</td>				<td>1</td></tr>
	</table>

	\param pInput Pointer to a normalized quaternion
	\sa Matrix3D_t::TransposeSetFromQuaternion(const Vector4D_t *pInput) or SetFromQuaternion(const Vector4D_t *pInput)
	
***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetFromQuaternion(const Vector4D_t *pInput)
{
	// Create the constants for the conversion
	// First the 2x constants

	float q_2x=pInput->x;	// Perform an addition to fake a multiply by 2.0f
	float q_2y=pInput->y;	// to remove a loading of the 2.0f constant
	float q_2z=pInput->z;
	q_2x+=q_2x;
	q_2y+=q_2y;
	q_2z+=q_2z;

	// Now the matrix multiply constants

	float q_2xw=q_2x*pInput->w;
	float q_2yw=q_2y*pInput->w;
	float q_2zw=q_2z*pInput->w;

	float q_2xx=q_2x*pInput->x;
	float q_2xy=q_2x*pInput->y;
	float q_2yy=q_2y*pInput->y;

	float q_2xz=q_2x*pInput->z;
	float q_2yz=q_2y*pInput->z;
	float q_2zz=q_2z*pInput->z;

	// x.x = 1 - 2y^2 - 2z^2

	x.x=1.0f-q_2yy-q_2zz;

	// x.y = 2xy - 2wz

	y.x=q_2xy-q_2zw;

	// x.z = 2xz + 2wy

	z.x=q_2xz+q_2yw;
	w.x = 0.0f;

	// y.x = 2xy + 2wz

	x.y=q_2xy+q_2zw;

	// y.y = 1 - 2x^2 - 2z^2

	y.y=1.0f-q_2xx-q_2zz;

	// y.z = 2yz - 2wx

	z.y=q_2yz-q_2xw;
	w.y = 0.0f;

	// z.x = 2xz - 2wy

	x.z=q_2xz-q_2yw;

	// z.y = 2yz + 2wx

	y.z=q_2yz+q_2xw;

	// z.z = 1 - 2x^2 - 2y^2

	z.z=1.0f-q_2xx-q_2yy;
	w.z = 0.0f;

	// Scale is 1.0f
	x.w = 0.0f;
	y.w = 0.0f;
	z.w = 0.0f;
	w.w = 1.0f;
}

/*! ************************************

	\brief Create a 4D translation matrix

	Sets the w row to fX, fY and fZ and the rest of
	the values to that of an identity matrix.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>1</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>w</td>	<td>fX</td>	<td>fY</td>	<td>fZ</td>	<td>1</td></tr>
	</table>

	\param fX new w.x component
	\param fY new w.y component
	\param fZ new w.z component
	\sa Identity(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetTranslate(float fX,float fY,float fZ)
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

	Sets the x.x, y.y and z.z components to the input values, w.w
	is set to 1.0f and all others are set to 0.0f

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>fX</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>fY</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>fZ</td>	<td>0</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fX new x.x component
	\param fY new y.y component
	\param fZ new z.z component
	\sa Identity(void) and SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetScale(float fX,float fY,float fZ)
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

	Sets the x.x, y.y, z.z and w.w components to the input values
	and all others are set to 0.0f

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>fX</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>fY</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>fZ</td>	<td>0</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>fW</td></tr>
	</table>

	\param fX new x.x component
	\param fY new y.y component
	\param fZ new z.z component
	\param fW new w.w component
	\sa Identity(void) and SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::SetScale(float fX,float fY,float fZ,float fW)
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

/*! ************************************

	\brief Create a transposed 4D projection matrix

	Using a frustum, create a projection matrix in a format that
	OpenGL can use directly. If a DirectX version is desired, call TransposeSetFrustum() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>(fNear * 2.0f) / (fRight - fLeft)</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>(fNear * 2.0f) / (fTop - fBottom)</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>(fRight + fLeft) / (fRight - fLeft)</td>	<td>(fTop + fBottom) / (fTop - fBottom)</td>	<td>-(fFar+fNear) / (fFar - fNear)</td>	<td>-1</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>-(2.0f * fFar * fNear) / (fFar - fNear)</td>	<td>0</td></tr>
	</table>

	\note This is functional duplicate of glFrustum()

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa SetOrtho(), SetPerspective() or TransposeSetFrustum()

***************************************/

void BURGER_API Burger::Matrix4D_t::SetFrustum(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
{
	float fWidth = fRight-fLeft;
	float fHeight = fTop-fBottom;
	float fNegativeDepth = -(fFar-fNear);
	float fNear2 = fNear * 2.0f;

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
	z.z = (fFar+fNear) / fNegativeDepth;
	z.w = -1.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = (2.0f * fFar * fNear) / fNegativeDepth;
	w.w = 0.0f;
}

/*! ************************************

	\brief Create a 4D projection matrix

	Using a frustum, create a projection matrix in a format that
	DirectX can use directly. If a OpenGL version is desired, call SetFrustum() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>(fNear * 2.0f) / (fRight - fLeft)</td>	<td>0</td>	<td>(fRight + fLeft) / (fRight - fLeft)</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>(fNear * 2.0f) / (fTop - fBottom)</td>	<td>(fTop + fBottom) / (fTop - fBottom)</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>-(fFar+fNear) / (fFar - fNear)</td>	<td>-(2.0f * fFar * fNear) / (fFar - fNear)</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>-1</td>	<td>0</td></tr>
	</table>

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa TransposeSetOrtho(), TransposeSetPerspective() or SetFrustum()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetFrustum(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
{
	float fWidth = fRight-fLeft;
	float fHeight = fTop-fBottom;
	float fNegativeDepth = -(fFar-fNear);
	float fNear2 = fNear * 2.0f;

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
	z.z = (fFar+fNear) / fNegativeDepth;
	z.w = (2.0f * fFar * fNear) / fNegativeDepth;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = -1.0f;
	w.w = 0.0f;
}


/*! ************************************

	\brief Create a transposed orthogonal 4D projection matrix

	Using a frustum, create an orthogonal matrix in a format that
	OpenGL can use directly. If a DirectX version is desired, call TransposeSetOrtho() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>2.0f / (fRight - fLeft)</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>2.0f / (fTop - fBottom)</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>-2.0f / (fFar - fNear)</td>	<td></td>0</tr>
	<tr><th>w</td>	<td>-(fRight + fLeft) / (fRight - fLeft)</td>	<td>-(fTop + fBottom) / (fTop - fBottom)</td>	<td>-(Far + fNear) / (fFar - fNear)</td>	<td>1</td></tr>
	</table>

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa SetFrustum() or TransposeSetOrtho()

***************************************/

void BURGER_API Burger::Matrix4D_t::SetOrtho(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
{
	float fWidth = fRight-fLeft;
	float fHeight = fTop-fBottom;
	float fNegativeDepth = -(fFar-fNear);

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

	Using a frustum, create an orthogonal matrix in a format that
	DirectX can use directly. If an OpenGL version is desired, call SetOrtho() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>2.0f / (fRight - fLeft)</td>	<td>0</td>	<td>0</td>	<td>-(fRight + fLeft) / (fRight - fLeft)</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>2.0f / (fTop - fBottom)</td>	<td>0</td>	<td>-(fTop + fBottom) / (fTop - fBottom)</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>-2.0f / (fFar - fNear)</td>	<td>-(Far + fNear) / (fFar - fNear)</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\note This is functional duplicate of glOrtho()

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa TransposeSetFrustum() or SetOrtho()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetOrtho(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
{
	float fWidth = fRight-fLeft;
	float fHeight = fTop-fBottom;
	float fNegativeDepth = -(fFar-fNear);

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

	Using a field of view and an aspect ratio (x/y), create a projection matrix in a format that
	OpenGL can use directly. If a DirectX version is desired, call TransposeSetPerspective() instead.

	f = 1.0f / tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>f/fAspect</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>f</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>(fFar+fNear) / (fNear-fFar)</td>	<td>-1</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>(2.0f * fFar * fNear) / (fNear-fFar)</td>	<td>0</td></tr>
	</table>

	\note This is functional duplicate of gluPerspective()

	\param fFieldOfViewY Angle of the field of view in degrees
	\param fAspect Aspect ratio of the screen
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa SetOrtho(), SetFrustum() or TransposeSetPerspective()

***************************************/

void BURGER_API Burger::Matrix4D_t::SetPerspective(float fFieldOfViewY,float fAspect,float fNear,float fFar)
{
	// Calculate the cotangent of Field of View * 2.0f
	float f = 1.0f / Tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

	float fDepth = (fNear-fFar);

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
	z.z = (fFar+fNear) / fDepth;
	z.w = -1.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = (2.0f * fFar * fNear) / fDepth;
	w.w = 0.0f;
}

/*! ************************************

	\brief Create a 4D perspective matrix

	Using a field of view and an aspect ratio (x/y), create a projection matrix in a format that
	DirectX can use directly. If an OpenGL version is desired, call SetPerspective() instead.

	f = 1.0f / tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>f/fAspect</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>f</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>(fFar+fNear) / (fNear-fFar)</td>	<td>(2.0f * fFar * fNear) / (fNear-fFar)</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>-1</td>	<td>0</td></tr>
	</table>

	\param fFieldOfViewY Angle of the field of view in degrees
	\param fAspect Aspect ratio of the screen
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa TransposeSetOrtho(), TransposeSetFrustum() or SetPerspective()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetPerspective(float fFieldOfViewY,float fAspect,float fNear,float fFar)
{
	// Calculate the cotangent of Field of View * 2.0f
	float f = 1.0f / Tan((fFieldOfViewY*g_fDegreesToRadians)*0.5f);

	float fDepth = (fNear-fFar);

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
	z.z = (fFar+fNear) / fDepth;
	z.w = (2.0f * fFar * fNear) / fDepth;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = -1.0f;
	w.w = 0.0f;
}

/*! ************************************

	\brief Perform a matrix transposition.
	
	Swap the entries to convert a right handed matrix to a left handed
	one and vice versa.
	The matrix is assumed to be already initialized.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>xx</td><td>yx</td><td>zx</td><td>wx</td></tr>
	<tr><th>y</td>	<td>xy</td><td>yy</td><td>zy</td><td>wy</td></tr>
	<tr><th>z</td>	<td>xz</td><td>yz</td><td>zz</td><td>wz</td></tr>
	<tr><th>w</td>	<td>xw</td><td>yw</td><td>zw</td><td>ww</td></tr>
	</table>

	\sa Transpose(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Transpose(void)
{
	float fTemp1 = x.y;	// Swap x.y and y.x
	float fTemp2 = y.x;
	x.y = fTemp2;
	y.x = fTemp1;

	fTemp1 = x.z;		// Swap x.z and z.x
	fTemp2 = z.x;
	x.z = fTemp2;
	z.x = fTemp1;

	fTemp1 = x.w;		// Swap x.w and w.x
	fTemp2 = w.x;
	x.w = fTemp2;
	w.x = fTemp1;

	fTemp1 = y.z;		// Swap y.z and z.y
	fTemp2 = z.y;
	y.z = fTemp2;
	z.y = fTemp1;

	fTemp1 = y.w;		// Swap y.w and w.y
	fTemp2 = w.y;
	y.w = fTemp2;
	w.y = fTemp1;

	fTemp1 = z.w;		// Swap z.w and w.z
	fTemp2 = w.z;
	z.w = fTemp2;
	w.z = fTemp1;
}

/*! ************************************

	\brief Perform a matrix transposition.
	
	Swap the entries to convert a right handed matrix to a left handed
	one and vice versa. The other three entries are copied.
	The destination matrix is assumed to be uninitialized.
	
	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>xx</td><td>yx</td><td>zx</td><td>wx</td></tr>
	<tr><th>y</td>	<td>xy</td><td>yy</td><td>zy</td><td>wy</td></tr>
	<tr><th>z</td>	<td>xz</td><td>yz</td><td>zz</td><td>wz</td></tr>
	<tr><th>w</td>	<td>xw</td><td>yw</td><td>zw</td><td>ww</td></tr>
	</table>

	\note This function is optimized to write the new matrix in
	sequential order in the event this function is writing
	to write combined memory.

	\sa Transpose(void)

***************************************/

void BURGER_API Burger::Matrix4D_t::Transpose(const Matrix4D_t *pInput)
{
	float fTemp1 = pInput->x.x;	// I am copying the matrix, this 
	float fTemp2 = pInput->y.x;	// is why I am copying the x.x, y.y
	x.x = fTemp1;	// and z.z entries
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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xx</td></tr>
	<tr><th>y</th><td>xy</td></tr>
	<tr><th>z</th><td>xz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXRow(Vector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = x.y;
	pOutput->z = x.z;
}

/*! ************************************

	\brief Return the X row from a matrix
	
	Copy the x row of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xx</td></tr>
	<tr><th>y</th><td>xy</td></tr>
	<tr><th>z</th><td>xz</td></tr>
	<tr><th>w</th><td>xw</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXRow(Vector4D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = x.y;
	pOutput->z = x.z;
	pOutput->w = x.w;
}

/*! ************************************

	\brief Return the Y row from a matrix
	
	Copy the y row of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>yx</td></tr>
	<tr><th>y</th><td>yy</td></tr>
	<tr><th>z</th><td>yz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYRow(Vector3D_t *pOutput) const
{
	pOutput->x = y.x;
	pOutput->y = y.y;
	pOutput->z = y.z;
}

/*! ************************************

	\brief Return the Y row from a matrix
	
	Copy the y row of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>yx</td></tr>
	<tr><th>y</th><td>yy</td></tr>
	<tr><th>z</th><td>yz</td></tr>
	<tr><th>w</th><td>yw</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYRow(Vector4D_t *pOutput) const
{
	pOutput->x = y.x;
	pOutput->y = y.y;
	pOutput->z = y.z;
	pOutput->w = y.w;
}

/*! ************************************

	\brief Return the Z row from a matrix
	
	Copy the z row of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>zx</td></tr>
	<tr><th>y</th><td>zy</td></tr>
	<tr><th>z</th><td>zz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZRow(Vector3D_t *pOutput) const
{
	pOutput->x = z.x;
	pOutput->y = z.y;
	pOutput->z = z.z;
}

/*! ************************************

	\brief Return the Z row from a matrix
	
	Copy the z row of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>zx</td></tr>
	<tr><th>y</th><td>zy</td></tr>
	<tr><th>z</th><td>zz</td></tr>
	<tr><th>w</th><td>zw</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZRow(Vector4D_t *pOutput) const
{
	pOutput->x = z.x;
	pOutput->y = z.y;
	pOutput->z = z.z;
	pOutput->w = z.w;
}

/*! ************************************

	\brief Return the W row from a matrix
	
	Copy the w row of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>wx</td></tr>
	<tr><th>y</th><td>wy</td></tr>
	<tr><th>z</th><td>wz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWRow(Vector3D_t *pOutput) const
{
	pOutput->x = w.x;
	pOutput->y = w.y;
	pOutput->z = w.z;
}

/*! ************************************

	\brief Return the W row from a matrix
	
	Copy the w row of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>wx</td></tr>
	<tr><th>y</th><td>wy</td></tr>
	<tr><th>z</th><td>wz</td></tr>
	<tr><th>w</th><td>ww</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWRow(Vector4D_t *pOutput) const
{
	pOutput->x = w.x;
	pOutput->y = w.y;
	pOutput->z = w.z;
	pOutput->w = w.w;
}

/*! ************************************

	\brief Return the X column from a matrix
	
	Copy the x column of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xx</td></tr>
	<tr><th>y</th><td>yx</td></tr>
	<tr><th>z</th><td>zx</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = y.x;
	pOutput->z = z.x;
}

/*! ************************************

	\brief Return the X column from a matrix
	
	Copy the x column of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xx</td></tr>
	<tr><th>y</th><td>yx</td></tr>
	<tr><th>z</th><td>zx</td></tr>
	<tr><th>w</th><td>wx</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetXColumn(Vector4D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = y.x;
	pOutput->z = z.x;
	pOutput->w = w.x;
}

/*! ************************************

	\brief Return the Y column from a matrix
	
	Copy the y column of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xy</td></tr>
	<tr><th>y</th><td>yy</td></tr>
	<tr><th>z</th><td>zy</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.y;
	pOutput->y = y.y;
	pOutput->z = z.y;
}

/*! ************************************

	\brief Return the Y column from a matrix
	
	Copy the y column of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xy</td></tr>
	<tr><th>y</th><td>yy</td></tr>
	<tr><th>z</th><td>zy</td></tr>
	<tr><th>w</th><td>wy</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetYColumn(Vector4D_t *pOutput) const
{
	pOutput->x = x.y;
	pOutput->y = y.y;
	pOutput->z = z.y;
	pOutput->w = w.y;
}

/*! ************************************

	\brief Return the Z column from a matrix
	
	Copy the z column of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xz</td></tr>
	<tr><th>y</th><td>yz</td></tr>
	<tr><th>z</th><td>zz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.z;
	pOutput->y = y.z;
	pOutput->z = z.z;
}

/*! ************************************

	\brief Return the Z column from a matrix
	
	Copy the z column of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xz</td></tr>
	<tr><th>y</th><td>yz</td></tr>
	<tr><th>z</th><td>zz</td></tr>
	<tr><th>w</th><td>wz</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetZColumn(Vector4D_t *pOutput) const
{
	pOutput->x = x.z;
	pOutput->y = y.z;
	pOutput->z = z.z;
	pOutput->w = w.z;
}

/*! ************************************

	\brief Return the W column from a matrix
	
	Copy the w column of a matrix into a user supplied \ref Vector3D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xw</td></tr>
	<tr><th>y</th><td>yw</td></tr>
	<tr><th>z</th><td>zw</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.w;
	pOutput->y = y.w;
	pOutput->z = z.w;
}

/*! ************************************

	\brief Return the W column from a matrix
	
	Copy the w column of a matrix into a user supplied \ref Vector4D_t
	buffer.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>xw</td></tr>
	<tr><th>y</th><td>yw</td></tr>
	<tr><th>z</th><td>zw</td></tr>
	<tr><th>w</th><td>ww</td></tr>
	</table>

	\param pOutput Pointer to an uninitialized Vector4D_t structure to receive the data

***************************************/

void BURGER_API Burger::Matrix4D_t::GetWColumn(Vector4D_t *pOutput) const
{
	pOutput->x = x.w;
	pOutput->y = y.w;
	pOutput->z = z.w;
	pOutput->w = w.w;
}

/*! ************************************

	\brief Set the X row of a matrix
	
	Overwrite the X row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new X row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXRow(const Vector3D_t *pInput)
{
	x.x = pInput->x;
	x.y = pInput->y;
	x.z = pInput->z;
}

/*! ************************************

	\brief Set the X row of a matrix
	
	Overwrite the X row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>pInput->w</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new X row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXRow(const Vector4D_t *pInput)
{
	x.x = pInput->x;
	x.y = pInput->y;
	x.z = pInput->z;
	x.w = pInput->w;
}

/*! ************************************

	\brief Set the Y row of a matrix
	
	Overwrite the Y row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Y row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYRow(const Vector3D_t *pInput)
{
	y.x = pInput->x;
	y.y = pInput->y;
	y.z = pInput->z;
}

/*! ************************************

	\brief Set the Y row of a matrix
	
	Overwrite the Y row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>pInput->w</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new Y row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYRow(const Vector4D_t *pInput)
{
	y.x = pInput->x;
	y.y = pInput->y;
	y.z = pInput->z;
	y.w = pInput->w;
}

/*! ************************************

	\brief Set the Z row of a matrix

	Overwrite the Z row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Z row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZRow(const Vector3D_t *pInput)
{
	z.x = pInput->x;
	z.y = pInput->y;
	z.z = pInput->z;
}

/*! ************************************

	\brief Set the Z row of a matrix

	Overwrite the Z row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>pInput->w</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new Z row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZRow(const Vector4D_t *pInput)
{
	z.x = pInput->x;
	z.y = pInput->y;
	z.z = pInput->z;
	z.w = pInput->w;
}

/*! ************************************

	\brief Set the W row of a matrix

	Overwrite the W row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new W row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWRow(const Vector3D_t *pInput)
{
	w.x = pInput->x;
	w.y = pInput->y;
	w.z = pInput->z;
}

/*! ************************************

	\brief Set the W row of a matrix

	Overwrite the W row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td><td>pInput->w</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new W row

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWRow(const Vector4D_t *pInput)
{
	w.x = pInput->x;
	w.y = pInput->y;
	w.z = pInput->z;
	w.w = pInput->w;
}

/*! ************************************

	\brief Set the X column of a matrix
	
	Overwrite the X column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>pInput->y</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>pInput->z</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>			<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new X column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXColumn(const Vector3D_t *pInput)
{
	x.x = pInput->x;
	y.x = pInput->y;
	z.x = pInput->z;
}

/*! ************************************

	\brief Set the X column of a matrix
	
	Overwrite the X column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>xy</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>pInput->y</td>	<td>yy</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>pInput->z</td>	<td>zy</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>pInput->w</td>	<td>wy</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new X column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetXColumn(const Vector4D_t *pInput)
{
	x.x = pInput->x;
	y.x = pInput->y;
	z.x = pInput->z;
	w.x = pInput->w;
}

/*! ************************************

	\brief Set the Y column of a matrix
	
	Overwrite the Y column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>pInput->x</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>pInput->y</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>pInput->z</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>			<td>wz</td><td>ww</td></tr>
	</table>
	\param pInput Pointer to a Vector3D_t structure that has the new Y column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYColumn(const Vector3D_t *pInput)
{
	x.y = pInput->x;
	y.y = pInput->y;
	z.y = pInput->z;
}

/*! ************************************

	\brief Set the Y column of a matrix
	
	Overwrite the Y column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>pInput->x</td>	<td>xz</td><td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>pInput->y</td>	<td>yz</td><td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>pInput->z</td>	<td>zz</td><td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>pInput->w</td>	<td>wz</td><td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new Y column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetYColumn(const Vector4D_t *pInput)
{
	x.y = pInput->x;
	y.y = pInput->y;
	z.y = pInput->z;
	w.y = pInput->w;
}

/*! ************************************

	\brief Set the Z column of a matrix
	
	Overwrite the Z column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>pInput->x</td>	<td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>pInput->y</td>	<td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>pInput->z</td>	<td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td>			<td>ww</td></tr>
	</table>
	\param pInput Pointer to a Vector3D_t structure that has the new Z column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZColumn(const Vector3D_t *pInput)
{
	x.z = pInput->x;
	y.z = pInput->y;
	z.z = pInput->z;
}

/*! ************************************

	\brief Set the Z column of a matrix
	
	Overwrite the Z column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td><td>pInput->x</td>	<td>xw</td></tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td><td>pInput->y</td>	<td>yw</td></tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td><td>pInput->z</td>	<td>zw</td></tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td><td>pInput->w</td>	<td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new Z column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetZColumn(const Vector4D_t *pInput)
{
	x.z = pInput->x;
	y.z = pInput->y;
	z.z = pInput->z;
	w.z = pInput->w;
}

/*! ************************************

	\brief Set the W column of a matrix
	
	Overwrite the W column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td>	<td>xz</td>	<td>pInput->x</td>	</tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td>	<td>yz</td>	<td>pInput->y</td>	</tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td>	<td>zz</td>	<td>pInput->z</td>	</tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td>	<td>wz</td>	<td>ww</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new W column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWColumn(const Vector3D_t *pInput)
{
	x.w = pInput->x;
	y.w = pInput->y;
	z.w = pInput->z;
}

/*! ************************************

	\brief Set the W column of a matrix
	
	Overwrite the W column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>xx</td>	<td>xy</td><td>xz</td><td>pInput->x</td>	</tr>
	<tr><th>y</th><td>yx</td>	<td>yy</td><td>yz</td><td>pInput->y</td>	</tr>
	<tr><th>z</th><td>zx</td>	<td>zy</td><td>zz</td><td>pInput->z</td>	</tr>
	<tr><th>w</th><td>wx</td>	<td>wy</td><td>wz</td><td>pInput->w</td>	</tr>
	</table>

	\param pInput Pointer to a Vector4D_t structure that has the new W column

***************************************/

void BURGER_API Burger::Matrix4D_t::SetWColumn(const Vector4D_t *pInput)
{
	x.w = pInput->x;
	y.w = pInput->y;
	z.w = pInput->z;
	w.w = pInput->w;
}

/*! ************************************

	\brief Perform a matrix multiply against this matrix

	Multiply this matrix against another one

	Use this formula to create the final matrix, this matrix is
	matrix #1 and the input matrix is matrix #2

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(x.x*pInput->x.x)+(y.x*pInput->x.y)+(z.x*pInput->x.z)+(w.x*pInput->x.w)</td>
	<td>(x.y*pInput->x.x)+(y.y*pInput->x.y)+(z.y*pInput->x.z)+(w.y*pInput->x.w)</td>
	<td>(x.z*pInput->x.x)+(y.z*pInput->x.y)+(z.z*pInput->x.z)+(w.z*pInput->x.w)</td>
	<td>(x.w*pInput->x.x)+(y.w*pInput->x.y)+(z.w*pInput->x.z)+(w.w*pInput->x.w)</td>
	</tr>
	<tr><th>y</th>
	<td>(x.x*pInput->y.x)+(y.x*pInput->y.y)+(z.x*pInput->y.z)+(w.x*pInput->y.w)</td>
	<td>(x.y*pInput->y.x)+(y.y*pInput->y.y)+(z.y*pInput->y.z)+(w.y*pInput->y.w)</td>
	<td>(x.z*pInput->y.x)+(y.z*pInput->y.y)+(z.z*pInput->y.z)+(w.z*pInput->y.w)</td>
	<td>(x.w*pInput->y.x)+(y.w*pInput->y.y)+(z.w*pInput->y.z)+(w.w*pInput->y.w)</td>
	</tr>
	<tr><th>z</th>
	<td>(x.x*pInput->z.x)+(y.x*pInput->z.y)+(z.x*pInput->z.z)+(w.x*pInput->z.w)</td>
	<td>(x.y*pInput->z.x)+(y.y*pInput->z.y)+(z.y*pInput->z.z)+(w.y*pInput->z.w)</td>
	<td>(x.z*pInput->z.x)+(y.z*pInput->z.y)+(z.z*pInput->z.z)+(w.z*pInput->z.w)</td>
	<td>(x.w*pInput->z.x)+(y.w*pInput->z.y)+(z.w*pInput->z.z)+(w.w*pInput->z.w)</td>
	</tr>
	<tr><th>w</th>
	<td>(x.x*pInput->w.x)+(y.x*pInput->w.y)+(z.x*pInput->z.z)+(w.x*pInput->w.w)</td>
	<td>(x.y*pInput->w.x)+(y.y*pInput->w.y)+(z.y*pInput->z.z)+(w.y*pInput->w.w)</td>
	<td>(x.z*pInput->w.x)+(y.z*pInput->w.y)+(z.z*pInput->z.z)+(w.z*pInput->w.w)</td>
	<td>(x.w*pInput->w.x)+(y.w*pInput->w.y)+(z.w*pInput->z.z)+(w.w*pInput->w.w)</td>
	</tr>
	</table>

	\param pInput Matrix to multiply against
	\sa Multiply(const Matrix4D_t *,const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput)
{
	float fXX=(x.x*pInput->x.x)+(y.x*pInput->x.y)+(z.x*pInput->x.z)+(w.x*pInput->x.w);
	float fXY=(x.y*pInput->x.x)+(y.y*pInput->x.y)+(z.y*pInput->x.z)+(w.y*pInput->x.w);
	float fXZ=(x.z*pInput->x.x)+(y.z*pInput->x.y)+(z.z*pInput->x.z)+(w.z*pInput->x.w);
	float fXW=(x.w*pInput->x.x)+(y.w*pInput->x.y)+(z.w*pInput->x.z)+(w.w*pInput->x.w);

	float fYX=(x.x*pInput->y.x)+(y.x*pInput->y.y)+(z.x*pInput->y.z)+(w.x*pInput->y.w);
	float fYY=(x.y*pInput->y.x)+(y.y*pInput->y.y)+(z.y*pInput->y.z)+(w.y*pInput->y.w);
	float fYZ=(x.z*pInput->y.x)+(y.z*pInput->y.y)+(z.z*pInput->y.z)+(w.z*pInput->y.w);
	float fYW=(x.w*pInput->y.x)+(y.w*pInput->y.y)+(z.w*pInput->y.z)+(w.w*pInput->y.w);

	float fZX=(x.x*pInput->z.x)+(y.x*pInput->z.y)+(z.x*pInput->z.z)+(w.x*pInput->z.w);
	float fZY=(x.y*pInput->z.x)+(y.y*pInput->z.y)+(z.y*pInput->z.z)+(w.y*pInput->z.w);
	float fZZ=(x.z*pInput->z.x)+(y.z*pInput->z.y)+(z.z*pInput->z.z)+(w.z*pInput->z.w);
	float fZW=(x.w*pInput->z.x)+(y.w*pInput->z.y)+(z.w*pInput->z.z)+(w.w*pInput->z.w);

	float fWX=(x.x*pInput->w.x)+(y.x*pInput->w.y)+(z.x*pInput->w.z)+(w.x*pInput->w.w);
	float fWY=(x.y*pInput->w.x)+(y.y*pInput->w.y)+(z.y*pInput->w.z)+(w.y*pInput->w.w);
	float fWZ=(x.z*pInput->w.x)+(y.z*pInput->w.y)+(z.z*pInput->w.z)+(w.z*pInput->w.w);
	float fWW=(x.w*pInput->w.x)+(y.w*pInput->w.y)+(z.w*pInput->w.z)+(w.w*pInput->w.w);
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

/*! ************************************

	\brief Perform a matrix multiply

	Multiply two matrices together and store the result in this matrix.

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(pInput1->x.x*pInput2->x.x)+(pInput1->y.x*pInput2->x.y)+(pInput1->z.x*pInput2->x.z)+(pInput1->w.x*pInput2->x.w)</td>
	<td>(pInput1->x.y*pInput2->x.x)+(pInput1->y.y*pInput2->x.y)+(pInput1->z.y*pInput2->x.z)+(pInput1->w.y*pInput2->x.w)</td>
	<td>(pInput1->x.z*pInput2->x.x)+(pInput1->y.z*pInput2->x.y)+(pInput1->z.z*pInput2->x.z)+(pInput1->w.z*pInput2->x.w)</td>
	<td>(pInput1->x.w*pInput2->x.x)+(pInput1->y.w*pInput2->x.y)+(pInput1->z.w*pInput2->x.z)+(pInput1->w.w*pInput2->x.w)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput1->x.x*pInput2->y.x)+(pInput1->y.x*pInput2->y.y)+(pInput1->z.x*pInput2->y.z)+(pInput1->w.x*pInput2->y.w)</td>
	<td>(pInput1->x.y*pInput2->y.x)+(pInput1->y.y*pInput2->y.y)+(pInput1->z.y*pInput2->y.z)+(pInput1->w.y*pInput2->y.w)</td>
	<td>(pInput1->x.z*pInput2->y.x)+(pInput1->y.z*pInput2->y.y)+(pInput1->z.z*pInput2->y.z)+(pInput1->w.z*pInput2->y.w)</td>
	<td>(pInput1->x.w*pInput2->y.x)+(pInput1->y.w*pInput2->y.y)+(pInput1->z.w*pInput2->y.z)+(pInput1->w.w*pInput2->y.w)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput1->x.x*pInput2->z.x)+(pInput1->y.x*pInput2->z.y)+(pInput1->z.x*pInput2->z.z)+(pInput1->w.x*pInput2->z.w)</td>
	<td>(pInput1->x.y*pInput2->z.x)+(pInput1->y.y*pInput2->z.y)+(pInput1->z.y*pInput2->z.z)+(pInput1->w.y*pInput2->z.w)</td>
	<td>(pInput1->x.z*pInput2->z.x)+(pInput1->y.z*pInput2->z.y)+(pInput1->z.z*pInput2->z.z)+(pInput1->w.z*pInput2->z.w)</td>
	<td>(pInput1->x.w*pInput2->z.x)+(pInput1->y.w*pInput2->z.y)+(pInput1->z.w*pInput2->z.z)+(pInput1->w.w*pInput2->z.w)</td>
	</tr>
	<tr><th>w</th>
	<td>(pInput1->x.x*pInput2->w.x)+(pInput1->y.x*pInput2->w.y)+(pInput1->z.x*pInput2->z.z)+(pInput1->w.x*pInput2->w.w)</td>
	<td>(pInput1->x.y*pInput2->w.x)+(pInput1->y.y*pInput2->w.y)+(pInput1->z.y*pInput2->z.z)+(pInput1->w.y*pInput2->w.w)</td>
	<td>(pInput1->x.z*pInput2->w.x)+(pInput1->y.z*pInput2->w.y)+(pInput1->z.z*pInput2->z.z)+(pInput1->w.z*pInput2->w.w)</td>
	<td>(pInput1->x.w*pInput2->w.x)+(pInput1->y.w*pInput2->w.y)+(pInput1->z.w*pInput2->z.z)+(pInput1->w.w*pInput2->w.w)</td>
	</tr>
	</table>

	\note This function is optimized to reduce variable usage by storing into the output
		without buffering. Do not use the output matrix as an input matrix.

	\param pInput1 Matrix to multiply from
	\param pInput2 Matrix to multiply against
	\sa Multiply(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput1,const Matrix4D_t *pInput2)
{
	x.x=(pInput1->x.x*pInput2->x.x)+(pInput1->y.x*pInput2->x.y)+(pInput1->z.x*pInput2->x.z)+(pInput1->w.x*pInput2->x.w);
	x.y=(pInput1->x.y*pInput2->x.x)+(pInput1->y.y*pInput2->x.y)+(pInput1->z.y*pInput2->x.z)+(pInput1->w.y*pInput2->x.w);
	x.z=(pInput1->x.z*pInput2->x.x)+(pInput1->y.z*pInput2->x.y)+(pInput1->z.z*pInput2->x.z)+(pInput1->w.z*pInput2->x.w);
	x.w=(pInput1->x.w*pInput2->x.x)+(pInput1->y.w*pInput2->x.y)+(pInput1->z.w*pInput2->x.z)+(pInput1->w.w*pInput2->x.w);

	y.x=(pInput1->x.x*pInput2->y.x)+(pInput1->y.x*pInput2->y.y)+(pInput1->z.x*pInput2->y.z)+(pInput1->w.x*pInput2->y.w);
	y.y=(pInput1->x.y*pInput2->y.x)+(pInput1->y.y*pInput2->y.y)+(pInput1->z.y*pInput2->y.z)+(pInput1->w.y*pInput2->y.w);
	y.z=(pInput1->x.z*pInput2->y.x)+(pInput1->y.z*pInput2->y.y)+(pInput1->z.z*pInput2->y.z)+(pInput1->w.z*pInput2->y.w);
	y.w=(pInput1->x.w*pInput2->y.x)+(pInput1->y.w*pInput2->y.y)+(pInput1->z.w*pInput2->y.z)+(pInput1->w.w*pInput2->y.w);

	z.x=(pInput1->x.x*pInput2->z.x)+(pInput1->y.x*pInput2->z.y)+(pInput1->z.x*pInput2->z.z)+(pInput1->w.x*pInput2->z.w);
	z.y=(pInput1->x.y*pInput2->z.x)+(pInput1->y.y*pInput2->z.y)+(pInput1->z.y*pInput2->z.z)+(pInput1->w.y*pInput2->z.w);
	z.z=(pInput1->x.z*pInput2->z.x)+(pInput1->y.z*pInput2->z.y)+(pInput1->z.z*pInput2->z.z)+(pInput1->w.z*pInput2->z.w);
	z.w=(pInput1->x.w*pInput2->z.x)+(pInput1->y.w*pInput2->z.y)+(pInput1->z.w*pInput2->z.z)+(pInput1->w.w*pInput2->z.w);

	w.x=(pInput1->x.x*pInput2->w.x)+(pInput1->y.x*pInput2->w.y)+(pInput1->z.x*pInput2->w.z)+(pInput1->w.x*pInput2->w.w);
	w.y=(pInput1->x.y*pInput2->w.x)+(pInput1->y.y*pInput2->w.y)+(pInput1->z.y*pInput2->w.z)+(pInput1->w.y*pInput2->w.w);
	w.z=(pInput1->x.z*pInput2->w.x)+(pInput1->y.z*pInput2->w.y)+(pInput1->z.z*pInput2->w.z)+(pInput1->w.z*pInput2->w.w);
	w.w=(pInput1->x.w*pInput2->w.x)+(pInput1->y.w*pInput2->w.y)+(pInput1->z.w*pInput2->w.z)+(pInput1->w.w*pInput2->w.w);
}


/*! ************************************

	\brief Perform a matrix multiply by a scalar.
	
	Multiply a scalar value to every element in the matrix.

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>xx*fInput</td><td>xy*fInput</td><td>xz*fInput</td><td>xw*fInput</td></tr>
	<tr><th>y</td>	<td>yx*fInput</td><td>yy*fInput</td><td>yz*fInput</td><td>yw*fInput</td></tr>
	<tr><th>z</td>	<td>zx*fInput</td><td>zy*fInput</td><td>zz*fInput</td><td>zw*fInput</td></tr>
	<tr><th>w</td>	<td>wx*fInput</td><td>wy*fInput</td><td>wz*fInput</td><td>ww*fInput</td></tr>
	</table>

	\param fScale Scalar value to multiply all values by
	\sa Multiply(const Matrix4D_t *,float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(float fScale)
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

	Multiply all values of the matrix by a scalar constant
	and store the result in this matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>pInput->xx*fInput</td><td>pInput->xy*fInput</td><td>pInput->xz*fInput</td><td>pInput->xw*fInput</td></tr>
	<tr><th>y</td>	<td>pInput->yx*fInput</td><td>pInput->yy*fInput</td><td>pInput->yz*fInput</td><td>pInput->yw*fInput</td></tr>
	<tr><th>z</td>	<td>pInput->zx*fInput</td><td>pInput->zy*fInput</td><td>pInput->zz*fInput</td><td>pInput->zw*fInput</td></tr>
	<tr><th>w</td>	<td>pInput->wx*fInput</td><td>pInput->wy*fInput</td><td>pInput->wz*fInput</td><td>pInput->ww*fInput</td></tr>
	</table>

	\param pInput Pointer to Matrix to multiply
	\param fScale Scalar value to multiply all values by
	\sa Multiply(float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput,float fScale)
{
	x.x = pInput->x.x*fScale;
	x.y = pInput->x.y*fScale;
	x.z = pInput->x.z*fScale;
	x.w = pInput->x.w*fScale;

	y.x = pInput->y.x*fScale;
	y.y = pInput->y.y*fScale;
	y.z = pInput->y.z*fScale;
	y.w = pInput->y.w*fScale;

	z.x = pInput->z.x*fScale;
	z.y = pInput->z.y*fScale;
	z.z = pInput->z.z*fScale;
	z.w = pInput->z.w*fScale;

	w.x = pInput->w.x*fScale;
	w.y = pInput->w.y*fScale;
	w.z = pInput->w.z*fScale;
	w.w = pInput->w.w*fScale;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleX)</td>
	<td>(x.z*fScaleX)</td>
	<td>(x.w*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleY)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleY)</td>
	<td>(y.w*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleZ)</td>
	<td>(z.y*fScaleZ)</td>
	<td>(z.z*fScaleZ)</td>
	<td>(z.w*fScaleZ)</td>
	</tr>
	<tr><th>w</th>
	<td>(w.x*1.0f)</td>
	<td>(w.y*1.0f)</td>
	<td>(w.z*1.0f)</td>
	<td>(w.w*1.0f)</td>
	</tr>
	</table>

	\note Terms with a multiply by 1.0f perform no action in the implementation.

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa Multiply(const Matrix4D_t *,float,float,float), TransposeMultiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(float fScaleX,float fScaleY,float fScaleZ)
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

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix4D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleX)</td>
	<td>(pInput->x.z*fScaleX)</td>
	<td>(pInput->x.w*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleY)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleY)</td>
	<td>(pInput->y.w*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleZ)</td>
	<td>(pInput->z.y*fScaleZ)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	<td>(pInput->z.w*fScaleZ)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->w.x*1.0f)</td>
	<td>(pInput->w.y*1.0f)</td>
	<td>(pInput->w.z*1.0f)</td>
	<td>(pInput->w.w*1.0f)</td>
	</tr>
	</table>

	\note Terms with a multiply by 1.0f perform a copy operation instead of a multiply in the implementation.

	\param pInput Pointer to a valid Matrix4D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa Multiply(float,float,float), TransposeMultiply(const Matrix4D_t*,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput,float fScaleX,float fScaleY,float fScaleZ)
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
	w.x = pInput->w.x;	// * 1.0f;
	w.y = pInput->w.y;	// * 1.0f;
	w.z = pInput->w.z;	// * 1.0f;
	w.w = pInput->w.w;	// * 1.0f;
}

/*! ************************************

	\brief Multiply by an X, Y, Z and W scale

	Using only the x.x, y.y, z.z and w.w components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleX)</td>
	<td>(x.z*fScaleX)</td>
	<td>(x.w*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleY)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleY)</td>
	<td>(y.w*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleZ)</td>
	<td>(z.y*fScaleZ)</td>
	<td>(z.z*fScaleZ)</td>
	<td>(z.w*fScaleZ)</td>
	</tr>
	<tr><th>w</th>
	<td>(w.x*fScaleW)</td>
	<td>(w.y*fScaleW)</td>
	<td>(w.z*fScaleW)</td>
	<td>(w.w*fScaleW)</td>
	</tr>
	</table>

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\param fScaleW W scale value
	\sa Multiply(const Matrix4D_t *,float,float,float,float), TransposeMultiply(float,float,float,float) or SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(float fScaleX,float fScaleY,float fScaleZ,float fScaleW)
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

	Using only the x.x, y.y, z.z and w.w components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix4D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleX)</td>
	<td>(pInput->x.z*fScaleX)</td>
	<td>(pInput->x.w*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleY)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleY)</td>
	<td>(pInput->y.w*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleZ)</td>
	<td>(pInput->z.y*fScaleZ)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	<td>(pInput->z.w*fScaleZ)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->w.x*fScaleW)</td>
	<td>(pInput->w.y*fScaleW)</td>
	<td>(pInput->w.z*fScaleW)</td>
	<td>(pInput->w.w*fScaleW)</td>
	</tr>
	</table>

	\param pInput Pointer to a valid Matrix4D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\param fScaleW W scale value
	\sa Multiply(float,float,float,float), TransposeMultiply(const Matrix4D_t*,float,float,float,float) or SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput,float fScaleX,float fScaleY,float fScaleZ,float fScaleW)
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
	
	Multiply a scalar value to the 3x3 subset of a matrix (The equivalent of W equaling 1.0f)

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>xx*fInput</td><td>xy*fInput</td><td>xz*fInput</td><td>xw*1.0f</td></tr>
	<tr><th>y</td>	<td>yx*fInput</td><td>yy*fInput</td><td>yz*fInput</td><td>yw*1.0f</td></tr>
	<tr><th>z</td>	<td>zx*fInput</td><td>zy*fInput</td><td>zz*fInput</td><td>zw*1.0f</td></tr>
	<tr><th>w</td>	<td>wx*1.0f</td><td>wy*1.0f</td><td>wz*1.0f</td><td>ww*1.0f</td></tr>
	</table>

	\note The terms where the value is multiplied by 1.0f are actually not performed and
		are replaced with a value copy since mathematically they perform the same action.

	\param fInput Scalar value to multiply all values by
	\sa Multiply3x3(const Matrix4D_t *,float) or Multiply(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply3x3(float fInput)
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
	
	Multiply a scalar value to the 3x3 subset of a matrix (The equivalent of W equaling 1.0f)

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>pInput->xx*fInput</td><td>pInput->xy*fInput</td><td>pInput->xz*fInput</td><td>pInput->xw*1.0f</td></tr>
	<tr><th>y</td>	<td>pInput->yx*fInput</td><td>pInput->yy*fInput</td><td>pInput->yz*fInput</td><td>pInput->yw*1.0f</td></tr>
	<tr><th>z</td>	<td>pInput->zx*fInput</td><td>pInput->zy*fInput</td><td>pInput->zz*fInput</td><td>pInput->zw*1.0f</td></tr>
	<tr><th>w</td>	<td>pInput->wx*1.0f</td><td>pInput->wy*1.0f</td><td>pInput->wz*1.0f</td><td>pInput->ww*1.0f</td></tr>
	</table>

	\note The terms where the value is multiplied by 1.0f are actually not performed and
		are replaced with a value copy since mathematically they perform the same action.

	\param pInput Pointer to Matrix to multiply
	\param fInput Scalar value to multiply all values by
	\sa Multiply3x3(float) or Multiply(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply3x3(const Matrix4D_t *pInput,float fInput)
{
	x.x = pInput->x.x*fInput;
	x.y = pInput->x.y*fInput;
	x.z = pInput->x.z*fInput;
	x.w = pInput->x.w;

	y.x = pInput->y.x*fInput;
	y.y = pInput->y.y*fInput;
	y.z = pInput->y.z*fInput;
	y.w = pInput->y.w;

	z.x = pInput->z.x*fInput;
	z.y = pInput->z.y*fInput;
	z.z = pInput->z.z*fInput;
	z.w = pInput->z.w;

	w.x = pInput->w.x;
	w.y = pInput->w.y;
	w.z = pInput->w.z;
	w.w = pInput->w.w;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleY)</td>
	<td>(x.z*fScaleZ)</td>
	<td>(x.w*1.0f)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleX)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleZ)</td>
	<td>(y.w*1.0f)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleX)</td>
	<td>(z.y*fScaleY)</td>
	<td>(z.z*fScaleZ)</td>
	<td>(z.w*1.0f)</td>
	</tr>
	<tr><th>w</th>
	<td>(w.x*fScaleX)</td>
	<td>(w.y*fScaleY)</td>
	<td>(w.z*fScaleZ)</td>
	<td>(w.w*1.0f)</td>
	</tr>
	</table>

	\note Terms with a multiply by 1.0f perform no action in the implementation.

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa TransposeMultiply(const Matrix4D_t *,float,float,float), Multiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(float fScaleX,float fScaleY,float fScaleZ)
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

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix4D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleY)</td>
	<td>(pInput->x.z*fScaleZ)</td>
	<td>(pInput->x.w*1.0f)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleX)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleZ)</td>
	<td>(pInput->y.w*1.0f)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleX)</td>
	<td>(pInput->z.y*fScaleY)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	<td>(pInput->z.w*1.0f)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->w.x*fScaleX)</td>
	<td>(pInput->w.y*fScaleY)</td>
	<td>(pInput->w.z*fScaleZ)</td>
	<td>(pInput->w.w*1.0f)</td>
	</tr>
	</table>

	\note Terms with a multiply by 1.0f perform a copy operation instead of a multiply in the implementation.

	\param pInput Pointer to a valid Matrix4D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa TransposeMultiply(float,float,float), Multiply(const Matrix4D_t*,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(const Matrix4D_t *pInput,float fScaleX,float fScaleY,float fScaleZ)
{
	x.x = pInput->x.x * fScaleX;
	x.y = pInput->x.y * fScaleY;
	x.z = pInput->x.z * fScaleZ;
	x.w = pInput->x.w;	// * 1.0f;
	y.x = pInput->y.x * fScaleX;
	y.y = pInput->y.y * fScaleY;
	y.z = pInput->y.z * fScaleZ;
	y.w = pInput->y.w;	// * 1.0f;
	z.x = pInput->z.x * fScaleX;
	z.y = pInput->z.y * fScaleY;
	z.z = pInput->z.z * fScaleZ;
	z.w = pInput->z.w;	// * 1.0f;
	w.x = pInput->w.x * fScaleX;
	w.y = pInput->w.y * fScaleY;
	w.z = pInput->w.z * fScaleZ;
	w.w = pInput->w.w;	// * 1.0f;
}

/*! ************************************

	\brief Multiply by an X, Y, Z, and W scale

	Using only the x.x, y.y, z.z and w.w components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleY)</td>
	<td>(x.z*fScaleZ)</td>
	<td>(x.w*fScaleW)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleX)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleZ)</td>
	<td>(y.w*fScaleW)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleX)</td>
	<td>(z.y*fScaleY)</td>
	<td>(z.z*fScaleZ)</td>
	<td>(z.w*fScaleW)</td>
	</tr>
	<tr><th>w</th>
	<td>(w.x*fScaleX)</td>
	<td>(w.y*fScaleY)</td>
	<td>(w.z*fScaleZ)</td>
	<td>(w.w*fScaleW)</td>
	</tr>
	</table>

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\param fScaleW W scale value
	\sa TransposeMultiply(const Matrix4D_t *,float,float,float,float), Multiply(float,float,float,float) or SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(float fScaleX,float fScaleY,float fScaleZ,float fScaleW)
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

	Using only the x.x, y.y, z.z and w.w components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix4D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleY)</td>
	<td>(pInput->x.z*fScaleZ)</td>
	<td>(pInput->x.w*fScaleW)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleX)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleZ)</td>
	<td>(pInput->y.w*fScaleW)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleX)</td>
	<td>(pInput->z.y*fScaleY)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	<td>(pInput->z.w*fScaleW)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->w.x*fScaleX)</td>
	<td>(pInput->w.y*fScaleY)</td>
	<td>(pInput->w.z*fScaleZ)</td>
	<td>(pInput->w.w*fScaleW)</td>
	</tr>
	</table>

	\param pInput Pointer to a valid Matrix4D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\param fScaleW W scale value
	\sa TransposeMultiply(float,float,float,float), Multiply(const Matrix4D_t*,float,float,float,float) or SetScale(float,float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeMultiply(const Matrix4D_t *pInput,float fScaleX,float fScaleY,float fScaleZ,float fScaleW)
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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+(xw)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+(yw)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+(zw)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\sa Transform(Vector3D_t *,const Vector3D_t *) const or TransposeTransform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + x.y*fY + x.z*fZ + x.w;
	pInput->y=y.x*fX + y.y*fY + y.z*fZ + y.w;
	pInput->z=z.x*fX + z.y*fY + z.z*fZ + z.w;
}

/*! ************************************

	\brief Multiply a vector by a matrix

	Transform the point by the matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+(xw*w)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+(yw*w)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+(zw*w)</td></tr>
	<tr><th>w</th><td>(wx*x)+(wy*y)+(wz*z)+(ww*w)</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t to transform against this matrix
	\sa Transform(Vector4D_t *,const Vector4D_t *) const or TransposeTransform(Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(Vector4D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	pInput->x=x.x*fX + x.y*fY + x.z*fZ + x.w*fW;
	pInput->y=y.x*fX + y.y*fY + y.z*fZ + y.w*fW;
	pInput->z=z.x*fX + z.y*fY + z.z*fZ + z.w*fW;
	pInput->w=w.x*fX + w.y*fY + w.z*fZ + w.w*fW;
}

/*! ************************************

	\brief Multiply a vector by a matrix

	Transform the point by the matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+(xw)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+(yw)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+(zw)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa Transform(Vector3D_t *) const or TransposeTransform(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + x.y*fY + x.z*fZ + x.w;
	pOutput->y=y.x*fX + y.y*fY + y.z*fZ + y.w;
	pOutput->z=z.x*fX + z.y*fY + z.z*fZ + z.w;
}


/*! ************************************

	\brief Multiply a vector by a matrix

	Transform the point by the matrix and store the result in an
	uninitialized Vector4D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+(xw*w)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+(yw*w)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+(zw*w)</td></tr>
	<tr><th>w</th><td>(wx*x)+(wy*y)+(wz*z)+(ww*w)</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t to transform against this matrix
	\param pOutput Pointer to an uninitialized Vector4D_t to store the result
	\sa Transform(Vector4D_t *) const or TransposeTransform(Vector4D_t *,const Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform(Vector4D_t *pOutput,const Vector4D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	pOutput->x=x.x*fX + x.y*fY + x.z*fZ + x.w*fW;
	pOutput->y=y.x*fX + y.y*fY + y.z*fZ + y.w*fW;
	pOutput->z=z.x*fX + z.y*fY + z.z*fZ + z.w*fW;
	pOutput->w=w.x*fX + w.y*fY + w.z*fZ + w.w*fW;
}

/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+(wx)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+(wy)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+(wz)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\sa TransposeTransform(Vector3D_t *,const Vector3D_t *) const or Transform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + y.x*fY + z.x*fZ + w.x;
	pInput->y=x.y*fX + y.y*fY + z.y*fZ + w.y;
	pInput->z=x.z*fX + y.z*fY + z.z*fZ + w.z;
}

/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+(wx*w)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+(wy*w)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+(wz*w)</td></tr>
	<tr><th>w</th><td>(xw*x)+(yw*y)+(zw*z)+(ww*w)</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t to transform against this transposed matrix
	\sa TransposeTransform(Vector4D_t *,const Vector4D_t *) const or Transform(Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(Vector4D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	pInput->x=x.x*fX + y.x*fY + z.x*fZ + w.x*fW;
	pInput->y=x.y*fX + y.y*fY + z.y*fZ + w.y*fW;
	pInput->z=x.z*fX + y.z*fY + z.z*fZ + w.z*fW;
	pInput->w=x.w*fX + y.w*fY + z.w*fZ + w.w*fW;
}

/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+(wx)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+(wy)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+(wz)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa TransposeTransform(Vector3D_t *) const or Transform(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + y.x*fY + z.x*fZ + w.x;
	pOutput->y=x.y*fX + y.y*fY + z.y*fZ + w.y;
	pOutput->z=x.z*fX + y.z*fY + z.z*fZ + w.z;
}


/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix and store the result in an
	uninitialized Vector4D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+(wx*w)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+(wy*w)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+(wz*w)</td></tr>
	<tr><th>w</th><td>(xw*x)+(yw*y)+(zw*z)+(ww*w)</td></tr>
	</table>

	\param pInput Pointer to a Vector4D_t to transform against this transposed matrix
	\param pOutput Pointer to an uninitialized Vector4D_t to store the result
	\sa TransposeTransform(Vector4D_t *) const or Transform(Vector4D_t *,const Vector4D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform(Vector4D_t *pOutput,const Vector4D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	pOutput->x=x.x*fX + y.x*fY + z.x*fZ + w.x*fW;
	pOutput->y=x.y*fX + y.y*fY + z.y*fZ + w.y*fW;
	pOutput->z=x.z*fX + y.z*fY + z.z*fZ + w.z*fW;
	pOutput->w=x.w*fX + y.w*fY + z.w*fZ + w.w*fW;
}


/*! ************************************

	\brief Multiply a vector by a matrix without adding W

	Transform the point by the matrix only using the x, y and z terms

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\sa Transform3x3(Vector3D_t *,const Vector3D_t *) const or TransposeTransform3x3(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform3x3(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + x.y*fY + x.z*fZ;
	pInput->y=y.x*fX + y.y*fY + y.z*fZ;
	pInput->z=z.x*fX + z.y*fY + z.z*fZ;
}

/*! ************************************

	\brief Multiply a vector by a matrix without adding W

	Transform the point by the matrix only using the x, y and z terms
	and store the result in an uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa Transform3x3(Vector3D_t *) const or TransposeTransform3x3(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::Transform3x3(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + x.y*fY + x.z*fZ;
	pOutput->y=y.x*fX + y.y*fY + y.z*fZ;
	pOutput->z=z.x*fX + z.y*fY + z.z*fZ;
}

/*! ************************************

	\brief Multiply a vector by a transposed matrix without adding W

	Transform the point by the transposed matrix only using the x, y and z terms

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\sa TransposeTransform3x3(Vector3D_t *,const Vector3D_t *) const or Transform3x3(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform3x3(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + y.x*fY + z.x*fZ;
	pInput->y=x.y*fX + y.y*fY + z.y*fZ;
	pInput->z=x.z*fX + y.z*fY + z.z*fZ;
}

/*! ************************************

	\brief Multiply a vector by a transposed matrix without adding W

	Transform the point by the transposed matrix only using the x, y and z terms
	and store the result in an uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa TransposeTransform3x3(Vector3D_t *) const or Transform3x3(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTransform3x3(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + y.x*fY + z.x*fZ;
	pOutput->y=x.y*fX + y.y*fY + z.y*fZ;
	pOutput->z=x.z*fX + y.z*fY + z.z*fZ;
}

/*! ************************************
	
	\brief Rotate a matrix in the Y axis (Yaw)

	Given a Y angle in radians, rotate the matrix accordingly

	fCos = Cos(fYaw); fSin = Sin(fYaw);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(z.x*fSin)+(x.x*fCos)</td><td>(z.y*fSin)+(x.y*fCos)</td><td>(z.z*fSin)+(x.z*fCos)</td><td>(z.w*fSin)+(x.w*fCos)</td></tr>
	<tr><th>y</th><td>y.x</td><td>y.y</td><td>y.z</td><td>y.w</td></tr>
	<tr><th>z</th><td>(z.x*fCos)-(x.x*fSin)</td><td>(z.y*fCos)-(x,y*fSin)</td><td>(z.z*fCos)-(x.z*fSin)</td><td>(z.w*fCos)-(x.w*fSin)</td></tr>
	<tr><th>w</th><td>w.x</td><td>w.y</td><td>w.z</td><td>w.w</td></tr>
	</table>

	\param fYaw Angle in radians to yaw the matrix
	\sa Pitch(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Yaw(float fYaw)
{
	float fCos = Cos(fYaw);
	float fSin = Sin(fYaw);

	float fXX = x.x;
	float fXY = x.y;
	float fXZ = x.z;
	float fXW = x.w;

	x.x = (z.x*fSin)+(fXX*fCos);
	x.y = (z.y*fSin)+(fXY*fCos);
	x.z = (z.z*fSin)+(fXZ*fCos);
	x.w = (z.w*fSin)+(fXW*fCos);

	z.x = (z.x*fCos)-(fXX*fSin);
	z.y = (z.y*fCos)-(fXY*fSin);
	z.z = (z.z*fCos)-(fXZ*fSin);
	z.w = (z.w*fCos)-(fXW*fSin);
}

/*! ************************************
	
	\brief Rotate a matrix in the X axis (Pitch)

	Given a X angle in radians, rotate the matrix accordingly

	fCos = Cos(fPitch); fSin = Sin(fPitch);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>x.x</td><td>x.y</td><td>x.z</td><td>x.w</td></tr>
	<tr><th>y</th><td>(z.x*fSin)+(y.x*fCos)</td><td>(z.y*fSin)+(y.y*fCos)</td><td>(z.z*fSin)+(y.z*fCos)</td><td>(z.w*fSin)+(y.w*fCos)</td></tr>
	<tr><th>z</th><td>(z.x*fCos)-(y.x*fSin)</td><td>(z.y*fCos)-(y.y*fSin)</td><td>(z.z*fCos)-(y.z*fSin)</td><td>(z.w*fCos)-(y.w*fSin)</td></tr>
	<tr><th>w</th><td>w.x</td><td>w.y</td><td>w.z</td><td>w.w</td></tr>
	</table>

	\param fPitch Angle in radians to pitch the matrix
	\sa Yaw(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Pitch(float fPitch)
{
	float fCos = Cos(fPitch);
	float fSin = Sin(fPitch);

	float fYX = y.x;
	float fYY = y.y;
	float fYZ = y.z;
	float fYW = y.w;

	y.x = (z.x*fSin)+(fYX*fCos);
	y.y = (z.y*fSin)+(fYY*fCos);
	y.z = (z.z*fSin)+(fYZ*fCos);
	y.w = (z.w*fSin)+(fYW*fCos);

	z.x = (z.x*fCos)-(fYX*fSin);
	z.y = (z.y*fCos)-(fYY*fSin);
	z.z = (z.z*fCos)-(fYZ*fSin);
	z.w = (z.w*fCos)-(fYW*fSin);
}

/*! ************************************
	
	\brief Rotate a matrix in the Z axis (Roll)

	Given a Z angle in radians, rotate the matrix accordingly

	fCos = Cos(fRoll); fSin = Sin(fRoll);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>(y.x*fSin)+(x.x*fCos)</td><td>(y.y*fSin)+(x.y*fCos)</td><td>(y.z*fSin)+(x.z*fCos)</td><td>(y.w*fSin)+(x.w*fCos)</td></tr>
	<tr><th>y</th><td>(y.x*fCos)-(x.x*fSin)</td><td>(y.y*fCos)-(x.y*fSin)</td><td>(y.z*fCos)-(x.z*fSin)</td><td>(y.w*fCos)-(x.w*fSin)</td></tr>
	<tr><th>z</th><td>z.x</td><td>z.y</td><td>z.z</td><td>z.w</td></tr>
	<tr><th>w</th><td>w.x</td><td>w.y</td><td>w.z</td><td>w.w</td></tr>
	</table>

	\param fRoll Angle in radians to roll the matrix
	\sa Yaw(float) or Pitch(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Roll(float fRoll)
{
	float fCos = Cos(fRoll);
	float fSin = Sin(fRoll);

	float fXX = x.x;
	float fXY = x.y;
	float fXZ = x.z;
	float fXW = x.w;

	x.x = (y.x*fSin)+(fXX*fCos);
	x.y = (y.y*fSin)+(fXY*fCos);
	x.z = (y.z*fSin)+(fXZ*fCos);
	x.w = (y.w*fSin)+(fXW*fCos);

	y.x = (y.x*fCos)-(fXX*fSin);
	y.y = (y.y*fCos)-(fXY*fSin);
	y.z = (y.z*fCos)-(fXZ*fSin);
	y.w = (y.w*fCos)-(fXW*fSin);
}

/*! ************************************
	
	\brief Rotate a matrix in an arbitrary axis

	Given a vector to determine direction and an angle in radians, rotate the matrix accordingly

	\note This is a replacement for glRotate()

	\param fRadians Angle in radians to roll the matrix
	\param fX X value of the vector
	\param fY Y value of the vector
	\param fZ Z value of the vector
	\sa Yaw(float), Pitch(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Rotate(float fRadians,float fX,float fY,float fZ)
{
	if ((fY == 0.0f) && (fZ == 0.0f)) {
		Pitch(fRadians);
	} else if ((fX == 0.0f) && (fZ == 0.0f)) {
		Yaw(fRadians);
	} else if ((fX == 0.0f) && (fY == 0.0f)) {
		Roll(fRadians);
	} else {
		float fSin = Sin(fRadians);
		float fCos = Cos(fRadians);

		// Get the length of the vector
		float fLength = Sqrt((fX * fX) + (fY * fY) + (fZ * fZ));

		float fReciprocalLength = 1.0f / fLength;
		float fOneLessCosine = 1.0f - fCos;

		float fXNormal = fX * fReciprocalLength;
		float fYNormal = fY * fReciprocalLength;
		float fZNormal = fZ * fReciprocalLength;

		float fXNormalSquared = fXNormal * fXNormal;
		float fYNormalSquared = fYNormal * fYNormal;
		float fZNormalSquared = fZNormal * fZNormal;

		float fXYNegCosine = fXNormal * fYNormal * fOneLessCosine;
		float fYZNegCosine = fYNormal * fZNormal * fOneLessCosine;
		float fZXNegCosine = fZNormal * fXNormal * fOneLessCosine;

		fXNormal *= fSin;
		fYNormal *= fSin;
		fZNormal *= fSin;

		// Create the constants
		float fXXAdj = fXNormalSquared + fCos * (1.0f - fXNormalSquared);
		float fXYAdj = fXYNegCosine + fZNormal;
		float fXZAdj = fZXNegCosine - fYNormal;
		float fYXAdj = fXYNegCosine - fZNormal;
		float fYYAdj = fYNormalSquared + fCos * (1.0f - fYNormalSquared);
		float fYZAdj = fYZNegCosine + fXNormal;
		float fZXAdj = fZXNegCosine + fYNormal;
		float fZYAdj = fYZNegCosine - fXNormal;
		float fZZAdj = fZNormalSquared + fCos * (1.0f - fZNormalSquared);

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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>x.x</td><td>x.y</td><td>x.z</td><td>x.w</td></tr>
	<tr><th>y</th><td>y.x</td><td>y.y</td><td>y.z</td><td>y.w</td></tr>
	<tr><th>z</th><td>z.x</td><td>z.y</td><td>z.z</td><td>z.w</td></tr>
	<tr><th>w</th><td>(x.x*fX)+(y.x*fY)+(z.x*fZ)+w.x</td><td>(x.y*fX)+(y.y*fY)+(z.y*fZ)+w.y</td><td>(x.z*fX)+(y.z*fY)+(z.z*fZ)+w.z</td><td>(x.w*fX)+(y.w*fY)+(z.w*fZ)+w.w</td></tr>
	</table>

	\param fX X value of the temp translation matrix 
	\param fY Y value of the temp translation matrix 
	\param fZ Z value of the temp translation matrix 
	\sa TransposeTranslate(float,float,float) or SetTranslate(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Translate(float fX,float fY,float fZ)
{
	w.x += x.x*fX + y.x*fY + z.x*fZ;
	w.y += x.y*fX + y.y*fY + z.y*fZ;
	w.z += x.z*fX + y.z*fY + z.z*fZ;
	w.w += x.w*fX + y.w*fY + z.w*fZ;
}

/*! ************************************
	
	\brief Multiply the matrix by a generated transposed translation matrix

	With an X, Y and Z for translation, apply a matrix multiply if a
	matrix was generated with SetTranslate(float,float,float) and then Transpose()

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</th><td>x.x</td><td>x.y</td><td>x.z</td><td>(x.x*fX)+(x.y*fY)+(x.z*fZ)+x.w</td></tr>
	<tr><th>y</th><td>y.x</td><td>y.y</td><td>y.z</td><td>(y.x*fX)+(y.y*fY)+(y.z*fZ)+y.w</td></tr>
	<tr><th>z</th><td>z.x</td><td>z.y</td><td>z.z</td><td>(z.x*fX)+(z.y*fY)+(z.z*fZ)+z.w</td></tr>
	<tr><th>w</th><td>w.x</td><td>w.y</td><td>w.z</td><td>(w.x*fX)+(w.y*fY)+(w.z*fZ)+w.w</td></tr>
	</table>

	\param fX X value of the temp translation matrix 
	\param fY Y value of the temp translation matrix 
	\param fZ Z value of the temp translation matrix 
	\sa Translate(float,float,float) or SetTranslate(float,float,float)

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeTranslate(float fX,float fY,float fZ)
{
	x.w += x.x*fX + x.y*fY + x.z*fZ;
	y.w += y.x*fX + y.y*fY + y.z*fZ;
	z.w += z.x*fX + z.y*fY + z.z*fZ;
	w.w += w.x*fX + w.y*fY + w.z*fZ;
}

/*! ************************************
	
	\brief Generate an affine inverse of a matrix

	Using the 3x3 sub-matrix, generate a determinate and
	use it to calculate the inverse of the 3x3 matrix.
	Adjust the translate component and then clear out
	the scale.

	If the matrix cannot be inverted, \ref FALSE is returned and
	the original matrix is copied as is.

	\param pInput Pointer to a matrix to affine invert.
	\return \ref TRUE if the inversion was successful, \ref FALSE if not

***************************************/

Word BURGER_API Burger::Matrix4D_t::AffineInverse(const Matrix4D_t *pInput)
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

	Word bResult;
	if( (fDeterminant == 0.0f) || (Abs(fDeterminant / (fPositive - fNegative)) < fPrecisionLimit)) {
		Set(pInput);
		bResult = FALSE;
	} else {
		//  Calculate inverse(A) = original(A) / determinant(A)

		//Reciprocal multiply for speed
		fDeterminant = 1.0F / fDeterminant;

		x.x =  (pInput->y.y * pInput->z.z - pInput->y.z * pInput->z.y) * fDeterminant;
		y.x = -(pInput->y.x * pInput->z.z - pInput->y.z * pInput->z.x) * fDeterminant;
		z.x =  (pInput->y.x * pInput->z.y - pInput->y.y * pInput->z.x) * fDeterminant;
		x.y = -(pInput->x.y * pInput->z.z - pInput->x.z * pInput->z.y) * fDeterminant;
		y.y =  (pInput->x.x * pInput->z.z - pInput->x.z * pInput->z.x) * fDeterminant;
		z.y = -(pInput->x.x * pInput->z.y - pInput->x.y * pInput->z.x) * fDeterminant;
		x.z =  (pInput->x.y * pInput->y.z - pInput->x.z * pInput->y.y) * fDeterminant;
		y.z = -(pInput->x.x * pInput->y.z - pInput->x.z * pInput->y.x) * fDeterminant;
		z.z =  (pInput->x.x * pInput->y.y - pInput->x.y * pInput->y.x) * fDeterminant;

		// Calculate -C * inverse(A)

		float fX = pInput->w.x;
		float fY = pInput->w.y;
		float fZ = pInput->w.z;

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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>fYScale/fAspect</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>fYScale</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>fFar/(fFar-fNear)</td>	<td>1</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>(-fNear * fFar) / (fFar-fNear)</td>	<td>0</td></tr>
	</table>

	\note This is a drop in replacement for the Windows function <a href="http://msdn.microsoft.com/en-us/library/bb205350(VS.85).aspx">D3DXMatrixPerspectiveFovLH()</a>.

	\param fFieldOfViewY Angle of the field of view in radians
	\param fAspect Aspect ratio of the screen
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa PerspectiveFovRH()

***************************************/

void BURGER_API Burger::Matrix4D_t::PerspectiveFovLH(float fFieldOfViewY,float fAspect,float fNear,float fFar)
{
	// Calculate the cotangent of Field of View * 2.0f
	float fYScale = 1.0f / Tan(fFieldOfViewY*0.5f);
	float fDepth = (fFar-fNear);

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

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>fYScale/fAspect</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>fYScale</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>fFar/(fNear-fFar)</td>	<td>-1</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>(fNear * fFar) / (fNear-fFar)</td>	<td>0</td></tr>
	</table>

	\note This is a drop in replacement for the Windows function <a href="http://msdn.microsoft.com/en-us/library/bb205351(v=vs.85).aspx">D3DXMatrixPerspectiveFovRH()</a>.

	\param fFieldOfViewY Angle of the field of view in radians
	\param fAspect Aspect ratio of the screen
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa PerspectiveFovLH()

***************************************/

void BURGER_API Burger::Matrix4D_t::PerspectiveFovRH(float fFieldOfViewY,float fAspect,float fNear,float fFar)
{
	// Calculate the cotangent of Field of View * 2.0f
	float fYScale = 1.0f / Tan(fFieldOfViewY*0.5f);
	float fDepth = (fNear-fFar);

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

	This convenience operator converts the Matrix4D_t into 
	a float pointer to pass to other APIs that treat this as
	an array of 32 bit floats.

	\return The pointer to the object typecast as a (const float *)

***************************************/

/*! ************************************

	\brief Constant 4x4 identity matrix

***************************************/

const Burger::Matrix4D_t Burger::s_Matrix4DIdentity = {
	{1.0f,0.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f},
	{0.0f,0.0f,1.0f,0.0f},
	{0.0f,0.0f,0.0f,1.0f}
};