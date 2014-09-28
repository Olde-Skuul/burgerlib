/***************************************

	Code library (DLL) manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HINSTANCE__ *m_pLibInstance;		///< Instance of the system shared library (Windows only)
#endif
#if defined(BURGER_MAC) || defined(DOXYGEN)
	OpaqueCFragConnectionID *m_pLibInstance;	///< Instance of the code fragment library (MacOS only)
#endif
#if (defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	void *m_pLibInstance;			///< Instance of the code library (Android/MacOSX/iOS only)
#endif
public:
#if (defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	CodeLibrary() : m_pLibInstance(NULL) {}
	~CodeLibrary() { Shutdown(); }
#endif
	Word Init(const char *pFilename);
	void Shutdown(void);
	void *GetFunction(const char *pFunctionName);
#if (defined(BURGER_WINDOWS) || defined(BURGER_MAC) || defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	BURGER_INLINE Word IsInitialized(void) const { return m_pLibInstance!=NULL; }
#else
	BURGER_INLINE Word IsInitialized(void) const { return FALSE; }
#endif
};
}

/* END */

#endif
