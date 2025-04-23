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
#include "bratomic.h"
#include "brfpinfo.h"
#include "brmemoryfunctions.h"
#include "brnumberstring.h"
#include "brprintf.h"
#include "brstringfunctions.h"
#include "brstructs.h"
#include "brtick.h"
#include "brutf32.h"
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

/***************************************

	Some data types are signed or unsigned due to compiler defaults and
	settings. In order to test that ArgumentType classifies these values
	properly, determine the size and signness of char, wchar_t, char8_t,
	char16_t, char32_t in advance.

***************************************/

// Char is always 1 byte in size, it is unknown if it's signed or unsigned
static Burger::eArgumentType g_CharARG = static_cast<char>(-1) < 0 ?
    Burger::kArgumentTypeInt8 :
    Burger::kArgumentTypeUInt8;

// wchar_t can be 16 or 32 bits in size and signed or unsigned.
static Burger::eArgumentType g_WCharARG = static_cast<wchar_t>(-1) < 0 ?
    (sizeof(wchar_t) == 2) ? Burger::kArgumentTypeInt16 :
                             Burger::kArgumentTypeInt32 :
	(sizeof(wchar_t) == 2) ? Burger::kArgumentTypeUInt16 :
                             Burger::kArgumentTypeUInt32;
static Burger::eArgumentType g_pWCharARG = static_cast<wchar_t>(-1) < 0 ?
    (sizeof(wchar_t) == 2) ? Burger::kArgumentTypeInt16Ptr :
                             Burger::kArgumentTypeInt32Ptr :
	(sizeof(wchar_t) == 2) ? Burger::kArgumentTypeUInt16Ptr :
                             Burger::kArgumentTypeUInt32Ptr;

// These types are a set size, but signness is not guaranteed.
#if defined(BURGER_HAS_CHAR8_T)
static Burger::eArgumentType g_Char8ARG = static_cast<char8_t>(-1) < 0 ?
    Burger::kArgumentTypeInt8 :
    Burger::kArgumentTypeUInt8;
static Burger::eArgumentType g_pChar8ARG = static_cast<char8_t>(-1) < 0 ?
    Burger::kArgumentTypeInt8Ptr :
    Burger::kArgumentTypeUInt8Ptr;
#endif

#if defined(BURGER_HAS_CHAR16_T)
static Burger::eArgumentType g_Char16ARG = static_cast<char16_t>(-1) < 0 ?
    Burger::kArgumentTypeInt16 :
    Burger::kArgumentTypeUInt16;
static Burger::eArgumentType g_pChar16ARG = static_cast<char16_t>(-1) < 0 ?
    Burger::kArgumentTypeInt16Ptr :
    Burger::kArgumentTypeUInt16Ptr;
static Burger::eArgumentType g_Char32ARG = static_cast<char32_t>(-1) < 0 ?
    Burger::kArgumentTypeInt32 :
    Burger::kArgumentTypeUInt32;
static Burger::eArgumentType g_pChar32ARG = static_cast<char32_t>(-1) < 0 ?
    Burger::kArgumentTypeInt32Ptr :
    Burger::kArgumentTypeUInt32Ptr;
#endif

/***************************************

	Check the output of FPPrintInfo against expected results.

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

static uint_t BURGER_API FloatResultOut(const Burger::FPPrintInfo* pFPPrintInfo,
	const ExpectedFPPrintInfoState_t* pExpectedState) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;

	// Test every call against the expected results.

	uint_t uTest = !(pFPPrintInfo->get_special_form_digits() ==
		pExpectedState->m_uSpecialFormDigits);
	uResult |= uTest;
	ReportFailure("get_special_form_digits() = %u, expected %u", uTest,
		pFPPrintInfo->get_special_form_digits(),
		pExpectedState->m_uSpecialFormDigits);

	uTest = !(pFPPrintInfo->has_integer() == pExpectedState->m_bHasInteger);
	uResult |= uTest;
	ReportFailure("has_integer() = %u, expected %u", uTest,
		pFPPrintInfo->has_integer(), pExpectedState->m_bHasInteger);

	uTest = !(pFPPrintInfo->get_first_non_zero_int_digit_position() ==
		pExpectedState->m_uFirstNonZero);
	uResult |= uTest;
	ReportFailure("get_first_non_zero_int_digit_position() = %u, expected %u", uTest,
		pFPPrintInfo->get_first_non_zero_int_digit_position(),
		pExpectedState->m_uFirstNonZero);

	uTest = !(pFPPrintInfo->get_last_non_zero_int_digit_position() ==
		pExpectedState->m_uLastNonZero);
	uResult |= uTest;
	ReportFailure("get_last_non_zero_int_digit_position() = %u, expected %u", uTest,
		pFPPrintInfo->get_last_non_zero_int_digit_position(),
		pExpectedState->m_uLastNonZero);

	uTest = !(pFPPrintInfo->has_fractional() == pExpectedState->m_bHasFraction);
	uResult |= uTest;
	ReportFailure("has_fractional() = %u, expected %u", uTest,
		pFPPrintInfo->has_fractional(), pExpectedState->m_bHasFraction);

	uTest = !(pFPPrintInfo->get_first_non_zero_frac_digit_position() ==
		pExpectedState->m_uFirstNonZeroFraction);
	uResult |= uTest;
	ReportFailure("get_first_non_zero_frac_digit_position() = %u, expected %u", uTest,
		pFPPrintInfo->get_first_non_zero_frac_digit_position(),
		pExpectedState->m_uFirstNonZeroFraction);

	uTest = !(pFPPrintInfo->get_last_non_zero_frac_digit_position() ==
		pExpectedState->m_uLastNonZeroFraction);
	uResult |= uTest;
	ReportFailure("get_last_non_zero_frac_digit_position() = %u, expected %u", uTest,
		pFPPrintInfo->get_last_non_zero_frac_digit_position(),
		pExpectedState->m_uLastNonZeroFraction);

	uTest = !(pFPPrintInfo->get_result() == pExpectedState->m_Result);
	uResult |= uTest;
	ReportFailure("get_result() = %u, expected %u", uTest,
		static_cast<uint_t>(pFPPrintInfo->get_result()),
		static_cast<uint_t>(pExpectedState->m_Result));

	return uResult;
}

/***************************************

	Test floating point decomposition

***************************************/

