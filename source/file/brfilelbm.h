/***************************************

	LBM File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILELBM_H__
#define __BRFILELBM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRIMAGE_H__
#include "brimage.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger {
class FileLBM {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
	static const char * BURGER_API UnpackILBMData(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	static const char * BURGER_API UnpackILBM(Word8 *pOutput,Word uWidth,Word uHeight,Word uDepth,InputMemoryStream *pInput);
	static const char * BURGER_API SeekIffChunk(InputMemoryStream *pInput,Word32 uID,WordPtr uStartOffset);
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x494C424D;		///< 'ILBM' Mac File type for LBM files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for LBM files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x4D424C49;		///< 'ILBM'
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileLBM();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	void SetPalette(const RGBWord8_t *pPalette,Word uStartIndex=0,Word uPaletteSize=256);
	void SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex=0,Word uPaletteSize=256);
};
}
/* END */

#endif
