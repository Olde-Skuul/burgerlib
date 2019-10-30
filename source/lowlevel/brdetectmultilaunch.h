/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDETECTMULTILAUNCH_H__
#define __BRDETECTMULTILAUNCH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
namespace Burger {
class DetectMultiLaunch {
    BURGER_DISABLE_COPY(DetectMultiLaunch);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	void *m_hInstanceLock;			///< Windows handle to an instance lock (Windows only)
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	NSConnection *m_pConnection;		///< Global app connection (MacOSX only)
#endif
	
#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
public:
	DetectMultiLaunch();
	~DetectMultiLaunch();
	Word IsMultiLaunched(const char *pSignature);
#else

public:
	DetectMultiLaunch() {}
	BURGER_INLINE Word IsMultiLaunched(const char * /* pSignature */) { return FALSE; }
#endif
};
}
/* END */

#endif
