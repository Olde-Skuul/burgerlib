/***************************************

	Typedefs specific to Windows

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

struct Burger_CRITICAL_SECTION {
	_RTL_CRITICAL_SECTION_DEBUG* DebugInfo;
	uint32_t LockCount;
	uint32_t RecursionCount;
	void* OwningThread;
	void* LockSemaphore;
	uintptr_t SpinCount;
};

struct Burger_tagSTICKYKEYS {
	uint_t cbSize;
	uint_t dwFlags;
};

struct Burger_tagTOGGLEKEYS {
	uint_t cbSize;
	uint_t dwFlags;
};

struct Burger_tagFILTERKEYS {
	uint_t cbSize;
	uint_t dwFlags;
	uint_t iWaitMSec;
	uint_t iDelayMSec;
	uint_t iRepeatMSec;
	uint_t iBounceMSec;
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
	uint8_t m_bInstancesTested[DLL_COUNT];
	/** Flags to determine if a function was tested for loading \windowsonly
	 */
	uint8_t m_bFunctionsTested[CALL_COUNT];

	/** \ref eVersion as set by \ref TestVersion(). */
	uint8_t m_bVersionFlags;

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

	static uint_t BURGER_API TestVersion(void) BURGER_NOEXCEPT;

	static BURGER_INLINE uint_t IsWin95orWin98(void) BURGER_NOEXCEPT
	{
		return TestVersion() & VERSION_9598;
	}
	static BURGER_INLINE uint_t IsWinXPOrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_XPORGREATER) != 0;
	}
	static BURGER_INLINE uint_t IsVistaOrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_VISTAORGREATER) != 0;
	}
	static BURGER_INLINE uint_t IsWin7OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_7ORGREATER) != 0;
	}
	static BURGER_INLINE uint_t IsWin8OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_8ORGREATER) != 0;
	}
	static BURGER_INLINE uint_t IsWin10OrGreater(void) BURGER_NOEXCEPT
	{
		return (TestVersion() & VERSION_10ORGREATER) != 0;
	}

	static HINSTANCE__* BURGER_API LoadLibraryIndex(eDLLIndex eIndex) BURGER_NOEXCEPT;
	static void* BURGER_API LoadFunctionIndex(eCallIndex eIndex) BURGER_NOEXCEPT;

	static BURGER_INLINE uint_t IsDirectInputPresent(void)
	{
		return LoadLibraryIndex(DINPUT_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsDirectInput8Present(void)
	{
		return LoadLibraryIndex(DINPUT8_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsXInputPresent(void)
	{
		return LoadLibraryIndex(XINPUT1_4_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsDirectDrawPresent(void)
	{
		return LoadLibraryIndex(DDRAW_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsD3D9Present(void)
	{
		return LoadLibraryIndex(D3D9_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsDirectSoundPresent(void)
	{
		return LoadLibraryIndex(DSOUND_DLL) != nullptr;
	}
	static BURGER_INLINE uint_t IsDirectPlayPresent(void)
	{
		return LoadLibraryIndex(DPLAYX_DLL) != nullptr;
	}
	static uint_t BURGER_API IsXAudio2Present(void);

	// DINPUT.dll and DINPUT8.dll
	static uint_t BURGER_API DirectInputCreateA(HINSTANCE__* hInst,
		uint32_t uVersion, IDirectInputA** ppOutput,
		IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectInputCreateW(HINSTANCE__* hInst,
		uint32_t uVersion, IDirectInputW** ppOutput,
		IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectInput8Create(HINSTANCE__* hInst,
		uint32_t uVersion, const GUID& rGUID, void** ppOutput,
		IUnknown* pOuter = nullptr);
	static IDirectInputW* BURGER_API GetDirectInputSingleton(
		uint_t uVersion = 0x700);
	static IDirectInput8W* BURGER_API GetDirectInput8Singleton(void);

	// XINPUT1_4.dll or XINPUT1_3.dll
	static uint32_t BURGER_API XInputGetState(
		uint32_t dwUserIndex, _XINPUT_STATE* pState);
	static uint32_t BURGER_API XInputSetState(
		uint32_t dwUserIndex, _XINPUT_VIBRATION* pVibration);
	static uint32_t BURGER_API XInputGetCapabilities(uint32_t dwUserIndex,
		uint32_t dwFlags, _XINPUT_CAPABILITIES* pCapabilities);
	static uint32_t BURGER_API XInputGetDSoundAudioDeviceGuids(
		uint32_t dwUserIndex, GUID* pDSoundRenderGuid,
		GUID* pDSoundCaptureGuid);
	static void BURGER_API XInputEnable(uint_t bEnable);
	static uint32_t BURGER_API XInputGetAudioDeviceIds(uint32_t dwUserIndex,
		uint16_t* pRenderDeviceId, uint_t* pRenderCount,
		uint16_t* pCaptureDeviceId, uint_t* pCaptureCount);
	static uint32_t BURGER_API XInputGetBatteryInformation(uint32_t dwUserIndex,
		uint_t devType, _XINPUT_BATTERY_INFORMATION* pBatteryInformation);
	static uint32_t BURGER_API XInputGetKeystroke(uint32_t dwUserIndex,
		uint32_t dwReserved, _XINPUT_KEYSTROKE* pKeystroke);

	// DDRAW.dll
	static uint_t BURGER_API DirectDrawCreate(
		const GUID* pGuid, IDirectDraw** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectDrawCreateEx(const GUID* pGuid,
		void** ppOutput, const GUID& rGUID, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectDrawCreateClipper(uint32_t uFlags,
		IDirectDrawClipper** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectDrawEnumerateA(
		void* pCallback, void* pContext);
	static uint_t BURGER_API DirectDrawEnumerateW(
		void* pCallback, void* pContext);
	static uint_t BURGER_API DirectDrawEnumerateExA(
		void* pCallback, void* pContext, uint32_t uFlags);
	static uint_t BURGER_API DirectDrawEnumerateExW(
		void* pCallback, void* pContext, uint32_t uFlags);

	// D3D9.dll or D3DX9_43.dll
	static IDirect3D9* BURGER_API Direct3DCreate9(uint_t uSDKVersion);
	static int BURGER_API D3DPERF_BeginEvent(
		uint32_t col, const uint16_t* wszName);
	static int BURGER_API D3DPERF_EndEvent(void);
	static void BURGER_API D3DPERF_SetMarker(
		uint32_t col, const uint16_t* wszName);
	static void BURGER_API D3DPERF_SetRegion(
		uint32_t col, const uint16_t* wszName);
	static int BURGER_API D3DPERF_QueryRepeatFrame(void);
	static void BURGER_API D3DPERF_SetOptions(uint32_t dwOptions);
	static uint_t BURGER_API D3DPERF_GetStatus(void);
	static uint_t BURGER_API D3DXCreateMatrixStack(
		uint_t uFlags, ID3DXMatrixStack** ppStack);

	// D3D11.dll or DXGI.dll
	static uint_t BURGER_API D3D11CreateDevice(IDXGIAdapter* pAdapter,
		uint_t DriverType, HINSTANCE__* Software, uint_t Flags,
		const uint_t* pFeatureLevels, uint_t FeatureLevels, uint_t SDKVersion,
		ID3D11Device** ppDevice, uint_t* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext);
	static uint_t BURGER_API CreateDXGIFactory(
		const GUID* pGuidFactory, void** ppFactory);
	static uint_t BURGER_API CreateDXGIFactory1(
		const GUID* pGuidFactory, void** ppFactory);
	static uint_t BURGER_API CreateDXGIFactory2(
		uint_t uFlags, const GUID* pGuidFactory, void** ppFactory);

	// DSOUND.dll
	static uint_t BURGER_API DirectSoundCreate(const GUID* pGuidDevice,
		IDirectSound** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectSoundEnumerateA(
		void* pDSEnumCallback, void* pContext);
	static uint_t BURGER_API DirectSoundEnumerateW(
		void* pDSEnumCallback, void* pContext);
	static uint_t BURGER_API DirectSoundCaptureCreate(const GUID* pGuidDevice,
		IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectSoundCaptureEnumerateA(
		void* pDSEnumCallback, void* pContext);
	static uint_t BURGER_API DirectSoundCaptureEnumerateW(
		void* pDSEnumCallback, void* pContext);
	static uint_t BURGER_API DirectSoundCreate8(const GUID* pGuidDevice,
		IDirectSound8** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectSoundCaptureCreate8(const GUID* pGuidDevice,
		IDirectSoundCapture** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API DirectSoundFullDuplexCreate(
		const GUID* pGuidCaptureDevice, const GUID* pGuidRenderDevice,
		const _DSCBUFFERDESC* pDSCBufferDesc,
		const _DSBUFFERDESC* pDSBufferDesc, HWND__* hWnd, uint32_t uLevel,
		IDirectSoundFullDuplex** ppDSFD,
		IDirectSoundCaptureBuffer8** ppDSCBuffer8,
		IDirectSoundBuffer8** ppDSBuffer8, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API GetDeviceID(const GUID* pGuidSrc, GUID* pGuidDest);

	// DPLAYX.dll
	static uint_t BURGER_API DirectPlayCreate(
		GUID* pGuidSrc, IDirectPlay** ppOutput, IUnknown* pOuter = nullptr);
	static uint_t BURGER_API CallDirectPlayEnumerate(
		void* pCallback, void* pContext);
	static uint_t BURGER_API DirectPlayEnumerateA(
		void* pCallback, void* pContext);
	static uint_t BURGER_API DirectPlayEnumerateW(
		void* pCallback, void* pContext);
	static uint_t BURGER_API DirectPlayLobbyCreateA(GUID* pGuidSrc,
		IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
		void* pData = nullptr, uint_t uDataSize = 0);
	static uint_t BURGER_API DirectPlayLobbyCreateW(GUID* pGuidSrc,
		IDirectPlayLobby** ppOutput, IUnknown* pOuter = nullptr,
		void* pData = nullptr, uint_t uDataSize = 0);

	// RPCRT4.dll
	static uint_t BURGER_API UuidCreateSequential(GUID* pOutput);

	// WINMM.dll
	static uint32_t BURGER_API timeGetTime(void) BURGER_NOEXCEPT;

	// SHLWAPI.dll
	static uint_t BURGER_API PathSearchAndQualifyA(
		const char* pszPath, char* pszBuf, uint32_t cchBuf);
	static uint_t BURGER_API PathSearchAndQualifyW(
		const uint16_t* pszPath, uint16_t* pszBuf, uint32_t cchBuf);

	// VERSION.dll
	static uint_t BURGER_API VerQueryValueA(const void* pBlock,
		const char* pSubBlock, void** ppBuffer, uint_t* pLen);
	static uint_t BURGER_API VerQueryValueW(const void* pBlock,
		const uint16_t* pSubBlock, void** ppBuffer, uint_t* pLen);
	static uint_t BURGER_API GetFileVersionInfoA(const char* ptstrFilename,
		uint32_t dwHandle, uint32_t dwLen, void* pData);
	static uint_t BURGER_API GetFileVersionInfoW(const uint16_t* ptstrFilename,
		uint32_t dwHandle, uint32_t dwLen, void* pData);
	static uint32_t BURGER_API GetFileVersionInfoSizeA(
		const char* ptstrFilename, unsigned long* pdwHandle);
	static uint32_t BURGER_API GetFileVersionInfoSizeW(
		const uint16_t* ptstrFilename, unsigned long* pdwHandle);

	// HID.dll
	static void BURGER_API HidD_GetHidGuid(GUID* pHidGuid);

	// SETUPAPI.dll
	static void* BURGER_API SetupDiGetClassDevsA(const GUID* ClassGuid,
		const char* Enumerator, HWND__* hwndParent, uint32_t Flags);
	static void* BURGER_API SetupDiGetClassDevsW(const GUID* ClassGuid,
		const uint16_t* Enumerator, HWND__* hwndParent, uint32_t Flags);
	static uint_t BURGER_API SetupDiGetDeviceInterfaceDetailA(
		void* DeviceInfoSet, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
		_SP_DEVICE_INTERFACE_DETAIL_DATA_A* DeviceInterfaceDetailData,
		uint32_t DeviceInterfaceDetailDataSize, uint32_t* RequiredSize,
		_SP_DEVINFO_DATA* DeviceInfoData);
	static uint_t BURGER_API SetupDiGetDeviceInterfaceDetailW(
		void* DeviceInfoSet, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
		_SP_DEVICE_INTERFACE_DETAIL_DATA_W* DeviceInterfaceDetailData,
		uint32_t DeviceInterfaceDetailDataSize, uint32_t* RequiredSize,
		_SP_DEVINFO_DATA* DeviceInfoData);
	static uint_t BURGER_API SetupDiEnumDeviceInterfaces(void* DeviceInfoSet,
		_SP_DEVINFO_DATA* DeviceInfoData, const GUID* InterfaceClassGuid,
		uint32_t MemberIndex, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData);
	static uint_t BURGER_API SetupDiDestroyDeviceInfoList(void* DeviceInfoSet);

	// USER32.dll
	static uint_t BURGER_API TrackMouseEvent(tagTRACKMOUSEEVENT* pEventTrack);
	static uint_t BURGER_API GetMonitorInfoA(
		HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo);
	static uint_t BURGER_API GetMonitorInfoW(
		HMONITOR__* hMonitor, tagMONITORINFO* pMonitorInfo);
	static HMONITOR__* BURGER_API MonitorFromWindow(
		HWND__* pWindow, uint_t uFlags);
	static HMONITOR__* BURGER_API MonitorFromRect(
		const tagRECT* pRect, uint_t uFlags);

	// KERNEL32.dll
	static uint_t BURGER_API GetSystemWow64DirectoryA(
		char* pBuffer, uint32_t uSize);
	static uint_t BURGER_API GetSystemWow64DirectoryW(
		uint16_t* pBuffer, uint32_t uSize);
	static uint_t BURGER_API IsDebuggerPresent(void) BURGER_NOEXCEPT;

	// SHELL32.dll
	static uint_t BURGER_API SHGetKnownFolderPath(const GUID* pGuid,
		uint32_t uFlags, void* hHandle, uint16_t** ppResult) BURGER_NOEXCEPT;

	// LoadLibary helper functions
	static HINSTANCE__* BURGER_API LoadLibraryA(const char* pInput);
	static HINSTANCE__* BURGER_API LoadLibraryW(const uint16_t* pInput);
	static HINSTANCE__* BURGER_API LoadLibraryExA(
		const char* pInput, void* hFile, uint32_t uFlags);
	static HINSTANCE__* BURGER_API LoadLibraryExW(
		const uint16_t* pInput, void* hFile, uint32_t uFlags);
	// static eError BURGER_API GetPathFromRegistry(HKEY__* hKey,
	//	const char* pSubKey, const char* pValueName, Filename* pOutput);

	// Window helper functions
	static void ChangeStyle(HWND__* hWindow, uint32_t uAddStyle,
		uint32_t uAddStyleEx, uint32_t uSubStyle, uint32_t uSubStyleEx);

	// Helper functions
	static uintptr_t BURGER_API ShellExecuteOpen(const char* pFileToOpen);
	static uint_t BURGER_API LaunchMediaCenter(void);
};

}
#endif

/* END */

#endif
