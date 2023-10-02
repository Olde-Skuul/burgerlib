/***************************************

    DirectX 9 manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx9software8.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brgameapp.h"
#include "brglobals.h"
#include "brwindowstypes.h"
#include "brmemoryfunctions.h"

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma disable_message(15)
#endif

#include <d3d9.h>
#include <ddraw.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma enable_message(15)
#endif

#if !defined(DOXYGEN)
#if defined(BURGER_WATCOM)
#define BYTE static uint8_t
#else
#define BYTE static uint8_t __declspec(align(16))
#endif
#include "ps20display8bitdx9.h"
#undef BYTE
#endif

//
// Structure for the quad to draw the main screen
//

#if !defined(DOXYGEN)
struct VERTEXSTRUCT {
	float sx;			// Screen coordinate from -1 to 1
	float sy;
	float sz;			// Not used, set to 0.5f
	float tu;			// UV coords to the offscreen buffer from 0.0f to 1.0f
	float tv;
};
#endif

Burger::DisplayDirectX9Software8::DisplayDirectX9Software8(GameApp *pGameApp) :
	DisplayDirectX9(pGameApp),
	m_pPixelShader8Bit(nullptr),
	m_pBitMapTextureSysMem(nullptr),
	m_pBitMapTexture(nullptr),
	m_pPaletteTexture(nullptr),
	m_pVertexBuffer(nullptr),
	m_bFrontBufferTrueColor(FALSE)
{
	m_pRenderer = &m_Renderer;
	m_Renderer.SetDisplay(this);
}

uint_t Burger::DisplayDirectX9Software8::Init(uint_t uWidth,uint_t uHeight,uint_t /* uDepth */,uint_t uFlags)
{
	uint_t uResult = DisplayDirectX9::Init(uWidth,uHeight,32,uFlags);
	if (!uResult) {
		m_uDepth = 8;

		//
		// Create the vertex buffer for software rendering
		//

		if (AllocateResources()!=D3D_OK) {
			uResult = 10;
		} else {
			m_Renderer.Init(uWidth,uHeight,8,0);
			uint8_t TempPalette[768];
			MemoryClear(TempPalette,sizeof(TempPalette));
			TempPalette[765]=255;
			TempPalette[766]=255;
			TempPalette[767]=255;
			//SetPalette(pSelf,TempPalette);
			FillVertexBuffer();
		}
	}
	return uResult;
}

void Burger::DisplayDirectX9Software8::Shutdown(void)
{
	ReleaseResources();
	DisplayDirectX9::Shutdown();
}

void Burger::DisplayDirectX9Software8::BeginScene(void)
{
}

