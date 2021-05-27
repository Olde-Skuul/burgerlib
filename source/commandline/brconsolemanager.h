/***************************************

	Console manager

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCONSOLEMANAGER_H__
#define __BRCONSOLEMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRMEMORYANSI_H__
#include "brmemoryansi.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

/* BEGIN */
namespace Burger {

class ConsoleApp: public Base {
	BURGER_DISABLE_COPY(ConsoleApp);
	BURGER_RTTI_IN_CLASS();

protected:
	/** Use ANSI memory manager for tools */
	MemoryManagerGlobalANSI m_ANSIMemoryManager;
	/** Arguments passed to the console */
	const char** m_ppArgv;
	/** Number of valid arguments */
	int m_iArgc;
	/** \ref TRUE if the application was launched by double clicking from a
	 * desktop */
	uint_t m_bLaunchedFromDesktop;

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	/** Saved pointer to the global __argv for restoring on exit */
	const char** m_ppOldArgv;
#endif

public:
	/** Callback for doing the application's work */
	typedef int(BURGER_API* CallbackProc)(ConsoleApp* pThis,
		const char* pInputFilename, const char* pOutputFilename);
	/** Callback for printing instructions */
	typedef void(BURGER_API* UsageProc)(ConsoleApp* pThis);

	ConsoleApp(int iArgc, const char** ppArgv, uint_t uFlags = 0);
	virtual ~ConsoleApp();
	BURGER_INLINE void SetArgv(const char** ppArgv) BURGER_NOEXCEPT
	{
		m_ppArgv = ppArgv;
	}
	BURGER_INLINE const char** GetArgv(void) const BURGER_NOEXCEPT
	{
		return m_ppArgv;
	}
	BURGER_INLINE void SetArgc(int iArgc) BURGER_NOEXCEPT
	{
		m_iArgc = iArgc;
	}
	BURGER_INLINE int GetArgc(void) const BURGER_NOEXCEPT
	{
		return m_iArgc;
	}
	uint_t BURGER_API WasDesktopLaunched(void) BURGER_NOEXCEPT;
	void BURGER_API PauseOnError(eError uError=kErrorGeneric);
	uint_t BURGER_API ProcessFilenames(CallbackProc pCallback);
	int BURGER_API InputAndOutput(CallbackProc pCallback, UsageProc pUsage);
	int BURGER_API SimpleTool(CallbackProc pCallback, UsageProc pUsage);
	eError BURGER_API SetWindowTitle(const char *pTitle);
	eError BURGER_API SetWindowSize(uint_t uWidth, uint_t uHeight);
};
}

/* END */

#endif
