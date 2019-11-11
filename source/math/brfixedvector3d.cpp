/***************************************

	3D fixed point vector manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedvector3d.h"

/*! ************************************

	\struct Burger::FixedVector3D_t
	\brief 3D \ref Fixed32 vector.
	
	This 12 byte structure contains 3 32-bit
	Fixed point values as a 3D vector.

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.
		
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::Zero(void)
	\brief Initialize the vector elements to zero
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Set(Fixed32,Fixed32,Fixed32)
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::One(void)
	\brief Initialize the vector elements to 1.0f (Fixed)
	
	Fills in all of the entries with zero, thereby
	initializing the structure to a known state.
	
	\sa Set(Fixed32,Fixed32,Fixed32)
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::GetX(void) const
	\brief Return the x component of the vector.
	
	\return The x component of the vector.
	
	\sa SetX(Fixed32)
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::GetY(void) const
	\brief Return the y component of the vector.
	
	\return The y component of the vector.

	\sa SetY(Fixed32)
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::GetZ(void) const
	\brief Return the z component of the vector.
	
	\return The z component of the vector.

	\sa SetZ(Fixed32)
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::SetX(Fixed32 fX)
	\brief Set the x component of the vector.
	
	\param fX The new x component of the vector.
	
	\sa GetX(void) const
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::SetY(Fixed32 fY)
	\brief Set the y component of the vector.
	
	\param fY The new y component of the vector.
	
	\sa GetY(void) const
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::SetZ(Fixed32 fZ)
	\brief Set the z component of the vector.
	
	\param fZ The new z component of the vector.
	
	\sa GetZ(void) const
	
***************************************/

/*! ************************************

	\fn Burger::FixedVector3D_t::Set(Fixed32 fX,Fixed32 fY,Fixed32 fZ)
	\brief Initialize the vector elements to specific values,
	
	Given the new values for x,y and z, store them into the structure.

	\param fX The new x component to store in the vector.
	\param fY The new y component to store in the vector.
	\param fZ The new z component to store in the vector.
		
	\sa Zero(void)
	
***************************************/

/*! ************************************

	\fn void Burger::FixedVector3D_t::Set(const FixedVector3D_t *pInput)
	\brief Copy a FixedVector3D_t

	Make a copy of a FixedVector3D_t

	\param pInput Valid pointer to a FixedVector3D_t structure to copy 
	\sa Set(Fixed32,Fixed32,Fixed32) or Zero(void)

***************************************/

/*! ************************************

	\brief Convert a floating point vector into fixed point vector.
	
	Using round to nearest, convert a \ref Vector3D_t 's data into
	a \ref FixedVector3D_t. Saturation is performed. NaN and Infinity
	will yield undefined results.
	
	\param pInput Pointer to an initialized \ref Vector3D_t structure.
	\sa SetAsInt(const Vector3D_t *)

***************************************/

void BURGER_API Burger::FixedVector3D_t::Set(const Vector3D_t *pInput)
{
	FloatToFixedNearest(&x,pInput->x);
	FloatToFixedNearest(&y,pInput->y);
	FloatToFixedNearest(&z,pInput->z);
}

/*! ************************************

	\brief Convert a floating point vector into an integer vector.
	
	Using round to nearest, convert a \ref Vector3D_t 's data into
	a \ref FixedVector3D_t and the conversion will place integer values
	in the members instead of performing the float->\ref Fixed32 conversion.
	Saturation is performed. NaN and Infinity will yield undefined results.
	
	\param pInput Pointer to an initialized \ref Vector3D_t structure.

	\note If \ref Fixed32 versions of the values are desired, use Set(const Vector3D_t *)
	instead.
	
	\sa Set(const Vector3D_t *pInput)
	
***************************************/

void BURGER_API Burger::FixedVector3D_t::SetAsInt(const Vector3D_t *pInput)
{
	FloatToIntRound(reinterpret_cast<Int32*>(&x),pInput->x);
	FloatToIntRound(reinterpret_cast<Int32*>(&y),pInput->y);
	FloatToIntRound(reinterpret_cast<Int32*>(&z),pInput->z);
}

/*! ************************************

	\brief Negate a fixed vector
	
	Perform a negation on all three elements of the vector.

***************************************/

void BURGER_API Burger::FixedVector3D_t::Negate(void)
{
	x = -x;
	y = -y;
	z = -z;
}

/*! ************************************

	\brief Negate and copy a vector

	Perform a negation on all three elements of a given vector
	and store the results.
	
	\param pInput Pointer to a valid \ref FixedVector3D_t
	
	\note The *this vector does not need to be initialized.

***************************************/

void BURGER_API Burger::FixedVector3D_t::Negate(const FixedVector3D_t *pInput)
{
	x = -pInput->x;
	y = -pInput->y;
	z = -pInput->z;
}

