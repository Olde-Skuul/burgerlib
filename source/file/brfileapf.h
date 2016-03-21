/***************************************

	Apple Preferred File handler class (Apple IIgs)

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEAPF_H__
#define __BRFILEAPF_H__

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
class FileAPF {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x41504620;		///< 'APF ' Mac File type for APF files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for APF files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x20465041;		///< 'APF '
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileAPF();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
