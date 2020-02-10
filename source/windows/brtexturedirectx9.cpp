/***************************************

    Texture for rendering class, DirectX9 version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtexturedirectx9.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#include "brglobalmemorymanager.h"
#include "brdisplaydirectx9.h"
#include "brmemoryfunctions.h"
#include <d3d9.h>

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::TextureDirectX9,Burger::Texture);
#endif

/*! ************************************

	\class Burger::TextureDirectX9
	\brief Class for containing raw images
	
	This class is the mechanism for loading and saving popular file formats
	for image files such as TGA, LBM, PCX, PNG and the like. It's used as
	input for the Burger::Texture manager to convert the data from an image into
	a hardware texture for GPU rendering. Graphics tools can
	use this class exclusively for image conversion and manipulation

	\sa Texture or TextureOpenGL

***************************************/

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty image

***************************************/

Burger::TextureDirectX9::TextureDirectX9() :
	Texture(),
	m_pD3DTexture(NULL)
{
}

Burger::TextureDirectX9::TextureDirectX9(eWrapping uWrapping,eFilter uFilter) :
	Texture(uWrapping,uFilter),
	m_pD3DTexture(NULL)
{
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::TextureDirectX9::~TextureDirectX9()
{
	if (m_pD3DTexture) {
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}
}

Word Burger::TextureDirectX9::CheckLoad(Display *pDisplay)
{
	Word bLoaded = FALSE;
	IDirect3DTexture9 *pTexture = m_pD3DTexture;
	IDirect3DDevice9 *pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
	if (!pTexture || (m_uDirty&DIRTY_IMAGE)) {
		if (LoadImageMemory()) {
			return 10;
		}
		bLoaded = TRUE;
		if (!pTexture) {
			if (pDevice->CreateTexture(m_Image.GetWidth(),m_Image.GetHeight(),1,0,static_cast<D3DFORMAT>(GetD3DFormat()),D3DPOOL_MANAGED,&m_pD3DTexture,0)<0) {
				UnloadImageMemory();
				return 10;
			}
			pTexture = m_pD3DTexture;
		}
		// Copy the images into the DirectX surfaces.
		for (Word i = 0; i < m_Image.GetMipMapCount(); i++) {
			IDirect3DSurface9 *pD3DSurface;
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
	} else {
		m_uDirty = m_uDirty&(~(DIRTY_IMAGE|DIRTY_WRAPPING_S|DIRTY_WRAPPING_T|DIRTY_MIN|DIRTY_MAG));
	}
	return uResult;
}

void Burger::TextureDirectX9::Release(Display * /* pDisplay */)
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

Word BURGER_API Burger::TextureDirectX9::GetD3DFormat(void) const
{
	Word uResult;
	switch (m_Image.GetType()) {
	case Image::PIXELTYPEDXT1:
		uResult = D3DFMT_DXT1;
		break;
	case Image::PIXELTYPEDXT5:
		uResult = D3DFMT_DXT5;
		break;
	case Image::PIXELTYPE888:
		uResult = D3DFMT_X8R8G8B8;
		break;
	case Image::PIXELTYPE8888:
		uResult = D3DFMT_A8R8G8B8;
		break;
	default:
		uResult = D3DFMT_A8R8G8B8;
		break;
	}
	return uResult;
}

#endif