/***************************************

	3D Floating point vector manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvector3d.h"
#include "brfixedvector3d.h"

/*! ************************************

	\struct Burger::Vector3D_t
	\brief 3 dimensional floating point vector

	This 12 byte vector contains x,y and z 32 bit floating point coordinates. A
	set of common functions for simple 3 dimensional math are part of the structure.

	It is expected to be 4 byte aligned and use scalar math. The members
	are hard coded to be "x", "y" and "z" for maximum compatibility

	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::Vector2D_t or Burger::Vector4D_t
	
***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Zero(void)
	\brief Set all values to zero
	\sa Set(const Vector3D_t *) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::One(void)
	\brief Set all values to 1.0f
	\sa Set(const Vector3D_t *) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn float Burger::Vector3D_t::GetX(void) const
	\brief Return the x component of the vector.
	
	\return The x component of the vector.
	
	\sa SetX(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector3D_t::GetY(void) const
	\brief Return the y component of the vector.
	
	\return The y component of the vector.

	\sa SetY(float)
	
***************************************/

/*! ************************************

	\fn float Burger::Vector3D_t::GetZ(void) const
	\brief Return the z component of the vector.
	
	\return The z component of the vector.

	\sa SetZ(float)
	
***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::SetX(float fX)
	\brief Set the x component of the vector.
	
	\param fX The new x component of the vector.
	
	\sa GetX(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::SetY(float fY)
	\brief Set the y component of the vector.
	
	\param fY The new y component of the vector.
	
	\sa GetY(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::SetZ(float fZ)
	\brief Set the z component of the vector.
	
	\param fZ The new z component of the vector.
	
	\sa GetZ(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Set(float fInput)
	\brief Set all of the members to specific value

	Sets the x,y and z values to a specific value
	\param fInput New value for all members
	\sa Zero(void), SetX(float), SetY(float), SetZ(float) or Set(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Set(float fX,float fY)
	\brief Set the values to specific values

	Sets the x and y values to specific values. z is set to 0.0f

	\param fX New x value
	\param fY New y value
	\sa Zero(void), Set(const Vector3D_t *) or Set(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Set(float fX,float fY,float fZ)
	\brief Set the values to specific values

	Sets the x, y and z values to specific values.

	\param fX New x value
	\param fY New y value
	\param fZ New z value
	\sa Zero(void) or Set(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Set(const Vector3D_t *pInput)
	\brief Copy a Vector3D_t

	Make a copy of a Vector3D_t

	\param pInput Valid pointer to a Vector3D_t structure to copy 
	\sa Set(float,float,float) or Zero(void)

***************************************/

/*! ************************************

	\brief Convert a fixed point vector into a floating point vector

	\param pInput Valid pointer to a FixedVector3D_t structure to copy 

	\sa Set(const Vector3D_t *), Set(float,float,float) or Zero(void)

***************************************/

void BURGER_API Burger::Vector3D_t::Set(const FixedVector3D_t *pInput)
{
	FixedToFloat(&x,&pInput->x);
	FixedToFloat(&y,&pInput->y);
	FixedToFloat(&z,&pInput->z);
}

/*! ************************************

	\fn void Burger::Vector3D_t::Negate(void)
	\brief Negate a 3D vector

	Set the x,y and z values to -x,-y and -z respectively
	\sa Negate(const Vector3D_t *) or Negate(float,float,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Negate(float fX,float fY,float fZ)
	\brief Negate the input and store it in this structure

	Set the x,y and z values to -fX,-fY and -fZ respectively

	\param fX X value to negate
	\param fY Y value to negate
	\param fZ Z value to negate
	\sa Negate(const Vector3D_t *) or Negate(void)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Negate(const Vector3D_t *pInput)
	\brief Make a copy of the input after it's been negated

	Copy the input data and negate it.

	\param pInput Valid pointer to a Vector3D_t structure to copy and negate
	\sa Negate(float,float,float) or Negate(void)

***************************************/


/*! ************************************

	\brief Interpolate between two vectors

	Given a factor between 0.0f and 1.0f inclusive, perform
	a linear scale between the two vectors and return pFrom
	if the factor is 0.0f and pTo if the factor is 1.0f

	No clamping is performed.

	\param pFrom Valid pointer to a Vector3D_t structure for a factor of 0.0f
	\param pTo Valid pointer to a Vector3D_t structure for a factor of 1.0f
	\param fFactor Scale value between 0.0f and 1.0f inclusive
	\sa Vector2D_t::Interpolate() and Vector4D_t::Interpolate()

***************************************/

void BURGER_API Burger::Vector3D_t::Interpolate(const Vector3D_t *pFrom,const Vector3D_t *pTo,float fFactor)
{
	float fFromX = pFrom->x;
	float fFromY = pFrom->y;
	float fFromZ = pFrom->z;
	x = ((pTo->x - fFromX) * fFactor) + fFromX;
	y = ((pTo->y - fFromY) * fFactor) + fFromY;
	z = ((pTo->z - fFromZ) * fFactor) + fFromZ;
}



