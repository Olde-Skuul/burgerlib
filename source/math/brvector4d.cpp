/***************************************

	4D Floating point vector manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvector4d.h"
#include "brfixedvector4d.h"

/*! ************************************

	\struct Burger::Vector4D_t
	\brief 4 dimensional floating point vector

	This 16 byte vector contains x,y,z and w 32 bit floating point coordinates. A
	set of common functions for simple 4 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y", "z" and "w" for maximum compatibility

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::Vector2D_t, Burger::Vector3D_t or Burger::FixedVector4D_t
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Zero(void)
	\brief Set all values to zero
	\sa Set(float,float,float,float) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::One(void)
	\brief Set all values to 1.0f
	\sa Set(float,float,float,float) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Identity(void)
	\brief Initialize the vector elements to identity for a quaternion
	
	Fills x, y, and z with 0.0f and w is filled in with 1.0f.
	Thereby initializing the structure to the equivalant of a quaternion identity..
	
	\sa Zero(void) or Set(float,float,float,float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetX(void) const
	\brief Return the x component of the vector.
	
	\return The x component of the vector.
	
	\sa SetX(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetY(void) const
	\brief Return the y component of the vector.
	
	\return The y component of the vector.

	\sa SetY(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetZ(void) const
	\brief Return the z component of the vector.
	
	\return The z component of the vector.

	\sa SetZ(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetW(void) const
	\brief Return the w component of the vector.
	
	\return The w component of the vector.

	\sa SetW(float)
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::SetX(float fX)
	\brief Set the x component of the vector.
	
	\param fX The new x component of the vector.
	
	\sa GetX(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::SetY(float fY)
	\brief Set the y component of the vector.
	
	\param fY The new y component of the vector.
	
	\sa GetY(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::SetZ(float fZ)
	\brief Set the z component of the vector.
	
	\param fZ The new z component of the vector.
	
	\sa GetZ(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::SetW(float fW)
	\brief Set the w component of the vector.
	
	\param fW The new w component of the vector.
	
	\sa GetW(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Set(float fInput)
	\brief Set all of the members to specific value

	Sets the x,y,z and w values to a specific value
	\param fInput New value for all members
	\sa Zero(void), SetX(float), SetY(float), SetZ(float), SetW(float) or Set(const Vector4D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Set(float fX,float fY)
	\brief Set the values to specific values

	Sets the x and y values to specific values. z is set to 0.0f and w is set to 1.0f

	\param fX New x value
	\param fY New y value
	\sa Zero(void), Identity(void) or Set(float,float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Set(float fX,float fY,float fZ)
	\brief Set the values to specific values

	Sets the x, y and z values to specific values. w is set to 1.0f

	\param fX New x value
	\param fY New y value
	\param fZ New z value
	\sa Zero(void), Identity(void) or Set(float,float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Set(float fX,float fY,float fZ,float fW)
	\brief Set the values to specific values

	Sets the x, y, z and w values to specific values.

	\param fX New x value
	\param fY New y value
	\param fZ New z value
	\param fW New w value
	\sa Zero(void) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Set(const Vector4D_t *pInput)
	\brief Copy a Vector4D_t

	Make a copy of a Vector4D_t

	\param pInput Valid pointer to a Vector4D_t structure to copy 
	\sa Zero(void), Identity(void) or Set(float,float,float,float)

***************************************/

/*! ************************************

	\brief Convert a fixed point vector into a floating point vector

	\param pInput Valid pointer to a FixedVector4D_t structure to copy 

	\sa Set(const Vector4D_t *), Set(float,float,float,float) or Zero(void)

***************************************/

void BURGER_API Burger::Vector4D_t::Set(const FixedVector4D_t *pInput)
{
	FixedToFloat(&x,&pInput->x);
	FixedToFloat(&y,&pInput->y);
	FixedToFloat(&z,&pInput->z);
	FixedToFloat(&w,&pInput->w);
}

/*! ************************************

	\fn void Burger::Vector4D_t::Negate(void)
	\brief Negate a 4D vector

	Set the x,y,z and w values to -x,-y,-z and -w respectively
	\sa Negate(const Vector4D_t *) or Negate(float,float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Negate(float fX,float fY,float fZ,float fW)
	\brief Negate the input and store it in this structure

	Set the x,y,z and w values to -fX,-fY,-fZ and -fW respectively

	\param fX X value to negate
	\param fY Y value to negate
	\param fZ Z value to negate
	\param fW W value to negate
	\sa Negate(const Vector4D_t *) or Negate(void)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Negate(const Vector4D_t *pInput)
	\brief Make a copy of the input after it's been negated

	Copy the input data and negate it.

	\param pInput Valid pointer to a Vector4D_t structure to copy and negate
	\sa Negate(float,float,float,float) or Negate(void)

***************************************/

