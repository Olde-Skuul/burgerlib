/***************************************

	TGA File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILETGA_H__
#define __BRFILETGA_H__

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
class FileTGA {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
	static void BURGER_API UnpackPixel8(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	static void BURGER_API UnpackPixel16(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	static void BURGER_API UnpackPixel24(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	static void BURGER_API UnpackPixel32(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	enum {
		TGA_NOIMAGE=0,		///< Palette only
		TGA_RGBINDEXED=1,	///< 8 bit uncompressed
		TGA_RGB=2,			///< RGB uncompressed
		TGA_GRAY=3,			///< Grayscale uncompressed
		TGA_RLEINDEXED=9,	///< 8 bit compressed
		TGA_RLERGB=10,		///< RGB compressed
		TGA_RLEGRAY=11		///< Grayscale compressed
	};
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x54504943;		///< 'TPIC' Mac File type for TGA files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x474B4F4E;		///< 'GKON' Mac creator code for TGA files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x43495054;		///< 'TPIC'
	static const Word32 AUXTYPE = 0x4E4F4B47;		///< 'GKON'
#endif
	Word BURGER_API Load(Image *pOutput,InputMemoryStream *pInput);
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