/*! ************************************

	\fn float Burger::Vector3D_t::Dot(float fX,float fY,float fZ) const
	\brief Return a dot product of two 3D vectors
	\param fX X value to dot against
	\param fY Y value to dot against
	\param fZ Z value to dot against

	\return Dot product of the two vectors
	\sa Dot(const Vector3D_t *) const

***************************************/

/*! ************************************

	\fn float Burger::Vector3D_t::Dot(const Vector3D_t *pInput) const
	\brief Return a dot product of two 3D vectors
	\param pInput Valid pointer to a Vector3D_t structure to perform a dot product against
	\return Dot product of the two vectors
	\sa Dot(float,float,float) const

***************************************/

#if 0
// X86 assembly version when inlining sucks
BURGER_DECLSPECNAKED float BURGER_API Burger::Vector3D_t::Dot(const Vector3D_t *pInput) const
{
	BURGER_ASM {
	fld		dword ptr [ecx+0]
	fmul	dword ptr [edx+0]
	fld		dword ptr [ecx+4]
	fmul	dword ptr [edx+4]
	fld		dword ptr [ecx+8]
	fmul	dword ptr [edx+8]
	fxch	st(1)
	faddp	st(2),st(0)
	faddp	st(1),st(0)
	ret
	}
}
#endif

/*! ************************************

	\fn void Burger::Vector3D_t::Add(const Vector3D_t *pInput)
	\brief Add another vector to this one

	\param pInput Vector to add
	\sa Add(float), Add(const Vector3D_t *,const Vector3D_t *) or Add(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Add(float fInput)
	\brief Add the same scalar value to each of member values

	\param fInput Value to add
	\sa Add(const Vector3D_t *), Add(const Vector3D_t *,const Vector3D_t *) or Add(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Add(const Vector3D_t *pInput1,const Vector3D_t *pInput2)
	\brief Add two vectors and store the result in this vector

	Initialize this vector with the result of the addition of the two
	input vectors.

	\param pInput1 First vector to add
	\param pInput2 Second vector to add
	\sa Add(float), Add(const Vector3D_t *) or Add(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Add(const Vector3D_t *pInput,float fInput)
	\brief Add a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the addition of the
	input vector and a scalar applied to every member.

	\param pInput Vector to add the scalar to
	\param fInput Scalar to add to the vector
	\sa Add(float), Add(const Vector3D_t *) or Add(const Vector3D_t *,const Vector3D_t *)

***************************************/


/*! ************************************

	\fn void Burger::Vector3D_t::Sub(const Vector3D_t *pInput)
	\brief Subtract another vector from this one

	\param pInput Vector to subtract
	\sa Sub(float), Sub(const Vector3D_t *,const Vector3D_t *) or Sub(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Sub(float fInput)
	\brief Subtract the same scalar value from each of member values

	\param fInput Value to subtract
	\sa Sub(const Vector3D_t *), Sub(const Vector3D_t *,const Vector3D_t *) or Sub(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Sub(const Vector3D_t *pInput1,const Vector3D_t *pInput2)
	\brief Subtract two vectors and store the result in this vector

	Initialize this vector with the result of the subtraction of the two
	input vectors.

	\param pInput1 First vector to subtract from
	\param pInput2 Second vector to subtract with
	\sa Sub(float), Sub(const Vector3D_t *) or Sub(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Sub(const Vector3D_t *pInput,float fInput)
	\brief Subract a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the subtraction of the
	input vector and a scalar applied to every member.

	\param pInput Vector to subract the scalar from
	\param fInput Scalar to subtract from the vector
	\sa Sub(float), Sub(const Vector3D_t *) or Sub(const Vector3D_t *,const Vector3D_t *)

***************************************/



