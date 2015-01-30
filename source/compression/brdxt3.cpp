/***************************************

	DXT3 compressor and decompresser

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdxt3.h"
#include "brendian.h"

/*! ************************************

	\struct Burger::Dxt3Packet_t
	\brief Data structure for a DXT3 compressed texture block

	This structure is a 1:1 mapping of a 16 byte DXT3 encoded
	data block used by many modern video cards. The first 8 bytes
	represent the 4 bits per pixel alpha for the 4x4 block
	and the next 8 bytes represent the color for the 4x4 block.

	The data compressor and decompresser are designed to be able
	to use frame buffers as input and output to reduce the
	need for intermediate buffers for speed.

***************************************/

/*! ************************************

	\brief Decompress a single 4x4 block compressed with DXT3

	Given a 16 byte block of DXT3 compressed data, extract the 4x4 RGBA
	color block.

	\param pOutput Pointer to the start of an array of sixteen \ref RGBAWord8_t encoded 32 bit
	pixels
	\param uStride Byte width of each scan line for the block of uncompressed data. The default
	is sizeof(\ref RGBAWord8_t)*4 to create a single array of 16 RGBAWord8_t pixels in a linear row.

***************************************/

void Burger::Dxt3Packet_t::Decompress(RGBAWord8_t *pOutput,WordPtr uStride) const
{
	// Secondly, let's unpack the color (Use RGBA structures to avoid a mul by 3 in indexing)
	// 4 unique colors

	RGBAWord8_t Colors[4];

	Palette::FromRGB16(&Colors[0],LittleEndian::Load(&m_uRGB565Color1));
	Palette::FromRGB16(&Colors[1],LittleEndian::Load(&m_uRGB565Color2));
	
	// Create the two middle colors by scaling
	
	Word uColor1 = Colors[0].m_uRed;
	Word uColor2 = Colors[1].m_uRed;
	Colors[2].m_uRed = static_cast<Word8>(((uColor1*2U) + (uColor2))/3U);
	Colors[3].m_uRed = static_cast<Word8>(((uColor1) + (uColor2*2))/3U);

	uColor1 = Colors[0].m_uGreen;
	uColor2 = Colors[1].m_uGreen;
	Colors[2].m_uGreen = static_cast<Word8>(((uColor1*2U) + (uColor2))/3U);
	Colors[3].m_uGreen = static_cast<Word8>(((uColor1) + (uColor2*2U))/3U);

	uColor1 = Colors[0].m_uBlue;
	uColor2 = Colors[1].m_uBlue;
	Colors[2].m_uBlue = static_cast<Word8>(((uColor1*2U) + (uColor2))/3U);
	Colors[3].m_uBlue = static_cast<Word8>(((uColor1) + (uColor2*2U))/3U);

	// Color and alpha indexes
	const Word8 *pColorIndexes = m_uColorIndexes;
	const Word8 *pAlphaIndexes = m_uAlpha;

	// The code is going to add this to the pointer, so remove
	// it from the input stride to undo the pointer addition
	uStride -= (sizeof(RGBAWord8_t)*4U);

	Word j=16/4;			// 16 pixels, 4 pixels per loop
	do {
		// 4 2 bits per pixel indexes
		Word uIndex = pColorIndexes[0];
		++pColorIndexes;
		Word k=4/2;		// 4 pixels per loop
		do {
			// Select the pixel to use for drawing
			const RGBAWord8_t *pSource = &Colors[uIndex&3U];
			uIndex>>=2U;
			// Get the 4 bit per pixel alpha
			Word uAlpha = pAlphaIndexes[0];
			++pAlphaIndexes;

			// Get the final color
			pOutput->m_uRed = pSource->m_uRed;
			pOutput->m_uGreen = pSource->m_uGreen;
			pOutput->m_uBlue = pSource->m_uBlue;
			pOutput->m_uAlpha = static_cast<Word8>((uAlpha&0xFU)|(uAlpha<<4U));
			++pOutput;

			// Select the pixel to use for drawing
			pSource = &Colors[uIndex&3U];
			uIndex>>=2U;
			// Get the final color
			pOutput->m_uRed = pSource->m_uRed;
			pOutput->m_uGreen = pSource->m_uGreen;
			pOutput->m_uBlue = pSource->m_uBlue;
			pOutput->m_uAlpha = static_cast<Word8>((uAlpha>>4U)|(uAlpha&0xF0U));
			++pOutput;
		} while (--k);
		// 4 pixels (A row) are drawn, skip to the next line
		pOutput = reinterpret_cast<RGBAWord8_t *>(reinterpret_cast<Word8 *>(pOutput)+uStride);
	} while (--j);
}

