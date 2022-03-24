/***************************************

	Unit tests for the printf library

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

// ================================================================================================
// *************************************** mprintf's license
// **************************************
// ================================================================================================
//
// mprintf uses the (very permissive) open source ZLIB license:
//
// http://opensource.org/licenses/Zlib
//
// License text for mprintf.h:
//
// Copyright (c) 2012-2013 Matt Pritchard
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the
//   author(s) be held liable for any damages arising from the use of this
//   software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial
//   applications, and to alter it and redistribute it freely, subject to the
//   following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the
//     original software. If you use this software in a product, an
//     acknowledgment in the product documentation would be appreciated but is
//     not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as
//     being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//

#include "testbrprintf.h"
#include "brassert.h"
#include "brfpinfo.h"
#include "brmemoryfunctions.h"
#include "brmetrowerks.h"
#include "brnumberstring.h"
#include "brprintf.h"
#include "brstringfunctions.h"
#include "brstructs.h"
#include "brtick.h"
#include "brutf8.h"
#include "common.h"

#if defined(BURGER_MSVC)
// Disable conditional expression is constant
#pragma warning(disable : 4127)

// Available with Visual Studio 2005 and higher
#if (BURGER_MSVC >= 140000000)
// Disable potential comparison of a constant with another constant
#pragma warning(disable : 6326)
#endif
#endif

#if defined(BURGER_WATCOM)
// Integral constant will be truncated during assignment
#pragma warning 388 9
#endif

#define DOFLOATDECOMPTEST

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)
#define snprintf _snprintf
#elif defined(BURGER_METROWERKS) && !defined(__MSL__)
#include <stdarg.h>
static int snprintf(char* s, uintptr_t uSize, const char* format, ...)
{
	BURGER_UNUSED(uSize);
	va_list Args;
	va_start(Args, format);
	int iResult = vsprintf(s, format, Args);
	va_end(Args);
	return iResult;
}
#endif

static Burger::SafePrintArgument::eType g_CharARG = static_cast<char>(-1) < 0 ?
    Burger::SafePrintArgument::ARG_INT8 :
    Burger::SafePrintArgument::ARG_UINT8;

#if defined(BURGER_HAS_WCHAR_T)
static Burger::SafePrintArgument::eType g_WCharARG =
	static_cast<wchar_t>(-1) < 0 ? Burger::SafePrintArgument::ARG_INT16 :
                                   Burger::SafePrintArgument::ARG_UINT16;
#endif

#if defined(BURGER_HAS_CHAR8_T)
static Burger::SafePrintArgument::eType g_Char8ARG =
	static_cast<char8_t>(-1) < 0 ? Burger::SafePrintArgument::ARG_INT8 :
                                   Burger::SafePrintArgument::ARG_UINT8;
#endif

#if defined(BURGER_HAS_CHAR16_T)
static Burger::SafePrintArgument::eType g_Char16ARG =
	static_cast<char16_t>(-1) < 0 ? Burger::SafePrintArgument::ARG_INT16 :
                                    Burger::SafePrintArgument::ARG_UINT16;
static Burger::SafePrintArgument::eType g_Char32ARG =
	static_cast<char32_t>(-1) < 0 ? Burger::SafePrintArgument::ARG_INT32 :
                                    Burger::SafePrintArgument::ARG_UINT32;
#endif

// ======================================================================================
// Special values for 32-bit floats
#if 0
static const uint32_t g_FloatSpecialConstants[] = {
    0x00000000, 0x80000000, // Zero and -Zero
    0x7F800000, 0xFF800000, // Infinity, -Infinity

    // Note: Some functions expect signaling NAN values are at index 4
    0x7F800001, 0xFF800001, // NAN, -NAN (smallest signaling)
    0x7F80FFFF, 0xFF80FFFF, // NAN, -NAN (signaling)
    0x7FBFFFFF, 0xFFBFFFFF, // NAN, -NAN (largest signaling)

    // Note: Quiet NANs are at index 10
    0x7FC00000, 0xFFC00000, // NAN, -NAN (smallest quiet NaN, indeterminate NAN)
    0x7FC00001, 0xFFC00001, // NAN, -NAN (next smallest quiet NaN)
    0x7FC0F0F0, 0xFFC0F0F0, // NAN, -NAN (quiet)
    0x7FFFFFFF, 0xFFFFFFFF, // NAN, -NAN (largest)

    0x00000001, 0x80000001, // smallest subnormals
    0x00000002, 0x80000002, // smallest subnormals
    0x000F0F0F, 0x800F0F0F, // subnormals
    0x007FFFFF, 0x807FFFFF  // largest subnormals
};
#endif

// ======================================================================================
// Special values for 64-bit doubles
#if 0
static const uint64_t g_DoubleSpecialConstants[] = {
    0x0000000000000000ULL, 0x8000000000000000ULL, // Zero and -Zero

    0x7FF0000000000000ULL, 0xFFF0000000000000ULL, // Infinity, -Infinity
    0x7FF0000000000001ULL,
    0xFFF0000000000001ULL, // NAN, -NAN (smallest signaling)
    0x7FF0000FFFFFFFFFULL, 0xFFF000FFFFFFFFFFULL, // NAN, -NAN (signaling)
    0x7FF7FFFFFFFFFFFFULL,
    0xFFF7FFFFFFFFFFFFULL, // NAN, -NAN (largest signaling)
    0x7FF8000000000000ULL,
    0xFFF8000000000000ULL, // NAN, -NAN (smallest quiet NaN, indeterminate NAN)
    0x7FF8000000000001ULL,
    0xFFF8000000000001ULL, // NAN, -NAN (next smallest quiet NaN)
    0x7FF80F0F0F0F0F0FULL, 0xFFF80F0F0F0F0F0FULL, // NAN, -NAN (quiet)
    0x7FFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, // NAN, -NAN (largest)

    0x0000000000000001ULL, 0x8000000000000001ULL, // smallest subnormals
    0x0000000000000002ULL, 0x8000000000000002ULL, // smallest subnormals
    0x00000000F0F0F0F0ULL, 0x80000000F0F0F0F0ULL, // subnormals
    0x000FFFFFFFFFFFFFULL, 0x800FFFFFFFFFFFFFULL  // largest subnormals
};
#endif

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
struct FloatTestSet_t {
	float m_fFloatValue;
	uint_t m_uMaxPrecision;
};

static const FloatTestSet_t g_FloatConstants[] = {
	{1.0f, 10}, {10.0f, 2}, {0.1f, 10}, {0.125f, 10}, {0.0001f, 10},
	{0.00001f, 10}, {0.000001f, 10}, {0.0000001f, 10}, {0.00000001f, 10},
	{100.0f, 2}, {99.999f, 10}, {9.9999999f, 12}, {1e+12f, 1},
	{5.09823e+27f, 1}, {4.567324e+3f, 8}, {3.402823466e+38f, 1}, // FLT_MAX
	{1.175494351e-38f, 17}                                       // FLT_MIN
};

struct DoubleTestSet_t {
	double m_doubleVal;
	uint_t m_MaxPrecision;
};

static const DoubleTestSet_t g_DoubleConstants[] = {
	{1.0, 10}, {10.0, 2}, {0.1, 10}, {0.125, 10}, {0.0001, 10}, {0.00001, 10},
	{0.000001, 10}, {0.0000001, 10}, {0.00000001, 11}, {0.000000001, 12},
	{0.0000000001, 13}, {0.00000000001, 14}, {0.000000000001, 15},
	{0.0000000000001, 16}, {0.00000000000001, 17}, {0.000000000000001, 18},
	{0.0000000000000001, 19}, {100.0, 2}, {99.999, 10}, {9.9999999, 12},
	{999999.9999999, 10}, {9.9999999999999999, 18},

	{9999999999999999999999.0, 22}, {99999999999999900000.0, 20},
	{99999999999999990000.0, 20}, {99999999999999999000.0, 20},
	{99999999999999999900.0, 20},

	{0.99999999999999900000, 20}, {0.99999999999999990000, 20},
	{0.99999999999999999000, 20}, {0.99999999999999999900, 20},

	{9.99999999999999900000, 20}, {9.99999999999999990000, 20},
	{9.99999999999999999000, 20}, {9.99999999999999999900, 20},

	{99.99999999999999000000, 20}, {99.99999999999999900000, 20},
	{99.99999999999999990000, 20}, {99.99999999999999999000, 20},
	{99.99999999999999999900, 20},

	{1823457.392831, 8},

	{1e+12, 1}, {5.09823e+27, 1}, {4.567324e+3, 8},
	{1.7976931348623157e+308, 1}, // DBL_MAX
	{2.2250738585072014e-308, 25} // DBL_MIN
};
#endif

#if 0
static const double g_NineTests[] = {9999999999999999999999.0,

    99999999999999900000.0, 99999999999999990000.0, 99999999999999999000.0,
    99999999999999999900.0,

    0.99999999999999900000, 0.99999999999999990000, 0.99999999999999999000,
    0.99999999999999999900,

    9.99999999999999900000, 9.99999999999999990000, 9.99999999999999999000,
    9.99999999999999999900,

    99.99999999999999000000, 99.99999999999999900000, 99.99999999999999990000,
    99.99999999999999999000, 99.99999999999999999900};
#endif

/***************************************

	Output a float

***************************************/

