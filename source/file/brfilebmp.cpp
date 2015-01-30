/***************************************

	BMP File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilebmp.h"
#include "brdebug.h"
#include "brendian.h"

/*! ************************************

	\class Burger::FileBMP
	\brief Reads and writes BMP files

	This class will read and write out BMP files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a BMP file (Little Endian, 14 bytes)
//

#if !defined(DOXYGEN)
struct BMPHeader {
	char m_ID[2];			// Ascii "BM"
	Word32 m_uFileSize;		// Length of the file (Forces a 4 Gig limit)
	Word16 m_uReserved1;	// Reserved, as of 4/6/2014, not used
	Word16 m_uReserved2;	
	Word32 m_uHeaderSize;	// Offset to the bitmap bits
};

// Info of a BMP file (Little endian, 40 bytes)

struct BMPInfo {
	Word32 InfoSize;		// Size of the info struct
	Word32 Width;			// Width of the bit map in pixels
	Word32 Height;			// Height of the bit map in pixels
	Word16 BitPlanes;		// Number of bit planes (1)
	Word16 BitDepth;		// Pixel bit depth (8)
	Word32 biCompression;	// Data compression?
	Word32 biSizeImage;
	Word32 biXPelsPerMeter;	// Pixels per meter for printing
	Word32 biYPelsPerMeter;
	Word32 biClrUsed;		// Number of colors used
	Word32 biClrImportant;
};
#endif

/*! ************************************

	\brief Take a line of 8 bit pixel data and compress is using the RLE8 format.

	The format itself sucks eggs.

	First send a compress token of 0 or a fill count of 1-255
	If it's 1-255, then follow the token with the byte to fill with.

	If it's zero, then follow it with zero for EOL, 1 for EOP data
	and 3-FF for raw data (Followed by raw data). Don't use token 2 (Jump)
	
	Since the format needs to know when to send the end of image token, the
	parameter bSendEOP must be set to \ref TRUE when it's to be sent

	\param pOutput Pointer to the memory stream to output the compressed bytes into
	\param pInput Pointer to a scan line of pixels
	\param uInputLength Number of input bytes to process
	\param bSendEOP \ref TRUE when sending the last scan line

***************************************/

void BURGER_API Burger::FileBMP::CompressRLE8(OutputMemoryStream *pOutput,const Word8 *pInput,WordPtr uInputLength,Word bSendEOP)
{
	Word8 TempBuffer[16];	// Word8 array for file writing
	Word uMaximum;		/* Maximum run length */
	Word uCount;		/* Current run length */
	
	// Only compress if there are more than 2 bytes in the queue
	if (uInputLength>=3) {
		do {
			// Maximum number of bytes to check (3-255)
			if (uInputLength>255) {
				uMaximum = 255;			// Maximum run
			} else {
				uMaximum = static_cast<Word>(uInputLength);
			}
			Word uFill = pInput[0];		// Get the first byte
			if (pInput[1]==uFill) {		// Try for a run...
				uCount = 2;				// Assume 2 bytes for fill
				do {
					if (pInput[uCount]!=uFill) {	// Match?
						break;						// Get out now!
					}
				} while (++uCount<uMaximum);
				TempBuffer[0] = static_cast<Word8>(uCount);	// Save the run length
				TempBuffer[1] = static_cast<Word8>(uFill);	// Fill byte
				pOutput->Append(TempBuffer,2);				// Output the run
			} else {
				// This is for raw data
				uCount = 3;				// Minimum of 3
				uMaximum -= 2;			// Reduce the maximum run to 1-253 (To allow for 2 byte lookahead)
				if (uCount<uMaximum) {
					do {
						// Scan for a matching future pair (To break the raw compression)
						if ((pInput[uCount]==pInput[uCount+1]) &&
							(pInput[uCount+1]==pInput[uCount+2])) {
							goto DoIt;
						}
					} while (++uCount<uMaximum);
					uCount = uMaximum+2;					// Undo the maximum reduction
					if ((uCount&1) && uCount<uInputLength) {	// Try to not waste data
						--uCount;
					}
				}
DoIt:
				TempBuffer[0] = 0;							// Raw token
				TempBuffer[1] = static_cast<Word8>(uCount);	// Run count
				pOutput->Append(TempBuffer,2);
				pOutput->Append(pInput,uCount);				// Send the raw data
				if (uCount&1) {
					pOutput->Append(static_cast<Word8>(0));	// Pad to a short
				}
			}
			uInputLength-=uCount;		// Remove length
			pInput+=uCount;				// Adjust the source pointer
		} while (uInputLength>=3);		// More remaining?
	}

	// There are 0, 1 or 2 bytes remaining
	// Force a raw packet to clean up the rest

	switch (uInputLength) {		// Any data remaining?
	default:					// Should never enter here
		uCount = 0;				// Init the output index
		break;

	case 2:
		uMaximum = pInput[0];	// Fill byte
		TempBuffer[1] = static_cast<Word8>(uMaximum);
		if (uMaximum==pInput[1]) {
			TempBuffer[0] = 2;			// Create a run of 2 with a fill
			uCount = 2;
			break;
		}
		TempBuffer[0] = 1;			// Save the two raw bytes
		TempBuffer[2] = 1;
		TempBuffer[3] = pInput[1];
		uCount = 4;
		break;

	case 1:
		TempBuffer[0] = 1;			// Save the single byte
		TempBuffer[1] = pInput[0];	// Send the last byte
		uCount = 2;
		break;
	}

	// Send the End of Page or End of Line token

	bSendEOP = (bSendEOP==TRUE);	// Line = 1 for EOP (Force Bool)
	TempBuffer[uCount] = 0;			// END token
	++uCount;
	TempBuffer[uCount] = static_cast<Word8>(bSendEOP);	// EOL or EOP (0 or 1)
	++uCount;
	pOutput->Append(TempBuffer,uCount);
}


