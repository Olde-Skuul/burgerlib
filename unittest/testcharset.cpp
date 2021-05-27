/***************************************

	Test the character encoding functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testcharset.h"
#include "brisolatin1.h"
#include "brmacromanus.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"
#include "brutf8.h"
#include "brwin1252.h"
#include "common.h"
#include <stdio.h>

#if defined(BURGER_WINDOWS)
#include <Windows.h>
#endif

#if defined(BURGER_MACOSX)
#include <CoreFoundation/CFString.h>
#endif

/***************************************

	Test Win1252 character set

***************************************/

static uint_t BURGER_API TestWin1252(void)
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes
	char Buffer[256];
	uintptr_t i = 0;
	do {
		Buffer[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer[255] = 0;

	// Perform the conversions
	char BufferUTF8[512];
	uintptr_t uUTF8Length =
		Burger::UTF8::FromWin1252(BufferUTF8, sizeof(BufferUTF8), Buffer, 255);

	char BufferWin1252[512];
	uintptr_t uWin1252Length = Burger::Win1252::FromUTF8(
		BufferWin1252, sizeof(BufferWin1252), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uint_t uTest = (uWin1252Length != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win1252 to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uWin1252Length));

	uTest =
		static_cast<uint_t>(Burger::MemoryCompare(Buffer, BufferWin1252, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win1252 to UTF8 yielded a different data!", uTest);

	// Perform this test on Windows platforms
#if defined(BURGER_WINDOWS)
	// Convert using the Windows calls
	int iRequiredSize = MultiByteToWideChar(1252, 0, Buffer, 255, 0, 0);
	uint16_t WideBuffer[512];
	int iRequiredSize2 = MultiByteToWideChar(
		1252, 0, Buffer, 255, (LPWSTR)WideBuffer, iRequiredSize);

	// Convert to UTF-8
	int iDestSize = WideCharToMultiByte(65001, 0, (LPCWSTR)WideBuffer,
		iRequiredSize2, nullptr, 0, nullptr, nullptr);
	char Buffer2[512];
	int iDestSize2 = WideCharToMultiByte(65001, 0, (LPCWSTR)WideBuffer,
		iRequiredSize2, Buffer2, iDestSize, nullptr, nullptr);

	// Test the result with what Burgerlib output
	uTest = (uUTF8Length != static_cast<uintptr_t>(iDestSize2));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win1252 to UTF8 yielded a different size! %u = Expected %u",
		uTest, static_cast<uint_t>(iDestSize2),
		static_cast<uint_t>(uWin1252Length));

	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(BufferUTF8, Buffer2, uUTF8Length));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win1252 to UTF8 yielded a different data!",
		uTest);
#endif

	return uFailure;
}

/***************************************

	Test MacRoman character set

***************************************/

static uint_t BURGER_API TestMacRoman(void)
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes
	char Buffer[256];
	uintptr_t i = 0;
	do {
		Buffer[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer[255] = 0;

	// Perform the conversions
	char BufferUTF8[512];
	uintptr_t uUTF8Length = Burger::UTF8::FromMacRomanUS(
		BufferUTF8, sizeof(BufferUTF8), Buffer, 255);

	char BufferMacRoman[512];
	uintptr_t uMacRomanLength = Burger::MacRomanUS::FromUTF8(
		BufferMacRoman, sizeof(BufferMacRoman), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uint_t uTest = (uMacRomanLength != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from MacRoman to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uMacRomanLength));

	uTest =
		static_cast<uint_t>(Burger::MemoryCompare(Buffer, BufferMacRoman, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from MacRoman to UTF8 yielded a different data!", uTest);

	// Perform this test on macOS platforms
#if defined(BURGER_MACOSX)
	// Convert using the MacOSX calls
	CFStringRef StringRef = CFStringCreateWithBytes(
		nullptr, (UInt8*)Buffer, 255, kCFStringEncodingMacRoman, false);

	// Convert to UTF-8
	char Buffer2[512];
	uintptr_t uBuffer2Size = 0;
	if (CFStringGetCString(
			StringRef, Buffer2, sizeof(Buffer2), kCFStringEncodingUTF8)) {
		// Success!
		uBuffer2Size = Burger::StringLength(Buffer2);
	}

	// Test the result with what Burgerlib output
	uTest = (uUTF8Length != static_cast<uintptr_t>(uBuffer2Size));
	uFailure |= uTest;
	ReportFailure(
		"MacOSX conversion from MacRoman to UTF8 yielded a different size! %u = Expected %u",
		uTest, static_cast<uint_t>(uBuffer2Size),
		static_cast<uint_t>(uMacRomanLength));

	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(BufferUTF8, Buffer2, uUTF8Length));
	uFailure |= uTest;
	ReportFailure(
		"MacOSX conversion from MacRoman to UTF8 yielded a different data!",
		uTest);
#endif

	return uFailure;
}

/***************************************

	Test Win1252 character set

***************************************/

static uint_t BURGER_API TestISOLatin1(void)
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes
	char Buffer[256];
	uintptr_t i = 0;
	do {
		Buffer[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer[255] = 0;

	// Perform the conversions
	char BufferUTF8[512];
	uintptr_t uUTF8Length = Burger::UTF8::FromISOLatin1(
		BufferUTF8, sizeof(BufferUTF8), Buffer, 255);

	char BufferISOLatin1[512];
	uintptr_t uISOLatin1Length = Burger::ISOLatin1::FromUTF8(
		BufferISOLatin1, sizeof(BufferISOLatin1), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uint_t uTest = (uISOLatin1Length != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from ISOLatin1 to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uISOLatin1Length));

	uTest =
		static_cast<uint_t>(Burger::MemoryCompare(Buffer, BufferISOLatin1, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from ISOLatin1 to UTF8 yielded a different data!", uTest);

	// Perform this test on Windows platforms
#if defined(BURGER_WINDOWS)
	// Convert using the Windows calls
	int iRequiredSize = MultiByteToWideChar(28591, 0, Buffer, 255, 0, 0);
	uint16_t WideBuffer[512];
	int iRequiredSize2 = MultiByteToWideChar(
		28591, 0, Buffer, 255, (LPWSTR)WideBuffer, iRequiredSize);

	// Convert to UTF-8
	int iDestSize = WideCharToMultiByte(65001, 0, (LPCWSTR)WideBuffer,
		iRequiredSize2, nullptr, 0, nullptr, nullptr);
	char Buffer2[512];
	int iDestSize2 = WideCharToMultiByte(65001, 0, (LPCWSTR)WideBuffer,
		iRequiredSize2, Buffer2, iDestSize, nullptr, nullptr);

	// Test the result with what Burgerlib output
	uTest = (uUTF8Length != static_cast<uintptr_t>(iDestSize2));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from ISOLatin1 to UTF8 yielded a different size! %u = Expected %u",
		uTest, static_cast<uint_t>(iDestSize2),
		static_cast<uint_t>(uISOLatin1Length));

	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(BufferUTF8, Buffer2, uUTF8Length));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from ISOLatin1 to UTF8 yielded a different data!",
		uTest);
#endif
	return uFailure;
}

/***************************************

	Perform the tests for the macros and compiler settings

***************************************/

int BURGER_API TestCharset(uint_t uVerbose)
{
	uint_t uFailure = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Testing character set encoders");
	}

	uFailure = TestWin1252();
	uFailure |= TestMacRoman();
	uFailure |= TestISOLatin1();

	return static_cast<int>(uFailure);
}
