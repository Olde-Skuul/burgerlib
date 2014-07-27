/***************************************

	Console manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

/* BEGIN */
namespace Burger {
class ConsoleApp : public Burger::Base {
	BURGER_DISABLECOPYCONSTRUCTORS(ConsoleApp);
	MemoryManagerGlobalANSI m_ANSIMemoryManager;	///< Use ANSI memory manager for tools
	const char **m_ppArgv;	///< Arguments passed to the console
	int m_iArgc;			///< Number of valid arguments
	Word m_bLaunchedFromDesktop;	///< \ref TRUE if the application was launched by double clicking from a desktop
public:
	typedef int (BURGER_API *CallbackProc)(ConsoleApp *pThis,const char *pInputFilename,const char *pOutputFilename);	///< Callback for doing the application's work
	typedef void (BURGER_API *UsageProc)(ConsoleApp *pThis); ///< Callback for printing instructions
	ConsoleApp(int iArgc,const char **ppArgv);
	~ConsoleApp();
	BURGER_INLINE void SetArgv(const char **ppArgv) { m_ppArgv=ppArgv; }
	BURGER_INLINE const char ** GetArgv(void) const { return m_ppArgv; }
	BURGER_INLINE void SetArgc(int iArgc) { m_iArgc=iArgc; }
	BURGER_INLINE int GetArgc(void) const { return m_iArgc; }
	BURGER_INLINE Word WasDesktopLaunched(void) const { return m_bLaunchedFromDesktop; }
	void BURGER_API PauseOnError(void) const;
	Word BURGER_API ProcessFilenames(Burger::ConsoleApp::CallbackProc pCallback);
	int BURGER_API InputAndOutput(Burger::ConsoleApp::CallbackProc pCallback,Burger::ConsoleApp::UsageProc pUsage);
	int BURGER_API SimpleTool(Burger::ConsoleApp::CallbackProc pCallback,Burger::ConsoleApp::UsageProc pUsage);
};
}

/* END */

#endif
