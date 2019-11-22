/***************************************

	Global variable manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdisplay.h"
#include "brfilemanager.h"
#include "brgameapp.h"
#include "brkeyboard.h"
#include "brmouse.h"
#include "brstring.h"
#include "brstring16.h"
#include "brnumberto.h"
#include "brwindowstypes.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not
// have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x800
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(BUILD_WINDOWS)
#define BUILD_WINDOWS
#endif

#include <Windows.h>

#include <MMReg.h>
#include <SetupAPI.h>
#include <WindowsX.h>
#include <Xinput.h>
#include <d3d.h>
#include <d3dcommon.h>
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
#include <io.h>
#include <shellapi.h>
#include <shlobj.h>
#include <xaudio2.h>

//
// These defines are missing from some versions of windows.h
// especially when building against older versions of the
// windows SDK (Necessary, since some obscure compilers
// don't ship with up to date headers)
//

#if !defined(GET_SC_WPARAM)
#define GET_SC_WPARAM(wParam) ((int)wParam & 0xFFF0)
#endif

#if !defined(PBT_APMQUERYSUSPEND)
#define PBT_APMQUERYSUSPEND 0x0000
#endif

#if !defined(PBT_APMRESUMESUSPEND)
#define PBT_APMRESUMESUSPEND 0x0007
#endif

#if !defined(WM_KEYF1)
#define WM_KEYF1 0x004D
#endif

#if !defined(WM_UAHDESTROYWINDOW)
#define WM_UAHDESTROYWINDOW 0x0090
#endif

#if !defined(WM_UAHDRAWMENU)
#define WM_UAHDRAWMENU 0x0091
#endif

#if !defined(WM_UAHDRAWMENUITEM)
#define WM_UAHDRAWMENUITEM 0x0092
#endif

#if !defined(WM_UAHINITMENU)
#define WM_UAHINITMENU 0x0093
#endif

#if !defined(WM_UAHMEASUREMENUITEM)
#define WM_UAHMEASUREMENUITEM 0x0094
#endif

#if !defined(WM_UAHNCPAINTMENUPOPUP)
#define WM_UAHNCPAINTMENUPOPUP 0x0095
#endif

#if !defined(WM_NCUAHDRAWCAPTION)
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif

#if !defined(WM_NCUAHDRAWFRAME)
#define WM_NCUAHDRAWFRAME 0x00AF
#endif

#if !defined(WM_INPUT_DEVICE_CHANGE)
#define WM_INPUT_DEVICE_CHANGE 0x00FE
#endif

#if !defined(WM_INPUT)
#define WM_INPUT 0x00FF
#endif

#if !defined(WM_GESTURE)
#define WM_GESTURE 0x0119
#endif

#if !defined(WM_GESTURENOTIFY)
#define WM_GESTURENOTIFY 0x011A
#endif

#if !defined(WM_MOUSEHWHEEL)
#define WM_MOUSEHWHEEL 0x020E
#endif

#if !defined(WM_TOUCH)
#define WM_TOUCH 0x0240
#endif

#if !defined(WM_NCMOUSEHOVER)
#define WM_NCMOUSEHOVER 0x02A0
#endif

#if !defined(WM_NCMOUSELEAVE)
#define WM_NCMOUSELEAVE 0x02A2
#endif

#if !defined(WM_CLIPBOARDUPDATE)
#define WM_CLIPBOARDUPDATE 0x031D
#endif

#if !defined(WM_DWMCOMPOSITIONCHANGED)
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

#if !defined(WM_DWMNCRENDERINGCHANGED)
#define WM_DWMNCRENDERINGCHANGED 0x031F
#endif

#if !defined(WM_DWMCOLORIZATIONCOLORCHANGED)
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif

#if !defined(WM_DWMWINDOWMAXIMIZEDCHANGE)
#define WM_DWMWINDOWMAXIMIZEDCHANGE 0x0321
#endif

static const char g_SoftwareClasses[] = "Software\\Classes\\";

//
// Globals
//

const Word16 Burger::Globals::g_GameClass[] = {'B', 'u', 'r', 'g', 'e', 'r',
	'G', 'a', 'm', 'e', 'C', 'l', 'a', 's', 's', 0}; //"BurgerGameClass";
ATOM Burger::Globals::g_uAtom =
	INVALID_ATOM; ///< Atom assigned to my class (Windows only)
HWND Burger::Globals::g_hWindow;
Word32 Burger::Globals::g_uQuickTimeVersion;
Word8 Burger::Globals::g_bQuickTimeVersionValid;
Word32 Burger::Globals::g_uDirectXVersion;
Word8 Burger::Globals::g_bDirectXVersionValid;
#if defined(BURGER_WIN32) || defined(DOXYGEN)
Word8 Burger::Globals::g_bIsWindows64Bit;
#endif

#endif // Allow doxygen


/*! ************************************

	\fn const Word16 *Burger::Globals::GetWindowClassName()
	\brief Get the registered class name

	When registering a window class for Burgerlib, this is the name
	used to declare it. It's a wchar_t string of "BurgerGameClass"

	\windowsonly

	\return Pointer to a wchar_t * compatible pointer

	\sa Globals::RegisterWindowClass(Word)

***************************************/


/*! ************************************

	\fn Burger::Globals::GetWindow()
	\brief Get the application window
	
	\windowsonly

	\return Window set by Globals::SetWindow()

	\sa Globals::SetWindow()

***************************************/

/*! ************************************

	\fn Burger::Globals::SetWindow(HWND__ *)
	\brief Set the application window

	A window is created on application startup, it's pointer
	is stored via this call so other parts of Burgerlib
	can use this window for other systems.

	\windowsonly
	\param pInput Instance of the application

	\sa Globals::GetWindow()
	
***************************************/

#if defined(BURGER_WIN32) || defined(DOXYGEN)

/*! ************************************

	\brief Detect if a 32 bit windows app is running under 64 bit windows

	If an application is 32 bits, the operating system is tested if it's
	actually 64 bits. If so, the function will return \ref TRUE. If
	the application is 64 bits, or if the operating system is 32 bit
	then this function will return \ref FALSE.

	\windowsonly
	\return Returns \ref TRUE if the 32 bit application was running in 64 bit
	Windows \sa GetSystemWow64DirectoryW(Word16 *,Word32)

***************************************/