struct ExpectedFPPrintInfoState_t {
	uint_t m_uSpecialFormDigits;
	uint_t m_bHasInteger;
	uint_t m_uFirstNonZero;
	uint_t m_uLastNonZero;
	uint_t m_bHasFraction;
	uint_t m_uFirstNonZeroFraction;
	uint_t m_uLastNonZeroFraction;
	Burger::FPPrintInfo::eResult m_Result;
};

struct TestDecompDouble_t {
	Burger::uint64_double_t m_Value;
	uint_t m_uDigits;
	ExpectedFPPrintInfoState_t m_State;
};

struct TestDecompFloat_t {
	Burger::uint32_float_t m_Value;
	uint_t m_uDigits;
	ExpectedFPPrintInfoState_t m_State;
};

static uint_t BURGER_API FloatResultOut(const Burger::FPPrintInfo* pFPPrintInfo,
	const ExpectedFPPrintInfoState_t* pExpectedState) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	uint_t uTest = !(pFPPrintInfo->GetSpecialFormDigits() ==
		pExpectedState->m_uSpecialFormDigits);
	uResult |= uTest;
	ReportFailure("GetSpecialFormDigits() = %u, expected %u", uTest,
		pFPPrintInfo->GetSpecialFormDigits(),
		pExpectedState->m_uSpecialFormDigits);

	uTest = !(pFPPrintInfo->HasInteger() == pExpectedState->m_bHasInteger);
	uResult |= uTest;
	ReportFailure("HasInteger() = %u, expected %u", uTest,
		pFPPrintInfo->HasInteger(), pExpectedState->m_bHasInteger);

	uTest = !(pFPPrintInfo->GetFirstNonZeroIntDigitPosition() ==
		pExpectedState->m_uFirstNonZero);
	uResult |= uTest;
	ReportFailure("GetFirstNonZeroIntDigitPosition() = %u, expected %u", uTest,
		pFPPrintInfo->GetFirstNonZeroIntDigitPosition(),
		pExpectedState->m_uFirstNonZero);

	uTest = !(pFPPrintInfo->GetLastNonZeroIntDigitPosition() ==
		pExpectedState->m_uLastNonZero);
	uResult |= uTest;
	ReportFailure("GetLastNonZeroIntDigitPosition() = %u, expected %u", uTest,
		pFPPrintInfo->GetLastNonZeroIntDigitPosition(),
		pExpectedState->m_uLastNonZero);

	uTest = !(pFPPrintInfo->HasFractional() == pExpectedState->m_bHasFraction);
	uResult |= uTest;
	ReportFailure("HasFractional() = %u, expected %u", uTest,
		pFPPrintInfo->HasFractional(), pExpectedState->m_bHasFraction);

	uTest = !(pFPPrintInfo->GetFirstNonZeroFracDigitPosition() ==
		pExpectedState->m_uFirstNonZeroFraction);
	uResult |= uTest;
	ReportFailure("GetFirstNonZeroFracDigitPosition() = %u, expected %u", uTest,
		pFPPrintInfo->GetFirstNonZeroFracDigitPosition(),
		pExpectedState->m_uFirstNonZeroFraction);

	uTest = !(pFPPrintInfo->GetLastNonZeroFracDigitPosition() ==
		pExpectedState->m_uLastNonZeroFraction);
	uResult |= uTest;
	ReportFailure("GetLastNonZeroFracDigitPosition() = %u, expected %u", uTest,
		pFPPrintInfo->GetLastNonZeroFracDigitPosition(),
		pExpectedState->m_uLastNonZeroFraction);

	uTest = !(pFPPrintInfo->GetResult() == pExpectedState->m_Result);
	uResult |= uTest;
	ReportFailure("GetResult() = %u, expected %u", uTest,
		static_cast<uint_t>(pFPPrintInfo->GetResult()),
		static_cast<uint_t>(pExpectedState->m_Result));

	return uResult;
}

/***************************************

	Output a single decompressed value

***************************************/

template<class T>
static uint_t BURGER_API FloatDecompTest(const T* pDecomp) BURGER_NOEXCEPT
{
	const Burger::FPInfo FloatInfo(pDecomp->m_Value.Get());
	Burger::FPPrintInfo FloatPrintInfo;
	FloatPrintInfo.AnalyzeFloat(&FloatInfo, pDecomp->m_uDigits);
	return FloatResultOut(&FloatPrintInfo, &pDecomp->m_State);
}

/***************************************

	Test floating point decomposition

***************************************/

static const TestDecompFloat_t g_DecompFloats[] = {
	// inf
	{0x7F800000U, 6,
		{3, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultPositiveInfinity}},
	// -inf
	{0xFF800000U, 6,
		{4, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultNegativeInfinity}},
	// 0.123f
	{0x3DFBE76DU, 6,
		{0, FALSE, 0, 0, TRUE, 1, 6, Burger::FPPrintInfo::kResultNormalFloat}},
	// 2000.125f
	{0x44FA0400U, 3,
		{0, TRUE, 4, 4, TRUE, 1, 3, Burger::FPPrintInfo::kResultNormalFloat}},
	// 999.9999f
	{0x4479FFFEU, 3,
		{0, TRUE, 4, 4, TRUE, 1, 3,
			Burger::FPPrintInfo::kResultFloatRoundedUpADigit}}};

static const TestDecompDouble_t g_DecompDoubles[] = {
	// inf
	{0x7FF0000000000000ULL, 6,
		{3, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultPositiveInfinity}},
	// -inf
	{0xFFF0000000000000ULL, 6,
		{4, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultNegativeInfinity}},
	// 0.0000063240
	{0x3EDA8657E22DF6CDULL, 6,
		{0, FALSE, 0, 0, TRUE, 6, 6, Burger::FPPrintInfo::kResultNormalFloat}},
	// 6324000000.0625
	{0x41F78F0950010000ULL, 3,
		{0, TRUE, 7, 10, TRUE, 2, 3,
			Burger::FPPrintInfo::kResultFloatRoundedUpAtEnd}},
	// 4320.0635
	{0x40B0E0104189374CULL, 3,
		{0, TRUE, 2, 4, TRUE, 2, 3,
			Burger::FPPrintInfo::kResultFloatRoundedUpAtEnd}}};

static uint_t BURGER_API TestFloatDecomp(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const TestDecompFloat_t* pWork = g_DecompFloats;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_DecompFloats);
	do {
		const uint_t uTest = FloatDecompTest(pWork);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Failed with float(%g)", uTest, pWork->m_Value.Get());
		}
		++pWork;
	} while (--uCount);

	const TestDecompDouble_t* pWork2 = g_DecompDoubles;
	uCount = BURGER_ARRAYSIZE(g_DecompDoubles);
	do {
		const uint_t uTest = FloatDecompTest(pWork2);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure(
				"Failed with double(%g)", uTest, pWork2->m_Value.Get());
		}
		++pWork2;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Argument detection testing - these routines check to make sure we're marking
	up the m-version function arguments with the same type that the compiler
	thinks it is.

***************************************/

static uint_t BURGER_API TestArgType(const Burger::SafePrintArgument* pArg,
	const char* pType, Burger::SafePrintArgument::eType uType) BURGER_NOEXCEPT
{
	uint_t uResult = 0;
	const Burger::SafePrintArgument::eType uReported = pArg->GetType();
	if (uReported != uType) {
		ReportFailure(
			"SafePrintArgument(%s) reported its type as %s, expected %s", 1,
			pType, Burger::SafePrintArgument::GetTypeName(uReported),
			Burger::SafePrintArgument::GetTypeName(uType));
		uResult = 1;
	}
	return uResult;
}

/***************************************

	Test single byte class SafePrintArgument

***************************************/

static uint_t BURGER_API ArgTypeUnitTest1ByteTypes(void) BURGER_NOEXCEPT
{
	// Char is both signed and unsigned
	const Burger::SafePrintArgument E1(static_cast<char>(12));
	uint_t uResult = TestArgType(&E1, "char", g_CharARG);

	const Burger::SafePrintArgument E2(static_cast<signed char>(12));
	uResult |=
		TestArgType(&E2, "signed char", Burger::SafePrintArgument::ARG_INT8);

	const Burger::SafePrintArgument E3(static_cast<unsigned char>(12));
	uResult |=
		TestArgType(&E3, "unsigned char", Burger::SafePrintArgument::ARG_UINT8);

	const Burger::SafePrintArgument E4(static_cast<int8_t>(12));
	uResult |= TestArgType(&E4, "int8_t", Burger::SafePrintArgument::ARG_INT8);

	const Burger::SafePrintArgument E5(static_cast<uint8_t>(12));
	uResult |=
		TestArgType(&E5, "uint8_t", Burger::SafePrintArgument::ARG_UINT8);

#if defined(BURGER_HAS_CHAR8_T)
	const Burger::SafePrintArgument E6(static_cast<char8_t>('a'));
	uResult |= TestArgType(&E6, "char8_t", g_Char8ARG);
#endif
	return uResult;
}

/***************************************

	Test 2 byte class SafePrintArgument

***************************************/

