/***************************************

    BMP File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	uint16_t m_uReserved1;			///< BMP Header reserved field #1
	uint16_t m_uReserved2;			///< BMP Header reserved field #2
	int32_t m_iXPixelsPerMeter;		///< X Pixels per meter
	int32_t m_iYPixelsPerMeter;		///< Y Pixels per meter
	uint32_t m_uColorsUsed;			///< Number of colors used
	uint32_t m_uColorsImportant;		///< Number of important colors
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
public:
	static void BURGER_API CompressRLE8(OutputMemoryStream *pOutput,const uint8_t *pInput,uintptr_t uInputLength,uint_t bSendEOP);
	static const char * BURGER_API DecompressRLE8(uint8_t *pOutput,uintptr_t uOutputLength,InputMemoryStream *pInput);
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
	static const uint32_t FILETYPE = 0x424D5070;		///< 'BMPp' Mac File type for BMP files (Byte swapped on little endian machines)
	static const uint32_t AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for BMP files (Byte swapped on little endian machines)
#else
	static const uint32_t FILETYPE = 0x70504D42;		///< 'BMPp'
	static const uint32_t AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	static const int32_t DEFAULT_PIXELS_PER_METER = 2835;	///< Default pixels per meter value for BMP files (2834.6472f)
	FileBMP();
	uint_t BURGER_API Load(Image *pOutput,InputMemoryStream *pInput);
	uint_t Save(OutputMemoryStream *pOutput,const Image *pImage,uint_t bCompression=FALSE);
	BURGER_INLINE uint16_t GetReserved1(void) const { return m_uReserved1;}
	BURGER_INLINE void SetReserved1(uint16_t uReserved1) { m_uReserved1 = uReserved1;}
	BURGER_INLINE uint16_t GetReserved2(void) const { return m_uReserved2;}
	BURGER_INLINE void SetReserved2(uint16_t uReserved2) { m_uReserved2 = uReserved2;}
	BURGER_INLINE int32_t GetXPixelsPerMeter(void) const { return m_iXPixelsPerMeter;}
	BURGER_INLINE void SetXPixelsPerMeter(int32_t iXPixelsPerMeter) { m_iXPixelsPerMeter = iXPixelsPerMeter;}
	BURGER_INLINE int32_t GetYPixelsPerMeter(void) const { return m_iYPixelsPerMeter;}
	BURGER_INLINE void SetYPixelsPerMeter(int32_t iYPixelsPerMeter) { m_iYPixelsPerMeter = iYPixelsPerMeter;}
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
