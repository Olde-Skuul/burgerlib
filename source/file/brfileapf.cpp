/***************************************

    Apple Preferred File handler class (Apple IIgs)

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfileapf.h"
#include "brdebug.h"
#include "brrenderer.h"
#include "brunpackbytes.h"

/*! ************************************

	\class Burger::FileAPF
	\brief Reads files

	This class will read Apple IIgs Apple Preferred Graphic files and use Burger::Image
	class instances as data transporters

***************************************/

#if !defined(DOXYGEN)

struct ScanLineDescription_t {
	uint16_t m_uChunkLength;			// Number of compressed bytes for this scan line
	uint16_t m_uScanLineControl;		// Scan line control byte
};

static const uint8_t g_MainName[] = {0x04,0x4d,0x41,0x49,0x4e};	// "\pMAIN"
#endif

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileAPF::FileAPF()
{
}

/*! ************************************

	\brief Parses an Apple Preferred File (Apple IIgs)

	Read in an Apple Preferred File made for an Apple IIgs and set the extra variables

	Will parse 4 bit compressed files only.

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has an APF file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FileAPF::Load(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	
	for (;;) {
		if (pInput->BytesRemaining()<4) {
			pBadNews = "Not enough data for a chunk size.";
			break;
		}
		// Get the mark to skip to the next chunk
		uintptr_t uMark = pInput->GetMark();
		uint32_t uChunkSize = pInput->GetWord32();
		if ((pInput->BytesRemaining()+4)<uChunkSize) {
			pBadNews = "Not enough data remaining for data chunk.";
			break;
		}

		// Is this a "MAIN" chunk?
		if (pInput->IsDataMatch(g_MainName,5)) {
			uint_t uMasterMode = pInput->GetShort();
			if (uMasterMode&0x80) {
				pBadNews = "Only 640 pixels per scan line is not supported.";
				break;

			}
			uint_t uPixelsPerScanLine = pInput->GetShort();
			if (uPixelsPerScanLine!=320) {
				pBadNews = "Only 320 pixels per scan line are supported.";
				break;
			}
			uint_t uNumberColorTables = pInput->GetShort();
			if (uNumberColorTables>16) {
				pBadNews = "More than 16 color tables detected.";
				break;
			}

			// Load in the palette
			memory_clear(m_Palette,sizeof(m_Palette));
			if (uNumberColorTables) {
				uNumberColorTables <<= 4U;
				RGBAWord8_t *pColor = m_Palette;
				do {
					uint_t uColor = pInput->GetShort();
					pColor->m_uRed = Renderer::RGB4ToRGB8Table[(uColor>>8U)&0xFU];
					pColor->m_uGreen = Renderer::RGB4ToRGB8Table[(uColor>>4U)&0xFU];
					pColor->m_uBlue = Renderer::RGB4ToRGB8Table[uColor&0xFU];
					pColor->m_uAlpha = 0xFF;
					++pColor;
				} while (--uNumberColorTables);
			}

			uint_t uNumberOfScanLines = pInput->GetShort();
			if (!uNumberOfScanLines) {
				pBadNews = "The image has a height of zero.";
				break;
			}
			pImage = Image::new_object(uPixelsPerScanLine,uNumberOfScanLines,Image::PIXELTYPE8BIT);
			if (!pImage) {
				pBadNews = "Memory error in creating Image.";
				break;
			}

			// Create a buffer for the scan line description

			ScanLineDescription_t *pScanLineDescription = static_cast<ScanLineDescription_t *>(allocate_memory(sizeof(ScanLineDescription_t)*uNumberOfScanLines + (uPixelsPerScanLine)));
			if (!pScanLineDescription) {
				pBadNews = "Memory error in temporary buffer.";
				break;
			}

			{
				// Fetch the buffer
				ScanLineDescription_t *pScanWork = pScanLineDescription;
				uint_t uCounter = uNumberOfScanLines;
				do {
					pScanWork->m_uChunkLength = pInput->GetShort();
					pScanWork->m_uScanLineControl = pInput->GetShort();
					++pScanWork;
				} while (--uCounter);
			}

			// Perform the decompression
			{
				uint8_t *pDest = pImage->GetImage();
				ScanLineDescription_t *pScanWork = pScanLineDescription;
				uint8_t *pTemp = reinterpret_cast<uint8_t*>(&pScanLineDescription[uNumberOfScanLines]);
				uint_t uBytesPerScanLine = (uPixelsPerScanLine+1)>>1U;
				do {

					// These values will be updated by a call to UnpackBytes, so
					// they must be refreshed

					uint8_t *pUnpacked = pTemp;
					uintptr_t uUnpackedSize = (uPixelsPerScanLine+1)>>1U;

					// Decompress the data
					if (UnpackBytes(pInput->GetPtr(),pScanWork->m_uChunkLength,&pUnpacked,&uUnpackedSize)!=pScanWork->m_uChunkLength) {
						pBadNews = "Decompression error";
						break;
					}
					pInput->SkipForward(pScanWork->m_uChunkLength);

					// Convert from 4 bits per pixel to 8 bits per pixel
					uint_t i = 0;

					// Using the palette index, look up the correct palette color
					uint_t uColorBase = (pScanWork->m_uScanLineControl&0xFU)<<4U;
					do {
						pDest[i*2] = static_cast<uint8_t>((pTemp[i]>>4)+uColorBase);
						pDest[(i*2)+1] = static_cast<uint8_t>((pTemp[i]&0xF)+uColorBase);
					} while (++i<uBytesPerScanLine);
					++pScanWork;
					pDest+=pImage->GetStride();
				} while (--uNumberOfScanLines);
			}
			free_memory(pScanLineDescription);
			break;
		}

		// Jump to the next chunk
		pInput->SetMark(uMark+uChunkSize);
	}

	// If there was an error, clean up

	if (pBadNews) {
		Debug::Warning(pBadNews);
		delete_object(pImage);
		pImage = NULL;
	}
	return pImage;
}

/*! ************************************

	\fn const RGBAWord8_t *Burger::FileAPF::GetPalette(void) const
	\brief Get the file image's palette

	Obtain a constant pointer to the 256 entry RGBA color palette found in the APF file

	\return Constant pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileAPF::GetPalette(void)

***************************************/

/*! ************************************

	\fn RGBAWord8_t *Burger::FileAPF::GetPalette(void)
	\brief Get the file image's palette

	Obtain a pointer to the 256 entry RGBA color palette found in the APF file

	\return Pointer to the 256 entry RGBAWord8_t palette
	\sa Burger::FileAPF::GetPalette(void) const

***************************************/

/*! ************************************

	\fn void Burger::FileAPF::SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
	\brief Set the file image's palette (RGB)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit APF file. Since the Alpha is missing from the
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

	\fn void Burger::FileAPF::SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
	\brief Set the file image's palette (RGBA)

	Given a pointer to a palette, copy the colors into this class
	for writing to an 8 bit APF file.

	If the starting index is greater than 255, this function does nothing. If
	the color entry count exceeds 256 colors, the remaining colors are ignored
	to prevent buffer overruns

	\param pInput Pointer to the palette to copy
	\param uStartIndex Color index of the 256 color internal palette to start modification
	\param uPaletteSize Number of color entries in the palette (Maximum 256)
	\sa SetPalette(const RGBWord8_t *,uint_t,uint_t)

***************************************/
