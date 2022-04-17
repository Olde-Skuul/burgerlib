/***************************************

	Code library (DLL) manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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

	/** Instance of the code library */
	void* m_pLibInstance;

public:
	CodeLibrary() BURGER_NOEXCEPT: m_pLibInstance(nullptr) {}
	~CodeLibrary()
	{
		Shutdown();
	}
	uint_t Init(const char* pFilename) BURGER_NOEXCEPT;
	void Shutdown(void) BURGER_NOEXCEPT;
	void* GetFunction(const char* pFunctionName) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t IsInitialized(void) const BURGER_NOEXCEPT
	{
		return m_pLibInstance != nullptr;
	}
};

#if (defined(BURGER_MACCARBON) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

class CodeFramework {
	BURGER_DISABLE_COPY(CodeFramework);

	/** Instance of the framework bundle */
	__CFBundle* m_pBundle;

public:
	CodeFramework() BURGER_NOEXCEPT;
	~CodeFramework();
	eError BURGER_API Init(const char* pName) BURGER_NOEXCEPT;
	void BURGER_API Shutdown(void) BURGER_NOEXCEPT;
	void* BURGER_API GetFunction(const char* pFunctionName) BURGER_NOEXCEPT;
};

#endif

#if defined(BURGER_POWERPC) || defined(DOXYGEN)
extern CodeLibrary* BURGER_API GetInterfaceLib(void) BURGER_NOEXCEPT;
extern CodeLibrary* BURGER_API GetDriverLoaderLib(void) BURGER_NOEXCEPT;
extern CodeLibrary* BURGER_API GetNameRegistryLib(void) BURGER_NOEXCEPT;
#endif
}

/* END */

#endif
