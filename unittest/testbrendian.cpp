/***************************************

	Unit tests for the Endian manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrendian.h"
#include "brendian.h"
#include "brmemoryfunctions.h"
#include "common.h"

#if defined(BURGER_XBOX360) && defined(NDEBUG)
// #define XBOXBUG
#endif

// These are unions to be able to access
// shorts, longs and long longs in a byte
// manner so I can do endian checking

union Union16_t {
	uint8_t m_bData[2];
	int16_t m_Int16;
	uint16_t m_Word16;
};

union Union32_t {
	uint8_t m_bData[4];
	int32_t m_Int32;
	uint32_t m_Word32;
	float m_Float;
};

union Union64_t {
	uint8_t m_bData[8];
	uint32_t m_Word32[2];
	int64_t m_Int64;
	uint64_t m_Word64;
	double m_Double;
};

// These are hard coded values in a known endian

const int16_t NEi16 = 0x1234;
const uint16_t NEu16 = 0x1234;
const int32_t NEi32 = 0x12345678;
const uint32_t NEu32 = 0x12345678;
const int64_t NEi64 = 0x123456789ABCDEF0LL;
const uint64_t NEu64 = 0x123456789ABCDEF0LL;
const float NEf32 = float(0x923456) / (65536.0f * 256.0f);
const double NEf64 =
	(double(0x789ABCD) / (65536.0 * 65536.0 * 65536.0 * 16.0)) +
	(double(0x1123456) / (65536.0 * 256.0));

const int16_t REi16 = 0x3412;
const uint16_t REu16 = 0x3412;
const int32_t REi32 = 0x78563412;
const uint32_t REu32 = 0x78563412;
const int64_t REi64 = 0xF0DEBC9A78563412LL;
const uint64_t REu64 = 0xF0DEBC9A78563412ULL;
const float REf32 = float(0xB4123F) * (65536.0f * 64.0f);
const double REf64 =
	-((double(0x23F13F) *
		  (65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 *
			  65536.0 * 65536.0 * 65536.0 * 128.0)) +
		(double(0x1B896745) *
			(65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 *
				65536.0 * 65536.0 * 65536.0 * 65536.0 * 65536.0 * 32768.0)));

static const Union16_t g_BE16_1234 = {{0x12, 0x34}};
static const Union32_t g_BE32_1234 = {{0x12, 0x34, 0x56, 0x78}};
static const Union64_t g_BE64_1234 = {
	{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}};
static const Union32_t g_BEF32_1234 = {{0x3F, 0x12, 0x34, 0x56}};
static const Union64_t g_BEF64_1234 = {
	{0x3F, 0xF1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD}};
static const Union16_t g_LE16_1234 = {{0x34, 0x12}};
static const Union32_t g_LE32_1234 = {{0x78, 0x56, 0x34, 0x12}};
static const Union64_t g_LE64_1234 = {
	{0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12}};
static const Union32_t g_LEF32_1234 = {{0x56, 0x34, 0x12, 0x3f}};
static const Union64_t g_LEF64_1234 = {
	{0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0xF1, 0x3F}};

/***************************************

	Primary test!

	Test the endian flags to see if the endian REALLY matches what the compiler
	flags are set to.

	This test MUST pass. Failure of this test renders ALL remaining Endian
	Manager tests invalid.

***************************************/

static uint_t BURGER_API TestEndianFlags(void) BURGER_NOEXCEPT
{
	// Test for Little Endian
#if defined(BURGER_LITTLEENDIAN)

	// If both are set, die immediately, compiling is bogus!
#if defined(BURGER_BIGENDIAN)
#error Both Endian flags are present!

#else
	// Test if the machine REALLY is little endian
	const uint_t uResult = g_LE32_1234.m_Word32;
#endif

#elif defined(BURGER_BIGENDIAN)
	// Test if the machine REALLY is big endian
	const uint_t uResult = g_BE32_1234.m_Word32;
#else

	// Neither flag was set, die immediately, compiling is bogus!
#error Endian flags are incorrect!
#endif

	// Load a binary representation of a value in
	// what should be native endian and test it against to
	// what the machine really has
	const uint_t uFailure = (uResult != 0x12345678);
	ReportFailure(
		"BURGER_ENDIAN = 0x%08X / Wanted (0x12345678)", uFailure, uResult);
	return uFailure;
}

/***************************************

	Tests for the SwapEndian class.

***************************************/

static void BURGER_API TestBurgerSwapEndianTypes(void) BURGER_NOEXCEPT
{
	char VarChar = 1;
	unsigned char VarUChar = 1;
	signed char VarSChar = 1;

	short VarShort = 1;
	unsigned short VarUShort = 1;
	signed short VarSShort = 1;

	int VarInt = 1;
	unsigned int VarUInt = 1;
	signed int VarSInt = 1;

	long VarLong = 1;
	unsigned long VarULong = 1;
	signed long VarSLong = 1;

	long long VarLongLong = 1;
	unsigned long long VarULongLong = 1;
	signed long long VarSLongLong = 1;

	float VarFloat = 1.0f;
	double VarDouble = 1.0;

#if defined(BURGER_HAS_WCHAR_T)
	wchar_t VarWChar = 1;
#endif

	volatile unsigned long Vol1 = 1;
	volatile int Vol2 = 1;
	volatile short Vol3 = 2;

	uint8_t Unaligned[33] = {0};

	Burger::SwapEndian::load('a');
#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::load(L'a');
#endif
	Burger::SwapEndian::load(1);
	Burger::SwapEndian::load(1U);
	Burger::SwapEndian::load(1LL);
	Burger::SwapEndian::load(1ULL);

	Burger::SwapEndian::load(1.0);
	Burger::SwapEndian::load(1.0f);

	Burger::SwapEndian::load(static_cast<char>('a'));
	Burger::SwapEndian::load(static_cast<signed char>('a'));
	Burger::SwapEndian::load(static_cast<unsigned char>('a'));

	Burger::SwapEndian::load(static_cast<short>(123));
	Burger::SwapEndian::load(static_cast<signed short>(123));
	Burger::SwapEndian::load(static_cast<unsigned short>(123));

	Burger::SwapEndian::load(static_cast<int>(123));
	Burger::SwapEndian::load(static_cast<signed int>(123));
	Burger::SwapEndian::load(static_cast<unsigned int>(123));

	Burger::SwapEndian::load(static_cast<long>(123));
	Burger::SwapEndian::load(static_cast<signed long>(123));
	Burger::SwapEndian::load(static_cast<unsigned long>(123));

	Burger::SwapEndian::load(static_cast<long long>(123));
	Burger::SwapEndian::load(static_cast<signed long long>(123));
	Burger::SwapEndian::load(static_cast<unsigned long long>(123));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::load(&VarWChar);
#endif

	Burger::SwapEndian::load(&VarChar);
	Burger::SwapEndian::load(&VarUChar);
	Burger::SwapEndian::load(&VarSChar);

	Burger::SwapEndian::load(&VarShort);
	Burger::SwapEndian::load(&VarUShort);
	Burger::SwapEndian::load(&VarSShort);

	Burger::SwapEndian::load(&VarInt);
	Burger::SwapEndian::load(&VarUInt);
	Burger::SwapEndian::load(&VarSInt);

	Burger::SwapEndian::load(&VarLong);
	Burger::SwapEndian::load(&VarULong);
	Burger::SwapEndian::load(&VarSLong);

	Burger::SwapEndian::load(&VarLongLong);
	Burger::SwapEndian::load(&VarULongLong);
	Burger::SwapEndian::load(&VarSLongLong);

	Burger::SwapEndian::load(&VarFloat);
	Burger::SwapEndian::load(&VarDouble);

	Burger::SwapEndian::load(&Vol1);
	Burger::SwapEndian::load(&Vol2);
	Burger::SwapEndian::load(&Vol3);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::store(&VarWChar, L'a');
#endif

	Burger::SwapEndian::store(&VarChar, static_cast<char>(1));
	Burger::SwapEndian::store(&VarUChar, static_cast<unsigned char>(1));
	Burger::SwapEndian::store(&VarSChar, static_cast<signed char>(1));

	Burger::SwapEndian::store(&VarShort, static_cast<short>(1));
	Burger::SwapEndian::store(&VarUShort, static_cast<unsigned short>(1));
	Burger::SwapEndian::store(&VarSShort, static_cast<signed short>(1));

	Burger::SwapEndian::store(&VarInt, static_cast<int>(1));
	Burger::SwapEndian::store(&VarUInt, static_cast<unsigned int>(1));
	Burger::SwapEndian::store(&VarSInt, static_cast<signed int>(1));

	Burger::SwapEndian::store(&VarLong, static_cast<long>(1));
	Burger::SwapEndian::store(&VarULong, static_cast<unsigned long>(1));
	Burger::SwapEndian::store(&VarSLong, static_cast<signed long>(1));

	Burger::SwapEndian::store(&VarLongLong, static_cast<long long>(1));
	Burger::SwapEndian::store(
		&VarULongLong, static_cast<unsigned long long>(1));
	Burger::SwapEndian::store(&VarSLongLong, static_cast<signed long long>(1));

	Burger::SwapEndian::store(&VarFloat, 1.0f);
	Burger::SwapEndian::store(&VarDouble, 1.0);

	Burger::SwapEndian::store(&Vol1, static_cast<unsigned long>(0xAA));
	Burger::SwapEndian::store(&Vol2, 0x55);
	Burger::SwapEndian::store(&Vol3, static_cast<short>(0x1234));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const wchar_t*>(Unaligned + 1));
