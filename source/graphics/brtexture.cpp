/***************************************

    Texture for rendering class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtexture.h"
#include "brglobalmemorymanager.h"
#include "brfiletga.h"
#include "brfilepng.h"
#include "brfilebmp.h"
#include "brfilegif.h"
#include "brfilemanager.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Texture,Burger::DisplayObject);
#endif

/*! ************************************

	\enum Burger::Texture::eFilter
	\brief Texture filter enumeration

	Settings to determine how a texture is filtered when expanded
	or contracted when being rendered

	\sa eWrapping

***************************************/

/*! ************************************

	\enum Burger::Texture::eWrapping
	\brief Texture filter enumeration

	Settings to determine how a texture is mapped when
	U/V coordinates exceeded the range of 0.0f and 1.0f

	\sa eFilter

***************************************/

/*! ************************************

	\enum Burger::Texture::eLoader
	\brief Texture callback command enumeration

	When the custom texture loader is called, one
	of these three commands are passed. \ref LOADER_LOAD
	is when the Image record needed to be set up,
	\ref LOADER_UNLOAD when the pixel data can be
	purged from memory and \ref LOADER_SHUTDOWN when
	the function will never be called again.

	\sa LoadImage(void), UnloadImage(void) or ShutdownImage(void)

***************************************/

/*! ************************************

	\enum Burger::Texture::eDirtyFlags
	\brief Bit flags for data to update

	Flags to mark which records have been changed to
	so they can be uploaded to the device driver

***************************************/

/*! ************************************

	\class Burger::Texture
	\brief Class for managing hardware images
	
	This class is the mechanism for loading raw pixels
	into a video card for GPU rendering. A mechanism
	is provided to allow arbitrary uploads to
	video memory in cases of GPU memory purges
	by the video display driver

	\sa TextureOpenGL or TextureDirectX9

***************************************/

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty texture,
	set the wrapping to WRAP_REPEAT and
	filter FILTER_NEAREST

***************************************/

#if defined(BURGER_WINDOWS) || !(defined(BURGER_XBOX360) || defined(BURGER_OPENGL)) || defined(DOXYGEN)
Burger::Texture::Texture() :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_Image(),
	m_eWrappingS(WRAP_REPEAT),
	m_eWrappingT(WRAP_REPEAT),
	m_eMinFilter(FILTER_NEAREST),
	m_eMagFilter(FILTER_NEAREST),
	m_uDirty(UINT32_MAX)
{
}

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty texture,
	set the wrapping to WRAP_REPEAT and
	filter FILTER_NEAREST

***************************************/

