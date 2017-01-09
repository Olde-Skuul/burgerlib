/***************************************

	BMP File handler class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEBMP_H__
#define __BRFILEBMP_H__

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
class FileBMP {
	Word16 m_uReserved1;			///< BMP Header reserved field #1
	Word16 m_uReserved2;			///< BMP Header reserved field #2
	Int32 m_iXPixelsPerMeter;		///< X Pixels per meter
	Int32 m_iYPixelsPerMeter;		///< Y Pixels per meter
	Word32 m_uColorsUsed;			///< Number of colors used
	Word32 m_uColorsImportant;		///< Number of important colors
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
	static void BURGER_API CompressRLE8(OutputMemoryStream *pOutput,const Word8 *pInput,WordPtr uInputLength,Word bSendEOP);
	static const char * BURGER_API DecompressRLE8(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
	enum {
		BMP_RGB=0,			///< Raw pixels
		BMP_RLE8=1,			///< 8 bit compression
		BMP_RLE4=2,			///< 4 bit compression
		BMP_BITFIELDS=3,	///< RGB bit field masks
		BMP_JPEG=4,			///< JPEG compression
		BMP_PNG=5,			///< PNG compression
		BMP_ALPHABITFIELDS=6,	///< RGBA bit field masks
		BMP_CMYK=11,		///< CMYK pixels
		BMP_CMYKRLE8=12,	///< 8 bit compression using a CMYK palette
		BMP_CMYKTLE4=13		///< 4 bit compression using a CMYK palette
	};
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x424D5070;		///< 'BMPp' Mac File type for BMP files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for BMP files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x70504D42;		///< 'BMPp'
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	static const Int32 DEFAULT_PIXELS_PER_METER = 2835;	///< Default pixels per meter value for BMP files (2834.6472f)
	FileBMP();
	Word BURGER_API Load(Image *pOutput,InputMemoryStream *pInput);
	Word Save(OutputMemoryStream *pOutput,const Image *pImage,Word bCompression=FALSE);
	BURGER_INLINE Word16 GetReserved1(void) const { return m_uReserved1;}
	BURGER_INLINE void SetReserved1(Word16 uReserved1) { m_uReserved1 = uReserved1;}
	BURGER_INLINE Word16 GetReserved2(void) const { return m_uReserved2;}
	BURGER_INLINE void SetReserved2(Word16 uReserved2) { m_uReserved2 = uReserved2;}
	BURGER_INLINE Int32 GetXPixelsPerMeter(void) const { return m_iXPixelsPerMeter;}
	BURGER_INLINE void SetXPixelsPerMeter(Int32 iXPixelsPerMeter) { m_iXPixelsPerMeter = iXPixelsPerMeter;}
	BURGER_INLINE Int32 GetYPixelsPerMeter(void) const { return m_iYPixelsPerMeter;}
	BURGER_INLINE void SetYPixelsPerMeter(Int32 iYPixelsPerMeter) { m_iYPixelsPerMeter = iYPixelsPerMeter;}
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