Word BURGER_API Burger::Globals::IsWindows64Bit(void)
{
	Word bResult = g_bIsWindows64Bit;
	// Was it already tested?
	if (!(bResult & 0x80)) {
		Word16 Temp[MAX_PATH];
		if ((Windows::GetSystemWow64DirectoryW(Temp, BURGER_ARRAYSIZE(Temp)) == 0)
			&& (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)) {
			bResult = 0x80
				| FALSE; // Not present? This is a 32 bit version of Windows
		} else {
			bResult = 0x80 | TRUE; // The 32 bit app is running in a 64 bit
								   // version of Windows
		}
		g_bIsWindows64Bit = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult & 1U;
}

#endif


/*! ************************************

	\brief Returns a 64 bit version of a file.

	Given a filename in Windows Unicode format, open the file and return
	the 64 bit Windows extended version number from the dwFileVersionMS
	and dwFileVersionLS entries in the VS_FIXEDFILEINFO structure.
	
	\return Version in 64 bit Windows format or 0 on error

***************************************/

Word64 BURGER_API Burger::Globals::GetFileVersion64(
	const Word16* pWindowsFilename)
{
	Word64 uResult = 0;
	if (pWindowsFilename) {
		DWORD uNotUsed;
		// Get the size of the data
		UINT uBufferSize = Windows::GetFileVersionInfoSizeW(pWindowsFilename, &uNotUsed);
		if (uBufferSize) {
			HANDLE hHeap = GetProcessHeap();
			BYTE* pFileVersionBuffer =
				static_cast<BYTE*>(HeapAlloc(hHeap, 0, uBufferSize));
			if (pFileVersionBuffer) {
				// Load the data
				if (Windows::GetFileVersionInfoW(
						pWindowsFilename, 0, uBufferSize, pFileVersionBuffer)) {
					VS_FIXEDFILEINFO* pVersion = NULL;
					// Extract the version value
					if (Windows::VerQueryValueW(pFileVersionBuffer,
							reinterpret_cast<const Word16*>(L"\\"),
							(VOID**)&pVersion, &uBufferSize)) {
						if (pVersion != NULL) {
							uResult =
								(static_cast<Word64>(pVersion->dwFileVersionMS)
									<< 32U)
								+ pVersion->dwFileVersionLS;
						}
					}
				}
				// Release the buffer
				HeapFree(hHeap, 0, pFileVersionBuffer);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of DirectX by scanning file versions

	Tries to get the DirectX version by looking at DirectX file versions
	Uses http://en.wikipedia.org/wiki/DirectX as a reference for file version
	lookups \return Version number of DirectX in 16 bit format 0x900 is 9.0, 0x903
	= 9.0c

***************************************/

Word BURGER_API Burger::Globals::GetDirectXVersionViaFileVersions(void)
{
	Word16 szPath[MAX_PATH * 2];

	Word uResult = 0;
	if (GetSystemDirectoryW(reinterpret_cast<LPWSTR>(szPath), MAX_PATH) != 0) {

		szPath[MAX_PATH - 1] = 0; // Failsafe

		// Switch off the ddraw version
		WordPtr uLen = StringLength(szPath);
		Word16* pDest = szPath + uLen;
		WordPtr uRemaining = sizeof(szPath) - (uLen * sizeof(Word16));

		StringCopy(
			pDest, uRemaining, reinterpret_cast<const Word16*>(L"\\ddraw.dll"));

		Word64 uVersionDDraw = GetFileVersion64(szPath);
		if (uVersionDDraw >= 0x000400020000005FULL) { // Win9x version
			// file is >= DX1.0 version, so we must be at least DX1.0
			uResult = 0x0100;
		}
		if (uVersionDDraw >= 0x0004000300000448ULL) { // Win9x version
			// file is is >= DX2.0 version, so we must DX2.0 or DX2.0a (no
			// redist change)
			uResult = 0x0200;
		}
		if (uVersionDDraw >= 0x0004000400000044ULL) { // Win9x version
			// file is is >= DX3.0 version, so we must be at least DX3.0
			uResult = 0x0300;
		}

		// Switch off the d3drg8x.dll version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const Word16*>(L"\\d3drg8x.dll"));
		Word64 uVersionD3Drg8x = GetFileVersion64(szPath);
		if (uVersionD3Drg8x >= 0x0004000400000046ULL) { // Win9x version
			// d3drg8x.dll is the DX3.0a version, so we must be DX3.0a or DX3.0b
			// (no redist change)
			uResult = 0x301;
		}

		// No DirectX 4

		// Switch off the ddraw version
		if (uVersionDDraw >= 0x000400050000009BULL) { // Win9x version
			// ddraw.dll is the DX5.0 version, so we must be DX5.0 or DX5.2 (no
			// redist change)
			uResult = 0x0500;
		}

		if (uVersionDDraw >= 0x000400060000013EULL) { // Win9x version
			// ddraw.dll is the DX6.0 version, so we must be at least DX6.0
			uResult = 0x0600;
		}

		if (uVersionDDraw >= 0x00040006000001B4) { // Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1
			uResult = 0x0610;
		}

		// Switch off the dplayx.dll version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const Word16*>(L"\\dplayx.dll"));
		Word64 uVersionDPlayx = GetFileVersion64(szPath);
		if (uVersionDPlayx >= 0x0004000600030206ULL) { // Win9x version
			// ddraw.dll is the DX6.1 version, so we must be at least DX6.1a
			uResult = 0x0611;
		}

		// Switch off the ddraw version
		if (uVersionDDraw >= 0x00040007000002BC) { // Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0
			uResult = 0x0700;
		}

		// Switch off the dinput version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const Word16*>(L"\\dinput.dll"));
		Word64 uVersionDInput = GetFileVersion64(szPath);
		if (uVersionDInput >= 0x00040007000002CC) { // Win9x version
			// ddraw.dll is the DX7.0 version, so we must be at least DX7.0a
			uResult = 0x0701;
		}

		// Switch off the ddraw version
		if ((((uVersionDDraw & 0xFFFF000000000000ULL) == 0x0004000000000000ULL)
				&& (uVersionDDraw >= 0x0004000800000190ULL))
			|| // Win9x version
			(((uVersionDDraw & 0xFFFF000000000000ULL) == 0x0005000000000000ULL)
				&& (uVersionDDraw
					   >= 0x0005000108D20190ULL))) { // Win2k/WinXP version
			// ddraw.dll is the DX8.0 version, so we must be at least DX8.0 or
			// DX8.0a (no redist change)
			uResult = 0x0800;
		}

		// Switch off the d3d8 version
		StringCopy(
			pDest, uRemaining, reinterpret_cast<const Word16*>(L"\\d3d8.dll"));
		Word64 uVersionD3D8 = GetFileVersion64(szPath);
		if ((((uVersionD3D8 & 0xFFFF000000000000ULL) == 0x0004000000000000ULL)
				&& (uVersionD3D8 >= 0x0004000800010371ULL))
			|| // Win9x version
			(((uVersionD3D8 & 0xFFFF000000000000ULL) == 0x0005000000000000ULL)
				&& (uVersionD3D8
					   >= 0x000500010A280371ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1 version, so we must be at least DX8.1
			uResult = 0x0810;
		}

		if ((((uVersionD3D8 & 0xFFFF000000000000ULL) == 0x0004000000000000ULL)
				&& (uVersionD3D8 >= 0x0004000800010385ULL))
			|| // Win9x version
			(((uVersionD3D8 & 0xFFFF000000000000ULL) == 0x0005000000000000ULL)
				&& (uVersionD3D8
					   >= 0x000500010A280385ULL))) { // Win2k/WinXP version
			// d3d8.dll is the DX8.1a version, so we must be at least DX8.1a
			uResult = 0x0811;
		}

		// Switch off the Mpg2splt version
		StringCopy(pDest, uRemaining,
			reinterpret_cast<const Word16*>(L"\\mpg2splt.ax"));
		Word64 uVersionMPG2Splt = GetFileVersion64(szPath);
		if (uVersionMPG2Splt
			>= 0x0006000300010375ULL) { // Win9x/Win2k/WinXP version
			// quartz.dll is the DX8.1b version, so we must be at least DX8.1b
			uResult = 0x0812;
		}

		// Switch off the dpnet version
		StringCopy(
			pDest, uRemaining, reinterpret_cast<const Word16*>(L"\\dpnet.dll"));
		Word64 uVersionDPNet = GetFileVersion64(szPath);
		if ((((uVersionDPNet & 0xFFFF000000000000ULL) == 0x0004000000000000ULL)
				&& (uVersionDPNet >= 0x0004000900000086ULL))
			|| // Win9x version
			(((uVersionDPNet & 0xFFFF000000000000ULL) == 0x0005000000000000ULL)
				&& (uVersionDPNet
					   >= 0x000500020E5D0086ULL))) { // Win2k/WinXP version
			// dpnet.dll is the DX8.2 version, so we must be at least DX8.2
			uResult = 0x0820;
		}

		// Switch off the d3d9 version
		StringCopy(
			pDest, uRemaining, reinterpret_cast<const Word16*>(L"\\d3d9.dll"));
		Word64 uVersionD3D9 = GetFileVersion64(szPath);

		if (uVersionD3D9) {
			// File exists, so it must be at least DX9
			uResult = 0x0900; // 9.0
		}

		if (uVersionD3D9 >= 0x0004000900000385ULL) { // 4.09.00.0901
			uResult = 0x0901;						 // 9.0a
		}

		if (uVersionD3D9 >= 0x0004000900000386ULL) { // 4.09.00.0902
			uResult = 0x0902;						 // 9.0b
		}

		if (uVersionD3D9 >= 0x0004000900000387ULL) { // 4.09.00.0903
			uResult = 0x0903;						 // 9.0c
		}

		// DirectX 10
		if (uVersionD3D9 >= 0x0006000017704002ULL) { // 6.00.6000.16386
			uResult = 0x0A00;						 // 10.0
		}
		if (uVersionD3D9 >= 0x0006000017714650ULL) { // 6.00.6001.18000
			uResult = 0x0A10;						 // 10.1
		}

		// DirectX 11
		if (uVersionD3D9 >= 0x00060000177246BBULL) { // 6.00.6002.18107
			uResult = 0x0B00;						 // 11.0
		}
		if (uVersionD3D9 >= 0x0006000223F04000ULL) { // 6.02.9200.16384
			uResult = 0x0B10;						 // 11.1
		}

		if (uVersionD3D9 >= 0x0006000225804000ULL) { // 6.02.9600.16384
			uResult = 0x0B20;						 // 11.2
		}

		if (uVersionD3D9 >= 0x00060002383901BFULL) { // 6.02.14393.447
			uResult = 0x0B30;						 // 11.3
		}

		//		if (uVersionD3D9>=0x000A000028004000ULL) {
		//// 10.00.10240.16384 (Not found yet) 			uResult = 0x0C00;
		//// 12.0
		//		}
	}
	return uResult;
}

/*! ************************************

	\brief Return the version of DirectX.
	


	Detect if DirectX is available, and if so, query
	it for the version present. If DirectX is not available,
	the version returned is zero.
	


	This function is written so it only asks for the version
	once from DirectX. It will cache the version and
	return the cached value on subsequent calls.
	


	By invoking DEEP magic, I will divine the version
	of DirectX that is present. It will do a manual
	check of the system folder for the DLLs
	and pull the version resource
	from them.

	\windowsonly
	\return Version in the format of 0x0902 -> 9.2, 0xB01 = 11.0.1

***************************************/

Word BURGER_API Burger::Globals::GetDirectXVersion(void)
{
	// There is a version number in the registry, however, it is only valid for
	// DirectX versions 1 through 9.0c. The registry key is not valid for
	// DirectX 10 and higher

	// The same issue exists for obtaining the version of DirectX using the
	// "Dialog" string, it tops out at DirectX 9, and doesn't properly map to
	// versions of DirectX later than 11

	if (!g_bDirectXVersionValid) {
		g_bDirectXVersionValid = TRUE; // I got the version
		Word uResult = GetDirectXVersionViaFileVersions();
		g_uDirectXVersion = uResult;
	}
	// Return the DirectX version
	return g_uDirectXVersion;
}

/***************************************

	This callback is used to find a specific GUID for
	an enumerated device

	Note: Do NOT use the pName or pDeviceName parameters since it's
	unknown if the ASCII or wchar_t call was issued to call this
	function

***************************************/

#if !defined(DOXYGEN)
struct DeviceGuid_t {
	GUID* m_pGUID;  // Buffer to store the located GUID
	Word m_uDevNum; // Count down
};
#endif

static int CALLBACK FindDeviceCallback(GUID* pGUID, LPSTR /* pName */,
	LPSTR /* pDeviceName */, void* pThis, void* /* pMonitor */)
{
	DeviceGuid_t* pRef = static_cast<DeviceGuid_t*>(pThis); // Deref the pointer
	int iResult = DDENUMRET_OK;								// Keep going
	if (!--pRef->m_uDevNum) { // Found the device yet?
		if (pGUID) {		  // Specific device?
			// Copy the GUID
			Burger::MemoryCopy(pRef->m_pGUID, pGUID, sizeof(GUID));
		} else {
			// Clear the GUID
			Burger::MemoryClear(pRef->m_pGUID, sizeof(GUID));
		}
		iResult = DDENUMRET_CANCEL; // Stop now
	}
	return iResult;
}

/*! ************************************

    \brief Given a specific device number, return the DirectX GUID

    Scan the device list for the GUID of the requested device.
    Device #0 returns the global display device (All screens)

    \windowsonly
    \param pOutput Pointer to a buffer to accept the returned GUID. Cannot be
        \ref nullptr
    \param uDevNum 0 for the master global device, 1-??? for the enumerated
        displays

    \return Zero if no error, non-zero if an error has occurred

***************************************/

Word BURGER_API Burger::Globals::GetVideoGUID(GUID* pOutput, Word uDevNum)
{
	HRESULT uError = E_FAIL;
	if (pOutput) {

		DeviceGuid_t Ref;
		Ref.m_pGUID =
			pOutput; // Set the pointer to the GUID to store the result
		Ref.m_uDevNum = ++uDevNum; // Scan for this device

		uError = static_cast<HRESULT>(Windows::DirectDrawEnumerateExW(FindDeviceCallback,
			&Ref,
			DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES
				| DDENUM_NONDISPLAYDEVICES));
		// The nVidia GT 545 fails on this call, so call using the 8 Bit Ascii
		// version instead
		if (uError == E_NOTIMPL) {
			Ref.m_uDevNum = uDevNum; // Scan for this device
			uError = static_cast<HRESULT>(
				Windows::DirectDrawEnumerateExA(FindDeviceCallback, &Ref,
					DDENUM_ATTACHEDSECONDARYDEVICES
						| DDENUM_DETACHEDSECONDARYDEVICES
						| DDENUM_NONDISPLAYDEVICES));
		}
		if (uError == DD_OK) {
			if (Ref.m_uDevNum) { // Got it?
				uError = E_FAIL; // Force an error
			}
		}
	}
	return static_cast<Word>(uError);
}


/*! ************************************

	\brief Adds a directory to the start folder.

	When installing an application, it may be desirable to create an entry in
	the start menu to show an application folder and links for
files/applications of interest. This function locates the users start menu
folder and ensures that the folder is created and Windows Explorer is notified
	of the change

	\param pGroupName UTF8 "C" string of the folder name for the start menu

	\windowsonly
	\return Returns TRUE if an error occurs

***************************************/

Word BURGER_API Burger::Globals::AddGroupToProgramMenu(const char* pGroupName)
{
	// Get the pidl for the start menu
	// this will be used to initialize the folder browser

	Word uResult = 10;
	ITEMIDLIST* pIDListStartMenu; // Item list for the start menu
	if (SHGetSpecialFolderLocation(
			GetWindow(), CSIDL_PROGRAMS, &pIDListStartMenu)
		== NOERROR) {
		Word16 WorkPath[MAX_PATH * 2];
		if (SHGetPathFromIDListW(
				pIDListStartMenu, reinterpret_cast<LPWSTR>(WorkPath))) {
			// Append a directory divider
			WordPtr uLength = StringLength(WorkPath);
			WorkPath[uLength] = '\\';
			WorkPath[uLength + 1] = 0;

			// Append the new folder name
			String16 GroupName(pGroupName);
			StringConcatenate(WorkPath, sizeof(WorkPath), GroupName.GetPtr());
			// Convert to UTF8 for Burgerlib
			String UTF8(WorkPath);
			Filename TempPath;
			TempPath.SetFromNative(UTF8.GetPtr());
			if (!FileManager::CreateDirectoryPath(
					&TempPath)) { // Create the directory
				// Notify the shell that this folder was updated
				// Use SHCNF_PATHW since WorkPath is UTF16
				SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, WorkPath, NULL);
			}
			uResult = 0; // Success!
		}
	}
	return uResult;
}

/*! ************************************

	\brief Set a user registry key with a string

	Strings are all UTF-8. This function will perform
	conversion to UTF-16 for Windows

	\windowsonly
	\param pKey Key found in HKEY_CURRENT_USER
	\param pSubKey Name of the sub-key of interest, can be \ref nullptr
	\param pData String to store in the registry
	\return Zero if successful, non-zero is the Windows error code.
	\sa AssociateFileExtensionToExe(const char *,const char *,const char *)

***************************************/

int BURGER_API Burger::Globals::CreateUserRegistryKey(
	const char* pKey, const char* pSubKey, const char* pData)
{
	// Convert from UTF8 to UTF16 for Windows
	String16 KeyUTF16(pKey);
	HKEY hKey = NULL;
	// Create the registry key
	LONG lStatus = RegCreateKeyExW(HKEY_CURRENT_USER,
		reinterpret_cast<LPWSTR>(KeyUTF16.GetPtr()), 0, 0,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lStatus == ERROR_SUCCESS) {
		const wchar_t* pSub16 = NULL;
		// SubKeyUTF16 has to remain in scope
		String16 SubKeyUTF16(pSubKey);
		if (SubKeyUTF16.GetLength()) {
			pSub16 = reinterpret_cast<const wchar_t*>(SubKeyUTF16.GetPtr());
		}
		String16 DataUTF16(pData);
		lStatus = RegSetValueExW(hKey, reinterpret_cast<LPCWSTR>(pSub16), 0,
			REG_SZ, reinterpret_cast<const BYTE*>(DataUTF16.GetPtr()),
			(static_cast<DWORD>(DataUTF16.GetLength()) + 1) * 2);
		RegCloseKey(hKey);
	}
	return lStatus;
}

/*! ************************************

    \brief Associate a data file to the application.

    Set the user registry to associate a data file type with the
    currently running executable.

    \windowsonly

    \code
    // Tell Windows Explorer to launch .datafile files with the currently
    // running app by double-clicking

    AssociateFileExtensionToExe(".datafile",
        "Data for the Fubar application","com.oldskuul.fubar");

    \endcode

    \param pFileExtension ".foo" Pointer to a "C" string with the file extension
    \param pDescription "Foo file" Description of this file type to be visible
        to Explorer
    \param pProgramID "com.company.application" program ID

    \sa CreateUserRegistryKey(const char *,const char *,const char *)

***************************************/

void BURGER_API Burger::Globals::AssociateFileExtensionToExe(
	const char* pFileExtension, const char* pDescription,
	const char* pProgramID)
{
	// Create the keys for the file extension and the description to show in
	// Explorer
	{
		// Create the key for the file extension itself. .foo -> Unique program
		// ID
		String ClassExtension(g_SoftwareClasses, pFileExtension);
		CreateUserRegistryKey(ClassExtension.GetPtr(), NULL, pProgramID);
	}
	{
		// Create the key for the unique program ID, with the file's description
		String ClassExtension(g_SoftwareClasses, pProgramID);
		CreateUserRegistryKey(ClassExtension.GetPtr(), NULL, pDescription);
	}

	// With the program ID already requested, generate the app's location for
	// the ID and the sample command line to use if you "drag and drop" a file
	// on the exe.
	Word16 TempBuffer[MAX_PATH];
	// Get the pathname to the currently running application
	if (GetModuleFileNameW(NULL, reinterpret_cast<LPWSTR>(TempBuffer), MAX_PATH)
		< MAX_PATH) {
		String ExePath(TempBuffer);
		{
			// Create the key and command to launch on double click
			String ClassShellOpen(
				g_SoftwareClasses, pProgramID, "\\shell\\open\\command");
			String Command("\"", ExePath.GetPtr(), "\" \"%1\"");
			CreateUserRegistryKey(
				ClassShellOpen.GetPtr(), NULL, Command.GetPtr());
		}
		{
			// Create the key and reference to the icon for the data file
			String ClassIcon(g_SoftwareClasses, pProgramID, "\\DefaultIcon");
			String Command2("\"", ExePath.GetPtr(), "\",1");
			CreateUserRegistryKey(ClassIcon.GetPtr(), NULL, Command2.GetPtr());
		}
	}
}

/*! ************************************

	\brief Print windows events to Debug::Message()

	This helper function will take a windows Window Event
	and print it to the debugging message system to
	allow a programmer to trace events going through
	a window procedure. This should not be called in
	released code.

	\windowsonly
	\param uMessage Windows message enumeration
	\param wParam wParam value passed to the windows callback
	\param lParam lParam value passed to the windows callback

***************************************/

#if !defined(DOXYGEN)
#define CASE(x)                                                                \
	{                                                                          \
#x, x                                                                  \
	}

struct MessageLookup_t {
	const char* m_pName; // String of the enum
	Word m_uEnum;		 // Enum value that matched the string
};

//
// All known event messages for a window are here.
//

static const MessageLookup_t g_MessageLookup[] = {CASE(WM_NULL),
	CASE(WM_CREATE), CASE(WM_DESTROY), CASE(WM_MOVE), CASE(WM_SIZE),
	CASE(WM_ACTIVATE), CASE(WM_SETFOCUS), CASE(WM_KILLFOCUS), CASE(WM_ENABLE),
	CASE(WM_SETREDRAW), CASE(WM_SETTEXT), CASE(WM_GETTEXT),
	CASE(WM_GETTEXTLENGTH), CASE(WM_PAINT), CASE(WM_CLOSE),
	CASE(WM_QUERYENDSESSION), CASE(WM_QUIT), CASE(WM_QUERYOPEN),
	CASE(WM_ERASEBKGND), CASE(WM_SYSCOLORCHANGE), CASE(WM_SHOWWINDOW),
	CASE(WM_SETTINGCHANGE), CASE(WM_DEVMODECHANGE), CASE(WM_ACTIVATEAPP),
	CASE(WM_FONTCHANGE), CASE(WM_TIMECHANGE), CASE(WM_CANCELMODE),
	CASE(WM_SETCURSOR), CASE(WM_MOUSEACTIVATE), CASE(WM_CHILDACTIVATE),
	CASE(WM_QUEUESYNC), CASE(WM_GETMINMAXINFO), CASE(WM_PAINTICON),
	CASE(WM_ICONERASEBKGND), CASE(WM_NEXTDLGCTL), CASE(WM_SPOOLERSTATUS),
	CASE(WM_DRAWITEM), CASE(WM_MEASUREITEM), CASE(WM_DELETEITEM),
	CASE(WM_VKEYTOITEM), CASE(WM_CHARTOITEM), CASE(WM_SETFONT),
	CASE(WM_GETFONT), CASE(WM_SETHOTKEY), CASE(WM_GETHOTKEY),
	CASE(WM_QUERYDRAGICON), CASE(WM_COMPAREITEM), CASE(WM_GETOBJECT),
	CASE(WM_COMPACTING), CASE(WM_COMMNOTIFY), CASE(WM_WINDOWPOSCHANGING),
	CASE(WM_WINDOWPOSCHANGED), CASE(WM_POWER), CASE(WM_COPYDATA),
	CASE(WM_CANCELJOURNAL), CASE(WM_KEYF1), CASE(WM_NOTIFY),
	CASE(WM_INPUTLANGCHANGEREQUEST), CASE(WM_INPUTLANGCHANGE), CASE(WM_TCARD),
	CASE(WM_HELP), CASE(WM_USERCHANGED), CASE(WM_NOTIFYFORMAT),
	CASE(WM_CONTEXTMENU), CASE(WM_STYLECHANGING), CASE(WM_STYLECHANGED),
	CASE(WM_DISPLAYCHANGE), CASE(WM_GETICON), CASE(WM_SETICON),
	CASE(WM_NCCREATE), CASE(WM_NCDESTROY), CASE(WM_NCCALCSIZE),
	CASE(WM_NCHITTEST), CASE(WM_NCPAINT), CASE(WM_NCACTIVATE),
	CASE(WM_GETDLGCODE), CASE(WM_SYNCPAINT), CASE(WM_UAHDESTROYWINDOW),
	CASE(WM_UAHDRAWMENU), CASE(WM_UAHDRAWMENUITEM), CASE(WM_UAHINITMENU),
	CASE(WM_UAHMEASUREMENUITEM), CASE(WM_UAHNCPAINTMENUPOPUP),
	CASE(WM_NCMOUSEMOVE), CASE(WM_NCLBUTTONDOWN), CASE(WM_NCLBUTTONUP),
	CASE(WM_NCLBUTTONDBLCLK), CASE(WM_NCRBUTTONDOWN), CASE(WM_NCRBUTTONUP),
	CASE(WM_NCRBUTTONDBLCLK), CASE(WM_NCMBUTTONDOWN), CASE(WM_NCMBUTTONUP),
	CASE(WM_NCMBUTTONDBLCLK), CASE(WM_NCXBUTTONDOWN), CASE(WM_NCXBUTTONUP),
	CASE(WM_NCXBUTTONDBLCLK), CASE(WM_NCUAHDRAWCAPTION),
	CASE(WM_NCUAHDRAWFRAME), CASE(WM_INPUT_DEVICE_CHANGE), CASE(WM_INPUT),
	CASE(WM_KEYDOWN), CASE(WM_KEYUP), CASE(WM_CHAR), CASE(WM_DEADCHAR),
	CASE(WM_SYSKEYDOWN), CASE(WM_SYSKEYUP), CASE(WM_SYSCHAR),
	CASE(WM_SYSDEADCHAR), CASE(WM_UNICHAR), CASE(WM_IME_STARTCOMPOSITION),
	CASE(WM_IME_ENDCOMPOSITION), CASE(WM_IME_COMPOSITION), CASE(WM_INITDIALOG),
	CASE(WM_COMMAND), CASE(WM_SYSCOMMAND), CASE(WM_TIMER), CASE(WM_HSCROLL),
	CASE(WM_VSCROLL), CASE(WM_INITMENU), CASE(WM_INITMENUPOPUP),
	CASE(WM_GESTURE), CASE(WM_GESTURENOTIFY), CASE(WM_MENUSELECT),
	CASE(WM_MENUCHAR), CASE(WM_ENTERIDLE), CASE(WM_MENURBUTTONUP),
	CASE(WM_MENUDRAG), CASE(WM_MENUGETOBJECT), CASE(WM_UNINITMENUPOPUP),
	CASE(WM_MENUCOMMAND), CASE(WM_CHANGEUISTATE), CASE(WM_UPDATEUISTATE),
	CASE(WM_QUERYUISTATE), CASE(WM_CTLCOLORMSGBOX), CASE(WM_CTLCOLOREDIT),
	CASE(WM_CTLCOLORLISTBOX), CASE(WM_CTLCOLORBTN), CASE(WM_CTLCOLORDLG),
	CASE(WM_CTLCOLORSCROLLBAR), CASE(WM_CTLCOLORSTATIC), CASE(MN_GETHMENU),
	CASE(WM_MOUSEMOVE), CASE(WM_LBUTTONDOWN), CASE(WM_LBUTTONUP),
	CASE(WM_LBUTTONDBLCLK), CASE(WM_RBUTTONDOWN), CASE(WM_RBUTTONUP),
	CASE(WM_RBUTTONDBLCLK), CASE(WM_MBUTTONDOWN), CASE(WM_MBUTTONUP),
	CASE(WM_MBUTTONDBLCLK), CASE(WM_MOUSEWHEEL), CASE(WM_XBUTTONDOWN),
	CASE(WM_XBUTTONUP), CASE(WM_XBUTTONDBLCLK), CASE(WM_MOUSEHWHEEL),
	CASE(WM_PARENTNOTIFY), CASE(WM_ENTERMENULOOP), CASE(WM_EXITMENULOOP),
	CASE(WM_NEXTMENU), CASE(WM_SIZING), CASE(WM_CAPTURECHANGED),
	CASE(WM_MOVING), CASE(WM_POWERBROADCAST), CASE(WM_DEVICECHANGE),
	CASE(WM_MDICREATE), CASE(WM_MDIDESTROY), CASE(WM_MDIACTIVATE),
	CASE(WM_MDIRESTORE), CASE(WM_MDINEXT), CASE(WM_MDIMAXIMIZE),
	CASE(WM_MDITILE), CASE(WM_MDICASCADE), CASE(WM_MDIICONARRANGE),
	CASE(WM_MDIGETACTIVE), CASE(WM_MDISETMENU), CASE(WM_ENTERSIZEMOVE),
	CASE(WM_EXITSIZEMOVE), CASE(WM_DROPFILES), CASE(WM_MDIREFRESHMENU),
	CASE(WM_TOUCH), CASE(WM_IME_SETCONTEXT), CASE(WM_IME_NOTIFY),
	CASE(WM_IME_CONTROL), CASE(WM_IME_COMPOSITIONFULL), CASE(WM_IME_SELECT),
	CASE(WM_IME_CHAR), CASE(WM_IME_REQUEST), CASE(WM_IME_KEYDOWN),
	CASE(WM_IME_KEYUP), CASE(WM_NCMOUSEHOVER), CASE(WM_MOUSEHOVER),
	CASE(WM_NCMOUSELEAVE), CASE(WM_MOUSELEAVE), CASE(WM_WTSSESSION_CHANGE),
	CASE(WM_CUT), CASE(WM_COPY), CASE(WM_PASTE), CASE(WM_CLEAR), CASE(WM_UNDO),
	CASE(WM_RENDERFORMAT), CASE(WM_RENDERALLFORMATS), CASE(WM_DESTROYCLIPBOARD),
	CASE(WM_DRAWCLIPBOARD), CASE(WM_PAINTCLIPBOARD), CASE(WM_VSCROLLCLIPBOARD),
	CASE(WM_SIZECLIPBOARD), CASE(WM_ASKCBFORMATNAME), CASE(WM_CHANGECBCHAIN),
	CASE(WM_HSCROLLCLIPBOARD), CASE(WM_QUERYNEWPALETTE),
	CASE(WM_PALETTEISCHANGING), CASE(WM_PALETTECHANGED), CASE(WM_HOTKEY),
	CASE(WM_PRINT), CASE(WM_PRINTCLIENT), CASE(WM_APPCOMMAND),
	CASE(WM_THEMECHANGED), CASE(WM_CLIPBOARDUPDATE),
	CASE(WM_DWMCOMPOSITIONCHANGED), CASE(WM_DWMNCRENDERINGCHANGED),
	CASE(WM_DWMCOLORIZATIONCOLORCHANGED), CASE(WM_DWMWINDOWMAXIMIZEDCHANGE)};
#endif

void BURGER_API Burger::Globals::OutputWindowsMessage(
	Word uMessage, WordPtr wParam, WordPtr lParam)
{
	// Static global value containing the number of times this function was
	// called
	static Word uMessageCount = 0;

	// Scan the table for a match
	WordPtr uCount = BURGER_ARRAYSIZE(g_MessageLookup);
	const MessageLookup_t* pLookup = g_MessageLookup;
	const char* pMessage = NULL;
	do {
		if (uMessage == pLookup->m_uEnum) {
			pMessage = pLookup->m_pName;
			break;
		}
		++pLookup;
	} while (--uCount);

	// If a message wasn't found, convert the unknown code to an
	// ASCII message

	char HexAsASCII[32];
	if (!pMessage) {
		NumberToAsciiHex(
			HexAsASCII, static_cast<Word32>(uMessage), LEADINGZEROS | 8);
		pMessage = HexAsASCII;
	}
	// Output the message and parameter values. It's not 64 bit clean for the
	// parameters, however, this is only for information to give clues on how
	// events are sent to a window from the operating system

	Debug::Message("Message %08X is %s with parms %08X, %08X\n", uMessageCount,
		pMessage, static_cast<Word>(wParam), static_cast<Word>(lParam));
	++uMessageCount;
}

/***************************************

	Internal windows dispatcher

***************************************/

#if !defined(DOXYGEN)

static LRESULT CALLBACK InternalCallBack(
	HWND pWindow, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	// Firstly, get the "this" pointer
	// Note that this is not automatically set, the pointer is
	// passed in the WM_NCCREATE command and it's manually set into the
	// WindowLongPtr index of GWLP_USERDATA.

	// Use GetWindowLongPtrW to be 64 bit clean
	Burger::GameApp* pThis = reinterpret_cast<Burger::GameApp*>(
		GetWindowLongPtrW(pWindow, GWLP_USERDATA));

	// Never initialized?
	if (!pThis) {
		// If this is a WM_NCCREATE event, then get the class instance pointer
		// passed.
		if (uMessage == WM_NCCREATE) {
			// Use SetWindowLongPtrW to be 64 bit clean
			pThis = static_cast<Burger::GameApp*>(
				reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtrW(pWindow, GWLP_USERDATA,
				static_cast<LONG_PTR>(reinterpret_cast<WordPtr>(pThis)));
		}
		// If I didn't get a pThis set, just call the default procedure and exit
		if (!pThis) {
			return DefWindowProcW(pWindow, uMessage, wParam, lParam);
		}
	}

	// The pThis class instance is valid!

	// For debugging, if needed
#if defined(_DEBUG)
	if (Burger::Globals::GetTraceFlag() & Burger::Globals::TRACE_MESSAGES) {
		Burger::Globals::OutputWindowsMessage(
			uMessage, wParam, static_cast<WordPtr>(lParam));
	}
#endif

	// If there is a user supplied callback, issue it

	Burger::GameApp::MainWindowProc pCallBack = pThis->GetCallBack();
	if (pCallBack) {
		WordPtr uOutput = FALSE; // Assume not handled (In case the callback
								 // doesn't set the variable)
		// If the function returns non-zero, assume it should terminate
		// immediately
		if (pCallBack(pThis, pWindow, static_cast<Word>(uMessage),
				static_cast<WordPtr>(wParam), static_cast<WordPtr>(lParam),
				&uOutput)) {
			// Return the passed result code
			return static_cast<LRESULT>(uOutput);
		}
	}

	//
	// Handle burgerlib update events
	//

	switch (uMessage) {

		//
		// This function will disable the ability to resize the window
		//

	case WM_GETMINMAXINFO: {
		// Ensure a MINIMUM size of 320x200
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 320;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		// Only if a video display is present
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			if (pDisplay->HandleMinMax(pWindow, static_cast<WordPtr>(lParam))) {
				return FALSE;
			}
		}
	} break;

	case WM_ENTERSIZEMOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			// Halt frame movement while the app is sizing or moving
			pDisplay->Pause(TRUE);
			pThis->SetInSizeMove(TRUE);
		}
	} break;

	case WM_EXITSIZEMOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			pDisplay->Pause(FALSE);
			pDisplay->CheckForWindowSizeChange();
			pDisplay->CheckForWindowChangingMonitors();
			pThis->SetInSizeMove(FALSE);
		}
	} break;

		//
		// This handles cursor updates when the Windows cursor is
		// moving over the window when the window doesn't have focus
		//

	case WM_SETCURSOR:
		if (pThis->HandleCursor(static_cast<Word>(lParam))) {
			return TRUE; // Handled
		}
		break;

		//
		// The app is "activated"
		//

	case WM_ACTIVATEAPP:
		// If quitting, do NOT activate!
		if (pThis->GetQuitCode()) {
			return 0; // Message is processed
		}

		// Is it active and was in the background?
		if ((wParam == TRUE) && pThis->IsInBackground()) {

			// If the joypad is present, enable XInput
			if (pThis->GetJoypad()) {
				Burger::Windows::XInputEnable(TRUE);
			}
			// Move to the foreground
			if (pThis->IsAppFullScreen()) {

				// For full screen, disable the short cut keys
				pThis->GetKeyboard()->DisableAccessibilityShortcutKeys();

				// Reacquire DirectInput
				pThis->GetInputFocus();
			} else {
				// Use the OS for input
				pThis->KillInputFocus();
			}
			pThis->SetInBackground(FALSE);

			// Is it being deactivated and was in the foreground?
		} else if ((!wParam) && !pThis->IsInBackground()) {

			// If the joypad is present, disable XInput
			if (pThis->GetJoypad()) {
				Burger::Windows::XInputEnable(FALSE);
			}

			// Ensure the OS has input
			pThis->KillInputFocus();

			// Make sure the short cut keys are available
			pThis->GetKeyboard()->RestoreAccessibilityShortcutKeys();
			pThis->SetInBackground(TRUE);
		}

		// State hasn't changed, leave as is
		return 0;

	//
	// Pass mouse motion to the mouse driver
	//

	// Mouse move events only happen when the mouse cursor is on the screen
	case WM_MOUSEMOVE:
		// If not previously tracked, ask Windows to send
		// me an event if the mouse is OFF this window
		// so the application is aware that the mouse
		// is no longer available

		if (!pThis->IsMouseOnScreen()) {
			TRACKMOUSEEVENT TrackIt;
			TrackIt.cbSize = sizeof(TrackIt);
			TrackIt.dwFlags = TME_LEAVE;
			TrackIt.hwndTrack = pWindow;
			TrackIt.dwHoverTime = 0;
			Burger::Windows::TrackMouseEvent(&TrackIt);
			pThis->SetMouseOnScreen(TRUE);
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEHWHEEL: {
		// If there's a mouse device, set the position
		Burger::Mouse* pMouse = pThis->GetMouse();
		if (pMouse) {
			Word uMouseX;
			Word uMouseY;

			// Mouse wheel events give global coordinates. Go figure
			if ((uMessage == WM_MOUSEWHEEL) || (uMessage == WM_MOUSEHWHEEL)) {
				// Must use GET_X_LPARAM because the values
				// are signed shorts on multiple monitors
				POINT TempPoint;
				TempPoint.x = static_cast<LONG>(GET_X_LPARAM(lParam));
				TempPoint.y = static_cast<LONG>(GET_Y_LPARAM(lParam));
				ScreenToClient(pThis->GetWindow(), &TempPoint);
				uMouseX = static_cast<Word>(TempPoint.x);
				uMouseY = static_cast<Word>(TempPoint.y);
			} else {
				// They are unsigned values!
				uMouseX = LOWORD(lParam);
				uMouseY = HIWORD(lParam);
			}
			// Pass the value to the mouse driver
			pMouse->PostMousePosition(uMouseX, uMouseY);

			// Pass the mouse button events down
			switch (uMessage) {
			case WM_MOUSEWHEEL: {
				int iDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				pMouse->PostMouseWheel(0, iDelta);
			} break;
			case WM_MOUSEHWHEEL: {
				int iDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				pMouse->PostMouseWheel(iDelta, 0);
			} break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_LEFT);
				break;
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_RIGHT);
				break;
			case WM_MBUTTONDOWN:
			case WM_MBUTTONDBLCLK:
				pMouse->PostMouseDown(Burger::Mouse::BUTTON_MIDDLE);
				break;
			case WM_XBUTTONDOWN:
			case WM_XBUTTONDBLCLK: {
				// uBits is 1 or 2, convert to 0x8 or 0x10
				Word uBits = GET_XBUTTON_WPARAM(wParam);
				pMouse->PostMouseDown(uBits << 3U);
			}
				// XBUTTON events need to return TRUE
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646245(v=vs.85).aspx
				return TRUE;
			case WM_LBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_LEFT);
				break;
			case WM_RBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_RIGHT);
				break;
			case WM_MBUTTONUP:
				pMouse->PostMouseUp(Burger::Mouse::BUTTON_MIDDLE);
				break;
			case WM_XBUTTONUP: {
				// uBits is 1 or 2, convert to 0x8 or 0x10
				Word uBits = GET_XBUTTON_WPARAM(wParam);
				pMouse->PostMouseUp(uBits << 3U);
			}
				// XBUTTON events need to return TRUE
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646245(v=vs.85).aspx
				return TRUE;

			default:
				break;
			}
			return 0;
		}
	}
	// No mouse driver, pass the events down
	break;

	// Mouse is off the client area. Turn off any software cursor
	case WM_MOUSELEAVE:
	case WM_NCMOUSELEAVE:
	case WM_NCMOUSEMOVE:
		pThis->SetMouseOnScreen(FALSE);
		break;

		// An external program (The Keyboard control panel most likely)
		// changed the keyboard repeat speed.
		// Alert the keyboard manager, if one was started, about the
		// event.

	case WM_SETTINGCHANGE:
		if ((wParam == SPI_SETKEYBOARDSPEED)
			|| (wParam == SPI_SETKEYBOARDDELAY)) {
			Burger::Keyboard* pKeyboard = pThis->GetKeyboard();
			if (pKeyboard) {
				pKeyboard->ReadSystemKeyboardDelays();
			}
		} else if (wParam == SPI_SETMOUSEBUTTONSWAP) {
			Burger::Mouse* pMouse = pThis->GetMouse();
			if (pMouse) {
				pMouse->ReadSystemMouseValues();
			}
		}
		break;

	case WM_SIZE:
	case WM_MOVE: {
		Burger::Display* pDisplay = pThis->GetDisplay();
		if (pDisplay) {
			if (!(pDisplay->GetFlags() & Burger::Display::FULLSCREEN)) {
				pThis->RecordWindowLocation();
			}
			RECT TempRect;
			GetClientRect(pThis->GetWindow(), &TempRect);
			pDisplay->Resize(static_cast<Word>(TempRect.right),
				static_cast<Word>(TempRect.bottom));
			if (pDisplay->GetResizeCallback()) {
				(pDisplay->GetResizeCallback())(
					pDisplay->GetResizeCallbackData(),
					static_cast<Word>(TempRect.right),
					static_cast<Word>(TempRect.bottom));
			}
			// Alert the mouse subsystem to the new mouse bounds
			Burger::Mouse* pMouse = pThis->GetMouse();
			if (pMouse) {
				// Reset the mouse coords for mouse handler
				pMouse->SetRange(pDisplay->GetWidth(), pDisplay->GetHeight());
			}
		} else {
			pThis->RecordWindowLocation();
		}
	} break;

		// Windows is asking for the window to be redrawn, possibly
		// from recovering from minimization?

	case WM_NCPAINT:
	case WM_PAINT:
		// Any region to draw?
		if (GetUpdateRect(pWindow, NULL, FALSE)) {
			PAINTSTRUCT ps;
			// Save the old context
			HDC PaintDC = BeginPaint(pWindow, &ps);
			if (PaintDC) {
				// Get the video context
				Burger::Display* pDisplay = pThis->GetDisplay();
				if (pDisplay) {
					// Force a front screen update
					Burger::Display::RenderProc pCallback =
						pDisplay->GetRenderCallback();
					if (pCallback) {
						(pCallback)(pDisplay->GetRenderCallbackData());
					}
				}
				EndPaint(pWindow, &ps);
			}
			RECT TempRect;
			GetClientRect(pThis->GetWindow(), &TempRect);
			ValidateRect(pWindow, &TempRect);
		}
		if (uMessage == WM_PAINT) {
			return 1;
		}
		break;

	// Power functions
	case WM_POWERBROADCAST:
		switch (wParam) {
		case PBT_APMQUERYSUSPEND:
			// Do not allow the app to suspend!
			// Note: Screen savers should quit and apps should send a
			// notification!
			return BROADCAST_QUERY_DENY;

		// Resume from power saving?
		case PBT_APMRESUMESUSPEND:
			return 1;
		}
		break;

	// A menu was active and the user pressed an invalid key. Disable the beep
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	// Disable menu selection by forcing high level code to think
	// everything is part of the client area.
	case WM_NCHITTEST:
		if (pThis->IsAppFullScreen()) {
			return HTCLIENT;
		}
		if (!pThis->IsResizingAllowed()) {
			// Process the test
			LRESULT lResult = DefWindowProcW(pWindow, uMessage, wParam, lParam);
			// Override the borders to the caption to change resizing
			// to window movement events
			switch (lResult) {
			case HTRIGHT:
			case HTLEFT:
			case HTTOP:
			case HTTOPLEFT:
			case HTTOPRIGHT:
			case HTBOTTOM:
			case HTBOTTOMRIGHT:
			case HTBOTTOMLEFT:
				lResult = HTCAPTION;
			default:
				break;
			}
			return lResult;
		}
		// Allow the OS handle handle the hot spots
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch (GET_SC_WPARAM(wParam)) {
		case SC_SIZE:
			if (!pThis->IsResizingAllowed()) {
				return 0; // Discard resize commands
			}
		case SC_MOVE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if (pThis->IsAppFullScreen()) {
				return 0;
			}
			break;
		}
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		// If there's a keyboard manager, pass the keys to it.
		Burger::Keyboard* pKeyboard = pThis->GetKeyboard();
		if (pKeyboard) {
			Burger::eEvent uEvent;

			// Key up event?
			if (lParam & (KF_UP << 16)) {
				uEvent = Burger::EVENT_KEYUP;
			} else {
				// Repeated key event?
				if (lParam & (KF_REPEAT << 16)) {
					uEvent = Burger::EVENT_KEYAUTO;
				} else {
					// Normal key down event
					uEvent = Burger::EVENT_KEYDOWN;
				}
			}
			pKeyboard->PostWindowsKeyEvent(uEvent,
				((static_cast<Word32>(lParam) >> 16U) & 0x7FU)
					| ((static_cast<Word32>(lParam) >> 17U) & 0x80U));
			return 0;
		}
	} break;

	case WM_KILLFOCUS:
		pThis->KillInputFocus();
		break;

	case WM_SETFOCUS:
		pThis->GetInputFocus();
		break;

	case WM_DISPLAYCHANGE:
		return 0;

	// Ignore update events since redraws are happening constantly
	case WM_ERASEBKGND:
		return 1;

	case WM_DESTROY:
	case WM_QUIT: // External quit event (System shutdown)
		pThis->SetQuitCode();
		return 0;
	default:
		break;
	}
	return DefWindowProcW(pWindow, uMessage, wParam, lParam);
}

