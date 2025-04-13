/***************************************

    Texture for rendering class, OpenGL version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtextureopengl.h"

#if defined(BURGER_OPENGL)
#include "brglobalmemorymanager.h"
#include "brdisplayopengl.h"

#define __BURGER__
#include "brgl.h"
#include "brglext.h"

//
// OpenGL is a derived class for Windows
// to allow multiple API support. All other
// OpenGL based platforms, this is a base class
//

#if !defined(BURGER_WINDOWS)
#define TextureOpenGL Texture
#else
#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::TextureOpenGL,Burger::Texture);
#endif
#endif

/*! ************************************

	\class Burger::TextureOpenGL
	\brief Class for containing raw images
	
	This class is the mechanism for loading and saving popular file formats
	for image files such as TGA, LBM, PCX, PNG and the like. It's used as
	input for the Burger::Texture manager to convert the data from an image into
	a hardware texture for GPU rendering. Graphics tools can
	use this class exclusively for image conversion and manipulation

	\sa Texture or TextureDirectX9

***************************************/

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty image

***************************************/

#if defined(BURGER_WINDOWS)
Burger::TextureOpenGL::TextureOpenGL() :
	Texture(),
	m_uTextureID(0)
{
}

Burger::TextureOpenGL::TextureOpenGL(eWrapping uWrapping,eFilter uFilter) :
	Texture(uWrapping,uFilter),
	m_uTextureID(0)
{
}
#else

Burger::Texture::Texture() :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_uTextureID(0),
	m_Image(),
	m_eWrappingS(WRAP_REPEAT),
	m_eWrappingT(WRAP_REPEAT),
	m_eMinFilter(FILTER_NEAREST),
	m_eMagFilter(FILTER_NEAREST),
	m_uDirty(UINT32_MAX)
{
}

Burger::Texture::Texture(eWrapping uWrapping,eFilter uFilter) :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_uTextureID(0),
	m_Image(),
	m_eWrappingS(uWrapping),
	m_eWrappingT(uWrapping),
	m_eMinFilter(uFilter),
	m_eMagFilter(uFilter),
	m_uDirty(UINT32_MAX)
{
}
#endif

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::TextureOpenGL::~TextureOpenGL()
{
	if (m_uTextureID) {
		GLuint uID = m_uTextureID;
		glDeleteTextures(1,&uID);
		m_uTextureID = 0;
	}
#if !defined(BURGER_WINDOWS)
	// Release all resources created by loader
	ShutdownImageMemory();
#endif
}

/*! ************************************

	\brief Load and bind an OpenGL texture
	
	Upload a texture to the OpenGL driver and 
	bind it to OpenGL

	\param pDisplay Pointer to the display object
	\return Zero if no error, non zero on error

***************************************/

