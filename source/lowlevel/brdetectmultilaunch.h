/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

/* BEGIN */
namespace Burger {
class DetectMultiLaunch {
	BURGER_DISABLECOPYCONSTRUCTORS(DetectMultiLaunch);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	void *m_hInstanceLock;			///< Windows handle to an instance lock (Windows only)
public:
	DetectMultiLaunch();
	~DetectMultiLaunch();
	Word IsMultiLaunched(const char *pSignature);
#else
public:
	BURGER_INLINE Word IsMultiLaunched(const char * /* pSignature */) { return FALSE; }
#endif
};
}
/* END */

#endif
