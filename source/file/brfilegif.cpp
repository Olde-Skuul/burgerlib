/***************************************

	GIF File handler class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilegif.h"
#include "brdebug.h"
#include "brglobalmemorymanager.h"

/***************************************

	The structures below are for reference only

	Since they are all byte aligned, their use is
	discouraged for use due to some compilers forcing
	16 bit alignment for some members causing alterations
	in the desired sizes.

	As per the GIF specification, assume all data is
	in Little Endian format

***************************************/

#if !defined(DOXYGEN)
//#define DEBUG_READER

//
// Header of a GIF file (Little Endian, 13 bytes)
//

struct GIFHeader_t {
	char m_GIFName[6];		// GIF87a in ASCII form or GIF89a
	Word16 m_uWidth;		// Width of the image in pixels
	Word16 m_uHeight;		// Height of the image in pixels
	Word8 m_bDescriptor;	// Bits per pixel descriptor
	Word8 m_bBackColor;		// Background color
	Word8 m_bAspectRatio;	// Aspect ratio / PixelAspectRatio = (m_bAspectRatio + 15) / 64;
};

//
// Description record for an image
//

struct GIFImageDescriptor_t {
	Word8 m_bSeparator;	// Image Descriptor identifier ','
	Word16 m_uLeft;		// X position of image on the display
	Word16 m_uTop;		// Y position of image on the display
	Word16 m_uWidth;	// Width of the image in pixels
	Word16 m_uHeight;	// Height of the image in pixels
	Word8 m_bPacked;	// Image and Color Table Data Information
	Burger::RGBWord8_t m_Palette[256];	// Optional palette that's a power of 2 entries in size
};

//
// End of data block
//

struct GIFEnd_t {
	Word8 m_bSeperator;		// ';'
};


struct GIFExtension_t {
	Word8 m_bSeperator;		// '!'
	Word8 m_bExtensionCode;	// Extension code
	Word8 m_bBlockSize;		// Number of bytes in this block
};

//
// Extension Code 249, Blocksize 4
//
struct GIFAnimationTimer_t {
	Word8 m_bSeperator;			// '!'
	Word8 m_bExtensionCode;		// Extension code 0xF7
	Word8 m_bBlockSize;			// Number of bytes in this block (4)
	Word8 m_Packed;				// Bits 0x1 = Use transparency, 0x02 = User, 0x1C = Disposal
	Word16 m_uDelay;			// Delay time for animation in 1/100ths of a second
	Word8 m_bTransparentIndex;	// Color index to not draw
	Word8 m_bBlockTerminator;	// 0
};

//
// Extension Code 255, Blocksize 11
//

struct GIFNetscape_t {
	Word8 m_bSeperator;			// '!'
	Word8 m_bExtensionCode;		// Extension code (0xFF)
	Word8 m_bBlockSize;			// Number of bytes in this block (11)
	char m_Netscape[8];			// "NETSCAPE"
	char m_Version[3];			// "2.0"
	Word8 m_bSubBlockSize;		// 3
	Word8 m_bSubBlockID;		// 1
	Word16 m_uLoopCount;		// Number of iterations to play the animation (0 = forever)
	Word8 m_bBlockTerminator;	// 0
};

#endif


/*! ************************************

	\class Burger::GIFEncoder
	\brief GIF style LZW compressor

	This class will compress pixel data into using GIF LZW format

	\sa FileGIF or GIFDecoder

***************************************/

/*! ************************************

	\brief Init the encoder to an inert state

***************************************/

Burger::GIFEncoder::GIFEncoder() :
	m_pOutput(NULL),
	m_uColorDepth(8U),
	m_uRunningCode((1U << 8U) + 2U),
	m_uRunningBits(8U+1U),
	m_uMaximumRunningCode(1U << (8U+1U)),
	m_uCode(cStartingCode),
	m_uShiftState(0),
	m_uShiftData(0),
	m_eEncodingState(ENCODE_FINAL),
	m_uBytesInBuffer(0)
{
}

/*! ************************************

	\brief Allocate and initialize a GIFEncoder

	\return A pointer to a default GIFEncoder class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::GIFEncoder * BURGER_API Burger::GIFEncoder::New(void)
{
	// Allocate the memory
	GIFEncoder *pThis = new (Alloc(sizeof(GIFEncoder))) GIFEncoder();
	return pThis;
}

/*! ************************************

	\fn Word Burger::GIFEncoder::GetHashKey(Word uInput)
	\brief Create a hash key

	Given a 16 bit value of the current and previous pixel, return a hash
	value to index into the hash table

	\param uInput Value with LZW code shifted left by 8 bits and the pixel in the lower 8 bits
	\return Hash of the key

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::GetKey(Word uInput)
	\brief Return the LZW key from a hash

	\param uInput Hash value
	\return LZW Key

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::PutKey(Word uInput)
	\brief Prepare a LZW key to form a key

	\param uInput LZW token
	\return Token shifted to a position for a key

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::GetCode(Word uInput)
	\brief Return the LZW code from a hash

	\param uInput Hash value
	\return LZW code

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::PutCode(Word uInput)
	\brief Prepare a LZW code to form a key

	\param uInput LZW code
	\return Code shifted to a position for a key

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::GetClearCode(void)
	\brief Return the LZW Clear code

	Based on the bit depth, return the code to clear the LZW cache

	\return LZW clear code

***************************************/

/*! ************************************

	\fn Word Burger::GIFEncoder::GetEOFCode(void)
	\brief Return the LZW End of File code

	Based on the bit depth, return the code to end decompression of LZW data

	\return LZW EOF code

***************************************/	

/*! ************************************

	\brief Given a palette depth, create a GIF LZW encoder

	Call this function before any encoding is performed.

	\param pOutput Pointer to a valid \ref OutputMemoryStream to store the compressed data
	\param uDepth Depth of the pixel data in bits (2-8)

***************************************/

