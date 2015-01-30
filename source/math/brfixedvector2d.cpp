/***************************************

	2D fixed point vector manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedvector2d.h"

/*! ************************************

	\struct Burger::FixedVector2D_t
	\brief 2 dimensional fixed point vector
	
	This 8 byte vector contains x and y 32 bit fixed point coordinates. A
	set of common functions for simple 2 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x" and "y" for maximum compatibility
	
	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.
	
	\sa Burger::FixedVector3D_t, Burger::FixedVector4D_t or Burger::Vector2D_t

***************************************/


/*! ************************************

	\fn void Burger::FixedVector2D_t::Zero(void)
	\brief Initialize the vector elements to zero
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Set(Fixed32,Fixed32), SetX(Fixed32) or SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::One(void)
	\brief Initialize the vector elements to 1.0f (Fixed)
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Set(Fixed32,Fixed32), SetX(Fixed32) or SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FixedVector2D_t::GetX(void) const
	\brief Return the x component of the vector.
	
	\return The x component of the vector.
	
	\sa GetY(void) const or SetX(Fixed32)
	
***************************************/

/*! ************************************

	\fn Fixed32 Burger::FixedVector2D_t::GetY(void) const
	\brief Return the y component of the vector.
	
	\return The y component of the vector.

	\sa GetX(void) const or SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::SetX(Fixed32 fX)
	\brief Set the x component of the vector.
	
	\param fX The new x component of the vector.
	
	\sa GetX(void) const or SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::SetY(Fixed32 fY)
	\brief Set the y component of the vector.
	
	\param fY The new y component of the vector.
	
	\sa GetY(void) const or SetX(Fixed32)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::Set(Fixed32 fX,Fixed32 fY)
	\brief Initialize the vector elements to specific values,
	
	Given the new values for x and y, store them into the structure.

	\param fX The new x component to store in the vector.
	\param fY The new y component to store in the vector.
		
	\sa Zero(void), SetX(Fixed32) or SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::Negate(void)
	\brief Negate a 2D vector

	Set the x and y value to -x and -y respectively
	\sa Negate(const FixedVector2D_t *) or Negate(Fixed32,Fixed32)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::Negate(Fixed32 fX,Fixed32 fY)
	\brief Negate the input and store it in this structure

	Set the x and y value to -fX and -fY respectively

	\param fX X value to negate
	\param fY Y value to negate
	\sa Negate(const FixedVector2D_t *) or Negate(void)

***************************************/

/*! ************************************

	\fn void Burger::FixedVector2D_t::Negate(const FixedVector2D_t *pInput)
	\brief Make a copy of the input after it's been negated

	Copy the input data and negate it.

	\param pInput Valid pointer to a FixedVector2D_t structure to copy and negate
	\sa Negate(Fixed32,Fixed32) or Negate(void)

***************************************/