#endif

/*! ************************************

	\brief Create the global Burgerlib Window Class

	Windows requires a WNDCLASS definition declared to allow
	the creation of application specific windows. Burgerlib uses it's
	own custom callback function and it's declared after this
	function is called which calls RegisterClassExW() in
	windows that creates an ATOM of the type BurgerGameClass.
	


	The string can be obtained with a call to GetWindowClassName()
	if it's desired for an application to create a Burgerlib window
	without Burgerlib doing it for you.

	The ATOM is released on application shutdown, or manually with a call
	to UnregisterWindowClass()

	If this function had previously created the game class, it will return
	the ATOM that was created before and exit immediately.

	\note This function requires that SetInstance() has been called. Burgerlib
	does this automatically when creating a GameApp class instance, however
	if no GameApp was created, the INSTANCE must be passed manually for this
	function to work properly

	\windowsonly

	\param uIconResID Numeric ID of the Windows Icon to attach to the window
that's in the EXE files resource data (0 to use the default application icon
from Windows) \sa UnregisterWindowClass(void)

***************************************/

Word16 BURGER_API Burger::Globals::RegisterWindowClass(Word uIconResID)
{
	ATOM uAtom = g_uAtom;
	if (uAtom == INVALID_ATOM) {

		// Is the an app instance?
		if (!Windows::GetInstance()) {
			// Obtain it
			Windows::SetInstance(GetModuleHandleW(NULL));
		}

		// Try to load the icon for the app
		HICON hIcon = NULL;
		if (uIconResID) {
			hIcon = LoadIcon(Windows::GetInstance(), MAKEINTRESOURCE(uIconResID));
		}
		// No icon?
		if (hIcon) {
			// Try pulling the icon from the app itself by getting the first
			// icon found
			WCHAR TheExePath[1024];
			GetModuleFileNameW(NULL, TheExePath, BURGER_ARRAYSIZE(TheExePath));
			hIcon = ExtractIconW(Windows::GetInstance(), TheExePath, 0);
		}

		WNDCLASSEXW WindowClass;
		MemoryClear(&WindowClass, sizeof(WindowClass));
		WindowClass.cbSize = sizeof(WindowClass);
		WindowClass.style = CS_DBLCLKS;				// Accept double clicks
		WindowClass.lpfnWndProc = InternalCallBack; // My window callback
		//	WindowClass.cbClsExtra = 0;			// No extra class bytes
		//	WindowClass.cbWndExtra = 0;			// No extra space was needed
		WindowClass.hInstance = Windows::GetInstance();
		WindowClass.hIcon = hIcon;
		// Keep the cursor NULL to allow updating of the cursor by the app
		//	WindowClass.hCursor = NULL;
		////LoadCursorW(NULL,MAKEINTRESOURCEW(IDC_ARROW));
		WindowClass.hbrBackground =
			reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		//	WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = (LPCWSTR)Globals::GetWindowClassName();
		//	WindowClass.hIconSm = NULL;		// Assume hIcon has the small icon
		//in the set

		// Register my window's class
		uAtom = RegisterClassExW(&WindowClass);

		// If this function fails, it will be INVALID_ATOM
		g_uAtom = uAtom;
	}
	return uAtom;
}

