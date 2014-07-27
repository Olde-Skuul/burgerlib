/***************************************

	TGA File handler class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfiletga.h"
#include "brdebug.h"
#include "brrenderer.h"

/*! ************************************

	\class Burger::FileTGA
	\brief Reads and writes TGA files

	This class will read and write out TGA files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a TGA file (Little Endian, 14 bytes)
//

#if !defined(DOXYGEN)
struct TGAHeader {				// This is the header for a TGA file
	Word8 m_bImageIdent;		// Image identification field size in bytes
	Word8 m_bColorMapType;		// 1 for a color image
	Word8 m_bImageType;			// Data type present
	Word8 m_bColorMapOriginLow;	// Index of the first color map entry
	Word8 m_bColorMapOriginHigh;
	Word8 m_bColorMapLengthLow;	// Count of color map entries
	Word8 m_bColorMapLengthHigh;
	Word8 m_bColorMapEntrySize;	// Size (In bits) for each color entry
	Word16 m_uXOrigin;			// Lower left x origin of image
	Word16 m_uYOrigin;			// Lower left y origin of image
	Word16 m_uWidth;			// Width of the image in pixels
	Word16 m_uHeight;			// Height of the image in pixels
	Word8 m_bBitDepth;			// Bits per pixels for the image
	Word8 m_bDescriptor;		// Image descriptor bits
};

#endif

/*! ************************************

	\brief Decompress 8 bit pixels

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from

***************************************/

void BURGER_API Burger::FileTGA::UnpackPixel8(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	do {
		WordPtr uCount = pInput->GetByte();	// Get the counter
		if (uCount&0x80) {					// Packed?
			uCount = uCount-0x7F;			// Remove the high bit
			if (uCount>uOutputLength) {		// Test for buffer overrun
				uCount = uOutputLength;
			}
			uOutputLength = uOutputLength-uCount;
			Word8 uTemp = pInput->GetByte();	// Get the repeater
			do {
				pOutput[0] = uTemp;				// Fill memory
				++pOutput;
			} while (--uCount);
		} else {
			++uCount;								// +1 to the count
			if (uCount>uOutputLength) {				// Test for buffer overrun
				// Fetch the data
				pInput->Get(pOutput,uOutputLength);
				// Skip past the overrun (This is needed for some badly made TGA files)
				pInput->SkipForward(uCount-uOutputLength);
				// Force exit
				uOutputLength = 0;
			} else {
				pInput->Get(pOutput,uCount);
				pOutput+=uCount;
				uOutputLength = uOutputLength-uCount;	// Adjust amount
			}
		}
	} while (uOutputLength);	// Is there still more?
}

/*! ************************************

	\brief Decompress 16 bit pixels

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer in Word16 for decompressed data
	\param pInput Data stream to read compressed data from

***************************************/

void BURGER_API Burger::FileTGA::UnpackPixel16(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	do {
		WordPtr uCount = pInput->GetByte();	// Get the counter
		if (uCount&0x80) {					// Packed?
			uCount = uCount-0x7F;			// Remove the high bit
			if (uCount>uOutputLength) {		// Test for buffer overrun
				uCount = uOutputLength;
			}
			uOutputLength = uOutputLength-uCount;
			Word16 uTemp = pInput->GetShort();	// Get the repeater
			do {
				reinterpret_cast<Word16 *>(pOutput)[0] = uTemp;				// Fill memory
				pOutput+=2;
			} while (--uCount);
		} else {
			++uCount;								// +1 to the count
			if (uCount>uOutputLength) {				// Test for buffer overrun
#if defined(BURGER_LITTLEENDIAN)
				// Fetch the data
				pInput->Get(pOutput,uOutputLength*2);
#else
				WordPtr uTemp = uCount;
				do {
					reinterpret_cast<Word16 *>(pOutput)[0] = pInput->GetShort();
					pOutput+=2;
				} while (--uTemp);
#endif
				// Skip past the overrun (This is needed for some badly made TGA files)
				pInput->SkipForward((uCount-uOutputLength)*2);
				// Force exit
				uOutputLength = 0;
			} else {
				uOutputLength = uOutputLength-uCount;	// Adjust amount
#if defined(BURGER_LITTLEENDIAN)
				pInput->Get(pOutput,uCount*2);
				pOutput+=(uCount*2);
#else
				// Swap endian
				do {
					reinterpret_cast<Word16 *>(pOutput)[0] = pInput->GetShort();
					pOutput+=2;
				} while (--uCount);
#endif
			}
		}
	} while (uOutputLength);	// Is there still more?
}