/*! ************************************

	\fn void Burger::Vector3D_t::Mul(const Vector3D_t *pInput)
	\brief Multiply another vector to this one

	\param pInput Vector to multiply
	\sa Mul(float), Mul(const Vector3D_t *,const Vector3D_t *) or Mul(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Mul(float fInput)
	\brief Multiply the same scalar value to each of member values

	\param fInput Value to multiply
	\sa Mul(const Vector3D_t *), Mul(const Vector3D_t *,const Vector3D_t *) or Mul(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Mul(const Vector3D_t *pInput1,const Vector3D_t *pInput2)
	\brief Multiply two vectors and store the result in this vector

	Initialize this vector with the result of the multiplication of the two
	input vectors.

	\param pInput1 First vector to multiply
	\param pInput2 Second vector to multiply
	\sa Mul(float), Mul(const Vector3D_t *) or Mul(const Vector3D_t *,float)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::Mul(const Vector3D_t *pInput,float fInput)
	\brief Multiply a vector and a scalar to every member of the vector and store the result in this vector

	Initialize this vector with the result of the multiplication of the
	input vector and a scalar applied to every member.

	\param pInput Vector to multiply the scalar to
	\param fInput Scalar to multiply to the vector
	\sa Mul(float), Mul(const Vector3D_t *) or Mul(const Vector3D_t *,const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::MulAdd(const Vector3D_t *pMul,const Vector3D_t *pAdd)
	\brief Multiply two vectors and add a third and store the result in this vector

	Initialize this vector with the result of the multiplication of this vector
	and pMul and add the vector pAdd

	this = (this*pMul)+pAdd;

	\param pMul Vector to multiply
	\param pAdd Vector to add
	\sa MulAdd(float,const Vector3D_t *), MulAdd(const Vector3D_t *,const Vector3D_t *,const Vector3D_t *) or MulAdd(const Vector3D_t *,float,const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::MulAdd(const Vector3D_t *pMul1,const Vector3D_t *pMul2,const Vector3D_t *pAdd)
	\brief Multiply two vectors and add a third and store the result in this vector

	Initialize this vector with the result of the multiplication of two vectors
	and add the vector pAdd

	this = (pMul1*pMu2)+pAdd;

	\param pMul1 First vector to multiply
	\param pMul2 Second vector to multiply
	\param pAdd Vector to add
	\sa MulAdd(float,const Vector3D_t *), MulAdd(const Vector3D_t *,const Vector3D_t *) or MulAdd(const Vector3D_t *,float,const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::MulAdd(float fInput,const Vector3D_t *pAdd)
	\brief Multiply this vector by a scalar and add a second vector and store the result in this vector

	Initialize this vector with the result of the multiplication of a vector and a scalar
	and add the vector pAdd

	this = (this*fInput)+pAdd;

	\param fInput Scalar to multiply by all members of this vector
	\param pAdd Vector to add
	\sa MulAdd(const Vector3D_t *,const Vector3D_t *,const Vector3D_t *), MulAdd(const Vector3D_t *,const Vector3D_t *) or MulAdd(const Vector3D_t *,float,const Vector3D_t *)

***************************************/

/*! ************************************

	\fn void Burger::Vector3D_t::MulAdd(const Vector3D_t *pMul,float fInput,const Vector3D_t *pAdd)
	\brief Multiply a vector by a scalar and add a second vector and store the result in this vector

	Initialize this vector with the result of the multiplication of a vector and a scalar
	and add the vector pAdd

	this = (pMul*fInput)+pAdd;

	\param pMul Vector to multiply
	\param fInput Scalar to multiply by all members of the vector pMul
	\param pAdd Vector to add
	\sa MulAdd(const Vector3D_t *,const Vector3D_t *,const Vector3D_t *), MulAdd(const Vector3D_t *,const Vector3D_t *) or MulAdd(float,const Vector3D_t *)

***************************************/



/*! ************************************

	\brief Return a cross product of two 3D vectors

	Calculate the cross product between two 3D vectors. The end result is
	the perpendicular vector

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><th>y</th><th>z</th></tr>
	<tr>
	<td>(y * pInput->z) - (z * pInput->y)</td>
	<td>(z * pInput->x) - (x * pInput->z)</td>
	<td>(x * pInput->y) - (y * pInput->x)</td>
	</tr>
	</table>

	\param pInput Valid pointer to a Vector3D_t structure to perform a cross product against
	\return Cross product of the two vectors
	\sa Cross(const Vector3D_t *,const Vector3D_t *)

***************************************/

void BURGER_API Burger::Vector3D_t::Cross(const Vector3D_t *pInput)
{
	float fX = (y * pInput->z) - (z * pInput->y);
	float fY = (z * pInput->x) - (x * pInput->z);
	float fZ = (x * pInput->y) - (y * pInput->x);
	x = fX;
	y = fY;
	z = fZ;
}

/*! ************************************

	\brief Return a cross product of two 3D vectors

	Calculate the cross product between two 3D vectors. The end result is
	the perpendicular vector stored in this Vector3D_t instance.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>x</th><th>y</th><th>z</th></tr>
	<tr>
	<td>(pInput1->y * pInput2->z) - (pInput1->z * pInput2->y)</td>
	<td>(pInput1->z * pInput2->x) - (pInput1->x * pInput2->z)</td>
	<td>(pInput1->x * pInput2->y) - (pInput1->y * pInput2->x)</td>
	</tr>
	</table>

	\param pInput1 Valid pointer to a Vector3D_t structure to perform a cross product from
	\param pInput2 Valid pointer to a Vector3D_t structure to perform a cross product against
	\return Cross product of the two vectors
	\sa Cross(const Vector3D_t *)

***************************************/

void BURGER_API Burger::Vector3D_t::Cross(const Vector3D_t *pInput1,const Vector3D_t *pInput2)
{
	float fX = (pInput1->y * pInput2->z) - (pInput1->z * pInput2->y);
	float fY = (pInput1->z * pInput2->x) - (pInput1->x * pInput2->z);
	float fZ = (pInput1->x * pInput2->y) - (pInput1->y * pInput2->x);
	x = fX;
	y = fY;
	z = fZ;
}

