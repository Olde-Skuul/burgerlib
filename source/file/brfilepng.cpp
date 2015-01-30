/***************************************

	PNG File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilepng.h"
#include "brdebug.h"
#include "brdecompressdeflate.h"
#include "brfixedpoint.h"

/*! ************************************

	\class Burger::FilePNG
	\brief Reads and writes PNG files

	This class will read and write out PNG files and use Burger::Image
	class instances as data transporters

***************************************/

//
// Header of a PNG file (Big Endian, 8 bytes)
//

#if !defined(DOXYGEN)

#define IHDRASCII 0x49484452		// IHDR Header
#define PLTEASCII 0x504C5445		// PLTE Palette
#define IDATASCII 0x49444154		// IDAT Pixels
#define IENDASCII 0x49454E44		// IEND End of file

struct MasterHeader {	// Beginning of the file
	char m_ID[8];		// g_Signature
};

struct IHDRHeader {		// IHDR record
	Word32 m_uLength;	// Size of the record (9)
	char m_ID[4];		// IHDR
	Word32 m_uWidth;	// Size of the final shape
	Word8 m_uDepth;		// Bit depth per color component
	Word8 m_uColorType;	// Type of color (PNG_*)
	Word8 m_uCompressionMethod;		// Only zero
	Word8 m_uFilterMethod;			// Only zero
	Word8 m_uInterlaceMethod;		// TRUE or FALSE
};

static const Word8 g_Signature[8] = { 0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A };

#endif

/*! ************************************

	\brief Scan a PNG file in memory and return pointer to a chunk
	\param pInput Data stream to read compressed data from
	\param uID PNG token to scan for (Must be upper case)
	\param uStartOffset Offset into the stream to begin the scan
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * Burger::FilePNG::SeekChunk(InputMemoryStream *pInput,Word32 uID,WordPtr uStartOffset)
{
	pInput->SetMark(uStartOffset);
	// Only run if there's enough data to scan
	while (pInput->BytesRemaining()>=8) {
		// Get the chunk length and then the ID
		Word32 uLength = pInput->GetBigWord32();
		Word32 uPNGID = pInput->GetBigWord32();
		// Force upper case
		Word32 uTest = uPNGID&0xDFDFDFDFU;
		if (uTest==uID) {
			// Found the chunk. Save the start offset and relevant info
			m_uNextOffset = pInput->GetMark()+uLength+4;
			// Store the ID and chunk
			m_uPNGID = uPNGID;
			m_uChunkSize = uLength;
			return NULL;
		}
		// Was this the end of PNG chunk?
		if (uTest==IENDASCII) {
			// Abort now!
			break;
		}
		// Skip the chunk data and the appended CRC (4 bytes for CRC)
		pInput->SkipForward(uLength+4);
	}
	return "PNG Chunk was not found";
}

/*! ************************************

	\brief Scan a PNG file in memory and return pointer to a chunk
	\param pInput Data stream to read compressed data from
	\param uID PNG token to scan for (Must be upper case)
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * Burger::FilePNG::SeekPNGChunk(InputMemoryStream *pInput,Word32 uID)
{
	return SeekChunk(pInput,uID,m_uStartOffset);
}

/*! ************************************

	\brief Scan a PNG file in memory starting AFTER the previously found chunk and return pointer to a chunk
	\param pInput Data stream to read compressed data from
	\param uID PNG token to scan for (Must be upper case)
	\return \ref NULL if success or a pointer to a string describing the error

***************************************/

const char * Burger::FilePNG::SeekNextPNGChunk(InputMemoryStream *pInput,Word32 uID)
{
	return SeekChunk(pInput,uID,m_uNextOffset);
}

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FilePNG::FilePNG() :
	m_uStartOffset(0),
	m_uNextOffset(0),
	m_uChunkSize(0),
	m_uPNGID(0)
{
	MemoryClear(m_Palette,sizeof(m_Palette));
}