void Burger::GIFEncoder::Init(Burger::OutputMemoryStream *pOutput,Word uDepth)
{
	m_pOutput = pOutput;
	m_uBytesInBuffer = 0;		// No bytes in the cache
	m_eEncodingState = ENCODE_INPROGRESS;	// Allow encoding
	m_uCode = cStartingCode;	// No data encoded yet

	// Enforce a minimum bit depth
	if (uDepth < 2U) {
		uDepth = 2U;
	}

	// Set up the constants
	m_uColorDepth = uDepth;
	m_uRunningCode = (1U << uDepth) + 2U;
	m_uRunningBits = uDepth + 1U;
	m_uMaximumRunningCode = 1U << m_uRunningBits;
	m_uShiftState = 0;
	m_uShiftData = 0;
	ClearHash();

	// Store the initial LZW color depth into the output stream
	pOutput->Append(static_cast<Word8>(uDepth));
	// Alert the LZW stream to issue a hash clear code
	WriteCode(GetClearCode());
}

/*! ************************************

	\brief Erase the GIF LZW hash

***************************************/

void Burger::GIFEncoder::ClearHash(void)
{
	Word *pWork = m_HashTable;
	WordPtr uCounter = BURGER_ARRAYSIZE(m_HashTable);
	do {
		pWork[0] = BURGER_MAXUINT;
		++pWork;
	} while (--uCounter);
}

/*! ************************************

	\brief Given a code, create a hash index
	
	Scan the hash table for a LZW token or return 
	\ref BURGER_MAXUINT if there is no matching entry.

	\param uInput Encoded pixel and previous pixel
	\return \ref BURGER_MAXUINT on error or a valid 12 bit hash index

***************************************/

Word Burger::GIFEncoder::LookupHash(Word uInput)
{
	// Get the initial hash value
	Word uHashKey = GetHashKey(uInput);
	Word uResult;
	for (;;) {
		// Is this a valid entry?
		Word uKey = GetKey(m_HashTable[uHashKey]);
		if (uKey == (BURGER_MAXUINT>>cLZBits)) {
			// Not in the hash
			uResult = BURGER_MAXUINT;	
			break;
		}
		// Match?
		if (uInput == uKey) {
			// Return the code found in the hash
			uResult = GetCode(m_HashTable[uHashKey]);
			break;
		}
		// Index to the next hash entry (With table wrap)
		uHashKey = (++uHashKey) & cHashKeyMask;
	}
	return uResult;
}

/*! ************************************

	\brief Insert a new code into the hash

	Given a hash key, look for an empty entry in the hash and insert
	the LZW token into it.

	\param uKey Hash key (Index into the hash)
	\param uCode LZW code

***************************************/

void Burger::GIFEncoder::AddHashEntry(Word uKey,Word uCode)
{
	Word uHashKey = GetHashKey(uKey);

	while (GetKey(m_HashTable[uHashKey]) != (BURGER_MAXUINT>>cLZBits)) {
		// Keep going while wrapping around
		uHashKey = (++uHashKey) & cHashKeyMask;
	}
	m_HashTable[uHashKey] = PutKey(uKey) | PutCode(uCode);
}

/*! ************************************
	
	\brief Write a byte of data to the GIF byte stream

	Cache a byte of output data and send it out as a
	"pascal" string with a length followed by a stream of 
	data. Only write to the stream when 255 bytes have
	been accumulated

	\param uInput 8 bit data to write to the byte stream

***************************************/

void Burger::GIFEncoder::WriteByte(Word uInput)
{
	// Is the buffer already full?
	Word uIndex = m_uBytesInBuffer;
	if (uIndex == 255U) {
		// Write the length
		m_pOutput->Append(static_cast<Word8>(uIndex));
		// Write the data
		m_pOutput->Append(m_Buffer,uIndex);
		// Clear the buffer
		uIndex = 0;
	}
	m_Buffer[uIndex] = static_cast<Word8>(uInput);
	++uIndex;
	m_uBytesInBuffer = uIndex;
}

/*! ************************************

	\brief Given an LZW code, encode it into the bit stream
	
	Write out an LZW code using bit packing.

	\param uCode Bit pattern for LZW code

***************************************/

void Burger::GIFEncoder::WriteCode(Word uCode)
{
	// Insert bits into the stream
	m_uShiftData |= (uCode << m_uShiftState);
	m_uShiftState += m_uRunningBits;

	// Is the bit stream overflowing?
	while (m_uShiftState >= 8U) {
		WriteByte(m_uShiftData);
		m_uShiftData >>= 8U;
		m_uShiftState -= 8U;
	}

	// Determine if the codes are larger than the shift
	// value can handle, increase the shift

	if ((m_uRunningCode >= m_uMaximumRunningCode) && 
		(uCode <= cLZLastCode)) {
   		m_uMaximumRunningCode = (1U << (++m_uRunningBits));
	}
}

/*! ************************************

	\brief Encoding is complete, flush the buffer

	At the conclusion of compression, call this function to perform 
	cleanup and flush out any cached data

***************************************/

void Burger::GIFEncoder::Flush(void)
{
	if (m_eEncodingState==ENCODE_INPROGRESS) {
		// Write out the last codes
		WriteCode(m_uCode);
		WriteCode(GetEOFCode());

		// Is there any bits in the bit stream?
		while (m_uShiftState >= 8U) {
			// Flush the bit stream
			WriteByte(m_uShiftData);
			m_uShiftData >>= 8U;
			m_uShiftState -= 8U;
		}
		// Last bits?
		if (m_uShiftState) {
			WriteByte(m_uShiftData);
		}
		// Clear out the bit stream
		m_uShiftState = 0;
		m_uShiftData = 0;

		// If there is any data in the byte buffer? 
		if (m_uBytesInBuffer) {
			// Flush the byte stream
			m_pOutput->Append(static_cast<Word8>(m_uBytesInBuffer));
			m_pOutput->Append(m_Buffer,m_uBytesInBuffer);
			m_uBytesInBuffer = 0;
		}

		// Write out a zero to alert LZW there is no more data
		m_pOutput->Append(static_cast<Word8>(0));
		m_eEncodingState = ENCODE_FINAL;
	}
}

