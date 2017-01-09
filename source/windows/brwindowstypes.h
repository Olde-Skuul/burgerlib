/***************************************

	Typedefs specific to Windows

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

/* BEGIN */
#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#if defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

struct IUnknown;
struct HBITMAP__;
struct HDC__;
struct HGLRC__;
struct HHOOK__;
struct HICON__;
struct HINSTANCE__;
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

namespace Burger {

struct CRITICAL_SECTION {
	_RTL_CRITICAL_SECTION_DEBUG *DebugInfo;
	Word32 LockCount;
	Word32 RecursionCount;
	void *OwningThread;
	void *LockSemaphore;
	WordPtr SpinCount;
};

struct tagSTICKYKEYS {
	Word cbSize;
	Word dwFlags;
};

struct tagTOGGLEKEYS {
	Word cbSize;
	Word dwFlags;
};

struct tagFILTERKEYS {
	Word cbSize;
	Word dwFlags;
	Word iWaitMSec;
	Word iDelayMSec;
	Word iRepeatMSec;
	Word iBounceMSec;
};

struct tagTRACKMOUSEEVENT {
	Word cbSize;
	Word dwFlags;
	HWND__ *hwndTrack;
	Word dwHoverTime;
};

}
#endif
/* END */

#endif