#endif

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const char*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const unsigned char*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const signed char*>(Unaligned + 1));

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const short*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const unsigned short*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const signed short*>(Unaligned + 1));

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const int*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const unsigned int*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const signed int*>(Unaligned + 1));

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const long*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const unsigned long*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const signed long*>(Unaligned + 1));

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const long long*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const unsigned long long*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const signed long long*>(Unaligned + 1));

	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const float*>(Unaligned + 1));
	Burger::SwapEndian::load_unaligned(
		reinterpret_cast<const double*>(Unaligned + 1));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<wchar_t*>(Unaligned + 1), L'A');
#endif

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<char*>(Unaligned + 1), static_cast<char>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<unsigned char*>(Unaligned + 1),
		static_cast<unsigned char>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<signed char*>(Unaligned + 1),
		static_cast<signed char>(1));

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<short*>(Unaligned + 1), static_cast<short>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<unsigned short*>(Unaligned + 1),
		static_cast<unsigned short>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<signed short*>(Unaligned + 1),
		static_cast<signed short>(1));

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<int*>(Unaligned + 1), static_cast<int>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<unsigned int*>(Unaligned + 1),
		static_cast<unsigned int>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<signed int*>(Unaligned + 1),
		static_cast<signed int>(1));

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<long*>(Unaligned + 1), static_cast<long>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<unsigned long*>(Unaligned + 1),
		static_cast<unsigned long>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<signed long*>(Unaligned + 1),
		static_cast<signed long>(1));

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<long long*>(Unaligned + 1), static_cast<long long>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<unsigned long long*>(Unaligned + 1),
		static_cast<unsigned long long>(1));
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<signed long long*>(Unaligned + 1),
		static_cast<signed long long>(1));

	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<float*>(Unaligned + 1), 1.0f);
	Burger::SwapEndian::store_unaligned(
		reinterpret_cast<double*>(Unaligned + 1), 1.0);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::fixup(&VarWChar);
#endif

	Burger::SwapEndian::fixup(&VarChar);
	Burger::SwapEndian::fixup(&VarUChar);
	Burger::SwapEndian::fixup(&VarSChar);

	Burger::SwapEndian::fixup(&VarShort);
	Burger::SwapEndian::fixup(&VarUShort);
	Burger::SwapEndian::fixup(&VarSShort);

	Burger::SwapEndian::fixup(&VarInt);
	Burger::SwapEndian::fixup(&VarUInt);
	Burger::SwapEndian::fixup(&VarSInt);

	Burger::SwapEndian::fixup(&VarLong);
	Burger::SwapEndian::fixup(&VarULong);
	Burger::SwapEndian::fixup(&VarSLong);

	Burger::SwapEndian::fixup(&VarLongLong);
	Burger::SwapEndian::fixup(&VarULongLong);
	Burger::SwapEndian::fixup(&VarSLongLong);

	Burger::SwapEndian::fixup(&VarFloat);
	Burger::SwapEndian::fixup(&VarDouble);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<wchar_t*>(Unaligned + 1));
#endif

	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<char*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<unsigned char*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<signed char*>(Unaligned + 1));

	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<short*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<unsigned short*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<signed short*>(Unaligned + 1));

	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<int*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<unsigned int*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<signed int*>(Unaligned + 1));

	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<long*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<unsigned long*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<signed long*>(Unaligned + 1));

	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<long long*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<unsigned long long*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(
		reinterpret_cast<signed long long*>(Unaligned + 1));

	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<float*>(Unaligned + 1));
	Burger::SwapEndian::fixup_unaligned(reinterpret_cast<double*>(Unaligned + 1));
}

/***************************************

	Tests for the SwapEndian class.

***************************************/

//
// Test SwapEndian::load()
//