#if 0
//#if defined(BURGER_X86) && (defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || defined(BURGER_MSVC))
BURGER_DECLSPECNAKED void Burger::Vector3D_t::Cross(const Vector3D_t *pInput1,const Vector3D_t *pInput2)
{
	BURGER_ASM {
	mov		eax,[esp+4]
	fld		dword ptr [edx+4]
	fmul	dword ptr [eax+8]
	fld		dword ptr [edx+8]
	fmul	dword ptr [eax+0]
	fld		dword ptr [edx+0]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+8]
	fmul	dword ptr [eax+4]
	fld		dword ptr [edx+0]
	fmul	dword ptr [eax+8]
	fld		dword ptr [edx+4]
	fmul	dword ptr [eax+0]
	fxch	st(2)
	fsubp	st(5),st
	fsubp	st(3),st
	mov		eax,[esp+0]
	add		esp,8
	fsubp	st(1),st
	fxch	st(2)
	fstp	dword ptr [ecx+0]
	fstp	dword ptr [ecx+4]
	fstp	dword ptr [ecx+8]
	jmp		eax
	}
}
#endif

/*! ************************************

	\fn float Burger::Vector3D_t::GetLengthSquared(void) const
	\brief Returns the square of the length of a 3D vector
	\return x*x+y*y+z*z
	\sa GetLengthFast(void) const or GetLength(void) const

***************************************/

#if 0
BURGER_DECLSPECNAKED float Burger::Vector3D_t::GetLengthSquared(void) const
{
	BURGER_ASM {
	fld		dword ptr [ecx+0]		// Get X
	fmul	st,st					// Square it
	fld		dword ptr [ecx+4]		// Get Y
	fmul	st,st					// Square it
	fld		dword ptr [ecx+8]		// Get Z
	fmul	st,st					// Square it
	fxch	st(1)					// Swap to give Z time to process
	faddp	st(2), st(0)			// Add X and Y
	faddp	st(1), st(0)			// The only stall when Z is added
	ret
	}
}
#endif

/*! ************************************

	\fn float Burger::Vector3D_t::GetLength(void) const
	\brief Return the length of a vector (High precision)
	\return sqrt(x*x+y*y+z*z)
	\sa GetLengthFast(void) const or GetLengthSquared(void) const

***************************************/

/*! ************************************

	\fn float Burger::Vector3D_t::GetLengthFast(void) const
	\brief Return the length of a vector (Good precision)

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\return sqrt(x*x+y*y+z*z)
	\sa GetLength(void) const or GetLengthSquared(void) const

***************************************/

#if 0
float BURGER_API Burger::Vector3D_t::GetLengthFast(void) const
{
	float n = (x*x) + (y*y) + (z*z);
	if (n) {				// Valid number
		Word32 num = ((Word32 *)&n)[0];
		num = (num>>(23-TABLEBITS))&(0xFFFFFF>>(23-TABLEBITS));	// leave only the mantissa
		Word32 Result = (BurgerSqrtTable[num] + (((Word32 *)&n)[0]&0x7F000000))>>1;
		return ((float *)&Result)[0];
	}
	return 0.0f;		// Square root of zero
}
#endif

