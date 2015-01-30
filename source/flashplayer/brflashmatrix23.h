/***************************************

	Flash player 2x3 matrix manager
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHMATRIX23_H__
#define __BRFLASHMATRIX23_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLASHSTREAM_H__
#include "brflashstream.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class Matrix23 {
public:
	float m_fScaleX;			///< XScale value
	float m_fRotateSkew0;		///< X rotation skew
	float m_fRotateSkew1;		///< Y rotation skew
	float m_fScaleY;			///< YScale value
	float m_fTranslateX;		///< X translation
	float m_fTranslateY;		///< Y translation

	Matrix23();
	void BURGER_API Identity(void);
	void BURGER_API Concatenate(const Matrix23 *pInput);
	void BURGER_API ConcatenateTranslation(float fX,float fY);
	void BURGER_API ConcatenateScale(float fScale);
	void BURGER_API Interpolate(const Matrix23 *pInput1,const Matrix23 *pInput2,float fFactor);
	void BURGER_API SetScaleRotation(float fXScale,float fYScale,float fRotation);
	void BURGER_API Read(Stream* pStream);
	void BURGER_API Transform(Vector2D_t* pOutput,const Vector2D_t *pInput) const;
	void BURGER_API Transform(Vector2D_t* pOutput,float fX,float fY) const;
	void BURGER_API Transform(Vector4D_t* pBounds) const;
	void BURGER_API TransformVector(Vector2D_t* pOutput,const Vector2D_t *pInput) const;
	void BURGER_API TransformByInverse(Vector2D_t* pOutput,const Vector2D_t *pInput) const;
	void BURGER_API TransformByInverse(Vector4D_t* pBounds) const;
	void BURGER_API SetInverse(const Matrix23 *pInput);
	Word BURGER_API DoesFlip(void) const;
	BURGER_INLINE float GetDeterminant(void) const { return (m_fScaleX * m_fScaleY) - (m_fRotateSkew1 * m_fRotateSkew0); }
	float BURGER_API GetMaxScale(void) const;
	float BURGER_API GetXScale(void) const;
	float BURGER_API GetYScale(void) const;
	float BURGER_API GetRotation(void) const;
	void BURGER_API Print(void) const;
	Word BURGER_API operator==(const Matrix23& rInput) const;
	Word BURGER_API operator!=(const Matrix23& rInput) const;
};
}}
/* END */

#endif
