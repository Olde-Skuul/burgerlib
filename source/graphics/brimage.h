/***************************************

    Graphic file format transport class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIMAGE_H__
#define __BRIMAGE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

/* BEGIN */
namespace Burger {
class Image {
    BURGER_DISABLE_COPY(Image);
public:
	enum eFlags {
		FLAGS_IMAGENOTALLOCATED=0x0020,	///< \ref TRUE if the image pointer was not allocated (And won't be released when Image goes out of scope)
		FLAGS_MIPMAPMASK=0x001F			///< Mask for the number of mip maps present (0 means only one level, max 16)
	};
	enum ePixelTypes {
		PIXELTYPE1BIT=1,			///< 1 bit per pixel monochrome
		PIXELTYPE2BIT=2,			///< 2 bits per pixel indexed by RGB palette
		PIXELTYPE4BIT=4,			///< 4 bits per pixel indexed by RGB palette
		PIXELTYPE332=7,				///< 8 bits per pixel R:3, G:3, B:2
		PIXELTYPE8BIT=8,			///< 8 bits per pixel indexed RGB palette (Or greyscale)
		PIXELTYPE4444=13,			///< 16 bits per pixel native endian A:4, R:4, G:4, B:4
		PIXELTYPE1555=14,			///< 16 bits per pixel native endian A:1, R:5, G:5, B:5
		PIXELTYPE555=15,			///< 16 bits per pixel native endian R:5, G:5, B:5
		PIXELTYPE565=16,			///< 16 bits per pixel native endian R:5, G:6, B:5
		PIXELTYPEDXT1=17,			///< Compressed with DXT1
		PIXELTYPEDXT2=18,			///< Compressed with DXT2
		PIXELTYPEDXT3=19,			///< Compressed with DXT3
		PIXELTYPEDXT4=20,			///< Compressed with DXT4
		PIXELTYPEDXT5=21,			///< Compressed with DXT5
		PIXELTYPE888=24,			///< 24 bits per pixel in bytes of R:8, G:8 and B:8
		PIXELTYPE1010102=31,		///< 32 bits per pixel in bytes of R:10, G:10, B:10 and A:2 in that order
		PIXELTYPE8888=32			///< 32 bits per pixel in bytes of R:8, G:8, B:8 and A:8 in that order
	};
protected:
	uint8_t *m_pImage;		///< Pointer to pixel array
	uint_t m_uStride;			///< Number of bytes per scan line (Likely not the same value as m_uWidth)
	uint_t m_uWidth;			///< Width of image in pixels
	uint_t m_uHeight;			///< Height of image in pixels
	uint_t m_uFlags;			///< Internal behavior flags
	ePixelTypes m_eType;	///< Type of pixel image
public:
	Image();
	~Image();
	static Image * BURGER_API new_object(void);
	static Image * BURGER_API new_object(uint_t uWidth,uint_t uHeight,ePixelTypes eType);
	static Image * BURGER_API new_object(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMaps);
	static Image * BURGER_API new_object(const Image *pInput);
	void Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uintptr_t uStride,const uint8_t *pImage,uint_t uFlags=0,uint_t uMipMaps=0);
	uint_t Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType);
	uint_t Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMaps);
	uint_t Init(const Image *pInput);
	uint_t InitPNG(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	uint_t InitPNG(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitPNG(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitPNG(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette=NULL);
	uint_t InitGIF(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	uint_t InitGIF(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitGIF(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitGIF(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette=NULL);
	uint_t InitTGA(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	uint_t InitTGA(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitTGA(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitTGA(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette=NULL);
	uint_t InitBMP(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	uint_t InitBMP(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitBMP(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	uint_t InitBMP(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette=NULL);
	void Transfer(Image *pInput);
	void Shutdown(void);
	BURGER_INLINE const uint8_t *GetImage(void) const { return m_pImage; }
	BURGER_INLINE uint8_t *GetImage(void) { return m_pImage; }
	const uint8_t *GetImage(uint_t uMipMap) const;
	BURGER_INLINE uint_t GetWidth(void) const { return m_uWidth; }
	uint_t GetWidth(uint_t uMipMap) const;
	BURGER_INLINE uint_t GetHeight(void) const { return m_uHeight; }
	uint_t GetHeight(uint_t uMipMap) const;
	BURGER_INLINE uintptr_t GetStride(void) const { return m_uStride; }
	uintptr_t GetStride(uint_t uMipMap) const;
	BURGER_INLINE ePixelTypes GetType(void) const { return m_eType; }
	uint_t GetMipMapCount(void) const;
	static uint_t BURGER_API GetBytesPerPixel(ePixelTypes eType);
	BURGER_INLINE uint_t GetBytesPerPixel(void) const { return GetBytesPerPixel(m_eType); }
	static uint_t BURGER_API GetBitsPerPixel(ePixelTypes eType);
	BURGER_INLINE uint_t GetBitsPerPixel(void) const { return GetBitsPerPixel(m_eType); }
	static uintptr_t BURGER_API GetSuggestedStride(uint_t uWidth,ePixelTypes eType);
	uintptr_t GetSuggestedStride(void) const;
	static uintptr_t BURGER_API GetSuggestedBufferSize(uint_t uWidth,uint_t uHeight,ePixelTypes eType);
	static uintptr_t BURGER_API GetSuggestedBufferSize(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMapLevels);
	uintptr_t GetSuggestedBufferSize(void) const;
	uintptr_t GetImageSize(uint_t uMipMap) const;
	const char *ValidateForSave(void) const;
	void ClearBitmap(void);
	uint_t VerticleFlip(void);
	uint_t HorizontalFlip(void);
	uint_t Store8888(const Image *pInput,const RGBAWord8_t *pPalette);
};
}
/* END */
#endif