/*! ************************************

	\brief Normalize a 3D vector to a specific length (High precision)
	
	Set the 3D vector's length to fInput

	\note fInput cannot be zero or negative. The vector will be set to 0.0f in this case

	\param fInput Length to set the vector to
	\sa SetLengthFast(float) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector3D_t::SetLength(float fInput)
{
	if (fInput>0.0f) {	// Handles NaN
		float fLength = Sqrt((x*x) + (y*y) + (z*z));
		if (fLength>0.0f) {		// Handles NaN
			fInput = fInput/fLength;	// Scale to the new length
			x *= fInput;
			y *= fInput;
			z *= fInput;
		} else {
			x = fInput;
			y = 0.0f;
			z = 0.0f;
		}
	} else {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
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

void BURGER_API Burger::Vector3D_t::SetLengthFast(float fInput)
{
	if (fInput>0.0f) {		// Handles NaN
		float fLength = Sqrt((x*x) + (y*y) + (z*z));
		if (fLength>0.0f) {	// Handles NaN
			fInput = fInput/fLength;		// Scale to the new length
			x *= fInput;
			y *= fInput;
			z *= fInput;
		} else {
			x = fInput;
			y = 0.0f;
			z = 0.0f;
		}
	} else {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}



/*! ************************************

	\brief Returns the square of the distance between two 3D points

	Create a vector from the difference of two 3D points and return
	the vector's length squared

	\param fX X value of the other 3D point
	\param fY Y value of the other 3D point
	\param fZ Z value of the other 3D point
	\return The distance between the two points, squared
	\sa GetDistanceSquared(const Vector3D_t *) const, GetDistance(float,float,float) const or GetDistanceFast(float,float,float) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistanceSquared(float fX,float fY,float fZ) const
{
	// Don't worry about absolute values. The squares will remove it
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	return (fX*fX)+(fY*fY)+(fZ*fZ);
}

/*! ************************************

	\brief Returns the square of the distance between two 3D points

	Create a vector from the difference of two 3D points and return
	the vector's length squared

	\param pInput Valid pointer to a Vector3D_t structure that contains the other 3D point
	\return The distance between the two points, squared
	\sa GetDistanceSquared(float,float,float) const, GetDistance(const Vector3D_t *) const or GetDistanceFast(const Vector3D_t *) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistanceSquared(const Vector3D_t *pInput) const
{
	// Don't worry about absolute values. The squares will remove it
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	return (fX*fX)+(fY*fY)+(fZ*fZ);
}

/*! ************************************

	\brief Returns the square of the distance between two 3D points

	Create a vector from the difference of two 3D points and return
	the vector's length

	\param fX X value of the other 3D point
	\param fY Y value of the other 3D point
	\param fZ Z value of the other 3D point
	\return The distance between the two points
	\sa GetDistance(const Vector3D_t *) const, GetDistanceSquared(float,float,float) const or GetDistanceFast(float,float,float) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistance(float fX,float fY,float fZ) const
{
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ));
}

/*! ************************************

	\brief Returns the square of the distance between two 3D points

	Create a vector from the difference of two 3D points and return
	the vector's length

	\param pInput Valid pointer to a Vector3D_t structure that contains the other 3D point
	\return The distance between the two points
	\sa GetDistance(float,float,float) const, GetDistanceSquared(const Vector3D_t *) const or GetDistanceFast(const Vector3D_t *) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistance(const Vector3D_t *pInput) const
{
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ));
}

/*! ************************************

	\brief Returns the square of the distance between two 3D points (Good precision)

	Create a vector from the difference of two 3D points and return
	the vector's length

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param fX X value of the other 3D point
	\param fY Y value of the other 3D point
	\param fZ Z value of the other 3D point
	\return The distance between the two points
	\sa GetDistanceFast(const Vector3D_t *) const, GetDistanceSquared(float,float,float) const or GetDistance(float,float,float) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistanceFast(float fX,float fY,float fZ) const
{
	fX = x-fX;
	fY = y-fY;
	fZ = z-fZ;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ));
}

/*! ************************************

	\brief Returns the square of the distance between two 3D points (Good precision)

	Create a vector from the difference of two 3D points and return
	the vector's length

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param pInput Valid pointer to a Vector3D_t structure that contains the other 3D point
	\return The distance between the two points
	\sa GetDistanceFast(float,float,float) const, GetDistanceSquared(const Vector3D_t *) const or GetDistance(const Vector3D_t *) const

***************************************/

float BURGER_API Burger::Vector3D_t::GetDistanceFast(const Vector3D_t *pInput) const
{
	float fX = x-pInput->x;
	float fY = y-pInput->y;
	float fZ = z-pInput->z;
	return Sqrt((fX*fX)+(fY*fY)+(fZ*fZ));
}

#if 0
float BURGER_API Burger::Vector3D_t::GetDistanceFast(const Vector3D_t *pInput) const
{
	float x,y,z;
	x = Input1->x-Input2->x;
	y = Input1->y-Input2->y;
	z = Input1->z-Input2->z;
	float n = ((x*x)+(y*y)+(z*z));
	if (n) {	/* Valid number */
		Word32 num = ((Word32 *)&n)[0];
		num = (num>>(23-TABLEBITS))&(0xFFFFFF>>(23-TABLEBITS));	// leave only the mantissa
		Word32 Result = (BurgerSqrtTable[num] + (((Word32 *)&n)[0]&0x7F000000))>>1;
		return ((float *)&Result)[0];
	}
	return 0.0f;
}
#endif

/*! ************************************

	\brief Normalize a 3D vector
	
	Set the 3D vector's length to 1.0f
	\sa Normalize(float,float,float), Normalize(const Vector3D_t *) or NormalizeFast(void)

***************************************/

void BURGER_API Burger::Vector3D_t::Normalize(void)
{
	float fLengthSquared = (x*x) + (y*y) + (z*z);
	if (fLengthSquared > 0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		x *= fLengthSquared;	// Divide by 1.0f
		y *= fLengthSquared;
		z *= fLengthSquared;
	}
}