Burger::Texture::Texture(eWrapping uWrapping,eFilter uFilter) :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_Image(),
	m_eWrappingS(uWrapping),
	m_eWrappingT(uWrapping),
	m_eMinFilter(uFilter),
	m_eMagFilter(uFilter),
	m_uDirty(UINT32_MAX)
{
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::Texture::~Texture()
{
	// Release all resources created by loader
	ShutdownImageMemory();
}

/*! ************************************

	\brief Load the pixel data 
	
	Allow the derived class to load a texture from disk
	and convert into a proper Image

	\sa LoadImage(void) or UnloadImage(void)

***************************************/

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
uint_t Burger::Texture::CheckLoad(Display * /*pDisplay */)
{
	m_uDirty = 0;
	// Error!
	return 10;
}

/*! ************************************

	\brief Release hardware resources
	
	Allow the derived class to release hardware
	resources

	\sa CheckLoad(Display *)

***************************************/

void Burger::Texture::Release(Display * /*pDisplay */)
{
	m_uDirty = UINT32_MAX;
}
#endif
#endif

/*! ************************************

	\brief Call the loader callback with the command \ref LOADER_LOAD

	\sa UnloadImageMemory(void) or ShutdownImageMemory(void)

***************************************/

uint_t BURGER_API Burger::Texture::LoadImageMemory(void)
{
	// Get the callback
	LoaderProc pProc = m_pLoader;
	uint_t uResult = 0;
	if (pProc) {
		// Call it if not NULL
		uResult = pProc(this,LOADER_LOAD);
	}
	return uResult;
}

/*! ************************************

	\brief Call the loader callback with the command \ref LOADER_UNLOAD

	\sa LoadImageMemory(void) or ShutdownImageMemory(void)

***************************************/

void BURGER_API Burger::Texture::UnloadImageMemory(void)
{
	// Get the callback
	LoaderProc pProc = m_pLoader;
	if (pProc) {
		// Call it if not NULL
		pProc(this,LOADER_UNLOAD);
	}
}

/*! ************************************

	\brief Call the loader callback with the command \ref LOADER_SHUTDOWN

	\sa LoadImageMemory(void) or UnloadImageMemory(void)

***************************************/

void BURGER_API Burger::Texture::ShutdownImageMemory(void)
{
	// Get the callback
	LoaderProc pProc = m_pLoader;
	if (pProc) {
		// Call it if not NULL
		pProc(this,LOADER_SHUTDOWN);
	}
}

/*! ************************************

	\fn const Image *Burger::Texture::GetImage(void) const
	\brief Get a pointer to the contained \ref Image record

	\return Pointer to the contained \ref Image record.
	\sa GetImage(void) or GetImage(uint_t) const

***************************************/

/*! ************************************

	\fn Image *Burger::Texture::GetImage(void)
	\brief Get a pointer to the contained \ref Image record

	\return Pointer to the contained \ref Image record.
	\sa GetImage(void) const or GetImage(uint_t) const

***************************************/

/*! ************************************

	\fn const uint8_t *Burger::Texture::GetImage(uint_t uMipMap) const
	\brief Get a pointer to the bitmap

	\param uMipMap Mip map level (0 is for the base bitmap)
	\return \ref NULL if no bitmap is present or a pointer to the bitmap.

	\sa GetImage(void) const or GetImage(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Texture::GetWidth(void) const
	\brief Get the width of the texture

	\return Width of the texture in the Image record

	\sa GetHeight(void) const or GetWidth(uint_t) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Texture::GetWidth(uint_t uMipMap) const
	\brief Get the width of the texture at a mip map level

	\param uMipMap Mip map level to query
	\return Width of the texture in the Image record at the mip map level

	\sa GetHeight(uint_t) const or GetWidth(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Texture::GetHeight(void) const
	\brief Get the height of the texture

	\return Height of the texture in the Image record

	\sa GetWidth(void) const or GetHeight(uint_t) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Texture::GetHeight(uint_t uMipMap) const
	\brief Get the height of the texture at a mip map level

	\param uMipMap Mip map level to query
	\return Height of the texture in the Image record at the mip map level

	\sa GetWidth(uint_t) const or GetHeight(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::Texture::GetStride(void) const
	\brief Get the byte stride of each scan line of the texture

	\return Byte stride of the texture in the Image record

	\sa GetWidth(void) const or GetStride(uint_t) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::Texture::GetStride(uint_t uMipMap) const
	\brief Get the byte stride of each scan line of the texture at a mip map level

	\param uMipMap Mip map level to query
	\return Byte stride of the texture in the Image record at the mip map level

	\sa GetWidth(uint_t) const or GetStride(void) const

***************************************/

/*! ************************************

	\fn Image::ePixelTypes Burger::Texture::GetType(void) const
	\brief Get the type of data contained in the texture

	\return Enumeration of the format of the pixel data

	\sa GetMipMapCount(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Texture::GetMipMapCount(void) const
	\brief Get the number of mip map levels

	\return Number of mip maps in the texture (Usually 1)

	\sa GetType(void) const

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetImageDirty(void)
	\brief Set the image updated flag

	If the texture is modified by the application, call this
	function to alert the texture manager to upload
	the new bitmap into the hardware on the next rendering
	pass.

***************************************/


/*! ************************************

	\fn eWrapping Burger::Texture::GetWrappingS(void) const
	\brief Get the wrapping setting for the S (U) coordinate

	\return The wrapping setting for the S (U) coordinate

	\sa SetWrappingS(eWrapping) or GetWrappingT(void) const

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetWrappingS(eWrapping uWrapping) const
	\brief Set the wrapping setting for the S (U) coordinate

	\param uWrapping The new wrapping setting for the S (U) coordinate

	\sa GetWrappingS(void) const or SetWrappingT(eWrapping)

***************************************/

/*! ************************************

	\fn eWrapping Burger::Texture::GetWrappingT(void) const
	\brief Get the wrapping setting for the T (V) coordinate

	\return The wrapping setting for the T (V) coordinate

	\sa SetWrappingT(eWrapping) or GetWrappingS(void) const

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetWrappingT(eWrapping uWrapping) const
	\brief Set the wrapping setting for the T (V) coordinate

	\param uWrapping The new wrapping setting for the T (V) coordinate

	\sa GetWrappingT(void) const or SetWrappingS(eWrapping)

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetWrapping(eWrapping uWrapping) const
	\brief Set the wrapping setting for the S and T (U/V) coordinate

	\param uWrapping The new wrapping setting for the S and T (U/V) coordinate

	\sa SetWrappingS(eWrapping) or SetWrappingT(eWrapping)

***************************************/

/*! ************************************

	\fn eFilter Burger::Texture::GetMinFilter(void) const
	\brief Get the filter setting for the minimum scaling

	\return The filter setting for the minimum scaling

	\sa SetMinFilter(eFilter) or GetMagFilter(void) const

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetMinFilter(eFilter uFilter) const
	\brief Set the filter setting for the minimum scaling

	\param uFilter The new filter setting for the minimum scaling

	\sa GetMinFilter(void) const or SetMagFilter(eFilter)

***************************************/

/*! ************************************

	\fn eFilter Burger::Texture::GetMagFilter(void) const
	\brief Get the filter setting for the maximum scaling

	\return The filter setting for the maximum scaling

	\sa SetMagFilter(eFilter) or GetMinFilter(void) const

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetMagFilter(eFilter uFilter) const
	\brief Set the filter setting for the maximum scaling

	\param uFilter The new filter setting for the maximum scaling

	\sa GetMagFilter(void) const or SetMinFilter(eFilter)

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetFilter(eFilter uFilter) const
	\brief Set the filter setting for the minimum and maximum scaling

	\param uFilter The new wrapping setting for the minimum and maximum scaling

	\sa SetMinFilter(eFilter) or SetMagFilter(eFilter)

***************************************/

/*! ************************************

	\fn LoaderProc Burger::Texture::GetLoader(void) const
	\brief Get the pointer to the texture reload callback

	\return The pointer to the callback function

	\sa SetLoader(LoaderProc) or UploadImage(void)

***************************************/

/*! ************************************

	\fn void Burger::Texture::SetLoader(LoaderProc pCallback) const
	\brief Set the function pointer to the texture loading callback

	\param pCallback The new pointer to the texture loading callback

	\sa GetLoader(void) const or UploadImage(void)

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Texture::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\brief Private callback to load TGA files from a resource file

	Function to handle the loading and conversion to an Image
	of a TGA file found in a resource file

	\sa LoadTGA(RezFile *,uint_t)

***************************************/

#if !defined(DOXYGEN)
struct RezFileLoad_t {
	Burger::RezFile *m_pRezFile;
	uint_t m_uRezNum;
};
#endif

uint_t BURGER_API Burger::Texture::CallbackRezFileTGA(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the record
			const RezFileLoad_t *pRezFileLoad = static_cast<const RezFileLoad_t *>(pTexture->m_pUserData);
			// Load the resource file
			uResult = pTexture->m_Image.InitTGA(pRezFileLoad->m_pRezFile,pRezFileLoad->m_uRezNum);
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a TGA file

	Set up for loading a hardware texture from a TGA file
	stored in a RezFile

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadPNG(RezFile *,uint_t), LoadBMP(RezFile *,uint_t) or LoadGIF(RezFile *,uint_t), 

***************************************/

void BURGER_API Burger::Texture::LoadTGA(RezFile *pRezFile,uint_t uRezNum)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pRezFile) {
		RezFileLoad_t *pRezFileLoad = static_cast<RezFileLoad_t *>(allocate_memory(sizeof(RezFileLoad_t)));
		pRezFileLoad->m_pRezFile = pRezFile;
		pRezFileLoad->m_uRezNum = uRezNum;
		m_pUserData = pRezFileLoad;
		m_pLoader = CallbackRezFileTGA;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load TGA files from a file

	Function to handle the loading and conversion to an Image
	of a TGA file found in a file

	\sa LoadTGA(const char *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFileTGA(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		// Convert TGA to Image
		uResult = pTexture->m_Image.InitTGA(static_cast<const char *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a TGA file

	Set up for loading a hardware texture from a TGA file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadPNG(const char *pFilename), LoadBMP(const char *pFilename), LoadGIF(const char *pFilename), 

***************************************/

void BURGER_API Burger::Texture::LoadTGA(const char *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = StringDuplicate(pFilename);
		m_pLoader = CallbackFileTGA;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load TGA files from a file

	Function to handle the loading and conversion to an Image
	of a TGA file found in a file

	\sa LoadTGA(Filename *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFilenameTGA(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		// Convert TGA to Image
		uResult = pTexture->m_Image.InitTGA(static_cast<Filename *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		delete_object(static_cast<Filename *>(pTexture->m_pUserData));
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a TGA file

	Set up for loading a hardware texture from a TGA file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(const char *), LoadTGA(RezFile *,uint_t)

***************************************/

void BURGER_API Burger::Texture::LoadTGA(Filename *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = Filename::new_object(pFilename[0]);
		m_pLoader = CallbackFilenameTGA;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load PNG files from a resource file

	Function to handle the loading and conversion to an Image
	of a PNG file found in a resource file

	\sa LoadPNG(RezFile *,uint_t)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackRezFilePNG(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the record
			const RezFileLoad_t *pRezFileLoad = static_cast<const RezFileLoad_t *>(pTexture->m_pUserData);
			// Load the resource file
			uResult = pTexture->m_Image.InitPNG(pRezFileLoad->m_pRezFile,pRezFileLoad->m_uRezNum);
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a PNG file

	Set up for loading a hardware texture from a PNG file
	stored in a RezFile

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(RezFile *,uint_t), LoadBMP(RezFile *,uint_t) or LoadGIF(RezFile *,uint_t)

***************************************/

void BURGER_API Burger::Texture::LoadPNG(RezFile *pRezFile,uint_t uRezNum)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pRezFile) {
		RezFileLoad_t *pRezFileLoad = static_cast<RezFileLoad_t *>(allocate_memory(sizeof(RezFileLoad_t)));
		pRezFileLoad->m_pRezFile = pRezFile;
		pRezFileLoad->m_uRezNum = uRezNum;
		m_pUserData = pRezFileLoad;
		m_pLoader = CallbackRezFilePNG;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load PNG files from a file

	Function to handle the loading and conversion to an Image
	of a PNG file found in a file

	\sa LoadPNG(const char *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFilePNG(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		uResult = pTexture->m_Image.InitPNG(static_cast<const char *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a PNG file

	Set up for loading a hardware texture from a PNG file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(const char *), LoadBMP(const char *) or LoadGIF(const char *)

***************************************/

void BURGER_API Burger::Texture::LoadPNG(const char *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = StringDuplicate(pFilename);
		m_pLoader = CallbackFilePNG;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load PNG files from a file

	Function to handle the loading and conversion to an Image
	of a PNG file found in a file

	\sa LoadPNG(Filename *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFilenamePNG(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		// Convert PNG to Image
		uResult = pTexture->m_Image.InitPNG(static_cast<Filename *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		delete_object(static_cast<Filename *>(pTexture->m_pUserData));
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a PNG file

	Set up for loading a hardware texture from a PNG file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(Filename *pFilename), LoadBMP(Filename *pFilename) or LoadGIF(Filename *pFilename)

***************************************/

void BURGER_API Burger::Texture::LoadPNG(Filename *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = Filename::new_object(pFilename[0]);
		m_pLoader = CallbackFilenamePNG;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load BMP files from a resource file

	Function to handle the loading and conversion to an Image
	of a BMP file found in a resource file

	\sa LoadBMP(RezFile *,uint_t)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackRezFileBMP(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the record
			const RezFileLoad_t *pRezFileLoad = static_cast<const RezFileLoad_t *>(pTexture->m_pUserData);
			// Load the resource file
			uResult = pTexture->m_Image.InitBMP(pRezFileLoad->m_pRezFile,pRezFileLoad->m_uRezNum);
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a BMP file

	Set up for loading a hardware texture from a BMP file
	stored in a RezFile

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(RezFile *,uint_t), LoadPNG(RezFile *,uint_t) or LoadGIF(RezFile *,uint_t)

***************************************/

void BURGER_API Burger::Texture::LoadBMP(RezFile *pRezFile,uint_t uRezNum)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pRezFile) {
		RezFileLoad_t *pRezFileLoad = static_cast<RezFileLoad_t *>(allocate_memory(sizeof(RezFileLoad_t)));
		pRezFileLoad->m_pRezFile = pRezFile;
		pRezFileLoad->m_uRezNum = uRezNum;
		m_pUserData = pRezFileLoad;
		m_pLoader = CallbackRezFileBMP;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load BMP files from a file

	Function to handle the loading and conversion to an Image
	of a BMP file found in a file

	\sa LoadBMP(const char *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFileBMP(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		uResult = pTexture->m_Image.InitBMP(static_cast<const char *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a BMP file

	Set up for loading a hardware texture from a BMP file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(const char *), LoadPNG(const char *) or LoadGIF(const char *)

***************************************/

void BURGER_API Burger::Texture::LoadBMP(const char *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = StringDuplicate(pFilename);
		m_pLoader = CallbackFileBMP;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load BMP files from a file

	Function to handle the loading and conversion to an Image
	of a BMP file found in a file

	\sa LoadBMP(Filename *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFilenameBMP(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		// Convert BMP to Image
		uResult = pTexture->m_Image.InitBMP(static_cast<Filename *>(pTexture->m_pUserData));
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		delete_object(static_cast<Filename *>(pTexture->m_pUserData));
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a BMP file

	Set up for loading a hardware texture from a BMP file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(Filename *pFilename), LoadPNG(Filename *pFilename) or LoadGIF(Filename *pFilename)

***************************************/

void BURGER_API Burger::Texture::LoadBMP(Filename *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = Filename::new_object(pFilename[0]);
		m_pLoader = CallbackFilenameBMP;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load GIF files from a resource file

	Function to handle the loading and conversion to an Image
	of a GIF file found in a resource file

	\sa LoadGIF(RezFile *,uint_t)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackRezFileGIF(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the record
			const RezFileLoad_t *pRezFileLoad = static_cast<const RezFileLoad_t *>(pTexture->m_pUserData);
			RezFile *pRezFile = pRezFileLoad->m_pRezFile;
			uint_t uRezNum = pRezFileLoad->m_uRezNum;
			// Load the resource file
			void *pData = pRezFile->Load(uRezNum);
			// Assume error
			uResult = 10;
			if (pData) {
				// Convert into a stream (TRUE to not auto delete the buffer)
				InputMemoryStream RawStream(pData,pRezFile->GetSize(uRezNum),TRUE);
				FileGIF RawGIF;
				// Convert GIF to Image
				Image TempImage;
				uResult = RawGIF.Load(&TempImage,&RawStream);
				if (!uResult) {
					// Convert from 8 bit paletted to RGBA
					uResult = pTexture->m_Image.Init(TempImage.GetWidth(),TempImage.GetHeight(),Image::PIXELTYPE8888);
					if (!uResult) {
						uResult = pTexture->m_Image.Store8888(&TempImage,RawGIF.GetPalette());
					}
				}
				pRezFile->Release(uRezNum);
			}
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a GIF file

	Set up for loading a hardware texture from a GIF file
	stored in a RezFile

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(RezFile *,uint_t), LoadPNG(RezFile *,uint_t) or LoadBMP(RezFile *,uint_t)

***************************************/

void BURGER_API Burger::Texture::LoadGIF(RezFile *pRezFile,uint_t uRezNum)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pRezFile) {
		RezFileLoad_t *pRezFileLoad = static_cast<RezFileLoad_t *>(allocate_memory(sizeof(RezFileLoad_t)));
		pRezFileLoad->m_pRezFile = pRezFile;
		pRezFileLoad->m_uRezNum = uRezNum;
		m_pUserData = pRezFileLoad;
		m_pLoader = CallbackRezFileGIF;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load GIF files from a file

	Function to handle the loading and conversion to an Image
	of a GIF file found in a file

	\sa LoadGIF(const char *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFileGIF(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the file
			uintptr_t uDataSize;
			void *pData = FileManager::load_file(static_cast<const char *>(pTexture->m_pUserData),&uDataSize);
			// Assume error
			uResult = 10;
			if (pData) {
				// Convert into a stream (TRUE to not auto delete the buffer)
				InputMemoryStream RawStream(pData,uDataSize);
				FileGIF RawGIF;
				// Convert GIF to Image
				Image TempImage;
				uResult = RawGIF.Load(&TempImage,&RawStream);
				if (!uResult) {
					// Convert from 8 bit paletted to RGBA
					uResult = pTexture->m_Image.Init(TempImage.GetWidth(),TempImage.GetHeight(),Image::PIXELTYPE8888);
					if (!uResult) {
						uResult = pTexture->m_Image.Store8888(&TempImage,RawGIF.GetPalette());
					}
				}
			}
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		free_memory(pTexture->m_pUserData);
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a GIF file

	Set up for loading a hardware texture from a GIF file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(const char *), LoadPNG(const char *) or LoadBMP(const char *)

***************************************/

void BURGER_API Burger::Texture::LoadGIF(const char *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = StringDuplicate(pFilename);
		m_pLoader = CallbackFileGIF;
		m_uDirty |= DIRTY_IMAGE;
	}
}

/*! ************************************

	\brief Private callback to load GIF files from a file

	Function to handle the loading and conversion to an Image
	of a GIF file found in a file

	\sa LoadGIF(Filename *)

***************************************/

uint_t BURGER_API Burger::Texture::CallbackFilenameGIF(Texture *pTexture,eLoader uLoader)
{
	uint_t uResult;
	switch (uLoader) {
	default:
	case LOADER_LOAD:
		{
			// Get the file
			uintptr_t uDataSize;
			void *pData = FileManager::load_file(static_cast<Filename *>(pTexture->m_pUserData),&uDataSize);
			// Assume error
			uResult = 10;
			if (pData) {
				// Convert into a stream (TRUE to not auto delete the buffer)
				InputMemoryStream RawStream(pData,uDataSize);
				FileGIF RawGIF;
				// Convert GIF to Image
				Image TempImage;
				uResult = RawGIF.Load(&TempImage,&RawStream);
				if (!uResult) {
					// Convert from 8 bit paletted to RGBA
					uResult = pTexture->m_Image.Init(TempImage.GetWidth(),TempImage.GetHeight(),Image::PIXELTYPE8888);
					if (!uResult) {
						uResult = pTexture->m_Image.Store8888(&TempImage,RawGIF.GetPalette());
					}
				}
			}
		}
		break;

	// Release the image itself
	case LOADER_UNLOAD:
		pTexture->m_Image.Shutdown();
		uResult = 0;
		break;

	// Release the user data
	case LOADER_SHUTDOWN:
		delete_object(static_cast<Filename *>(pTexture->m_pUserData));
		pTexture->m_pUserData = NULL;
		uResult = 0;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Set the texture to load the image from a GIF file

	Set up for loading a hardware texture from a GIF file
	stored in a file

	\note The file is not loaded immediately. It will be loaded
	after a call to LoadImage(void)

	\sa LoadTGA(Filename *), LoadPNG(Filename *) or LoadBMP(Filename *)

***************************************/

void BURGER_API Burger::Texture::LoadGIF(Filename *pFilename)
{
	// Get rid of any previous loader
	ShutdownImageMemory();
	if (pFilename) {
		m_pUserData = Filename::new_object(pFilename[0]);
		m_pLoader = CallbackFilenameGIF;
		m_uDirty |= DIRTY_IMAGE;
	}
}