/*! ************************************

	\brief Unpack a single line of RLE data for a BMP file using RLE8 format.

	It's a little strange but here goes...

	Grab a byte, if it's not zero then use it as a repeat
		count and grab the next byte as the fill byte.

	If it is zero, then this is an escape code.
		0 : end of line
		1 : end of data
		2 : Delta, get two bytes for a pen move code (Not used)
		3-0xFF absolute run (But Word16 align the source pointer after run)
	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileBMP::DecompressRLE8(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	// Any data to parse?
	if (uOutputLength) {
		for (;;) {
			Word uToken = pInput->GetByte();	// Get the token
			if (uToken) {						// Is it a run length?
				if (uOutputLength<uToken) {		// Too big for the output?
					// Abort
					pBadNews = "Run length is too large in RLE8 BMP file!";
					break;
				}
				uOutputLength=uOutputLength-uToken;	// Remove the length
				Word8 uFill = pInput->GetByte();	// Get the fill byte
				do {
					pOutput[0] = uFill;		// Fill the buffer
					++pOutput;
				} while (--uToken);	// Run done?
			} else {
				uToken = pInput->GetByte();	// Get the uncompressed run size
				if (uToken<3) {		// End of file or line?
					if (uToken==2) {
						pBadNews = "Can't accept a jump RLE opcode!";
						break;
					}
					if (uOutputLength) {	// Check if the output has been satisfied
						pBadNews = "Insufficient data in RLE8 packed array!";
						break;
					}
					if (uToken==1) {
						pInput->SkipBack(2);	// Force this token permanently!
					}
					break;				// Exit processing
				}
				if (uToken>uOutputLength) {		// Too big?
					pBadNews = "Run length is too large!";
					break;
				}
				uOutputLength=uOutputLength-uToken;	// Remove the length
				pInput->Get(pOutput,uToken);
				pOutput+=uToken;				// Adjust the output pointer
				if (uToken&1) {
					pInput->GetByte();			// Skip a single byte
				}
			}
		}
	}

	// Was there an error?
	return pBadNews;
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileBMP::FileBMP() :
	m_uReserved1(0),
	m_uReserved2(0),
	m_iXPixelsPerMeter(DEFAULT_PIXELS_PER_METER),		//2834.6472 pixels per meter
	m_iYPixelsPerMeter(DEFAULT_PIXELS_PER_METER),		//2834.6472 pixels per meter
	m_uColorsUsed(0),
	m_uColorsImportant(0)
{
}

/*! ************************************

	\brief Parses a BMP file

	Read in a BMP file and set the extra variables

	Will parse 16, 24 and 32 bit uncompressed and 8 bit compressed and
	uncompressed BMP files only. Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a BMP file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Word BURGER_API Burger::FileBMP::Load(Image *pOutput,InputMemoryStream *pInput)
{
	Word8 ID[4];
	const char *pBadNews = NULL;
	Word uResult = 10;
	Word32 uHeaderSize = 0;
	Word32 uInfoSize = 0;

	// Start with processing the 14 byte header of the BMP file

	if (pInput->BytesRemaining()<14) {
		pBadNews = "Insufficient data for BMP file header.";
	} else {
		
		// Read the 14 byte header

		pInput->Get(ID,2);
		Word32 uFileSize = pInput->GetWord32();
		m_uReserved1 = pInput->GetShort();
		m_uReserved2 = pInput->GetShort();
		uHeaderSize = pInput->GetWord32();

		if ((ID[0]!='B') || (ID[1]!='M')) {
			pBadNews = "No 'BM' id string found.";
		} else if (uFileSize != (pInput->BytesRemaining()+14)) {
			pBadNews = "File size entry doesn't match the file length.";
		} else if (uHeaderSize < (14+40)) {
			pBadNews = "Header size is smaller than 54 bytes.";
		} else {
			// Skip past the extra data, if any
			if (pInput->BytesRemaining()<40) {
				pBadNews = "Insufficient data for BMP info header.";
			}
		}
	}

	// If no error, process the 40 byte info record

	Word32 uWidth = 0;
	Word32 uHeight = 0;
	Word32 uBitPlanes = 0;
	Word32 uBitDepth = 0;
	Word32 uCompression = BMP_RGB;

	if (!pBadNews) {
		// Pull in the 40 byte header
		uInfoSize = pInput->GetWord32();
		uWidth = pInput->GetWord32();
		uHeight = pInput->GetWord32();
		uBitPlanes = pInput->GetShort();
		uBitDepth = pInput->GetShort();
		uCompression = pInput->GetWord32();
		/* Word32 uSizeImage = */ pInput->GetWord32();
		m_iXPixelsPerMeter = static_cast<Int32>(pInput->GetWord32());
		m_iYPixelsPerMeter = static_cast<Int32>(pInput->GetWord32());
		m_uColorsUsed = pInput->GetWord32();
		m_uColorsImportant = pInput->GetWord32();

		if ((uBitDepth != 8) && 
			(uBitDepth != 16) && 
			(uBitDepth != 24) && 
			(uBitDepth != 32)) {
			Debug::Warning("Can't process %u bits per pixel, only 8, 16, 24 or 32.",static_cast<unsigned int>(uBitDepth));
			return 10;
		}
		if ((uCompression!=BMP_RGB) && (uCompression!=BMP_RLE8)) {
			pBadNews = "Compression algorithm is not supported.";
		} else if ((uBitDepth!=8) && (uCompression!=BMP_RGB)) {		// No compression on 24 bit
			pBadNews = "Non 8 bit image file is compressed.";
		} else if (!uWidth) {
			pBadNews = "Image width is zero.";
		} else if (!uHeight) {
			pBadNews = "Image height is zero.";
		} else if (uBitPlanes!=1) {
			pBadNews = "Bit plane count is not one.";
		} else if (uInfoSize<40) {
			pBadNews = "Header size is less than 40 bytes in size.";
		}
	}

	// Process the palette

	if (!pBadNews && (uBitDepth==8)) {
		pInput->SetMark(14+uInfoSize);
		if (pInput->BytesRemaining()<1024) {
			pBadNews = "Insufficient data for BMP palette.";
		} else {
			MemoryClear(m_Palette,sizeof(m_Palette));
			Word i = 256;
			RGBAWord8_t *pWork = m_Palette;
			do {
				pWork->m_uBlue = pInput->GetByte();		// Read in the palette
				pWork->m_uGreen = pInput->GetByte();
				pWork->m_uRed = pInput->GetByte();
				pWork->m_uAlpha = pInput->GetByte();
				++pWork;
			} while (--i);
		}
	}

	if (!pBadNews) {
		Image::ePixelTypes eType;
		if (uBitDepth==8) {
			eType = Image::PIXELTYPE8BIT;
		} else if (uBitDepth==16) {
			eType = Image::PIXELTYPE555;
		} else if (uBitDepth==24) {
			eType = Image::PIXELTYPE888;
		} else {
			eType = Image::PIXELTYPE8888;
		}
		uResult = pOutput->Init(uWidth,uHeight,eType);
		if (uResult) {
			pBadNews = "Out of memory.";
		} else {
			pOutput->ClearBitmap();
		}
	}

	// At this point, if pBadNews wasn't set, assume
	// that an Image was allocated and data is ready to be processed

	if (!pBadNews) {
		WordPtr uStride = pOutput->GetStride();
		// Get the pointer to the BOTTOM scan line to flip
		// the image
		Word8 *pData = pOutput->GetImage()+((uHeight-1)*uStride);
		if (uCompression==BMP_RLE8) {
			Word i = uHeight;
			do {
				pBadNews = DecompressRLE8(pData,uWidth,pInput);
				if (pBadNews) {			// Error?
					break;
				}
				pData = pData-uStride;	// Next line up
			} while (--i);				// All done?
		} else {
			Word i = uHeight;
			if (uBitDepth==8) {
				Word uBMPPadding = (0-uWidth)&3;
				do {
					pInput->Get(pData,uWidth);	// Copy the line
					pInput->SkipForward(uBMPPadding);	// Handle padding
					pData = pData-uStride;		// Next line up
				} while(--i);
			} else if (uBitDepth==16) {
				Word uBMPPadding = (0-(uWidth*2))&3;
				do {
					Word j = uWidth;		// Pixel count
					Word8 *pDest = pData;
					do {
						reinterpret_cast<Word16 *>(pDest)[0] = static_cast<Word16>(pInput->GetShort()&0x7FFF);
						pDest=pDest+2;		// Next pixel
					} while (--j);			// Count down
					pInput->SkipForward(uBMPPadding);	// Handle padding
					pData = pData-uStride;		// Next line up
				} while(--i);
			} else if (uBitDepth==24) {
				Word uBMPPadding = (0-(uWidth*3))&3;
				do {
					Word j = uWidth;		// Pixel count
					Word8 *pDest = pData;
					do {
						pDest[2] = pInput->GetByte();	// Blue
						pDest[1] = pInput->GetByte();	// Green
						pDest[0] = pInput->GetByte();	// Red
						pDest=pDest+3;		// Next pixel
					} while (--j);			// Count down
					pInput->SkipForward(uBMPPadding);	// Handle padding
					pData = pData-uStride;		// Next line up
				} while(--i);
			} else {
				// RGBA
				do {
					Word j = uWidth;		// Pixel count
					Word8 *pDest = pData;
					do {
						pDest[2] = pInput->GetByte();	// Blue
						pDest[1] = pInput->GetByte();	// Green
						pDest[0] = pInput->GetByte();	// Red
						pDest[3] = pInput->GetByte();	// Alpha
						pDest=pDest+4;		// Next pixel
					} while (--j);			// Count down
					pData = pData-uStride;		// Next line up
				} while(--i);
			}
		}
	}

	// If there was an error, clean up
	if (pBadNews) {
		Debug::Warning(pBadNews);
		uResult = 10;
	}
	return uResult;
}

