/***************************************

	Flash player utility functions
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashutils.h"
#include "brstringfunctions.h"

/*! ************************************

	\fn float Burger::Flash::TwipsToPixels(float fInput)
	\brief Convert a value from Flash Twips to Pixels

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from twips to pixels by a
	multiplication of 1.0f/20.0f
	
	\param fInput Value in Flash twips
	\return Value converted to pixels
	\sa TwipsToPixels(Vector2D_t *) or PixelsToTwips(float)

***************************************/

/*! ************************************

	\fn float Burger::Flash::PixelsToTwips(float fInput)
	\brief Convert a value from Pixels to Flash Twips

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from pixels to twips by a
	multiplication of 20.0f
	
	\param fInput Value in pixels
	\return Value converted to Flash twips
	\sa PixelsToTwips(Vector2D_t *) or TwipsToPixels(float)

***************************************/

/*! ************************************

	\brief Convert a 2D point from Flash Twips to Pixels

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from twips to pixels by a
	multiplication of 1.0f/20.0f
	
	\param pInput Pointer to a Vector2D_t to convert
	\sa TwipsToPixels(float) or PixelsToTwips(Vector2D_t *)

***************************************/

void BURGER_API Burger::Flash::TwipsToPixels(Vector2D_t *pInput)
{
	pInput->x = TwipsToPixels(pInput->x);
	pInput->y = TwipsToPixels(pInput->y);
}

/*! ************************************

	\brief Convert a 2D point from Pixels to Flash Twips

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from pixels to twips by a
	multiplication of 20.0f
	
	\param pInput Pointer to a Vector2D_t to convert
	\sa PixelsToTwips(float) or TwipsToPixels(Vector2D_t *)

***************************************/

void BURGER_API Burger::Flash::PixelsToTwips(Vector2D_t *pInput)
{
	pInput->x = PixelsToTwips(pInput->x);
	pInput->y = PixelsToTwips(pInput->y);

}

/*! ************************************

	\brief Convert a texture width/height to a power of 2

	Textures may need to be scaled to a power of two, this function
	will take a coordinate and scale it up to the nearest power of 2.

	In the case where the scaling up would result in an excess of
	stretched pixels (60% stretching), it will be scaled to the next lower
	power of two.
	
	\param uInput Value to convert
	\sa PowerOf2(Word32)

***************************************/

Word32 BURGER_API Burger::Flash::TextureSizePower2(Word32 uInput)
{
	// Convert to the next power of 2
	Word32 uResult = PowerOf2(uInput);
	// If the ratio of uResult/uInput is greater than 3/5, scale down one shift
	if ((uInput*5U) < (uResult*3U)) {
		uResult>>=1;
	}
	return uResult;
}