/*! ************************************

	\brief Interpolate between two vectors

	Given a factor between 0.0f and 1.0f inclusive, perform
	a linear scale between the two vectors and return pFrom
	if the factor is 0.0f and pTo if the factor is 1.0f

	No clamping is performed.

	\param pFrom Valid pointer to a Vector4D_t structure for a factor of 0.0f
	\param pTo Valid pointer to a Vector4D_t structure for a factor of 1.0f
	\param fFactor Scale value between 0.0f and 1.0f inclusive
	\sa Vector2D_t::Interpolate() and Vector3D_t::Interpolate()

***************************************/

void BURGER_API Burger::Vector4D_t::Interpolate(const Vector4D_t *pFrom,const Vector4D_t *pTo,float fFactor)
{
	float fFromX = pFrom->x;
	float fFromY = pFrom->y;
	float fFromZ = pFrom->z;
	float fFromW = pFrom->w;
	x = ((pTo->x - fFromX) * fFactor) + fFromX;
	y = ((pTo->y - fFromY) * fFactor) + fFromY;
	z = ((pTo->z - fFromZ) * fFactor) + fFromZ;
	w = ((pTo->w - fFromW) * fFactor) + fFromW;
}


/*! ************************************

	\fn float Burger::Vector4D_t::Dot(float fX,float fY,float fZ,float fW) const
	\brief Return a dot product of two 4D vectors
	\param fX X value to dot against
	\param fY Y value to dot against
	\param fZ Z value to dot against
	\param fW W value to dot against

	\return Dot product of the two vectors
	\sa Dot(const Vector4D_t *) const or Dot3(float,float,float) const

***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::Dot(const Vector4D_t *pInput) const
	\brief Return a dot product of two 4D vectors
	\param pInput Valid pointer to a Vector4D_t structure to perform a dot product against
	\return Dot product of the two vectors
	\sa Dot(float,float,float,float) const or Dot3(const Vector4D_t *) const

***************************************/



/*! ************************************

	\fn void Burger::Vector4D_t::Add(const Vector4D_t *pInput)
	\brief Add another vector to this one

	\param pInput Vector to add
	\sa Add(float), Add(const Vector4D_t *,const Vector4D_t *) or Add(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Add(float fInput)
	\brief Add the same scalar value to each of member values

	\param fInput Value to add
	\sa Add(const Vector4D_t *), Add(const Vector4D_t *,const Vector4D_t *) or Add(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Add(const Vector4D_t *pInput1,const Vector4D_t *pInput2)
	\brief Add two vectors and store the result in this vector

	Initialize this vector with the result of the addition of the two
	input vectors.

	\param pInput1 First vector to add
	\param pInput2 Second vector to add
	\sa Add(float), Add(const Vector4D_t *) or Add(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Add(const Vector4D_t *pInput,float fInput)
	\brief Add a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the addition of the
	input vector and a scalar applied to every member.

	\param pInput Vector to add the scalar to
	\param fInput Scalar to add to the vector
	\sa Add(float), Add(const Vector4D_t *) or Add(const Vector4D_t *,const Vector4D_t *)

***************************************/


/*! ************************************

	\fn void Burger::Vector4D_t::Sub(const Vector4D_t *pInput)
	\brief Subtract another vector from this one

	\param pInput Vector to subtract
	\sa Sub(float), Sub(const Vector4D_t *,const Vector4D_t *) or Sub(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Sub(float fInput)
	\brief Subtract the same scalar value from each of member values

	\param fInput Value to subtract
	\sa Sub(const Vector4D_t *), Sub(const Vector4D_t *,const Vector4D_t *) or Sub(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Sub(const Vector4D_t *pInput1,const Vector4D_t *pInput2)
	\brief Subtract two vectors and store the result in this vector

	Initialize this vector with the result of the subtraction of the two
	input vectors.

	\param pInput1 First vector to subtract from
	\param pInput2 Second vector to subtract with
	\sa Sub(float), Sub(const Vector4D_t *) or Sub(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Sub(const Vector4D_t *pInput,float fInput)
	\brief Subtract a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the subtraction of the
	input vector and a scalar applied to every member.

	\param pInput Vector to subtract the scalar from
	\param fInput Scalar to subtract from the vector
	\sa Sub(float), Sub(const Vector4D_t *) or Sub(const Vector4D_t *,const Vector4D_t *)

***************************************/



