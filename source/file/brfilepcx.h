/***************************************

	PCX File handler class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEPCX_H__
#define __BRFILEPCX_H__

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
class FilePCX {
	Word16 m_uXPixelsPerInch;		///< X Pixels per inch (72)
	Word16 m_uYPixelsPerInch;		///< Y Pixels per inch (72)
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
	Word8 m_EGAPalette[48];			///< EGA palette
public:
	static const char * BURGER_API DecompressPCX(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	static void BURGER_API Merge3Planes(Word8 *pOutput,const Word8 *pInput,WordPtr uWidth);
	enum {
		PCX_VERSION25=0,	///< File version 2.5
		PCX_VERSION28=2,	///< File version 2.8
		PCX_VERSION28COLOR=3,	///< File version 2.8 with true color support
		PCX_VERSION30=5		///< File version 3.0
	};
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x50435878;		///< 'PCXx' Mac File type for PCX files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x474B4F4E;		///< 'GKON' Mac creator code for PCX files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x78584350;		///< 'PCXx'
	static const Word32 AUXTYPE = 0x4E4F4B47;		///< 'GKON'
#endif
	static const Word16 DEFAULT_PIXELS_PER_INCH = 72;	///< Default pixels per inch value for PCX files
	FilePCX();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE Int32 GetXPixelsPerInch(void) const { return m_uXPixelsPerInch;}
	BURGER_INLINE void SetXPixelsPerInch(Word16 uXPixelsPerInch) { m_uXPixelsPerInch = uXPixelsPerInch;}
	BURGER_INLINE Int32 GetYPixelsPerInch(void) const { return m_uYPixelsPerInch;}
	BURGER_INLINE void SetYPixelsPerInch(Word16 uYPixelsPerInch) { m_uYPixelsPerInch = uYPixelsPerInch;}
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE const Word8 *GetEGAPalette(void) const { return m_EGAPalette; }
	BURGER_INLINE Word8 *GetEGAPalette(void) { return m_EGAPalette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
