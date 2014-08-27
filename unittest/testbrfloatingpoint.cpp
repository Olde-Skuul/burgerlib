/***************************************

	Unit tests for the Integer Math library

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfloatingpoint.h"
#include "common.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"

typedef struct FloatInt_t {
	float m_fValue;
	int m_iValue;
} FloatInt_t;

static const Word64 DoubleInf = 0x7FF0000000000000ULL;		///< Constant for Infinity in the double format.
static const Word64 DoubleNan = 0x7FF7FFFFFFFFFFFFULL;		///< Constant for Not a Number (NaN) in the double format
static const Word64 DoubleQNan = 0x7FFFFFFFFFFFFFFFULL;		///< Constant for Not a Number (NaN) in the double format
static const Word64 DoubleMin = 0x0010000000000000ULL;		///< Constant for the smallest number (Non-zero) in the double format.
static const Word64 DoubleMax = 0x7FEFFFFFFFFFFFFFULL;		///< Constant for the largest number in the double format.
static const Word64 DoubleEpsilon = 0x3CB0000000000000ULL;	///< Constant for Epsilon in the double format.
static const Word64 DoublePi = 0x400921FB54442D18ULL;
static const Word32 SingleInf = 0x7F800000U;		///< Constant for Infinity in the float format.
static const Word32 SingleNan = 0x7FBFFFFFU;		///< Constant for Not a Number (NaN) in the float format.
static const Word32 SingleQNan = 0x7FFFFFFFU;		///< Constant for Not a Number (NaN) in the float format.
static const Word32 SingleMin = 0x00800000U;		///< Constant for smallest number (Non-zero) in the float format.
static const Word32 SingleMax = 0x7F7FFFFFU;		///< Constant for largest number in the float format.
static const Word32 SingleEpsilon = 0x34000000U;	///< Constant for Epsilon in the float format.
static const Word32 SinglePi = 0x40490FDBU; 

//
// Test all the floating point constants
// Perform this test first, since other tests
// rely on these tests passing.
//

static Word TestFPConsts(void)
{
	// Test all the single values
	
	float fExpected = reinterpret_cast<const float *>(&::SingleInf)[0];
	float fTest = Burger::g_fInf;
	Word uFailure = (fTest!=fExpected);
	Word uResult = uFailure;
	ReportFailure("Burger::SingleInf = %g / Wanted %g",uFailure,fTest,fExpected);

	fExpected = reinterpret_cast<const float *>(&::SingleMin)[0];
	fTest = Burger::g_fMin;
	uFailure = (fTest!=fExpected);
	uResult |= uFailure;
	ReportFailure("Burger::SingleMin = %g / Wanted %g",uFailure,fTest,fExpected);

	fExpected = reinterpret_cast<const float *>(&::SingleMax)[0];
	fTest = Burger::g_fMax;
	uFailure = (fTest!=fExpected);
	uResult |= uFailure;
	ReportFailure("Burger::SingleMax = %g / Wanted %g",uFailure,fTest,fExpected);

	fExpected = reinterpret_cast<const float *>(&::SingleEpsilon)[0];
	fTest = Burger::g_fEpsilon;
	uFailure = (fTest!=fExpected);
	uResult |= uFailure;
	ReportFailure("Burger::SingleEpsilon = %g / Wanted %g",uFailure,fTest,fExpected);

	fExpected = reinterpret_cast<const float *>(&::SinglePi)[0];
	fTest = Burger::g_fPi;
	uFailure = (fTest!=fExpected);
	uResult |= uFailure;
	ReportFailure("Burger::SinglePi = %g / Wanted %g",uFailure,fTest,fExpected);


	// Test all the double precision values
	
	double dExpected = reinterpret_cast<const double *>(&::DoubleInf)[0];
	double dTest = Burger::g_dInf;
	uFailure = (dTest!=dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dInf = %g / Wanted %g",uFailure,dTest,dExpected);

	dExpected = reinterpret_cast<const double *>(&::DoubleMin)[0];
	dTest = Burger::g_dMin;
	uFailure = (dTest!=dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dMin = %g / Wanted %g",uFailure,dTest,dExpected);

	dExpected = reinterpret_cast<const double *>(&::DoubleMax)[0];
	dTest = Burger::g_dMax;
	uFailure = (dTest!=dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dMax = %g / Wanted %g",uFailure,dTest,dExpected);

	dExpected = reinterpret_cast<const double *>(&::DoubleEpsilon)[0];
	dTest = Burger::g_dEpsilon;
	uFailure = (dTest!=dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dEpsilon = %g / Wanted %g",uFailure,dTest,dExpected);

	dExpected = reinterpret_cast<const double *>(&::DoublePi)[0];
	dTest = Burger::g_dPi;
	uFailure = (dTest!=dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dPi = %g / Wanted %g",uFailure,dTest,dExpected);


	//
	// Detecting NaN is a different case. NaN's can't equal anything, so I
	// have to use the integer unit to test for equality
	// Don't use float != float, some compilers will return equal when that's against the
	// standard!
	//

	Word32 wfFloat = Burger::g_fNan.w;
	uFailure = (wfFloat!=::SingleNan);
	uResult |= uFailure;
	ReportFailure("Burger::SingleNan = %g / Wanted %g",uFailure,Burger::g_fNan,reinterpret_cast<const float *>(&::SingleNan)[0]);

	wfFloat = Burger::g_fQNan.w;
	uFailure = (wfFloat!=::SingleQNan);
	uResult |= uFailure;
	ReportFailure("Burger::SingleQNan = %g / Wanted %g",uFailure,Burger::g_fQNan,reinterpret_cast<const float *>(&::SingleQNan)[0]);

	Word64 wdFloat = Burger::g_dNan.w;
	uFailure = (wdFloat!=::DoubleNan);
	uResult |= uFailure;
	ReportFailure("Burger::g_dNan() = %g / Wanted %g",uFailure,Burger::g_dNan,reinterpret_cast<const double *>(&::DoubleNan)[0]);

	wdFloat = Burger::g_dQNan.w;
	uFailure = (wdFloat!=::DoubleQNan);
	uResult |= uFailure;
	ReportFailure("Burger::g_dQNan() = %g / Wanted %g",uFailure,Burger::g_dQNan,reinterpret_cast<const double *>(&::DoubleQNan)[0]);

	return uResult;
}

//
// Test IsNan(float)
//

#define NANTEST 0x01		// NaN
#define INFTEST 0x02		// Infinity
#define FINITETEST 0x04		// Finite number
#define NORMALTEST 0x08		// Normal number
#define SIGNTEST 0x10		// Negative number

static const Word32 IsNanTest[][2] = {
	{0x00000000U,FINITETEST},						// 0.0f
	{0x00000001U,FINITETEST},						// Lowest denormal
	{0x007FFFFFU,FINITETEST},						// Highest denormal
	{0x00800000U,FINITETEST+NORMALTEST},			// Min
	{0x34000000U,FINITETEST+NORMALTEST},			// Epsilon
	{0x3F800000U,FINITETEST+NORMALTEST},			// 1.0f
	{0x7F7FFFFFU,FINITETEST+NORMALTEST},			// Max
	{0x7F800000U,INFTEST},							// Inf
	{0x7F800001U,NANTEST},							// Nan
	{0x7FBFFFFFU,NANTEST},							// Nan
	{0x7FC00000U,NANTEST},							// QNan	
	{0x7FFFFFFFU,NANTEST},							// QNan	
	{0x80000000U,FINITETEST+SIGNTEST},				// -0.0f
	{0x80000001U,FINITETEST+SIGNTEST},				// Lowest denormal
	{0x807FFFFFU,FINITETEST+SIGNTEST},				// Highest denormal
	{0x80800000U,FINITETEST+NORMALTEST+SIGNTEST},	// Min
	{0xB4000000U,FINITETEST+NORMALTEST+SIGNTEST},	// Epsilon
	{0xBF800000U,FINITETEST+NORMALTEST+SIGNTEST},	// 1.0f
	{0xFF7FFFFFU,FINITETEST+NORMALTEST+SIGNTEST},	// Max
	{0xFF800000U,INFTEST+SIGNTEST},					// Inf
	{0xFF800001U,NANTEST+SIGNTEST},					// Nan
	{0xFFBFFFFFU,NANTEST+SIGNTEST},					// Nan
	{0xFFC00000U,NANTEST+SIGNTEST},					// QNan	
	{0xFFFFFFFFU,NANTEST+SIGNTEST}					// QNan	
};

static Word TestIsNanFloat(void)
{
	const Word32 *pWork = IsNanTest[0];
	WordPtr i = BURGER_ARRAYSIZE(IsNanTest);
	Word uResult = FALSE;
	do {
		float fOriginal = reinterpret_cast<const float *>(pWork)[0];
		Word uExpected = (pWork[1]&NANTEST)!=0;
		pWork+=2;
		Word uTest = Burger::IsNan(fOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsNan((float)%.16g) = %u / Wanted %u",uFailure,fOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsNan(double)
//

struct NanTest_t {
	Word64 uInput;
	Word32 uFlags;
};

static const NanTest_t IsNanTestDouble[] = {
	{0x0000000000000000ULL,FINITETEST},						// 0.0f
	{0x0000000000000001ULL,FINITETEST},						// Lowest denormal
	{0x000FFFFFFFFFFFFFULL,FINITETEST},						// Highest denormal
	{0x0010000000000000ULL,FINITETEST+NORMALTEST},			// Min
	{0x3CB0000000000000ULL,FINITETEST+NORMALTEST},			// Epsilon
	{0x3FF0000000000000ULL,FINITETEST+NORMALTEST},			// 1.0f
	{0x7FEFFFFFFFFFFFFFULL,FINITETEST+NORMALTEST},			// Max
	{0x7FF0000000000000ULL,INFTEST},						// Inf
	{0x7FF0000000000001ULL,NANTEST},						// Nan
	{0x7FF7FFFFFFFFFFFFULL,NANTEST},						// Nan
	{0x7FF8000000000000ULL,NANTEST},						// QNan	
	{0x7FFFFFFFFFFFFFFFULL,NANTEST},						// QNan	
	{0x8000000000000000ULL,FINITETEST+SIGNTEST},			// -0.0f
	{0x8000000000000001ULL,FINITETEST+SIGNTEST},			// Lowest denormal
	{0x800FFFFFFFFFFFFFULL,FINITETEST+SIGNTEST},			// Highest denormal
	{0x8010000000000000ULL,FINITETEST+NORMALTEST+SIGNTEST},	// Min
	{0xBCB0000000000000ULL,FINITETEST+NORMALTEST+SIGNTEST},	// Epsilon
	{0xBFF0000000000000ULL,FINITETEST+NORMALTEST+SIGNTEST},	// 1.0f
	{0xFFEFFFFFFFFFFFFFULL,FINITETEST+NORMALTEST+SIGNTEST},	// Max
	{0xFFF0000000000000ULL,INFTEST+SIGNTEST},				// Inf
	{0xFFF0000000000001ULL,NANTEST+SIGNTEST},				// Nan
	{0xFFF7FFFFFFFFFFFFULL,NANTEST+SIGNTEST},				// Nan
	{0xFFF8000000000000ULL,NANTEST+SIGNTEST},				// QNan	
	{0xFFFFFFFFFFFFFFFFULL,NANTEST+SIGNTEST}				// QNan	
};

static Word TestIsNanDouble(void)
{
	const NanTest_t *pWork = IsNanTestDouble;
	WordPtr i = BURGER_ARRAYSIZE(IsNanTestDouble);
	Word uResult = FALSE;
	do {
		double dOriginal = reinterpret_cast<const double *>(&pWork->uInput)[0];
		Word uExpected = (pWork->uFlags&NANTEST)!=0;
		++pWork;
		Word uTest = Burger::IsNan(dOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsNan((double)%.16g) = %u / Wanted %u",uFailure,dOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsInf(float)
//

static Word TestIsInfFloat(void)
{
	const Word32 *pWork = IsNanTest[0];
	WordPtr i = BURGER_ARRAYSIZE(IsNanTest);
	Word uResult = FALSE;
	do {
		float fOriginal = reinterpret_cast<const float *>(pWork)[0];
		Word uExpected = (pWork[1]&INFTEST)!=0;
		pWork+=2;
		Word uTest = Burger::IsInf(fOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsInf((float)%.16g) = %u / Wanted %u",uFailure,fOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsInf(double)
//

static Word TestIsInfDouble(void)
{
	const NanTest_t *pWork = IsNanTestDouble;
	WordPtr i = BURGER_ARRAYSIZE(IsNanTestDouble);
	Word uResult = FALSE;
	do {
		double dOriginal = reinterpret_cast<const double *>(&pWork->uInput)[0];
		Word uExpected = (pWork->uFlags&INFTEST)!=0;
		++pWork;
		Word uTest = Burger::IsInf(dOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsInf((double)%.16g) = %u / Wanted %u",uFailure,dOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsFinite(float)
//

static Word TestIsFiniteFloat(void)
{
	const Word32 *pWork = IsNanTest[0];
	WordPtr i = BURGER_ARRAYSIZE(IsNanTest);
	Word uResult = FALSE;
	do {
		float fOriginal = reinterpret_cast<const float *>(pWork)[0];
		Word uExpected = (pWork[1]&FINITETEST)!=0;
		pWork+=2;
		Word uTest = Burger::IsFinite(fOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsFinite((float)%.16g) = %u / Wanted %u",uFailure,fOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsFinite(double)
//

static Word TestIsFiniteDouble(void)
{
	const NanTest_t *pWork = IsNanTestDouble;
	WordPtr i = BURGER_ARRAYSIZE(IsNanTestDouble);
	Word uResult = FALSE;
	do {
		double dOriginal = reinterpret_cast<const double *>(&pWork->uInput)[0];
		Word uExpected = (pWork->uFlags&FINITETEST)!=0;
		++pWork;
		Word uTest = Burger::IsFinite(dOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsFinite((double)%.16g) = %u / Wanted %u",uFailure,dOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsNormal(float)
//

static Word TestIsNormalFloat(void)
{
	const Word32 *pWork = IsNanTest[0];
	WordPtr i = BURGER_ARRAYSIZE(IsNanTest);
	Word uResult = FALSE;
	do {
		float fOriginal = reinterpret_cast<const float *>(pWork)[0];
		Word uExpected = (pWork[1]&NORMALTEST)!=0;
		pWork+=2;
		Word uTest = Burger::IsNormal(fOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsNormal((float)%.16g) = %u / Wanted %u",uFailure,fOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test IsNormal(double)
//

static Word TestIsNormalDouble(void)
{
	const NanTest_t *pWork = IsNanTestDouble;
	WordPtr i = BURGER_ARRAYSIZE(IsNanTestDouble);
	Word uResult = FALSE;
	do {
		double dOriginal = reinterpret_cast<const double *>(&pWork->uInput)[0];
		Word uExpected = (pWork->uFlags&NORMALTEST)!=0;
		++pWork;
		Word uTest = Burger::IsNormal(dOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::IsNormal((double)%.16g) = %u / Wanted %u",uFailure,dOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test SignBit(float)
//

static Word TestSignBitFloat(void)
{
	const Word32 *pWork = IsNanTest[0];
	WordPtr i = BURGER_ARRAYSIZE(IsNanTest);
	Word uResult = FALSE;
	do {
		float fOriginal = reinterpret_cast<const float *>(pWork)[0];
		Word uExpected = (pWork[1]&SIGNTEST)!=0;
		pWork+=2;
		Word uTest = Burger::SignBit(fOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::SignBit((float)%.16g) = %u / Wanted %u",uFailure,fOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test SignBit(double)
//

static Word TestSignBitDouble(void)
{
	const NanTest_t *pWork = IsNanTestDouble;
	WordPtr i = BURGER_ARRAYSIZE(IsNanTestDouble);
	Word uResult = FALSE;
	do {
		double dOriginal = reinterpret_cast<const double *>(&pWork->uInput)[0];
		Word uExpected = (pWork->uFlags&SIGNTEST)!=0;
		++pWork;
		Word uTest = Burger::SignBit(dOriginal);
		Word uFailure = (uTest!=uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::SignBit((double)%.16g) = %u / Wanted %u",uFailure,dOriginal,uTest,uExpected);
	} while (--i);
	return uResult;
}

//
// Test Abs(float)
//

static const Burger::Word32ToFloat AbsFloatArray[][2] = {
	{{{0x00000000U}},{{0x00000000U}}},	// 0.0f,0.0f
	{{{0x80000000U}},{{0x00000000U}}},	// -0.0f,0.0f
	{{{0x3F800000U}},{{0x3F800000U}}},	// 1.0f,1.0f
	{{{0xBF800000U}},{{0x3F800000U}}},	// -1.0f,1.0f
	{{{0x40490CDDU}},{{0x40490CDDU}}},	// 3.14141f,3.14141f
	{{{0xC0490CDDU}},{{0x40490CDDU}}},	// -3.14141f,3.14141f
	{{{0x4423C000U}},{{0x4423C000U}}},	// 655.0f,655.0f
	{{{0xC423C000U}},{{0x4423C000U}}},	// -655.0f,655.0f
	{{{0x4640E400U}},{{0x4640E400U}}},	// 12345.0f,12345.0f
	{{{0xC640E400U}},{{0x4640E400U}}},	// -12345.0f,12345.0f
	{{{0x7F800000U}},{{0x7F800000U}}},	// Burger::SingleInf,Burger::SingleInf
	{{{0xFF800000U}},{{0x7F800000U}}}	// -Burger::SingleInf,Burger::SingleInf
//	{{{0x7F800001U}},{{0x7F800001U}}},	// Burger::SingleNaN,Burger::SingleQaN
//	{{{0xFF800001U}},{{0x7F800001U}}},	// -Burger::SingleNaN,Burger::SingleQaN
//	{{{0x7FC00000U}},{{0x7FC00000U}}},	// Burger::SingleQaN,Burger::SingleQaN
//	{{{0xFFC00001U}},{{0x7FC00001U}}}	// -Burger::SingleQNaN,Burger::SingleQaN
};

static Word TestAbsFloat(void)
{
	const Burger::Word32ToFloat *pWork = AbsFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(AbsFloatArray);
	Word uResult = FALSE;
	do {
		Burger::Word32ToFloat fTest;
		fTest.f = Burger::Abs(pWork[0]);
		Word uFailure = (fTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Abs(float) = %g 0x%s / Wanted %g 0x%s",uFailure,fTest.f,Test.GetPtr(),pWork[1].f,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Abs(double)
//

static const Burger::Word64ToDouble AbsDoubleArray[][2] = {
	{{{0x0000000000000000ULL}},{{0x0000000000000000ULL}}},	// 0.0,0.0
	{{{0x8000000000000000ULL}},{{0x0000000000000000ULL}}},	// -0.0,0.0
	{{{0x3FF0000000000000ULL}},{{0x3FF0000000000000ULL}}},	// 1.0,1.0
	{{{0xBFF0000000000000ULL}},{{0x3FF0000000000000ULL}}},	// -1.0,1.0
	{{{0x4009219B90EA9E6FULL}},{{0x4009219B90EA9E6FULL}}},	// 3.14141,3.14141
	{{{0xC009219B90EA9E6FULL}},{{0x4009219B90EA9E6FULL}}},	// -3.14141,3.14141
	{{{0x4084780000000000ULL}},{{0x4084780000000000ULL}}},	// 655.0,655.0
	{{{0xC084780000000000ULL}},{{0x4084780000000000ULL}}},	// -655.0,655.0
	{{{0x40C81C8000000000ULL}},{{0x40C81C8000000000ULL}}},	// 12345.0,12345.0
	{{{0xC0C81C8000000000ULL}},{{0x40C81C8000000000ULL}}},	// -12345.0,12345.0
	{{{0x7FF0000000000000ULL}},{{0x7FF0000000000000ULL}}},	// Burger::DoubleInf,Burger::DoubleInf
	{{{0xFFF0000000000000ULL}},{{0x7FF0000000000000ULL}}}	// -Burger::DoubleInf,Burger::DoubleInf
};

static Word TestAbsDouble(void)
{
	const Burger::Word64ToDouble *pWork = AbsDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(AbsDoubleArray);
	Word uResult = FALSE;
	do {
		double dTest = pWork[0];
		double dExpected = pWork[1];
		pWork+=2;
		dTest = Burger::Abs(dTest);
		Word uFailure = (dTest!=dExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Abs(double) = %g / Wanted %g",uFailure,dTest,dExpected);
	} while (--i);
	return uResult;
}

//
// Test Sqrt(float)
//

static const Burger::Word32ToFloat SqrtFloatArray[][2] = {
	{{{0x40800000U}},{{0x40000000U}}},	// 4.0f,2.0f
	{{{0x40000000U}},{{0x3FB504F3U}}},	// 2.0f,1.4142135381698608f
	{{{0x3F800000U}},{{0x3F800000U}}},	// 1.0f,1.0f
	{{{0x3F733333U}},{{0x3F798497U}}},	// 0.95f,0.9746794f
	{{{0x00000000U}},{{0x00000000U}}},	// 0.0f,0.0f
	{{{0x3E800000U}},{{0x3F000000U}}},	// 0.25f,0.5f
	{{{0x3E028F5CU}},{{0x3EB6D211U}}},	// 0.1275f,0.3570714294910431f
	{{{0x41C00000U}},{{0x409CC471U}}},	// 24.0f,4.898979663848877f
	{{{0x4019999AU}},{{0x3FC64BF8U}}},	// 2.4f,1.549193382263184f
	{{{0x43BD472BU}},{{0x419BA6F2U}}}	// 378.556f,19.45651626586914f
//	{{{0xBF800000U}},{{0xFFC00000U}}}	// -1.0f,1.0f
};

static Word TestSqrtFloat(void)
{
	const Burger::Word32ToFloat *pWork = SqrtFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(SqrtFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		volatile float fTest = Burger::Sqrt(fOriginal);
		float fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Sqrt(%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Floor(float)
//

static const Burger::Word32ToFloat FloorFloatArray[][2] = {
	{{{0x3F8CCCCDU}},{{0x3F800000U}}},	// 1.1f,1.0f
	{{{0x3FF9999AU}},{{0x3F800000U}}},	// 1.95f,1.0f
	{{{0xBF8CCCCDU}},{{0xC0000000U}}},	// -1.1f,-2.0f
	{{{0xBFF9999AU}},{{0xC0000000U}}},	// -1.95f,-2.0f
	{{{0x3DCCCCCDU}},{{0x00000000U}}},	// 0.1f,0.0f
	{{{0x3F733333U}},{{0x00000000U}}},	// 0.95f,0.0f
	{{{0xBDCCCCCDU}},{{0xBF800000U}}},	// -0.1f,-1.0f
	{{{0xBF7333BDU}},{{0xBF800000U}}},	// -0.95f,-1.0f
	{{{0x4B000000U}},{{0x4B000000U}}},	// 8388608.0f,8388608.0f
	{{{0xCB000000U}},{{0xCB000000U}}},	// -8388608.0f,-8388608.0f
	{{{0x4CA00000U}},{{0x4CA00000U}}},	// 83886080.0f,83886080.0f
	{{{0xCCA00000U}},{{0xCCA00000U}}},	// -83886080.0f,-83886080.0f
	{{{0x4E480000U}},{{0x4E480000U}}},	// 838860800.0f,838860800.0f
	{{{0xCE480000U}},{{0xCE480000U}}},	// -838860800.0f,-838860800.0f
	{{{0x4F000000U}},{{0x4F000000U}}},	// 0x7FFFFFF
	{{{0x4EFFFFFFU}},{{0x4EFFFFFFU}}},	// 0x7FFFFFF-Epsilon
	{{{0xCF000000U}},{{0xCF000000U}}}	// 0x8000000
};

static Word TestFloorFloat(void)
{
	const Burger::Word32ToFloat *pWork = FloorFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(FloorFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		volatile float fTest = Burger::Floor(fOriginal);
		float fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Floor((float)%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Floor(double)
//

static const Burger::Word64ToDouble FloorDoubleArray[][2] = {
	{{{0x3FF199999999999AULL}},{{0x3FF0000000000000ULL}}},	// 1.1f,1.0f
	{{{0x3FFF333333333333ULL}},{{0x3FF0000000000000ULL}}},	// 1.95f,1.0f
	{{{0xBFF199999999999AULL}},{{0xC000000000000000ULL}}},	// -1.1f,-2.0f
	{{{0xBFFF333333333333ULL}},{{0xC000000000000000ULL}}},	// -1.95f,-2.0f
	{{{0x3FB999999999999AULL}},{{0x0000000000000000ULL}}},	// 0.1f,0.0f
	{{{0x3FEE666666666666ULL}},{{0x0000000000000000ULL}}},	// 0.95f,0.0f
	{{{0xBFB999999999999AULL}},{{0xBFF0000000000000ULL}}},	// -0.1f,-1.0f
	{{{0xBFEE666666666666ULL}},{{0xBFF0000000000000ULL}}},	// -0.95f,-1.0f
	{{{0x4330000000000000ULL}},{{0x4330000000000000ULL}}},	// 4503599627370496.0f,4503599627370496.0f
	{{{0xC330000000000000ULL}},{{0xC330000000000000ULL}}},	// -4503599627370496.0f,-4503599627370496.0f
	{{{0x4364000000000000ULL}},{{0x4364000000000000ULL}}},	// 45035996273704960.0f,45035996273704960.0f
	{{{0xC364000000000000ULL}},{{0xC364000000000000ULL}}},	// -45035996273704960.0f,-45035996273704960.0f
	{{{0x4399000000000000ULL}},{{0x4399000000000000ULL}}},	// 450359962737049600.0f,450359962737049600.0f
	{{{0xC399000000000000ULL}},{{0xC399000000000000ULL}}},	// -450359962737049600.0f,-450359962737049600.0f
	{{{0x43E0000000000000ULL}},{{0x43E0000000000000ULL}}},	// 0x7FFFFFFFFFFFFFF
	{{{0x43DFFFFFFFFFFFFFULL}},{{0x43DFFFFFFFFFFFFFULL}}},	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{{0xC3E0000000000000ULL}},{{0xC3E0000000000000ULL}}}	// 0x800000000000000
};

static Word TestFloorDouble(void)
{
	const Burger::Word64ToDouble *pWork = FloorDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(FloorDoubleArray);
	Word uResult = FALSE;
	do {
		double fOriginal = pWork[0];
		double fTest = Burger::Floor(fOriginal);
		double fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Floor((double)%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Ceil(float)
//

static const Burger::Word32ToFloat CeilFloatArray[][2] = {
	{{{0x3F8CCCCDU}},{{0x40000000U}}},	// 1.1f,2.0f
	{{{0x3FF9999AU}},{{0x40000000U}}},	// 1.95f,2.0f
	{{{0xBF8CCCCDU}},{{0xBF800000U}}},	// -1.1f,-1.0f
	{{{0xBFF9999AU}},{{0xBF800000U}}},	// -1.95f,-1.0f
	{{{0x3DCCCCCDU}},{{0x3F800000U}}},	// 0.1f,1.0f
	{{{0x3F733333U}},{{0x3F800000U}}},	// 0.95f,1.0f
	{{{0xBDCCCCCDU}},{{0x00000000U}}},	// -0.1f,0.0f
	{{{0xBF7333BDU}},{{0x00000000U}}},	// -0.95f,0.0f
	{{{0x4B000000U}},{{0x4B000000U}}},	// 8388608.0f,8388608.0f
	{{{0xCB000000U}},{{0xCB000000U}}},	// -8388608.0f,-8388608.0f
	{{{0x4CA00000U}},{{0x4CA00000U}}},	// 83886080.0f,83886080.0f
	{{{0xCCA00000U}},{{0xCCA00000U}}},	// -83886080.0f,-83886080.0f
	{{{0x4E480000U}},{{0x4E480000U}}},	// 838860800.0f,838860800.0f
	{{{0xCE480000U}},{{0xCE480000U}}},	// -838860800.0f,-838860800.0f
	{{{0x4F000000U}},{{0x4F000000U}}},	// 0x7FFFFFF
	{{{0x4EFFFFFFU}},{{0x4EFFFFFFU}}},	// 0x7FFFFFF-Epsilon
	{{{0xCF000000U}},{{0xCF000000U}}}	// 0x8000000
};

static Word TestCeilFloat(void)
{
	const Burger::Word32ToFloat *pWork = CeilFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(CeilFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		volatile float fTest = Burger::Ceil(fOriginal);
		float fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Ceil(%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Ceil(double)
//

static const Burger::Word64ToDouble CeilDoubleArray[][2] = {
	{{{0x3FF199999999999AULL}},{{0x4000000000000000ULL}}},	// 1.1f,2.0f
	{{{0x3FFF333333333333ULL}},{{0x4000000000000000ULL}}},	// 1.95f,2.0f
	{{{0xBFF199999999999AULL}},{{0xBFF0000000000000ULL}}},	// -1.1f,-1.0f
	{{{0xBFFF333333333333ULL}},{{0xBFF0000000000000ULL}}},	// -1.95f,-1.0f
	{{{0x3FB999999999999AULL}},{{0x3FF0000000000000ULL}}},	// 0.1f,1.0f
	{{{0x3FEE666666666666ULL}},{{0x3FF0000000000000ULL}}},	// 0.95f,1.0f
	{{{0xBFB999999999999AULL}},{{0x0000000000000000ULL}}},	// -0.1f,0.0f
	{{{0xBFEE666666666666ULL}},{{0x0000000000000000ULL}}},	// -0.95f,0.0f
	{{{0x4330000000000000ULL}},{{0x4330000000000000ULL}}},	// 4503599627370496.0f,4503599627370496.0f
	{{{0xC330000000000000ULL}},{{0xC330000000000000ULL}}},	// -4503599627370496.0f,-4503599627370496.0f
	{{{0x4364000000000000ULL}},{{0x4364000000000000ULL}}},	// 45035996273704960.0f,45035996273704960.0f
	{{{0xC364000000000000ULL}},{{0xC364000000000000ULL}}},	// -45035996273704960.0f,-45035996273704960.0f
	{{{0x4399000000000000ULL}},{{0x4399000000000000ULL}}},	// 450359962737049600.0f,450359962737049600.0f
	{{{0xC399000000000000ULL}},{{0xC399000000000000ULL}}},	// -450359962737049600.0f,-450359962737049600.0f
	{{{0x43E0000000000000ULL}},{{0x43E0000000000000ULL}}},	// 0x7FFFFFFFFFFFFFF
	{{{0x43DFFFFFFFFFFFFFULL}},{{0x43DFFFFFFFFFFFFFULL}}},	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{{0xC3E0000000000000ULL}},{{0xC3E0000000000000ULL}}}	// 0x800000000000000
};

static Word TestCeilDouble(void)
{
	const Burger::Word64ToDouble *pWork = CeilDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(CeilDoubleArray);
	Word uResult = FALSE;
	do {
		double fOriginal = pWork[0];
		double fTest = Burger::Ceil(fOriginal);
		double fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Ceil((double)%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Round(float)
//

static const Burger::Word32ToFloat RoundFloatArray[][2] = {
	{{{0x3F8CCCCDU}},{{0x3F800000U}}},	// 1.1f,1.0f
	{{{0x3FF9999AU}},{{0x40000000U}}},	// 1.95f,2.0f
	{{{0xBF8CCCCDU}},{{0xBF800000U}}},	// -1.1f,-1.0f
	{{{0xBFF9999AU}},{{0xC0000000U}}},	// -1.95f,-2.0f
	{{{0x3DCCCCCDU}},{{0x00000000U}}},	// 0.1f,0.0f
	{{{0x3F733333U}},{{0x3F800000U}}},	// 0.95f,1.0f
	{{{0xBDCCCCCDU}},{{0x00000000U}}},	// -0.1f,0.0f
	{{{0xBF7333BDU}},{{0xBF800000U}}},	// -0.95f,-1.0f
	{{{0x4B000000U}},{{0x4B000000U}}},	// 8388608.0f,8388608.0f
	{{{0xCB000000U}},{{0xCB000000U}}},	// -8388608.0f,-8388608.0f
	{{{0x4CA00000U}},{{0x4CA00000U}}},	// 83886080.0f,83886080.0f
	{{{0xCCA00000U}},{{0xCCA00000U}}},	// -83886080.0f,-83886080.0f
	{{{0x4E480000U}},{{0x4E480000U}}},	// 838860800.0f,838860800.0f
	{{{0xCE480000U}},{{0xCE480000U}}},	// -838860800.0f,-838860800.0f
	{{{0x4F000000U}},{{0x4F000000U}}},	// 0x7FFFFFF
	{{{0x4EFFFFFFU}},{{0x4EFFFFFFU}}},	// 0x7FFFFFF-Epsilon
	{{{0xCF000000U}},{{0xCF000000U}}}	// 0x8000000
};

static Word TestRoundFloat(void)
{
	const Burger::Word32ToFloat *pWork = RoundFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(RoundFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		volatile float fTest = Burger::Round(fOriginal);
		float fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::Round((float)%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Round(double)
//

static const Burger::Word64ToDouble RoundDoubleArray[][2] = {
	{{{0x3FF199999999999AULL}},{{0x3FF0000000000000ULL}}},	// 1.1f,1.0f
	{{{0x3FFF333333333333ULL}},{{0x4000000000000000ULL}}},	// 1.95f,2.0f
	{{{0xBFF199999999999AULL}},{{0xBFF0000000000000ULL}}},	// -1.1f,-1.0f
	{{{0xBFFF333333333333ULL}},{{0xC000000000000000ULL}}},	// -1.95f,-2.0f
	{{{0x3FB999999999999AULL}},{{0x0000000000000000ULL}}},	// 0.1f,0.0f
	{{{0x3FEE666666666666ULL}},{{0x3FF0000000000000ULL}}},	// 0.95f,1.0f
	{{{0xBFB999999999999AULL}},{{0x0000000000000000ULL}}},	// -0.1f,0.0f
	{{{0xBFEE666666666666ULL}},{{0xBFF0000000000000ULL}}},	// -0.95f,-1.0f
	{{{0x4330000000000000ULL}},{{0x4330000000000000ULL}}},	// 4503599627370496.0f,4503599627370496.0f
	{{{0xC330000000000000ULL}},{{0xC330000000000000ULL}}},	// -4503599627370496.0f,-4503599627370496.0f
	{{{0x4364000000000000ULL}},{{0x4364000000000000ULL}}},	// 45035996273704960.0f,45035996273704960.0f
	{{{0xC364000000000000ULL}},{{0xC364000000000000ULL}}},	// -45035996273704960.0f,-45035996273704960.0f
	{{{0x4399000000000000ULL}},{{0x4399000000000000ULL}}},	// 450359962737049600.0f,450359962737049600.0f
	{{{0xC399000000000000ULL}},{{0xC399000000000000ULL}}},	// -450359962737049600.0f,-450359962737049600.0f
	{{{0x43E0000000000000ULL}},{{0x43E0000000000000ULL}}},	// 0x7FFFFFFFFFFFFFF
	{{{0x43DFFFFFFFFFFFFFULL}},{{0x43DFFFFFFFFFFFFFULL}}},	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{{0xC3E0000000000000ULL}},{{0xC3E0000000000000ULL}}}	// 0x800000000000000
};

static Word TestRoundDouble(void)
{
	const Burger::Word64ToDouble *pWork = RoundDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(RoundDoubleArray);
	Word uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float precision
		Burger::Word64ToDouble dTest;
		dTest.d = Burger::Round(pWork[0]);
		Word uFailure = (dTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(dTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Round((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,pWork[0].d,dTest.d,Test.GetPtr(),pWork[1].d,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test RoundToZero(float)
//

static const Burger::Word32ToFloat RoundToZeroFloatArray[][2] = {
	{{{0x3F8CCCCDU}},{{0x3F800000U}}},	// 1.1f,1.0f
	{{{0x3FF9999AU}},{{0x3F800000U}}},	// 1.95f,1.0f
	{{{0xBF8CCCCDU}},{{0xBF800000U}}},	// -1.1f,-1.0f
	{{{0xBFF9999AU}},{{0xBF800000U}}},	// -1.95f,-1.0f
	{{{0x3DCCCCCDU}},{{0x00000000U}}},	// 0.1f,0.0f
	{{{0x3F733333U}},{{0x00000000U}}},	// 0.95f,0.0f
	{{{0xBDCCCCCDU}},{{0x00000000U}}},	// -0.1f,0.0f
	{{{0xBF7333BDU}},{{0x00000000U}}},	// -0.95f,0.0f
	{{{0x4B000000U}},{{0x4B000000U}}},	// 8388608.0f,8388608.0f
	{{{0xCB000000U}},{{0xCB000000U}}},	// -8388608.0f,-8388608.0f
	{{{0x4CA00000U}},{{0x4CA00000U}}},	// 83886080.0f,83886080.0f
	{{{0xCCA00000U}},{{0xCCA00000U}}},	// -83886080.0f,-83886080.0f
	{{{0x4E480000U}},{{0x4E480000U}}},	// 838860800.0f,838860800.0f
	{{{0xCE480000U}},{{0xCE480000U}}},	// -838860800.0f,-838860800.0f
	{{{0x4F000000U}},{{0x4F000000U}}},	// 0x7FFFFFF
	{{{0x4EFFFFFFU}},{{0x4EFFFFFFU}}},	// 0x7FFFFFF-Epsilon
	{{{0xCF000000U}},{{0xCF000000U}}}	// 0x8000000
};

static Word TestRoundToZeroFloat(void)
{
	const Burger::Word32ToFloat *pWork = RoundToZeroFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(RoundToZeroFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		volatile float fTest = Burger::RoundToZero(fOriginal);
		float fExpected = pWork[1];
		Word uFailure = (fTest!=fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::RoundToZero((float)%.16g) = %.16g / Wanted %.16g",uFailure,fOriginal,fTest,fExpected);
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test RoundToNearest(double)
//

static const Burger::Word64ToDouble RoundToZeroDoubleArray[][2] = {
	{{{0x3FF199999999999AULL}},{{0x3FF0000000000000ULL}}},	// 1.1f,1.0f
	{{{0x3FFF333333333333ULL}},{{0x3FF0000000000000ULL}}},	// 1.95f,1.0f
	{{{0xBFF199999999999AULL}},{{0xBFF0000000000000ULL}}},	// -1.1f,-1.0f
	{{{0xBFFF333333333333ULL}},{{0xBFF0000000000000ULL}}},	// -1.95f,-1.0f
	{{{0x3FB999999999999AULL}},{{0x0000000000000000ULL}}},	// 0.1f,0.0f
	{{{0x3FEE666666666666ULL}},{{0x0000000000000000ULL}}},	// 0.95f,0.0f
	{{{0xBFB999999999999AULL}},{{0x0000000000000000ULL}}},	// -0.1f,0.0f
	{{{0xBFEE666666666666ULL}},{{0x0000000000000000ULL}}},	// -0.95f,0.0f
	{{{0x4330000000000000ULL}},{{0x4330000000000000ULL}}},	// 4503599627370496.0f,4503599627370496.0f
	{{{0xC330000000000000ULL}},{{0xC330000000000000ULL}}},	// -4503599627370496.0f,-4503599627370496.0f
	{{{0x4364000000000000ULL}},{{0x4364000000000000ULL}}},	// 45035996273704960.0f,45035996273704960.0f
	{{{0xC364000000000000ULL}},{{0xC364000000000000ULL}}},	// -45035996273704960.0f,-45035996273704960.0f
	{{{0x4399000000000000ULL}},{{0x4399000000000000ULL}}},	// 450359962737049600.0f,450359962737049600.0f
	{{{0xC399000000000000ULL}},{{0xC399000000000000ULL}}},	// -450359962737049600.0f,-450359962737049600.0f
	{{{0x43E0000000000000ULL}},{{0x43E0000000000000ULL}}},	// 0x7FFFFFFFFFFFFFF
	{{{0x43DFFFFFFFFFFFFFULL}},{{0x43DFFFFFFFFFFFFFULL}}},	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{{0xC3E0000000000000ULL}},{{0xC3E0000000000000ULL}}}	// 0x800000000000000
};

static Word TestRoundToZeroDouble(void)
{
	const Burger::Word64ToDouble *pWork = RoundToZeroDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(RoundToZeroDoubleArray);
	Word uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float precision
		Burger::Word64ToDouble dTest;
		dTest.d = Burger::RoundToZero(pWork[0]);
		Word uFailure = (dTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(dTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::RoundToZero((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,pWork[0].d,dTest.d,Test.GetPtr(),pWork[1].d,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test ModuloRadians(float)
//

static const Burger::Word32ToFloat ModuloRadiansFloatArray[][2] = {
	{{{0x40490FDBU}},{{0xC0490FDBU}}},	// Pi,-Pi


	{{{0x3FC90FDBU}},{{0x3FC90FDBU}}},	// .5Pi,.5Pi
	{{{0x40490FDBU}},{{0xC0490FDBU}}},	// Pi,-Pi
	{{{0x4096CBE4U}},{{0xBFC90FDCU}}},	// 1.5Pi,-.5Pif
	{{{0x40C90FDBU}},{{0x00000000U}}},	// 2Pi,0.0f
	{{{0x41490FDBU}},{{0x00000000U}}},	// 4Pi,0.0f
	{{{0x42490FDBU}},{{0x00000000U}}},	// 16Pi,0.0f
	{{{0xBFC90FDBU}},{{0xBFC90FDBU}}},	// -.5Pi,-.5Pi
	{{{0xC0490FDBU}},{{0xC0490FDBU}}},	// -Pi,-Pi
	{{{0xC096CBE4U}},{{0x3FC90FDCU}}},	// -1.5Pi,.5Pi
	{{{0xC0C90FDBU}},{{0x00000000U}}},	// -2Pi,0.0f
	{{{0xC1490FDBU}},{{0x00000000U}}},	// -4Pi,0.0f
	{{{0xC2490FDBU}},{{0x00000000U}}},	// 16Pi,0.0f
	{{{0x3F8CCCCDU}},{{0x3F8CCCCDU}}},	// 1.1f,1.1f
	{{{0x00800000U}},{{0x00800000U}}}	// Min,Min
};

static Word TestModuloRadiansFloat(void)
{
//	Burger::Set8087Precision(Burger::PRECISION24);
	const Burger::Word32ToFloat *pWork = ModuloRadiansFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(ModuloRadiansFloatArray);
	Word uResult = FALSE;
	do {
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		Burger::Word32ToFloat fTest;
		fTest.f = Burger::ModuloRadians(fOriginal);
		//Word uFailure = ((fTest.w^=pWork[1].w)&0x7FFFFFFFU)!=0;
		Word uFailure = (fTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::ModuloRadians((float)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,fOriginal,fTest.f,Test.GetPtr(),pWork[1].f,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test ModuloRadians(double)
//

static const Burger::Word64ToDouble ModuloRadiansDoubleArray[][2] = {
	{{{0x3FF921FB54442D18ULL}},{{0x3FF921FB54442D18ULL}}},	// .5Pi,.5Pi
	{{{0x400921FB54442D18ULL}},{{0xC00921FB54442D18ULL}}},	// Pi,-Pi
	{{{0x4012D97C7F3321D2ULL}},{{0xBFF921FB54442D18ULL}}},	// 1.5Pi,-.5Pif
	{{{0x401921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// 2Pi,0.0f
	{{{0x402921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// 4Pi,0.0f
	{{{0x404921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// 16Pi,0.0f
	{{{0x40A921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// 1024Pi,0.0f
	{{{0xBFF921FB54442D18ULL}},{{0xBFF921FB54442D18ULL}}},	// -.5Pi,-.5Pi
	{{{0xC00921FB54442D18ULL}},{{0xC00921FB54442D18ULL}}},	// -Pi,-Pi
	{{{0xC012D97C7F3321D2ULL}},{{0x3FF921FB54442D18ULL}}},	// -1.5Pi,.5Pi
	{{{0xC02921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// -2Pi,0.0f
	{{{0xC02921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// -4Pi,0.0f
	{{{0xC04921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// -16Pi,0.0f
	{{{0xC0A921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// -1024Pi,0.0f
	{{{0x3FF199999999999AULL}},{{0x3FF199999999999AULL}}},	// 1.1,1.1
	{{{0x0010000000000000ULL}},{{0x0010000000000000ULL}}}	// Min,Min
};

static Word TestModuloRadiansDouble(void)
{
	const Burger::Word64ToDouble *pWork = ModuloRadiansDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(ModuloRadiansDoubleArray);
	Word uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float precision
		Burger::Word64ToDouble dTest;
		dTest.d = Burger::ModuloRadians(pWork[0]);
		//Word uFailure = ((dTest.w^=pWork[1].w)&0x7FFFFFFFFFFFFFFFULL)!=0;
		Word uFailure = (dTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(dTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::ModuloRadians((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,pWork[0].d,dTest.d,Test.GetPtr(),pWork[1].d,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Sin(float)
//

static const Burger::Word32ToFloat SinFloatArray[][2] = {
// Due to precision differences between using true 24 bit floating point
// vs. 80 bit 8087 FPU, use unit test tables based on CPU types
#if defined(BURGER_X86)
	{{{0x3FC90FDBU}},{{0x3F800000U}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0x33FCA5E6U}}},	// PI,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x00000000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x00000000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0x33FCA5E6U}}},	// SingleDegreesToRadians*180.0f,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0xB6C00000U}}},	// SingleDegreesToRadians*36000.0f,-5.7220458984375e-006
	{{{0xC0490FDBU}},{{0x33FCA5E6U}}},	// -PI,1.176484119014276e-007
#elif defined(BURGER_POWERPC)
	{{{0x3FC90FDBU}},{{0x3F7FFFFFU}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0x348BC409U}}},	// PI,1.22461e-16f
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x00000000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x00000000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0x348BC409U}}},	// SingleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0xB6C00000U}}},	// SingleDegreesToRadians*36000.0f,1.17625e-005f
	{{{0xC0490FDBU}},{{0x348BC409U}}},	// -PI,-1.22461e-16f
#else
	{{{0x3FC90FDBU}},{{0x3F7FFFFFU}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0xB3355359U}}},	// PI,1.22461e-16f
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x00000000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x00000000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0xB3355359U}}},	// SingleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4096CBE4U}},{{0xBF800000U}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0x00000000U}}},	// SingleDegreesToRadians*36000.0f,1.17625e-005f
	{{{0xC0490FDBU}},{{0xB3355359U}}},	// -PI,-1.22461e-16f
#endif
	{{{0x00000000U}},{{0x00000000U}}},	// 0.0f,0.0f
	{{{0x00000001U}},{{0x00000001U}}},	// 0.0f,0.0f
	{{{0x00000010U}},{{0x00000010U}}},	// 0.0f,0.0f
	{{{0x00000100U}},{{0x00000100U}}},	// 0.0f,0.0f
	{{{0x00001000U}},{{0x00001000U}}},	// 0.0f,0.0f
	{{{0x00010000U}},{{0x00010000U}}}	// 0.0f,0.0f
//	{{{0x7F800000U}},{{0xFFC00000U}}},	// Inf,Indefinite
//	{{{0xFF800000U}},{{0xFFC00000U}}},	// -Inf,Indefinite
//	{{{0x7F800001U}},{{0x7FC00001U}}},	// NaN,QNaN
//	{{{0x7FC00000U}},{{0x7FC00000U}}},	// NaN,QNaN
//	{{{0x7FC00001U}},{{0x7FC00001U}}},	// NaN,QNaN
//	{{{0x7FFFFFFFU}},{{0x7FFFFFFFU}}}	// NaN,QNaN
};

static Word TestSinFloat(void)
{
	const Burger::Word32ToFloat *pWork = SinFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(SinFloatArray);
	Word uResult = FALSE;
	do {
		Burger::Word32ToFloat fTest;
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		fTest.f = Burger::Sin(fOriginal);
		Word uFailure = (fTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Sin(%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,fOriginal,fTest.f,Test.GetPtr(),pWork[1].f,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Sin(double)
//

static const Burger::Word64ToDouble SinDoubleArray[][2] = {
#if defined(BURGER_POWERPC)
	{{{0x0000000000000000ULL}},{{0x0000000000000000ULL}}},	// 0.0f,0.0f
	{{{0x3FF921FB54442D18ULL}},{{0x3FF0000000000001ULL}}},	// PI/2,1.0f
	{{{0x400921FB54442D18ULL}},{{0x3D47E87817621143ULL}}},	// PI,1.22461e-16f
	{{{0x4012D97C7F3321D2ULL}},{{0xBFF0000000000001ULL}}},	// PI*1.5f,-1.0f
	{{{0x401921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// PI*2,-2.44921e-16f
	{{{0x402921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// PI*4,-4.89843e-16f
	{{{0x400921FB54442D18ULL}},{{0x3D47E87817621143ULL}}},	// DoubleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4012D97C7F3321D2ULL}},{{0xBFF0000000000001ULL}}},	// DoubleDegreesToRadians*270.0f,-1.0f
	{{{0x4083A28C59D5433BULL}},{{0x3D20000000000000ULL}}}	// DoubleDegreesToRadians*36000.0f,1.17625e-005f
#else
	{{{0x0000000000000000ULL}},{{0x0000000000000000ULL}}},	// 0.0f,0.0f
	{{{0x3FF921FB54442D18ULL}},{{0x3FF0000000000001ULL}}},	// PI/2,1.0f
	{{{0x400921FB54442D18ULL}},{{0x3D47F640F23BF080ULL}}},	// PI,1.22461e-16f
	{{{0x4012D97C7F3321D2ULL}},{{0xBFF0000000000001ULL}}},	// PI*1.5f,-1.0f
	{{{0x401921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// PI*2,-2.44921e-16f
	{{{0x402921FB54442D18ULL}},{{0x0000000000000000ULL}}},	// PI*4,-4.89843e-16f
	{{{0x400921FB54442D18ULL}},{{0x3D47F640F23BF080ULL}}},	// DoubleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4012D97C7F3321D2ULL}},{{0xBFF0000000000001ULL}}},	// DoubleDegreesToRadians*270.0f,-1.0f
	{{{0x4083A28C59D5433BULL}},{{0x0000000000000000ULL}}}	// DoubleDegreesToRadians*36000.0f,1.17625e-005f
#endif
};

static Word TestSinDouble(void)
{
	const Burger::Word64ToDouble *pWork = SinDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(SinDoubleArray);
	Word uResult = FALSE;
	do {
		Burger::Word64ToDouble fTest;
		double fOriginal = pWork[0];
		fTest.d = Burger::Sin(fOriginal);
		double fExpected = pWork[1];
		Word uFailure = (fTest.d!=fExpected);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Sin((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,fOriginal,fTest.d,Test.GetPtr(),fExpected,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Cos(float)
//

static const Burger::Word32ToFloat CosFloatArray[][2] = {
	// Due to precision differences between using true 24 bit floating point
	// vs. 80 bit 8087 FPU, use unit test tables based on CPU types
#if defined(BURGER_X86)
	{{{0x3FC90FDBU}},{{0xB31D6350U}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0xBF7FFFFEU}}},	// PI,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xB42758D4U}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x3F800000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x3F800000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0xBF7FFFFEU}}},	// SingleDegreesToRadians*180.0f,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xB42758D4U}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0x3F800000U}}},	// SingleDegreesToRadians*36000.0f,-5.7220458984375e-006
	{{{0xC0490FDBU}},{{0xBF7FFFFEU}}},	// -PI,1.176484119014276e-007
#elif defined(BURGER_POWERPC)
	{{{0x3FC90FDBU}},{{0xB350E6E7U}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0xBF7FFFFEU}}},	// PI,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xB3F5EA3DU}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x3F800000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x3F800000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0xBF7FFFFEU}}},	// SingleDegreesToRadians*180.0f,1.176484119014276e-007
	{{{0x4096CBE4U}},{{0xB3F5EA3DU}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0x3F800000U}}},	// SingleDegreesToRadians*36000.0f,-5.7220458984375e-006
	{{{0xC0490FDBU}},{{0xBF7FFFFEU}}},	// -PI,1.176484119014276e-007
#else
	{{{0x3FC90FDBU}},{{0xB31B9628U}}},	// PI/2,1.0f
	{{{0x40490FDBU}},{{0xBF7FFFFCU}}},	// PI,1.22461e-16f
	{{{0x4096CBE4U}},{{0xB3D911EEU}}},	// PI*1.5f,-1.0f
	{{{0x40C90FDBU}},{{0x3F800000U}}},	// PI*2,-2.44921e-16f
	{{{0x41490FDBU}},{{0x3F800000U}}},	// PI*4,-4.89843e-16f
	{{{0x40490FDBU}},{{0xBF7FFFFCU}}},	// SingleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4096CBE4U}},{{0xB3D911EEU}}},	// SingleDegreesToRadians*270.0f,-1.0f
	{{{0x441D1463U}},{{0x3F800000U}}},	// SingleDegreesToRadians*36000.0f,1.17625e-005f
	{{{0xC0490FDBU}},{{0xBF7FFFFCU}}},	// -PI,-1.22461e-16f
#endif
	{{{0x00000000U}},{{0x3F800000U}}},	// 0.0f,0.0f
	{{{0x00000001U}},{{0x3F800000U}}},	// 0.0f,0.0f
	{{{0x00000010U}},{{0x3F800000U}}},	// 0.0f,0.0f
	{{{0x00000100U}},{{0x3F800000U}}},	// 0.0f,0.0f
	{{{0x00001000U}},{{0x3F800000U}}},	// 0.0f,0.0f
	{{{0x00010000U}},{{0x3F800000U}}}	// 0.0f,0.0f
	//	{{{0x7F800000U}},{{0xFFC00000U}}},	// Inf,Indefinite
	//	{{{0xFF800000U}},{{0xFFC00000U}}},	// -Inf,Indefinite
	//	{{{0x7F800001U}},{{0x7FC00001U}}},	// NaN,QNaN
	//	{{{0x7FC00000U}},{{0x7FC00000U}}},	// NaN,QNaN
	//	{{{0x7FC00001U}},{{0x7FC00001U}}},	// NaN,QNaN
	//	{{{0x7FFFFFFFU}},{{0x7FFFFFFFU}}}	// NaN,QNaN
};

static Word TestCosFloat(void)
{
	const Burger::Word32ToFloat *pWork = CosFloatArray[0];
	WordPtr i = BURGER_ARRAYSIZE(CosFloatArray);
	Word uResult = FALSE;
	do {
		Burger::Word32ToFloat fTest;
		float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float precision
		fTest.f = Burger::Cos(fOriginal);
		Word uFailure = (fTest.w!=pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Cos(%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,fOriginal,fTest.f,Test.GetPtr(),pWork[1].f,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

//
// Test Cos(double)
//

static const Burger::Word64ToDouble CosDoubleArray[][2] = {
#if defined(BURGER_POWERPC)
	{{{0x0000000000000000ULL}},{{0x3FF0000000000000ULL}}},	// 0.0f,0.0f
	{{{0x3FF921FB54442D18ULL}},{{0x3FE45F306DC9C884ULL}}},	// PI/2,1.0f
	{{{0x400921FB54442D18ULL}},{{0xBD2E8321619D4797ULL}}},	// PI,1.22461e-16f
	{{{0x4012D97C7F3321D2ULL}},{{0x3FE45F306DC9C884ULL}}},	// PI*1.5f,-1.0f
	{{{0x401921FB54442D18ULL}},{{0x3FF0000000000000ULL}}},	// PI*2,-2.44921e-16f
	{{{0x402921FB54442D18ULL}},{{0x3FF0000000000000ULL}}},	// PI*4,-4.89843e-16f
	{{{0x400921FB54442D18ULL}},{{0xBD2E8321619D4797ULL}}},	// DoubleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4012D97C7F3321D2ULL}},{{0x3FE45F306DC9C884ULL}}},	// DoubleDegreesToRadians*270.0f,-1.0f
	{{{0x4083A28C59D5433BULL}},{{0x3FF0000000000000ULL}}}	// DoubleDegreesToRadians*36000.0f,1.17625e-005f
#else
	{{{0x0000000000000000ULL}},{{0x3FF0000000000000ULL}}},	// 0.0f,0.0f
	{{{0x3FF921FB54442D18ULL}},{{0x3C8889ACE24800AEULL}}},	// PI/2,1.0f
	{{{0x400921FB54442D18ULL}},{{0xBFF00000000017DDULL}}},	// PI,1.22461e-16f
	{{{0x4012D97C7F3321D2ULL}},{{0x3C8889ACE24800AEULL}}},	// PI*1.5f,-1.0f
	{{{0x401921FB54442D18ULL}},{{0x3FF0000000000000ULL}}},	// PI*2,-2.44921e-16f
	{{{0x402921FB54442D18ULL}},{{0x3FF0000000000000ULL}}},	// PI*4,-4.89843e-16f
	{{{0x400921FB54442D18ULL}},{{0xBFF00000000017DDULL}}},	// DoubleDegreesToRadians*180.0f,-8.74228e-008f
	{{{0x4012D97C7F3321D2ULL}},{{0x3C8889ACE24800AEULL}}},	// DoubleDegreesToRadians*270.0f,-1.0f
	{{{0x4083A28C59D5433BULL}},{{0x3FF0000000000000ULL}}}	// DoubleDegreesToRadians*36000.0f,1.17625e-005f
#endif
};

static Word TestCosDouble(void)
{
	const Burger::Word64ToDouble *pWork = CosDoubleArray[0];
	WordPtr i = BURGER_ARRAYSIZE(CosDoubleArray);
	Word uResult = FALSE;
	do {
		Burger::Word64ToDouble fTest;
		double fOriginal = pWork[0];
		fTest.d = Burger::Cos(fOriginal);
		double fExpected = pWork[1];
		Word uFailure = (fTest.d!=fExpected);
		uResult |= uFailure;
		if (uFailure) {
			Burger::NumberStringHex Test(fTest.w);
			Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::Cos((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",uFailure,fOriginal,fTest.d,Test.GetPtr(),fExpected,Expected.GetPtr());
		}
		pWork+=2;
	} while (--i);
	return uResult;
}

/*

cosine
Input arg =+1.50000000000000000e+00
Computed value =-2.73750233727737824e-02
Correct value =+7.07372016677029203e-02
*/

