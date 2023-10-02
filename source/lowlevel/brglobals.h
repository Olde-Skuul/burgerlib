/***************************************

	Global variable manager

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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

#ifndef __BRSTRING_H__
#include "brstring.h"
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

enum eTraceFlags {
	/** Test and display memory leaks */
	kTraceFlagMemoryLeak = 1,
	/** Print the name of a resource file being loaded */
	kTraceFlagRezLoad = 2,
	/** Print the name of a file being loaded */
	kTraceFlagFileLoad = 4,
	/** Print possible errors */
	kTraceFlagRaceWarnings = 8,
	/** Print the network commands */
	kTraceFlagNetwork = 0x10,
	/** Print operating system messages */
	kTraceFlagMessages = 0x20,
	/** Actively debugging the code (NEVER SET THIS for applications sent to
	   testers or release) */
	kTraceFlagActiveDebugging = 0x40,
	/** GIVE ME everything! */
	kTraceFlagAll = 0x7F
};

extern uint32_t BURGER_API get_traceflags(void) BURGER_NOEXCEPT;
extern void BURGER_API set_traceflags(uint32_t uTraceFlags) BURGER_NOEXCEPT;

enum eHintFlags {
	/** Disable the throwing of an exception to name a thread */
	kHintWin32DisableThreadNamingException = 0x01
};

extern uint32_t BURGER_API get_hintflags(void) BURGER_NOEXCEPT;
extern void BURGER_API set_hintflags(uint32_t uHintFlags) BURGER_NOEXCEPT;

class OutputMemoryStream;
class String;

class Globals {
public:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

private:
	/** Atom assigned to my class (Windows only) */
	static uint16_t g_uAtom;
	/** QuickTime's version in 0x0102 (1.2) format. (Windows only) */
	static uint32_t g_uQuickTimeVersion;
	/** \ref TRUE if Quicktime's version is valid. (Windows only) */
	static uint8_t g_bQuickTimeVersionValid;
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	/** Discovered version of MacOS (MacOS only) */
	uint_t m_uMacOSVersion;
	/** QuickTime's version in 0x0102 (1.2) format. (MacOS only) */
	uint_t m_uQuickTimeVersion;
	/** MacOS version was tested (MacOS only) */
	uint8_t m_bMacOSTested;
	/** \ref TRUE if Quicktime's version is valid. (MacOS only) */
	uint8_t m_bQuickTimeVersionValid;
#endif

private:
	/** Global default error code used by \ref Globals::Shutdown(). */
	static eError g_iErrorCode;
	/** Global Buffer containing the last fatal error or warning */
	static char g_ErrorMsg[512];
	/** \ref TRUE if non-fatal errors are treated as fatal */
	static uint_t g_bBombFlag;
	/** Global \ref TRUE if the app is in the process of shutting down. */
	static uint_t g_bExitFlag;
	/** Singleton instance of the global variables */
	static Globals g_Globals;

public:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static uint16_t BURGER_API RegisterWindowClass(uint_t uIconResID = 0);
	static void BURGER_API UnregisterWindowClass(void);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	static void BURGER_API AddToMenubar(NSMenu* pNSMenu);
	static NSString* BURGER_API GetApplicationName(void);
	static void BURGER_API CreateApplicationMenu(void);
	static void BURGER_API CreateWindowMenu(void);
	static void BURGER_API CreateViewMenu(void);
	static void BURGER_API CreateHelpMenu(void);
	static void BURGER_API CreateDefaultMenus(void);
	static void BURGER_API GetHIDDeviceName(
		String* pOutput, __IOHIDDevice* pDevice);
	static __CFDictionary* BURGER_API CreateHIDDictionary(
		uint_t uPage, uint_t uUsage);
	static void GetDisplayName(String* pOutput, uint_t uDisplayID);
	static int NumberFromKey(
		const __CFDictionary* pDictionary, const char* pKey);
#endif

#if defined(BURGER_MACOSX) || defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static uint_t BURGER_API GetQuickTimeVersion(void) BURGER_NOEXCEPT;
#endif

	static BURGER_INLINE eError GetErrorCode(void) BURGER_NOEXCEPT
	{
		return g_iErrorCode;
	}
	static BURGER_INLINE void SetErrorCode(eError iNewError) BURGER_NOEXCEPT
	{
		g_iErrorCode = iNewError;
	}
	static BURGER_INLINE char* GetErrorMsg(void) BURGER_NOEXCEPT
	{
		return g_ErrorMsg;
	}
	static void BURGER_ANSIAPI SetErrorMsg(const char* pMessage, ...);
	static BURGER_INLINE uint_t AreWarningsEnabled(void) BURGER_NOEXCEPT
	{
		return get_traceflags() & kTraceFlagRaceWarnings;
	}
	static BURGER_INLINE uint_t GetErrorBombFlag(void) BURGER_NOEXCEPT
	{
		return g_bBombFlag;
	}
	static BURGER_INLINE uint_t SetErrorBombFlag(
		uint_t uNewFlag) BURGER_NOEXCEPT
	{
		const uint_t uOld = g_bBombFlag;
		g_bBombFlag = uNewFlag;
		return uOld;
	}
	static BURGER_INLINE uint_t GetExitFlag(void) BURGER_NOEXCEPT
	{
		return g_bExitFlag;
	}
	static BURGER_INLINE void SetExitFlag(uint_t uNewFlag) BURGER_NOEXCEPT
	{
		g_bExitFlag = uNewFlag;
	}
	static void BURGER_API Shutdown(void);
	static void BURGER_API Shutdown(int iError);
	static uint32_t BURGER_API Version(void);
	static uint32_t BURGER_API VersionBuild(void);
	static uint_t BURGER_API LaunchURL(const char* pURL);
	static int BURGER_API ExecuteTool(const char* pFilename,
		const char* pParameters, OutputMemoryStream* pOutput = nullptr);
};

extern const char* BURGER_API GetEnvironmentString(
	const char* pKey) BURGER_NOEXCEPT;
extern eError BURGER_API SetEnvironmentString(
	const char* pKey, const char* pInput) BURGER_NOEXCEPT;
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || defined(BURGER_LINUX)
extern uint_t BURGER_API IsElevated(void) BURGER_NOEXCEPT;
#else
BURGER_INLINE uint_t IsElevated(void) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS) || defined(BURGER_MSDOS)
	return TRUE;
#else
	return FALSE;
#endif
}
#endif
}
/* END */

#endif
