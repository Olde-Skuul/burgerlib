/***************************************

	DLL for testing

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "test_dll.h"

DLL_API const char* DLL_CDECL test_string(void)
{
	return "test_dll";
}

DLL_API uintptr_t DLL_CDECL test_add(uintptr_t a, uintptr_t b)
{
	return a + b;
}