/*! ************************************

	\brief Add two vectors
	
	Perform a vector addition in the form of x += pInput->x, y+=pInput->y etc.
	
	\param pInput Pointer to a valid \ref FixedVector3D_t

***************************************/

void BURGER_API Burger::FixedVector3D_t::Add(const FixedVector3D_t *pInput)
{
	x += pInput->x;
	y += pInput->y;
	z += pInput->z;
}

/*! ************************************

	\brief Add and copy two vectors

	Perform a vector addition in the form of x = pInput1->x+pInput2->x, y = pInput2->y+pInput2->y etc.

	\param pInput1 Pointer to a valid \ref FixedVector3D_t
	\param pInput2 Pointer to a valid \ref FixedVector3D_t to add with

	\note The *this vector does not need to be initialized.

***************************************/

void BURGER_API Burger::FixedVector3D_t::Add(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2)
{
	x = pInput1->x+pInput2->x;
	y = pInput1->y+pInput2->y;
	z = pInput1->z+pInput2->z;
}

/*! ************************************

	\brief Subtract two vectors

	Perform a vector subtraction in the form of x -= pInput->x, y-=pInput->y etc.

	\param pInput Pointer to a valid \ref FixedVector3D_t

***************************************/

void BURGER_API Burger::FixedVector3D_t::Sub(const FixedVector3D_t *pInput)
{
	x -= pInput->x;
	y -= pInput->y;
	z -= pInput->z;
}

/*! ************************************

	\brief Subtract and copy two vectors

	Perform a vector subtraction in the form of x = pInput1->x-pInput2->x, y = pInput2->y-pInput2->y etc.

	\param pInput1 Pointer to a valid \ref FixedVector3D_t
	\param pInput2 Pointer to a valid \ref FixedVector3D_t to subtract from

	\note The *this vector does not need to be initialized.

***************************************/

void BURGER_API Burger::FixedVector3D_t::Sub(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2)
{
	x = pInput1->x - pInput2->x;
	y = pInput1->y - pInput2->y;
	z = pInput1->z - pInput2->z;
}

/*! ************************************

	\brief Perform a dot product

	Perform a dot product with *this and the supplied vector.
	
	\param pInput Pointer to a valid \ref FixedVector3D_t
	\return \ref Fixed32 result of the dot product.
	
***************************************/

Fixed32 BURGER_API Burger::FixedVector3D_t::Dot(const FixedVector3D_t *pInput) const
{
	return (FixedMultiply(x,pInput->x)+
		FixedMultiply(y,pInput->y)+
		FixedMultiply(z,pInput->z));
}

/*! ************************************

	\brief Perform a cross product
	
	Perform a cross product between two vector and store the result in
	the *this value. The values are cached, so the *this pointer can
	be used as an input parameter without causing a problem.

	\param pInput1 Pointer to a valid \ref FixedVector3D_t
	\param pInput2 Pointer to a valid \ref FixedVector3D_t to add with

***************************************/

void BURGER_API Burger::FixedVector3D_t::Cross(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2)
{
	Fixed32 fx = FixedMultiply(pInput1->y,pInput2->z) - FixedMultiply(pInput1->z,pInput2->y);
	Fixed32 fy = FixedMultiply(pInput1->z,pInput2->x) - FixedMultiply(pInput1->x,pInput2->z);
	Fixed32 fz = FixedMultiply(pInput1->x,pInput2->y) - FixedMultiply(pInput1->y,pInput2->x);
	x = fx;		// Store after it's complete
	y = fy;
	z = fz;
}

const Burger::FixedVector3D_t Burger::s_FixedVector3DZero = {FLOATTOFIXED(0.0f),FLOATTOFIXED(0.0f),FLOATTOFIXED(0.0f)};	///< Constant of 0.0f,0.0f,0.0f in fixed point
const Burger::FixedVector3D_t Burger::s_FixedVector3DOne = {FLOATTOFIXED(1.0f),FLOATTOFIXED(1.0f),FLOATTOFIXED(1.0f)};	///< Constant of 1.0f,1.0f,1.0f in fixed point
const Burger::FixedVector3D_t Burger::s_FixedVector3DOneX = {FLOATTOFIXED(1.0f),FLOATTOFIXED(0.0f),FLOATTOFIXED(0.0f)};	///< Constant of 1.0f,0.0f,0.0f in fixed point
const Burger::FixedVector3D_t Burger::s_FixedVector3DOneY = {FLOATTOFIXED(0.0f),FLOATTOFIXED(1.0f),FLOATTOFIXED(0.0f)};	///< Constant of 0.0f,1.0f,0.0f in fixed point
const Burger::FixedVector3D_t Burger::s_FixedVector3DOneZ = {FLOATTOFIXED(0.0f),FLOATTOFIXED(0.0f),FLOATTOFIXED(1.0f)};	///< Constant of 0.0f,0.0f,1.0f in fixed point

