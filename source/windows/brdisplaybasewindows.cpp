/***************************************

	Display base class

	Windows version

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_WINDOWS)
#include "brgameapp.h"
#include "brglobals.h"
#include "brstring16.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <d3d9.h>
#include <ddraw.h>

/***************************************

	If a window is present, set the text to a specific string

***************************************/

void Burger::Display::SetWindowTitle(const char *pTitle)
{
	HWND pWindow = m_pGameApp->GetWindow();
	// Is the window present?
	if (pWindow) {
		String16 MyString(pTitle);
		// Set the new title text in unicode
		SetWindowTextW(pWindow,reinterpret_cast<LPCWSTR>(MyString.c_str()));
	}
}

//
// Called for each and every display resolution
//

static HRESULT __stdcall ModeCallBack(DDSURFACEDESC2 *pSurface,void *pInput)
{
	Burger::Display::VideoCardDescription *pOutput = static_cast<Burger::Display::VideoCardDescription *>(pInput);
	Burger::Display::VideoMode_t Entry;		// Video mode found

	// Is there a valid size?
	if (pSurface->dwFlags & (DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT)) {
		Entry.m_uWidth = pSurface->dwWidth;		// Save the pixel width
		Entry.m_uHeight = pSurface->dwHeight;
		Entry.m_uFlags = 0;
		Entry.m_uDepth = 0;

		// Was there a refresh rate?
		if (pSurface->dwFlags & DDSD_REFRESHRATE) {
			Entry.m_uHertz = pSurface->dwRefreshRate;					// Monitor refresh rate
		} else {
			Entry.m_uHertz = 0;
		}

		// Check for supported video modes

		// Test for 8 bit paletted format

		if (pSurface->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXED8) {
			if (pSurface->ddpfPixelFormat.dwRGBBitCount==8) {
				// 8 bit palette
				Entry.m_uDepth = 8;
			}

		// True color modes
		} else if (pSurface->ddpfPixelFormat.dwFlags&DDPF_RGB) {
			// Depth
			switch (pSurface->ddpfPixelFormat.dwRGBBitCount) {
			case 32:
			case 24:
				if ((pSurface->ddpfPixelFormat.dwRBitMask==0xFF0000) &&
					(pSurface->ddpfPixelFormat.dwGBitMask==0xFF00) &&
					(pSurface->ddpfPixelFormat.dwBBitMask==0xFF)) {
					Entry.m_uDepth = pSurface->ddpfPixelFormat.dwRGBBitCount;
				}
				break;

			// Drivers can't agree on 15/16 bit modes, so try to
			// sort it out by testing bit masks

			case 15:
			case 16:
				if ((pSurface->ddpfPixelFormat.dwRBitMask==0xF800) &&
					(pSurface->ddpfPixelFormat.dwGBitMask==0x07E0) &&
					(pSurface->ddpfPixelFormat.dwBBitMask==0x001F)) {
						Entry.m_uDepth = 16;
				} else if ((pSurface->ddpfPixelFormat.dwRBitMask==0x7C00) &&
					(pSurface->ddpfPixelFormat.dwGBitMask==0x03E0) &&
					(pSurface->ddpfPixelFormat.dwBBitMask==0x001F)) {
						Entry.m_uDepth = 15;
				}
				break;
			default:
				break;
			}
		}

		// If valid, add to the list
		if (Entry.m_uDepth) {
			pOutput->m_Array.push_back(Entry);
		}
	}
	return DDENUMRET_OK;					// Get another one!
}

//
// Local function to enumerate all video device adapters
//

