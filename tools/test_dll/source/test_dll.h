/***************************************

	DLL for testing

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __TEST_DLL_H__
#define __TEST_DLL_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS)
#define DLL_API extern "C" __declspec(dllexport)
#define DLL_CALL __cdecl
#elif defined(BURGER_CLANG) || defined(BURGER_GNUC)
#define DLL_API extern "C" __attribute__((visibility("default")))
#else
#define DLL_API extern "C"
#endif

#if defined(BURGER_X86) && defined(BURGER_WINDOWS)
#define DLL_CDECL __cdecl
#else
#define DLL_CDECL
#endif

DLL_API const char* DLL_CDECL test_string(void);
DLL_API uintptr_t DLL_CDECL test_add(uintptr_t a, uintptr_t b);

#endif
