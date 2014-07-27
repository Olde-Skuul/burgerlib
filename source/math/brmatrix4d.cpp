/***************************************

	4D Floating point matrix manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
	x.w  = 0.0f;

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
	x.w  = 0.0f;

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
	x.w  = 0.0f;

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
	x.w  = 0.0f;

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
	x.w  = 0.0f;

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
	x.w  = 0.0f;

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
	<tr><th>x</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
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

	\brief Create a 4D projection matrix

	Using a frustum, create a projection matrix in a format that
	OpenGL can use directly. If a DirectX version is desired, call TransposeSetFrustum() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>(fNear * 2.0f) / (fRight - fLeft)</td>	<td>0</td>	<td>(fRight + fLeft) / (fRight - fLeft)</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>(fNear * 2.0f) / (fTop - fBottom)</td>	<td>(fTop + fBottom) / (fTop - fBottom)</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>0</td>	<td>0</td>	<td>-(fFar+fNear) / (fFar - fNear)</td>	<td>-(2.0f * fFar * fNear) / (fFar - fNear)</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>-1</td>	<td>0</td></tr>
	</table>

	\note This is functional duplicate of glFrustum()

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa SetOrtho() or TransposeSetFrustum()

***************************************/

void BURGER_API Burger::Matrix4D_t::SetFrustum(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
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

	\brief Create a transposed 4D projection matrix

	Using a frustum, create a projection matrix in a format that
	DirectX can use directly. If a OpenGL version is desired, call SetFrustum() instead.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th><th>w</th></tr>
	<tr><th>x</td>	<td>(fNear * 2.0f) / (fRight - fLeft)</td>	<td>0</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>y</td>	<td>0</td>	<td>(fNear * 2.0f) / (fTop - fBottom)</td>	<td>0</td>	<td>0</td></tr>
	<tr><th>z</td>	<td>(fRight + fLeft) / (fRight - fLeft)</td>	<td>(fTop + fBottom) / (fTop - fBottom)</td>	<td>-(fFar+fNear) / (fFar - fNear)</td>	<td>-1</td></tr>
	<tr><th>w</td>	<td>0</td>	<td>0</td>	<td>-(2.0f * fFar * fNear) / (fFar - fNear)</td>	<td>0</td></tr>
	</table>

	\param fLeft Leftmost X coordinate
	\param fRight Rightmost X coordinate
	\param fBottom Bottommost Y coordinate
	\param fTop Topmost Y coordinate
	\param fNear Near clip plane
	\param fFar Far clip plane
	\sa TransposeSetOrtho() or SetFrustum()

***************************************/

void BURGER_API Burger::Matrix4D_t::TransposeSetFrustum(float fLeft,float fRight,float fBottom,float fTop,float fNear,float fFar)
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

	\brief Create an orthogonal 4D projection matrix

	Using a frustum, create an orthogonal matrix in a format that
	OpenGL can use directly. If a DirectX version is desired, call TransposeSetOrtho() instead.

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

	\brief Create a transposed orthogonal 4D projection matrix

	Using a frustum, create an orthogonal matrix in a format that
	DirectX can use directly. If an OpenGL version is desired, call SetOrtho() instead.

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

	fTemp1 = pInput->y.x;
	fTemp2 = pInput->y.y;
	y.x = fTemp1;
	y.y = fTemp2;

	fTemp1 = pInput->y.z;
	fTemp2 = pInput->y.w;
	y.z = fTemp1;
	y.w = fTemp2;

	fTemp1 = pInput->z.x;
	fTemp2 = pInput->z.y;
	z.x = fTemp1;
	z.y = fTemp2;

	fTemp1 = pInput->z.z;
	fTemp2 = pInput->z.w;
	z.z = fTemp1;
	z.w = fTemp2;

	fTemp1 = pInput->w.x;
	fTemp2 = pInput->w.y;
	w.x = fTemp1;
	w.y = fTemp2;

	fTemp1 = pInput->w.z;
	fTemp2 = pInput->w.w;
	w.z = fTemp1;
	w.w = fTemp2;
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

	\param fInput Scalar value to multiply all values by
	\sa Multiply(const Matrix4D_t *,float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(float fInput)
{
	x.x *= fInput;
	x.y *= fInput;
	x.z *= fInput;
	x.w *= fInput;

	y.x *= fInput;
	y.y *= fInput;
	y.z *= fInput;
	y.w *= fInput;

	z.x *= fInput;
	z.y *= fInput;
	z.z *= fInput;
	z.w *= fInput;

	w.x *= fInput;
	w.y *= fInput;
	w.z *= fInput;
	w.w *= fInput;
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
	\param fInput Scalar value to multiply all values by
	\sa Multiply(float) or Multiply3x3(float)

***************************************/

void BURGER_API Burger::Matrix4D_t::Multiply(const Matrix4D_t *pInput,float fInput)
{
	x.x = pInput->x.x*fInput;
	x.y = pInput->x.y*fInput;
	x.z = pInput->x.z*fInput;
	x.w = pInput->x.w*fInput;

	y.x = pInput->y.x*fInput;
	y.y = pInput->y.y*fInput;
	y.z = pInput->y.z*fInput;
	y.w = pInput->y.w*fInput;

	z.x = pInput->z.x*fInput;
	z.y = pInput->z.y*fInput;
	z.z = pInput->z.z*fInput;
	z.w = pInput->z.w*fInput;

	w.x = pInput->w.x*fInput;
	w.y = pInput->w.y*fInput;
	w.z = pInput->w.z*fInput;
	w.w = pInput->w.w*fInput;
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

	\brief Constant 4x4 identity matrix

***************************************/

const Burger::Matrix4D_t Burger::s_Matrix4DIdentity = {
	{1.0f,0.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f,0.0f},
	{0.0f,0.0f,1.0f,0.0f},
	{0.0f,0.0f,0.0f,1.0f}
};