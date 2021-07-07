/***************************************

	Test the character encoding functions

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#include "brutf32.h"
#include "brutf8.h"
#include "brwin1252.h"
#include "brwin437.h"
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

static uint_t BURGER_API TestWin1252(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes, 1 through 255 with terminating zero
	char Buffer1_255[256];
	uintptr_t i = 0;
	do {
		Buffer1_255[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer1_255[255] = 0;

	// Perform the conversions using Burgerlib
	char BufferUTF8[512];
	const uintptr_t uUTF8Length = Burger::UTF8::FromWin1252(
		BufferUTF8, sizeof(BufferUTF8), Buffer1_255, 255);

	// Expected length of the conversion
	uint_t uTest = (uUTF8Length != 400);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win1252 to UTF8 yielded a different size! %u = Expected 400",
		uTest, static_cast<uint_t>(uUTF8Length));

	// Temp buffer, use a larger one in case of test failure
	char BufferWin1252[512];
	const uintptr_t uWin1252Length = Burger::Win1252::TranslateFromUTF8(
		BufferWin1252, sizeof(BufferWin1252), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uTest = (uWin1252Length != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win1252 to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uWin1252Length));

	// Verify it was converted back.
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(Buffer1_255, BufferWin1252, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win1252 to UTF8 yielded different data!", uTest);

	// Perform this test on Windows platforms using the API to ensure the values
	// are correct.
#if defined(BURGER_WINDOWS)

	// There is no call to convert 8 bit to 8 bit, so do a 16 bit conversion
	const int iRequiredSize =
		MultiByteToWideChar(1252, 0, Buffer1_255, 255, 0, 0);

	uTest = (iRequiredSize != 255);
	uFailure |= uTest;
	ReportFailure("TestWin1252() iRequiredSize size change %u = Expected 255",
		uTest, static_cast<uint_t>(iRequiredSize));

	WCHAR WideBuffer[512];
	const int iRequiredSize2 = MultiByteToWideChar(
		1252, 0, Buffer1_255, 255, WideBuffer, iRequiredSize);

	// Test ToUTF16Table
	uTest =
		Burger::MemoryCompare(WideBuffer + 127, Burger::Win1252::ToUTF16Table,
			sizeof(Burger::Win1252::ToUTF16Table)) != 0;
	uFailure |= uTest;
	ReportFailure(
		"Win1252::ToUTF16Table does not match 16 bit lookup table", uTest);

	// Convert to UTF-8
	const int iDestSize = WideCharToMultiByte(
		65001, 0, WideBuffer, iRequiredSize2, nullptr, 0, nullptr, nullptr);

	uTest = (iDestSize != 400);
	uFailure |= uTest;
	ReportFailure("TestWin1252() iDestSize size change %u = Expected 400",
		uTest, static_cast<uint_t>(iDestSize));

	char Buffer2[512];
	const int iDestSize2 = WideCharToMultiByte(65001, 0, WideBuffer,
		iRequiredSize2, Buffer2, iDestSize, nullptr, nullptr);

	// Test the result with what Burgerlib output
	uTest = (uUTF8Length != static_cast<uintptr_t>(iDestSize2));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win1252 to UTF8 yielded a different size! %u = Expected %u",
		uTest, static_cast<uint_t>(iDestSize2),
		static_cast<uint_t>(uWin1252Length));

	// Perform a binary compare to the UTF8 output
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(BufferUTF8, Buffer2, uUTF8Length));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win1252 to UTF8 yielded different data!",
		uTest);

	// Perform a test for conversion to lower case using Win1252
	WCHAR WideBufferCase[256];
	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharLowerBuffW(WideBufferCase, 255);
	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::Win1252::LowerCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::Win1252::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::Win1252::LowerCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));

	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharUpperBuffW(WideBufferCase, 255);

	// The character 0x192 'f' does not have the upper case version in Win1252,
	// but Unicode does. Undo the conversion to allow the unit test to pass,
	// since the table is only handling characters in Win1252 encoding.
	WideBufferCase[0x82] = 0x192;
	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::Win1252::UpperCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::Win1252::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::Win1252::UpperCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));
#endif

	return uFailure;
}

/***************************************

	Test MacRoman character set

***************************************/