/*! ************************************

	\brief Encode a scan line of pixels

	Given a scan line of pixels, compress using LZW. Assume
	the pixels are bytes.

	\param pInput Pointer to an array of pixels for a single scan line
	\param uInputLength Number of pixels to encode

***************************************/

void Burger::GIFEncoder::WritePixels(const Word8 *pInput,WordPtr uInputLength)
{
	if (uInputLength) {
		Word uCurrentCode = m_uCode;
		if (uCurrentCode == cStartingCode) {
			// Initialize the current code value
			uCurrentCode = pInput[0];
			++pInput;
			--uInputLength;
		}

		while (uInputLength) {
			// Grab a pixel
			Word uPixel = pInput[0];
			++pInput;
			--uInputLength;
			// Create a hash key based on the current pixel and the previous one
			Word uKey = (uCurrentCode << 8U) + uPixel;
			Word uHashCode = LookupHash(uKey);
			if (uHashCode != BURGER_MAXUINT) {
				// Key is found!
				uCurrentCode = uHashCode;
			} else {
				// Not found, add it to the hash
				WriteCode(uCurrentCode);
				uCurrentCode = uPixel;

				// Is the hash full?
				if (m_uRunningCode >= cLZLastCode) {
					// Flush the hash and send a cache flush code
					WriteCode(GetClearCode());
					m_uRunningCode = GetEOFCode() + 1U;
					m_uRunningBits = m_uColorDepth + 1U;
					m_uMaximumRunningCode = 1U << m_uRunningBits;
					ClearHash();
				} else {
					// Add this to the hash
					AddHashEntry(uKey,m_uRunningCode);
					++m_uRunningCode;
				}
			}	
		}

		// Save the last code checked
		m_uCode = uCurrentCode;
	}
}


/***************************************

	These variables are used by the LZW decompresser

***************************************/

static const Word g_MaskTable[] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1FFF};

/*! ************************************

	\class Burger::GIFDecoder
	\brief GIF style LZW decompresser

	This class will decompress LZW data in the GIF format

	\sa FileGIF or GIFEncoder

***************************************/

/*! ************************************

	\brief Init the LZW permanent tokens

***************************************/

void Burger::GIFDecoder::Init(void)
{
	m_uBitBucket = 0;
	m_uBitCount = 0;
	m_uChunkCount = 0;
	WordPtr i = 0;
	Word8 *pDest = m_LZWArray;

	// Note, only the first 256 entries of m_LZWSize
	// and m_LZWTable need to be initialized

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

void Burger::GIFDecoder::Reset(void)
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

Word Burger::GIFDecoder::GetCode(Word uBitsRequested)
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
			uResult |= uMask;			// Blend...
		}
		uBitBucket = uBitBucket>>1U;	// Shift out a bit
		uMask <<= 1U;					// Shift up the mask
	} while (--uBitsRequested);			// More?
	m_uBitCount=uBitCount;				// Save new count
	m_uBitBucket=uBitBucket;			// Save new bit bucket
	return uResult;						// Return the code
}

/*! ************************************

	\brief Initialize a GIFDecoder

***************************************/

Burger::GIFDecoder::GIFDecoder() :
	m_pInput(NULL),
	m_uLZWCodeSize(0)
{
//	Init();
	Reset();
}

/*! ************************************

	\brief Allocate and initialize a GIFDecoder

	\return A pointer to a default GIFDecoder class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::GIFDecoder * BURGER_API Burger::GIFDecoder::New(void)
{
	// Allocate the memory
	GIFDecoder *pThis = new (Alloc(sizeof(GIFDecoder))) GIFDecoder();
	return pThis;
}

/*! ************************************

	\brief Decompress the image

	Decompress the LZW compressed image data into a linear buffer

	\param pOutput Buffer to accept the decompressed data
	\param uOutputLength Length of the buffer for decompressed data
	\param pInput Data stream to read compressed data from
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * Burger::GIFDecoder::Unpack(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput)
{
	m_pInput = pInput;
	m_uLZWCodeSize = pInput->GetByte();	// Number of bits for code size
	Init();

	Word uCount;				// Temp index counter
	const Word8 *pPreviousMark=NULL;	// Pointer from previous loop
	Word uPreviousCode = static_cast<Word>(-1);
	Word uNewCode = 256+2;		// Newest token being generated
	Word uCodeBitSize = m_uLZWCodeSize+1U;
	Word uCodeMask = g_MaskTable[uCodeBitSize];		// NewCode number to match to increase token bit size
	do {
		Word uLZWCode = GetCode(uCodeBitSize);		// Get LZW token
		if (uLZWCode == 257) {			// End token?
			break;
		}
		
		if (uLZWCode==256) {		// Reset token?
			Reset();	// Reset the dictionary
			uNewCode = 256+2;		// Reset next new token
			uCodeBitSize = m_uLZWCodeSize+1;	// 9 bits again
			uCodeMask = g_MaskTable[uCodeBitSize];	// Reset the mask
			uPreviousCode = static_cast<Word>(-1);
			pPreviousMark = pOutput;
			continue;
		}

		const Word8 *pPreviousOutput = pOutput;
		const Word8 *pTable = m_LZWTable[uLZWCode];	// Get the string pointer
		if (pTable) {					// Valid pointer?
			uCount = m_LZWSize[uLZWCode];		// Size of the string
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
			uCount = m_LZWSize[uPreviousCode]+1;	// Get the previous match
			uOutputLength -= uCount;		// Remove from length
			pTable = pPreviousMark;			// Set the pointer
			do {
				pOutput[0] = pTable[0];		// Copy the string
				++pOutput;
				++pTable;
			} while (--uCount);
		}
		m_LZWSize[uNewCode] = m_LZWSize[uPreviousCode]+1;	// Set the new length
		m_LZWTable[uNewCode] = pPreviousMark;		// Data pointer
		++uNewCode;		// Next new LZW code

		if (uNewCode==uCodeMask) {		// Do I need another bit next pass?
			++uCodeBitSize;				// Increase the bit size
			uCodeMask = g_MaskTable[uCodeBitSize];	// Use table
		}
		uPreviousCode = uLZWCode;				// Set the new previous index
		pPreviousMark = pPreviousOutput;		// Pointer to this work data
	} while (uOutputLength);

	// Exit
 	if (!uOutputLength) {	// Check if properly decompressed
		uCount = m_uChunkCount;
		do {
			pInput->SkipForward(uCount);	// Move the pointer
			uCount = pInput->GetByte();	// Get the data count
		} while (uCount);		// Still more data?
		return NULL;		// Good!
	}
	return "Error in GIF decompression.";
}

/*! ************************************

	\class Burger::FileGIF
	\brief Reads and writes GIF files

	This class will read and write out GIF files and use \ref Image
	class instances as data transporters

	It follows the specification found in <a href="spec-gif89a.txt">spec-gif89a.txt</a>

	\sa GIFEncoder or GIFDecoder

***************************************/