/*! ************************************

	\fn void Burger::Vector4D_t::Mul(const Vector4D_t *pInput)
	\brief Multiply another vector to this one

	\param pInput Vector to multiply
	\sa Mul(float), Mul(const Vector4D_t *,const Vector4D_t *) or Mul(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Mul(float fInput)
	\brief Multiply the same scalar value to each of member values

	\param fInput Value to multiply
	\sa Mul(const Vector4D_t *), Mul(const Vector4D_t *,const Vector4D_t *) or Mul(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Mul(const Vector4D_t *pInput1,const Vector4D_t *pInput2)
	\brief Multiply two vectors and store the result in this vector

	Initialize this vector with the result of the multiplication of the two
	input vectors.

	\param pInput1 First vector to multiply
	\param pInput2 Second vector to multiply
	\sa Mul(float), Mul(const Vector4D_t *) or Mul(const Vector4D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::Mul(const Vector4D_t *pInput,float fInput)
	\brief Multiply a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the multiplication of the
	input vector and a scalar applied to every member.

	\param pInput Vector to multiply the scalar to
	\param fInput Scalar to multiply to the vector
	\sa Mul(float), Mul(const Vector4D_t *) or Mul(const Vector4D_t *,const Vector4D_t *)

***************************************/


/*! ************************************

	\fn float Burger::Vector4D_t::GetLengthSquared(void) const
	\brief Returns the square of the length of a 4D vector
	\return x*x+y*y+z*z+w*w
	\sa GetLengthFast(void) const or GetLength(void) const

***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetLength(void) const
	\brief Return the length of a vector (High precision)
	\return sqrt(x*x+y*y+z*z+w*w)
	\sa GetLengthFast(void) const or GetLengthSquared(void) const

***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::GetLengthFast(void) const
	\brief Return the length of a vector (Good precision)

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\return sqrt(x*x+y*y+z*z+w*w)
	\sa GetLength(void) const or GetLengthSquared(void) const

***************************************/


/*! ************************************

	\brief Normalize a 4D vector to a specific length (High precision)
	
	Set the 4D vector's length to fInput

	\note fInput cannot be zero or negative. The vector will be set to 0.0f in this case

	\param fInput Length to set the vector to
	\sa SetLengthFast(float) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector4D_t::SetLength(float fInput)
{
	if (fInput>0.0f) {		// Handles NaN
		float fLength = Sqrt((x*x) + (y*y) + (z*z) + (w*w));
		if (fLength>0.0f) {	// Handles NaN
			fInput = fInput/fLength;		// Scale to the new length
			x *= fInput;
			y *= fInput;
			z *= fInput;
			w *= fInput;
		} else {
			x = fInput;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}
	} else {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
}

/*! ************************************

	\brief Normalize a 3D vector to a specific length (Good precision)
	
	Set the 3D vector's length to fInput

	\note fInput cannot be zero or negative. The vector will be set to 0.0f in this case

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param fInput Length to set the vector to
	\sa SetLength(float) or NormalizeFast(void)

***************************************/