#if 0
BURGER_DECLSPECNAKED void Burger::Vector3D_t::Normalize(void)
{
	BURGER_ASM {
	fld dword ptr [ecx]
	fmul st,st
	fld dword ptr [ecx+4]
	fmul st,st
	fld dword ptr [ecx+8]
	fmul st,st
	fxch st(1)
	faddp st(2),st(0)
	push eax
	faddp st(1),st(0)
	fsqrt
	fld dword ptr [One]
	fxch st(1)
	fstp dword ptr [esp]
	mov eax,[esp]
	test eax,0x7FFFFFFF
	jz NoGood
	fdiv dword ptr [esp]
	pop	eax
	fld dword ptr [ecx+0]
	fld dword ptr [ecx+4]
	fld dword ptr [ecx+8]
	fxch st(2)
	fmul st(0),st(3)
	fxch st(1)
	fmul st(0),st(3)
	fxch st(2)
	fmulp st(3),st(0)
	fstp dword ptr [ecx+0]
	fstp dword ptr [ecx+4]
	fstp dword ptr [ecx+8]
	ret
NoGood:
	pop eax
	fstp st(0)
	ret
	}
}
#endif

/*! ************************************

	\brief Copy a normalized 3D vector

	Make a copy of a 3D vector and set the copy's
	length to 1.0f

	\param fX X value of the 3D vector to normalize
	\param fY Y value of the 3D vector to normalize
	\param fZ Z value of the 3D vector to normalize
	\sa Normalize(void), Normalize(const Vector3D_t *) or NormalizeFast(float,float,float)

***************************************/

void BURGER_API Burger::Vector3D_t::Normalize(float fX,float fY,float fZ)
{
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ);
	if (fLengthSquared > 0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
}

/*! ************************************

	\brief Copy a normalized 3D vector

	Make a copy of a 3D vector and set the copy's
	length to 1.0f

	\param pInput Valid pointer to a Vector3D_t structure that will be copied and normalize
	\sa Normalize(void), Normalize(float,float,float) or NormalizeFast(const Vector3D_t *)

***************************************/

void BURGER_API Burger::Vector3D_t::Normalize(const Vector3D_t *pInput)
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ);
	if (fLengthSquared > 0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
}

#if 0
BURGER_DECLSPECNAKED void BURGER_API Burger::Vector3D_t::Normalize(const Vector3D_t *pInput)
{
	BURGER_ASM {
	fld dword ptr [edx]
	fmul st,st
	fld dword ptr [edx+4]
	fmul st,st
	fld dword ptr [edx+8]
	fmul st,st
	fxch st(1)
	faddp st(2),st(0)
	push eax
	faddp st(1),st(0)
	fsqrt
	fld dword ptr [One]
	fxch st(1)
	fstp dword ptr [esp]
	mov eax,[esp]
	test eax,0x7FFFFFFF
	jz NoGood
	fdiv dword ptr [esp]
	pop	eax
	fld dword ptr [edx+0]
	fld dword ptr [edx+4]
	fld dword ptr [edx+8]
	fxch st(2)
	fmul st(0),st(3)
	fxch st(1)
	fmul st(0),st(3)
	fxch st(2)
	fmulp st(3),st(0)
	fstp dword ptr [ecx+0]
	fstp dword ptr [ecx+4]
	fstp dword ptr [ecx+8]
	ret
NoGood:
	pop eax
	fstp st(0)
	ret
	}
}
#endif

/*! ************************************

	\brief Calculate Plane Normal from three points on plane's

	The three input vectors are treated as points on a single plane
	and are used to make two vectors which a cross product is
	applied to create the normal.

	\code
		Vector3D_t temp1;
		Vector3D_t temp2;
		temp1.Sub(pOrigin,pPoint1);
		temp2.Sub(pOrigin,pPoint2);
		Cross(&temp1,&temp2);
		Normalize();
	\endcode

	\param pOrigin Pointer to the origin point on the plane
	\param pPoint1 Pointer to the first ray endpoint
	\param pPoint2 Pointer to the second ray endpoint
	\sa Cross() and Normalize()

***************************************/

void BURGER_API Burger::Vector3D_t::PlaneNormal(const Vector3D_t *pOrigin,const Vector3D_t *pPoint1,const Vector3D_t *pPoint2)
{
	float fVect0x = pOrigin->x-pPoint1->x;		// First difference
	float fVect0y = pOrigin->y-pPoint1->y;
	float fVect0z = pOrigin->z-pPoint1->z;

	float fVect1x = pOrigin->x-pPoint2->x;		// Second difference
	float fVect1y = pOrigin->y-pPoint2->y;
	float fVect1z = pOrigin->z-pPoint2->z;	

	x = (fVect0y * fVect1z) - (fVect0z * fVect1y);
	y = (fVect0z * fVect1x) - (fVect0x * fVect1z);
	z = (fVect0x * fVect1y) - (fVect0y * fVect1x);
	Normalize();
}


