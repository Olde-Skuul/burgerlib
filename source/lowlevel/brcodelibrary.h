/***************************************

	Code library (DLL) manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCODELIBRARY_H__
#define __BRCODELIBRARY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {
class CodeLibrary {
	BURGER_DISABLECOPYCONSTRUCTORS(CodeLibrary);
	void *m_pLibInstance;			///< Instance of the code library
public:
	CodeLibrary() : m_pLibInstance(NULL) {}
	~CodeLibrary() { Shutdown(); }
	Word Init(const char *pFilename);
	void Shutdown(void);
	void *GetFunction(const char *pFunctionName);
	BURGER_INLINE Word IsInitialized(void) const { return m_pLibInstance!=NULL; }
};
}

/* END */

#endif
