/***************************************

	Graphic file format transport class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	BURGER_DISABLECOPYCONSTRUCTORS(Image);
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
	Word8 *m_pImage;		///< Pointer to pixel array
	Word m_uStride;			///< Number of bytes per scan line (Likely not the same value as m_uWidth)
	Word m_uWidth;			///< Width of image in pixels
	Word m_uHeight;			///< Height of image in pixels
	Word m_uFlags;			///< Internal behavior flags
	ePixelTypes m_eType;	///< Type of pixel image
public:
	Image();
	~Image();
	static Image * BURGER_API New(void);
	static Image * BURGER_API New(Word uWidth,Word uHeight,ePixelTypes eType);
	static Image * BURGER_API New(Word uWidth,Word uHeight,ePixelTypes eType,Word uMipMaps);
	static Image * BURGER_API New(const Image *pInput);
	void Init(Word uWidth,Word uHeight,ePixelTypes eType,WordPtr uStride,const Word8 *pImage,Word uFlags=0,Word uMipMaps=0);
	Word Init(Word uWidth,Word uHeight,ePixelTypes eType);
	Word Init(Word uWidth,Word uHeight,ePixelTypes eType,Word uMipMaps);
	Word Init(const Image *pInput);
	Word InitPNG(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	Word InitPNG(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitPNG(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitPNG(RezFile *pRezFile,Word uRezNum,RGBAWord8_t *pPalette=NULL);
	Word InitGIF(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	Word InitGIF(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitGIF(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitGIF(RezFile *pRezFile,Word uRezNum,RGBAWord8_t *pPalette=NULL);
	Word InitTGA(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	Word InitTGA(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitTGA(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitTGA(RezFile *pRezFile,Word uRezNum,RGBAWord8_t *pPalette=NULL);
	Word InitBMP(InputMemoryStream *pInput,RGBAWord8_t *pPalette=NULL);
	Word InitBMP(const char *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitBMP(Filename *pFilename,RGBAWord8_t *pPalette=NULL);
	Word InitBMP(RezFile *pRezFile,Word uRezNum,RGBAWord8_t *pPalette=NULL);
	void Transfer(Image *pInput);
	void Shutdown(void);
	BURGER_INLINE const Word8 *GetImage(void) const { return m_pImage; }
	BURGER_INLINE Word8 *GetImage(void) { return m_pImage; }
	const Word8 *GetImage(Word uMipMap) const;
	BURGER_INLINE Word GetWidth(void) const { return m_uWidth; }
	Word GetWidth(Word uMipMap) const;
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	Word GetHeight(Word uMipMap) const;
	BURGER_INLINE WordPtr GetStride(void) const { return m_uStride; }
	WordPtr GetStride(Word uMipMap) const;
	BURGER_INLINE ePixelTypes GetType(void) const { return m_eType; }
	Word GetMipMapCount(void) const;
	static Word BURGER_API GetBytesPerPixel(ePixelTypes eType);
	BURGER_INLINE Word GetBytesPerPixel(void) const { return GetBytesPerPixel(m_eType); }
	static Word BURGER_API GetBitsPerPixel(ePixelTypes eType);
	BURGER_INLINE Word GetBitsPerPixel(void) const { return GetBitsPerPixel(m_eType); }
	static WordPtr BURGER_API GetSuggestedStride(Word uWidth,ePixelTypes eType);
	WordPtr GetSuggestedStride(void) const;
	static WordPtr BURGER_API GetSuggestedBufferSize(Word uWidth,Word uHeight,ePixelTypes eType);
	static WordPtr BURGER_API GetSuggestedBufferSize(Word uWidth,Word uHeight,ePixelTypes eType,Word uMipMapLevels);
	WordPtr GetSuggestedBufferSize(void) const;
	WordPtr GetImageSize(Word uMipMap) const;
	const char *ValidateForSave(void) const;
	void ClearBitmap(void);
	Word VerticleFlip(void);
	Word HorizontalFlip(void);
	Word Store8888(const Image *pInput,const RGBAWord8_t *pPalette);
};
}
/* END */
#endif