/*! ************************************

	\brief Parse the GIF header
	\param pInput Pointer to the input stream

	As per section 17 of the GIF specification, read in 
	the 13 byte header and set up the file global information

	\return \ref NULL if the header was read successfully, or a pointer to an
	English error message with the reason for failure.

***************************************/

const char *Burger::FileGIF::ParseHeader(InputMemoryStream *pInput)
{
	char ID[6];
	const char *pBadNews = NULL;
	m_uLogicalWidth = 0;
	m_uLogicalHeight = 0;
	m_uColorShift = 0;
	m_bGIF89 = FALSE;
	
	// Start with processing the 13 byte header of the GIF file

	if (pInput->BytesRemaining()<13) {
		pBadNews = "Insufficient data for GIF file header. 13 bytes required.";
	} else {

		// Read the 13 byte header

		pInput->Get(ID,6);
		m_uLogicalWidth = pInput->GetShort();
		m_uLogicalHeight = pInput->GetShort();
		Word uDescriptor = pInput->GetByte();
		m_bBackgroundColorIndex = pInput->GetByte();
		m_bAspectRatio = pInput->GetByte();

		// Parse the header
		if (MemoryCompare("GIF",ID,3)) {
			pBadNews = "Not a GIF file.";
		} else {
			if (MemoryCompare(ID+3,"87a",3)) {	// Header ok?
				if (MemoryCompare(ID+3,"89a",3)) {
					pBadNews = "Unknown GIF version (Not 87a or 89a).";
				} else {
					// It's the most recent format!
					m_bGIF89 = TRUE;
				}
			}
		}

		// Was it identified as GIF?
		if (!pBadNews) {
			// Initialize the global palette to black
			ClearPalette(m_Palette,BURGER_ARRAYSIZE(m_Palette));
			ClearPalette(m_GlobalPalette,BURGER_ARRAYSIZE(m_GlobalPalette));

			// Is there a global palette?
			if (uDescriptor&0x80) {
				// Get the number of colors in the palette
				Word uColorCount = 1U<<((uDescriptor&7U)+1U);
				RGBAWord8_t *pWork = m_Palette;
				Word uColorShift = 7U-((uDescriptor>>4U)&7U);
				m_uColorShift = uColorShift;
				do {
					pWork->m_uRed = static_cast<Word8>(pInput->GetByte()<<uColorShift);		// Read in the palette
					pWork->m_uGreen = static_cast<Word8>(pInput->GetByte()<<uColorShift);
					pWork->m_uBlue = static_cast<Word8>(pInput->GetByte()<<uColorShift);
					pWork->m_uAlpha = 255;
					++pWork;
				} while (--uColorCount);
				MemoryCopy(m_GlobalPalette,m_Palette,sizeof(m_GlobalPalette));
			}
		}
	}
	return pBadNews;
}

/*! ************************************

	\brief Process a GIF extension block

	For all GIF blocks that are identified with a '!',
	process them
	\param pInput Pointer to the input stream
	\return \ref NULL if the header was read successfully, or a pointer to an
	English error message with the reason for failure.

***************************************/

const char *Burger::FileGIF::ParseExtension(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;

	// Get the type and the byte size of the first packet
	Word uType = pInput->GetByte();
	WordPtr uCount = pInput->GetByte();
	if ((uType==0xF9) && (uCount==4)) {
		Word uFlags = pInput->GetByte();
		m_uFrameDelay = pInput->GetShort();
		m_bTransparentColor = pInput->GetByte();
		m_bTransparentColorEnable = static_cast<Word8>(uFlags&1U);
		if (pInput->GetByte()!=0) {
			pBadNews = "Graphic Control Extension has an non zero terminator.";
		}
#if defined(DEBUG_READER)
		Debug::Message("m_uFrameDelay %u\n",m_uFrameDelay);
#endif
	} else if ((uType==0xFF) && (uCount==11)) {
		char Netscape[8];
		char Authentication[3];
		pInput->Get(Netscape,8);
		pInput->Get(Authentication,3);
		if (MemoryCompare(Netscape,"NETSCAPE",8) || MemoryCompare(Authentication,"2.0",3)) {
			// Since this isn't a supported extension, ignore it.
			uCount = pInput->GetByte();
			do {
				pInput->SkipForward(uCount);	// Move the pointer
				uCount = pInput->GetByte();	// Get the data count
			} while (uCount);		// Still more data?
		} else {
			Word uSubBlock = pInput->GetByte();
			Word uSubBlockID = pInput->GetByte();
			if ((uSubBlock!=3U) || (uSubBlockID!=1U)) {
				pBadNews = "Invalid Netscape Looping Application Extension sub block ID.";
			} else {
				m_uLoopCount = pInput->GetShort();
#if defined(DEBUG_READER)
				Debug::Message("m_uLoopCount %u\n",m_uLoopCount);
#endif
				if (pInput->GetByte()!=0) {
					pBadNews = "Netscape Looping Application Extension has an non zero terminator.";
				}
			}
		}
	} else {
		// Since this isn't a supported extension, ignore it.
		do {
			pInput->SkipForward(uCount);	// Move the pointer
			uCount = pInput->GetByte();	// Get the data count
		} while (uCount);		// Still more data?
	}
	return pBadNews;
}

