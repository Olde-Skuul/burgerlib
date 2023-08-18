/***************************************

    Flash player 2x3 matrix manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashmatrix23.h"
#include "brdebug.h"
#include "brflashutils.h"

/*! ************************************

	\class Burger::Flash::Matrix23
	\brief 2x3 matrix for two dimensional rotations
	
	This class implements the functions to support the
	two dimensional matrix found on page 22
	of the Adobe SWF file format manual
	swf-file-format-spec.pdf

	\sa Flash::Rect

***************************************/

/*! ************************************

	\brief Initialize to an identity matrix
	
	Set to an identity matrix (Scale = 1, all others 0)

	\sa Identity(void)

***************************************/

Burger::Flash::Matrix23::Matrix23() :
	m_fScaleX(1.0f),
	m_fRotateSkew0(0.0f),
	m_fRotateSkew1(0.0f),
	m_fScaleY(1.0f),
	m_fTranslateX(0.0f),
	m_fTranslateY(0.0f)
{
}

/*! ************************************

	\brief Initialize to an identity matrix
	
	Set to an identity matrix (Scale = 1, all others 0)

***************************************/

void BURGER_API Burger::Flash::Matrix23::Identity(void)
{
	m_fScaleX = 1.0f;
	m_fRotateSkew0 = 0.0f;
	m_fRotateSkew1 = 0.0f;
	m_fScaleY = 1.0f;
	m_fTranslateX = 0.0f;
	m_fTranslateY = 0.0f;
}

/*! ************************************

	\brief Concatenate a matrix to this matrix.
	
	Concatenate the other matrix's transform onto this
	one. When transforming points, the input happens first,
	and then our original transform.

	\param pInput Pointer to a matrix to concatenate
	\sa ConcatenateTranslation(float,float) or ConcatenateScale(float)

***************************************/

void BURGER_API Burger::Flash::Matrix23::Concatenate(const Matrix23 *pInput)
{
	// Calculate the translation first, since it can be stored immediately
	float fTranslateX = (m_fScaleX * pInput->m_fTranslateX) + (m_fRotateSkew0 * pInput->m_fTranslateY + m_fTranslateX);
	float fTranslateY = (m_fRotateSkew1 * pInput->m_fTranslateX) + (m_fScaleY * pInput->m_fTranslateY + m_fTranslateY);
	m_fTranslateX = fTranslateX;
	m_fTranslateY = fTranslateY;

	float fScaleX = (m_fScaleX * pInput->m_fScaleX) + (m_fRotateSkew0 * pInput->m_fRotateSkew1);
	float fRotateSkew1 = (m_fRotateSkew1 * pInput->m_fScaleX) + (m_fScaleY * pInput->m_fRotateSkew1);
	float fRotateSkew0 = (m_fScaleX * pInput->m_fRotateSkew0) + (m_fRotateSkew0 * pInput->m_fScaleY);
	float fScaleY = (m_fRotateSkew1 * pInput->m_fRotateSkew0) + (m_fScaleY * pInput->m_fScaleY);
	m_fScaleX = fScaleX;
	m_fRotateSkew0 = fRotateSkew0;
	m_fRotateSkew1 = fRotateSkew1;
	m_fScaleY = fScaleY;
}

/*! ************************************

	\brief Concatenate a translation to this matrix
	
	Concatenate a translation onto the front of this
	matrix. When transforming points, the translation
	happens first, then our original transform.

	\param fX Translation X to concatenate
	\param fY Translation Y to concatenate
	\sa ConcatenateTranslation(const Matrix23 *) or ConcatenateScale(float)

***************************************/

void BURGER_API Burger::Flash::Matrix23::ConcatenateTranslation(float fX,float fY)
{
	m_fTranslateX += m_fScaleX * fX + m_fRotateSkew0 * fY;
	m_fTranslateY += m_fRotateSkew1 * fX + m_fScaleY * fY;
}

/*! ************************************

	\brief Concatenate a scale to this matrix
	
	Concatenate a uniform scale onto the front of this
	matrix. When transforming points, the scale
	happens first, then our original transform.

	\param fScale Scale to concatenate
	\sa ConcatenateTranslation(const Matrix23 *) or ConcatenateScale(float,float)

***************************************/

void BURGER_API Burger::Flash::Matrix23::ConcatenateScale(float fScale)
{
	m_fScaleX *= fScale;
	m_fRotateSkew0 *= fScale;
	m_fRotateSkew1 *= fScale;
	m_fScaleY *= fScale;
}