static uint_t BURGER_API TestMacRoman(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes, 1 through 255 with terminating zero
	char Buffer1_255[256];
	uintptr_t i = 0;
	do {
		Buffer1_255[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer1_255[255] = 0;

	// Perform the conversions using Burgerlib
	char BufferUTF8[512];
	const uintptr_t uUTF8Length = Burger::UTF8::FromMacRomanUS(
		BufferUTF8, sizeof(BufferUTF8), Buffer1_255, 255);

	// Expected length of the conversion
	uint_t uTest = (uUTF8Length != 416);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from MacRomanUS to UTF8 yielded a different size! %u = Expected 416",
		uTest, static_cast<uint_t>(uUTF8Length));

	// Temp buffer, use a larger one in case of test failure
	char BufferMacRoman[512];
	const uintptr_t uMacRomanLength = Burger::MacRomanUS::TranslateFromUTF8(
		BufferMacRoman, sizeof(BufferMacRoman), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uTest = (uMacRomanLength != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from MacRoman to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uMacRomanLength));

	// Verify it was converted back.
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(Buffer1_255, BufferMacRoman, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from MacRoman to UTF8 yielded different data!", uTest);

	// Perform this test on macOS platforms
#if defined(BURGER_MACOSX)
	// Convert using the MacOSX calls
	CFStringRef StringRef = CFStringCreateWithBytes(
		nullptr, (UInt8*)Buffer1_255, 255, kCFStringEncodingMacRoman, false);

	// Convert to UTF-8
	char Buffer2[512];
	uintptr_t uBuffer2Size = 0;
	if (CFStringGetCString(
			StringRef, Buffer2, sizeof(Buffer2), kCFStringEncodingUTF8)) {
		// Success!
		uBuffer2Size = Burger::StringLength(Buffer2);
	}
	CFRelease(StringRef);

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
		"MacOSX conversion from MacRoman to UTF8 yielded different data!",
		uTest);
#endif

	return uFailure;
}

/***************************************

	Test Windows 28591 character set

***************************************/

static uint_t BURGER_API TestISOLatin1(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes, 1 through 255 with terminating zero
	char Buffer1_255[256];
	uintptr_t i = 0;
	do {
		Buffer1_255[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer1_255[255] = 0;

	// Perform the conversions using Burgerlib
	char BufferUTF8[512];
	const uintptr_t uUTF8Length = Burger::UTF8::FromISOLatin1(
		BufferUTF8, sizeof(BufferUTF8), Buffer1_255, 255);

	// Expected length of the conversion
	uint_t uTest = (uUTF8Length != 383);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from ISOLatin1 to UTF8 yielded a different size! %u = Expected 383",
		uTest, static_cast<uint_t>(uUTF8Length));

	// Temp buffer, use a larger one in case of test failure
	char BufferISOLatin1[512];
	const uintptr_t uISOLatin1Length = Burger::ISOLatin1::TranslateFromUTF8(
		BufferISOLatin1, sizeof(BufferISOLatin1), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uTest = (uISOLatin1Length != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from ISOLatin1 to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uISOLatin1Length));

	// Verify it was converted back.
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(Buffer1_255, BufferISOLatin1, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from ISOLatin1 to UTF8 yielded different data!", uTest);

	// Perform this test on Windows platforms using the API to ensure the values
	// are correct.
#if defined(BURGER_WINDOWS)

	// There is no call to convert 8 bit to 8 bit, so do a 16 bit conversion
	const int iRequiredSize =
		MultiByteToWideChar(28591, 0, Buffer1_255, 255, 0, 0);

	uTest = (iRequiredSize != 255);
	uFailure |= uTest;
	ReportFailure("TestISOLatin1() iRequiredSize size change %u = Expected 255",
		uTest, static_cast<uint_t>(iRequiredSize));

	WCHAR WideBuffer[512];
	const int iRequiredSize2 = MultiByteToWideChar(
		28591, 0, Buffer1_255, 255, WideBuffer, iRequiredSize);

	// Test ToUTF16Table
	uTest =
		Burger::MemoryCompare(WideBuffer + 127, Burger::ISOLatin1::ToUTF16Table,
			sizeof(Burger::ISOLatin1::ToUTF16Table)) != 0;
	uFailure |= uTest;
	ReportFailure(
		"ISOLatin1::ToUTF16Table does not match 16 bit lookup table", uTest);

	// Convert to UTF-8
	const int iDestSize = WideCharToMultiByte(
		65001, 0, WideBuffer, iRequiredSize2, nullptr, 0, nullptr, nullptr);
	char Buffer2[512];
	const int iDestSize2 = WideCharToMultiByte(65001, 0, WideBuffer,
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
		"Windows conversion from ISOLatin1 to UTF8 yielded different data!",
		uTest);

	// Perform a test for conversion to lower case using Win1252
	WCHAR WideBufferCase[256];
	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharLowerBuffW(WideBufferCase, 255);

	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::ISOLatin1::LowerCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::ISOLatin1::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::ISOLatin1::LowerCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));

	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharUpperBuffW(WideBufferCase, 255);

	// The character 0xFF 'y' with umlaut does not have the upper case version
	// in ISOLatin1, but Unicode does. Undo the conversion to allow the unit
	// test to pass, since the table is only handling characters in ISOLatin1
	// encoding.
	WideBufferCase[0xFEU] = 0xFF;

	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::ISOLatin1::UpperCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::ISOLatin1::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::ISOLatin1::UpperCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));
