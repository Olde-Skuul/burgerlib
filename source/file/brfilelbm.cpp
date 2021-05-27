/***************************************

    LBM File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilelbm.h"
#include "brdebug.h"
#include "brendian.h"

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
	uint32_t m_uLength;	// Size of the file - 8
	char m_TypeName[4];	// 'ILBM' or 'PBM '
};

struct ILBMHeader {		// BMHD record
	char m_ID[4];		// BMHD
	uint32_t m_uLength;	// Size of the record
	uint16_t m_uWidth;	// Size of the final shape
	uint16_t m_uHeight;
	uint32_t Dat1;		// Reserved
	uint8_t m_bPlanes;	// Bits per pixel (8)
	uint8_t Dat2;			// 0
	uint8_t Dat3;			// 1
	uint8_t Dat4;			// 0
	uint16_t Dat5;		// 0,0
	uint8_t Dat6;			// 1
	uint8_t Dat7;			// 1
	uint16_t Width2;		// Another copy of the size
	uint16_t Height2;
};

struct ILBMPalette {		// CMAP record
	char m_ID[4];			// CMAP
	uint32_t m_uLength;		// Size of the record
	uint8_t Palette[768];		// RGB Triplets
};

struct ILBMBody {		// BODY record
	char m_ID[4];		// BODY
	uint32_t m_uLength;	// Size of the record
	uint8_t Data[1];		// Raw packed data
};

#endif

/*! ************************************

	\brief Unpack data using ILBM compression

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileLBM::UnpackILBMData(uint8_t *pOutput,uintptr_t uOutputLength,InputMemoryStream *pInput)
{
	if (uOutputLength) {
		do {
			uint_t uTemp = pInput->GetByte();		// Get the run token
			if (uTemp & 0x80U) {				// Run length?
				uTemp = 0x101U-uTemp;			// Count the run
				if (uOutputLength<uTemp) {		// Too large?
					break;
				}
				uOutputLength = uOutputLength-uTemp;	// Remove from count
				uint8_t uFill = pInput->GetByte();
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

const char * BURGER_API Burger::FileLBM::UnpackILBM(uint8_t *pOutput,uint_t uWidth,uint_t uHeight,uint_t uDepth,InputMemoryStream *pInput)
{
	uint_t uBytesPerPixel = (uDepth+7U)>>3U;
	uint8_t *pTempLineBuffer = static_cast<uint8_t *>(Alloc((uWidth+16)*uBytesPerPixel));	// Buffer to deinterleave memory
	if (!pTempLineBuffer) {			// Error (How!!)
		return "Out of memory.";
	}
	uint_t uPlaneStep = ((uWidth+15U)&(~15U))>>3U;		// Number of bytes per plane (Padded to short)
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
		uint_t uPlaneIndex = 0;			// Start at the first bit plane
		uint_t uPlaneMask = 1;			// Init dest mask
		do {
			uint_t j = uPlaneIndex>>3U;			// Init dest index
			uint_t i = uPlaneStep*uPlaneIndex;
			uint_t uBitMask = 0;
			uint_t uInput = 0;
			do {
				if (!uBitMask) {		// Mask needs to be set?
					uInput = pTempLineBuffer[i];	// Get a source byte
					++i;
					uBitMask = 0x80U;	// Reset the mask
				}
				if (uBitMask&uInput) {	// Shall I convert it?
					pOutput[j] |= static_cast<uint8_t>(uPlaneMask);	// Save it
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

const char * BURGER_API Burger::FileLBM::SeekIffChunk(InputMemoryStream *pInput,uint32_t uID,uintptr_t uStartOffset)
{
	pInput->SetMark(uStartOffset);
	// Only run if there's enough data to scan
	while (pInput->BytesRemaining()>=8) {
		uint32_t uTest = pInput->GetBigWord32();
		if (uTest==uID) {
			return NULL;
		}
		uint32_t uLength = (pInput->GetBigWord32()+1U)&(~1U);	// Align to short
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
	//uint32_t uFileLength = 0;
	uint_t FormType = FALSE;
	uint_t uWidth = 0;
	uint_t uHeight = 0;
	uint_t uDepth = 0;
	uintptr_t uStartOffset = 0;
	uint32_t uID = pInput->GetBigWord32();
	if (uID!=FORMASCII) {
		pBadNews = "No FORM record (Not an LBM or PBM File).";
	} else {
		/* uFileLength = */ pInput->GetBigWord32();
		uint32_t uFileID = pInput->GetBigWord32();
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
		pBadNews = SeekIffChunk(pInput,BMHDASCII,uStartOffset);	// Find the common chunk
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
			pBadNews = SeekIffChunk(pInput,CMAPASCII,uStartOffset);	// Read in the palette
			if (!pBadNews) {
				MemoryClear(m_Palette,sizeof(m_Palette));
				uint32_t uPaletteSize = pInput->GetBigWord32()/3U;
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
		pBadNews = SeekIffChunk(pInput,BODYASCII,uStartOffset);	// Find the image
		if (!pBadNews) {
			pInput->SkipForward(4);
			if (FormType) {
				pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
				if (pImage) {
					pBadNews = UnpackILBMData(pImage->GetImage(),(uint32_t)uWidth*uHeight,pInput);
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

	\fn void Burger::FileLBM::SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit LBM file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,uint_t,uint_t)

***************************************/

/*! ************************************

	\fn void Burger::FileLBM::SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
	\brief Set the file image's palette (RGBA)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit LBM file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,uint_t,uint_t)

***************************************/

/*! ************************************

	\brief Scan an AIFF file for a specific chunk

	Scan an AIFF file in memory and return pointer to a chunk
	or \ref NULL if the chunk was not found

	\param pInput Pointer to the AIFF file in memory
	\param uLength Length of the AIFF file in memory
	\param uChunkName 4 byte code of the chunk to find in ASCII format 'CODE' = 0x434F4445U big endian or 0x45444F43U on little endian machines
	\return Pointer to the start of the chunk located, or \ref NULL if not found

***************************************/

const void * BURGER_API Burger::FindAIFFChunk(const void *pInput,uintptr_t uLength,uint32_t uChunkName)
{
	// Not enough data?
	const void *pResult = NULL;
	if (uLength>=(12+8)) {
		uintptr_t uSkip = 12;		// Initial skip
		do {
			// Remove processed bytes
			uLength-=uSkip;
			pInput = static_cast<const uint8_t *>(pInput)+uSkip;
			if (BigEndian::LoadAny(static_cast<const uint32_t *>(pInput)) == uChunkName) {	// Match?
				pResult = pInput;
				break;
			}
			uSkip = BigEndian::LoadAny(&static_cast<const uint32_t *>(pInput)[1]);
			uSkip = (uSkip+8U+1U)&(~1U);	// Align to short
		} while (uSkip<uLength);
	}
	// Return NULL or the chunk
	return pResult;
}

/*! ************************************

	\brief Scan a RIFF file for a specific chunk

	Scan a RIFF file in memory and return pointer to a chunk
	or \ref NULL if the chunk was not found

	\param pInput Pointer to the WAV/RIFF file in memory
	\param uLength Length of the WAV/RIFF file in memory
	\param uChunkName 4 byte code of the chunk to find in ASCII format 'CODE' = 0x434F4445U native endian
	\return Pointer to the start of the chunk located, or \ref NULL if not found

***************************************/

const void * BURGER_API Burger::FindRIFFChunk(const void *pInput,uintptr_t uLength,uint32_t uChunkName)
{
	// Not enough data?
	const void *pResult = NULL;
	if (uLength>=(12+8)) {
		uintptr_t uSkip = 12;		// Initial skip
		do {
			// Remove processed bytes
			uLength-=uSkip;
			pInput = static_cast<const uint8_t *>(pInput)+uSkip;
			if (BigEndian::LoadAny(static_cast<const uint32_t *>(pInput)) == uChunkName) {	// Match?
				pResult = pInput;
				break;
			}
			uSkip = LittleEndian::LoadAny(&static_cast<const uint32_t *>(pInput)[1]);
			uSkip = (uSkip+8U+1U)&(~1U);	// Align to short
		} while (uSkip<uLength);
	}
	// Return NULL or the chunk
	return pResult;
}