/*! ************************************

	\brief Normalize a 3D vector (Good precision)
	
	Set the 3D vector's length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\sa NormalizeFast(float,float,float), NormalizeFast(const Vector3D_t *) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector3D_t::NormalizeFast(void)
{
	float fLengthSquared = (x*x) + (y*y) + (z*z);
	if (fLengthSquared > 0.0f) {		// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		x *= fLengthSquared;	// Divide by 1.0f
		y *= fLengthSquared;
		z *= fLengthSquared;
	}
}

#if 0
void BURGER_API Burger::Vector3D_t::NormalizeFast(void)
{
	float n = ((Input->x*Input->x) + (Input->y*Input->y) + (Input->z*Input->z));
	if (n) {	/* Valid number */
		Word32 num = ((Word32 *)&n)[0];
		num = (num>>(23-TABLEBITS))&(0xFFFFFF>>(23-TABLEBITS));	// leave only the mantissa
		Word32 Result = (BurgerSqrtTable[num] + (((Word32 *)&n)[0]&0x7F000000))>>1;
		if (Result) {
			float Len = ((float *)&Result)[0];
			Len= 1.0f / Len;	/* Get the reciprocal */
			Input->x *= Len;	/* Perform the division */
			Input->y *= Len;
			Input->z *= Len;
		}
	}
}
#endif


/*! ************************************

	\brief Copy a normalized 3D vector

	Make a copy of a 3D vector and set the copy's
	length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param fX X value of the 3D vector to normalize
	\param fY Y value of the 3D vector to normalize
	\param fZ Z value of the 3D vector to normalize
	\sa NormalizeFast(void), NormalizeFast(const Vector3D_t *) or Normalize(float,float,float)

***************************************/

void BURGER_API Burger::Vector3D_t::NormalizeFast(float fX,float fY,float fZ)
{
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ);
	if (fLengthSquared > 0.0f) {	// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
}

#if 0
void BURGER_API Burger::Vector3D_t::NormalizeFast(float fX,float fY,float fZ)
{
	float n = ((x*x) + (y*y) + (z*z));
	if (n) {	/* Valid number */
		Word32 num = ((Word32 *)&n)[0];
		num = (num>>(23-TABLEBITS))&(0xFFFFFF>>(23-TABLEBITS));	// leave only the mantissa
		Word32 Result = (BurgerSqrtTable[num] + (((Word32 *)&n)[0]&0x7F000000))>>1;
		if (Result) {
			float Len = ((float *)&Result)[0];
			Len= 1.0f / Len;	/* Get the reciprocal */
			Output->x = x * Len;	/* Perform the division */
			Output->y = y * Len;
			Output->z = z * Len;
			return;
		}
	}
	Output->x = 0;
	Output->y = 0;
	Output->z = 0;
}
#endif

/*! ************************************

	\brief Copy a normalized 3D vector

	Make a copy of a 3D vector and set the copy's
	length to 1.0f

	On some systems, lower precision instructions are used to increase speed at the cost
	of precision. If the calculation doesn't rely on high precision, use this
	function for higher performance

	\param pInput Valid pointer to a Vector3D_t structure that will be copied and normalize
	\sa NormalizeFast(void), NormalizeFast(float,float,float) or Normalize(const Vector3D_t *)

***************************************/

void BURGER_API Burger::Vector3D_t::NormalizeFast(const Vector3D_t *pInput)
{
	float fX = pInput->x;
	float fY = pInput->y;
	float fZ = pInput->z;
	float fLengthSquared = (fX*fX) + (fY*fY) + (fZ*fZ);
	if (fLengthSquared > 0.0f) {		// Handles NaN
		fLengthSquared = 1.0f / Sqrt(fLengthSquared);	// Reciprocal
		fX = fX*fLengthSquared;			// Divide by 1.0f
		fY = fY*fLengthSquared;
		fZ = fZ*fLengthSquared;
	} else {
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
	}
	x = fX;
	y = fY;
	z = fZ;
}

#if 0
void BURGER_API Burger::Vector3D_t::NormalizeFast(const Vector3D_t *pInput)
{
	float n = ((Input->x*Input->x) + (Input->y*Input->y) + (Input->z*Input->z));
	if (n) {	/* Valid number */
		Word32 num = ((Word32 *)&n)[0];
		num = (num>>(23-TABLEBITS))&(0xFFFFFF>>(23-TABLEBITS));	// leave only the mantissa
		Word32 Result = (BurgerSqrtTable[num] + (((Word32 *)&n)[0]&0x7F000000))>>1;
		if (Result) {
			float Len = ((float *)&Result)[0];
			Len= 1.0f / Len;	/* Get the reciprocal */
			Output->x = Input->x * Len;	/* Perform the division */
			Output->y = Input->y * Len;
			Output->z = Input->z * Len;
		}
	}
}
#endif

/*! ************************************

	\brief Check if two vectors are equal

	Perform a binary compare of two vectors for equality.

	\note This performs a binary comparison, so NaN forced
	inequality will not work. Use Vector3D_t::Equal(const Vector3D_t *,float) const
	instead

	\param pInput Vector to compare against
	\return \ref TRUE if the vectors are a binary match or \ref FALSE if not.

	\sa Vector3D_t::Equal(const Vector3D_t *,float) const

***************************************/