#endif
	return uFailure;
}

/***************************************

	Test Win437 character set

***************************************/

static uint_t BURGER_API TestWin437(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	// Generate all 8 bit codes, 1 through 255 with terminating zero
	char Buffer1_255[256];
	uintptr_t i = 0;
	do {
		Buffer1_255[i] = static_cast<char>(i + 1);
	} while (++i < 255);
	Buffer1_255[255] = 0;

	// Perform the conversions using Burgerlib
	char BufferUTF8[512];
	const uintptr_t uUTF8Length = Burger::UTF8::FromWin437(
		BufferUTF8, sizeof(BufferUTF8), Buffer1_255, 255);

	// Expected length of the conversion
	uint_t uTest = (uUTF8Length != 445);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win437 to UTF8 yielded a different size! %u = Expected 445",
		uTest, static_cast<uint_t>(uUTF8Length));

	// Temp buffer, use a larger one in case of test failure
	char BufferWin437[512];
	const uintptr_t uWin437Length = Burger::Win437::TranslateFromUTF8(
		BufferWin437, sizeof(BufferWin437), BufferUTF8, uUTF8Length);

	// Test the result from the round trip
	uTest = (uWin437Length != 255);
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win437 to UTF8 yielded a different size! %u = Expected 255",
		uTest, static_cast<uint_t>(uWin437Length));

	// Verify it was converted back.
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(Buffer1_255, BufferWin437, 255));
	uFailure |= uTest;
	ReportFailure(
		"Conversion from Win437 to UTF8 yielded different data!", uTest);

	// Perform this test on Windows platforms using the API to ensure the values
	// are correct.