static uint_t BURGER_API ArgTypeUnitTest2ByteTypes(void) BURGER_NOEXCEPT
{
	// short
	const Burger::SafePrintArgument E1(static_cast<short>(12));
	uint_t uResult =
		TestArgType(&E1, "short", Burger::SafePrintArgument::ARG_INT16);

	const Burger::SafePrintArgument E2(static_cast<signed short>(12));
	uResult |=
		TestArgType(&E2, "signed short", Burger::SafePrintArgument::ARG_INT16);

	const Burger::SafePrintArgument E3(static_cast<unsigned short>(12));
	uResult |= TestArgType(
		&E3, "unsigned short", Burger::SafePrintArgument::ARG_UINT16);

	const Burger::SafePrintArgument E4(static_cast<int16_t>(12));
	uResult |=
		TestArgType(&E4, "int16_t", Burger::SafePrintArgument::ARG_INT16);

	const Burger::SafePrintArgument E5(static_cast<uint16_t>(12));
	uResult |=
		TestArgType(&E5, "uint16_t", Burger::SafePrintArgument::ARG_UINT16);

	// Special test, this must map to short
#if defined(BURGER_HAS_WCHAR_T)
	const Burger::SafePrintArgument E6(static_cast<wchar_t>(10));
	uResult |= TestArgType(&E6, "wchar_t", g_WCharARG);
#endif

#if defined(BURGER_HAS_CHAR16_T)
	const Burger::SafePrintArgument E7(static_cast<char16_t>(10));
	uResult |= TestArgType(&E7, "char16_t", g_Char16ARG);
#endif
	return uResult;
}

/***************************************

	Test 4 byte class SafePrintArgument

***************************************/

