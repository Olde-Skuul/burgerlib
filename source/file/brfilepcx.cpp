/***************************************

	PCX File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilepcx.h"
#include "brdebug.h"

/*! ************************************

	\class Burger::FilePCX
	\brief Reads and writes PCX files

	This routine will take a PCX file image and
	convert it into an Image structure.

	It will handle both compressed 8,15,24 and 32
	bit TGA images.

	This class will read and write out PCX files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a PCX file (Little Endian, 128 bytes)
//

#if !defined(DOXYGEN)

struct PCXHeader {
	Word8 m_bManufacturer;		// Must be 10 for PC Paintbrush
	Word8 m_bVersion;			// 0 = 2.5, 2 = 2.8, 3 = 2.8 with color 5 = 3.0
	Word8 m_bEncoding;			// 1 = PCX run length encoding
	Word8 m_bBitsPerPixel;		// Bits per pixel
	Word16 m_uMinX,m_uMinY,m_uMaxX,m_uMaxY;	// Bounding rect (Inclusive)
	Word16 m_uXDPI,m_uYDPI;		// DPI (Usually 72)
	Word8 m_EGAPalette[48];		// Standard EGA Palette (Ignore)
	Word8 m_bZero1;				// Reserved
	Word8 m_bBitPlanes;			// Number of bit planes (1)
	Word16 m_uBytesPerLine;		// Bytes per line
	Word8 m_Padding[60];		// This struct must be 128 bytes in size
};

#endif

/*! ************************************

	\brief Decompress PCX bitmap data

	The format is simplicity itself.

	If the input byte is less than 0xC0, then output the byte.
	Otherwise and with 0x3F and use it as a count and fill with
	the next byte.

	Opcode 0xC0 does nothing.

	Note : When asked to decode an ODD number of pixels, there is a hack
	to fix a bug in the data overrun buffer. Debabelizer can write out bad
	PCX files when the width is odd.

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FilePCX::DecompressPCX(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	if (uOutputLength) {
		do {
			Word uTemp = pInput->GetByte();		// Fetch byte from input stream
			if (uTemp<0xC0) {					// Unpacked data?
				pOutput[0] = static_cast<Word8>(uTemp);	// Save byte
				++pOutput;
				--uOutputLength;				// Count down the length
			} else {
				uTemp = uTemp&0x3F;		// Mask count
				if (uTemp) {			// Was it not 0xC0?
					if (uTemp>uOutputLength) {				// Overrun buffer?
						return "PCX decompresser overrun\n";
					}

					Word8 uFill = pInput->GetByte();		// Get fill byte
					uOutputLength = uOutputLength-uTemp;	// Adjust length
					do {
						pOutput[0] = uFill;	// Memory fill
						++pOutput;
					} while (--uTemp);		// Count down
				}
			}
		} while (uOutputLength);		// More?
	}
	return NULL;		/* Return input stream pointer */
}

/*! ************************************

	\brief Merge a red, green and blue strip

	PCX files interleave an 8 bit per pixel scan line of only
	red pixels, followed by a line of green and then blue.

	This function will merge the three into a strip of RGB triplets

	\param pOutput Buffer to receive the RGB triplets
	\param pInput Buffer containing three strips of Red, Green and Blue pixels
	\param uWidth Number of bytes each strip contains

***************************************/

