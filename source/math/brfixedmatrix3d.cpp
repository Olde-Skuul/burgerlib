/***************************************

    3D fixed point matrix manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfixedmatrix3d.h"

/*! ************************************

	\struct Burger::FixedMatrix3D_t
	\brief 3 dimensional fixed point matrix
	
	This 36 byte structure contains 9 32-bit
	Fixed point values arranged as a 3x3 matrix.
	
	\note Since this is a structure, there is no
	constructor or destructor, so assume the data 
	is uninitialized when creating this data type.

	\sa Burger::FixedVector3D_t, Burger::FixedMatrix4D_t or Burger::Matrix3D_t

***************************************/

/*! ************************************

	\brief Initialize the matrix to zero
	
	Fill every entry in the matrix with zero.
	The destination matrix is assumed to be uninitialized.

	\sa Identity(void)

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Zero(void)
{
	fixed16_16_t fTemp = 0;	// Force the compiler to use a register
	x.x = fTemp;		// Fill all the elements with zero
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

	\brief Initialize the matrix to identity.
	
	Fill in all entries with zero except x,x, y.y and z.z.
	Those three will be filled in with \ref BURGER_FLOAT_TO_FIXED(1.0f)
	The destination matrix is assumed to be uninitialized.

	\sa Zero(void)

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Identity(void)
{
	x.x = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave X alone
	x.y = 0;
	x.z = 0;
	y.x = 0;
	y.y = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave Y alone
	y.z = 0;
	z.x = 0;
	z.y = 0;
	z.z = BURGER_FLOAT_TO_FIXED(1.0f);	// Leave Z alone
}

/*! ************************************

	\brief Convert a floating point matrix into a \ref fixed16_16_t matrix
	
	Using round to nearest, convert a matrix using floating point
	values into one that has \ref fixed16_16_t values.
	The destination matrix is assumed to be uninitialized.

	\param pInput Pointer to a valid Matrix3D_t
	\sa float_to_fixed_round(fixed16_16_t *,float)

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Set(const Matrix3D_t *pInput)
{
	float_to_fixed_round(&x.x,pInput->x.x);
	float_to_fixed_round(&x.y,pInput->x.y);
	float_to_fixed_round(&x.z,pInput->x.z);
	float_to_fixed_round(&y.x,pInput->y.x);
	float_to_fixed_round(&y.y,pInput->y.y);
	float_to_fixed_round(&y.z,pInput->y.z);
	float_to_fixed_round(&z.x,pInput->z.x);
	float_to_fixed_round(&z.y,pInput->z.y);
	float_to_fixed_round(&z.z,pInput->z.z);
}

/*! ************************************

	\brief Preform a matrix transposition.
	
	Swap the entries x.y and y.x, x.z and z.x, y,z and z.y 
	to convert a right handed matrix to a left handed
	one and vice versa.
	The matrix is assumed to be already initialized.

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Transpose(void)
{
	fixed16_16_t fTemp1 = x.y;	// Swap x.y and y.x
	fixed16_16_t fTemp2 = y.x;
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

	\brief Preform a matrix transposition.
	
	Swap the entries x.y and y.x, x.z and z.x, y,z and z.y 
	to convert a right handed matrix to a left handed
	one and vice versa. The other three entries are copied.
	The destination matrix is assumed to be uninitialized.
	
	\note This function is optimized to write the new matrix in
	sequential order in the event this function is writing
	to write combined memory.

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Transpose(const FixedMatrix3D_t *pInput)
{
	fixed16_16_t fTemp1 = pInput->x.x;	// I am copying the matrix, this 
	fixed16_16_t fTemp2 = pInput->y.x;	// is why I am copying the x.x, y.y
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
	
	Copy the x row of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetXRow(FixedVector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = x.y;
	pOutput->z = x.z;
}

/*! ************************************

	\brief Return the Y row from a matrix
	
	Copy the y row of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetYRow(FixedVector3D_t *pOutput) const
{
	pOutput->x = y.x;
	pOutput->y = y.y;
	pOutput->z = y.z;
}

/*! ************************************

	\brief Return the Z row from a matrix
	
	Copy the z row of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetZRow(FixedVector3D_t *pOutput) const
{
	pOutput->x = z.x;
	pOutput->y = z.y;
	pOutput->z = z.z;
}

/*! ************************************

	\brief Return the X column from a matrix
	
	Copy the x column of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetXColumn(FixedVector3D_t *pOutput) const
{
	pOutput->x = x.x;
	pOutput->y = y.x;
	pOutput->z = z.x;
}

/*! ************************************

	\brief Return the Y column from a matrix
	
	Copy the y column of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetYColumn(FixedVector3D_t *pOutput) const
{
	pOutput->x = x.y;
	pOutput->y = y.y;
	pOutput->z = z.y;
}

/*! ************************************

	\brief Return the Z column from a matrix
	
	Copy the z column of a matrix into a user supplied \ref FixedVector3D_t
	buffer.

	\param pOutput Pointer to an uninitialized FixedVector3D_t structure to receive the data

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::GetZColumn(FixedVector3D_t *pOutput) const
{
	pOutput->x = x.z;
	pOutput->y = y.z;
	pOutput->z = z.z;
}

/*! ************************************

	\brief Set the X row of a matrix
	
	Overwrite the X row of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new X row

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetXRow(const FixedVector3D_t *pInput)
{
	x.x = pInput->x;
	x.y = pInput->y;
	x.z = pInput->z;
}

/*! ************************************

	\brief Set the Y row of a matrix
	
	Overwrite the Y row of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new Y row

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetYRow(const FixedVector3D_t *pInput)
{
	y.x = pInput->x;
	y.y = pInput->y;
	y.z = pInput->z;
}

/*! ************************************

	\brief Set the Z row of a matrix

	Overwrite the Z row of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new Z row

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetZRow(const FixedVector3D_t *pInput)
{
	z.x = pInput->x;
	z.y = pInput->y;
	z.z = pInput->z;
}

/*! ************************************

	\brief Set the X column of a matrix
	
	Overwrite the X column of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new X column

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetXColumn(const FixedVector3D_t *pInput)
{
	x.x = pInput->x;
	y.x = pInput->y;
	z.x = pInput->z;
}

/*! ************************************

	\brief Set the Y column of a matrix
	
	Overwrite the Y column of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new Y column

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetYColumn(const FixedVector3D_t *pInput)
{
	x.y = pInput->x;
	y.y = pInput->y;
	z.y = pInput->z;
}

/*! ************************************

	\brief Set the Z column of a matrix
	
	Overwrite the Z column of the matrix with the input vector

	\param pInput Pointer to a FixedVector3D_t structure that has the new Z column

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::SetZColumn(const FixedVector3D_t *pInput)
{
	x.z = pInput->x;
	y.z = pInput->y;
	z.z = pInput->z;
}


/*! ************************************

	\brief Multiply all entries by a scalar

	Multiply all values in the matrix by a scalar constant

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(xx*fScale)</td>
	<td>(xy*fScale)</td>
	<td>(xz*fScale)</td>
	</tr>
	<tr><th>y</th>
	<td>(yx*fScale)</td>
	<td>(yy*fScale)</td>
	<td>(yz*fScale)</td>
	</tr>
	<tr><th>z</th>
	<td>(zx*fScale)</td>
	<td>(zy*fScale)</td>
	<td>(zz*fScale)</td>
	</tr>
	</table>

	\param fScale Scalar to multiply all entries by
	\sa Multiply(const FixedMatrix3D_t *,fixed16_16_t)

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Multiply(fixed16_16_t fScale)
{
	x.x = FixedMultiply(x.x,fScale);
	x.y = FixedMultiply(x.y,fScale);
	x.z = FixedMultiply(x.z,fScale);
	y.x = FixedMultiply(y.x,fScale);
	y.y = FixedMultiply(y.y,fScale);
	y.z = FixedMultiply(y.z,fScale);
	z.x = FixedMultiply(z.x,fScale);
	z.y = FixedMultiply(z.y,fScale);
	z.z = FixedMultiply(z.z,fScale);
}

/*! ************************************

	\brief Initialize with a matrix multiplied by a scalar

	Multiply all values of the matrix by a scalar constant
	and store the result in this matrix

	Use this formula to create the final matrix

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th/><th>x</th><th>y</th><th>z</th></tr>
	<tr><th>x</th>
	<td>(pInput->xx*fScale)</td>
	<td>(pInput->xy*fScale)</td>
	<td>(pInput->xz*fScale)</td>
	</tr>
	<tr><th>y</th>
	<td>(pInput->yx*fScale)</td>
	<td>(pInput->yy*fScale)</td>
	<td>(pInput->yz*fScale)</td>
	</tr>
	<tr><th>z</th>
	<td>(pInput->zx*fScale)</td>
	<td>(pInput->zy*fScale)</td>
	<td>(pInput->zz*fScale)</td>
	</tr>
	</table>

	\param pInput Pointer to Matrix to multiply
	\param fScale Scalar to multiply all entries by
	\sa Multiply(fixed16_16_t)

***************************************/

void BURGER_API Burger::FixedMatrix3D_t::Multiply(const FixedMatrix3D_t *pInput,fixed16_16_t fScale)
{
	x.x = FixedMultiply(pInput->x.x,fScale);
	x.y = FixedMultiply(pInput->x.y,fScale);
	x.z = FixedMultiply(pInput->x.z,fScale);
	y.x = FixedMultiply(pInput->y.x,fScale);
	y.y = FixedMultiply(pInput->y.y,fScale);
	y.z = FixedMultiply(pInput->y.z,fScale);
	z.x = FixedMultiply(pInput->z.x,fScale);
	z.y = FixedMultiply(pInput->z.y,fScale);
	z.z = FixedMultiply(pInput->z.z,fScale);
}

/*! ************************************

	\brief Constant 3x3 fixed point identity matrix

***************************************/

const Burger::FixedMatrix3D_t Burger::s_FixedMatrix3DIdentity = {
	{BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f)},
	{BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f),BURGER_FLOAT_TO_FIXED(0.0f)},
	{BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(0.0f),BURGER_FLOAT_TO_FIXED(1.0f)}
};
