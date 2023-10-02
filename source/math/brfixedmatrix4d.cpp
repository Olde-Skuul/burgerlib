/***************************************

    4D fixed point matrix manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedmatrix4d.h"

/*! ************************************

	\struct Burger::FixedMatrix4D_t
	\brief 4x4 \ref Fixed32 matrix.
	
	This 64 byte matrix contains x,y,z and w 32 bit 16.16 fixed point rows and columns. A
	set of common functions for simple 4 dimensional math are part of the structure.
	
	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y", "z" and "w" for maximum compatibility
	
	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::FixedVector4D_t, Burger::FixedMatrix3D_t or Burger::Matrix4D_t
	
***************************************/

/*! ************************************

	\brief Clear out a 4D matrix

	Set all of the entries to zero

	\sa Identity(void)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::Zero(void)
{
	Fixed32 fTemp = BURGER_FLOAT_TO_FIXED(0.0f);		// Force the compiler to use a register
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

	Sets the x.x, y.y, z.z and w.w components to \ref BURGER_FLOAT_TO_FIXED(1.0f), all others to \ref BURGER_FLOAT_TO_FIXED(0.0f)

	\sa Zero(void)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::Identity(void)
{
	x.x = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave X alone
	x.y = BURGER_FLOAT_TO_FIXED(0.0f);
	x.z = BURGER_FLOAT_TO_FIXED(0.0f);
	x.w = BURGER_FLOAT_TO_FIXED(0.0f);
	y.x = BURGER_FLOAT_TO_FIXED(0.0f);
	y.y = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave Y alone
	y.z = BURGER_FLOAT_TO_FIXED(0.0f);
	y.w = BURGER_FLOAT_TO_FIXED(0.0f);
	z.x = BURGER_FLOAT_TO_FIXED(0.0f);
	z.y = BURGER_FLOAT_TO_FIXED(0.0f);
	z.z = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave Z alone
	z.w = BURGER_FLOAT_TO_FIXED(0.0f);
	w.x = BURGER_FLOAT_TO_FIXED(0.0f);
	w.y = BURGER_FLOAT_TO_FIXED(0.0f);
	w.z = BURGER_FLOAT_TO_FIXED(0.0f);	
	w.w = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave W alone
}

/*! ************************************

	\brief Convert a floating point matrix into a \ref Fixed32 matrix
	
	Using round to nearest, convert a matrix using floating point
	values into one that has \ref Fixed32 values.
	The destination matrix is assumed to be uninitialized.

	\param pInput Pointer to a valid Matrix4D_t
	\sa Set(const FixedMatrix4D_t *) or float_to_fixed_round(Fixed32 *,float)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::Set(const Matrix4D_t *pInput)
{
	float_to_fixed_round(&x.x,pInput->x.x);
	float_to_fixed_round(&x.y,pInput->x.y);
	float_to_fixed_round(&x.z,pInput->x.z);
	float_to_fixed_round(&x.w,pInput->x.w);
	float_to_fixed_round(&y.x,pInput->y.x);
	float_to_fixed_round(&y.y,pInput->y.y);
	float_to_fixed_round(&y.z,pInput->y.z);
	float_to_fixed_round(&y.w,pInput->y.w);
	float_to_fixed_round(&z.x,pInput->z.x);
	float_to_fixed_round(&z.y,pInput->z.y);
	float_to_fixed_round(&z.z,pInput->z.z);
	float_to_fixed_round(&z.w,pInput->z.w);
	float_to_fixed_round(&w.x,pInput->w.x);
	float_to_fixed_round(&w.y,pInput->w.y);
	float_to_fixed_round(&w.z,pInput->w.z);
	float_to_fixed_round(&w.w,pInput->w.w);
}

/*! ************************************

	\brief Copy a matrix into this one
	
	Perform a literal memory copy operation

	\param pInput Pointer to a valid FixedMatrix4D_t
	\sa Set(const Matrix4D_t *)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::Set(const FixedMatrix4D_t *pInput)
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

	\brief Create a 4D translation matrix

	Sets the w row to fX, fY and fZ and the rest of
	the values to that of an identity matrix.

	\param fX new w.x component
	\param fY new w.y component
	\param fZ new w.z component
	\sa Identity(void)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::SetTranslate(Fixed32 fX,Fixed32 fY,Fixed32 fZ)
{
	w.x = fX;
	w.y = fY;
	w.z = fZ;
	w.w = BURGER_FLOAT_TO_FIXED(1.0f);
	x.x = BURGER_FLOAT_TO_FIXED(1.0f);
	x.y = BURGER_FLOAT_TO_FIXED(0.0f);
	x.z = BURGER_FLOAT_TO_FIXED(0.0f);
	x.w = BURGER_FLOAT_TO_FIXED(0.0f);
	y.x = BURGER_FLOAT_TO_FIXED(0.0f);
	y.y = BURGER_FLOAT_TO_FIXED(1.0f);
	y.z = BURGER_FLOAT_TO_FIXED(0.0f);
	y.w = BURGER_FLOAT_TO_FIXED(0.0f);
	z.x = BURGER_FLOAT_TO_FIXED(0.0f);
	z.y = BURGER_FLOAT_TO_FIXED(0.0f);
	z.z = BURGER_FLOAT_TO_FIXED(1.0f);
	z.w = BURGER_FLOAT_TO_FIXED(0.0f);
}

/*! ************************************

	\brief Create a 4D scale matrix

	Sets the x.x, y.y and z.z components to the input values, w.w
	is set to \ref BURGER_FLOAT_TO_FIXED(1.0f) and all others are set to \ref BURGER_FLOAT_TO_FIXED(0.0f)

	\param fX new x.x component
	\param fY new y.y component
	\param fZ new z.z component
	\sa Identity(void)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::SetScale(Fixed32 fX,Fixed32 fY,Fixed32 fZ)
{
	x.x = fX;
	x.y = BURGER_FLOAT_TO_FIXED(0.0f);
	x.z = BURGER_FLOAT_TO_FIXED(0.0f);
	x.w = BURGER_FLOAT_TO_FIXED(0.0f);
	y.x = BURGER_FLOAT_TO_FIXED(0.0f);
	y.y = fY;
	y.z = BURGER_FLOAT_TO_FIXED(0.0f);
	y.w = BURGER_FLOAT_TO_FIXED(0.0f);
	z.x = BURGER_FLOAT_TO_FIXED(0.0f);
	z.y = BURGER_FLOAT_TO_FIXED(0.0f);
	z.z = fZ;
	z.w = BURGER_FLOAT_TO_FIXED(0.0f);
	w.x = BURGER_FLOAT_TO_FIXED(0.0f);
	w.y = BURGER_FLOAT_TO_FIXED(0.0f);
	w.z = BURGER_FLOAT_TO_FIXED(0.0f);
	w.w = BURGER_FLOAT_TO_FIXED(1.0f);
}

/*! ************************************

	\brief Create a 4D scale matrix

	Sets the x.x, y.y, z.z and w.w components to the input values
	and all others are set to \ref BURGER_FLOAT_TO_FIXED(0.0f)

	\param fX new x.x component
	\param fY new y.y component
	\param fZ new z.z component
	\param fW new w.w component
	\sa Identity(void)

***************************************/