/*! ************************************

	\brief Write an Image structure out as a BMP file image

	Given an image in 8, 15, 16, 24 or 32 bit format, write it out as a BMP
	file.

	8, 15, 24 and 32 bit images are written out without alteration. 16 bit formats
	will have any alpha data from them stripped, this is a limitation of the BMP file format

	\note The BMP file format only supports compression on 8 bit paletted
	images. For all other image formats, the bCompress parameter is ignored.

	\param pOutput Pointer to the output stream to store the file image
	\param pImage Pointer to a valid Image structure containing the image data
	\param bCompress \ref TRUE to apply compression to 8 bit images

	\return Zero if successful

***************************************/

Word Burger::FileBMP::Save(OutputMemoryStream *pOutput,const Image *pImage,Word bCompress)
{
	// Temp buffer
	Word8 Filler[16];

	Image::ePixelTypes eType = pImage->GetType();
	// Only 24 and 8 bit formats are supported
	if ((eType!=Image::PIXELTYPE8BIT) &&
		(eType!=Image::PIXELTYPE555) && 
		(eType!=Image::PIXELTYPE1555) && 
		(eType!=Image::PIXELTYPE888) && 
		(eType!=Image::PIXELTYPE8888)) {
		return 10;
	}

	// Save the signature (14 bytes)

	pOutput->Append("BM");
	WordPtr uFileLengthMark = pOutput->GetSize();
	pOutput->Append(static_cast<Word32>(0));		// Write the file length (Change later)
	pOutput->Append(m_uReserved1);		// Write the reserved entries
	pOutput->Append(m_uReserved2);

	// Save the image description block and maybe the palette

	Word32 uMark = 14+40+1024;
	Word uDepth = 8;
	if (eType!=Image::PIXELTYPE8BIT) {
		bCompress = FALSE;	// No compression is allowed!!!
		uMark = 14+40;		// No palette
		if (eType==Image::PIXELTYPE888) {
			uDepth = 24;
		} else if (eType==Image::PIXELTYPE8888) {
			uDepth = 32;
		} else {
			uDepth = 16;
		}
	}
	Word uWidth = pImage->GetWidth();
	Word uHeight = pImage->GetHeight();
	WordPtr uStride = pImage->GetStride();

	pOutput->Append(uMark);		// Header with palette size
	pOutput->Append(static_cast<Word32>(40));		// Data info size
	pOutput->Append(static_cast<Word32>(uWidth));	// Image size
	pOutput->Append(static_cast<Word32>(uHeight));
	pOutput->Append(static_cast<Word16>(1));		// Bit planes
	pOutput->Append(static_cast<Word16>(uDepth));	// Bit depth
	pOutput->Append(static_cast<Word32>(bCompress ? BMP_RLE8 : BMP_RGB));		// Data type
	WordPtr uOutputMark = pOutput->GetSize();
	pOutput->Append(static_cast<Word32>(0));		// Output length
	pOutput->Append(static_cast<Word32>(m_iXPixelsPerMeter));	// Pixels per meter
	pOutput->Append(static_cast<Word32>(m_iYPixelsPerMeter));
	pOutput->Append(static_cast<Word32>(m_uColorsUsed));		// Colors used
	pOutput->Append(static_cast<Word32>(m_uColorsImportant));	// Colors important

	// Output the supported types! 8 Bit, 24 and 32 bit

	MemoryClear(Filler,sizeof(Filler));
	const Word8 *pData = pImage->GetImage()+((uHeight-1)*uStride);
	WordPtr uSizeMark;
	if (eType==Image::PIXELTYPE8BIT) {

		// 8 bit paletted!
		// Write out the palette first

		RGBAWord8_t *pPalette = m_Palette;	// First save the palette
		Word i = 256;
		do {
			pOutput->Append(pPalette->m_uBlue);		// Blue
			pOutput->Append(pPalette->m_uGreen);	// Green
			pOutput->Append(pPalette->m_uRed);		// Red
			pOutput->Append(pPalette->m_uAlpha);	// Extra
			++pPalette;
		} while (--i);				// All done?

		// Adjust the start mark for the pixel data to skip past the inserted palette
		uSizeMark = pOutput->GetSize();

		// Compressed?
		if (bCompress) {
			do {
				CompressRLE8(pOutput,pData,uWidth,uHeight==1);	// Compress a line
				pData = pData-uStride;	// Next line up
			} while (--uHeight);		// All done?
		} else {

			// Raw!
			Word uPadding = (0-uWidth)&3;	// 0,3,2,1
			do {
				pOutput->Append(pData,uWidth);	// Save a line to disk 
				pOutput->Append(Filler,uPadding);	// Filler
				pData = pData-uStride;	// Next line up
			} while (--uHeight);				// All done?
		}
	} else {
		uSizeMark = pOutput->GetSize();
		if (eType==Image::PIXELTYPE888) {
			Word uPadding = (0-(uWidth*3))&3;	// Padding after each pixel
			do {
				Word j = uWidth;		// Get count
				const Word8 *pWork = pData;
				do {
					pOutput->Append(pWork[2]);	// Save out the scan line
					pOutput->Append(pWork[1]);
					pOutput->Append(pWork[0]);
					pWork=pWork+3;		// Next triplet
				} while (--j);			// Next pixel
				pOutput->Append(Filler,uPadding);	// Filler
				pData = pData-uStride;	// Next line up
			} while (--uHeight);		// All done?
		} else if (eType==Image::PIXELTYPE8888) {
			// 8888 format
			do {
				Word j = uWidth;		// Get count
				const Word8 *pWork = pData;
				do {
					pOutput->Append(pWork[2]);	// Save out the scan line
					pOutput->Append(pWork[1]);
					pOutput->Append(pWork[0]);
					pOutput->Append(pWork[3]);
					pWork=pWork+4;		// Next triplet
				} while (--j);			// Next pixel
				pData = pData-uStride;	// Next line up
			} while (--uHeight);		// All done?	
		} else {
			Word uPadding = (0-(uWidth*2))&3;	// Padding after each pixel
			// 1555 or x555 format
			do {
				Word j = uWidth;		// Get count
				const Word8 *pWork = pData;
				do {
					pOutput->Append(static_cast<Word16>(reinterpret_cast<const Word16 *>(pWork)[0]&0x7FFFU));	// Save out the scan line
					pWork=pWork+2;		// Next triplet
				} while (--j);			// Next pixel
				pOutput->Append(Filler,uPadding);	// Filler
				pData = pData-uStride;	// Next line up
			} while (--uHeight);		// All done?
		}
	}

	// Wrap up the file

	WordPtr uTell = pOutput->GetSize();

	// Write the length of the total file
	// +2 is for "BM"
	Word32 uLittleData = LittleEndian::Load(static_cast<Word32>((uTell-uFileLengthMark)+2));
	pOutput->Overwrite(&uLittleData,sizeof(Word32),uFileLengthMark);

	// Write the length of the packed data
	uLittleData = LittleEndian::Load(static_cast<Word32>(uTell-uSizeMark));
	pOutput->Overwrite(&uLittleData,sizeof(Word32),uOutputMark);
	return FALSE;
}