#if defined(BURGER_WINDOWS)

	// There is no call to convert 8 bit to 8 bit, so do a 16 bit conversion
	const int iRequiredSize =
		MultiByteToWideChar(437, 0, Buffer1_255, 255, 0, 0);

	uTest = (iRequiredSize != 255);
	uFailure |= uTest;
	ReportFailure("TestWin437() iRequiredSize size change %u = Expected 255",
		uTest, static_cast<uint_t>(iRequiredSize));

	WCHAR WideBuffer[512];
	const int iRequiredSize2 = MultiByteToWideChar(
		437, 0, Buffer1_255, 255, WideBuffer, iRequiredSize);

	// Test ToUTF16Table
	uTest =
		Burger::MemoryCompare(WideBuffer + 127, Burger::Win437::ToUTF16Table,
			sizeof(Burger::Win437::ToUTF16Table)) != 0;
	uFailure |= uTest;
	ReportFailure(
		"Win437::ToUTF16Table does not match 16 bit lookup table", uTest);

	// Convert to UTF-8
	const int iDestSize = WideCharToMultiByte(
		65001, 0, WideBuffer, iRequiredSize2, nullptr, 0, nullptr, nullptr);

	uTest = (iDestSize != 445);
	uFailure |= uTest;
	ReportFailure("TestWin437() iDestSize size change %u = Expected 445", uTest,
		static_cast<uint_t>(iDestSize));

	char Buffer2[512];
	const int iDestSize2 = WideCharToMultiByte(65001, 0, WideBuffer,
		iRequiredSize2, Buffer2, iDestSize, nullptr, nullptr);

	// Test the result with what Burgerlib output
	uTest = (uUTF8Length != static_cast<uintptr_t>(iDestSize2));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win437 to UTF8 yielded a different size! %u = Expected %u",
		uTest, static_cast<uint_t>(iDestSize2),
		static_cast<uint_t>(uWin437Length));

	// Perform a binary compare to the UTF8 output
	uTest = static_cast<uint_t>(
		Burger::MemoryCompare(BufferUTF8, Buffer2, uUTF8Length));
	uFailure |= uTest;
	ReportFailure(
		"Windows conversion from Win437 to UTF8 yielded different data!",
		uTest);

	// Perform a test for conversion to lower case using Win437
	WCHAR WideBufferCase[256];
	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharLowerBuffW(WideBufferCase, 255);

	// Several characters are Greek letters used for math, both their lower case
	// counterparts don't exist in MSDOS, so they are reverted from the UTF-8
	// conversion.
	WideBufferCase[0xE1] = 0x393; // Gamma
	WideBufferCase[0xE8] = 0x398; // Theta
	WideBufferCase[0xE9] = 0x3A9; // Omega

	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::Win437::LowerCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::Win437::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::Win437::LowerCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));

	Burger::MemoryCopy(WideBufferCase, WideBuffer, 255 * sizeof(WCHAR));
	CharUpperBuffW(WideBufferCase, 255);

	// The character 0x192 'f' does not have the upper case version in Win437,
	// but Unicode does. Undo the conversion to allow the unit test to pass,
	// since the table is only handling characters in Win437 encoding.
	WideBufferCase[0x82] = 0xE2;  // A-Circumflex
	WideBufferCase[0x84] = 0xE0;  // A-Grave
	WideBufferCase[0x87] = 0xEA;  // E-Circumflex
	WideBufferCase[0x88] = 0xEB;  // E-Umlaut
	WideBufferCase[0x89] = 0xE8;  // E-Grave
	WideBufferCase[0x8A] = 0xEF;  // I-Umlaut
	WideBufferCase[0x8B] = 0xEE;  // I-Circumflex
	WideBufferCase[0x8C] = 0xEC;  // I-Grave
	WideBufferCase[0x92] = 0xF4;  // O-Circumflex
	WideBufferCase[0x94] = 0xF2;  // O-Grave
	WideBufferCase[0x95] = 0xFB;  // U-Circumflex
	WideBufferCase[0x96] = 0xF9;  // U-Grave
	WideBufferCase[0x97] = 0xFF;  // Y-Umlaut
	WideBufferCase[0x9E] = 0x192; // F with hook (Folder)
	WideBufferCase[0x9F] = 0xE1;  // A-Acute
	WideBufferCase[0xA0] = 0xED;  // I-Acute
	WideBufferCase[0xA1] = 0xF3;  // O-Acute
	WideBufferCase[0xA2] = 0xFA;  // U-Acute
	WideBufferCase[0xDF] = 0x3B1; // Alpha
	WideBufferCase[0xE2] = 0x3C0; // Pi
	WideBufferCase[0xE6] = 0x3C4; // Tau
	WideBufferCase[0xEA] = 0x3B4; // Delta
	WideBufferCase[0xED] = 0x3B5; // Epsilon
	uTest = FALSE;
	i = 0;
	do {
		WCHAR uTemp = Burger::Win437::UpperCaseTable[i + 1];
		if (uTemp >= 128) {
			uTemp = Burger::Win437::ToUTF16Table[uTemp - 128];
		}
		if (WideBufferCase[i] != uTemp) {
			uTest = TRUE;
			break;
		}
	} while (++i < 255);

	uFailure |= uTest;
	ReportFailure("Burger::Win437::UpperCaseTable[%u] is invalid!", uTest,
		static_cast<uint_t>(i + 1));
