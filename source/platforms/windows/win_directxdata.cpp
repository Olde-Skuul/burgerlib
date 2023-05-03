/***************************************

	DirectX GUIDs that are needed for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_directxdata.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)
#define DIRECTINPUT_VERSION 0x800
#define DIRECT3D_VERSION 0x900
#define DIRECTDRAW_VERSION 0x700

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

// Windows.h always comes first
#include <windows.h>

#include <d3d9.h>
#include <dinput.h>

// Needed for Code Warrior
#if !defined(DIDFT_OPTIONAL) && !defined(DOXYGEN)
#define DIDFT_OPTIONAL 0x80000000
#endif

/***************************************

	These are Windows GUIDs that are used to communicate
	with DirectX's COM API. They are here so dxguid.lib, dinput.lib
	etc do not need to be linked in. They also retain backward
	compatibility to older, obsolete APIs.

***************************************/

extern "C" const GUID DECLSPEC_SELECTANY IID_IDirect3DTexture = {0x2CDCD9E0,
	0x25A0, 0x11CF, {0xA3, 0x1A, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirect3D = {0x3BBA0080, 0x2421,
	0x11CF, {0xA3, 0x1A, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirect3DHALDevice = {0x84E63dE0,
	0x46AA, 0x11CF, {0x81, 0x6F, 0x00, 0x00, 0xC0, 0x20, 0x15, 0x6E}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectDraw2 = {0xB3A6F3E0, 0x2B43,
	0x11CF, {0xA2, 0xDE, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectDrawGammaControl = {
	0x69C11C3E, 0xB46B, 0x11D1,
	{0xAD, 0x7A, 0x00, 0xC0, 0x4F, 0xC2, 0x9B, 0x4E}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectDrawSurface3 = {0xDA044E00,
	0x69B2, 0x11D0, {0xA1, 0xD5, 0x00, 0xAA, 0x00, 0xB8, 0xDF, 0xBB}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectDraw4 = {0x9c59509a, 0x39bd,
	0x11d1, {0x8c, 0x4a, 0x00, 0xc0, 0x4f, 0xd9, 0x30, 0xc5}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectDraw7 = {0x15e65ec0, 0x3b9c,
	0x11d2, {0xb9, 0x2f, 0x00, 0x60, 0x97, 0x97, 0xea, 0x5b}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectInput8W = {0xBF798031,
	0x483A, 0x4DA2, {0xAA, 0x99, 0x5D, 0x64, 0xED, 0x36, 0x97, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_SysMouse = {0x6F1D2B60, 0xD5A0,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_SysKeyboard = {0x6F1D2B61, 0xD5A0,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_Joystick = {0x6F1D2B70, 0xD5A0,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_XAxis = {0xA36D02E0, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_YAxis = {0xA36D02E1, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_ZAxis = {0xA36D02E2, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_RxAxis = {0xA36D02F4, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_RyAxis = {0xA36D02F5, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_RzAxis = {0xA36D02E3, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_Slider = {0xA36D02E4, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_Button = {0xA36D02F0, 0xC9F3,
	0x11CF, {0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_Key = {0x55728220, 0xD33C, 0x11CF,
	{0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_POV = {0xA36D02F2, 0xC9F3, 0x11CF,
	{0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectInputEffectDriver = {
	0x02538130, 0x898F, 0x11D0,
	{0x9A, 0xD0, 0x00, 0xA0, 0xC9, 0xA0, 0x6E, 0x35}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectInputJoyConfig = {
	0x1DE12AB1, 0xC9F5, 0x11CF,
	{0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectInputPIDDriver = {
	0xEEC6993A, 0xB3FD, 0x11D2,
	{0xA9, 0x16, 0x00, 0xC0, 0x4F, 0xB9, 0x86, 0x38}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectInputJoyConfig8 = {
	0xeb0d7dfa, 0x1990, 0x4f27,
	{0xb4, 0xd6, 0xed, 0xf2, 0xee, 0xc4, 0xa4, 0x4c}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DxDiagProvider = {0xA65B8071,
	0x3BFE, 0x4213, {0x9A, 0x5B, 0x49, 0x1D, 0xA4, 0x46, 0x1C, 0xA7}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDxDiagProvider = {0x9C6B4CB0,
	0x23F8, 0x49CC, {0xA3, 0xED, 0x45, 0xA5, 0x50, 0x00, 0xA6, 0xD2}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDxDiagContainer = {0x7D0F462F,
	0x4064, 0x4862, {0xBC, 0x7F, 0x93, 0x3E, 0x50, 0x58, 0xC1, 0x0F}};

// DirectSound8 GUIDs

extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectSound = {
	0x47d4d946, 0x62e8, 0x11cf, {0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectSound8 = {0x3901cc3f,
	0x84b5, 0x4fa4, {0xba, 0x35, 0xaa, 0x81, 0x72, 0xb8, 0xa0, 0x9b}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectSoundCapture = {
	0xb0210780, 0x89cd, 0x11d0, {0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectSoundCapture8 = {
	0xe4bcac13, 0x7f99, 0x4908,
	{0x9a, 0x8e, 0x74, 0xe3, 0xbf, 0x24, 0xb6, 0xe1}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectSoundFullDuplex = {
	0xfea4300c, 0x7959, 0x4147,
	{0xb2, 0x6a, 0x23, 0x77, 0xb9, 0xe7, 0xa9, 0x1d}};
extern "C" const GUID DECLSPEC_SELECTANY DSDEVID_DefaultPlayback = {0xdef00000,
	0x9c6d, 0x47ed, {0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03}};
extern "C" const GUID DECLSPEC_SELECTANY DSDEVID_DefaultCapture = {0xdef00001,
	0x9c6d, 0x47ed, {0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03}};
extern "C" const GUID DECLSPEC_SELECTANY DSDEVID_DefaultVoicePlayback = {
	0xdef00002, 0x9c6d, 0x47ed,
	{0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03}};
extern "C" const GUID DECLSPEC_SELECTANY DSDEVID_DefaultVoiceCapture = {
	0xdef00003, 0x9c6d, 0x47ed,
	{0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IReferenceClock = {0x56a86897,
	0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSound = {0x279AFA83, 0x4981,
	0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSound8 = {0xC50A7E93,
	0xF395, 0x4834, {0x9E, 0xF6, 0x7F, 0xA9, 0x9D, 0xE5, 0x09, 0x66}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundBuffer = {0x279AFA85,
	0x4981, 0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundBuffer8 = {0x6825a449,
	0x7524, 0x4d82, {0x92, 0x0f, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_All_Objects = {0xaa114de5, 0xc262,
	0x4169, {0xa1, 0xc8, 0x23, 0xd6, 0x98, 0xcc, 0x73, 0xb5}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSound3DListener = {
	0x279AFA84, 0x4981, 0x11CE,
	{0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSound3DBuffer = {0x279AFA86,
	0x4981, 0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundCapture = {
	0xb0210781, 0x89cd, 0x11d0, {0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundCaptureBuffer = {
	0xb0210782, 0x89cd, 0x11d0, {0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundCaptureBuffer8 = {
	0x990df4, 0xdbb, 0x4872, {0x83, 0x3e, 0x6d, 0x30, 0x3e, 0x80, 0xae, 0xb6}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundNotify = {
	0xb0210783, 0x89cd, 0x11d0, {0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IKsPropertySet = {0x31efac30,
	0x515c, 0x11d0, {0xa9, 0xaa, 0x00, 0xaa, 0x00, 0x61, 0xbe, 0x93}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXGargle = {0xd616f352,
	0xd622, 0x11ce, {0xaa, 0xc5, 0x00, 0x20, 0xaf, 0x0b, 0x99, 0xa3}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXChorus = {0x880842e3,
	0x145f, 0x43e6, {0xa9, 0x34, 0xa7, 0x18, 0x06, 0xe5, 0x05, 0x47}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXFlanger = {
	0x903e9878, 0x2c92, 0x4072,
	{0x9b, 0x2c, 0xea, 0x68, 0xf5, 0x39, 0x67, 0x83}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXEcho = {0x8bd28edf,
	0x50db, 0x4e92, {0xa2, 0xbd, 0x44, 0x54, 0x88, 0xd1, 0xed, 0x42}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXDistortion = {
	0x8ecf4326, 0x455f, 0x4d8b,
	{0xbd, 0xa9, 0x8d, 0x5d, 0x3e, 0x9e, 0x3e, 0x0b}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXCompressor = {
	0x4bbd1154, 0x62f6, 0x4e2c,
	{0xa1, 0x5c, 0xd3, 0xb6, 0xc4, 0x17, 0xf7, 0xa0}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXParamEq = {
	0xc03ca9fe, 0xfe90, 0x4204,
	{0x80, 0x78, 0x82, 0x33, 0x4c, 0xd1, 0x77, 0xda}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXI3DL2Reverb = {
	0x4b166a6a, 0x0d66, 0x43f3,
	{0x80, 0xe3, 0xee, 0x62, 0x80, 0xde, 0xe1, 0xa4}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFXWavesReverb = {
	0x46858c3a, 0x0dc6, 0x45e3,
	{0xb7, 0x60, 0xd4, 0xee, 0xf1, 0x6c, 0xb3, 0x25}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundCaptureFXAec = {
	0xad74143d, 0x903d, 0x4ab7,
	{0x80, 0x66, 0x28, 0xd3, 0x63, 0x03, 0x6d, 0x65}};
extern "C" const GUID DECLSPEC_SELECTANY
	IID_IDirectSoundCaptureFXNoiseSuppress = {0xed311e41, 0xfbae, 0x4175,
		{0x96, 0x25, 0xcd, 0x8, 0x54, 0xf6, 0x93, 0xca}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectSoundFullDuplex = {
	0xedcb4c7a, 0xdaab, 0x4216,
	{0xa4, 0x2e, 0x6c, 0x50, 0x59, 0x6d, 0xdc, 0x1d}};
extern "C" const GUID DECLSPEC_SELECTANY DS3DALG_NO_VIRTUALIZATION = {
	0xc241333f, 0x1c1b, 0x11d2,
	{0x94, 0xf5, 0x0, 0xc0, 0x4f, 0xc2, 0x8a, 0xca}};
extern "C" const GUID DECLSPEC_SELECTANY DS3DALG_HRTF_FULL = {0xc2413340,
	0x1c1b, 0x11d2, {0x94, 0xf5, 0x0, 0xc0, 0x4f, 0xc2, 0x8a, 0xca}};
extern "C" const GUID DECLSPEC_SELECTANY DS3DALG_HRTF_LIGHT = {0xc2413342,
	0x1c1b, 0x11d2, {0x94, 0xf5, 0x0, 0xc0, 0x4f, 0xc2, 0x8a, 0xca}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_GARGLE = {
	0xdafd8210, 0x5711, 0x4b91,
	{0x9f, 0xe3, 0xf7, 0x5b, 0x7a, 0xe2, 0x79, 0xbf}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_CHORUS = {
	0xefe6629c, 0x81f7, 0x4281,
	{0xbd, 0x91, 0xc9, 0xd6, 0x04, 0xa9, 0x5a, 0xf6}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_FLANGER = {
	0xefca3d92, 0xdfd8, 0x4672,
	{0xa6, 0x03, 0x74, 0x20, 0x89, 0x4b, 0xad, 0x98}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_ECHO = {0xef3e932c,
	0xd40b, 0x4f51, {0x8c, 0xcf, 0x3f, 0x98, 0xf1, 0xb2, 0x9d, 0x5d}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_DISTORTION = {
	0xef114c90, 0xcd1d, 0x484e,
	{0x96, 0xe5, 0x09, 0xcf, 0xaf, 0x91, 0x2a, 0x21}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_COMPRESSOR = {
	0xef011f79, 0x4000, 0x406d,
	{0x87, 0xaf, 0xbf, 0xfb, 0x3f, 0xc3, 0x9d, 0x57}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_PARAMEQ = {
	0x120ced89, 0x3bf4, 0x4173,
	{0xa1, 0x32, 0x3c, 0xb4, 0x06, 0xcf, 0x32, 0x31}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_STANDARD_I3DL2REVERB = {
	0xef985e71, 0xd5c7, 0x42d4,
	{0xba, 0x4d, 0x2d, 0x07, 0x3e, 0x2e, 0x96, 0xf4}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSFX_WAVES_REVERB = {0x87fc0268,
	0x9a55, 0x4360, {0x95, 0xaa, 0x00, 0x4a, 0x1d, 0x9d, 0xe2, 0x6c}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_CLASS_AEC = {0xBF963D80,
	0xC559, 0x11D0, {0x8A, 0x2B, 0x00, 0xA0, 0xC9, 0x25, 0x5A, 0xC1}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_MS_AEC = {0xcdebb919,
	0x379a, 0x488a, {0x87, 0x65, 0xf5, 0x3c, 0xfd, 0x36, 0xde, 0x40}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_SYSTEM_AEC = {0x1c22c56d,
	0x9879, 0x4f5b, {0xa3, 0x89, 0x27, 0x99, 0x6d, 0xdc, 0x28, 0x10}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_CLASS_NS = {0xe07f903f,
	0x62fd, 0x4e60, {0x8c, 0xdd, 0xde, 0xa7, 0x23, 0x66, 0x65, 0xb5}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_MS_NS = {0x11c5c73b, 0x66e9,
	0x4ba1, {0xa0, 0xba, 0xe8, 0x14, 0xc6, 0xee, 0xd9, 0x2d}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DSCFX_SYSTEM_NS = {0x5ab0882e,
	0x7274, 0x4516, {0x87, 0x7d, 0x4e, 0xee, 0x99, 0xba, 0x4f, 0xd0}};

// GUIDs from xaudio2.h
// XAudio 2.7 (June 2010 SDK)
extern "C" const GUID DECLSPEC_SELECTANY CLSID_XAudio2 = {0x5a508685, 0xa254,
	0x4fba, {0x9b, 0x82, 0x9a, 0x24, 0xb0, 0x03, 0x06, 0xaf}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_XAudio2_Debug = {0xdb05ea35,
	0x0329, 0x4d4b, {0xa5, 0x3a, 0x6d, 0xea, 0xd0, 0x3d, 0x38, 0x52}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IXAudio2 = {0x8bcf1f58, 0x9fe7,
	0x4583, {0x8a, 0xc6, 0xe2, 0xad, 0xc4, 0x65, 0xc8, 0xbb}};

// GUIDs from xaudio2fx.h
// XAudio 2.7 (June 2010 SDK)
extern "C" const GUID DECLSPEC_SELECTANY CLSID_AudioVolumeMeter = {0xcac1105f,
	0x619b, 0x4d04, {0x83, 0x1a, 0x44, 0xe1, 0xcb, 0xf1, 0x2d, 0x57}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_AudioVolumeMeter_Debug = {
	0x2d9a0f9c, 0xe67b, 0x4b24,
	{0xab, 0x44, 0x92, 0xb3, 0xe7, 0x70, 0xc0, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_AudioReverb = {0x6a93130e,
	0x1d53, 0x41d1, {0xa9, 0xcf, 0xe7, 0x58, 0x80, 0x0b, 0xb1, 0x79}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_AudioReverb_Debug = {0xc4f82dd4,
	0xcb4e, 0x4ce1, {0x8b, 0xdb, 0xee, 0x32, 0xd4, 0x19, 0x82, 0x69}};

// Updated versions
// XAudio 2.8 (Windows 8)
extern "C" {
const GUID IID_IXAudio2_2_8 = {0x60d8dac8, 0x5aa1, 0x4e8e,
	{0x97, 0xb5, 0x2f, 0x5e, 0x28, 0x83, 0xd4, 0x84}};
const GUID CLSID_AudioVolumeMeter_2_8 = {0x4FC3B166, 0x972A, 0x40CF,
	{0x37, 0xBC, 0x7D, 0xB0, 0x3D, 0xB2, 0xFB, 0xA3}};
const GUID CLSID_AudioReverb_2_8 = {0xC2633B16, 0x471B, 0x4498,
	{0xC5, 0xB8, 0x4F, 0x09, 0x59, 0xE2, 0xEC, 0x09}};

// XAudio 2.9 (Windows 10)
const GUID IID_IXAudio2_2_9 = {0x2B02E3CF, 0x2E0B, 0x4ec3,
	{0x45, 0xBE, 0x1B, 0x2A, 0x3F, 0xE7, 0x21, 0x0D}};
}

// GUIDs from dplay.h

extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay = {0x5454e9a0, 0xdb65,
	0x11ce, {0x92, 0x1c, 0x00, 0xaa, 0x00, 0x6c, 0x49, 0x72}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay2 = {
	0x2b74f7c0, 0x9154, 0x11cf, {0xa9, 0xcd, 0x0, 0xaa, 0x0, 0x68, 0x86, 0xe3}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay2A = {
	0x9d460580, 0xa822, 0x11cf, {0x96, 0xc, 0x0, 0x80, 0xc7, 0x53, 0x4e, 0x82}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay3 = {
	0x133efe40, 0x32dc, 0x11d0, {0x9c, 0xfb, 0x0, 0xa0, 0xc9, 0xa, 0x43, 0xcb}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay3A = {
	0x133efe41, 0x32dc, 0x11d0, {0x9c, 0xfb, 0x0, 0xa0, 0xc9, 0xa, 0x43, 0xcb}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay4 = {
	0xab1c530, 0x4745, 0x11d1, {0xa7, 0xa1, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay4A = {
	0xab1c531, 0x4745, 0x11d1, {0xa7, 0xa1, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay = {
	0xd1eb6d20, 0x8923, 0x11d0, {0x9d, 0x97, 0x0, 0xa0, 0xc9, 0xa, 0x43, 0xcb}};
extern "C" const GUID DECLSPEC_SELECTANY DPSPGUID_IPX = {
	0x685bc400, 0x9d2c, 0x11cf, {0xa9, 0xcd, 0x0, 0xaa, 0x0, 0x68, 0x86, 0xe3}};
extern "C" const GUID DECLSPEC_SELECTANY DPSPGUID_TCPIP = {
	0x36E95EE0, 0x8577, 0x11cf, {0x96, 0xc, 0x0, 0x80, 0xc7, 0x53, 0x4e, 0x82}};
extern "C" const GUID DECLSPEC_SELECTANY DPSPGUID_SERIAL = {
	0xf1d6860, 0x88d9, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPSPGUID_MODEM = {
	0x44eaa760, 0xcb68, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};

// GUIDs from dplay8.h

extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8Client = {0x743f1dc6,
	0x5aba, 0x429f, {0x8b, 0xdf, 0xc5, 0x4d, 0x3, 0x25, 0x3d, 0xc2}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8Server = {0xda825e1b,
	0x6830, 0x43d7, {0x83, 0x5d, 0xb, 0x5a, 0xd8, 0x29, 0x56, 0xa2}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8Peer = {0x286f484d,
	0x375e, 0x4458, {0xa2, 0x72, 0xb1, 0x38, 0xe2, 0xf8, 0xa, 0x6a}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8ThreadPool = {
	0xfc47060e, 0x6153, 0x4b34,
	{0xb9, 0x75, 0x8e, 0x41, 0x21, 0xeb, 0x7f, 0x3c}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8NATResolver = {
	0xe4c1d9a2, 0xcbf7, 0x48bd,
	{0x9a, 0x69, 0x34, 0xa5, 0x5e, 0xd, 0x89, 0x41}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8Client = {0x5102dacd,
	0x241b, 0x11d3, {0xae, 0xa7, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8Server = {0x5102dace,
	0x241b, 0x11d3, {0xae, 0xa7, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8Peer = {0x5102dacf,
	0x241b, 0x11d3, {0xae, 0xa7, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8ThreadPool = {
	0xd22ee73, 0x4a46, 0x4a0d, {0x89, 0xb2, 0x4, 0x5b, 0x4d, 0x66, 0x64, 0x25}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8NATResolver = {
	0xa9e213f2, 0x9a60, 0x486f,
	{0xbf, 0x3b, 0x53, 0x40, 0x8b, 0x6d, 0x1c, 0xbb}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DP8SP_IPX = {
	0x53934290, 0x628d, 0x11d2, {0xae, 0xf, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DP8SP_MODEM = {
	0x6d4a3650, 0x628d, 0x11d2, {0xae, 0xf, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DP8SP_SERIAL = {
	0x743b5d60, 0x628d, 0x11d2, {0xae, 0xf, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DP8SP_TCPIP = {
	0xebfe7ba0, 0x628d, 0x11d2, {0xae, 0xf, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DP8SP_BLUETOOTH = {0x995513af,
	0x3027, 0x4b9a, {0x95, 0x6e, 0xc7, 0x72, 0xb3, 0xf7, 0x80, 0x6}};

// GUIDs from dplobby.h

extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobby = {
	0xaf465c71, 0x9588, 0x11cf, {0xa0, 0x20, 0x0, 0xaa, 0x0, 0x61, 0x57, 0xac}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobbyA = {
	0x26c66a70, 0xb367, 0x11cf, {0xa0, 0x24, 0x0, 0xaa, 0x0, 0x61, 0x57, 0xac}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobby2 = {
	0x194c220, 0xa303, 0x11d0, {0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobby2A = {
	0x1bb4af80, 0xa303, 0x11d0, {0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobby3 = {
	0x2db72490, 0x652c, 0x11d1, {0xa7, 0xa8, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlayLobby3A = {
	0x2db72491, 0x652c, 0x11d1, {0xa7, 0xa8, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlayLobby = {
	0x2fe8f810, 0xb2a5, 0x11d0, {0xa7, 0x87, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY DPLPROPERTY_MessagesSupported = {
	0x762ccda1, 0xd916, 0x11d0,
	{0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67}};
extern "C" const GUID DECLSPEC_SELECTANY DPLPROPERTY_LobbyGuid = {0xf56920a0,
	0xd218, 0x11d0, {0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67}};
extern "C" const GUID DECLSPEC_SELECTANY DPLPROPERTY_PlayerGuid = {0xb4319322,
	0xd20d, 0x11d0, {0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67}};
extern "C" const GUID DECLSPEC_SELECTANY DPLPROPERTY_PlayerScore = {0x48784000,
	0xd219, 0x11d0, {0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_TotalSize = {
	0x1318f560, 0x912c, 0x11d0, {0x9d, 0xaa, 0x0, 0xa0, 0xc9, 0xa, 0x43, 0xcb}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_ServiceProvider = {
	0x7d916c0, 0xe0af, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_LobbyProvider = {
	0x59b95640, 0x9667, 0x11d0, {0xa7, 0x7d, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_Phone = {
	0x78ec89a0, 0xe0af, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_PhoneW = {
	0xba5a7a70, 0x9dbf, 0x11d0, {0x9c, 0xc1, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_Modem = {
	0xf6dcc200, 0xa2fe, 0x11d0, {0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_ModemW = {
	0x1fd92e0, 0xa2ff, 0x11d0, {0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_INet = {
	0xc4a54da0, 0xe0af, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_INetW = {
	0xe63232a0, 0x9dbf, 0x11d0, {0x9c, 0xc1, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_INetPort = {0xe4524541, 0x8ea5,
	0x11d1, {0x8a, 0x96, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY DPAID_ComPort = {
	0xf2f0ce00, 0xe0af, 0x11cf, {0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e}};

// GUIDs from dplobby8.h

extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8LobbiedApplication = {
	0x667955ad, 0x6b3b, 0x43ca,
	{0xb9, 0x49, 0xbc, 0x69, 0xb5, 0xba, 0xff, 0x7f}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_DirectPlay8LobbyClient = {
	0x3b2b6775, 0x70b6, 0x45af,
	{0x8d, 0xea, 0xa2, 0x9, 0xc6, 0x95, 0x59, 0xf3}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8LobbiedApplication = {
	0x819074a3, 0x16c, 0x11d3, {0xae, 0x14, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDirectPlay8LobbyClient = {
	0x819074a2, 0x16c, 0x11d3, {0xae, 0x14, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11}};

// GUIDs from devguid.h

extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_1394 = {0x6bdd1fc1,
	0x810f, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_1394DEBUG = {0x66f250d6,
	0x7801, 0x4a64, {0xb1, 0x39, 0xee, 0xa8, 0x0a, 0x45, 0x0b, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_61883 = {0x7ebefbc0,
	0x3200, 0x11d2, {0xb4, 0xc2, 0x00, 0xa0, 0xc9, 0x69, 0x7d, 0x07}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_ADAPTER = {0x4d36e964,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_APMSUPPORT = {0xd45b1c18,
	0xc8fa, 0x11d1, {0x9f, 0x77, 0x00, 0x00, 0xf8, 0x05, 0xf5, 0x30}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_AVC = {0xc06ff265,
	0xae09, 0x48f0, {0x81, 0x2c, 0x16, 0x75, 0x3d, 0x7c, 0xba, 0x83}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_BATTERY = {0x72631e54,
	0x78a4, 0x11d0, {0xbc, 0xf7, 0x00, 0xaa, 0x00, 0xb7, 0xb3, 0x2a}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_BIOMETRIC = {0x53d29ef7,
	0x377c, 0x4d14, {0x86, 0x4b, 0xeb, 0x3a, 0x85, 0x76, 0x93, 0x59}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_BLUETOOTH = {0xe0cbf06c,
	0xcd8b, 0x4647, {0xbb, 0x8a, 0x26, 0x3b, 0x43, 0xf0, 0xf9, 0x74}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_CDROM = {0x4d36e965,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_COMPUTER = {0x4d36e966,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_DECODER = {0x6bdd1fc2,
	0x810f, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_DISKDRIVE = {0x4d36e967,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_DISPLAY = {0x4d36e968,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_DOT4 = {0x48721b56,
	0x6795, 0x11d2, {0xb1, 0xa8, 0x00, 0x80, 0xc7, 0x2e, 0x74, 0xa2}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_DOT4PRINT = {0x49ce6ac8,
	0x6f86, 0x11d2, {0xb1, 0xe5, 0x00, 0x80, 0xc7, 0x2e, 0x74, 0xa2}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_ENUM1394 = {0xc459df55,
	0xdb08, 0x11d1, {0xb0, 0x09, 0x00, 0xa0, 0xc9, 0x08, 0x1f, 0xf6}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FDC = {0x4d36e969,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FLOPPYDISK = {0x4d36e980,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_GPS = {0x6bdd1fc3,
	0x810f, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_HDC = {0x4d36e96a,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_HIDCLASS = {0x745a17a0,
	0x74d3, 0x11d0, {0xb6, 0xfe, 0x00, 0xa0, 0xc9, 0x0f, 0x57, 0xda}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_IMAGE = {0x6bdd1fc6,
	0x810f, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_INFINIBAND = {0x30ef7132,
	0xd858, 0x4a0c, {0xac, 0x24, 0xb9, 0x02, 0x8a, 0x5c, 0xca, 0x3f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_INFRARED = {0x6bdd1fc5,
	0x810f, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_KEYBOARD = {0x4d36e96b,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_LEGACYDRIVER = {
	0x8ecc055d, 0x047f, 0x11d1,
	{0xa5, 0x37, 0x00, 0x00, 0xf8, 0x75, 0x3e, 0xd1}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MEDIA = {0x4d36e96c,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MEDIUM_CHANGER = {
	0xce5939ae, 0xebde, 0x11d0,
	{0xb1, 0x81, 0x00, 0x00, 0xf8, 0x75, 0x3e, 0xc4}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MODEM = {0x4d36e96d,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MONITOR = {0x4d36e96e,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MOUSE = {0x4d36e96f,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MTD = {0x4d36e970,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MULTIFUNCTION = {
	0x4d36e971, 0xe325, 0x11ce,
	{0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_MULTIPORTSERIAL = {
	0x50906cb8, 0xba12, 0x11d1,
	{0xbf, 0x5d, 0x00, 0x00, 0xf8, 0x05, 0xf5, 0x30}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_NET = {0x4d36e972,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_NETCLIENT = {0x4d36e973,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_NETSERVICE = {0x4d36e974,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_NETTRANS = {0x4d36e975,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_NODRIVER = {0x4d36e976,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PCMCIA = {0x4d36e977,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PNPPRINTERS = {
	0x4658ee7e, 0xf050, 0x11d1,
	{0xb6, 0xbd, 0x00, 0xc0, 0x4f, 0xa3, 0x72, 0xa7}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PORTS = {0x4d36e978,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PRINTER = {0x4d36e979,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PRINTERUPGRADE = {
	0x4d36e97a, 0xe325, 0x11ce,
	{0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_PROCESSOR = {0x50127dc3,
	0x0f36, 0x415e, {0xa6, 0xcc, 0x4c, 0xb3, 0xbe, 0x91, 0x0B, 0x65}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SBP2 = {0xd48179be,
	0xec20, 0x11d1, {0xb6, 0xb8, 0x00, 0xc0, 0x4f, 0xa3, 0x72, 0xa7}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SCSIADAPTER = {
	0x4d36e97b, 0xe325, 0x11ce,
	{0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SECURITYACCELERATOR = {
	0x268c95a1, 0xedfe, 0x11d3,
	{0x95, 0xc3, 0x00, 0x10, 0xdc, 0x40, 0x50, 0xa5}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SENSOR = {0x5175d334,
	0xc371, 0x4806, {0xb3, 0xba, 0x71, 0xfd, 0x53, 0xc9, 0x25, 0x8d}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SIDESHOW = {0x997b5d8d,
	0xc442, 0x4f2e, {0xba, 0xf3, 0x9c, 0x8e, 0x67, 0x1e, 0x9e, 0x21}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SMARTCARDREADER = {
	0x50dd5230, 0xba8a, 0x11d1,
	{0xbf, 0x5d, 0x00, 0x00, 0xf8, 0x05, 0xf5, 0x30}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SOUND = {0x4d36e97c,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_SYSTEM = {0x4d36e97d,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_TAPEDRIVE = {0x6d807884,
	0x7d21, 0x11cf, {0x80, 0x1c, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_UNKNOWN = {0x4d36e97e,
	0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_USB = {0x36fc9e60,
	0xc465, 0x11cf, {0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_VOLUME = {0x71a27cdd,
	0x812a, 0x11d0, {0xbe, 0xc7, 0x08, 0x00, 0x2b, 0xe2, 0x09, 0x2f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_VOLUMESNAPSHOT = {
	0x533c5b84, 0xec70, 0x11d2,
	{0x95, 0x05, 0x00, 0xc0, 0x4f, 0x79, 0xde, 0xaf}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_WCEUSBS = {0x25dbce51,
	0x6c8f, 0x4a72, {0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_WPD = {0xeec5ad98,
	0x8080, 0x425f, {0x92, 0x2a, 0xda, 0xbf, 0x3d, 0xe3, 0xf6, 0x9a}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_ACTIVITYMONITOR = {0xb86dff51, 0xa31e, 0x4bac,
		{0xb3, 0xcf, 0xe8, 0xcf, 0xe7, 0x5c, 0x9f, 0xc2}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_UNDELETE = {
	0xfe8f1572, 0xc67a, 0x48c0,
	{0xbb, 0xac, 0x0b, 0x5c, 0x6d, 0x66, 0xca, 0xfb}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_ANTIVIRUS = {
	0xb1d1a169, 0xc54f, 0x4379,
	{0x81, 0xdb, 0xbe, 0xe7, 0xd8, 0x8d, 0x74, 0x54}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_REPLICATION = {
	0x48d3ebc4, 0x4cf8, 0x48ff,
	{0xb8, 0x69, 0x9c, 0x68, 0xad, 0x42, 0xeb, 0x9f}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_CONTINUOUSBACKUP = {0x71aa14f8, 0x6fad, 0x4622,
		{0xad, 0x77, 0x92, 0xbb, 0x9d, 0x7e, 0x69, 0x47}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_CONTENTSCREENER = {0x3e3f0674, 0xc83c, 0x4558,
		{0xbb, 0x26, 0x98, 0x20, 0xe1, 0xeb, 0xa5, 0xc5}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_QUOTAMANAGEMENT = {0x8503c911, 0xa6c7, 0x4919,
		{0x8f, 0x79, 0x50, 0x28, 0xf5, 0x86, 0x6b, 0x0c}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_SYSTEMRECOVERY = {0x2db15374, 0x706e, 0x4131,
		{0xa0, 0xc7, 0xd7, 0xc7, 0x8e, 0xb0, 0x28, 0x9a}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_CFSMETADATASERVER = {0xcdcf0939, 0xb75b, 0x4630,
		{0xbf, 0x76, 0x80, 0xf7, 0xba, 0x65, 0x58, 0x84}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_HSM = {
	0xd546500a, 0x2aeb, 0x45f6,
	{0x94, 0x82, 0xf4, 0xb1, 0x79, 0x9c, 0x31, 0x77}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_COMPRESSION = {
	0xf3586baf, 0xb5aa, 0x49b5,
	{0x8d, 0x6c, 0x05, 0x69, 0x28, 0x4c, 0x63, 0x9f}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_ENCRYPTION = {
	0xa0a701c0, 0xa511, 0x42ff,
	{0xaa, 0x6c, 0x06, 0xdc, 0x03, 0x95, 0x57, 0x6f}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_PHYSICALQUOTAMANAGEMENT = {0x6a0a8e78, 0xbba6,
		0x4fc4, {0xa7, 0x09, 0x1e, 0x33, 0xcd, 0x09, 0xd6, 0x7e}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_OPENFILEBACKUP = {0xf8ecafa6, 0x66d1, 0x41a5,
		{0x89, 0x9b, 0x66, 0x58, 0x5d, 0x72, 0x16, 0xb7}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_SECURITYENHANCER = {0xd02bc3da, 0x0c8e, 0x4945,
		{0x9b, 0xd5, 0xf1, 0x88, 0x3c, 0x22, 0x6c, 0x8c}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_COPYPROTECTION = {0x89786ff1, 0x9c12, 0x402f,
		{0x9c, 0x9e, 0x17, 0x75, 0x3c, 0x7f, 0x43, 0x75}};
extern "C" const GUID DECLSPEC_SELECTANY GUID_DEVCLASS_FSFILTER_SYSTEM = {
	0x5d1b9aaa, 0x01e2, 0x46af,
	{0x84, 0x9f, 0x27, 0x2b, 0x3f, 0x32, 0x4c, 0x46}};
extern "C" const GUID DECLSPEC_SELECTANY
	GUID_DEVCLASS_FSFILTER_INFRASTRUCTURE = {0xe55fa6f9, 0x128c, 0x4d04,
		{0xab, 0xab, 0x63, 0x0c, 0x74, 0xb1, 0x45, 0x3a}};

// GUIDs from dxgi.h

extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIObject = {0xaec22fb8, 0x76f3,
	0x4639, {0x9b, 0xe0, 0x28, 0xeb, 0x43, 0xa6, 0x7a, 0x2e}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIDeviceSubObject = {0x3d3e0379,
	0xf9de, 0x4d58, {0xbb, 0x6c, 0x18, 0xd6, 0x29, 0x92, 0xf1, 0xa6}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIResource = {0x035f3ab4,
	0x482e, 0x4e50, {0xb4, 0x1f, 0x8a, 0x7f, 0x8b, 0xd8, 0x96, 0x0b}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIKeyedMutex = {0x9d8e1289,
	0xd7b3, 0x465f, {0x81, 0x26, 0x25, 0x0e, 0x34, 0x9a, 0xf8, 0x5d}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGISurface = {0xcafcb56c, 0x6ac3,
	0x4889, {0xbf, 0x47, 0x9e, 0x23, 0xbb, 0xd2, 0x60, 0xec}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGISurface1 = {0x4AE63092,
	0x6327, 0x4c1b, {0x80, 0xAE, 0xBF, 0xE1, 0x2E, 0xA3, 0x2B, 0x86}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIAdapter = {0x2411e7e1, 0x12ac,
	0x4ccf, {0xbd, 0x14, 0x97, 0x98, 0xe8, 0x53, 0x4d, 0xc0}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIOutput = {0xae02eedb, 0xc735,
	0x4690, {0x8d, 0x52, 0x5a, 0x8d, 0xc2, 0x02, 0x13, 0xaa}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGISwapChain = {0x310d36a0,
	0xd2e7, 0x4c0a, {0xaa, 0x04, 0x6a, 0x9d, 0x23, 0xb8, 0x88, 0x6a}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIFactory = {0x7b7166ec, 0x21c7,
	0x44ae, {0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIDevice = {0x54ec77fa, 0x1377,
	0x44e6, {0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIFactory1 = {0x770aae78,
	0xf26f, 0x4dba, {0xa8, 0x29, 0x25, 0x3c, 0x83, 0xd1, 0xb3, 0x87}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIAdapter1 = {0x29038f61,
	0x3839, 0x4626, {0x91, 0xfd, 0x08, 0x68, 0x79, 0x01, 0x1a, 0x05}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IDXGIDevice1 = {0x77db970f, 0x6276,
	0x48ba, {0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c}};

// GUIDs from d3dcommon.h

extern "C" const GUID DECLSPEC_SELECTANY WKPDID_D3DDebugObjectName = {
	0x429b8c22, 0x9188, 0x4b0c,
	{0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00}};
extern "C" const GUID DECLSPEC_SELECTANY IID_ID3D10Blob = {
	0x8ba5fb08, 0x5195, 0x40e2, {0xac, 0x58, 0xd, 0x98, 0x9c, 0x3a, 0x1, 0x2}};

// GUIDs from wbemcli.h

extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemLocator = {0x4590f811,
	0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemContext = {0x674B6698,
	0xEE92, 0x11d0, {0xAD, 0x71, 0x00, 0xC0, 0x4F, 0xD8, 0xFD, 0xFF}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_UnsecuredApartment = {0x49bd2028,
	0x1523, 0x11d1, {0xad, 0x79, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemClassObject = {0x9A653086,
	0x174F, 0x11d2, {0xB5, 0xF9, 0x0, 0x10, 0x4B, 0x70, 0x3E, 0xFD}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_MofCompiler = {0x6daf9757,
	0x2e37, 0x11d2, {0xae, 0xc9, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemStatusCodeText = {0xeb87e1bd,
	0x3233, 0x11d2, {0xae, 0xc9, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemBackupRestore = {0xC49E32C6,
	0xBC8B, 0x11d2, {0x85, 0xD4, 0x00, 0x10, 0x5A, 0x1F, 0x83, 0x04}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemRefresher = {0xc71566f2,
	0x561e, 0x11d1, {0xad, 0x87, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff}};
extern "C" const GUID DECLSPEC_SELECTANY CLSID_WbemObjectTextSrc = {0x8D1C559D,
	0x84F0, 0x4bb3, {0xA7, 0xD5, 0x56, 0xA7, 0x43, 0x5A, 0x9B, 0xA6}};

extern "C" const GUID DECLSPEC_SELECTANY LIBID_WbemClient_v1 = {0x7EC196FE,
	0x7005, 0x11D1, {0xAD, 0x90, 0x00, 0xC0, 0x4F, 0xD8, 0xFD, 0xFF}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemClassObject = {0xdc12a681,
	0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemObjectAccess = {0x49353c9a,
	0x516b, 0x11d1, {0xae, 0xa6, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemQualifierSet = {0xdc12a680,
	0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemServices = {0x9556dc99,
	0x828c, 0x11cf, {0xa3, 0x7e, 0x00, 0xaa, 0x00, 0x32, 0x40, 0xc7}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemLocator = {0xdc12a687, 0x737f,
	0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemObjectSink = {0x7c857801,
	0x7381, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IEnumWbemClassObject = {0x027947e1,
	0xd731, 0x11ce, {0xa3, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemCallResult = {0x44aca675,
	0xe8fc, 0x11d0, {0xa0, 0x7c, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemContext = {0x44aca674, 0xe8fc,
	0x11d0, {0xa0, 0x7c, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IUnsecuredApartment = {0x1cfaba8c,
	0x1523, 0x11d1, {0xad, 0x79, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemUnsecuredApartment = {
	0x31739d04, 0x3471, 0x4cf4,
	{0x9a, 0x7c, 0x57, 0xa4, 0x4a, 0xe7, 0x19, 0x56}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemStatusCodeText = {0xeb87e1bc,
	0x3233, 0x11d2, {0xae, 0xc9, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemBackupRestore = {0xC49E32C7,
	0xBC8B, 0x11d2, {0x85, 0xD4, 0x00, 0x10, 0x5A, 0x1F, 0x83, 0x04}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemBackupRestoreEx = {0xA359DEC5,
	0xE813, 0x4834, {0x8A, 0x2A, 0xBA, 0x7F, 0x1D, 0x77, 0x7D, 0x76}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemRefresher = {0x49353c99,
	0x516b, 0x11d1, {0xae, 0xa6, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemHiPerfEnum = {0x2705C288,
	0x79AE, 0x11d2, {0xB3, 0x48, 0x00, 0x10, 0x5A, 0x1F, 0x81, 0x77}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemConfigureRefresher = {
	0x49353c92, 0x516b, 0x11d1,
	{0xae, 0xa6, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemShutdown = {0xb7b31df9,
	0xd515, 0x11d3, {0xa1, 0x1c, 0x00, 0x10, 0x5a, 0x1f, 0x51, 0x5a}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IWbemObjectTextSrc = {0xbfbf883a,
	0xcad7, 0x11d3, {0xa1, 0x1b, 0x00, 0x10, 0x5a, 0x1f, 0x51, 0x5a}};
extern "C" const GUID DECLSPEC_SELECTANY IID_IMofCompiler = {0x6daf974e, 0x2e37,
	0x11d2, {0xae, 0xc9, 0x00, 0xc0, 0x4f, 0xb6, 0x88, 0x20}};

/***************************************

	Joystick with two x/y/z sticks, two sliders, four POVs and up to 32 buttons

***************************************/

DIOBJECTDATAFORMAT c_rgodfDIJoy[44] = {
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RxAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lRx)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RyAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lRy)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RzAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, lRz)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rglSlider[0])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rglSlider[1])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgdwPOV[0])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgdwPOV[1])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgdwPOV[2])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgdwPOV[3])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[0])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[1])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[2])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[3])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[4])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[5])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[6])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[7])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[8])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[9])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[10])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[11])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[12])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[13])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[14])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[15])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[16])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[17])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[18])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[19])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[20])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[21])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[22])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[23])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[24])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[25])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[26])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[27])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[28])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[29])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[30])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE, rgbButtons[31])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0}};

const DIDATAFORMAT c_dfDIJoystick = {sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),     // Buffer sizes
	DIDF_ABSAXIS,                   // Absolute axis data
	sizeof(DIJOYSTATE),             // Size of the structure to store into
	BURGER_ARRAYSIZE(c_rgodfDIJoy), // Size of array
	c_rgodfDIJoy};

/***************************************

	Joystick with two x/y/z sticks, two sliders, four POVs and up to 128 buttons
	and a force, acceleration and velocity x/y/z stick

***************************************/

DIOBJECTDATAFORMAT c_rgodfDIJoy2[164] = {
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RxAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lRx)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RyAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lRy)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_RzAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lRz)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglSlider[0])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglSlider[1])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgdwPOV[0])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgdwPOV[1])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgdwPOV[2])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_POV, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgdwPOV[3])),
		DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[0])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[1])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[2])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[3])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[4])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[5])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[6])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[7])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[8])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[9])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[10])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[11])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[12])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[13])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[14])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[15])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[16])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[17])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[18])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[19])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[20])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[21])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[22])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[23])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[24])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[25])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[26])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[27])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[28])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[29])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[30])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[31])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[32])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[33])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[34])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[35])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[36])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[37])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[38])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[39])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[40])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[41])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[42])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[43])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[44])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[45])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[46])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[47])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[48])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[49])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[50])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[51])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[52])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[53])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[54])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[55])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[56])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[57])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[58])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[59])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[60])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[61])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[62])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[63])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[64])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[65])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[66])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[67])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[68])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[69])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[70])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[71])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[72])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[73])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[74])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[75])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[76])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[77])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[78])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[79])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[80])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[81])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[82])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[83])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[84])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[85])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[86])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[87])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[88])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[89])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[90])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[91])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[92])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[93])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[94])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[95])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[96])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[97])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[98])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[99])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[100])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[101])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[102])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[103])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[104])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[105])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[106])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[107])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[108])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[109])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[110])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[111])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[112])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[113])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[114])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[115])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[116])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[117])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[118])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[119])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[120])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[121])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[122])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[123])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[124])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[125])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[126])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rgbButtons[127])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_RxAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVRx)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_RyAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVRy)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_RzAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lVRz)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglVSlider[0])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglVSlider[1])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTVELOCITY},
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lAX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lAY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lAZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_RxAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lARx)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_RyAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lARy)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_RzAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lARz)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglASlider[0])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglASlider[1])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTACCEL},
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_RxAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFRx)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_RyAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFRy)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_RzAxis, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, lFRz)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglFSlider[0])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE},
	{&GUID_Slider, static_cast<DWORD>(FIELD_OFFSET(DIJOYSTATE2, rglFSlider[1])),
		DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTFORCE}};

/***************************************

	Keyboard with 256 optional keys

***************************************/

const DIDATAFORMAT c_dfDIJoystick2 = {sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),      // Buffer sizes
	DIDF_ABSAXIS,                    // Absolute axis data
	sizeof(DIJOYSTATE2),             // Size of the structure to store into
	BURGER_ARRAYSIZE(c_rgodfDIJoy2), // Size of array
	c_rgodfDIJoy2};

/***************************************

	Keyboard with 256 optional keys

***************************************/

DIOBJECTDATAFORMAT c_rgodfDIKeyboard[256] = {
	{&GUID_Key, 0x00, DIDFT_MAKEINSTANCE(0x00) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x01, DIDFT_MAKEINSTANCE(0x01) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x02, DIDFT_MAKEINSTANCE(0x02) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x03, DIDFT_MAKEINSTANCE(0x03) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x04, DIDFT_MAKEINSTANCE(0x04) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x05, DIDFT_MAKEINSTANCE(0x05) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x06, DIDFT_MAKEINSTANCE(0x06) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x07, DIDFT_MAKEINSTANCE(0x07) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x08, DIDFT_MAKEINSTANCE(0x08) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x09, DIDFT_MAKEINSTANCE(0x09) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0A, DIDFT_MAKEINSTANCE(0x0A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0B, DIDFT_MAKEINSTANCE(0x0B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0C, DIDFT_MAKEINSTANCE(0x0C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0D, DIDFT_MAKEINSTANCE(0x0D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0E, DIDFT_MAKEINSTANCE(0x0E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x0F, DIDFT_MAKEINSTANCE(0x0F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x10, DIDFT_MAKEINSTANCE(0x10) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x11, DIDFT_MAKEINSTANCE(0x11) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x12, DIDFT_MAKEINSTANCE(0x12) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x13, DIDFT_MAKEINSTANCE(0x13) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x14, DIDFT_MAKEINSTANCE(0x14) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x15, DIDFT_MAKEINSTANCE(0x15) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x16, DIDFT_MAKEINSTANCE(0x16) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x17, DIDFT_MAKEINSTANCE(0x17) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x18, DIDFT_MAKEINSTANCE(0x18) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x19, DIDFT_MAKEINSTANCE(0x19) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1A, DIDFT_MAKEINSTANCE(0x1A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1B, DIDFT_MAKEINSTANCE(0x1B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1C, DIDFT_MAKEINSTANCE(0x1C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1D, DIDFT_MAKEINSTANCE(0x1D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1E, DIDFT_MAKEINSTANCE(0x1E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x1F, DIDFT_MAKEINSTANCE(0x1F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x20, DIDFT_MAKEINSTANCE(0x20) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x21, DIDFT_MAKEINSTANCE(0x21) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x22, DIDFT_MAKEINSTANCE(0x22) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x23, DIDFT_MAKEINSTANCE(0x23) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x24, DIDFT_MAKEINSTANCE(0x24) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x25, DIDFT_MAKEINSTANCE(0x25) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x26, DIDFT_MAKEINSTANCE(0x26) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x27, DIDFT_MAKEINSTANCE(0x27) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x28, DIDFT_MAKEINSTANCE(0x28) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x29, DIDFT_MAKEINSTANCE(0x29) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2A, DIDFT_MAKEINSTANCE(0x2A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2B, DIDFT_MAKEINSTANCE(0x2B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2C, DIDFT_MAKEINSTANCE(0x2C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2D, DIDFT_MAKEINSTANCE(0x2D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2E, DIDFT_MAKEINSTANCE(0x2E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x2F, DIDFT_MAKEINSTANCE(0x2F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x30, DIDFT_MAKEINSTANCE(0x30) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x31, DIDFT_MAKEINSTANCE(0x31) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x32, DIDFT_MAKEINSTANCE(0x32) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x33, DIDFT_MAKEINSTANCE(0x33) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x34, DIDFT_MAKEINSTANCE(0x34) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x35, DIDFT_MAKEINSTANCE(0x35) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x36, DIDFT_MAKEINSTANCE(0x36) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x37, DIDFT_MAKEINSTANCE(0x37) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x38, DIDFT_MAKEINSTANCE(0x38) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x39, DIDFT_MAKEINSTANCE(0x39) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3A, DIDFT_MAKEINSTANCE(0x3A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3B, DIDFT_MAKEINSTANCE(0x3B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3C, DIDFT_MAKEINSTANCE(0x3C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3D, DIDFT_MAKEINSTANCE(0x3D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3E, DIDFT_MAKEINSTANCE(0x3E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x3F, DIDFT_MAKEINSTANCE(0x3F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x40, DIDFT_MAKEINSTANCE(0x40) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x41, DIDFT_MAKEINSTANCE(0x41) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x42, DIDFT_MAKEINSTANCE(0x42) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x43, DIDFT_MAKEINSTANCE(0x43) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x44, DIDFT_MAKEINSTANCE(0x44) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x45, DIDFT_MAKEINSTANCE(0x45) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x46, DIDFT_MAKEINSTANCE(0x46) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x47, DIDFT_MAKEINSTANCE(0x47) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x48, DIDFT_MAKEINSTANCE(0x48) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x49, DIDFT_MAKEINSTANCE(0x49) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4A, DIDFT_MAKEINSTANCE(0x4A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4B, DIDFT_MAKEINSTANCE(0x4B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4C, DIDFT_MAKEINSTANCE(0x4C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4D, DIDFT_MAKEINSTANCE(0x4D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4E, DIDFT_MAKEINSTANCE(0x4E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x4F, DIDFT_MAKEINSTANCE(0x4F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x50, DIDFT_MAKEINSTANCE(0x50) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x51, DIDFT_MAKEINSTANCE(0x51) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x52, DIDFT_MAKEINSTANCE(0x52) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x53, DIDFT_MAKEINSTANCE(0x53) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x54, DIDFT_MAKEINSTANCE(0x54) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x55, DIDFT_MAKEINSTANCE(0x55) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x56, DIDFT_MAKEINSTANCE(0x56) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x57, DIDFT_MAKEINSTANCE(0x57) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x58, DIDFT_MAKEINSTANCE(0x58) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x59, DIDFT_MAKEINSTANCE(0x59) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5A, DIDFT_MAKEINSTANCE(0x5A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5B, DIDFT_MAKEINSTANCE(0x5B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5C, DIDFT_MAKEINSTANCE(0x5C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5D, DIDFT_MAKEINSTANCE(0x5D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5E, DIDFT_MAKEINSTANCE(0x5E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x5F, DIDFT_MAKEINSTANCE(0x5F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x60, DIDFT_MAKEINSTANCE(0x60) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x61, DIDFT_MAKEINSTANCE(0x61) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x62, DIDFT_MAKEINSTANCE(0x62) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x63, DIDFT_MAKEINSTANCE(0x63) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x64, DIDFT_MAKEINSTANCE(0x64) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x65, DIDFT_MAKEINSTANCE(0x65) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x66, DIDFT_MAKEINSTANCE(0x66) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x67, DIDFT_MAKEINSTANCE(0x67) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x68, DIDFT_MAKEINSTANCE(0x68) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x69, DIDFT_MAKEINSTANCE(0x69) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6A, DIDFT_MAKEINSTANCE(0x6A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6B, DIDFT_MAKEINSTANCE(0x6B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6C, DIDFT_MAKEINSTANCE(0x6C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6D, DIDFT_MAKEINSTANCE(0x6D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6E, DIDFT_MAKEINSTANCE(0x6E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x6F, DIDFT_MAKEINSTANCE(0x6F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x70, DIDFT_MAKEINSTANCE(0x70) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x71, DIDFT_MAKEINSTANCE(0x71) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x72, DIDFT_MAKEINSTANCE(0x72) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x73, DIDFT_MAKEINSTANCE(0x73) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x74, DIDFT_MAKEINSTANCE(0x74) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x75, DIDFT_MAKEINSTANCE(0x75) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x76, DIDFT_MAKEINSTANCE(0x76) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x77, DIDFT_MAKEINSTANCE(0x77) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x78, DIDFT_MAKEINSTANCE(0x78) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x79, DIDFT_MAKEINSTANCE(0x79) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7A, DIDFT_MAKEINSTANCE(0x7A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7B, DIDFT_MAKEINSTANCE(0x7B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7C, DIDFT_MAKEINSTANCE(0x7C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7D, DIDFT_MAKEINSTANCE(0x7D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7E, DIDFT_MAKEINSTANCE(0x7E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x7F, DIDFT_MAKEINSTANCE(0x7F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x80, DIDFT_MAKEINSTANCE(0x80) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x81, DIDFT_MAKEINSTANCE(0x81) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x82, DIDFT_MAKEINSTANCE(0x82) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x83, DIDFT_MAKEINSTANCE(0x83) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x84, DIDFT_MAKEINSTANCE(0x84) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x85, DIDFT_MAKEINSTANCE(0x85) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x86, DIDFT_MAKEINSTANCE(0x86) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x87, DIDFT_MAKEINSTANCE(0x87) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x88, DIDFT_MAKEINSTANCE(0x88) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x89, DIDFT_MAKEINSTANCE(0x89) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8A, DIDFT_MAKEINSTANCE(0x8A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8B, DIDFT_MAKEINSTANCE(0x8B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8C, DIDFT_MAKEINSTANCE(0x8C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8D, DIDFT_MAKEINSTANCE(0x8D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8E, DIDFT_MAKEINSTANCE(0x8E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x8F, DIDFT_MAKEINSTANCE(0x8F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x90, DIDFT_MAKEINSTANCE(0x90) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x91, DIDFT_MAKEINSTANCE(0x91) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x92, DIDFT_MAKEINSTANCE(0x92) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x93, DIDFT_MAKEINSTANCE(0x93) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x94, DIDFT_MAKEINSTANCE(0x94) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x95, DIDFT_MAKEINSTANCE(0x95) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x96, DIDFT_MAKEINSTANCE(0x96) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x97, DIDFT_MAKEINSTANCE(0x97) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x98, DIDFT_MAKEINSTANCE(0x98) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x99, DIDFT_MAKEINSTANCE(0x99) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9A, DIDFT_MAKEINSTANCE(0x9A) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9B, DIDFT_MAKEINSTANCE(0x9B) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9C, DIDFT_MAKEINSTANCE(0x9C) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9D, DIDFT_MAKEINSTANCE(0x9D) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9E, DIDFT_MAKEINSTANCE(0x9E) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0x9F, DIDFT_MAKEINSTANCE(0x9F) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA0, DIDFT_MAKEINSTANCE(0xA0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA1, DIDFT_MAKEINSTANCE(0xA1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA2, DIDFT_MAKEINSTANCE(0xA2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA3, DIDFT_MAKEINSTANCE(0xA3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA4, DIDFT_MAKEINSTANCE(0xA4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA5, DIDFT_MAKEINSTANCE(0xA5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA6, DIDFT_MAKEINSTANCE(0xA6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA7, DIDFT_MAKEINSTANCE(0xA7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA8, DIDFT_MAKEINSTANCE(0xA8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xA9, DIDFT_MAKEINSTANCE(0xA9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAA, DIDFT_MAKEINSTANCE(0xAA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAB, DIDFT_MAKEINSTANCE(0xAB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAC, DIDFT_MAKEINSTANCE(0xAC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAD, DIDFT_MAKEINSTANCE(0xAD) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAE, DIDFT_MAKEINSTANCE(0xAE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xAF, DIDFT_MAKEINSTANCE(0xAF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB0, DIDFT_MAKEINSTANCE(0xB0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB1, DIDFT_MAKEINSTANCE(0xB1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB2, DIDFT_MAKEINSTANCE(0xB2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB3, DIDFT_MAKEINSTANCE(0xB3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB4, DIDFT_MAKEINSTANCE(0xB4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB5, DIDFT_MAKEINSTANCE(0xB5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB6, DIDFT_MAKEINSTANCE(0xB6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB7, DIDFT_MAKEINSTANCE(0xB7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB8, DIDFT_MAKEINSTANCE(0xB8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xB9, DIDFT_MAKEINSTANCE(0xB9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBA, DIDFT_MAKEINSTANCE(0xBA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBB, DIDFT_MAKEINSTANCE(0xBB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBC, DIDFT_MAKEINSTANCE(0xBC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBD, DIDFT_MAKEINSTANCE(0xBD) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBE, DIDFT_MAKEINSTANCE(0xBE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xBF, DIDFT_MAKEINSTANCE(0xBF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC0, DIDFT_MAKEINSTANCE(0xC0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC1, DIDFT_MAKEINSTANCE(0xC1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC2, DIDFT_MAKEINSTANCE(0xC2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC3, DIDFT_MAKEINSTANCE(0xC3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC4, DIDFT_MAKEINSTANCE(0xC4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC5, DIDFT_MAKEINSTANCE(0xC5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC6, DIDFT_MAKEINSTANCE(0xC6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC7, DIDFT_MAKEINSTANCE(0xC7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC8, DIDFT_MAKEINSTANCE(0xC8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xC9, DIDFT_MAKEINSTANCE(0xC9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCA, DIDFT_MAKEINSTANCE(0xCA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCB, DIDFT_MAKEINSTANCE(0xCB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCC, DIDFT_MAKEINSTANCE(0xCC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCD, DIDFT_MAKEINSTANCE(0xCD) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCE, DIDFT_MAKEINSTANCE(0xCE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xCF, DIDFT_MAKEINSTANCE(0xCF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD0, DIDFT_MAKEINSTANCE(0xD0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD1, DIDFT_MAKEINSTANCE(0xD1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD2, DIDFT_MAKEINSTANCE(0xD2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD3, DIDFT_MAKEINSTANCE(0xD3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD4, DIDFT_MAKEINSTANCE(0xD4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD5, DIDFT_MAKEINSTANCE(0xD5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD6, DIDFT_MAKEINSTANCE(0xD6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD7, DIDFT_MAKEINSTANCE(0xD7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD8, DIDFT_MAKEINSTANCE(0xD8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xD9, DIDFT_MAKEINSTANCE(0xD9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDA, DIDFT_MAKEINSTANCE(0xDA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDB, DIDFT_MAKEINSTANCE(0xDB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDC, DIDFT_MAKEINSTANCE(0xDC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDD, DIDFT_MAKEINSTANCE(0xDD) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDE, DIDFT_MAKEINSTANCE(0xDE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xDF, DIDFT_MAKEINSTANCE(0xDF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE0, DIDFT_MAKEINSTANCE(0xE0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE1, DIDFT_MAKEINSTANCE(0xE1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE2, DIDFT_MAKEINSTANCE(0xE2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE3, DIDFT_MAKEINSTANCE(0xE3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE4, DIDFT_MAKEINSTANCE(0xE4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE5, DIDFT_MAKEINSTANCE(0xE5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE6, DIDFT_MAKEINSTANCE(0xE6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE7, DIDFT_MAKEINSTANCE(0xE7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE8, DIDFT_MAKEINSTANCE(0xE8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xE9, DIDFT_MAKEINSTANCE(0xE9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xEA, DIDFT_MAKEINSTANCE(0xEA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xEB, DIDFT_MAKEINSTANCE(0xEB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xEC, DIDFT_MAKEINSTANCE(0xEC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xED, DIDFT_MAKEINSTANCE(0xED) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xEE, DIDFT_MAKEINSTANCE(0xEE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xEF, DIDFT_MAKEINSTANCE(0xEF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF0, DIDFT_MAKEINSTANCE(0xF0) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF1, DIDFT_MAKEINSTANCE(0xF1) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF2, DIDFT_MAKEINSTANCE(0xF2) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF3, DIDFT_MAKEINSTANCE(0xF3) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF4, DIDFT_MAKEINSTANCE(0xF4) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF5, DIDFT_MAKEINSTANCE(0xF5) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF6, DIDFT_MAKEINSTANCE(0xF6) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF7, DIDFT_MAKEINSTANCE(0xF7) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF8, DIDFT_MAKEINSTANCE(0xF8) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xF9, DIDFT_MAKEINSTANCE(0xF9) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFA, DIDFT_MAKEINSTANCE(0xFA) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFB, DIDFT_MAKEINSTANCE(0xFB) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFC, DIDFT_MAKEINSTANCE(0xFC) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFD, DIDFT_MAKEINSTANCE(0xFD) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFE, DIDFT_MAKEINSTANCE(0xFE) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0},
	{&GUID_Key, 0xFF, DIDFT_MAKEINSTANCE(0xFF) | DIDFT_BUTTON | DIDFT_OPTIONAL,
		0}};

const DIDATAFORMAT c_dfDIKeyboard = {sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),          // Buffer sizes
	DIDF_RELAXIS,                        // Relative axis data
	256,                                 // Size of the structure to store into
	BURGER_ARRAYSIZE(c_rgodfDIKeyboard), // Size of array
	c_rgodfDIKeyboard};

/***************************************

	Wheel mouse with 2 buttons (2 optional extra buttons)

***************************************/

DIOBJECTDATAFORMAT c_rgodfDIMouse[7] = {
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, lX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, lY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, lZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, rgbButtons[0])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, rgbButtons[1])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, rgbButtons[2])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE, rgbButtons[3])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0}};

const DIDATAFORMAT c_dfDIMouse = {sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),       // Buffer sizes
	DIDF_RELAXIS,                     // Relative axis data
	sizeof(DIMOUSESTATE),             // Size of the structure to store into
	BURGER_ARRAYSIZE(c_rgodfDIMouse), // Size of array
	c_rgodfDIMouse};

/***************************************

	Wheel mouse with 2 buttons (6 optional extra buttons)

***************************************/

DIOBJECTDATAFORMAT c_rgodfDIMouse2[11] = {
	{&GUID_XAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, lX)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_YAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, lY)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_ZAxis, static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, lZ)),
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[0])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[1])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[2])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[3])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[4])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[5])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[6])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{&GUID_Button,
		static_cast<DWORD>(FIELD_OFFSET(DIMOUSESTATE2, rgbButtons[7])),
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0}};

const DIDATAFORMAT c_dfDIMouse2 = {sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),        // Buffer sizes
	DIDF_RELAXIS,                      // Relative axis data
	sizeof(DIMOUSESTATE2),             // Size of the structure to store into
	BURGER_ARRAYSIZE(c_rgodfDIMouse2), // Size of array
	c_rgodfDIMouse2};

#endif

/*! ************************************

	\var IID_IXAudio2_2_8
	\brief GUID of XAudio2 version 2.8 (Windows 8)

***************************************/

/*! ************************************

	\var CLSID_AudioVolumeMeter_2_8
	\brief GUID of AudioVolumeMeter version 2.8 (Windows 8)

***************************************/

/*! ************************************

	\var CLSID_AudioReverb_2_8
	\brief GUID of AudioReverb version 2.8 (Windows 8)

***************************************/

/*! ************************************

	\var IID_IXAudio2_2_9
	\brief GUID of XAudio2 version 2.9 (Windows 10)

***************************************/

#endif
