/***************************************

	Unit tests for the Integer Math library

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfixedpoint.h"
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"
#include "brstringfunctions.h"
#include "common.h"

using namespace Burger;

struct WordTest32_t {
	uint32_t m_uInput;  // Source value
	uint32_t m_uOutput; // Expected output
};

struct WordTest64_t {
	uint64_t m_uInput;  // Source value
	uint64_t m_uOutput; // Expected output
};

struct IntTest8_t {
	int8_t m_iInput;  // Source value
	int8_t m_iOutput; // Expected output
};

struct IntTest16_t {
	int16_t m_iInput;  // Source value
	int16_t m_iOutput; // Expected output
};

struct IntTest32_t {
	int32_t m_iInput;  // Source value
	int32_t m_iOutput; // Expected output
};

struct FixedTest32_t {
	fixed16_16_t m_iInput;  // Source value
	fixed16_16_t m_iOutput; // Expected output
};

struct IntTest64_t {
	int64_t m_iInput;  // Source value
	int64_t m_iOutput; // Expected output
};

struct FloatToIntTest32_t {
	uint32_float_t m_fInput; // Source value
	int32_t m_iOutput;       // Expected output
};

struct FloatToUIntTest32_t {
	uint32_float_t m_fInput; // Source value
	uint32_t m_uOutput;      // Expected output
};

struct OldFloatToIntTest32_t {
	float m_fInput;    // Source value
	int32_t m_iOutput; // Expected output
};

struct IntTest32x32_t {
	int32_t m_iInput1; // Source value 1
	int32_t m_iInput2; // Source value 2
	int32_t m_iOutput; // Expected output
};

struct IntTest64x64_t {
	int64_t m_iInput1; // Source value 1
	int64_t m_iInput2; // Source value 2
	int64_t m_iOutput; // Expected output
};

struct FloatToFloatTest_t {
	uint32_float_t m_fInput;  // Source value
	uint32_float_t m_fOutput; // Expected output
};

/***************************************

	Test Burger::get_low_word

***************************************/

static const WordTest32_t GetLoWordTable[] = {{0x00000000U, 0x00000000U},
	{0x00000001U, 0x00000001U}, {0x00000002U, 0x00000002U},
	{0x00000003U, 0x00000003U}, {0x00000004U, 0x00000004U},
	{0x00000005U, 0x00000005U}, {0x00002000U, 0x00002000U},
	{0x00002001U, 0x00002001U}, {0x20000001U, 0x00000001U},
	{0x40000000U, 0x00000000U}, {0x40000001U, 0x00000001U},
	{0x40004020U, 0x00004020U}, {0x7FFFFFFFU, 0x0000FFFFU},
	{0x80000000U, 0x00000000U}, {0x80000001U, 0x00000001U},
	{0xFFFFFFFFU, 0x0000FFFFU}};

