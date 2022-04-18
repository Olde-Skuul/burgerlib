/***************************************

    Graphic file format transport class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brimage.h"
#include "brglobalmemorymanager.h"
#include "brpalette.h"
#include "brrenderer.h"
#include "brfilepng.h"
#include "brfilegif.h"
#include "brfilebmp.h"
#include "brfiletga.h"

/*! ************************************

	\class Burger::Image
	\brief Class for containing raw images
	
	This class is the mechanism for loading and saving popular file formats
	for image files such as TGA, LBM, PCX, PNG and the like. It's used as
	input for the Burger::Texture manager to convert the data from an image into
	a hardware texture for GPU rendering. Graphics tools can
	use this class exclusively for image conversion and manipulation

	\sa Burger::FileBMP and Burger::FileTGA

***************************************/

/*! ************************************

	\enum Burger::Image::ePixelTypes
	\brief Pixel data type
	
	An enumeration to declare how each pixel is formatted.

***************************************/

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty image

***************************************/

Burger::Image::Image() :
	m_pImage(NULL),
	m_uStride(0),
	m_uWidth(0),
	m_uHeight(0),
	m_uFlags(0),
	m_eType(PIXELTYPE8888)
{
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::Image::~Image()
{
	Shutdown();
}

/*! ************************************

	\brief Returns a pointer to a newly allocated instance of an \ref Image

	Creates an empty \ref Image object.

	Release this object with a call to delete

	\code 
		Image *pFoo = Burger::Image::New();
		if (pFoo) {
			DoStuff(pFoo);
			Delete(pFoo);
		}
	\endcode

	\return A valid pointer if memory is available or \ref NULL if out of memory
	\sa New(uint_t,uint_t,ePixelTypes)

***************************************/

Burger::Image * BURGER_API Burger::Image::New(void)
{
	// Manually allocate the memory
	Image *pThis = new (Alloc(sizeof(Image))) Image();
	return pThis;
}

/*! ************************************

	\brief Returns a pointer to a copy of the image

	Makes a duplicate of this image and returns a pointer to the new instance.

	Release this object with a call to delete

	\code 
		Image *pFoo = Burger::Image::New(pOldImage);
		if (pFoo) {
			DoStuff(pFoo);
			Delete(pFoo);
		}
	\endcode

	\param pInput Pointer to a valid Image to copy from
	\return A valid pointer if memory is available or \ref NULL if out of memory
	\sa New(void), New(uint_t,uint_t,ePixelTypes)

***************************************/

Burger::Image* BURGER_API Burger::Image::New(const Image *pInput)
{
	// Manually allocate the memory
	Image *pThis = new (Alloc(sizeof(Image))) Image();
	if (pThis) {
		if (pThis->Init(pInput)) {
			Delete(pThis);
			pThis = NULL;
		}
	}
	return pThis;
}

/*! ************************************

	\brief Returns a pointer to a newly allocated instance of an \ref Image

	Creates an object of the specified type. All memory is
	initialized.

	Release this object with a call to delete

	\code 
		// Create memory for a 64x128 32 bits per pixel image
		Image *pFoo = Burger::Image::New(64,128,Image::PIXELTYPE8888);
		if (pFoo) {
			DoStuff(pFoo);
			Delete(pFoo);
		}
	\endcode

	\param uWidth Width of the image in pixels
	\param uHeight Height of the image in pixels
	\param eType Type of pixel data

	\return A valid pointer if memory is available or \ref NULL if out of memory
	\sa Init(uint_t,uint_t,ePixelTypes), New(void)

***************************************/

Burger::Image * BURGER_API Burger::Image::New(uint_t uWidth,uint_t uHeight,ePixelTypes eType)
{
	// Manually allocate the memory
	Image *pThis = new (Alloc(sizeof(Image))) Image();
	if (pThis) {
		// Invoke the constructor manually
		if (pThis->Init(uWidth,uHeight,eType)) {
			Delete(pThis);
			pThis = NULL;
		}
	}
	return pThis;
}

/*! ************************************

	\brief Returns a pointer to a newly allocated instance of an \ref Image

	Creates an object of the specified type. All memory is
	initialized.

	Release this object with a call to delete

	\code 
		// Create memory for a 256x256 32 bits per pixel image with 3 mip maps (256x256,128x128,64x64)
		Image *pFoo = Burger::Image::New(256,256,Image::PIXELTYPE8888,3);
		if (pFoo) {
			DoStuff(pFoo);
			Delete(pFoo);
		}
	\endcode

	\param uWidth Width of the image in pixels
	\param uHeight Height of the image in pixels
	\param eType Type of pixel data
	\param uMipMaps Number of levels of mipmaps for the image

	\return A valid pointer if memory is available or \ref NULL if out of memory
	\sa Init(uint_t,uint_t,ePixelTypes,uint_t), New(void)

***************************************/

Burger::Image * BURGER_API Burger::Image::New(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMaps)
{
	// Manually allocate the memory
	Image *pThis = new (Alloc(sizeof(Image))) Image();
	if (pThis) {
		// Invoke the constructor manually
		if (pThis->Init(uWidth,uHeight,eType,uMipMaps)) {
			Delete(pThis);
			pThis = NULL;
		}
	}
	return pThis;
}

/*! ************************************

	\brief Manually initialize an Image

	Given all entries for an Image record, initialize it.
	If there was a previous buffer, it's released.

	\note If the buffer is manually controlled, pass FLAGS_IMAGENOTALLOCATED in the uFlags parameter.

	\param uWidth Width of the new image in pixels
	\param uHeight Height of the new image in pixels
	\param eType Pixel type of the new image
	\param uStride Bytes per scan line of the new image
	\param pImage Pointer to the pixels of the new image
	\param uFlags Flag controlling whether or not the class will call Free() on pImage on shutdown
	\param uMipMaps Number of mip maps in the image (Max 16)
	\sa Shutdown(void), Init(const Image *)

***************************************/

void Burger::Image::Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uintptr_t uStride,const uint8_t *pImage,uint_t uFlags,uint_t uMipMaps)
{
	if (!(m_uFlags&FLAGS_IMAGENOTALLOCATED)) {
		Free(m_pImage);		// Dispose of the memory
	}
	m_pImage = const_cast<uint8_t*>(pImage);
	m_uStride = static_cast<uint_t>(uStride);
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	uFlags &= static_cast<uint_t>(~FLAGS_MIPMAPMASK);
	if (uMipMaps>16) {
		uMipMaps = 16;
	}
	m_uFlags = uFlags|uMipMaps;
	m_eType = eType;
}

/*! ************************************

	\brief Allocates memory suitable for an image

	Given a width, height and pixel depth of a requested image type,
	allocate an uninitialized buffer and set up the constants
	to describe this data.

	If the image dimensions/type is already a match, this function returns
	immediately. If not, any previous data is released and new data is allocated.

	\param uWidth Width of the new image in pixels
	\param uHeight Height of the new image in pixels
	\param eType Pixel type of the new image
	\return Zero on success, non-zero on failure
	\sa Shutdown(void), Init(const Image *)

***************************************/