/*! ************************************

	\brief Release the global Burgerlib Window Class

	When RegisterWindowClass(Word) is called, it will create a global
	ATOM of the window class. This function will unregister the
	class. This function is called on shutdown automatically

	\windowsonly

	\sa RegisterWindowClass(Word)

***************************************/

void BURGER_API Burger::Globals::UnregisterWindowClass(void)
{
	ATOM uAtom = g_uAtom;
	if (uAtom != INVALID_ATOM) {
		// Release the class
		UnregisterClassW(MAKEINTRESOURCEW(uAtom),Windows::GetInstance());
		g_uAtom = INVALID_ATOM;
	}
}

/*! ************************************

	\brief Pump windows messages

	Windows requires a function to pump messages

	\windowsonly

	\sa RegisterWindowClass(Word)

***************************************/

void BURGER_API Burger::Globals::PumpMessages(void)
{
	MSG TempMessage;
	while (PeekMessageW(&TempMessage, NULL, 0, 0, PM_REMOVE)) {
		// Translate the keyboard (Localize)
		TranslateMessage(&TempMessage);
		// Pass to the window event proc
		DispatchMessageW(&TempMessage);
	}
}

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
	Word uResult =
		(Windows::ShellExecuteOpen(pURL) > 32); // I launched if greater than 32
	return uResult;
}

