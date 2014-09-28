/***************************************

	Flash player utility functions
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHUTILS_H__
#define __BRFLASHUTILS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
BURGER_INLINE float TwipsToPixels(float fInput) { return fInput * (1.0f / 20.f); }
BURGER_INLINE float PixelsToTwips(float fInput) { return fInput * 20.f; }
extern void BURGER_API TwipsToPixels(Vector2D_t *pInput);
extern void BURGER_API PixelsToTwips(Vector2D_t *pInput);
extern Word32 BURGER_API TextureSizePower2(Word32 uInput);
}}
/* END */

#endif