struct TestDecompFloat_t {
	Burger::uint32_float_t m_Value;
	uint_t m_uDigits;
	ExpectedFPPrintInfoState_t m_State;
};

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

struct TestDecompDouble_t {
	Burger::uint64_double_t m_Value;
	uint_t m_uDigits;
	ExpectedFPPrintInfoState_t m_State;
};

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

struct TestDecompHalf_t {
	uint16_t m_Value;
	uint16_t m_uDigits;
	ExpectedFPPrintInfoState_t m_State;
};

static const TestDecompHalf_t g_DecompHalves[] = {
	// inf
	{0x7C00U, 6,
		{3, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultPositiveInfinity}},
	// -inf
	{0xFC00U, 6,
		{4, FALSE, 0, 0, FALSE, 0, 0,
			Burger::FPPrintInfo::kResultNegativeInfinity}},
	// 0.123f
	{0x2FDFU, 6,
		{0, FALSE, 0, 0, TRUE, 1, 6,
			Burger::FPPrintInfo::kResultFloatRoundedUpAtEnd}},
	// 2000.125f (No fraction, it's really 2000)
	{0x67D0U, 3,
		{0, TRUE, 4, 4, FALSE, 0, 0, Burger::FPPrintInfo::kResultNormalFloat}},
	// 9.99f
	{0x48FF, 3,
		{0, TRUE, 1, 1, TRUE, 1, 3, Burger::FPPrintInfo::kResultNormalFloat}}};

static uint_t BURGER_API TestFloatDecomp(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	// Test 32 bit floats
	const TestDecompFloat_t* pWork = g_DecompFloats;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_DecompFloats);
	do {
		const Burger::FPInfo FloatInfo(pWork->m_Value.get_float());
		Burger::FPPrintInfo FloatPrintInfo;
		FloatPrintInfo.analyze_float(&FloatInfo, pWork->m_uDigits);
		const uint_t uTest = FloatResultOut(&FloatPrintInfo, &pWork->m_State);

		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Failed with float(%g)", uTest, pWork->m_Value.get_float());
		}
		++pWork;
	} while (--uCount);

	// Test 64 bit floats
	const TestDecompDouble_t* pWork2 = g_DecompDoubles;
	uCount = BURGER_ARRAYSIZE(g_DecompDoubles);
	do {
		const Burger::FPInfo FloatInfo(pWork2->m_Value.get_double());
		Burger::FPPrintInfo FloatPrintInfo;
		FloatPrintInfo.analyze_float(&FloatInfo, pWork2->m_uDigits);
		const uint_t uTest = FloatResultOut(&FloatPrintInfo, &pWork2->m_State);

		uFailure |= uTest;
		if (uTest) {
			ReportFailure(
				"Failed with double(%g)", uTest, pWork2->m_Value.get_double());
		}
		++pWork2;
	} while (--uCount);

	// Test 16 bit floats
	const TestDecompHalf_t* pWork3 = g_DecompHalves;
	uCount = BURGER_ARRAYSIZE(g_DecompHalves);
	do {
		const Burger::FPInfo FloatInfo(pWork3->m_Value);
		Burger::FPPrintInfo FloatPrintInfo;
		FloatPrintInfo.analyze_float(&FloatInfo, pWork3->m_uDigits);
		const uint_t uTest = FloatResultOut(&FloatPrintInfo, &pWork3->m_State);

		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Failed with Half(%04X)", uTest, pWork3->m_Value);
		}
		++pWork3;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Argument detection testing - these routines check to make sure we're marking
	up the m-version function arguments with the same type that the compiler
	thinks it is.

***************************************/

static uint_t BURGER_API TestArgType(const Burger::ArgumentType* pArg,
	const char* pType, Burger::eArgumentType uType) BURGER_NOEXCEPT
{
	uint_t uResult = 0;
	const Burger::eArgumentType uReported = pArg->GetType();
	if (uReported != uType) {
		ReportFailure("ArgumentType(%s) reported its type as %s, expected %s",
			1, pType, Burger::ArgumentType::GetTypeName(uReported),
			Burger::ArgumentType::GetTypeName(uType));
		uResult = 1;
	}
	return uResult;
}

/***************************************

	Test single byte class ArgumentType

***************************************/

static uint_t BURGER_API ArgTypeUnitTest1ByteTypes(void) BURGER_NOEXCEPT
{
	// Char is both signed and unsigned
	const Burger::ArgumentType E1(static_cast<char>(12));
	uint_t uResult = TestArgType(&E1, "char", g_CharARG);

	const Burger::ArgumentType E2(static_cast<signed char>(12));
	uResult |= TestArgType(&E2, "signed char", Burger::kArgumentTypeInt8);

	const Burger::ArgumentType E3(static_cast<unsigned char>(12));
	uResult |= TestArgType(&E3, "unsigned char", Burger::kArgumentTypeUInt8);

	const Burger::ArgumentType E4(static_cast<int8_t>(12));
	uResult |= TestArgType(&E4, "int8_t", Burger::kArgumentTypeInt8);

	const Burger::ArgumentType E5(static_cast<uint8_t>(12));
	uResult |= TestArgType(&E5, "uint8_t", Burger::kArgumentTypeUInt8);

#if defined(BURGER_HAS_CHAR8_T)
	const Burger::ArgumentType E6(static_cast<char8_t>('a'));
	uResult |= TestArgType(&E6, "char8_t", g_Char8ARG);
#endif
	return uResult;
}

/***************************************

	Test 2 byte class ArgumentType

***************************************/

