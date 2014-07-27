/***************************************

	LBM File handler class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilelbm.h"
#include "brdebug.h"

/*! ************************************

	\class Burger::FileLBM
	\brief Reads and writes LBM files

	This class will read and write out LBM files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a LBM file (Big Endian, 8 bytes)
//

#if !defined(DOXYGEN)

#define FORMASCII 0x464F524D		// FORM
#define ILBMASCII 0x494C424D		// ILBM
#define PBMASCII 0x50424D20			// PBM 
#define BMHDASCII 0x424D4844		// BMHD
#define CMAPASCII 0x434D4150		// CMAP
#define BODYASCII 0x424F4459		// BODY

struct MasterHeader {	// Beginning of the file
	char m_ID[4];		// 'FORM'
	Word32 m_uLength;	// Size of the file - 8
	char m_TypeName[4];	// 'ILBM' or 'PBM '
};

struct ILBMHeader {		// BMHD record
	char m_ID[4];		// BMHD
	Word32 m_uLength;	// Size of the record
	Word16 m_uWidth;	// Size of the final shape
	Word16 m_uHeight;
	Word32 Dat1;		// Reserved
	Word8 m_bPlanes;	// Bits per pixel (8)
	Word8 Dat2;			// 0
	Word8 Dat3;			// 1
	Word8 Dat4;			// 0
	Word16 Dat5;		// 0,0
	Word8 Dat6;			// 1
	Word8 Dat7;			// 1
	Word16 Width2;		// Another copy of the size
	Word16 Height2;
};

struct ILBMPalette {		// CMAP record
	char m_ID[4];			// CMAP
	Word32 m_uLength;		// Size of the record
	Word8 Palette[768];		// RGB Triplets
};

struct ILBMBody {		// BODY record
	char m_ID[4];		// BODY
	Word32 m_uLength;	// Size of the record
	Word8 Data[1];		// Raw packed data
};

#endif

/*! ************************************

	\brief Unpack data using ILBM compression

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileLBM::UnpackILBMData(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	if (uOutputLength) {
		do {
			Word uTemp = pInput->GetByte();		// Get the run token
			if (uTemp & 0x80U) {				// Run length?
				uTemp = 0x101U-uTemp;			// Count the run
				if (uOutputLength<uTemp) {		// Too large?
					break;
				}
				uOutputLength = uOutputLength-uTemp;	// Remove from count
				Word8 uFill = pInput->GetByte();
				do {
					pOutput[0] = uFill;		// Perform the fill
					++pOutput;
				} while (--uTemp);
			} else {
				++uTemp;						// +1 to the count
				if (uOutputLength<uTemp) {		// Too large?
					break;
				}
				uOutputLength = uOutputLength-uTemp;	// Remove from count
				pInput->Get(pOutput,uTemp);
				pOutput+=uTemp;
			}
		} while (uOutputLength);	// More?
		if (uOutputLength) {
			return "Data overrun in packed ILBM data.";
		}
	}
	return NULL;		// Return the current data pointer
}

/*! ************************************

	\brief Unpack bit planed data
	
	Supports 8 and 24 bit data

	\param pOutput Buffer to accept the decompressed data
	\param uWidth Width of the image in pixels
	\param uHeight Height of the image in pixels
	\param uDepth Depth of the image in bits
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileLBM::UnpackILBM(Word8 *pOutput,Word uWidth,Word uHeight,Word uDepth,InputMemoryStream *pInput)
{
	Word uBytesPerPixel = (uDepth+7U)>>3U;
	Word8 *pTempLineBuffer = static_cast<Word8 *>(Alloc((uWidth+16)*uBytesPerPixel));	// Buffer to deinterleave memory
	if (!pTempLineBuffer) {			// Error (How!!)
		return "Out of memory.";
	}
	Word uPlaneStep = ((uWidth+15U)&(~15U))>>3U;		// Number of bytes per plane (Padded to short)
	uWidth = uWidth*uBytesPerPixel;
	const char *pBadNews = NULL;
	do {
		// Decompress all of the bit planes (4, 8 or 24)
		pBadNews = UnpackILBMData(pTempLineBuffer,uPlaneStep*uDepth,pInput);
		if (pBadNews) {		// Error?
			break;			// Exit
		}

		// Merge the planes
		MemoryClear(pOutput,uWidth);	// Clear out the old line
		Word uPlaneIndex = 0; 			// Start at the first bit plane
		Word uPlaneMask = 1;			// Init dest mask
		do {
			Word j = uPlaneIndex>>3U;			// Init dest index
			Word i = uPlaneStep*uPlaneIndex;
			Word uBitMask = 0;
			Word uInput = 0;
			do {
				if (!uBitMask) {		// Mask needs to be set?
					uInput = pTempLineBuffer[i];	// Get a source byte
					++i;
					uBitMask = 0x80U;	// Reset the mask
				}
				if (uBitMask&uInput) {	// Shall I convert it?
					pOutput[j] |= static_cast<Word8>(uPlaneMask);	// Save it
				}
				uBitMask>>=1U;		// Adjust the mask
				j+=uBytesPerPixel;
			} while(j<uWidth);	// At the end?
			uPlaneMask = uPlaneMask<<1;
			if (uPlaneMask>=256) {	// If 24 bit, then I need to loop the dest mask
				uPlaneMask = 1;
			}
		} while (++uPlaneIndex<uDepth);	// All 8 bit planes done?
		pOutput=pOutput+uWidth;			// Next line down please
	} while (--uHeight);

	Free(pTempLineBuffer);		// Release the line buffer
	return pBadNews;			// Return error code if any
}


/*! ************************************

	\brief Scan an AIFF file in memory and return pointer to a chunk
	\param pInput Data stream to read compressed data from
	\param uID IFF token to scan for
	\param uStartOffset Offset into the stream where the IFF file begins
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileLBM::SeekIffChunk(InputMemoryStream *pInput,Word32 uID,WordPtr uStartOffset)
{
	pInput->SetMark(uStartOffset);
	// Only run if there's enough data to scan
	while (pInput->BytesRemaining()>=8) {
		Word32 uTest = pInput->GetBigWord32();
		if (uTest==uID) {
			return NULL;
		}
		Word32 uLength = (pInput->GetBigWord32()+1U)&(~1U);	// Align to short
		if (!uLength) {
			break;
		}
		pInput->SkipForward(uLength);
	}
	return "IFF Chunk was not found";
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileLBM::FileLBM()
{
}

/*! ************************************

	\brief Parses a LBM file

	Read in a LBM file and set the extra variables

	Will parse 24 bit uncompressed and 8 bit compressed and
	uncompressed LBM files only. Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a LBM file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FileLBM::Load(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	//Word32 uFileLength = 0;
	Word FormType = FALSE;
	Word uWidth = 0;
	Word uHeight = 0;
	Word uDepth = 0;
	WordPtr uStartOffset = 0;
	Word32 uID = pInput->GetBigWord32();
	if (uID!=FORMASCII) {
		pBadNews = "No FORM record (Not an LBM or PBM File).";
	} else {
		/* uFileLength = */ pInput->GetBigWord32();
		Word32 uFileID = pInput->GetBigWord32();
		if (uFileID != ILBMASCII) {
			if (uFileID != PBMASCII) {
				pBadNews = "Not a supported IFF file.";
			} else {
				FormType = TRUE;
			}
		}
	}
	
	// Identified as ILBM or PBM?

	if (!pBadNews) {
		// Scan the IFF file from here
		uStartOffset = pInput->GetMark();
		pBadNews = SeekIffChunk(pInput,BMHDASCII,uStartOffset); // Find the common chunk
		if (!pBadNews) {
			pInput->SkipForward(4);
			uWidth = pInput->GetBigShort();
			uHeight = pInput->GetBigShort();
			pInput->SkipForward(4);
			uDepth = pInput->GetByte();

			// Sanity check
			if (!uWidth) {
				pBadNews = "Width can't be zero.";
			} else if (!uHeight) {
				pBadNews = "Height can't be zero.";
			} else if (!uDepth) {
				pBadNews = "Can't process anything but 8 or 24 bit color images.";
			} else if (uDepth > 9) {
				if (uDepth!=24) {
					pBadNews = "Can't process anything but 8 or 24 bit color images.";		// Not 8 or 24 bit!
				} else if (FormType) {
					pBadNews = "Can't process a 24 bit color PBM file.";		// I don't support 24 bit PBM files
				}
			}
		}
	}

	// Does it need a palette?

	if (!pBadNews) {
		if (uDepth<9) {		// Get the palette
			pBadNews = SeekIffChunk(pInput,CMAPASCII,uStartOffset); // Read in the palette
			if (!pBadNews) {
				MemoryClear(m_Palette,sizeof(m_Palette));
				Word32 uPaletteSize = pInput->GetBigWord32()/3U;
				if (uPaletteSize>256) {
					uPaletteSize = 256;
				}
				RGBAWord8_t *pWork = m_Palette;
				do {
					pWork->m_uRed = pInput->GetByte();		// Read in the palette
					pWork->m_uGreen = pInput->GetByte();
					pWork->m_uBlue = pInput->GetByte();
					pWork->m_uAlpha = 0;
					++pWork;
				} while (--uPaletteSize);
			}
		}
	}

	//
	// Read in the pixel data
	//

	if (!pBadNews) {
		pBadNews = SeekIffChunk(pInput,BODYASCII,uStartOffset); // Find the image
		if (!pBadNews) {
			pInput->SkipForward(4);
			if (FormType) {
				pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
				if (pImage) {
					pBadNews = UnpackILBMData(pImage->GetImage(),(Word32)uWidth*uHeight,pInput);
				}
			} else {
				if (uDepth==24) {
					pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE888);
				} else {
					pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
				}
				if (pImage) {
					pBadNews = UnpackILBM(pImage->GetImage(),uWidth,uHeight,uDepth,pInput);
				}
			}
		}
	}
	// If there was an error, clean up
	if (pBadNews) {
		Debug::Warning(pBadNews);
		Delete(pImage);
		pImage = NULL;
	}
	return pImage;
}

/*! ************************************

	\fn const RGBAWord8_t *Burger::FileLBM::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the LBM file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileLBM::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FileLBM::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the LBM file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileLBM::GetPalette(void) const

***************************************/



/*! ************************************

	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit LBM file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

void Burger::FileLBM::SetPalette(const RGBWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
	for writing to an 8 bit LBM file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

void Burger::FileLBM::SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