/*! ************************************

	\brief Decompress an array of 4x4 block compressed with DXT3

	Given an array of 16 byte blocks of DXT3 compressed data, extract the 4x4 RGBA
	color blocks into a bit map. 

	\note This function will handle clipping if the destination bitmap is not divisible by
	four in the width or height

	\param pOutput Pointer to the start of a bitmap in the size of uWidth and uHeight \ref RGBAWord8_t encoded 32 bit
	pixels.
	\param uOutputStride Number of bytes per scan line of the output bit map (Usually it's uWidth*4)
	\param uWidth Width of the output bitmap in pixels
	\param uHeight Height of the output bitmap in pixels
	\param pInput Pointer to a matching bitmap of DXT3 compressed pixels
	\param uInputStride Byte width of each scan line for the block of compressed data. (Usually it's uWidth*4)

***************************************/

void BURGER_API Burger::DecompressImage(RGBAWord8_t *pOutput,WordPtr uOutputStride,Word uWidth,Word uHeight,const Dxt3Packet_t *pInput,WordPtr uInputStride)
{
	// Anything to process?
	if (uWidth && uHeight) {
		// Process a line of blocks
		do {
			// Init the steppers
			Word uTempWidth = uWidth;
			const Dxt3Packet_t *pDXTInput = pInput;
			RGBAWord8_t *pDest = pOutput;
			do {
				// Handle clipping
				if ((uTempWidth<4) || (uHeight<4)) {
					// Decompress the block locally
					RGBAWord8_t LocalPixels[16];
					pDXTInput->Decompress(LocalPixels,sizeof(RGBAWord8_t)*4);
					// Write the decompressed pixels to the correct image locations
					const RGBAWord8_t *pLocal = LocalPixels;
					RGBAWord8_t *pDest2 = pDest;
					Word uHeightTemp = uHeight;
					do {
						Word uTempWidth2 = uTempWidth;
						do {
							// Copy the pixel value
							pDest2[0] = pLocal[0];
							// skip this pixel as its outside the image
							++pLocal;
							++pDest2;
						} while (--uTempWidth2);
						pLocal = pLocal+(4-uTempWidth);
						pDest2 = reinterpret_cast<RGBAWord8_t *>(reinterpret_cast<Word8 *>(pDest2)+uOutputStride)-(4-uTempWidth);
					} while (--uHeightTemp);
					if (uTempWidth<4) {
						break;
					}
				} else {
					// Decompress directly into the output
					pDXTInput->Decompress(pDest,uOutputStride);
				}
				++pDXTInput;		// Next cell
				pDest+=4;			// Skip 4 pixels
				uTempWidth-=4;
			} while (uTempWidth);

			// Clipped?
			if (uHeight<4) {
				break;
			}
			uHeight -= 4;
			// Step to the next line of compressed pixels
			pInput = reinterpret_cast<const Dxt3Packet_t *>(reinterpret_cast<const Word8 *>(pInput)+uInputStride);
			// Step to the next 4 lines
			pOutput = reinterpret_cast<RGBAWord8_t *>(reinterpret_cast<Word8 *>(pOutput)+(uOutputStride*4));
		} while (uHeight);
	}
}