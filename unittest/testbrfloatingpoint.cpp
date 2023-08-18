/***************************************

	Unit tests for the Floating Point Math library

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfloatingpoint.h"
#include "brfloatingpoint.h"
#include "brfpinfo.h"
#include "brmemoryfunctions.h"
#include "brnumberstringhex.h"
#include "common.h"
#include <math.h>

struct FloatInt_t {
	float m_fValue;
	int m_iValue;
};

struct NanTest_t {
	uint64_t uInput;
	uint32_t uFlags;
};

/** Constant for Infinity in the double format. */
static BURGER_CONSTEXPR const uint64_t g_dInf = 0x7FF0000000000000ULL;

/** Constant for Not a Number (NaN) in the double format */
static BURGER_CONSTEXPR const uint64_t g_dNan = 0x7FF7FFFFFFFFFFFFULL;

/** Constant for Not a Number (NaN) in the double format */
static BURGER_CONSTEXPR const uint64_t g_dQNan = 0x7FFFFFFFFFFFFFFFULL;

/** Constant for the smallest number (Non-zero) in the double format. */
static BURGER_CONSTEXPR const uint64_t g_dMin = 0x0010000000000000ULL;

/** Constant for the largest number in the double format. */
static BURGER_CONSTEXPR const uint64_t g_dMax = 0x7FEFFFFFFFFFFFFFULL;

/** Constant for Epsilon in the double format. */
static BURGER_CONSTEXPR const uint64_t g_dEpsilon = 0x3CB0000000000000ULL;

/** Constant for Pi in the double format. */
static BURGER_CONSTEXPR const uint64_t g_dPi = 0x400921FB54442D18ULL;

/** Constant for Infinity in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fInf = 0x7F800000U;

/** Constant for Not a Number (NaN) in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fNan = 0x7FBFFFFFU;

/** Constant for Not a Number (NaN) in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fQNan = 0x7FFFFFFFU;

/** Constant for smallest number (Non-zero) in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fMin = 0x00800000U;

/** Constant for largest number in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fMax = 0x7F7FFFFFU;

/** Constant for Epsilon in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fEpsilon = 0x34000000U;

/** Constant for Pi in the float format. */
static BURGER_CONSTEXPR const uint32_t g_fPi = 0x40490FDBU;

//
// Test all the floating point constants
// Perform this test first, since other tests
// rely on these tests passing.
//