/*! ************************************

	\brief Decompress 24 bit pixels

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer in 24 bit chunks for decompressed data
	\param pInput Data stream to read compressed data from

***************************************/

void BURGER_API Burger::FileTGA::UnpackPixel24(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	do {
		WordPtr uCount = pInput->GetByte();	// Get the counter
		if (uCount&0x80) {					// Packed?
			uCount = uCount-0x7F;			// Remove the high bit
			if (uCount>uOutputLength) {		// Test for buffer overrun
				uCount = uOutputLength;
			}
			uOutputLength = uOutputLength-uCount;
			Word8 uBlue = pInput->GetByte();	// Get the 24 bit repeater
			Word8 uGreen = pInput->GetByte();	
			Word8 uRed = pInput->GetByte();
			do {
				pOutput[0] = uRed;				// Fill memory
				pOutput[1] = uGreen;
				pOutput[2] = uBlue;
				pOutput+=3;
			} while (--uCount);
		} else {
			++uCount;								// +1 to the count
			if (uCount>uOutputLength) {				// Test for buffer overrun
				// Fetch the data
				WordPtr uTemp = uOutputLength;
				do {
					pOutput[2] = pInput->GetByte();	// Blue
					pOutput[1] = pInput->GetByte();	// Green
					pOutput[0] = pInput->GetByte();	// Red
					pOutput+=3;
				} while (--uTemp);
				// Skip past the overrun (This is needed for some badly made TGA files)
				pInput->SkipForward((uCount-uOutputLength)*3);
				// Force exit
				uOutputLength = 0;
			} else {
				uOutputLength = uOutputLength-uCount;	// Adjust amount
				do {
					pOutput[2] = pInput->GetByte();	// Blue
					pOutput[1] = pInput->GetByte();	// Green
					pOutput[0] = pInput->GetByte();	// Red
					pOutput+=3;
				} while (--uCount);
			}
		}
	} while (uOutputLength);	// Is there still more?
}


/*! ************************************

	\brief Decompress 32 bit pixels

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer in 24 bit chunks for decompressed data
	\param pInput Data stream to read compressed data from

***************************************/

void BURGER_API Burger::FileTGA::UnpackPixel32(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	do {
		WordPtr uCount = pInput->GetByte();	// Get the counter
		if (uCount&0x80) {					// Packed?
			uCount = uCount-0x7F;			// Remove the high bit
			if (uCount>uOutputLength) {		// Test for buffer overrun
				uCount = uOutputLength;
			}
			uOutputLength = uOutputLength-uCount;
			Word8 uBlue = pInput->GetByte();	// Get the 24 bit repeater
			Word8 uGreen = pInput->GetByte();	
			Word8 uRed = pInput->GetByte();
			Word8 uAlpha = pInput->GetByte();
			do {
				pOutput[0] = uRed;				// Fill memory
				pOutput[1] = uGreen;
				pOutput[2] = uBlue;
				pOutput[3] = uAlpha;
				pOutput+=4;
			} while (--uCount);
		} else {
			++uCount;								// +1 to the count
			if (uCount>uOutputLength) {				// Test for buffer overrun
				// Fetch the data
				WordPtr uTemp = uOutputLength;
				do {
					pOutput[2] = pInput->GetByte();	// Blue
					pOutput[1] = pInput->GetByte();	// Green
					pOutput[0] = pInput->GetByte();	// Red
					pOutput[3] = pInput->GetByte();	// Alpha
					pOutput+=4;
				} while (--uTemp);
				// Skip past the overrun (This is needed for some badly made TGA files)
				pInput->SkipForward((uCount-uOutputLength)*3);
				// Force exit
				uOutputLength = 0;
			} else {
				uOutputLength = uOutputLength-uCount;	// Adjust amount
				do {
					pOutput[2] = pInput->GetByte();	// Blue
					pOutput[1] = pInput->GetByte();	// Green
					pOutput[0] = pInput->GetByte();	// Red
					pOutput[3] = pInput->GetByte();	// Alpha
					pOutput+=4;
				} while (--uCount);
			}
		}
	} while (uOutputLength);	// Is there still more?
}

