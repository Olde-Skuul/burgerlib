/***************************************

    Typedefs specific to Windows

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRWINDOWSTYPES_H__
#define __BRWINDOWSTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

/* BEGIN */
#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#if defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Windows forward declarations
struct IUnknown;
struct HBITMAP__;
struct HDC__;
struct HGLRC__;
struct HHOOK__;
struct HICON__;
struct HINSTANCE__;
struct HKEY__;
struct HMENU__;
struct HMONITOR__;
struct HPALETTE__;
struct HWND__;
struct IDirectInputA;
struct IDirectInputW;
struct IDirectInput8A;
struct IDirectInput8W;
struct IDirectInputDevice2A;
struct IDirectInputDevice2W;
struct IDirectInputDevice8A;
struct IDirectInputDevice8W;
struct IDirectDraw;
struct IDirectDraw2;
struct IDirectDraw4;
struct IDirectDraw7;
struct IDirectDrawClipper;
struct IDirectDrawColorControl;
struct IDirectDrawGammaControl;
struct IDirectDrawPalette;
struct IDirectDrawSurface;
struct IDirectDrawSurface2;
struct IDirectDrawSurface3;
struct IDirectDrawSurface4;
struct IDirectDrawSurface7;
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DStateBlock9;
struct IDirect3DVertexDeclaration9;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
struct IDirect3DResource9;
struct IDirect3DBaseTexture9;
struct IDirect3DTexture9;
struct IDirect3DVolumeTexture9;
struct IDirect3DCubeTexture9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DSurface9;
struct IDirect3DVolume9;
struct IDirect3DSwapChain9;
struct IDirect3DQuery9;
struct IDXGIAdapter;
struct IDXGIObject;
struct ID3D10Device;
struct ID3D10DeviceChild;
struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSound3DListener;
struct IDirectSound3DBuffer;
struct IDirectSoundCapture;
struct IDirectSoundCaptureBuffer;
struct IDirectSoundNotify;
struct IDirectSound8;
struct IDirectSoundBuffer8;
struct IDirectSoundCaptureBuffer8;
struct IDirectSoundFXGargle;
struct IDirectSoundFXChorus;
struct IDirectSoundFXFlanger;
struct IDirectSoundFXEcho;
struct IDirectSoundFXDistortion;
struct IDirectSoundFXCompressor;
struct IDirectSoundFXParamEq;
struct IDirectSoundFXWavesReverb;
struct IDirectSoundFXI3DL2Reverb;
struct IDirectSoundCaptureFXAec;
struct IDirectSoundCaptureFXNoiseSuppress;
struct IDirectSoundFullDuplex;
struct IDirectPlay;
struct IDirectPlay2;
struct IDirectPlay3;
struct IDirectPlay4;
struct IDirectPlay8Client;
struct IDirectPlay8Server;
struct IDirectPlay8Peer;
struct IDirectPlay8ThreadPool;
struct IDirectPlay8NATResolver;
struct IDirectPlay8LobbiedApplication;
struct IDirectPlay8Address;
struct IDirectPlayLobby;
struct IDirectPlayLobby2;
struct IDirectPlayLobby3;
struct IDirectPlay8LobbiedApplication;
struct IDirectPlay8LobbyClient;
struct ID3DXEffect;
struct ID3DXEffectPool;
struct ID3DXMatrixStack;
struct _D3DADAPTER_IDENTIFIER9;
struct _D3DCAPS9;
struct _D3DDISPLAYMODE;
struct _D3DPRESENT_PARAMETERS_;
struct _D3DSURFACE_DESC;
struct _DIDATAFORMAT;
struct _DSBUFFERDESC;
struct _DSCBUFFERDESC;
struct _FILETIME;
struct _SYSTEMTIME;
struct _GUID;
struct _RTL_CRITICAL_SECTION_DEBUG;
struct _SP_DEVICE_INTERFACE_DATA;
struct _SP_DEVINFO_DATA;
struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A;
struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W;
struct _XINPUT_STATE;
struct _XINPUT_VIBRATION;
struct _XINPUT_CAPABILITIES;
struct _XINPUT_BATTERY_INFORMATION;
struct _XINPUT_KEYSTROKE;
struct tagFILTERKEYS;
struct tagMONITORINFO;
struct tagPOINT;
struct tagRECT;
struct tagSTICKYKEYS;
struct tagTOGGLEKEYS;
struct tagTRACKMOUSEEVENT;
struct tagWINDOWPLACEMENT;