//
// Perform all the tests for the Burgerlib FP Math library
//

int BURGER_API TestBrfloatingpoint(void)
{
#if !defined(BURGER_68K)

	Word uTotal;	// Assume no failures

	Message("Running FPMath tests");
	// Test floating point constants

	uTotal = TestFPConsts();

	// Test the test functions
	uTotal |= TestIsNanFloat();
	uTotal |= TestIsNanDouble();
	uTotal |= TestIsInfFloat();
	uTotal |= TestIsInfDouble();
	uTotal |= TestIsFiniteFloat();
	uTotal |= TestIsFiniteDouble();
	uTotal |= TestIsNormalFloat();
	uTotal |= TestIsNormalDouble();
	uTotal |= TestSignBitFloat();
	uTotal |= TestSignBitDouble();
	
	// Test simple floating point operations
	uTotal |= TestAbsFloat();
	uTotal |= TestAbsDouble();
	uTotal |= TestSqrtFloat();
	uTotal |= TestFloorFloat();
	uTotal |= TestFloorDouble();
	uTotal |= TestCeilFloat();
	uTotal |= TestCeilDouble();
	uTotal |= TestRoundFloat();
	uTotal |= TestRoundDouble();
	uTotal |= TestRoundToZeroFloat();
	uTotal |= TestRoundToZeroDouble();
	uTotal |= TestModuloRadiansFloat();
	uTotal |= TestModuloRadiansDouble();
	uTotal |= TestSinFloat();
	uTotal |= TestSinDouble();
	uTotal |= TestCosFloat();
	uTotal |= TestCosDouble();

	return static_cast<int>(uTotal);
#else
	return 0;
#endif
}
