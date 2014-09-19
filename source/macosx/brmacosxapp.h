/***************************************

	MacOSX application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACOSXAPP_H__
#define __BRMACOSXAPP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
namespace Burger {
class MacOSXApp : public GameApp {
	BURGER_DISABLECOPYCONSTRUCTORS(MacOSXApp);
	NSApplication *m_pApplication;			///< Pointer to the current application context (Copy of NSApp)
	NSApplicationDelegate *m_pApplicationDelegate;	///< Pointer to the current application delegate
	NSWindow *m_pWindow;					///< Game window
	Word m_bCenterWindow;					///< \ref TRUE if the window needs to be centered
	static RunQueue::eReturnCode BURGER_API EventPoll(void *pData);
public:
	MacOSXApp(WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	~MacOSXApp();
	void BURGER_API FocusWindow(void);
	Word BURGER_API SetWindowSize(Word uWidth,Word uHeight);
	BURGER_INLINE NSApplication *GetApplication(void) const { return m_pApplication; }
	BURGER_INLINE NSApplicationDelegate *GetDelegate(void) const { return m_pApplicationDelegate; }
	BURGER_INLINE NSWindow *GetWindow(void) const { return m_pWindow; }
};
}
#endif
/* END */

#endif