/*! ************************************

	\brief Interpolate between two matrices
	
	Set this matrix to a blend of pInput1 and pInput2, parameterized by fFactor.

	\param pInput1 Pointer to a matrix for the factor of 0.0f
	\param pInput2 Pointer to a matrix for the factor of 1.0f
	\param fFactor Interpolation factor from 0.0f to 1.0f

***************************************/

void BURGER_API Burger::Flash::Matrix23::Interpolate(const Matrix23 *pInput1,const Matrix23 *pInput2,float fFactor)
{
	m_fScaleX = Burger::interpolate(pInput1->m_fScaleX,pInput2->m_fScaleX,fFactor);
	m_fRotateSkew1 = Burger::interpolate(pInput1->m_fRotateSkew1,pInput2->m_fRotateSkew1,fFactor);
	m_fRotateSkew0 = Burger::interpolate(pInput1->m_fRotateSkew0,pInput2->m_fRotateSkew0,fFactor);
	m_fScaleY = Burger::interpolate(pInput1->m_fScaleY,pInput2->m_fScaleY,fFactor);
	m_fTranslateX = Burger::interpolate(pInput1->m_fTranslateX,pInput2->m_fTranslateX,fFactor);
	m_fTranslateY = Burger::interpolate(pInput1->m_fTranslateY,pInput2->m_fTranslateY,fFactor);
}

/*! ************************************

	\brief Set a scale an rotation to the matrix
	
	Sets the scale and rotation to a matrix while leaving the
	translation values untouched.

	\param fXScale Scale in the X direction
	\param fYScale Scale in the Y direction
	\param fRotation Rotation in radians

***************************************/

void BURGER_API Burger::Flash::Matrix23::SetScaleRotation(float fXScale,float fYScale,float fRotation)
{
	float fCos = get_cosine(fRotation);
	float fSin = get_sine(fRotation);
	m_fScaleX = fXScale * fCos;
	m_fRotateSkew0 = fYScale * -fSin;
	m_fRotateSkew1 = fXScale * fSin;
	m_fScaleY = fYScale * fCos;
}

/*! ************************************

	\brief Read a matrix from the file stream
	
	Read from the bit stream and fill in all of the matrix entries
	using the compressed format Adobe uses for Flash
	files

	\param pStream Pointer to a flash byte stream

***************************************/

void BURGER_API Burger::Flash::Matrix23::Read(Stream* pStream)
{
	// Clear out the bits from the last read
	pStream->ByteAlign();

	// Check if there is a scale

	if (pStream->GetWord(1)) {
		uint_t uBitCount = pStream->GetWord(5);
		m_fScaleX = FIXEDTOFLOAT(pStream->GetInt(uBitCount));
		m_fScaleY = FIXEDTOFLOAT(pStream->GetInt(uBitCount));
	} else {
		m_fScaleX = 1.0f;
		m_fScaleY = 1.0f;
	}

	if (pStream->GetWord(1)) {
		uint_t uBitCount2 = pStream->GetWord(5);
		m_fRotateSkew1 = FIXEDTOFLOAT(pStream->GetInt(uBitCount2));
		m_fRotateSkew0 = FIXEDTOFLOAT(pStream->GetInt(uBitCount2));
	} else {
		m_fRotateSkew0 = 0.0f;
		m_fRotateSkew1 = 0.0f;
	}

	uint_t uTranslateBitCount = pStream->GetWord(5);
	if (uTranslateBitCount) {
		m_fTranslateX = static_cast<float>(pStream->GetInt(uTranslateBitCount));
		m_fTranslateY = static_cast<float>(pStream->GetInt(uTranslateBitCount));
	} else {
		m_fTranslateX = 0.0f;
		m_fTranslateY = 0.0f;
	}
}