static uint_t BURGER_API TestBurgerSwapEndianLoadWord16(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::SwapEndian::load(NEu16);
	uint_t uTest = (uResult != REu16);
	uint_t uFailure = uTest;
	ReportFailure(
		"Burger::SwapEndian::load(uint16_t) = 0x%04X / Wanted (0x%04X)", uTest,
		uResult, REu16);

	uResult = _swapendian16(NEu16);
	uTest = (uResult != REu16);
	uFailure |= uTest;
	ReportFailure("_swapendian16(uint16_t) = 0x%04X / Wanted (0x%04X)", uTest,
		uResult, REu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt16(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::SwapEndian::load(NEi16);
	uint_t uFailure = (iResult != REi16);
	ReportFailure(
		"Burger::SwapEndian::load(int16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, REi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord32(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::SwapEndian::load(NEu32);
	uint_t uTest = (uResult != REu32);
	uint_t uFailure = uTest;
	ReportFailure(
		"Burger::SwapEndian::load(uint32_t) = 0x%08X / Wanted (0x%08X)", uTest,
		uResult, REu32);

	uResult = _swapendian32(NEu32);
	uTest = (uResult != REu32);
	uFailure |= uTest;
	ReportFailure("_swapendian32(uint32_t) = 0x%08X / Wanted (0x%08X)", uTest,
		uResult, REu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt32(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::SwapEndian::load(NEi32);
	uint_t uFailure = (iResult != REi32);
	ReportFailure(
		"Burger::SwapEndian::load(int32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, REi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord64(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::SwapEndian::load(NEu64);
	uint_t uTest = (uResult != REu64);
	uint_t uFailure = uTest;
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure(
		"Burger::SwapEndian::load(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uTest, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);

	uResult = _swapendian64(NEu64);
	uTest = (uResult != REu64);
	uFailure |= uTest;
	uMem1.m_Word64 = uResult;
	uMem2.m_Word64 = REu64;
	ReportFailure("_swapendian64(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uTest, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt64(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::SwapEndian::load(NEi64);
	uint_t uFailure = (iResult != REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure(
		"Burger::SwapEndian::load(int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadFloat(void) BURGER_NOEXCEPT
{
	float fResult = Burger::SwapEndian::load(NEf32);
	uint_t uFailure = (fResult != REf32);
	ReportFailure("Burger::SwapEndian::load(float) = %g / Wanted (%g)",
		uFailure, fResult, REf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadDouble(void) BURGER_NOEXCEPT
{
	double fResult = Burger::SwapEndian::load(NEf64);
	uint_t uFailure = (fResult != REf64);
	ReportFailure("Burger::SwapEndian::load(double) = %g / Wanted (%g)",
		uFailure, fResult, REf64);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord16Ptr(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::SwapEndian::load(&NEu16);
	uint_t uFailure = (uResult != REu16);
	ReportFailure(
		"Burger::SwapEndian::load(const uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, REu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt16Ptr(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::SwapEndian::load(&NEi16);
	uint_t uFailure = (iResult != REi16);
	ReportFailure(
		"Burger::SwapEndian::load(const int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, REi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord32Ptr(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::SwapEndian::load(&NEu32);
	uint_t uFailure = (uResult != REu32);
	ReportFailure(
		"Burger::SwapEndian::load(const uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, REu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt32Ptr(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::SwapEndian::load(&NEi32);
	uint_t uFailure = (iResult != REi32);
	ReportFailure(
		"Burger::SwapEndian::load(const int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, REi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord64Ptr(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::SwapEndian::load(&NEu64);
	uint_t uFailure = (uResult != REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure(
		"Burger::SwapEndian::load(const uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt64Ptr(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::SwapEndian::load(&NEi64);
	uint_t uFailure = (iResult != REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure(
		"Burger::SwapEndian::load(const int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadFloatPtr(void) BURGER_NOEXCEPT
{
	float fResult = Burger::SwapEndian::load(&NEf32);
	uint_t uFailure = (fResult != REf32);
	ReportFailure("Burger::SwapEndian::load(const float *) = %g / Wanted (%g)",
		uFailure, fResult, REf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadDoublePtr(void) BURGER_NOEXCEPT
{
	double fResult = Burger::SwapEndian::load(&NEf64);
	uint_t uFailure = (fResult != REf64);
	ReportFailure("Burger::SwapEndian::load(const double*) = %g / Wanted (%g)",
		uFailure, fResult, REf64);
	return uFailure;
}

//
// Test SwapEndian::store()
//

static uint_t BURGER_API TestBurgerSwapEndianStoreWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult;
	Burger::SwapEndian::store(&uResult, NEu16);
	uint_t uFailure = (uResult != REu16);
	ReportFailure(
		"Burger::SwapEndian::store(const uint16_t *,uint16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, REu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreInt16Ptr(void) BURGER_NOEXCEPT
{
	int16_t iResult;
	Burger::SwapEndian::store(&iResult, NEi16);
	uint_t uFailure = (iResult != REi16);
	ReportFailure(
		"Burger::SwapEndian::store(const int16_t *, int16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, REi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult;
	Burger::SwapEndian::store(&uResult, NEu32);
	uint_t uFailure = (uResult != REu32);
	ReportFailure(
		"Burger::SwapEndian::store(const uint32_t *, uint32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, REu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreInt32Ptr(void) BURGER_NOEXCEPT
{
	int32_t iResult;
	Burger::SwapEndian::store(&iResult, NEi32);
	uint_t uFailure = (iResult != REi32);
	ReportFailure(
		"Burger::SwapEndian::store(const int32_t *, int32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, REi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult;
	Burger::SwapEndian::store(&uResult, NEu64);
	uint_t uFailure = (uResult != REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure(
		"Burger::SwapEndian::store(const uint64_t *, uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreInt64Ptr(void) BURGER_NOEXCEPT
{
	int64_t iResult;
	Burger::SwapEndian::store(&iResult, NEi64);
	uint_t uFailure = (iResult != REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure(
		"Burger::SwapEndian::store(const int64_t *, int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreFloatPtr(void) BURGER_NOEXCEPT
{
	float fResult;
	Burger::SwapEndian::store(&fResult, NEf32);
	uint_t uFailure = (fResult != REf32);
	ReportFailure(
		"Burger::SwapEndian::store(const float *, float) = %g / Wanted (%g)",
		uFailure, fResult, REf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult;
	Burger::SwapEndian::store(&fResult, NEf64);
	uint_t uFailure = (fResult != REf64);
	ReportFailure("Burger::SwapEndian::store(const double*) = %g / Wanted (%g)",
		uFailure, fResult, REf64);
	return uFailure;
}

//
// Test SwapEndian::load_unaligned()
//

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)

// There was a bug where the code below would generate bad code on the XBox 360
// compiler
#if defined(XBOXBUG)
		memcpy(pWork, &NEu16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu16)[1];
#endif
		// Load the value
		uint16_t uResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const uint16_t*>(pWork));
		uint_t uFailure = (uResult != REu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, REu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)

// There was a bug where the code below would generate bad code on the XBox 360
// compiler
#if defined(XBOXBUG)
		memcpy(pWork, &NEi16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi16)[1];
#endif
		// Load the value
		int16_t iResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const int16_t*>(pWork));
		uint_t uFailure = (iResult != REi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, REi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu32)[3];
#endif
		// Load the value
		uint32_t uResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const uint32_t*>(pWork));
		uint_t uFailure = (uResult != REu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, REu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi32)[3];
#endif
		// Load the value
		int32_t iResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const int32_t*>(pWork));
		uint_t uFailure = (iResult != REi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, REi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEu64)[7];
#endif
		// Load the value
		uint64_t uResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const uint64_t*>(pWork));
		uint_t uFailure = (uResult != REu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = REu64;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEi64)[7];
#endif
		// Load the value
		int64_t iResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const int64_t*>(pWork));
		uint_t uFailure = (iResult != REi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = REi64;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf32)[3];
#endif
		// Load the value
		float fResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const float*>(pWork));
		uint_t uFailure = (fResult != REf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, REf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEf64)[7];
#endif
		// Load the value
		double dResult = Burger::SwapEndian::load_unaligned(
			reinterpret_cast<const double*>(pWork));
		uint_t uFailure = (dResult != REf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::load_unaligned(const double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, REf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Test SwapEndian::store_unaligned()
//

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Store the value
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<uint16_t*>(pWork), NEu16);

		// Copy the value to read (Keep endian native)
		uint16_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 2);
		uint_t uFailure = (uResult != REu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(const uint16_t *%p,0x%04X) = Wanted (0x%04X)",
			uFailure, pWork, uResult, REu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<int16_t*>(pWork), NEi16);
		// Copy the value to read (Keep endian native)
		int16_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 2);
		uint_t uFailure = (iResult != REi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(int16_t *%p,0x%04X) = Wanted (0x%04X)",
			uFailure, pWork, iResult, REi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<uint32_t*>(pWork), NEu32);
		uint32_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 4);
		// Load the value
		uint_t uFailure = (uResult != REu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(uint32_t *%p,0x%08X) = Wanted (0x%08X)",
			uFailure, pWork, uResult, REu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<int32_t*>(pWork), NEi32);
		int32_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 4);
		// Load the value
		uint_t uFailure = (iResult != REi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(int32_t *%p,0x%08X) = Wanted (0x%08X)",
			uFailure, pWork, iResult, REi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<uint64_t*>(pWork), NEu64);
		uint64_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 8);
		uint_t uFailure = (uResult != REu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = REu64;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(uint64_t *%p,0x%08X%08X) = Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));

		// Load the value
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<int64_t*>(pWork), NEi64);
		int64_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 8);
		uint_t uFailure = (iResult != REi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = REi64;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(int64_t *%p,0x%08X%08X) = Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<float*>(pWork), NEf32);
		float fResult;
		Burger::MemoryCopy(&fResult, pWork, 4);
		uint_t uFailure = (fResult != REf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(float *%p,%g) = Wanted (%g)",
			uFailure, pWork, fResult, REf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianStoreAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::SwapEndian::store_unaligned(
			reinterpret_cast<double*>(pWork), NEf64);
		double dResult;
		Burger::MemoryCopy(&dResult, pWork, 8);
		uint_t uFailure = (dResult != REf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::store_unaligned(double *%p,%g) = Wanted (%g)",
			uFailure, pWork, dResult, REf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::SwapEndian::fixup()
//

static uint_t BURGER_API TestBurgerSwapEndianFixupWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult = NEu16;
	Burger::SwapEndian::fixup(&uResult);
	uint_t uFailure = (uResult != REu16);
	ReportFailure(
		"Burger::SwapEndian::fixup(uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, REu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupInt16Ptr(void) BURGER_NOEXCEPT
{
	int16_t iResult = NEi16;
	Burger::SwapEndian::fixup(&iResult);
	uint_t uFailure = (iResult != REi16);
	ReportFailure(
		"Burger::SwapEndian::fixup(int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, REi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult = NEu32;
	Burger::SwapEndian::fixup(&uResult);
	uint_t uFailure = (uResult != REu32);
	ReportFailure(
		"Burger::SwapEndian::fixup(uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, REu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupInt32Ptr(void) BURGER_NOEXCEPT
{
	int32_t iResult = NEi32;
	Burger::SwapEndian::fixup(&iResult);
	uint_t uFailure = (iResult != REi32);
	ReportFailure(
		"Burger::SwapEndian::fixup(int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, REi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult = NEu64;
	Burger::SwapEndian::fixup(&uResult);
	uint_t uFailure = (uResult != REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure(
		"Burger::SwapEndian::fixup(uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupInt64Ptr(void) BURGER_NOEXCEPT
{
	int64_t iResult = NEi64;
	Burger::SwapEndian::fixup(&iResult);
	uint_t uFailure = (iResult != REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure(
		"Burger::SwapEndian::fixup(int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupFloatPtr(void) BURGER_NOEXCEPT
{
	float fResult = NEf32;
	Burger::SwapEndian::fixup(&fResult);
	uint_t uFailure = (fResult != REf32);
	ReportFailure("Burger::SwapEndian::fixup(float *) = %g / Wanted (%g)",
		uFailure, fResult, REf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult = NEf64;
	Burger::SwapEndian::fixup(&fResult);
	uint_t uFailure = (fResult != REf64);
	ReportFailure("Burger::SwapEndian::fixup(double*) = %g / Wanted (%g)",
		uFailure, fResult, REf64);
	return uFailure;
}

//
// Burger::SwapEndian::fixup_unaligned()
//

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu16)[1];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<uint16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		uint16_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		uFailure |= (uResult != REu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, REu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi16)[1];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<int16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		int16_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		uFailure |= (iResult != REi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, REi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu32)[3];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<uint32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		uint32_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		uFailure |= (uResult != REu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, REu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi32)[3];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<int32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		int32_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		uFailure |= (iResult != REi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, REi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEu64)[7];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<uint64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		uint64_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&uResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&uResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&uResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&uResult)[7] = pWork[7];
		uFailure |= (uResult != REu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = REu64;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEi64)[7];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<int64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		int64_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&iResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&iResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&iResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&iResult)[7] = pWork[7];
		uFailure |= (iResult != REi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = REi64;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf32)[3];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<float*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		float fResult;
		reinterpret_cast<uint8_t*>(&fResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&fResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&fResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&fResult)[3] = pWork[3];
		uFailure |= (fResult != REf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, REf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEf64)[7];
#endif
		Burger::SwapEndian::fixup_unaligned(reinterpret_cast<double*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		double dResult;
		reinterpret_cast<uint8_t*>(&dResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&dResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&dResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&dResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&dResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&dResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&dResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&dResult)[7] = pWork[7];
		uFailure |= (dResult != REf64);
		ReportFailure(
			"Burger::SwapEndian::fixup_unaligned(double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, REf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

/***************************************

	Tests for the NativeEndian class.

***************************************/

static void BURGER_API TestBurgerNativeEndianTypes(void) BURGER_NOEXCEPT
{
	char VarChar = 1;
	unsigned char VarUChar = 1;
	signed char VarSChar = 1;

	short VarShort = 1;
	unsigned short VarUShort = 1;
	signed short VarSShort = 1;

	int VarInt = 1;
	unsigned int VarUInt = 1;
	signed int VarSInt = 1;

	long VarLong = 1;
	unsigned long VarULong = 1;
	signed long VarSLong = 1;

	long long VarLongLong = 1;
	unsigned long long VarULongLong = 1;
	signed long long VarSLongLong = 1;

	float VarFloat = 1.0f;
	double VarDouble = 1.0;

	volatile unsigned long Vol1 = 1;
	volatile int Vol2 = 1;
	volatile short Vol3 = 2;

#if defined(BURGER_HAS_WCHAR_T)
	wchar_t VarWChar = 1;
#endif

	uint8_t Unaligned[33] = {0};

	Burger::NativeEndian::load('a');
#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::load(L'a');
#endif
	Burger::NativeEndian::load(1);
	Burger::NativeEndian::load(1U);
	Burger::NativeEndian::load(1LL);
	Burger::NativeEndian::load(1ULL);
	Burger::NativeEndian::load(1.0);
	Burger::NativeEndian::load(1.0f);

	Burger::NativeEndian::load(static_cast<char>('a'));
	Burger::NativeEndian::load(static_cast<signed char>('a'));
	Burger::NativeEndian::load(static_cast<unsigned char>('a'));

	Burger::NativeEndian::load(static_cast<short>(123));
	Burger::NativeEndian::load(static_cast<signed short>(123));
	Burger::NativeEndian::load(static_cast<unsigned short>(123));

	Burger::NativeEndian::load(static_cast<int>(123));
	Burger::NativeEndian::load(static_cast<signed int>(123));
	Burger::NativeEndian::load(static_cast<unsigned int>(123));

	Burger::NativeEndian::load(static_cast<long>(123));
	Burger::NativeEndian::load(static_cast<signed long>(123));
	Burger::NativeEndian::load(static_cast<unsigned long>(123));

	Burger::NativeEndian::load(static_cast<long long>(123));
	Burger::NativeEndian::load(static_cast<signed long long>(123));
	Burger::NativeEndian::load(static_cast<unsigned long long>(123));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::load(&VarWChar);
#endif

	Burger::NativeEndian::load(&VarChar);
	Burger::NativeEndian::load(&VarUChar);
	Burger::NativeEndian::load(&VarSChar);

	Burger::NativeEndian::load(&VarShort);
	Burger::NativeEndian::load(&VarUShort);
	Burger::NativeEndian::load(&VarSShort);

	Burger::NativeEndian::load(&VarInt);
	Burger::NativeEndian::load(&VarUInt);
	Burger::NativeEndian::load(&VarSInt);

	Burger::NativeEndian::load(&VarLong);
	Burger::NativeEndian::load(&VarULong);
	Burger::NativeEndian::load(&VarSLong);

	Burger::NativeEndian::load(&VarLongLong);
	Burger::NativeEndian::load(&VarULongLong);
	Burger::NativeEndian::load(&VarSLongLong);

	Burger::NativeEndian::load(&VarFloat);
	Burger::NativeEndian::load(&VarDouble);

	Burger::NativeEndian::load(&Vol1);
	Burger::NativeEndian::load(&Vol2);
	Burger::NativeEndian::load(&Vol3);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::store(&VarWChar, L'a');
#endif

	Burger::NativeEndian::store(&VarChar, static_cast<char>(1));
	Burger::NativeEndian::store(&VarUChar, static_cast<unsigned char>(1));
	Burger::NativeEndian::store(&VarSChar, static_cast<signed char>(1));

	Burger::NativeEndian::store(&VarShort, static_cast<short>(1));
	Burger::NativeEndian::store(&VarUShort, static_cast<unsigned short>(1));
	Burger::NativeEndian::store(&VarSShort, static_cast<signed short>(1));

	Burger::NativeEndian::store(&VarInt, static_cast<int>(1));
	Burger::NativeEndian::store(&VarUInt, static_cast<unsigned int>(1));
	Burger::NativeEndian::store(&VarSInt, static_cast<signed int>(1));

	Burger::NativeEndian::store(&VarLong, static_cast<long>(1));
	Burger::NativeEndian::store(&VarULong, static_cast<unsigned long>(1));
	Burger::NativeEndian::store(&VarSLong, static_cast<signed long>(1));

	Burger::NativeEndian::store(&VarLongLong, static_cast<long long>(1));
	Burger::NativeEndian::store(
		&VarULongLong, static_cast<unsigned long long>(1));
	Burger::NativeEndian::store(
		&VarSLongLong, static_cast<signed long long>(1));

	Burger::NativeEndian::store(&VarFloat, 1.0f);
	Burger::NativeEndian::store(&VarDouble, 1.0);

	Burger::NativeEndian::store(&Vol1, static_cast<unsigned long>(0xAA));
	Burger::NativeEndian::store(&Vol2, 0x55);
	Burger::NativeEndian::store(&Vol3, static_cast<short>(0x1234));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const wchar_t*>(Unaligned + 1));
#endif

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const char*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const unsigned char*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const signed char*>(Unaligned + 1));

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const short*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const unsigned short*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const signed short*>(Unaligned + 1));

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const int*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const unsigned int*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const signed int*>(Unaligned + 1));

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const long*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const unsigned long*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const signed long*>(Unaligned + 1));

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const long long*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const unsigned long long*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const signed long long*>(Unaligned + 1));

	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const float*>(Unaligned + 1));
	Burger::NativeEndian::load_unaligned(
		reinterpret_cast<const double*>(Unaligned + 1));

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<wchar_t*>(Unaligned + 1), L'A');
#endif

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<char*>(Unaligned + 1), static_cast<char>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<unsigned char*>(Unaligned + 1),
		static_cast<unsigned char>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<signed char*>(Unaligned + 1),
		static_cast<signed char>(1));

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<short*>(Unaligned + 1), static_cast<short>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<unsigned short*>(Unaligned + 1),
		static_cast<unsigned short>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<signed short*>(Unaligned + 1),
		static_cast<signed short>(1));

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<int*>(Unaligned + 1), static_cast<int>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<unsigned int*>(Unaligned + 1),
		static_cast<unsigned int>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<signed int*>(Unaligned + 1),
		static_cast<signed int>(1));

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<long*>(Unaligned + 1), static_cast<long>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<unsigned long*>(Unaligned + 1),
		static_cast<unsigned long>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<signed long*>(Unaligned + 1),
		static_cast<signed long>(1));

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<long long*>(Unaligned + 1), static_cast<long long>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<unsigned long long*>(Unaligned + 1),
		static_cast<unsigned long long>(1));
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<signed long long*>(Unaligned + 1),
		static_cast<signed long long>(1));

	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<float*>(Unaligned + 1), 1.0f);
	Burger::NativeEndian::store_unaligned(
		reinterpret_cast<double*>(Unaligned + 1), 1.0);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::fixup(&VarWChar);
#endif

	Burger::NativeEndian::fixup(&VarChar);
	Burger::NativeEndian::fixup(&VarUChar);
	Burger::NativeEndian::fixup(&VarSChar);

	Burger::NativeEndian::fixup(&VarShort);
	Burger::NativeEndian::fixup(&VarUShort);
	Burger::NativeEndian::fixup(&VarSShort);

	Burger::NativeEndian::fixup(&VarInt);
	Burger::NativeEndian::fixup(&VarUInt);
	Burger::NativeEndian::fixup(&VarSInt);

	Burger::NativeEndian::fixup(&VarLong);
	Burger::NativeEndian::fixup(&VarULong);
	Burger::NativeEndian::fixup(&VarSLong);

	Burger::NativeEndian::fixup(&VarLongLong);
	Burger::NativeEndian::fixup(&VarULongLong);
	Burger::NativeEndian::fixup(&VarSLongLong);

	Burger::NativeEndian::fixup(&VarFloat);
	Burger::NativeEndian::fixup(&VarDouble);

#if defined(BURGER_HAS_WCHAR_T)
	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<wchar_t*>(Unaligned + 1));
#endif

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<char*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<unsigned char*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<signed char*>(Unaligned + 1));

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<short*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<unsigned short*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<signed short*>(Unaligned + 1));

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<int*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<unsigned int*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<signed int*>(Unaligned + 1));

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<long*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<unsigned long*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<signed long*>(Unaligned + 1));

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<long long*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<unsigned long long*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(
		reinterpret_cast<signed long long*>(Unaligned + 1));

	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<float*>(Unaligned + 1));
	Burger::NativeEndian::fixup_unaligned(reinterpret_cast<double*>(Unaligned + 1));
}

//
// Test NativeEndian::load()
//

static uint_t BURGER_API TestBurgerNativeEndianLoadWord16(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::NativeEndian::load(NEu16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::NativeEndian::load(uint16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt16(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::NativeEndian::load(NEi16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::NativeEndian::load(int16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadWord32(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::NativeEndian::load(NEu32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::NativeEndian::load(uint32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt32(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::NativeEndian::load(NEi32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::NativeEndian::load(int32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadWord64(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::NativeEndian::load(NEu64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::NativeEndian::load(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt64(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::NativeEndian::load(NEi64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::NativeEndian::load(int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadFloat(void) BURGER_NOEXCEPT
{
	float fResult = Burger::NativeEndian::load(NEf32);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::NativeEndian::load(float) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadDouble(void) BURGER_NOEXCEPT
{
	double fResult = Burger::NativeEndian::load(NEf64);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::NativeEndian::load(double) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::NativeEndian::load(&NEu16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::NativeEndian::load(const uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt16Ptr(
	void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::NativeEndian::load(&NEi16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::NativeEndian::load(const int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::NativeEndian::load(&NEu32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::NativeEndian::load(const uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt32Ptr(
	void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::NativeEndian::load(&NEi32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::NativeEndian::load(const int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::NativeEndian::load(&NEu64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::NativeEndian::load(const uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadInt64Ptr(
	void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::NativeEndian::load(&NEi64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::NativeEndian::load(const int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadFloatPtr(
	void) BURGER_NOEXCEPT
{
	float fResult = Burger::NativeEndian::load(&NEf32);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure(
		"Burger::NativeEndian::load(const float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult = Burger::NativeEndian::load(&NEf64);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure(
		"Burger::NativeEndian::load(const double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Test NativeEndian::load_unaligned()
//

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu16)[1];
#endif
		// Load the value
		uint16_t uResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const uint16_t*>(pWork));
		uint_t uFailure = (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi16)[1];
#endif
		// Load the value
		int16_t iResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const int16_t*>(pWork));
		uint_t uFailure = (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu32)[3];
#endif
		// Load the value
		uint32_t uResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const uint32_t*>(pWork));
		uint_t uFailure = (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi32)[3];
#endif
		// Load the value
		int32_t iResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const int32_t*>(pWork));
		uint_t uFailure = (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEu64)[7];
#endif
		// Load the value
		uint64_t uResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const uint64_t*>(pWork));
		uint_t uFailure = (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEi64)[7];
#endif
		// Load the value
		int64_t iResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const int64_t*>(pWork));
		uint_t uFailure = (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf32)[3];
#endif
		// Load the value
		float fResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const float*>(pWork));
		uint_t uFailure = (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianLoadAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEf64)[7];
#endif
		// Load the value
		double dResult = Burger::NativeEndian::load_unaligned(
			reinterpret_cast<const double*>(pWork));
		uint_t uFailure = (dResult != NEf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::load_unaligned(const double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Test NativeEndian::store_unaligned()
//

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<uint16_t*>(pWork), NEu16);
		uint16_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 2);
		uint_t uFailure = (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(uint16_t *%p,0x%04X) = Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<int16_t*>(pWork), NEi16);
		int16_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 2);
		uint_t uFailure = (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(int16_t *%p,0x%04X) = Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<uint32_t*>(pWork), NEu32);
		uint32_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 4);
		uint_t uFailure = (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(uint32_t *%p,0x%08X) = Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<int32_t*>(pWork), NEi32);
		int32_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 4);
		uint_t uFailure = (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(int32_t *%p,0x%08X) = Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<uint64_t*>(pWork), NEu64);
		uint64_t uResult;
		Burger::MemoryCopy(&uResult, pWork, 8);
		uint_t uFailure = (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(uint64_t *%p,0x%08X%08X) = Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<int64_t*>(pWork), NEi64);
		int64_t iResult;
		Burger::MemoryCopy(&iResult, pWork, 8);
		uint_t uFailure = (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(int64_t *%p,0x%08X%08X) = Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<float*>(pWork), NEf32);
		float fResult;
		Burger::MemoryCopy(&fResult, pWork, 4);
		uint_t uFailure = (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(float *%p,%g) = Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianStoreAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));

		// Load the value
		Burger::NativeEndian::store_unaligned(
			reinterpret_cast<double*>(pWork), NEf64);
		double dResult;
		Burger::MemoryCopy(&dResult, pWork, 8);
		uint_t uFailure = (dResult != NEf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::store_unaligned(double *%p,%g) = Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::NativeEndian::fixup()
//

static uint_t BURGER_API TestBurgerNativeEndianFixupWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult = NEu16;
	Burger::NativeEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::NativeEndian::fixup(uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupInt16Ptr(
	void) BURGER_NOEXCEPT
{
	int16_t iResult = NEi16;
	Burger::NativeEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::NativeEndian::fixup(int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult = NEu32;
	Burger::NativeEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::NativeEndian::fixup(uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupInt32Ptr(
	void) BURGER_NOEXCEPT
{
	int32_t iResult = NEi32;
	Burger::NativeEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::NativeEndian::fixup(int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult = NEu64;
	Burger::NativeEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::NativeEndian::fixup(uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupInt64Ptr(
	void) BURGER_NOEXCEPT
{
	int64_t iResult = NEi64;
	Burger::NativeEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::NativeEndian::fixup(int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupFloatPtr(
	void) BURGER_NOEXCEPT
{
	float fResult = NEf32;
	Burger::NativeEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::NativeEndian::fixup(float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult = NEf64;
	Burger::NativeEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::NativeEndian::fixup(double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Burger::NativeEndian::fixup_unaligned()
//

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu16)[1];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<uint16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		uint16_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		uFailure |= (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi16, 2);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi16)[1];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<int16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		int16_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		uFailure |= (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu32)[3];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<uint32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		uint32_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		uFailure |= (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi32)[3];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<int32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		int32_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		uFailure |= (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEu64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEu64)[7];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<uint64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		uint64_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&uResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&uResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&uResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&uResult)[7] = pWork[7];
		uFailure |= (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEi64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEi64)[7];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<int64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		int64_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&iResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&iResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&iResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&iResult)[7] = pWork[7];
		uFailure |= (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf32, 4);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf32)[3];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<float*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		float fResult;
		reinterpret_cast<uint8_t*>(&fResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&fResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&fResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&fResult)[3] = pWork[3];
		uFailure |= (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork, &NEf64, 8);
#else
		pWork[0] = reinterpret_cast<const uint8_t*>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&NEf64)[7];
#endif
		Burger::NativeEndian::fixup_unaligned(reinterpret_cast<double*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		double dResult;
		reinterpret_cast<uint8_t*>(&dResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&dResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&dResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&dResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&dResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&dResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&dResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&dResult)[7] = pWork[7];
		uFailure |= (dResult != NEf64);
		ReportFailure(
			"Burger::NativeEndian::fixup_unaligned(double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

/***************************************

	Tests for the BigEndian class.

***************************************/

//
// Test BigEndian::load()
//

static uint_t BURGER_API TestBurgerBigEndianLoadWord16(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::BigEndian::load(g_BE16_1234.m_Word16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::BigEndian::load(uint16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt16(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::BigEndian::load(g_BE16_1234.m_Int16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure("Burger::BigEndian::load(int16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord32(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::BigEndian::load(g_BE32_1234.m_Word32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::BigEndian::load(uint32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt32(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::BigEndian::load(g_BE32_1234.m_Int32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure("Burger::BigEndian::load(int32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord64(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::BigEndian::load(g_BE64_1234.m_Word64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::BigEndian::load(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt64(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::BigEndian::load(g_BE64_1234.m_Int64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::BigEndian::load(int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadFloat(void) BURGER_NOEXCEPT
{
	float fResult = Burger::BigEndian::load(g_BEF32_1234.m_Float);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::BigEndian::load(float) = %g / Wanted (%g)", uFailure,
		fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadDouble(void) BURGER_NOEXCEPT
{
	double fResult = Burger::BigEndian::load(g_BEF64_1234.m_Double);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::BigEndian::load(double) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord16Ptr(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::BigEndian::load(&g_BE16_1234.m_Word16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::BigEndian::load(const uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt16Ptr(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::BigEndian::load(&g_BE16_1234.m_Int16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::BigEndian::load(const int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord32Ptr(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::BigEndian::load(&g_BE32_1234.m_Word32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::BigEndian::load(const uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt32Ptr(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::BigEndian::load(&g_BE32_1234.m_Int32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::BigEndian::load(const int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord64Ptr(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::BigEndian::load(&g_BE64_1234.m_Word64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::BigEndian::load(const uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt64Ptr(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::BigEndian::load(&g_BE64_1234.m_Int64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::BigEndian::load(const int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadFloatPtr(void) BURGER_NOEXCEPT
{
	float fResult = Burger::BigEndian::load(&g_BEF32_1234.m_Float);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::BigEndian::load(const float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadDoublePtr(void) BURGER_NOEXCEPT
{
	double fResult = Burger::BigEndian::load(&g_BEF64_1234.m_Double);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::BigEndian::load(const double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Test BigEndian::load_unaligned()
//

static uint_t BURGER_API TestBurgerBigEndianLoadAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Word16)[1];
		// Load the value
		uint16_t uResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const uint16_t*>(pWork));
		uint_t uFailure = (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Int16)[1];
		// Load the value
		int16_t iResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const int16_t*>(pWork));
		uint_t uFailure = (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[3];
		// Load the value
		uint32_t uResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const uint32_t*>(pWork));
		uint_t uFailure = (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[3];
		// Load the value
		int32_t iResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const int32_t*>(pWork));
		uint_t uFailure = (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[7];
		// Load the value
		uint64_t uResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const uint64_t*>(pWork));
		uint_t uFailure = (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[7];
		// Load the value
		int64_t iResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const int64_t*>(pWork));
		uint_t uFailure = (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[3];
		// Load the value
		float fResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const float*>(pWork));
		uint_t uFailure = (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianLoadAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[7];
		// Load the value
		double dResult = Burger::BigEndian::load_unaligned(
			reinterpret_cast<const double*>(pWork));
		uint_t uFailure = (dResult != NEf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::load_unaligned(const double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::BigEndian::fixup()
//

static uint_t BURGER_API TestBurgerBigEndianFixupWord16Ptr(void) BURGER_NOEXCEPT
{
	uint16_t uResult = g_BE16_1234.m_Word16;
	Burger::BigEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::BigEndian::fixup(uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupInt16Ptr(void) BURGER_NOEXCEPT
{
	int16_t iResult = g_BE16_1234.m_Int16;
	Burger::BigEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::BigEndian::fixup(int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupWord32Ptr(void) BURGER_NOEXCEPT
{
	uint32_t uResult = g_BE32_1234.m_Word32;
	Burger::BigEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::BigEndian::fixup(uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupInt32Ptr(void) BURGER_NOEXCEPT
{
	int32_t iResult = g_BE32_1234.m_Int32;
	Burger::BigEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::BigEndian::fixup(int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupWord64Ptr(void) BURGER_NOEXCEPT
{
	uint64_t uResult = g_BE64_1234.m_Word64;
	Burger::BigEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::BigEndian::fixup(uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupInt64Ptr(void) BURGER_NOEXCEPT
{
	int64_t iResult = g_BE64_1234.m_Int64;
	Burger::BigEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::BigEndian::fixup(int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupFloatPtr(void) BURGER_NOEXCEPT
{
	float fResult = g_BEF32_1234.m_Float;
	Burger::BigEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::BigEndian::fixup(float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianFixupDoublePtr(void) BURGER_NOEXCEPT
{
	double fResult = g_BEF64_1234.m_Double;
	Burger::BigEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::BigEndian::fixup(double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Burger::BigEndian::fixup_unaligned()
//

static uint_t BURGER_API TestBurgerBigEndianFixupAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Word16)[1];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<uint16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		uint16_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		uFailure |= (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE16_1234.m_Int16)[1];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<int16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		int16_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		uFailure |= (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Word32)[3];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<uint32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		uint32_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		uFailure |= (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE32_1234.m_Int32)[3];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<int32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		int32_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		uFailure |= (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Word64)[7];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<uint64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		uint64_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&uResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&uResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&uResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&uResult)[7] = pWork[7];
		uFailure |= (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BE64_1234.m_Int64)[7];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<int64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		int64_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&iResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&iResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&iResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&iResult)[7] = pWork[7];
		uFailure |= (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BEF32_1234.m_Float)[3];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<float*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		float fResult;
		reinterpret_cast<uint8_t*>(&fResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&fResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&fResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&fResult)[3] = pWork[3];
		uFailure |= (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerBigEndianFixupAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_BEF64_1234.m_Double)[7];
		Burger::BigEndian::fixup_unaligned(reinterpret_cast<double*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		double dResult;
		reinterpret_cast<uint8_t*>(&dResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&dResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&dResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&dResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&dResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&dResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&dResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&dResult)[7] = pWork[7];
		uFailure |= (dResult != NEf64);
		ReportFailure(
			"Burger::BigEndian::fixup_unaligned(double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

/***************************************

	Tests for the LittleEndian class.

***************************************/

//
// Test LittleEndian::load()
//

static uint_t BURGER_API TestBurgerLittleEndianLoadWord16(void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::LittleEndian::load(g_LE16_1234.m_Word16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::LittleEndian::load(uint16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt16(void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::LittleEndian::load(g_LE16_1234.m_Int16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::LittleEndian::load(int16_t) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadWord32(void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::LittleEndian::load(g_LE32_1234.m_Word32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::LittleEndian::load(uint32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt32(void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::LittleEndian::load(g_LE32_1234.m_Int32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::LittleEndian::load(int32_t) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadWord64(void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::LittleEndian::load(g_LE64_1234.m_Word64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::LittleEndian::load(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt64(void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::LittleEndian::load(g_LE64_1234.m_Int64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::LittleEndian::load(int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadFloat(void) BURGER_NOEXCEPT
{
	float fResult = Burger::LittleEndian::load(g_LEF32_1234.m_Float);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::LittleEndian::load(float) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadDouble(void) BURGER_NOEXCEPT
{
	double fResult = Burger::LittleEndian::load(g_LEF64_1234.m_Double);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::LittleEndian::load(double) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult = Burger::LittleEndian::load(&g_LE16_1234.m_Word16);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::LittleEndian::load(const uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt16Ptr(
	void) BURGER_NOEXCEPT
{
	int16_t iResult = Burger::LittleEndian::load(&g_LE16_1234.m_Int16);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::LittleEndian::load(const int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult = Burger::LittleEndian::load(&g_LE32_1234.m_Word32);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::LittleEndian::load(const uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt32Ptr(
	void) BURGER_NOEXCEPT
{
	int32_t iResult = Burger::LittleEndian::load(&g_LE32_1234.m_Int32);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::LittleEndian::load(const int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult = Burger::LittleEndian::load(&g_LE64_1234.m_Word64);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::LittleEndian::load(const uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadInt64Ptr(
	void) BURGER_NOEXCEPT
{
	int64_t iResult = Burger::LittleEndian::load(&g_LE64_1234.m_Int64);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::LittleEndian::load(const int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadFloatPtr(
	void) BURGER_NOEXCEPT
{
	float fResult = Burger::LittleEndian::load(&g_LEF32_1234.m_Float);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure(
		"Burger::LittleEndian::load(const float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult = Burger::LittleEndian::load(&g_LEF64_1234.m_Double);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure(
		"Burger::LittleEndian::load(const double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Test LittleEndian::load_unaligned()
//

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Word16)[1];
		// Load the value
		uint16_t uResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const uint16_t*>(pWork));
		uint_t uFailure = (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Int16)[1];
		// Load the value
		int16_t iResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const int16_t*>(pWork));
		uint_t uFailure = (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[3];
		// Load the value
		uint32_t uResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const uint32_t*>(pWork));
		uint_t uFailure = (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[3];
		// Load the value
		int32_t iResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const int32_t*>(pWork));
		uint_t uFailure = (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[7];
		// Load the value
		uint64_t uResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const uint64_t*>(pWork));
		uint_t uFailure = (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[7];
		// Load the value
		int64_t iResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const int64_t*>(pWork));
		uint_t uFailure = (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[3];
		// Load the value
		float fResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const float*>(pWork));
		uint_t uFailure = (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianLoadAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[7];
		// Load the value
		double dResult = Burger::LittleEndian::load_unaligned(
			reinterpret_cast<const double*>(pWork));
		uint_t uFailure = (dResult != NEf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::load_unaligned(const double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::LittleEndian::fixup()
//

static uint_t BURGER_API TestBurgerLittleEndianFixupWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint16_t uResult = g_LE16_1234.m_Word16;
	Burger::LittleEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu16);
	ReportFailure(
		"Burger::LittleEndian::fixup(uint16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, uResult, NEu16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupInt16Ptr(
	void) BURGER_NOEXCEPT
{
	int16_t iResult = g_LE16_1234.m_Int16;
	Burger::LittleEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi16);
	ReportFailure(
		"Burger::LittleEndian::fixup(int16_t *) = 0x%04X / Wanted (0x%04X)",
		uFailure, iResult, NEi16);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint32_t uResult = g_LE32_1234.m_Word32;
	Burger::LittleEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu32);
	ReportFailure(
		"Burger::LittleEndian::fixup(uint32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, uResult, NEu32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupInt32Ptr(
	void) BURGER_NOEXCEPT
{
	int32_t iResult = g_LE32_1234.m_Int32;
	Burger::LittleEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi32);
	ReportFailure(
		"Burger::LittleEndian::fixup(int32_t *) = 0x%08X / Wanted (0x%08X)",
		uFailure, iResult, NEi32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint64_t uResult = g_LE64_1234.m_Word64;
	Burger::LittleEndian::fixup(&uResult);
	uint_t uFailure = (uResult != NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure(
		"Burger::LittleEndian::fixup(uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupInt64Ptr(
	void) BURGER_NOEXCEPT
{
	int64_t iResult = g_LE64_1234.m_Int64;
	Burger::LittleEndian::fixup(&iResult);
	uint_t uFailure = (iResult != NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure(
		"Burger::LittleEndian::fixup(int64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
		uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
		uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
		uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupFloatPtr(
	void) BURGER_NOEXCEPT
{
	float fResult = g_LEF32_1234.m_Float;
	Burger::LittleEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf32);
	ReportFailure("Burger::LittleEndian::fixup(float *) = %g / Wanted (%g)",
		uFailure, fResult, NEf32);
	return uFailure;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupDoublePtr(
	void) BURGER_NOEXCEPT
{
	double fResult = g_LEF64_1234.m_Double;
	Burger::LittleEndian::fixup(&fResult);
	uint_t uFailure = (fResult != NEf64);
	ReportFailure("Burger::LittleEndian::fixup(double*) = %g / Wanted (%g)",
		uFailure, fResult, NEf64);
	return uFailure;
}

//
// Burger::LittleEndian::fixup_unaligned()
//

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyWord16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Word16)[1];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<uint16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		uint16_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		uFailure |= (uResult != NEu16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(uint16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, uResult, NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyInt16Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 4;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE16_1234.m_Int16)[1];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<int16_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 2);
		// Load the value
		int16_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		uFailure |= (iResult != NEi16);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(int16_t *%p) = 0x%04X / Wanted (0x%04X)",
			uFailure, pWork, iResult, NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyWord32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Word32)[3];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<uint32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		uint32_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		uFailure |= (uResult != NEu32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, uResult, NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyInt32Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE32_1234.m_Int32)[3];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<int32_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		int32_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		uFailure |= (iResult != NEi32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(int32_t *%p) = 0x%08X / Wanted (0x%08X)",
			uFailure, pWork, iResult, NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyWord64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Word64)[7];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<uint64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		uint64_t uResult;
		reinterpret_cast<uint8_t*>(&uResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&uResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&uResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&uResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&uResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&uResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&uResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&uResult)[7] = pWork[7];
		uFailure |= (uResult != NEu64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(uint64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyInt64Ptr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LE64_1234.m_Int64)[7];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<int64_t*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		int64_t iResult;
		reinterpret_cast<uint8_t*>(&iResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&iResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&iResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&iResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&iResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&iResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&iResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&iResult)[7] = pWork[7];
		uFailure |= (iResult != NEi64);
		uFailureTotal |= uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(int64_t *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",
			uFailure, pWork, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
			uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
			uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyFloatPtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[16];
	uint_t i = 8;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LEF32_1234.m_Float)[3];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<float*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 4);
		// Load the value
		float fResult;
		reinterpret_cast<uint8_t*>(&fResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&fResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&fResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&fResult)[3] = pWork[3];
		uFailure |= (fResult != NEf32);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(float *%p) = %g / Wanted (%g)",
			uFailure, pWork, fResult, NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerLittleEndianFixupAnyDoublePtr(
	void) BURGER_NOEXCEPT
{
	uint8_t Buffer[32];
	uint_t i = 16;
	uint8_t* pWork = Buffer;
	uint_t uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer, sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const uint8_t*>(&g_LEF64_1234.m_Double)[7];
		Burger::LittleEndian::fixup_unaligned(reinterpret_cast<double*>(pWork));
		uint_t uFailure = VerifyBuffer(Buffer, sizeof(Buffer), pWork, 8);
		// Load the value
		double dResult;
		reinterpret_cast<uint8_t*>(&dResult)[0] = pWork[0];
		reinterpret_cast<uint8_t*>(&dResult)[1] = pWork[1];
		reinterpret_cast<uint8_t*>(&dResult)[2] = pWork[2];
		reinterpret_cast<uint8_t*>(&dResult)[3] = pWork[3];
		reinterpret_cast<uint8_t*>(&dResult)[4] = pWork[4];
		reinterpret_cast<uint8_t*>(&dResult)[5] = pWork[5];
		reinterpret_cast<uint8_t*>(&dResult)[6] = pWork[6];
		reinterpret_cast<uint8_t*>(&dResult)[7] = pWork[7];
		uFailure |= (dResult != NEf64);
		uFailureTotal |= uFailure;
		ReportFailure(
			"Burger::LittleEndian::fixup_unaligned(double *%p) = %g / Wanted (%g)",
			uFailure, pWork, dResult, NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrendian(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Endian tests");
	}

	// Test compiler switches
	uint_t uTotal = TestEndianFlags();

	TestBurgerSwapEndianTypes();

	// SwapEndian::load()
	uTotal |= TestBurgerSwapEndianLoadWord16();
	uTotal |= TestBurgerSwapEndianLoadInt16();
	uTotal |= TestBurgerSwapEndianLoadWord32();
	uTotal |= TestBurgerSwapEndianLoadInt32();
	uTotal |= TestBurgerSwapEndianLoadWord64();
	uTotal |= TestBurgerSwapEndianLoadInt64();
	uTotal |= TestBurgerSwapEndianLoadFloat();
	uTotal |= TestBurgerSwapEndianLoadDouble();

	// SwapEndian::LoadPtr()
	uTotal |= TestBurgerSwapEndianLoadWord16Ptr();
	uTotal |= TestBurgerSwapEndianLoadInt16Ptr();
	uTotal |= TestBurgerSwapEndianLoadWord32Ptr();
	uTotal |= TestBurgerSwapEndianLoadInt32Ptr();
	uTotal |= TestBurgerSwapEndianLoadWord64Ptr();
	uTotal |= TestBurgerSwapEndianLoadInt64Ptr();
	uTotal |= TestBurgerSwapEndianLoadFloatPtr();
	uTotal |= TestBurgerSwapEndianLoadDoublePtr();

	// SwapEndian::store()
	uTotal |= TestBurgerSwapEndianStoreWord16Ptr();
	uTotal |= TestBurgerSwapEndianStoreInt16Ptr();
	uTotal |= TestBurgerSwapEndianStoreWord32Ptr();
	uTotal |= TestBurgerSwapEndianStoreInt32Ptr();
	uTotal |= TestBurgerSwapEndianStoreWord64Ptr();
	uTotal |= TestBurgerSwapEndianStoreInt64Ptr();
	uTotal |= TestBurgerSwapEndianStoreFloatPtr();
	uTotal |= TestBurgerSwapEndianStoreDoublePtr();

	// SwapEndian::load_unaligned()
	uTotal |= TestBurgerSwapEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerSwapEndianLoadAnyDoublePtr();

	// SwapEndian::store_unaligned()
	uTotal |= TestBurgerSwapEndianStoreAnyWord16Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyInt16Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyWord32Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyInt32Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyWord64Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyInt64Ptr();
	uTotal |= TestBurgerSwapEndianStoreAnyFloatPtr();
	uTotal |= TestBurgerSwapEndianStoreAnyDoublePtr();

	// SwapEndian::fixup()
	uTotal |= TestBurgerSwapEndianFixupWord16Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt16Ptr();
	uTotal |= TestBurgerSwapEndianFixupWord32Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt32Ptr();
	uTotal |= TestBurgerSwapEndianFixupWord64Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt64Ptr();
	uTotal |= TestBurgerSwapEndianFixupFloatPtr();
	uTotal |= TestBurgerSwapEndianFixupDoublePtr();

	// SwapEndian::fixup_unaligned()
	uTotal |= TestBurgerSwapEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerSwapEndianFixupAnyDoublePtr();

	TestBurgerNativeEndianTypes();

	// NativeEndian::load()
	uTotal |= TestBurgerNativeEndianLoadWord16();
	uTotal |= TestBurgerNativeEndianLoadInt16();
	uTotal |= TestBurgerNativeEndianLoadWord32();
	uTotal |= TestBurgerNativeEndianLoadInt32();
	uTotal |= TestBurgerNativeEndianLoadWord64();
	uTotal |= TestBurgerNativeEndianLoadInt64();
	uTotal |= TestBurgerNativeEndianLoadFloat();
	uTotal |= TestBurgerNativeEndianLoadDouble();

	// NativeEndian::LoadPtr()
	uTotal |= TestBurgerNativeEndianLoadWord16Ptr();
	uTotal |= TestBurgerNativeEndianLoadInt16Ptr();
	uTotal |= TestBurgerNativeEndianLoadWord32Ptr();
	uTotal |= TestBurgerNativeEndianLoadInt32Ptr();
	uTotal |= TestBurgerNativeEndianLoadWord64Ptr();
	uTotal |= TestBurgerNativeEndianLoadInt64Ptr();
	uTotal |= TestBurgerNativeEndianLoadFloatPtr();
	uTotal |= TestBurgerNativeEndianLoadDoublePtr();

	// NativeEndian::load_unaligned()
	uTotal |= TestBurgerNativeEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerNativeEndianLoadAnyDoublePtr();

	// NativeEndian::store_unaligned()
	uTotal |= TestBurgerNativeEndianStoreAnyWord16Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyInt16Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyWord32Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyInt32Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyWord64Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyInt64Ptr();
	uTotal |= TestBurgerNativeEndianStoreAnyFloatPtr();
	uTotal |= TestBurgerNativeEndianStoreAnyDoublePtr();

	// NativeEndian::fixup()
	uTotal |= TestBurgerNativeEndianFixupWord16Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt16Ptr();
	uTotal |= TestBurgerNativeEndianFixupWord32Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt32Ptr();
	uTotal |= TestBurgerNativeEndianFixupWord64Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt64Ptr();
	uTotal |= TestBurgerNativeEndianFixupFloatPtr();
	uTotal |= TestBurgerNativeEndianFixupDoublePtr();

	// NativeEndian::fixup_unaligned()
	uTotal |= TestBurgerNativeEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerNativeEndianFixupAnyDoublePtr();

	// BigEndian::load()
	uTotal |= TestBurgerBigEndianLoadWord16();
	uTotal |= TestBurgerBigEndianLoadInt16();
	uTotal |= TestBurgerBigEndianLoadWord32();
	uTotal |= TestBurgerBigEndianLoadInt32();
	uTotal |= TestBurgerBigEndianLoadWord64();
	uTotal |= TestBurgerBigEndianLoadInt64();
	uTotal |= TestBurgerBigEndianLoadFloat();
	uTotal |= TestBurgerBigEndianLoadDouble();

	// BigEndian::LoadPtr()
	uTotal |= TestBurgerBigEndianLoadWord16Ptr();
	uTotal |= TestBurgerBigEndianLoadInt16Ptr();
	uTotal |= TestBurgerBigEndianLoadWord32Ptr();
	uTotal |= TestBurgerBigEndianLoadInt32Ptr();
	uTotal |= TestBurgerBigEndianLoadWord64Ptr();
	uTotal |= TestBurgerBigEndianLoadInt64Ptr();
	uTotal |= TestBurgerBigEndianLoadFloatPtr();
	uTotal |= TestBurgerBigEndianLoadDoublePtr();

	// BigEndian::load_unaligned()
	uTotal |= TestBurgerBigEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerBigEndianLoadAnyDoublePtr();

	// BigEndian::fixup()
	uTotal |= TestBurgerBigEndianFixupWord16Ptr();
	uTotal |= TestBurgerBigEndianFixupInt16Ptr();
	uTotal |= TestBurgerBigEndianFixupWord32Ptr();
	uTotal |= TestBurgerBigEndianFixupInt32Ptr();
	uTotal |= TestBurgerBigEndianFixupWord64Ptr();
	uTotal |= TestBurgerBigEndianFixupInt64Ptr();
	uTotal |= TestBurgerBigEndianFixupFloatPtr();
	uTotal |= TestBurgerBigEndianFixupDoublePtr();

	// BigEndian::fixup_unaligned()
	uTotal |= TestBurgerBigEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerBigEndianFixupAnyDoublePtr();

	// LittleEndian::load()
	uTotal |= TestBurgerLittleEndianLoadWord16();
	uTotal |= TestBurgerLittleEndianLoadInt16();
	uTotal |= TestBurgerLittleEndianLoadWord32();
	uTotal |= TestBurgerLittleEndianLoadInt32();
	uTotal |= TestBurgerLittleEndianLoadWord64();
	uTotal |= TestBurgerLittleEndianLoadInt64();
	uTotal |= TestBurgerLittleEndianLoadFloat();
	uTotal |= TestBurgerLittleEndianLoadDouble();

	// LittleEndian::LoadPtr()
	uTotal |= TestBurgerLittleEndianLoadWord16Ptr();
	uTotal |= TestBurgerLittleEndianLoadInt16Ptr();
	uTotal |= TestBurgerLittleEndianLoadWord32Ptr();
	uTotal |= TestBurgerLittleEndianLoadInt32Ptr();
	uTotal |= TestBurgerLittleEndianLoadWord64Ptr();
	uTotal |= TestBurgerLittleEndianLoadInt64Ptr();
	uTotal |= TestBurgerLittleEndianLoadFloatPtr();
	uTotal |= TestBurgerLittleEndianLoadDoublePtr();

	// LittleEndian::load_unaligned()
	uTotal |= TestBurgerLittleEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerLittleEndianLoadAnyDoublePtr();

	// LittleEndian::fixup()
	uTotal |= TestBurgerLittleEndianFixupWord16Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt16Ptr();
	uTotal |= TestBurgerLittleEndianFixupWord32Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt32Ptr();
	uTotal |= TestBurgerLittleEndianFixupWord64Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt64Ptr();
	uTotal |= TestBurgerLittleEndianFixupFloatPtr();
	uTotal |= TestBurgerLittleEndianFixupDoublePtr();

	// LittleEndian::fixup_unaligned()
	uTotal |= TestBurgerLittleEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerLittleEndianFixupAnyDoublePtr();

	if (!uTotal && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Endian tests!");
	}
	return static_cast<int>(uTotal);
}
