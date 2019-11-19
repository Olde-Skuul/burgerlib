/***************************************

	8 bit software manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRENDERERSOFTWARE8_H__
#define __BRRENDERERSOFTWARE8_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRENDERER_H__
#include "brrenderer.h"
#endif

/* BEGIN */
namespace Burger {
class RendererSoftware8 : public Renderer {
public:
	RendererSoftware8(void);
	virtual void Draw8BitPixels(int iX,int iY,Word uWidth,Word uHeight,WordPtr uStride,const Word8 *pPixels);
	virtual void Draw8BitPixelsMasked(int iX,int iY,Word uWidth,Word uHeight,WordPtr uStride,const Word8 *pPixels);
	virtual void DrawPixel(int iX,int iY,Word uColorIndex);
	virtual void DrawRect(int iX,int iY,Word uWidth,Word uHeight,Word uColorIndex);
};
}
/* END */

#endif