uint_t Burger::TextureOpenGL::CheckLoad(Display * /* pDisplay */)
{
	// Is a texture object defined?
	GLuint uID = m_uTextureID;
	if (!uID) {
		GLuint uNewID = 0;
		glGenTextures(1,&uNewID);
		uID = uNewID;
		if (!uID) {
			return 10;
		}
		m_uTextureID = uID;
	}

	// Bind the texture
	glBindTexture(GL_TEXTURE_2D,uID);

	uint_t uResult = 0;
	// Any updating?
	if (m_uDirty) {
		if (m_uDirty&DIRTY_IMAGE) {
			if (!LoadImageMemory()) {
	
				Image::ePixelTypes eType = m_Image.GetType();
				GLenum iType = GL_UNSIGNED_BYTE;
				GLenum iFormat = 0;
				switch (eType) {
				case Image::PIXELTYPE888:
					iFormat = GL_RGB;
					break;
				case Image::PIXELTYPE8888:
					iFormat = GL_RGBA;
					break;
				default:
					break;	
				}

				// Is the format supported?

				if (iFormat) {
					// Bytes are packed together (Needed for RGB format)
					glPixelStorei(GL_UNPACK_ALIGNMENT,1);

					// If the bytes in the image are packed together, then
					// it's a simple upload, otherwise, do it the hard way

					uint_t uMipMap = 0;
					do {
						uint_t uWidth = m_Image.GetWidth(uMipMap);
						uint_t uHeight = m_Image.GetHeight(uMipMap);
						const uint8_t *pSource = m_Image.GetImage(uMipMap);
						if (uMipMap || (m_Image.GetSuggestedStride()==m_Image.GetStride())) {
							// Allocate and load image data into texture in one go
							glTexImage2D(GL_TEXTURE_2D,static_cast<GLint>(uMipMap),static_cast<GLint>(iFormat),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight),0,iFormat,iType,pSource);
						} else {
							// Allocate the memory for the texture
							glTexImage2D(GL_TEXTURE_2D,0,static_cast<GLint>(iFormat),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight),0,iFormat,iType,NULL);
							// Upload one line at a time to support stride
							if (uHeight) {
								uint_t uY=0;
								uintptr_t uStride = m_Image.GetStride(uMipMap);
								do {
									glTexSubImage2D(GL_TEXTURE_2D,0,0,static_cast<GLsizei>(uY),static_cast<GLsizei>(uWidth),1,iFormat,iType,pSource);
									pSource += uStride;
									++uY;
								} while (--uHeight);
							}
						}
					} while (++uMipMap<m_Image.GetMipMapCount());

					// If needed, generate mipmaps

					if (m_Image.GetMipMapCount()==1) {
						glGenerateMipmap(GL_TEXTURE_2D);
					}
				}
				UnloadImageMemory();
			} else {
				uResult = 5;
			}
		}
		if (m_uDirty&DIRTY_WRAPPING_S) {
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GetWrapping(m_eWrappingS));
		}
		if (m_uDirty&DIRTY_WRAPPING_T) {
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GetWrapping(m_eWrappingT));
		}
		if (m_uDirty&DIRTY_MIN) {
			int iParm = GetFilter(m_eMinFilter);
			if (iParm==GL_LINEAR /* && (m_Image.GetMipMapCount()>=2) */) {
				iParm = GL_LINEAR_MIPMAP_NEAREST;
			}
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,static_cast<GLint>(iParm));
		}
		if (m_uDirty&DIRTY_MAG) {
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GetFilter(m_eMagFilter));
		}
		m_uDirty = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Release hardware resources
	
	Allow the derived class to release hardware
	resources

	\sa CheckLoad(Display *)

***************************************/

void Burger::TextureOpenGL::Release(Display * /*pDisplay */)
{
	if (m_uTextureID) {
		GLuint uID = m_uTextureID;
		glDeleteTextures(1,&uID);
		m_uTextureID = 0;
	}
	m_uDirty = UINT32_MAX;
}


/*! ************************************

	\brief Convert the UV wrapping enumeration into an OpenGL value
	\param uWrapping Generic texture wrapping enumeration
	\return GL_REPEAT, GL_CLAMP (Default) etc...

***************************************/

int Burger::TextureOpenGL::GetWrapping(eWrapping uWrapping)
{
	int iWrapping;
	switch (uWrapping) {
	default:
	case WRAP_CLAMP:
#if defined(BURGER_IOS) || defined(BURGER_ANDROID)
		iWrapping = GL_CLAMP_TO_EDGE;
#else
		iWrapping = GL_CLAMP;
#endif
		break;
	case WRAP_REPEAT:
		iWrapping = GL_REPEAT;
		break;
	}
	return iWrapping;
}

/*! ************************************

	\brief Convert the filter enumeration into an OpenGL filter
	\param uFilter Generic texture filter enumeration
	\return GL_LINEAR, GL_NEAREST (Default) etc...

***************************************/

int Burger::TextureOpenGL::GetFilter(eFilter uFilter)
{
	int iFilter;
	switch (uFilter) {
	default:
	case FILTER_NEAREST:
		iFilter = GL_NEAREST;
		break;
	case FILTER_LINEAR:
		iFilter = GL_LINEAR;
		break;
	}
	return iFilter;
}

#endif
