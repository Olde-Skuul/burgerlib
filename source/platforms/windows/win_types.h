/***************************************

	Forward declarations of windows types

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_TYPES_H__
#define __WIN_TYPES_H__

/* BEGIN */
#if !defined(DOXYGEN)

#if defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

struct HINSTANCE__;
struct HMONITOR__;
struct HWND__;
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
struct IDirectInputA;
struct IDirectInputW;
struct IDirectInput8A;
struct IDirectInput8W;
struct IDirectInputDevice2A;
struct IDirectInputDevice2W;
struct IDirectInputDevice8A;
struct IDirectInputDevice8W;
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
struct ID3DXEffect;
struct ID3DXEffectPool;
struct ID3DXMatrixStack;
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
struct IXAudio2;
struct IXAudio2Voice;
struct IXAudio2SourceVoice;
struct IXAudio2SubmixVoice;
struct IXAudio2MasteringVoice;
struct IXAudio2EngineCallback;
struct IXAudio2VoiceCallback;
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
struct IUnknown;
struct tagMONITORINFO;
struct tagRECT;
struct tagTRACKMOUSEEVENT;
struct _DSBUFFERDESC;
struct _DSCBUFFERDESC;
struct _SP_DEVICE_INTERFACE_DATA;
struct _SP_DEVINFO_DATA;
struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A;
struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W;
struct _XINPUT_BATTERY_INFORMATION;
struct _XINPUT_CAPABILITIES;
struct _XINPUT_KEYSTROKE;
struct _XINPUT_STATE;
struct _XINPUT_VIBRATION;
#endif
/* END */

#endif
