/***************************************

	Unit tests for the Integer Math library

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	Fixed32 m_iInput;  // Source value
	Fixed32 m_iOutput; // Expected output
};

struct IntTest64_t {
	int64_t m_iInput;  // Source value
	int64_t m_iOutput; // Expected output
};

struct FloatToIntTest32_t {
	Burger::uint32_float_t m_fInput; // Source value
	int32_t m_iOutput;               // Expected output
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

/***************************************

	Test Burger::PowerOf2(uint32_t)

***************************************/

static const WordTest32_t PowerOf2_32Table[] = {{0x00000000U, 0x00000000U},
	{0x00000001U, 0x00000001U}, {0x00000002U, 0x00000002U},
	{0x00000003U, 0x00000004U}, {0x00000004U, 0x00000004U},
	{0x00000005U, 0x00000008U}, {0x00002000U, 0x00002000U},
	{0x00002001U, 0x00004000U}, {0x20000001U, 0x40000000U},
	{0x40000000U, 0x40000000U}, {0x40000001U, 0x80000000U},
	{0x40004020U, 0x80000000U}, {0x7FFFFFFFU, 0x80000000U},
	{0x80000000U, 0x80000000U}, {0x80000001U, 0x00000000U},
	{0xC0000000U, 0x00000000U}, {0xE0000001U, 0x00000000U},
	{0xFFFFFFFFU, 0x00000000U}};

