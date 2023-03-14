/***************************************

	Unit tests for CodeLibrary

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrcodelibrary.h"
#include "brcodelibrary.h"
#include "brfilename.h"
#include "common.h"

using namespace Burger;

#if defined(BURGER_LINUX)
static const char g_DLLPath[] = "linux:libtest_dllmaklnxrel.so";
#elif defined(BURGER_MACOSX)
static const char g_DLLPath[] = "macosx:libtest_dll.dylib";
#elif defined(BURGER_WINDOWS)
#if defined(BURGER_X86)
static const char g_DLLPath[] = "windows:test_dllv22w32ltc.dll";
#elif defined(BURGER_AMD64)
static const char g_DLLPath[] = "windows:test_dllv22w64ltc.dll";
#elif defined(BURGER_ARM32)
static const char g_DLLPath[] = "windows:test_dllv22wina32ltc.dll";
#elif defined(BURGER_ARM64)
static const char g_DLLPath[] = "windows:test_dllv22wina64ltc.dll";
#else
#error Unknown Windows CPU
#endif
#else
#define NOT_SUPPORTED
#endif

#if defined(BURGER_X86) && defined(BURGER_WINDOWS)
#define DLL_CDECL __cdecl
#else
#define DLL_CDECL
#endif

#if !defined(NOT_SUPPORTED)
static const char g_ROOTPath[] = "9:..:..:data:test_dlls";

/***************************************

	Test CodeLibrary

***************************************/

static uint_t BURGER_API TestLoadDLL(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	// Get the pathname to the library
	Filename MyName(g_ROOTPath);
	MyName.join(g_DLLPath);

	// Load in the library
	CodeLibrary MyLib;
	const eError uError = MyLib.init(MyName.c_str());
	if (uError) {
		uFailure = TRUE;
		const char *pError = error_lookup_string(uError);
		ReportFailure("CodeLibrary::init(\"%s\") returned %s", TRUE,
			MyName.c_str(), pError);
	} else {

		// The library was loaded, test for the functions
		void* p = MyLib.get_function("test_string");
		if (!p) {
			uFailure = TRUE;
			ReportFailure(
				"CodeLibrary::get_function(\"test_string\") returned nullptr",
				TRUE);
		} else {
			const char* pString =
				reinterpret_cast<const char*(DLL_CDECL*)(void)>(p)();
			if (StringCompare(pString, "test_dll")) {
				uFailure = TRUE;
				ReportFailure(
					"CodeLibrary::get_function(\"test_string\") returned \"%s\"",
					TRUE, pString);
			}
		}
		p = MyLib.get_function("test_add");
		if (!p) {
			uFailure = TRUE;
			ReportFailure(
				"CodeLibrary::get_function(\"test_add\") returned nullptr",
				TRUE);
		} else {
			const uintptr_t uAnswer =
				reinterpret_cast<uintptr_t(DLL_CDECL*)(uintptr_t, uintptr_t)>(
					p)(12, 43);
			if (uAnswer != (12 + 43)) {
				uFailure = TRUE;
				ReportFailure(
					"CodeLibrary::get_function(\"test_add\") returned \"%u\"",
					TRUE, static_cast<uint_t>(uAnswer));
			}
		}
		MyLib.shutdown();
	}
	return uFailure;
}

#endif

/***************************************

	Test CodeLibrary

***************************************/

int BURGER_API TestBrCodeLibrary(uint_t uVerbose) BURGER_NOEXCEPT
{
#if !defined(NOT_SUPPORTED)
	uint_t uResult = 0;
	if (uVerbose & VERBOSE_DLL) {
		if (uVerbose & VERBOSE_MSG) {
			Message("Running DLL tests");
		}

		uResult = TestLoadDLL();

		if (!uResult && (uVerbose & VERBOSE_MSG)) {
			Message("Passed all DLL tests!");
		}
	}
	return static_cast<int>(uResult);
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}