static uint_t BURGER_API TestFPConsts(void) BURGER_NOEXCEPT
{
	union {
		float f;
		uint32_t u;
	} val32;

	// Test all the single values

	val32.u = ::g_fInf;
	float fExpected = val32.f;

	float fTest = Burger::g_fInf;
	uint_t uFailure = (fTest != fExpected);
	uint_t uResult = uFailure;
	ReportFailure(
		"Burger::g_fInf = %g / Wanted %g", uFailure, fTest, fExpected);

	val32.u = ::g_fMin;
	fExpected = val32.f;

	fTest = Burger::g_fMin;
	uFailure = (fTest != fExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_fMin = %g / Wanted %g", uFailure, fTest, fExpected);

	val32.u = ::g_fMax;
	fExpected = val32.f;

	fTest = Burger::g_fMax;
	uFailure = (fTest != fExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_fMax = %g / Wanted %g", uFailure, fTest, fExpected);

	val32.u = ::g_fEpsilon;
	fExpected = val32.f;

	fTest = Burger::g_fEpsilon;
	uFailure = (fTest != fExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_fEpsilon = %g / Wanted %g", uFailure, fTest, fExpected);

	val32.u = ::g_fPi;
	fExpected = val32.f;

	fTest = Burger::g_fPi;
	uFailure = (fTest != fExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_fPi = %g / Wanted %g", uFailure, fTest, fExpected);

	// Test all the double precision values

	union {
		double d;
		uint64_t u;
	} val64;

	val64.u = ::g_dInf;
	double dExpected = val64.d;

	double dTest = Burger::g_dInf;
	uFailure = (dTest != dExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_dInf = %g / Wanted %g", uFailure, dTest, dExpected);

	val64.u = ::g_dMin;
	dExpected = val64.d;

	dTest = Burger::g_dMin;
	uFailure = (dTest != dExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_dMin = %g / Wanted %g", uFailure, dTest, dExpected);

	val64.u = ::g_dMax;
	dExpected = val64.d;

	dTest = Burger::g_dMax;
	uFailure = (dTest != dExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_dMax = %g / Wanted %g", uFailure, dTest, dExpected);

	val64.u = ::g_dEpsilon;
	dExpected = val64.d;

	dTest = Burger::g_dEpsilon;
	uFailure = (dTest != dExpected);
	uResult |= uFailure;
	ReportFailure(
		"Burger::g_dEpsilon = %g / Wanted %g", uFailure, dTest, dExpected);

	val64.u = ::g_dPi;
	dExpected = val64.d;

	dTest = Burger::g_dPi;
	uFailure = (dTest != dExpected);
	uResult |= uFailure;
	ReportFailure("Burger::g_dPi = %g / Wanted %g", uFailure, dTest, dExpected);

	//
	// Detecting NaN is a different case. NaN's can't equal anything, so I
	// have to use the integer unit to test for equality
	// Don't use float != float, some compilers will return equal when that's
	// against the standard!
	//

	uint32_t wfFloat = Burger::g_fNan.w;
	uFailure = (wfFloat != ::g_fNan);
	uResult |= uFailure;
	val32.u = ::g_fNan;
	ReportFailure(
		"Burger::g_fNan = %g / Wanted %g", uFailure, Burger::g_fNan.f, val32.f);

	wfFloat = Burger::g_fQNan.w;
	uFailure = (wfFloat != ::g_fQNan);
	uResult |= uFailure;
	val32.u = ::g_fQNan;
	ReportFailure("Burger::g_fQNan = %g / Wanted %g", uFailure,
		Burger::g_fQNan.f, val32.f);

	uint64_t wdFloat = Burger::g_dNan.w;
	uFailure = (wdFloat != ::g_dNan);
	uResult |= uFailure;
	val64.u = ::g_dNan;
	ReportFailure(
		"Burger::g_dNan = %g / Wanted %g", uFailure, Burger::g_dNan.d, val64.d);

	wdFloat = Burger::g_dQNan.w;
	uFailure = (wdFloat != ::g_dQNan);
	uResult |= uFailure;
	val64.u = ::g_dQNan;
	ReportFailure("Burger::g_dQNan = %g / Wanted %g", uFailure,
		Burger::g_dQNan.d, val64.d);

	return uResult;
}

/***************************************

	Testing various features that are known to vary by compiler version

	Original code supplied by Matt Pritchard

***************************************/

static const uint32_t g_FloatSNANTests[] = {
	0x7F800001, 0xFF800001, // NAN, -NAN (smallest signaling)
	0x7F80FFFF, 0xFF80FFFF, // NAN, -NAN (signaling)
	0x7FBFFFFF, 0xFFBFFFFF  // NAN, -NAN (largest signaling)
};

static const uint64_t g_DoubleSNANTests[] = {
	0x7FF0000000000001ULL,
	0xFFF0000000000001ULL, // NAN, -NAN (smallest signaling)
	0x7FF0000FFFFFFFFFULL, 0xFFF000FFFFFFFFFFULL, // NAN, -NAN (signaling)
	0x7FF7FFFFFFFFFFFFULL,
	0xFFF7FFFFFFFFFFFFULL // NAN, -NAN (largest signaling)
};

static uint_t BURGER_API TestSNANToQNAN(void) BURGER_NOEXCEPT
{
	uint_t uResult = 0;

	// Test float NAN/QNAN

	float FloatBuffer[BURGER_ARRAYSIZE(g_FloatSNANTests)];

	// Copy the floats to a table
	uint_t i = 0;
	union {
		float f;
		uint32_t u;
	} val32;
	do {
		val32.u = g_FloatSNANTests[i];
		const float fSNAN = val32.f;
		// Copy the NAN
		const float fTempNAN = fSNAN;
		// Store the NAN
		FloatBuffer[i] = fTempNAN;
	} while (++i < BURGER_ARRAYSIZE(g_FloatSNANTests));

	uint_t bQNANFound = FALSE;
	i = 0;
	do {
		const uint32_t uOriginal = g_FloatSNANTests[i];
		val32.f = FloatBuffer[i];
		const uint32_t uTest = val32.u;

		// Not a match?!?
		if (uOriginal != uTest) {
			if ((uOriginal | 0x00400000) == uTest) {
				bQNANFound = TRUE;
			} else {
				uResult = 1; // Failed
				ReportFailure("Float SNAN 0x%08X converted to 0x%08X", 1,
					uOriginal, uTest);
			}
		}
	} while (++i < BURGER_ARRAYSIZE(g_FloatSNANTests));

	// Print the warning on QNan CPUs
	if (bQNANFound) {
		// So far, x86 fires this
		Message("Float SNAN was converted to QNAN on this CPU / Compiler");
	}

	// Test double NAN/QNAN

	double DoubleBuffer[BURGER_ARRAYSIZE(g_DoubleSNANTests)];
	union {
		double d;
		uint64_t u;
	} val64;
	// Copy the doubles to a table
	i = 0;
	do {
		val64.u = g_DoubleSNANTests[i];
		const double dSNAN = val64.d;
		// Copy the NAN
		const double dTempNAN = dSNAN;
		// Store the NAN
		DoubleBuffer[i] = dTempNAN;
	} while (++i < BURGER_ARRAYSIZE(g_DoubleSNANTests));

	bQNANFound = FALSE;
	i = 0;
	do {
		const uint64_t uOriginal = g_DoubleSNANTests[i];
		val64.d = DoubleBuffer[i];
		const uint64_t uTest = val64.u;

		// Not a match?!?
		if (uOriginal != uTest) {
			if ((uOriginal | 0x0008000000000000ULL) == uTest) {
				bQNANFound = TRUE;
			} else {
				uResult = 1; // Failed
				ReportFailure("Double SNAN 0x%llX converted to 0x%llX", 1,
					uOriginal, uTest);
			}
		}
	} while (++i < BURGER_ARRAYSIZE(g_DoubleSNANTests));

	// Print the warning on QNan CPUs
	if (bQNANFound) {
		// So far, x86 fires this
		Message("Double SNAN was converted to QNAN on this CPU / Compiler");
	}

	return uResult;
}

//
// Test is_NaN(float)
//

#define NANTEST 0x01    // NaN
#define INFTEST 0x02    // Infinity
#define FINITETEST 0x04 // Finite number
#define NORMALTEST 0x08 // Normal number
#define SIGNTEST 0x10   // Negative number

static const uint32_t IsNanTest[][2] = {
	{0x00000000U, FINITETEST},                         // 0.0f
	{0x00000001U, FINITETEST},                         // Lowest denormal
	{0x00000010U, FINITETEST},                         // Denormal
	{0x00400000U, FINITETEST},                         // Denormal
	{0x007FFFFFU, FINITETEST},                         // Highest denormal
	{0x00800000U, FINITETEST + NORMALTEST},            // Min
	{0x34000000U, FINITETEST + NORMALTEST},            // Epsilon
	{0x3F800000U, FINITETEST + NORMALTEST},            // 1.0f5
	{0x7F7FFFFFU, FINITETEST + NORMALTEST},            // Max
	{0x7F800000U, INFTEST},                            // Inf
	{0x7F800001U, NANTEST},                            // Nan
	{0x7FBFFFFFU, NANTEST},                            // Nan
	{0x7FC00000U, NANTEST},                            // QNan
	{0x7FFFFFFFU, NANTEST},                            // QNan
	{0x80000000U, FINITETEST + SIGNTEST},              // -0.0f
	{0x80000001U, FINITETEST + SIGNTEST},              // Lowest denormal
	{0x80000010U, FINITETEST + SIGNTEST},              // Denormal
	{0x80400000U, FINITETEST + SIGNTEST},              // Denormal
	{0x807FFFFFU, FINITETEST + SIGNTEST},              // Highest denormal
	{0x80800000U, FINITETEST + NORMALTEST + SIGNTEST}, // Min
	{0xB4000000U, FINITETEST + NORMALTEST + SIGNTEST}, // Epsilon
	{0xBF800000U, FINITETEST + NORMALTEST + SIGNTEST}, // 1.0f
	{0xFF7FFFFFU, FINITETEST + NORMALTEST + SIGNTEST}, // Max
	{0xFF800000U, INFTEST + SIGNTEST},                 // Inf
	{0xFF800001U, NANTEST + SIGNTEST},                 // Nan
	{0xFFBFFFFFU, NANTEST + SIGNTEST},                 // Nan
	{0xFFC00000U, NANTEST + SIGNTEST},                 // QNan
	{0xFFFFFFFFU, NANTEST + SIGNTEST}                  // QNan
};

static uint_t BURGER_API TestIsNanFloat(void) BURGER_NOEXCEPT
{
	const uint32_t* pWork = IsNanTest[0];
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTest);
	uint_t uResult = FALSE;
	do {
		const float fOriginal =
			static_cast<const float*>(static_cast<const void*>(pWork))[0];
		const uint_t uExpected = (pWork[1] & NANTEST) != 0;
		pWork += 2;
		uint_t uTest = Burger::is_NaN(fOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_NaN((float)%.16g) = %u / Wanted %u", uFailure,
			fOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(fOriginal);
		uTest = FPTest.is_NaN();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::FPInfo::is_NaN((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);

	} while (--i);
	return uResult;
}

//
// Test is_NaN(double)
//

static const NanTest_t IsNanTestDouble[] = {
	{0x0000000000000000ULL, FINITETEST},              // 0.0f
	{0x0000000000000001ULL, FINITETEST},              // Lowest denormal
	{0x0000000000000010ULL, FINITETEST},              // Denormal
	{0x0008000000000000ULL, FINITETEST},              // Denormal
	{0x000FFFFFFFFFFFFFULL, FINITETEST},              // Highest denormal
	{0x0010000000000000ULL, FINITETEST + NORMALTEST}, // Min
	{0x3CB0000000000000ULL, FINITETEST + NORMALTEST}, // Epsilon
	{0x3FF0000000000000ULL, FINITETEST + NORMALTEST}, // 1.0f
	{0x7FEFFFFFFFFFFFFFULL, FINITETEST + NORMALTEST}, // Max
	{0x7FF0000000000000ULL, INFTEST},                 // Inf
	{0x7FF0000000000001ULL, NANTEST},                 // Nan
	{0x7FF7FFFFFFFFFFFFULL, NANTEST},                 // Nan
	{0x7FF8000000000000ULL, NANTEST},                 // QNan
	{0x7FFFFFFFFFFFFFFFULL, NANTEST},                 // QNan
	{0x8000000000000000ULL, FINITETEST + SIGNTEST},   // -0.0f
	{0x8000000000000001ULL, FINITETEST + SIGNTEST},   // Lowest denormal
	{0x8000000000000010ULL, FINITETEST + SIGNTEST},   // Denormal
	{0x8008000000000000ULL, FINITETEST + SIGNTEST},   // Denormal
	{0x800FFFFFFFFFFFFFULL, FINITETEST + SIGNTEST},   // Highest denormal
	{0x8010000000000000ULL, FINITETEST + NORMALTEST + SIGNTEST}, // Min
	{0xBCB0000000000000ULL, FINITETEST + NORMALTEST + SIGNTEST}, // Epsilon
	{0xBFF0000000000000ULL, FINITETEST + NORMALTEST + SIGNTEST}, // 1.0f
	{0xFFEFFFFFFFFFFFFFULL, FINITETEST + NORMALTEST + SIGNTEST}, // Max
	{0xFFF0000000000000ULL, INFTEST + SIGNTEST},                 // Inf
	{0xFFF0000000000001ULL, NANTEST + SIGNTEST},                 // Nan
	{0xFFF7FFFFFFFFFFFFULL, NANTEST + SIGNTEST},                 // Nan
	{0xFFF8000000000000ULL, NANTEST + SIGNTEST},                 // QNan
	{0xFFFFFFFFFFFFFFFFULL, NANTEST + SIGNTEST}                  // QNan
};

static uint_t BURGER_API TestIsNanDouble(void) BURGER_NOEXCEPT
{
	const NanTest_t* pWork = IsNanTestDouble;
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTestDouble);
	uint_t uResult = FALSE;
	do {
		union {
			double d;
			uint64_t u;
		} val64;
		val64.u = pWork->uInput;
		const double dOriginal = val64.d;
		const uint_t uExpected = (pWork->uFlags & NANTEST) != 0;
		++pWork;
		uint_t uTest = Burger::is_NaN(dOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_NaN((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(dOriginal);
		uTest = FPTest.is_NaN();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::FPInfo::is_NaN((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test is_infinite(float)
//

static uint_t BURGER_API TestIsInfFloat(void) BURGER_NOEXCEPT
{
	const uint32_t* pWork = IsNanTest[0];
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTest);
	uint_t uResult = FALSE;
	do {
		union {
			float f;
			uint32_t u;
		} val32;
		val32.u = pWork[0];
		const float fOriginal = val32.f;
		const uint_t uExpected = (pWork[1] & INFTEST) != 0;
		pWork += 2;
		uint_t uTest = Burger::is_infinite(fOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_infinite((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(fOriginal);
		uTest = FPTest.is_infinity();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_infinity((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);

	} while (--i);
	return uResult;
}

//
// Test is_infinite(double)
//

static uint_t BURGER_API TestIsInfDouble(void) BURGER_NOEXCEPT
{
	const NanTest_t* pWork = IsNanTestDouble;
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTestDouble);
	uint_t uResult = FALSE;
	do {
		union {
			double d;
			uint64_t u;
		} val64;
		val64.u = pWork->uInput;
		const double dOriginal = val64.d;
		const uint_t uExpected = (pWork->uFlags & INFTEST) != 0;
		++pWork;
		uint_t uTest = Burger::is_infinite(dOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_infinite((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(dOriginal);
		uTest = FPTest.is_infinity();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_infinity((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test is_finite(float)
//

static uint_t BURGER_API TestIsFiniteFloat(void) BURGER_NOEXCEPT
{
	const uint32_t* pWork = IsNanTest[0];
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTest);
	uint_t uResult = FALSE;
	do {
		const float fOriginal =
			static_cast<const float*>(static_cast<const void*>(pWork))[0];
		const uint_t uExpected = (pWork[1] & FINITETEST) != 0;
		pWork += 2;
		uint_t uTest = Burger::is_finite(fOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_finite((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(fOriginal);
		uTest = FPTest.is_finite();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_finite((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test is_finite(double)
//

static uint_t BURGER_API TestIsFiniteDouble(void) BURGER_NOEXCEPT
{
	const NanTest_t* pWork = IsNanTestDouble;
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTestDouble);
	uint_t uResult = FALSE;
	do {
		union {
			double d;
			uint64_t u;
		} val64;
		val64.u = pWork->uInput;
		const double dOriginal = val64.d;
		const uint_t uExpected = (pWork->uFlags & FINITETEST) != 0;
		++pWork;
		uint_t uTest = Burger::is_finite(dOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_finite((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(dOriginal);
		uTest = FPTest.is_finite();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_finite((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test is_normal(float)
//

static uint_t BURGER_API TestIsNormalFloat(void) BURGER_NOEXCEPT
{
	const uint32_t* pWork = IsNanTest[0];
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTest);
	uint_t uResult = FALSE;
	do {
		const float fOriginal =
			static_cast<const float*>(static_cast<const void*>(pWork))[0];
		const uint_t uExpected = (pWork[1] & NORMALTEST) != 0;
		pWork += 2;
		const uint_t uTest = Burger::is_normal(fOriginal);
		const uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_normal((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test is_normal(double)
//

static uint_t BURGER_API TestIsNormalDouble(void) BURGER_NOEXCEPT
{
	const NanTest_t* pWork = IsNanTestDouble;
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTestDouble);
	uint_t uResult = FALSE;
	do {
		union {
			double d;
			uint64_t u;
		} val64;
		val64.u = pWork->uInput;
		const double dOriginal = val64.d;
		const uint_t uExpected = (pWork->uFlags & NORMALTEST) != 0;
		++pWork;
		const uint_t uTest = Burger::is_normal(dOriginal);
		const uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::is_normal((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test get_sign_bit(float)
//

static uint_t BURGER_API TestSignBitFloat(void) BURGER_NOEXCEPT
{
	const uint32_t* pWork = IsNanTest[0];
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTest);
	uint_t uResult = FALSE;
	do {
		const float fOriginal =
			static_cast<const float*>(static_cast<const void*>(pWork))[0];
		const uint_t uExpected = (pWork[1] & SIGNTEST) != 0;
		pWork += 2;
		uint_t uTest = Burger::get_sign_bit(fOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_sign_bit((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(fOriginal);
		uTest = FPTest.is_negative();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_negative((float)%.16g) = %u / Wanted %u",
			uFailure, fOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test get_sign_bit(double)
//

static uint_t BURGER_API TestSignBitDouble(void) BURGER_NOEXCEPT
{
	const NanTest_t* pWork = IsNanTestDouble;
	uintptr_t i = BURGER_ARRAYSIZE(IsNanTestDouble);
	uint_t uResult = FALSE;
	do {
		union {
			double d;
			uint64_t u;
		} val64;
		val64.u = pWork->uInput;
		const double dOriginal = val64.d;
		const uint_t uExpected = (pWork->uFlags & SIGNTEST) != 0;
		++pWork;
		uint_t uTest = Burger::get_sign_bit(dOriginal);
		uint_t uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_sign_bit((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);

		// Test FPInfo
		const Burger::FPInfo FPTest(dOriginal);
		uTest = FPTest.is_negative();
		uFailure = (uTest != uExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::FPInfo::is_negative((double)%.16g) = %u / Wanted %u",
			uFailure, dOriginal, uTest, uExpected);
	} while (--i);
	return uResult;
}

//
// Test absolute(float)
//

static const Burger::uint32_float_t AbsFloatArray[][2] = {
	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x80000000U}, {0x00000000U}}, // -0.0f,0.0f
	{{0x00000001U}, {0x00000001U}}, // 0.0f,0.0f
	{{0x80000001U}, {0x00000001U}}, // -0.0f,0.0f
	{{0x00000010U}, {0x00000010U}}, // 0.0f,0.0f
	{{0x80000010U}, {0x00000010U}}, // -0.0f,0.0f
	{{0x007FFFFFU}, {0x007FFFFFU}}, // 0.0f,0.0f
	{{0x807FFFFFU}, {0x007FFFFFU}}, // -0.0f,0.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0xBF800000U}, {0x3F800000U}}, // -1.0f,1.0f
	{{0x40490CDDU}, {0x40490CDDU}}, // 3.14141f,3.14141f
	{{0xC0490CDDU}, {0x40490CDDU}}, // -3.14141f,3.14141f
	{{0x4423C000U}, {0x4423C000U}}, // 655.0f,655.0f
	{{0xC423C000U}, {0x4423C000U}}, // -655.0f,655.0f
	{{0x4640E400U}, {0x4640E400U}}, // 12345.0f,12345.0f
	{{0xC640E400U}, {0x4640E400U}}, // -12345.0f,12345.0f
	{{0x7F800000U}, {0x7F800000U}}, // Burger::g_fInf,Burger::g_fInf
	{{0xFF800000U}, {0x7F800000U}}  // -Burger::g_fInf,Burger::g_fInf
	//	{{0x7F800001U},{0x7F800001U}},	// Burger::SingleNaN,Burger::SingleQaN
	//	{{0xFF800001U},{0x7F800001U}},	// -Burger::SingleNaN,Burger::SingleQaN
	//	{{0x7FC00000U},{0x7FC00000U}},	// Burger::SingleQaN,Burger::SingleQaN
	//	{{0xFFC00001U},{0x7FC00001U}}	// -Burger::SingleQNaN,Burger::SingleQaN
};
static uint_t BURGER_API TestAbsFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = AbsFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(AbsFloatArray);
	uint_t uResult = FALSE;
	do {
		Burger::uint32_float_t fTest;
		fTest.f = Burger::absolute(pWork[0]);
		const uint_t uFailure = (fTest.w != pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure("Burger::absolute(float) = %g 0x%s / Wanted %g 0x%s",
				uFailure, fTest.f, Test.c_str(), pWork[1].f, Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test absolute(double)
//

static const Burger::uint64_double_t AbsDoubleArray[][2] = {
	// 0.0,0.0
	{{0x0000000000000000ULL}, {0x0000000000000000ULL}},
	// -0.0,0.0
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}},
	// 0.0,0.0
	{{0x0000000000000001ULL}, {0x0000000000000001ULL}},
	// -0.0,0.0
	{{0x8000000000000001ULL}, {0x0000000000000001ULL}},
	// 0.0,0.0
	{{0x0000000000000010ULL}, {0x0000000000000010ULL}},
	// -0.0,0.0
	{{0x8000000000000010ULL}, {0x0000000000000010ULL}},
	// 0.0,0.0
	{{0x000FFFFFFFFFFFFFULL}, {0x000FFFFFFFFFFFFFULL}},
	// -0.0,0.0
	{{0x800FFFFFFFFFFFFFULL}, {0x000FFFFFFFFFFFFFULL}},
	// 1.0,1.0
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}},
	// -1.0,1.0
	{{0xBFF0000000000000ULL}, {0x3FF0000000000000ULL}},
	// 3.14141,3.14141
	{{0x4009219B90EA9E6FULL}, {0x4009219B90EA9E6FULL}},
	// -3.14141,3.14141
	{{0xC009219B90EA9E6FULL}, {0x4009219B90EA9E6FULL}},
	// 655.0,655.0
	{{0x4084780000000000ULL}, {0x4084780000000000ULL}},
	// -655.0,655.0
	{{0xC084780000000000ULL}, {0x4084780000000000ULL}},
	// 12345.0,12345.0
	{{0x40C81C8000000000ULL}, {0x40C81C8000000000ULL}},
	// -12345.0,12345.0
	{{0xC0C81C8000000000ULL}, {0x40C81C8000000000ULL}},
	// Burger::g_dInf,Burger::g_dInf
	{{0x7FF0000000000000ULL}, {0x7FF0000000000000ULL}},
	// -Burger::g_dInf,Burger::g_dInf
	{{0xFFF0000000000000ULL}, {0x7FF0000000000000ULL}}};

static uint_t BURGER_API TestAbsDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = AbsDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(AbsDoubleArray);
	uint_t uResult = FALSE;
	do {
		double dTest = pWork[0];
		const double dExpected = pWork[1];
		pWork += 2;
		dTest = Burger::absolute(dTest);
		const uint_t uFailure = (dTest != dExpected);
		uResult |= uFailure;
		ReportFailure("Burger::absolute(double) = %g / Wanted %g", uFailure,
			dTest, dExpected);
	} while (--i);
	return uResult;
}

//
// Test get_sign(float)
//

static const Burger::uint32_float_t SignFloatArray[][2] = {
	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x80000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x00000001U}, {0x3F800000U}}, // 0.0f,1.0f
	{{0x80000001U}, {0xBF800000U}}, // 0.0f,-1.0f
	{{0x00000010U}, {0x3F800000U}}, // 0.0f,1.0f
	{{0x80000010U}, {0xBF800000U}}, // 0.0f,-1.0f
	{{0x007FFFFFU}, {0x3F800000U}}, // 0.0f,1.0f
	{{0x807FFFFFU}, {0xBF800000U}}, // 0.0f,-1.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0x40490CDDU}, {0x3F800000U}}, // 3.14141f,1.0f
	{{0xC0490CDDU}, {0xBF800000U}}, // -3.14141f,-1.0f
	{{0x4640E400U}, {0x3F800000U}}, // 12345.0f,1.0f
	{{0xC640E400U}, {0xBF800000U}}, // -12345.0f,-1.0f
	{{0x7F800000U}, {0x3F800000U}}, // Burger::g_fInf,1.0f
	{{0xFF800000U}, {0xBF800000U}}, // -Burger::g_fInf,-1.0f,
	{{0x7FBFFFFFU}, {0x3F800000U}}, // Burger::g_fNan,1.0f
	{{0xFFBFFFFFU}, {0xBF800000U}}, // -Burger::g_fNan,1.0f
	{{0x7FFFFFFFU}, {0x3F800000U}}, // Burger::g_fQNan,1.0f
	{{0xFFFFFFFFU}, {0xBF800000U}}  // -Burger::g_fNan,1.0f
};

static uint_t BURGER_API TestSignFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = SignFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(SignFloatArray);
	uint_t uResult = FALSE;
	do {
		Burger::uint32_float_t fTest;
		fTest.f = Burger::get_sign(pWork[0]);
		const uint_t uFailure = (fTest.w != pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::get_sign(float(%f)) = %f 0x%s / Wanted %f 0x%s",
				uFailure, pWork[0].f, fTest.f, Test.c_str(), pWork[1].f,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_sign(double)
//

static const Burger::uint64_double_t SignDoubleArray[][2] = {
	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x0000000000000001ULL}, {0x3FF0000000000000ULL}}, // 0.0f,1.0f
	{{0x8000000000000001ULL}, {0xBFF0000000000000ULL}}, // 0.0f,-1.0f
	{{0x0000000000000010ULL}, {0x3FF0000000000000ULL}}, // 0.0f,1.0f
	{{0x8000000000000010ULL}, {0xBFF0000000000000ULL}}, // 0.0f,-1.0f
	{{0x000FFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 0.0f,1.0f
	{{0x800FFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // 0.0f,-1.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0x400921FB54442D18ULL}, {0x3FF0000000000000ULL}}, // 3.14141f,1.0f
	{{0xC00921FB54442D18ULL}, {0xBFF0000000000000ULL}}, // -3.14141f,-1.0f
	{{0x40C81C8000000000ULL}, {0x3FF0000000000000ULL}}, // 12345.0f,1.0f
	{{0xC0C81C8000000000ULL}, {0xBFF0000000000000ULL}}, // -12345.0f,-1.0f
	{{0x7FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // Burger::g_fInf,1.0f
	{{0xFFF0000000000000ULL},
		{0xBFF0000000000000ULL}}, // -Burger::g_fInf,-1.0f,
	{{0x7FF7FFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // Burger::g_dNan,1.0f
	{{0xFFF7FFFFFFFFFFFFULL},
		{0xBFF0000000000000ULL}}, // -Burger::g_dNan,-1.0f,
	{{0x7FFFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // Burger::g_dQNan,1.0f
	{{0xFFFFFFFFFFFFFFFFULL},
		{0xBFF0000000000000ULL}} // -Burger::g_dQNan,-1.0f,
};

static uint_t BURGER_API TestSignDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = SignDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(SignDoubleArray);
	uint_t uResult = FALSE;
	do {
		Burger::uint64_double_t fTest;
		fTest = Burger::get_sign(pWork[0]);
		const uint_t uFailure = (fTest.w != pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::get_sign(double(%g)) = %g 0x%s / Wanted %g 0x%s",
				uFailure, pWork[0].d, fTest.d, Test.c_str(), pWork[1].d,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test clamp(float,float,float)
//

static const Burger::uint32_float_t ClampFloatArray[][4] = {
	// 0.0f,-1.0f,1.0f,0.0f
	{{0x00000000U}, {0xBF800000U}, {0x3F800000U}, {0x00000000U}},
	// 1.0f,-1.0f,1.0f,1.0f
	{{0x3F800000U}, {0xBF800000U}, {0x3F800000U}, {0x3F800000U}},
	// -1.0f,-1.0f,1.0f,-1.0f
	{{0xBF800000U}, {0xBF800000U}, {0x3F800000U}, {0xBF800000U}},
	// 3.14141f,-1.0f,1.0f,1.0f
	{{0x40490CDDU}, {0xBF800000U}, {0x3F800000U}, {0x3F800000U}},
	// -3.14141f,-1.0f,1.0f,-1.0f
	{{0xC0490CDDU}, {0xBF800000U}, {0x3F800000U}, {0xBF800000U}},
	// 12345.0f,-1.0f,1.0f,1.0f
	{{0x4640E400U}, {0xBF800000U}, {0x3F800000U}, {0x3F800000U}},
	// -12345.0f,-1.0f,1.0f,-1.0f
	{{0xC640E400U}, {0xBF800000U}, {0x3F800000U}, {0xBF800000U}},
	// Burger::g_fInf,-1.0f,1.0f,1.0f
	{{0x7F800000U}, {0xBF800000U}, {0x3F800000U}, {0x3F800000U}},
	// -Burger::g_fInf,-1.0f,1.0f,-1.0f
	{{0xFF800000U}, {0xBF800000U}, {0x3F800000U}, {0xBF800000U}}};

static uint_t BURGER_API TestClampFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = ClampFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(ClampFloatArray);
	uint_t uResult = FALSE;
	do {
		Burger::uint32_float_t fTest;
		fTest.f = Burger::clamp(pWork[0], pWork[1], pWork[2]);
		const uint_t uFailure = (fTest.w != pWork[3].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[3].w);
			ReportFailure(
				"Burger::clamp(float(%g),float(%g),float(%g)) = %g 0x%s / Wanted %g 0x%s",
				uFailure, pWork[0].f, pWork[1].f, pWork[2].f, fTest.f,
				Test.c_str(), pWork[3].f, Expected.c_str());
		}
		pWork += 4;
	} while (--i);
	return uResult;
}

//
// Test clamp(double,double,double)
//

static const Burger::uint64_double_t ClampDoubleArray[][4] = {
	// 0.0f,-1.0f,1.0f,0.0f
	{{0x0000000000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0x0000000000000000ULL}},
	// 1.0f,-1.0f,1.0f,1.0f
	{{0x3FF0000000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0x3FF0000000000000ULL}},
	// -1.0f,-1.0f,1.0f,-1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0xBFF0000000000000ULL}},
	// 3.14141f,-1.0f,1.0f,1.0f
	{{0x400921FB54442D18ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0x3FF0000000000000ULL}},
	// -3.14141f,-1.0f,1.0f,-1.0f
	{{0xC00921FB54442D18ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0xBFF0000000000000ULL}},
	// 12345.0f,-1.0f,1.0f,1.0f
	{{0x40C81C8000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0x3FF0000000000000ULL}},
	// -12345.0f,-1.0f,1.0f,-1.0f
	{{0xC0C81C8000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0xBFF0000000000000ULL}},
	// Burger::g_fInf,-1.0f,1.0f,1.0f
	{{0x7FF0000000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0x3FF0000000000000ULL}},
	// -Burger::g_fInf,-1.0f,1.0f,-1.0f
	{{0xFFF0000000000000ULL}, {0xBFF0000000000000ULL}, {0x3FF0000000000000ULL},
		{0xBFF0000000000000ULL}}};

static uint_t BURGER_API TestClampDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = ClampDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(ClampDoubleArray);
	uint_t uResult = FALSE;
	do {
		Burger::uint64_double_t fTest;
		fTest = Burger::clamp(pWork[0], pWork[1], pWork[2]);
		const uint_t uFailure = (fTest.w != pWork[3].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[3].w);
			ReportFailure(
				"Burger::clamp(float(%g),float(%g),float(%g)) = %g 0x%s / Wanted %g 0x%s",
				uFailure, pWork[0].d, pWork[1].d, pWork[2].d, fTest.d,
				Test.c_str(), pWork[3].d, Expected.c_str());
		}
		pWork += 4;
	} while (--i);
	return uResult;
}

//
// Test square_root(float)
//

static const Burger::uint32_float_t SqrtFloatArray[][2] = {
	// 0.0f,0.0f
	{{0x00000000U}, {0x00000000U}},
	// 1.401298464324817e-045,3.743392066509216e-023
	{{0x00000001U}, {0x1a3504f3U}},
	// 2.242077542919707e-044,1.497356826603687e-022
	{{0x00000010U}, {0x1b3504f3U}},
	// 4.701975721730993e-039,6.857095279752172e-020
	{{0x00333332U}, {0x1fa1e899U}},
	// 4.701977123029457e-039,6.85709657222188e-020
	{{0x00333333U}, {0x1fa1e89bU}},
	// 5.877471754111438e-039,7.666466952210875e-020
	{{0x00400000U}, {0x1fb504f3U}},
	// 1.175494210692441e-038,1.084202107862019e-019
	{{0x007FFFFFU}, {0x1fffffffU}},
	// 0.1275f,0.3570714294910431f
	{{0x3E028F5CU}, {0x3EB6D211U}},
	// 0.25f,0.5f
	{{0x3E800000U}, {0x3F000000U}},
	// 0.95f,0.9746794f
	{{0x3F733333U}, {0x3F798497U}},
	// 1.0f,1.0f
	{{0x3F800000U}, {0x3F800000U}},
	// 2.0f,1.4142135381698608f
	{{0x40000000U}, {0x3FB504F3U}},
	// 2.4f,1.549193382263184f
	{{0x4019999AU}, {0x3FC64BF8U}},
	// 4.0f,2.0f
	{{0x40800000U}, {0x40000000U}},
	// 24.0f,4.898979663848877f
	{{0x41C00000U}, {0x409CC471U}},
	// 378.556f,19.45651626586914f
	{{0x43BD472BU}, {0x419BA6F2U}},
	// 1.701411834604692e+038,1.304381760209735e+019
	{{0x7F000000U}, {0x5f3504f3U}},
	// 2.552117751907039e+038,1.597534869821325e+019
	{{0x7F400000U}, {0x5f5db3d7U}},
	// FLT_MAX,1.844674297419792e+019
	{{0x7F7FFFFFU}, {0x5f7fffffU}}
	// Don't test negative numbers for sqrt
	//{{0xBF800000U},{0xFFC00000U}}	// -1.0f,1.0f
};

static uint_t BURGER_API TestSqrtFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = SqrtFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(SqrtFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];

		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		volatile float fTest = Burger::square_root(fOriginal);

		const float fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::square_root((float)%.16g) = %.16g / Wanted %.16g",
			uFailure, fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test square_root(double)
//

static const Burger::uint64_double_t SqrtDoubleArray[][2] = {
	// 0.0f,0.0f
	{{0x0000000000000000ULL}, {0x0000000000000000ULL}},
	// 4.940656458412e-324, 2.2227587494850775e-162
	{{0x0000000000000001ULL}, {0x1e60000000000000ULL}},
	// 7.905050333460e-323, 8.8910349979403099e-162
	{{0x0000000000000010ULL}, {0x1e80000000000000ULL}},
	// 1.112536929254e-308, 1.0547686614863000e-154
	{{0x0008000000000000ULL}, {0x1ff6a09e667f3bcdULL}},
	// 2.225073858507e-308, 1.4916681462400412e-154
	{{0x000FFFFFFFFFFFFFULL}, {0x1fffffffffffffffULL}},
	// 0.1275,0.35707142142714249
	{{0x3fc051eb851eb852ULL}, {0x3fd6da4217576971ULL}},
	// 0.25,0.5
	{{0x3FD0000000000000ULL}, {0x3FE0000000000000ULL}},
	// 0.95,0.9746794344808963
	{{0x3FEE666666666666ULL}, {0x3fef3092ece5bc35ULL}},
	// 1.0f,1.0
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}},
	// 2.0f,1.4142135623730951
	{{0x4000000000000000ULL}, {0x3ff6a09e667f3bcdULL}},
	// 2.4,1.5491933384829668
	{{0x4003333333333333ULL}, {0x3ff8c97ef43f7248ULL}},
	// 4.0,2.0
	{{0x4010000000000000ULL}, {0x4000000000000000ULL}},
	// 24.0,4.8989794855663558
	{{0x4038000000000000ULL}, {0x4013988e1409212eULL}},
	// 378.556,19.456515618167607
	{{0x4077a8e560418937ULL}, {0x403374de352224a2ULL}},
	// = 8.9884656743115795e+307,9.4807519081091774e+153
	{{0x7FE0000000000000ULL}, {0x5fe6a09e667f3bcdULL}}, // dOriginal
	// = 1.3482698511467369e+308,1.1611502276392735e+154
	{{0x7FE8000000000000ULL}, {0x5febb67ae8584caaULL}}, // dOriginal
	// FLT_MAX,1.3407807929942596e+154
	{{0x7FEFFFFFFFFFFFFFULL}, {0x5fefffffffffffffULL}}
	// Don't test negative numbers for sqrt
	//{{0xBFF0000000000000ULL},{0xFFF0000000000000ULL}}	// -1.0f,1.0f
};

static uint_t BURGER_API TestSqrtDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = SqrtDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(SqrtDoubleArray);
	uint_t uResult = FALSE;
	do {
		const double dOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 64 bit float
		// precision
		volatile double dTest = Burger::square_root(dOriginal);
		const double dExpected = pWork[1];
		const uint_t uFailure = (dTest != dExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::square_root((double)%.16g) = %.16g / Wanted %.16g",
			uFailure, dOriginal, dTest, dExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test int_to_float(float *,const int32_t *)
//

static const Burger::uint32_float_t IntToFloatArray[][2] = {
	{{0x00000000U}, {0x00000000U}}, // 0
	{{0x00000001U}, {0x3F800000U}}, // 1
	{{0xFFFFFFFFU}, {0xBF800000U}}, // -1
	{{0x7FFFFFFFU}, {0x4F000000U}}, // MAXINT
	{{0x80000000U}, {0xCF000000U}}, // -MAXINT
	{{0x7FFFFFFEU}, {0x4F000000U}}, // MAXINT-1
	{{0x80000001U}, {0xCF000000U}}, // -MAXINT+1
	{{0x00800000U}, {0x4B000000U}}, // 8388608
	{{0xFF800000U}, {0xCB000000U}}, // -8388608
	{{0x00800001U}, {0x4B000001U}}, // 8388609
	{{0xFF7FFFFFU}, {0xCB000001U}}, // -8388609
	{{0x01000000U}, {0x4B800000U}}, // 16777216
	{{0xFF000000U}, {0xCB800000U}}, // -16777216
	{{0x01000001U}, {0x4B800000U}}, // 16777217
	{{0xFEFFFFFFU}, {0xCB800000U}}, // -16777217
	{{0x01000002U}, {0x4B800001U}}, // 16777218
	{{0xFEFFFFFEU}, {0xCB800001U}}, // -16777218
	{{0x01000003U}, {0x4B800002U}}, // 16777219
	{{0xFEFFFFFDU}, {0xCB800002U}}, // -16777219
	{{0x01000004U}, {0x4B800002U}}, // 16777220
	{{0xFEFFFFFCU}, {0xCB800002U}}, // -16777220
	{{0x01000005U}, {0x4B800002U}}, // 16777221
	{{0xFEFFFFFBU}, {0xCB800002U}}, // -16777221
	{{0x01000006U}, {0x4B800003U}}, // 16777222
	{{0xFEFFFFFAU}, {0xCB800003U}}, // -16777222
	{{0x01000007U}, {0x4B800004U}}, // 16777223
	{{0xFEFFFFF9U}, {0xCB800004U}}, // -16777223
	{{0x01000008U}, {0x4B800004U}}, // 16777224
	{{0xFEFFFFF8U}, {0xCB800004U}}, // -16777224
	{{0x01000009U}, {0x4B800004U}}, // 16777225
	{{0xFEFFFFF7U}, {0xCB800004U}}, // -16777225
	{{0x0100000AU}, {0x4B800005U}}, // 16777226
	{{0xFEFFFFF6U}, {0xCB800005U}}, // -16777226
	{{0x0100000BU}, {0x4B800006U}}, // 16777227
	{{0xFEFFFFF5U}, {0xCB800006U}}  // -16777227
};

static uint_t BURGER_API TestIntToFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = IntToFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(IntToFloatArray);
	uint_t uResult = FALSE;
	do {
		const int32_t iOriginal = static_cast<int_t>(pWork[0].get_uint32());
		const float fExpected = pWork[1];

		volatile float fTest = Burger::int_to_float(iOriginal);
		uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::int_to_float((int32_t)%d) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		fTest = Burger::int_to_float(&iOriginal);
		uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::int_to_float((const int32_t *)%d) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		float fTemp;
		Burger::int_to_float(&fTemp, iOriginal);
		fTest = fTemp;
		uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::int_to_float((float *),(int_t)%d) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		uintptr_t index = 0;
		float fTempArray[4];
		do {
			Burger::MemoryFill(fTempArray, 0x55, sizeof(fTempArray));
			Burger::int_to_float(&fTempArray[index], &iOriginal);
			fTest = fTempArray[index];
			uFailure = (fTest != fExpected);
			uResult |= uFailure;
			ReportFailure(
				"Burger::int_to_float((float *) + %u,(int_t *)%d) = %.16g / Wanted %.16g",
				uFailure, static_cast<uint_t>(index), iOriginal, fTest,
				fExpected);
		} while (++index < BURGER_ARRAYSIZE(fTempArray));

		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test fixed_to_float(float *,const Fixed32 *)
//

static const Burger::uint32_float_t FixedToFloatArray[][2] = {
	{{0x00000000U}, {0x00000000U}}, // 0
	{{0x00010000U}, {0x3F800000U}}, // 1
	{{0xFFFF0000U}, {0xBF800000U}}, // -1
	{{0x7FFFFFFFU}, {0x47000000U}}, // 32767.999999
	{{0x80000000U}, {0xC7000000U}}, // -32768
	{{0x7FFFFFFEU}, {0x47000000U}}, // 32767.999999-1
	{{0x80000001U}, {0xC7000000U}}, // -32768+1
	{{0x00800000U}, {0x43000000U}}, // 128
	{{0xFF800000U}, {0xC3000000U}}, // -128
	{{0x00800001U}, {0x43000001U}}, // 128.0000152587891
	{{0xFF7FFFFFU}, {0xC3000001U}}, // -128.0000152587891
	{{0x01000000U}, {0x43800000U}}, // 256
	{{0xFF000000U}, {0xC3800000U}}, // -256
	{{0x01000001U}, {0x43800000U}}, // 256.0000152587891
	{{0xFEFFFFFFU}, {0xC3800000U}}, // -256.0000152587891
	{{0x01000002U}, {0x43800001U}}, // 256.0000305175781
	{{0xFEFFFFFEU}, {0xC3800001U}}, // -256.0000305175781
	{{0x01000003U}, {0x43800002U}}, // 256.0000305175781
	{{0xFEFFFFFDU}, {0xC3800002U}}, // -256.0000305175781
	{{0x01000004U}, {0x43800002U}}, // 256.0000305175781
	{{0xFEFFFFFCU}, {0xC3800002U}}, // -256.0000305175781
	{{0x01000005U}, {0x43800002U}}, // 256.0000305175781
	{{0xFEFFFFFBU}, {0xC3800002U}}, // -256.0000305175781
	{{0x01000006U}, {0x43800003U}}, // 256.0000305175781
	{{0xFEFFFFFAU}, {0xC3800003U}}, // -256.0000305175781
	{{0x01000007U}, {0x43800004U}}, // 256.0000305175781
	{{0xFEFFFFF9U}, {0xC3800004U}}, // -256.0000305175781
	{{0x01000008U}, {0x43800004U}}, // 256.0000305175781
	{{0xFEFFFFF8U}, {0xC3800004U}}, // -256.0000305175781
	{{0x01000009U}, {0x43800004U}}, // 256.0000305175781
	{{0xFEFFFFF7U}, {0xC3800004U}}, // -256.0000305175781
	{{0x0100000AU}, {0x43800005U}}, // 256.0000305175781
	{{0xFEFFFFF6U}, {0xC3800005U}}, // -256.0000305175781
	{{0x0100000BU}, {0x43800006U}}, // 256.0000305175781
	{{0xFEFFFFF5U}, {0xC3800006U}}  // -256.0000305175781
};

static uint_t BURGER_API TestFixedToFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = FixedToFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(FixedToFloatArray);
	uint_t uResult = FALSE;
	do {
		const int32_t iOriginal = static_cast<int_t>(pWork[0].get_uint32());
		const float fExpected = pWork[1];

		volatile float fTest = Burger::fixed_to_float(iOriginal);
		uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::fixed_to_float((Fixed)0x%08X) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		fTest = Burger::fixed_to_float(&iOriginal);
		uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::fixed_to_float((const Fixed *)0x%08X) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		float fTemp;
		Burger::fixed_to_float(&fTemp, iOriginal);
		fTest = fTemp;
		uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::fixed_to_float((float *),(Fixed)0x%08X) = %.16g / Wanted %.16g",
			uFailure, iOriginal, fTest, fExpected);

		uintptr_t index = 0;
		float fTempArray[4];
		do {
			Burger::MemoryFill(fTempArray, 0x55, sizeof(fTempArray));
			Burger::fixed_to_float(&fTempArray[index], &iOriginal);
			fTest = fTempArray[index];
			uFailure = (fTest != fExpected);
			uResult |= uFailure;
			ReportFailure(
				"Burger::fixed_to_float((float *) + %u,(Fixed *)0x%08X) = %.16g / Wanted %.16g",
				uFailure, static_cast<uint_t>(index), iOriginal, fTest,
				fExpected);
		} while (++index < BURGER_ARRAYSIZE(fTempArray));
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_floor(float)
//

static const Burger::uint32_float_t FloorFloatArray[][2] = {
	{{0xC07FFFFFU}, {0xC0800000U}}, // -3.999f,-4.0f
	{{0xC0600000U}, {0xC0800000U}}, // -3.5f,-4.0f
	{{0xC05FFFFFU}, {0xC0800000U}}, // -3.499f,-4.0f
	{{0xC0400000U}, {0xC0400000U}}, // -3.0f,-3.0f
	{{0xC03FFFFFU}, {0xC0400000U}}, // -2.999f,-3.0f
	{{0xC0200000U}, {0xC0400000U}}, // -2.5f,-3.0f
	{{0xC01FFFFFU}, {0xC0400000U}}, // -2.499f,-3.0f
	{{0xC0000000U}, {0xC0000000U}}, // -2.0f,-2.0f
	{{0xBFFFFFFFU}, {0xC0000000U}}, // -1.999f,-2.0f
	{{0xBFC00000U}, {0xC0000000U}}, // -1.5f,-2.0f
	{{0xBFBFFFFFU}, {0xC0000000U}}, // -1.499f,-2.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0xBF7FFFFFU}, {0xBF800000U}}, // -0.999f,-1.0f
	{{0xBF000000U}, {0xBF800000U}}, // -0.5f,-1.0f
	{{0xBEFFFFFFU}, {0xBF800000U}}, // -0.499f,-1.0f
	{{0x80000000U}, {0x00000000U}}, // -0.0f,0.0f

	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x3EFFFFFFU}, {0x00000000U}}, // 0.499f,0.0f
	{{0x3F000000U}, {0x00000000U}}, // 0.5f,0.0f
	{{0x3F7FFFFFU}, {0x00000000U}}, // 0.999f,0.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0x3FBFFFFFU}, {0x3F800000U}}, // 1.499f,1.0f
	{{0x3FC00000U}, {0x3F800000U}}, // 1.5f,1.0f
	{{0x3FFFFFFFU}, {0x3F800000U}}, // 1.999f,1.0f
	{{0x40000000U}, {0x40000000U}}, // 2.0f,2.0f
	{{0x401FFFFFU}, {0x40000000U}}, // 2.499f,2.0f
	{{0x40200000U}, {0x40000000U}}, // 2.5f,2.0f
	{{0x403FFFFFU}, {0x40000000U}}, // 2.999f,2.0f
	{{0x40400000U}, {0x40400000U}}, // 3.0f,3.0f
	{{0x405FFFFFU}, {0x40400000U}}, // 3.499f,3.0f
	{{0x40600000U}, {0x40400000U}}, // 3.5f,3.0f
	{{0x407FFFFFU}, {0x40400000U}}, // 3.999f,3.0f

	{{0x3F8CCCCDU}, {0x3F800000U}}, // 1.1f,1.0f
	{{0x3FF9999AU}, {0x3F800000U}}, // 1.95f,1.0f
	{{0x40066666U}, {0x40000000U}}, // 2.1f,2.0f
	{{0x403CCCCDU}, {0x40000000U}}, // 2.95f,2.0f
	{{0xBF8CCCCDU}, {0xC0000000U}}, // -1.1f,-2.0f
	{{0xBFF9999AU}, {0xC0000000U}}, // -1.95f,-2.0f
	{{0xC0066666U}, {0xC0400000U}}, // -2.1f,-3.0f
	{{0xC03CCCCDU}, {0xC0400000U}}, // -2.95f,-3.0f
	{{0x3DCCCCCDU}, {0x00000000U}}, // 0.1f,0.0f
	{{0x3F733333U}, {0x00000000U}}, // 0.95f,0.0f
	{{0xBDCCCCCDU}, {0xBF800000U}}, // -0.1f,-1.0f
	{{0xBF7333BDU}, {0xBF800000U}}, // -0.95f,-1.0f
	{{0x4B000000U}, {0x4B000000U}}, // 8388608.0f,8388608.0f
	{{0xCB000000U}, {0xCB000000U}}, // -8388608.0f,-8388608.0f
	{{0x4CA00000U}, {0x4CA00000U}}, // 83886080.0f,83886080.0f
	{{0xCCA00000U}, {0xCCA00000U}}, // -83886080.0f,-83886080.0f
	{{0x4E480000U}, {0x4E480000U}}, // 838860800.0f,838860800.0f
	{{0xCE480000U}, {0xCE480000U}}, // -838860800.0f,-838860800.0f
	{{0x4F000000U}, {0x4F000000U}}, // 0x7FFFFFF
	{{0x4EFFFFFFU}, {0x4EFFFFFFU}}, // 0x7FFFFFF-Epsilon
	{{0xCF000000U}, {0xCF000000U}}, // 0x8000000
	{{0x00000001U}, {0x00000000U}}, // Min Denormal
	{{0x00000010U}, {0x00000000U}}, {{0x00400000U}, {0x00000000U}},
	{{0x007FFFFFU}, {0x00000000U}}, // Max Denormal
	{{0x00800000U}, {0x00000000U}}, // FLT_MIN
	{{0x7F7FFFFFU}, {0x7F7FFFFFU}}, // FLT_MAX
	{{0x80000001U}, {0xBF800000U}}, // -Min Denormal
	{{0x80000010U}, {0xBF800000U}}, {{0x80400000U}, {0xBF800000U}},
	{{0x807FFFFFU}, {0xBF800000U}}, // -Max Denormal
	{{0x80800000U}, {0xBF800000U}}, // -FLT_MIN
	{{0xFF7FFFFFU}, {0xFF7FFFFFU}}  // -FLT_MAX
};

static uint_t BURGER_API TestFloorFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = FloorFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(FloorFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		volatile float fTest = Burger::get_floor(fOriginal);
		const float fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_floor((float)%.16g) = %.16g / Wanted %.16g",
			uFailure, fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_floor(double)
//

static const Burger::uint64_double_t FloorDoubleArray[][2] = {
	{{0xC00FFFFFFFFFFFFFULL}, {0xC010000000000000ULL}}, // -3.999f,-4.0f
	{{0xC00C000000000000ULL}, {0xC010000000000000ULL}}, // -3.5f,-4.0f
	{{0xC00BFFFFFFFFFFFFULL}, {0xC010000000000000ULL}}, // -3.499f,-4.0f
	{{0xC008000000000000ULL}, {0xC008000000000000ULL}}, // -3.0f,-3.0f
	{{0xC007FFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -2.999f,-3.0f
	{{0xC004000000000000ULL}, {0xC008000000000000ULL}}, // -2.5f,-3.0f
	{{0xC003FFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -2.499f,-3.0f
	{{0xC000000000000000ULL}, {0xC000000000000000ULL}}, // -2.0f,-2.0f
	{{0xBFFFFFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -1.999f,-2.0f
	{{0xBFF8000000000000ULL}, {0xC000000000000000ULL}}, // -1.5f,-2.0f
	{{0xBFF7FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -1.499f,-2.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0xBFEFFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -0.999f,-1.0f
	{{0xBFE0000000000000ULL}, {0xBFF0000000000000ULL}}, // -0.5f,-1.0f
	{{0xBFDFFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -0.499f,-1.0f
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}}, // -0.0f,0.0f

	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x3FDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // 0.499f,0.0f
	{{0x3FE0000000000000ULL}, {0x0000000000000000ULL}}, // 0.5f,0.0f
	{{0x3FEFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // 0.999f,0.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0x3FF7FFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 1.499f,1.0f
	{{0x3FF8000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.5f,1.0f
	{{0x3FFFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 1.999f,1.0f
	{{0x4000000000000000ULL}, {0x4000000000000000ULL}}, // 2.0f,2.0f
	{{0x4003FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 2.499f,2.0f
	{{0x4004000000000000ULL}, {0x4000000000000000ULL}}, // 2.5f,2.0f
	{{0x4007FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 2.999f,2.0f
	{{0x4008000000000000ULL}, {0x4008000000000000ULL}}, // 3.0f,3.0f
	{{0x400BFFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 3.499f,3.0f
	{{0x400C000000000000ULL}, {0x4008000000000000ULL}}, // 3.5f,3.0f
	{{0x400FFFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 3.999f,3.0f

	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x8000000000000000ULL}, {0x8000000000000000ULL}}, // -0.0f,-0.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0x3FF199999999999AULL}, {0x3FF0000000000000ULL}}, // 1.1f,1.0f
	{{0x3FFF333333333333ULL}, {0x3FF0000000000000ULL}}, // 1.95f,1.0f
	{{0x4000CCCCCCCCCCCDULL}, {0x4000000000000000ULL}}, // 2.1f,2.0f
	{{0x400799999999999aULL}, {0x4000000000000000ULL}}, // 2.95f,2.0f
	{{0xBFF199999999999AULL}, {0xC000000000000000ULL}}, // -1.1f,-2.0f
	{{0xBFFF333333333333ULL}, {0xC000000000000000ULL}}, // -1.95f,-2.0f
	{{0xC000CCCCCCCCCCCDULL}, {0xC008000000000000ULL}}, // -2.1f,-3.0f
	{{0xC00799999999999aULL}, {0xC008000000000000ULL}}, // -2.95f,-3.0f
	{{0x3FB999999999999AULL}, {0x0000000000000000ULL}}, // 0.1f,0.0f
	{{0x3FEE666666666666ULL}, {0x0000000000000000ULL}}, // 0.95f,0.0f
	{{0xBFB999999999999AULL}, {0xBFF0000000000000ULL}}, // -0.1f,-1.0f
	{{0xBFEE666666666666ULL}, {0xBFF0000000000000ULL}}, // -0.95f,-1.0f
	// 4503599627370496.0f,4503599627370496.0f
	{{0x4330000000000000ULL}, {0x4330000000000000ULL}},
	// -4503599627370496.0f,-4503599627370496.0f
	{{0xC330000000000000ULL}, {0xC330000000000000ULL}},
	// 45035996273704960.0f,45035996273704960.0f
	{{0x4364000000000000ULL}, {0x4364000000000000ULL}},
	// -45035996273704960.0f,-45035996273704960.0f
	{{0xC364000000000000ULL}, {0xC364000000000000ULL}},
	// 450359962737049600.0f,450359962737049600.0f
	{{0x4399000000000000ULL}, {0x4399000000000000ULL}},
	// -450359962737049600.0f,-450359962737049600.0f
	{{0xC399000000000000ULL}, {0xC399000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF
	{{0x43E0000000000000ULL}, {0x43E0000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{0x43DFFFFFFFFFFFFFULL}, {0x43DFFFFFFFFFFFFFULL}},
	{{0xC3E0000000000000ULL}, {0xC3E0000000000000ULL}}, // 0x800000000000000
	{{0x0000000000000001ULL}, {0x0000000000000000ULL}}, // Min Denormal
	{{0x0000000000000010ULL}, {0x0000000000000000ULL}},
	{{0x0008000000000000ULL}, {0x0000000000000000ULL}},
	{{0x000FFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // Max Denormal
	{{0x0010000000000000ULL}, {0x0000000000000000ULL}}, // FLT_MIN
	{{0x7FEFFFFFFFFFFFFFULL}, {0x7FEFFFFFFFFFFFFFULL}}, // FLT_MAX
	{{0x8000000000000001ULL}, {0xBFF0000000000000ULL}}, // -Min Denormal
	{{0x8000000000000010ULL}, {0xBFF0000000000000ULL}},
	{{0x8008000000000000ULL}, {0xBFF0000000000000ULL}},
	{{0x800FFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -Max Denormal
	{{0x8010000000000000ULL}, {0xBFF0000000000000ULL}}, // -FLT_MIN
	{{0xFFEFFFFFFFFFFFFFULL}, {0xFFEFFFFFFFFFFFFFULL}}  // -FLT_MAX
};

static uint_t TestFloorDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = FloorDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(FloorDoubleArray);
	uint_t uResult = FALSE;
	do {
		const double dOriginal = pWork[0];
		const double dTest = Burger::get_floor(dOriginal);
		const double dExpected = pWork[1];
		const uint_t uFailure = (dTest != dExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_floor((double)%.16g) = %.16g / Wanted %.16g",
			uFailure, dOriginal, dTest, dExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_ceiling(float)
//

static const Burger::uint32_float_t CeilFloatArray[][2] = {
	{{0xC07FFFFFU}, {0xC0400000U}}, // -3.999f,-3.0f
	{{0xC0600000U}, {0xC0400000U}}, // -3.5f,-3.0f
	{{0xC05FFFFFU}, {0xC0400000U}}, // -3.499f,-3.0f
	{{0xC0400000U}, {0xC0400000U}}, // -3.0f,-3.0f
	{{0xC03FFFFFU}, {0xC0000000U}}, // -2.999f,-2.0f
	{{0xC0200000U}, {0xC0000000U}}, // -2.5f,-2.0f
	{{0xC01FFFFFU}, {0xC0000000U}}, // -2.499f,-2.0f
	{{0xC0000000U}, {0xC0000000U}}, // -2.0f,-2.0f
	{{0xBFFFFFFFU}, {0xBF800000U}}, // -1.999f,-1.0f
	{{0xBFC00000U}, {0xBF800000U}}, // -1.5f,-1.0f
	{{0xBFBFFFFFU}, {0xBF800000U}}, // -1.499f,-1.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0xBF7FFFFFU}, {0x00000000U}}, // -0.999f,-0.0f
	{{0xBF000000U}, {0x00000000U}}, // -0.5f,-0.0f
	{{0xBEFFFFFFU}, {0x00000000U}}, // -0.499f,0.0f
	{{0x80000000U}, {0x00000000U}}, // -0.0f,0.0f

	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x3EFFFFFFU}, {0x3F800000U}}, // 0.499f,1.0f
	{{0x3F000000U}, {0x3F800000U}}, // 0.5f,1.0f
	{{0x3F7FFFFFU}, {0x3F800000U}}, // 0.999f,1.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0x3FBFFFFFU}, {0x40000000U}}, // 1.499f,2.0f
	{{0x3FC00000U}, {0x40000000U}}, // 1.5f,2.0f
	{{0x3FFFFFFFU}, {0x40000000U}}, // 1.999f,2.0f
	{{0x40000000U}, {0x40000000U}}, // 2.0f,2.0f
	{{0x401FFFFFU}, {0x40400000U}}, // 2.499f,3.0f
	{{0x40200000U}, {0x40400000U}}, // 2.5f,3.0f
	{{0x403FFFFFU}, {0x40400000U}}, // 2.999f,3.0f
	{{0x40400000U}, {0x40400000U}}, // 3.0f,3.0f
	{{0x405FFFFFU}, {0x40800000U}}, // 3.499f,4.0f
	{{0x40600000U}, {0x40800000U}}, // 3.5f,4.0f
	{{0x407FFFFFU}, {0x40800000U}}, // 3.999f,4.0f

	{{0x3F8CCCCDU}, {0x40000000U}}, // 1.1f,2.0f
	{{0x3FF9999AU}, {0x40000000U}}, // 1.95f,2.0f
	{{0x40066666U}, {0x40400000U}}, // 2.1f,3.0f
	{{0x403CCCCDU}, {0x40400000U}}, // 2.95f,3.0f
	{{0xBF8CCCCDU}, {0xBF800000U}}, // -1.1f,-1.0f
	{{0xBFF9999AU}, {0xBF800000U}}, // -1.95f,-1.0f
	{{0xC0066666U}, {0xC0000000U}}, // -2.1f,-2.0f
	{{0xC03CCCCDU}, {0xC0000000U}}, // -2.95f,-2.0f
	{{0x3DCCCCCDU}, {0x3F800000U}}, // 0.1f,1.0f
	{{0x3F733333U}, {0x3F800000U}}, // 0.95f,1.0f
	{{0xBDCCCCCDU}, {0x00000000U}}, // -0.1f,0.0f
	{{0xBF7333BDU}, {0x00000000U}}, // -0.95f,0.0f
	{{0x4B000000U}, {0x4B000000U}}, // 8388608.0f,8388608.0f
	{{0xCB000000U}, {0xCB000000U}}, // -8388608.0f,-8388608.0f
	{{0x4CA00000U}, {0x4CA00000U}}, // 83886080.0f,83886080.0f
	{{0xCCA00000U}, {0xCCA00000U}}, // -83886080.0f,-83886080.0f
	{{0x4E480000U}, {0x4E480000U}}, // 838860800.0f,838860800.0f
	{{0xCE480000U}, {0xCE480000U}}, // -838860800.0f,-838860800.0f
	{{0x4F000000U}, {0x4F000000U}}, // 0x7FFFFFF
	{{0x4EFFFFFFU}, {0x4EFFFFFFU}}, // 0x7FFFFFF-Epsilon
	{{0xCF000000U}, {0xCF000000U}}, // 0x8000000
	{{0x00000001U}, {0x3F800000U}}, // Min Denormal
	{{0x00000010U}, {0x3F800000U}}, {{0x00400000U}, {0x3F800000U}},
	{{0x007FFFFFU}, {0x3F800000U}}, // Max Denormal
	{{0x00800000U}, {0x3F800000U}}, // FLT_MIN
	{{0x7F7FFFFFU}, {0x7F7FFFFFU}}, // FLT_MAX
	{{0x80000001U}, {0x00000000U}}, // -Min Denormal
	{{0x80000010U}, {0x00000000U}}, {{0x80400000U}, {0x00000000U}},
	{{0x807FFFFFU}, {0x00000000U}}, // -Max Denormal
	{{0x80800000U}, {0x00000000U}}, // -FLT_MIN
	{{0xFF7FFFFFU}, {0xFF7FFFFFU}}  // -FLT_MAX
};

static uint_t TestCeilFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = CeilFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(CeilFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		volatile float fTest = Burger::get_ceiling(fOriginal);
		const float fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_ceiling(%.16g) = %.16g / Wanted %.16g", uFailure,
			fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_ceiling(double)
//

static const Burger::uint64_double_t CeilDoubleArray[][2] = {
	{{0xC00FFFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -3.999f,-3.0f
	{{0xC00C000000000000ULL}, {0xC008000000000000ULL}}, // -3.5f,-3.0f
	{{0xC00BFFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -3.499f,-3.0f
	{{0xC008000000000000ULL}, {0xC008000000000000ULL}}, // -3.0f,-3.0f
	{{0xC007FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -2.999f,-2.0f
	{{0xC004000000000000ULL}, {0xC000000000000000ULL}}, // -2.5f,-2.0f
	{{0xC003FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -2.499f,-2.0f
	{{0xC000000000000000ULL}, {0xC000000000000000ULL}}, // -2.0f,-2.0f
	{{0xBFFFFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -1.999f,-1.0f
	{{0xBFF8000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.5f,-1.0f
	{{0xBFF7FFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -1.499f,-1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0xBFEFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -0.999f,-0.0f
	{{0xBFE0000000000000ULL}, {0x0000000000000000ULL}}, // -0.5f,-0.0f
	{{0xBFDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -0.499f,-0.0f
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}}, // -0.0f,0.0f

	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x3FDFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 0.499f,1.0f
	{{0x3FE0000000000000ULL}, {0x3FF0000000000000ULL}}, // 0.5f,1.0f
	{{0x3FEFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 0.999f,1.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0x3FF7FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 1.499f,2.0f
	{{0x3FF8000000000000ULL}, {0x4000000000000000ULL}}, // 1.5f,2.0f
	{{0x3FFFFFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 1.999f,2.0f
	{{0x4000000000000000ULL}, {0x4000000000000000ULL}}, // 2.0f,2.0f
	{{0x4003FFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 2.499f,3.0f
	{{0x4004000000000000ULL}, {0x4008000000000000ULL}}, // 2.5f,3.0f
	{{0x4007FFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 2.999f,3.0f
	{{0x4008000000000000ULL}, {0x4008000000000000ULL}}, // 3.0f,3.0f
	{{0x400BFFFFFFFFFFFFULL}, {0x4010000000000000ULL}}, // 3.499f,4.0f
	{{0x400C000000000000ULL}, {0x4010000000000000ULL}}, // 3.5f,4.0f
	{{0x400FFFFFFFFFFFFFULL}, {0x4010000000000000ULL}}, // 3.999f,4.0f

	{{0x3FF199999999999AULL}, {0x4000000000000000ULL}}, // 1.1f,2.0f
	{{0x3FFF333333333333ULL}, {0x4000000000000000ULL}}, // 1.95f,2.0f
	{{0x4000CCCCCCCCCCCDULL}, {0x4008000000000000ULL}}, // 2.1f,3.0f
	{{0x400799999999999aULL}, {0x4008000000000000ULL}}, // 2.95f,3.0f
	{{0xBFF199999999999AULL}, {0xBFF0000000000000ULL}}, // -1.1f,-1.0f
	{{0xBFFF333333333333ULL}, {0xBFF0000000000000ULL}}, // -1.95f,-1.0f
	{{0xC000CCCCCCCCCCCDULL}, {0xC000000000000000ULL}}, // -2.1f,-2.0f
	{{0xC00799999999999aULL}, {0xC000000000000000ULL}}, // -2.95f,-2.0f
	{{0x3FB999999999999AULL}, {0x3FF0000000000000ULL}}, // 0.1f,1.0f
	{{0x3FEE666666666666ULL}, {0x3FF0000000000000ULL}}, // 0.95f,1.0f
	{{0xBFB999999999999AULL}, {0x0000000000000000ULL}}, // -0.1f,0.0f
	{{0xBFEE666666666666ULL}, {0x0000000000000000ULL}}, // -0.95f,0.0f
	// 4503599627370496.0f,4503599627370496.0f
	{{0x4330000000000000ULL}, {0x4330000000000000ULL}},
	// -4503599627370496.0f,-4503599627370496.0f
	{{0xC330000000000000ULL}, {0xC330000000000000ULL}},
	// 45035996273704960.0f,45035996273704960.0f
	{{0x4364000000000000ULL}, {0x4364000000000000ULL}},
	// -45035996273704960.0f,-45035996273704960.0f
	{{0xC364000000000000ULL}, {0xC364000000000000ULL}},
	// 450359962737049600.0f,450359962737049600.0f
	{{0x4399000000000000ULL}, {0x4399000000000000ULL}},
	// -450359962737049600.0f,-450359962737049600.0f
	{{0xC399000000000000ULL}, {0xC399000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF
	{{0x43E0000000000000ULL}, {0x43E0000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{0x43DFFFFFFFFFFFFFULL}, {0x43DFFFFFFFFFFFFFULL}},
	{{0xC3E0000000000000ULL}, {0xC3E0000000000000ULL}}, // 0x800000000000000
	{{0x0000000000000001ULL}, {0x3FF0000000000000ULL}}, // Min Denormal
	{{0x0000000000000010ULL}, {0x3FF0000000000000ULL}},
	{{0x0008000000000000ULL}, {0x3FF0000000000000ULL}},
	{{0x000FFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // Max Denormal
	{{0x0010000000000000ULL}, {0x3FF0000000000000ULL}}, // FLT_MIN
	{{0x7FEFFFFFFFFFFFFFULL}, {0x7FEFFFFFFFFFFFFFULL}}, // FLT_MAX
	{{0x8000000000000001ULL}, {0x0000000000000000ULL}}, // -Min Denormal
	{{0x8000000000000010ULL}, {0x0000000000000000ULL}},
	{{0x8008000000000000ULL}, {0x0000000000000000ULL}},
	{{0x800FFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -Max Denormal
	{{0x8010000000000000ULL}, {0x0000000000000000ULL}}, // -FLT_MIN
	{{0xFFEFFFFFFFFFFFFFULL}, {0xFFEFFFFFFFFFFFFFULL}}  // -FLT_MAX
};

static uint_t TestCeilDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = CeilDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(CeilDoubleArray);
	uint_t uResult = FALSE;
	do {
		const double fOriginal = pWork[0];
		const double fTest = Burger::get_ceiling(fOriginal);
		const double fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_ceiling((double)%.16g) = %.16g / Wanted %.16g",
			uFailure, fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_round(float)
//

static const Burger::uint32_float_t RoundFloatArray[][2] = {
	{{0xC07FFFFFU}, {0xC0800000U}}, // -3.999f,-4.0f
	{{0xC0600000U}, {0xC0800000U}}, // -3.5f,-4.0f
	{{0xC05FFFFFU}, {0xC0400000U}}, // -3.499f,-3.0f
	{{0xC0400000U}, {0xC0400000U}}, // -3.0f,-3.0f
	{{0xC03FFFFFU}, {0xC0400000U}}, // -2.999f,-3.0f
	{{0xC0200000U}, {0xC0400000U}}, // -2.5f,-3.0f
	{{0xC01FFFFFU}, {0xC0000000U}}, // -2.499f,-2.0f
	{{0xC0000000U}, {0xC0000000U}}, // -2.0f,-2.0f
	{{0xBFFFFFFFU}, {0xC0000000U}}, // -1.999f,-2.0f
	{{0xBFC00000U}, {0xC0000000U}}, // -1.5f,-2.0f
	{{0xBFBFFFFFU}, {0xBF800000U}}, // -1.499f,-1.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0xBF7FFFFFU}, {0xBF800000U}}, // -0.999f,-1.0f
	{{0xBF000000U}, {0xBF800000U}}, // -0.5f,-1.0f
	{{0xBEFFFFFFU}, {0x00000000U}}, // -0.499f,0.0f
	{{0x80000000U}, {0x00000000U}}, // -0.0f,0.0f

	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x3EFFFFFFU}, {0x00000000U}}, // 0.499f,0.0f
	{{0x3F000000U}, {0x3F800000U}}, // 0.5f,1.0f
	{{0x3F7FFFFFU}, {0x3F800000U}}, // 0.999f,1.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0x3FBFFFFFU}, {0x3F800000U}}, // 1.499f,1.0f
	{{0x3FC00000U}, {0x40000000U}}, // 1.5f,2.0f
	{{0x3FFFFFFFU}, {0x40000000U}}, // 1.999f,2.0f
	{{0x40000000U}, {0x40000000U}}, // 2.0f,2.0f
	{{0x401FFFFFU}, {0x40000000U}}, // 2.499f,2.0f
	{{0x40200000U}, {0x40400000U}}, // 2.5f,3.0f
	{{0x403FFFFFU}, {0x40400000U}}, // 2.999f,3.0f
	{{0x40400000U}, {0x40400000U}}, // 3.0f,3.0f
	{{0x405FFFFFU}, {0x40400000U}}, // 3.499f,3.0f
	{{0x40600000U}, {0x40800000U}}, // 3.5f,4.0f
	{{0x407FFFFFU}, {0x40800000U}}, // 3.999f,4.0f

	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x80000000U}, {0x80000000U}}, // -0.0f,-0.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0x3F8CCCCDU}, {0x3F800000U}}, // 1.1f,1.0f
	{{0x3FF9999AU}, {0x40000000U}}, // 1.95f,2.0f
	{{0x40066666U}, {0x40000000U}}, // 2.1f,3.0f
	{{0x403CCCCDU}, {0x40400000U}}, // 2.95f,3.0f
	{{0xBF8CCCCDU}, {0xBF800000U}}, // -1.1f,-1.0f
	{{0xBFF9999AU}, {0xC0000000U}}, // -1.95f,-2.0f
	{{0xC0066666U}, {0xC0000000U}}, // -2.1f,3.0f
	{{0xC03CCCCDU}, {0xC0400000U}}, // -2.95f,3.0f
	{{0x3DCCCCCDU}, {0x00000000U}}, // 0.1f,0.0f
	{{0x3F733333U}, {0x3F800000U}}, // 0.95f,1.0f
	{{0xBDCCCCCDU}, {0x00000000U}}, // -0.1f,0.0f
	{{0xBF7333BDU}, {0xBF800000U}}, // -0.95f,-1.0f
	{{0x4B000000U}, {0x4B000000U}}, // 8388608.0f,8388608.0f
	{{0xCB000000U}, {0xCB000000U}}, // -8388608.0f,-8388608.0f
	{{0x4CA00000U}, {0x4CA00000U}}, // 83886080.0f,83886080.0f
	{{0xCCA00000U}, {0xCCA00000U}}, // -83886080.0f,-83886080.0f
	{{0x4E480000U}, {0x4E480000U}}, // 838860800.0f,838860800.0f
	{{0xCE480000U}, {0xCE480000U}}, // -838860800.0f,-838860800.0f
	{{0x4F000000U}, {0x4F000000U}}, // 0x7FFFFFF
	{{0x4EFFFFFFU}, {0x4EFFFFFFU}}, // 0x7FFFFFF-Epsilon
	{{0xCF000000U}, {0xCF000000U}}, // 0x8000000
	{{0x00000001U}, {0x00000000U}}, // Min Denormal
	{{0x00000010U}, {0x00000000U}}, {{0x00400000U}, {0x00000000U}},
	{{0x007FFFFFU}, {0x00000000U}}, // Max Denormal
	{{0x00800000U}, {0x00000000U}}, // FLT_MIN
	{{0x7F7FFFFFU}, {0x7F7FFFFFU}}, // FLT_MAX
	{{0x80000001U}, {0x00000000U}}, // -Min Denormal
	{{0x80000010U}, {0x00000000U}}, {{0x80400000U}, {0x00000000U}},
	{{0x807FFFFFU}, {0x00000000U}}, // -Max Denormal
	{{0x80800000U}, {0x00000000U}}, // -FLT_MIN
	{{0xFF7FFFFFU}, {0xFF7FFFFFU}}  // -FLT_MAX
};

static uint_t TestRoundFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = RoundFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(RoundFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		volatile float fTest = Burger::get_round(fOriginal);
		const float fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure("Burger::get_round((float)%.16g) = %.16g / Wanted %.16g",
			uFailure, fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_round(double)
//

static const Burger::uint64_double_t RoundDoubleArray[][2] = {
	{{0xC00FFFFFFFFFFFFFULL}, {0xC010000000000000ULL}}, // -3.999f,-4.0f
	{{0xC00C000000000000ULL}, {0xC010000000000000ULL}}, // -3.5f,-4.0f
	{{0xC00BFFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -3.499f,-3.0f
	{{0xC008000000000000ULL}, {0xC008000000000000ULL}}, // -3.0f,-3.0f
	{{0xC007FFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -2.999f,-3.0f
	{{0xC004000000000000ULL}, {0xC008000000000000ULL}}, // -2.5f,-3.0f
	{{0xC003FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -2.499f,-2.0f
	{{0xC000000000000000ULL}, {0xC000000000000000ULL}}, // -2.0f,-2.0f
	{{0xBFFFFFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -1.999f,-2.0f
	{{0xBFF8000000000000ULL}, {0xC000000000000000ULL}}, // -1.5f,-2.0f
	{{0xBFF7FFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -1.499f,-1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0xBFEFFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -0.999f,-1.0f
	{{0xBFE0000000000000ULL}, {0xBFF0000000000000ULL}}, // -0.5f,-1.0f
	{{0xBFDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -0.499f,-0.0f
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}}, // -0.0f,0.0f

	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x3FDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // 0.499f,0.0f
	{{0x3FE0000000000000ULL}, {0x3FF0000000000000ULL}}, // 0.5f,1.0f
	{{0x3FEFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 0.999f,1.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0x3FF7FFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 1.499f,1.0f
	{{0x3FF8000000000000ULL}, {0x4000000000000000ULL}}, // 1.5f,2.0f
	{{0x3FFFFFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 1.999f,2.0f
	{{0x4000000000000000ULL}, {0x4000000000000000ULL}}, // 2.0f,2.0f
	{{0x4003FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 2.499f,2.0f
	{{0x4004000000000000ULL}, {0x4008000000000000ULL}}, // 2.5f,3.0f
	{{0x4007FFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 2.999f,3.0f
	{{0x4008000000000000ULL}, {0x4008000000000000ULL}}, // 3.0f,3.0f
	{{0x400BFFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 3.499f,3.0f
	{{0x400C000000000000ULL}, {0x4010000000000000ULL}}, // 3.5f,4.0f
	{{0x400FFFFFFFFFFFFFULL}, {0x4010000000000000ULL}}, // 3.999f,4.0f

	{{0x3FF199999999999AULL}, {0x3FF0000000000000ULL}}, // 1.1f,1.0f
	{{0x3FFF333333333333ULL}, {0x4000000000000000ULL}}, // 1.95f,2.0f
	{{0x4000CCCCCCCCCCCDULL}, {0x4000000000000000ULL}}, // 2.1f,3.0f
	{{0x400799999999999aULL}, {0x4008000000000000ULL}}, // 2.95f,3.0f
	{{0xBFF199999999999AULL}, {0xBFF0000000000000ULL}}, // -1.1f,-1.0f
	{{0xBFFF333333333333ULL}, {0xC000000000000000ULL}}, // -1.95f,-2.0f
	{{0xC000CCCCCCCCCCCDULL}, {0xC000000000000000ULL}}, // -2.1f,-2.0f
	{{0xC00799999999999aULL}, {0xC008000000000000ULL}}, // -2.95f,-3.0f
	{{0x3FB999999999999AULL}, {0x0000000000000000ULL}}, // 0.1f,0.0f
	{{0x3FEE666666666666ULL}, {0x3FF0000000000000ULL}}, // 0.95f,1.0f
	{{0xBFB999999999999AULL}, {0x0000000000000000ULL}}, // -0.1f,0.0f
	{{0xBFEE666666666666ULL}, {0xBFF0000000000000ULL}}, // -0.95f,-1.0f
	// 4503599627370496.0f,4503599627370496.0f
	{{0x4330000000000000ULL}, {0x4330000000000000ULL}},
	// -4503599627370496.0f,-4503599627370496.0f
	{{0xC330000000000000ULL}, {0xC330000000000000ULL}},
	// 45035996273704960.0f,45035996273704960.0f
	{{0x4364000000000000ULL}, {0x4364000000000000ULL}},
	// -45035996273704960.0f,-45035996273704960.0f
	{{0xC364000000000000ULL}, {0xC364000000000000ULL}},
	// 450359962737049600.0f,450359962737049600.0f
	{{0x4399000000000000ULL}, {0x4399000000000000ULL}},
	// -450359962737049600.0f,-450359962737049600.0f
	{{0xC399000000000000ULL}, {0xC399000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF
	{{0x43E0000000000000ULL}, {0x43E0000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{0x43DFFFFFFFFFFFFFULL}, {0x43DFFFFFFFFFFFFFULL}},
	{{0xC3E0000000000000ULL}, {0xC3E0000000000000ULL}}, // 0x800000000000000
	{{0x0000000000000001ULL}, {0x0000000000000000ULL}}, // Min Denormal
	{{0x0000000000000010ULL}, {0x0000000000000000ULL}},
	{{0x0008000000000000ULL}, {0x0000000000000000ULL}},
	{{0x000FFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // Max Denormal
	{{0x0010000000000000ULL}, {0x0000000000000000ULL}}, // FLT_MIN
	{{0x7FEFFFFFFFFFFFFFULL}, {0x7FEFFFFFFFFFFFFFULL}}, // FLT_MAX
	{{0x8000000000000001ULL}, {0x0000000000000000ULL}}, // -Min Denormal
	{{0x8000000000000010ULL}, {0x0000000000000000ULL}},
	{{0x8008000000000000ULL}, {0x0000000000000000ULL}},
	{{0x800FFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -Max Denormal
	{{0x8010000000000000ULL}, {0x0000000000000000ULL}}, // -FLT_MIN
	{{0xFFEFFFFFFFFFFFFFULL}, {0xFFEFFFFFFFFFFFFFULL}}  // -FLT_MAX
};

static uint_t TestRoundDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = RoundDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(RoundDoubleArray);
	uint_t uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		Burger::uint64_double_t dTest;
		dTest.d = Burger::get_round(pWork[0]);
		const uint_t uFailure = (dTest.d != pWork[1].d);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(dTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::get_round((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",
				uFailure, pWork[0].d, dTest.d, Test.c_str(), pWork[1].d,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test round_to_zero(float)
//

static const Burger::uint32_float_t RoundToZeroFloatArray[][2] = {
	{{0xC07FFFFFU}, {0xC0400000U}}, // -3.999f,-3.0f
	{{0xC0600000U}, {0xC0400000U}}, // -3.5f,-3.0f
	{{0xC05FFFFFU}, {0xC0400000U}}, // -3.499f,-3.0f
	{{0xC0400000U}, {0xC0400000U}}, // -3.0f,-3.0f
	{{0xC03FFFFFU}, {0xC0000000U}}, // -2.999f,-2.0f
	{{0xC0200000U}, {0xC0000000U}}, // -2.5f,-2.0f
	{{0xC01FFFFFU}, {0xC0000000U}}, // -2.499f,-2.0f
	{{0xC0000000U}, {0xC0000000U}}, // -2.0f,-2.0f
	{{0xBFFFFFFFU}, {0xBF800000U}}, // -1.999f,-1.0f
	{{0xBFC00000U}, {0xBF800000U}}, // -1.5f,-1.0f
	{{0xBFBFFFFFU}, {0xBF800000U}}, // -1.499f,-1.0f
	{{0xBF800000U}, {0xBF800000U}}, // -1.0f,-1.0f
	{{0xBF7FFFFFU}, {0x00000000U}}, // -0.999f,-0.0f
	{{0xBF000000U}, {0x00000000U}}, // -0.5f,-0.0f
	{{0xBEFFFFFFU}, {0x00000000U}}, // -0.499f,0.0f
	{{0x80000000U}, {0x00000000U}}, // -0.0f,0.0f

	{{0x00000000U}, {0x00000000U}}, // 0.0f,0.0f
	{{0x3EFFFFFFU}, {0x00000000U}}, // 0.499f,0.0f
	{{0x3F000000U}, {0x00000000U}}, // 0.5f,0.0f
	{{0x3F7FFFFFU}, {0x00000000U}}, // 0.999f,0.0f
	{{0x3F800000U}, {0x3F800000U}}, // 1.0f,1.0f
	{{0x3FBFFFFFU}, {0x3F800000U}}, // 1.499f,1.0f
	{{0x3FC00000U}, {0x3F800000U}}, // 1.5f,1.0f
	{{0x3FFFFFFFU}, {0x3F800000U}}, // 1.999f,1.0f
	{{0x40000000U}, {0x40000000U}}, // 2.0f,2.0f
	{{0x401FFFFFU}, {0x40000000U}}, // 2.499f,2.0f
	{{0x40200000U}, {0x40000000U}}, // 2.5f,2.0f
	{{0x403FFFFFU}, {0x40000000U}}, // 2.999f,2.0f
	{{0x40400000U}, {0x40400000U}}, // 3.0f,3.0f
	{{0x405FFFFFU}, {0x40400000U}}, // 3.499f,3.0f
	{{0x40600000U}, {0x40400000U}}, // 3.5f,3.0f
	{{0x407FFFFFU}, {0x40400000U}}, // 3.999f,3.0f

	{{0x3F8CCCCDU}, {0x3F800000U}}, // 1.1f,1.0f
	{{0x3FF9999AU}, {0x3F800000U}}, // 1.95f,1.0f
	{{0xBF8CCCCDU}, {0xBF800000U}}, // -1.1f,-1.0f
	{{0xBFF9999AU}, {0xBF800000U}}, // -1.95f,-1.0f
	{{0x3DCCCCCDU}, {0x00000000U}}, // 0.1f,0.0f
	{{0x3F733333U}, {0x00000000U}}, // 0.95f,0.0f
	{{0xBDCCCCCDU}, {0x00000000U}}, // -0.1f,0.0f
	{{0xBF7333BDU}, {0x00000000U}}, // -0.95f,0.0f
	{{0x4B000000U}, {0x4B000000U}}, // 8388608.0f,8388608.0f
	{{0xCB000000U}, {0xCB000000U}}, // -8388608.0f,-8388608.0f
	{{0x4CA00000U}, {0x4CA00000U}}, // 83886080.0f,83886080.0f
	{{0xCCA00000U}, {0xCCA00000U}}, // -83886080.0f,-83886080.0f
	{{0x4E480000U}, {0x4E480000U}}, // 838860800.0f,838860800.0f
	{{0xCE480000U}, {0xCE480000U}}, // -838860800.0f,-838860800.0f
	{{0x4F000000U}, {0x4F000000U}}, // 0x7FFFFFF
	{{0x4EFFFFFFU}, {0x4EFFFFFFU}}, // 0x7FFFFFF-Epsilon
	{{0xCF000000U}, {0xCF000000U}}  // 0x8000000
};

static uint_t TestRoundToZeroFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = RoundToZeroFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(RoundToZeroFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		volatile float fTest = Burger::round_to_zero(fOriginal);
		const float fExpected = pWork[1];
		const uint_t uFailure = (fTest != fExpected);
		uResult |= uFailure;
		ReportFailure(
			"Burger::round_to_zero((float)%.16g) = %.16g / Wanted %.16g",
			uFailure, fOriginal, fTest, fExpected);
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test round_to_zero(double)
//

static const Burger::uint64_double_t RoundToZeroDoubleArray[][2] = {
	{{0xC00FFFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -3.999f,-3.0f
	{{0xC00C000000000000ULL}, {0xC008000000000000ULL}}, // -3.5f,-3.0f
	{{0xC00BFFFFFFFFFFFFULL}, {0xC008000000000000ULL}}, // -3.499f,-3.0f
	{{0xC008000000000000ULL}, {0xC008000000000000ULL}}, // -3.0f,-3.0f
	{{0xC007FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -2.999f,-2.0f
	{{0xC004000000000000ULL}, {0xC000000000000000ULL}}, // -2.5f,-2.0f
	{{0xC003FFFFFFFFFFFFULL}, {0xC000000000000000ULL}}, // -2.499f,-2.0f
	{{0xC000000000000000ULL}, {0xC000000000000000ULL}}, // -2.0f,-2.0f
	{{0xBFFFFFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -1.999f,-1.0f
	{{0xBFF8000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.5f,-1.0f
	{{0xBFF7FFFFFFFFFFFFULL}, {0xBFF0000000000000ULL}}, // -1.499f,-1.0f
	{{0xBFF0000000000000ULL}, {0xBFF0000000000000ULL}}, // -1.0f,-1.0f
	{{0xBFEFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -0.999f,-0.0f
	{{0xBFE0000000000000ULL}, {0x0000000000000000ULL}}, // -0.5f,-0.0f
	{{0xBFDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // -0.499f,-0.0f
	{{0x8000000000000000ULL}, {0x0000000000000000ULL}}, // -0.0f,0.0f

	{{0x0000000000000000ULL}, {0x0000000000000000ULL}}, // 0.0f,0.0f
	{{0x3FDFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // 0.499f,0.0f
	{{0x3FE0000000000000ULL}, {0x0000000000000000ULL}}, // 0.5f,1.0f
	{{0x3FEFFFFFFFFFFFFFULL}, {0x0000000000000000ULL}}, // 0.999f,1.0f
	{{0x3FF0000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.0f,1.0f
	{{0x3FF7FFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 1.499f,1.0f
	{{0x3FF8000000000000ULL}, {0x3FF0000000000000ULL}}, // 1.5f,2.0f
	{{0x3FFFFFFFFFFFFFFFULL}, {0x3FF0000000000000ULL}}, // 1.999f,2.0f
	{{0x4000000000000000ULL}, {0x4000000000000000ULL}}, // 2.0f,2.0f
	{{0x4003FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 2.499f,2.0f
	{{0x4004000000000000ULL}, {0x4000000000000000ULL}}, // 2.5f,2.0f
	{{0x4007FFFFFFFFFFFFULL}, {0x4000000000000000ULL}}, // 2.999f,2.0f
	{{0x4008000000000000ULL}, {0x4008000000000000ULL}}, // 3.0f,3.0f
	{{0x400BFFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 3.499f,3.0f
	{{0x400C000000000000ULL}, {0x4008000000000000ULL}}, // 3.5f,3.0f
	{{0x400FFFFFFFFFFFFFULL}, {0x4008000000000000ULL}}, // 3.999f,3.0f

	{{0x3FF199999999999AULL}, {0x3FF0000000000000ULL}}, // 1.1f,1.0f
	{{0x3FFF333333333333ULL}, {0x3FF0000000000000ULL}}, // 1.95f,1.0f
	{{0xBFF199999999999AULL}, {0xBFF0000000000000ULL}}, // -1.1f,-1.0f
	{{0xBFFF333333333333ULL}, {0xBFF0000000000000ULL}}, // -1.95f,-1.0f
	{{0x3FB999999999999AULL}, {0x0000000000000000ULL}}, // 0.1f,0.0f
	{{0x3FEE666666666666ULL}, {0x0000000000000000ULL}}, // 0.95f,0.0f
	{{0xBFB999999999999AULL}, {0x0000000000000000ULL}}, // -0.1f,0.0f
	{{0xBFEE666666666666ULL}, {0x0000000000000000ULL}}, // -0.95f,0.0f
	// 4503599627370496.0f,4503599627370496.0f
	{{0x4330000000000000ULL}, {0x4330000000000000ULL}},
	// -4503599627370496.0f,-4503599627370496.0f
	{{0xC330000000000000ULL}, {0xC330000000000000ULL}},
	// 45035996273704960.0f,45035996273704960.0f
	{{0x4364000000000000ULL}, {0x4364000000000000ULL}},
	// -45035996273704960.0f,-45035996273704960.0f
	{{0xC364000000000000ULL}, {0xC364000000000000ULL}},
	// 450359962737049600.0f,450359962737049600.0f
	{{0x4399000000000000ULL}, {0x4399000000000000ULL}},
	// -450359962737049600.0f,-450359962737049600.0f
	{{0xC399000000000000ULL}, {0xC399000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF
	{{0x43E0000000000000ULL}, {0x43E0000000000000ULL}},
	// 0x7FFFFFFFFFFFFFF-Epsilon
	{{0x43DFFFFFFFFFFFFFULL}, {0x43DFFFFFFFFFFFFFULL}},
	// 0x800000000000000
	{{0xC3E0000000000000ULL}, {0xC3E0000000000000ULL}}};

static uint_t TestRoundToZeroDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = RoundToZeroDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(RoundToZeroDoubleArray);
	uint_t uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		Burger::uint64_double_t dTest;
		dTest.d = Burger::round_to_zero(pWork[0]);
		const uint_t uFailure = (dTest.d != pWork[1].d);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(dTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::round_to_zero((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",
				uFailure, pWork[0].d, dTest.d, Test.c_str(), pWork[1].d,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test modulo_radians(float)
//

static const Burger::uint32_float_t ModuloRadiansFloatArray[][2] = {
	{{0x40490FDBU}, {0xC0490FDBU}}, // Pi,-Pi
	{{0x3FC90FDBU}, {0x3FC90FDBU}}, // .5Pi,.5Pi
	{{0x40490FDBU}, {0xC0490FDBU}}, // Pi,-Pi
	{{0x4096CBE4U}, {0xBFC90FDCU}}, // 1.5Pi,-.5Pif
	{{0x40C90FDBU}, {0x00000000U}}, // 2Pi,0.0f
	{{0x41490FDBU}, {0x00000000U}}, // 4Pi,0.0f
	{{0x42490FDBU}, {0x00000000U}}, // 16Pi,0.0f
	{{0xBFC90FDBU}, {0xBFC90FDBU}}, // -.5Pi,-.5Pi
	{{0xC0490FDBU}, {0xC0490FDBU}}, // -Pi,-Pi
	{{0xC096CBE4U}, {0x3FC90FDCU}}, // -1.5Pi,.5Pi
	{{0xC0C90FDBU}, {0x00000000U}}, // -2Pi,0.0f
	{{0xC1490FDBU}, {0x00000000U}}, // -4Pi,0.0f
	{{0xC2490FDBU}, {0x00000000U}}, // 16Pi,0.0f
	{{0x3F8CCCCDU}, {0x3F8CCCCDU}}, // 1.1f,1.1f
	{{0x00800000U}, {0x00800000U}}  // Min,Min
};

static uint_t TestModuloRadiansFloat(void) BURGER_NOEXCEPT
{
	const Burger::uint32_float_t* pWork = ModuloRadiansFloatArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(ModuloRadiansFloatArray);
	uint_t uResult = FALSE;
	do {
		const float fOriginal = pWork[0];
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		Burger::uint32_float_t fTest;
		fTest.f = Burger::modulo_radians(fOriginal);
		// uint_t uFailure = ((fTest.w^=pWork[1].w)&0x7FFFFFFFU)!=0;
		const uint_t uFailure = (fTest.w != pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(fTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::modulo_radians((float)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",
				uFailure, fOriginal, fTest.f, Test.c_str(), pWork[1].f,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test modulo_radians(double)
//

static const Burger::uint64_double_t ModuloRadiansDoubleArray[][2] = {
	{{0x3FF921FB54442D18ULL}, {0x3FF921FB54442D18ULL}}, // .5Pi,.5Pi
	{{0x400921FB54442D18ULL}, {0xC00921FB54442D18ULL}}, // Pi,-Pi
	{{0x4012D97C7F3321D2ULL}, {0xBFF921FB54442D18ULL}}, // 1.5Pi,-.5Pif
	{{0x401921FB54442D18ULL}, {0x0000000000000000ULL}}, // 2Pi,0.0f
	{{0x402921FB54442D18ULL}, {0x0000000000000000ULL}}, // 4Pi,0.0f
	{{0x404921FB54442D18ULL}, {0x0000000000000000ULL}}, // 16Pi,0.0f
	{{0x40A921FB54442D18ULL}, {0x0000000000000000ULL}}, // 1024Pi,0.0f
	{{0xBFF921FB54442D18ULL}, {0xBFF921FB54442D18ULL}}, // -.5Pi,-.5Pi
	{{0xC00921FB54442D18ULL}, {0xC00921FB54442D18ULL}}, // -Pi,-Pi
	{{0xC012D97C7F3321D2ULL}, {0x3FF921FB54442D18ULL}}, // -1.5Pi,.5Pi
	{{0xC02921FB54442D18ULL}, {0x0000000000000000ULL}}, // -2Pi,0.0f
	{{0xC02921FB54442D18ULL}, {0x0000000000000000ULL}}, // -4Pi,0.0f
	{{0xC04921FB54442D18ULL}, {0x0000000000000000ULL}}, // -16Pi,0.0f
	{{0xC0A921FB54442D18ULL}, {0x0000000000000000ULL}}, // -1024Pi,0.0f
	{{0x3FF199999999999AULL}, {0x3FF199999999999AULL}}, // 1.1,1.1
	{{0x0010000000000000ULL}, {0x0010000000000000ULL}}  // Min,Min
};

static uint_t TestModuloRadiansDouble(void) BURGER_NOEXCEPT
{
	const Burger::uint64_double_t* pWork = ModuloRadiansDoubleArray[0];
	uintptr_t i = BURGER_ARRAYSIZE(ModuloRadiansDoubleArray);
	uint_t uResult = FALSE;
	do {
		// Note: Use volatile to force the compiler to use 32 bit float
		// precision
		Burger::uint64_double_t dTest;
		dTest.d = Burger::modulo_radians(pWork[0]);
		// uint_t uFailure = ((dTest.w^=pWork[1].w)&0x7FFFFFFFFFFFFFFFULL)!=0;
		const uint_t uFailure = (dTest.w != pWork[1].w);
		uResult |= uFailure;
		if (uFailure) {
			const Burger::NumberStringHex Test(dTest.w);
			const Burger::NumberStringHex Expected(pWork[1].w);
			ReportFailure(
				"Burger::modulo_radians((double)%.16g) = %.16g 0x%s / Wanted %.16g 0x%s",
				uFailure, pWork[0].d, dTest.d, Test.c_str(), pWork[1].d,
				Expected.c_str());
		}
		pWork += 2;
	} while (--i);
	return uResult;
}

//
// Test get_sine(float)
//

static uint_t TestSinFloat(void) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int x = 0;
	do {
		const float fRadians = static_cast<float>(
			(static_cast<float>(320 - x) / 320.0f) * Burger::g_fPi * 4.0f);
		const float fSin = static_cast<float>(sin(fRadians));
		const float fXSin = Burger::get_sine(fRadians);
		const float fError = Burger::absolute(fSin - fXSin);
		const uint_t uFailure = (9.5e-07f < fError);
		uResult |= uFailure;
		ReportFailure("Burger::get_sine(%.16gf) = %.16gf / Difference %.16gf",
			uFailure, fRadians, fXSin, fSin, fError);

	} while (++x < 640);
	return uResult;
}

//
// Test get_sine(double)
//

static uint_t TestSinDouble(void) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int x = 0;
	do {
		const double fRadians =
			(static_cast<double>(320 - x) / 320.0) * Burger::g_dPi * 4.0;
		const double fSin = sin(fRadians);
		const double fXSin = Burger::get_sine(fRadians);
		const double fError = Burger::absolute(fSin - fXSin);
		const uint_t uFailure = (1.2e-15 < fError);
		uResult |= uFailure;
		ReportFailure("Burger::get_sine(%.16g) = %.16g / Difference %.16g", uFailure,
			fRadians, fXSin, fSin, fError);

	} while (++x < 640);
	return uResult;
}

//
// Test get_cosine(float)
//

static uint_t TestCosFloat(void) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int x = 0;
	do {
		const float fRadians = static_cast<float>(
			(static_cast<float>(320 - x) / 320.0f) * Burger::g_fPi * 4.0f);
		const float fCos = static_cast<float>(cos(fRadians));
		const float fXCos = Burger::get_cosine(fRadians);
		const float fError = Burger::absolute(fCos - fXCos);
		const uint_t uFailure = (7.6e-07 < fError);
		uResult |= uFailure;
		ReportFailure("Burger::get_cosine(%.16gf) = %.16gf / Difference %.16gf",
			uFailure, fRadians, fXCos, fCos, fError);

	} while (++x < 640);
	return uResult;
}

//
// Test get_cosine(double)
//

static uint_t TestCosDouble(void) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	int x = 0;
	do {
		const double fRadians =
			(static_cast<double>(320 - x) / 320.0) * Burger::g_dPi * 4.0;
		const double fCos = cos(fRadians);
		const double fXCos = Burger::get_cosine(fRadians);
		const double fError = Burger::absolute(fCos - fXCos);
		const uint_t uFailure = (1.2e-15 < fError);
		uResult |= uFailure;
		ReportFailure("Burger::get_cosine(%.16g) = %.16g / Difference %.16g", uFailure,
			fRadians, fXCos, fCos, fError);

	} while (++x < 640);
	return uResult;
}

//
// Perform all the tests for the Burgerlib FP Math library
//

int BURGER_API TestBrfloatingpoint(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Floating Point Math tests");
	}

	// Test floating point constants

	uint_t uResult = TestFPConsts();
	uResult |= TestSNANToQNAN();

	// Test the test functions
	uResult |= TestIsNanFloat();
	uResult |= TestIsNanDouble();
	uResult |= TestIsInfFloat();
	uResult |= TestIsInfDouble();
	uResult |= TestIsFiniteFloat();
	uResult |= TestIsFiniteDouble();
	uResult |= TestIsNormalFloat();
	uResult |= TestIsNormalDouble();
	uResult |= TestSignBitFloat();
	uResult |= TestSignBitDouble();

	// Test simple floating point operations
	uResult |= TestAbsFloat();
	uResult |= TestAbsDouble();
	uResult |= TestSignFloat();
	uResult |= TestSignDouble();
	uResult |= TestClampFloat();
	uResult |= TestClampDouble();
	uResult |= TestSqrtFloat();
	uResult |= TestSqrtDouble();
	uResult |= TestIntToFloat();
	uResult |= TestFixedToFloat();
	uResult |= TestFloorFloat();
	uResult |= TestFloorDouble();
	uResult |= TestCeilFloat();
	uResult |= TestCeilDouble();
	uResult |= TestRoundFloat();
	uResult |= TestRoundDouble();
	uResult |= TestRoundToZeroFloat();
	uResult |= TestRoundToZeroDouble();
	uResult |= TestModuloRadiansFloat();
	uResult |= TestModuloRadiansDouble();
	uResult |= TestSinFloat();
	uResult |= TestSinDouble();
	uResult |= TestCosFloat();
	uResult |= TestCosDouble();

	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Floating Point Math tests!");
	}
	return static_cast<int>(uResult);
}