/***************************************

	Execute a tool and capture the text output

***************************************/

int BURGER_API Burger::Globals::ExecuteTool(
	const char* pFilename, const char* pParameters, OutputMemoryStream* pOutput)
{
	// Get the parameter list
	Filename AppName(pFilename);
	String Full("\"", AppName.GetNative(), "\" ", pParameters);
	// Create the full Unicode command string
	String16 Unicode(Full);

	// Prepare the process information
	PROCESS_INFORMATION ProcessInfo;
	MemoryClear(&ProcessInfo, sizeof(ProcessInfo));
	STARTUPINFOW StartupInfo;
	MemoryClear(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);

	// Assume no text capturing

	HANDLE hCaptureIn = NULL;
	HANDLE hCaptureOut = NULL;

	// Is capturing requested?
	if (pOutput) {
		// Create a pipe for STDOUT
		SECURITY_ATTRIBUTES SecurityAttributes;
		MemoryClear(&SecurityAttributes, sizeof(SecurityAttributes));
		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.bInheritHandle = TRUE;
		SecurityAttributes.lpSecurityDescriptor = NULL;
		// Create them and allow the capture pipe to inherit permissions
		if (CreatePipe(&hCaptureIn, &hCaptureOut, &SecurityAttributes, 0)) {
			if (SetHandleInformation(hCaptureIn, HANDLE_FLAG_INHERIT, 0)) {
				// It's good, capture the output
				StartupInfo.hStdError = hCaptureOut;
				StartupInfo.hStdOutput = hCaptureOut;
				StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			}
		}
	}

	// Assume failure
	DWORD nExitCode = 10;

	// Call the tool
	BOOL bResult =
		CreateProcessW(NULL, reinterpret_cast<LPWSTR>(Unicode.GetPtr()), NULL,
			NULL, pOutput != NULL, 0, NULL, NULL, &StartupInfo, &ProcessInfo);

	// Did it even launch?
	if (bResult) {
		// Wait for the tool to finish executing
		if (WaitForSingleObject(ProcessInfo.hProcess, INFINITE)
			== WAIT_OBJECT_0) {
			// Get the exit code from the tool
			if (!GetExitCodeProcess(ProcessInfo.hProcess, &nExitCode)) {
				// Failure! Assume an error code of 10
				nExitCode = 10;
			}
		}
		// Release the handles (Since launch was successful)
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}

	// Release the capture pipe (So the input pipe is finite)
	if (hCaptureOut) {
		CloseHandle(hCaptureOut);
	}

	// Only capture if needed
	if (bResult && pOutput) {
		DWORD uBytesRead;
		Word8 Buffer[1024];
		for (;;) {
			// Read from the finite pipe
			BOOL bSuccess =
				ReadFile(hCaptureIn, Buffer, sizeof(Buffer), &uBytesRead, NULL);
			// Error or all done?
			if ((!bSuccess) || (!uBytesRead)) {
				break;
			}
			pOutput->Append(Buffer, uBytesRead);
		}
	}
	// Clean up the last handle
	if (hCaptureIn) {
		CloseHandle(hCaptureIn);
	}
	// Exit with the tool's error code
	return static_cast<int>(nExitCode);
}