struct IXAudio2;
struct IXAudio2Voice;
struct IXAudio2SourceVoice;
struct IXAudio2SubmixVoice;
struct IXAudio2MasteringVoice;
struct IXAudio2EngineCallback;
struct IXAudio2VoiceCallback;

struct BurgerCRITICAL_SECTION {
	_RTL_CRITICAL_SECTION_DEBUG* DebugInfo;
	Word32 LockCount;
	Word32 RecursionCount;
	void* OwningThread;
	void* LockSemaphore;
	WordPtr SpinCount;
};

struct BurgertagSTICKYKEYS {
	Word cbSize;
	Word dwFlags;
};

struct BurgertagTOGGLEKEYS {
	Word cbSize;
	Word dwFlags;
};

struct BurgertagFILTERKEYS {
	Word cbSize;
	Word dwFlags;
	Word iWaitMSec;
	Word iDelayMSec;
	Word iRepeatMSec;
	Word iBounceMSec;
};

#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
namespace Burger {
class Filename;
class Windows {
public:
	enum eVersion {
		/** Set if ancient version of windows. */
		VERSION_9598 = 0x01,
		/** Set if running on XP or higher. */
		VERSION_XPORGREATER = 0x02,
		/** Set if Vista or higher. */
		VERSION_VISTAORGREATER = 0x04,
		/** Set if Windows 7 or higher. */
		VERSION_7ORGREATER = 0x08,
		/** Set if Windows 8 or higher. */
		VERSION_8ORGREATER = 0x10,
		/** Set if Windows 10 or higher. */
		VERSION_10ORGREATER = 0x20,
		/** Set if the rest of the flags are valid. */
		VERSION_TESTED = 0x80
	};

	enum eDLLIndex {
		/** Index for ddraw.dll */
		DDRAW_DLL,
		/** Index for dinput.dll */
		DINPUT_DLL,
		/** Index for dinput8.dll */
		DINPUT8_DLL,
		/** Index for xinput1_4.dll */
		XINPUT1_4_DLL,
		/** Index for xinput1_3.dll */
		XINPUT1_3_DLL,
		/** Index for d3d9.dll */
		D3D9_DLL,
		/** Index for d3dx9_43.dll */
		D3DX9_43_DLL,
		/** Index for d3d11.dll */
		D3D11_DLL,
		/** Index for dxgi.dll */
		DXGI_DLL,
		/** Index for dsound.dll */
		DSOUND_DLL,
		/** Index for dplayx.dll */
		DPLAYX_DLL,
		/** Index for dplay.dll */
		DPLAY_DLL,
		/** Index for rpcrt4.dll */
		RPCRT4_DLL,
		/** Index for winmm.dll */
		WINMM_DLL,
		/** Index for shlwapi.dll */
		SHLWAPI_DLL,
		/** Index for version.dll */
		VERSION_DLL,
		/** Index for hid.dll */
		HID_DLL,
		/** Index for setupapi.dll */
		SETUPAPI_DLL,
		/** Index for user32.dll */
		USER32_DLL,
		/** Index for kernel32.dll */
		KERNEL32_DLL,
		/** Index for shell32.dll */
		SHELL32_DLL,
		/** Total number of DLLs to be managed */
		DLL_COUNT
	};