#endif

	return uFailure;
}

/***************************************

	Test UTF32::IsValid()

***************************************/

struct UTF8_UTF32_t {
	uint8_t m_UTF8[4]; // UTF8 version
	uint32_t m_UTF32;  // UTF32 version
};

static const UTF8_UTF32_t gIsValidTest[] = {
	{{0x00, 0x00, 0x00, 0x00}, 0x000000U}, // Zero
	{{0x72, 0x00, 0x00, 0x00}, 0x000072U}, // Capital R
	{{0x7F, 0x00, 0x00, 0x00}, 0x00007FU}, // Last 8 bit character
	{{0xC2, 0x80, 0x00, 0x00}, 0x000080U}, // First 2 byte character
	{{0xC3, 0xBF, 0x00, 0x00}, 0x0000FFU},
	{{0xC4, 0x80, 0x00, 0x00}, 0x000100U},
	{{0xC7, 0xBF, 0x00, 0x00}, 0x0001FFU},
	{{0xCF, 0xBF, 0x00, 0x00}, 0x0003FFU},
	{{0xD0, 0x80, 0x00, 0x00}, 0x000400U},
	{{0xDF, 0xBF, 0x00, 0x00}, 0x0007FFU}, // Last 2 byte character
	{{0xE0, 0xA0, 0x80, 0x00}, 0x000800U}, // First 3 byte character
	{{0xE1, 0x80, 0x80, 0x00}, 0x001000U},
	{{0xE4, 0x80, 0x80, 0x00}, 0x004000U},
	{{0xE7, 0xBF, 0xBF, 0x00}, 0x007FFFU},
	{{0xE8, 0x80, 0x80, 0x00}, 0x008000U},
	{{0xED, 0x9F, 0xBF, 0x00}, 0x00D7FFU},
	{{0xEE, 0x80, 0x80, 0x00}, 0x00E000U},
	{{0xEF, 0xBF, 0xBF, 0x00}, 0x00FFFFU}, // Last 3 byte character
	{{0xF0, 0x90, 0x80, 0x80}, 0x010000U}, // First 4 byte character
	{{0xF0, 0x9F, 0xA4, 0xA2}, 0x01F922U}, // Sad face emoji
	{{0xF4, 0x8F, 0xBF, 0xBF}, 0x10FFFFU}, // Last valid 4 byte character
	// These are invalid entries
	{{0xED, 0xA0, 0x80, 0x00}, 0x00D800U},
	{{0xED, 0xA4, 0x80, 0x00}, 0x00D900U},
	{{0xED, 0xBF, 0xBF, 0x00}, 0x00DFFFU},
	{{0xF4, 0x90, 0x80, 0x80}, 0x110000U},
	{{0xF7, 0xBF, 0xBF, 0xBF}, 0x1FFFFFU}, // Last 4 byte character
	{{0xF8, 0x00, 0x00, 0x00}, 0x200000U}, // Garbage UTF8
	{{0xFF, 0x00, 0x00, 0x00}, 0x200001U}};

