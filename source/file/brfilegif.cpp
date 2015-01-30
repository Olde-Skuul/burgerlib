/***************************************

	GIF File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilegif.h"
#include "brdebug.h"

/*! ************************************

	\class Burger::FileGIF
	\brief Reads and writes GIF files

	This class will read and write out GIF files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a GIF file (Little Endian, 13 bytes)
//

#if !defined(DOXYGEN)
struct GIFHeader {
	char m_GIFName[6];		// GIF87a in ASCII form or GIF89a
	Word16 m_uWidth;		// Width of the image in pixels
	Word16 m_uHeight;		// Height of the image in pixels
	Word8 m_bDescriptor;	// Bits per pixel descriptor
	Word8 m_bBackColor;		// Background color
	Word8 m_bAspectRatio;	// Aspect ratio / PixelAspectRatio = (m_bAspectRatio + 15) / 64;
};

struct GIFImageDescriptor {
	Word8 m_bSeparator;	// Image Descriptor identifier
	Word16 m_uLeft;		// X position of image on the display
	Word16 m_uTop;		// Y position of image on the display
	Word16 m_uWidth;	// Width of the image in pixels
	Word16 m_uHeight;	// Height of the image in pixels
	Word8 m_bPacked;	// Image and Color Table Data Information
};

#endif

/***************************************

	These variables are used by the LZW decompresser

***************************************/

static const Word MaskTable[] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1FFF};

/*! ************************************

	\struct Burger::FileGIF::GIFDecoder
	\brief GIF style LZW decompresser

	This class will decompress LZW data in the GIF format

***************************************/

/*! ************************************

	\brief Init the LZW permanent tokens

***************************************/

void Burger::FileGIF::GIFDecoder::Init(void)
{
	m_uBitCount = 0;
	m_uChunkCount = 0;
	Word i = 0;
	Word8 *pDest = m_LZWArray;
	do {
		pDest[0] = static_cast<Word8>(i);	// 0-255
		m_LZWSize[i] = 1;					// All are 1 byte in size
		m_LZWTable[i] = pDest;				// Init pointers to LZWArray table
		++pDest;
	} while (++i<256);
}

/*! ************************************

	\brief Reset the LZW token tree

***************************************/

void Burger::FileGIF::GIFDecoder::Reset(void)
{
	Word i = 256;				// Generated tokens
	do {
		m_LZWSize[i] = 0;		// Nothing here!
		m_LZWTable[i] = NULL;	// No pointers
	} while (++i<4096);			// All of them
}

/*! ************************************

	\brief Get an LZW code token

	\param uBitsRequested Number of bits to pull from the byte stream
	\return Value pulled from the bit stream (Zero if data is depleted)

***************************************/

Word Burger::FileGIF::GIFDecoder::GetCode(Word uBitsRequested)
{
	Word uMask = 1;		// Bit or mask
	Word uResult = 0;	// Init returned value
	Word uBitBucket = m_uBitBucket;		// Get the bit bucket
	Word uBitCount = m_uBitCount;		// Get the valid bit count
	do {
		if (!uBitCount) {					// Empty?
			uBitCount = m_uChunkCount;		// Any more fresh data?
			if (!uBitCount) {				// No more!
				uBitCount = m_pInput->GetByte();	// Get the packet count
				if (!uBitCount) {		// If zero then keep returning zeros
					m_pInput->SkipBack(1);		// Revert the fetch
					uResult = 0;	// Exit now
					break;
				}
			}
			--uBitCount;				// I accept a byte
			m_uChunkCount = uBitCount;	// Save result
			uBitBucket = m_pInput->GetByte();	// Get next byte
			uBitCount = 8;				// 8 valid bits!
		}
		--uBitCount;					// Accept a bit
		if (uBitBucket&1) {				// Return true or false
			uResult |= uMask;		// Blend...
		}
		uBitBucket = uBitBucket>>1U;	// Shift out a bit
		uMask <<= 1U;				// Shift up the mask
	} while (--uBitsRequested);		// More?
	m_uBitCount=uBitCount;			// Save new count
	m_uBitBucket=uBitBucket;		// Save new bit bucket
	return uResult;					// Return the code
}

