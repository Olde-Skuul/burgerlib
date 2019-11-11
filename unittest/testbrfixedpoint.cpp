/***************************************

    Unit tests for the Integer Math library

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfixedpoint.h"
#include "common.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"
#include "brnumberstringhex.h"
#include "brfloatingpoint.h"

struct WordTest32_t {
	Word32 m_uInput;	// Source value
	Word32 m_uOutput;	// Expected output
};

struct WordTest64_t {
	Word64 m_uInput;	// Source value
	Word64 m_uOutput;	// Expected output
};

struct IntTest8_t {
	Int8 m_iInput;		// Source value
	Int8 m_iOutput;		// Expected output
};

struct IntTest16_t {
	Int16 m_iInput;		// Source value
	Int16 m_iOutput;	// Expected output
};

struct IntTest32_t {
	Int32 m_iInput;		// Source value
	Int32 m_iOutput;	// Expected output
};

struct FixedTest32_t {
	Fixed32 m_iInput;		// Source value
	Fixed32 m_iOutput;	// Expected output
};

struct IntTest64_t {
	Int64 m_iInput;		// Source value
	Int64 m_iOutput;	// Expected output
};

struct FloatToIntTest32_t {
	Burger::Word32ToFloat m_fInput;		// Source value
	Int32 m_iOutput;	// Expected output
};

struct OldFloatToIntTest32_t {
	float m_fInput;		// Source value
	Int32 m_iOutput;	// Expected output
};

struct IntTest32x32_t {
	Int32 m_iInput1;	// Source value 1
	Int32 m_iInput2;	// Source value 2
	Int32 m_iOutput;	// Expected output
};

struct IntTest64x64_t {
	Int64 m_iInput1;	// Source value 1
	Int64 m_iInput2;	// Source value 2
	Int64 m_iOutput;	// Expected output
};

/***************************************
 
	Test Burger::PowerOf2(Word32)
 
***************************************/

static const WordTest32_t PowerOf2_32Table[] = {
	{0x00000000U,0x00000000U},
	{0x00000001U,0x00000001U},
	{0x00000002U,0x00000002U},
	{0x00000003U,0x00000004U},
	{0x00000004U,0x00000004U},
	{0x00000005U,0x00000008U},
	{0x00002000U,0x00002000U},
	{0x00002001U,0x00004000U},
	{0x20000001U,0x40000000U},
	{0x40000000U,0x40000000U},
	{0x40000001U,0x80000000U},
	{0x40004020U,0x80000000U},
	{0x7FFFFFFFU,0x80000000U},
	{0x80000000U,0x80000000U},
	{0x80000001U,0x00000000U},
	{0xC0000000U,0x00000000U},
	{0xE0000001U,0x00000000U},
	{0xFFFFFFFFU,0x00000000U}
};

