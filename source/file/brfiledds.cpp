/***************************************

	DDS File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfiledds.h"
#include "brdebug.h"
#include "brrenderer.h"

/*! ************************************

	\class Burger::FileDDS
	\brief Reads and writes DDS files

	This class will read and write out Direct Draw Surface (DDS) files
	and use Burger::Image class instances as data transporters

***************************************/

//
// Header of a DDS file (Little Endian, 128 bytes)
//

#if !defined(DOXYGEN)

#define DDS_MAGIC 0x20534444	// 'DDS '

#define DDS_FOURCC 0x00000004		// DDPF_FOURCC
#define DDS_RGB 0x00000040			// DDPF_RGB
#define DDS_RGBA 0x00000041			// DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE 0x00020000	// DDPF_LUMINANCE
#define DDS_ALPHA 0x00000002		// DDPF_ALPHA

#define DDS_DXT1 0x31545844		// 'DXT1'
#define DDS_DXT2 0x32545844		// 'DXT2'
#define DDS_DXT3 0x33545844		// 'DXT3'
#define DDS_DXT4 0x34545844		// 'DXT4'
#define DDS_DXT5 0x35545844		// 'DXT5'

struct DDS_PIXELFORMAT {
	Word32 m_uSize;
	Word32 m_uFlags;		// Usually DDPF_RGB for raw and DDS_FOURCC for compressed
	Word32 m_uFourCC;		// Compression codec
	Word32 m_uRGBBitCount;
	Word32 m_uRBitMask;
	Word32 m_uGBitMask;
	Word32 m_uBBitMask;
	Word32 m_uABitMask;
};

#define DDS_HEADER_FLAGS_TEXTURE 0x00001007U		// DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP 0x00020000U			// DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME 0x00800000U			// DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH 0x00000008U			// DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE 0x00080000U		// DDSD_LINEARSIZE

struct DDSHeader {			// This is the header for a DDS file
	Word32 m_uMagic;		// Image identification field 'DDS '
	Word32 m_uSize;
	Word32 m_uHeaderFlags;
	Word32 m_uHeight;
	Word32 m_uWidth;
	Word32 m_uPitchOrLinearSize;
	Word32 m_uDepth;			// only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
	Word32 m_uMipMapCount;
	Word32 m_uReserved1[11];
	DDS_PIXELFORMAT m_PixelFormat;
	Word32 m_uSurfaceFlags;
	Word32 m_uCubemapFlags;
	Word32 m_uReserved2[3];
};

#endif

/*! ************************************

	\brief Default constructor

	Initializes all extra file data to defaults

***************************************/

Burger::FileDDS::FileDDS()
{
	WordPtr uIndex = 0;
	do {
		m_uReserved[uIndex] = 0;
	} while (++uIndex<BURGER_ARRAYSIZE(m_uReserved));
}

/*! ************************************

	\brief Parses a DDS file

	Read in a DDS file and set the extra variables

	Will parse 16, 24, 32 bit uncompressed and DXT compressed DDS files only.
	Other formats are not supported

	\note A descriptive error message is passed to Debug::Warning()

	\param pInput Pointer to a memory stream that has a DDS file image

	\return A pointer to a valid Image on success or \ref NULL on failure

***************************************/