/*! ************************************

	\fn Word16 Burger::FileBMP::GetReserved1(void) const
	\brief Get the file image's first reserved value

	Return the BMP file header's first reserved 16 bit value in native endian

	\note This value is usually zero. It's only to contain application specific data and
	is not recommended for storing data.

	\return The first 16 bit reserved value.
	\sa Burger::FileBMP::SetReserved1(Word16) or Burger::FileBMP::GetReserved2(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileBMP::SetReserved1(Word16 uReserved1)
	\brief Set the file image's first reserved value

	Sets the BMP file header's first reserved 16 bit value from native endian
	
	\param uReserved1 New first reserved value
	\sa Burger::FileBMP::GetReserved1(void) const or Burger::FileBMP::SetReserved2(Word16)

***************************************/

/*! ************************************

	\fn Word16 Burger::FileBMP::GetReserved2(void) const
	\brief Get the file image's second reserved value

	Return the BMP file header's second reserved 16 bit value in native endian

	\note This value is usually zero. It's only to contain application specific data and
	is not recommended for storing data.

	\return The second 16 bit reserved value.
	\sa Burger::FileBMP::SetReserved2(Word16) or Burger::FileBMP::GetReserved1(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileBMP::SetReserved2(Word16 uReserved2)
	\brief Set the file image's second reserved value

	Sets the BMP file header's second reserved 16 bit value from native endian
	
	\param uReserved2 New second reserved value
	\sa Burger::FileBMP::GetReserved2(void) const or Burger::FileBMP::SetReserved1(Word16)

***************************************/