/*! ************************************

	\brief Parses a TGA file

	Read in a TGA file and set the extra variables

	Will parse 24 bit uncompressed and 8 bit compressed and
	uncompressed TGA files only. Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a TGA file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FileTGA::Load(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	// Start with processing the 18 byte header of the TGA file

	Word uImageIdent = 0;
	Word uColorMapType = 0;
	Word uImageType=0;		// Data type present
	Word uColorMapOrigin=0;	// Index of the first color map entry
	Word uColorMapLength=0;	// Count of color map entries
	Word uColorMapEntrySize=0;	// Size (In bits) for each color entry
	Word uXOrigin;		// Lower left x origin of image
	Word uYOrigin;		// Lower left y origin of image
	Word uWidth=0;		// Width of the image in pixels
	Word uHeight=0;		// Height of the image in pixels
	Word uBitDepth=0;	// Bits per pixels for the image
	//Word uDescriptor;	// Image descriptor bits

	if (pInput->BytesRemaining()<18) {
		pBadNews = "Insufficient data for TGA file header.";
	} else {
		// Read the 18 byte header

		uImageIdent = pInput->GetByte();
		uColorMapType = pInput->GetByte();
		uImageType = pInput->GetByte();
		uColorMapOrigin = pInput->GetShort();
		uColorMapLength = pInput->GetShort();
		uColorMapEntrySize = pInput->GetByte();
		uXOrigin = pInput->GetShort();
		uYOrigin = pInput->GetShort();
		uWidth = pInput->GetShort();
		uHeight = pInput->GetShort();
		uBitDepth = pInput->GetByte();
		/* uDescriptor = */ pInput->GetByte();

		switch (uImageType) {
		default:
			Debug::Warning("Can't parse image type %u, must be 1,2,9 or 10",uImageIdent);
			return NULL;
		case TGA_RGBINDEXED:
		case TGA_RGB:
		case TGA_RLEINDEXED:
		case TGA_RLERGB:
			break;
		}
		
		// Test for invalid formats

		if (uXOrigin || uYOrigin) {
			pBadNews = "TGA file has a non-zero origin";
#if 0		// It appears that no one really cares about this...
		} else if (uDescriptor) {
			pBadNews = "Must have zero for image descriptor";
#endif
		} else if (!uWidth) {
			pBadNews = "Image width is zero.";
		} else if (!uHeight) {
			pBadNews = "Image height is zero.";
		} else if (uImageIdent) {
			// Handle the extra data record (Skip the data)
			if (pInput->BytesRemaining()<uImageIdent) {
				pBadNews = "Insufficient data for TGA info header.";
			} else {
				pInput->SkipForward(uImageIdent);
			}
		}
	}

	// Process the palette, if any

	if (!pBadNews) {
		if (uColorMapType == 1 || uColorMapType==9) {		// Palette present!
			MemoryClear(m_Palette,sizeof(m_Palette));

			if (uColorMapLength) {			// Any colors used?
				Word i = uColorMapOrigin;	// Get the starting color
				if ((i+uColorMapLength)>256) {
					i = (i+uColorMapLength)-1;		// Get the final color index
					Debug::Warning("Color index %u cannot be > 255\n",i);
					return NULL;
				}
				RGBAWord8_t *pPalette = m_Palette+i;

				if (uColorMapEntrySize == 16) {
					do {
						Word uColor = pInput->GetShort();
						pPalette->m_uRed = Renderer::RGB5ToRGB8Table[(uColor>>10U)&0x1FU];	// Red						
						pPalette->m_uGreen = Renderer::RGB5ToRGB8Table[(uColor>>5U)&0x1FU];	// Green
						pPalette->m_uBlue = Renderer::RGB5ToRGB8Table[uColor&0x1FU];	// Blue
						pPalette->m_uAlpha = 0;
						++pPalette;
					} while (--uColorMapLength);

				} else if (uColorMapEntrySize == 24) {
					do {
						pPalette->m_uBlue = pInput->GetByte();				
						pPalette->m_uGreen = pInput->GetByte();
						pPalette->m_uRed = pInput->GetByte();
						pPalette->m_uAlpha = 0;
						++pPalette;
					} while (--uColorMapLength);
				} else if (uColorMapEntrySize == 32) {
					do {
						pPalette->m_uBlue = pInput->GetByte();				
						pPalette->m_uGreen = pInput->GetByte();
						pPalette->m_uRed = pInput->GetByte();
						pPalette->m_uAlpha = pInput->GetByte();
						++pPalette;
					} while (--uColorMapLength);
				} else {
					Debug::Warning("Palette uses %u bits per color entry, only 16,24 and 32 allowed",uColorMapEntrySize);
					return NULL;
				}
			}
		}
	}

