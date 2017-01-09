/***************************************

	Texture for rendering class, DirectX9 version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtexture.h"
#if defined(BURGER_XBOX360)

#include "brglobalmemorymanager.h"
#include "brdisplay.h"
#define NONET
#include <xtl.h>

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty image

***************************************/

Burger::Texture::Texture() :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_Image(),
	m_eWrappingS(WRAP_REPEAT),
	m_eWrappingT(WRAP_REPEAT),
	m_eMinFilter(FILTER_NEAREST),
	m_eMagFilter(FILTER_NEAREST),
	m_uDirty(BURGER_MAXUINT),
	m_pD3DTexture(NULL)
{
}

Burger::Texture::Texture(eWrapping uWrapping,eFilter uFilter) :
	m_pLoader(NULL),
	m_pUserData(NULL),
	m_Image(),
	m_eWrappingS(uWrapping),
	m_eWrappingT(uWrapping),
	m_eMinFilter(uFilter),
	m_eMagFilter(uFilter),
	m_uDirty(BURGER_MAXUINT),
	m_pD3DTexture(NULL)
{
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::Texture::~Texture()
{
	if (m_pD3DTexture) {
		// Make sure there is no connection to a D3D device
		IDirect3DDevice9 *pDevice;
		m_pD3DTexture->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetTexture(0,0);
		}
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}
	// Release all resources created by loader
	ShutdownImageMemory();
}

Word Burger::Texture::CheckLoad(Display *pDisplay)
{
	Word bLoaded = FALSE;
	D3DTexture *pTexture = m_pD3DTexture;
	D3DDevice *pDevice = pDisplay->GetD3DDevice();
	if (!pTexture) {
		if (LoadImageMemory()) {
			return 10;
		}
		bLoaded = TRUE;
		if (pDevice->CreateTexture(m_Image.GetWidth(),m_Image.GetHeight(),1,0,static_cast<D3DFORMAT>(GetD3DFormat()),D3DPOOL_MANAGED,&m_pD3DTexture,0)<0) {
			UnloadImageMemory();
			return 10;
		}
		pTexture = m_pD3DTexture;
		// Copy the images into the DirectX surfaces.
		for (Word i = 0; i < m_Image.GetMipMapCount(); i++) {
			D3DSurface *pD3DSurface;
			pTexture->GetSurfaceLevel(i,&pD3DSurface);
			D3DLOCKED_RECT DestRect;
			pTexture->LockRect(i,&DestRect,NULL,0);
			WordPtr uBufferSize = m_Image.GetImageSize(i);
			const Word8 *pImage = m_Image.GetImage(i);
			if (m_Image.GetType()==Image::PIXELTYPE8888) {
				uBufferSize>>=2U;
				if (uBufferSize) {
					Word32 *pDest = static_cast<Word32 *>(DestRect.pBits);
					do {
						Word32 uPixel = static_cast<Word32>((pImage[0]<<16U) + (pImage[1]<<8U) + (pImage[2]) + (pImage[3]<<24U));
						pDest[0] = uPixel;
						pImage += 4;
						++pDest;
					} while (--uBufferSize);
				}
			} else if (m_Image.GetType()==Image::PIXELTYPE888) {
				uBufferSize = uBufferSize/3;
				if (uBufferSize) {
					Word32 *pDest = static_cast<Word32 *>(DestRect.pBits);
					do {
						Word32 uPixel = static_cast<Word32>((pImage[0]<<16U) + (pImage[1]<<8U) + (pImage[2]) + 0xFF000000U);
						pDest[0] = uPixel;
						pImage += 3;
						++pDest;
					} while (--uBufferSize);
				}

			} else {
				if (static_cast<WordPtr>(DestRect.Pitch)==m_Image.GetStride(i)) {
					MemoryCopy(DestRect.pBits,m_Image.GetImage(),uBufferSize);
				} else {
					Word uHeight = m_Image.GetHeight(i);
					Word32 *pDest = static_cast<Word32 *>(DestRect.pBits);
					do {
						MemoryCopy(pDest,pImage,m_Image.GetStride());
						pDest += DestRect.Pitch;
						pImage += m_Image.GetStride();
					} while (--uHeight);

				}
			}
			pTexture->UnlockRect(i);
			pD3DSurface->Release();
		}
		UnloadImageMemory();
	}
	Word uResult = 0;
	if (pDevice->SetTexture(0,pTexture)<0) {
		uResult = 10;
	} else if (m_uDirty) {
		m_uDirty = 0;
	}
	return uResult;
}

void Burger::Texture::Release(Display * /* pDisplay */)
{
	if (m_pD3DTexture) {
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}
	m_uDirty = BURGER_MAXUINT;
}

//
// Return the D3DFORMAT for the texture type
//

Word BURGER_API Burger::Texture::GetD3DFormat(void) const
{
	Word uResult;
	switch (m_Image.GetType()) {
	case Image::PIXELTYPEDXT1:
		uResult = D3DFMT_LIN_DXT1;
		break;
	case Image::PIXELTYPEDXT5:
		uResult = D3DFMT_LIN_DXT5;
		break;
	case Image::PIXELTYPE888:
		uResult = D3DFMT_LIN_X8R8G8B8;
		break;
	case Image::PIXELTYPE8888:
		uResult = D3DFMT_LIN_A8R8G8B8;
		break;
	default:
		uResult = D3DFMT_LIN_A8R8G8B8;
		break;
	}
	return uResult;
}

#endif