/*! ************************************

	\brief Parse a GIF image block

	\param pOutput Pointer to an Image record to store the new image into
	\param pInput Pointer to the input stream

***************************************/

const char * Burger::FileGIF::ParseImage(Image *pOutput,InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;

	Word uOriginX = pInput->GetShort();
	Word uOriginY = pInput->GetShort();
	if (uOriginX|uOriginY) {
		pBadNews = "Origin of GIF image is not zero.";
	} else {
		Word uImageWidth = pInput->GetShort();
		Word uImageHeight = pInput->GetShort();
		if ((uImageWidth!=m_uLogicalWidth) || (uImageHeight!=m_uLogicalHeight)) {
			pBadNews = "Image parsed is not the same size as total image.";
		} else {
			// Local image descriptor
			Word uDescriptor = pInput->GetByte();
			if (uDescriptor&0x80U) {
				// Get the number of colors in the palette
				Word uColorCount = 1U<<((uDescriptor&7U)+1U);
				RGBAWord8_t *pWork = m_Palette;
				Word uColorShift = m_uColorShift;
				do {
					pWork->m_uRed = static_cast<Word8>(pInput->GetByte()<<uColorShift);		// Read in the palette
					pWork->m_uGreen = static_cast<Word8>(pInput->GetByte()<<uColorShift);
					pWork->m_uBlue = static_cast<Word8>(pInput->GetByte()<<uColorShift);
					pWork->m_uAlpha = 255;
					++pWork;
				} while (--uColorCount);
			} else {
				// Use the global palette
				MemoryCopy(m_Palette,m_GlobalPalette,sizeof(m_GlobalPalette));
			}
		}
	}

	// Header is good, get the bitmap

	if (!pBadNews) {
		Word uResult = pOutput->Init(m_uLogicalWidth,m_uLogicalHeight,Image::PIXELTYPE8BIT);
		if (uResult) {
			pBadNews = "Out of memory.";
		} else {
			pOutput->ClearBitmap();
			GIFDecoder *pDecoder = GIFDecoder::New();
			if (pDecoder) {
				pBadNews = pDecoder->Unpack(pOutput->GetImage(),m_uLogicalWidth*m_uLogicalHeight,pInput);	// Decompress it
				Delete(pDecoder);
			} else {
				pBadNews = "Out of memory.";
			}
		}
	}
	return pBadNews;
}

/*! ************************************

	\brief Write a GIF header

	As per section 17 of the GIF specification, write out a 
	the 13 byte header from the file global information

	\param pOutput Pointer to the output stream
	\return 0 for no error, non zero on error

***************************************/

Word Burger::FileGIF::WriteHeader(OutputMemoryStream *pOutput)
{
	// Always assume GIF89a
	Word uResult = pOutput->Append("GIF89a");
	if (!uResult) {
		// Store the size of the image
		uResult |= pOutput->Append(static_cast<Word16>(m_uLogicalWidth));
		uResult |= pOutput->Append(static_cast<Word16>(m_uLogicalHeight));

		// Flags
		// 0x80 = Color table exists
		// 0x70 = Bits per color value (0x70 = 8, 0x60 = 7...)
		// 0x10 = Colors are sorted (Not true)
		// 0x07 = Size of the color table in power of 2 (7 = 2^8, 6 = 2^7, 5 = 2^6...)

		uResult |= pOutput->Append(static_cast<Word8>(0xF7));
		uResult |= pOutput->Append(static_cast<Word8>(m_bBackgroundColorIndex));
		uResult |= pOutput->Append(static_cast<Word8>(m_bAspectRatio));

		// Write out the initial palette

		const RGBAWord8_t *pPalette = m_Palette;	// First save the palette
		WordPtr i = 256;
		do {
			uResult |= pOutput->Append(reinterpret_cast<const RGBWord8_t *>(pPalette));		// Red, Green and Blue
			++pPalette;
		} while (--i);				// All done?
	}
	return uResult;
}

/*! ************************************

	\brief Write a frame using GIF compression

	Compress the image using GIF LZW and write out the data into
	the stream. If the palette was changed, write out a new local palette.

	\param pOutput Pointer to the output stream
	\param pInput Pointer to the pixel data
	\return 0 for no error, non zero on error

***************************************/