/*! ************************************

	\fn Int32 Burger::FileBMP::GetXPixelsPerMeter(void) const
	\brief Get the file image's pixels per meter for X

	Return the pixels per meter constant, usually it's 2835 (2834.6472f)

	\return The pixels per meter constant for the X direction
	\sa Burger::FileBMP::SetXPixelsPerMeter(Int32) or Burger::FileBMP::GetYPixelsPerMeter(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileBMP::SetXPixelsPerMeter(Int32 iXPixelsPerMeter)
	\brief Set the file image's pixels per meter for X

	Set the pixels per meter constant

	\param iXPixelsPerMeter New pixels per meter constant
	\sa Burger::FileBMP::DEFAULT_PIXELS_PER_METER, Burger::FileBMP::GetXPixelsPerMeter(void) const or Burger::FileBMP::SetYPixelsPerMeter(Int32)

***************************************/

/*! ************************************

	\fn Int32 Burger::FileBMP::GetYPixelsPerMeter(void) const
	\brief Get the file image's pixels per meter for Y

	Return the pixels per meter constant, usually it's 2835 (2834.6472f)

	\return The pixels per meter constant for the Y direction
	\sa Burger::FileBMP::SetYPixelsPerMeter(Int32) or Burger::FileBMP::GetXPixelsPerMeter(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileBMP::SetYPixelsPerMeter(Int32 iYPixelsPerMeter)
	\brief Set the file image's pixels per meter for Y

	Set the pixels per meter constant

	\param iYPixelsPerMeter New pixels per meter constant
	\sa Burger::FileBMP::DEFAULT_PIXELS_PER_METER, Burger::FileBMP::GetYPixelsPerMeter(void) const or Burger::FileBMP::SetXPixelsPerMeter(Int32)

***************************************/



/*! ************************************

	\fn const RGBAWord8_t *Burger::FileBMP::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the BMP file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileBMP::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FileBMP::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the BMP file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileBMP::GetPalette(void) const

***************************************/



/*! ************************************

	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit BMP file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

void Burger::FileBMP::SetPalette(const RGBWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
	for writing to an 8 bit BMP file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

void Burger::FileBMP::SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