void Burger::DisplayDirectX9Software8::EndScene(void)
{
	if (m_bPaletteDirty) {
		// Only work if a palette texture has been initialized
		if (m_pPaletteTexture) {
			D3DLOCKED_RECT LockedRect;
			// Don't worry about a system wide lock
			const HRESULT hResult = m_pPaletteTexture->LockRect(0,&LockedRect, nullptr,D3DLOCK_NOSYSLOCK);
			if (hResult==D3D_OK) {
				// Fill in all 256 colors for the palette
				const uint8_t *pPalette = m_Palette;
				uint_t uCount=256;
				uint8_t *pColors = static_cast<uint8_t*>(LockedRect.pBits);
			
				// Note, the format is ARGB little endian. So, it's actually
				// stored as this fake struct
				//	struct colors {
				//		uint8_t m_bBlue;
				//		uint8_t m_bGreen;
				//		uint8_t m_bRed;
				//		uint8_t m_bAlpha;
				//	};

				do {
					pColors[0] = pPalette[2];	// Blue
					pColors[1] = pPalette[1];	// Green
					pColors[2] = pPalette[0];	// Red
					pColors[3] = 255;			// Alpha
					pPalette+=3;				// Next Burgerlib palette entry
					pColors+=4;					// Next ARGB texture entry
				} while (--uCount);
				m_pPaletteTexture->UnlockRect(0);
			}
		}
	}

	// Initialized?
	IDirect3DTexture9 *pBitMapTexture = m_pBitMapTexture;
	if (pBitMapTexture) {
		// If the directX device was lost (Or minimized), yield the CPU
		// to give the device some time to recover
		if (m_bLostDevice || m_pGameApp->IsInBackground()) {
			::Sleep(50);
		}

		// Did the device get lost?

		IDirect3DDevice9 *pDirect3DDevice9 = m_pDirect3DDevice9;
		HRESULT hResult;
		if (m_bLostDevice) {
			hResult = pDirect3DDevice9->TestCooperativeLevel();
			if ((hResult<0) && (hResult!=D3DERR_DEVICELOST)) {
				// Try to restart
				hResult = ResetLostDevice();
				if ((hResult<0) && (hResult!=D3DERR_DEVICELOST) && (!m_uResetAttempts)) {
					// The device has totally failed. 
					// The application is totally boned.
					Globals::SetErrorCode(static_cast<Burger::eError>(67));
					m_pGameApp->SetQuitCode();
				}
			}
		}

		m_pBitMapTextureSysMem->AddDirtyRect(nullptr);
		pDirect3DDevice9->UpdateTexture(m_pBitMapTextureSysMem,m_pBitMapTexture);

		// Render the scene to the display
		hResult = pDirect3DDevice9->BeginScene();
		if (hResult==D3D_OK) {
			pDirect3DDevice9->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
			pDirect3DDevice9->SetVertexShader(nullptr);
			pDirect3DDevice9->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VERTEXSTRUCT));

			// The shader needs the palette and the 8 bit bitmap
			pDirect3DDevice9->SetTexture(0,pBitMapTexture);
			pDirect3DDevice9->SetTexture(1,m_pPaletteTexture);
			pDirect3DDevice9->SetPixelShader(m_pPixelShader8Bit);
			pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
			pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			pDirect3DDevice9->EndScene();
			// Display the final result!
			HWND pWindow = m_pGameApp->GetWindow();
			hResult = pDirect3DDevice9->Present(NULL,NULL,pWindow,NULL);
			if (hResult==D3D_OK) {
				ValidateRect(pWindow,NULL);
			} else {
				if (hResult==D3DERR_DEVICELOST) {
					m_bLostDevice = TRUE;
				// Uh oh... Somehow we lost the device!
				} else if (hResult==D3DERR_DRIVERINTERNALERROR) {
					// The driver has just exploded. Force the application to quit
					m_bLostDevice = TRUE;
					Globals::SetErrorCode(static_cast<Burger::eError>(66));
					m_pGameApp->SetQuitCode();
				}
			}
		}

		// Required to prevent a race condition when rendering this texture
		D3DLOCKED_RECT LockedRect;
		hResult = m_pBitMapTextureSysMem->LockRect(0,&LockedRect,nullptr,0);
		if (hResult == D3D_OK) {
			m_Renderer.SetFrameBuffer(LockedRect.pBits);
			m_Renderer.SetStride(static_cast<uintptr_t>(LockedRect.Pitch));
			m_pBitMapTextureSysMem->UnlockRect(0);
		}
	}
}

/*! ************************************

	\brief Initialize the screen display vertex buffer

	On initialization or screen resizing, a vertex buffer needs to 
	be generated and data filled so that the screen update
	function can operate.

	If the VideoContext was not initialized, this function does nothing.
	
***************************************/

void Burger::DisplayDirectX9Software8::FillVertexBuffer(void)
{
	// Is there a vertex buffer?
	IDirect3DVertexBuffer9 *pVertexBuffer = m_pVertexBuffer;
	if (pVertexBuffer) {
		void *pData;
		// Lock down the vertex buffer
		HRESULT hResult = pVertexBuffer->Lock(0,0,&pData,D3DLOCK_DISCARD);
		if (hResult==D3D_OK) {

			// Due to pixels needing to be sourced from the
			// center of each cell, a pixel half step is added to the
			// destination to ensure sampling is done at a 
			// 1:1 ratio so no distortion is performed on result.

			// 1.0f is actually half the range (-1.0f -> 1.0f = 2.0f)

			//Display::DisplayContextCallbacks *pVideo = m_pCallBacks;
			//float fWidthAdjust = 1.0f/static_cast<float>(pVideo->m_uWidth);
			//float fHeightAdjust = 1.0f/static_cast<float>(pVideo->m_uHeight);
			
			// Fill in the vertex buffer
			VERTEXSTRUCT *pVertexs = static_cast<VERTEXSTRUCT *>(pData);
			pVertexs[0].sx = -1.0f;	//+fWidthAdjust;
			pVertexs[0].sy = -1.0f;	//+fHeightAdjust;
			pVertexs[0].sz = 0.5f;
			pVertexs[0].tu = 0.0f;
			pVertexs[0].tv = 1.0f;

			pVertexs[1].sx = 1.0f;	//-fWidthAdjust;
			pVertexs[1].sy = -1.0f;	//+fHeightAdjust;
			pVertexs[1].sz = 0.5f;
			pVertexs[1].tu = 1.0f;
			pVertexs[1].tv = 1.0f;
	
			pVertexs[2].sx = -1.0f;	//+fWidthAdjust;
			pVertexs[2].sy = 1.0f;	//+fHeightAdjust;
			pVertexs[2].sz = 0.5f;
			pVertexs[2].tu = 0.0f;
			pVertexs[2].tv = 0.0f;

			pVertexs[3].sx = 1.0f;	//-fWidthAdjust;
			pVertexs[3].sy = 1.0f;	//+fHeightAdjust;
			pVertexs[3].sz = 0.5f;
			pVertexs[3].tu = 1.0f;
			pVertexs[3].tv = 0.0f;
			pVertexBuffer->Unlock();
		}
	}
}