/***************************************

	Read an environment variable as UTF8

***************************************/

const char* BURGER_API Burger::Globals::GetEnvironmentString(const char* pKey)
{
	// Convert the key to UTF16
	String16 Key16(pKey);
	// How long is the key?
	DWORD uLength = GetEnvironmentVariableW(
		reinterpret_cast<LPCWSTR>(Key16.GetPtr()), NULL, 0);
	char* pValue = nullptr;
	// Any key?
	if (uLength) {
		// Set the buffer to accept the value
		String16 Output;
		Output.SetBufferSize(uLength);
		// Read in the environment variable as UTF16
		GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(Key16.GetPtr()),
			reinterpret_cast<LPWSTR>(Output.GetPtr()), uLength + 1);
		// Convert to UTF8
		String Final(Output.GetPtr());
		// Return a copy
		pValue = StringDuplicate(Final.GetPtr());
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Word BURGER_API Burger::Globals::SetEnvironmentString(
	const char* pKey, const char* pInput)
{
	// Convert the key to UTF16
	String16 Key16(pKey);
	// Convert the input to UTF16
	String16 Input16(pInput);
	// If the input is an empty string or null, get rid of the string
	LPWSTR pInput16 = NULL;
	if (pInput && pInput[0]) {
		// Set to the new value
		pInput16 = reinterpret_cast<LPWSTR>(Input16.GetPtr());
	}
	// Set the variable!
	Word uResult = 0;
	if (!SetEnvironmentVariableW(
			reinterpret_cast<LPCWSTR>(Key16.GetPtr()), pInput16)) {
		uResult = static_cast<Word>(GetLastError());
	}
	return uResult;
}

#endif
