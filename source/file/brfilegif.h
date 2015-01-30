/***************************************

	GIF File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEGIF_H__
#define __BRFILEGIF_H__

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
class FileGIF {
	Word8 m_bAspectRatio;			///< GIF aspect ratio
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
	struct GIFDecoder {
		InputMemoryStream *m_pInput;	///< Pointer to packed data
		Word m_uBitBucket;				///< Current bit bucket
		Word m_uBitCount;				///< Current valid bit count in bucket
		Word m_uChunkCount;				///< Number of bytes in stream
		Word m_uLZWCodeSize;			///< Starting data size
		const Word8 *m_LZWTable[4096];	///< Dictionary pointers
		Word8 m_LZWArray[256]; 			///< 0-255 for default data
		Word m_LZWSize[4096]; 			///< Size of each entry
		void Init(void);
		void Reset(void);
		Word GetCode(Word uBitsRequested);
	};
	static void BURGER_API DiscardExtension(InputMemoryStream *pInput);
	static const char *BURGER_API UnpackGIFLZW(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x47494666;		///< 'GIFf' Mac File type for GIF files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for GIF files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x66464947;		///< 'GIFf'
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileGIF();
	Image *Load(InputMemoryStream *pInput);
	BURGER_INLINE Word GetAspectRatio(void) const { return m_bAspectRatio;}
	BURGER_INLINE void SetAspectRatio(Word bAspectRatio) { m_bAspectRatio = static_cast<Word8>(bAspectRatio);}
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	void SetPalette(const RGBWord8_t *pPalette,Word uStartIndex=0,Word uPaletteSize=256);
	void SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex=0,Word uPaletteSize=256);
};
}
/* END */

#endif
