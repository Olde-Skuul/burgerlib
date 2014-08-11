/***************************************

	DirectX 9 manager class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx9.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brwindowsapp.h"
#include "brglobals.h"

#if !defined(DIRECTDRAW_VERSION) && !defined(DOXYGEN)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(DIRECT3D_VERSION) && !defined(DOXYGEN)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <d3d9.h>
#include <ddraw.h>

Burger::DisplayDirectX9::DisplayDirectX9(GameApp *pGameApp) :
	Display(pGameApp,DIRECTX9),
	m_pDirect3D9(NULL),
	m_pDirect3DDevice9(NULL),
	m_bLostDevice(FALSE)
{
	SetRenderer(NULL);
}

Word Burger::DisplayDirectX9::InitContext(void)
{
//	Word uDepth = m_uDepth;

	// Set the new size of the screen
	Word uWidth = m_uWidth;
	Word uHeight = m_uHeight;
	m_uFlags |= FULLPALETTEALLOWED;

	//
	// Create my directx9 instance and store it
	//
	IDirect3D9 *pDirect3D9 = Globals::Direct3DCreate9(DIRECT3D_VERSION);
	if (!pDirect3D9) {
		return 10;		// Boned?
	}
	m_pDirect3D9 = pDirect3D9;

	//
	// Select the adapter mode
	//

	D3DDISPLAYMODE SelectedDisplayMode;
	HRESULT hResult = pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&SelectedDisplayMode);
	if (hResult != D3D_OK) {
		return 10;
	}

	//
	// Set up the window
	//

	if (m_uFlags & FULLSCREEN) {
		// Get the application's window

		HWND pWindow = Globals::GetWindow();

		// Get the style of the window 
		LONG dwStyle = GetWindowLongW(pWindow,GWL_STYLE);
		dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
		dwStyle |= WS_POPUP | WS_OVERLAPPED | WS_EX_TOPMOST;		// Can't be a pop-up window
		SetWindowLongW(pWindow,GWL_STYLE,dwStyle);					// Set the style

		// Position the window on the screen in the center or where it was located last time
		SetWindowPos(pWindow,HWND_TOP,0,0,static_cast<int>(uWidth),static_cast<int>(uHeight),SWP_NOZORDER | SWP_NOACTIVATE);

		// Set the style (Makes it visible)
		ShowWindow(pWindow,SW_SHOWNORMAL);
	} else {
		static_cast<WindowsApp *>(m_pGameApp)->SetWindowSize(uWidth,uHeight);
	}

	m_uBackBufferFormat = SelectedDisplayMode.Format;

	//
	// Create my D3D device
	//

	D3DPRESENT_PARAMETERS DisplayPresentParms;
	CreatePresentParameters(&DisplayPresentParms);
	DisplayPresentParms.BackBufferWidth = SelectedDisplayMode.Width;
	DisplayPresentParms.BackBufferHeight = SelectedDisplayMode.Height;

	HWND pWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();

	hResult = pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,&DisplayPresentParms,&m_pDirect3DDevice9);
	if (hResult != D3D_OK) {
		hResult = pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,&DisplayPresentParms,&m_pDirect3DDevice9);
		if (hResult != D3D_OK) {
			return 10;
		}
	}
	m_bLostDevice = FALSE;

	// 
	// Determine if power of 2 textures are required
	//
	m_pDirect3DDevice9->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);
	m_pDirect3DDevice9->Present(NULL,NULL,pWindow,NULL);

	D3DCAPS9 Caps;
	m_pDirect3DDevice9->GetDeviceCaps(&Caps);
	m_bPower2Textures = (Caps.TextureCaps & (D3DPTEXTURECAPS_NONPOW2CONDITIONAL|D3DPTEXTURECAPS_POW2))==D3DPTEXTURECAPS_POW2;

	return 0;
}

void Burger::DisplayDirectX9::PostShutdown(void)
{
	if (m_pDirect3DDevice9) {
		// Because there was a video context, capture the location
		// of the window, so if the window was re-opened, use it's
		// old location
		if (!(m_uFlags & Display::FULLSCREEN)) {
			static_cast<WindowsApp *>(m_pGameApp)->RecordWindowLocation();
		}
		if (!m_bLostDevice) {
			m_pDirect3DDevice9->Release();
		}
		m_pDirect3DDevice9 = NULL;
	}
	if (m_pDirect3D9) {
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
}

void Burger::DisplayDirectX9::PostEndScene(void)
{
	// Display the final result!
	IDirect3DDevice9 *pDirect3DDevice9 = m_pDirect3DDevice9;
	if (pDirect3DDevice9) {
		HWND pWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();
		HRESULT hResult = pDirect3DDevice9->Present(NULL,NULL,pWindow,NULL);
		if (hResult==D3D_OK) {
			ValidateRect(pWindow,NULL);
		} else {
			if (hResult==D3DERR_DEVICELOST) {
				m_bLostDevice = TRUE;
			// Uh oh... Somehow we lost the device!
			} else if (hResult==D3DERR_DRIVERINTERNALERROR) {
				// The driver has just exploded. Force the application to quit
				m_bLostDevice = TRUE;
				Globals::SetErrorCode(66);
				m_pGameApp->SetQuitCode();
			}
		}
	}
}

/*! ************************************

	\brief Initialize a directx D3DPRESENT_PARAMETERS structure.

	This is called on device creation or device reset
	
***************************************/

void Burger::DisplayDirectX9::CreatePresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput)
{
	MemoryClear(pOutput,sizeof(*pOutput));

	// Shared values
	pOutput->hDeviceWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();
	pOutput->SwapEffect = D3DSWAPEFFECT_DISCARD;
	pOutput->BackBufferFormat = static_cast<D3DFORMAT>(m_uBackBufferFormat);
	pOutput->BackBufferWidth = m_uWidth;
	pOutput->BackBufferHeight = m_uHeight;

	if (m_uFlags & FULLSCREEN) {
		pOutput->BackBufferCount = 1;
		pOutput->MultiSampleType = D3DMULTISAMPLE_NONE;
		pOutput->MultiSampleQuality = 0;
		pOutput->Windowed = FALSE;
		pOutput->EnableAutoDepthStencil = TRUE;
		pOutput->AutoDepthStencilFormat = D3DFMT_D24X8;
		pOutput->Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		pOutput->FullScreen_RefreshRateInHz = 0;
		pOutput->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		pOutput->Windowed = TRUE;
	}
}

#endif