static HRESULT __stdcall EnumerateVideoDevice(GUID *pGUID,char *pDescription,char *pName,void *pInput,HMONITOR /* pMonitor */)
{
	// Ignore NULL GUID (Primary)
	if (pGUID) {
		Burger::ClassArray<Burger::Display::VideoCardDescription> *pOutput = static_cast<Burger::ClassArray<Burger::Display::VideoCardDescription> *>(pInput);
		IDirectDraw7 *pDD;
		if (Burger::Globals::DirectDrawCreateEx(pGUID,reinterpret_cast<void **>(&pDD),IID_IDirectDraw7)==DD_OK) {
			Burger::Display::VideoCardDescription Entry;

			// Get the monitor GUID
			Burger::MemoryCopy(&Entry.m_GUID,pGUID,sizeof(GUID));
			Entry.m_uDevNumber = static_cast<Word>(pOutput->size());
			Entry.m_DeviceName = pDescription;

			// Is it hardware accelerated?
			DDCAPS DriverCaps;
			DDCAPS HardwareCaps;
			Burger::MemoryClear(&DriverCaps,sizeof(DriverCaps));
			Burger::MemoryClear(&HardwareCaps,sizeof(HardwareCaps));
			DriverCaps.dwSize = sizeof(DriverCaps);
			HardwareCaps.dwSize = sizeof(HardwareCaps);
			if (pDD->GetCaps(&DriverCaps,&HardwareCaps)==DD_OK) {
				// Allow 3D?
				Entry.m_bHardwareAccelerated = DriverCaps.dwCaps & DDCAPS_3D;
			}

			// Get the name of the monitor as found by
			// the device driver. (Will fall back to Generic PnP)

			DISPLAY_DEVICEA MonitorDesc;
			Burger::MemoryClear(&MonitorDesc,sizeof(MonitorDesc));
			MonitorDesc.cb = sizeof(MonitorDesc);
			if (EnumDisplayDevicesA(pName,0,&MonitorDesc,0)) {
				Entry.m_MonitorName = MonitorDesc.DeviceString;
			}

			// Iterate over the display modes
			Word bFailed = FALSE;
			if (pDD->EnumDisplayModes(DDEDM_REFRESHRATES,0,&Entry,ModeCallBack)!=DD_OK) {
				// If refresh rates were not permitted, try again without them
				if (pDD->EnumDisplayModes(0,0,&Entry,ModeCallBack) != DD_OK) {	// Oh oh...
					bFailed = TRUE;
				}
			}
			if (!bFailed) {
				pOutput->push_back(Entry);
			}
		}
	}
	return DDENUMRET_OK;
}

//
// Get the list of video modes available
//

Word Burger::Display::GetVideoModes(ClassArray<VideoCardDescription> *pOutput)
{
	pOutput->clear();
	Word uResult = 10;
	// Enumerate all devices
	if (Globals::DirectDrawEnumerateExA(EnumerateVideoDevice,pOutput,DDENUM_ATTACHEDSECONDARYDEVICES|
		DDENUM_DETACHEDSECONDARYDEVICES|DDENUM_NONDISPLAYDEVICES)==DD_OK) {
			uResult = 0;
	}
	return uResult;
}

//
// Get the default values
//

void BURGER_API Burger::Display::InitGlobals(void)
{
	if (!g_Globals.m_bInitialized) {
		// Get the main monitor device context
		HDC hDC = GetDC(NULL);
		g_Globals.m_uDefaultWidth = static_cast<Word>(GetDeviceCaps(hDC,HORZRES));
		g_Globals.m_uDefaultHeight = static_cast<Word>(GetDeviceCaps(hDC,VERTRES));

		// Get the bit depth
		g_Globals.m_uDefaultDepth = static_cast<Word>(GetDeviceCaps(hDC,PLANES) * GetDeviceCaps(hDC,BITSPIXEL));

		g_Globals.m_uDefaultHertz = static_cast<Word>(GetDeviceCaps(hDC,VREFRESH));

		g_Globals.m_uDefaultTotalWidth = static_cast<Word>(GetSystemMetrics(SM_CXVIRTUALSCREEN)-GetSystemMetrics(SM_XVIRTUALSCREEN));
		g_Globals.m_uDefaultTotalHeight = static_cast<Word>(GetSystemMetrics(SM_CYVIRTUALSCREEN)-GetSystemMetrics(SM_YVIRTUALSCREEN));
		g_Globals.m_uDefaultMonitorCount = static_cast<Word>(GetSystemMetrics(SM_CMONITORS));
		ReleaseDC(NULL,hDC);
		g_Globals.m_bInitialized = TRUE;
	}
}

#endif