	enum eCallIndex {
		/** Index for DirectInputCreateA() */
		CALL_DirectInputCreateA,
		/** Index for DirectInputCreateW() */
		CALL_DirectInputCreateW,
		/** Index for DirectInput8Create() */
		CALL_DirectInput8Create,
		/** Index for XInputGetState() */
		CALL_XInputGetState,
		/** Index for XInputSetState() */
		CALL_XInputSetState,
		/** Index for XInputGetCapabilities() */
		CALL_XInputGetCapabilities,
		/** Index for XInputGetDSoundAudioDeviceGuids(). */
		CALL_XInputGetDSoundAudioDeviceGuids,
		/** Index for XInputEnable() */
		CALL_XInputEnable,
		/** Index for XInputGetAudioDeviceIds() */
		CALL_XInputGetAudioDeviceIds,
		/** Index for XInputGetBatteryInformation() */
		CALL_XInputGetBatteryInformation,
		/** Index for XInputGetKeystroke() */
		CALL_XInputGetKeystroke,
		/** Index for DirectDrawCreate() */
		CALL_DirectDrawCreate,
		/** Index for DirectDrawCreateEx() */
		CALL_DirectDrawCreateEx,
		/** Index for DirectDrawCreateClipper() */
		CALL_DirectDrawCreateClipper,
		/** Index for DirectDrawEnumerateA() */
		CALL_DirectDrawEnumerateA,
		/** Index for DirectDrawEnumerateW() */
		CALL_DirectDrawEnumerateW,
		/** Index for DirectDrawEnumerateExA() */
		CALL_DirectDrawEnumerateExA,
		/** Index for DirectDrawEnumerateExW() */
		CALL_DirectDrawEnumerateExW,
		/** Index for Direct3DCreate9() */
		CALL_Direct3DCreate9,
		/** Index for D3DPERF_BeginEvent() */
		CALL_D3DPERF_BeginEvent,
		/** Index for D3DPERF_EndEvent() */
		CALL_D3DPERF_EndEvent,
		/** Index for D3DPERF_SetMarker() */
		CALL_D3DPERF_SetMarker,
		/** Index for D3DPERF_SetRegion() */
		CALL_D3DPERF_SetRegion,
		/** Index for D3DPERF_QueryRepeatFrame() */
		CALL_D3DPERF_QueryRepeatFrame,
		/** Index for D3DPERF_SetOptions() */
		CALL_D3DPERF_SetOptions,
		/** Index for D3DPERF_GetStatus() */
		CALL_D3DPERF_GetStatus,
		/** Index for D3DXCreateMatrixStack() */
		CALL_D3DXCreateMatrixStack,
		/** Index for D3D11CreateDevice() */
		CALL_D3D11CreateDevice,
		/** Index for CreateDXGIFactory() */
		CALL_CreateDXGIFactory,
		/** Index for CreateDXGIFactory1() */
		CALL_CreateDXGIFactory1,
		/** Index for CreateDXGIFactory2() */
		CALL_CreateDXGIFactory2,
		/** Index for DirectSoundCreate() */
		CALL_DirectSoundCreate,
		/** Index for DirectSoundEnumerateA() */
		CALL_DirectSoundEnumerateA,
		/** Index for DirectSoundEnumerateW() */
		CALL_DirectSoundEnumerateW,
		/** Index for DirectSoundCaptureCreate() */
		CALL_DirectSoundCaptureCreate,
		/** Index forDirectSoundCaptureEnumerateA() */
		CALL_DirectSoundCaptureEnumerateA,
		/** Index for DirectSoundCaptureEnumerateW() */
		CALL_DirectSoundCaptureEnumerateW,
		/** Index for DirectSoundCreate8() */
		CALL_DirectSoundCreate8,
		/** Index for DirectSoundCaptureCreate8() */
		CALL_DirectSoundCaptureCreate8,
		/** Index for DirectSoundFullDuplexCreate() */
		CALL_DirectSoundFullDuplexCreate,
		/** Index for DirectPlayCreate() */
		CALL_DirectPlayCreate,
		/** Index for DirectPlayEnumerate() Note, this is REALLY obsolete */
		CALL_DirectPlayEnumerate,
		/** Index for DirectPlayEnumerateA() */
		CALL_DirectPlayEnumerateA,
		/** Index for DirectPlayEnumerateW() */
		CALL_DirectPlayEnumerateW,
		/** Index for DirectPlayLobbyCreateA() */
		CALL_DirectPlayLobbyCreateA,
		/** Index for DirectPlayLobbyCreateW() */
		CALL_DirectPlayLobbyCreateW,
		/** Index for GetDeviceID() */
		CALL_GetDeviceID,
		/** Index for UuidCreateSequential() */
		CALL_UuidCreateSequential,
		/** Index for timeGetTime() */
		CALL_timeGetTime,
		/** Index for PathSearchAndQualifyA() */
		CALL_PathSearchAndQualifyA,
		/** Index for PathSearchAndQualifyW() */
		CALL_PathSearchAndQualifyW,
		/** Index for VerQueryValueA() */
		CALL_VerQueryValueA,
		/** Index for VerQueryValueW() */
		CALL_VerQueryValueW,
		/** Index for GetFileVersionInfoA() */
		CALL_GetFileVersionInfoA,
		/** Index for GetFileVersionInfoW() */
		CALL_GetFileVersionInfoW,
		/** Index for GetFileVersionInfoSizeA() */
		CALL_GetFileVersionInfoSizeA,
		/** Index for GetFileVersionInfoSizeW() */
		CALL_GetFileVersionInfoSizeW,
		/** Index for HidD_GetHidGuid() */
		CALL_HidD_GetHidGuid,
		/** Index for SetupDiGetClassDevsA() */
		CALL_SetupDiGetClassDevsA,
		/** Index for SetupDiGetClassDevsW() */
		CALL_SetupDiGetClassDevsW,
		/** Index for SetupDiGetDeviceInterfaceDetailA() */
		CALL_SetupDiGetDeviceInterfaceDetailA,
		/** Index for SetupDiGetDeviceInterfaceDetailW() */
		CALL_SetupDiGetDeviceInterfaceDetailW,
		/** Index for SetupDiEnumDeviceInterfaces() */
		CALL_SetupDiEnumDeviceInterfaces,
		/** Index for SetupDiDestroyDeviceInfoList() */
		CALL_SetupDiDestroyDeviceInfoList,
		/** Index for TrackMouseEvent() */
		CALL_TrackMouseEvent,
		/** Index for GetMonitorInfoA() */
		CALL_GetMonitorInfoA,
		/** Index for GetMonitorInfoW() */
		CALL_GetMonitorInfoW,
		/** Index for MonitorFromWindow() */
		CALL_MonitorFromWindow,
		/** Index for MonitorFromRect() */
		CALL_MonitorFromRect,
		/** Index for GetSystemWow64DirectoryA() */
		CALL_GetSystemWow64DirectoryA,
		/** Index for GetSystemWow64DirectoryW() */
		CALL_GetSystemWow64DirectoryW,
		/** Index for IsDebuggerPresent() */
		CALL_IsDebuggerPresent,
		/** Index for SHGetKnownFolderPath() */
		CALL_SHGetKnownFolderPath,
		/** Total number of function calls to be managed */
		CALL_COUNT
	};

private:
	/** Singleton DirectInputW instance \windowsonly */
	IDirectInputW* m_pDirectInputW;
	/** Singleton DirectInput8W instance \windowsonly */
	IDirectInput8W* m_pDirectInput8W;

