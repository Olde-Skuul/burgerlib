/***************************************

    Flash player rectangle manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashrect.h"
#include "brdebug.h"
#include "brflashutils.h"

/*! ************************************

	\class Burger::Flash::Rect
	\brief Rectangle specialization for Adobe Flash

	This class extends a Vector4D_t to give the specializations for
	Adobe Flash

	\sa Vector4D_t

***************************************/


/*! ************************************

	\fn Burger::Flash::Rect::Rect()
	\brief Default constructor

	Initialize the rectangle to 0,0,0,0

	\sa Rect

***************************************/

/*! ************************************

	\brief Load a rectangle from the byte stream

	Using Adobe Flash protocol, read in a rectangle

	Consult page 22 of the file swf-file-format-spec.pdf

	\param pStream Pointer to a Flash input stream

***************************************/

void BURGER_API Burger::Flash::Rect::Read(Stream* pStream)
{
	pStream->ByteAlign();
	uint_t uCount = pStream->GetWord(5);
	x = static_cast<float>(pStream->GetInt(uCount));	// XMin
	z = static_cast<float>(pStream->GetInt(uCount));	// XMax
	y = static_cast<float>(pStream->GetInt(uCount));	// YMIn
	w = static_cast<float>(pStream->GetInt(uCount));	// YMax
}

/*! ************************************

	\brief Test for rectangle collision

	If the passed rectangle intersects with this rectangle,
	return \ref TRUE. This differs from BurgerLib because
	the test does not return TRUE if the rectangle is
	touching on the bottom or right.

	\param pRect Pointer to a rectangle to test against
	\return \ref TRUE if the rectangles are 

***************************************/

uint_t BURGER_API Burger::Flash::Rect::IsIntersecting(const Vector4D_t *pRect) const
{
	if ((z < pRect->x) ||	// right < left
		(x > pRect->z) ||	// left > right
		(y > pRect->w) ||	// top > bottom
		(w < pRect->y)) {	// bottom < top
		return FALSE;
	}
	return TRUE;
}

/*! ************************************

	\brief Set a rectangle bounds to a transformed rectangle

	Apply a transformation to the input rectangle and store
	the result in this rectangle.

	\note This function allows the input rect to be the same as the output

	\param pMatrix Pointer to a matrix to transform the input rectangle
	\param pRect Pointer to a rectangle to transform

***************************************/

void BURGER_API Burger::Flash::Rect::SetRect(const Flash::Matrix23 *pMatrix,const Vector4D_t *pRect)
{
	// Transform the points
	float fX = pRect->GetLeft();
	float fY = pRect->GetTop();
	Vector2D_t p0;
	pMatrix->Transform(&p0,fX,fY);

	fX = pRect->GetRight();
	Vector2D_t p1;
	pMatrix->Transform(&p1,fX,fY);

	fY = pRect->GetBottom();
	Vector2D_t p2;
	pMatrix->Transform(&p2,fX,fY);

	fX = pRect->GetLeft();
	Vector2D_t p3;
	pMatrix->Transform(&p3,fX,fY);

	// Create the new rectangle
	Vector4D_t::SetRect(&p0);
	ExpandRect(&p1);
	ExpandRect(&p2);
	ExpandRect(&p3);
}

/*! ************************************
	
	\brief Convert the rectangle from Flash Twips to Pixels

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from twips to pixels by a
	multiplication of 1.0f/20.0f

	\sa PixelsToTwips(void)

***************************************/

void BURGER_API Burger::Flash::Rect::TwipsToPixels(void)
{
	x = Flash::TwipsToPixels(x);
	y = Flash::TwipsToPixels(y);
	z = Flash::TwipsToPixels(z);
	w = Flash::TwipsToPixels(w);
}

/*! ************************************

	\brief Convert the rectangle from Pixels to Flash Twips

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from pixels to twips by a
	multiplication of 20.0f
	
	\sa TwipsToPixels(void)

***************************************/

void BURGER_API Burger::Flash::Rect::PixelsToTwips(void)
{
	x = Flash::PixelsToTwips(x);
	y = Flash::PixelsToTwips(y);
	z = Flash::PixelsToTwips(z);
	w = Flash::PixelsToTwips(w);
}

/*! ************************************

	\brief Dump the rectangle to Debug::Message()

	Output the rectangle values to the console for debugging.
	\sa Debug::Message()

***************************************/

void BURGER_API Burger::Flash::Rect::Print(void) const
{
	Debug::Message("fLeft = %g, fTop = %g, fRight = %g, fBottom = %g\n",
		Flash::TwipsToPixels(x),Flash::TwipsToPixels(y),
		Flash::TwipsToPixels(z),Flash::TwipsToPixels(w));
}