static uint_t BURGER_API ArgTypeUnitTest2ByteTypes(void) BURGER_NOEXCEPT
{
	// short
	const Burger::ArgumentType E1(static_cast<short>(12));
	uint_t uResult = TestArgType(&E1, "short", Burger::kArgumentTypeInt16);

	const Burger::ArgumentType E2(static_cast<signed short>(12));
	uResult |= TestArgType(&E2, "signed short", Burger::kArgumentTypeInt16);

	const Burger::ArgumentType E3(static_cast<unsigned short>(12));
	uResult |= TestArgType(&E3, "unsigned short", Burger::kArgumentTypeUInt16);

	const Burger::ArgumentType E4(static_cast<int16_t>(12));
	uResult |= TestArgType(&E4, "int16_t", Burger::kArgumentTypeInt16);

	const Burger::ArgumentType E5(static_cast<uint16_t>(12));
	uResult |= TestArgType(&E5, "uint16_t", Burger::kArgumentTypeUInt16);

	// Special test, this must map to short or 32 bit int
	const Burger::ArgumentType E6(static_cast<wchar_t>(10));
	uResult |= TestArgType(&E6, "wchar_t", g_WCharARG);

#if defined(BURGER_HAS_CHAR16_T)
	const Burger::ArgumentType E7(static_cast<char16_t>(10));
	uResult |= TestArgType(&E7, "char16_t", g_Char16ARG);
#endif
	return uResult;
}

/***************************************

	Test 4 byte class ArgumentType

***************************************/

static uint_t BURGER_API ArgTypeUnitTest4ByteTypes(void) BURGER_NOEXCEPT
{
	const Burger::ArgumentType E1(static_cast<int>(12));
	uint_t uResult = TestArgType(&E1, "int", Burger::kArgumentTypeInt32);

	const Burger::ArgumentType E2(static_cast<signed int>(12));
	uResult |= TestArgType(&E2, "signed int", Burger::kArgumentTypeInt32);

	const Burger::ArgumentType E3(static_cast<unsigned int>(12));
	uResult |= TestArgType(&E3, "unsigned int", Burger::kArgumentTypeUInt32);

	const Burger::ArgumentType E4(static_cast<int32_t>(12));
	uResult |= TestArgType(&E4, "int32_t", Burger::kArgumentTypeInt32);

	const Burger::ArgumentType E5(static_cast<uint32_t>(12));
	uResult |= TestArgType(&E5, "uint32_t", Burger::kArgumentTypeUInt32);

	const Burger::ArgumentType E6(static_cast<long>(12));
	uResult |= TestArgType(&E6, "long",
		sizeof(long) == 4 ? Burger::kArgumentTypeInt32 :
                            Burger::kArgumentTypeInt64);

	const Burger::ArgumentType E7(static_cast<unsigned long>(12));
	uResult |= TestArgType(&E7, "unsigned long",
		sizeof(long) == 4 ? Burger::kArgumentTypeUInt32 :
                            Burger::kArgumentTypeUInt64);

#if defined(BURGER_HAS_CHAR16_T)
	const Burger::ArgumentType E8(static_cast<char32_t>(10));
	uResult |= TestArgType(&E8, "char32_t", g_Char32ARG);
#endif

	return uResult;
}

/***************************************

	Test 8 byte class ArgumentType

***************************************/