/*! ************************************

	\brief Decompress the image

	Decompress the LZW compressed image data into a linear buffer

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * BURGER_API Burger::FileGIF::UnpackGIFLZW(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	GIFDecoder *pGIFDecoder = static_cast<GIFDecoder *>(AllocClear(sizeof(GIFDecoder)));
	if (!pGIFDecoder) {
		return "Out of memory.";
	}
	pGIFDecoder->m_uLZWCodeSize = pInput->GetByte();	// Number of bits for code size
	pGIFDecoder->m_pInput = pInput;
	pGIFDecoder->Init();

	Word uCount;				// Temp index counter
	const Word8 *pPreviousMark=NULL;	// Pointer from previous loop
	Word uPreviousCode = static_cast<Word>(-1);
	Word uNewCode = 256+2;		// Newest token being generated
	Word uCodeBitSize = pGIFDecoder->m_uLZWCodeSize+1U;
	Word uCodeMask = MaskTable[uCodeBitSize];		// NewCode number to match to increase token bit size
	do {
		Word uLZWCode = pGIFDecoder->GetCode(uCodeBitSize);		// Get LZW token
		if (uLZWCode == 257) {			// End token?
			break;
		}
		
		if (uLZWCode==256) {		// Reset token?
			pGIFDecoder->Reset();	// Reset the dictionary
			uNewCode = 256+2;		// Reset next new token
			uCodeBitSize = pGIFDecoder->m_uLZWCodeSize+1;	// 9 bits again
			uCodeMask = MaskTable[uCodeBitSize];	// Reset the mask
			uPreviousCode = static_cast<Word>(-1);
			pPreviousMark = pOutput;
			continue;
		}

		const Word8 *pPreviousOutput = pOutput;
		const Word8 *pTable = pGIFDecoder->m_LZWTable[uLZWCode];	// Get the string pointer
		if (pTable) {					// Valid pointer?
			uCount = pGIFDecoder->m_LZWSize[uLZWCode];		// Size of the string
			uOutputLength -= uCount;						// Remove from failsafe
			do {
				pOutput[0] = pTable[0];	// Copy the string
				++pOutput;
				++pTable;
			} while (--uCount);
			if (uPreviousCode==static_cast<Word>(-1)) {	// First pass?
				// Then don't make a new record
				uPreviousCode = uLZWCode;			// Set the new previous index
				pPreviousMark = pPreviousOutput;	// Pointer to this work data
				continue;
			}
		} else {
			uCount = pGIFDecoder->m_LZWSize[uPreviousCode]+1;	// Get the previous match
			uOutputLength -= uCount;		// Remove from length
			pTable = pPreviousMark;			// Set the pointer
			do {
				pOutput[0] = pTable[0];		// Copy the string
				++pOutput;
				++pTable;
			} while (--uCount);
		}
		pGIFDecoder->m_LZWSize[uNewCode] = pGIFDecoder->m_LZWSize[uPreviousCode]+1;	// Set the new length
		pGIFDecoder->m_LZWTable[uNewCode] = pPreviousMark;		// Data pointer
		++uNewCode;		// Next new LZW code

		if (uNewCode==uCodeMask) {		// Do I need another bit next pass?
			++uCodeBitSize;				// Increase the bit size
			uCodeMask = MaskTable[uCodeBitSize];	// Use table
		}
		uPreviousCode = uLZWCode;				// Set the new previous index
		pPreviousMark = pPreviousOutput;		// Pointer to this work data
	} while (uOutputLength);

	// Exit
	Free(pGIFDecoder);		// Release the temp memory
 	if (!uOutputLength) {	// Check if properly decompressed
		return NULL;		// Good!
	}
	return "Error in GIF decompression.";
}

/*! ************************************

	\brief Skip over a GIF extension block
	\param pInput Pointer to the input stream

***************************************/