void BURGER_API Burger::Vector4D_t::SetLengthFast(float fInput)
{
	if (fInput>0.0f) {			// Handles NaN
		float fLength = Sqrt((x*x) + (y*y) + (z*z) + (w*w));
		if (fLength>0.0f) {			// Handles NaN
			fInput = fInput/fLength;		// Scale to the new length
			x *= fInput;
			y *= fInput;
			z *= fInput;
			w *= fInput;
		} else {
			x = fInput;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}
	} else {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
}


/*! ************************************

	\brief Returns the square of the distance between two 4D points

	Create a vector from the difference of two 4D points and return
	the vector's length squared

	\param fX X value of the other 4D point
	\param fY Y value of the other 4D point
	\param fZ Z value of the other 4D point
	\param fW W value of the other 4D point
	\return The distance between the two points, squared
	\sa GetDistanceSquared(const Vector4D_t *) const, GetDistance(float,float,float,float) const or GetDistanceFast(float,float,float,float) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistanceSquared(float fX,float fY,float fZ,float fW) const
{
	// Don't worry about absolute values. The squares will remove it
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	fW = w-fW;
	return (fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW);
}

/*! ************************************

	\brief Returns the square of the distance between two 4D points

	Create a vector from the difference of two 4D points and return
	the vector's length squared

	\param pInput Valid pointer to a Vector4D_t structure that contains the other 4D point
	\return The distance between the two points, squared
	\sa GetDistanceSquared(float,float,float,float) const, GetDistance(const Vector4D_t *) const or GetDistanceFast(const Vector4D_t *) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistanceSquared(const Vector4D_t *pInput) const
{
	// Don't worry about absolute values. The squares will remove it
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	float fW = w-pInput->w;
	return (fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW);
}

/*! ************************************

	\brief Returns the square of the distance between two 4D points

	Create a vector from the difference of two 4D points and return
	the vector's length

	\param fX X value of the other 4D point
	\param fY Y value of the other 4D point
	\param fZ Z value of the other 4D point
	\param fW W value of the other 4D point
	\return The distance between the two points
	\sa GetDistance(const Vector4D_t *) const, GetDistanceSquared(float,float,float,float) const or GetDistanceFast(float,float,float,float) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistance(float fX,float fY,float fZ,float fW) const
{
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	fW = w-fW;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW));
}

/*! ************************************

	\brief Returns the square of the distance between two 4D points

	Create a vector from the difference of two 4D points and return
	the vector's length

	\param pInput Valid pointer to a Vector4D_t structure that contains the other 4D point
	\return The distance between the two points
	\sa GetDistance(float,float,float,float) const, GetDistanceSquared(const Vector4D_t *) const or GetDistanceFast(const Vector4D_t *) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistance(const Vector4D_t *pInput) const
{
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	float fW = w-pInput->w;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW));
}

/*! ************************************

	\brief Returns the square of the distance between two 4D points (Good precision)

	Create a vector from the difference of two 4D points and return
	the vector's length

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param fX X value of the other 4D point
	\param fY Y value of the other 4D point
	\param fZ Z value of the other 4D point
	\param fW W value of the other 4D point
	\return The distance between the two points
	\sa GetDistanceFast(const Vector4D_t *) const, GetDistanceSquared(float,float,float,float) const or GetDistance(float,float,float,float) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistanceFast(float fX,float fY,float fZ,float fW) const
{
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	fW = w-fW;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW));
}

/*! ************************************

	\brief Returns the square of the distance between two 4D points (Good precision)

	Create a vector from the difference of two 4D points and return
	the vector's length

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param pInput Valid pointer to a Vector4D_t structure that contains the other 4D point
	\return The distance between the two points
	\sa GetDistanceFast(float,float,float,float) const, GetDistanceSquared(const Vector4D_t *) const or GetDistance(const Vector4D_t *) const

***************************************/

float BURGER_API Burger::Vector4D_t::GetDistanceFast(const Vector4D_t *pInput) const
{
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	float fW = w-pInput->w;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ)+(fW*fW));
}



/*! ************************************

	\brief Normalize a 4D vector
	
	Set the 4D vector's length to 1.0f
	\sa Normalize(float,float,float,float), Normalize(const Vector4D_t *) or NormalizeFast(void)

***************************************/

void BURGER_API Burger::Vector4D_t::Normalize(void)
{
	float fLengthSquared = (x*x) + (y*y) + (z*z) + (w*w);
	if (fLengthSquared>0.0f) {		// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		x *= fLengthSquared;	// Divide by 1.0f
		y *= fLengthSquared;
		z *= fLengthSquared;
		w *= fLengthSquared;
	}
}

/*! ************************************

	\brief Copy a normalized 4D vector

	Make a copy of a 4D vector and set the copy's
	length to 1.0f

	\param fX X value of the 4D vector to normalize
	\param fY Y value of the 4D vector to normalize
	\param fZ Z value of the 4D vector to normalize
	\param fW W value of the 4D vector to normalize
	\sa Normalize(void), Normalize(const Vector4D_t *) or NormalizeFast(float,float,float,float)

***************************************/