static uint_t BURGER_API TestPowerOf2_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = PowerOf2_32Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_32Table);
	do {
		const uint32_t uReturn = Burger::PowerOf2(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::PowerOf2(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::PowerOf2(uint64_t)

***************************************/

static const WordTest64_t PowerOf2_64Table[] = {
	{0x0000000000000000ULL, 0x0000000000000000ULL},
	{0x0000000000000001ULL, 0x0000000000000001ULL},
	{0x0000000000000002ULL, 0x0000000000000002ULL},
	{0x0000000000000003ULL, 0x0000000000000004ULL},
	{0x0000000000000004ULL, 0x0000000000000004ULL},
	{0x0000000000000005ULL, 0x0000000000000008ULL},
	{0x0000000000002000ULL, 0x0000000000002000ULL},
	{0x0000000000002001ULL, 0x0000000000004000ULL},
	{0x0000000020000001ULL, 0x0000000040000000ULL},
	{0x0000000040000000ULL, 0x0000000040000000ULL},
	{0x0000000040000001ULL, 0x0000000080000000ULL},
	{0x0000000040004020ULL, 0x0000000080000000ULL},
	{0x000000007FFFFFFFULL, 0x0000000080000000ULL},
	{0x0000000080000000ULL, 0x0000000080000000ULL},
	{0x0000000080000001ULL, 0x0000000100000000ULL},
	{0x00000000FFFFFFFFULL, 0x0000000100000000ULL},
	{0x4000000000000000ULL, 0x4000000000000000ULL},
	{0x4000000000000001ULL, 0x8000000000000000ULL},
	{0x4000000000004020ULL, 0x8000000000000000ULL},
	{0x8000000000000000ULL, 0x8000000000000000ULL},
	{0x8000000000000001ULL, 0x0000000000000000ULL},
	{0xC000000000000000ULL, 0x0000000000000000ULL},
	{0xE000000000000001ULL, 0x0000000000000000ULL},
	{0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL}};

static uint_t BURGER_API TestPowerOf2_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest64_t* pWork = PowerOf2_64Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_64Table);
	do {
		const uint64_t uReturn = Burger::PowerOf2(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		if (uTest) {
			const Burger::NumberStringHex Input(pWork->m_uInput);
			const Burger::NumberStringHex Return(uReturn);
			const Burger::NumberStringHex Output(pWork->m_uOutput);
			ReportFailure("Burger::PowerOf2(0x%s) = 0x%s, expected 0x%s", uTest,
				Input.c_str(), Return.c_str(), Output.c_str());
		}
		++pWork;
	} while (--uCount);

	// Should compile without warnings
	Burger::PowerOf2(static_cast<uint8_t>(1));
	Burger::PowerOf2(static_cast<uint16_t>(1));
	Burger::PowerOf2(static_cast<uintptr_t>(1));
	Burger::PowerOf2(static_cast<unsigned long>(1));
	Burger::PowerOf2(static_cast<unsigned>(1));
	Burger::PowerOf2(static_cast<unsigned long long>(1));
	Burger::PowerOf2(static_cast<uintptr_t>(1));
	return uFailure;
}

/***************************************

	Test Burger::PowerOf2(uint32_t)

***************************************/

static const WordTest32_t BCDWordTable[] = {{0x00000000U, 0U},
	{0x00000001U, 1U}, {0x00000002U, 2U}, {0x00000003U, 3U}, {0x00000004U, 4U},
	{0x98765432U, 98765432U}, {0x00002000U, 2000U}, {0x00002001U, 2001U},
	{0x20000001U, 20000001U}, {0x40000000U, 40000000U},
	{0x40000001U, 40000001U}, {0x40004020U, 40004020U},
	{0x12345678U, 12345678U}, {0x99999999U, 99999999U},
	{0x98765432U, 98765432U}, {0x87654321U, 87654321U},
	{0x8000000FU, 80000015U}, {0xFFFFFFFFU, 166666665}};

static uint_t BURGER_API TestBCDWord(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = BCDWordTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(BCDWordTable);
	do {
		const uint32_t uReturn = Burger::BCDToWord(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::BCDToWord(0x%08X) = %u, expected %u", uTest,
			pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::BitReverse(uint32_t,uint_t)

***************************************/

static const WordTest32_t BitReverse_32Table[] = {{0x00000000U, 0x00000000U},
	{0x00000001U, 0x80000000U}, {0x00000002U, 0x40000000U},
	{0x00000003U, 0xC0000000U}, {0x00000004U, 0x20000000U},
	{0x00000005U, 0xA0000000U}, {0x00002000U, 0x00040000U},
	{0x00002001U, 0x80040000U}, {0x20000001U, 0x80000004U},
	{0x40000000U, 0x00000002U}, {0x40000001U, 0x80000002U},
	{0x40004020U, 0x04020002U}, {0x7FFFFFFFU, 0xFFFFFFFEU},
	{0x80000000U, 0x00000001U}, {0x80000001U, 0x80000001U},
	{0xFFFFFFFFU, 0xFFFFFFFFU}, {0xAAAAAAAAU, 0x55555555U},
	{0xAAAA0000U, 0x00005555U}, {0x12345678U, 0x1E6A2C48U}};

static uint_t BURGER_API TestBitReverse_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uBitCount = 1;
	do {
		const WordTest32_t* pWork = BitReverse_32Table;
		uintptr_t uCount = BURGER_ARRAYSIZE(BitReverse_32Table);
		do {
			const uint32_t uReturn =
				Burger::BitReverse(pWork->m_uInput, uBitCount);
			const uint32_t uToTest = pWork->m_uOutput >> (32 - uBitCount);
			const uint_t uTest = uReturn != uToTest;
			uFailure |= uTest;
			ReportFailure(
				"Burger::BitReverse(0x%08X,%u) = 0x%08X, expected 0x%08X",
				uTest, pWork->m_uInput, uBitCount, uReturn, uToTest);
			++pWork;
		} while (--uCount);
	} while (++uBitCount < (32 + 1));
	return uFailure;
}

/***************************************

	Test Burger::BitReverse(uint64_t,uint_t)

***************************************/

static const WordTest64_t BitReverse_64Table[] = {
	{0x0000000000000000ULL, 0x0000000000000000ULL},
	{0x0000000000000001ULL, 0x8000000000000000ULL},
	{0x0000000000000002ULL, 0x4000000000000000ULL},
	{0x0000000000000003ULL, 0xC000000000000000ULL},
	{0x0000000000000004ULL, 0x2000000000000000ULL},
	{0x0000000000000005ULL, 0xA000000000000000ULL},
	{0x0000000000002000ULL, 0x0004000000000000ULL},
	{0x0000000000002001ULL, 0x8004000000000000ULL},
	{0x2000000000000001ULL, 0x8000000000000004ULL},
	{0x4000000000000000ULL, 0x0000000000000002ULL},
	{0x4000000000000001ULL, 0x8000000000000002ULL},
	{0x4000000000004020ULL, 0x0402000000000002ULL},
	{0x7FFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFEULL},
	{0x8000000000000000ULL, 0x0000000000000001ULL},
	{0x8000000000000001ULL, 0x8000000000000001ULL},
	{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL},
	{0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL},
	{0xAAAAAAAA00000000ULL, 0x0000000055555555ULL},
	{0x1234567890ABCDEFULL, 0xF7B3D5091E6A2C48ULL}};

static uint_t BURGER_API TestBitReverse_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uBitCount = 1;
	do {
		const WordTest64_t* pWork = BitReverse_64Table;
		uintptr_t uCount = BURGER_ARRAYSIZE(BitReverse_64Table);
		do {
			const uint64_t uReturn =
				Burger::BitReverse(pWork->m_uInput, uBitCount);
			const uint64_t uToTest = pWork->m_uOutput >> (64 - uBitCount);
			const uint_t uTest = uReturn != uToTest;
			uFailure |= uTest;
			if (uTest) {
				const Burger::NumberStringHex Input(pWork->m_uInput);
				const Burger::NumberStringHex Return(uReturn);
				const Burger::NumberStringHex Output(pWork->m_uOutput);
				ReportFailure(
					"Burger::BitReverse(0x%s,%u) = 0x%s, expected 0x%s", uTest,
					Input.c_str(), uBitCount, Return.c_str(), Output.c_str());
			}
			++pWork;
		} while (--uCount);
	} while (++uBitCount < (64 + 1));
	return uFailure;
}

/***************************************

	Test Burger::BitSetCount(uint32_t)

***************************************/

static const WordTest32_t BitSetCount_32Table[] = {{0x00000000U, 0},
	{0x00000001U, 1}, {0x00000002U, 1}, {0x00000003U, 2}, {0x00000004U, 1},
	{0x00000005U, 2}, {0x00002000U, 1}, {0x00002001U, 2}, {0x20000001U, 2},
	{0x40000000U, 1}, {0x40000001U, 2}, {0x40004020U, 3}, {0x7FFFFFFFU, 31},
	{0x80000000U, 1}, {0x80000001U, 2}, {0x55555555U, 16}, {0x55550000U, 8},
	{0x0000AAAAU, 8}, {0xAAAAAAAAU, 16}, {0xFFFFFFFFU, 32}, {0x12345678U, 13}};

static uint_t BURGER_API TestBitSetCount_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = BitSetCount_32Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(BitSetCount_32Table);
	do {
		const uint_t uReturn = Burger::BitSetCount(pWork->m_uInput);
		const uint_t uToTest = static_cast<uint_t>(pWork->m_uOutput);
		const uint_t uTest = uReturn != uToTest;
		uFailure |= uTest;
		ReportFailure("Burger::BitSetCount(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, uToTest);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::BitSetCount(uint64_t)

***************************************/

static const WordTest64_t BitSetCount_64Table[] = {{0x0000000000000000ULL, 0},
	{0x0000000000000001ULL, 1}, {0x0000000000000002ULL, 1},
	{0x0000000000000003ULL, 2}, {0x0000000000000004ULL, 1},
	{0x0000000000000005ULL, 2}, {0x0000000000002000ULL, 1},
	{0x0000000000002001ULL, 2}, {0x2000000000000001ULL, 2},
	{0x4000000000000000ULL, 1}, {0x4000000000000001ULL, 2},
	{0x4000000000004020ULL, 3}, {0x7FFFFFFFFFFFFFFFULL, 63},
	{0x8000000000000000ULL, 1}, {0x8000000000000001ULL, 2},
	{0xFFFFFFFFFFFFFFFFULL, 64}, {0x5555555555555555ULL, 32},
	{0xAAAAAAAAAAAAAAAAULL, 32}, {0x5555555500000000ULL, 16},
	{0x00000000AAAAAAAAULL, 16}, {0x1234567890ABCDEFULL, 32}};

static uint_t BURGER_API TestBitSetCount_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest64_t* pWork = BitSetCount_64Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(BitSetCount_64Table);
	do {
		const uint_t uReturn = Burger::BitSetCount(pWork->m_uInput);
		const uint_t uToTest = static_cast<uint_t>(pWork->m_uOutput);
		const uint_t uTest = uReturn != uToTest;
		uFailure |= uTest;
		if (uTest) {
			const Burger::NumberStringHex Input(pWork->m_uInput);
			ReportFailure("Burger::BitSetCount(0x%s) = 0x%08X, expected 0x%08X",
				uTest, Input.c_str(), uReturn, uToTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::GetLoWord

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
		const uint32_t uReturn = Burger::GetLoWord(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::GetLoWord(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::GetHiWord

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
		const uint32_t uReturn = Burger::GetHiWord(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::GetHiWord(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test IntToFixed(int32_t)
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
		const int32_t iReturn = Burger::IntToFixed(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::IntToFixed(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test IntToFixedSaturate(int32_t)
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
		const int32_t iReturn = Burger::IntToFixedSaturate(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::IntToFixedSaturate(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntFloor(int32_t)
//

static const IntTest32_t FixedToIntFloorTestTable[] = {
	{FLOATTOFIXED(-4.0) + 1, -4}, {FLOATTOFIXED(-3.5f), -4},
	{FLOATTOFIXED(-3.5f) + 1, -4}, {FLOATTOFIXED(-3.0f), -3},
	{FLOATTOFIXED(-3.0f) + 1, -3}, {FLOATTOFIXED(-2.5f), -3},
	{FLOATTOFIXED(-2.5f) + 1, -3}, {FLOATTOFIXED(-2.0f), -2},
	{FLOATTOFIXED(-2.0f) + 1, -2}, {FLOATTOFIXED(-1.5f), -2},
	{FLOATTOFIXED(-1.5f) + 1, -2}, {FLOATTOFIXED(-1.0f), -1},
	{FLOATTOFIXED(-1.0f) + 1, -1}, {FLOATTOFIXED(-0.5f), -1},
	{FLOATTOFIXED(-0.5f) + 1, -1}, {FLOATTOFIXED(-0.0f), 0},
	{FLOATTOFIXED(0.0f), 0}, {FLOATTOFIXED(0.5f) - 1, 0},
	{FLOATTOFIXED(0.5f), 0}, {FLOATTOFIXED(1.0f) - 1, 0},
	{FLOATTOFIXED(1.0f), 1}, {FLOATTOFIXED(1.5f) - 1, 1},
	{FLOATTOFIXED(1.5f), 1}, {FLOATTOFIXED(2.0f) - 1, 1},
	{FLOATTOFIXED(2.0f), 2}, {FLOATTOFIXED(2.5f) - 1, 2},
	{FLOATTOFIXED(2.5f), 2}, {FLOATTOFIXED(3.0f) - 1, 2},
	{FLOATTOFIXED(3.0f), 3}, {FLOATTOFIXED(3.5f) - 1, 3},
	{FLOATTOFIXED(3.5f), 3}, {FLOATTOFIXED(4.0f) - 1, 3},

	{FLOATTOFIXED(1.1f), 1}, {FLOATTOFIXED(1.95f), 1},
	{FLOATTOFIXED(-1.1f), -2}, {FLOATTOFIXED(-1.95f), -2},
	{FLOATTOFIXED(0.1f), 0}, {FLOATTOFIXED(0.95f), 0},
	{FLOATTOFIXED(-0.1f), -1}, {FLOATTOFIXED(-0.95f), -1}};

static uint_t BURGER_API TestFixedToIntFloor(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntFloorTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntFloorTestTable);
	do {
		const int32_t iReturn = Burger::FixedToIntFloor(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToIntFloor(0x%08X) = %d, expected %d",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntFloor(int32_t)
//

static const IntTest32_t FixedToIntTestTable[] = {{FLOATTOFIXED(-4.0) + 1, -3},
	{FLOATTOFIXED(-3.5f), -3}, {FLOATTOFIXED(-3.5f) + 1, -3},
	{FLOATTOFIXED(-3.0f), -3}, {FLOATTOFIXED(-3.0f) + 1, -2},
	{FLOATTOFIXED(-2.5f), -2}, {FLOATTOFIXED(-2.5f) + 1, -2},
	{FLOATTOFIXED(-2.0f), -2}, {FLOATTOFIXED(-2.0f) + 1, -1},
	{FLOATTOFIXED(-1.5f), -1}, {FLOATTOFIXED(-1.5f) + 1, -1},
	{FLOATTOFIXED(-1.0f), -1}, {FLOATTOFIXED(-1.0f) + 1, -0},
	{FLOATTOFIXED(-0.5f), -0}, {FLOATTOFIXED(-0.5f) + 1, -0},
	{FLOATTOFIXED(-0.0f), 0}, {FLOATTOFIXED(0.0f), 0},
	{FLOATTOFIXED(0.5f) - 1, 0}, {FLOATTOFIXED(0.5f), 0},
	{FLOATTOFIXED(1.0f) - 1, 0}, {FLOATTOFIXED(1.0f), 1},
	{FLOATTOFIXED(1.5f) - 1, 1}, {FLOATTOFIXED(1.5f), 1},
	{FLOATTOFIXED(2.0f) - 1, 1}, {FLOATTOFIXED(2.0f), 2},
	{FLOATTOFIXED(2.5f) - 1, 2}, {FLOATTOFIXED(2.5f), 2},
	{FLOATTOFIXED(3.0f) - 1, 2}, {FLOATTOFIXED(3.0f), 3},
	{FLOATTOFIXED(3.5f) - 1, 3}, {FLOATTOFIXED(3.5f), 3},
	{FLOATTOFIXED(4.0f) - 1, 3},

	{FLOATTOFIXED(1.1f), 1}, {FLOATTOFIXED(1.95f), 1},
	{FLOATTOFIXED(-1.1f), -1}, {FLOATTOFIXED(-1.95f), -1},
	{FLOATTOFIXED(0.1f), 0}, {FLOATTOFIXED(0.95f), 0}, {FLOATTOFIXED(-0.1f), 0},
	{FLOATTOFIXED(-0.95f), 0}};

static uint_t BURGER_API TestFixedToInt(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntTestTable);
	do {
		const int32_t iReturn = Burger::FixedToInt(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToInt(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntCeil(int32_t)
//

static const IntTest32_t FixedToIntCeilTestTable[] = {
	{FLOATTOFIXED(-4.0) + 1, -3}, {FLOATTOFIXED(-3.5f), -3},
	{FLOATTOFIXED(-3.5f) + 1, -3}, {FLOATTOFIXED(-3.0f), -3},
	{FLOATTOFIXED(-3.0f) + 1, -2}, {FLOATTOFIXED(-2.5f), -2},
	{FLOATTOFIXED(-2.5f) + 1, -2}, {FLOATTOFIXED(-2.0f), -2},
	{FLOATTOFIXED(-2.0f) + 1, -1}, {FLOATTOFIXED(-1.5f), -1},
	{FLOATTOFIXED(-1.5f) + 1, -1}, {FLOATTOFIXED(-1.0f), -1},
	{FLOATTOFIXED(-1.0f) + 1, -0}, {FLOATTOFIXED(-0.5f), -0},
	{FLOATTOFIXED(-0.5f) + 1, -0}, {FLOATTOFIXED(-0.0f), 0},
	{FLOATTOFIXED(0.0f), 0}, {FLOATTOFIXED(0.5f) - 1, 1},
	{FLOATTOFIXED(0.5f), 1}, {FLOATTOFIXED(1.0f) - 1, 1},
	{FLOATTOFIXED(1.0f), 1}, {FLOATTOFIXED(1.5f) - 1, 2},
	{FLOATTOFIXED(1.5f), 2}, {FLOATTOFIXED(2.0f) - 1, 2},
	{FLOATTOFIXED(2.0f), 2}, {FLOATTOFIXED(2.5f) - 1, 3},
	{FLOATTOFIXED(2.5f), 3}, {FLOATTOFIXED(3.0f) - 1, 3},
	{FLOATTOFIXED(3.0f), 3}, {FLOATTOFIXED(3.5f) - 1, 4},
	{FLOATTOFIXED(3.5f), 4}, {FLOATTOFIXED(4.0f) - 1, 4},

	{FLOATTOFIXED(1.1f), 2}, {FLOATTOFIXED(1.95f), 2},
	{FLOATTOFIXED(-1.1f), -1}, {FLOATTOFIXED(-1.95f), -1},
	{FLOATTOFIXED(0.1f), 1}, {FLOATTOFIXED(0.95f), 1}, {FLOATTOFIXED(-0.1f), 0},
	{FLOATTOFIXED(-0.95f), 0}};

static uint_t BURGER_API TestFixedToIntCeil(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntCeilTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntCeilTestTable);
	do {
		const int32_t iReturn = Burger::FixedToIntCeil(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FixedToIntCeil(0x%08X) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntNearest(int32_t)
//

static const IntTest32_t FixedToIntNearestTestTable[] = {
	{FLOATTOFIXED(-4.0) + 1, -4}, {FLOATTOFIXED(-3.5f), -4},
	{FLOATTOFIXED(-3.5f) + 1, -3}, {FLOATTOFIXED(-3.0f), -3},
	{FLOATTOFIXED(-3.0f) + 1, -3}, {FLOATTOFIXED(-2.5f), -3},
	{FLOATTOFIXED(-2.5f) + 1, -2}, {FLOATTOFIXED(-2.0f), -2},
	{FLOATTOFIXED(-2.0f) + 1, -2}, {FLOATTOFIXED(-1.5f), -2},
	{FLOATTOFIXED(-1.5f) + 1, -1}, {FLOATTOFIXED(-1.0f), -1},
	{FLOATTOFIXED(-1.0f) + 1, -1}, {FLOATTOFIXED(-0.5f), -1},
	{FLOATTOFIXED(-0.5f) + 1, -0}, {FLOATTOFIXED(-0.0f), 0},
	{FLOATTOFIXED(0.0f), 0}, {FLOATTOFIXED(0.5f) - 1, 0},
	{FLOATTOFIXED(0.5f), 1}, {FLOATTOFIXED(1.0f) - 1, 1},
	{FLOATTOFIXED(1.0f), 1}, {FLOATTOFIXED(1.5f) - 1, 1},
	{FLOATTOFIXED(1.5f), 2}, {FLOATTOFIXED(2.0f) - 1, 2},
	{FLOATTOFIXED(2.0f), 2}, {FLOATTOFIXED(2.5f) - 1, 2},
	{FLOATTOFIXED(2.5f), 3}, {FLOATTOFIXED(3.0f) - 1, 3},
	{FLOATTOFIXED(3.0f), 3}, {FLOATTOFIXED(3.5f) - 1, 3},
	{FLOATTOFIXED(3.5f), 4}, {FLOATTOFIXED(4.0f) - 1, 4},

	{FLOATTOFIXED(1.1f), 1}, {FLOATTOFIXED(1.95f), 2},
	{FLOATTOFIXED(-1.1f), -1}, {FLOATTOFIXED(-1.95f), -2},
	{FLOATTOFIXED(0.1f), 0}, {FLOATTOFIXED(0.95f), 1}, {FLOATTOFIXED(-0.1f), 0},
	{FLOATTOFIXED(-0.95f), -1}};

static uint_t BURGER_API TestFixedToIntNearest(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32_t* pWork = FixedToIntNearestTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(FixedToIntNearestTestTable);
	do {
		const int32_t iReturn = Burger::FixedToIntNearest(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FixedToIntNearest(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntFloor(float)
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
		const int32_t iReturn = Burger::FloatToIntFloor(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntFloor(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToIntFloor(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToIntFloor(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntRoundToZero(float)
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
		const int32_t iReturn = Burger::FloatToIntRoundToZero(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToIntRoundToZero(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToIntRoundToZero(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToIntRoundToZero(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntCeil(float)
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
		const int32_t iReturn = Burger::FloatToIntCeil(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntCeil(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToIntCeil(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToIntCeil(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntRound(float)
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
		const int32_t iReturn = Burger::FloatToIntRound(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntRound(%g) = %d, expected %d", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToIntRound(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToIntRound(int32_t *pOutput,%g) = %d, expected %d",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixedFloor(float)
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
		const int32_t iReturn = Burger::FloatToFixedFloor(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedFloor(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToFixedFloor(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToFixedFloor(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixed(float)
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
		const int32_t iReturn = Burger::FloatToFixed(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixed(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToFixed(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToFixed(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixedCeil(float)
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
		const int32_t iReturn = Burger::FloatToFixedCeil(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedCeil(%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToFixedCeil(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToFixedCeil(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_fInput, iReturnMemory, pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixedNearest(float)
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
		const int32_t iReturn = Burger::FloatToFixedNearest(pWork->m_fInput);
		uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToFixedNearest(%g) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_fInput, iReturn, pWork->m_iOutput);

		int32_t iReturnMemory;
		Burger::FloatToFixedNearest(&iReturnMemory, pWork->m_fInput);
		uTest = iReturnMemory != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::FloatToFixedNearest(int32_t *pOutput,%g) = 0x%08X, expected 0x%08X",
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
		ReportFailure("Burger::absolute((int8_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
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
		ReportFailure("Burger::absolute((int16_t)0x%08X) = 0x%08X, expected 0x%08X",
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
		ReportFailure("Burger::absolute((int32_t)0x%08X) = 0x%08X, expected 0x%08X",
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
			ReportFailure("Burger::absolute((int64_t)0x%s) = 0x%s, expected 0x%s",
				uTest, Text1.c_str(), Text2.c_str(), Text3.c_str());
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
		ReportFailure("Burger::get_sign((int8_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, iReturn, pWork->m_iOutput);
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
		ReportFailure("Burger::get_sign((int16_t)0x%08X) = 0x%08X, expected 0x%08X",
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
		ReportFailure("Burger::get_sign((int32_t)0x%08X) = 0x%08X, expected 0x%08X",
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
			ReportFailure("Burger::get_sign((int64_t)0x%s) = 0x%s, expected 0x%s",
				uTest, Text1.c_str(), Text2.c_str(), Text3.c_str());
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
			"Burger::square_root((uint32_t)0x%08X) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test SqrtFixedToWord32(Fixed32)
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
			"Burger::SqrtFixedToWord32((Fixed32)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput, uReturn, pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test square_root(Fixed32)
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
		const Fixed32 iReturn = Burger::square_root(pWork->m_iInput);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::square_root((Fixed32)0x%08X) = 0x%08X, expected 0x%08X",
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

	uint_t uResult = TestPowerOf2_32();
	uResult |= TestPowerOf2_64();
	uResult |= TestBCDWord();

	uResult |= TestBitReverse_32();
	uResult |= TestBitReverse_64();
	uResult |= TestBitSetCount_32();
	uResult |= TestBitSetCount_64();

	uResult |= TestGetLoWord();
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
