/***************************************

    Unit tests for the Endian manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
//#define XBOXBUG
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

static uint_t BURGER_API TestEndianFlags(void)
{
    // Test for Little Endian
#if defined(BURGER_LITTLEENDIAN)

    // If both are set, die immediately, compiling is bogus!
#if defined(BURGER_BIGENDIAN)
#error Both Endian flags are present!

#else
    // Test if the machine REALLY is little endian
    uint_t uResult = g_LE32_1234.m_Word32;
#endif

#elif defined(BURGER_BIGENDIAN)
    // Test if the machine REALLY is big endian
    uint_t uResult = g_BE32_1234.m_Word32;
#else

    // Neither flag was set, die immediately, compiling is bogus!
#error Endian flags are incorrect!
#endif

    // Load a binary representation of a value in
    // what should be native endian and test it against to
    // what the machine really has
    uint_t uFailure = (uResult != 0x12345678);
    ReportFailure(
        "BURGER_ENDIAN = 0x%08X / Wanted (0x12345678)", uFailure, uResult);
    return uFailure;
}

/***************************************

    Tests for the SwapEndian class.

***************************************/

//
// Test SwapEndian::Load()
//

static uint_t BURGER_API TestBurgerSwapEndianLoadWord16(void)
{
    uint16_t uResult = Burger::SwapEndian::Load(NEu16);
    uint_t uFailure = (uResult != REu16);
    ReportFailure(
        "Burger::SwapEndian::Load(uint16_t) = 0x%04X / Wanted (0x%04X)",
        uFailure, uResult, REu16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadInt16(void)
{
    int16_t iResult = Burger::SwapEndian::Load(NEi16);
    uint_t uFailure = (iResult != REi16);
    ReportFailure(
        "Burger::SwapEndian::Load(int16_t) = 0x%04X / Wanted (0x%04X)",
        uFailure, iResult, REi16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerSwapEndianLoadWord32(void)
{
    uint32_t uResult = Burger::SwapEndian::Load(NEu32);
    uint_t uFailure = (uResult != REu32);
    ReportFailure(
        "Burger::SwapEndian::Load(uint32_t) = 0x%08X / Wanted (0x%08X)",
        uFailure, uResult, REu32);
    return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadInt32(void)
{
	Int32 iResult = Burger::SwapEndian::Load(NEi32);
	Word uFailure = (iResult!=REi32);
	ReportFailure("Burger::SwapEndian::Load(Int32) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,REi32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadWord64(void)
{
	Word64 uResult = Burger::SwapEndian::Load(NEu64);
	Word uFailure = (uResult!=REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure("Burger::SwapEndian::Load(Word64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadInt64(void)
{
	Int64 iResult = Burger::SwapEndian::Load(NEi64);
	Word uFailure = (iResult!=REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure("Burger::SwapEndian::Load(Int64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadFloat(void)
{
	float fResult = Burger::SwapEndian::Load(NEf32);
	Word uFailure = (fResult!=REf32);
	ReportFailure("Burger::SwapEndian::Load(float) = %g / Wanted (%g)",uFailure,fResult,REf32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadDouble(void)
{
	double fResult = Burger::SwapEndian::Load(NEf64);
	Word uFailure = (fResult!=REf64);
	ReportFailure("Burger::SwapEndian::Load(double) = %g / Wanted (%g)",uFailure,fResult,REf64);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadWord16Ptr(void)
{
	Word16 uResult = Burger::SwapEndian::Load(&NEu16);
	Word uFailure = (uResult!=REu16);
	ReportFailure("Burger::SwapEndian::Load(const Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,REu16);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadInt16Ptr(void)
{
	Int16 iResult = Burger::SwapEndian::Load(&NEi16);
	Word uFailure = (iResult!=REi16);
	ReportFailure("Burger::SwapEndian::Load(const Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,REi16);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadWord32Ptr(void)
{
	Word32 uResult = Burger::SwapEndian::Load(&NEu32);
	Word uFailure = (uResult!=REu32);
	ReportFailure("Burger::SwapEndian::Load(const Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,REu32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadInt32Ptr(void)
{
	Int32 iResult = Burger::SwapEndian::Load(&NEi32);
	Word uFailure = (iResult!=REi32);
	ReportFailure("Burger::SwapEndian::Load(const Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,REi32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadWord64Ptr(void)
{
	Word64 uResult = Burger::SwapEndian::Load(&NEu64);
	Word uFailure = (uResult!=REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure("Burger::SwapEndian::Load(const Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadInt64Ptr(void)
{
	Int64 iResult = Burger::SwapEndian::Load(&NEi64);
	Word uFailure = (iResult!=REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure("Burger::SwapEndian::Load(const Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadFloatPtr(void)
{
	float fResult = Burger::SwapEndian::Load(&NEf32);
	Word uFailure = (fResult!=REf32);
	ReportFailure("Burger::SwapEndian::Load(const float *) = %g / Wanted (%g)",uFailure,fResult,REf32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianLoadDoublePtr(void)
{
	double fResult = Burger::SwapEndian::Load(&NEf64);
	Word uFailure = (fResult!=REf64);
	ReportFailure("Burger::SwapEndian::Load(const double*) = %g / Wanted (%g)",uFailure,fResult,REf64);
	return uFailure;
}

//
// Test SwapEndian::LoadAny()
//

static Word BURGER_API TestBurgerSwapEndianLoadAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu16)[1];
#endif
		// Load the value
		Word16 uResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
		Word uFailure = (uResult!=REu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,REu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi16)[1];
#endif
		// Load the value
		Int16 iResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Int16 *>(pWork));
		Word uFailure = (iResult!=REi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,REi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu32)[3];
#endif
		// Load the value
		Word32 uResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Word32 *>(pWork));
		Word uFailure = (uResult!=REu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,REu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi32)[3];
#endif
		// Load the value
		Int32 iResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Int32 *>(pWork));
		Word uFailure = (iResult!=REi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,REi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEu64)[7];
#endif
		// Load the value
		Word64 uResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
		Word uFailure = (uResult!=REu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = REu64;
		ReportFailure("Burger::SwapEndian::LoadAny(const Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEi64)[7];
#endif
		// Load the value
		Int64 iResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const Int64 *>(pWork));
		Word uFailure = (iResult!=REi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = REi64;
		ReportFailure("Burger::SwapEndian::LoadAny(const Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf32)[3];
#endif
		// Load the value
		float fResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const float *>(pWork));
		Word uFailure = (fResult!=REf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,REf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianLoadAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEf64)[7];
#endif
		// Load the value
		double dResult = Burger::SwapEndian::LoadAny(reinterpret_cast<const double *>(pWork));
		Word uFailure = (dResult!=REf64);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::LoadAny(const double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,REf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::SwapEndian::Fixup()
//

static Word BURGER_API TestBurgerSwapEndianFixupWord16Ptr(void)
{
	Word16 uResult = NEu16;
	Burger::SwapEndian::Fixup(&uResult);
	Word uFailure = (uResult!=REu16);
	ReportFailure("Burger::SwapEndian::Fixup(Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,REu16);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupInt16Ptr(void)
{
	Int16 iResult = NEi16;
	Burger::SwapEndian::Fixup(&iResult);
	Word uFailure = (iResult!=REi16);
	ReportFailure("Burger::SwapEndian::Fixup(Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,REi16);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupWord32Ptr(void)
{
	Word32 uResult = NEu32;
	Burger::SwapEndian::Fixup(&uResult);
	Word uFailure = (uResult!=REu32);
	ReportFailure("Burger::SwapEndian::Fixup(Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,REu32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupInt32Ptr(void)
{
	Int32 iResult = NEi32;
	Burger::SwapEndian::Fixup(&iResult);
	Word uFailure = (iResult!=REi32);
	ReportFailure("Burger::SwapEndian::Fixup(Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,REi32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupWord64Ptr(void)
{
	Word64 uResult = NEu64;
	Burger::SwapEndian::Fixup(&uResult);
	Word uFailure = (uResult!=REu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = REu64;
	ReportFailure("Burger::SwapEndian::Fixup(Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupInt64Ptr(void)
{
	Int64 iResult = NEi64;
	Burger::SwapEndian::Fixup(&iResult);
	Word uFailure = (iResult!=REi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = REi64;
	ReportFailure("Burger::SwapEndian::Fixup(Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupFloatPtr(void)
{
	float fResult = NEf32;
	Burger::SwapEndian::Fixup(&fResult);
	Word uFailure = (fResult!=REf32);
	ReportFailure("Burger::SwapEndian::Fixup(float *) = %g / Wanted (%g)",uFailure,fResult,REf32);
	return uFailure;
}

static Word BURGER_API TestBurgerSwapEndianFixupDoublePtr(void)
{
	double fResult = NEf64;
	Burger::SwapEndian::Fixup(&fResult);
	Word uFailure = (fResult!=REf64);
	ReportFailure("Burger::SwapEndian::Fixup(double*) = %g / Wanted (%g)",uFailure,fResult,REf64);
	return uFailure;
}

//
// Burger::SwapEndian::FixupAny()
//

static Word BURGER_API TestBurgerSwapEndianFixupAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu16)[1];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Word16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Word16 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		uFailure |= (uResult!=REu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::FixupAny(Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,REu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi16)[1];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Int16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Int16 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		uFailure |= (iResult!=REi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::FixupAny(Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,REi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu32)[3];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Word32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Word32 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		uFailure |= (uResult!=REu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::FixupAny(Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,REu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi32)[3];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Int32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Int32 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		uFailure |= (iResult!=REi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::FixupAny(Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,REi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEu64)[7];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Word64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Word64 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&uResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&uResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&uResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&uResult)[7] = pWork[7];
		uFailure |= (uResult!=REu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = REu64;
		ReportFailure("Burger::SwapEndian::FixupAny(Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEi64)[7];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<Int64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Int64 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&iResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&iResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&iResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&iResult)[7] = pWork[7];
		uFailure |= (iResult!=REi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = REi64;
		ReportFailure("Burger::SwapEndian::FixupAny(Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf32)[3];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<float *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		float fResult;
		reinterpret_cast<Word8 *>(&fResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&fResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&fResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&fResult)[3] = pWork[3];
		uFailure |= (fResult!=REf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::SwapEndian::FixupAny(float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,REf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerSwapEndianFixupAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEf64)[7];
#endif
		Burger::SwapEndian::FixupAny(reinterpret_cast<double *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		double dResult;
		reinterpret_cast<Word8 *>(&dResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&dResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&dResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&dResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&dResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&dResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&dResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&dResult)[7] = pWork[7];
		uFailure |= (dResult!=REf64);
		ReportFailure("Burger::SwapEndian::FixupAny(double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,REf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

/***************************************

	Tests for the NativeEndian class.

***************************************/

//
// Test NativeEndian::Load()
//

static Word BURGER_API TestBurgerNativeEndianLoadWord16(void)
{
	Word16 uResult = Burger::NativeEndian::Load(NEu16);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::NativeEndian::Load(Word16) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt16(void)
{
	Int16 iResult = Burger::NativeEndian::Load(NEi16);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::NativeEndian::Load(Int16) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadWord32(void)
{
	Word32 uResult = Burger::NativeEndian::Load(NEu32);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::NativeEndian::Load(Word32) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt32(void)
{
	Int32 iResult = Burger::NativeEndian::Load(NEi32);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::NativeEndian::Load(Int32) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadWord64(void)
{
	Word64 uResult = Burger::NativeEndian::Load(NEu64);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::NativeEndian::Load(Word64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt64(void)
{
	Int64 iResult = Burger::NativeEndian::Load(NEi64);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::NativeEndian::Load(Int64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadFloat(void)
{
	float fResult = Burger::NativeEndian::Load(NEf32);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::NativeEndian::Load(float) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadDouble(void)
{
	double fResult = Burger::NativeEndian::Load(NEf64);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::NativeEndian::Load(double) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadWord16Ptr(void)
{
	Word16 uResult = Burger::NativeEndian::Load(&NEu16);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::NativeEndian::Load(const Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt16Ptr(void)
{
	Int16 iResult = Burger::NativeEndian::Load(&NEi16);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::NativeEndian::Load(const Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadWord32Ptr(void)
{
	Word32 uResult = Burger::NativeEndian::Load(&NEu32);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::NativeEndian::Load(const Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt32Ptr(void)
{
	Int32 iResult = Burger::NativeEndian::Load(&NEi32);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::NativeEndian::Load(const Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadWord64Ptr(void)
{
	Word64 uResult = Burger::NativeEndian::Load(&NEu64);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::NativeEndian::Load(const Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadInt64Ptr(void)
{
	Int64 iResult = Burger::NativeEndian::Load(&NEi64);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::NativeEndian::Load(const Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadFloatPtr(void)
{
	float fResult = Burger::NativeEndian::Load(&NEf32);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::NativeEndian::Load(const float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianLoadDoublePtr(void)
{
	double fResult = Burger::NativeEndian::Load(&NEf64);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::NativeEndian::Load(const double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Test NativeEndian::LoadAny()
//

static Word BURGER_API TestBurgerNativeEndianLoadAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu16)[1];
#endif
		// Load the value
		Word16 uResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
		Word uFailure = (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::LoadAny(const Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi16)[1];
#endif
		// Load the value
		Int16 iResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const Int16 *>(pWork));
		Word uFailure = (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::LoadAny(const Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static uint_t BURGER_API TestBurgerSwapEndianFixupAnyWord32Ptr(void)
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
        Burger::SwapEndian::FixupAny(reinterpret_cast<uint32_t*>(pWork));
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
            "Burger::SwapEndian::FixupAny(uint32_t *%p) = 0x%08X / Wanted (0x%08X)",
            uFailure, pWork, uResult, REu32);
        ++pWork;
    } while (--i);
    return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi32)[3];
#endif
		// Load the value
		Int32 iResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const Int32 *>(pWork));
		Word uFailure = (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::LoadAny(const Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEu64)[7];
#endif
		// Load the value
		Word64 uResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
		Word uFailure = (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::NativeEndian::LoadAny(const Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEi64)[7];
#endif
		// Load the value
		Int64 iResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const Int64 *>(pWork));
		Word uFailure = (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::NativeEndian::LoadAny(const Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf32)[3];
#endif
		// Load the value
		float fResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const float *>(pWork));
		Word uFailure = (fResult!=NEf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::LoadAny(const float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianLoadAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEf64)[7];
#endif
		// Load the value
		double dResult = Burger::NativeEndian::LoadAny(reinterpret_cast<const double *>(pWork));
		Word uFailure = (dResult!=NEf64);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::LoadAny(const double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::NativeEndian::Fixup()
//

static Word BURGER_API TestBurgerNativeEndianFixupWord16Ptr(void)
{
	Word16 uResult = NEu16;
	Burger::NativeEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::NativeEndian::Fixup(Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupInt16Ptr(void)
{
	Int16 iResult = NEi16;
	Burger::NativeEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::NativeEndian::Fixup(Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupWord32Ptr(void)
{
	Word32 uResult = NEu32;
	Burger::NativeEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::NativeEndian::Fixup(Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupInt32Ptr(void)
{
	Int32 iResult = NEi32;
	Burger::NativeEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::NativeEndian::Fixup(Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupWord64Ptr(void)
{
	Word64 uResult = NEu64;
	Burger::NativeEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::NativeEndian::Fixup(Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupInt64Ptr(void)
{
	Int64 iResult = NEi64;
	Burger::NativeEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::NativeEndian::Fixup(Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupFloatPtr(void)
{
	float fResult = NEf32;
	Burger::NativeEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::NativeEndian::Fixup(float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerNativeEndianFixupDoublePtr(void)
{
	double fResult = NEf64;
	Burger::NativeEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::NativeEndian::Fixup(double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Burger::NativeEndian::FixupAny()
//

static Word BURGER_API TestBurgerNativeEndianFixupAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu16)[1];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Word16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Word16 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		uFailure |= (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::FixupAny(Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi16,2);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi16)[1];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Int16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Int16 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		uFailure |= (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::FixupAny(Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu32)[3];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Word32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Word32 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		uFailure |= (uResult!=NEu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::FixupAny(Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi32)[3];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Int32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Int32 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		uFailure |= (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::FixupAny(Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEu64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEu64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEu64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEu64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEu64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEu64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEu64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEu64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEu64)[7];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Word64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Word64 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&uResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&uResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&uResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&uResult)[7] = pWork[7];
		uFailure |= (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::NativeEndian::FixupAny(Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEi64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEi64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEi64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEi64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEi64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEi64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEi64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEi64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEi64)[7];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<Int64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Int64 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&iResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&iResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&iResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&iResult)[7] = pWork[7];
		uFailure |= (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::NativeEndian::FixupAny(Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf32,4);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf32)[3];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<float *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		float fResult;
		reinterpret_cast<Word8 *>(&fResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&fResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&fResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&fResult)[3] = pWork[3];
		uFailure |= (fResult!=NEf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::NativeEndian::FixupAny(float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerNativeEndianFixupAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
#if defined(XBOXBUG)
		memcpy(pWork,&NEf64,8);
#else
		pWork[0] = reinterpret_cast<const Word8 *>(&NEf64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&NEf64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&NEf64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&NEf64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&NEf64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&NEf64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&NEf64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&NEf64)[7];
#endif
		Burger::NativeEndian::FixupAny(reinterpret_cast<double *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		double dResult;
		reinterpret_cast<Word8 *>(&dResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&dResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&dResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&dResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&dResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&dResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&dResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&dResult)[7] = pWork[7];
		uFailure |= (dResult!=NEf64);
		ReportFailure("Burger::NativeEndian::FixupAny(double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

/***************************************

	Tests for the BigEndian class.

***************************************/

//
// Test BigEndian::Load()
//

static Word BURGER_API TestBurgerBigEndianLoadWord16(void)
{
	Word16 uResult = Burger::BigEndian::Load(g_BE16_1234.m_Word16);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::BigEndian::Load(Word16) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt16(void)
{
	Int16 iResult = Burger::BigEndian::Load(g_BE16_1234.m_Int16);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::BigEndian::Load(Int16) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadWord32(void)
{
	Word32 uResult = Burger::BigEndian::Load(g_BE32_1234.m_Word32);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::BigEndian::Load(Word32) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt32(void)
{
	Int32 iResult = Burger::BigEndian::Load(g_BE32_1234.m_Int32);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::BigEndian::Load(Int32) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadWord64(void)
{
	Word64 uResult = Burger::BigEndian::Load(g_BE64_1234.m_Word64);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::BigEndian::Load(Word64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt64(void)
{
	Int64 iResult = Burger::BigEndian::Load(g_BE64_1234.m_Int64);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::BigEndian::Load(Int64) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadFloat(void)
{
	float fResult = Burger::BigEndian::Load(g_BEF32_1234.m_Float);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::BigEndian::Load(float) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadDouble(void)
{
	double fResult = Burger::BigEndian::Load(g_BEF64_1234.m_Double);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::BigEndian::Load(double) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadWord16Ptr(void)
{
	Word16 uResult = Burger::BigEndian::Load(&g_BE16_1234.m_Word16);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::BigEndian::Load(const Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt16Ptr(void)
{
	Int16 iResult = Burger::BigEndian::Load(&g_BE16_1234.m_Int16);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::BigEndian::Load(const Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadWord32Ptr(void)
{
	Word32 uResult = Burger::BigEndian::Load(&g_BE32_1234.m_Word32);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::BigEndian::Load(const Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt32Ptr(void)
{
	Int32 iResult = Burger::BigEndian::Load(&g_BE32_1234.m_Int32);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::BigEndian::Load(const Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadWord64Ptr(void)
{
	Word64 uResult = Burger::BigEndian::Load(&g_BE64_1234.m_Word64);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::BigEndian::Load(const Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadInt64Ptr(void)
{
	Int64 iResult = Burger::BigEndian::Load(&g_BE64_1234.m_Int64);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::BigEndian::Load(const Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadFloatPtr(void)
{
	float fResult = Burger::BigEndian::Load(&g_BEF32_1234.m_Float);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::BigEndian::Load(const float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianLoadDoublePtr(void)
{
	double fResult = Burger::BigEndian::Load(&g_BEF64_1234.m_Double);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::BigEndian::Load(const double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Test BigEndian::LoadAny()
//

static Word BURGER_API TestBurgerBigEndianLoadAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Word16)[1];
		// Load the value
		Word16 uResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
		Word uFailure = (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Int16)[1];
		// Load the value
		Int16 iResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Int16 *>(pWork));
		Word uFailure = (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[3];
		// Load the value
		Word32 uResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Word32 *>(pWork));
		Word uFailure = (uResult!=NEu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[3];
		// Load the value
		Int32 iResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Int32 *>(pWork));
		Word uFailure = (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[7];
		// Load the value
		Word64 uResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
		Word uFailure = (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::BigEndian::LoadAny(const Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[7];
		// Load the value
		Int64 iResult = Burger::BigEndian::LoadAny(reinterpret_cast<const Int64 *>(pWork));
		Word uFailure = (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::BigEndian::LoadAny(const Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BEF32_1234.m_Float)[3];
		// Load the value
		float fResult = Burger::BigEndian::LoadAny(reinterpret_cast<const float *>(pWork));
		Word uFailure = (fResult!=NEf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianLoadAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_BEF64_1234.m_Double)[7];
		// Load the value
		double dResult = Burger::BigEndian::LoadAny(reinterpret_cast<const double *>(pWork));
		Word uFailure = (dResult!=NEf64);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::LoadAny(const double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::BigEndian::Fixup()
//

static Word BURGER_API TestBurgerBigEndianFixupWord16Ptr(void)
{
	Word16 uResult = g_BE16_1234.m_Word16;
	Burger::BigEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::BigEndian::Fixup(Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupInt16Ptr(void)
{
	Int16 iResult = g_BE16_1234.m_Int16;
	Burger::BigEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::BigEndian::Fixup(Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupWord32Ptr(void)
{
	Word32 uResult = g_BE32_1234.m_Word32;
	Burger::BigEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::BigEndian::Fixup(Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupInt32Ptr(void)
{
	Int32 iResult = g_BE32_1234.m_Int32;
	Burger::BigEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::BigEndian::Fixup(Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupWord64Ptr(void)
{
	Word64 uResult = g_BE64_1234.m_Word64;
	Burger::BigEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::BigEndian::Fixup(Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupInt64Ptr(void)
{
	Int64 iResult = g_BE64_1234.m_Int64;
	Burger::BigEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::BigEndian::Fixup(Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupFloatPtr(void)
{
	float fResult = g_BEF32_1234.m_Float;
	Burger::BigEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::BigEndian::Fixup(float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerBigEndianFixupDoublePtr(void)
{
	double fResult = g_BEF64_1234.m_Double;
	Burger::BigEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::BigEndian::Fixup(double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Burger::BigEndian::FixupAny()
//

static Word BURGER_API TestBurgerBigEndianFixupAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Word16)[1];
		Burger::BigEndian::FixupAny(reinterpret_cast<Word16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Word16 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		uFailure |= (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::FixupAny(Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE16_1234.m_Int16)[1];
		Burger::BigEndian::FixupAny(reinterpret_cast<Int16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Int16 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		uFailure |= (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::FixupAny(Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Word32)[3];
		Burger::BigEndian::FixupAny(reinterpret_cast<Word32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Word32 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		uFailure |= (uResult!=NEu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::FixupAny(Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE32_1234.m_Int32)[3];
		Burger::BigEndian::FixupAny(reinterpret_cast<Int32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Int32 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		uFailure |= (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::BigEndian::FixupAny(Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Word64)[7];
		Burger::BigEndian::FixupAny(reinterpret_cast<Word64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Word64 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&uResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&uResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&uResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&uResult)[7] = pWork[7];
		uFailure |= (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::BigEndian::FixupAny(Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_BE64_1234.m_Int64)[7];
		Burger::BigEndian::FixupAny(reinterpret_cast<Int64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Int64 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&iResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&iResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&iResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&iResult)[7] = pWork[7];
		uFailure |= (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::BigEndian::FixupAny(Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerBigEndianFixupAnyFloatPtr(void)
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
        Burger::NativeEndian::FixupAny(reinterpret_cast<float*>(pWork));
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
            "Burger::NativeEndian::FixupAny(float *%p) = %g / Wanted (%g)",
            uFailure, pWork, fResult, NEf32);
        ++pWork;
    } while (--i);
    return uFailureTotal;
}

static uint_t BURGER_API TestBurgerNativeEndianFixupAnyDoublePtr(void)
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
        Burger::NativeEndian::FixupAny(reinterpret_cast<double*>(pWork));
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
            "Burger::NativeEndian::FixupAny(double *%p) = %g / Wanted (%g)",
            uFailure, pWork, dResult, NEf64);
        ++pWork;
    } while (--i);
    return uFailureTotal;
}

/***************************************

    Tests for the BigEndian class.

***************************************/

//
// Test BigEndian::Load()
//

static uint_t BURGER_API TestBurgerBigEndianLoadWord16(void)
{
    uint16_t uResult = Burger::BigEndian::Load(g_BE16_1234.m_Word16);
    uint_t uFailure = (uResult != NEu16);
    ReportFailure(
        "Burger::BigEndian::Load(uint16_t) = 0x%04X / Wanted (0x%04X)",
        uFailure, uResult, NEu16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt16(void)
{
    int16_t iResult = Burger::BigEndian::Load(g_BE16_1234.m_Int16);
    uint_t uFailure = (iResult != NEi16);
    ReportFailure("Burger::BigEndian::Load(int16_t) = 0x%04X / Wanted (0x%04X)",
        uFailure, iResult, NEi16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord32(void)
{
    uint32_t uResult = Burger::BigEndian::Load(g_BE32_1234.m_Word32);
    uint_t uFailure = (uResult != NEu32);
    ReportFailure(
        "Burger::BigEndian::Load(uint32_t) = 0x%08X / Wanted (0x%08X)",
        uFailure, uResult, NEu32);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt32(void)
{
    int32_t iResult = Burger::BigEndian::Load(g_BE32_1234.m_Int32);
    uint_t uFailure = (iResult != NEi32);
    ReportFailure("Burger::BigEndian::Load(int32_t) = 0x%08X / Wanted (0x%08X)",
        uFailure, iResult, NEi32);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord64(void)
{
    uint64_t uResult = Burger::BigEndian::Load(g_BE64_1234.m_Word64);
    uint_t uFailure = (uResult != NEu64);
    Union64_t uMem1;
    uMem1.m_Word64 = uResult;
    Union64_t uMem2;
    uMem2.m_Word64 = NEu64;
    ReportFailure(
        "Burger::BigEndian::Load(uint64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
        uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
        uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt64(void)
{
    int64_t iResult = Burger::BigEndian::Load(g_BE64_1234.m_Int64);
    uint_t uFailure = (iResult != NEi64);
    Union64_t uMem1;
    uMem1.m_Int64 = iResult;
    Union64_t uMem2;
    uMem2.m_Int64 = NEi64;
    ReportFailure(
        "Burger::BigEndian::Load(int64_t) = 0x%08X%08X / Wanted (0x%08X%08X)",
        uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
        uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadFloat(void)
{
    float fResult = Burger::BigEndian::Load(g_BEF32_1234.m_Float);
    uint_t uFailure = (fResult != NEf32);
    ReportFailure("Burger::BigEndian::Load(float) = %g / Wanted (%g)", uFailure,
        fResult, NEf32);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadDouble(void)
{
    double fResult = Burger::BigEndian::Load(g_BEF64_1234.m_Double);
    uint_t uFailure = (fResult != NEf64);
    ReportFailure("Burger::BigEndian::Load(double) = %g / Wanted (%g)",
        uFailure, fResult, NEf64);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord16Ptr(void)
{
    uint16_t uResult = Burger::BigEndian::Load(&g_BE16_1234.m_Word16);
    uint_t uFailure = (uResult != NEu16);
    ReportFailure(
        "Burger::BigEndian::Load(const uint16_t *) = 0x%04X / Wanted (0x%04X)",
        uFailure, uResult, NEu16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt16Ptr(void)
{
    int16_t iResult = Burger::BigEndian::Load(&g_BE16_1234.m_Int16);
    uint_t uFailure = (iResult != NEi16);
    ReportFailure(
        "Burger::BigEndian::Load(const int16_t *) = 0x%04X / Wanted (0x%04X)",
        uFailure, iResult, NEi16);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord32Ptr(void)
{
    uint32_t uResult = Burger::BigEndian::Load(&g_BE32_1234.m_Word32);
    uint_t uFailure = (uResult != NEu32);
    ReportFailure(
        "Burger::BigEndian::Load(const uint32_t *) = 0x%08X / Wanted (0x%08X)",
        uFailure, uResult, NEu32);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadInt32Ptr(void)
{
    int32_t iResult = Burger::BigEndian::Load(&g_BE32_1234.m_Int32);
    uint_t uFailure = (iResult != NEi32);
    ReportFailure(
        "Burger::BigEndian::Load(const int32_t *) = 0x%08X / Wanted (0x%08X)",
        uFailure, iResult, NEi32);
    return uFailure;
}

static uint_t BURGER_API TestBurgerBigEndianLoadWord64Ptr(void)
{
    uint64_t uResult = Burger::BigEndian::Load(&g_BE64_1234.m_Word64);
    uint_t uFailure = (uResult != NEu64);
    Union64_t uMem1;
    uMem1.m_Word64 = uResult;
    Union64_t uMem2;
    uMem2.m_Word64 = NEu64;
    ReportFailure(
        "Burger::BigEndian::Load(const uint64_t *) = 0x%08X%08X / Wanted (0x%08X%08X)",
        uFailure, uMem1.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem1.m_Word32[BURGER_ENDIANINDEX_LOW],
        uMem2.m_Word32[BURGER_ENDIANINDEX_HIGH],
        uMem2.m_Word32[BURGER_ENDIANINDEX_LOW]);
    return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianLoadInt64Ptr(void)
{
	Int64 iResult = Burger::LittleEndian::Load(&g_LE64_1234.m_Int64);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::LittleEndian::Load(const Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianLoadFloatPtr(void)
{
	float fResult = Burger::LittleEndian::Load(&g_LEF32_1234.m_Float);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::LittleEndian::Load(const float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianLoadDoublePtr(void)
{
	double fResult = Burger::LittleEndian::Load(&g_LEF64_1234.m_Double);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::LittleEndian::Load(const double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Test LittleEndian::LoadAny()
//

static Word BURGER_API TestBurgerLittleEndianLoadAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Word16)[1];
		// Load the value
		Word16 uResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
		Word uFailure = (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Int16)[1];
		// Load the value
		Int16 iResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Int16 *>(pWork));
		Word uFailure = (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[3];
		// Load the value
		Word32 uResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Word32 *>(pWork));
		Word uFailure = (uResult!=NEu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[3];
		// Load the value
		Int32 iResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Int32 *>(pWork));
		Word uFailure = (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[7];
		// Load the value
		Word64 uResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
		Word uFailure = (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::LittleEndian::LoadAny(const Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[7];
		// Load the value
		Int64 iResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const Int64 *>(pWork));
		Word uFailure = (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::LittleEndian::LoadAny(const Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[3];
		// Load the value
		float fResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork));
		Word uFailure = (fResult!=NEf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianLoadAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[7];
		// Load the value
		double dResult = Burger::LittleEndian::LoadAny(reinterpret_cast<const double *>(pWork));
		Word uFailure = (dResult!=NEf64);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::LoadAny(const double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Burger::LittleEndian::Fixup()
//

static Word BURGER_API TestBurgerLittleEndianFixupWord16Ptr(void)
{
	Word16 uResult = g_LE16_1234.m_Word16;
	Burger::LittleEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu16);
	ReportFailure("Burger::LittleEndian::Fixup(Word16 *) = 0x%04X / Wanted (0x%04X)",uFailure,uResult,NEu16);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupInt16Ptr(void)
{
	Int16 iResult = g_LE16_1234.m_Int16;
	Burger::LittleEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi16);
	ReportFailure("Burger::LittleEndian::Fixup(Int16 *) = 0x%04X / Wanted (0x%04X)",uFailure,iResult,NEi16);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupWord32Ptr(void)
{
	Word32 uResult = g_LE32_1234.m_Word32;
	Burger::LittleEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu32);
	ReportFailure("Burger::LittleEndian::Fixup(Word32 *) = 0x%08X / Wanted (0x%08X)",uFailure,uResult,NEu32);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupInt32Ptr(void)
{
	Int32 iResult = g_LE32_1234.m_Int32;
	Burger::LittleEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi32);
	ReportFailure("Burger::LittleEndian::Fixup(Int32 *) = 0x%08X / Wanted (0x%08X)",uFailure,iResult,NEi32);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupWord64Ptr(void)
{
	Word64 uResult = g_LE64_1234.m_Word64;
	Burger::LittleEndian::Fixup(&uResult);
	Word uFailure = (uResult!=NEu64);
	Union64_t uMem1;
	uMem1.m_Word64 = uResult;
	Union64_t uMem2;
	uMem2.m_Word64 = NEu64;
	ReportFailure("Burger::LittleEndian::Fixup(Word64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupInt64Ptr(void)
{
	Int64 iResult = g_LE64_1234.m_Int64;
	Burger::LittleEndian::Fixup(&iResult);
	Word uFailure = (iResult!=NEi64);
	Union64_t uMem1;
	uMem1.m_Int64 = iResult;
	Union64_t uMem2;
	uMem2.m_Int64 = NEi64;
	ReportFailure("Burger::LittleEndian::Fixup(Int64 *) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupFloatPtr(void)
{
	float fResult = g_LEF32_1234.m_Float;
	Burger::LittleEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf32);
	ReportFailure("Burger::LittleEndian::Fixup(float *) = %g / Wanted (%g)",uFailure,fResult,NEf32);
	return uFailure;
}

static Word BURGER_API TestBurgerLittleEndianFixupDoublePtr(void)
{
	double fResult = g_LEF64_1234.m_Double;
	Burger::LittleEndian::Fixup(&fResult);
	Word uFailure = (fResult!=NEf64);
	ReportFailure("Burger::LittleEndian::Fixup(double*) = %g / Wanted (%g)",uFailure,fResult,NEf64);
	return uFailure;
}

//
// Burger::LittleEndian::FixupAny()
//

static Word BURGER_API TestBurgerLittleEndianFixupAnyWord16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Word16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Word16)[1];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Word16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Word16 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		uFailure |= (uResult!=NEu16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::FixupAny(Word16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,uResult,NEu16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyInt16Ptr(void)
{
	Word8 Buffer[16];
	Word i=4;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Int16)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE16_1234.m_Int16)[1];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Int16 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,2);
		// Load the value
		Int16 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		uFailure |= (iResult!=NEi16);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::FixupAny(Int16 *%p) = 0x%04X / Wanted (0x%04X)",uFailure,pWork,iResult,NEi16);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyWord32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Word32)[3];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Word32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Word32 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		uFailure |= (uResult!=NEu32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::FixupAny(Word32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,uResult,NEu32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyInt32Ptr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE32_1234.m_Int32)[3];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Int32 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		Int32 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		uFailure |= (iResult!=NEi32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::FixupAny(Int32 *%p) = 0x%08X / Wanted (0x%08X)",uFailure,pWork,iResult,NEi32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyWord64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Word64)[7];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Word64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Word64 uResult;
		reinterpret_cast<Word8 *>(&uResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&uResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&uResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&uResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&uResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&uResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&uResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&uResult)[7] = pWork[7];
		uFailure |= (uResult!=NEu64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Word64 = uResult;
		Union64_t uMem2;
		uMem2.m_Word64 = NEu64;
		ReportFailure("Burger::LittleEndian::FixupAny(Word64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyInt64Ptr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LE64_1234.m_Int64)[7];
		Burger::LittleEndian::FixupAny(reinterpret_cast<Int64 *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		Int64 iResult;
		reinterpret_cast<Word8 *>(&iResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&iResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&iResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&iResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&iResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&iResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&iResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&iResult)[7] = pWork[7];
		uFailure |= (iResult!=NEi64);
		uFailureTotal|=uFailure;
		Union64_t uMem1;
		uMem1.m_Int64 = iResult;
		Union64_t uMem2;
		uMem2.m_Int64 = NEi64;
		ReportFailure("Burger::LittleEndian::FixupAny(Int64 *%p) = 0x%08X%08X / Wanted (0x%08X%08X)",uFailure,pWork,uMem1.m_Word32[WORD64HIGH],uMem1.m_Word32[WORD64LOW],uMem2.m_Word32[WORD64HIGH],uMem2.m_Word32[WORD64LOW]);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyFloatPtr(void)
{
	Word8 Buffer[16];
	Word i=8;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LEF32_1234.m_Float)[3];
		Burger::LittleEndian::FixupAny(reinterpret_cast<float *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,4);
		// Load the value
		float fResult;
		reinterpret_cast<Word8 *>(&fResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&fResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&fResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&fResult)[3] = pWork[3];
		uFailure |= (fResult!=NEf32);
		uFailureTotal|=uFailure;
		ReportFailure("Burger::LittleEndian::FixupAny(float *%p) = %g / Wanted (%g)",uFailure,pWork,fResult,NEf32);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

static Word BURGER_API TestBurgerLittleEndianFixupAnyDoublePtr(void)
{
	Word8 Buffer[32];
	Word i=16;
	Word8 *pWork = Buffer;
	Word uFailureTotal = FALSE;
	do {
		// Init the source buffer
		BlastBuffer(Buffer,sizeof(Buffer));
		// Copy the value to read (Keep endian native)
		pWork[0] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[0];
		pWork[1] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[1];
		pWork[2] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[2];
		pWork[3] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[3];
		pWork[4] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[4];
		pWork[5] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[5];
		pWork[6] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[6];
		pWork[7] = reinterpret_cast<const Word8 *>(&g_LEF64_1234.m_Double)[7];
		Burger::LittleEndian::FixupAny(reinterpret_cast<double *>(pWork));
		Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),pWork,8);
		// Load the value
		double dResult;
		reinterpret_cast<Word8 *>(&dResult)[0] = pWork[0];
		reinterpret_cast<Word8 *>(&dResult)[1] = pWork[1];
		reinterpret_cast<Word8 *>(&dResult)[2] = pWork[2];
		reinterpret_cast<Word8 *>(&dResult)[3] = pWork[3];
		reinterpret_cast<Word8 *>(&dResult)[4] = pWork[4];
		reinterpret_cast<Word8 *>(&dResult)[5] = pWork[5];
		reinterpret_cast<Word8 *>(&dResult)[6] = pWork[6];
		reinterpret_cast<Word8 *>(&dResult)[7] = pWork[7];
		uFailure |= (dResult!=NEf64);
		ReportFailure("Burger::LittleEndian::FixupAny(double *%p) = %g / Wanted (%g)",uFailure,pWork,dResult,NEf64);
		++pWork;
	} while (--i);
	return uFailureTotal;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrendian(Word bVerbose)
{	
	Word uTotal;	// Assume no failures

	if (bVerbose) {
		Message("Running Endian tests");
	}

	// Test compiler switches
	uTotal = TestEndianFlags();

	// SwapEndian::Load()
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

	// SwapEndian::LoadAny()
	uTotal |= TestBurgerSwapEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerSwapEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerSwapEndianLoadAnyDoublePtr();

	// SwapEndian::Fixup()
	uTotal |= TestBurgerSwapEndianFixupWord16Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt16Ptr();
	uTotal |= TestBurgerSwapEndianFixupWord32Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt32Ptr();
	uTotal |= TestBurgerSwapEndianFixupWord64Ptr();
	uTotal |= TestBurgerSwapEndianFixupInt64Ptr();
	uTotal |= TestBurgerSwapEndianFixupFloatPtr();
	uTotal |= TestBurgerSwapEndianFixupDoublePtr();

	// SwapEndian::FixupAny()
	uTotal |= TestBurgerSwapEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerSwapEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerSwapEndianFixupAnyDoublePtr();

	// NativeEndian::Load()
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

	// NativeEndian::LoadAny()
	uTotal |= TestBurgerNativeEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerNativeEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerNativeEndianLoadAnyDoublePtr();

	// NativeEndian::Fixup()
	uTotal |= TestBurgerNativeEndianFixupWord16Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt16Ptr();
	uTotal |= TestBurgerNativeEndianFixupWord32Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt32Ptr();
	uTotal |= TestBurgerNativeEndianFixupWord64Ptr();
	uTotal |= TestBurgerNativeEndianFixupInt64Ptr();
	uTotal |= TestBurgerNativeEndianFixupFloatPtr();
	uTotal |= TestBurgerNativeEndianFixupDoublePtr();

	// NativeEndian::FixupAny()
	uTotal |= TestBurgerNativeEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerNativeEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerNativeEndianFixupAnyDoublePtr();

	// BigEndian::Load()
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

	// BigEndian::LoadAny()
	uTotal |= TestBurgerBigEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerBigEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerBigEndianLoadAnyDoublePtr();

	// BigEndian::Fixup()
	uTotal |= TestBurgerBigEndianFixupWord16Ptr();
	uTotal |= TestBurgerBigEndianFixupInt16Ptr();
	uTotal |= TestBurgerBigEndianFixupWord32Ptr();
	uTotal |= TestBurgerBigEndianFixupInt32Ptr();
	uTotal |= TestBurgerBigEndianFixupWord64Ptr();
	uTotal |= TestBurgerBigEndianFixupInt64Ptr();
	uTotal |= TestBurgerBigEndianFixupFloatPtr();
	uTotal |= TestBurgerBigEndianFixupDoublePtr();

	// BigEndian::FixupAny()
	uTotal |= TestBurgerBigEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerBigEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerBigEndianFixupAnyDoublePtr();

	// LittleEndian::Load()
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

	// LittleEndian::LoadAny()
	uTotal |= TestBurgerLittleEndianLoadAnyWord16Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt16Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyWord32Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt32Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyWord64Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyInt64Ptr();
	uTotal |= TestBurgerLittleEndianLoadAnyFloatPtr();
	uTotal |= TestBurgerLittleEndianLoadAnyDoublePtr();

	// LittleEndian::Fixup()
	uTotal |= TestBurgerLittleEndianFixupWord16Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt16Ptr();
	uTotal |= TestBurgerLittleEndianFixupWord32Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt32Ptr();
	uTotal |= TestBurgerLittleEndianFixupWord64Ptr();
	uTotal |= TestBurgerLittleEndianFixupInt64Ptr();
	uTotal |= TestBurgerLittleEndianFixupFloatPtr();
	uTotal |= TestBurgerLittleEndianFixupDoublePtr();

	// LittleEndian::FixupAny()
	uTotal |= TestBurgerLittleEndianFixupAnyWord16Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt16Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyWord32Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt32Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyWord64Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyInt64Ptr();
	uTotal |= TestBurgerLittleEndianFixupAnyFloatPtr();
	uTotal |= TestBurgerLittleEndianFixupAnyDoublePtr();

	if (!uTotal && bVerbose) {
		Message("Passed all Endian tests!");
	}
	return static_cast<int>(uTotal);
}