static uint_t BURGER_API ArgTypeUnitTest4ByteTypes(void) BURGER_NOEXCEPT
{
	const Burger::SafePrintArgument E1(static_cast<int>(12));
	uint_t uResult =
		TestArgType(&E1, "int", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument E2(static_cast<signed int>(12));
	uResult |=
		TestArgType(&E2, "signed int", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument E3(static_cast<unsigned int>(12));
	uResult |=
		TestArgType(&E3, "unsigned int", Burger::SafePrintArgument::ARG_UINT32);

	const Burger::SafePrintArgument E4(static_cast<int32_t>(12));
	uResult |=
		TestArgType(&E4, "int32_t", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument E5(static_cast<uint32_t>(12));
	uResult |=
		TestArgType(&E5, "uint32_t", Burger::SafePrintArgument::ARG_UINT32);

#if BURGER_SIZEOF_LONG == 8
	const Burger::SafePrintArgument E6(static_cast<long>(12));
	uResult |= TestArgType(&E6, "long", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument E7(static_cast<unsigned long>(12));
	uResult |= TestArgType(
		&E7, "unsigned long", Burger::SafePrintArgument::ARG_UINT64);
#else
	const Burger::SafePrintArgument E6(static_cast<long>(12));
	uResult |= TestArgType(&E6, "long", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument E7(static_cast<unsigned long>(12));
	uResult |= TestArgType(
		&E7, "unsigned long", Burger::SafePrintArgument::ARG_UINT32);
#endif

#if defined(BURGER_HAS_CHAR16_T)
	const Burger::SafePrintArgument E8(static_cast<char32_t>(10));
	uResult |= TestArgType(&E8, "char32_t", g_Char32ARG);
#endif

	return uResult;
}

/***************************************

	Test 8 byte class SafePrintArgument

***************************************/

static uint_t BURGER_API ArgTypeUnitTest8ByteTypes(void) BURGER_NOEXCEPT
{
	const Burger::SafePrintArgument E1(static_cast<int64_t>(12));
	uint_t uResult =
		TestArgType(&E1, "int64_t", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument E2(static_cast<uint64_t>(12));
	uResult |=
		TestArgType(&E2, "uint64_t", Burger::SafePrintArgument::ARG_UINT64);

	return uResult;
}

/***************************************

	Test real and bool types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestcoreTypes(void) BURGER_NOEXCEPT
{
	const Burger::SafePrintArgument E1(1.0f);
	uint_t uResult =
		TestArgType(&E1, "float", Burger::SafePrintArgument::ARG_FLOAT);

	const Burger::SafePrintArgument E2(1.0);
	uResult |=
		TestArgType(&E2, "double", Burger::SafePrintArgument::ARG_DOUBLE);

	const Burger::SafePrintArgument E3(true);
	uResult |= TestArgType(&E3, "bool", Burger::SafePrintArgument::ARG_BOOL);

	const Burger::SafePrintArgument E4(
		static_cast<uint16_t>(12), Burger::SafePrintArgument::ARG_HALF);
	uResult |= TestArgType(&E4, "half", Burger::SafePrintArgument::ARG_HALF);
	return uResult;
}

/***************************************

	Test pointer types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestPointerTypes(void) BURGER_NOEXCEPT
{
	const char* cp = "hello world";
	const Burger::SafePrintArgument E1(cp);
	uint_t uResult =
		TestArgType(&E1, "const char *", Burger::SafePrintArgument::ARG_PCHAR);

	char c = 12;
	char* pc2 = &c;
	const Burger::SafePrintArgument E2(pc2);
	uResult |= TestArgType(&E2, "char *", Burger::SafePrintArgument::ARG_PCHAR);

	signed char sc = 37;
	signed char* psc = &sc;
	const Burger::SafePrintArgument EF(psc);
	uResult |= TestArgType(
		&EF, "signed char *", Burger::SafePrintArgument::ARG_PSCHAR);

	unsigned char uc = 0xAC;
	unsigned char* puc = &uc;
	const Burger::SafePrintArgument EA(puc);
	uResult |= TestArgType(
		&EA, "unsigned char *", Burger::SafePrintArgument::ARG_PUCHAR);

#if defined(BURGER_HAS_WCHAR_T)
	wchar_t wc = 0x0000;
	const wchar_t* pwc = &wc;
	const Burger::SafePrintArgument EX(pwc);
	uResult |=
		TestArgType(&EX, "wchar_t *", Burger::SafePrintArgument::ARG_PUINT16);

	const Burger::SafePrintArgument EY(L"HELLO WORLD");
	uResult |= TestArgType(
		&EY, "L\"Hello World\"", Burger::SafePrintArgument::ARG_PUINT16);
#endif

	short s1 = 0x4754;
	const Burger::SafePrintArgument EB(&s1);
	uResult |= TestArgType(&EB, "short", Burger::SafePrintArgument::ARG_PINT16);

	unsigned short us2 = 0xFEFE;
	const Burger::SafePrintArgument EC(&us2);
	uResult |= TestArgType(
		&EC, "unsigned short", Burger::SafePrintArgument::ARG_PUINT16);

	int i = 12;
	int* pi = &i;
	const Burger::SafePrintArgument E3(pi);
	uResult |= TestArgType(&E3, "int", Burger::SafePrintArgument::ARG_PINT32);

	unsigned int ui = 32982;
	unsigned int* pui = &ui;
	const Burger::SafePrintArgument ED(pui);
	uResult |= TestArgType(
		&ED, "unsigned int", Burger::SafePrintArgument::ARG_PUINT32);

#if BURGER_SIZEOF_LONG == 8
	long l = 0x34567812;
	const Burger::SafePrintArgument EL(&l);
	uResult |= TestArgType(&EL, "long", Burger::SafePrintArgument::ARG_PINT64);

	unsigned long ul = 0xAEFF0123;
	const Burger::SafePrintArgument EM(&ul);
	uResult |= TestArgType(
		&EM, "unsigned long", Burger::SafePrintArgument::ARG_PUINT64);
#else
	long l = 0x34567812;
	const Burger::SafePrintArgument EL(&l);
	uResult |= TestArgType(&EL, "long", Burger::SafePrintArgument::ARG_PINT32);

	unsigned long ul = 0xAEFF0123;
	const Burger::SafePrintArgument EM(&ul);
	uResult |= TestArgType(
		&EM, "unsigned long", Burger::SafePrintArgument::ARG_PUINT32);
#endif

	int64_t ll = 0x1234567812345678LL;
	const Burger::SafePrintArgument E8(&ll);
	uResult |=
		TestArgType(&E8, "int64_t *", Burger::SafePrintArgument::ARG_PINT64);

	uint64_t ull = 0x9876543212345678ULL;
	const Burger::SafePrintArgument E9(&ull);
	uResult |=
		TestArgType(&E9, "uint64_t *", Burger::SafePrintArgument::ARG_PUINT64);

	float f = 12.0f;
	const float* pf = &f;
	const Burger::SafePrintArgument E4(pf);
	uResult |= TestArgType(&E4, "float", Burger::SafePrintArgument::ARG_PFLOAT);

	double d = 12.0;
	double* pd = &d;
	const Burger::SafePrintArgument E5(pd);
	uResult |=
		TestArgType(&E5, "double", Burger::SafePrintArgument::ARG_PDOUBLE);

	void* pv = nullptr;
	const Burger::SafePrintArgument E6(pv);
	uResult |= TestArgType(&E6, "void *", Burger::SafePrintArgument::ARG_PVOID);

	bool b = true;
	bool* pb = &b;
	const Burger::SafePrintArgument E7(pb);
	uResult |= TestArgType(&E7, "void *", Burger::SafePrintArgument::ARG_PVOID);
	return uResult;
}

/***************************************

	Test literal types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestLiterals(void) BURGER_NOEXCEPT
{
	const Burger::SafePrintArgument F1("hello world");
	uint_t uResult = TestArgType(
		&F1, "\"hello world\"", Burger::SafePrintArgument::ARG_PCHAR);

	const Burger::SafePrintArgument F2('1');
	uResult |= TestArgType(&F2, "'1'", Burger::SafePrintArgument::ARG_INT8);

#if defined(BURGER_HAS_WCHAR_T)
	const Burger::SafePrintArgument F3(L'a');
	uResult |= TestArgType(&F3, "L'a'", Burger::SafePrintArgument::ARG_UINT16);
#endif

	const Burger::SafePrintArgument F4(0172);
	uResult |= TestArgType(&F4, "0172", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument F5(12);
	uResult |= TestArgType(&F5, "12", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument F6(0x1BCDEF12);
	uResult |=
		TestArgType(&F6, "0x1BCDEF12", Burger::SafePrintArgument::ARG_INT32);

	// Watcom doesn't support 'ABCD'
#if !defined(BURGER_WATCOM)
	const Burger::SafePrintArgument F7('ABCD');
#if defined(BURGER_METROWERKS) && defined(BURGER_68K)
	uResult |=
		TestArgType(&F7, "'ABCD'", Burger::SafePrintArgument::ARG_UINT32);
#else
	uResult |= TestArgType(&F7, "'ABCD'", Burger::SafePrintArgument::ARG_INT32);
#endif
#endif

	const Burger::SafePrintArgument F8(0172U);
	uResult |= TestArgType(&F8, "0172U", Burger::SafePrintArgument::ARG_UINT32);

	const Burger::SafePrintArgument F9(12U);
	uResult |= TestArgType(&F9, "12U", Burger::SafePrintArgument::ARG_UINT32);

	const Burger::SafePrintArgument F0(0x1bcdef12U);
	uResult |=
		TestArgType(&F0, "0x1bcdf12U", Burger::SafePrintArgument::ARG_UINT32);
	return uResult;
}

/***************************************

	Test long literal types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestLiterals2(void) BURGER_NOEXCEPT
{
#if BURGER_SIZEOF_LONG == 8
	const Burger::SafePrintArgument G1(0172L);
	uint_t uResult =
		TestArgType(&G1, "0172L", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G2(12L);
	uResult |= TestArgType(&G2, "12L", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G3(0x1bcdef12L);
	uResult |=
		TestArgType(&G3, "0x1bcdef12L", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G4(0172UL);
	uResult |=
		TestArgType(&G4, "0172UL", Burger::SafePrintArgument::ARG_UINT64);

	const Burger::SafePrintArgument G5(12UL);
	uResult |= TestArgType(&G5, "12UL", Burger::SafePrintArgument::ARG_UINT64);

	const Burger::SafePrintArgument G6(0x1bcdef12UL);
	uResult |=
		TestArgType(&G6, "0x1bcdef12UL", Burger::SafePrintArgument::ARG_UINT64);
#else
	const Burger::SafePrintArgument G1(0172L);
	uint_t uResult =
		TestArgType(&G1, "0172L", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument G2(12L);
	uResult |= TestArgType(&G2, "12L", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument G3(0x1bcdef12L);
	uResult |=
		TestArgType(&G3, "0x1bcdef12L", Burger::SafePrintArgument::ARG_INT32);

	const Burger::SafePrintArgument G4(0172UL);
	uResult |=
		TestArgType(&G4, "0172UL", Burger::SafePrintArgument::ARG_UINT32);

	const Burger::SafePrintArgument G5(12UL);
	uResult |= TestArgType(&G5, "12UL", Burger::SafePrintArgument::ARG_UINT32);

	const Burger::SafePrintArgument G6(0x1bcdef12UL);
	uResult |=
		TestArgType(&G6, "0x1bcdef12UL", Burger::SafePrintArgument::ARG_UINT32);
#endif

#if !defined(BURGER_LINUX) && !defined(BURGER_SWITCH)
	const Burger::SafePrintArgument G7(012LL);
	uResult |= TestArgType(&G7, "012LL", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G8(12LL);
	uResult |= TestArgType(&G8, "12LL", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G9(0x1bcdef12LL);
	uResult |=
		TestArgType(&G9, "0x1bcdef12LL", Burger::SafePrintArgument::ARG_INT64);

	const Burger::SafePrintArgument G0(12ULL);
	uResult |= TestArgType(&G0, "12ULL", Burger::SafePrintArgument::ARG_UINT64);
#endif
	return uResult;
}

/***************************************

	Test SIMD types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestSIMDTypes(void) BURGER_NOEXCEPT
{
	uint_t uResult = 0;

#if defined(BURGER_WINDOWS)
#if defined(BURGER_X86)
	__m64 simd64;

#if defined(BURGER_WATCOM)
	simd64._64[0] = 0xFF00FF00AA22AA22;
#else
	simd64.m64_u64 = 0xFF00FF00AA22AA22;
#endif

	Burger::SafePrintArgument S64a(simd64);
	uResult |=
		TestArgType(&S64a, "__m64", Burger::SafePrintArgument::ARG_SIMD64);

#if defined(BURGER_WATCOM)
	_m_empty();
#else
	_mm_empty();
#endif
#endif
#endif

#if defined(BURGER_WINDOWS)
#if defined(BURGER_INTEL) && !defined(BURGER_WATCOM)
	__m128 simd128;
#if defined(BURGER_METROWERKS) || (defined(_MSC_VER) && (_MSC_VER < 1400))
	simd128.m128_f32[0] = 0;
	simd128.m128_f32[1] = 0;
	simd128.m128_f32[2] = -1;
	simd128.m128_f32[3] = -1;
#elif defined(BURGER_CLANG)
	simd128[0] = 0;
	simd128[1] = 0;
	simd128[2] = -1;
	simd128[3] = -1;
#else
	simd128.m128_i64[0] = 0;
	simd128.m128_i64[1] = -1;
#endif
	Burger::SafePrintArgument S128(simd128);
	uResult |=
		TestArgType(&S128, "__m128", Burger::SafePrintArgument::ARG_SIMD128);

	__m128d simd128d;
#if defined(BURGER_METROWERKS)
	simd128d.m128_f64[0] = 0.0;
	simd128d.m128_f64[1] = -123456789.9988776655;
#elif defined(BURGER_CLANG)
	simd128d[0] = 0.0;
	simd128d[1] = -123456789.9988776655;
#else
	simd128d.m128d_f64[0] = 0.0;
	simd128d.m128d_f64[1] = -123456789.9988776655;
#endif
	Burger::SafePrintArgument S128d(simd128d);
	uResult |=
		TestArgType(&S128d, "__m128d", Burger::SafePrintArgument::ARG_SIMD128D);

	__m128i simd128i;
#if defined(BURGER_METROWERKS)
	simd128i.m128_u64[0] = 0;
	simd128i.m128_u64[1] = BURGER_MAXUINT64;
#elif defined(BURGER_CLANG)
	simd128i[0] = 0;
	simd128i[1] = BURGER_MAXUINT64;
#else
	simd128i.m128i_u64[0] = 0;
	simd128i.m128i_i64[1] = -1;
#endif
	Burger::SafePrintArgument S128i(simd128i);
	uResult |=
		TestArgType(&S128i, "__m128i", Burger::SafePrintArgument::ARG_SIMD128I);
#endif

#if defined(BURGER_X86)
	__m64* pS64 = &simd64;
	Burger::SafePrintArgument P64a(pS64);
	uResult |=
		TestArgType(&P64a, "__m64*", Burger::SafePrintArgument::ARG_PSIMD64);
#endif

#if defined(BURGER_INTEL) && !defined(BURGER_WATCOM)
	__m128* pS128 = &simd128;
	Burger::SafePrintArgument P128(pS128);
	uResult |=
		TestArgType(&P128, "__m128*", Burger::SafePrintArgument::ARG_PSIMD128);

	__m128d* pS128d = &simd128d;
	Burger::SafePrintArgument P128d(pS128d);
	uResult |= TestArgType(
		&P128d, "__m128d", Burger::SafePrintArgument::ARG_PSIMD128D);

	__m128i* pS128i = &simd128i;
	Burger::SafePrintArgument P128i(pS128i);
	uResult |= TestArgType(
		&P128i, "__m128i*", Burger::SafePrintArgument::ARG_PSIMD128I);
#endif
#endif

	return uResult;
}

/***************************************

	Test SafePrintArgument

***************************************/

static uint_t BURGER_API TestArgumentDetection(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of argument lookup");
	}

	uint_t uResult = ArgTypeUnitTest1ByteTypes();
	uResult |= ArgTypeUnitTest2ByteTypes();
	uResult |= ArgTypeUnitTest4ByteTypes();
	uResult |= ArgTypeUnitTest8ByteTypes();
	uResult |= ArgTypeUnitTestcoreTypes();
	uResult |= ArgTypeUnitTestPointerTypes();
	uResult |= ArgTypeUnitTestLiterals();
	uResult |= ArgTypeUnitTestLiterals2();
	uResult |= ArgTypeUnitTestSIMDTypes();
	return uResult;
}

/***************************************

	Test integer number formatting

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API UnitTestFormattingInt(int iStartWidth, int iEndWidth,
	int iStartPrecision, int iEndPrecision, const char* pFlagsString,
	const char* pLocalFormat, const char* pBurgerFormat,
	const Burger::SafePrintArgument& rStartValue,
	const Burger::SafePrintArgument& rEndValue,
	const Burger::SafePrintArgument& rStepValue)
{
	// Sanity checks
	const Burger::SafePrintArgument::eType uStartType = rStartValue.GetType();
	BURGER_ASSERT(rStartValue.IsInteger());
	BURGER_ASSERT(
		(rEndValue.GetType() == uStartType) && rStepValue.IsInteger());
	BURGER_ASSERT(!rStepValue.IsZero() && !rStepValue.IsNegative());

	enum eTestType { TESTING_I32, TESTING_U32, TESTING_I64, TESTING_U64 };

	eTestType uTestType = TESTING_I32;

	switch (uStartType) {
	case Burger::SafePrintArgument::ARG_INT8:
	case Burger::SafePrintArgument::ARG_INT16:
	case Burger::SafePrintArgument::ARG_INT32:
		uTestType = TESTING_I32;
		break;
	case Burger::SafePrintArgument::ARG_INT64:
		uTestType = TESTING_I64;
		break;
	case Burger::SafePrintArgument::ARG_UINT8:
	case Burger::SafePrintArgument::ARG_UINT16:
	case Burger::SafePrintArgument::ARG_UINT32:
		uTestType = TESTING_U32;
		break;
	case Burger::SafePrintArgument::ARG_UINT64:
		uTestType = TESTING_U64;
		break;
	default:
		BURGER_ASSERT(FALSE);
		return 1; // Failure
	}

	int32_t i32 = 0;
	uint32_t u32 = 0;
	int64_t i64 = 0;
	uint64_t u64 = 0;

	switch (uTestType) {
	case TESTING_I32:
		i32 = rStartValue.GetInt32();
		break;
	case TESTING_U32:
		u32 = rStartValue.GetUInt32();
		break;
	case TESTING_I64:
		i64 = rStartValue.GetInt64();
		break;
	case TESTING_U64:
		u64 = rStartValue.GetUInt64();
		break;
	default:
		BURGER_ASSERT(FALSE);
		return 1; // Failure
	}

	// Create the base string with the default flags
	char BaseString[64];
	BaseString[0] = '%';
	Burger::StringCopy(BaseString + 1, sizeof(BaseString) - 1, pFlagsString);
	uint_t uResult = 0; // Success
	uint_t bContinueTesting = FALSE;
	do {
		for (int32_t iWidth = iStartWidth; iWidth <= iEndWidth; iWidth++) {
			for (int32_t iPrecision = iStartPrecision;
				 iPrecision <= iEndPrecision; iPrecision++) {

				// Generate the reference strings (Don't use sprintf, since
				// that's can't be assumed to be bug free.)

				char ReferenceFormatString[64]; // Format string
				char TestFormatString[64];
				char ReferenceString[128]; // Output buffer (Reference)
				char TestString[128];      // Output buffer (Burgerlib)

				// Initial with the flags
				Burger::StringCopy(ReferenceFormatString, BaseString);

				if (iWidth >= 0) {
					Burger::NumberString WidthString(iWidth);
					if (iPrecision >= 0) {
						// Insert Width.Precision
						const Burger::NumberString PrecisionString(iPrecision);
						Burger::StringConcatenate(
							ReferenceFormatString, WidthString.c_str());
						Burger::StringConcatenate(ReferenceFormatString, ".");
						Burger::StringConcatenate(
							ReferenceFormatString, PrecisionString.c_str());
					} else {
						// Insert Width alone
						Burger::StringConcatenate(
							ReferenceFormatString, WidthString.c_str());
					}
				} else if (iPrecision >= 0) {
					// Insert .Precision
					const Burger::NumberString PrecisionString(iPrecision);
					Burger::StringConcatenate(ReferenceFormatString, ".");
					Burger::StringConcatenate(
						ReferenceFormatString, PrecisionString.c_str());
				}

				// Copy to the test (Format is the same)
				Burger::StringCopy(TestFormatString, ReferenceFormatString);

				// Append the ANSI and the Burgerlib formats
				Burger::StringConcatenate(ReferenceFormatString, pLocalFormat);
				Burger::StringConcatenate(TestFormatString, pBurgerFormat);

				// call the C++ RTL and our formatter
				int isprintfResult = -1;
				intptr_t uReturnedLength = 0;
				switch (uTestType) {
				case TESTING_I32:
					isprintfResult = snprintf(ReferenceString,
						sizeof(ReferenceString), ReferenceFormatString, i32);
					uReturnedLength = Burger::Snprintf(
						TestString, sizeof(TestString), TestFormatString, i32);
					break;
				case TESTING_U32:
					isprintfResult = snprintf(ReferenceString,
						sizeof(ReferenceString), ReferenceFormatString, u32);
					uReturnedLength = Burger::Snprintf(
						TestString, sizeof(TestString), TestFormatString, u32);
					break;
				case TESTING_I64:
					isprintfResult = snprintf(ReferenceString,
						sizeof(ReferenceString), ReferenceFormatString, i64);
					uReturnedLength = Burger::Snprintf(
						TestString, sizeof(TestString), TestFormatString, i64);
					break;
				case TESTING_U64:
					isprintfResult = snprintf(ReferenceString,
						sizeof(ReferenceString), ReferenceFormatString, u64);
					uReturnedLength = Burger::Snprintf(
						TestString, sizeof(TestString), TestFormatString, u64);
					break;
				}

				// Failed?
				if ((uReturnedLength !=
						static_cast<intptr_t>(isprintfResult)) ||
					(uReturnedLength &&
						Burger::StringCompare(ReferenceString, TestString,
							static_cast<uintptr_t>(uReturnedLength)))) {
					uResult = TRUE;
					ReportFailure(
						"Reference %s didn't match Test %s, Ref format was %s, Burger format was %s",
						TRUE, ReferenceString, TestString,
						ReferenceFormatString, TestFormatString);
				}
			}
		}

		// now loop to the next int
		bContinueTesting = FALSE;
		switch (uTestType) {
		case TESTING_I32: {
			const int32_t t = i32;
			i32 += rStepValue.GetInt32();
			if ((i32 <= rEndValue.m_Data.m_iInt32) && (i32 > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		case TESTING_U32: {
			const uint32_t t = u32;
			u32 += rStepValue.GetUInt32();
			if ((u32 <= rEndValue.m_Data.m_uWord32) && (u32 > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		case TESTING_I64: {
			const int64_t t = i64;
			i64 += rStepValue.GetInt64();
			if ((i64 <= rEndValue.m_Data.m_iInt64) && (i64 > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		case TESTING_U64: {
			const uint64_t t = u64;
			u64 += rStepValue.GetUInt64();
			if ((u64 <= rEndValue.m_Data.m_uWord64) && (u64 > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		default:
			break;
		}
	} while (bContinueTesting);
	return uResult;
}
#endif

/***************************************

	Test integer number formatting

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static const int32_t g_TestInt32s[] = {INT32_MIN, -2147483647, -1234567890,
	-1147483647, -147483647, -47483647 - 7483647, -483647, -83647, -3647, -647,
	-47, 9, -1, 0, 1, 5, 12, 432, 5439, 48923, 439671, 9876543, 53286473,
	123456789, 653294098, 1234567890, 2147483646, BURGER_MAXINT};

static const uint32_t g_TestWord32s[] = {0, 1, 2, 3, 4, 8, 10, 11, 16, 32, 64,
	65, 99, 100, 432, 5439, 48923, 439671, 9876543, 53286473, 123456789,
	653294098, 1234567890, 2147483646, BURGER_MAXINT, BURGER_MAXUINT - 1,
	BURGER_MAXUINT};

static const int64_t g_TestInt64s[] = {INT64_MIN, -9223372036854775807LL,
	-9223372036854775806LL, -922337203685477580LL, -92233720368547758LL,
	-9223372036854775LL, -922337203685477LL, -92233720368547LL,
	-92233720368547LL, -9223372036854LL, -922337203685LL, -53578897654LL,
	-3147483649LL, // these two are in the 'bit 32' zone
	-2147483648LL, -2147483647, -1234567890, -1147483647, -147483647,
	-47483647 - 7483647, -483647, -83647, -3647, -647, -47, 9, -1, 0, 1, 5, 12,
	432, 5439, 48923, 439671, 9876543, 53286473, 123456789, 653294098,
	1234567890, 2147483646, BURGER_MAXINT, 21474836468LL, 214748364683LL,
	2147483646876LL, 21474836468234LL, 214748364689105LL, 2147483646854896LL,
	BURGER_MAXINT64 - 1, BURGER_MAXINT64};

static const uint64_t g_TestWord64s[] = {0, 1, 5, 12, 432, 5439, 48923, 439671,
	9876543, 53286473, 123456789, 653294098, 1234567890, 2147483646,
	BURGER_MAXINT, 21474836468LL, 214748364683LL, 2147483646876LL,
	21474836468234LL, 214748364689105LL, 2147483646854896LL,
	BURGER_MAXINT64 - 1, BURGER_MAXINT64, 9223372036854775808ULL,
	BURGER_MAXUINT64};

static const char* g_SignedIntegerTestFlags[] =
	{ // combinations of -, +, ' ' and '0'
		"", "+", " ", "+ ", "0", "+0", " 0", "+ 0", "-", "-+", "- ", "-+ ",
		"0-", "+0-", " -0", "+ 0-"};

static const char* g_UnsignedIntegerTestFlags[] = { // combinations of - and '0'
	"", "-", "0", "-0"};

static const char* g_UnsignedHexOctalTestFlags[] =
	{ // combinations of -, # and '0'
		"", "-", "0", "-0", "#", "-#", "#0", "-0#"};

struct IntegerTestConfig_t {
	const char* m_pName;
	const char** m_ppTestFlags;
	uintptr_t m_uTestFlagCount;
	uint_t m_bSigned;

	const char* m_p32BitConversion1;
	const char* m_p32BitConversion2;
	int m_32BitWidthMin, m_32BitWidthMax;
	int m_32BitPrecisionMin, m_32BitPrecisionMax;

	const char* m_p64BitConversion1;
	const char* m_p64BitConversion2;
	const char* m_p64BitConversion3;
	const char* m_p64BitConversion4;
	int m_64BitWidthMin, m_64BitWidthMax;
	int m_64BitPrecisionMin, m_64BitPrecisionMax;
};

static const IntegerTestConfig_t SignedIntegerTests = {"Signed Integer",
	g_SignedIntegerTestFlags, BURGER_ARRAYSIZE(g_SignedIntegerTestFlags), TRUE,
	"d", "", -1, 12, -1, 12, "lld", "", "lld", "", -1, 21, -1, 21};

static const IntegerTestConfig_t UnsignedIntegerTests = {"Unsigned Integer",
	g_UnsignedIntegerTestFlags, BURGER_ARRAYSIZE(g_UnsignedIntegerTestFlags),
	FALSE, "u", "", -1, 12, -1, 12, "llu", "", "llu", "", -1, 21, -1, 21};

static const IntegerTestConfig_t UnsignedHexTests = {"Hex",
	g_UnsignedHexOctalTestFlags, BURGER_ARRAYSIZE(g_UnsignedHexOctalTestFlags),
	FALSE, "x", "X", -1, 12, -1, 12, "llx", "llX", "llx", "llX", -1, 20, -1,
	20};

static const IntegerTestConfig_t UnsignedOctalTests = {"Octal",
	g_UnsignedHexOctalTestFlags, BURGER_ARRAYSIZE(g_UnsignedHexOctalTestFlags),
	FALSE, "o", "", -1, 13, -1, 13, "llo", "", "llo", "", -1, 24, -1, 24};
#endif

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API TestIntegerFormat(
	const IntegerTestConfig_t* pTestConfig, uint_t uVerbose)
{
	uint_t uResult = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of %s formatting", pTestConfig->m_pName);
	}
	const char** ppTestFlags = pTestConfig->m_ppTestFlags;
	uintptr_t uTestFlagCount = pTestConfig->m_uTestFlagCount;

	//
	// Perform the 32 bit tests
	//

	int iStartWidth = pTestConfig->m_32BitWidthMin;
	int iEndWidth = pTestConfig->m_32BitWidthMax;
	int iPrecisionStart = pTestConfig->m_32BitPrecisionMin;
	int iPrecisionEnd = pTestConfig->m_32BitPrecisionMax;

	const char* p32BitConversion1 = pTestConfig->m_p32BitConversion1;
	const char* p32BitConversion2 = pTestConfig->m_p32BitConversion2;
	uint_t bAltFormPresent = (Burger::StringLength(p32BitConversion2) != 0);

	if (pTestConfig->m_bSigned) {
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestInt32s); i++) {
			int32_t iVal = g_TestInt32s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, p32BitConversion1,
					p32BitConversion1, iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		// Sweep test
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", p32BitConversion1,
			p32BitConversion1, INT32_MIN, INT32_MAX, 10000);
	} else {
		// unsigned
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestWord32s); i++) {
			uint32_t iVal = g_TestWord32s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, p32BitConversion1,
					p32BitConversion1, iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", p32BitConversion1,
			p32BitConversion1, static_cast<uint32_t>(0), BURGER_MAXUINT, 10000);
	}

	//
	// Perform the 64 bit tests
	//

	iStartWidth = pTestConfig->m_64BitWidthMin;
	iEndWidth = pTestConfig->m_64BitWidthMax;
	iPrecisionStart = pTestConfig->m_64BitPrecisionMin;
	iPrecisionEnd = pTestConfig->m_64BitPrecisionMax;

	const char* pLocalconversion = pTestConfig->m_p64BitConversion1;
	const char* pAltlocalConversion = pTestConfig->m_p64BitConversion2;

	p32BitConversion1 = pTestConfig->m_p64BitConversion3;
	p32BitConversion2 = pTestConfig->m_p64BitConversion4;

	bAltFormPresent = (Burger::StringLength(pAltlocalConversion) != 0);

	if (pTestConfig->m_bSigned) {
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestInt64s); i++) {
			int64_t iVal = g_TestInt64s[i];
			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, pLocalconversion,
					p32BitConversion1, iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						pAltlocalConversion, p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		// Sweep test
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", pLocalconversion,
			p32BitConversion1, static_cast<int64_t>(0), BURGER_MAXINT64,
			9510030001301LL);
	} else { // unsigned
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestWord64s); i++) {
			uint64_t iVal = g_TestWord64s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, pLocalconversion,
					p32BitConversion1, iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						pAltlocalConversion, p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", pLocalconversion,
			p32BitConversion1, static_cast<uint64_t>(0), BURGER_MAXUINT64,
			9510030001301ULL);
	}
	return uResult;
}
#endif

/***************************************

	Char testing

***************************************/

static uint_t BURGER_API UnitTestFormattingChar(int iStartWidth, int iEndWidth,
	const char* pFlags, const char* pLocalFormat,
	const Burger::SafePrintArgument& rStartChar,
	const Burger::SafePrintArgument& rEndChar)
{
	BURGER_ASSERT(rStartChar.IsCharacter() && rEndChar.IsCharacter());
	BURGER_ASSERT(rStartChar.GetType() == rEndChar.GetType());

	enum eCharType { TYPE_SIGNED_CHAR, TYPE_UNSIGNED_CHAR, TYPE_WCHAR };
	eCharType uCharType;

	signed char c = 0;
	signed char fc = 0;
	unsigned char uc = 0;
	unsigned char fuc = 0;
	uint16_t wc = 0;
	uint16_t fwc = 0;

	if (rStartChar.GetType() == Burger::SafePrintArgument::ARG_INT8) {
		uCharType = TYPE_SIGNED_CHAR;
		c = rStartChar.m_Data.m_iInt8;
		fc = rEndChar.m_Data.m_iInt8;
	} else if (rStartChar.GetType() == Burger::SafePrintArgument::ARG_UINT8) {
		uCharType = TYPE_UNSIGNED_CHAR;
		uc = rStartChar.m_Data.m_uWord8;
		fuc = rEndChar.m_Data.m_uWord8;
	} else if (rStartChar.GetType() == Burger::SafePrintArgument::ARG_UINT16) {
		uCharType = TYPE_WCHAR;
		wc = rStartChar.m_Data.m_uWord16;
		fwc = rEndChar.m_Data.m_uWord16;
	} else {
		BURGER_ASSERT(FALSE);
		return 1; // Failure
	}

	char BaseString[64];
	BaseString[0] = '%';
	Burger::StringCopy(BaseString + 1, sizeof(BaseString) - 1, pFlags);

	uint_t uResult = 0;
	uint_t bContinueTesting;
	do {
		for (int32_t iWidth = iStartWidth; iWidth <= iEndWidth; iWidth++) {
			// Get the string to pass to each formatter
			char TestFormatString[64];
			char ReferenceString[128]; // Output buffer (Reference)
			char TestString[128];      // Output buffer (Burgerlib)
			char UTF8Buffer[8];

			// Initial with the flags
			Burger::StringCopy(TestFormatString, BaseString);
			if (iWidth >= 0) {
				Burger::NumberString WidthString(iWidth);
				// Insert Width alone
				Burger::StringConcatenate(
					TestFormatString, WidthString.c_str());
			}

			// Append the ANSI format
			Burger::StringConcatenate(TestFormatString, pLocalFormat);

			// call the C++ RTL and our formatter
			int isprintfResult = -1;
			intptr_t uReturnedLength = 0;
			switch (uCharType) {
			case TYPE_SIGNED_CHAR:
				isprintfResult = snprintf(ReferenceString,
					sizeof(ReferenceString), TestFormatString, c);
				uReturnedLength = Burger::Snprintf(
					TestString, sizeof(TestString), TestFormatString, c);
				break;
			case TYPE_UNSIGNED_CHAR:
				isprintfResult = snprintf(ReferenceString,
					sizeof(ReferenceString), TestFormatString, uc);
				uReturnedLength = Burger::Snprintf(
					TestString, sizeof(TestString), TestFormatString, uc);
				break;
			case TYPE_WCHAR:

				// This is tricky, since Burgerlib only uses UTF-8 encoding, and
				// Windows/Mac/et all uses whatever the hell they feel like,
				// snprintf() can't be used to generate a unit test. One needs
				// to be made manually

				// If there is no width, just store the char in the final buffer
				// and be done
				isprintfResult =
					static_cast<int>(Burger::UTF8::FromUTF16(UTF8Buffer, wc));
				if (iWidth <= isprintfResult) {
					Burger::StringCopy(ReferenceString, UTF8Buffer);
				} else {
					// Fill the buffer
					Burger::MemoryFill(
						ReferenceString, 0x20, static_cast<uintptr_t>(iWidth));
					ReferenceString[iWidth] = 0;

					// Left justify?
					char* pStart = ReferenceString;
					if (pFlags[0] != '-') {
						// Right justify
						pStart += (iWidth - isprintfResult);
					}
					// Copy in the "char"
					Burger::MemoryCopy(pStart, UTF8Buffer,
						static_cast<uintptr_t>(isprintfResult));
					isprintfResult = iWidth;
				}
				uReturnedLength = Burger::Snprintf(
					TestString, sizeof(TestString), TestFormatString, wc);
				break;
			}

			// count

			if ((uReturnedLength != static_cast<intptr_t>(isprintfResult)) ||
				(uReturnedLength &&
					Burger::MemoryCompare(ReferenceString, TestString,
						static_cast<uintptr_t>(uReturnedLength)) != 0)) {
				uResult = TRUE;
				ReportFailure(
					"Char reference '%s' didn't match Test '%s', Ref format was %s",
					TRUE, ReferenceString, TestString, TestFormatString);
			}
		}

		bContinueTesting = FALSE;
		// now loop to the next int
		switch (uCharType) {
		case TYPE_SIGNED_CHAR: {
			char t = c;
			++c;
			if ((c <= fc) && (c > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		case TYPE_UNSIGNED_CHAR: {
			unsigned char t = uc;
			++uc;
			if ((uc <= fuc) && (uc > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		case TYPE_WCHAR: {
			uint16_t t = wc;
			++wc;
			if ((wc <= fwc) && (wc > t)) {
				bContinueTesting = TRUE;
			}
			break;
		}
		}
	} while (bContinueTesting);
	return uResult;
}

/***************************************

	Invoke the char unit testing

***************************************/

static uint_t BURGER_API TestCharFormats(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of char formatting");
	}

	char s1 = -128;
	char s2 = 0x7f;
	unsigned char u1 = 0x00;
	unsigned char u2 = 0xff;

	uint_t uResult = UnitTestFormattingChar(-1, 2, "", "c", s1, s2);
	uResult |= UnitTestFormattingChar(-1, 2, "-", "c", s1, s2);

	uResult |= UnitTestFormattingChar(-1, 2, "", "c", u1, u2);
	uResult |= UnitTestFormattingChar(-1, 2, "-", "c", u1, u2);

	wchar_t w1 = 0x0000;
	wchar_t w2 = 0x03ff;

	uResult |= UnitTestFormattingChar(-1, 3, "", "lc", w1, w2);
	uResult |= UnitTestFormattingChar(-1, 3, "-", "lc", w1, w2);

	wchar_t pwText[] = L"HELLO WORLD! 1234567asdfg";

	for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(pwText); i++) {
		wchar_t wChar = pwText[i];
		uResult |= UnitTestFormattingChar(-1, 3, "", "lc", wChar, wChar);
	}
	return uResult;
}

/***************************************

	Run tests for strings

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API UnitTestFormattingString(int iStartWidth,
	int iEndWidth, int iStartPrecision, int iEndPrecision, const char* pFlags,
	const char* fmt, const Burger::SafePrintArgument& rTheStr)
{
	BURGER_ASSERT(rTheStr.IsTextPointer());

	char ReferenceString[128];
	char TestString[128];
	char TestFormatString[64];
	uint_t uResult = 0;
	for (int iWidth = iStartWidth; iWidth <= iEndWidth; iWidth++) {
		for (int iPrecision = iStartPrecision; iPrecision <= iEndPrecision;
			 iPrecision++) {
			// Get the string to pass to each formatter
			if ((iWidth >= 0) && (iPrecision >= 0)) {
				snprintf(TestFormatString, sizeof(TestFormatString),
					"%%%s%d.%d%s", pFlags, iWidth, iPrecision, fmt);
			} else if (iWidth >= 0) {
				snprintf(TestFormatString, sizeof(TestFormatString), "%%%s%d%s",
					pFlags, iWidth, fmt);
			} else if (iPrecision >= 0) {
				snprintf(TestFormatString, sizeof(TestFormatString),
					"%%%s.%d%s", pFlags, iPrecision, fmt);
			} else {
				snprintf(TestFormatString, sizeof(TestFormatString), "%%%s%s",
					pFlags, fmt);
			}

			// call the C++ RTL and our formatter
			int isprintfResult = -1;
			intptr_t uReturnedLength = 0;

			isprintfResult = snprintf(ReferenceString, sizeof(ReferenceString),
				TestFormatString, rTheStr.m_Data.m_pChar);
			uReturnedLength = Burger::Snprintf(
				TestString, sizeof(TestString), TestFormatString, rTheStr);

			if ((uReturnedLength != static_cast<intptr_t>(isprintfResult)) ||
				(uReturnedLength &&
					Burger::StringCompare(ReferenceString, TestString,
						static_cast<uintptr_t>(uReturnedLength)) != 0)) {
				uResult = TRUE;
				ReportFailure(
					"Reference %s didn't match Test %s, Ref format was %s, Burger format was %s",
					TRUE, ReferenceString, TestString, TestFormatString,
					TestFormatString);
			}
		}
	}
	return uResult;
}
#endif

/***************************************

	Run tests for strings

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API TestStringFormats(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of string formatting");
	}
	const char* pS1 = "HELLO";
	const wchar_t* pWS1 = L"HELLO";

	const char* pS2 = "";
	const wchar_t* pWS2 = L"";

	const char* pS3 = NULL;
	const wchar_t* pWS3 = NULL;

	uint_t uResult = UnitTestFormattingString(-1, 8, -1, 8, "", "s", pS1);
	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "-", "s", pS1);

	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "", "ls", pWS1);
	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "-", "ls", pWS1);

	uResult |= UnitTestFormattingString(-1, 2, -1, 2, "", "s", pS2);
	uResult |= UnitTestFormattingString(-1, 2, -1, 2, "-", "s", pS2);

	uResult |= UnitTestFormattingString(-1, 2, -1, 2, "", "ls", pWS2);
	uResult |= UnitTestFormattingString(-1, 2, -1, 2, "-", "ls", pWS2);

	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "", "s", pS3);
	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "-", "s", pS3);

	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "", "ls", pWS3);
	uResult |= UnitTestFormattingString(-1, 8, -1, 8, "-", "ls", pWS3);
	return uResult;
}
#endif

/***************************************

	Run tests for binary formatting

***************************************/

struct BinaryTests_t {
	const char* m_pFormat;
	const char* m_pExpectedResult;
};

static BinaryTests_t g_BinaryTests[] = {
	{"%!", "00000000000000000001001000110100"},
	{"%#!", "00101100010010000000000000000000"}};

static uint_t BURGER_API TestBinaryFormats(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of binary formatting");
	}
	char Buffer[128];
	uintptr_t i = BURGER_ARRAYSIZE(g_BinaryTests);
	const BinaryTests_t* pTests = g_BinaryTests;
	uint_t uResult = 0;
	do {
		// Perform the test
		Burger::Snprintf(Buffer, sizeof(Buffer), pTests->m_pFormat, 0x1234);
		if (Burger::StringCompare(pTests->m_pExpectedResult, Buffer)) {
			uResult = 1;
			ReportFailure(
				"Binary reference %s didn't match Test %s, Burger format was %s",
				TRUE, pTests->m_pExpectedResult, Buffer, pTests->m_pFormat);
		}
		++pTests;
	} while (--i);
	return uResult;
}

/***************************************

	Run tests for float formatting

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API UnitTestFormattingReal(int iStartWidth, int iEndWidth,
	int iStartPrecision, int iEndPrecision, const char* pFlags,
	const char* pFormatString, const Burger::SafePrintArgument& rTheReal)
{
	Burger::SafePrintArgument::eType uType = rTheReal.GetType();
	BURGER_ASSERT(rTheReal.IsReal());

	char ReferenceString[512];
	char TestString[512];
	char TestFormat[256];
	uint_t uResult = 0;
	for (int iWidth = iStartWidth; iWidth <= iEndWidth; iWidth++) {
		for (int iPrecision = iStartPrecision; iPrecision <= iEndPrecision;
			 iPrecision++) {
			// Get the string to pass to each formatter
			if (iWidth >= 0 && iPrecision >= 0) {
				snprintf(TestFormat, sizeof(TestFormat), "%%%s%d.%d%s", pFlags,
					iWidth, iPrecision, pFormatString);
			} else if (iWidth >= 0) {
				snprintf(TestFormat, sizeof(TestFormat), "%%%s%d%s", pFlags,
					iWidth, pFormatString);
			} else if (iPrecision >= 0) {
				snprintf(TestFormat, sizeof(TestFormat), "%%%s.%d%s", pFlags,
					iPrecision, pFormatString);
			} else {
				snprintf(TestFormat, sizeof(TestFormat), "%%%s%s", pFlags,
					pFormatString);
			}

			// call the C++ RTL and our formatter
			int isprintfResult = -1;
			intptr_t uReturnedLength = 0;
			switch (uType) {
			case Burger::SafePrintArgument::ARG_FLOAT: {
				float fValue = rTheReal.m_Data.m_fFloat;
				isprintfResult = snprintf(ReferenceString,
					sizeof(ReferenceString), TestFormat, fValue);
				uReturnedLength = Burger::Snprintf(
					TestString, sizeof(TestString), TestFormat, fValue);
				break;
			}
			case Burger::SafePrintArgument::ARG_DOUBLE: {
				isprintfResult =
					snprintf(ReferenceString, sizeof(ReferenceString),
						TestFormat, rTheReal.m_Data.m_dDouble);
				uReturnedLength = Burger::Snprintf(TestString,
					sizeof(TestString), TestFormat, rTheReal.m_Data.m_dDouble);
				break;
			}
			default:

				break;
			}
			if ((uReturnedLength != static_cast<intptr_t>(isprintfResult)) ||
				(uReturnedLength &&
					Burger::StringCompare(ReferenceString, TestString,
						static_cast<uintptr_t>(uReturnedLength)))) {
				uResult = TRUE;
				ReportFailure(
					"Reference %s didn't match Test %s, Ref format was %s",
					TRUE, ReferenceString, TestString, TestFormat);
			}
		}
	}
	return uResult;
}
#endif

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static const char* g_FloatTestFlags[] =
	{ // combinations of -, +, ' ', # and '0'
		"", "+", " ", "+ ", "#", "#+", "# ", "#+ ", "0", "+0", " 0", "+ 0",
		"#0", "#+0", "# 0", "#+ 0",

		"-", "-+", "- ", "-+ ", "-#", "-#+", "-# ", "-#+ ", "-0", "-+0", "- 0",
		"-+ 0", "-#0", "-#+0", "-# 0", "-#+ 0"};

static uint_t BURGER_API TestRealFormats(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of floating point formatting");
	}
	int i;
	int n;

	uint_t uResult = 0;

#if 0
	const float* pSpecialFloats = (const float*)g_FloatSpecialConstants;
	for (i = 0; i < BURGER_ARRAYSIZE(g_FloatSpecialConstants); i++) {
		float fValue = pSpecialFloats[i];
		for (n = 0; n < BURGER_ARRAYSIZE(g_FloatTestFlags); n++) {
			uResult |= UnitTestFormattingReal(
				-1, 15, -1, 10, g_FloatTestFlags[n], "f", fValue);
		}
	}
#endif
	for (i = 0; i < BURGER_ARRAYSIZE(g_FloatConstants); i++) {
		float fValue = g_FloatConstants[i].m_fFloatValue;
		int iMaxPrecision =
			static_cast<int>(g_FloatConstants[i].m_uMaxPrecision);
		for (n = 0; n < BURGER_ARRAYSIZE(g_FloatTestFlags); n++) {
			uResult |= UnitTestFormattingReal(
				-1, 15, -1, iMaxPrecision, g_FloatTestFlags[n], "f", fValue);
		}
	}

#if 0
	const double* specialDoubles = (const double*)g_DoubleSpecialConstants;

	for (i = 0; i < BURGER_ARRAYSIZE(g_DoubleSpecialConstants); i++) {
		for (n = 0; n < BURGER_ARRAYSIZE(g_FloatTestFlags); n++) {
			const char* flags = g_FloatTestFlags[n];
			double theDbl = specialDoubles[i];
			uResult |=
				UnitTestFormattingReal(-1, 20, -1, 18, flags, "f", theDbl);
		}
	}
#endif

	for (i = 0; i < BURGER_ARRAYSIZE(g_DoubleConstants); i++) {
		for (n = 0; n < BURGER_ARRAYSIZE(g_FloatTestFlags); n++) {
			const char* flags = g_FloatTestFlags[n];
			double theDbl = g_DoubleConstants[i].m_doubleVal;
			int maxPrecision =
				static_cast<int>(g_DoubleConstants[i].m_MaxPrecision);
			uResult |= UnitTestFormattingReal(
				-1, 20, -1, maxPrecision, flags, "f", theDbl);
		}
	}
	return uResult;
}
#endif

int BURGER_API TestBrprintf(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running printf tests");
	}

	uint_t uResult = TestFloatDecomp();
	uResult |= TestArgumentDetection(uVerbose);
	uResult |= TestBinaryFormats(uVerbose);
	uResult |= TestCharFormats(uVerbose);

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
	uResult |= TestIntegerFormat(&SignedIntegerTests, uVerbose);
	uResult |= TestIntegerFormat(&UnsignedIntegerTests, uVerbose);
	uResult |= TestIntegerFormat(&UnsignedHexTests, uVerbose);
	uResult |= TestIntegerFormat(&UnsignedOctalTests, uVerbose);
	uResult |= TestStringFormats(uVerbose);
	uResult |= TestRealFormats(uVerbose);
#endif

#if 0
	int n = 0;
	{
		float* fdata = (float*)g_FloatSpecialConstants;
		float inf = fdata[2];
		float ninf = fdata[3];
		char text[4096] = { 0 };
		for (n = 0; n <20; n++ ) {
			printf( "'%.*f'   '%.*f'\n", n, inf, n, ninf );
			printf( "'%.*F'   '%.*F'\n", n, inf, n, ninf );
		}

		printf ( "'%F'  '%.10F'  '%1.0F'  '%.3F'\n", 191.0f, 333.333f, 3613.0f, 1234.0f );

		printf ( "'%f'  '%.10f'  '%1.0f'  '%.3f'\n", inf, inf, inf, inf );
		printf ( "'%f'  '%.10f'  '%1.0f'  '%.3f'\n", ninf, ninf, ninf, ninf );

		Burger::Snprintf( text, sizeof(text), "%f", inf );
	}
	{
		char text[4096];

		char* name = setlocale( LC_ALL, "" );
		//struct lconv * lc = localeconv();

		for ( n = 5 ; n < BURGER_ARRAYSIZE( g_NineTests ); n++ ) {
			double  nine = g_NineTests[n];
		
			printf ( "RTL The double %%.f     = '%.f'\n", nine );
			Burger::Snprintf ( text, sizeof(text), "MGP The double %%.f     = '%.f'\n\n", nine );
			printf( "%s", text );

			Burger::FPPrintInfo::SetClip(17);
			printf ( "RTL The double %%.20f   = '%.20f'\n", nine );
			Burger::Snprintf ( text, sizeof(text), "MGP The double %%.20f   = '%.20f'\n\n", nine );
			printf( "%s", text );

			Burger::FPPrintInfo::SetClip(17);
			Burger::Snprintf ( text, sizeof(text), "MGP#The double %%.20f   = '%.20f'\n\n", nine );
			printf( "%s", text );

			Burger::FPPrintInfo::SetClip(17);
		}

		printf ( "locale name ='%s'\n ", name );

		printf( "('%%+e',  0.0f )= '%+e'\n", 0.0f );
		printf( "('%%+e',  1.0f )= '%+e'\n", 1.0f );
		printf( "('%%+e', -1.0f )= '%+e'\n", -1.0f );


		printf( "('%%03e', 1.0f )= '%03e'\n", 1.0f );
		printf( "('%%03.0e', 1.0f )= '%03.0e'\n", 1.0f );
		printf( "('%%03.1e', 1.0f )= '%03.1e'\n", 1.0f );
		printf( "('%%05.0e', 1.0f )= '%05.0e'\n", 1.0f );
		printf( "('%%05.1e', 1.0f )= '%05.1e'\n", 1.0f );

		for ( int n = 0; n < BURGER_ARRAYSIZE( g_FloatConstants ); n++ ) {
			float f = g_FloatConstants[n].m_fFloatValue;
			printf( "( '%%e' , %.9f ) = '%e'\n", f, f );
		}

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = %f \n", 123.0f );
		printf( "CRT 123.0f = %f \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = %.f \n", 123.0f );
		printf( "CRT 123.0f = %.f \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = '%9.f' \n", 123.0f );
		printf( "CRT 123.0f = '%9.f' \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = '%14f' \n", 123.0f );
		printf( "CRT 123.0f = '%14f' \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = '%-9.f' \n", 123.0f );
		printf( "CRT 123.0f = '%-9.f' \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 123.0f = '%-14f' \n", 123.0f );
		printf( "CRT 123.0f = '%-14f' \n", 123.0f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 0.123f = %.f \n", 0.123f );
		printf( "CRT 0.123f = %.f \n", 0.123f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 2000.125f = %.3f \n", 2000.125f );
		printf( "CRT 2000.125f = %.3f \n", 2000.125f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 2000.125f = %.2f \n", 2000.125f );
		printf( "CRT 2000.125f = %.2f \n", 2000.125f );
		printf( text );

		Burger::Snprintf( text, sizeof(text), "HPF 999.999 %%.2f = %.2f \n", 999.999f );
		printf( "CRT 999.999 %%.2f = %.2f \n", 999.999f );
		printf( "%s", text );

	}

	{
		char text[4096];

		Burger::Snprintf( text, sizeof(text), "the %s in %s falls on the %s\n", "rain", "spain", "plain" );
		printf( text );

		const char* pMsg = "Width";
		int  w = 20313;

		Burger::Snprintf( text, sizeof(text), "the %s is %d of %d\n", pMsg, 1, w );
		printf( text );
		Burger::Printf( "THE %s is %d of %d\n", pMsg, 1, w );

		Burger::Snprintf( text, sizeof(text), "the %s is %d of %,d\n", pMsg, 1, w );
		printf( text );

		const char* testStr = " the decimal =%#8d\n";

		printf( testStr,  51234 );
		Burger::Printf( testStr,  51234 );

	}
#endif
	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all printf tests!");
	}
	return static_cast<int>(uResult);
}