static uint_t BURGER_API ArgTypeUnitTest8ByteTypes(void) BURGER_NOEXCEPT
{
	const Burger::ArgumentType E1(static_cast<int64_t>(12));
	uint_t uResult = TestArgType(&E1, "int64_t", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType E2(static_cast<uint64_t>(12));
	uResult |= TestArgType(&E2, "uint64_t", Burger::kArgumentTypeUInt64);

	const Burger::ArgumentType E6(static_cast<long long>(12));
	uResult |= TestArgType(&E6, "long long", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType E7(static_cast<unsigned long long>(12));
	uResult |=
		TestArgType(&E7, "unsigned long long", Burger::kArgumentTypeUInt64);

#if defined(BURGER_MSVC)
	const Burger::ArgumentType E8(static_cast<__int64>(12));
	uResult |= TestArgType(&E8, "__int64", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType E9(static_cast<unsigned __int64>(12));
	uResult |=
		TestArgType(&E9, "unsigned __int64", Burger::kArgumentTypeUInt64);
#endif

	return uResult;
}

/***************************************

	Test real and bool types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestcoreTypes(void) BURGER_NOEXCEPT
{
	const Burger::ArgumentType E1(1.0f);
	uint_t uResult = TestArgType(&E1, "float", Burger::kArgumentTypeFloat32);

	const Burger::ArgumentType E2(1.0);
	uResult |= TestArgType(&E2, "double", Burger::kArgumentTypeFloat64);

	const Burger::ArgumentType E3(true);
	uResult |= TestArgType(&E3, "bool", Burger::kArgumentTypeBool);

	const Burger::ArgumentType E4(
		static_cast<uint16_t>(12), Burger::kArgumentTypeFloat16);
	uResult |= TestArgType(&E4, "half", Burger::kArgumentTypeFloat16);
	return uResult;
}

/***************************************

	Test pointer types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestPointerTypes(void) BURGER_NOEXCEPT
{
	const char* cp = "hello world";
	const Burger::ArgumentType E1(cp);
	uint_t uResult =
		TestArgType(&E1, "const char *", Burger::kArgumentTypeCharPtr);

	char c = 12;
	char* pc2 = &c;
	const Burger::ArgumentType E2(pc2);
	uResult |= TestArgType(&E2, "char *", Burger::kArgumentTypeCharPtr);

	signed char sc = 37;
	signed char* psc = &sc;
	const Burger::ArgumentType EF(psc);
	uResult |= TestArgType(&EF, "signed char *", Burger::kArgumentTypeInt8Ptr);

	unsigned char uc = 0xAC;
	unsigned char* puc = &uc;
	const Burger::ArgumentType EA(puc);
	uResult |=
		TestArgType(&EA, "unsigned char *", Burger::kArgumentTypeUInt8Ptr);

#if !defined(BURGER_WIIU)
	wchar_t wc = 0x0000;
	const wchar_t* pwc = &wc;
	const Burger::ArgumentType EX(pwc);
	uResult |= TestArgType(&EX, "wchar_t *", g_pWCharARG);

	const Burger::ArgumentType EY(L"Hello World");
	uResult |= TestArgType(&EY, "L\"Hello World\"", g_pWCharARG);
#endif

#if defined(BURGER_HAS_CHAR8_T)
	char8_t char8data = 0;
	const char8_t* pChar8 = &char8data;
	const Burger::ArgumentType Echar8(pChar8);
	uResult |= TestArgType(&Echar8, "char8_t *", g_pChar8ARG);
#endif

#if defined(BURGER_HAS_CHAR16_T)
	char16_t char16data = 0;
	const char16_t* pChar16 = &char16data;
	const Burger::ArgumentType Echar16(pChar16);
	uResult |= TestArgType(&Echar16, "char16_t *", g_pChar16ARG);

	char32_t char32data = 0;
	char32_t* pChar32 = &char32data;
	const Burger::ArgumentType Echar32(pChar32);
	uResult |= TestArgType(&Echar32, "char32_t *", g_pChar32ARG);
#endif

	short s1 = 0x4754;
	const Burger::ArgumentType EB(&s1);
	uResult |= TestArgType(&EB, "short", Burger::kArgumentTypeInt16Ptr);

	unsigned short us2 = 0xFEFE;
	const Burger::ArgumentType EC(&us2);
	uResult |=
		TestArgType(&EC, "unsigned short", Burger::kArgumentTypeUInt16Ptr);

	int i = 12;
	int* pi = &i;
	const Burger::ArgumentType E3(pi);
	uResult |= TestArgType(&E3, "int", Burger::kArgumentTypeInt32Ptr);

	unsigned int ui = 32982;
	unsigned int* pui = &ui;
	const Burger::ArgumentType ED(pui);
	uResult |= TestArgType(&ED, "unsigned int", Burger::kArgumentTypeUInt32Ptr);

	long l = 0x34567812;
	const Burger::ArgumentType EL(&l);
	uResult |= TestArgType(&EL, "long",
		sizeof(long) == 4 ? Burger::kArgumentTypeInt32Ptr :
                            Burger::kArgumentTypeInt64Ptr);

	unsigned long ul = 0xAEFF0123;
	const Burger::ArgumentType EM(&ul);
	uResult |= TestArgType(&EM, "unsigned long",
		sizeof(long) == 4 ? Burger::kArgumentTypeUInt32Ptr :
                            Burger::kArgumentTypeUInt64Ptr);

	int64_t ll = 0x1234567812345678LL;
	const Burger::ArgumentType E8(&ll);
	uResult |= TestArgType(&E8, "int64_t *", Burger::kArgumentTypeInt64Ptr);

	uint64_t ull = 0x9876543212345678ULL;
	const Burger::ArgumentType E9(&ull);
	uResult |= TestArgType(&E9, "uint64_t *", Burger::kArgumentTypeUInt64Ptr);

	float f = 12.0f;
	const float* pf = &f;
	const Burger::ArgumentType E4(pf);
	uResult |= TestArgType(&E4, "float", Burger::kArgumentTypeFloat32Ptr);

	double d = 12.0;
	const double* pd = &d;
	const Burger::ArgumentType E5(pd);
	uResult |= TestArgType(&E5, "double", Burger::kArgumentTypeFloat64Ptr);

	void* pv = nullptr;
	const Burger::ArgumentType E6(pv);
	uResult |= TestArgType(&E6, "void *", Burger::kArgumentTypeVoidPtr);

	bool b = true;
	bool* pb = &b;
	const Burger::ArgumentType E7(pb);
	uResult |= TestArgType(&E7, "bool *", Burger::kArgumentTypeBoolPtr);
	return uResult;
}

/***************************************

	Test literal types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestLiterals(void) BURGER_NOEXCEPT
{
	const Burger::ArgumentType F1("hello world");
	uint_t uResult =
		TestArgType(&F1, "\"hello world\"", Burger::kArgumentTypeCharPtr);

	const Burger::ArgumentType F2('1');
	uResult |= TestArgType(&F2, "'1'", Burger::kArgumentTypeInt8);

	const Burger::ArgumentType F3(L'a');
	uResult |= TestArgType(&F3, "L'a'", g_WCharARG);

	const Burger::ArgumentType F4(0172);
	uResult |= TestArgType(&F4, "0172", Burger::kArgumentTypeInt32);

	const Burger::ArgumentType F5(12);
	uResult |= TestArgType(&F5, "12", Burger::kArgumentTypeInt32);

	const Burger::ArgumentType F6(0x1BCDEF12);
	uResult |= TestArgType(&F6, "0x1BCDEF12", Burger::kArgumentTypeInt32);

	// Watcom doesn't support 'ABCD'
#if !defined(BURGER_WATCOM)
	const Burger::ArgumentType F7('ABCD');
#if defined(BURGER_METROWERKS) && defined(BURGER_68K)
	uResult |= TestArgType(&F7, "'ABCD'", Burger::kArgumentTypeUInt32);
#else
	uResult |= TestArgType(&F7, "'ABCD'", Burger::kArgumentTypeInt32);
#endif
#endif

	const Burger::ArgumentType F8(0172U);
	uResult |= TestArgType(&F8, "0172U", Burger::kArgumentTypeUInt32);

	const Burger::ArgumentType F9(12U);
	uResult |= TestArgType(&F9, "12U", Burger::kArgumentTypeUInt32);

	const Burger::ArgumentType F0(0x1bcdef12U);
	uResult |= TestArgType(&F0, "0x1bcdf12U", Burger::kArgumentTypeUInt32);
	return uResult;
}

/***************************************

	Test long literal types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestLiterals2(void) BURGER_NOEXCEPT
{
	const Burger::ArgumentType G1(0172L);
	uint_t uResult = TestArgType(&G1, "0172L",
		sizeof(long) == 4 ? Burger::kArgumentTypeInt32 :
                            Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G2(12L);
	uResult |= TestArgType(&G2, "12L",
		sizeof(long) == 4 ? Burger::kArgumentTypeInt32 :
                            Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G3(0x1bcdef12L);
	uResult |= TestArgType(&G3, "0x1bcdef12L",
		sizeof(long) == 4 ? Burger::kArgumentTypeInt32 :
                            Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G4(0172UL);
	uResult |= TestArgType(&G4, "0172UL",
		sizeof(long) == 4 ? Burger::kArgumentTypeUInt32 :
                            Burger::kArgumentTypeUInt64);

	const Burger::ArgumentType G5(12UL);
	uResult |= TestArgType(&G5, "12UL",
		sizeof(long) == 4 ? Burger::kArgumentTypeUInt32 :
                            Burger::kArgumentTypeUInt64);

	const Burger::ArgumentType G6(0x1bcdef12UL);
	uResult |= TestArgType(&G6, "0x1bcdef12UL",
		sizeof(long) == 4 ? Burger::kArgumentTypeUInt32 :
                            Burger::kArgumentTypeUInt64);

	const Burger::ArgumentType G7(012LL);
	uResult |= TestArgType(&G7, "012LL", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G8(12LL);
	uResult |= TestArgType(&G8, "12LL", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G9(0x1bcdef12LL);
	uResult |= TestArgType(&G9, "0x1bcdef12LL", Burger::kArgumentTypeInt64);

	const Burger::ArgumentType G0(12ULL);
	uResult |= TestArgType(&G0, "12ULL", Burger::kArgumentTypeUInt64);

	return uResult;
}

/***************************************

	Test SIMD types

***************************************/

static uint_t BURGER_API ArgTypeUnitTestSIMDTypes(void) BURGER_NOEXCEPT
{
	uint_t uResult = 0;

	// Only run the unit tests on Intel platforms that support MMX or SSE

#if defined(BURGER_INTEL)
	const uint_t uCPUIDPresent = Burger::has_CPUID();
	if (uCPUIDPresent) {

		Burger::CPUID_t MyID;
		Burger::CPUID(&MyID);

#if defined(BURGER_X86)
		if (MyID.has_MMX()) {
			__m64 simd64 = {0x7F00FF00AA22AA22LL};

			Burger::ArgumentType S64a(simd64);
			uResult |=
				TestArgType(&S64a, "__m64", Burger::kArgumentTypeVector2);

			__m64* pS64 = &simd64;
			Burger::ArgumentType P64a(pS64);
			uResult |=
				TestArgType(&P64a, "__m64*", Burger::kArgumentTypeVector2Ptr);

#if defined(BURGER_WATCOM)
			_m_empty();
#else
			_mm_empty();
#endif
		}
#endif

#if defined(BURGER_INTEL) && !defined(BURGER_WATCOM)
		if (MyID.has_SSE()) {
			__m128 simd128 = {0.0f, 0.0f, -1.0f, -1.0f};

			const Burger::ArgumentType S128(simd128);
			uResult |=
				TestArgType(&S128, "__m128", Burger::kArgumentTypeVector4);

			__m128d simd128d = {0.0, -123456789.9988776655};

			const Burger::ArgumentType S128d(simd128d);
			uResult |=
				TestArgType(&S128d, "__m128d", Burger::kArgumentTypeVector4Dbl);

			__m128i simd128i = _mm_set_epi32(0, 0, -1, -1);
			const Burger::ArgumentType S128i(simd128i);
			uResult |=
				TestArgType(&S128i, "__m128i", Burger::kArgumentTypeVector4Int);

			__m128* pS128 = &simd128;
			const Burger::ArgumentType P128(pS128);
			uResult |=
				TestArgType(&P128, "__m128*", Burger::kArgumentTypeVector4Ptr);

			__m128d* pS128d = &simd128d;
			const Burger::ArgumentType P128d(pS128d);
			uResult |= TestArgType(
				&P128d, "__m128d", Burger::kArgumentTypeVector4DblPtr);

			__m128i* pS128i = &simd128i;
			const Burger::ArgumentType P128i(pS128i);
			uResult |= TestArgType(
				&P128i, "__m128i*", Burger::kArgumentTypeVector4IntPtr);
		}
#endif
	}
#endif

	return uResult;
}

/***************************************

	Test ArgumentType

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

	Test single character chars

***************************************/

static uint_t BURGER_API UnitTestFormattingSingleChar(intptr_t iStartWidth,
	intptr_t iEndWidth, uint_t bLeftJustified,
	const Burger::ArgumentType& rStartChar,
	const Burger::ArgumentType& rEndChar)
{
	uint_t uTestCharacter = static_cast<uint_t>(rStartChar.GetInt32());
	uint_t uFinalCharacter = static_cast<uint_t>(rEndChar.GetInt32());
	uint_t uResult = 0;

	for (;;) {
		// Given a range of widths, perform a string creation and test if it's
		// correct.

		for (intptr_t iWidth = iStartWidth; iWidth <= iEndWidth; ++iWidth) {

			// Create the formatting string
			char FormatString[16];

			// Starting length of 1 for the %
			uintptr_t uTestLength = 1;
			FormatString[0] = '%';

			// Left justified?
			if (bLeftJustified) {
				FormatString[uTestLength] = '-';
				++uTestLength;
			}

			// Append the width (0 is default)
			if (iWidth >= 0) {
				Burger::NumberString WidthString(static_cast<uint32_t>(iWidth));
				// Insert width value
				Burger::string_copy(
					FormatString + uTestLength, WidthString.c_str());
				uTestLength += WidthString.length();
			}

			FormatString[uTestLength] = 'c';
			++uTestLength;

			// Zero terminate
			FormatString[uTestLength] = 0;
			// At this point TestFormatString has the format like %-3lc

			// Manually create the expected output string
			char Expected[128];
			uintptr_t uExpectedLength;

			// No padding possible.
			if (iWidth > 1) {
				// Fill the buffer
				Burger::MemoryFill(
					Expected, 0x20, static_cast<uintptr_t>(iWidth));
				Expected[iWidth] = 0;

				// Left justify?
				uintptr_t uIndex = 0;
				if (!bLeftJustified) {
					// Right justified
					uIndex = static_cast<uintptr_t>(iWidth) - 1U;
				}
				// Copy in the "char"
				Expected[uIndex] = static_cast<char>(uTestCharacter);
				uExpectedLength = static_cast<uintptr_t>(iWidth);
			} else {
				Expected[0] = static_cast<char>(uTestCharacter);
				uExpectedLength = 1;
			}
			Expected[uExpectedLength] = 0;

			// Perform the test with a garbage filled buffer
			char TestString[128];
			Burger::MemoryFill(TestString, 0x44, sizeof(TestString));
			Burger::ArgumentType Parm(rStartChar);
			Parm.m_Data.m_uInt8 = static_cast<uint8_t>(uTestCharacter);

			// Invoke the function
			intptr_t iReturnedLength = Burger::Snprintf(
				TestString, sizeof(TestString), FormatString, Parm);

			// Test for the terminating zero
			if (iReturnedLength >= 0) {
				if (TestString[iReturnedLength]) {
					uResult = TRUE;
					ReportFailure(
						"UnitTestFormattingSingleChar() terminating zero didn't match "
						"iReturnedLength '%s' didn't match Test '%s', Ref format was %s",
						TRUE, Expected, TestString, FormatString);
				}
			}

			// Verify if it's a match.
			if ((iReturnedLength != static_cast<intptr_t>(uExpectedLength)) ||
				(iReturnedLength &&
					Burger::MemoryCompare(Expected, TestString,
						static_cast<uintptr_t>(iReturnedLength)) != 0)) {
				uResult = TRUE;
				ReportFailure(
					"UnitTestFormattingSingleChar() "
					"Char reference '%s' didn't match Test '%s', Ref format was %s",
					TRUE, Expected, TestString, FormatString);
			}
		}

		// Loop for all values
		if (++uTestCharacter > uFinalCharacter) {
			break;
		}
	}
	return uResult;
}

/***************************************

	wchar_t testing

***************************************/

static uint_t BURGER_API UnitTestFormattingSingleWChar(intptr_t iStartWidth,
	intptr_t iEndWidth, uint_t bLeftJustified,
	const Burger::ArgumentType& rStartChar,
	const Burger::ArgumentType& rEndChar)
{
	uint_t uTestCharacter = static_cast<uint_t>(rStartChar.GetInt32());
	uint_t uFinalCharacter = static_cast<uint_t>(rEndChar.GetInt32());
	uint_t uResult = 0;

	for (;;) {
		// Given a range of widths, perform a string creation and test if it's
		// correct.

		for (intptr_t iWidth = iStartWidth; iWidth <= iEndWidth; ++iWidth) {

			// Create the formatting string
			char FormatString[16];

			// Starting length of 1 for the %
			uintptr_t uTestLength = 1;
			FormatString[0] = '%';

			// Left justified?
			if (bLeftJustified) {
				FormatString[uTestLength] = '-';
				++uTestLength;
			}

			// Append the width (0 is default)
			if (iWidth >= 0) {
				Burger::NumberString WidthString(static_cast<uint32_t>(iWidth));
				// Insert width value
				Burger::string_copy(
					FormatString + uTestLength, WidthString.c_str());
				uTestLength += WidthString.length();
			}

			FormatString[uTestLength] = 'l';
			FormatString[uTestLength + 1] = 'c';
			uTestLength += 2;

			// Zero terminate
			FormatString[uTestLength] = 0;
			// At this point TestFormatString has the format like %-3lc

			uintptr_t uExpectedLength;

			// Temp buffer for UTF8 conversion, note, it's unknown if it's going
			// to be 16 or 32 bits in size
			char UTF8Buffer[8];
			if (rStartChar.GetDataLengthInBytes() == 2) {
				uExpectedLength = Burger::UTF8::from_UTF16(
					UTF8Buffer, static_cast<uint16_t>(uTestCharacter));
			} else {
				uExpectedLength = Burger::UTF8::from_UTF32(
					UTF8Buffer, static_cast<uint32_t>(uTestCharacter));
			}
			UTF8Buffer[uExpectedLength] = 0;

			// Buffer for the expected string
			char Expected[128];

			// No padding possible.
			if (iWidth <= static_cast<intptr_t>(uExpectedLength)) {
				Burger::string_copy(Expected, UTF8Buffer);
			} else {
				// Fill the buffer
				Burger::MemoryFill(
					Expected, 0x20, static_cast<uintptr_t>(iWidth));
				Expected[iWidth] = 0;

				// Left justify?
				char* pStart = Expected;
				if (!bLeftJustified) {
					// Right justified
					pStart += (iWidth - uExpectedLength);
				}
				// Copy in the "char"
				Burger::MemoryCopy(pStart, UTF8Buffer, uExpectedLength);
				uExpectedLength = static_cast<uintptr_t>(iWidth);
			}
			Expected[uExpectedLength] = 0;

			// Perform the test with a garbage filled buffer
			char TestString[128];
			Burger::MemoryFill(TestString, 0x44, sizeof(TestString));
			Burger::ArgumentType Parm(rStartChar);
			if (Parm.GetDataLengthInBytes() == 2) {
				Parm.m_Data.m_uInt16 = static_cast<uint16_t>(uTestCharacter);
			} else {
				Parm.m_Data.m_uInt32 = static_cast<uint32_t>(uTestCharacter);
			}

			// Invoke the function
			intptr_t iReturnedLength = Burger::Snprintf(
				TestString, sizeof(TestString), FormatString, Parm);

			// Test for the terminating zero
			if (iReturnedLength >= 0) {
				if (TestString[iReturnedLength]) {
					uResult = TRUE;
					ReportFailure(
						"UnitTestFormattingSingleChar() terminating zero didn't match "
						"iReturnedLength = %d Expected '%s' TestString '%s', FormatString %s",
						TRUE, static_cast<int_t>(iReturnedLength), Expected,
						TestString, FormatString);
				}
			}

			// Verify if it's a match.
			if ((iReturnedLength != static_cast<intptr_t>(uExpectedLength)) ||
				(iReturnedLength &&
					Burger::MemoryCompare(Expected, TestString,
						static_cast<uintptr_t>(iReturnedLength)) != 0)) {
				uResult = TRUE;
				ReportFailure(
					"UnitTestFormattingSingleChar() "
					"Expected '%s' TestString '%s', FormatString %s",
					TRUE, Expected, TestString, FormatString);
			}
		}

		// Loop for all values
		if (++uTestCharacter > uFinalCharacter) {
			break;
		}
	}
	return uResult;
}

/***************************************

	Invoke the char unit testing

***************************************/

static const wchar_t g_pwText[] = L"HELLO WORLD! 1234567asdfg";

static uint_t BURGER_API TestCharFormats(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running tests of char formatting");
	}

	// "-" is left align, default is right align
	const int8_t s1 = -128;
	int8_t s2 = 0x7F;

	uint_t uResult = UnitTestFormattingSingleChar(-1, 2, FALSE, s1, s2);
	uResult |= UnitTestFormattingSingleChar(-1, 2, TRUE, s1, s2);

	const uint8_t u1 = 0x00;
	uint8_t u2 = 0xFF;
	uResult |= UnitTestFormattingSingleChar(-1, 2, FALSE, u1, u2);
	uResult |= UnitTestFormattingSingleChar(-1, 2, TRUE, u1, u2);

	const char c1 = 0x00;
	char c2 = 0x7F;
	uResult |= UnitTestFormattingSingleChar(-1, 2, FALSE, c1, c2);
	uResult |= UnitTestFormattingSingleChar(-1, 2, TRUE, c1, c2);

	const wchar_t w1 = 0x0000;
	wchar_t w2 = 0x03ff;

	uResult |= UnitTestFormattingSingleWChar(-1, 3, FALSE, w1, w2);
	uResult |= UnitTestFormattingSingleWChar(-1, 3, TRUE, w1, w2);

	for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_pwText); i++) {
		wchar_t wChar = g_pwText[i];
		uResult |= UnitTestFormattingSingleWChar(-1, 3, FALSE, wChar, wChar);
	}
	return uResult;
}

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

