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

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
namespace Burger {
class CodeLibrary {
    BURGER_DISABLE_COPY(CodeLibrary);
	void *m_pLibInstance;			///< Instance of the code library
public:
	CodeLibrary() : m_pLibInstance(nullptr) {}
	~CodeLibrary() { Shutdown(); }
	Word Init(const char *pFilename);
	void Shutdown(void);
	void *GetFunction(const char *pFunctionName);
	BURGER_INLINE Word IsInitialized(void) const { return m_pLibInstance!=nullptr; }
};

#if (defined(BURGER_MACCARBON) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

class CodeFramework {
    BURGER_DISABLE_COPY(CodeFramework);
	__CFBundle *m_pBundle;			///< Instance of the framework bundle
public:
	CodeFramework();
	~CodeFramework();
	eError BURGER_API Init(const char *pName);
	void BURGER_API Shutdown(void);
	void * BURGER_API GetFunction(const char *pFunctionName);
};

#endif

#if defined(BURGER_MACCARBON) || defined(DOXYGEN)
extern CodeLibrary* BURGER_API GetInterfaceLib(void);
extern CodeLibrary* BURGER_API GetDriverLoaderLib(void);
extern CodeLibrary* BURGER_API GetNameRegistryLib(void);
#endif
}

/* END */

#endif