Word Burger::FileGIF::WriteImage(OutputMemoryStream *pOutput,const Image *pInput)
{
	// Write out the image descriptor
	Word uResult = pOutput->Append(',');
	// Is the stream working?
	if (!uResult) {
		// Output a frame
		//	uResult |= pOutput->Append(static_cast<Word16>(0));
		//	uResult |= pOutput->Append(static_cast<Word16>(0));
		uResult |= pOutput->Append(static_cast<Word32>(0));		// Replace the two calls above with a single call
		uResult |= pOutput->Append(static_cast<Word16>(m_uLogicalWidth));
		uResult |= pOutput->Append(static_cast<Word16>(m_uLogicalHeight));

		// Has the palette changed?
		if (!MemoryCompare(m_Palette,m_GlobalPalette,sizeof(m_Palette))) {
			uResult |= pOutput->Append(static_cast<Word8>(0));
		} else {
			// Write out a full 256 entry palette
			uResult |= pOutput->Append(static_cast<Word8>(0x87));
			RGBAWord8_t *pPalette = m_Palette;
			WordPtr uColorCount = 256;
			do {
				uResult |= pOutput->Append(reinterpret_cast<const RGBWord8_t *>(pPalette));		// Write out the palette
				++pPalette;
			} while (--uColorCount);

		}
		if (!uResult) {
			// Write out the pixel data
			GIFEncoder Compressor;
			Compressor.Init(pOutput,8);
			const Word8 *pData = pInput->GetImage();
			Word uTemp = m_uLogicalHeight;
			Word uWidth = m_uLogicalWidth;
			if (uTemp) {
				do {
					Compressor.WritePixels(pData,uWidth);
					pData += pInput->GetStride();
				} while (--uTemp);
			}
			Compressor.Flush();
		}
	}
	return uResult;
}

/*! ************************************

	\brief Write out the loop count

	Insert the GIF extension of "NETSCAPE2.0" that contains the
	loop count for a GIF animation file.

	\param pOutput Pointer to the output stream
	\return 0 for no error, non zero on error
	\sa SetLoopCount(Word) or GetLoopCount(void) const

***************************************/

Word Burger::FileGIF::WriteExtensionNetScape20(OutputMemoryStream *pOutput)
{
	// Write out the image descriptor
	Word uResult = pOutput->Append('!');
	// Is the stream working?
	if (!uResult) {
		// Output the NetScape 2.0 extension
		uResult |= pOutput->Append(static_cast<Word8>(255));
		uResult |= pOutput->Append(static_cast<Word8>(11));
		uResult |= pOutput->Append("NETSCAPE2.0");
		uResult |= pOutput->Append(static_cast<Word8>(3));
		uResult |= pOutput->Append(static_cast<Word8>(1));
		uResult |= pOutput->Append(static_cast<Word16>(m_uLoopCount));
		uResult |= pOutput->Append(static_cast<Word8>(0));
	}
	return uResult;
}

/*! ************************************

	\brief Write out a GIF Graphic Control Extension

	Insert the GIF extension for Graphic Control as per
	section 23 of the gif98a specification. This
	record contains the transparent color and the frame
	delay for a gif animation

	\param pOutput Pointer to the output stream
	\return 0 for no error, non zero on error

***************************************/

Word Burger::FileGIF::WriteExtensionGraphicControl(OutputMemoryStream *pOutput)
{
	// Write out the image descriptor
	Word uResult = pOutput->Append('!');
	// Is the stream working?
	if (!uResult) {
		// Output the NetScape 2.0 extension
		uResult |= pOutput->Append(static_cast<Word8>(249));
		uResult |= pOutput->Append(static_cast<Word8>(4));
		Word uFlag = 8;
		if (m_bTransparentColorEnable) {
			uFlag |= 1;
		}
		uResult |= pOutput->Append(static_cast<Word8>(uFlag));
		uResult |= pOutput->Append(static_cast<Word16>(m_uFrameDelay));
		uResult |= pOutput->Append(static_cast<Word8>(m_bTransparentColor));
		uResult |= pOutput->Append(static_cast<Word8>(0));
	}
	return uResult;
}

/*! ************************************

	\brief Write out the terminator descriptor

	Insert a ';' at the end of the GIF file being written to
	terminate the file

	\param pOutput Pointer to the output stream
	\return 0 for no error, non zero on error

***************************************/

