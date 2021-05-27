/***************************************

    PCX File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	uint16_t m_uXPixelsPerInch;		///< X Pixels per inch (72)
	uint16_t m_uYPixelsPerInch;		///< Y Pixels per inch (72)
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
	uint8_t m_EGAPalette[48];			///< EGA palette
public:
	static const char * BURGER_API DecompressPCX(uint8_t *pOutput,uintptr_t uOutputLength,InputMemoryStream *pInput);
	static void BURGER_API Merge3Planes(uint8_t *pOutput,const uint8_t *pInput,uintptr_t uWidth);
	enum {
		PCX_VERSION25=0,	///< File version 2.5
		PCX_VERSION28=2,	///< File version 2.8
		PCX_VERSION28COLOR=3,	///< File version 2.8 with true color support
		PCX_VERSION30=5		///< File version 3.0
	};
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const uint32_t FILETYPE = 0x50435878;		///< 'PCXx' Mac File type for PCX files (Byte swapped on little endian machines)
	static const uint32_t AUXTYPE = 0x474B4F4E;		///< 'GKON' Mac creator code for PCX files (Byte swapped on little endian machines)
#else
	static const uint32_t FILETYPE = 0x78584350;		///< 'PCXx'
	static const uint32_t AUXTYPE = 0x4E4F4B47;		///< 'GKON'
#endif
	static const uint16_t DEFAULT_PIXELS_PER_INCH = 72;	///< Default pixels per inch value for PCX files
	FilePCX();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE int32_t GetXPixelsPerInch(void) const { return m_uXPixelsPerInch;}
	BURGER_INLINE void SetXPixelsPerInch(uint16_t uXPixelsPerInch) { m_uXPixelsPerInch = uXPixelsPerInch;}
	BURGER_INLINE int32_t GetYPixelsPerInch(void) const { return m_uYPixelsPerInch;}
	BURGER_INLINE void SetYPixelsPerInch(uint16_t uYPixelsPerInch) { m_uYPixelsPerInch = uYPixelsPerInch;}
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE const uint8_t *GetEGAPalette(void) const { return m_EGAPalette; }
	BURGER_INLINE uint8_t *GetEGAPalette(void) { return m_EGAPalette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