	/** Instances of dynamically loaded system DLLs \windowsonly */
	HINSTANCE__* m_hInstances[DLL_COUNT];
	/** Pointers to resolved windows function calls \windowsonly */
	void* m_pWindowsCalls[CALL_COUNT];
	/** Flags to determine if a DLL was tested for loading \windowsonly */
	Word8 m_bInstancesTested[DLL_COUNT];
	/** Flags to determine if a function was tested for loading \windowsonly
	 */
	Word8 m_bFunctionsTested[CALL_COUNT];

	/** \ref eVersion as set by \ref TestVersion(). */
	Word8 m_bVersionFlags;

	static Windows g_Globals;

	/** Current singular instance of the application \windowsonly */
	static HINSTANCE__* g_hInstance;

public:
	~Windows();

    static BURGER_INLINE HINSTANCE__* GetInstance(void) BURGER_NOEXCEPT
	{
		return g_hInstance;
	}
    static BURGER_INLINE void SetInstance(HINSTANCE__* pInput) BURGER_NOEXCEPT
	{
		g_hInstance = pInput;
	}

	static Word BURGER_API TestVersion(void) BURGER_NOEXCEPT;

    static BURGER_INLINE Word IsWin95orWin98(void) BURGER_NOEXCEPT
	{
		return TestVersion() & VERSION_9598;
	}
    static BURGER_INLINE Word IsWinXPOrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_XPORGREATER) != 0;
	}
    static BURGER_INLINE Word IsVistaOrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_VISTAORGREATER) != 0;
	}
    static BURGER_INLINE Word IsWin7OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_7ORGREATER) != 0;
	}
    static BURGER_INLINE Word IsWin8OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_8ORGREATER) != 0;
	}
    static BURGER_INLINE Word IsWin10OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_10ORGREATER) != 0;
	}

	static HINSTANCE__* BURGER_API LoadLibraryIndex(eDLLIndex eIndex);
	static void* BURGER_API LoadFunctionIndex(eCallIndex eIndex);

    static BURGER_INLINE Word IsDirectInputPresent(void)
	{
		return LoadLibraryIndex(DINPUT_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsDirectInput8Present(void)
	{
		return LoadLibraryIndex(DINPUT8_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsXInputPresent(void)
	{
		return LoadLibraryIndex(XINPUT1_4_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsDirectDrawPresent(void)
	{
		return LoadLibraryIndex(DDRAW_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsD3D9Present(void)
	{
		return LoadLibraryIndex(D3D9_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsDirectSoundPresent(void)
	{
		return LoadLibraryIndex(DSOUND_DLL) != nullptr;
	}
    static BURGER_INLINE Word IsDirectPlayPresent(void)
	{
		return LoadLibraryIndex(DPLAYX_DLL) != nullptr;
	}
	static Word BURGER_API IsXAudio2Present(void);

	// DINPUT.dll and DINPUT8.dll
	static Word BURGER_API DirectInputCreateA(HINSTANCE__* hInst,
		Word32 uVersion, IDirectInputA** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectInputCreateW(HINSTANCE__* hInst,
		Word32 uVersion, IDirectInputW** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectInput8Create(HINSTANCE__* hInst,
		Word32 uVersion, const GUID& rGUID, void** ppOutput,
		IUnknown* pOuter = nullptr);
	static IDirectInputW* BURGER_API GetDirectInputSingleton(
		Word uVersion = 0x700);
	static IDirectInput8W* BURGER_API GetDirectInput8Singleton(void);

	// XINPUT1_4.dll or XINPUT1_3.dll
	static Word32 BURGER_API XInputGetState(
		Word32 dwUserIndex, _XINPUT_STATE* pState);
	static Word32 BURGER_API XInputSetState(
		Word32 dwUserIndex, _XINPUT_VIBRATION* pVibration);
	static Word32 BURGER_API XInputGetCapabilities(Word32 dwUserIndex,
		Word32 dwFlags, _XINPUT_CAPABILITIES* pCapabilities);
	static Word32 BURGER_API XInputGetDSoundAudioDeviceGuids(
		Word32 dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
	static void BURGER_API XInputEnable(Word bEnable);
	static Word32 BURGER_API XInputGetAudioDeviceIds(Word32 dwUserIndex,
		Word16* pRenderDeviceId, Word* pRenderCount, Word16* pCaptureDeviceId,
		Word* pCaptureCount);
	static Word32 BURGER_API XInputGetBatteryInformation(Word32 dwUserIndex,
		Word devType, _XINPUT_BATTERY_INFORMATION* pBatteryInformation);
	static Word32 BURGER_API XInputGetKeystroke(
		Word32 dwUserIndex, Word32 dwReserved, _XINPUT_KEYSTROKE* pKeystroke);

	// DDRAW.dll
	static Word BURGER_API DirectDrawCreate(
		const GUID* pGuid, IDirectDraw** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectDrawCreateEx(const GUID* pGuid,
		void** ppOutput, const GUID& rGUID, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectDrawCreateClipper(Word32 uFlags,
		IDirectDrawClipper** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectDrawEnumerateA(
		void* pCallback, void* pContext);
	static Word BURGER_API DirectDrawEnumerateW(
		void* pCallback, void* pContext);
	static Word BURGER_API DirectDrawEnumerateExA(
		void* pCallback, void* pContext, Word32 uFlags);
	static Word BURGER_API DirectDrawEnumerateExW(
		void* pCallback, void* pContext, Word32 uFlags);

	// D3D9.dll or D3DX9_43.dll
	static IDirect3D9* BURGER_API Direct3DCreate9(Word uSDKVersion);
	static int BURGER_API D3DPERF_BeginEvent(Word32 col, const Word16* wszName);
	static int BURGER_API D3DPERF_EndEvent(void);
	static void BURGER_API D3DPERF_SetMarker(Word32 col, const Word16* wszName);
	static void BURGER_API D3DPERF_SetRegion(Word32 col, const Word16* wszName);
	static int BURGER_API D3DPERF_QueryRepeatFrame(void);
	static void BURGER_API D3DPERF_SetOptions(Word32 dwOptions);
	static Word BURGER_API D3DPERF_GetStatus(void);
	static Word BURGER_API D3DXCreateMatrixStack(
		Word uFlags, ID3DXMatrixStack** ppStack);

	// D3D11.dll or DXGI.dll
	static Word BURGER_API D3D11CreateDevice(IDXGIAdapter* pAdapter,
		Word DriverType, HINSTANCE__* Software, Word Flags,
		const Word* pFeatureLevels, Word FeatureLevels, Word SDKVersion,
		ID3D11Device** ppDevice, Word* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext);
	static Word BURGER_API CreateDXGIFactory(
		const GUID* pGuidFactory, void** ppFactory);
	static Word BURGER_API CreateDXGIFactory1(
		const GUID* pGuidFactory, void** ppFactory);
	static Word BURGER_API CreateDXGIFactory2(
		Word uFlags, const GUID* pGuidFactory, void** ppFactory);

	// DSOUND.dll
	static Word BURGER_API DirectSoundCreate(const GUID* pGuidDevice,
		IDirectSound** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectSoundEnumerateA(
		void* pDSEnumCallback, void* pContext);
	static Word BURGER_API DirectSoundEnumerateW(
		void* pDSEnumCallback, void* pContext);
	static Word BURGER_API DirectSoundCaptureCreate(const GUID* pGuidDevice,
		IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectSoundCaptureEnumerateA(
		void* pDSEnumCallback, void* pContext);
	static Word BURGER_API DirectSoundCaptureEnumerateW(
		void* pDSEnumCallback, void* pContext);
	static Word BURGER_API DirectSoundCreate8(const GUID* pGuidDevice,
		IDirectSound8** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectSoundCaptureCreate8(const GUID* pGuidDevice,
		IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API DirectSoundFullDuplexCreate(
		const GUID* pGuidCaptureDevice, const GUID* pGuidRenderDevice,
		const _DSCBUFFERDESC* pDSCBufferDesc,
		const _DSBUFFERDESC* pDSBufferDesc, HWND__* hWnd, Word32 uLevel,
		IDirectSoundFullDuplex** ppDSFD,
		IDirectSoundCaptureBuffer8** ppDSCBuffer8,
		IDirectSoundBuffer8** ppDSBuffer8, IUnknown* pOuter = nullptr);
	static Word BURGER_API GetDeviceID(const GUID* pGuidSrc, GUID* pGuidDest);

	// DPLAYX.dll
	static Word BURGER_API DirectPlayCreate(
		GUID* pGuidSrc, IDirectPlay** ppOutput, IUnknown* pOuter = nullptr);
	static Word BURGER_API CallDirectPlayEnumerate(
		void* pCallback, void* pContext);
	static Word BURGER_API DirectPlayEnumerateA(
		void* pCallback, void* pContext);
	static Word BURGER_API DirectPlayEnumerateW(
		void* pCallback, void* pContext);
	static Word BURGER_API DirectPlayLobbyCreateA(GUID* pGuidSrc,
		IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
		void* pData = nullptr, Word uDataSize = 0);
	static Word BURGER_API DirectPlayLobbyCreateW(GUID* pGuidSrc,
		IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
		void* pData = nullptr, Word uDataSize = 0);

	// RPCRT4.dll
	static Word BURGER_API UuidCreateSequential(GUID* pOutput);

	// WINMM.dll
	static Word32 BURGER_API timeGetTime(void);

	// SHLWAPI.dll
	static Word BURGER_API PathSearchAndQualifyA(
		const char* pszPath, char* pszBuf, Word32 cchBuf);
	static Word BURGER_API PathSearchAndQualifyW(
		const Word16* pszPath, Word16* pszBuf, Word32 cchBuf);

	// VERSION.dll
	static Word BURGER_API VerQueryValueA(
		const void* pBlock, const char* pSubBlock, void** ppBuffer, Word* pLen);
	static Word BURGER_API VerQueryValueW(const void* pBlock,
		const Word16* pSubBlock, void** ppBuffer, Word* pLen);
	static Word BURGER_API GetFileVersionInfoA(
		const char* ptstrFilename, Word32 dwHandle, Word32 dwLen, void* pData);
	static Word BURGER_API GetFileVersionInfoW(const Word16* ptstrFilename,
		Word32 dwHandle, Word32 dwLen, void* pData);
	static Word32 BURGER_API GetFileVersionInfoSizeA(
		const char* ptstrFilename, unsigned long* pdwHandle);
	static Word32 BURGER_API GetFileVersionInfoSizeW(
		const Word16* ptstrFilename, unsigned long* pdwHandle);

	// HID.dll
	static void BURGER_API HidD_GetHidGuid(GUID* pHidGuid);

	// SETUPAPI.dll
	static void* BURGER_API SetupDiGetClassDevsA(const GUID* ClassGuid,
		const char* Enumerator, HWND__* hwndParent, Word32 Flags);
	static void* BURGER_API SetupDiGetClassDevsW(const GUID* ClassGuid,
		const Word16* Enumerator, HWND__* hwndParent, Word32 Flags);
	static Word BURGER_API SetupDiGetDeviceInterfaceDetailA(void* DeviceInfoSet,
		_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
		_SP_DEVICE_INTERFACE_DETAIL_DATA_A* DeviceInterfaceDetailData,
		Word32 DeviceInterfaceDetailDataSize, Word32* RequiredSize,
		_SP_DEVINFO_DATA* DeviceInfoData);
	static Word BURGER_API SetupDiGetDeviceInterfaceDetailW(void* DeviceInfoSet,
		_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
		_SP_DEVICE_INTERFACE_DETAIL_DATA_W* DeviceInterfaceDetailData,
		Word32 DeviceInterfaceDetailDataSize, Word32* RequiredSize,
		_SP_DEVINFO_DATA* DeviceInfoData);
	static Word BURGER_API SetupDiEnumDeviceInterfaces(void* DeviceInfoSet,
		_SP_DEVINFO_DATA* DeviceInfoData, const GUID* InterfaceClassGuid,
		Word32 MemberIndex, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData);
	static Word BURGER_API SetupDiDestroyDeviceInfoList(void* DeviceInfoSet);

	// USER32.dll
	static Word BURGER_API TrackMouseEvent(tagTRACKMOUSEEVENT* pEventTrack);
	static Word BURGER_API GetMonitorInfoA(
		HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo);
	static Word BURGER_API GetMonitorInfoW(
		HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo);
	static HMONITOR__* BURGER_API MonitorFromWindow(
		HWND__* pWindow, Word uFlags);
	static HMONITOR__* BURGER_API MonitorFromRect(
		const tagRECT* pRect, Word uFlags);

	// KERNEL32.dll
	static Word BURGER_API GetSystemWow64DirectoryA(
		char* pBuffer, Word32 uSize);
	static Word BURGER_API GetSystemWow64DirectoryW(
		Word16* pBuffer, Word32 uSize);
	static Word BURGER_API IsDebuggerPresent(void);

	// SHELL32.dll
	static Word BURGER_API SHGetKnownFolderPath(
		const GUID* pGuid, Word32 uFlags, void* hHandle, Word16** ppResult);

	// LoadLibary helper functions
	static HINSTANCE__* BURGER_API LoadLibraryA(const char* pInput);
	static HINSTANCE__* BURGER_API LoadLibraryW(const Word16* pInput);
	static HINSTANCE__* BURGER_API LoadLibraryExA(
		const char* pInput, void* hFile, Word32 uFlags);
	static HINSTANCE__* BURGER_API LoadLibraryExW(
		const Word16* pInput, void* hFile, Word32 uFlags);
	// static eError BURGER_API GetPathFromRegistry(HKEY__* hKey,
	//	const char* pSubKey, const char* pValueName, Filename* pOutput);

	// Window helper functions
	static void ChangeStyle(HWND__* hWindow, Word32 uAddStyle,
		Word32 uAddStyleEx, Word32 uSubStyle, Word32 uSubStyleEx);

	// Helper functions
	static WordPtr BURGER_API ShellExecuteOpen(const char *pFileToOpen);
	static Word BURGER_API LaunchMediaCenter(void);
};

}
#endif

/* END */

#endif
