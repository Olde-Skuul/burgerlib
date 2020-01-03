/***************************************

	Global variable manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRERROR_H__
#include "brerror.h"
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

#ifndef __BRCODELIBRARY_H__
#include "brcodelibrary.h"
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
	
private:
	static const Word16 g_GameClass[];		///< Windows class name "BurgerGameClass" (Windows only)
	static Word16 g_uAtom;					///< Atom assigned to my class (Windows only) 
	static HWND__ *g_hWindow;				///< Main window for the application (Windows only)
	static Word32 g_uQuickTimeVersion;		///< QuickTime's version in 0x0102 (1.2) format. (Windows only)
	static Word8 g_bQuickTimeVersionValid;	///< \ref TRUE if Quicktime's version is valid. (Windows only)
	static Word32 g_uDirectXVersion;		///< DirectX version 0x0900 (9.0) format (Windows only)
	static Word8 g_bDirectXVersionValid;	///< \ref TRUE if DirectX's version is valid (Windows only)

#if defined(BURGER_WIN32) || defined(DOXYGEN)
	static Word8 g_bIsWindows64Bit;				///< Non-zero if tested, low bit has \ref TRUE or \ref FALSE if the 32 bit app was running under 64 bit windows (Windows only)
#endif

#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
    /** Reference to InterfaceLib if needed (Mac only) */
	CodeLibrary m_Interface;		

    /** Reference to DriverLoaderLib if needed (Mac only) */
	CodeLibrary m_DriverLoader;		
#endif
	
#if defined(BURGER_MACOS) || defined(DOXYGEN)
	uint_t m_uMacOSVersion;					///< Discovered version of MacOS (MacOS only)
	uint_t m_uQuickTimeVersion;				///< QuickTime's version in 0x0102 (1.2) format. (MacOS only)
	uint8_t m_bMacOSTested;					///< MacOS version was tested (MacOS only)
	uint8_t m_bQuickTimeVersionValid;		///< \ref TRUE if Quicktime's version is valid. (MacOS only)
#endif

private:
	static eError g_iErrorCode;		///< Global default error code used by \ref Globals::Shutdown().
	static Word g_uTraceFlags;		///< Debug information level
	static char g_ErrorMsg[512];	///< Global Buffer containing the last fatal error or warning
	static Word g_bBombFlag;		///< \ref TRUE if non-fatal errors are treated as fatal
	static Word g_bExitFlag;		///< Global \ref TRUE if the app is in the process of shutting down.
	static Globals g_Globals;		///< Singleton instance of the global variables
public:

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
    static BURGER_INLINE const Word16 *GetWindowClassName(void) { return g_GameClass;}
    static BURGER_INLINE HWND__ *GetWindow(void) { return g_hWindow; }
    static BURGER_INLINE void SetWindow(HWND__ *pInput) { g_hWindow = pInput; }

#if defined(BURGER_WIN32) || defined(DOXYGEN)
	static Word BURGER_API IsWindows64Bit(void);
#else
    static BURGER_INLINE Word IsWindows64Bit(void) { return TRUE; }
