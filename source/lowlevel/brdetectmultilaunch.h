/***************************************

	Detect multiple launches class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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
	BURGER_DISABLE_COPY(DetectMultiLaunch);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	/** Windows handle to an instance lock (Windows only) */
	void* m_hInstanceLock;
#endif

#if defined(BURGER_MACOSX) || defined(BURGER_LINUX) || defined(DOXYGEN)
	/** Name of the sem_t record (MacOSX/Linux only) */
	char* m_pName;
#endif

public:
#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || \
	defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
	DetectMultiLaunch() BURGER_NOEXCEPT;
	~DetectMultiLaunch();
	uint_t is_multi_launched(const char* pSignature) BURGER_NOEXCEPT;
#else
	DetectMultiLaunch() BURGER_NOEXCEPT {}
	BURGER_INLINE uint_t is_multi_launched(
		const char* /* pSignature */) BURGER_NOEXCEPT
	{
		return FALSE;
	}
#endif
};

}
/* END */

#endif