Word Burger::FileGIF::WriteTerminator(OutputMemoryStream *pOutput)
{
	// Write out the terminator descriptor
	return pOutput->Append(';');
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileGIF::FileGIF() :
	m_uLogicalWidth(0),		// No image yet
	m_uLogicalHeight(0),
	m_uColorShift(0),		// No shifting of palette colors
	m_uLoopCount(0),		// No looping
	m_uFrameDelay(0),			// No delay
	m_bTransparentColor(0),	// No transparent color
	m_bTransparentColorEnable(FALSE),	// Disable transparent color
	m_bAspectRatio(0),		// No aspect ratio (Default)
	m_bBackgroundColorIndex(0),
	m_bGIF89(TRUE)			// Default to GIF89a
{
	ClearPalette(m_Palette,BURGER_ARRAYSIZE(m_Palette));
	ClearPalette(m_GlobalPalette,BURGER_ARRAYSIZE(m_GlobalPalette));
}

/*! ************************************

	\brief Allocate and initialize a FileGIF

	\return A pointer to a default FileGIF class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::FileGIF * BURGER_API Burger::FileGIF::New(void)
{
	// Allocate the memory
	FileGIF *pThis = new (Alloc(sizeof(FileGIF))) FileGIF();
	return pThis;
}

/*! ************************************

	\brief Parses a GIF file

	Read in a GIF file and set the extra variables

	Will parse an 8 bit compressed and uncompressed GIF files.

	\note A descriptive error message is passed to Debug::Warning()

	\param pOutput Pointer to an Image record to store the new image into
	\param pInput Pointer to a memory stream that has a GIF file image
	\return Zero for success or non-zero on failure

***************************************/

Word Burger::FileGIF::Load(Image *pOutput,InputMemoryStream *pInput)
{
	// Always parse the GIF header!
	const char *pBadNews = ParseHeader(pInput);

	Word uResult = 0;
	if (!pBadNews) {

		// Loop here until an image is found and parsed

		do {
			Word uToken = pInput->GetByte();
			// Extension?
			if (uToken=='!') {
				pBadNews = ParseExtension(pInput);	// Discard it
			// End of data?
			} else if (uToken==';') {
				pBadNews = "Reached end of data without parsing image data.";
				break;

			// Image?
			} else if (uToken==',') {
				pBadNews = ParseImage(pOutput,pInput);	// Extract an image
				break;

			} else {
				pBadNews = "Unknown token found in GIF file.";
			}
		} while (!pBadNews);
	}

	// If there was an error, clean up
	if (pBadNews) {
		Debug::Warning(pBadNews);
		uResult = 10;
	}
	return uResult;
}

/*! ************************************

	\brief Parses a GIF file

	Read in a GIF file and set the extra variables

	Will parse an 8 bit compressed and uncompressed GIF files.

	\note A descriptive error message is passed to Debug::Warning()

	\param pOutput Pointer to an Image record to store the new image into
	\param pInput Pointer to a memory stream that has a GIF file image
	\return Zero for success or non-zero on failure

***************************************/

Word Burger::FileGIF::LoadNextFrame(Image *pOutput,InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Word uResult = 0;

	// Loop here until an image is found and parsed

	do {
		Word uToken = pInput->GetByte();
		// Extension?
		if (uToken=='!') {
			pBadNews = ParseExtension(pInput);	// Discard it
		
		// End of data?
		} else if (uToken==';') {
			// Put it back and exit with a "stop" error code
			pInput->SkipBack(1);
			uResult = 1;
			break;
		// Image?
		} else if (uToken==',') {
			pBadNews = ParseImage(pOutput,pInput);	// Extract an image
			break;
		} else {
			pBadNews = "Unknown token found in GIF file.";
		}
	} while (!pBadNews);

	// If there was an error, clean up
	if (pBadNews) {
		Debug::Warning(pBadNews);
		uResult = 10;
	}
	return uResult;
}

/*! ************************************

	\brief Write an Image structure out as a GIF file image

	Given an image in 8 bit format, write it out as a GIF
	file. No other image data formats are supported, this is a limitation
	to the GIF file format.

	\note Before calling this function, make sure the color palette
	for the pixel data is already set with a call to SetPalette(const RGBWord8_t *,Word,Word)

	\param pOutput Pointer to the output stream to store the file image
	\param pImage Pointer to a valid Image structure containing the image data

	\return Zero if successful

***************************************/

Word Burger::FileGIF::Save(OutputMemoryStream *pOutput,const Image *pImage)
{
	Image::ePixelTypes eType = pImage->GetType();
	Word uResult = 10;
	// Only 8 bit formats are supported
	if (eType==Image::PIXELTYPE8BIT) {
		Word uWidth = pImage->GetWidth();
		Word uHeight = pImage->GetHeight();
		if (uWidth && uHeight) {
			m_uLogicalWidth = uWidth;
			m_uLogicalHeight = uHeight;

			// Output the GIF header
			uResult = WriteHeader(pOutput);
			if (!uResult) {
				
				// Write out the initial image
				// Force WriteImage() to NOT save a local palette

				MemoryCopy(m_GlobalPalette,m_Palette,sizeof(m_Palette));

				// Special case
				// If the image is transparent, write out the control block to 
				// keep the transparency
				
				if (m_bTransparentColorEnable) {
					uResult = WriteExtensionGraphicControl(pOutput);
				}
				if (!uResult) {
					uResult = WriteImage(pOutput,pImage);
					if (!uResult) {
						// Terminate the file
						uResult = WriteTerminator(pOutput);
					}
				}
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Start writing a GIF animation file

	Given an image in 8 bit format, obtain the size of the image
	and write out the image size and initial palette it out to the GIF file.
	
	\note NO IMAGE DATA is written with this call. Call AnimationSaveFrame() to
	output the image data

	\note Before calling this function, make sure the color palette
	for the pixel data is already set with a call to SetPalette(const RGBWord8_t *,Word,Word).
	This palette is used for the global palette that's included in the GIF
	file header.

	\param pOutput Pointer to the output stream to store the file image
	\param pImage Pointer to a valid Image structure containing the image data
	\param uLoopCount Value to store in the GIF file loop count record. 0 = infinite loop

	\return Zero if successful
	\sa AnimationSaveFrame(OutputMemoryStream *,const Image *,Word) or AnimationSaveFinish(OutputMemoryStream *)

***************************************/

Word Burger::FileGIF::AnimationSaveStart(OutputMemoryStream *pOutput,const Image *pImage,Word uLoopCount)
{
	Image::ePixelTypes eType = pImage->GetType();
	Word uResult = 10;
	// Only 8 bit formats are supported
	if (eType==Image::PIXELTYPE8BIT) {
		Word uWidth = pImage->GetWidth();
		Word uHeight = pImage->GetHeight();
		if (uWidth && uHeight) {
			m_uLogicalWidth = uWidth;
			m_uLogicalHeight = uHeight;
			m_uLoopCount = static_cast<Word16>(uLoopCount);

			// Output the GIF header
			uResult = WriteHeader(pOutput);
			if (!uResult) {
				uResult = WriteExtensionNetScape20(pOutput);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Write out an image frame to a GIF animation file

	Append a new frame of animation data to the GIF file being created.

	\param pOutput Pointer to the output stream to store the file image
	\param pImage Pointer to a valid Image structure containing the image data
	\param uDelay Time delay between frames in 1/100ths of a second (0 = no delay)
	\return Zero if successful
	\sa AnimationSaveStart(OutputMemoryStream *,const Image *,Word) or AnimationSaveFinish(OutputMemoryStream *)

***************************************/

Word Burger::FileGIF::AnimationSaveFrame(OutputMemoryStream *pOutput,const Image *pImage,Word uDelay)
{
	m_uFrameDelay = static_cast<Word16>(uDelay);
	Word uResult = WriteExtensionGraphicControl(pOutput);
	if (!uResult) {
		uResult = WriteImage(pOutput,pImage);
	}
	return uResult;
}

/*! ************************************

	\brief Wrap up writing a GIF animation file

	Finish writing out an in-progress animation stream. After
	issuing this call, the OutputMemoryStream has a valid GIF
	file image.

	\param pOutput Pointer to the output stream to store the file image
	\return Zero if successful
	\sa AnimationSaveStart(OutputMemoryStream *,const Image *,Word) or AnimationSaveFrame(OutputMemoryStream *,const Image *,Word)

***************************************/

Word Burger::FileGIF::AnimationSaveFinish(OutputMemoryStream *pOutput)
{
	return WriteTerminator(pOutput);
}

/*! ************************************

	\fn Word Burger::FileGIF::GetAspectRatio(void) const
	\brief Get the file image's pixel aspect ratio

	Factor used to compute an approximation
	of the aspect ratio of the pixel in the original image.  If the
	value of the field is not 0, this approximation of the aspect ratio
	is computed based on the formula:

	Aspect Ratio = (GetAspectRatio() + 15) / 64 (15 is for rounding up)

	The Pixel Aspect Ratio is defined to be the quotient of the pixel's
	width over its height.  The value range in this field allows
	specification of the widest pixel of 4:1 to the tallest pixel of
	1:4 in increments of 1/64th.

	\return The GIF formatted aspect ratio value
	\sa SetAspectRatio(Word), GetBackgroundColorIndex(void) const or GetLoopCount(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetAspectRatio(Word bAspectRatio)
	\brief Set the file image's aspect ratio

	Sets the GIF file header's aspect ratio. Use zero for default.
	
	\param bAspectRatio New GIF formatted aspect ratio token
	\sa GetAspectRatio(void) const, SetBackgroundColorIndex(Word) or SetLoopCount(Word)

***************************************/


/*! ************************************

	\fn Word Burger::FileGIF::GetBackgroundColorIndex(void) const
	\brief Get the file image's background color index

	Return the GIF file header's background color index. This value is
	usually set to zero.

	\return The color index to use as the background color
	\sa SetBackgroundColorIndex(Word), GetAspectRatio(void) const or GetLoopCount(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetBackgroundColorIndex(Word bBackGroundColorIndex)
	\brief Set the file image's background color index

	Sets the GIF file header's background color index
	
	\param bBackGroundColorIndex New GIF background color index
	\sa GetBackgroundColorIndex(void) const, SetAspectRatio(Word) or SetLoopCount(Word)

***************************************/

/*! ************************************

	\fn Word Burger::FileGIF::GetTransparentColorIndex(void) const
	\brief Get the file image's transparent color index

	Return the GIF file frame's transparent color index. This value is
	usually set to zero and can change from frame to frame.

	\return The color index to use as the transparent color
	\sa SetTransparentColorIndex(Word)

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetTransparentColorIndex(Word bTransparentColor)
	\brief Set the file image's transparent color index

	Sets the GIF file frame's transparent color index. It must be set before a frame
	is converted into GIF format.
	
	\param bTransparentColor New GIF frame transparent color index
	\sa GetTransparentColorIndex(void) const

***************************************/

/*! ************************************

	\fn Word Burger::FileGIF::GetTransparentColorEnable(void) const
	\brief Return \ref TRUE if the frame has a transparent color index

	Return the GIF file frame's transparent color index enable flat. This value is
	usually set to \ref FALSE, meaning there is no transparent color index
	and can change from frame to frame.

	\return \ref FALSE if there is no transparent color index, \ref TRUE if transparency is enabled
	\sa SetTransparentColorEnable(Word)

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetTransparentColorEnable(Word bTransparentColorEnable)
	\brief Enable/disable the file image's transparent color index

	Enables or disables the GIF file frame's transparent color index. It must be set before a frame
	is converted into GIF format. Pass zero to disable transparency,
	non-zero to enable it.
	
	\param bTransparentColorEnable \ref FALSE to disable transparency, \ref TRUE to enable it
	\sa GetTransparentColorIndex(void) const

***************************************/


/*! ************************************

	\fn Word Burger::FileGIF::GetLoopCount(void) const
	\brief Get the file image's animation loop count

	GIF files have a special record to contain the number of times
	the animation should loop. If a file is written as an animation,
	this record should be set to a count, or leave as zero
	meaning this animation should loop forever.

	\return The GIF animation loop count (Zero means forever)
	\sa SetLoopCount(Word), GetAspectRatio(void) const or GetBackgroundColorIndex(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetLoopCount(Word uLoopCount)
	\brief Set the file image's loop count

	Set the GIF animation's loop count. Only used when writing
	animated gifs

	\note This value is written as a \ref Word16. Any values larger than 65535
	will be truncated.
	
	\param uLoopCount New animation loop count
	\sa GetLoopCount(void) const, SetAspectRatio(Word) or SetBackgroundColorIndex(Word)

***************************************/

/*! ************************************

	\fn Word Burger::FileGIF::GetFrameDelay(void) const
	\brief Get the current animation frame delay

	Animated GIF files have a delay time in 1/100ths of a second.
	This function, when called after parsing a frame, will have the 
	time delay the player should wait before displaying the
	next frame in the sequence. Zero means no delay.

	\return The time delay before showing the next frame in 1/100ths of a second
	\sa SetFrameDelay(Word)

***************************************/

/*! ************************************

	\fn void Burger::FileGIF::SetFrameDelay(Word uFrameDelay)
	\brief Set the animation frame delay.

	Before appending a new frame to an animation, set this value
	to the desired frame delay to pause before showing the next
	frame in 1/100ths of a second.
	
	\param uFrameDelay Time delay before the next frame
	\sa GetFrameDelay(void) const

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

	\fn void Burger::FileGIF::SetPalette(const RGBWord8_t *pInput,Word uStartIndex,Word uPaletteSize)
	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit GIF file. Since the Alpha is missing from the
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

	\fn void Burger::FileGIF::SetPalette(const RGBAWord8_t *pInput,Word uStartIndex,Word uPaletteSize)
	\brief Set the file image's palette (RGBA)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit GIF file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/