static uint_t TestUTF32(void) BURGER_NOEXCEPT
{
	const UTF8_UTF32_t* pWork = gIsValidTest;
	uintptr_t uCounter = BURGER_ARRAYSIZE(gIsValidTest);
	uint_t uResult = FALSE;
	uint_t uMatch = TRUE;

	// Mini UTF32 string
	uint32_t TempString32[2];
	TempString32[1] = 0; // Force zero termination

	// Test UTF8 string
	char TempString[8];
	TempString[4] = 0; // Force zero termination

	do {
		//
		// Test IsValid
		//
		uint_t uTest = Burger::UTF32::IsValid(pWork->m_UTF32) != uMatch;
		uResult |= uTest;
		ReportFailure(
			"Burger::UTF32::IsValid(%08X) is invalid!", uTest, pWork->m_UTF32);

		TempString32[0] = pWork->m_UTF32;
		uTest = Burger::UTF32::IsValid(TempString32) != uMatch;
		uResult |= uTest;
		ReportFailure(
			"Burger::UTF32::IsValid(TempString32[0] %08X) is invalid!", uTest,
			pWork->m_UTF32);

		uTest = Burger::UTF32::IsValid(&pWork->m_UTF32, 1) != uMatch;
		uResult |= uTest;
		ReportFailure(
			"Burger::UTF32::IsValid(&pWork->m_UTF32 %08X) is invalid!", uTest,
			pWork->m_UTF32);

		//
		// Test TranslateFromUTF8
		//

		// Create a byte stream
		Burger::MemoryCopy(TempString, pWork->m_UTF8, 4);

		// Value to match to
		uint32_t uMatch32;
		if (!uMatch) {
			uMatch32 = Burger::UTF32::kInvalid;
		} else {
			// Test for a match
			uMatch32 = pWork->m_UTF32;
		}

		const uint32_t uTest32 = Burger::UTF32::TranslateFromUTF8(TempString);
		uTest = uTest32 != uMatch32;
		uResult |= uTest;
		ReportFailure(
			"Burger::UTF32::TranslateFromUTF8(TempString) %08X isn't %08X!",
			uTest, uTest32, pWork->m_UTF32);

		// Skip the null entry
		uintptr_t uTestX;
		if (pWork->m_UTF32) {
			uTestX = Burger::UTF32::TranslateFromUTF8(
				TempString32, BURGER_ARRAYSIZE(TempString32), TempString);
			// uMatch is 1 with values are valid, 0 when invalid.
			uTest = uTestX != uMatch;
			uResult |= uTest;
			ReportFailure(
				"Burger::UTF32::TranslateFromUTF8(TempString32, BURGER_ARRAYSIZE(TempString32), TempString) %u != 1 %08X!",
				uTest, static_cast<uint_t>(uTestX), pWork->m_UTF32);

			if (uTestX) {
				uTest = TempString32[0] != uMatch32;
				uResult |= uTest;
				ReportFailure(
					"Burger::UTF32::TranslateFromUTF8(TempString32, BURGER_ARRAYSIZE(TempString32), TempString) %08X %08X!",
					uTest, TempString32[0], pWork->m_UTF32);
			}
		}
		const uintptr_t uWidth = pWork->m_UTF8[3] ? 4 :
			pWork->m_UTF8[2]                      ? 3 :
			pWork->m_UTF8[1]                      ? 2 :
                                                    1;
		uTestX = Burger::UTF32::TranslateFromUTF8(TempString32,
			BURGER_ARRAYSIZE(TempString32),
			reinterpret_cast<const char*>(pWork->m_UTF8), uWidth);
		// uMatch is 1 with values are valid, 0 when invalid.
		uTest = uTestX != uMatch;
		uResult |= uTest;
		ReportFailure(
			"Burger::UTF32::TranslateFromUTF8(TempString32, BURGER_ARRAYSIZE(TempString32), TempString, uWidth) %u != 1 %08X!",
			uTest, static_cast<uint_t>(uTestX), pWork->m_UTF32);

		if (uTestX) {
			uTest = TempString32[0] != uMatch32;
			uResult |= uTest;
			ReportFailure(
				"Burger::UTF32::TranslateFromUTF8(TempString32, BURGER_ARRAYSIZE(TempString32), TempString, uWidth) %08X %08X!",
				uTest, TempString32[0], pWork->m_UTF32);
		}

		//
		// Test TranslateToUTF8()
		//

		char TestUTF8[8];
		uTestX = Burger::UTF32::TranslateToUTF8(TestUTF8, pWork->m_UTF32);
		if (uMatch) {
			uTest = uTestX != uWidth;
		} else {
			uTest = uTestX != 0;
		}
		uResult |= uTest;
		ReportFailure("Burger::UTF32::TranslateToUTF8(TestUTF8, %08X) = %08X!",
			uTest, pWork->m_UTF32, static_cast<uint_t>(uTestX));
		if (uTestX) {
			uTest = Burger::MemoryCompare(pWork->m_UTF8, TestUTF8, uWidth) != 0;
			uResult |= uTest;
			ReportFailure(
				"Burger::UTF32::TranslateToUTF8(TestUTF8, %08X) data mismatch!",
				uTest, pWork->m_UTF32);
		}

		TempString32[0] = pWork->m_UTF32;
		TempString32[1] = 0;
		uTestX = Burger::UTF32::TranslateToUTF8(TestUTF8, BURGER_ARRAYSIZE(TestUTF8), TempString32);
		if (uMatch && pWork->m_UTF32) {
			uTest = uTestX != uWidth;
		} else {
			uTest = uTestX != 0;
		}
		uResult |= uTest;
		ReportFailure("Burger::UTF32::TranslateToUTF8(TestUTF8, BURGER_ARRAYSIZE(TestUTF8), %08X) = %08X!",
			uTest, pWork->m_UTF32, static_cast<uint_t>(uTestX));
		if (uTestX) {
			uTest = Burger::MemoryCompare(pWork->m_UTF8, TestUTF8, uWidth) != 0;
			uResult |= uTest;
			ReportFailure(
				"Burger::UTF32::TranslateToUTF8(TestUTF8, %08X) data mismatch!",
				uTest, pWork->m_UTF32);
		}

		uTestX = Burger::UTF32::TranslateToUTF8(TestUTF8, BURGER_ARRAYSIZE(TestUTF8), TempString32, 1);
		if (uMatch) {
			uTest = uTestX != uWidth;
		} else {
			uTest = uTestX != 0;
		}
		uResult |= uTest;
		ReportFailure("Burger::UTF32::TranslateToUTF8(TestUTF8, BURGER_ARRAYSIZE(TestUTF8), 1, %08X) = %08X!",
			uTest, pWork->m_UTF32, static_cast<uint_t>(uTestX));
		if (uTestX) {
			uTest = Burger::MemoryCompare(pWork->m_UTF8, TestUTF8, uWidth) != 0;
			uResult |= uTest;
			ReportFailure(
				"Burger::UTF32::TranslateToUTF8(TestUTF8, %08X) data mismatch!",
				uTest, pWork->m_UTF32);
		}
		// Test to switch for testing from valid to invalid entries.
		if (pWork->m_UTF32 == 0x10FFFFU) {
			uMatch = FALSE;
		}
		++pWork;
	} while (--uCounter);
	return uResult;
}

/***************************************

	Perform the tests for the macros and compiler settings

***************************************/

int BURGER_API TestCharset(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Testing character set encoders");
	}

	uFailure = TestUTF32();
	uFailure |= TestWin1252();
	uFailure |= TestMacRoman();
	uFailure |= TestISOLatin1();
	uFailure |= TestWin437();

	return static_cast<int>(uFailure);
}
