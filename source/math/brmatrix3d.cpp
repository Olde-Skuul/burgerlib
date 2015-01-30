/***************************************

	3D Floating point matrix manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmatrix3d.h"
#include "brfixedmatrix3d.h"
#include "brmatrix4d.h"

/*! ************************************

	\struct Burger::Matrix3D_t
	\brief 3 dimensional floating point matrix

	This 36 byte matrix contains x,y and z 32 bit floating point rows and columns. A
	set of common functions for simple 3 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y" and "z" for maximum compatibility

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::Vector3D_t, Burger::Matrix4D_t or Burger::FixedMatrix3D_t
	
***************************************/

/*! ************************************

	\brief Clear out a 3D matrix

	Set all of the entries to zero

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>0</td></tr>
	</table>

	\sa Identity(void)

***************************************/

void BURGER_API Burger::Matrix3D_t::Zero(void)
{
	float fTemp = 0.0f;		// Force the compiler to use a register
	x.x = fTemp;			// Fill all the elements with zero
	x.y = fTemp;
	x.z = fTemp;
	y.x = fTemp;
	y.y = fTemp;
	y.z = fTemp;
	z.x = fTemp;
	z.y = fTemp;
	z.z = fTemp;
}

/*! ************************************

	\brief Initialize a 3D matrix so that it is inert

	Sets the x.x, y.y and z.z components to 1.0f, all others to 0.0f

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\sa Zero(void)

***************************************/

void BURGER_API Burger::Matrix3D_t::Identity(void)
{
	x.x = 1.0f;		// Leave X alone
	x.y = 0.0f;
	x.z = 0.0f;
	y.x = 0.0f;
	y.y = 1.0f;		// Leave Y alone
	y.z = 0.0f;
	z.x = 0.0f;
	z.y = 0.0f;
	z.z = 1.0f;		// Leave Z alone
}

/*! ************************************

	\brief Copy the matrix

	Copy a matrix into this one

	\param pInput Pointer to a valid Matrix3D_t for copying
	\sa Identity() or Zero()

***************************************/

void BURGER_API Burger::Matrix3D_t::Set(const Matrix3D_t *pInput)
{
	x.x = pInput->x.x;
	x.y = pInput->x.y;
	x.z = pInput->x.z;
	y.x = pInput->y.x;
	y.y = pInput->y.y;
	y.z = pInput->y.z;
	z.x = pInput->z.x;
	z.y = pInput->z.y;
	z.z = pInput->z.z;
}

/*! ************************************

	\brief Convert a fixed point matrix into a floating point matrix

	Using FixedToFloat(float *,Fixed32), convert all of the entries
	from a 3x3 16.16 fixed point matrix into a floating point matrix

	\param pInput Pointer to a valid FixedMatrix3D_t for conversion
	\sa FixedToFloat(float *,Fixed32)

***************************************/

void BURGER_API Burger::Matrix3D_t::Set(const FixedMatrix3D_t *pInput)
{
	FixedToFloat(&x.x,&pInput->x.x);
	FixedToFloat(&x.y,&pInput->x.y);
	FixedToFloat(&x.z,&pInput->x.z);
	FixedToFloat(&y.x,&pInput->y.x);
	FixedToFloat(&y.y,&pInput->y.y);
	FixedToFloat(&y.z,&pInput->y.z);
	FixedToFloat(&z.x,&pInput->z.x);
	FixedToFloat(&z.y,&pInput->z.y);
	FixedToFloat(&z.z,&pInput->z.z);
}

/*! ************************************

	\brief Copy a Matrix4D_t to a Matrix3D_t

	Copy a 4x4 matrix into this one by truncating all of the w components

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>pInput->x.x</td>	<td>pInput->x.y</td>	<td>pInput->x.z</td></tr>
	<tr><th>y</th><td>pInput->y.x</td>	<td>pInput->y.y</td>	<td>pInput->y.z</td></tr>
	<tr><th>z</th><td>pInput->z.x</td>	<td>pInput->z.y</td>	<td>pInput->z.z</td></tr>
	</table>

	\param pInput Pointer to a valid Matrix4D_t for copying
	\sa Set(const Matrix3D_t *) or Matrix4D_t::Set(const Matrix3D_t *)

***************************************/

