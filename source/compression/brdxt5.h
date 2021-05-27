/***************************************

    DXT5 compressor and decompressor

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDXT5_H__
#define __BRDXT5_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger {
struct Dxt5Packet_t {
	uint8_t m_uAlpha1;				///< First alpha endpoint in 8 bit intensity
	uint8_t m_uAlpha2;				///< Second alpha endpoint in 8 bit intensity
	uint8_t m_uAlphaIndexes[2][3];	///< Two 24 bit little endian values that contain 3 bits per pixel indexes for the 4x4 tile
	uint16_t m_uRGB565Color1;			///< First color endpoint in R5:G6:B5 little endian format
	uint16_t m_uRGB565Color2;			///< Second color endpoint in R5:G6:B5 little endian format
	uint8_t m_uColorIndexes[4];		///< 2 bits per pixel color indexes for 4x4 tile
	void Decompress(RGBAWord8_t *pOutput,uintptr_t uStride = sizeof(RGBAWord8_t)*4) const;
	void Compress(const RGBAWord8_t *pInput,uintptr_t uStride = sizeof(RGBAWord8_t)*4);
};
extern void BURGER_API DecompressImage(RGBAWord8_t *pOutput,uintptr_t uOutputStride,uint_t uWidth,uint_t uHeight,const Dxt5Packet_t *pInput,uintptr_t uInputStride);
}
/* END */

#endif
