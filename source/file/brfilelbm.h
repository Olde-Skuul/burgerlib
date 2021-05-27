/***************************************

    LBM File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	static const char * BURGER_API UnpackILBMData(uint8_t *pOutput,uintptr_t uOutputLength,InputMemoryStream *pInput);
	static const char * BURGER_API UnpackILBM(uint8_t *pOutput,uint_t uWidth,uint_t uHeight,uint_t uDepth,InputMemoryStream *pInput);
	static const char * BURGER_API SeekIffChunk(InputMemoryStream *pInput,uint32_t uID,uintptr_t uStartOffset);
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const uint32_t FILETYPE = 0x494C424D;		///< 'ILBM' Mac File type for LBM files (Byte swapped on little endian machines)
	static const uint32_t AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for LBM files (Byte swapped on little endian machines)
#else
	static const uint32_t FILETYPE = 0x4D424C49;		///< 'ILBM'
	static const uint32_t AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileLBM();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
extern const void * BURGER_API FindAIFFChunk(const void *pInput,uintptr_t uLength,uint32_t uChunkName);
extern const void * BURGER_API FindRIFFChunk(const void *pInput,uintptr_t uLength,uint32_t uChunkName);
}
/* END */

#endif