/*! ************************************

	\brief Transform a point by this matrix

	\note The output and input pointers can be the same pointer

	\param pOutput Pointer to a point to store the result
	\param pInput Pointer to the point to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::Transform(Vector2D_t* pOutput,const Vector2D_t *pInput) const
{
	BURGER_ASSERT(pOutput);
	float fX = pInput->x;
	float fY = pInput->y;
	pOutput->x = m_fScaleX * fX + m_fRotateSkew0 * fY + m_fTranslateX;
	pOutput->y = m_fRotateSkew1 * fX + m_fScaleY * fY + m_fTranslateY;
}

/*! ************************************

	\brief Transform a point by this matrix

	\param pOutput Pointer to a point to store the result
	\param fX X coordinate of the point to transform
	\param fY Y coordinate of the point to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::Transform(Vector2D_t* pOutput,float fX,float fY) const
{
	BURGER_ASSERT(pOutput);
	pOutput->x = m_fScaleX * fX + m_fRotateSkew0 * fY + m_fTranslateX;
	pOutput->y = m_fRotateSkew1 * fX + m_fScaleY * fY + m_fTranslateY;
}

/*! ************************************

	\brief Transform a rectangle by this matrix

	\param pBounds Pointer to a rectangle to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::Transform(Vector4D_t* pBounds) const
{
	// Transformed corners
	Vector2D_t Temp[4];

	float fX = pBounds->GetLeft();
	float fY = pBounds->GetTop();
	Transform(&Temp[0],fX,fY);

	fX = pBounds->GetRight();
	//fY = bound->GetTop();
	Transform(&Temp[1],fX,fY);

	//fX = bound->GetRight();
	fY = pBounds->GetBottom();
	Transform(&Temp[2],fX,fY);

	fX = pBounds->GetLeft();
	//fY = bound->GetBottom();
	Transform(&Temp[3],fX,fY);

	// Create the new rectangle
	pBounds->SetRect(&Temp[0]);
	pBounds->ExpandRect(&Temp[1]);
	pBounds->ExpandRect(&Temp[2]);
	pBounds->ExpandRect(&Temp[3]);
}

/*! ************************************

	\brief Transform a point by this matrix

	Apply the transform to a point, however this function
	does not apply the translation.

	\param pOutput Pointer to a point to store the result
	\param pInput Pointer to a point to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::TransformVector(Vector2D_t* pOutput,const Vector2D_t *pInput) const
{
	BURGER_ASSERT(pOutput);
	float fX = pInput->x;
	float fY = pInput->y;
	pOutput->x = m_fScaleX * fX + m_fRotateSkew0 * fY;
	pOutput->y = m_fRotateSkew1 * fX + m_fScaleY * fY;
}

/*! ************************************

	\brief Transform a point by this inverse of this matrix

	Create the inverse of this matrix and then
	apply the transform to a point.

	\param pOutput Pointer to a point to store the result
	\param pInput Pointer to a point to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::TransformByInverse(Vector2D_t* pOutput, const Vector2D_t *pInput) const
{
	// Create the inverse
	Matrix23 Temp;
	Temp.SetInverse(this);
	// Apply the transform
	Temp.Transform(pOutput,pInput);
}

/*! ************************************

	\brief Transform a rectangle by this inverse of this matrix

	Create the inverse of this matrix and then
	apply the transform to a rectangle.

	\param pBounds Pointer to a rectangle to transform

***************************************/

void BURGER_API Burger::Flash::Matrix23::TransformByInverse(Vector4D_t* pBounds) const
{
	// Create the inverse
	Matrix23 Temp;
	Temp.SetInverse(this);
	Temp.Transform(pBounds);
}

/*! ************************************

	\brief Set this matrix to the inverse of the input matrix

	\param pInput Pointer to a matrix to invert.

***************************************/

void BURGER_API BURGER_API Burger::Flash::Matrix23::SetInverse(const Matrix23 *pInput)
{
	BURGER_ASSERT(this != pInput);

	// Invert the rotation part.
	float fDeterminate = (pInput->m_fScaleX * pInput->m_fScaleY) - (pInput->m_fRotateSkew0 * pInput->m_fRotateSkew1);
	if (fDeterminate == 0.0f) {
		// Arbitrary fallback.
		m_fScaleX = 1.0f;
		m_fScaleY = 1.0f;
		m_fRotateSkew0 = 0.0f;
		m_fRotateSkew1 = 0.0f;
		m_fTranslateX = -pInput->m_fTranslateX;
		m_fTranslateY = -pInput->m_fTranslateY;
	} else {
		float fDeterminateInv = 1.0f / fDeterminate;
		m_fScaleX = pInput->m_fScaleY * fDeterminateInv;
		m_fScaleY = pInput->m_fScaleX * fDeterminateInv;
		m_fRotateSkew0 = -pInput->m_fRotateSkew0 * fDeterminateInv;
		m_fRotateSkew1 = -pInput->m_fRotateSkew1 * fDeterminateInv;
		m_fTranslateX = -(m_fScaleX * pInput->m_fTranslateX + m_fRotateSkew0 * pInput->m_fTranslateY);
		m_fTranslateY = -(m_fRotateSkew1 * pInput->m_fTranslateX + m_fScaleY * pInput->m_fTranslateY);
	}
}

/*! ************************************

	\brief Test for left handed matrix

	\return \ref TRUE if the matrix determinate is less than 0.0f

***************************************/

uint_t BURGER_API Burger::Flash::Matrix23::DoesFlip(void) const
{
	float fDeterminate = (m_fScaleX * m_fScaleY) - (m_fRotateSkew0 * m_fRotateSkew1);
	return (fDeterminate < 0.f);
}