uint_t Burger::Image::Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType)
{
	uint_t uResult = 0;
	if ((!m_pImage) ||
		(m_uWidth!=uWidth) ||
		(m_uHeight!=uHeight) ||
		(m_eType!=eType) ||
		((m_uFlags&FLAGS_MIPMAPMASK)>=2)) {
		// Release the previous image, if any
		Shutdown();
		m_eType = eType;
		uintptr_t uSize = GetSuggestedBufferSize(uWidth,uHeight,eType);
		if (uSize) {
			uResult = 10;
			void *pData = Alloc(uSize);
			if (pData) {
				// Initialize the record
				m_pImage = static_cast<uint8_t*>(pData);
				m_uWidth = uWidth;
				m_uHeight = uHeight;
				m_uStride = static_cast<uint_t>(GetSuggestedStride(uWidth,eType));
				uResult = 0;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Allocates memory suitable for an image and its mipmaps

	Given a width, height, pixel depth and mip map count, of a requested image type,
	allocate an uninitialized buffer and set up the constants
	to describe this data.

	If the image dimensions/type is already a match, this function returns
	immediately. If not, any previous data is released and new data is allocated.

	\param uWidth Width of the new image in pixels
	\param uHeight Height of the new image in pixels
	\param eType Pixel type of the new image
	\param uMipMaps Number of mip maps requested
	\return Zero on success, non-zero on failure
	\sa Shutdown(void), Init(const Image *)

***************************************/

uint_t Burger::Image::Init(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMaps)
{
	uint_t uResult = 0;
	if ((!m_pImage) ||
		(m_uWidth!=uWidth) ||
		(m_uHeight!=uHeight) ||
		(m_eType!=eType) ||
		((m_uFlags&FLAGS_MIPMAPMASK)!=uMipMaps)) {
		// Release the previous image, if any
		Shutdown();
		m_eType = eType;
		uintptr_t uSize = GetSuggestedBufferSize(uWidth,uHeight,eType,uMipMaps);
		if (uSize) {
			uResult = 10;
			void *pData = Alloc(uSize);
			if (pData) {
				// Initialize the record
				m_pImage = static_cast<uint8_t*>(pData);
				m_uWidth = uWidth;
				m_uHeight = uHeight;
				m_uFlags = uMipMaps;
				m_uStride = static_cast<uint_t>(GetSuggestedStride(uWidth,eType));
				uResult = 0;
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Make a copy of an image

	Given an image, make a copy of it in this image

	\param pInput Pointer to a valid image to copy from
	\return Zero on success, non-zero on failure
	\sa Shutdown(void), Init(uint_t,uint_t,ePixelTypes)

***************************************/

uint_t Burger::Image::Init(const Image *pInput)
{
	uint_t uResult = 1;
	if (!Init(pInput->m_uWidth,pInput->m_uHeight,pInput->m_eType)) {
		// The buffer is ready!
		// The image has size, so get the bits per pixel for
		// an uncompressed image type
		uResult = 0;
		uint_t uHeight = m_uHeight;
		uint_t uWidth = m_uWidth;
		if (uWidth && uHeight) {
			uint_t uPixels = GetBitsPerPixel();
			if (uPixels) {
				uPixels = ((uPixels*uWidth)+7)>>3;
			} else {
				// Calculate the number of 4x4 pixel blocks needed
				uPixels = (uWidth+3)>>2;
				// Adjust the height by 4x4 cells
				uHeight = (uHeight+3)>>2;
				uPixels = uPixels*16;
				if (m_eType==PIXELTYPEDXT1) {
					// DXT1 only uses 8 bytes per cell, not 16
					uPixels>>=1U;
				}
			}
			// Perform the memory clear operation
			const uint8_t *pSource = pInput->m_pImage;
			uint8_t *pWork = m_pImage;
			uintptr_t uSourceStride = pInput->m_uStride;
			uintptr_t uStride = m_uStride;
			do {
				MemoryCopy(pWork,pSource,uPixels);
				pSource += uSourceStride;
				pWork += uStride;
			} while (--uHeight);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Initialize an Image from a PNG file stream

	Given a InputMemoryStream containing a PNG file,
	convert the contents into an Image.

	If this is an animated PNG file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pInput Pointer to a stream with a PNG file inside
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitPNG(const char *,RGBAWord8_t *), InitPNG(Filename *,RGBAWord8_t *), InitPNG(RezFile *,uint_t,RGBAWord8_t *) or FilePNG::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitPNG(InputMemoryStream *pInput,RGBAWord8_t *pPalette)
{
	// Graphic file translator
	FilePNG Loader;
	uint_t uResult = 10;
	// Failure on load or an invalid stream?
	if (!pInput->IsEmpty()) {
		// Convert to an Image
		uResult = Loader.Load(this,pInput);
	}
	// Was a palette requested?
	if (pPalette) {
		// Error?
		if (uResult) {
			// Clear the palette on error
			ClearPalette(pPalette);
		} else {
			// Copy on success
			MemoryCopy(pPalette,Loader.GetPalette(),sizeof(pPalette[0])*256);
		}
	}
	// Return 0 on success!
	return uResult;
}

/*! ************************************

	\brief Initialize an Image from a PNG file

	Given a Burgerlib format filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated PNG file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitPNG(InputMemoryStream *,RGBAWord8_t *), InitPNG(Filename *,RGBAWord8_t *), InitPNG(RezFile *,uint_t,RGBAWord8_t *) or FilePNG::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitPNG(const char *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitPNG(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a PNG file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated PNG file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib Filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitPNG(InputMemoryStream *,RGBAWord8_t *), InitPNG(const char *,RGBAWord8_t *), InitPNG(RezFile *,uint_t,RGBAWord8_t *) or FilePNG::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitPNG(Filename *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitPNG(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a PNG file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated PNG file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Index number of the file in the RezFile
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitPNG(InputMemoryStream *,RGBAWord8_t *), InitPNG(Filename *,RGBAWord8_t *), InitPNG(const char *,RGBAWord8_t *) or FilePNG::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitPNG(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette)
{
	// Load into a memory stream7
	InputRezStream Input(pRezFile,uRezNum);
	return InitPNG(&Input,pPalette);
}


/*! ************************************

	\brief Initialize an Image from a GIF file stream

	Given a InputMemoryStream containing a GIF file,
	convert the contents into an Image.

	If this is an animated GIF file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note The palette will be returned to 
	the buffer pointed to by pPalette. If there was an error, the palette
	will be cleared to black with an alpha set to 255,

	\param pInput Pointer to a stream with a GIF file inside
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitGIF(const char *,RGBAWord8_t *), InitGIF(Filename *,RGBAWord8_t *), InitGIF(RezFile *,uint_t,RGBAWord8_t *) or FileGIF::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitGIF(InputMemoryStream *pInput,RGBAWord8_t *pPalette)
{
	// Graphic file translator
	FileGIF Loader;
	uint_t uResult = 10;
	// Failure on load or an invalid stream?
	if (!pInput->IsEmpty()) {
		// Convert to an Image
		uResult = Loader.Load(this,pInput);
	}
	// Was a palette requested?
	if (pPalette) {
		// Error?
		if (uResult) {
			// Clear the palette on error
			ClearPalette(pPalette);
		} else {
			// Copy on success
			MemoryCopy(pPalette,Loader.GetPalette(),sizeof(pPalette[0])*256);
		}
	}
	// Return 0 on success!
	return uResult;
}

/*! ************************************

	\brief Initialize an Image from a GIF file

	Given a Burgerlib format filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated GIF file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note The palette will be returned to 
	the buffer pointed to by pPalette. If there was an error, the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitGIF(InputMemoryStream *,RGBAWord8_t *), InitGIF(Filename *,RGBAWord8_t *), InitGIF(RezFile *,uint_t,RGBAWord8_t *) or FileGIF::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitGIF(const char *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitGIF(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a GIF file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated GIF file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note The palette will be returned to 
	the buffer pointed to by pPalette. If there was an error, the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib Filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitGIF(InputMemoryStream *,RGBAWord8_t *), InitGIF(const char *,RGBAWord8_t *), InitGIF(RezFile *,uint_t,RGBAWord8_t *) or FileGIF::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitGIF(Filename *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitGIF(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a GIF file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	If this is an animated GIF file, only load in the first
	frame. This is a convenience routine to simplify loading
	a graphic file into memory.

	\note The palette will be returned to 
	the buffer pointed to by pPalette. If there was an error, the palette
	will be cleared to black with an alpha set to 255,

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Index number of the file in the RezFile
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitGIF(InputMemoryStream *,RGBAWord8_t *), InitGIF(Filename *,RGBAWord8_t *), InitGIF(const char *,RGBAWord8_t *) or FileGIF::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitGIF(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette)
{
	// Load into a memory stream7
	InputRezStream Input(pRezFile,uRezNum);
	return InitGIF(&Input,pPalette);
}


/*! ************************************

	\brief Initialize an Image from a TGA file stream

	Given a InputMemoryStream containing a TGA file,
	convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pInput Pointer to a stream with a TGA file inside
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitPNG(const char *,RGBAWord8_t *), InitTGA(Filename *,RGBAWord8_t *), InitTGA(RezFile *,uint_t,RGBAWord8_t *) or FileTGA::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitTGA(InputMemoryStream *pInput,RGBAWord8_t *pPalette)
{
	// Graphic file translator
	FileTGA Loader;
	uint_t uResult = 10;
	// Failure on load or an invalid stream?
	if (!pInput->IsEmpty()) {
		// Convert to an Image
		uResult = Loader.Load(this,pInput);
	}
	// Was a palette requested?
	if (pPalette) {
		// Error?
		if (uResult) {
			// Clear the palette on error
			ClearPalette(pPalette);
		} else {
			// Copy on success
			MemoryCopy(pPalette,Loader.GetPalette(),sizeof(pPalette[0])*256);
		}
	}
	// Return 0 on success!
	return uResult;
}

/*! ************************************

	\brief Initialize an Image from a TGA file

	Given a Burgerlib format filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitTGA(InputMemoryStream *,RGBAWord8_t *), InitTGA(Filename *,RGBAWord8_t *), InitTGA(RezFile *,uint_t,RGBAWord8_t *) or FileTGA::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitTGA(const char *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitTGA(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a TGA file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib Filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitTGA(InputMemoryStream *,RGBAWord8_t *), InitTGA(const char *,RGBAWord8_t *), InitTGA(RezFile *,uint_t,RGBAWord8_t *) or FileTGA::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitTGA(Filename *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitTGA(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a TGA file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Index number of the file in the RezFile
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitTGA(InputMemoryStream *,RGBAWord8_t *), InitTGA(Filename *,RGBAWord8_t *), InitTGA(const char *,RGBAWord8_t *) or FileTGA::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitTGA(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette)
{
	// Load into a memory stream7
	InputRezStream Input(pRezFile,uRezNum);
	return InitTGA(&Input,pPalette);
}


/*! ************************************

	\brief Initialize an Image from a BMP file stream

	Given a InputMemoryStream containing a BMP file,
	convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pInput Pointer to a stream with a BMP file inside
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitBMP(const char *,RGBAWord8_t *), InitBMP(Filename *,RGBAWord8_t *), InitBMP(RezFile *,uint_t,RGBAWord8_t *) or FileBMP::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitBMP(InputMemoryStream *pInput,RGBAWord8_t *pPalette)
{
	// Graphic file translator
	FileBMP Loader;
	uint_t uResult = 10;
	// Failure on load or an invalid stream?
	if (!pInput->IsEmpty()) {
		// Convert to an Image
		uResult = Loader.Load(this,pInput);
	}
	// Was a palette requested?
	if (pPalette) {
		// Error?
		if (uResult) {
			// Clear the palette on error
			ClearPalette(pPalette);
		} else {
			// Copy on success
			MemoryCopy(pPalette,Loader.GetPalette(),sizeof(pPalette[0])*256);
		}
	}
	// Return 0 on success!
	return uResult;
}

/*! ************************************

	\brief Initialize an Image from a BMP file

	Given a Burgerlib format filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitBMP(InputMemoryStream *,RGBAWord8_t *), InitBMP(Filename *,RGBAWord8_t *), InitBMP(RezFile *,uint_t,RGBAWord8_t *) or FileBMP::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitBMP(const char *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitBMP(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a BMP file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pFilename Pointer to a Burgerlib Filename
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitBMP(InputMemoryStream *,RGBAWord8_t *), InitBMP(const char *,RGBAWord8_t *), InitBMP(RezFile *,uint_t,RGBAWord8_t *) or FileBMP::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitBMP(Filename *pFilename,RGBAWord8_t *pPalette)
{
	// Load into a memory stream
	InputMemoryStream Input(pFilename);
	return InitBMP(&Input,pPalette);
}

/*! ************************************

	\brief Initialize an Image from a BMP file

	Given a Burgerlib Filename, open the file,
	read it in, and convert the contents into an Image.

	\note If the graphic file has a palette, it will be returned to 
	the buffer pointed to by pPalette. If there was an error or if
	the image doesn't have a palette (24/32 bit color), the palette
	will be cleared to black with an alpha set to 255,

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Index number of the file in the RezFile
	\param pPalette Pointer to a 256 entry RGBAWord8_t palette, or \ref NULL if no buffer exists
	\return Zero on success, non-zero on failure
	\sa InitBMP(InputMemoryStream *,RGBAWord8_t *), InitBMP(Filename *,RGBAWord8_t *), InitBMP(const char *,RGBAWord8_t *) or FileBMP::Load(Image *,InputMemoryStream *)

***************************************/

uint_t Burger::Image::InitBMP(RezFile *pRezFile,uint_t uRezNum,RGBAWord8_t *pPalette)
{
	// Load into a memory stream7
	InputRezStream Input(pRezFile,uRezNum);
	return InitBMP(&Input,pPalette);
}


/*! ************************************

	\brief Move an image to the control of another Image

	Given an image, move the control of its contents into another
	Image. The pInput image will have no data within itself and
	can be immediately released or reappropriated.

	No buffers are allocated or released. Only pointers are exchanged.

	This Image will be purged before the transfer is started.

	\param pInput Pointer to a valid image to transfer from
	\sa Shutdown(void), Init(uint_t,uint_t,ePixelTypes)

***************************************/

void Burger::Image::Transfer(Image *pInput)
{
	Shutdown();
	m_pImage = pInput->m_pImage;
	m_uStride = pInput->m_uStride;
	m_uWidth = pInput->m_uWidth;
	m_uHeight = pInput->m_uHeight;
	m_uFlags = pInput->m_uFlags;
	m_eType = pInput->m_eType;
	pInput->m_pImage = NULL;
	pInput->m_uFlags |= FLAGS_IMAGENOTALLOCATED;
}


/*! ************************************

	\brief Releases all memory managed by this class instance

	Releases all memory attached to this image and
	clears out all of the width/height/depth variables.

	\sa Init(uint_t,uint_t,ePixelTypes), Init(const Image *)

***************************************/

void Burger::Image::Shutdown(void)
{
	if (!(m_uFlags&FLAGS_IMAGENOTALLOCATED)) {
		Free(m_pImage);		// Dispose of the memory
	}
	m_pImage = NULL;
	m_uStride = 0;
	m_uFlags &= static_cast<uint_t>(~FLAGS_MIPMAPMASK);
}

/*! ************************************

	\fn const uint8_t * Burger::Image::GetImage(void) const
	\brief Return the base pointer of the image

	Returns a constant pointer to the upper left corner of the pixel image.

	\return \ref NULL if there's no data, or a valid pointer
	\sa GetImage(void) or GetType(void) const

***************************************/

/*! ************************************

	\fn uint8_t * Burger::Image::GetImage(void)
	\brief Return the base pointer of the image

	Returns the pointer to the upper left corner of the pixel image.

	\return \ref NULL if there's no data, or a valid pointer
	\sa GetImage(void) const or GetType(void) const

***************************************/

/*! ************************************

	\brief Return the base pointer of the image

	Returns a constant pointer to the upper left corner of the pixel image.

	\return \ref NULL if there's no data, or a valid pointer
	\sa GetImage(void) or GetType(void) const

***************************************/

const uint8_t *Burger::Image::GetImage(uint_t uMipMap) const
{
	const uint8_t *pImage = m_pImage;
	if (pImage && uMipMap) {
		// Any mip maps available?
		uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
		if (uMaxMip) {
			// Bounds check
			if (uMipMap>=uMaxMip) {
				uMipMap = uMaxMip-1;
			}
			// Iterate over the mip maps until the one is found
			if (uMipMap) {
				uMaxMip = 0;
				do {
					pImage += GetImageSize(uMaxMip);
					++uMaxMip;
				} while (--uMipMap);
			}
		}
	}
	return pImage;
}

/*! ************************************

	\fn uint_t Burger::Image::GetWidth(void) const
	\brief Return the width of the image in pixels

	\return The pixel width
	\sa GetWidth(uint_t) const, GetHeight(void) const or GetType(void) const

***************************************/

/*! ************************************

	\brief Return the width of the image in pixels for the requested mip map

	An index of zero will get the largest (Native sized) mip map,
	an index of one gets the next largest and so on until the
	size of the smallest is found. If a mip map index higher than
	the smallest mip map is passed, the width
	of the smallest available mip map is returned.
	
	\param uMipMap Zero based index into the array of mip maps
	\return The pixel width for the selected mip map
	\sa GetWidth(void) const, GetHeight(uint_t) const or GetType(void) const

***************************************/

uint_t Burger::Image::GetWidth(uint_t uMipMap) const
{
	// Get the pixel width
	uint_t uWidth = m_uWidth;
	// Asking for any mip map?
	if (uMipMap) {
		// Any mip maps available?
		uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
		if (uMaxMip) {
			// Bounds check
			if (uMipMap>=uMaxMip) {
				uMipMap = uMaxMip-1;
			}
			uWidth>>=uMipMap;
		}
	}
	return uWidth;
}

/*! ************************************

	\fn uint_t Burger::Image::GetHeight(void) const
	\brief Return the height of the image in pixels

	\return The pixel height
	\sa GetWidth(void) const or GetType(void) const

***************************************/

/*! ************************************

	\brief Return the height of the image in pixels for the requested mip map

	An index of zero will get the largest (Native sized) mip map,
	an index of one gets the next largest and so on until the
	size of the smallest is found. If a mip map index higher than
	the smallest mip map is passed, the height
	of the smallest available mip map is returned.
	
	\param uMipMap Zero based index into the array of mip maps
	\return The pixel height for the selected mip map
	\sa GetWidth(uint_t) const, GetHeight(void) const or GetType(void) const

***************************************/

uint_t Burger::Image::GetHeight(uint_t uMipMap) const
{
	// Get the pixel height
	uint_t uHeight = m_uHeight;
	// Asking for any mip map?
	if (uMipMap) {
		// Any mip maps available?
		uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
		if (uMaxMip) {
			// Bounds check
			if (uMipMap>=uMaxMip) {
				uMipMap = uMaxMip-1;
			}
			uHeight>>=uMipMap;
		}
	}
	return uHeight;
}

/*! ************************************

	\fn uintptr_t Burger::Image::GetStride(void) const
	\brief Return the number of bytes per scan line

	\return The number of bytes for one scan line of the image
	\sa GetWidth(void) const or GetHeight(void) const

***************************************/

/*! ************************************

	\brief Return the height of the image in pixels for the requested mip map

	An index of zero will get the largest (Native sized) mip map,
	an index of one gets the next largest and so on until the
	size of the smallest is found. If a mip map index higher than
	the smallest mip map is passed, the byte stride
	of the smallest available mip map is returned.
	
	\param uMipMap Zero based index into the array of mip maps
	\return The number of bytes for one scan line for the selected mip map
	\sa GetStride(void) const, GetWidth(uint_t) const, GetHeight(void) const or GetType(void) const

***************************************/

uintptr_t Burger::Image::GetStride(uint_t uMipMap) const
{
	// Get the stride
	uint_t uStride = m_uStride;
	// Asking for any mip map?
	if (uStride && uMipMap) {
		// Any mip maps available?
		uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
		if (uMaxMip) {
			// Bounds check
			if (uMipMap>=uMaxMip) {
				uMipMap = uMaxMip-1;
			}
			// Calculate the stride for a single pixel
			uStride>>=uMipMap;
			// For multibyte textures, failsafe and see if it's smaller than the minimum
			// needed for the proposed pixel width
			uintptr_t uMinStride = GetSuggestedStride(m_uWidth>>uMipMap,m_eType);
			if (uStride<uMinStride) {
				// This fixes 2x2 and 1x1 compressed DXT images
				uStride = static_cast<uint_t>(uMinStride);
			}
		}
	}
	return uStride;
}

/*! ************************************

	\fn ePixelTypes Burger::Image::GetType(void) const
	\brief Return the pixel format type.

	\return The pixel type enumeration
	\sa GetWidth(void) const or GetHeight(void) const

***************************************/

/*! ************************************

	\brief Return the number of available mip maps

	\return 1 through 16 levels of mip maps
	\sa GetWidth(void) const or GetHeight(void) const

***************************************/

uint_t Burger::Image::GetMipMapCount(void) const 
{
	// Get the mip map count
	uint_t uResult = m_uFlags&FLAGS_MIPMAPMASK;
	if (!uResult) { 
		uResult = 1;		// Only one
	}
	return uResult;			// Save the result
}

/*! ************************************

	\brief Given a pixel type, return the bytes needed for each pixel.

	Return 1-4 for number of bytes per pixel. This will return zero
	where bytes per pixel doesn't make sense, like compressed formats
	or 2 or more pixels per byte

	\return 0 if compressed or multiple pixels per byte, 1-4 otherwise
	\sa GetBytesPerPixel(void) const

***************************************/

uint_t BURGER_API Burger::Image::GetBytesPerPixel(ePixelTypes eType)
{
	uint_t uResult;
	switch (eType) {
	default:
	case PIXELTYPE1BIT:			// 1 bit per pixel monochrome
	case PIXELTYPE2BIT:			// 2 bits per pixel indexed by RGB palette
	case PIXELTYPE4BIT:			// 4 bits per pixel indexed by RGB palette
	case PIXELTYPEDXT1:			// Compressed with DXT1
	case PIXELTYPEDXT2:			// Compressed with DXT2
	case PIXELTYPEDXT3:			// Compressed with DXT3
	case PIXELTYPEDXT4:			// Compressed with DXT4
	case PIXELTYPEDXT5:			// Compressed with DXT5
		uResult = 0;
		break;
	case PIXELTYPE332:			// 8 bits per pixel R:3, G:3, B:2
	case PIXELTYPE8BIT:			// 8 bits per pixel indexed RGB palette (Or greyscale)
		uResult = 1;
		break;
	case PIXELTYPE4444:			// 16 bits per pixel native endian A:4, R:4, G:4, B:4
	case PIXELTYPE1555:			// 16 bits per pixel native endian A:1, R:5, G:5, B:5
	case PIXELTYPE555:			// 16 bits per pixel native endian R:5, G:5, B:5
	case PIXELTYPE565:			// 16 bits per pixel native endian R:5, G:6, B:5
		uResult = 2;
		break;
	case PIXELTYPE888:			// 24 bits per pixel in bytes of Red, Green and Blue
		uResult = 3;
		break;
	case PIXELTYPE8888:			// 32 bits per pixel in bytes of Red, Green, Blue and Alpha in that order
	case PIXELTYPE1010102:		// 32 bits per pixel in 10 bits of Red, Green, Blue and 2 bits of Alpha
		uResult = 4;
		break;
	}
	return uResult;
}

/*! ************************************

	\fn uint_t Burger::Image::GetBytesPerPixel(void) const
	\brief Return the bytes needed for each pixel.

	Return 1-4 for number of bytes per pixel. This will return zero
	where bytes per pixel doesn't make sense, like compressed formats
	or 2 or more pixels per byte

	\return 0 if compressed or multiple pixels per byte, 1-4 otherwise
	\sa GetBytesPerPixel(ePixelTypes)

***************************************/

/*! ************************************

	\brief Given a pixel type, return the bits needed for each pixel.

	Return 1-32 for number of bits per pixel. This will return zero
	where bytes per pixel doesn't make sense, like compressed formats.

	\param eType Pixel type to convert to a bit size
	\return 0 for compressed formats, 1-32 for uncompressed types
	\sa GetBitsPerPixel(void) const

***************************************/

uint_t BURGER_API Burger::Image::GetBitsPerPixel(ePixelTypes eType)
{
	uint_t uStride;
	switch (eType) {
	case PIXELTYPE1BIT:			// 1 bit per pixel monochrome
		uStride = 1;
		break;
	case PIXELTYPE2BIT:			// 2 bits per pixel indexed by RGB palette
		uStride = 2;
		break;
	case PIXELTYPE4BIT:			// 4 bits per pixel indexed by RGB palette
		uStride = 4;
		break;
	case PIXELTYPE332:			// 8 bits per pixel R:3, G:3, B:2
	case PIXELTYPE8BIT:			// 8 bits per pixel indexed RGB palette (Or greyscale)
		uStride = 8;
		break;
	case PIXELTYPE4444:			// 16 bits per pixel native endian A:4, R:4, G:4, B:4
	case PIXELTYPE1555:			// 16 bits per pixel native endian A:1, R:5, G:5, B:5
	case PIXELTYPE555:			// 16 bits per pixel native endian R:5, G:5, B:5
	case PIXELTYPE565:			// 16 bits per pixel native endian R:5, G:6, B:5
		uStride = 16;
		break;
		// DXT is special case, since it's in 4x4 blocks with the pixels rounded up
	case PIXELTYPEDXT1:			// Compressed with DXT1
	case PIXELTYPEDXT2:			// Compressed with DXT2
	case PIXELTYPEDXT3:			// Compressed with DXT3
	case PIXELTYPEDXT4:			// Compressed with DXT4
	case PIXELTYPEDXT5:			// Compressed with DXT5
		uStride = 0;
		break;

	case PIXELTYPE888:			// 24 bits per pixel in bytes of Red, Green and Blue
		uStride = 24;
		break;
	default:
	case PIXELTYPE8888:			// 32 bits per pixel in bytes of Red, Green, Blue and Alpha in that order
	case PIXELTYPE1010102:		// 32 bits per pixel in 10 bits of Red, Green, Blue and 2 bits of Alpha
		uStride = 32;
		break;
	}
	return uStride;
}

/*! ************************************

	\fn Burger::Image::GetBitsPerPixel() const
	\brief Return the bits needed for each pixel.

	Return 1-32 for number of bits per pixel. This will return zero
	where bytes per pixel doesn't make sense, like compressed formats.

	\return 0 for compressed formats, 1-32 for uncompressed types
	\sa GetBitsPerPixel(ePixelTypes eType)

***************************************/

/*! ************************************

	\brief Determine the minimum number of bytes for a stride
	
	Given a Width and pixel type, calculate the number of bytes for
	a single scan line of an image.

	Note: Compressed formats will give the stride to the next block group, not the next line.

	\param uWidth The width of the proposed image in pixels
	\param eType The format of a single pixel in the image
	\return The minimum number of bytes needed for a single scan line for this image
	\sa GetSuggestedStride(void) const or GetSuggestedBufferSize(uint_t,uint_t,ePixelTypes)

***************************************/

uintptr_t BURGER_API Burger::Image::GetSuggestedStride(uint_t uWidth,ePixelTypes eType)
{
	uintptr_t uStride;
	switch (eType) {
	case PIXELTYPE1BIT:			// 1 bit per pixel monochrome
		uStride = (uWidth+7)>>3;
		break;
	case PIXELTYPE2BIT:			// 2 bits per pixel indexed by RGB palette
		uStride = (uWidth+3)>>2;
		break;
	case PIXELTYPE4BIT:			// 4 bits per pixel indexed by RGB palette
		uStride = (uWidth+1)>>1;
		break;
	case PIXELTYPE332:			// 8 bits per pixel R:3, G:3, B:2
	case PIXELTYPE8BIT:			// 8 bits per pixel indexed RGB palette (Or greyscale)
		uStride = uWidth;
		break;
	case PIXELTYPE4444:			// 16 bits per pixel native endian A:4, R:4, G:4, B:4
	case PIXELTYPE1555:			// 16 bits per pixel native endian A:1, R:5, G:5, B:5
	case PIXELTYPE555:			// 16 bits per pixel native endian R:5, G:5, B:5
	case PIXELTYPE565:			// 16 bits per pixel native endian R:5, G:6, B:5
		uStride = uWidth*2;
		break;
		// DXT is special case, since it's in 4x4 blocks with the pixels rounded up
	case PIXELTYPEDXT1:			// Compressed with DXT1
	case PIXELTYPEDXT2:			// Compressed with DXT2
	case PIXELTYPEDXT3:			// Compressed with DXT3
	case PIXELTYPEDXT4:			// Compressed with DXT4
	case PIXELTYPEDXT5:			// Compressed with DXT5
		{
			// Calculate the number of 4x4 pixel blocks needed
			uint_t uBlockWidth = (uWidth+3)>>2;
			uStride = uBlockWidth*16;
			if (eType==PIXELTYPEDXT1) {
				// DXT1 only uses 8 bytes per cell, not 16
				uStride>>=1U;
			}
		}
		return uStride;
	case PIXELTYPE888:			// 24 bits per pixel in bytes of Red, Green and Blue
		uStride = uWidth*3;
		break;
	default:
	case PIXELTYPE8888:			// 32 bits per pixel in bytes of Red, Green, Blue and Alpha in that order
	case PIXELTYPE1010102:		// 32 bits per pixel in 10 bits of Red, Green, Blue and 2 bits of Alpha
		uStride = uWidth*4;
		break;
	}
	return uStride;
}


/*! ************************************

	\brief Determine the minimum number of bytes for a stride
	
	Given an Image and using it's pixel format and pixel width, calculate the number of bytes for
	a single scan line of an image.

	\note This function does not return the m_uStride value, use GetStride() const instead.

	\note Compressed formats will give the stride to the next block group, not the next line.

	\return The minimum number of bytes needed for a single scan line for this image
	\sa GetSuggestedStride(uint_t,ePixelTypes) or GetSuggestedBufferSize(uint_t,uint_t,ePixelTypes)

***************************************/

uintptr_t Burger::Image::GetSuggestedStride(void) const
{
	return GetSuggestedStride(m_uWidth,m_eType);
}

/*! ************************************

	\brief Determine the amount of memory needed for an image buffer.
	
	Given a width, height and pixel type, calculate the size of a buffer that would
	contain the entire image. This function is used to allocate an image buffer.

	\param uWidth The width of the proposed image in pixels
	\param uHeight The height of the proposed image in pixels
	\param eType The format of a single pixel in the image
	\return The number of bytes needed to allocate a buffer for this image.
	\sa GetSuggestedBufferSize(uint_t,uint_t,ePixelTypes,uint_t), GetSuggestedStride(uint_t,ePixelTypes) or GetSuggestedBufferSize(void) const

***************************************/

uintptr_t BURGER_API Burger::Image::GetSuggestedBufferSize(uint_t uWidth,uint_t uHeight,ePixelTypes eType)
{
	uintptr_t uStride;
	switch (eType) {
	case PIXELTYPE1BIT:			// 1 bit per pixel monochrome
		uStride = (uWidth+7)>>3;
		break;
	case PIXELTYPE2BIT:			// 2 bits per pixel indexed by RGB palette
		uStride = (uWidth+3)>>2;
		break;
	case PIXELTYPE4BIT:			// 4 bits per pixel indexed by RGB palette
		uStride = (uWidth+1)>>1;
		break;
	case PIXELTYPE332:			// 8 bits per pixel R:3, G:3, B:2
	case PIXELTYPE8BIT:			// 8 bits per pixel indexed RGB palette (Or greyscale)
		uStride = uWidth;
		break;
	case PIXELTYPE4444:			// 16 bits per pixel native endian A:4, R:4, G:4, B:4
	case PIXELTYPE1555:			// 16 bits per pixel native endian A:1, R:5, G:5, B:5
	case PIXELTYPE555:			// 16 bits per pixel native endian R:5, G:5, B:5
	case PIXELTYPE565:			// 16 bits per pixel native endian R:5, G:6, B:5
		uStride = uWidth*2;
		break;
	// DXT is special case, since it's in 4x4 blocks with the pixels rounded up
	case PIXELTYPEDXT1:			// Compressed with DXT1
	case PIXELTYPEDXT2:			// Compressed with DXT2
	case PIXELTYPEDXT3:			// Compressed with DXT3
	case PIXELTYPEDXT4:			// Compressed with DXT4
	case PIXELTYPEDXT5:			// Compressed with DXT5
		{
			// Calculate the number of 4x4 pixel blocks needed
			uint_t uBlockWidth = (uWidth+3)>>2;
			uint_t uBlockHeight = (uHeight+3)>>2;
			uint_t uTotalBlocks = uBlockWidth*uBlockHeight;
			uStride = uTotalBlocks*16;
			if (eType==PIXELTYPEDXT1) {
				// DXT1 only uses 8 bytes per cell, not 16
				uStride>>=1U;
			}
		}
		return uStride;
	case PIXELTYPE888:			// 24 bits per pixel in bytes of Red, Green and Blue
		uStride = uWidth*3;
		break;
	default:
	case PIXELTYPE8888:			// 32 bits per pixel in bytes of Red, Green, Blue and Alpha in that order
	case PIXELTYPE1010102:		// 32 bits per pixel in 10 bits of Red, Green, Blue and 2 bits of Alpha
		uStride = uWidth*4;
		break;
	}
	return uStride*uHeight;	// For simple formats, just multiply the height
}

/*! ************************************

	\brief Determine the amount of memory needed for an image buffer with mip maps.
	
	Given a Width, Height,pixel type and number of levels of mip maps, calculate the size of a buffer that would
	contain the entire image. This function is used to allocate an image buffer.

	\param uWidth The width of the proposed image in pixels
	\param uHeight The height of the proposed image in pixels
	\param eType The format of a single pixel in the image
	\param uMipMapLevels Number of mip map levels deep. Zero is treated as one.
	\return The number of bytes needed to allocate a buffer for this image.
	\sa GetSuggestedStride(uint_t,ePixelTypes) or GetSuggestedBufferSize(void) const

***************************************/

uintptr_t BURGER_API Burger::Image::GetSuggestedBufferSize(uint_t uWidth,uint_t uHeight,ePixelTypes eType,uint_t uMipMapLevels)
{
	// Get the main buffer level size
	uintptr_t uResult = GetSuggestedBufferSize(uWidth,uHeight,eType);
	// Any requested mip maps?
	if (uMipMapLevels>=2) {
		// Start counting from 2
		--uMipMapLevels;
		do {
			uWidth>>=1U;		// Shrink to the next mip map level
			uHeight>>=1U;
			// Add in the size for the next level
			uResult += GetSuggestedBufferSize(uWidth,uHeight,eType);
		} while (--uMipMapLevels);
	}
	// Return the total size
	return uResult;
}

/*! ************************************

	\brief Determine the amount of memory needed for an image buffer.
	
	Given an Image set with Width, Height and pixel type, calculate the size of a buffer that would
	contain the entire image. This function is used to allocate an image buffer.

	\return The number of bytes needed to allocate a buffer for this image.
	\sa GetSuggestedBufferSize(uint_t,uint_t,ePixelTypes)

***************************************/

uintptr_t Burger::Image::GetSuggestedBufferSize(void) const
{
	return GetSuggestedBufferSize(m_uWidth,m_uHeight,m_eType,m_uFlags&FLAGS_MIPMAPMASK);
}

/*! ************************************

	\brief Return the number of bytes a specific mip map image occupies

	Return the size in bytes of a specific mip map sub image.

	Used in conjunction with GetImage(uint_t) const, this will allow
	an application to extract a specific mip map for uploading
	onto a video card.

	\return The number of bytes that a specific mip map occupies
	\sa GetImage(uint_t) const

***************************************/

uintptr_t Burger::Image::GetImageSize(uint_t uMipMap) const
{
	uint_t uHeight = m_uHeight;
	uint_t uStride = m_uStride;
	uintptr_t uResult;
	switch (m_eType) {
		// DXT is special case, since it's in 4x4 blocks with the pixels rounded up
	case PIXELTYPEDXT1:			// Compressed with DXT1
	case PIXELTYPEDXT2:			// Compressed with DXT2
	case PIXELTYPEDXT3:			// Compressed with DXT3
	case PIXELTYPEDXT4:			// Compressed with DXT4
	case PIXELTYPEDXT5:			// Compressed with DXT5
		if (uMipMap) {
			// Any mip maps available?
			uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
			if (uMaxMip) {
				// Bounds check
				if (uMipMap>=uMaxMip) {
					uMipMap = uMaxMip-1;
				}
				// Iterate over the mip maps until the one is found
				uHeight>>=uMipMap;
				uStride>>=uMipMap;
			}
		}
		uHeight = (uHeight+3U)>>2U;
		uResult = uHeight*uStride;
		if (uHeight) {
			if (m_eType==PIXELTYPEDXT1) {
				if (uResult<8) {
					uResult = 8;
				}
			} else if (uResult<16) {
				uResult = 16;
			}
		}
		break;

	// Simple bitmaps are trivial to calculate
	default:
		if (uMipMap) {
			// Any mip maps available?
			uint_t uMaxMip = m_uFlags&FLAGS_MIPMAPMASK;
			if (uMaxMip) {
				// Bounds check
				if (uMipMap>=uMaxMip) {
					uMipMap = uMaxMip-1;
				}
				// Iterate over the mip maps until the one is found
				if (uMipMap) {
					do {
						uHeight>>=1U;
						uStride>>=1U;
					} while (--uMipMap);
				}
			}
		}
		uResult = uHeight*uStride;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Test the Image for validity before using it for a save.

	Test the image if it has a non-zero width, height 
	and image pointer.

	\return \ref NULL if the Image is valid or a pointer to a "C" string with the reason for failure.

***************************************/

const char *Burger::Image::ValidateForSave(void) const
{
	const char *pBadNews = NULL;
	if (!m_uWidth) {
		pBadNews = "The image is zero pixels wide.";
	} else if (!m_uHeight) {
		pBadNews = "The image is zero pixels high.";
	} else if (!m_pImage) {
		pBadNews = "No pixel data in the image structure.";
	}
	return pBadNews;
}


/*! ************************************

	\brief Erase all of the pixels in the bitmap

	Using the width value, zero out the pixels in the bitmap
	and use the stride value to skip to the next scan line.

	Compressed and uncompressed formats are fully supported

***************************************/

void Burger::Image::ClearBitmap(void)
{
	uint_t uWidth = m_uWidth;
	if (uWidth) {
		uint_t uHeight = m_uHeight;
		if (uHeight) {

			// The image has size, so get the bits per pixel for
			// an uncompressed image type

			uint_t uPixels = GetBitsPerPixel();
			if (uPixels) {
				uPixels = ((uPixels*uWidth)+7)>>3;
			} else {
				// Calculate the number of 4x4 pixel blocks needed
				uPixels = (uWidth+3)>>2;
				// Adjust the height by 4x4 cells
				uHeight = (uHeight+3)>>2;
				uPixels = uPixels*16;
				if (m_eType==PIXELTYPEDXT1) {
					// DXT1 only uses 8 bytes per cell, not 16
					uPixels>>=1U;
				}
			}
			// Perform the memory clear operation
			uint8_t *pWork = m_pImage;
			uintptr_t uStride = m_uStride;
			do {
				MemoryClear(pWork,uPixels);
				pWork += uStride;
			} while (--uHeight);
		}
	}
}

/*! ************************************

	\brief Flip an image upside down

	For uncompressed pixel formats, this function will flip the image upside down

	\return Zero if successful, non zero if the image can't be flipped
	\sa HorizontalFlip()

***************************************/

uint_t Burger::Image::VerticleFlip(void)
{
	// Get the size of the pixel in bytes
	uint_t uBytesPerPixels = GetBitsPerPixel();
	// Get the buffer
	uint8_t *pBuffer = m_pImage;	// Image pointer
	// Compressed textures are not supported
	// Bad buffer!
	if (!uBytesPerPixels || !pBuffer) {
		return 10;
	}

	uBytesPerPixels = (uBytesPerPixels+7)>>3;	// Round the bits to bytes
	uint_t uWidth = m_uWidth;						// Cache all the flags
	uint_t uHeight = m_uHeight;
	if (uWidth && (uHeight>=2)) {				// Single pixel height can't be flipped
		uWidth = uWidth*uBytesPerPixels;		// Scan line in BYTES
		uint8_t *pBottom = &pBuffer[(uHeight-1)*m_uStride];	// Bottommost line 
		uHeight = uHeight>>1;					// Divide height by 2 since the copy will meet in the middle
		do {
			uint_t uCount = 0;					// Init byte index
			do {
				uint8_t uTemp = pBuffer[uCount];	// Swap the two pixels
				pBuffer[uCount] = pBottom[uCount];
				pBottom[uCount] = uTemp;
			} while (++uCount<uWidth);			// All done?
			pBuffer = pBuffer+m_uStride;		// Next line down
			pBottom = pBottom-m_uStride;		// Next line up
		} while (--uHeight);					// All done?
	}
	return 0;
}

/*! ************************************

	\brief Flip an image horizontally

	For uncompressed pixel formats, this function will flip the image left
	to right

	\return Zero if successful, non zero if the image can't be flipped
	\sa VerticleFlip()

***************************************/

uint_t Burger::Image::HorizontalFlip(void)
{
	uint8_t *pBuffer = m_pImage;	// Image pointer
	if (!pBuffer) {
		return 10;				// No data
	}
	uint_t uBytesPerPixel = GetBitsPerPixel();
	if (!uBytesPerPixel) {
		return 10;				// Not supported
	}
	uint_t uWidth = m_uWidth;		// Cache all the flags
	uint_t uHeight = m_uHeight;
	if ((uWidth>=2) && uHeight) {		// Single pixel width can't be flipped
		uBytesPerPixel = (uBytesPerPixel+7)>>3;	// Round the bits to bytes
		uWidth = uWidth*uBytesPerPixel;	// Offset to the end of the line
		switch (uBytesPerPixel) {		// Use a different for each pixel width

		// Swap an image in bytes

		default:
			do {
				uint_t uLeft = 0;		// Init byte index
				uint_t uRight = uWidth-1;
				do {
					uint8_t uTemp = pBuffer[uLeft];	// Swap the left and right pixels
					pBuffer[uLeft] = pBuffer[uRight];
					pBuffer[uRight] = uTemp;
				} while (++uLeft<--uRight);			// All done?
				pBuffer = pBuffer+m_uStride;		// Next line down
			} while (--uHeight);					// All done?
			break;

		// Swap an image in shorts

		case 2:
			do {
				uint_t uLeft = 0;		// Init byte index
				uint_t uRight = uWidth-2;
				do {
					uint16_t uTemp = reinterpret_cast<uint16_t *>(pBuffer+uLeft)[0];	// Swap the left and right pixels
					reinterpret_cast<uint16_t *>(pBuffer+uLeft)[0] = reinterpret_cast<uint16_t *>(pBuffer+uRight)[0];
					reinterpret_cast<uint16_t *>(pBuffer+uRight)[0] = uTemp;
					uLeft=uLeft+2;				// Move towards the middle
					uRight=uRight-2;
				} while (uLeft<uRight);			// All done?
				pBuffer = pBuffer+m_uStride;	// Next line down
			} while (--uHeight);				// All done?
			break;

			/* Swap an image in 3 byte triplets */

		case 3:
			do {
				uint_t uLeft = 0;				// Init byte index
				uint_t uRight = uWidth-3;
				do {
					uint8_t uTemp = pBuffer[uLeft];	// Swap the left and right pixels
					pBuffer[uLeft] = pBuffer[uRight];
					pBuffer[uRight] = uTemp;
					uTemp = pBuffer[uLeft+1];		// Swap the left and right pixels
					pBuffer[uLeft+1] = pBuffer[uRight+1];
					pBuffer[uRight+1] = uTemp;
					uTemp = pBuffer[uLeft+2];		// Swap the left and right pixels
					pBuffer[uLeft+2] = pBuffer[uRight+2];
					pBuffer[uRight+2] = uTemp;
					uLeft=uLeft+3;					// Move towards the middle
					uRight=uRight-3;
				} while (uLeft<uRight);				// All done?
				pBuffer = pBuffer+m_uStride;		// Next line down
			} while (--uHeight);					// All done?
			break;

		// Swap an image in 32 bit chunks

		case 4:
			do {
				uint_t uLeft = 0;				// Init byte index
				uint_t uRight = uWidth-4;
				do {
					uint32_t uTemp;
					uTemp = reinterpret_cast<uint32_t *>(pBuffer+uLeft)[0];	// Swap the left and right pixels
					reinterpret_cast<uint32_t *>(pBuffer+uLeft)[0] = reinterpret_cast<uint32_t *>(pBuffer+uRight)[0];
					reinterpret_cast<uint32_t *>(pBuffer+uRight)[0] = uTemp;
					uLeft=uLeft+4;				// Move towards the middle
					uRight=uRight-4;
				} while (uLeft<uRight);			// All done?
				pBuffer = pBuffer+m_uStride;	// Next line down
			} while (--uHeight);				// All done?
			break;
		}
	}
	return 0;
}

/*! ************************************

	\brief Convert an image into PIXELTYPE8888 format

	Assume that the Image is already an PIXELTYPE8888
	format shape and a buffer is present. Return an error
	if not.
	
	\note Supported input formats are PIXELTYPE8BIT, PIXELTYPE4444,
		PIXELTYPE1555, PIXELTYPE555, PIXELTYPE565, PIXELTYPE888 and PIXELTYPE8888

	\param pInput Pointer to an image to convert from
	\param pPalette Optional color palette if converting from an 8 bit format

***************************************/

uint_t Burger::Image::Store8888(const Image *pInput,const RGBAWord8_t *pPalette)
{
	uint_t uResult = 10;
	if ((m_eType==PIXELTYPE8888) &&
		(m_uWidth==pInput->m_uWidth) &&
		(m_uHeight==pInput->m_uHeight) &&
		(m_uWidth && m_uHeight) &&
		m_pImage) {
		switch (pInput->m_eType) {
		case PIXELTYPE8BIT:
			{
				if (pPalette) {
					const uint8_t *pInputBuffer = pInput->m_pImage;
					uint8_t *pOutputBuffer = m_pImage;
					uint_t uHeight = m_uHeight;
					do {
						uint_t uWidth = m_uWidth;
						const uint8_t *pSrc = pInputBuffer;
						uint8_t *pDest = pOutputBuffer;
						do {
							const RGBAWord8_t *pColor = pPalette+pSrc[0];
							++pSrc;
							pDest[0] = pColor->m_uRed;
							pDest[1] = pColor->m_uGreen;
							pDest[2] = pColor->m_uBlue;
							pDest[3] = pColor->m_uAlpha;
							pDest+=4;
						} while (--uWidth);
						pInputBuffer += pInput->m_uStride;
						pOutputBuffer += m_uStride;
					} while (--uHeight);
					uResult = 0;
				}
			}
			break;

		case PIXELTYPE4444:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						uint_t uTemp = reinterpret_cast<const uint16_t *>(pSrc)[0];
						pDest[0] = Renderer::RGB4ToRGB8Table[(uTemp>>8U)&0xFU];
						pDest[1] = Renderer::RGB4ToRGB8Table[(uTemp>>4U)&0xFU];
						pDest[2] = Renderer::RGB4ToRGB8Table[uTemp&0xFU];
						pDest[3] = Renderer::RGB4ToRGB8Table[(uTemp>>12U)&0xFU];
						pSrc+=2;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;

		case PIXELTYPE1555:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						uint_t uTemp = reinterpret_cast<const uint16_t *>(pSrc)[0];
						pDest[0] = Renderer::RGB5ToRGB8Table[(uTemp>>10U)&0x1FU];
						pDest[1] = Renderer::RGB5ToRGB8Table[(uTemp>>5U)&0x1FU];
						pDest[2] = Renderer::RGB5ToRGB8Table[uTemp&0x1FU];
						pDest[3] = static_cast<uint8_t>((uTemp&0x8000U) ? 0xFFU : 0);
						pSrc+=2;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;

		case PIXELTYPE555:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						uint_t uTemp = reinterpret_cast<const uint16_t *>(pSrc)[0];
						pDest[0] = Renderer::RGB5ToRGB8Table[(uTemp>>10U)&0x1FU];
						pDest[1] = Renderer::RGB5ToRGB8Table[(uTemp>>5U)&0x1FU];
						pDest[2] = Renderer::RGB5ToRGB8Table[uTemp&0x1FU];
						pDest[3] = 0xFF;
						pSrc+=2;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;



		case PIXELTYPE565:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						uint_t uTemp = reinterpret_cast<const uint16_t *>(pSrc)[0];
						pDest[0] = Renderer::RGB5ToRGB8Table[(uTemp>>11U)&0x1FU];
						pDest[1] = Renderer::RGB6ToRGB8Table[(uTemp>>5U)&0x3FU];
						pDest[2] = Renderer::RGB5ToRGB8Table[uTemp&0x1FU];
						pDest[3] = 0xFF;
						pSrc+=2;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;

		case PIXELTYPE888:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						pDest[0] = pSrc[0];
						pDest[1] = pSrc[1];
						pDest[2] = pSrc[2];
						pDest[3] = 0xFF;
						pSrc+=3;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;

		case PIXELTYPE8888:
			{
				const uint8_t *pInputBuffer = pInput->m_pImage;
				uint8_t *pOutputBuffer = m_pImage;
				uint_t uHeight = m_uHeight;
				do {
					uint_t uWidth = m_uWidth;
					const uint8_t *pSrc = pInputBuffer;
					uint8_t *pDest = pOutputBuffer;
					do {
						pDest[0] = pSrc[0];
						pDest[1] = pSrc[1];
						pDest[2] = pSrc[2];
						pDest[3] = pSrc[3];
						pSrc+=4;
						pDest+=4;
					} while (--uWidth);
					pInputBuffer += pInput->m_uStride;
					pOutputBuffer += m_uStride;
				} while (--uHeight);
				uResult = 0;
			}
			break;

		// Not supported
		default:
			break;
		}
	}
	return uResult;
}