static Word BURGER_API TestPowerOf2_32(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = PowerOf2_32Table;
	WordPtr uCount = BURGER_ARRAYSIZE(PowerOf2_32Table);
	do {
		Word32 uReturn = Burger::PowerOf2(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::PowerOf2(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uReturn,pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************
 
	Test Burger::PowerOf2(Word64)
 
***************************************/

static const WordTest64_t PowerOf2_64Table[] = {
	{0x0000000000000000ULL,0x0000000000000000ULL},
	{0x0000000000000001ULL,0x0000000000000001ULL},
	{0x0000000000000002ULL,0x0000000000000002ULL},
	{0x0000000000000003ULL,0x0000000000000004ULL},
	{0x0000000000000004ULL,0x0000000000000004ULL},
	{0x0000000000000005ULL,0x0000000000000008ULL},
	{0x0000000000002000ULL,0x0000000000002000ULL},
	{0x0000000000002001ULL,0x0000000000004000ULL},
	{0x0000000020000001ULL,0x0000000040000000ULL},
	{0x0000000040000000ULL,0x0000000040000000ULL},
	{0x0000000040000001ULL,0x0000000080000000ULL},
	{0x0000000040004020ULL,0x0000000080000000ULL},
	{0x000000007FFFFFFFULL,0x0000000080000000ULL},
	{0x0000000080000000ULL,0x0000000080000000ULL},
	{0x0000000080000001ULL,0x0000000100000000ULL},
	{0x00000000FFFFFFFFULL,0x0000000100000000ULL},
	{0x4000000000000000ULL,0x4000000000000000ULL},
	{0x4000000000000001ULL,0x8000000000000000ULL},
	{0x4000000000004020ULL,0x8000000000000000ULL},
	{0x8000000000000000ULL,0x8000000000000000ULL},
	{0x8000000000000001ULL,0x0000000000000000ULL},
	{0xC000000000000000ULL,0x0000000000000000ULL},
	{0xE000000000000001ULL,0x0000000000000000ULL},
	{0xFFFFFFFFFFFFFFFFULL,0x0000000000000000ULL}
};

static Word BURGER_API TestPowerOf2_64(void)
{
	Word uFailure = FALSE;
	const WordTest64_t *pWork = PowerOf2_64Table;
	WordPtr uCount = BURGER_ARRAYSIZE(PowerOf2_64Table);
	do {
		Word64 uReturn = Burger::PowerOf2(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::NumberStringHex Input(pWork->m_uInput);
			Burger::NumberStringHex Return(uReturn);
			Burger::NumberStringHex Output(pWork->m_uOutput);
			ReportFailure("Burger::PowerOf2(0x%s) = 0x%s, expected 0x%s",uTest,Input.GetPtr(),Return.GetPtr(),Output.GetPtr());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************
 
	Test Burger::PowerOf2(Word32)
 
***************************************/

static const WordTest32_t BCDWordTable[] = {
	{0x00000000U,0U},
	{0x00000001U,1U},
	{0x00000002U,2U},
	{0x00000003U,3U},
	{0x00000004U,4U},
	{0x98765432U,98765432U},
	{0x00002000U,2000U},
	{0x00002001U,2001U},
	{0x20000001U,20000001U},
	{0x40000000U,40000000U},
	{0x40000001U,40000001U},
	{0x40004020U,40004020U},
	{0x12345678U,12345678U},
	{0x99999999U,99999999U},
	{0x98765432U,98765432U},
	{0x87654321U,87654321U},
	{0x8000000FU,80000015U},
	{0xFFFFFFFFU,166666665}
};

static Word BURGER_API TestBCDWord(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = BCDWordTable;
	WordPtr uCount = BURGER_ARRAYSIZE(BCDWordTable);
	do {
		Word32 uReturn = Burger::BCDToWord(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::BCDToWord(0x%08X) = %u, expected %u",uTest,pWork->m_uInput,uReturn,pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************
 
	Test Burger::BitReverse(Word32,Word)
 
***************************************/

static const WordTest32_t BitReverse_32Table[] = {
	{0x00000000U,0x00000000U},
	{0x00000001U,0x80000000U},
	{0x00000002U,0x40000000U},
	{0x00000003U,0xC0000000U},
	{0x00000004U,0x20000000U},
	{0x00000005U,0xA0000000U},
	{0x00002000U,0x00040000U},
	{0x00002001U,0x80040000U},
	{0x20000001U,0x80000004U},
	{0x40000000U,0x00000002U},
	{0x40000001U,0x80000002U},
	{0x40004020U,0x04020002U},
	{0x7FFFFFFFU,0xFFFFFFFEU},
	{0x80000000U,0x00000001U},
	{0x80000001U,0x80000001U},
	{0xFFFFFFFFU,0xFFFFFFFFU},
	{0xAAAAAAAAU,0x55555555U},
	{0xAAAA0000U,0x00005555U},
	{0x12345678U,0x1E6A2C48U}
};

static Word BURGER_API TestBitReverse_32(void)
{
	Word uFailure = FALSE;
	Word uBitCount = 1;
	do {
		const WordTest32_t *pWork = BitReverse_32Table;
		WordPtr uCount = BURGER_ARRAYSIZE(BitReverse_32Table);
		do {
			Word32 uReturn = Burger::BitReverse(pWork->m_uInput,uBitCount);
			Word32 uToTest = pWork->m_uOutput>>(32-uBitCount);
			Word uTest = uReturn!=uToTest;
			uFailure |= uTest;
			ReportFailure("Burger::BitReverse(0x%08X,%u) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uBitCount,uReturn,uToTest);
			++pWork;
		} while (--uCount);
	} while (++uBitCount<(32+1));
	return uFailure;
}


/***************************************
 
	Test Burger::BitReverse(Word64,Word)
 
***************************************/

static const WordTest64_t BitReverse_64Table[] = {
	{0x0000000000000000ULL,0x0000000000000000ULL},
	{0x0000000000000001ULL,0x8000000000000000ULL},
	{0x0000000000000002ULL,0x4000000000000000ULL},
	{0x0000000000000003ULL,0xC000000000000000ULL},
	{0x0000000000000004ULL,0x2000000000000000ULL},
	{0x0000000000000005ULL,0xA000000000000000ULL},
	{0x0000000000002000ULL,0x0004000000000000ULL},
	{0x0000000000002001ULL,0x8004000000000000ULL},
	{0x2000000000000001ULL,0x8000000000000004ULL},
	{0x4000000000000000ULL,0x0000000000000002ULL},
	{0x4000000000000001ULL,0x8000000000000002ULL},
	{0x4000000000004020ULL,0x0402000000000002ULL},
	{0x7FFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFEULL},
	{0x8000000000000000ULL,0x0000000000000001ULL},
	{0x8000000000000001ULL,0x8000000000000001ULL},
	{0xFFFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL},
	{0xAAAAAAAAAAAAAAAAULL,0x5555555555555555ULL},
	{0xAAAAAAAA00000000ULL,0x0000000055555555ULL},
	{0x1234567890ABCDEFULL,0xF7B3D5091E6A2C48ULL}
};

static Word BURGER_API TestBitReverse_64(void)
{
	Word uFailure = FALSE;
	Word uBitCount = 1;
	do {
		const WordTest64_t *pWork = BitReverse_64Table;
		WordPtr uCount = BURGER_ARRAYSIZE(BitReverse_64Table);
		do {
			Word64 uReturn = Burger::BitReverse(pWork->m_uInput,uBitCount);
			Word64 uToTest = pWork->m_uOutput>>(64-uBitCount);
			Word uTest = uReturn!=uToTest;
			uFailure |= uTest;
			if (uTest) {
				Burger::NumberStringHex Input(pWork->m_uInput);
				Burger::NumberStringHex Return(uReturn);
				Burger::NumberStringHex Output(pWork->m_uOutput);
				ReportFailure("Burger::BitReverse(0x%s,%u) = 0x%s, expected 0x%s",uTest,Input.GetPtr(),uBitCount,Return.GetPtr(),Output.GetPtr());
			}
			++pWork;
		} while (--uCount);
	} while (++uBitCount<(64+1));
	return uFailure;
}

/***************************************
 
	Test Burger::BitSetCount(Word32)
 
***************************************/

static const WordTest32_t BitSetCount_32Table[] = {
	{0x00000000U,0},
	{0x00000001U,1},
	{0x00000002U,1},
	{0x00000003U,2},
	{0x00000004U,1},
	{0x00000005U,2},
	{0x00002000U,1},
	{0x00002001U,2},
	{0x20000001U,2},
	{0x40000000U,1},
	{0x40000001U,2},
	{0x40004020U,3},
	{0x7FFFFFFFU,31},
	{0x80000000U,1},
	{0x80000001U,2},
	{0x55555555U,16},
	{0x55550000U,8},
	{0x0000AAAAU,8},
	{0xAAAAAAAAU,16},
	{0xFFFFFFFFU,32},
	{0x12345678U,13}
};
			

static Word BURGER_API TestBitSetCount_32(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = BitSetCount_32Table;
	WordPtr uCount = BURGER_ARRAYSIZE(BitSetCount_32Table);
	do {
		Word uReturn = Burger::BitSetCount(pWork->m_uInput);
		Word uToTest = static_cast<Word>(pWork->m_uOutput);
		Word uTest = uReturn!=uToTest;
		uFailure |= uTest;
		ReportFailure("Burger::BitSetCount(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uReturn,uToTest);
		++pWork;
	} while (--uCount);
	return uFailure;
}


/***************************************
 
	Test Burger::BitSetCount(Word64)
 
***************************************/

static const WordTest64_t BitSetCount_64Table[] = {
	{0x0000000000000000ULL,0},
	{0x0000000000000001ULL,1},
	{0x0000000000000002ULL,1},
	{0x0000000000000003ULL,2},
	{0x0000000000000004ULL,1},
	{0x0000000000000005ULL,2},
	{0x0000000000002000ULL,1},
	{0x0000000000002001ULL,2},
	{0x2000000000000001ULL,2},
	{0x4000000000000000ULL,1},
	{0x4000000000000001ULL,2},
	{0x4000000000004020ULL,3},
	{0x7FFFFFFFFFFFFFFFULL,63},
	{0x8000000000000000ULL,1},
	{0x8000000000000001ULL,2},
	{0xFFFFFFFFFFFFFFFFULL,64},
	{0x5555555555555555ULL,32},
	{0xAAAAAAAAAAAAAAAAULL,32},
	{0x5555555500000000ULL,16},
	{0x00000000AAAAAAAAULL,16},
	{0x1234567890ABCDEFULL,32}
};

static Word BURGER_API TestBitSetCount_64(void)
{
	Word uFailure = FALSE;
	const WordTest64_t *pWork = BitSetCount_64Table;
	WordPtr uCount = BURGER_ARRAYSIZE(BitSetCount_64Table);
	do {
		Word uReturn = Burger::BitSetCount(pWork->m_uInput);
		Word uToTest = static_cast<Word>(pWork->m_uOutput);
		Word uTest = uReturn!=uToTest;
		uFailure |= uTest;
		if (uTest) {
			Burger::NumberStringHex Input(pWork->m_uInput);
			ReportFailure("Burger::BitSetCount(0x%s) = 0x%08X, expected 0x%08X",uTest,Input.GetPtr(),uReturn,uToTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************
 
	Test Burger::GetLoWord
 
***************************************/

static const WordTest32_t GetLoWordTable[] = {
	{0x00000000U,0x00000000U},
	{0x00000001U,0x00000001U},
	{0x00000002U,0x00000002U},
	{0x00000003U,0x00000003U},
	{0x00000004U,0x00000004U},
	{0x00000005U,0x00000005U},
	{0x00002000U,0x00002000U},
	{0x00002001U,0x00002001U},
	{0x20000001U,0x00000001U},
	{0x40000000U,0x00000000U},
	{0x40000001U,0x00000001U},
	{0x40004020U,0x00004020U},
	{0x7FFFFFFFU,0x0000FFFFU},
	{0x80000000U,0x00000000U},
	{0x80000001U,0x00000001U},
	{0xFFFFFFFFU,0x0000FFFFU}
};

static Word BURGER_API TestGetLoWord(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = GetLoWordTable;
	WordPtr uCount = BURGER_ARRAYSIZE(GetLoWordTable);
	do {
		Word32 uReturn = Burger::GetLoWord(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::GetLoWord(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uReturn,pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************
 
	Test Burger::GetHiWord
 
***************************************/

static const WordTest32_t GetHiWordTable[] = {
	{0x00000000U,0x00000000U},
	{0x00000001U,0x00000000U},
	{0x00000002U,0x00000000U},
	{0x00000003U,0x00000000U},
	{0x00000004U,0x00000000U},
	{0x00000005U,0x00000000U},
	{0x00002000U,0x00000000U},
	{0x00002001U,0x00000000U},
	{0x20000001U,0x00002000U},
	{0x40000000U,0x00004000U},
	{0x40000001U,0x00004000U},
	{0x40004020U,0x00004000U},
	{0x7FFFFFFFU,0x00007FFFU},
	{0x80000000U,0x00008000U},
	{0x80000001U,0x00008000U},
	{0xFFFFFFFFU,0x0000FFFFU}
};

static Word BURGER_API TestGetHiWord(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = GetHiWordTable;
	WordPtr uCount = BURGER_ARRAYSIZE(GetHiWordTable);
	do {
		Word32 uReturn = Burger::GetHiWord(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::GetHiWord(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uReturn,pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test IntToFixed(Int32)
//

static const IntTest32_t IntToFixedTestTable[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00010000},
	{static_cast<Int32>(0xFFFFFFFF),static_cast<Int32>(0xFFFF0000)},
	{0x00007FFF,0x7FFF0000},
	{0x00008000,static_cast<Int32>(0x80000000)},
	{static_cast<Int32>(0x80000000),0x00000000},
	{static_cast<Int32>(0x80000001),0x00010000},
	{0x00004000,0x40000000},
	{0x40000000,0x00000000},
	{0x0000C000,static_cast<Int32>(0xC0000000)},
	{static_cast<Int32>(0xFFFFC000),static_cast<Int32>(0xC0000000)}
};

static Word BURGER_API TestIntToFixed(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = IntToFixedTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(IntToFixedTestTable);
	do {
		Int32 iReturn = Burger::IntToFixed(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::IntToFixed(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test IntToFixedSaturate(Int32)
//

static const IntTest32_t IntToFixedSaturateTestTable[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00010000},
	{0x00004000,0x40000000},
	{0x00007FFF,0x7FFF0000},
	{0x00008000,0x7FFFFFFF},		// Saturation hit
	{0x00008001,0x7FFFFFFF},
	{0x0000C000,0x7FFFFFFF},
	{0x40000000,0x7FFFFFFF},
	{0x7FFFFFFF,0x7FFFFFFF},
	{static_cast<Int32>(0xFFFFFFFF),static_cast<Int32>(0xFFFF0000)},
	{static_cast<Int32>(0xFFFFFFFE),static_cast<Int32>(0xFFFE0000)},
	{static_cast<Int32>(0xFFFFC000),static_cast<Int32>(0xC0000000)},
	{static_cast<Int32>(0xFFFF8000),static_cast<Int32>(0x80000000)},	// Saturation hit
	{static_cast<Int32>(0xFFFF8001),static_cast<Int32>(0x80000000)},
	{static_cast<Int32>(0xC0000000),static_cast<Int32>(0x80000000)},
	{static_cast<Int32>(0x80000001),static_cast<Int32>(0x80000000)},
	{static_cast<Int32>(0x80000000),static_cast<Int32>(0x80000000)},
};

static Word BURGER_API TestIntToFixedSaturate(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = IntToFixedSaturateTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(IntToFixedSaturateTestTable);
	do {
		Int32 iReturn = Burger::IntToFixedSaturate(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::IntToFixedSaturate(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntFloor(Int32)
//

static const IntTest32_t FixedToIntFloorTestTable[] = {
	{FLOATTOFIXED(-4.0)+1	,-4},
	{FLOATTOFIXED(-3.5f)	,-4},
	{FLOATTOFIXED(-3.5f)+1	,-4},
	{FLOATTOFIXED(-3.0f)	,-3},
	{FLOATTOFIXED(-3.0f)+1	,-3},
	{FLOATTOFIXED(-2.5f)	,-3},
	{FLOATTOFIXED(-2.5f)+1	,-3},
	{FLOATTOFIXED(-2.0f)	,-2},
	{FLOATTOFIXED(-2.0f)+1	,-2},
	{FLOATTOFIXED(-1.5f)	,-2},
	{FLOATTOFIXED(-1.5f)+1	,-2},
	{FLOATTOFIXED(-1.0f)	,-1},
	{FLOATTOFIXED(-1.0f)+1	,-1},
	{FLOATTOFIXED(-0.5f)	,-1},
	{FLOATTOFIXED(-0.5f)+1	,-1},
	{FLOATTOFIXED(-0.0f)	,0},
	{FLOATTOFIXED(0.0f)		,0},
	{FLOATTOFIXED(0.5f)-1	,0},
	{FLOATTOFIXED(0.5f)		,0},
	{FLOATTOFIXED(1.0f)-1	,0},
	{FLOATTOFIXED(1.0f)		,1},
	{FLOATTOFIXED(1.5f)-1	,1},
	{FLOATTOFIXED(1.5f)		,1},
	{FLOATTOFIXED(2.0f)-1	,1},
	{FLOATTOFIXED(2.0f)		,2},
	{FLOATTOFIXED(2.5f)-1	,2},
	{FLOATTOFIXED(2.5f)		,2},
	{FLOATTOFIXED(3.0f)-1	,2},
	{FLOATTOFIXED(3.0f)		,3},
	{FLOATTOFIXED(3.5f)-1	,3},
	{FLOATTOFIXED(3.5f)		,3},
	{FLOATTOFIXED(4.0f)-1	,3},

	{FLOATTOFIXED(1.1f)		,1},
	{FLOATTOFIXED(1.95f)	,1},
	{FLOATTOFIXED(-1.1f)	,-2},
	{FLOATTOFIXED(-1.95f)	,-2},
	{FLOATTOFIXED(0.1f)		,0},
	{FLOATTOFIXED(0.95f)	,0},
	{FLOATTOFIXED(-0.1f)	,-1},
	{FLOATTOFIXED(-0.95f)	,-1}
};

static Word BURGER_API TestFixedToIntFloor(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = FixedToIntFloorTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FixedToIntFloorTestTable);
	do {
		Int32 iReturn = Burger::FixedToIntFloor(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToIntFloor(0x%08X) = %d, expected %d",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntFloor(Int32)
//

static const IntTest32_t FixedToIntTestTable[] = {
	{FLOATTOFIXED(-4.0)+1	,-3},
	{FLOATTOFIXED(-3.5f)	,-3},
	{FLOATTOFIXED(-3.5f)+1	,-3},
	{FLOATTOFIXED(-3.0f)	,-3},
	{FLOATTOFIXED(-3.0f)+1	,-2},
	{FLOATTOFIXED(-2.5f)	,-2},
	{FLOATTOFIXED(-2.5f)+1	,-2},
	{FLOATTOFIXED(-2.0f)	,-2},
	{FLOATTOFIXED(-2.0f)+1	,-1},
	{FLOATTOFIXED(-1.5f)	,-1},
	{FLOATTOFIXED(-1.5f)+1	,-1},
	{FLOATTOFIXED(-1.0f)	,-1},
	{FLOATTOFIXED(-1.0f)+1	,-0},
	{FLOATTOFIXED(-0.5f)	,-0},
	{FLOATTOFIXED(-0.5f)+1	,-0},
	{FLOATTOFIXED(-0.0f)	,0},
	{FLOATTOFIXED(0.0f)		,0},
	{FLOATTOFIXED(0.5f)-1	,0},
	{FLOATTOFIXED(0.5f)		,0},
	{FLOATTOFIXED(1.0f)-1	,0},
	{FLOATTOFIXED(1.0f)		,1},
	{FLOATTOFIXED(1.5f)-1	,1},
	{FLOATTOFIXED(1.5f)		,1},
	{FLOATTOFIXED(2.0f)-1	,1},
	{FLOATTOFIXED(2.0f)		,2},
	{FLOATTOFIXED(2.5f)-1	,2},
	{FLOATTOFIXED(2.5f)		,2},
	{FLOATTOFIXED(3.0f)-1	,2},
	{FLOATTOFIXED(3.0f)		,3},
	{FLOATTOFIXED(3.5f)-1	,3},
	{FLOATTOFIXED(3.5f)		,3},
	{FLOATTOFIXED(4.0f)-1	,3},

	{FLOATTOFIXED(1.1f)		,1},
	{FLOATTOFIXED(1.95f)	,1},
	{FLOATTOFIXED(-1.1f)	,-1},
	{FLOATTOFIXED(-1.95f)	,-1},
	{FLOATTOFIXED(0.1f)		,0},
	{FLOATTOFIXED(0.95f)	,0},
	{FLOATTOFIXED(-0.1f)	,0},
	{FLOATTOFIXED(-0.95f)	,0}
};

static Word BURGER_API TestFixedToInt(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = FixedToIntTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FixedToIntTestTable);
	do {
		Int32 iReturn = Burger::FixedToInt(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToInt(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntCeil(Int32)
//

static const IntTest32_t FixedToIntCeilTestTable[] = {
	{FLOATTOFIXED(-4.0)+1	,-3},
	{FLOATTOFIXED(-3.5f)	,-3},
	{FLOATTOFIXED(-3.5f)+1	,-3},
	{FLOATTOFIXED(-3.0f)	,-3},
	{FLOATTOFIXED(-3.0f)+1	,-2},
	{FLOATTOFIXED(-2.5f)	,-2},
	{FLOATTOFIXED(-2.5f)+1	,-2},
	{FLOATTOFIXED(-2.0f)	,-2},
	{FLOATTOFIXED(-2.0f)+1	,-1},
	{FLOATTOFIXED(-1.5f)	,-1},
	{FLOATTOFIXED(-1.5f)+1	,-1},
	{FLOATTOFIXED(-1.0f)	,-1},
	{FLOATTOFIXED(-1.0f)+1	,-0},
	{FLOATTOFIXED(-0.5f)	,-0},
	{FLOATTOFIXED(-0.5f)+1	,-0},
	{FLOATTOFIXED(-0.0f)	,0},
	{FLOATTOFIXED(0.0f)		,0},
	{FLOATTOFIXED(0.5f)-1	,1},
	{FLOATTOFIXED(0.5f)		,1},
	{FLOATTOFIXED(1.0f)-1	,1},
	{FLOATTOFIXED(1.0f)		,1},
	{FLOATTOFIXED(1.5f)-1	,2},
	{FLOATTOFIXED(1.5f)		,2},
	{FLOATTOFIXED(2.0f)-1	,2},
	{FLOATTOFIXED(2.0f)		,2},
	{FLOATTOFIXED(2.5f)-1	,3},
	{FLOATTOFIXED(2.5f)		,3},
	{FLOATTOFIXED(3.0f)-1	,3},
	{FLOATTOFIXED(3.0f)		,3},
	{FLOATTOFIXED(3.5f)-1	,4},
	{FLOATTOFIXED(3.5f)		,4},
	{FLOATTOFIXED(4.0f)-1	,4},

	{FLOATTOFIXED(1.1f)		,2},
	{FLOATTOFIXED(1.95f)	,2},
	{FLOATTOFIXED(-1.1f)	,-1},
	{FLOATTOFIXED(-1.95f)	,-1},
	{FLOATTOFIXED(0.1f)		,1},
	{FLOATTOFIXED(0.95f)	,1},
	{FLOATTOFIXED(-0.1f)	,0},
	{FLOATTOFIXED(-0.95f)	,0}
};

static Word BURGER_API TestFixedToIntCeil(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = FixedToIntCeilTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FixedToIntCeilTestTable);
	do {
		Int32 iReturn = Burger::FixedToIntCeil(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToIntCeil(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FixedToIntNearest(Int32)
//

static const IntTest32_t FixedToIntNearestTestTable[] = {
	{FLOATTOFIXED(-4.0)+1	,-4},
	{FLOATTOFIXED(-3.5f)	,-4},
	{FLOATTOFIXED(-3.5f)+1	,-3},
	{FLOATTOFIXED(-3.0f)	,-3},
	{FLOATTOFIXED(-3.0f)+1	,-3},
	{FLOATTOFIXED(-2.5f)	,-3},
	{FLOATTOFIXED(-2.5f)+1	,-2},
	{FLOATTOFIXED(-2.0f)	,-2},
	{FLOATTOFIXED(-2.0f)+1	,-2},
	{FLOATTOFIXED(-1.5f)	,-2},
	{FLOATTOFIXED(-1.5f)+1	,-1},
	{FLOATTOFIXED(-1.0f)	,-1},
	{FLOATTOFIXED(-1.0f)+1	,-1},
	{FLOATTOFIXED(-0.5f)	,-1},
	{FLOATTOFIXED(-0.5f)+1	,-0},
	{FLOATTOFIXED(-0.0f)	,0},
	{FLOATTOFIXED(0.0f)		,0},
	{FLOATTOFIXED(0.5f)-1	,0},
	{FLOATTOFIXED(0.5f)		,1},
	{FLOATTOFIXED(1.0f)-1	,1},
	{FLOATTOFIXED(1.0f)		,1},
	{FLOATTOFIXED(1.5f)-1	,1},
	{FLOATTOFIXED(1.5f)		,2},
	{FLOATTOFIXED(2.0f)-1	,2},
	{FLOATTOFIXED(2.0f)		,2},
	{FLOATTOFIXED(2.5f)-1	,2},
	{FLOATTOFIXED(2.5f)		,3},
	{FLOATTOFIXED(3.0f)-1	,3},
	{FLOATTOFIXED(3.0f)		,3},
	{FLOATTOFIXED(3.5f)-1	,3},
	{FLOATTOFIXED(3.5f)		,4},
	{FLOATTOFIXED(4.0f)-1	,4},

	{FLOATTOFIXED(1.1f)		,1},
	{FLOATTOFIXED(1.95f)	,2},
	{FLOATTOFIXED(-1.1f)	,-1},
	{FLOATTOFIXED(-1.95f)	,-2},
	{FLOATTOFIXED(0.1f)		,0},
	{FLOATTOFIXED(0.95f)	,1},
	{FLOATTOFIXED(-0.1f)	,0},
	{FLOATTOFIXED(-0.95f)	,-1}
};

static Word BURGER_API TestFixedToIntNearest(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = FixedToIntNearestTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FixedToIntNearestTestTable);
	do {
		Int32 iReturn = Burger::FixedToIntNearest(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FixedToIntNearest(0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntFloor(float)
//

static const OldFloatToIntTest32_t FloatToIntFloorTestTable[] = {
	{-3.999f	,-4},
	{-3.5f		,-4},
	{-3.499f	,-4},
	{-3.0f		,-3},
	{-2.999f	,-3},
	{-2.5f		,-3},
	{-2.499f	,-3},
	{-2.0f		,-2},
	{-1.999f	,-2},
	{-1.5f		,-2},
	{-1.499f	,-2},
	{-1.0f		,-1},
	{-0.999f	,-1},
	{-0.5f		,-1},
	{-0.499f	,-1},
	{-0.0f		,0},

	{0.0f		,0},
	{0.499f		,0},
	{0.5f		,0},
	{0.999f		,0},
	{1.0f		,1},
	{1.499f		,1},
	{1.5f		,1},
	{1.999f		,1},
	{2.0f		,2},
	{2.499f		,2},
	{2.5f		,2},
	{2.999f		,2},
	{3.0f		,3},
	{3.499f		,3},
	{3.5f		,3},
	{3.999f		,3},

	{1.1f	,1},
	{1.95f	,1},
	{-1.1f	,-2},
	{-1.95f	,-2},
	{0.1f	,0},
	{0.95f	,0},
	{-0.1f	,-1},
	{-0.95f	,-1}
};

static Word BURGER_API TestFloatToIntFloor(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToIntFloorTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToIntFloorTestTable);
	do {
		Int32 iReturn = Burger::FloatToIntFloor(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntFloor(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToIntFloor(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntFloor(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntRoundToZero(float)
//

static const OldFloatToIntTest32_t FloatToIntTestTable[] = {
	{-3.999f	,-3},
	{-3.5f		,-3},
	{-3.499f	,-3},
	{-3.0f		,-3},
	{-2.999f	,-2},
	{-2.5f		,-2},
	{-2.499f	,-2},
	{-2.0f		,-2},
	{-1.999f	,-1},
	{-1.5f		,-1},
	{-1.499f	,-1},
	{-1.0f		,-1},
	{-0.999f	,-0},
	{-0.5f		,-0},
	{-0.499f	,-0},
	{-0.0f		,0},

	{0.0f		,0},
	{0.499f		,0},
	{0.5f		,0},
	{0.999f		,0},
	{1.0f		,1},
	{1.499f		,1},
	{1.5f		,1},
	{1.999f		,1},
	{2.0f		,2},
	{2.499f		,2},
	{2.5f		,2},
	{2.999f		,2},
	{3.0f		,3},
	{3.499f		,3},
	{3.5f		,3},
	{3.999f		,3},

	{1.1f	,1},
	{1.95f	,1},
	{-1.1f	,-1},
	{-1.95f	,-1},
	{0.1f	,0},
	{0.95f	,0},
	{-0.1f	,0},
	{-0.95f	,0}
};

static Word BURGER_API TestFloatToIntRoundToZero(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToIntTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToIntTestTable);
	do {
		Int32 iReturn = Burger::FloatToIntRoundToZero(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntRoundToZero(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToIntRoundToZero(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntRoundToZero(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntCeil(float)
//

static const OldFloatToIntTest32_t FloatToIntCeilTestTable[] = {
	{-3.999f	,-3},
	{-3.5f		,-3},
	{-3.499f	,-3},
	{-3.0f		,-3},
	{-2.999f	,-2},
	{-2.5f		,-2},
	{-2.499f	,-2},
	{-2.0f		,-2},
	{-1.999f	,-1},
	{-1.5f		,-1},
	{-1.499f	,-1},
	{-1.0f		,-1},
	{-0.999f	,-0},
	{-0.5f		,-0},
	{-0.499f	,-0},
	{-0.0f		,0},

	{0.0f		,0},
	{0.499f		,1},
	{0.5f		,1},
	{0.999f		,1},
	{1.0f		,1},
	{1.499f		,2},
	{1.5f		,2},
	{1.999f		,2},
	{2.0f		,2},
	{2.499f		,3},
	{2.5f		,3},
	{2.999f		,3},
	{3.0f		,3},
	{3.499f		,4},
	{3.5f		,4},
	{3.999f		,4},

	{1.1f	,2},
	{1.95f	,2},
	{-1.1f	,-1},
	{-1.95f	,-1},
	{0.1f	,1},
	{0.95f	,1},
	{-0.1f	,0},
	{-0.95f	,0}
};

static Word BURGER_API TestFloatToIntCeil(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToIntCeilTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToIntCeilTestTable);
	do {
		Int32 iReturn = Burger::FloatToIntCeil(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntCeil(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToIntCeil(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntCeil(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToIntRound(float)
//

static const OldFloatToIntTest32_t FloatToIntNearestTestTable[] = {
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

	{1.1f	,1},
	{1.95f	,2},
	{-1.1f	,-1},
	{-1.95f	,-2},
	{0.1f	,0},
	{0.95f	,1},
	{-0.1f	,0},
	{-0.95f	,-1}
};

static Word BURGER_API TestFloatToIntRound(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToIntNearestTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToIntNearestTestTable);
	do {
		Int32 iReturn = Burger::FloatToIntRound(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntRound(%g) = %d, expected %d",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToIntRound(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToIntRound(Int32 *pOutput,%g) = %d, expected %d",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

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

	{1.1f	,0x00011999},
	{1.95f	,0x0001F333},
	{-1.1f	,static_cast<Int32>(0xFFFEE666)},
	{-1.95f	,static_cast<Int32>(0xFFFE0CCC)},
	{0.1f	,0x00001999},
	{0.95f	,0x0000F333},
	{-0.1f	,static_cast<Int32>(0xFFFFE666)},
	{-0.95f	,static_cast<Int32>(0xFFFF0CCC)}
};

static Word BURGER_API TestFloatToFixedFloor(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToFixedFloorTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToFixedFloorTestTable);
	do {
		Int32 iReturn = Burger::FloatToFixedFloor(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedFloor(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToFixedFloor(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedFloor(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixed(float)
//

static const OldFloatToIntTest32_t FloatToFixedTestTable[] = {
	{1.1f	,0x00011999},
	{1.95f	,0x0001F333},
	{-1.1f	,static_cast<Int32>(0xFFFEE667)},
	{-1.95f	,static_cast<Int32>(0xFFFE0CCD)},
	{0.1f	,0x00001999},
	{0.95f	,0x0000F333},
	{-0.1f	,static_cast<Int32>(0xFFFFE667)},
	{-0.95f	,static_cast<Int32>(0xFFFF0CCD)}
};

static Word BURGER_API TestFloatToFixed(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToFixedTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToFixedTestTable);
	do {
		Int32 iReturn = Burger::FloatToFixed(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixed(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToFixed(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixed(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixedCeil(float)
//

static const OldFloatToIntTest32_t FloatToFixedCeilTestTable[] = {
	{1.1f	,0x0001199A},
	{1.95f	,0x0001F334},
	{-1.1f	,static_cast<Int32>(0xFFFEE667)},
	{-1.95f	,static_cast<Int32>(0xFFFE0CCD)},
	{0.1f	,0x0000199A},
	{0.95f	,0x0000F334},
	{-0.1f	,static_cast<Int32>(0xFFFFE667)},
	{-0.95f	,static_cast<Int32>(0xFFFF0CCD)}
};

static Word BURGER_API TestFloatToFixedCeil(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToFixedCeilTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToFixedCeilTestTable);
	do {
		Int32 iReturn = Burger::FloatToFixedCeil(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedCeil(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToFixedCeil(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedCeil(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test FloatToFixedNearest(float)
//

static const OldFloatToIntTest32_t FloatToFixedNearestTestTable[] = {
	{1.1f	,0x0001199A},
	{1.95f	,0x0001F333},
	{-1.1f	,static_cast<Int32>(0xFFFEE666)},
	{-1.95f	,static_cast<Int32>(0xFFFE0CCD)},
	{0.1f	,0x0000199A},
	{0.95f	,0x0000F333},
	{-0.1f	,static_cast<Int32>(0xFFFFE666)},
	{-0.95f	,static_cast<Int32>(0xFFFF0CCD)}
};

static Word BURGER_API TestFloatToFixedNearest(void)
{
	Word uFailure = FALSE;
	const OldFloatToIntTest32_t *pWork = FloatToFixedNearestTestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(FloatToFixedNearestTestTable);
	do {
		Int32 iReturn = Burger::FloatToFixedNearest(pWork->m_fInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedNearest(%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturn,pWork->m_iOutput);

		Int32 iReturnMemory;
		Burger::FloatToFixedNearest(&iReturnMemory,pWork->m_fInput);
		uTest = iReturnMemory!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::FloatToFixedNearest(Int32 *pOutput,%g) = 0x%08X, expected 0x%08X",uTest,pWork->m_fInput,iReturnMemory,pWork->m_iOutput);

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Abs(Int8)
//

static const IntTest8_t AbsTestTable8[] = {
	{0x00,0x00},
	{0x01,0x01},
	{0x40,0x40},
	{(-0x7F)-1,(-0x7F)-1},
	{-0x7F,0x7F},
	{-0x40,0x40},
	{-0x01,0x01}
};

static Word BURGER_API TestAbs8(void)
{
	Word uFailure = FALSE;
	const IntTest8_t *pWork = AbsTestTable8;
	WordPtr uCount = BURGER_ARRAYSIZE(AbsTestTable8);
	do {
		Int8 iReturn = Burger::Abs(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Abs((Int8)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Abs(Int16)
//

static const IntTest16_t AbsTestTable16[] = {
	{0x0000,0x0000},
	{0x0001,0x0001},
	{0x4000,0x4000},
	{(-0x7FFF)-1,(-0x7FFF)-1},
	{-0x7FFF,0x7FFF},
	{-0x4000,0x4000},
	{-0x0001,0x0001}
};

static Word BURGER_API TestAbs16(void)
{
	Word uFailure = FALSE;
	const IntTest16_t *pWork = AbsTestTable16;
	WordPtr uCount = BURGER_ARRAYSIZE(AbsTestTable16);
	do {
		Int16 iReturn = Burger::Abs(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Abs((Int16)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Abs(Int32)
//

static const IntTest32_t AbsTestTable32[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00000001},
	{0x40000000,0x40000000},
	{static_cast<Int32>(0x80000000),static_cast<Int32>(0x80000000)},
	{static_cast<Int32>(0x80000001),0x7FFFFFFF},
	{static_cast<Int32>(0xC0000000),0x40000000},
	{static_cast<Int32>(0xFFFFFFFF),0x00000001}
};

static Word BURGER_API TestAbs32(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = AbsTestTable32;
	WordPtr uCount = BURGER_ARRAYSIZE(AbsTestTable32);
	do {
		Int32 iReturn = Burger::Abs(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Abs((Int32)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Abs(Int64)
//

static const IntTest64_t AbsTestTable64[] = {
	{0x0000000000000000LL,0x0000000000000000LL},
	{0x0000000000000001LL,0x0000000000000001LL},
	{0x4000000000000000LL,0x4000000000000000LL},
	{static_cast<Int64>(0x8000000000000000LL),static_cast<Int64>(0x8000000000000000LL)},
	{-0x7FFFFFFFFFFFFFFFLL,0x7FFFFFFFFFFFFFFFLL},
	{-0x4000000000000000LL,0x4000000000000000LL},
	{-0x000000000000001LL,0x0000000000000001LL}
};

static Word BURGER_API TestAbs64(void)
{
	Word uFailure = FALSE;
	const IntTest64_t *pWork = AbsTestTable64;
	WordPtr uCount = BURGER_ARRAYSIZE(AbsTestTable64);
	do {
		Int64 iReturn = Burger::Abs(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::NumberStringHex Text1(static_cast<Word64>(pWork->m_iInput));
			Burger::NumberStringHex Text2(static_cast<Word64>(iReturn));
			Burger::NumberStringHex Text3(static_cast<Word64>(pWork->m_iOutput));
			ReportFailure("Burger::Abs((Int64)0x%s) = 0x%s, expected 0x%s",uTest,Text1.GetPtr(),Text2.GetPtr(),Text3.GetPtr());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}


//
// Test Sign(Int8)
//

static const IntTest8_t SignTestTable8[] = {
	{0x00,0},
	{0x01,1},
	{0x40,1},
	{(-0x7F)-1,-1},
	{-0x7F,-1},
	{-0x40,-1},
	{-0x01,-1}
};

static Word BURGER_API TestSign8(void)
{
	Word uFailure = FALSE;
	const IntTest8_t *pWork = SignTestTable8;
	WordPtr uCount = BURGER_ARRAYSIZE(SignTestTable8);
	do {
		Int8 iReturn = Burger::Sign(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Sign((Int8)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Sign(Int16)
//

static const IntTest16_t SignTestTable16[] = {
	{0x0000,0},
	{0x0001,1},
	{0x4000,1},
	{(-0x7FFF)-1,-1},
	{-0x7FFF,-1},
	{-0x4000,-1},
	{-0x0001,-1}
};

static Word BURGER_API TestSign16(void)
{
	Word uFailure = FALSE;
	const IntTest16_t *pWork = SignTestTable16;
	WordPtr uCount = BURGER_ARRAYSIZE(SignTestTable16);
	do {
		Int16 iReturn = Burger::Sign(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Sign((Int16)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Sign(Int32)
//

static const IntTest32_t SignTestTable32[] = {
	{0x00000000,0},
	{0x00000001,1},
	{0x40000000,1},
	{static_cast<Int32>(0x80000000),-1},
	{static_cast<Int32>(0x80000001),-1},
	{static_cast<Int32>(0xC0000000),-1},
	{static_cast<Int32>(0xFFFFFFFF),-1}
};

static Word BURGER_API TestSign32(void)
{
	Word uFailure = FALSE;
	const IntTest32_t *pWork = SignTestTable32;
	WordPtr uCount = BURGER_ARRAYSIZE(SignTestTable32);
	do {
		Int32 iReturn = Burger::Sign(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Sign((Int32)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Sign(Int64)
//

static const IntTest64_t SignTestTable64[] = {
	{0x0000000000000000LL,0},
	{0x0000000000000001LL,1},
	{0x4000000000000000LL,1},
	{static_cast<Int64>(0x8000000000000000LL),-1},
	{static_cast<Int64>(0x8000000000000001LL),-1},
	{static_cast<Int64>(0xC000000000000000LL),-1},
	{static_cast<Int64>(0xFFFFFFFFFFFFFFFFLL),-1}
};

static Word BURGER_API TestSign64(void)
{
	Word uFailure = FALSE;
	const IntTest64_t *pWork = SignTestTable64;
	WordPtr uCount = BURGER_ARRAYSIZE(SignTestTable64);
	do {
		Int64 iReturn = Burger::Sign(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::NumberStringHex Text1(static_cast<Word64>(pWork->m_iInput));
			Burger::NumberStringHex Text2(static_cast<Word64>(iReturn));
			Burger::NumberStringHex Text3(static_cast<Word64>(pWork->m_iOutput));
			ReportFailure("Burger::Sign((Int64)0x%s) = 0x%s, expected 0x%s",uTest,Text1.GetPtr(),Text2.GetPtr(),Text3.GetPtr());
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Sqrt(Int32)
//

static const WordTest32_t Sqrt32TestTable[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00000001},		// 1^2
	{0x00000010,0x00000004},		// 4^2
	{0x00000020,0x00000006},		// 5.8^2
	{0x40000000,0x00008000},
	{0x7FFFFFFF,0x0000B505},
	{0x80000000,0x0000B505},
	{0x80000001,0x0000B505},
	{0xC0000000,0x0000DDB4},
	{0xFFFC0004,0x0000FFFE},		// 65534^2
	{0xFFFD0002,0x0000FFFE},
	{0xFFFD0003,0x0000FFFF},
	{0xFFFE0001,0x0000FFFF},		// 65535^2
	{0xFFFFFFFF,0x00010000}
};

static Word BURGER_API TestSqrt32(void)
{
	Word uFailure = FALSE;
	const WordTest32_t *pWork = Sqrt32TestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(Sqrt32TestTable);
	do {
		Word32 uReturn = Burger::Sqrt(pWork->m_uInput);
		Word uTest = uReturn!=pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Sqrt((Word32)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_uInput,uReturn,pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test SqrtFixedToWord32(Fixed32)
//

static const FixedTest32_t SqrtFixedToWord32TestTable[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00000000},		// 1^2
	{0x00000010,0x00000000},		// 4^2
	{0x00000020,0x00000000},		// 5.8^2
	{0x40000000,0x00000080},
	{0x7FFFFFFF,0x000000B5},		// Maximum square root (65535.999999)
	{static_cast<Int32>(0x80000000),0x00000000},		// Fail all negative numbers
	{static_cast<Int32>(0x80000001),0x00000000},
	{static_cast<Int32>(0xC0000000),0x00000000},
	{static_cast<Int32>(0xFFFC0004),0x00000000},
	{static_cast<Int32>(0xFFFD0002),0x00000000},
	{static_cast<Int32>(0xFFFD0003),0x00000000},
	{static_cast<Int32>(0xFFFE0001),0x00000000},
	{static_cast<Int32>(0xFFFFFFFF),0x00000000}
};

static Word BURGER_API TestSqrtFixedToWord32(void)
{
	Word uFailure = FALSE;
	const FixedTest32_t *pWork = SqrtFixedToWord32TestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(SqrtFixedToWord32TestTable);
	do {
		Word32 uReturn = Burger::SqrtFixedToWord32(pWork->m_iInput);
		Word uTest = uReturn!=static_cast<Word32>(pWork->m_iOutput);
		uFailure |= uTest;
		ReportFailure("Burger::SqrtFixedToWord32((Fixed32)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,uReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test Sqrt(Fixed32)
//

static const FixedTest32_t SqrtFixed32TestTable[] = {
	{0x00000000,0x00000000},
	{0x00000001,0x00000100},		// 1^2
	{0x00000010,0x00000400},		// 4^2
	{0x00000020,0x000005A8},		// 5.8^2
	{0x00010000,0x00010000},
	{0x00030000,0x0001BB68},		// 3 -> 1.732050807568877f
	{0x00100000,0x00040000},
	{0x017A8E56,0x001374DE},		// 378.556f -> 19.45652f
	{0x40000000,0x00800000},
	{0x7FFFFFFF,0x00B5043E},		// Maximum square root (65535.999999)
	{static_cast<Int32>(0x80000000),0x00000000},		// Fail all negative numbers
	{static_cast<Int32>(0x80000001),0x00000000},
	{static_cast<Int32>(0xC0000000),0x00000000},
	{static_cast<Int32>(0xFFFC0004),0x00000000},
	{static_cast<Int32>(0xFFFD0002),0x00000000},
	{static_cast<Int32>(0xFFFD0003),0x00000000},
	{static_cast<Int32>(0xFFFE0001),0x00000000},
	{static_cast<Int32>(0xFFFFFFFF),0x00000000}
};

static Word BURGER_API TestSqrtFixed32(void)
{
	Word uFailure = FALSE;
	const FixedTest32_t *pWork = SqrtFixed32TestTable;
	WordPtr uCount = BURGER_ARRAYSIZE(SqrtFixed32TestTable);
	do {
		Fixed32 iReturn = Burger::Sqrt(pWork->m_iInput);
		Word uTest = iReturn!=pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure("Burger::Sqrt((Fixed32)0x%08X) = 0x%08X, expected 0x%08X",uTest,pWork->m_iInput,iReturn,pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test fixed point math

***************************************/

int BURGER_API TestBrfixedpoint(Word uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Integer Math tests");
	}

	Word uResult = TestPowerOf2_32();
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