void BURGER_API Burger::Vector4D_t::Normalize(float fX,float fY,float fZ,float fW)
{
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ) + (fW*fW);
	if (fLengthSquared>0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
		fW = fW*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
		fW = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\brief Copy a normalized 4D vector

	Make a copy of a 4D vector and set the copy's
	length to 1.0f

	\param pInput Valid pointer to a Vector4D_t structure that will be copied and normalize
	\sa Normalize(void), Normalize(float,float,float,float) or NormalizeFast(const Vector4D_t *)

***************************************/

void BURGER_API Burger::Vector4D_t::Normalize(const Vector4D_t *pInput)
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ) + (fW*fW);
	if (fLengthSquared>0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
		fW = fW*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
		fW = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\brief Normalize a 4D vector (Good precision)
	
	Set the 4D vector's length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\sa NormalizeFast(float,float,float,float), NormalizeFast(const Vector4D_t *) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector4D_t::NormalizeFast(void)
{
	float fLengthSquared = (x*x) + (y*y) + (z*z) + (w*w);
	if (fLengthSquared>0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		x *= fLengthSquared;	// Divide by 1.0f
		y *= fLengthSquared;
		z *= fLengthSquared;
		w *= fLengthSquared;
	}
}

/*! ************************************

	\brief Copy a normalized 4D vector

	Make a copy of a 4D vector and set the copy's
	length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param fX X value of the 4D vector to normalize
	\param fY Y value of the 4D vector to normalize
	\param fZ Z value of the 4D vector to normalize
	\param fW W value of the 4D vector to normalize
	\sa NormalizeFast(void), NormalizeFast(const Vector4D_t *) or Normalize(float,float,float,float)

***************************************/

void BURGER_API Burger::Vector4D_t::NormalizeFast(float fX,float fY,float fZ,float fW)
{
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ) + (fW*fW);
	if (fLengthSquared>0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
		fW = fW*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
		fW = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\brief Copy a normalized 4D vector

	Make a copy of a 4D vector and set the copy's
	length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param pInput Valid pointer to a Vector4D_t structure that will be copied and normalize
	\sa NormalizeFast(void), NormalizeFast(float,float,float,float) or Normalize(const Vector4D_t *)

***************************************/

void BURGER_API Burger::Vector4D_t::NormalizeFast(const Vector4D_t *pInput)
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fW = pInput->w;
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ) + (fW*fW);
	if (fLengthSquared>0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
		fW = fW*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
		fW = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\fn float Burger::Vector4D_t::Dot3(float fX,float fY,float fZ) const
	\brief Return a dot product of two 3D vectors
	\param fX X value to dot against
	\param fY Y value to dot against
	\param fZ Z value to dot against

	Performs a dot product with the w term set to 0.0f

	\return Dot product of the two vectors
	\sa Dot3(const Vector4D_t *) const or Dot(float,float,float,float) const

***************************************/

/*! ************************************

	\fn float Burger::Vector4D_t::Dot3(const Vector4D_t *pInput) const
	\brief Return a dot product of two 4D vectors using only x,y and z

	Performs a dot product with the w term set to 0.0f

	\param pInput Valid pointer to a Vector4D_t structure to perform a three term dot product against
	\return Dot product of the two vectors
	\sa Dot3(float,float,float) const or Dot(const Vector4D_t *) const

***************************************/


/*! ************************************

	\fn void Burger::Vector4D_t::QuaternionNegate(void)
	\brief Negate a quaternion

	Set the x,y,z and w values to -x,-y,-z and w respectively
	\sa QuaternionNegate(const Vector4D_t *) or QuaternionNegate(float,float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::QuaternionNegate(float fX,float fY,float fZ,float fW)
	\brief Negate the input quaternion and store it in this structure

	Set the x,y,z and w values to -fX,-fY,-fZ and fW respectively

	\param fX X value to negate
	\param fY Y value to negate
	\param fZ Z value to negate
	\param fW W value to negate
	\sa QuaternionNegate(const Vector4D_t *) or QuaternionNegate(void)

***************************************/

/*! ************************************

	\fn void Burger::Vector4D_t::QuaternionNegate(const Vector4D_t *pInput)
	\brief Make a copy of the input quaternion after it's been negated

	Copy the input data and negate it in the form of -x, -y, -z and w

	\param pInput Valid pointer to a Vector4D_t structure to copy and negate as a quaternion
	\sa QuaternionNegate(float,float,float,float) or QuaternionNegate(void)

***************************************/


/*! ************************************

	\brief Multiply a quaternion by another quaternion

	Apply the multiplication using this formula
	from Wikipedia http://en.wikipedia.org/wiki/Quaternion#Ordered_list_form

	Quaternion 1 is this instance and Quaternion2 is the parameter

	\code
	x = ((y1*z2)-(z1*y2))+(w1*x2)+(x1*w2);
	y = ((z1*x2)-(x1*z2))+(w1*y2)+(y1*w2);
	z = ((x1*y2)-(y1*x2))+(w1*z2)+(z1*w2);
	w = (w1*w2)-((x1*x2)+(y1*y2)+(z1*z2));
	\endcode

	\param pInput Valid pointer to a Vector4D_t quaternion to multiply against
	\sa QuaternionMul(const Vector4D_t *,const Vector4D_t *)

***************************************/

void BURGER_API Burger::Vector4D_t::QuaternionMul(const Vector4D_t *pInput)
{
	float fX = ((y*pInput->z) - (z*pInput->y)) + (w*pInput->x)+(x*pInput->w);
	float fY = ((z*pInput->x) - (x*pInput->z)) + (w*pInput->y)+(y*pInput->w);
	float fZ = ((x*pInput->y) - (y*pInput->x)) + (w*pInput->z)+(z*pInput->w);
	float fW = (w*pInput->w) - ((x*pInput->x) + (y*pInput->y) + (z*pInput->z));

	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\brief Multiply a quaternion by another quaternion and store it here

	Apply the multiplication using this formula
	from Wikipedia http://en.wikipedia.org/wiki/Quaternion#Ordered_list_form

	Quaternion 1 is the first parameter and Quaternion2 is the second parameter

	\code
	x = ((y1*z2)-(z1*y2))+(w1*x2)+(x1*w2);
	y = ((z1*x2)-(x1*z2))+(w1*y2)+(y1*w2);
	z = ((x1*y2)-(y1*x2))+(w1*z2)+(z1*w2);
	w = (w1*w2)-((x1*x2)+(y1*y2)+(z1*z2));
	\endcode

	\param pInput1 Valid pointer to a Vector4D_t quaternion to multiply with
	\param pInput2 Valid pointer to a Vector4D_t quaternion to multiply against
	\sa QuaternionMul(const Vector4D_t *)

***************************************/

void BURGER_API Burger::Vector4D_t::QuaternionMul(const Vector4D_t *pInput1,const Vector4D_t *pInput2)
{
	float fX = ((pInput1->y*pInput2->z) - (pInput1->z*pInput2->y)) + (pInput1->w*pInput2->x) + (pInput1->x*pInput2->w);
	float fY = ((pInput1->z*pInput2->x) - (pInput1->x*pInput2->z)) + (pInput1->w*pInput2->y) + (pInput1->y*pInput2->w);
	float fZ = ((pInput1->x*pInput2->y) - (pInput1->y*pInput2->x)) + (pInput1->w*pInput2->z) + (pInput1->z*pInput2->w);
	float fW = (pInput1->w*pInput2->w) - ((pInput1->x*pInput2->x) + (pInput1->y*pInput2->y) + (pInput1->z*pInput2->z));

	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

/*! ************************************

	\brief Multiply a quaternion by another quaternion

	Apply the multiplication using this formula
	from Wikipedia http://en.wikipedia.org/wiki/Quaternion#Ordered_list_form

	Quaternion 1 is this instance and Quaternion2 is the parameter

	\code
	x = ((y1*z2)-(z1*y2))+(w1*x2)+(x1*w2);
	y = ((z1*x2)-(x1*z2))+(w1*y2)+(y1*w2);
	z = ((x1*y2)-(y1*x2))+(w1*z2)+(z1*w2);
	w = (w1*w2)-((x1*x2)+(y1*y2)+(z1*z2));
	\endcode

	Afterwards, the quaternion is normalized

	\param pInput Valid pointer to a Vector4D_t quaternion to multiply against
	\sa QuaternionMulNormalize(const Vector4D_t *,const Vector4D_t *) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector4D_t::QuaternionMulNormalize(const Vector4D_t *pInput)
{
	QuaternionMul(pInput);
	Normalize();
}

/*! ************************************

	\brief Multiply a quaternion by another quaternion and store it here

	Apply the multiplication using this formula
	from Wikipedia http://en.wikipedia.org/wiki/Quaternion#Ordered_list_form

	Quaternion 1 is the first parameter and Quaternion2 is the second parameter

	\code
	x = ((y1*z2)-(z1*y2))+(w1*x2)+(x1*w2);
	y = ((z1*x2)-(x1*z2))+(w1*y2)+(y1*w2);
	z = ((x1*y2)-(y1*x2))+(w1*z2)+(z1*w2);
	w = (w1*w2)-((x1*x2)+(y1*y2)+(z1*z2));
	\endcode

	Afterwards, the quaternion is normalized

	\param pInput1 Valid pointer to a Vector4D_t quaternion to multiply with
	\param pInput2 Valid pointer to a Vector4D_t quaternion to multiply against
	\sa QuaternionMulNormalize(const Vector4D_t *) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector4D_t::QuaternionMulNormalize(const Vector4D_t *pInput1,const Vector4D_t *pInput2)
{
	QuaternionMul(pInput1,pInput2);
	Normalize();
}

/*! ************************************

	\brief Compare two Vector4D_t's for bitwise equality
	
	Unlike operator==(const Vector4D_t &) const, this function performs
	a bitwise comparison, which in some cases is faster if pure
	equality detection is desired.

	\param pInput Pointer to the Vector4D_t to compare against
	\return \ref TRUE if equal, \ref FALSE if not
	\sa operator==(const Vector4D_t&) const

***************************************/

Word BURGER_API Burger::Vector4D_t::BitwiseEqual(const Vector4D_t *pInput) const
{
	return (
		(reinterpret_cast<const Word32 *>(&pInput->x)[0] == reinterpret_cast<const Word32 *>(&x)[0]) &&
		(reinterpret_cast<const Word32 *>(&pInput->y)[0] == reinterpret_cast<const Word32 *>(&y)[0]) &&
		(reinterpret_cast<const Word32 *>(&pInput->z)[0] == reinterpret_cast<const Word32 *>(&z)[0]) &&
		(reinterpret_cast<const Word32 *>(&pInput->w)[0] == reinterpret_cast<const Word32 *>(&w)[0]));
}

/*! ************************************

	\fn float & Burger::Vector4D_t::operator[](Word uInput)
	\brief Access the members as an array

	\param uInput 0 for x, 1 for y, 2 for z, 3 for w, any other value is an error
	\sa operator[](Word) const

***************************************/

/*! ************************************

	\fn const float & Burger::Vector4D_t::operator[](Word uInput) const
	\brief Access the members as an array

	\param uInput 0 for x, 1 for y, 2 for z, 3 for w, any other value is an error
	\sa operator[](Word)

***************************************/

/*! ************************************

	\fn Word Burger::Vector4D_t::operator==(const Vector4D_t& rInput) const
	\brief Compare two Vector4D_t's for equality
	
	\param rInput Reference to the Vector4D_t to compare against
	\return \ref TRUE if equal, \ref FALSE if not
	\sa operator!=(const Vector4D_t&) const

***************************************/

/*! ************************************

	\fn Word Burger::Vector4D_t::operator != (const Vector4D_t& rInput) const
	\brief Compare two Vector4D_t's for inequality
	
	\param rInput Reference to the Vector4D_t to compare against
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa operator==(const Vector4D_t&) const

***************************************/

const Burger::Vector4D_t Burger::g_Vector4DZero = {0.0f,0.0f,0.0f,0.0f};	///< Constant of 0.0f,0.0f,0.0f,0.0f
const Burger::Vector4D_t Burger::g_Vector4DOne = {1.0f,1.0f,1.0f,1.0f};	///< Constant of 1.0f,1.0f,1.0f,1.0f
const Burger::Vector4D_t Burger::g_Vector4DOneX = {1.0f,0.0f,0.0f,0.0f};	///< Constant of 1.0f,0.0f,0.0f,0.0f
const Burger::Vector4D_t Burger::g_Vector4DOneY = {0.0f,1.0f,0.0f,0.0f};	///< Constant of 0.0f,1.0f,0.0f,0.0f
const Burger::Vector4D_t Burger::g_Vector4DOneZ = {0.0f,0.0f,1.0f,0.0f};	///< Constant of 0.0f,0.0f,1.0f,0.0f
const Burger::Vector4D_t Burger::g_Vector4DOneW = {0.0f,0.0f,0.0f,1.0f};	///< Constant of 0.0f,0.0f,0.0f,1.0f