Word BURGER_API Burger::Vector3D_t::Equal(const Vector3D_t *pInput) const
{
	// I do this since floating point compares just plain suck 
	// on intel and PowerPC processors

	Word32 uTemp = ((reinterpret_cast<const Word32 *>(&x)[0]^reinterpret_cast<const Word32 *>(&pInput->x)[0]) |
		(reinterpret_cast<const Word32 *>(&y)[0]^reinterpret_cast<const Word32 *>(&pInput->y)[0]) |
		(reinterpret_cast<const Word32 *>(&z)[0]^reinterpret_cast<const Word32 *>(&pInput->z)[0]));
	return (uTemp==0);
}

/*! ************************************

	\brief Check if two vectors are equal within an epsilon range

	Perform a difference of each member and if they are less than
	a specific range, return \ref TRUE for equality

	\param pInput Vector to compare against
	\param fRange Epsilon range to use for equality
	\return \ref TRUE if the vectors are close enough to be equal or \ref FALSE if not.
	\sa Vector3D_t::Equal(const Vector3D_t *) const

***************************************/

Word BURGER_API Burger::Vector3D_t::Equal(const Vector3D_t *pInput,float fRange) const
{
	float fX = Abs(x-pInput->x);
	float fY = Abs(y-pInput->y);
	float fZ = Abs(z-pInput->z);
	if ((fX<fRange) &&
		(fY<fRange) &&
		(fZ<fRange)) {
		return TRUE;
	}
	return FALSE;
}


/*! ************************************

	\fn float & Burger::Vector3D_t::operator[](Word uInput)
	\brief Access the members as an array

	\param uInput 0 for x, 1 for y, 2 for z, any other value is an error
	\sa operator[](Word) const

***************************************/

/*! ************************************

	\fn const float & Burger::Vector3D_t::operator[](Word uInput) const
	\brief Access the members as an array

	\param uInput 0 for x, 1 for y, 2 for z, any other value is an error
	\sa operator[](Word)

***************************************/

/*! ************************************

	\fn Word Burger::Vector3D_t::operator == (const Vector3D_t& rInput) const
	\brief Compare two Vector3D_t's for equality
	
	\param rInput Reference to the Vector3D_t to compare against
	\return \ref TRUE if equal, \ref FALSE if not
	\sa Vector3D_t::operator != (const Vector3D_t& rInput) const

***************************************/

/*! ************************************

	\fn Word Burger::Vector3D_t::operator != (const Vector3D_t& rInput) const
	\brief Compare two Vector3D_t's for inequality
	
	\param rInput Reference to the Vector3D_t to compare against
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa Vector3D_t::operator == (const Vector3D_t& rInput) const

***************************************/

/*! ************************************

	\struct Burger::Word32ToVector3D_t {
	\brief Structure to create a Vector3D_t with 32 bit unsigned integer constants

	There is no standard for initializing 32 bit floats at compile time
	without generating startup glue code. This
	structure fixes the problem by creating a union between the Vector3D_t
	structure and an array of data that maps directly over it.

	\code
	// Initialize with 3 32 bit unsigned integers
	const Burger::Word32ToVector3D_t Burger::g_Vector3DInfinity = {{0x7F800000U,0x7F800000U,0x7F800000U}};

	\endcode

***************************************/

/*! ************************************

	\fn Burger::Word32ToVector3D_t::operator const Vector3D_t & () const
	\brief Accessor to load the Vector3D_t
	
	\sa Burger::Vector2D_t or Burger::Vector4D_t

***************************************/

const Burger::Vector3D_t Burger::g_Vector3DZero = {0.0f,0.0f,0.0f};	///< Constant of 0.0f,0.0f,0.0f
const Burger::Vector3D_t Burger::g_Vector3DOne = {1.0f,1.0f,1.0f};	///< Constant of 1.0f,1.0f,1.0f
const Burger::Vector3D_t Burger::g_Vector3DOneX = {1.0f,0.0f,0.0f};	///< Constant of 1.0f,0.0f,0.0f
const Burger::Vector3D_t Burger::g_Vector3DOneY = {0.0f,1.0f,0.0f};	///< Constant of 0.0f,1.0f,0.0f
const Burger::Vector3D_t Burger::g_Vector3DOneZ = {0.0f,0.0f,1.0f};	///< Constant of 0.0f,0.0f,1.0f
const Burger::Word32ToVector3D_t Burger::g_Vector3DMax = { { 0x7F7FFFFFU,0x7F7FFFFFU,0x7F7FFFFFU} };	///< Constant of Burger::g_fMax,Burger::g_fMax,Burger::g_fMax
const Burger::Word32ToVector3D_t Burger::g_Vector3DInfinity = { {0x7F800000U,0x7F800000U,0x7F800000U} };	///< Constant of Burger::g_fInf,Burger::g_fInf,Burger::g_fInf