void BURGER_API Burger::FileGIF::DiscardExtension(InputMemoryStream *pInput)
{
	WordPtr uCount = 2-1;		// Index past the '!' and opcode
	do {
		pInput->SkipForward(uCount);	// Move the pointer
		uCount = pInput->GetByte();	// Get the data count
	} while (uCount);		// Still more data?
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileGIF::FileGIF() :
	m_bAspectRatio(0)		/// No aspect ratio (Default)
{
}

/*! ************************************

	\brief Parses a GIF file

	Read in a GIF file and set the extra variables

	Will parse an 8 bit compressed and uncompressed GIF files.

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a GIF file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FileGIF::Load(InputMemoryStream *pInput)
{
	char ID[6];
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	Word uWidth = 0;
	Word uHeight = 0;
//	Word bGIF89 = FALSE;

	// Start with processing the 13 byte header of the GIF file

	if (pInput->BytesRemaining()<13) {
		pBadNews = "Insufficient data for GIF file header.";
	} else {

		// Read the 13 byte header

		pInput->Get(ID,6);
		uWidth = pInput->GetShort();
		uHeight = pInput->GetShort();
		Word uDescriptor = pInput->GetByte();
/*		Word uBackgroundColor = */ pInput->GetByte();
		m_bAspectRatio = pInput->GetByte();

		if (MemoryCompare(ID,"GIF87a",6)) {	// Header ok?
			if (MemoryCompare(ID,"GIF89a",6)) {
				pBadNews = "Invalid GIF 87/89 ASCII header.";
			} else {
//				bGIF89 = TRUE;
			}
		}
		if (!pBadNews) {
			if ((uDescriptor&7)!=7) {
				pBadNews = "GIF files must be 8 bits per pixel!";
			} else if (!(uDescriptor&0x80)) {		// Is there a palette?
				pBadNews = "No global palette in GIF file.";
			} else {

				// Fetch the initial palette

//				Word uColorCount = uDescriptor>>4U;		// Isolate the upper 4 bits
//				uColorCount = uColorCount&7U;		// Get the bits per pixel
//				uColorCount = 2U<<uColorCount;		// Convert to a color count
				Word uColorCount = 256;					// For malformed GIF files

				if (pInput->BytesRemaining()<(uColorCount*3)) {			// No initial palette?
					pBadNews = "Insufficient data for GIF palette.";
				} else {
					// uColorCount is 2,4,8,...,256
					// Initialize the palette

//					MemoryClear(m_Palette,sizeof(m_Palette));
					RGBAWord8_t *pWork = m_Palette;
//					WordPtr uSkip = 256-uColorCount;
					do {
						pWork->m_uRed = pInput->GetByte();		// Read in the palette
						pWork->m_uGreen = pInput->GetByte();
						pWork->m_uBlue = pInput->GetByte();
						pWork->m_uAlpha = 0;
						++pWork;
					} while (--uColorCount);
//					pInput->SkipForward(uSkip*3);

					Word uToken = pInput->GetByte();
					while (uToken=='!') {			// Is this an extension block?
						DiscardExtension(pInput);	// Discard it
						uToken = pInput->GetByte();
					}

					if (uToken!=',') {			// Data block?
						pBadNews = "Unknown data block found instead of bitmap";
					}
				}
			}
		}
	}

	//
	// Get the image descriptor
	//

	if (!pBadNews) {
		Word uOriginX = pInput->GetShort();
		Word uOriginY = pInput->GetShort();
		if (uOriginX|uOriginY) {
			pBadNews = "Origin of GIF image is not zero.";
		} else {
			uOriginX = pInput->GetShort();
			uOriginY = pInput->GetShort();
			if ((uOriginX!=uWidth) || (uOriginY!=uHeight)) {
				pBadNews = "Image parsed is not the same size as total image.";
			} else {
				// New palette information
				pInput->GetByte();
			}
		}
	}

	if (!pBadNews) {
		pImage = Image::New(uWidth,uHeight,Image::PIXELTYPE8BIT);
		if (pImage) {
			pBadNews = UnpackGIFLZW(pImage->GetImage(),uWidth*uHeight,pInput);	// Decompress it
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

	\fn Word Burger::FileGIF::GetAspectRatio(void) const
	\brief Get the file image's pixel aspect ratio

	Return the GIF file header's aspect ratio

	\note This value is usually zero, meaning it's not used. If it's non-zero
	then it's recommended to apply this formula to get a useful value. (GetAspectRatio() + 15) / 64;

	\return The GIF formatted aspect ratio token
	\sa Burger::FileGIF::SetAspectRatio(Word)

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetAspectRatio(Word bAspectRatio)
	\brief Set the file image's aspect ratio

	Sets the GIF file header's aspect ratio
	
	\param bAspectRatio New GIF formatted aspect ratio token
	\sa Burger::FileGIF::GetAspectRatio(void) const

***************************************/



/*! ************************************

	\fn const RGBAWord8_t *Burger::FileGIF::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the GIF file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileGIF::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FileGIF::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the GIF file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileGIF::GetPalette(void) const

***************************************/



/*! ************************************

	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit GIF file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

void Burger::FileGIF::SetPalette(const RGBWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
	for writing to an 8 bit GIF file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

void Burger::FileGIF::SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
