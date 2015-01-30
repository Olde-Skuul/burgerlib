/***************************************

	iOS application manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIOSAPP_H__
#define __BRIOSAPP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

/* BEGIN */
#if defined(BURGER_IOS) || defined(DOXYGEN)
namespace Burger {
class iOSApp : public GameApp {
	UIWindow *m_pWindow;					///< Pointer to the UIWindow for the iOS application
	UIViewController *m_pViewController;	///< Pointer to the UIViewController to the window
	UIView *m_pView;						///< Pointer to the UIView being used
	BURGER_DISABLECOPYCONSTRUCTORS(iOSApp);
	static iOSApp *g_piOSApp;				///< Global singular application instance
public:
	iOSApp(WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	~iOSApp();
	int BURGER_API Run(void);
	BURGER_INLINE UIWindow *GetWindow(void) const { return m_pWindow; }
	UIWindow *BURGER_API CreateWindow(void);
	void BURGER_API ReleaseWindow(void);
	UIViewController *GetViewController(void) const { return m_pViewController; }
	UIView *GetView(void) const { return m_pView; }
	void BURGER_API SetViewController(UIViewController *pViewController,UIView *pView);
	BURGER_INLINE static iOSApp *GetApp(void) { return g_piOSApp; }
	static RunQueue::eReturnCode BURGER_API Poll(void *pSelf);
};
}
#endif
/* END */

#endif
