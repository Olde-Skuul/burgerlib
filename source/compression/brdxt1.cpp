/***************************************

    DXT1 compressor and decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdxt1.h"
#include "brendian.h"

/*! ************************************

	\struct Burger::Dxt1Packet_t
	\brief Data structure for a DXT1 compressed texture block

	This structure is a 1:1 mapping of a 8 byte DXT1 encoded
	data block used by many modern video cards.
	The 8 bytes represent the color for the 4x4 block.

	The data compressor and decompresser are designed to be able
	to use frame buffers as input and output to reduce the
	need for intermediate buffers for speed.

	\sa Burger::Dxt3Packet_t and Burger::Dxt5Packet_t

***************************************/

/*! ************************************

	\brief Decompress a single 4x4 block compressed with DXT1

	Given an 8 byte block of DXT1 compressed data, extract the 4x4 RGBA
	color block. 
	
	\note Alpha will be set to 0xFF on non transparent blocks, 0x00 for transparent.

	\param pOutput Pointer to the start of an array of sixteen \ref RGBAWord8_t encoded 32 bit
	pixels
	\param uStride Byte width of each scan line for the block of uncompressed data. The default
	is sizeof(\ref RGBAWord8_t)*4 to create a single array of 16 RGBAWord8_t pixels in a linear row.

***************************************/

void Burger::Dxt1Packet_t::Decompress(RGBAWord8_t *pOutput,WordPtr uStride) const
{
	// Secondly, let's unpack the color (Use RGBA structures to avoid a mul by 3 in indexing)
	// 4 unique colors

	RGBAWord8_t Colors[4];

	Word uColor1 = LittleEndian::Load(&m_uRGB565Color1);
	Word uColor2 = LittleEndian::Load(&m_uRGB565Color2);
	Palette::FromRGB16(&Colors[0],uColor1);
	Palette::FromRGB16(&Colors[1],uColor2);
	
	// Create the two middle colors by scaling
	
	if (uColor1>uColor2) {
		uColor1 = Colors[0].m_uRed;
		uColor2 = Colors[1].m_uRed;
		Colors[2].m_uRed = static_cast<Word8>(((uColor1*2) + (uColor2))/3U);
		Colors[3].m_uRed = static_cast<Word8>(((uColor1) + (uColor2*2))/3U);

		uColor1 = Colors[0].m_uGreen;
		uColor2 = Colors[1].m_uGreen;
		Colors[2].m_uGreen = static_cast<Word8>(((uColor1*2) + (uColor2))/3U);
		Colors[3].m_uGreen = static_cast<Word8>(((uColor1) + (uColor2*2))/3U);

		uColor1 = Colors[0].m_uBlue;
		uColor2 = Colors[1].m_uBlue;
		Colors[2].m_uBlue = static_cast<Word8>(((uColor1*2) + (uColor2))/3U);
		Colors[3].m_uBlue = static_cast<Word8>(((uColor1) + (uColor2*2))/3U);
		// Set the alpha
		Colors[2].m_uAlpha = 255;
		Colors[3].m_uAlpha = 255;
	} else {
		uColor1 = Colors[0].m_uRed;
		uColor2 = Colors[1].m_uRed;
		Colors[2].m_uRed = static_cast<Word8>((uColor1 + uColor2)/2U);
		Colors[3].m_uRed = 0;

		uColor1 = Colors[0].m_uGreen;
		uColor2 = Colors[1].m_uGreen;
		Colors[2].m_uGreen = static_cast<Word8>((uColor1 + uColor2)/2U);
		Colors[3].m_uGreen = 0;

		uColor1 = Colors[0].m_uBlue;
		uColor2 = Colors[1].m_uBlue;
		Colors[2].m_uBlue = static_cast<Word8>((uColor1 + uColor2)/2U);
		Colors[3].m_uBlue = 0;
		// Set the alpha
		Colors[2].m_uAlpha = 255;
		Colors[3].m_uAlpha = 0;
	}
	// Color and alpha indexes
	const Word8 *pColorIndexes = m_uColorIndexes;

	// The code is going to add this to the pointer, so remove
	// it from the input stride to undo the pointer addition
	uStride -= (sizeof(RGBAWord8_t)*4);

	Word i=16/4;			// 16 pixels, 4 pixels per loop
	do {
		// 4 2 bits per pixel indexes
		Word uIndex = pColorIndexes[0];
		++pColorIndexes;
		Word j=4;		// 4 pixels per loop
		do {
			// Select the pixel to use for drawing
			const RGBAWord8_t *pSource = &Colors[uIndex&3U];
			uIndex>>=2;
			// Get the final color
			pOutput[0] = pSource[0];
			++pOutput;
		} while (--j);
		// 4 pixels (A row) are drawn, skip to the next line
		pOutput = reinterpret_cast<RGBAWord8_t *>(reinterpret_cast<Word8 *>(pOutput)+uStride);
	} while (--i);
}

#if 0
/*! ************************************

	\brief Compress a single 4x4 block with DXT1

	Given an 8 byte block of RGBA data, compress it into a 4x4 DXT1
	color block. 

	\param pInput Pointer to the start of an array of sixteen \ref RGBAWord8_t encoded 32 bit
	pixels
	\param uStride Byte width of each scan line for the block of uncompressed data. The default
	is sizeof(\ref RGBAWord8_t)*4 to create a single array of 16 RGBAWord8_t pixels in a linear row.

***************************************/

void Burger::Dxt1Packet_t::Compress(const RGBAWord8_t *pInput,WordPtr uStride)
{
	// No alpha detected
	Word Unique[16];
	Word uUnique = 0;
	Word uAlpha = FALSE;
	Word i=4;
	do {
		Word uColor = Palette::ToRGB16(pInput);
		pInput = reinterpret_cast<const RGBAWord8_t *>(reinterpret_cast<const Word8 *>(pInput)+uStride);
	} while (--i);
}
#endif

/*! ************************************

	\brief Decompress an array of 4x4 block compressed with DXT1

	Given an array of 8 byte blocks of DXT1 compressed data, extract the 4x4 RGBA
	color blocks into a bit map. 

	\note This function will handle clipping if the destination bitmap is not divisible by
	four in the width or height

	\param pOutput Pointer to the start of a bitmap in the size of uWidth and uHeight \ref RGBAWord8_t encoded 32 bit
	pixels.
	\param uOutputStride Number of bytes per scan line of the output bit map (Usually it's uWidth*4)
	\param uWidth Width of the output bitmap in pixels
	\param uHeight Height of the output bitmap in pixels
	\param pInput Pointer to a matching bitmap of DXT1 compressed pixels
	\param uInputStride Byte width of each scan line for the block of compressed data. (Usually it's uWidth*4)

***************************************/

void BURGER_API Burger::DecompressImage(RGBAWord8_t *pOutput,WordPtr uOutputStride,Word uWidth,Word uHeight,const Dxt1Packet_t *pInput,WordPtr uInputStride)
{
	// Anything to process?
	if (uWidth && uHeight) {
		// Process a line of blocks
		do {
			// Init the steppers
			Word uTempWidth = uWidth;
			const Dxt1Packet_t *pDXTInput = pInput;
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
			pInput = reinterpret_cast<const Dxt1Packet_t *>(reinterpret_cast<const Word8 *>(pInput)+uInputStride);
			// Step to the next 4 lines
			pOutput = reinterpret_cast<RGBAWord8_t *>(reinterpret_cast<Word8 *>(pOutput)+(uOutputStride*4));
		} while (uHeight);
	}
}