/*! ************************************

	\brief Initialize resources needed to draw a video context

	This is called on device creation or device reset
	
***************************************/

long Burger::DisplayDirectX9Software8::AllocateResources(void)
{
	// Create the pixel shader for 8 bit rendering

	HRESULT hResult = m_pDirect3DDevice9->CreatePixelShader((DWORD *)g_ps20display8bitdx9,&m_pPixelShader8Bit);
	if (hResult == D3D_OK) {
		// Create the back buffer for 8 bit graphics

		hResult = m_pDirect3DDevice9->CreateTexture(m_uWidth,m_uHeight,1,0,D3DFMT_A8,D3DPOOL_DEFAULT,&m_pBitMapTexture,nullptr);
		if (hResult == D3D_OK) {

			// Create the back buffer for 8 bit graphics

			hResult = m_pDirect3DDevice9->CreateTexture(m_uWidth,m_uHeight,1,0,D3DFMT_A8,D3DPOOL_SYSTEMMEM,&m_pBitMapTextureSysMem, nullptr);
			if (hResult == D3D_OK) {
						// Lock the offscreen buffer again without the system lock
				D3DLOCKED_RECT LockedRect;
				hResult = m_pBitMapTextureSysMem->LockRect(0,&LockedRect, nullptr,0);
				if (hResult == D3D_OK) {
					m_Renderer.SetFrameBuffer(LockedRect.pBits);
					m_Renderer.SetStride(static_cast<uintptr_t>(LockedRect.Pitch));
					m_pBitMapTextureSysMem->UnlockRect(0);
				}

				// Create the one dimensional palette

				hResult = m_pDirect3DDevice9->CreateTexture(256,1,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,&m_pPaletteTexture, nullptr);
				if (hResult == D3D_OK) {

					// Create the vertex buffer for software rendering

					hResult = m_pDirect3DDevice9->CreateVertexBuffer(sizeof(VERTEXSTRUCT)*4,D3DUSAGE_WRITEONLY,D3DFVF_XYZ|D3DFVF_TEX1,D3DPOOL_DEFAULT,&m_pVertexBuffer, nullptr);
				}
			}
		}
	}
	return hResult;
}

/*! ************************************

	\brief Release resources needed to draw a video context

	This is called on device shutdown or device reset
	
***************************************/

void Burger::DisplayDirectX9Software8::ReleaseResources(void)
{
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if (m_pPaletteTexture) {
		m_pPaletteTexture->Release();
		m_pPaletteTexture = NULL;
	}
	if (m_pBitMapTextureSysMem) {
		m_pBitMapTextureSysMem->Release();
		m_pBitMapTextureSysMem = NULL;
	}
	if (m_pBitMapTexture) {
		m_pBitMapTexture->UnlockRect(0);
		m_pBitMapTexture->Release();
		m_pBitMapTexture = NULL;
	}
	if (m_pPixelShader8Bit) {
		m_pPixelShader8Bit->Release();
		m_pPixelShader8Bit = NULL;
	}
}

/*! ************************************

	\brief Restart a lost device

	This is called on device shutdown or device reset
	
***************************************/

long Burger::DisplayDirectX9Software8::ResetLostDevice(void)
{
	ReleaseResources();
	D3DPRESENT_PARAMETERS Parms;
	m_D3D9Settings.GetPresentParameters(&Parms);
	HRESULT hResult = m_pDirect3DDevice9->Reset(&Parms);
	if (hResult<0) {
		if (hResult!=D3DERR_DEVICELOST) {
			--m_uResetAttempts;
		} else {
			m_uResetAttempts = DIRECTXRESETATTEMPTS;
		}
	} else {
		hResult = AllocateResources();
	}
	return hResult;
}

#endif