void BURGER_API Burger::FixedMatrix4D_t::SetScale(Fixed32 fX,Fixed32 fY,Fixed32 fZ,Fixed32 fW)
{
	x.x = fX;
	x.y = BURGER_FLOAT_TO_FIXED(0.0f);
	x.z = BURGER_FLOAT_TO_FIXED(0.0f);
	x.w = BURGER_FLOAT_TO_FIXED(0.0f);
	y.x = BURGER_FLOAT_TO_FIXED(0.0f);
	y.y = fY;
	y.z = BURGER_FLOAT_TO_FIXED(0.0f);
	y.w = BURGER_FLOAT_TO_FIXED(0.0f);
	z.x = BURGER_FLOAT_TO_FIXED(0.0f);
	z.y = BURGER_FLOAT_TO_FIXED(0.0f);
	z.z = fZ;
	z.w = BURGER_FLOAT_TO_FIXED(0.0f);
	w.x = BURGER_FLOAT_TO_FIXED(0.0f);
	w.y = BURGER_FLOAT_TO_FIXED(0.0f);
	w.z = BURGER_FLOAT_TO_FIXED(0.0f);
	w.w = fW;
}

/*! ************************************

	\brief Constant 4x4 fixed point identity matrix

***************************************/

const Burger::FixedMatrix4D_t Burger::s_FixedMatrix4DIdentity = {
	{BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)},
	{BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)},
	{BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f)},
	{BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f)}
};
