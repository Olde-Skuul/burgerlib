/***************************************

	PNG File handler class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEPNG_H__
#define __BRFILEPNG_H__

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
class FilePNG {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
	WordPtr m_uStartOffset;			///< PNG file image chunk start offset
	WordPtr m_uNextOffset;			///< Offset to the next chunk
	Word32 m_uChunkSize;			///< Size of the current chunk in bytes
	//Word32 m_uChunkCRC;			///< CRC of the current chunk
	Word32 m_uPNGID;				///< Untouched ID of the current PNG chunk
protected:
	const char * BURGER_API SeekChunk(InputMemoryStream *pInput,Word32 uID,WordPtr uStartOffset);
public:
	enum {
		PNG_GREYSCALE=0,		///< Each pixel is a grayscale sample.
		PNG_RGB=2,				///< Each pixel is an R,G,B triple.
		PNG_INDEXED=3,			///< Each pixel is a palette index;
		PNG_GREYSCALEALPHA=4,	///< Each pixel is a grayscale sample,
		PNG_RGBA=6				///< Each pixel is an R,G,B,A quad,
	};
	const char * BURGER_API SeekPNGChunk(InputMemoryStream *pInput,Word32 uID);
	const char * BURGER_API SeekNextPNGChunk(InputMemoryStream *pInput,Word32 uID);
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x504E4720;		///< 'PNG ' Mac File type for PNG files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for PNG files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x20474E50;		///< 'PNG '
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FilePNG();
	Word BURGER_API Load(Image *pOutput,InputMemoryStream *pInput);
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
