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

	// Is the minimum records valid?

	if (pSurface->dwFlags & (DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT)) {

		Burger::Display::VideoMode_t Entry;		// Video mode found
		Entry.m_uWidth = pSurface->dwWidth;		// Save the pixel width
		Entry.m_uHeight = pSurface->dwHeight;
		Word uFlags = Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;

		// Was there a refresh rate?
		
		// Refresh rate makes no sense for pad or directly driven
		// LED/LCD panels

		if (pSurface->dwFlags & DDSD_REFRESHRATE) {
			Entry.m_uHertz = pSurface->dwRefreshRate;					// Monitor refresh rate
			uFlags |= Burger::Display::VideoMode_t::VIDEOMODE_REFRESHVALID;
		} else {
			Entry.m_uHertz = 0;			// Force to zero as an alternate way of clearing the rate
		}

		// Check for supported video modes

		// Test for 8 bit paletted format

		Word uDepth = 0;
		if (pSurface->ddpfPixelFormat.dwFlags&DDPF_PALETTEINDEXED8) {
			if (pSurface->ddpfPixelFormat.dwRGBBitCount==8) {
				// 8 bit palette
				uDepth = 8;
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
					uDepth = pSurface->ddpfPixelFormat.dwRGBBitCount;
				}
				break;

			// Drivers can't agree on 15/16 bit modes, so try to
			// sort it out by testing bit masks

			case 15:
			case 16:
				if ((pSurface->ddpfPixelFormat.dwRBitMask==0xF800) &&
					(pSurface->ddpfPixelFormat.dwGBitMask==0x07E0) &&
					(pSurface->ddpfPixelFormat.dwBBitMask==0x001F)) {
						uDepth = 16;
				} else if ((pSurface->ddpfPixelFormat.dwRBitMask==0x7C00) &&
					(pSurface->ddpfPixelFormat.dwGBitMask==0x03E0) &&
					(pSurface->ddpfPixelFormat.dwBBitMask==0x001F)) {
						uDepth = 15;
				}
				break;
			default:
				break;
			}
		}
		// If valid, add to the list
		if (uDepth) {
			Entry.m_uDepth = uDepth;
			Entry.m_uFlags = uFlags;
			pOutput->m_Array.push_back(Entry);
		}
	}
	return DDENUMRET_OK;					// Get another one!
}

//
// Local function to enumerate all video device adapters
//

static HRESULT __stdcall EnumerateVideoDevice(GUID *pGUID,char *pDescription,char *pName,void *pInput,HMONITOR pMonitor)
{
	// Ignore NULL GUID (Primary)
	if (pGUID) {
		Burger::ClassArray<Burger::Display::VideoCardDescription> *pOutput = static_cast<Burger::ClassArray<Burger::Display::VideoCardDescription> *>(pInput);

		// Create a DirectDraw7 instance for queries

		IDirectDraw7 *pDirectDraw7;
		if (Burger::Globals::DirectDrawCreateEx(pGUID,reinterpret_cast<void **>(&pDirectDraw7),IID_IDirectDraw7)==DD_OK) {

			// This has a constructor, so all values are set to zero
			Burger::Display::VideoCardDescription Entry;

			// Get the specific display GUID
			Burger::MemoryCopy(&Entry.m_GUID,pGUID,sizeof(GUID));

			// Set the device enumeration
			Entry.m_uDevNumber = static_cast<Word>(pOutput->size());
			Entry.m_DeviceName = pDescription;		// Copy the name of the video card

			// Get the information for the monitor

			MONITORINFOEXW MonitorInfo;
			MonitorInfo.cbSize = sizeof(MonitorInfo);
			if (GetMonitorInfoW(pMonitor,&MonitorInfo)) {

				// Capture the area of the desktop this monitor resides in
				Entry.m_SystemRect.Set(&MonitorInfo.rcMonitor);
				Entry.m_CurrentResolution.SetRight(static_cast<int>(MonitorInfo.rcMonitor.right-MonitorInfo.rcMonitor.left));
				Entry.m_CurrentResolution.SetBottom(static_cast<int>(MonitorInfo.rcMonitor.bottom-MonitorInfo.rcMonitor.top));

				// Is this the primary monitor?
				if (MonitorInfo.dwFlags & MONITORINFOF_PRIMARY) {
					Entry.m_uFlags |= Burger::Display::VideoCardDescription::VIDEOCARD_PRIMARY;
				}
			}

			// Is it hardware accelerated?
			DDCAPS_DX7 DriverCaps;
			DDCAPS_DX7 HardwareCaps;
			Burger::MemoryClear(&DriverCaps,sizeof(DriverCaps));
			Burger::MemoryClear(&HardwareCaps,sizeof(HardwareCaps));
			DriverCaps.dwSize = sizeof(DriverCaps);
			HardwareCaps.dwSize = sizeof(HardwareCaps);
			if (pDirectDraw7->GetCaps(&DriverCaps,&HardwareCaps)==DD_OK) {
				// Allow 3D?
				if (DriverCaps.dwCaps & DDCAPS_3D) {
					Entry.m_uFlags |= Burger::Display::VideoCardDescription::VIDEOCARD_HARDWARE;
				}
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
			if (pDirectDraw7->EnumDisplayModes(DDEDM_REFRESHRATES,0,&Entry,ModeCallBack)!=DD_OK) {
				// If refresh rates were not permitted, try again without them
				if (pDirectDraw7->EnumDisplayModes(0,0,&Entry,ModeCallBack) != DD_OK) {	// Oh oh...
					bFailed = TRUE;
				}
			}

			// Release the DirectDraw7 instance
			pDirectDraw7->Release();

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
		uResult = 0;		// No error
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