void BURGER_API Burger::Matrix3D_t::Set(const Matrix4D_t *pInput)
{
	x.x = pInput->x.x;
	x.y = pInput->x.y;
	x.z = pInput->x.z;
	y.x = pInput->y.x;
	y.y = pInput->y.y;
	y.z = pInput->y.z;
	z.x = pInput->z.x;
	z.y = pInput->z.y;
	z.z = pInput->z.z;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for yaw (Y)

	\code
	float sy = Sin(fYaw);
	float cy = Cos(fYaw);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy</td>	<td>0</td>	<td>-sy</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>z</th><td>sy</td>	<td>0</td>	<td>cy</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\sa TransposeSetYaw(float), Set(float,float,float), SetPitch(float) and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetYaw(float fYaw)
{
	float fSY = Sin(fYaw);		// Get the sine/cosine
	float fCY = Cos(fYaw);

	x.x = fCY;
	x.y = 0.0f;
	x.z = -fSY;

	y.x = 0.0f;
	y.y = 1.0f;
	y.z = 0.0f;

	z.x = fSY;
	z.y = 0.0f;
	z.z = fCY;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for pitch (X)

	\code
	float sx = Sin(fPitch);
	float cx = Cos(fPitch);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>cx</td>	<td>-sx</td></tr>
	<tr><th>z</th><td>0</td>	<td>sx</td><td>cx</td></tr>
	</table>

	\param fPitch Pitch of the X in radians
	\sa TransposeSetPitch(float), Set(float,float,float), SetYaw(float) and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetPitch(float fPitch)
{
	float fSX = Sin(fPitch);		// Get the sine/cosine
	float fCX = Cos(fPitch);

	x.x = 1.0f;
	x.y = 0.0f;
	x.z = 0.0f;

	y.x = 0.0f;
	y.y = fCX;
	y.z = -fSX;

	z.x = 0.0f;
	z.y = fSX;
	z.z = fCX;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for roll (Z)

	\code
	float sz = Sin(fRoll);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cz</td>	<td>-sz</td>	<td>0</td></tr>
	<tr><th>y</th><td>sz</td>	<td>cz</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fRoll Roll of the Z in radians
	\sa TransposeSetRoll(float), Set(float,float,float), SetYaw(float) and SetPitch(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetRoll(float fRoll)
{
	float fSZ = Sin(fRoll);		// Get the sine/cosine
	float fCZ = Cos(fRoll);

	x.x = fCZ;
	x.y = -fSZ;
	x.z = 0.0f;

	y.x = fSZ;
	y.y = fCZ;
	y.z = 0.0f;

	z.x = 0.0f;
	z.y = 0.0f;
	z.z = 1.0f;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(cy*cz)+(sy*-sx*-sz)</td><td>cx*-sz</td><td>(-sy*cz)+(cy*-sx*-sz)</td></tr>
	<tr><th>y</th><td>(sy*-sx*cz)+(cy*sz)</td><td>cx*cz</td><td>(cy*-sx*cz)+(-sy*sz)</td></tr>
	<tr><th>z</th><td>sy*cx</td><td>sx</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetYXZ(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetYXZ(float fYaw,float fPitch,float fRoll)
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

	y.x = (fSY*fNegSXCZ)+(fCY*fSZ);
	y.y = fCX*fCZ;
	y.z = (fCY*fNegSXCZ)-(fSY*fSZ);

	z.x = fSY*fCX;
	z.y = fSX;
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>-sz</td><td>-sy*cz</td></tr>
	<tr><th>y</th><td>(cy*sz*cx)+(sy*-sx)</td><td>cx*cz</td><td>(-sy*sz*cx)+(cy*-sx)</td></tr>
	<tr><th>z</th><td>(sy*cx)+(cy*sx*sz)</td><td>sx*cz</td><td>(cy*cx)+(-sy*sx*sz)</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetYZX(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetYZX(float fYaw,float fPitch,float fRoll)
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

	y.x = (fCY*fSZCX)-(fSY*fSX);
	y.y = fCX*fCZ;
	y.z = (-fSY*fSZCX)-(fCY*fSX);

	z.x = (fSY*fCX)+(fCY*fSXSZ);
	z.y = fSX*fCZ;
	z.z = (fCY*fCX)-(fSY*fSXSZ);
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(-sy*sx*cz)+(cx*-sz)</td><td>(-sy*cx*cz)+(-sx*-sz)</td></tr>
	<tr><th>y</th><td>cy*sz</td><td>(cx*cz)+(-sy*sx*sz)</td><td>(-sx*cx)+(cy*sx*-sz)</td></tr>
	<tr><th>z</th><td>sy</td><td>cy*sx</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetXYZ(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetXYZ(float fYaw,float fPitch,float fRoll)
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

	y.x = fCY*fSZ;
	y.y = (fCX*fCZ)-(fSY*fSXSZ);
	y.z = (-fSX*fCX)-(fCY*fSXSZ);

	z.x = fSY;
	z.y = fCY*fSX;
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(cx*-sz*cy)+(-sy*sx)</td><td>(-sx*-sz*cy)+(-sy*cx)</td></tr>
	<tr><th>y</th><td>sz</td><td>cx*cz</td><td>-sx*cz</td></tr>
	<tr><th>z</th><td>sy*cz</td><td>(sx*cy)+(sy*cx*-sz)</td><td>(cx*cy)+(sy*-sx*-sz)</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetXZY(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetXZY(float fYaw,float fPitch,float fRoll)
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

	y.x = fSZ;
	y.y = fCX*fCZ;
	y.z = -fSX*fCZ;

	z.x = fSY*fCZ;
	z.y = (fSX*fCY)-(fSYSZ*fCX);
	z.z = (fCX*fCY)+(fSYSZ*fSX);
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>cy*-sz</td><td>-sy</td></tr>
	<tr><th>y</th><td>(sz*cx)+(sy*-sx*cz)</td><td>(cz*cx)+(sy*-sx*-sz)</td><td>cy*-sx</td></tr>
	<tr><th>z</th><td>(sy*cz*cx)+(sx*sz)</td><td>(sy*-sz*cx)+(sx*cz)</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetZYX(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetZYX(float fYaw,float fPitch,float fRoll)
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

	y.x = (fSZ*fCX)-(fSYSX*fCZ);
	y.y = (fCZ*fCX)+(fSYSX*fSZ);
	y.z = fCY*-fSX;

	z.x = (fSYCX*fCZ)+(fSX*fSZ);
	z.y = (fSX*fCZ)-(fSYCX*fSZ);
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(cz*cy)+(-sy*sx*sz)</td><td>(-sz*cy)+(-sy*sx*cz)</td><td>-sy*cx</td></tr>
	<tr><th>y</th><td>cx*sz</td><td>cx*cz</td><td>-sx</td></tr>
	<tr><th>z</th><td>(sx*sz*cy)+(sy*cz)</td><td>(sx*cz*cy)+(sy*-sz)</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa TransposeSetZXY(float,float,float), SetYaw(float), SetPitch(float), and SetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetZXY(float fYaw,float fPitch,float fRoll)
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

	y.x = fCX*fSZ;
	y.y = fCX*fCZ;
	y.z = -fSX;

	z.x = (fSXCY*fSZ)+(fSY*fCZ);
	z.y = (fSXCY*fCZ)-(fSY*fSZ);
	z.z = fCY*fCX;
}

/*! ************************************

	\brief Create a 2D Translation matrix

	Create an identity matrix and set the z.x and z.y terms to the
	input to create a translation matrix for 2d points

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>z</th><td>fX</td>	<td>fY</td>	<td>1</td></tr>
	</table>
	
	\param fX New z.x term
	\param fY New z.y term

***************************************/

void BURGER_API Burger::Matrix3D_t::SetTranslate(float fX,float fY)
{
	x.x = 1.0f;
	x.y = 0.0f;
	x.z = 0.0f;
	y.x = 0.0f;
	y.y = 1.0f;
	y.z = 0.0f;
	z.x = fX;
	z.y = fY;
	z.z = 1.0f;
}

/*! ************************************

	\brief Convert a quaternion to a 3x3 matrix

	Here's the resulting matrix of math used using terms from the input quaternion

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>	<td>1-(2yy+2zz)</td>	<td>2xy+2wz</td>		<td>2xz-2wy</td></tr>
	<tr><th>y</th>	<td>2xy-2wz</td>		<td>1-(2xx+2zz)</td>	<td>2yz+2wx</td></tr>
	<tr><th>z</th>	<td>2xz+2wy</td>		<td>2yz-2wx</td>		<td>1-(2xx+2yy)</td></tr>
	</table>

	\param pInput Pointer to a normalized quaternion
	\sa Matrix4D_t::SetFromQuaternion(const Vector4D_t *pInput) or TransposeSetFromQuaternion(const Vector4D_t *pInput) 
	
***************************************/

void BURGER_API Burger::Matrix3D_t::SetFromQuaternion(const Vector4D_t *pInput)
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

	// y.x = 2xy + 2wz

	y.x=q_2xy+q_2zw;

	// y.y = 1 - 2x^2 - 2z^2

	y.y=1.0f-q_2xx-q_2zz;

	// y.z = 2yz - 2wx

	y.z=q_2yz-q_2xw;

	// z.x = 2xz - 2wy

	z.x=q_2xz-q_2yw;

	// z.y = 2yz + 2wx

	z.y=q_2yz+q_2xw;

	// z.z = 1 - 2x^2 - 2y^2

	z.z=1.0f-q_2xx-q_2yy;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for yaw (Y)

	\code
	float sy = Sin(fYaw);
	float cy = Cos(fYaw);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy</td>	<td>0</td>	<td>sy</td></tr>
	<tr><th>y</th><td>0</td>	<td>1</td>	<td>0</td></tr>
	<tr><th>z</th><td>-sy</td>	<td>0</td>	<td>cy</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\sa SetYaw(float), TransposeSet(float,float,float), TransposeSetPitch(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetYaw(float fYaw)
{
	float fSY = Sin(fYaw);		// Get the sine/cosine
	float fCY = Cos(fYaw);

	x.x = fCY;
	x.y = 0.0f;
	x.z = fSY;

	y.x = 0.0f;
	y.y = 1.0f;
	y.z = 0.0f;

	z.x = -fSY;
	z.y = 0.0f;
	z.z = fCY;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for pitch (X)

	\code
	float sx = Sin(fPitch);
	float cx = Cos(fPitch);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>1</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</th><td>0</td>	<td>cx</td>	<td>sx</td></tr>
	<tr><th>z</th><td>0</td>	<td>-sx</td><td>cx</td></tr>
	</table>

	\param fPitch Pitch of the X in radians
	\sa SetPitch(float), TransposeSet(float,float,float), TransposeSetYaw(float) and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetPitch(float fPitch)
{
	float fSX = Sin(fPitch);		// Get the sine/cosine
	float fCX = Cos(fPitch);

	x.x = 1.0f;
	x.y = 0.0f;
	x.z = 0.0f;

	y.x = 0.0f;
	y.y = fCX;
	y.z = fSX;

	z.x = 0.0f;
	z.y = -fSX;
	z.z = fCX;
}

/*! ************************************

	\brief Initialize a rotation matrix with radians for roll (Z)

	\code
	float sz = Sin(fRoll);
	float cz = Cos(fRoll);
	\endcode

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cz</td>	<td>sz</td>	<td>0</td></tr>
	<tr><th>y</th><td>-sz</td>	<td>cz</td>	<td>0</td></tr>
	<tr><th>z</th><td>0</td>	<td>0</td>	<td>1</td></tr>
	</table>

	\param fRoll Roll of the Z in radians
	\sa SetRoll(float), TransposeSet(float,float,float), TransposeSetYaw(float) and TransposeSetPitch(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetRoll(float fRoll)
{
	float fSZ = Sin(fRoll);		// Get the sine/cosine
	float fCZ = Cos(fRoll);

	x.x = fCZ;
	x.y = fSZ;
	x.z = 0.0f;

	y.x = -fSZ;
	y.y = fCZ;
	y.z = 0.0f;

	z.x = 0.0f;
	z.y = 0.0f;
	z.z = 1.0f;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(cy*cz)+(-sy*sx*sz)</td><td>cx*sz</td><td>(sy*cz)+(cy*sx*sz)</td></tr>
	<tr><th>y</th><td>(-sy*sx*cz)+(cy*-sz)</td><td>cx*cz</td><td>(cy*sx*cz)+(sy*-sz)</td></tr>
	<tr><th>z</th><td>-sy*cx</td><td>-sx</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetYXZ(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetYXZ(float fYaw,float fPitch,float fRoll)
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

	y.x = (-fSY*fSXCZ)-(fCY*fSZ);
	y.y = fCX*fCZ;
	y.z = (fCY*fSXCZ)-(fSY*fSZ);

	z.x = -fSY*fCX;
	z.y = -fSX;
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>sz</td><td>sy*cz</td></tr>
	<tr><th>y</th><td>(cy*-sz*cx)+(-sy*sx)</td><td>cx*cz</td><td>(sy*-sz*cx)+(cy*sx)</td></tr>
	<tr><th>z</th><td>(-sy*cx)+(cy*-sx*-sz)</td><td>-sx*cz</td><td>(cy*cx)+(sy*-sx*-sz)</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetYZX(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetYZX(float fYaw,float fPitch,float fRoll)
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

	y.x = (fCY*fNegSZCX)-(fSY*fSX);
	y.y = fCX*fCZ;
	y.z = (fSY*fNegSZCX)+(fCY*fSX);

	z.x = (fCY*fSXSZ)-(fSY*fCX);
	z.y = -fSX*fCZ;
	z.z = (fCY*fCX)+(fSY*fSXSZ);
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(sy*-sx*cz)+(cx*sz)</td><td>(sy*cx*cz)+(sx*sz)</td></tr>
	<tr><th>y</th><td>cy*-sz</td><td>(cx*cz)+(sy*-sx*-sz)</td><td>(sx*cz)+(sy*cx*-sz)</td></tr>
	<tr><th>z</th><td>-sy</td><td>cy*-sx</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetXYZ(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetXYZ(float fYaw,float fPitch,float fRoll)
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

	y.x = fCY*-fSZ;
	y.y = (fCX*fCZ)+(fSYSX*fSZ);
	y.z = (fSX*fCZ)-(fSYCX*fSZ);

	z.x = -fSY;
	z.y = fCY*-fSX;
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>(cx*sz*cy)+(sy*-sx)</td><td>(sx*sz*cy)+(sy*cx)</td></tr>
	<tr><th>y</th><td>-sz</td><td>cx*cz</td><td>sx*cz</td></tr>
	<tr><th>z</th><td>-sy*cz</td><td>(-sx*cy)+(-sy*cx*sz)</td><td>(cx*cy)+(-sy*sx*sz)</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetXZY(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetXZY(float fYaw,float fPitch,float fRoll)
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

	y.x = -fSZ;
	y.y = fCX*fCZ;
	y.z = fSX*fCZ;

	z.x = -fSY*fCZ;
	z.y = (-fSX*fCY)-(fSYSZ*fCX);
	z.z = (fCX*fCY)-(fSYSZ*fSX);
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>cy*cz</td><td>cy*sz</td><td>sy</td></tr>
	<tr><th>y</th><td>(-sz*cx)+(-sy*sx*cz)</td><td>(cz*cx)+(-sy*sx*sz)</td><td>cy*sx</td></tr>
	<tr><th>z</th><td>(-sy*cz*cx)+(-sx*-sz)</td><td>(-sy*sz*cx)+(-sx*cz)</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetZYX(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetZYX(float fYaw,float fPitch,float fRoll)
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

	y.x = (-fSZ*fCX)-(fSYSX*fCZ);
	y.y = (fCZ*fCX)-(fSYSX*fSZ);
	y.z = fCY*fSX;

	z.x = (fSX*fSZ)+(fNegSYCX*fCZ);
	z.y = (fNegSYCX*fSZ)-(fSX*fCZ);
	z.z = fCY*fCX;
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
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(cz*cy)+(sy*-sx*-sz)</td><td>(sz*cy)+(sy*-sx*cz)</td><td>sy*cx</td></tr>
	<tr><th>y</th><td>cx*-sz</td><td>cx*cz</td><td>sx</td></tr>
	<tr><th>z</th><td>(-sx*-sz*cy)+(-sy*cz)</td><td>(-sx*cz*cy)+(-sy*sz)</td><td>cy*cx</td></tr>
	</table>

	\param fYaw Yaw of the Y in radians
	\param fPitch Pitch of the X in radians
	\param fRoll Roll of the Z in radians
	\sa SetZXY(float,float,float), TransposeSetYaw(float), TransposeSetPitch(float), and TransposeSetRoll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetZXY(float fYaw,float fPitch,float fRoll)
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

	y.x = fCX*-fSZ;
	y.y = fCX*fCZ;
	y.z = fSX;

	z.x = (fSXSZ*fCY)-(fSY*fCZ);
	z.y = (-fSZ*fSY)-(fSXCZ*fCY);
	z.z = fCY*fCX;
}

/*! ************************************

	\brief Convert a quaternion to a transposed 3x3 matrix

	Here's the resulting matrix of math used using terms from the input quaternion

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>	<td>1-(2yy+2zz)</td>	<td>2xy-2wz</td>		<td>2xz+2wy</td></tr>
	<tr><th>y</th>	<td>2xy+2wz</td>		<td>1-(2xx+2zz)</td>	<td>2yz-2wx</td></tr>
	<tr><th>z</th>	<td>2xz-2wy</td>		<td>2yz+2wx</td>		<td>1-(2xx+2yy)</td></tr>
	</table>

	\param pInput Pointer to a normalized quaternion
	\sa Matrix4D_t::TransposeSetFromQuaternion(const Vector4D_t *pInput) or SetFromQuaternion(const Vector4D_t *pInput) 
	
***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeSetFromQuaternion(const Vector4D_t *pInput)
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

	// y.x = 2xy + 2wz

	x.y=q_2xy+q_2zw;

	// y.y = 1 - 2x^2 - 2z^2

	y.y=1.0f-q_2xx-q_2zz;

	// y.z = 2yz - 2wx

	z.y=q_2yz-q_2xw;

	// z.x = 2xz - 2wy

	x.z=q_2xz-q_2yw;

	// z.y = 2yz + 2wx

	y.z=q_2yz+q_2xw;

	// z.z = 1 - 2x^2 - 2y^2

	z.z=1.0f-q_2xx-q_2yy;
}

/*! ************************************

	\brief Create a 3D scale matrix

	Sets the x.x, y.y and z.z components to the input values
	and all others are set to 0.0f

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</td>	<td>fX</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>fY</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>fZ</td></tr>
	</table>

	\param fX new x.x component
	\param fY new y.y component
	\param fZ new z.z component
	\sa Identity(void) and Multiply(float,float,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::SetScale(float fX,float fY,float fZ)
{
	x.x = fX;
	x.y = 0.0f;
	x.z = 0.0f;
	y.x = 0.0f;
	y.y = fY;
	y.z = 0.0f;
	z.x = 0.0f;
	z.y = 0.0f;
	z.z = fZ;
}

/*! ************************************

	\brief Perform a matrix transposition.
	
	Swap the entries x.y and y.x, x.z and z.x, y,z and z.y 
	to convert a right handed matrix to a left handed
	one and vice versa.
	The matrix is assumed to be already initialized.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>xx</td>	<td>yx</td>	<td>zx</td></tr>
	<tr><th>y</th><td>xy</td>	<td>yy</td>	<td>zy</td></tr>
	<tr><th>z</th><td>xz</td>	<td>yz</td>	<td>zz</td></tr>
	</table>

	\sa Transpose(const Matrix3D_t *)

***************************************/

void BURGER_API Burger::Matrix3D_t::Transpose(void)
{
	float fTemp1 = x.y;	// Swap x.y and y.x
	float fTemp2 = y.x;
	x.y = fTemp2;
	y.x = fTemp1;

	fTemp1 = x.z;		// Swap x.z and z.x
	fTemp2 = z.x;
	x.z = fTemp2;
	z.x = fTemp1;

	fTemp1 = y.z;		// Swap y.z and z.y
	fTemp2 = z.y;
	y.z = fTemp2;
	z.y = fTemp1;
}

/*! ************************************

	\brief Perform a matrix transposition.
	
	Swap the entries x.y and y.x, x.z and z.x, y,z and z.y 
	to convert a right handed matrix to a left handed
	one and vice versa. The other three entries are copied.
	The destination matrix is assumed to be uninitialized.
	
	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>xx</td>	<td>yx</td>	<td>zx</td></tr>
	<tr><th>y</th><td>xy</td>	<td>yy</td>	<td>zy</td></tr>
	<tr><th>z</th><td>xz</td>	<td>yz</td>	<td>zz</td></tr>
	</table>

	\note This function is optimized to write the new matrix in
	sequential order in the event this function is writing
	to write combined memory.

	\sa Transpose(void)

***************************************/

void BURGER_API Burger::Matrix3D_t::Transpose(const Matrix3D_t *pInput)
{
	float fTemp1 = pInput->x.x;	// I am copying the matrix, this 
	float fTemp2 = pInput->y.x;	// is why I am copying the x.x, y.y
	x.x = fTemp1;	// and z.z entries
	x.y = fTemp2;

	fTemp1 = pInput->z.x;
	fTemp2 = pInput->x.y;
	x.z = fTemp1;
	y.x = fTemp2;

	fTemp1 = pInput->y.y;
	fTemp2 = pInput->z.y;
	y.y = fTemp1;
	y.z = fTemp2;

	fTemp1 = pInput->x.z;
	fTemp2 = pInput->y.z;
	z.x = fTemp1;
	z.y = fTemp2;

	fTemp1 = pInput->z.z;	// Last one
	z.z = fTemp1;
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

void BURGER_API Burger::Matrix3D_t::GetXRow(Vector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = x.y;
	pOutput->z = x.z;
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

void BURGER_API Burger::Matrix3D_t::GetYRow(Vector3D_t *pOutput) const
{
	pOutput->x = y.x;
	pOutput->y = y.y;
	pOutput->z = y.z;
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

void BURGER_API Burger::Matrix3D_t::GetZRow(Vector3D_t *pOutput) const
{
	pOutput->x = z.x;
	pOutput->y = z.y;
	pOutput->z = z.z;
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

void BURGER_API Burger::Matrix3D_t::GetXColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = y.x;
	pOutput->z = z.x;
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

void BURGER_API Burger::Matrix3D_t::GetYColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.y;
	pOutput->y = y.y;
	pOutput->z = z.y;
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

void BURGER_API Burger::Matrix3D_t::GetZColumn(Vector3D_t *pOutput) const
{
	pOutput->x = x.z;
	pOutput->y = y.z;
	pOutput->z = z.z;
}

/*! ************************************

	\fn void Burger::Matrix3D_t::SetXRow(const Vector3D_t *pInput)
	\brief Set the X row of a matrix
	
	Overwrite the X row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new X row
	\sa SetXRow(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetXRow(float fX,float fY,float fZ)
	\brief Set the X row of a matrix
	
	Overwrite the X row of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>fX</td>	<td>fY</td>	<td>fZ</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param fX New x.x value
	\param fY New x.y value
	\param fZ New x.z value
	\sa SetXRow(const Vector3D_t *)

***************************************/


/*! ************************************

	\fn void Burger::Matrix3D_t::SetYRow(const Vector3D_t *pInput)
	\brief Set the Y row of a matrix
	
	Overwrite the Y row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Y row
	\sa SetYRow(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetYRow(float fX,float fY,float fZ)
	\brief Set the Y row of a matrix
	
	Overwrite the Y row of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>fX</td>	<td>fY</td>	<td>fZ</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param fX New y.x value
	\param fY New y.y value
	\param fZ New y.z value
	\sa SetYRow(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetZRow(const Vector3D_t *pInput)
	\brief Set the Z row of a matrix

	Overwrite the Z row of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>pInput->x</td>	<td>pInput->y</td>	<td>pInput->z</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Z row
	\sa SetZRow(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetZRow(float fX,float fY,float fZ)
	\brief Set the Z row of a matrix
	
	Overwrite the Z row of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>fX</td>	<td>fY</td>	<td>fZ</td></tr>
	</table>

	\param fX New z.x value
	\param fY New z.y value
	\param fZ New z.z value
	\sa SetZRow(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetXColumn(const Vector3D_t *pInput)
	\brief Set the X column of a matrix
	
	Overwrite the X column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>pInput->x</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>pInput->y</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>pInput->z</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new X column
	\sa SetXColumn(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetXColumn(float fX,float fY,float fZ)
	\brief Set the X column of a matrix
	
	Overwrite the X column of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>fX</td>	<td>x.y</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>fY</td>	<td>y.y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>fZ</td>	<td>z.y</td>	<td>z.z</td></tr>
	</table>

	\param fX New x.x value
	\param fY New y.x value
	\param fZ New z.x value
	\sa SetXColumn(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetYColumn(const Vector3D_t *pInput)
	\brief Set the Y column of a matrix
	
	Overwrite the Y column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td><td>pInput->x</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>y.x</td><td>pInput->y</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>z.x</td><td>pInput->z</td>	<td>z.z</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Y column
	\sa SetYColumn(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetYColumn(float fX,float fY,float fZ)
	\brief Set the Y column of a matrix
	
	Overwrite the Y column of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>fX</td>	<td>x.z</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>fY</td>	<td>y.z</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>fZ</td>	<td>z.z</td></tr>
	</table>

	\param fX New x.y value
	\param fY New y.y value
	\param fZ New z.y value
	\sa SetYColumn(const Vector3D_t *)

***************************************/


/*! ************************************

	\fn void Burger::Matrix3D_t::SetZColumn(const Vector3D_t *pInput)
	\brief Set the Z column of a matrix
	
	Overwrite the Z column of the matrix with the input vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td><td>x.y</td><td>pInput->x</td>	</tr>
	<tr><th>y</th><td>y.x</td><td>y.y</td><td>pInput->y</td>	</tr>
	<tr><th>z</th><td>z.x</td><td>z.y</td><td>pInput->z</td>	</tr>
	</table>

	\param pInput Pointer to a Vector3D_t structure that has the new Z column
	\sa SetZColumn(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Matrix3D_t::SetZColumn(float fX,float fY,float fZ)
	\brief Set the Z column of a matrix
	
	Overwrite the Z column of the matrix with the input values

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td>	<td>x.y</td>	<td>fX</td></tr>
	<tr><th>y</th><td>y.x</td>	<td>y.y</td>	<td>fY</td></tr>
	<tr><th>z</th><td>z.x</td>	<td>z.y</td>	<td>fZ</td></tr>
	</table>

	\param fX New x.z value
	\param fY New y.z value
	\param fZ New z.z value
	\sa SetZColumn(const Vector3D_t *)

***************************************/

/*! ************************************

	\brief Perform a matrix multiply against this matrix

	Multiply this matrix against another one

	Use this formula to create the final matrix, this matrix is
	matrix #1 and the input matrix is matrix #2

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(xx1*xx2)+(yx1*xy2)+(zx1*xz2)</td>
	<td>(xy1*xx2)+(yy1*xy2)+(zy1*xz2)</td>
	<td>(xz1*xx2)+(yz1*xy2)+(zz1*xz2)</td>
	</tr>
	<tr><th>y</th>
	<td>(xx1*yx2)+(yx1*yy2)+(zx1*yz2)</td>
	<td>(xy1*yx2)+(yy1*yy2)+(zy1*yz2)</td>
	<td>(xz1*yx2)+(yz1*yy2)+(zz1*yz2)</td>
	</tr>
	<tr><th>z</th>
	<td>(xx1*zx2)+(yx1*zy2)+(zx1*zz2)</td>
	<td>(xy1*zx2)+(yy1*zy2)+(zy1*zz2)</td>
	<td>(xz1*zx2)+(yz1*zy2)+(zz1*zz2)</td>
	</tr>
	</table>

	\param pInput Matrix to multiply against
	\sa Multiply(const Matrix3D_t *,const Matrix3D_t *)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(const Matrix3D_t *pInput)
{
	float fXX=(x.x*pInput->x.x)+(y.x*pInput->x.y)+(z.x*pInput->x.z);
	float fXY=(x.y*pInput->x.x)+(y.y*pInput->x.y)+(z.y*pInput->x.z);
	float fXZ=(x.z*pInput->x.x)+(y.z*pInput->x.y)+(z.z*pInput->x.z);

	float fYX=(x.x*pInput->y.x)+(y.x*pInput->y.y)+(z.x*pInput->y.z);
	float fYY=(x.y*pInput->y.x)+(y.y*pInput->y.y)+(z.y*pInput->y.z);
	float fYZ=(x.z*pInput->y.x)+(y.z*pInput->y.y)+(z.z*pInput->y.z);

	float fZX=(x.x*pInput->z.x)+(y.x*pInput->z.y)+(z.x*pInput->z.z);
	float fZY=(x.y*pInput->z.x)+(y.y*pInput->z.y)+(z.y*pInput->z.z);
	float fZZ=(x.z*pInput->z.x)+(y.z*pInput->z.y)+(z.z*pInput->z.z);
	x.x = fXX;
	x.y = fXY;
	x.z = fXZ;
	y.x = fYX;
	y.y = fYY;
	y.z = fYZ;
	z.x = fZX;
	z.y = fZY;
	z.z = fZZ;
}

/*! ************************************

	\brief Perform a matrix multiply

	Multiply two matrices together and store the result in this matrix.

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(xx1*xx2)+(yx1*xy2)+(zx1*xz2)</td>
	<td>(xy1*xx2)+(yy1*xy2)+(zy1*xz2)</td>
	<td>(xz1*xx2)+(yz1*xy2)+(zz1*xz2)</td>
	</tr>
	<tr><th>y</th>
	<td>(xx1*yx2)+(yx1*yy2)+(zx1*yz2)</td>
	<td>(xy1*yx2)+(yy1*yy2)+(zy1*yz2)</td>
	<td>(xz1*yx2)+(yz1*yy2)+(zz1*yz2)</td>
	</tr>
	<tr><th>z</th>
	<td>(xx1*zx2)+(yx1*zy2)+(zx1*zz2)</td>
	<td>(xy1*zx2)+(yy1*zy2)+(zy1*zz2)</td>
	<td>(xz1*zx2)+(yz1*zy2)+(zz1*zz2)</td>
	</tr>
	</table>

	\note This function is optimized to reduce variable usage by storing into the output
		without buffering. Do not use the output matrix as an input matrix.

	\param pInput1 Matrix to multiply from
	\param pInput2 Matrix to multiply against
	\sa Multiply(const Matrix3D_t *)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(const Matrix3D_t *pInput1,const Matrix3D_t *pInput2)
{
	x.x=(pInput1->x.x*pInput2->x.x)+(pInput1->y.x*pInput2->x.y)+(pInput1->z.x*pInput2->x.z);
	x.y=(pInput1->x.y*pInput2->x.x)+(pInput1->y.y*pInput2->x.y)+(pInput1->z.y*pInput2->x.z);
	x.z=(pInput1->x.z*pInput2->x.x)+(pInput1->y.z*pInput2->x.y)+(pInput1->z.z*pInput2->x.z);

	y.x=(pInput1->x.x*pInput2->y.x)+(pInput1->y.x*pInput2->y.y)+(pInput1->z.x*pInput2->y.z);
	y.y=(pInput1->x.y*pInput2->y.x)+(pInput1->y.y*pInput2->y.y)+(pInput1->z.y*pInput2->y.z);
	y.z=(pInput1->x.z*pInput2->y.x)+(pInput1->y.z*pInput2->y.y)+(pInput1->z.z*pInput2->y.z);

	z.x=(pInput1->x.x*pInput2->z.x)+(pInput1->y.x*pInput2->z.y)+(pInput1->z.x*pInput2->z.z);
	z.y=(pInput1->x.y*pInput2->z.x)+(pInput1->y.y*pInput2->z.y)+(pInput1->z.y*pInput2->z.z);
	z.z=(pInput1->x.z*pInput2->z.x)+(pInput1->y.z*pInput2->z.y)+(pInput1->z.z*pInput2->z.z);
}

#if 0
#define Output ECX
#define Input1 EDX
#define Input2 EAX

// Floating point will rot your brain on the Pentium

BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::Multiply(const Matrix3D_t *pInput1,const Matrix3D_t *pInput2)
{
	BURGER_ASM {

	mov	Input2,[esp+4]

;
; First I do the Muls for XX
;

	fld	dword ptr [Input1+XX]	;XX
	fmul	dword ptr [Input2+XX]
	fld	dword ptr [Input1+YX]	;XX XX
	fmul	dword ptr [Input2+XY]
	fld	dword ptr [Input1+ZX]	;XX XX XX
	fmul	dword ptr [Input2+XZ]
	fxch	st(1)
	faddp	st(2),st		;XX XX

;
; As I perform the Muls for XY
; I interleave the adds for XX
; to prevent any lame matrix multiply stalls
;

	fld	dword ptr [Input1+XY]	;XY XX XX
	fmul	dword ptr [Input2+XX]
	fxch	st(2)			;XX XX XY
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YY]
	fmul	dword ptr [Input2+XY]
	fld	dword ptr [Input1+ZY]	;XY XY XX XY
	fmul	dword ptr [Input2+XZ]
	fxch	st(2)
	fstp	dword ptr [Output+XX]	;Store result
	faddp	st(2),st		;XY XY

;
; As you see above, I have a pattern,
; Follow the pattern to its logical conclusion
;

; XZ

	fld	dword ptr [Input1+XZ]	;XZ XY XY
	fmul	dword ptr [Input2+XX]
	fxch	st(2)			;XY XY XZ
	faddp	st(1),st			;Second add
	fld	dword ptr [Input1+YZ]	;XZ XY XZ
	fmul	dword ptr [Input2+XY]
	fld	dword ptr [Input1+ZZ]	;XZ XZ XY XZ
	fmul	dword ptr [Input2+XZ]
	fxch	st(2)			;XY XZ XZ XZ
	fstp	dword ptr [Output+XY]	;Store result
	faddp	st(2),st		;XZ XZ

;YX

	fld	dword ptr [Input1+XX]	;YX XZ XZ
	fmul	dword ptr [Input2+YX]
	fxch	st(2)			;XZ XZ YX
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YX]
	fmul	dword ptr [Input2+YY]
	fld	dword ptr [Input1+ZX]	;YX YX XZ YX
	fmul	dword ptr [Input2+YZ]
	fxch	st(2)			;XZ YX YX YX
	fstp	dword ptr [Output+XZ]	;Store result
	faddp	st(2),st		;YX YX

;YY

	fld	dword ptr [Input1+XY]	;YY YX YX
	fmul	dword ptr [Input2+YX]
	fxch	st(2)			;YX YX YY
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YY]
	fmul	dword ptr [Input2+YY]
	fld	dword ptr [Input1+ZY]	;YY YY YX YY
	fmul	dword ptr [Input2+YZ]
	fxch	st(2)
	fstp	dword ptr [Output+YX]	;Store result
	faddp	st(2),st		;YY YY

;YZ

	fld	dword ptr [Input1+XZ]	;YZ YY YY
	fmul	dword ptr [Input2+YX]
	fxch	st(2)			;YY YY YZ
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YZ]
	fmul	dword ptr [Input2+YY]
	fld	dword ptr [Input1+ZZ]	;YZ YZ YY YZ
	fmul	dword ptr [Input2+YZ]
	fxch	st(2)			;YY YZ YZ YZ
	fstp	dword ptr [Output+YY]	;Store result
	faddp	st(2),st		;YZ YZ

;
; Now I do not save, I keep them on the stack
; to eat latency cycles at the end of the routine
;

;ZX

	fld	dword ptr [Input1+XX]	;ZX YZ YZ
	fmul	dword ptr [Input2+ZX]
	fxch	st(2)			;YZ YZ ZX
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YX]
	fmul	dword ptr [Input2+ZY]
	fld	dword ptr [Input1+ZX]	;ZX ZX YZ ZX
	fmul	dword ptr [Input2+ZZ]
	fxch	st(1)
	faddp	st(3),st		;ZX YZ ZX

;ZY

	fld	dword ptr [Input1+XY]	;ZY ZX YZ ZX
	fmul	dword ptr [Input2+ZX]
	fxch	st(3)			;ZX ZX YZ ZY
	faddp	st(1),st		;Second add
	fld	dword ptr [Input1+YY]
	fmul	dword ptr [Input2+ZY]
	fld	dword ptr [Input1+ZY]	;ZY ZY ZX YZ ZY
	fmul	dword ptr [Input2+ZZ]
	fxch	st(1)
	faddp	st(4),st		;ZY ZX YZ ZY

;ZZ

	fld	dword ptr [Input1+XZ]	;ZZ ZY ZX YZ ZY
	fmul	dword ptr [Input2+ZX]
	fxch	st(4)			;ZY ZY ZX YZ ZZ
	faddp	st(1),st		;Second add
	fxch	st(3)			;ZZ ZX YZ ZY
	fld	dword ptr [Input1+YZ]	;ZZ ZZ ZX YZ ZY
	fmul	dword ptr [Input2+ZY]
	fld	dword ptr [Input1+ZZ]	;ZZ ZZ ZZ ZX YZ ZY
	fmul	dword ptr [Input2+ZZ]
	fxch	st(1)
	faddp	st(2),st		;ZZ ZZ ZX YZ ZY
	fxch	st(2)			;ZX ZZ ZZ YZ ZY


; Since I can not interleave anymore I have to
; eat the 2 cycle + 2 cycle stall

	fstp	dword ptr [Output+ZX]	;Store result
	faddp	st(1),st		;ZZ YZ ZY
	fxch	st(2)			;ZY YZ ZZ
	fstp	dword ptr [Output+ZY]	;Store result
	fstp	dword ptr [Output+YZ]	;Store result
	fstp	dword ptr [Output+ZZ]	;Store result
	ret		4
	}
}

#undef Output
#undef Input1
#undef Input2
#endif

/*! ************************************

	\brief Multiply all entries by a scalar

	Multiply all values in the matrix by a scalar constant

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScale)</td>
	<td>(x.y*fScale)</td>
	<td>(x.z*fScale)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScale)</td>
	<td>(y.y*fScale)</td>
	<td>(y.z*fScale)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScale)</td>
	<td>(z.y*fScale)</td>
	<td>(z.z*fScale)</td>
	</tr>
	</table>

	\param fScale Scalar to multiply all entries by
	\sa Multiply(const Matrix3D_t *,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(float fScale)
{
	x.x *= fScale;
	x.y *= fScale;
	x.z *= fScale;
	y.x *= fScale;
	y.y *= fScale;
	y.z *= fScale;
	z.x *= fScale;
	z.y *= fScale;
	z.z *= fScale;
}

/*! ************************************

	\brief Initialize with a matrix multiplied by a scalar

	Multiply all values of the matrix by a scalar constant
	and store the result in this matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScale)</td>
	<td>(pInput->x.y*fScale)</td>
	<td>(pInput->x.z*fScale)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScale)</td>
	<td>(pInput->y.y*fScale)</td>
	<td>(pInput->y.z*fScale)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScale)</td>
	<td>(pInput->z.y*fScale)</td>
	<td>(pInput->z.z*fScale)</td>
	</tr>
	</table>

	\param pInput Pointer to Matrix to multiply
	\param fScale Scalar to multiply all entries by
	\sa Multiply(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(const Matrix3D_t *pInput,float fScale)
{
	x.x = pInput->x.x * fScale;
	x.y = pInput->x.y * fScale;
	x.z = pInput->x.z * fScale;
	y.x = pInput->y.x * fScale;
	y.y = pInput->y.y * fScale;
	y.z = pInput->y.z * fScale;
	z.x = pInput->z.x * fScale;
	z.y = pInput->z.y * fScale;
	z.z = pInput->z.z * fScale;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleX)</td>
	<td>(x.z*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleY)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleZ)</td>
	<td>(z.y*fScaleZ)</td>
	<td>(z.z*fScaleZ)</td>
	</tr>
	</table>

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa Multiply(const Matrix3D_t *,float,float,float), TransposeMultiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(float fScaleX,float fScaleY,float fScaleZ)
{
	x.x = x.x * fScaleX;
	x.y = x.y * fScaleX;
	x.z = x.z * fScaleX;
	y.x = y.x * fScaleY;
	y.y = y.y * fScaleY;
	y.z = y.z * fScaleY;
	z.x = z.x * fScaleZ;
	z.y = z.y * fScaleZ;
	z.z = z.z * fScaleZ;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale into a copy

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix3D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleX)</td>
	<td>(pInput->x.z*fScaleX)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleY)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleY)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleZ)</td>
	<td>(pInput->z.y*fScaleZ)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	</tr>
	</table>
	
	\param pInput Pointer to a valid Matrix3D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa Multiply(float,float,float), TransposeMultiply(const Matrix3D_t*,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Multiply(const Matrix3D_t *pInput,float fScaleX,float fScaleY,float fScaleZ)
{
	x.x = pInput->x.x * fScaleX;
	x.y = pInput->x.y * fScaleX;
	x.z = pInput->x.z * fScaleX;
	y.x = pInput->y.x * fScaleY;
	y.y = pInput->y.y * fScaleY;
	y.z = pInput->y.z * fScaleY;
	z.x = pInput->z.x * fScaleZ;
	z.y = pInput->z.y * fScaleZ;
	z.z = pInput->z.z * fScaleZ;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a transposed matrix multiply quickly
	that would yield a multiplication vs a scale matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(x.x*fScaleX)</td>
	<td>(x.y*fScaleY)</td>
	<td>(x.z*fScaleZ)</td>
	</tr>
	<tr><th>y</th>
	<td>(y.x*fScaleX)</td>
	<td>(y.y*fScaleY)</td>
	<td>(y.z*fScaleZ)</td>
	</tr>
	<tr><th>z</th>
	<td>(z.x*fScaleX)</td>
	<td>(z.y*fScaleY)</td>
	<td>(z.z*fScaleZ)</td>
	</tr>
	</table>

	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa TransposeMultiply(const Matrix3D_t *,float,float,float), Multiply(float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeMultiply(float fScaleX,float fScaleY,float fScaleZ)
{
	x.x = x.x * fScaleX;
	x.y = x.y * fScaleX;
	x.z = x.z * fScaleX;
	y.x = y.x * fScaleY;
	y.y = y.y * fScaleY;
	y.z = y.z * fScaleY;
	z.x = z.x * fScaleZ;
	z.y = z.y * fScaleZ;
	z.z = z.z * fScaleZ;
}

/*! ************************************

	\brief Multiply by an X,Y and Z scale into a copy

	Using only the x.x, y.y, and z.z components of a
	simulated matrix, perform a transposed matrix multiply quickly
	that would yield a multiplication vs a scale matrix
	and store the result in this Matrix3D_t

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(pInput->x.x*fScaleX)</td>
	<td>(pInput->x.y*fScaleY)</td>
	<td>(pInput->x.z*fScaleZ)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->y.x*fScaleX)</td>
	<td>(pInput->y.y*fScaleY)</td>
	<td>(pInput->y.z*fScaleZ)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->z.x*fScaleX)</td>
	<td>(pInput->z.y*fScaleY)</td>
	<td>(pInput->z.z*fScaleZ)</td>
	</tr>
	</table>
	
	\param pInput Pointer to a valid Matrix3D_t to apply the scale to
	\param fScaleX X scale value
	\param fScaleY Y scale value
	\param fScaleZ Z scale value
	\sa TransposeMultiply(const Matrix3D_t *,float,float,float), Multiply(const Matrix3D_t *,float,float,float) or SetScale(float,float,float)

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeMultiply(const Matrix3D_t *pInput,float fScaleX,float fScaleY,float fScaleZ)
{
	x.x = pInput->x.x * fScaleX;
	x.y = pInput->x.y * fScaleY;
	x.z = pInput->x.z * fScaleZ;
	y.x = pInput->y.x * fScaleX;
	y.y = pInput->y.y * fScaleY;
	y.z = pInput->y.z * fScaleZ;
	z.x = pInput->z.x * fScaleX;
	z.y = pInput->z.y * fScaleY;
	z.z = pInput->z.z * fScaleZ;
}

/*! ************************************

	\brief Multiply a vector by a matrix

	Transform the point by the matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\sa Transform(Vector3D_t *,const Vector3D_t *) const or TransposeTransform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::Transform(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + x.y*fY + x.z*fZ;
	pInput->y=y.x*fX + y.y*fY + y.z*fZ;
	pInput->z=z.x*fX + z.y*fY + z.z*fZ;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::Transform(Vector3D_t *pInput) const
{
	BURGER_ASM {
	fld		dword ptr [edx+XX]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+XY]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+XZ]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+YX]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+YY]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+YZ]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+ZX]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+ZY]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+ZZ]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(5)
	faddp	st(4),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(3),st
	fxch	st(1)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret
	}
}
#endif

/*! ************************************

	\brief Multiply a vector by a matrix

	Transform the point by the matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa Transform(Vector3D_t *) const or TransposeTransform(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::Transform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + x.y*fY + x.z*fZ;
	pOutput->y=y.x*fX + y.y*fY + y.z*fZ;
	pOutput->z=z.x*fX + z.y*fY + z.z*fZ;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::Transform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	BURGER_ASM {
	mov		eax,[esp+4]
	fld		dword ptr [edx+XX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+XY]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+XZ]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+YX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+YY]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+YZ]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+ZX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+ZY]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+ZZ]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(5)
	faddp	st(4),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(3),st
	fxch	st(1)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		4
	}
}
#endif

/*! ************************************

	\brief Transform a vector and then add a point

	Transform the point by the matrix and then add a translation vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+tx</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+ty</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+tz</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\param pTranslate Pointer to a Vector3D_t to add against the translated input
	\sa TransformAdd(Vector3D_t *,const Vector3D_t *,const Vector3D_t *) const or TransposeTransformAdd(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	float fX = pInput->x;		// Copy the struct
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=(x.x*fX) + (x.y*fY) + (x.z*fZ)+pTranslate->x;
	pInput->y=(y.x*fX) + (y.y*fY) + (y.z*fZ)+pTranslate->y;
	pInput->z=(z.x*fX) + (z.y*fY) + (z.z*fZ)+pTranslate->z;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	BURGER_ASM {
	mov		eax,[esp+4]
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+XX]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+XY]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+XZ]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+YX]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+YY]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+YZ]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+ZX]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+ZY]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+ZZ]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(4)
	faddp	st(5),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(2),st
	fxch	st(2)
	fadd	dword ptr [eax+0]
	fxch	st(2)
	fadd	dword ptr [eax+4]
	fxch	st(1)
	fadd	dword ptr [eax+8]
	fxch	st(2)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		4
	}
}
#endif

/*! ************************************

	\brief Transform a vector and then add a point

	Transform the point by the matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(xy*y)+(xz*z)+tx</td></tr>
	<tr><th>y</th><td>(yx*x)+(yy*y)+(yz*z)+ty</td></tr>
	<tr><th>z</th><td>(zx*x)+(zy*y)+(zz*z)+tz</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\param pTranslate Pointer to a Vector3D_t to add against the translated input
	\sa TransformAdd(Vector3D_t *,const Vector3D_t *) const or TransposeTransformAdd(Vector3D_t *,const Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	float fX = pInput->x;		// Copy the struct
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=(x.x*fX) + (x.y*fY) + (x.z*fZ)+pTranslate->x;
	pOutput->y=(y.x*fX) + (y.y*fY) + (y.z*fZ)+pTranslate->y;
	pOutput->z=(z.x*fX) + (z.y*fY) + (z.z*fZ)+pTranslate->z;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	BURGER_ASM {
	mov		eax,[esp+8]
	fld		dword ptr [edx+XX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+XY]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+XZ]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [eax+0]
	fmul	dword ptr [edx+YX]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+YY]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+YZ]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [eax+0]
	fmul	dword ptr [edx+ZX]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+ZY]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+ZZ]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(4)
	faddp	st(5),st
	fxch	st(1)
	faddp	st(2),st
	mov		eax,[esp+4]
	faddp	st(2),st
	fxch	st(2)
	fadd	dword ptr [eax+0]
	fxch	st(2)
	fadd	dword ptr [eax+4]
	fxch	st(1)
	fadd	dword ptr [eax+8]
	fxch	st(2)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		8
	}
}

#endif


/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\sa TransposeTransform(Vector3D_t *,const Vector3D_t *) const or Transform(Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeTransform(Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pInput->x=x.x*fX + y.x*fY + z.x*fZ;
	pInput->y=x.y*fX + y.y*fY + z.y*fZ;
	pInput->z=x.z*fX + y.z*fY + z.z*fZ;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransposeTransform(Vector3D_t *pInput) const
{
	BURGER_ASM {
	fld		dword ptr [edx+XX]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+YX]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+ZX]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+XY]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+YY]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+ZY]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+XZ]
	fmul	dword ptr [ecx+0]
	fld		dword ptr [edx+YZ]
	fmul	dword ptr [ecx+4]
	fld		dword ptr [edx+ZZ]
	fmul	dword ptr [ecx+8]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(5)
	faddp	st(4),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(3),st
	fxch	st(1)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret
	}
}
#endif

/*! ************************************

	\brief Multiply a vector by a transposed matrix

	Transform the point by the transposed matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\sa TransposeTransform(Vector3D_t *) const or Transform(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeTransform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	pOutput->x=x.x*fX + y.x*fY + z.x*fZ;
	pOutput->y=x.y*fX + y.y*fY + z.y*fZ;
	pOutput->z=x.z*fX + y.z*fY + z.z*fZ;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransposeTransform(Vector3D_t *pOutput,const Vector3D_t *pInput) const
{
	BURGER_ASM {
	mov		eax,[esp+4]
	fld		dword ptr [edx+XX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+YX]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+ZX]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+XY]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+YY]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+ZY]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [edx+XZ]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+YZ]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+ZZ]
	fmul	dword ptr [eax+8]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(5)
	faddp	st(4),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(3),st
	fxch	st(1)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		4
	}
}
#endif

/*! ************************************

	\brief Transform a vector and then add a point

	Transform the point by the transposed matrix and then add a translation vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+tx</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+ty</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+tz</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\param pTranslate Pointer to a Vector3D_t to add against the translated input
	\sa TransposeTransformAdd(Vector3D_t *,const Vector3D_t *,const Vector3D_t *) const or TransformAdd(Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeTransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	float fX = pInput->x;		// Copy the struct
	float fY = pInput->y;
	float fZ = pInput->z;;
	pInput->x=(x.x*fX) + (y.x*fY) + (z.x*fZ)+pTranslate->x;
	pInput->y=(x.y*fX) + (y.y*fY) + (z.y*fZ)+pTranslate->y;
	pInput->z=(x.z*fX) + (y.z*fY) + (z.z*fZ)+pTranslate->z;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransposeTransformAdd(Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	BURGER_ASM {
	mov		eax,[esp+4]
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+XX]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+YX]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+ZX]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+XY]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+YY]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+ZY]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+XZ]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+YZ]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+ZZ]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(4)
	faddp	st(5),st
	fxch	st(1)
	faddp	st(2),st
	faddp	st(2),st
	fxch	st(2)
	fadd	dword ptr [eax+0]
	fxch	st(2)
	fadd	dword ptr [eax+4]
	fxch	st(1)
	fadd	dword ptr [eax+8]
	fxch	st(2)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		4
	}
}
#endif

/*! ************************************

	\brief Transform a vector and then add a point

	Transform the point by the transposed matrix and store the result in an
	uninitialized Vector3D_t

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><td>(xx*x)+(yx*y)+(zx*z)+tx</td></tr>
	<tr><th>y</th><td>(xy*x)+(yy*y)+(zy*z)+ty</td></tr>
	<tr><th>z</th><td>(xz*x)+(yz*y)+(zz*z)+tz</td></tr>
	</table>

	\param pInput Pointer to a Vector3D_t to transform against this transposed matrix
	\param pOutput Pointer to an uninitialized Vector3D_t to store the result
	\param pTranslate Pointer to a Vector3D_t to add against the translated input
	\sa TransposeTransformAdd(Vector3D_t *,const Vector3D_t *) const or TransformAdd(Vector3D_t *,const Vector3D_t *,const Vector3D_t *) const

***************************************/

void BURGER_API Burger::Matrix3D_t::TransposeTransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	float fX = pInput->x;		// Copy the struct
	float fY = pInput->y;
	float fZ = pInput->z;;
	pOutput->x=(x.x*fX) + (y.x*fY) + (z.x*fZ)+pTranslate->x;
	pOutput->y=(x.y*fX) + (y.y*fY) + (z.y*fZ)+pTranslate->y;
	pOutput->z=(x.z*fX) + (y.z*fY) + (z.z*fZ)+pTranslate->z;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Matrix3D_t::TransposeTransformAdd(Vector3D_t *pOutput,const Vector3D_t *pInput,const Vector3D_t *pTranslate) const
{
	BURGER_ASM {
	mov		eax,[esp+8]
	fld		dword ptr [edx+XX]
	fmul	dword ptr [eax+0]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+YX]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+ZX]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [eax+0]
	fmul	dword ptr [edx+XY]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+YY]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+ZY]
	fxch	st(1)
	faddp	st(2),st
	fld		dword ptr [eax+0]
	fmul	dword ptr [edx+XZ]
	fld		dword ptr [eax+4]
	fmul	dword ptr [edx+YZ]
	fld		dword ptr [eax+8]
	fmul	dword ptr [edx+ZZ]
	fxch	st(1)
	faddp	st(2),st
	fxch	st(4)
	faddp	st(5),st
	fxch	st(1)
	faddp	st(2),st
	mov		eax,[esp+4]
	faddp	st(2),st
	fxch	st(2)
	fadd	dword ptr [eax+0]
	fxch	st(2)
	fadd	dword ptr [eax+4]
	fxch	st(1)
	fadd	dword ptr [eax+8]
	fxch	st(2)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	ret		8
	}
}
#endif

/*! ************************************
	
	\brief Perform an affine inverse

	Given an input matrix, perform an affine inverse and
	store it in this instance.

	\note Due to buffer use, do NOT perform this operation
	on the matrix itself!

	\param pInput Pointer to a matrix to obtain an inverse from
	\return \ref TRUE if the inverse was successful, \ref FALSE if failure

***************************************/

Word BURGER_API Burger::Matrix3D_t::AffineInverse(const Matrix3D_t *pInput)
{
	// Calculate the determinant of the input matrix and determine
	// if the the matrix is singular as limited by the float precision
	// floating-point data representation.

	float fPositive = 0.0f;
	float fNegative = 0.0f;

	float fTemp = pInput->x.x * pInput->y.y * pInput->z.z;
	if (fTemp >= 0.0F) {
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
	if (fTemp >= 0.0F) { 
		fPositive += fTemp;
	} else { 
		fNegative += fTemp; 
	}

	fTemp = -pInput->x.z * pInput->y.y * pInput->z.x;
	if (fTemp >= 0.0F) {
		fPositive += fTemp;
	} else {
		fNegative += fTemp; 
	}

	fTemp = -pInput->x.y * pInput->y.x * pInput->z.z;
	if (fTemp >= 0.0F) {
		fPositive += fTemp;
	} else { 
		fNegative += fTemp;
	}

	fTemp = -pInput->x.x * pInput->y.z * pInput->z.y;
	if (fTemp >= 0.0F) {
		fPositive += fTemp;
	} else {
		fNegative += fTemp; 
	}

	float fDeterminate = fPositive + fNegative;

	// Is the matrix singular?

	if((fDeterminate == 0.0f) || (Abs(fDeterminate / (fPositive - fNegative)) < (1.0e-15f))) {
		// There is no scale, not possible to convert
		Identity();
		// Matrix4 M has no inverse
		return FALSE;
	}
	//  Calculate inverse(A) = adj(A) / det(A)
	fDeterminate = 1.0F / fDeterminate;

	x.x =  (pInput->y.y * pInput->z.z - pInput->y.z * pInput->z.y) * fDeterminate;
	x.y = -(pInput->x.y * pInput->z.z - pInput->x.z * pInput->z.y) * fDeterminate;
	x.z =  (pInput->x.y * pInput->y.z - pInput->x.z * pInput->y.y) * fDeterminate;
	y.x = -(pInput->y.x * pInput->z.z - pInput->y.z * pInput->z.x) * fDeterminate;
	y.y =  (pInput->x.x * pInput->z.z - pInput->x.z * pInput->z.x) * fDeterminate;
	y.z = -(pInput->x.x * pInput->y.z - pInput->x.z * pInput->y.x) * fDeterminate;
	z.x =  (pInput->y.x * pInput->z.y - pInput->y.y * pInput->z.x) * fDeterminate;
	z.y = -(pInput->x.x * pInput->z.y - pInput->x.y * pInput->z.x) * fDeterminate;
	z.z =  (pInput->x.x * pInput->y.y - pInput->x.y * pInput->y.x) * fDeterminate;
	return TRUE;
}

/*! ************************************
	
	\brief Rotate a matrix in the Y axis (Yaw)

	Given a Y angle in radians, rotate the matrix accordingly

	fCos = Cos(fYaw); fSin = Sin(fYaw);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(z.x*fSin)+(x.x*fCos)</td><td>(z.y*fSin)+(x.y*fCos)</td><td>(z.z*fSin)+(x.z*fCos)</td></tr>
	<tr><th>y</th><td>y.x</td><td>y.y</td><td>y.z</td></tr>
	<tr><th>z</th><td>(z.x*fCos)-(x.x*fSin)</td><td>(z.y*fCos)-(x,y*fSin)</td><td>(z.z*fCos)-(x.z*fSin)</td></tr>
	</table>

	\param fYaw Angle in radians to yaw the matrix
	\sa Pitch(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Yaw(float fYaw)
{
	float fCos = Cos(fYaw);
	float fSin = Sin(fYaw);

	float fXX = x.x;
	float fXY = x.y;
	float fXZ = x.z;

	x.x = (z.x*fSin)+(fXX*fCos);
	x.y = (z.y*fSin)+(fXY*fCos);
	x.z = (z.z*fSin)+(fXZ*fCos);

	z.x = (z.x*fCos)-(fXX*fSin);
	z.y = (z.y*fCos)-(fXY*fSin);
	z.z = (z.z*fCos)-(fXZ*fSin);
}

/*! ************************************
	
	\brief Rotate a matrix in the X axis (Pitch)

	Given a X angle in radians, rotate the matrix accordingly

	fCos = Cos(fPitch); fSin = Sin(fPitch);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>x.x</td><td>x.y</td><td>x.z</td></tr>
	<tr><th>y</th><td>(z.x*fSin)+(y.x*fCos)</td><td>(z.y*fSin)+(y.y*fCos)</td><td>(z.z*fSin)+(y.z*fCos)</td></tr>
	<tr><th>z</th><td>(z.x*fCos)-(y.x*fSin)</td><td>(z.y*fCos)-(y.y*fSin)</td><td>(z.z*fCos)-(y.z*fSin)</td></tr>
	</table>

	\param fPitch Angle in radians to pitch the matrix
	\sa Yaw(float) or Roll(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Pitch(float fPitch)
{
	float fCos = Cos(fPitch);
	float fSin = Sin(fPitch);

	float fYX = y.x;
	float fYY = y.y;
	float fYZ = y.z;

	y.x = (z.x*fSin)+(fYX*fCos);
	y.y = (z.y*fSin)+(fYY*fCos);
	y.z = (z.z*fSin)+(fYZ*fCos);

	z.x = (z.x*fCos)-(fYX*fSin);
	z.y = (z.y*fCos)-(fYY*fSin);
	z.z = (z.z*fCos)-(fYZ*fSin);
}

/*! ************************************
	
	\brief Rotate a matrix in the Z axis (Roll)

	Given a Z angle in radians, rotate the matrix accordingly

	fCos = Cos(fRoll); fSin = Sin(fRoll);

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th><td>(y.x*fSin)+(x.x*fCos)</td><td>(y.y*fSin)+(x.y*fCos)</td><td>(y.z*fSin)+(x.z*fCos)</td></tr>
	<tr><th>y</th><td>(y.x*fCos)-(x.x*fSin)</td><td>(y.y*fCos)-(x.y*fSin)</td><td>(y.z*fCos)-(x.z*fSin)</td></tr>
	<tr><th>z</th><td>z.x</td><td>z.y</td><td>z.z</td></tr>
	</table>

	\param fRoll Angle in radians to roll the matrix
	\sa Yaw(float) or Pitch(float)

***************************************/

void BURGER_API Burger::Matrix3D_t::Roll(float fRoll)
{
	float fCos = Cos(fRoll);
	float fSin = Sin(fRoll);

	float fXX = x.x;
	float fXY = x.y;
	float fXZ = x.z;

	x.x = (y.x*fSin)+(fXX*fCos);
	x.y = (y.y*fSin)+(fXY*fCos);
	x.z = (y.z*fSin)+(fXZ*fCos);

	y.x = (y.x*fCos)-(fXX*fSin);
	y.y = (y.y*fCos)-(fXY*fSin);
	y.z = (y.z*fCos)-(fXZ*fSin);
}

/*! ************************************

	\fn Burger::Matrix3D_t::operator const float *() const
	\brief Convert to a const float pointer

	This convenience operator converts the Matrix3D_t into 
	a float pointer to pass to other APIs that treat this as
	an array of 32 bit floats.

	\return The pointer to the object typecast as a (const float *)

***************************************/


/*! ************************************

	\brief Constant 3x3 identity matrix

***************************************/

const Burger::Matrix3D_t Burger::g_Matrix3DIdentity = {
	{1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f}
};