void BURGER_API Burger::FilePCX::Merge3Planes(Word8 *pOutput,const Word8 *pInput,WordPtr uWidth)
{
	WordPtr i = uWidth;
	do {
		pOutput[0] = pInput[0];
		pOutput[1] = pInput[uWidth];
		pOutput[2] = pInput[uWidth*2];
		pOutput+=3;
		++pInput;
	} while (--i);
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FilePCX::FilePCX() :
	m_uXPixelsPerInch(DEFAULT_PIXELS_PER_INCH),		//72 DPI
	m_uYPixelsPerInch(DEFAULT_PIXELS_PER_INCH)		//72 DPI
{
	MemoryClear(m_EGAPalette,sizeof(m_EGAPalette));
}

/*! ************************************

	\brief Parses a PCX file

	Read in a PCX file and set the extra variables

	Will parse 24 bit uncompressed and 8 bit compressed and
	uncompressed PCX files only. Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a PCX file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FilePCX::Load(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	// Start with processing the 128 byte header of the PCX file

	Word uBitPlanes=0;
	Word uBytesPerLine=0;
	Word uWidth=0;
	Word uHeight=0;

	if (pInput->BytesRemaining()<128) {
		pBadNews = "Insufficient data for PCX file header.";
	} else {
		Word bManufacturer = pInput->GetByte();
		Word uVersion = pInput->GetByte();
		Word uEncoding = pInput->GetByte();
		Word uBitsPerPixel = pInput->GetByte();
		Word uMinX = pInput->GetShort();
		Word uMinY = pInput->GetShort();
		Word uMaxX = pInput->GetShort();
		Word uMaxY = pInput->GetShort();
		m_uXPixelsPerInch = pInput->GetShort();
		m_uYPixelsPerInch = pInput->GetShort();
		pInput->Get(m_EGAPalette,sizeof(m_EGAPalette));
		pInput->SkipForward(1);
		uBitPlanes = pInput->GetByte();
		uBytesPerLine = pInput->GetShort();
		pInput->SkipForward(60);

		if (bManufacturer!=10) {
			pBadNews = "Not a PC-Paintbrush PCX file.";
		} else if (uVersion!=PCX_VERSION30) {
			pBadNews = "Only version 3.0 PCX files are supported.";
		} else if (uEncoding!=1) {
			pBadNews = "Only PCX encoded PCX files are supported.";
		} else if (uBitsPerPixel!=8) {
			pBadNews = "Not an 8 bit per pixel PCX file.";
		} else if (uBitPlanes!=1 && uBitPlanes!=3) {
			pBadNews = "The PCX file must be \"Chunky\" pixel format.";
		} else {
			uWidth = (uMaxX-uMinX)+1;
			uHeight = (uMaxY-uMinY)+1;
			if (static_cast<int>(uWidth)<=0) {
				pBadNews = "Invalid image width in PCX file.";
			} else if (static_cast<int>(uHeight)<=0) {
				pBadNews = "Invalid image height in PCX file.";

			// Rounded up...
			} else if (uBytesPerLine!=((uWidth+1)&(~1))) {
				pBadNews = "Bytes per line does not match PCX image width.";
			}
		}
	}

	// Let's parse out the data

	if (!pBadNews) {

		// Handle the 8 bit paletted image

		if (uBitPlanes==1) {
			pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
			if (pImage) {
				Word8 *pDest = pImage->GetImage();
				do {
					pBadNews = DecompressPCX(pDest,uBytesPerLine,pInput);
					if (pBadNews) {
						break;
					}
					pDest+=pImage->GetStride();
				} while (--uHeight);
			
				// Let's parse out the palette!

				// Note, the PCX format SAYS I'm supposed to find a 12 as the next
				// byte, however, I've found a rare file that has a 0xC1,0xFF in the
				// byte stream of pad data. This little hack will be able to skip pad
				// data and find the palette

				if (!pBadNews) {
					Word uTemp = 8;					// Only scan 8 bytes. I assume I'm screwed if it's beyond this
					do {
						if (pInput->GetByte()==12) {		// Is the current byte a palette entry?
							break;
						}
					} while (--uTemp);
					if (!uTemp) {				// Did I not find a palette?
						// I can't find a palette token for 8 bytes
						pBadNews = "Palette command byte was not found in PCX file!\n";
					} else {
						// Read in the palette
						uTemp = 256;
						RGBAWord8_t *pPalette = m_Palette;
						do {
							pPalette->m_uRed = pInput->GetByte();
							pPalette->m_uGreen = pInput->GetByte();
							pPalette->m_uBlue = pInput->GetByte();
							pPalette->m_uAlpha = 0;
							++pPalette;
						} while (--uTemp);
					}
				}
			}
		} else {
			// Handle the 24 bits per pixel image
			pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE888);
			if (pImage) {
				// Allocate the temp buffer here to reduce the chance
				// of memory fragmentation
				// Note: Added 32 to the buffer size to prevent buffer overruns
				Word8 *pTempBuffer = static_cast<Word8 *>(Alloc(uWidth*3+32));
				if (!pTempBuffer) {
					pBadNews = "Out of memory.";
				} else {
					Word8 *pDest = pImage->GetImage();
					do {
						pBadNews = DecompressPCX(pTempBuffer,uBytesPerLine,pInput);
						if (pBadNews) {
							break;
						}
						pBadNews = DecompressPCX(pTempBuffer+uWidth,uBytesPerLine,pInput);
						if (pBadNews) {
							break;
						}
						pBadNews = DecompressPCX(pTempBuffer+uWidth*2,uBytesPerLine,pInput);
						if (pBadNews) {
							break;
						}
						// Merge the bit planes
						Merge3Planes(pDest,pTempBuffer,uWidth);
						pDest+=pImage->GetStride();
					} while (--uHeight);
					// Release the temp buffer
					Free(pTempBuffer);
				}
			}
		}
	}

	// If there was an error, clean up
	if (pBadNews) {
		Delete(pImage);
		Debug::Warning(pBadNews);
		pImage = NULL;
	}
	return pImage;
}



/*! ************************************

	\fn Int32 Burger::FilePCX::GetXPixelsPerInch(void) const
	\brief Get the file image's pixels per meter for X

	Return the pixels per meter constant, usually it's 2835 (2834.6472f)

	\return The pixels per meter constant for the X direction
	\sa Burger::FilePCX::SetXPixelsPerInch(Word16) or Burger::FilePCX::GetYPixelsPerInch(void) const

***************************************/

/*! ************************************

	\fn void Burger::FilePCX::SetXPixelsPerInch(Word16 uXPixelsPerInch)
	\brief Set the file image's pixels per meter for X

	Set the pixels per meter constant

	\param uXPixelsPerInch New pixels per meter constant
	\sa Burger::FilePCX::DEFAULT_PIXELS_PER_INCH, Burger::FilePCX::GetXPixelsPerInch(void) const or Burger::FilePCX::SetYPixelsPerInch(Word16)

***************************************/

/*! ************************************

	\fn Int32 Burger::FilePCX::GetYPixelsPerInch(void) const
	\brief Get the file image's pixels per meter for Y

	Return the pixels per meter constant, usually it's 2835 (2834.6472f)

	\return The pixels per meter constant for the Y direction
	\sa Burger::FilePCX::SetYPixelsPerInch(Word16) or Burger::FilePCX::GetXPixelsPerInch(void) const

***************************************/

/*! ************************************

	\fn void Burger::FilePCX::SetYPixelsPerInch(Word16 uYPixelsPerInch)
	\brief Set the file image's pixels per meter for Y

	Set the pixels per meter constant

	\param uYPixelsPerInch New pixels per meter constant
	\sa Burger::FilePCX::DEFAULT_PIXELS_PER_INCH, Burger::FilePCX::GetYPixelsPerInch(void) const or Burger::FilePCX::SetXPixelsPerInch(Word16)

***************************************/



/*! ************************************

	\fn const RGBAWord8_t *Burger::FilePCX::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the PCX file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FilePCX::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FilePCX::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the PCX file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FilePCX::GetPalette(void) const

***************************************/



/*! ************************************

	\fn const Word8 *Burger::FilePCX::GetEGAPalette(void) const
	\brief Get the file image's EGA palette

	Obtain a constant pointer to the 48 byte EGA palette (16 colors, 3 bytes per color)

	\return Constant pointer to the 16 entry EGA palette
	\sa Burger::FilePCX::GetEGAPalette(void)

***************************************/

/*! ************************************

	\fn Word8 *Burger::FilePCX::GetEGAPalette(void)
	\brief Get the file image's EGA palette

	Obtain a pointer to the 48 byte EGA palette (16 colors, 3 bytes per color)

	\return Pointer to the 16 entry EGA palette
	\sa Burger::FilePCX::GetEGAPalette(void) const

***************************************/



/*! ************************************

	\fn void Burger::FilePCX::SetPalette(const RGBWord8_t *pInput,Word uStartIndex,Word uPaletteSize)
	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit PCX file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

/*! ************************************

	\fn void Burger::FilePCX::SetPalette(const RGBAWord8_t *pInput,Word uStartIndex,Word uPaletteSize)
	\brief Set the file image's palette (RGBA)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit PCX file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