/***************************************

	Test integer number formatting

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API UnitTestFormattingInt(intptr_t iStartWidth,
	intptr_t iEndWidth, int32_t iStartPrecision, int32_t iEndPrecision,
	const char* pFlagsString, const char* pBurgerFormat,
	const Burger::ArgumentType& rStartValue,
	const Burger::ArgumentType& rEndValue,
	const Burger::ArgumentType& rStepValue)
{
	// Sanity checks
	const Burger::eArgumentType uStartType = rStartValue.GetType();
	BURGER_ASSERT(rStartValue.IsInteger());
	BURGER_ASSERT(
		(rEndValue.GetType() == uStartType) && rStepValue.IsInteger());
	BURGER_ASSERT(!rStepValue.IsZero() && !rStepValue.IsNegative());

	enum eTestType { TESTING_I32, TESTING_U32, TESTING_I64, TESTING_U64 };

	eTestType uTestType = TESTING_I32;

	switch (uStartType) {
	case Burger::kArgumentTypeInt8:
	case Burger::kArgumentTypeInt16:
	case Burger::kArgumentTypeInt32:
		uTestType = TESTING_I32;
		break;
	case Burger::kArgumentTypeInt64:
		uTestType = TESTING_I64;
		break;
	case Burger::kArgumentTypeUInt8:
	case Burger::kArgumentTypeUInt16:
	case Burger::kArgumentTypeUInt32:
		uTestType = TESTING_U32;
		break;
	case Burger::kArgumentTypeUInt64:
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
	char BaseString[8];
	BaseString[0] = '%';
	Burger::string_copy(BaseString + 1, sizeof(BaseString) - 1, pFlagsString);

	uint_t uResult = 0; // Success
	uint_t bContinueTesting = FALSE;
	do {
		for (intptr_t iWidth = iStartWidth; iWidth <= iEndWidth; ++iWidth) {
			for (int32_t iPrecision = iStartPrecision;
				 iPrecision <= iEndPrecision; iPrecision++) {

				// Generate the reference strings (Don't use sprintf, since
				// that's can't be assumed to be bug free.)

				char ReferenceFormatString[64]; // Format string
				char TestFormatString[64];
				char ReferenceString[128]; // Output buffer (Reference)
				char TestString[128];      // Output buffer (Burgerlib)

				// Initial with the flags
				Burger::string_copy(ReferenceFormatString, BaseString);

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
				Burger::string_copy(TestFormatString, ReferenceFormatString);

				// Append the ANSI and the Burgerlib formats
				Burger::StringConcatenate(ReferenceFormatString, pBurgerFormat);
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
			if ((u32 <= rEndValue.m_Data.m_uInt32) && (u32 > t)) {
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
			if ((u64 <= rEndValue.m_Data.m_uInt64) && (u64 > t)) {
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
	123456789, 653294098, 1234567890, 2147483646, INT32_MAX};

static const uint32_t g_TestWord32s[] = {0, 1, 2, 3, 4, 8, 10, 11, 16, 32, 64,
	65, 99, 100, 432, 5439, 48923, 439671, 9876543, 53286473, 123456789,
	653294098, 1234567890, 2147483646, INT32_MAX, UINT32_MAX - 1,
	UINT32_MAX};

static const int64_t g_TestInt64s[] = {INT64_MIN, -9223372036854775807LL,
	-9223372036854775806LL, -922337203685477580LL, -92233720368547758LL,
	-9223372036854775LL, -922337203685477LL, -92233720368547LL,
	-92233720368547LL, -9223372036854LL, -922337203685LL, -53578897654LL,
	-3147483649LL, // these two are in the 'bit 32' zone
	-2147483648LL, -2147483647, -1234567890, -1147483647, -147483647,
	-47483647 - 7483647, -483647, -83647, -3647, -647, -47, 9, -1, 0, 1, 5, 12,
	432, 5439, 48923, 439671, 9876543, 53286473, 123456789, 653294098,
	1234567890, 2147483646, INT32_MAX, 21474836468LL, 214748364683LL,
	2147483646876LL, 21474836468234LL, 214748364689105LL, 2147483646854896LL,
	INT64_MAX - 1, INT64_MAX};

static const uint64_t g_TestWord64s[] = {0, 1, 5, 12, 432, 5439, 48923, 439671,
	9876543, 53286473, 123456789, 653294098, 1234567890, 2147483646,
	INT32_MAX, 21474836468LL, 214748364683LL, 2147483646876LL,
	21474836468234LL, 214748364689105LL, 2147483646854896LL,
	INT64_MAX - 1, INT64_MAX, 9223372036854775808ULL,
	UINT64_MAX};

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
	int m_64BitWidthMin, m_64BitWidthMax;
	int m_64BitPrecisionMin, m_64BitPrecisionMax;
};

static const IntegerTestConfig_t SignedIntegerTests = {"Signed Integer",
	g_SignedIntegerTestFlags, BURGER_ARRAYSIZE(g_SignedIntegerTestFlags), TRUE,
	"d", "", -1, 12, -1, 12, "lld", "", -1, 21, -1, 21};

static const IntegerTestConfig_t UnsignedIntegerTests = {"Unsigned Integer",
	g_UnsignedIntegerTestFlags, BURGER_ARRAYSIZE(g_UnsignedIntegerTestFlags),
	FALSE, "u", "", -1, 12, -1, 12, "llu", "", -1, 21, -1, 21};

static const IntegerTestConfig_t UnsignedHexTests = {"Hex",
	g_UnsignedHexOctalTestFlags, BURGER_ARRAYSIZE(g_UnsignedHexOctalTestFlags),
	FALSE, "x", "X", -1, 12, -1, 12, "llx", "llX", -1, 20, -1, 20};

static const IntegerTestConfig_t UnsignedOctalTests = {"Octal",
	g_UnsignedHexOctalTestFlags, BURGER_ARRAYSIZE(g_UnsignedHexOctalTestFlags),
	FALSE, "o", "", -1, 13, -1, 13, "llo", "", -1, 24, -1, 24};
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
	uint_t bAltFormPresent = (Burger::string_length(p32BitConversion2) != 0);

	if (pTestConfig->m_bSigned) {
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestInt32s); i++) {
			int32_t iVal = g_TestInt32s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, p32BitConversion1,
					iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		// Sweep test
		uResult |= UnitTestFormattingInt(
			-1, -1, -1, -1, "", p32BitConversion1, INT32_MIN, INT32_MAX, 10000);
	} else {
		// unsigned
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestWord32s); i++) {
			uint32_t iVal = g_TestWord32s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, p32BitConversion1,
					iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", p32BitConversion1,
			static_cast<uint32_t>(0), UINT32_MAX, 10000);
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

	p32BitConversion1 = pTestConfig->m_p64BitConversion1;
	p32BitConversion2 = pTestConfig->m_p64BitConversion2;

	bAltFormPresent = (Burger::string_length(pAltlocalConversion) != 0);

	if (pTestConfig->m_bSigned) {
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestInt64s); i++) {
			int64_t iVal = g_TestInt64s[i];
			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, p32BitConversion1,
					iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		// Sweep test
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", pLocalconversion,
			static_cast<int64_t>(0), INT64_MAX, 9510030001301LL);
	} else { // unsigned
		for (uintptr_t i = 0; i < BURGER_ARRAYSIZE(g_TestWord64s); i++) {
			uint64_t iVal = g_TestWord64s[i];

			for (uintptr_t f = 0; f < uTestFlagCount; f++) {
				const char* pFlags = ppTestFlags[f];

				uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
					iPrecisionStart, iPrecisionEnd, pFlags, pLocalconversion,
					iVal, iVal, 1);
				if (bAltFormPresent) {
					uResult |= UnitTestFormattingInt(iStartWidth, iEndWidth,
						iPrecisionStart, iPrecisionEnd, pFlags,
						p32BitConversion2, iVal, iVal, 1);
				}
			}
		}
		uResult |= UnitTestFormattingInt(-1, -1, -1, -1, "", pLocalconversion,
			static_cast<uint64_t>(0), UINT64_MAX, 9510030001301ULL);
	}
	return uResult;
}
#endif

/***************************************

	Run tests for strings

***************************************/