Burger::Image * Burger::FileDDS::Load(InputMemoryStream *pInput)
{
	const char *pBadNews = NULL;
	Image *pImage = NULL;
	Word32 uWidth = 0;
	Word32 uHeight = 0;
	Word32 uDepth = 0;
	Word32 uPixelFormatFlags = 0;
	Word32 uMipMapCount = 1;
	Image::ePixelTypes eType = Image::PIXELTYPE8888;

	// Start with processing the 128 byte header of the DDS file
	if (pInput->BytesRemaining()<128) {
		pBadNews = "Insufficient data for DDS file header.";
	} else {
		Word uMagic = pInput->GetWord32();
		if (uMagic!=DDS_MAGIC) {
			pBadNews = "Invalid DDS header.";
		} else {

			// Read in the start of the header

			Word32 uSize = pInput->GetWord32();
			Word32 uHeaderFlags = pInput->GetWord32();
			uHeight = pInput->GetWord32();
			uWidth = pInput->GetWord32();
			/* Word32 uPitchOrLinearSize = */ pInput->GetWord32();
			/* Word32 uVolumeDepth = */ pInput->GetWord32();		// Z for 3D textures
			uMipMapCount = pInput->GetWord32();
			WordPtr i = 0;
			do {
				m_uReserved[i] = pInput->GetWord32();
			} while (++i<BURGER_ARRAYSIZE(m_uReserved));
			
			// Read in the Pixel format
			Word32 uPixelFormatSize = pInput->GetWord32();
			uPixelFormatFlags = pInput->GetWord32();
			Word32 uFourCC = pInput->GetWord32();
			uDepth = pInput->GetWord32();
			Word32 uRBitMask = pInput->GetWord32();
			Word32 uGBitMask = pInput->GetWord32();
			Word32 uBBitMask = pInput->GetWord32();
			Word32 uABitMask = pInput->GetWord32();

			// Read in the remainder of the header
			/* Word32 uSurfaceFlags = */ pInput->GetWord32();
			/* Word32 uCubemapFlags = */ pInput->GetWord32();
			// Skip the rest of the reserved data
			pInput->SkipForward(3*4);

			// Sanity checks

			if (uSize!=124) {
				pBadNews = "Invalid DDS size.";
			} else if (uPixelFormatSize!=32) {
				pBadNews = "Invalid Pixelformat size.";
			} else if ((uHeaderFlags&DDS_HEADER_FLAGS_TEXTURE)!=DDS_HEADER_FLAGS_TEXTURE) {
				pBadNews = "Invalid texture flags.";
			} else if (!uWidth) {
				pBadNews = "Image width is zero.";
			} else if (!uHeight) {
				pBadNews = "Image height is zero.";
			} else {
				// Force mip map count to 1 if no mip maps

				if (!(DDS_HEADER_FLAGS_MIPMAP&uHeaderFlags) || (!uMipMapCount)) {
					uMipMapCount = 1;
				}

				// Let's determine the pixel type
				// Standard RGB?
				if (uPixelFormatFlags&DDS_RGB) {

					if (uDepth==24) {
						eType = Image::PIXELTYPE888;

					} else if (uDepth==32) {
						eType = Image::PIXELTYPE8888;

					} else if (uDepth==16) {
						if ((uRBitMask==0xF800) && 
							(uGBitMask==0x07E0) &&
							(uBBitMask==0x001F) &&
							(uABitMask==0)) {
							eType = Image::PIXELTYPE565;
						} else if ((uRBitMask==0x7C00) && 
							(uGBitMask==0x03E0) &&
							(uBBitMask==0x001F) &&
							(uABitMask==0x8000)) {
							eType = Image::PIXELTYPE1555;
						} else if ((uRBitMask==0xF00) && 
							(uGBitMask==0x00F0) &&
							(uBBitMask==0x000F) &&
							(uABitMask==0xF000)) {
							eType = Image::PIXELTYPE4444;
						} else {
							pBadNews = "Unsupported 16 bit pixel type.";
						}
					} else if ((uDepth==8) &&
						(uRBitMask==0xE0) &&
						(uGBitMask==0x1C) &&
						(uBBitMask==0x03) &&
						(uABitMask==0x00)) {
						eType = Image::PIXELTYPE332;
					} else {
						pBadNews = "Unsupported raw bit depth.";
					}
					// FourCC?
				} else if (uPixelFormatFlags&DDS_FOURCC) {
					switch (uFourCC) {
					default:
						pBadNews = "Unknown four CC code.";
						break;
					case DDS_DXT1:
						eType = Image::PIXELTYPEDXT1;
						break;
					case DDS_DXT2:
						eType = Image::PIXELTYPEDXT2;
						break;
					case DDS_DXT3:
						eType = Image::PIXELTYPEDXT3;
						break;
					case DDS_DXT4:
						eType = Image::PIXELTYPEDXT4;
						break;
					case DDS_DXT5:
						eType = Image::PIXELTYPEDXT5;
						break;
					}
				} else {
					pBadNews = "Unknown pixel format.";
				}
			}
		}
	}
	if (!pBadNews) {
		// Get the input buffer
		pImage = Image::New(uWidth,uHeight,eType,uMipMapCount);
		if (pImage) {
			WordPtr uLength = Image::GetSuggestedBufferSize(uWidth,uHeight,eType,uMipMapCount);
			Word8 *pDest = pImage->GetImage();
			if (uDepth==8) {
				// Copy in one shot!
				pInput->Get(pDest,uLength);
			} else if (uDepth == 16) {			// 16 bit
				Word16 uMask;
				if (eType==Image::PIXELTYPE555) {
					uMask = 0x7FFF;
				} else {
					uMask = 0xFFFFU;
				}
				do {
					reinterpret_cast<Word16 *>(pDest)[0] = static_cast<Word16>(pInput->GetShort()&uMask);	// Save pixel
					pDest=pDest+2;
					uLength-=2;
				} while (uLength);		// All done?
			} else if (eType==Image::PIXELTYPE888) {
				do {
					pDest[2] = pInput->GetByte();	// Blue
					pDest[1] = pInput->GetByte();	// Green
					pDest[0] = pInput->GetByte();	// Red
					pDest = pDest+3;
					uLength -= 3;
				} while (uLength);
			} else if (eType==Image::PIXELTYPE8888) {

				// If the file doesn't have an alpha, add one
				Word uMask = (uPixelFormatFlags&1) ? 0x00 : 0xFFU;
				do {
					pDest[2] = pInput->GetByte();	// Blue
					pDest[1] = pInput->GetByte();	// Green
					pDest[0] = pInput->GetByte();	// Red
					pDest[3] = static_cast<Word8>(pInput->GetByte()|uMask);	// Alpha
					pDest = pDest+4;
					uLength -= 4;
				} while (uLength);				// All done?
			} else {
				// DXT1-5 just copies down
				pInput->Get(pDest,uLength);
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

	\brief Write an Image structure out as a DDS file image

	Given an image in true color and DXT compressed formats, write it out as a DDS
	file.

	\param pOutput Pointer to the output stream to store the file image
	\param pImage Pointer to a valid Image structure containing the image data

	\return Zero if successful

***************************************/

Word Burger::FileDDS::Save(OutputMemoryStream *pOutput,const Image *pImage)
{
	Word32 uPixelFlags = 0;
	Word32 uFourCC = 0;
	Word32 uBitCount = 0;
	Word32 uRBitMask = 0;
	Word32 uGBitMask = 0;
	Word32 uBBitMask = 0;
	Word32 uABitMask = 0;

	Image::ePixelTypes eType = pImage->GetType();
	// Certain formats are supported
	switch (eType) {
	case Image::PIXELTYPE332:
		uPixelFlags = DDS_RGB;
		uBitCount = 8;
		uRBitMask = 0xE0;
		uGBitMask = 0x1C;
		uBBitMask = 0x03;
		break;
	case Image::PIXELTYPE555:
		uPixelFlags = DDS_RGB;
		uBitCount = 16;
		uRBitMask = 0x7C00;
		uGBitMask = 0x03E0;
		uBBitMask = 0x001F;
		break;
	case Image::PIXELTYPE1555:
		uPixelFlags = DDS_RGBA;
		uBitCount = 16;
		uRBitMask = 0x7C00;
		uGBitMask = 0x03E0;
		uBBitMask = 0x001F;
		uABitMask = 0x8000;
		break;
	case Image::PIXELTYPE565:
		uPixelFlags = DDS_RGB;
		uBitCount = 16;
		uRBitMask = 0xF800;
		uGBitMask = 0x07E0;
		uBBitMask = 0x001F;
		break;
	case Image::PIXELTYPE4444:
		uPixelFlags = DDS_RGBA;
		uBitCount = 16;
		uRBitMask = 0x0F00;
		uGBitMask = 0x00F0;
		uBBitMask = 0x000F;
		uABitMask = 0xF000;
		break;
	case Image::PIXELTYPE888:
		uPixelFlags = DDS_RGB;
		uBitCount = 24;
		uRBitMask = 0xFF0000;
		uGBitMask = 0x00FF00;
		uBBitMask = 0x0000FF;
		break;
	case Image::PIXELTYPE8888:
		uPixelFlags = DDS_RGBA;
		uBitCount = 32;
		uRBitMask = 0x00FF0000U;
		uGBitMask = 0x0000FF00U;
		uBBitMask = 0x000000FFU;
		uABitMask = 0xFF000000U;
		break;

	// Compressed formats
	case Image::PIXELTYPEDXT1:
		uFourCC = DDS_DXT1;
		uPixelFlags = DDS_FOURCC;
		break;
	case Image::PIXELTYPEDXT2:
		uFourCC = DDS_DXT2;
		uPixelFlags = DDS_FOURCC;
		break;
	case Image::PIXELTYPEDXT3:
		uFourCC = DDS_DXT3;
		uPixelFlags = DDS_FOURCC;
		break;
	case Image::PIXELTYPEDXT4:
		uFourCC = DDS_DXT4;
		uPixelFlags = DDS_FOURCC;
		break;
	case Image::PIXELTYPEDXT5:
		uFourCC = DDS_DXT5;
		uPixelFlags = DDS_FOURCC;
		break;
	default:
		return 10;
	}
	Word32 uWidth = static_cast<Word32>(pImage->GetWidth());
	Word32 uHeight = static_cast<Word32>(pImage->GetHeight());
	pOutput->Append(static_cast<Word32>(DDS_MAGIC));	// DDS ID
	pOutput->Append(static_cast<Word32>(124));			// Size of the structure
	pOutput->Append(static_cast<Word32>(DDS_HEADER_FLAGS_TEXTURE));	// Texture
	pOutput->Append(uWidth);
	pOutput->Append(uHeight);
	pOutput->Append(static_cast<Word32>(0));			// PitchOrLinearSize
	pOutput->Append(static_cast<Word32>(0));			// Z Depth
	pOutput->Append(static_cast<Word32>(0));			// Mip Map count

	WordPtr i = 0;
	do {
		pOutput->Append(m_uReserved[i]);
	} while (++i<BURGER_ARRAYSIZE(m_uReserved));

	pOutput->Append(static_cast<Word32>(32));			// Pixel map format
	pOutput->Append(uPixelFlags);			// Pixel flags
	pOutput->Append(uFourCC);				// Texture code
	pOutput->Append(uBitCount);				// Bit depth
	pOutput->Append(uRBitMask);				// Texture bit masks
	pOutput->Append(uGBitMask);
	pOutput->Append(uBBitMask);
	pOutput->Append(uABitMask);

	pOutput->Append(static_cast<Word32>(0));			// Surface flags
	pOutput->Append(static_cast<Word32>(0));			// Cubemap flags
	pOutput->Append(static_cast<Word32>(0));			// Reserved
	pOutput->Append(static_cast<Word32>(0));			// Reserved
	pOutput->Append(static_cast<Word32>(0));			// Reserved

	if (uWidth && uHeight) {
		const Word8 *pData = pImage->GetImage();
		WordPtr uStride = pImage->GetStride();
		switch (eType) {
		case Image::PIXELTYPE332:
			do {
				pOutput->Append(pData,uWidth);
				pData += uStride;
			} while (--uHeight);
			break;
		case Image::PIXELTYPE555:
		case Image::PIXELTYPE1555:
		case Image::PIXELTYPE565:
		case Image::PIXELTYPE4444:
			{
				Word32 Zero = 0;
				WordPtr uPad = (0-uWidth*2)&3;
				do {
					Word uLength = uWidth;
					const Word8 *pDest = pData;
					do {
						pOutput->Append(reinterpret_cast<const Word16 *>(pDest)[0]);	// Pixel
						pDest = pDest+2;
					} while (--uLength);			// All done?
					pData += uStride;
					pOutput->Append(&Zero,uPad);
				} while (--uHeight);
			}
			break;
		case Image::PIXELTYPE888:
			{
				Word32 Zero = 0;
				WordPtr uPad = (0-uWidth*3)&3;
				do {
					Word uLength = uWidth;
					const Word8 *pDest = pData;
					do {
						pOutput->Append(pDest[2]);	// Blue
						pOutput->Append(pDest[1]);	// Green
						pOutput->Append(pDest[0]);	// Red
						pDest = pDest+3;
					} while (--uLength);			// All done?
					pData += uStride;
					pOutput->Append(&Zero,uPad);
				} while (--uHeight);
			}
			break;
		case Image::PIXELTYPE8888:
			{
				do {
					Word uLength = uWidth;
					const Word8 *pDest = pData;
					do {
						pOutput->Append(pDest[2]);	// Blue
						pOutput->Append(pDest[1]);	// Green
						pOutput->Append(pDest[0]);	// Red
						pOutput->Append(pDest[3]);	// Alpha
						pDest = pDest+4;
					} while (--uLength);			// All done?
					pData += uStride;
				} while (--uHeight);
			}
			break;

			// Compressed formats
		case Image::PIXELTYPEDXT1:
		case Image::PIXELTYPEDXT2:
		case Image::PIXELTYPEDXT3:
		case Image::PIXELTYPEDXT4:
		case Image::PIXELTYPEDXT5:
			{
				uHeight = (uHeight+3)>>2;
				WordPtr uLine = Image::GetSuggestedStride(uWidth,eType);
				do {
					pOutput->Append(pData,uLine);
					pData += uStride;
				} while (--uHeight);
			}
			break;
		default:
			break;
		}
	}
	return FALSE;
}