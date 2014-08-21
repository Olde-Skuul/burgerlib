/***************************************

	Global variable manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

/* BEGIN */
namespace Burger {
class OutputMemoryStream;
struct Globals {
private:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HINSTANCE__ *m_hInstance;		///< Current instance of the application (Windows only)
	HWND__ *m_hWindow;				///< Main window for the application (Windows only)
	HINSTANCE__ *m_hDInput8DLL;		///< Reference to DInput8.dll (Windows only)
	void *m_pDirectInput8Create;	///< Pointer to DirectInput8CreateW function (Windows only)
	IDirectInput8W* m_pDirectInput8W;	///< DirectInput8W instance (Windows only)
	HINSTANCE__ *m_hDInputDLL;		///< Reference to DInput.dll (Windows only)
	void *m_pDirectInputCreateW;	///< Pointer to DirectInputCreateW function (Windows only)
	IDirectInputW* m_pDirectInputW;	///< DirectInputW instance (Windows only)
	HINSTANCE__ *m_hDDraw;			///< Reference to DDraw.dll (Windows only)
	void *m_pDirectDrawCreate;		///< Pointer to DirectDrawCreate function (Windows only)
	void *m_pDirectDrawCreateEx;	///< Pointer to DirectDrawCreateEx function (Windows only)
	void *m_pDirectDrawCreateClipper;	///< Pointer to DirectDrawCreateClipper function (Windows only)
	void *m_pDirectDrawEnumerateA;	///< Pointer to DirectDrawEnumerateA function (Windows only)
	void *m_pDirectDrawEnumerateW;	///< Pointer to DirectDrawEnumerateW function (Windows only)
	void *m_pDirectDrawEnumerateExA;	///< Pointer to DirectDrawEnumerateExA function (Windows only)
	void *m_pDirectDrawEnumerateExW;	///< Pointer to DirectDrawEnumerateExW function (Windows only)
	HINSTANCE__ *m_hD3D9;			///< Reference to D3D9.dll (Windows only)
	void *m_pDirect3DCreate9;		///< Pointer to Direct3DCreate9 function (Windows only)
	HINSTANCE__ *m_hDirectSound;	///< Reference to DSOUND.dll (Windows only)
	void *m_pDirectSoundCreate;		///< Pointer to DirectSoundCreate function (Windows only)
	void *m_pDirectSoundCreate8;	///< Pointer to DirectSoundCreate8 function (Windows only)
	void *m_pDirectSoundEnumerateA;	///< Pointer to DirectSoundEnumerateA function (Windows only)
	void *m_pDirectSoundEnumerateW;	///< Pointer to DirectSoundEnumerateW function (Windows only)
	void *m_pGetDeviceID;			///< Pointer to GetDeviceID function (Windows only)
	void *m_pDirectSoundFullDuplexCreate;	///< Pointer to DirectSoundFullDuplexCreate function (Windows only)
	Word32 m_uQuickTimeVersion;		///< QuickTime's version in 0x0102 (1.2) format. (Windows only)
	Word32 m_uDirectXVersion;		///< DirectX version 0x0900 (9.0) format (Windows only)
	Word8 m_bQuickTimeVersionValid;	///< \ref TRUE if Quicktime's version is valid. (Windows only)
	Word8 m_bDirectXVersionValid;	///< \ref TRUE if DirectX's version is valid (Windows only)
	Word8 m_bWindow95;				///< Non-zero if tested, low bit has \ref TRUE or \ref FALSE for the state
	Word8 m_bPadding;				///< Pad to 32 bit alignment
#endif
	int m_iErrorCode;		///< Global default error code used by \ref Globals::Shutdown().
	Word m_uTraceFlags;		///< Debug information level
	char m_ErrorMsg[512];	///< Global Buffer containing the last fatal error or warning
	Bool m_bBombFlag;		///< \ref TRUE if non-fatal errors are treated as fatal
	Bool m_bExitFlag;		///< Global \ref TRUE if the app is in the process of shutting down.
	friend class Debug;
	static Globals g_Globals;	///< Singleton instance of the global variables
public:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	~Globals();
#endif
	enum {
		TRACE_MEMORYLEAKS=1,	///< Test and display memory leaks
		TRACE_REZLOAD=2,		///< Print the name of a resource file being loaded
		TRACE_FILELOAD=4,		///< Print the name of a file being loaded
		TRACE_WARNINGS=8,		///< Print possible errors
		TRACE_NETWORK=0x10,		///< Print the network commands
		TRACE_MESSAGES=0x20,	///< Print operating system messages
		TRACE_THEWORKS=0x3F		///< GIMME everything!
	};
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static BURGER_INLINE HINSTANCE__ *GetInstance(void) { return g_Globals.m_hInstance; }
	static BURGER_INLINE void SetInstance(HINSTANCE__ *pInput) { g_Globals.m_hInstance = pInput; }
	static BURGER_INLINE HWND__ *GetWindow(void) { return g_Globals.m_hWindow; }
	static BURGER_INLINE void SetWindow(HWND__ *pInput) { g_Globals.m_hWindow = pInput; }
	static Word BURGER_API IsDirectInputPresent(void);
	static Word BURGER_API IsDirectInput8Present(void);
	static Word BURGER_API IsDirectDrawPresent(void);
	static Word BURGER_API IsD3D9Present(void);
	static Word BURGER_API IsDirectSoundPresent(void);
	static Word BURGER_API IsWin95orWin98(void);
	static Word BURGER_API DirectInputCreateW(IDirectInputW **pOutput);
	static Word BURGER_API DirectInput8Create(IDirectInput8W **pOutput);
	static Word BURGER_API DirectDrawCreateExW(const GUID *pGuid,IDirectDraw7 **pOutput);
	static Word BURGER_API DirectDrawEnumerateExA(void *lpCallback,void *lpContext,Word32 dwFlags);
	static Word BURGER_API DirectDrawEnumerateExW(void *lpCallback,void *lpContext,Word32 dwFlags);
	static IDirect3D9 * BURGER_API Direct3DCreate9(Word uSDKVersion);
	static Word BURGER_API DirectSoundCreate(const GUID *pGuidDevice,IDirectSound **pOutput);
	static Word BURGER_API DirectSoundCreate8(const GUID *pGuidDevice,IDirectSound8 **pOutput);
	static Word BURGER_API GetQuickTimeVersion(void);
	static Word BURGER_API GetDirectXVersion(void);
	static Word BURGER_API GetVideoGUID(GUID *pOutput,Word uDevNum);
	static HINSTANCE__ * BURGER_API LoadLibraryA(const char *pInput);
	static HINSTANCE__ * BURGER_API LoadLibraryW(const Word16 *pInput);
	static Word BURGER_API AddGroupToProgramMenu(const char *pGroupName);
	static int BURGER_API CreateUserRegistryKey(const char *pKey,const char *pSubKey,const char *pData);
	static void BURGER_API AssociateFileExtensionToExe(const char *pFileExtension,const char *pDescription,const char *pProgramID);
#endif
	static BURGER_INLINE int GetErrorCode(void) { return g_Globals.m_iErrorCode; }
	static BURGER_INLINE void SetErrorCode(int iNewError) { g_Globals.m_iErrorCode = iNewError; }
	static BURGER_INLINE const char *GetErrorMsg(void) { return g_Globals.m_ErrorMsg; }
	static void BURGER_ANSIAPI SetErrorMsg(const char *pMessage,...);
	static BURGER_INLINE Word GetTraceFlag(void) { return g_Globals.m_uTraceFlags; }
	static BURGER_INLINE void SetTraceFlag(Word uNewFlag) { g_Globals.m_uTraceFlags = uNewFlag; }
	static BURGER_INLINE Word AreWarningsEnabled(void) { return g_Globals.m_uTraceFlags&TRACE_WARNINGS; }
	static BURGER_INLINE Word GetErrorBombFlag(void) { return g_Globals.m_bBombFlag; }
	static BURGER_INLINE Word SetErrorBombFlag(Word uNewFlag) { Word uOld = g_Globals.m_bBombFlag; g_Globals.m_bBombFlag = (uNewFlag!=0); return uOld; }
	static BURGER_INLINE Word GetExitFlag(void) { return g_Globals.m_bExitFlag; }
	static BURGER_INLINE void SetExitFlag(Word uNewFlag) { g_Globals.m_bExitFlag = (uNewFlag!=0); }
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