/***************************************

	Now, there are 4 ways this file can be parsed,
	Packed/Unpacked Index color or TrueColor

	Indexed images REQUIRE a palette!

***************************************/

	if (!pBadNews) {
		if (uImageType == TGA_RGBINDEXED || uImageType==TGA_RLEINDEXED) {	// Indexed?

			if (uColorMapType != 1 && uColorMapType!=9) {
				pBadNews = "Image file is missing a palette.";
			} else if (!uBitDepth || uBitDepth >= 9) {
				Debug::Warning("The image has a bit depth of %u, only 1-8 bit indexed images can be processed.",uBitDepth);
				return NULL;
			} else {
				pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
				if (pImage) {
					WordPtr uLength = uWidth*uHeight;
					if (uImageType == TGA_RGBINDEXED) {		// Unpacked indexed data
						pInput->Get(pImage->GetImage(),uLength);
					} else {								// Compressed
						UnpackPixel8(pImage->GetImage(),uLength,pInput);	// Decompress it
					}
				}
			}
		} else {			// True color!
			if (uBitDepth==16) {
				uBitDepth = 15;
			}
			if (uBitDepth != 15 && uBitDepth != 24 && uBitDepth!=32) {
				Debug::Warning("The image has a bit depth of %u, only true color images of 15,16,24 or 32 bits can be processed.",uBitDepth);
				return NULL;
			}
			Image::ePixelTypes eType;
			if (uBitDepth==15) {
				eType = Image::PIXELTYPE555;
			} else if (uBitDepth==24) {
				eType = Image::PIXELTYPE888;
			} else {
				eType = Image::PIXELTYPE8888;
			}
			pImage = Image::New(uWidth,uHeight,eType);
			if (pImage) {
				WordPtr uLength = uWidth*uHeight;
				Word8 *pDest = pImage->GetImage();

				if (uBitDepth == 15) {				// 16 bit
					if (uImageType == TGA_RGB) {	// Unpacked?
						do {
							reinterpret_cast<Word16 *>(pDest)[0] = static_cast<Word16>(pInput->GetShort()&0x7FFF);	// Save pixel
							pDest=pDest+2;
						} while (--uLength);		// All done?
					} else {
						UnpackPixel16(pDest,uLength,pInput);
					}
				} else if (uBitDepth == 24) {
					if (uImageType == TGA_RGB) {		// Unpacked
						do {
							pDest[2] = pInput->GetByte();	// Blue
							pDest[1] = pInput->GetByte();	// Green
							pDest[0] = pInput->GetByte();	// Red
							pDest = pDest+3;
						} while (--uLength);
					} else {
						UnpackPixel24(pDest,uLength,pInput);
					}
				} else {
					if (uImageType == TGA_RGB) {		// Unpacked?
						do {
							pDest[2] = pInput->GetByte();	// Blue
							pDest[1] = pInput->GetByte();	// Green
							pDest[0] = pInput->GetByte();	// Red
							pDest[3] = pInput->GetByte();	// Alpha
							pDest = pDest+4;
						} while (--uLength);				// All done?
					} else {
						UnpackPixel32(pDest,uLength,pInput);
					}
				}
			}
		}
	}
	// If there was an error, clean up
	if (pBadNews) {
		Debug::Warning(pBadNews);
		Delete(pImage);
		pImage = NULL;
	} else if (pImage) {
		// TGA images are "upside down"
		// correct for it
		pImage->VerticleFlip();
	}
	return pImage;
}

/*! ************************************

	\fn const RGBAWord8_t *Burger::FileTGA::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the TGA file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileTGA::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FileTGA::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the TGA file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileTGA::GetPalette(void) const

***************************************/



/*! ************************************

	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit TGA file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

void Burger::FileTGA::SetPalette(const RGBWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		// Any colors to process?
		CopyPalette(m_Palette+uStartIndex,pPalette,uPaletteSize);
	}
}

/*! ************************************

	\brief Set the file image's palette (RGBA)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit TGA file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

void Burger::FileTGA::SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		CopyPalette(m_Palette+uStartIndex,pPalette,uPaletteSize);
	}
}
