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
struct HPALETTE__;
struct _D3DADAPTER_IDENTIFIER9;
struct _D3DCAPS9;
struct _D3DDISPLAYMODE;
struct _D3DPRESENT_PARAMETERS_;
struct _D3DSURFACE_DESC;
struct _DIDATAFORMAT;
struct _FILETIME;
struct _SYSTEMTIME;
struct _GUID;
struct tagFILTERKEYS;
struct tagPOINT;
struct tagSTICKYKEYS;
struct tagTOGGLEKEYS;
struct tagWINDOWPLACEMENT;
struct IXAudio2;
struct IXAudio2Voice;
struct IXAudio2SourceVoice;
struct IXAudio2SubmixVoice;
struct IXAudio2MasteringVoice;
struct IXAudio2EngineCallback;
struct IXAudio2VoiceCallback;

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

struct HWND__;

#endif

/* END */

#endif