/*! ************************************

	\fn float Burger::Flash::Matrix23::GetDeterminant(void) const
	\brief Calculate the matrix determinate

	Using the 2x2 rotation/scale values, return the determinant

	\return The determinant of (m_fScaleX * m_fScaleY) - (m_fRotateSkew1 * m_fRotateSkew0)

***************************************/

/*! ************************************

	\brief Get the maximum scale that this transform contains.

	Test the X and Y scale factors and return the larger of the two.

	\return The largest scale value.

***************************************/

float BURGER_API Burger::Flash::Matrix23::GetMaxScale(void) const
{
	float fBasisX = (m_fScaleX * m_fScaleX) + (m_fRotateSkew0 * m_fRotateSkew0);
	float fBasisY = (m_fScaleY * m_fScaleY) + (m_fRotateSkew1 * m_fRotateSkew1);
	float fLengthSquared = maximum(fBasisX,fBasisY);
	return square_root(fLengthSquared);
}

/*! ************************************

	\brief Get the X scale magnitude

	Get the distance of the scale and skew in the X direction

	\return The X scale magnitude
	\sa GetYScale(void) const 

***************************************/

float BURGER_API Burger::Flash::Matrix23::GetXScale(void) const
{
	float fScale = square_root(m_fScaleX * m_fScaleX + m_fRotateSkew1 * m_fRotateSkew1);
	// Are we turned inside out?
	if (GetDeterminant() < 0.f) {
		fScale = -fScale;		// Flip
	}
	return fScale;
}

/*! ************************************

	\brief Get the Y scale magnitude

	Get the distance of the scale and skew in the Y direction

	\return The Y scale magnitude
	\sa GetXScale(void) const 

***************************************/

float BURGER_API Burger::Flash::Matrix23::GetYScale(void) const
{
	return square_root(m_fScaleY * m_fScaleY + m_fRotateSkew0 * m_fRotateSkew0);
}

/*! ************************************

	\brief Get the matrix rotation value

	Calculate the rotation in radians.

	\return The rotation of the matrix.
	\sa GetXScale(void) const or GetYScale(void) const

***************************************/

float BURGER_API Burger::Flash::Matrix23::GetRotation(void) const
{
	float fScaleX = m_fScaleX;
	if (GetDeterminant() < 0.f) {
		fScaleX = -fScaleX;
	}
	// Calculate the radians
	return get_arctangent2(m_fRotateSkew1,fScaleX);
}

/*! ************************************

	\brief Dump the matrix to Debug::Message()

	Output the matrix values to the console for debugging.
	\sa Debug::Message()

***************************************/

void BURGER_API Burger::Flash::Matrix23::Print(void) const
{
	Debug::Message("| %4.4f %4.4f %4.4f |\n", m_fScaleX, m_fRotateSkew0,TwipsToPixels(m_fTranslateX));
	Debug::Message("| %4.4f %4.4f %4.4f |\n", m_fRotateSkew1, m_fScaleY,TwipsToPixels(m_fTranslateY));
}

/*! ************************************

	\brief Test matrix for equality

	\param rInput Matrix to compare against
	\return \ref TRUE if equal, \ref FALSE if not.
	\sa operator!=(const Matrix23 &) const

***************************************/

uint_t BURGER_API Burger::Flash::Matrix23::operator==(const Matrix23& rInput) const
{
	return (m_fScaleX == rInput.m_fScaleX) &&
		(m_fRotateSkew0 == rInput.m_fRotateSkew0) &&
		(m_fTranslateX == rInput.m_fTranslateX) &&
		(m_fRotateSkew1 == rInput.m_fRotateSkew1) &&
		(m_fScaleY == rInput.m_fScaleY) &&
		(m_fTranslateY == rInput.m_fTranslateY);
}

/*! ************************************

	\brief Test matrix for inequality

	\param rInput Matrix to compare against
	\return \ref TRUE if not equal, \ref FALSE if equal.
	\sa operator==(const Matrix23 &) const

***************************************/

uint_t BURGER_API Burger::Flash::Matrix23::operator!=(const Matrix23& rInput) const
{
	return (m_fScaleX != rInput.m_fScaleX) ||
		(m_fRotateSkew0 != rInput.m_fRotateSkew0) ||
		(m_fTranslateX != rInput.m_fTranslateX) ||
		(m_fRotateSkew1 != rInput.m_fRotateSkew1) ||
		(m_fScaleY != rInput.m_fScaleY) ||
		(m_fTranslateY != rInput.m_fTranslateY);
}
