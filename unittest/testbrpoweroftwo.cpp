/***************************************

	Unit tests for the power of two library

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrpoweroftwo.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"
#include "brpoweroftwo.h"
#include "brstructs.h"
#include "common.h"

using namespace Burger;

/***************************************

	Test Burger::power_of_two(uint32_t)

***************************************/

struct WordTest32_t {
	uint32_t m_uInput;  // Source value
	uint32_t m_uOutput; // Expected output
};

static const WordTest32_t PowerOf2_32Table[] = { //
	{0x00000000U, 0x00000000U}, {0x00000001U, 0x00000001U},
	{0x00000002U, 0x00000002U}, {0x00000003U, 0x00000004U},
	{0x00000004U, 0x00000004U}, {0x00000005U, 0x00000008U},
	{0x00002000U, 0x00002000U}, {0x00002001U, 0x00004000U},
	{0x20000001U, 0x40000000U}, {0x40000000U, 0x40000000U},
	{0x40000001U, 0x80000000U}, {0x40004020U, 0x80000000U},
	{0x7FFFFFFFU, 0x80000000U}, {0x80000000U, 0x80000000U},
	{0x80000001U, 0x00000000U}, {0xC0000000U, 0x00000000U},
	{0xE0000001U, 0x00000000U}, {0xFFFFFFFFU, 0x00000000U}};