/*! ************************************

	\brief Parses a PNG file

	Read in a PNG file and set the extra variables

	Will parse 8, 16, 24 and 32 bit compressed PNG files only.
	Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a PNG file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Word BURGER_API Burger::FilePNG::Load(Image *pOutput,InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Word uResult = 10;
	Word uWidth = 0;
	Word uHeight = 0;
	Word uDepth = 0;
	Word uColorType = 0;
	Word uCompressionMethod = 0;
	Word uFilterMethod = 0;
	Word uInterlaceMethod = 0;
	Image::ePixelTypes eType = Image::PIXELTYPE8888;

	Word8 ID[8];
	pInput->Get(ID,8);
	if (MemoryCompare(ID,g_Signature,8)) {
		pBadNews = "No PNG header (Not an PNG File).";
	} else {
		// Identified as PNG?
		// Scan the PNG file from here
		m_uStartOffset = pInput->GetMark();
		// Find the common chunk
		pBadNews = SeekPNGChunk(pInput,IHDRASCII);
		if (!pBadNews) {
			if (m_uChunkSize<13) {
				pBadNews = "IHDR chunk is too small.";
			} else {
				uWidth = pInput->GetBigWord32();
				uHeight = pInput->GetBigWord32();
				uDepth = pInput->GetByte();
				uColorType = pInput->GetByte();
				uCompressionMethod = pInput->GetByte();
				uFilterMethod = pInput->GetByte();
				uInterlaceMethod = pInput->GetByte();
				// Sanity check
				if (!uWidth) {
					pBadNews = "Width can't be zero.";
				} else if (!uHeight) {
					pBadNews = "Height can't be zero.";
				} else if (uCompressionMethod) {
					pBadNews = "Unknown compression.";
				} else if (uFilterMethod) {
					pBadNews = "Unknown filter method.";
				} else if (uInterlaceMethod) {
					pBadNews = "Can't process interlaced images.";
				} else {
					switch (uColorType) {
					default:
						pBadNews = "Unknown color type.";
						break;
					case PNG_INDEXED:
						if (uDepth==8) {
							eType = Image::PIXELTYPE8BIT;
							// uDepth = 8;
						} else {
							pBadNews = "Unsupported indexed color bit depth.";
						}
						break;
					case PNG_RGB:
						if (uDepth==8) {
							eType = Image::PIXELTYPE888;
							uDepth = 24;
						} else {
							pBadNews = "Unsupported RGB bit depth.";
						}
						break;
					case PNG_RGBA:
						if (uDepth==8) {
							eType = Image::PIXELTYPE8888;
							uDepth = 32;
						} else {
							pBadNews = "Unsupported RGBA bit depth.";
						}
						break;
					}
				}
			}
		}
	}

	// Does it need a palette?

	if (!pBadNews) {
		if (eType == Image::PIXELTYPE8BIT) {		// Get the palette
			pBadNews = SeekPNGChunk(pInput,PLTEASCII); // Read in the palette
			if (!pBadNews) {
				MemoryClear(m_Palette,sizeof(m_Palette));
				Word32 uPaletteSize = m_uChunkSize/3U;
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
		pBadNews = SeekPNGChunk(pInput,IDATASCII); // Find the image
		if (!pBadNews) {
			uResult = pOutput->Init(uWidth,uHeight,eType);
			if (!uResult) {

				// It uses Deflate (ZLIB) compression

				DecompressDeflate *pDecompressor = new (Alloc(sizeof(DecompressDeflate))) DecompressDeflate;
				const Word8 *pPacked = pInput->GetPtr();
				WordPtr uPackedSize = m_uChunkSize;
				Decompress::eError Error = Decompress::DECOMPRESS_OKAY;
				Word8 *pDest = pOutput->GetImage();
				uDepth = (uDepth+7U)>>3U;
				uWidth = uWidth*uDepth;
				do {
					Word8 bType = 0;

					// The loops are needed because there could be
					// sequential multiple IDAT chunks so the decompression
					// needs to be able to span these chunks

					do {
						if (!uPackedSize) {
							pBadNews = SeekNextPNGChunk(pInput,IDATASCII);
							if (pBadNews) {
								break;
							}
							pPacked = pInput->GetPtr();
							uPackedSize = m_uChunkSize;
						}
						Error = pDecompressor->Process(&bType,1,pPacked,uPackedSize);
						WordPtr uStep = pDecompressor->GetProcessedInputSize();
						pPacked += uStep;
						uPackedSize -= uStep;
					} while (pDecompressor->GetProcessedOutputSize()!=1);
					if (pBadNews) {
						break;
					}
					WordPtr uRemaining = uWidth;
					do {
						if (!uPackedSize) {
							pBadNews = SeekNextPNGChunk(pInput,IDATASCII);
							if (pBadNews) {
								break;
							}
							pPacked = pInput->GetPtr();
							uPackedSize = m_uChunkSize;
						}
						Error = pDecompressor->Process(pDest+(uWidth-uRemaining),uRemaining,pPacked,uPackedSize);
						WordPtr uStep = pDecompressor->GetProcessedInputSize();
						pPacked += uStep;
						uPackedSize -= uStep;
						uRemaining -= pDecompressor->GetProcessedOutputSize();
					} while (uRemaining);
					if (pBadNews) {
						break;
					}

					switch (bType) {
					// Add the value from the previous pixel
					case 1:
						{
							Word i = uDepth;
							const Word8 *pPrevious = pDest-uDepth;
							do {
								pDest[i] = static_cast<Word8>(pDest[i]+pPrevious[i]);
							} while (++i<uWidth);
						}
						break;
					// Add the value from the previous scan line
					case 2:
						{
							Word i = 0;
							const Word8 *pPrevious = pDest-uWidth;
							do {
								pDest[i] = static_cast<Word8>(pDest[i]+pPrevious[i]);
							} while (++i<uWidth);
						}
						break;
					case 3:
						{
							Word i = 0;
							const Word8 *pPrevious = pDest-uWidth;
							do {
								pDest[i] = static_cast<Word8>(pDest[i]+(pPrevious[i]/2));
							} while (++i<uDepth);

							do {
								pDest[i] = static_cast<Word8>(pDest[i]+((pPrevious[i] + pDest[i-uDepth])/2));
							} while (++i<uWidth);
						}
						break;
					case 4:
						{
							Word i = 0;
							const Word8 *pPrevious = pDest-uWidth;
							do {
								pDest[i] = static_cast<Word8>(pDest[i]+pPrevious[i]);
							} while (++i<uDepth);

							do {
								Int32 c = pPrevious[i-uDepth];
								Int32 a = pDest[i-uDepth];
								Int32 b = pPrevious[i];
								Int32 p = b - c;
								Int32 pc = a - c;

								Int32 pa = Abs(p);
								Int32 pb = Abs(pc);
								pc = Abs(p + pc);

								// Find the best predictor, the least of pa, pb, pc favoring the earlier
								// ones in the case of a tie.
								
								if (pb < pa) {
									pa = pb;
									a = b;
								}
								if (pc < pa) {
									a = c;
								}
								// Calculate the current pixel in a, and move the previous row pixel to c
								// for the next time round the loop
								c = b;
								a += pDest[i];
								pDest[i] = static_cast<Word8>(a);
							} while (++i<uWidth);
						}
						break;
					// Use the data as is
					default:		// None
						break;
					}
					pDest+=uWidth;
				} while (--uHeight);
				Delete(pDecompressor);
				if (Error!=Decompress::DECOMPRESS_OKAY) {
					pBadNews = "Decompression error.";
				}
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

	\fn const RGBAWord8_t *Burger::FilePNG::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the PNG file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FilePNG::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FilePNG::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the PNG file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FilePNG::GetPalette(void) const

***************************************/



/*! ************************************

	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit PNG file. Since the Alpha is missing from the
	input, each entry set will have the alpha set to zero.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBAWord8_t *,Word,Word)

***************************************/

void Burger::FilePNG::SetPalette(const RGBWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
	for writing to an 8 bit PNG file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pPalette Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,Word,Word)

***************************************/

void Burger::FilePNG::SetPalette(const RGBAWord8_t *pPalette,Word uStartIndex,Word uPaletteSize)
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