#endif

	static void BURGER_API GetQTFolderFromRegistry(const char *pSubKey,const char *pValueName,char *pBuffer,Word32 uSize);
	static Word BURGER_API GetPathToQuickTimeFolder(char *pBuffer,Word32 uSize,Word32 *pReserved);
	static Word32 BURGER_API GetQTSystemDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTApplicationDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTExtensionDirectoryA(char *pBuffer,Word32 uSize);
	static Word32 BURGER_API GetQTComponentDirectoryA(char *pBuffer,Word32 uSize);
	static HINSTANCE__ * BURGER_API QTLoadLibrary(const char *pDLLName);

	static Word64 BURGER_API GetFileVersion64(const Word16* pWindowsFilename);
	static Word BURGER_API GetDirectXVersionViaFileVersions(void);
	static Word BURGER_API GetDirectXVersion(void);
	static Word BURGER_API GetVideoGUID(GUID *pOutput,Word uDevNum);
	static Word BURGER_API AddGroupToProgramMenu(const char *pGroupName);
	static int BURGER_API CreateUserRegistryKey(const char *pKey,const char *pSubKey,const char *pData);
	static void BURGER_API AssociateFileExtensionToExe(const char *pFileExtension,const char *pDescription,const char *pProgramID);
	static void BURGER_API OutputWindowsMessage(Word uMessage,WordPtr wParam,WordPtr lParam);
	static Word16 BURGER_API RegisterWindowClass(Word uIconResID=0);
	static void BURGER_API UnregisterWindowClass(void);
	static void BURGER_API PumpMessages(void);
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
    static CodeLibrary * BURGER_API GetInterfaceLib(void);
    static CodeLibrary * BURGER_API GetDriverLoaderLib(void);
    static int16_t BURGER_API GetSpecFromNthDesc(AEDesc *pList, long iIndex, FSSpec *pFSSpec);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	static void BURGER_API AddToMenubar(NSMenu *pNSMenu);
	static NSString *BURGER_API GetApplicationName(void);
	static void BURGER_API CreateApplicationMenu(void);
	static void BURGER_API CreateWindowMenu(void);
	static void BURGER_API CreateViewMenu(void);
	static void BURGER_API CreateHelpMenu(void);
	static void BURGER_API CreateDefaultMenus(void);
	static void BURGER_API GetHIDDeviceName(String *pOutput,__IOHIDDevice *pDevice);
	static __CFDictionary * BURGER_API CreateHIDDictionary(Word uPage,Word uUsage);
	static void GetDisplayName(String *pOutput,Word uDisplayID);
	static int NumberFromKey(const __CFDictionary *pDictionary,const char *pKey);
#if !defined(DOXYGEN)
	BURGER_INLINE static Word IsRunningUnderMacOSX(void) { return TRUE; }
#endif

#endif

#if defined(BURGER_MACOS) || defined(DOXYGEN)
	static uint_t BURGER_API GetMacOSVersion(void);
#endif

#if ((defined(BURGER_MAC) && TARGET_API_MAC_CARBON) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
	static void BURGER_API StringCopy(String *pOutput,const __CFString *pInput);
#endif

#if defined(BURGER_MACOS) || defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static uint_t BURGER_API GetQuickTimeVersion(void);
#endif

	static BURGER_INLINE eError GetErrorCode(void) BURGER_NOEXCEPT { return g_iErrorCode; }
	static BURGER_INLINE void SetErrorCode(eError iNewError) BURGER_NOEXCEPT { g_iErrorCode = iNewError; }
	static BURGER_INLINE char *GetErrorMsg(void) BURGER_NOEXCEPT { return g_ErrorMsg; }
	static void BURGER_ANSIAPI SetErrorMsg(const char *pMessage,...);
	static BURGER_INLINE uint_t GetTraceFlag(void) BURGER_NOEXCEPT { return g_uTraceFlags; }
	static BURGER_INLINE void SetTraceFlag(uint_t uNewFlag) BURGER_NOEXCEPT { g_uTraceFlags = uNewFlag; }
	static BURGER_INLINE uint_t AreWarningsEnabled(void) BURGER_NOEXCEPT { return g_uTraceFlags&TRACE_WARNINGS; }
	static BURGER_INLINE uint_t GetErrorBombFlag(void) BURGER_NOEXCEPT { return g_bBombFlag; }
	static BURGER_INLINE Word SetErrorBombFlag(uint_t uNewFlag)BURGER_NOEXCEPT { uint_t uOld = g_bBombFlag; g_bBombFlag = uNewFlag; return uOld; }
	static BURGER_INLINE uint_t GetExitFlag(void)BURGER_NOEXCEPT { return g_bExitFlag; }
	static BURGER_INLINE void SetExitFlag(uint_t uNewFlag) BURGER_NOEXCEPT { g_bExitFlag = uNewFlag; }
	static void BURGER_API Shutdown(void);
	static void BURGER_API Shutdown(int iError);
	static Word32 BURGER_API Version(void);
	static Word32 BURGER_API VersionBuild(void);
	static uint_t BURGER_API LaunchURL(const char *pURL);
	static int BURGER_API ExecuteTool(const char *pFilename,const char *pParameters,OutputMemoryStream *pOutput=NULL);
	static const char * BURGER_API GetEnvironmentString(const char *pKey);
	static Word BURGER_API SetEnvironmentString(const char *pKey,const char *pInput);
};
}
/* END */

#endif