#if defined(BURGER_WINDOWS) && (BURGER_MSVC >= 190000000)
static uint_t BURGER_API UnitTestFormattingString(int iStartWidth,
	int iEndWidth, int iStartPrecision, int iEndPrecision, const char* pFlags,
	const char* fmt, const Burger::ArgumentType& rTheStr)
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
	const char* pFormatString, const Burger::ArgumentType& rTheReal)
{
	Burger::eArgumentType uType = rTheReal.GetType();
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
			case Burger::kArgumentTypeFloat32: {
				float fValue = rTheReal.m_Data.m_fFloat;
				isprintfResult = snprintf(ReferenceString,
					sizeof(ReferenceString), TestFormat, fValue);
				uReturnedLength = Burger::Snprintf(
					TestString, sizeof(TestString), TestFormat, fValue);
				break;
			}
			case Burger::kArgumentTypeFloat64: {
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

// Visual Studio 2015 and 2017 rounds this properly. VS 2019 and higher will
// not round up
#if BURGER_MSVC < 192000000
	{0.125f, 10},
#endif
	{1.0f, 10}, {10.0f, 2}, {0.1f, 10}, {0.0001f, 10}, {0.00001f, 10},
	{0.000001f, 10}, {0.0000001f, 10}, {0.00000001f, 10}, {100.0f, 2},
	{99.999f, 10}, {9.9999999f, 12}, {1e+12f, 1}, {5.09823e+27f, 1},
	{4.567324e+3f, 8}, {3.402823466e+38f, 1}, // FLT_MAX
	{1.175494351e-38f, 17}                    // FLT_MIN
};

struct DoubleTestSet_t {
	double m_doubleVal;
	uint_t m_MaxPrecision;
};

static const DoubleTestSet_t g_DoubleConstants[] = {
// Visual Studio 2015 and 2017 rounds this properly. VS 2019 and higher will
// not round up
#if BURGER_MSVC < 192000000
	{0.125, 10},
#endif
	{1.0, 10}, {10.0, 2}, {0.1, 10}, {0.0001, 10}, {0.00001, 10},
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

#if 1
	static const double g_NineTests[] = { 9999999999999999999999.0,

		99999999999999900000.0, 99999999999999990000.0, 99999999999999999000.0,
		99999999999999999900.0,

		0.99999999999999900000, 0.99999999999999990000, 0.99999999999999999000,
		0.99999999999999999900,

		9.99999999999999900000, 9.99999999999999990000, 9.99999999999999999000,
		9.99999999999999999900,

		99.99999999999999000000, 99.99999999999999900000, 99.99999999999999990000,
		99.99999999999999999000, 99.99999999999999999900 };
#endif

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

		for ( n = 5 ; n < BURGER_ARRAYSIZE( g_NineTests ); n++ ) {
			double  nine = g_NineTests[n];
		
			printf ( "RTL The double %%.f     = '%.f'\n", nine );
			Burger::Snprintf ( text, sizeof(text), "MGP The double %%.f     = '%.f'\n\n", nine );
			printf( "%s", text );

			printf ( "RTL The double %%.20f   = '%.20f'\n", nine );
			Burger::Snprintf ( text, sizeof(text), "MGP The double %%.20f   = '%.20f'\n\n", nine );
			printf( "%s", text );

			Burger::Snprintf ( text, sizeof(text), "MGP#The double %%.20f   = '%.20f'\n\n", nine );
			printf( "%s", text );
		}

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
