/***************************************

    4D fixed point vector manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedvector4d.h"

/*! ************************************

	\struct Burger::FixedVector4D_t
	\brief 4 dimensional fixed point vector

	This 16 byte vector contains x,y,z and w 32 bit fixed point coordinates. A
	set of common functions for simple 4 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y", "z" and "w" for maximum compatibility

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::FixedVector2D_t, Burger::FixedVector3D_t or Burger::Vector4D_t
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Zero(void)
	\brief Initialize the vector elements to zero
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Identity(void), Set(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::One(void)
	\brief Initialize the vector elements to 1.0f (Fixed)
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Identity(void), Set(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Identity(void)
	\brief Initialize the vector elements to identity for a quaternion
	
	Fills x, y, and z with \ref BURGER_FLOAT_TO_FIXED(0.0f) and w is filled in with \ref BURGER_FLOAT_TO_FIXED(1.0f).
	Thereby initializing the structure to the equivalant of a quaternion identity..
	
	\sa Zero(void) or Set(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::FixedVector4D_t::GetX(void) const
	\brief Return the x component of the vector.
	
	\return The x component of the vector.
	
	\sa SetX(fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::FixedVector4D_t::GetY(void) const
	\brief Return the y component of the vector.
	
	\return The y component of the vector.

	\sa SetY(fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::FixedVector4D_t::GetZ(void) const
	\brief Return the z component of the vector.
	
	\return The z component of the vector.

	\sa SetZ(fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn fixed16_16_t Burger::FixedVector4D_t::GetW(void) const
	\brief Return the w component of the vector.
	
	\return The w component of the vector.

	\sa SetW(fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::SetX(fixed16_16_t fX)
	\brief Set the x component of the vector.
	
	\param fX The new x component of the vector.
	
	\sa GetX(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::SetY(fixed16_16_t fY)
	\brief Set the y component of the vector.
	
	\param fY The new y component of the vector.
	
	\sa GetY(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::SetZ(fixed16_16_t fZ)
	\brief Set the z component of the vector.
	
	\param fZ The new z component of the vector.
	
	\sa GetZ(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::SetW(fixed16_16_t fW)
	\brief Set the w component of the vector.
	
	\param fW The new w component of the vector.
	
	\sa GetW(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Set(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ)
	\brief Set the values to specific values

	Sets the x, y and z values to specific values. w is set to \ref BURGER_FLOAT_TO_FIXED(1.0f)

	\param fX New x value
	\param fY New y value
	\param fZ New z value
	\sa Zero(void), Identity(void) or Set(float,float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Set(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ,fixed16_16_t fW)
	\brief Initialize the vector elements to specific values,
	
	Given the new values for x, y, z and w, store them into the structure.

	\param fX The new x component to store in the vector.
	\param fY The new y component to store in the vector.
	\param fZ The new z component to store in the vector.
	\param fW The new w component to store in the vector.
	\sa Zero(void), Identity(void) or Set(fixed16_16_t,fixed16_16_t,fixed16_16_t)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Set(const FixedVector4D_t *pInput)
	\brief Copy a FixedVector4D_t

	Make a copy of a FixedVector4D_t

	\param pInput Valid pointer to a FixedVector4D_t structure to copy 
	\sa Zero(void), Identity(void) or Set(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Negate(void)
	\brief Negate a 4D vector

	Set the x,y,z and w values to -x,-y,-z and -w respectively
	\sa Negate(const FixedVector4D_t *) or Negate(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Negate(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ,fixed16_16_t fW)
	\brief Negate the input and store it in this structure

	Set the x,y,z and w values to -fX,-fY,-fZ and -fW respectively

	\param fX X value to negate
	\param fY Y value to negate
	\param fZ Z value to negate
	\param fW W value to negate
	\sa Negate(const FixedVector4D_t *) or Negate(void)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector4D_t::Negate(const FixedVector4D_t *pInput)
	\brief Make a copy of the input after it's been negated

	Copy the input data and negate it.

	\param pInput Valid pointer to a FixedVector4D_t structure to copy and negate
	\sa Negate(fixed16_16_t,fixed16_16_t,fixed16_16_t,fixed16_16_t) or Negate(void)

***************************************/

const Burger::FixedVector4D_t Burger::s_FixedVector4DZero = {BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)};	///< Constant of 0.0f,0.0f,0.0f,0.0f in fixed point
const Burger::FixedVector4D_t Burger::s_FixedVector4DOne = {BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(1.0f)};	///< Constant of 1.0f,1.0f,1.0f,1.0f in fixed point
const Burger::FixedVector4D_t Burger::s_FixedVector4DOneX = {BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)};	///< Constant of 1.0f,0.0f,0.0f,0.0f in fixed point
const Burger::FixedVector4D_t Burger::s_FixedVector4DOneY = {BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)};	///< Constant of 0.0f,1.0f,0.0f,0.0f in fixed point
const Burger::FixedVector4D_t Burger::s_FixedVector4DOneZ = {BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f)};	///< Constant of 0.0f,0.0f,1.0f,0.0f in fixed point
const Burger::FixedVector4D_t Burger::s_FixedVector4DOneW = {BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f)};	///< Constant of 0.0f,0.0f,0.0f,1.0f in fixed point