static uint_t BURGER_API Test_power_of_two_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = PowerOf2_32Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_32Table);
	do {
		const uint32_t uReturn = power_of_two(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::power_of_two(0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::power_of_two(uint64_t)

***************************************/

struct WordTest64_t {
	uint64_t m_uInput;  // Source value
	uint64_t m_uOutput; // Expected output
};

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

static uint_t BURGER_API Test_power_of_two_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest64_t* pWork = PowerOf2_64Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_64Table);
	do {
		const uint64_t uReturn = power_of_two(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		if (uTest) {
			const NumberStringHex Input(pWork->m_uInput);
			const NumberStringHex Return(uReturn);
			const NumberStringHex Output(pWork->m_uOutput);
			ReportFailure("Burger::power_of_two(0x%s) = 0x%s, expected 0x%s",
				uTest, Input.c_str(), Return.c_str(), Output.c_str());
		}
		++pWork;
	} while (--uCount);

	// Should compile without warnings
	// Results don't matter, only test if they compile correctly
	power_of_two(static_cast<uint8_t>(1));
	power_of_two(static_cast<uint16_t>(1));
	power_of_two(static_cast<uintptr_t>(1));
	power_of_two(static_cast<unsigned long>(1));
	power_of_two(static_cast<unsigned>(1));
	power_of_two(static_cast<unsigned long long>(1));
	power_of_two(static_cast<uintptr_t>(1));
	return uFailure;
}

/***************************************

	Test Burger::power_of_two(float)

***************************************/

struct FloatToFloatTest_t {
	uint32_float_t m_fInput;  // Source value
	uint32_float_t m_fOutput; // Expected output
};

static const FloatToFloatTest_t PowerOf2_fTable[] = { //
	{0x00000000U, 0x7FFFFFFFU}, {0x00000001U, 0x00800000U},
	{0x007FFFFFU, 0x00800000U}, {0x00800000U, 0x00800000U},
	{0x00800001U, 0x01000000U}, {0x00FFFFFFU, 0x01000000U},
	{0x3F800000U, 0x3F800000U}, {0x40000000U, 0x40000000U},
	{0x40400000U, 0x40800000U}, {0x407FFFFFU, 0x40800000U},
	{0x40800000U, 0x40800000U}, {0x45CE7000U, 0x46000000U},
	{0x7F7FFFFFU, 0x7F800000U}, {0x7F800000U, 0x7F800000U},
	{0x7F800001U, 0x7F800001U}, {0x7FC00000U, 0x7FC00000U},
	{0x7FFFFFFFU, 0x7FFFFFFFU}, {0x80000000U, 0x7FFFFFFFU},
	{0x80000001U, 0x7FFFFFFFU}, {0xC0000000U, 0x7FFFFFFFU},
	{0xE0000001U, 0x7FFFFFFFU}, {0xFFFFFFFFU, 0x7FFFFFFFU}};

static uint_t BURGER_API Test_power_of_two_float(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const FloatToFloatTest_t* pWork = PowerOf2_fTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_fTable);
	do {
		uint32_float_t Return;
		Return.f = power_of_two(pWork->m_fInput.f);
		uint_t uTest;

		// Handle NaN testing
		if (is_NaN(pWork->m_fOutput.f) && is_NaN(Return.f)) {
			uTest = FALSE;
		} else {
			uTest = Return.w != pWork->m_fOutput.w;
		}
		uFailure |= uTest;
		if (uTest) {
			const NumberStringHex Input(pWork->m_fInput.w);
			const NumberStringHex ReturnX(Return.w);
			const NumberStringHex Output(pWork->m_fOutput.w);
			ReportFailure(
				"Burger::power_of_two((float)0x%s) = 0x%s, expected 0x%s",
				uTest, Input.c_str(), ReturnX.c_str(), Output.c_str());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::power_of_two(double)

***************************************/

struct DoubleToDoubleTest_t {
	uint64_double_t m_dInput;  // Source value
	uint64_double_t m_dOutput; // Expected output
};

static const DoubleToDoubleTest_t PowerOf2_dTable[] = { //
	{0x0000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0x0000000000000001ULL, 0x0010000000000000ULL},
	{0x000FFFFFFFFFFFFFULL, 0x0010000000000000ULL},
	{0x0010000000000000ULL, 0x0010000000000000ULL},
	{0x0010000000000001ULL, 0x0020000000000000ULL},
	{0x001FFFFFFFFFFFFFULL, 0x0020000000000000ULL},
	{0x3FF0000000000000ULL, 0x3FF0000000000000ULL},
	{0x4000000000000000ULL, 0x4000000000000000ULL},
	{0x4008000000000000ULL, 0x4010000000000000ULL},
	{0x400FFFFFFFFFFFFFULL, 0x4010000000000000ULL},
	{0x4010000000000000ULL, 0x4010000000000000ULL},
	{0x40B9CE0000000000ULL, 0x40C0000000000000ULL},
	{0x7FEFFFFFFFFFFFFFULL, 0x7FF0000000000000ULL},
	{0x7FF0000000000000ULL, 0x7FF0000000000000ULL},
	{0x7FF0000000000001ULL, 0x7FF0000000000001ULL},
	{0x7FF8000000000000ULL, 0x7FF8000000000000ULL},
	{0x7FFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL},
	{0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0x8000000000000001ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0xC000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0xE000000000000001ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0xFFFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL}};

static uint_t BURGER_API Test_power_of_two_double(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const DoubleToDoubleTest_t* pWork = PowerOf2_dTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(PowerOf2_dTable);
	do {
		uint64_double_t Return;
		Return.d = power_of_two(pWork->m_dInput.d);
		uint_t uTest;

		// Handle NaN testing
		if (is_NaN(pWork->m_dOutput.d) && is_NaN(Return.d)) {
			uTest = FALSE;
		} else {
			uTest = Return.w != pWork->m_dOutput.w;
		}
		uFailure |= uTest;
		if (uTest) {
			const NumberStringHex Input(pWork->m_dInput.w);
			const NumberStringHex ReturnX(Return.w);
			const NumberStringHex Output(pWork->m_dOutput.w);
			ReportFailure(
				"Burger::power_of_two((double)0x%s) = 0x%s, expected 0x%s",
				uTest, Input.c_str(), ReturnX.c_str(), Output.c_str());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::convert_from_BCD(uint32_t)

***************************************/

static const WordTest32_t BCDWordTable[] = { //
	{0x00000000U, 0U}, {0x00000001U, 1U}, {0x00000002U, 2U}, {0x00000003U, 3U},
	{0x00000004U, 4U}, {0x98765432U, 98765432U}, {0x00002000U, 2000U},
	{0x00002001U, 2001U}, {0x20000001U, 20000001U}, {0x40000000U, 40000000U},
	{0x40000001U, 40000001U}, {0x40004020U, 40004020U},
	{0x12345678U, 12345678U}, {0x99999999U, 99999999U},
	{0x98765432U, 98765432U}, {0x87654321U, 87654321U},
	{0x8000000FU, 80000015U}, {0xFFFFFFFFU, 166666665}};

static uint_t BURGER_API Test_convert_from_BCD(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = BCDWordTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(BCDWordTable);
	do {
		const uint32_t uReturn = convert_from_BCD(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::convert_from_BCD(0x%08X) = %u, expected %u",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::convert_to_BCD(uint32_t)

***************************************/

static const WordTest32_t g_to_BCD[] = { //
	{0x00000000U, 0U}, {0x00000001U, 1U}, {0x00000002U, 2U}, {0x00000003U, 3U},
	{0x00000004U, 4U}, {98765432U, 0x98765432U}, {2000U, 0x00002000U},
	{2001U, 0x00002001U}, {20000001U, 0x20000001U}, {40000000U, 0x40000000U},
	{40000001U, 0x40000001U}, {40004020U, 0x40004020U},
	{12345678U, 0x12345678U}, {99999999U, 0x99999999U},
	{98765432U, 0x98765432U}, {87654321U, 0x87654321U},
	{80000015U, 0x80000015U}, {166666665U, 0x99999999U}};

static uint_t BURGER_API Test_convert_to_BCD(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = g_to_BCD;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_to_BCD);
	do {
		const uint32_t uReturn = convert_to_BCD(pWork->m_uInput);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::convert_to_BCD(%u) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput, uReturn, pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::bit_reverse(uint32_t,uint_t)

***************************************/

static const WordTest32_t BitReverse_32Table[] = { //
	{0x00000000U, 0x00000000U}, {0x00000001U, 0x80000000U},
	{0x00000002U, 0x40000000U}, {0x00000003U, 0xC0000000U},
	{0x00000004U, 0x20000000U}, {0x00000005U, 0xA0000000U},
	{0x00002000U, 0x00040000U}, {0x00002001U, 0x80040000U},
	{0x20000001U, 0x80000004U}, {0x40000000U, 0x00000002U},
	{0x40000001U, 0x80000002U}, {0x40004020U, 0x04020002U},
	{0x7FFFFFFFU, 0xFFFFFFFEU}, {0x80000000U, 0x00000001U},
	{0x80000001U, 0x80000001U}, {0xFFFFFFFFU, 0xFFFFFFFFU},
	{0xAAAAAAAAU, 0x55555555U}, {0xAAAA0000U, 0x00005555U},
	{0x12345678U, 0x1E6A2C48U}};

static uint_t BURGER_API Test_bit_reverse_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uBitCount = 1U;
	do {
		const WordTest32_t* pWork = BitReverse_32Table;
		uintptr_t uCount = BURGER_ARRAYSIZE(BitReverse_32Table);
		do {
			const uint32_t uReturn = bit_reverse(pWork->m_uInput, uBitCount);
			const uint32_t uToTest = pWork->m_uOutput >> (32U - uBitCount);
			const uint_t uTest = uReturn != uToTest;
			uFailure |= uTest;
			ReportFailure(
				"Burger::bit_reverse(0x%08X,%u) = 0x%08X, expected 0x%08X",
				uTest, pWork->m_uInput, uBitCount, uReturn, uToTest);
			++pWork;
		} while (--uCount);
	} while (++uBitCount < (32U + 1U));
	return uFailure;
}

/***************************************

	Test Burger::bit_reverse(uint64_t,uint_t)

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

static uint_t BURGER_API Test_bit_reverse_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uBitCount = 1U;
	do {
		const WordTest64_t* pWork = BitReverse_64Table;
		uintptr_t uCount = BURGER_ARRAYSIZE(BitReverse_64Table);
		do {
			const uint64_t uReturn = bit_reverse(pWork->m_uInput, uBitCount);
			const uint64_t uToTest = pWork->m_uOutput >> (64U - uBitCount);
			const uint_t uTest = uReturn != uToTest;
			uFailure |= uTest;
			if (uTest) {
				const NumberStringHex Input(pWork->m_uInput);
				const NumberStringHex Return(uReturn);
				const NumberStringHex Output(pWork->m_uOutput);
				ReportFailure(
					"Burger::bit_reverse(0x%s,%u) = 0x%s, expected 0x%s", uTest,
					Input.c_str(), uBitCount, Return.c_str(), Output.c_str());
			}
			++pWork;
		} while (--uCount);
	} while (++uBitCount < (64U + 1U));
	return uFailure;
}

/***************************************

	Test Burger::count_set_bits(uint32_t)

***************************************/

static const WordTest32_t BitSetCount_32Table[] = { //
	{0x00000000U, 0}, {0x00000001U, 1}, {0x00000002U, 1}, {0x00000003U, 2},
	{0x00000004U, 1}, {0x00000005U, 2}, {0x00002000U, 1}, {0x00002001U, 2},
	{0x20000001U, 2}, {0x40000000U, 1}, {0x40000001U, 2}, {0x40004020U, 3},
	{0x7FFFFFFFU, 31}, {0x80000000U, 1}, {0x80000001U, 2}, {0x55555555U, 16},
	{0x55550000U, 8}, {0x0000AAAAU, 8}, {0xAAAAAAAAU, 16}, {0xFFFFFFFFU, 32},
	{0x12345678U, 13}};

static uint_t BURGER_API TestBitSetCount_32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest32_t* pWork = BitSetCount_32Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(BitSetCount_32Table);
	do {
		const uint_t uReturn = count_set_bits(pWork->m_uInput);
		const uint_t uToTest = static_cast<uint_t>(pWork->m_uOutput);
		const uint_t uTest = uReturn != uToTest;
		uFailure |= uTest;
		ReportFailure(
			"Burger::count_set_bits(0x%08X) = 0x%08X, expected 0x%08X", uTest,
			pWork->m_uInput, uReturn, uToTest);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::count_set_bits(uint64_t)

***************************************/

static const WordTest64_t BitSetCount_64Table[] = { //
	{0x0000000000000000ULL, 0}, {0x0000000000000001ULL, 1},
	{0x0000000000000002ULL, 1}, {0x0000000000000003ULL, 2},
	{0x0000000000000004ULL, 1}, {0x0000000000000005ULL, 2},
	{0x0000000000002000ULL, 1}, {0x0000000000002001ULL, 2},
	{0x2000000000000001ULL, 2}, {0x4000000000000000ULL, 1},
	{0x4000000000000001ULL, 2}, {0x4000000000004020ULL, 3},
	{0x7FFFFFFFFFFFFFFFULL, 63}, {0x8000000000000000ULL, 1},
	{0x8000000000000001ULL, 2}, {0xFFFFFFFFFFFFFFFFULL, 64},
	{0x5555555555555555ULL, 32}, {0xAAAAAAAAAAAAAAAAULL, 32},
	{0x5555555500000000ULL, 16}, {0x00000000AAAAAAAAULL, 16},
	{0x1234567890ABCDEFULL, 32}};

static uint_t BURGER_API TestBitSetCount_64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const WordTest64_t* pWork = BitSetCount_64Table;
	uintptr_t uCount = BURGER_ARRAYSIZE(BitSetCount_64Table);
	do {
		const uint_t uReturn = count_set_bits(pWork->m_uInput);
		const uint_t uToTest = static_cast<uint_t>(pWork->m_uOutput);
		const uint_t uTest = uReturn != uToTest;
		uFailure |= uTest;
		if (uTest) {
			const NumberStringHex Input(pWork->m_uInput);
			ReportFailure(
				"Burger::count_set_bits(0x%s) = 0x%08X, expected 0x%08X", uTest,
				Input.c_str(), uReturn, uToTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test fixed point math

***************************************/

int BURGER_API TestBrpoweroftwo(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running simple bit tests");
	}

	uint_t uResult = Test_power_of_two_32();
	uResult |= Test_power_of_two_64();
	uResult |= Test_power_of_two_float();
	uResult |= Test_power_of_two_double();
	uResult |= Test_convert_from_BCD();
	uResult |= Test_convert_to_BCD();
	uResult |= Test_bit_reverse_32();
	uResult |= Test_bit_reverse_64();
	uResult |= TestBitSetCount_32();
	uResult |= TestBitSetCount_64();
	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all simple bit tests!");
	}
	return static_cast<int>(uResult);
}