static uint_t BURGER_API TestGetLoWord(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = GetLoWordTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(GetLoWordTable);
	do {
		const uint32_t uReturn = Burger::get_low_word(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::get_low_word(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::get_high_word

***************************************/

static const WordTest32_t GetHiWordTable[] = {{0x00000000U, 0x00000000U},
	{0x00000001U, 0x00000000U}, {0x00000002U, 0x00000000U},
	{0x00000003U, 0x00000000U}, {0x00000004U, 0x00000000U},
	{0x00000005U, 0x00000000U}, {0x00002000U, 0x00000000U},
	{0x00002001U, 0x00000000U}, {0x20000001U, 0x00002000U},
	{0x40000000U, 0x00004000U}, {0x40000001U, 0x00004000U},
	{0x40004020U, 0x00004000U}, {0x7FFFFFFFU, 0x00007FFFU},
	{0x80000000U, 0x00008000U}, {0x80000001U, 0x00008000U},
	{0xFFFFFFFFU, 0x0000FFFFU}};

static uint_t BURGER_API TestGetHiWord(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = GetHiWordTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(GetHiWordTable);
	do {
		const uint32_t uReturn = Burger::get_high_word(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::get_high_word(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test int_to_fixed(int32_t)
//

static const IntTest32_t IntToFixedTestTable[] = {{0x00000000, 0x00000000},
	{0x00000001, 0x00010000},
	{static_cast<int32_t>(0xFFFFFFFF), static_cast<int32_t>(0xFFFF0000)},
	{0x00007FFF, 0x7FFF0000}, {0x00008000, static_cast<int32_t>(0x80000000)},
	{static_cast<int32_t>(0x80000000), 0x00000000},
	{static_cast<int32_t>(0x80000001), 0x00010000}, {0x00004000, 0x40000000},
	{0x40000000, 0x00000000}, {0x0000C000, static_cast<int32_t>(0xC0000000)},
	{static_cast<int32_t>(0xFFFFC000), static_cast<int32_t>(0xC0000000)}};

static uint_t BURGER_API TestIntToFixed(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = IntToFixedTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(IntToFixedTestTable);
	do {
		const int32_t iReturn = Burger::int_to_fixed(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::int_to_fixed(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test int_to_fixed_saturate(int32_t)
//

static const IntTest32_t IntToFixedSaturateTestTable[] = {
	{0x00000000, 0x00000000},
	{0x00000001, 0x00010000},
	{0x00004000, 0x40000000},
	{0x00007FFF, 0x7FFF0000},
	{0x00008000, 0x7FFFFFFF}, // Saturation hit
	{0x00008001, 0x7FFFFFFF},
	{0x0000C000, 0x7FFFFFFF},
	{0x40000000, 0x7FFFFFFF},
	{0x7FFFFFFF, 0x7FFFFFFF},
	{static_cast<int32_t>(0xFFFFFFFF), static_cast<int32_t>(0xFFFF0000)},
	{static_cast<int32_t>(0xFFFFFFFE), static_cast<int32_t>(0xFFFE0000)},
	{static_cast<int32_t>(0xFFFFC000), static_cast<int32_t>(0xC0000000)},
	{static_cast<int32_t>(0xFFFF8000),
		static_cast<int32_t>(0x80000000)}, // Saturation hit
	{static_cast<int32_t>(0xFFFF8001), static_cast<int32_t>(0x80000000)},
	{static_cast<int32_t>(0xC0000000), static_cast<int32_t>(0x80000000)},
	{static_cast<int32_t>(0x80000001), static_cast<int32_t>(0x80000000)},
	{static_cast<int32_t>(0x80000000), static_cast<int32_t>(0x80000000)},
};

static uint_t BURGER_API TestIntToFixedSaturate(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = IntToFixedSaturateTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(IntToFixedSaturateTestTable);
	do {
		const int32_t iReturn = Burger::int_to_fixed_saturate(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::int_to_fixed_saturate(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test fixed_to_int_floor(int32_t)
//

static const IntTest32_t FixedToIntFloorTestTable[] = {
	{BURGER_FLOAT_TO_FIXED(-4.0) + 1, -4}, {BURGER_FLOAT_TO_FIXED(-3.5f), -4},
	{BURGER_FLOAT_TO_FIXED(-3.5f) + 1, -4}, {BURGER_FLOAT_TO_FIXED(-3.0f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.0f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-2.5f), -3},
	{BURGER_FLOAT_TO_FIXED(-2.5f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-2.0f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.0f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-1.5f), -2},
	{BURGER_FLOAT_TO_FIXED(-1.5f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-1.0f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.0f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-0.5f), -1},
	{BURGER_FLOAT_TO_FIXED(-0.5f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-0.0f), 0},
	{BURGER_FLOAT_TO_FIXED(0.0f), 0}, {BURGER_FLOAT_TO_FIXED(0.5f) - 1, 0},
	{BURGER_FLOAT_TO_FIXED(0.5f), 0}, {BURGER_FLOAT_TO_FIXED(1.0f) - 1, 0},
	{BURGER_FLOAT_TO_FIXED(1.0f), 1}, {BURGER_FLOAT_TO_FIXED(1.5f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(1.5f), 1}, {BURGER_FLOAT_TO_FIXED(2.0f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(2.0f), 2}, {BURGER_FLOAT_TO_FIXED(2.5f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(2.5f), 2}, {BURGER_FLOAT_TO_FIXED(3.0f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(3.0f), 3}, {BURGER_FLOAT_TO_FIXED(3.5f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(3.5f), 3}, {BURGER_FLOAT_TO_FIXED(4.0f) - 1, 3},

	{BURGER_FLOAT_TO_FIXED(1.1f), 1}, {BURGER_FLOAT_TO_FIXED(1.95f), 1},
	{BURGER_FLOAT_TO_FIXED(-1.1f), -2}, {BURGER_FLOAT_TO_FIXED(-1.95f), -2},
	{BURGER_FLOAT_TO_FIXED(0.1f), 0}, {BURGER_FLOAT_TO_FIXED(0.95f), 0},
	{BURGER_FLOAT_TO_FIXED(-0.1f), -1}, {BURGER_FLOAT_TO_FIXED(-0.95f), -1}};

static uint_t BURGER_API TestFixedToIntFloor(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntFloorTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntFloorTestTable);
	do {
		const int32_t iReturn = Burger::fixed_to_int_floor(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::fixed_to_int_floor(0x%08X) = %d, expected %d",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test fixed_to_int(int32_t)
//

static const IntTest32_t FixedToIntTestTable[] = {
	{BURGER_FLOAT_TO_FIXED(-4.0) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-3.5f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.5f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-3.0f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.0f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-2.5f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.5f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-2.0f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.0f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-1.5f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.5f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-1.0f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.0f) + 1, -0}, {BURGER_FLOAT_TO_FIXED(-0.5f), -0},
	{BURGER_FLOAT_TO_FIXED(-0.5f) + 1, -0}, {BURGER_FLOAT_TO_FIXED(-0.0f), 0},
	{BURGER_FLOAT_TO_FIXED(0.0f), 0}, {BURGER_FLOAT_TO_FIXED(0.5f) - 1, 0},
	{BURGER_FLOAT_TO_FIXED(0.5f), 0}, {BURGER_FLOAT_TO_FIXED(1.0f) - 1, 0},
	{BURGER_FLOAT_TO_FIXED(1.0f), 1}, {BURGER_FLOAT_TO_FIXED(1.5f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(1.5f), 1}, {BURGER_FLOAT_TO_FIXED(2.0f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(2.0f), 2}, {BURGER_FLOAT_TO_FIXED(2.5f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(2.5f), 2}, {BURGER_FLOAT_TO_FIXED(3.0f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(3.0f), 3}, {BURGER_FLOAT_TO_FIXED(3.5f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(3.5f), 3}, {BURGER_FLOAT_TO_FIXED(4.0f) - 1, 3},

	{BURGER_FLOAT_TO_FIXED(1.1f), 1}, {BURGER_FLOAT_TO_FIXED(1.95f), 1},
	{BURGER_FLOAT_TO_FIXED(-1.1f), -1}, {BURGER_FLOAT_TO_FIXED(-1.95f), -1},
	{BURGER_FLOAT_TO_FIXED(0.1f), 0}, {BURGER_FLOAT_TO_FIXED(0.95f), 0},
	{BURGER_FLOAT_TO_FIXED(-0.1f), 0}, {BURGER_FLOAT_TO_FIXED(-0.95f), 0}};

static uint_t BURGER_API TestFixedToInt(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntTestTable);
	do {
		const int32_t iReturn = Burger::fixed_to_int(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::fixed_to_int(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test fixed_to_int_ceil(int32_t)
//

static const IntTest32_t FixedToIntCeilTestTable[] = {
	{BURGER_FLOAT_TO_FIXED(-4.0) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-3.5f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.5f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-3.0f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.0f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-2.5f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.5f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-2.0f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.0f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-1.5f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.5f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-1.0f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.0f) + 1, -0}, {BURGER_FLOAT_TO_FIXED(-0.5f), -0},
	{BURGER_FLOAT_TO_FIXED(-0.5f) + 1, -0}, {BURGER_FLOAT_TO_FIXED(-0.0f), 0},
	{BURGER_FLOAT_TO_FIXED(0.0f), 0}, {BURGER_FLOAT_TO_FIXED(0.5f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(0.5f), 1}, {BURGER_FLOAT_TO_FIXED(1.0f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(1.0f), 1}, {BURGER_FLOAT_TO_FIXED(1.5f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(1.5f), 2}, {BURGER_FLOAT_TO_FIXED(2.0f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(2.0f), 2}, {BURGER_FLOAT_TO_FIXED(2.5f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(2.5f), 3}, {BURGER_FLOAT_TO_FIXED(3.0f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(3.0f), 3}, {BURGER_FLOAT_TO_FIXED(3.5f) - 1, 4},
	{BURGER_FLOAT_TO_FIXED(3.5f), 4}, {BURGER_FLOAT_TO_FIXED(4.0f) - 1, 4},

	{BURGER_FLOAT_TO_FIXED(1.1f), 2}, {BURGER_FLOAT_TO_FIXED(1.95f), 2},
	{BURGER_FLOAT_TO_FIXED(-1.1f), -1}, {BURGER_FLOAT_TO_FIXED(-1.95f), -1},
	{BURGER_FLOAT_TO_FIXED(0.1f), 1}, {BURGER_FLOAT_TO_FIXED(0.95f), 1},
	{BURGER_FLOAT_TO_FIXED(-0.1f), 0}, {BURGER_FLOAT_TO_FIXED(-0.95f), 0}};

static uint_t BURGER_API TestFixedToIntCeil(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntCeilTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntCeilTestTable);
	do {
		const int32_t iReturn = Burger::fixed_to_int_ceil(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::fixed_to_int_ceil(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test fixed_to_int_nearest(int32_t)
//

static const IntTest32_t FixedToIntNearestTestTable[] = {
	{BURGER_FLOAT_TO_FIXED(-4.0) + 1, -4}, {BURGER_FLOAT_TO_FIXED(-3.5f), -4},
	{BURGER_FLOAT_TO_FIXED(-3.5f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-3.0f), -3},
	{BURGER_FLOAT_TO_FIXED(-3.0f) + 1, -3}, {BURGER_FLOAT_TO_FIXED(-2.5f), -3},
	{BURGER_FLOAT_TO_FIXED(-2.5f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-2.0f), -2},
	{BURGER_FLOAT_TO_FIXED(-2.0f) + 1, -2}, {BURGER_FLOAT_TO_FIXED(-1.5f), -2},
	{BURGER_FLOAT_TO_FIXED(-1.5f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-1.0f), -1},
	{BURGER_FLOAT_TO_FIXED(-1.0f) + 1, -1}, {BURGER_FLOAT_TO_FIXED(-0.5f), -1},
	{BURGER_FLOAT_TO_FIXED(-0.5f) + 1, -0}, {BURGER_FLOAT_TO_FIXED(-0.0f), 0},
	{BURGER_FLOAT_TO_FIXED(0.0f), 0}, {BURGER_FLOAT_TO_FIXED(0.5f) - 1, 0},
	{BURGER_FLOAT_TO_FIXED(0.5f), 1}, {BURGER_FLOAT_TO_FIXED(1.0f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(1.0f), 1}, {BURGER_FLOAT_TO_FIXED(1.5f) - 1, 1},
	{BURGER_FLOAT_TO_FIXED(1.5f), 2}, {BURGER_FLOAT_TO_FIXED(2.0f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(2.0f), 2}, {BURGER_FLOAT_TO_FIXED(2.5f) - 1, 2},
	{BURGER_FLOAT_TO_FIXED(2.5f), 3}, {BURGER_FLOAT_TO_FIXED(3.0f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(3.0f), 3}, {BURGER_FLOAT_TO_FIXED(3.5f) - 1, 3},
	{BURGER_FLOAT_TO_FIXED(3.5f), 4}, {BURGER_FLOAT_TO_FIXED(4.0f) - 1, 4},

	{BURGER_FLOAT_TO_FIXED(1.1f), 1}, {BURGER_FLOAT_TO_FIXED(1.95f), 2},
	{BURGER_FLOAT_TO_FIXED(-1.1f), -1}, {BURGER_FLOAT_TO_FIXED(-1.95f), -2},
	{BURGER_FLOAT_TO_FIXED(0.1f), 0}, {BURGER_FLOAT_TO_FIXED(0.95f), 1},
	{BURGER_FLOAT_TO_FIXED(-0.1f), 0}, {BURGER_FLOAT_TO_FIXED(-0.95f), -1}};

static uint_t BURGER_API TestFixedToIntNearest(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntNearestTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntNearestTestTable);
	do {
		const int32_t iReturn = Burger::fixed_to_int_nearest(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::fixed_to_int_nearest(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_int_floor(float)
//

static const OldFloatToIntTest32_t FloatToIntFloorTestTable[] = {{-3.999f, -4},
	{-3.5f, -4}, {-3.499f, -4}, {-3.0f, -3}, {-2.999f, -3}, {-2.5f, -3},
	{-2.499f, -3}, {-2.0f, -2}, {-1.999f, -2}, {-1.5f, -2}, {-1.499f, -2},
	{-1.0f, -1}, {-0.999f, -1}, {-0.5f, -1}, {-0.499f, -1}, {-0.0f, 0},

	{0.0f, 0}, {0.499f, 0}, {0.5f, 0}, {0.999f, 0}, {1.0f, 1}, {1.499f, 1},
	{1.5f, 1}, {1.999f, 1}, {2.0f, 2}, {2.499f, 2}, {2.5f, 2}, {2.999f, 2},
	{3.0f, 3}, {3.499f, 3}, {3.5f, 3}, {3.999f, 3},

	{1.1f, 1}, {1.95f, 1}, {-1.1f, -2}, {-1.95f, -2}, {0.1f, 0}, {0.95f, 0},
	{-0.1f, -1}, {-0.95f, -1}};

static uint_t BURGER_API TestFloatToIntFloor(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToIntFloorTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToIntFloorTestTable);
	do {
		const int32_t iReturn = Burger::float_to_int_floor(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_floor(%g) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_int_floor(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_floor(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_int_round_to_zero(float)
//

static const OldFloatToIntTest32_t FloatToIntTestTable[] = {{-3.999f, -3},
	{-3.5f, -3}, {-3.499f, -3}, {-3.0f, -3}, {-2.999f, -2}, {-2.5f, -2},
	{-2.499f, -2}, {-2.0f, -2}, {-1.999f, -1}, {-1.5f, -1}, {-1.499f, -1},
	{-1.0f, -1}, {-0.999f, -0}, {-0.5f, -0}, {-0.499f, -0}, {-0.0f, 0},

	{0.0f, 0}, {0.499f, 0}, {0.5f, 0}, {0.999f, 0}, {1.0f, 1}, {1.499f, 1},
	{1.5f, 1}, {1.999f, 1}, {2.0f, 2}, {2.499f, 2}, {2.5f, 2}, {2.999f, 2},
	{3.0f, 3}, {3.499f, 3}, {3.5f, 3}, {3.999f, 3},

	{1.1f, 1}, {1.95f, 1}, {-1.1f, -1}, {-1.95f, -1}, {0.1f, 0}, {0.95f, 0},
	{-0.1f, 0}, {-0.95f, 0}};

static uint_t BURGER_API TestFloatToIntRoundToZero(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToIntTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToIntTestTable);
	do {
		const int32_t iReturn =
			Burger::float_to_int_round_to_zero(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_round_to_zero(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_int_round_to_zero(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_round_to_zero(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_int_ceil(float)
//

static const OldFloatToIntTest32_t FloatToIntCeilTestTable[] = {{-3.999f, -3},
	{-3.5f, -3}, {-3.499f, -3}, {-3.0f, -3}, {-2.999f, -2}, {-2.5f, -2},
	{-2.499f, -2}, {-2.0f, -2}, {-1.999f, -1}, {-1.5f, -1}, {-1.499f, -1},
	{-1.0f, -1}, {-0.999f, -0}, {-0.5f, -0}, {-0.499f, -0}, {-0.0f, 0},

	{0.0f, 0}, {0.499f, 1}, {0.5f, 1}, {0.999f, 1}, {1.0f, 1}, {1.499f, 2},
	{1.5f, 2}, {1.999f, 2}, {2.0f, 2}, {2.499f, 3}, {2.5f, 3}, {2.999f, 3},
	{3.0f, 3}, {3.499f, 4}, {3.5f, 4}, {3.999f, 4},

	{1.1f, 2}, {1.95f, 2}, {-1.1f, -1}, {-1.95f, -1}, {0.1f, 1}, {0.95f, 1},
	{-0.1f, 0}, {-0.95f, 0}};

static uint_t BURGER_API TestFloatToIntCeil(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToIntCeilTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToIntCeilTestTable);
	do {
		const int32_t iReturn = Burger::float_to_int_ceil(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::float_to_int_ceil(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_int_ceil(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_ceil(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_int_round(float)
//

static const OldFloatToIntTest32_t FloatToIntNearestTestTable[] = {
	{-3.999f, -4}, {-3.5f, -4}, {-3.499f, -3}, {-3.0f, -3}, {-2.999f, -3},
	{-2.5f, -3}, {-2.499f, -2}, {-2.0f, -2}, {-1.999f, -2}, {-1.5f, -2},
	{-1.499f, -1}, {-1.0f, -1}, {-0.999f, -1}, {-0.5f, -1}, {-0.499f, 0},
	{-0.0f, 0},

	{0.0f, 0}, {0.499f, 0}, {0.5f, 1}, {0.999f, 1}, {1.0f, 1}, {1.499f, 1},
	{1.5f, 2}, {1.999f, 2}, {2.0f, 2}, {2.499f, 2}, {2.5f, 3}, {2.999f, 3},
	{3.0f, 3}, {3.499f, 3}, {3.5f, 4}, {3.999f, 4},

	{1.1f, 1}, {1.95f, 2}, {-1.1f, -1}, {-1.95f, -2}, {0.1f, 0}, {0.95f, 1},
	{-0.1f, 0}, {-0.95f, -1}};

static uint_t BURGER_API TestFloatToIntRound(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToIntNearestTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToIntNearestTestTable);
	do {
		const int32_t iReturn = Burger::float_to_int_round(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::float_to_int_round(%g) = %d, expected %d", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_int_round(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_int_round(int32_t *pOutput,%g) = %d, expected %d",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_fixed_floor(float)
//

static const OldFloatToIntTest32_t FloatToFixedFloorTestTable[] = {
#if 0
	{-3.999f,-4},
	{-3.5f	,-4},
	{-3.499f,-3},
	{-3.0f	,-3},
	{-2.999f,-3},
	{-2.5f	,-3},
	{-2.499f,-2},
	{-2.0f	,-2},
	{-1.999f,-2},
	{-1.5f	,-2},
	{-1.499f,-1},
	{-1.0f	,-1},
	{-0.999f,-1},
	{-0.5f	,-1},
	{-0.499f,0},
	{-0.0f	,0},
	{0.0f	,0},
	{0.499f	,0},
	{0.5f	,1},
	{0.999f	,1},
	{1.0f	,1},
	{1.499f	,1},
	{1.5f	,2},
	{1.999f	,2},
	{2.0f	,2},
	{2.499f	,2},
	{2.5f	,3},
	{2.999f	,3},
	{3.0f	,3},
	{3.499f	,3},
	{3.5f	,4},
	{3.999f	,4},
#endif

	{1.1f, 0x00011999}, {1.95f, 0x0001F333},
	{-1.1f, static_cast<int32_t>(0xFFFEE666)},
	{-1.95f, static_cast<int32_t>(0xFFFE0CCC)}, {0.1f, 0x00001999},
	{0.95f, 0x0000F333}, {-0.1f, static_cast<int32_t>(0xFFFFE666)},
	{-0.95f, static_cast<int32_t>(0xFFFF0CCC)}};

static uint_t BURGER_API TestFloatToFixedFloor(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToFixedFloorTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToFixedFloorTestTable);
	do {
		const int32_t iReturn = Burger::float_to_fixed_floor(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_floor(%g) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_fixed_floor(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_floor(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_fixed_round_to_zero(float)
//

static const OldFloatToIntTest32_t FloatToFixedTestTable[] = {
	{1.1f, 0x00011999}, {1.95f, 0x0001F333},
	{-1.1f, static_cast<int32_t>(0xFFFEE667)},
	{-1.95f, static_cast<int32_t>(0xFFFE0CCD)}, {0.1f, 0x00001999},
	{0.95f, 0x0000F333}, {-0.1f, static_cast<int32_t>(0xFFFFE667)},
	{-0.95f, static_cast<int32_t>(0xFFFF0CCD)}};

static uint_t BURGER_API TestFloatToFixed(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToFixedTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToFixedTestTable);
	do {
		const int32_t iReturn =
			Burger::float_to_fixed_round_to_zero(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_round_to_zero(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_fixed_round_to_zero(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_round_to_zero(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_fixed_ceil(float)
//

static const OldFloatToIntTest32_t FloatToFixedCeilTestTable[] = {
	{1.1f, 0x0001199A}, {1.95f, 0x0001F334},
	{-1.1f, static_cast<int32_t>(0xFFFEE667)},
	{-1.95f, static_cast<int32_t>(0xFFFE0CCD)}, {0.1f, 0x0000199A},
	{0.95f, 0x0000F334}, {-0.1f, static_cast<int32_t>(0xFFFFE667)},
	{-0.95f, static_cast<int32_t>(0xFFFF0CCD)}};

static uint_t BURGER_API TestFloatToFixedCeil(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToFixedCeilTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToFixedCeilTestTable);
	do {
		const int32_t iReturn = Burger::float_to_fixed_ceil(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_ceil(%g) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_fixed_ceil(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_ceil(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test float_to_fixed_round(float)
//

static const OldFloatToIntTest32_t FloatToFixedNearestTestTable[] = {
	{1.1f, 0x0001199A}, {1.95f, 0x0001F333},
	{-1.1f, static_cast<int32_t>(0xFFFEE666)},
	{-1.95f, static_cast<int32_t>(0xFFFE0CCD)}, {0.1f, 0x0000199A},
	{0.95f, 0x0000F333}, {-0.1f, static_cast<int32_t>(0xFFFFE666)},
	{-0.95f, static_cast<int32_t>(0xFFFF0CCD)}};

static uint_t BURGER_API TestFloatToFixedNearest(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const OldFloatToIntTest32_t* pWork = FloatToFixedNearestTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FloatToFixedNearestTestTable);
	do {
		const int32_t iReturn = Burger::float_to_fixed_round(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_round(%g) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::float_to_fixed_round(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::float_to_fixed_round(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test absolute(int8_t)
//

static const IntTest8_t AbsTestTable8[] = {{0x00, 0x00}, {0x01, 0x01},
	{0x40, 0x40}, {(-0x7F) - 1, (-0x7F) - 1}, {-0x7F, 0x7F}, {-0x40, 0x40},
	{-0x01, 0x01}};

static uint_t BURGER_API TestAbs8(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest8_t* pWork = AbsTestTable8;
	uintptr_t uCount = BURGER_ARRAYSIZE(AbsTestTable8);
	do {
		const int8_t iReturn = Burger::absolute(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::absolute((int8_t)0x%08X) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test absolute(int16_t)
//

static const IntTest16_t AbsTestTable16[] = {{0x0000, 0x0000}, {0x0001, 0x0001},
	{0x4000, 0x4000}, {(-0x7FFF) - 1, (-0x7FFF) - 1}, {-0x7FFF, 0x7FFF},
	{-0x4000, 0x4000}, {-0x0001, 0x0001}};

static uint_t BURGER_API TestAbs16(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest16_t* pWork = AbsTestTable16;
	uintptr_t uCount = BURGER_ARRAYSIZE(AbsTestTable16);
	do {
		const int16_t iReturn = Burger::absolute(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::absolute((int16_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test absolute(int32_t)
//

static const IntTest32_t AbsTestTable32[] = {{0x00000000, 0x00000000},
	{0x00000001, 0x00000001}, {0x40000000, 0x40000000},
	{static_cast<int32_t>(0x80000000), static_cast<int32_t>(0x80000000)},
	{static_cast<int32_t>(0x80000001), 0x7FFFFFFF},
	{static_cast<int32_t>(0xC0000000), 0x40000000},
	{static_cast<int32_t>(0xFFFFFFFF), 0x00000001}};

static uint_t BURGER_API TestAbs32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = AbsTestTable32;
	uintptr_t uCount = BURGER_ARRAYSIZE(AbsTestTable32);
	do {
		const int32_t iReturn = Burger::absolute(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::absolute((int32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test absolute(int64_t)
//

static const IntTest64_t AbsTestTable64[] = {
	{0x0000000000000000LL, 0x0000000000000000LL},
	{0x0000000000000001LL, 0x0000000000000001LL},
	{0x4000000000000000LL, 0x4000000000000000LL},
	{static_cast<int64_t>(0x8000000000000000LL),
		static_cast<int64_t>(0x8000000000000000LL)},
	{-0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL},
	{-0x4000000000000000LL, 0x4000000000000000LL},
	{-0x000000000000001LL, 0x0000000000000001LL}};

static uint_t BURGER_API TestAbs64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest64_t* pWork = AbsTestTable64;
	uintptr_t uCount = BURGER_ARRAYSIZE(AbsTestTable64);
	do {
		const int64_t iReturn = Burger::absolute(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			const Burger::NumberStringHex Text1(pWork->m_iInput);
			const Burger::NumberStringHex Text2(iReturn);
			const Burger::NumberStringHex Text3(pWork->m_iOutput);
			ReportFailure(
				"Burger::absolute((int64_t)0x%s) = 0x%s, expected 0x%s", uTest,
				Text1.c_str(), Text2.c_str(), Text3.c_str());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test get_sign(int8_t)
//

static const IntTest8_t SignTestTable8[] = {{0x00, 0}, {0x01, 1}, {0x40, 1},
	{(-0x7F) - 1, -1}, {-0x7F, -1}, {-0x40, -1}, {-0x01, -1}};

static uint_t BURGER_API TestSign8(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest8_t* pWork = SignTestTable8;
	uintptr_t uCount = BURGER_ARRAYSIZE(SignTestTable8);
	do {
		const int8_t iReturn = Burger::get_sign(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::get_sign((int8_t)0x%08X) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test get_sign(int16_t)
//

static const IntTest16_t SignTestTable16[] = {{0x0000, 0}, {0x0001, 1},
	{0x4000, 1}, {(-0x7FFF) - 1, -1}, {-0x7FFF, -1}, {-0x4000, -1},
	{-0x0001, -1}};

static uint_t BURGER_API TestSign16(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest16_t* pWork = SignTestTable16;
	uintptr_t uCount = BURGER_ARRAYSIZE(SignTestTable16);
	do {
		const int16_t iReturn = Burger::get_sign(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::get_sign((int16_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test get_sign(int32_t)
//

static const IntTest32_t SignTestTable32[] = {{0x00000000, 0}, {0x00000001, 1},
	{0x40000000, 1}, {static_cast<int32_t>(0x80000000), -1},
	{static_cast<int32_t>(0x80000001), -1},
	{static_cast<int32_t>(0xC0000000), -1},
	{static_cast<int32_t>(0xFFFFFFFF), -1}};

static uint_t BURGER_API TestSign32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = SignTestTable32;
	uintptr_t uCount = BURGER_ARRAYSIZE(SignTestTable32);
	do {
		const int32_t iReturn = Burger::get_sign(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::get_sign((int32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test get_sign(int64_t)
//

static const IntTest64_t SignTestTable64[] = {{0x0000000000000000LL, 0},
	{0x0000000000000001LL, 1}, {0x4000000000000000LL, 1},
	{static_cast<int64_t>(0x8000000000000000LL), -1},
	{static_cast<int64_t>(0x8000000000000001LL), -1},
	{static_cast<int64_t>(0xC000000000000000LL), -1},
	{static_cast<int64_t>(0xFFFFFFFFFFFFFFFFLL), -1}};

static uint_t BURGER_API TestSign64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest64_t* pWork = SignTestTable64;
	uintptr_t uCount = BURGER_ARRAYSIZE(SignTestTable64);
	do {
		const int64_t iReturn = Burger::get_sign(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			const Burger::NumberStringHex Text1(pWork->m_iInput);
			const Burger::NumberStringHex Text2(iReturn);
			const Burger::NumberStringHex Text3(pWork->m_iOutput);
			ReportFailure(
				"Burger::get_sign((int64_t)0x%s) = 0x%s, expected 0x%s", uTest,
				Text1.c_str(), Text2.c_str(), Text3.c_str());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test square_root(int32_t)
//

static const WordTest32_t Sqrt32TestTable[] = {{0x00000000, 0x00000000},
	{0x00000001, 0x00000001}, // 1^2
	{0x00000010, 0x00000004}, // 4^2
	{0x00000020, 0x00000006}, // 5.8^2
	{0x40000000, 0x00008000}, {0x7FFFFFFF, 0x0000B505},
	{0x80000000, 0x0000B505}, {0x80000001, 0x0000B505},
	{0xC0000000, 0x0000DDB4}, {0xFFFC0004, 0x0000FFFE}, // 65534^2
	{0xFFFD0002, 0x0000FFFE}, {0xFFFD0003, 0x0000FFFF},
	{0xFFFE0001, 0x0000FFFF}, // 65535^2
	{0xFFFFFFFF, 0x00010000}};

static uint_t BURGER_API TestSqrt32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = Sqrt32TestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(Sqrt32TestTable);
	do {
		const uint32_t uReturn = Burger::square_root(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::square_root((uint32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test SqrtFixedToWord32(fixed16_16_t)
//

static const FixedTest32_t SqrtFixedToWord32TestTable[] = {
	{0x00000000, 0x00000000}, {0x00000001, 0x00000000}, // 1^2
	{0x00000010, 0x00000000},                           // 4^2
	{0x00000020, 0x00000000},                           // 5.8^2
	{0x40000000, 0x00000080},
	{0x7FFFFFFF, 0x000000B5}, // Maximum square root (65535.999999)
	{static_cast<int32_t>(0x80000000), 0x00000000}, // Fail all negative numbers
	{static_cast<int32_t>(0x80000001), 0x00000000},
	{static_cast<int32_t>(0xC0000000), 0x00000000},
	{static_cast<int32_t>(0xFFFC0004), 0x00000000},
	{static_cast<int32_t>(0xFFFD0002), 0x00000000},
	{static_cast<int32_t>(0xFFFD0003), 0x00000000},
	{static_cast<int32_t>(0xFFFE0001), 0x00000000},
	{static_cast<int32_t>(0xFFFFFFFF), 0x00000000}};

static uint_t BURGER_API TestSqrtFixedToWord32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const FixedTest32_t* pWork = SqrtFixedToWord32TestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(SqrtFixedToWord32TestTable);
	do {
		const uint32_t uReturn = Burger::SqrtFixedToWord32(pWork->m_iInput);
		const uint_t uTest = uReturn != static_cast<uint32_t>(pWork->m_iOutput);
		uFailure |= uTest;
		ReportFailure(
			"Burger::SqrtFixedToWord32((fixed16_16_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, uReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test square_root(fixed16_16_t)
//

static const FixedTest32_t SqrtFixed32TestTable[] = {{0x00000000, 0x00000000},
	{0x00000001, 0x00000100}, // 1^2
	{0x00000010, 0x00000400}, // 4^2
	{0x00000020, 0x000005A8}, // 5.8^2
	{0x00010000, 0x00010000},
	{0x00030000, 0x0001BB68}, // 3 -> 1.732050807568877f
	{0x00100000, 0x00040000}, {0x017A8E56, 0x001374DE}, // 378.556f -> 19.45652f
	{0x40000000, 0x00800000},
	{0x7FFFFFFF, 0x00B5043E}, // Maximum square root (65535.999999)
	{static_cast<int32_t>(0x80000000), 0x00000000}, // Fail all negative numbers
	{static_cast<int32_t>(0x80000001), 0x00000000},
	{static_cast<int32_t>(0xC0000000), 0x00000000},
	{static_cast<int32_t>(0xFFFC0004), 0x00000000},
	{static_cast<int32_t>(0xFFFD0002), 0x00000000},
	{static_cast<int32_t>(0xFFFD0003), 0x00000000},
	{static_cast<int32_t>(0xFFFE0001), 0x00000000},
	{static_cast<int32_t>(0xFFFFFFFF), 0x00000000}};

static uint_t BURGER_API TestSqrtFixed32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const FixedTest32_t* pWork = SqrtFixed32TestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(SqrtFixed32TestTable);
	do {
		const fixed16_16_t iReturn = Burger::square_root(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::square_root((fixed16_16_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test fixed point math

***************************************/

int BURGER_API TestBrfixedpoint(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Integer Math tests");
	}

	uint_t uResult = TestGetLoWord();
	uResult |= TestGetHiWord();

	uResult |= TestIntToFixed();
	uResult |= TestIntToFixedSaturate();

	uResult |= TestFixedToIntFloor();
	uResult |= TestFixedToIntCeil();
	uResult |= TestFixedToInt();
	uResult |= TestFixedToIntNearest();

	uResult |= TestFloatToIntFloor();
	uResult |= TestFloatToIntCeil();
	uResult |= TestFloatToIntRound();
	uResult |= TestFloatToIntRoundToZero();

	uResult |= TestFloatToFixedFloor();
	uResult |= TestFloatToFixed();
	uResult |= TestFloatToFixedCeil();
	uResult |= TestFloatToFixedNearest();

	uResult |= TestAbs8();
	uResult |= TestAbs16();
	uResult |= TestAbs32();
	uResult |= TestAbs64();
	uResult |= TestSign8();
	uResult |= TestSign16();
	uResult |= TestSign32();
	uResult |= TestSign64();
	uResult |= TestSqrt32();
	uResult |= TestSqrtFixedToWord32();
	uResult |= TestSqrtFixed32();

	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Integer Math tests!");
	}
	return static_cast<int>(uResult);
}
