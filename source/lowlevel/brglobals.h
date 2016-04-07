/***************************************

	Global variable manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLOBALS_H__
#define __BRGLOBALS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDEBUG_H__
#include "brdebug.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {
class OutputMemoryStream;
class String;
class Globals {
public:
	enum {
		TRACE_MEMORYLEAKS=1,		///< Test and display memory leaks
		TRACE_REZLOAD=2,			///< Print the name of a resource file being loaded
		TRACE_FILELOAD=4,			///< Print the name of a file being loaded
		TRACE_WARNINGS=8,			///< Print possible errors
		TRACE_NETWORK=0x10,			///< Print the network commands
		TRACE_MESSAGES=0x20,		///< Print operating system messages
		TRACE_ACTIVEDEBUGGING=0x40,	///< Actively debugging the code (NEVER SET THIS for applications sent to testers or release)
		TRACE_THEWORKS=0x7F			///< GIVE ME everything!
	};

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	enum eWindowsVersionFound {
		WINDOWSVERSION_9598 = 0x01,				///< Set if ancient version of windows
		WINDOWSVERSION_XPORGREATER = 0x02,		///< Set if running on XP or higher
		WINDOWSVERSION_VISTAORGREATER = 0x04,	///< Set if Vista or higher
		WINDOWSVERSION_7ORGREATER = 0x08,		///< Set if Windows 7 or higher
		WINDOWSVERSION_8ORGREATER = 0x10,		///< Set if Windows 8 or higher
		WINDOWSVERSION_10ORGREATER = 0x20,		///< Set if Windows 10 or higher
		WINDOWSVERSION_TESTED = 0x80			///< Set if the rest of the flags are valid
	};
	enum eWindowsDLLIndex {
		DDRAW_DLL,		///< Index for ddraw.dll
		DINPUT_DLL,		///< Index for dinput.dll
		DINPUT8_DLL,	///< Index for dinput8.dll
		XINPUT1_4_DLL,	///< Index for xinput1_4.dll
		XINPUT1_3_DLL,	///< Index for xinput1_3.dll
		D3D9_DLL,		///< Index for d3d9.dll
		D3DX9_43_DLL,	///< Index for d3dx9_43.dll
		D3D11_DLL,		///< Index for d3d11.dll
		DXGI_DLL,		///< Index for dxgi.dll
		DSOUND_DLL,		///< Index for dsound.dll
		RPCRT4_DLL,		///< Index for rpcrt4.dll
		WINMM_DLL,		///< Index for winmm.dll
		SHLWAPI_DLL,	///< Index for shlwapi.dll
		VERSION_DLL,	///< Index for version.dll
		HID_DLL,		///< Index for hid.dll
		SETUPAPI_DLL,	///< Index for setupapi.dll
		USER32_DLL,		///< Index for user32.dll
		KERNEL32_DLL,	///< Index for kernel32.dll
		SHELL32_DLL,	///< Index for shell32.dll
		DLL_COUNT		///< Total number of DLLs to be managed
	};
	enum eWindowsCallIndex {
		CALL_DirectInputCreateA,				///< Index for DirectInputCreateA()
		CALL_DirectInputCreateW,				///< Index for DirectInputCreateW()
		CALL_DirectInput8Create,				///< Index for DirectInput8Create()
		CALL_XInputGetState,					///< Index for XInputGetState()
		CALL_XInputSetState,					///< Index for XInputSetState()
		CALL_XInputGetCapabilities,				///< Index for XInputGetCapabilities()
		CALL_XInputGetDSoundAudioDeviceGuids,	///< Index for XInputGetDSoundAudioDeviceGuids()
		CALL_XInputEnable,						///< Index for XInputEnable()
		CALL_XInputGetAudioDeviceIds,			///< Index for XInputGetAudioDeviceIds()
		CALL_XInputGetBatteryInformation,		///< Index for XInputGetBatteryInformation()
		CALL_XInputGetKeystroke,				///< Index for XInputGetKeystroke()
		CALL_DirectDrawCreate,					///< Index for DirectDrawCreate()
		CALL_DirectDrawCreateEx,				///< Index for DirectDrawCreateEx()
		CALL_DirectDrawCreateClipper,			///< Index for DirectDrawCreateClipper()
		CALL_DirectDrawEnumerateA,				///< Index for DirectDrawEnumerateA()
		CALL_DirectDrawEnumerateW,				///< Index for DirectDrawEnumerateW()
		CALL_DirectDrawEnumerateExA,			///< Index for DirectDrawEnumerateExA()
		CALL_DirectDrawEnumerateExW,			///< Index for DirectDrawEnumerateExW()
		CALL_Direct3DCreate9,					///< Index for Direct3DCreate9()
		CALL_D3DPERF_BeginEvent,				///< Index for D3DPERF_BeginEvent()
		CALL_D3DPERF_EndEvent,					///< Index for D3DPERF_EndEvent()
		CALL_D3DPERF_SetMarker,					///< Index for D3DPERF_SetMarker()
		CALL_D3DPERF_SetRegion,					///< Index for D3DPERF_SetRegion()
		CALL_D3DPERF_QueryRepeatFrame,			///< Index for D3DPERF_QueryRepeatFrame()
		CALL_D3DPERF_SetOptions,				///< Index for D3DPERF_SetOptions()
		CALL_D3DPERF_GetStatus,					///< Index for D3DPERF_GetStatus()
		CALL_D3DXCreateMatrixStack,				///< Index for D3DXCreateMatrixStack()
		CALL_D3D11CreateDevice,					///< Index for D3D11CreateDevice()
		CALL_CreateDXGIFactory,					///< Index for CreateDXGIFactory()
		CALL_CreateDXGIFactory1,				///< Index for CreateDXGIFactory1()
		CALL_CreateDXGIFactory2,				///< Index for CreateDXGIFactory2()
		CALL_DirectSoundCreate,					///< Index for DirectSoundCreate()
		CALL_DirectSoundEnumerateA,				///< Index for DirectSoundEnumerateA()
		CALL_DirectSoundEnumerateW,				///< Index for DirectSoundEnumerateW()
		CALL_DirectSoundCaptureCreate,			///< Index for DirectSoundCaptureCreate()
		CALL_DirectSoundCaptureEnumerateA,		///< Index for DirectSoundCaptureEnumerateA()
		CALL_DirectSoundCaptureEnumerateW,		///< Index for DirectSoundCaptureEnumerateW()
		CALL_DirectSoundCreate8,				///< Index for DirectSoundCreate8()
		CALL_DirectSoundCaptureCreate8,			///< Index for DirectSoundCaptureCreate8()
		CALL_DirectSoundFullDuplexCreate,		///< Index for DirectSoundFullDuplexCreate()
		CALL_GetDeviceID,						///< Index for GetDeviceID()
		CALL_UuidCreateSequential,				///< Index for UuidCreateSequential()
		CALL_timeGetTime,						///< Index for timeGetTime()
		CALL_PathSearchAndQualifyA,				///< Index for PathSearchAndQualifyA()
		CALL_PathSearchAndQualifyW,				///< Index for PathSearchAndQualifyW()
		CALL_VerQueryValueA,					///< Index for VerQueryValueA()
		CALL_VerQueryValueW,					///< Index for VerQueryValueW()
		CALL_GetFileVersionInfoA,				///< Index for GetFileVersionInfoA()
		CALL_GetFileVersionInfoW,				///< Index for GetFileVersionInfoW()
		CALL_GetFileVersionInfoSizeA,			///< Index for GetFileVersionInfoSizeA()
		CALL_GetFileVersionInfoSizeW,			///< Index for GetFileVersionInfoSizeW()
		CALL_HidD_GetHidGuid,					///< Index for HidD_GetHidGuid()
		CALL_SetupDiGetClassDevsA,				///< Index for SetupDiGetClassDevsA()
		CALL_SetupDiGetClassDevsW,				///< Index for SetupDiGetClassDevsW()
		CALL_SetupDiGetDeviceInterfaceDetailA,	///< Index for SetupDiGetDeviceInterfaceDetailA()
		CALL_SetupDiGetDeviceInterfaceDetailW,	///< Index for SetupDiGetDeviceInterfaceDetailW()
		CALL_SetupDiEnumDeviceInterfaces,		///< Index for SetupDiEnumDeviceInterfaces()
		CALL_SetupDiDestroyDeviceInfoList,		///< Index for SetupDiDestroyDeviceInfoList()
		CALL_TrackMouseEvent,					///< Index for TrackMouseEvent()
		CALL_GetSystemWow64DirectoryA,			///< Index for GetSystemWow64DirectoryA()
		CALL_GetSystemWow64DirectoryW,			///< Index for GetSystemWow64DirectoryW()
		CALL_SHGetKnownFolderPath,				///< Index for SHGetKnownFolderPath()
		CALL_COUNT
	};
private:
	static const Word16 g_GameClass[];		///< Windows class name "BurgerGameClass" (Windows only)
	static Word16 g_uAtom;					///< Atom assigned to my class (Windows only) 
	static HINSTANCE__ *g_hInstance;		///< Current singular instance of the application (Windows only)
	static HWND__ *g_hWindow;				///< Main window for the application (Windows only)
	static Word32 g_uQuickTimeVersion;		///< QuickTime's version in 0x0102 (1.2) format. (Windows only)
	static Word8 g_bQuickTimeVersionValid;	///< \ref TRUE if Quicktime's version is valid. (Windows only)
	static Word32 g_uDirectXVersion;		///< DirectX version 0x0900 (9.0) format (Windows only)
	static Word8 g_bDirectXVersionValid;	///< \ref TRUE if DirectX's version is valid (Windows only)
	static Word8 g_bWindowsVersionFlags;	///< See \ref eWindowsVersionFound for values. Set by \ref TestWindowsVersion()
#if defined(BURGER_WIN32) || defined(DOXYGEN)
	static Word8 g_bIsWindows64Bit;				///< Non-zero if tested, low bit has \ref TRUE or \ref FALSE if the 32 bit app was running under 64 bit windows (Windows only)
#endif
	// Data in the Globals instance, cleared via the destructor
	IDirectInputW* m_pDirectInputW;			///< Singleton DirectInputW instance (Windows only)
	IDirectInput8W* m_pDirectInput8W;		///< Singleton DirectInput8W instance (Windows only)
	HINSTANCE__ *m_hInstances[DLL_COUNT];	///< Instances of dynamically loaded system DLLs (Windows only)
	void *m_pWindowsCalls[CALL_COUNT];		///< Pointers to resolved windows function calls (Windows only)
	Word8 m_bInstancesTested[DLL_COUNT];	///< Flags to determine if a DLL was tested for loading (Windows only)
	Word8 m_bFunctionsTested[CALL_COUNT];	///< Flags to determine if a function was tested for loading (Windows only)
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	Word m_uAppleShareVersion;				///< Discovered version of AppleShare (Mac only)
	Word m_uInputSprocketVersion;			///< Discovered version of InputSprocket (Mac only)
	Word m_uDrawSprocketVersion;			///< Discovered version of DrawSprocket (Mac only)
	Word m_uSoundManagerVersion;			///< Sound manager version in 0x0102 (1.2) format (Mac only)
	Word m_uMacOSVersion;					///< Discovered version of MacOS (MacOS only)
	Word m_uQuickTimeVersion;				///< QuickTime's version in 0x0102 (1.2) format. (MacOS only)
	Word8 m_bIsQuickTimePlugInTested;		///< Non-zero if tested, low bit has \ref TRUE or \ref FALSE for QuickTime Plugin present (Mac only)
	Word8 m_bAppleShareVersionTested;		///< AppleShare version was tested (Mac only)
	Word8 m_bInputSprocketVersionTested;	///< InputSprocket version was tested (Mac only)
	Word8 m_bDrawSprocketVersionTested;		///< DrawSprocket version was tested (Mac only)
	Word8 m_bSoundManagerVersionValid;		///< \ref TRUE if sound manager version is valid (Mac only)
	Word8 m_bMacOSTested;					///< MacOS version was tested (MacOS only)
	Word8 m_bQuickTimeVersionValid;			///< \ref TRUE if Quicktime's version is valid. (MacOS only)
	Word8 m_bDrawSprocketActive;			///< \ref TRUE if DrawSprocket was started
#endif

#if defined(BURGER_MACOSX)
	Word m_uMacOSVersion;					///< Discovered version of MacOS (MacOS only)
	Word m_uQuickTimeVersion;				///< QuickTime's version in 0x0102 (1.2) format. (MacOS only)
	Word8 m_bMacOSTested;					///< MacOS version was tested (MacOS only)
	Word8 m_bQuickTimeVersionValid;			///< \ref TRUE if Quicktime's version is valid. (MacOS only)
#endif

private:
	static int g_iErrorCode;		///< Global default error code used by \ref Globals::Shutdown().
	static Word g_uTraceFlags;		///< Debug information level
	static char g_ErrorMsg[512];	///< Global Buffer containing the last fatal error or warning
	static Word g_bBombFlag;		///< \ref TRUE if non-fatal errors are treated as fatal
	static Word g_bExitFlag;		///< Global \ref TRUE if the app is in the process of shutting down.
	static Globals g_Globals;		///< Singleton instance of the global variables
public:

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	~Globals();
	BURGER_INLINE static const Word16 *GetWindowClassName(void) { return g_GameClass;}
	BURGER_INLINE static HINSTANCE__ *GetInstance(void) { return g_hInstance; }
	BURGER_INLINE static void SetInstance(HINSTANCE__ *pInput) { g_hInstance = pInput; }
	BURGER_INLINE static HWND__ *GetWindow(void) { return g_hWindow; }
	BURGER_INLINE static void SetWindow(HWND__ *pInput) { g_hWindow = pInput; }

	static Word BURGER_API TestWindowsVersion(void);
	BURGER_INLINE static Word IsWin95orWin98(void) { return (TestWindowsVersion()&WINDOWSVERSION_9598); }
	BURGER_INLINE static Word IsWinXPOrGreater(void) { return (TestWindowsVersion()&WINDOWSVERSION_XPORGREATER)!=0; }
	BURGER_INLINE static Word IsVistaOrGreater(void) { return (TestWindowsVersion()&WINDOWSVERSION_VISTAORGREATER)!=0; }
	BURGER_INLINE static Word IsWin7OrGreater(void) { return (TestWindowsVersion()&WINDOWSVERSION_7ORGREATER)!=0; }
	BURGER_INLINE static Word IsWin8OrGreater(void) { return (TestWindowsVersion()&WINDOWSVERSION_8ORGREATER)!=0; }
	BURGER_INLINE static Word IsWin10OrGreater(void) { return (TestWindowsVersion()&WINDOWSVERSION_10ORGREATER)!=0; }

#if defined(BURGER_WIN32) || defined(DOXYGEN)
	static Word BURGER_API IsWindows64Bit(void);
#else
	BURGER_INLINE static Word IsWindows64Bit(void) { return TRUE; }
#endif

	static HINSTANCE__ * BURGER_API LoadLibraryIndex(eWindowsDLLIndex eIndex);
	static void * BURGER_API LoadFunctionIndex(eWindowsCallIndex eIndex);

	BURGER_INLINE static Word IsDirectInputPresent(void) { return (LoadLibraryIndex(DINPUT_DLL)!=NULL); }
	BURGER_INLINE static Word IsDirectInput8Present(void) { return (LoadLibraryIndex(DINPUT8_DLL)!=NULL); }
	BURGER_INLINE static Word IsXInputPresent(void) { return (LoadLibraryIndex(XINPUT1_4_DLL)!=NULL); }
	BURGER_INLINE static Word IsDirectDrawPresent(void) { return (LoadLibraryIndex(DDRAW_DLL)!=NULL); }
	BURGER_INLINE static Word IsD3D9Present(void) { return (LoadLibraryIndex(D3D9_DLL)!=NULL); }
	BURGER_INLINE static Word IsDirectSoundPresent(void) { return (LoadLibraryIndex(DSOUND_DLL)!=NULL); }

	static void BURGER_API GetQTFolderFromRegistry(const char *pSubKey,const char *pValueName,char *pBuffer,Word32 uSize);
	static Word BURGER_API GetPathToQuickTimeFolder(char *pBuffer,Word32 uSize,Word32 *pReserved);
	static Word32 BURGER_API GetQTSystemDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTApplicationDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTExtensionDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTComponentDirectoryA(char *pBuffer,Word32 uSize);
	static HINSTANCE__ * BURGER_API QTLoadLibrary(const char *pDLLName);

	static IDirectInputW *BURGER_API GetDirectInputSingleton(Word uVersion=0x700);
	static IDirectInput8W *BURGER_API GetDirectInput8Singleton(void);

	static Word BURGER_API DirectInputCreateA(HINSTANCE__ *hInst,Word32 uVersion,IDirectInputA **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectInputCreateW(HINSTANCE__ *hInst,Word32 uVersion,IDirectInputW **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectInput8Create(HINSTANCE__ *hInst,Word32 uVersion,const GUID &rGUID,void **ppOutput,IUnknown *pOuter=NULL);
	static Word32 BURGER_API XInputGetState(Word32 dwUserIndex,_XINPUT_STATE* pState);
	static Word32 BURGER_API XInputSetState(Word32 dwUserIndex,_XINPUT_VIBRATION* pVibration);
	static Word32 BURGER_API XInputGetCapabilities(Word32 dwUserIndex,Word32 dwFlags,_XINPUT_CAPABILITIES* pCapabilities);
	static Word32 BURGER_API XInputGetDSoundAudioDeviceGuids(Word32 dwUserIndex,GUID* pDSoundRenderGuid,GUID* pDSoundCaptureGuid);
	static void BURGER_API XInputEnable(Word bEnable);
	static Word32 BURGER_API XInputGetAudioDeviceIds(Word32 dwUserIndex,Word16 *pRenderDeviceId,Word *pRenderCount,Word16 *pCaptureDeviceId,Word*pCaptureCount);
	static Word32 BURGER_API XInputGetBatteryInformation(Word32 dwUserIndex,Word devType,_XINPUT_BATTERY_INFORMATION* pBatteryInformation);
	static Word32 BURGER_API XInputGetKeystroke(Word32 dwUserIndex,Word32 dwReserved,_XINPUT_KEYSTROKE *pKeystroke);
	static Word BURGER_API DirectDrawCreate(const GUID *pGuid,IDirectDraw **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectDrawCreateEx(const GUID *pGuid,void **ppOutput,const GUID &rGUID,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectDrawCreateClipper(Word32 uFlags,IDirectDrawClipper **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectDrawEnumerateA(void *pCallback,void *pContext);
	static Word BURGER_API DirectDrawEnumerateW(void *pCallback,void *pContext);
	static Word BURGER_API DirectDrawEnumerateExA(void *pCallback,void *pContext,Word32 uFlags);
	static Word BURGER_API DirectDrawEnumerateExW(void *pCallback,void *pContext,Word32 uFlags);
	static IDirect3D9 * BURGER_API Direct3DCreate9(Word uSDKVersion);
	static int BURGER_API D3DPERF_BeginEvent(Word32 col,const Word16 *wszName);
	static int BURGER_API D3DPERF_EndEvent(void);
	static void BURGER_API D3DPERF_SetMarker(Word32 col,const Word16 *wszName);
	static void BURGER_API D3DPERF_SetRegion(Word32 col,const Word16 *wszName);
	static int BURGER_API D3DPERF_QueryRepeatFrame(void);
	static void BURGER_API D3DPERF_SetOptions(Word32 dwOptions);
	static Word BURGER_API D3DPERF_GetStatus(void);
	static Word BURGER_API D3DXCreateMatrixStack(Word uFlags,ID3DXMatrixStack **ppStack);
	static Word BURGER_API D3D11CreateDevice(IDXGIAdapter *pAdapter,Word DriverType,HINSTANCE__ *Software,
		Word Flags,const Word *pFeatureLevels,Word FeatureLevels,Word SDKVersion,ID3D11Device **ppDevice,
		Word *pFeatureLevel,ID3D11DeviceContext **ppImmediateContext);
	static Word BURGER_API CreateDXGIFactory(const GUID *pGuidFactory,void **ppFactory);
	static Word BURGER_API CreateDXGIFactory1(const GUID *pGuidFactory,void **ppFactory);
	static Word BURGER_API CreateDXGIFactory2(Word uFlags,const GUID *pGuidFactory,void **ppFactory);
	static Word BURGER_API DirectSoundCreate(const GUID *pGuidDevice,IDirectSound **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectSoundEnumerateA(void *pDSEnumCallback,void *pContext);
	static Word BURGER_API DirectSoundEnumerateW(void *pDSEnumCallback,void *pContext);
	static Word BURGER_API DirectSoundCaptureCreate(const GUID *pGuidDevice,IDirectSoundCapture **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectSoundCaptureEnumerateA(void *pDSEnumCallback,void *pContext);
	static Word BURGER_API DirectSoundCaptureEnumerateW(void *pDSEnumCallback,void *pContext);
	static Word BURGER_API DirectSoundCreate8(const GUID *pGuidDevice,IDirectSound8 **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectSoundCaptureCreate8(const GUID *pGuidDevice,IDirectSoundCapture **ppOutput,IUnknown *pOuter=NULL);
	static Word BURGER_API DirectSoundFullDuplexCreate(const GUID *pGuidCaptureDevice,const GUID *pGuidRenderDevice,
		const _DSCBUFFERDESC *pDSCBufferDesc,const _DSBUFFERDESC *pDSBufferDesc,HWND__ *hWnd,Word32 uLevel,
		IDirectSoundFullDuplex **ppDSFD,IDirectSoundCaptureBuffer8 **ppDSCBuffer8,IDirectSoundBuffer8 **ppDSBuffer8,IUnknown *pOuter=NULL);
	static Word BURGER_API GetDeviceID(const GUID *pGuidSrc,GUID *pGuidDest);
	static Word BURGER_API UuidCreateSequential(GUID *pOutput);
	static Word32 BURGER_API timeGetTime(void);
	static Word BURGER_API PathSearchAndQualifyA(const char *pszPath,char *pszBuf,Word32 cchBuf);
	static Word BURGER_API PathSearchAndQualifyW(const Word16 *pszPath,Word16 *pszBuf,Word32 cchBuf);
	static Word BURGER_API VerQueryValueA(const void *pBlock,const char *pSubBlock,void **ppBuffer,Word *pLen);
	static Word BURGER_API VerQueryValueW(const void *pBlock,const Word16 *pSubBlock,void **ppBuffer,Word *pLen);
	static Word BURGER_API GetFileVersionInfoA(const char *ptstrFilename,Word32 dwHandle,Word32 dwLen,void *pData);
	static Word BURGER_API GetFileVersionInfoW(const Word16 *ptstrFilename,Word32 dwHandle,Word32 dwLen,void *pData);
	static Word32 BURGER_API GetFileVersionInfoSizeA(const char *ptstrFilename,unsigned long *pdwHandle);
	static Word32 BURGER_API GetFileVersionInfoSizeW(const Word16 *ptstrFilename,unsigned long *pdwHandle);
	static void BURGER_API HidD_GetHidGuid(GUID *pHidGuid);
	static void * BURGER_API SetupDiGetClassDevsA(const GUID *ClassGuid,const char *Enumerator,HWND__ *hwndParent,Word32 Flags);
	static void * BURGER_API SetupDiGetClassDevsW(const GUID *ClassGuid,const Word16 *Enumerator,HWND__ *hwndParent,Word32 Flags);
	static Word BURGER_API SetupDiGetDeviceInterfaceDetailA(void *DeviceInfoSet,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData,_SP_DEVICE_INTERFACE_DETAIL_DATA_A *DeviceInterfaceDetailData,Word32 DeviceInterfaceDetailDataSize,Word32 *RequiredSize,_SP_DEVINFO_DATA *DeviceInfoData);
	static Word BURGER_API SetupDiGetDeviceInterfaceDetailW(void *DeviceInfoSet,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData,_SP_DEVICE_INTERFACE_DETAIL_DATA_W *DeviceInterfaceDetailData,Word32 DeviceInterfaceDetailDataSize,Word32 *RequiredSize,_SP_DEVINFO_DATA *DeviceInfoData);
	static Word BURGER_API SetupDiEnumDeviceInterfaces(void *DeviceInfoSet,_SP_DEVINFO_DATA *DeviceInfoData,const GUID *InterfaceClassGuid,Word32 MemberIndex,_SP_DEVICE_INTERFACE_DATA *DeviceInterfaceData);
	static Word BURGER_API SetupDiDestroyDeviceInfoList(void *DeviceInfoSet);
	static Word BURGER_API TrackMouseEvent(::tagTRACKMOUSEEVENT *pEventTrack);
	static Word BURGER_API GetSystemWow64DirectoryA(char *pBuffer,Word32 uSize);
	static Word BURGER_API GetSystemWow64DirectoryW(Word16 *pBuffer,Word32 uSize);
	static Word BURGER_API SHGetKnownFolderPath(const GUID *pGuid,Word32 uFlags,void *hHandle,Word16 **ppResult);

	static Word64 BURGER_API GetFileVersion64(const Word16* pWindowsFilename);
	static Word BURGER_API GetDirectXVersionViaFileVersions(void);
	static Word BURGER_API GetDirectXVersion(void);
	static Word BURGER_API GetVideoGUID(GUID *pOutput,Word uDevNum);
	static WordPtr BURGER_API ShellExecuteOpen(const char *pFileToOpen);
	static Word BURGER_API LaunchMediaCenter(void);
	static HINSTANCE__ * BURGER_API LoadLibraryA(const char *pInput);
	static HINSTANCE__ * BURGER_API LoadLibraryW(const Word16 *pInput);
	static HINSTANCE__ * BURGER_API LoadLibraryExA(const char *pInput,void *hFile,Word32 uFlags);
	static HINSTANCE__ * BURGER_API LoadLibraryExW(const Word16 *pInput,void *hFile,Word32 uFlags);
	static Word BURGER_API AddGroupToProgramMenu(const char *pGroupName);
	static int BURGER_API CreateUserRegistryKey(const char *pKey,const char *pSubKey,const char *pData);
	static void BURGER_API AssociateFileExtensionToExe(const char *pFileExtension,const char *pDescription,const char *pProgramID);
	static void BURGER_API OutputWindowsMessage(Word uMessage,WordPtr wParam,WordPtr lParam);
	static Word16 BURGER_API RegisterWindowClass(Word uIconResID=0);
	static void BURGER_API UnregisterWindowClass(void);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	static void BURGER_API AddToMenubar(NSMenu *pNSMenu);
	static NSString *BURGER_API GetApplicationName(void);
	static void BURGER_API CreateApplicationMenu(void);
	static void BURGER_API CreateWindowMenu(void);
	static void BURGER_API CreateViewMenu(void);
	static void BURGER_API CreateHelpMenu(void);
	static void BURGER_API CreateDefaultMenus(void);
	static void BURGER_API StringCopy(String *pOutput,const __CFString *pInput);
	static void BURGER_API GetHIDDeviceName(String *pOutput,__IOHIDDevice *pDevice);
	static __CFDictionary * BURGER_API CreateHIDDictionary(Word uPage,Word uUsage);
	static void GetDisplayName(String *pOutput,Word uDisplayID);
	static int NumberFromKey(const __CFDictionary *pDictionary,const char *pKey);
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	static Word BURGER_API IsTrapAvailable(Word uTrapNum);
	static Word BURGER_API IsQuickTimePowerPlugAvailable(void);
	static Word BURGER_API GetAppleShareVersion(void);
	static Word BURGER_API GetInputSprocketVersion(void);
	static Word BURGER_API GetDrawSprocketVersion(void);
	static Word BURGER_API GetSoundManagerVersion(void);
	static void BURGER_API KillProcess(ProcessSerialNumber *pVictim);
	static void BURGER_API KillAllProcesses(void);
	static Word BURGER_API StartDrawSprocket(void);
	static void BURGER_API StopDrawSprocket(void);
#endif

#if defined(BURGER_MACOS) || defined(DOXYGEN)
	static Word BURGER_API GetMacOSVersion(void);
#endif
#if defined(BURGER_MACOS) || defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static Word BURGER_API GetQuickTimeVersion(void);
#endif

	static BURGER_INLINE int GetErrorCode(void) { return g_iErrorCode; }
	static BURGER_INLINE void SetErrorCode(int iNewError) { g_iErrorCode = iNewError; }
	static BURGER_INLINE char *GetErrorMsg(void) { return g_ErrorMsg; }
	static void BURGER_ANSIAPI SetErrorMsg(const char *pMessage,...);
	static BURGER_INLINE Word GetTraceFlag(void) { return g_uTraceFlags; }
	static BURGER_INLINE void SetTraceFlag(Word uNewFlag) { g_uTraceFlags = uNewFlag; }
	static BURGER_INLINE Word AreWarningsEnabled(void) { return g_uTraceFlags&TRACE_WARNINGS; }
	static BURGER_INLINE Word GetErrorBombFlag(void) { return g_bBombFlag; }
	static BURGER_INLINE Word SetErrorBombFlag(Word uNewFlag) { Word uOld = g_bBombFlag; g_bBombFlag = uNewFlag; return uOld; }
	static BURGER_INLINE Word GetExitFlag(void) { return g_bExitFlag; }
	static BURGER_INLINE void SetExitFlag(Word uNewFlag) { g_bExitFlag = uNewFlag; }
	static void BURGER_API Shutdown(void);
	static void BURGER_API Shutdown(int iError);
	static Word BURGER_API Version(void);
	static Word BURGER_API LaunchURL(const char *pURL);
	static int BURGER_API ExecuteTool(const char *pFilename,const char *pParameters,OutputMemoryStream *pOutput=NULL);
	static const char * BURGER_API GetEnvironmentString(const char *pKey);
	static Word BURGER_API SetEnvironmentString(const char *pKey,const char *pInput);
};
}
/* END */

#endif
