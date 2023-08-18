/***************************************

	Sine/Cosine floating point math functions

	Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfloatingpoint.h"

#include "brendian.h"
#include "brfixedpoint.h"
#include "brmacros.h"

#if defined(BURGER_MSVC) && defined(BURGER_X86)
// Instruction may be inaccurate on some Pentiums
#pragma warning(disable : 4725)
#endif

//
// Note: The Xbox 360 PPC compiler has a bug. It's not acknowledging
// that fcmpu modifies the condition code register, so the inline assembler
// reorder optimizer will move the mfcr instruction BEFORE the fcmpu
// instruction, causing this function to fail.
//
// The workaround is to turn off the feature for this function
//

#if defined(BURGER_XBOX360)
#pragma optimize("z", off)
#endif

/*! ************************************

	\brief Modulo the range of the input so that -BURGER_PI <= fInput <
		BURGER_PI

	Wrap around a value in radians into remain in the valid range of radians.

	\note Due to 32 bit precision, numbers larger than 16*pi will likely contain
		rounding errors

	\param fInput 32 bit floating point value in radians
	\return The input converted to a 32 bit floating point value wrapped to fit
		within.

	\sa modulo_radians(double), get_sine(float), get_cosine(float)

***************************************/

// Written in assembly
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_PPC) || defined(BURGER_X86))) || \
	((defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
		defined(BURGER_PPC)) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC)))

#else

// Generic code

float BURGER_API Burger::modulo_radians(float fInput) BURGER_NOEXCEPT
{
	// Produce Input/(BURGER_PI*2)
	const float fTemp = (fInput * static_cast<float>(g_fReciprocalPi2)) + 0.5f;
	// Convert to int but rounded!
	const int iVar = static_cast<int>(fTemp);
	float fVar = static_cast<float>(iVar);
	if (fVar > fTemp) { // Did I round up?
		--fVar;         // Fix it
	}
	// Get the whole number and remove it
	return fInput - (fVar * static_cast<float>(g_fPi2));
}

#endif

/*! ************************************

	\brief Modulo the range of the input so that -BURGER_PI <= fInput <
		BURGER_PI

	Wrap around a value in radians into remain in the valid range of radians.

	\note Due to 64 bit precision, numbers larger than 1024*pi will likely
		contain rounding errors

	\param dInput 64 bit floating point value in radians
	\return The input converted to a 64 bit floating point value wrapped to fit
		within.

	\sa modulo_radians(float), get_sine(double), get_cosine(double)

***************************************/

// Written in assembly
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		(defined(BURGER_X86) || defined(BURGER_PPC))) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_PPC) || defined(BURGER_X86))) || \
	((defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
		defined(BURGER_PPC)) || \
	(defined(BURGER_MSVC) && (defined(BURGER_X86) || defined(BURGER_PPC)))

#else

// Generic code

double BURGER_API Burger::modulo_radians(double dInput) BURGER_NOEXCEPT
{
	// Produce Input/(BURGER_PI*2)
	const double dTemp = (dInput * g_dReciprocalPi2) + 0.5;
	//
	const int64_t iVar =
		static_cast<int64_t>(dTemp); // Convert to int but rounded!
	double dVar = static_cast<double>(iVar);
	if (dVar > dTemp) { // Did I round up?
		--dVar;         // Fix it
	}
	// Get the whole number and remove it
	return dInput - (dVar * g_dPi2);
}
#endif

/*! ************************************

	\fn float BURGER_API Burger::sine_387(float fInput)
	\brief Return the sine from radians

	Use the fsin instruction to generate sine. Will loop in cases
	where overflow is detected.

	\note Only available on 32 bit intel CPUs

	\param fInput Value in Radians
	\return Sine of fInput

	\sa sine_387(double) or cosine_387(float)

***************************************/

/*! ************************************

	\fn double BURGER_API Burger::sine_387(double dInput)
	\brief Return the sine from radians

	Use the fsin instruction to generate sine. Will loop in cases
	where overflow is detected.

	\note Only available on 32 bit intel CPUs

	\param dInput Value in Radians
	\return Sine of fInput

	\sa sine_387(float) or cosine_387(double)

***************************************/

/*! ************************************

	\fn float BURGER_API Burger::cosine_387(float fInput)
	\brief Return the cosine from radians

	Use the fcos instruction to generate sine. Will loop in cases
	where overflow is detected.

	\note Only available on 32 bit intel CPUs

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa cosine_387(double) or sine_387(float)

***************************************/

/*! ************************************

	\fn double BURGER_API Burger::cosine_387(double dInput)
	\brief Return the cosine from radians

	Use the fcos instruction to generate sine. Will loop in cases
	where overflow is detected.

	\note Only available on 32 bit intel CPUs

	\param dInput Value in Radians
	\return Cosine of fInput

	\sa cosine_387(float) or sine_387(double)

***************************************/

#if defined(BURGER_X86)

static BURGER_USED const Burger::Float80Bit g_lPI2 = {
	0x35, 0xC2, 0x68, 0x21, 0xA2, 0xDA, 0x0F, 0xC9, 0x01, 0x40};

#if defined(BURGER_METROWERKS) || defined(BURGER_WATCOM) || defined(BURGER_MSVC)

BURGER_DECLSPECNAKED float BURGER_API Burger::sine_387(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM {
        fld dword ptr[esp + 4]  // Load into the FPU
    TryAgain:
        fsin                    // This is good for float
        fstsw[esp + 4]          // Test for overflow
        fwait
        mov ah, byte ptr[esp + 5]
        sahf
        jpo NoError             // No overflow? Exit
        fld tbyte ptr[g_lPI2]   // Round to PI*2
        fxch st(1)
    Looper:
        fprem                   // Get the remainder
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]
        sahf
        jp Looper               // Still needs reduction?
        fstp st(1)              // Fix the FPU
        jmp TryAgain

    NoError:
        ret 4
    }
	// clang-format on
}

BURGER_DECLSPECNAKED double BURGER_API Burger::sine_387(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM {
        fld qword ptr[esp + 4]  // Load into the FPU
        fnstcw[esp + 8]         // Get the current status word
        mov eax, dword ptr[esp + 8] // Get the current value for return
        and eax, 0xFCFF         // Mask off unused bits
        or eax, 0x0300
        mov dword ptr[esp + 4], eax // Store in memory
        fldcw[esp + 4]          // Save the new status register
    TryAgain:
        fsin
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]
        or ah, 1
        sahf
        jpo NoError             // No overflow? Exit
        fld tbyte ptr[g_lPI2]   // Round to PI*2
        fxch st(1)
    Looper:
        fprem                   // Get the remainder
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]   
        sahf
        jp Looper               // Still needs reduction?
        fstp st(1)              // Fix the FPU
        jmp TryAgain
    NoError:
        fldcw[esp + 8]          // Save the new status register
        ret 8
    }
	// clang-format on
}

BURGER_DECLSPECNAKED float BURGER_API Burger::cosine_387(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM{
        fld dword ptr[esp + 4]  // Load into the FPU
    TryAgain:
        fcos                    // This is good for float
        fstsw[esp + 4]          // Test for overflow
        fwait
        mov ah, byte ptr[esp + 5]
        sahf
        jpo NoError             // No overflow? Exit
        fld tbyte ptr[g_lPI2]   // Round to PI*2
        fxch st(1)
    Looper:
        fprem                   // Get the remainder
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]
        sahf
        jp Looper               // Still needs reduction?
        fstp st(1)              // Fix the FPU
        jmp TryAgain

    NoError:
        ret 4
    }
	// clang-format on
}

BURGER_DECLSPECNAKED double BURGER_API Burger::cosine_387(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM {
        fld qword ptr[esp + 4]  // Load into the FPU
        fnstcw[esp + 8]         // Get the current status word
        mov eax, dword ptr[esp + 8] // Get the current value for return
        and eax, 0xFCFF         // Mask off unused bits
        or eax, 0x0300
        mov dword ptr[esp + 4], eax // Store in memory
        fldcw[esp + 4]          // Save the new status register
    TryAgain:
        fcos
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]
        or ah, 1
        sahf
        jpo NoError             // No overflow? Exit
        fld tbyte ptr[g_lPI2]   // Round to PI*2
        fxch st(1)
    Looper:
        fprem                   // Get the remainder
        fstsw[esp + 4]
        fwait
        mov ah, byte ptr[esp + 5]
        sahf
        jp Looper               // Still needs reduction?
        fstp st(1)              // Fix the FPU
        jmp TryAgain
    NoError:
        fldcw[esp + 8]          // Save the new status register
        ret 8
    }
	// clang-format on
}

#elif defined(BURGER_CLANG) || defined(BURGER_GNUC)
// __ZN6Burger8sine_387Ef = float BURGER_API Burger::sine_387(float fInput)
__asm__(
	".globl __ZN6Burger8sine_387Ef\n"
	"__ZN6Burger8sine_387Ef:\n"
	"	flds 4(%esp)\n" // Load into the FPU
	"1:\n"
	"	fsin\n"
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	orb $1,%ah\n"
	"	sahf\n"
	"	jpo 3f\n"             // No overflow? Exit
	"	fldt (__ZL6g_lPI2)\n" // Round to PI*2
	"	fxch %st(1)\n"
	"2:\n"
	"	fprem\n" // Get the remainder
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	sahf\n"
	"	jp 2b\n"       // Still needs reduction?
	"	fstp %st(1)\n" // Fix the FPU
	"	jmp 1b\n"
	"3:\n"
	"	ret\n" // Clean up and exit
);

// __ZN6Burger8sine_387Ed = double BURGER_API Burger::sine_387(double dInput)
__asm__(
	".globl __ZN6Burger8sine_387Ed\n"
	"__ZN6Burger8sine_387Ed:\n"
	"	fldl 4(%esp)\n"      // Load into the FPU
	"	fnstcw 8(%esp)\n"    // Get the current status word
	"	movl 8(%esp),%eax\n" // Get the current value for return
	"	andl $0xFCFF,%eax\n" // Mask off unused bits
	"	orl $0x0300,%eax\n"
	"	movl %eax,4(%esp)\n" // Store in memory
	"	fldcw 4(%esp)\n"     // Save the new status register
	"1:\n"
	"	fsin\n"
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	orb  $1,%ah\n"
	"	sahf\n"
	"	jpo 3f\n"             // No overflow? Exit
	"	fldt (__ZL6g_lPI2)\n" // Round to PI*2
	"	fxch %st(1)\n"
	"2:\n"
	"	fprem\n" // Get the remainder
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	sahf\n"
	"	jp 2b\n"       // Still needs reduction?
	"	fstp %st(1)\n" // Fix the FPU
	"	jmp 1b\n"
	"3:\n"
	"	fldcw 8(%esp)\n" // Save the new status register
	"	ret\n"           // Clean up and exit
);

// __ZN6Burger10cosine_387Ef = float BURGER_API Burger::cosine_387(float fInput)
__asm__(
	".globl __ZN6Burger10cosine_387Ef\n"
	"__ZN6Burger10cosine_387Ef:\n"
	"	flds 4(%esp)\n" // Load into the FPU
	"1:\n"
	"	fcos\n"
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	orb  $1,%ah\n"
	"	sahf\n"
	"	jpo 3f\n"             // No overflow? Exit
	"	fldt (__ZL6g_lPI2)\n" // Round to PI*2
	"	fxch %st(1)\n"
	"2:\n"
	"	fprem\n" // Get the remainder
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	sahf\n"
	"	jp 2b\n"       // Still needs reduction?
	"	fstp %st(1)\n" // Fix the FPU
	"	jmp 1b\n"
	"3:\n"
	"	ret\n" // Clean up and exit
);

// __ZN6Burger10cosine_387Ed = double BURGER_API Burger::cosine_387(double dInput)
__asm__(
	".globl __ZN6Burger10cosine_387Ed\n"
	"__ZN6Burger10cosine_387Ed:\n"
	"	fldl 4(%esp)\n"      // Load into the FPU
	"	fnstcw 8(%esp)\n"    // Get the current status word
	"	movl 8(%esp),%eax\n" // Get the current value for return
	"	andl $0xFCFF,%eax\n" // Mask off unused bits
	"	orl $0x0300,%eax\n"
	"	movl %eax,4(%esp)\n" // Store in memory
	"	fldcw 4(%esp)\n"     // Save the new status register
	"1:\n"
	"	fcos\n"
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	orb  $1,%ah\n"
	"	sahf\n"
	"	jpo 3f\n"             // No overflow? Exit
	"	fldt (__ZL6g_lPI2)\n" // Round to PI*2
	"	fxch %st(1)\n"
	"2:\n"
	"	fprem\n" // Get the remainder
	"	fstsw 4(%esp)\n"
	"	fwait\n"
	"	movb 5(%esp),%ah\n"
	"	sahf\n"
	"	jp 2b\n"       // Still needs reduction?
	"	fstp %st(1)\n" // Fix the FPU
	"	jmp 1b\n"
	"3:\n"
	"	fldcw 8(%esp)\n" // Save the new status register
	"	ret\n"           // Clean up and exit
);
#endif

#endif

// Terms for get_cosine_3_digits()
static const Burger::uint32_float_t g_fCos3DigitsValues[9] = {
	{0x00000000}, // 0.0
	{0x40490FDB}, // PI
	{0xC0490FDB}, // -PI
	{0x40C90FDB}, // 2PI
	{0x3E22F983}, // 1 / (PI*2)
	{0x3F22F983}, // 1 / (PI/2)
	{0x3f7fd8e1}, // 0.99940307
	{0xbefdbcc2}, // -0.49558072
	{0x3d16b2df}  // 0.03679168
};

/*! ************************************

	\brief Return the cosine from radians accurate to +-0.000597239

	Calculate cosine with 3 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa get_sine_3_digits(float)

***************************************/

float BURGER_API Burger::get_cosine_3_digits(float fInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	fInput = absolute(fInput);

	// Get the modulo of PI*2
	float fFractional = fInput * g_fCos3DigitsValues[4].f;

	// The integer denotes the quadrant
	int iQuadrant = static_cast<int>(fFractional);

	fFractional =
		fInput - (static_cast<float>(iQuadrant) * g_fCos3DigitsValues[3].f);

	iQuadrant = static_cast<int>(fFractional * g_fCos3DigitsValues[5].f);
	if (iQuadrant & 1) {
		fFractional = -fFractional;
	}
	fFractional = fFractional + g_fCos3DigitsValues[iQuadrant & 3].f;
	const float fFractionalSqr = fFractional * fFractional;
	float fResult = g_fCos3DigitsValues[6].f +
		fFractionalSqr *
			(g_fCos3DigitsValues[7].f +
				g_fCos3DigitsValues[8].f * fFractionalSqr);
	if ((iQuadrant + 1) & 2) {
		fResult = -fResult;
	}
	return fResult;
}

/*! ************************************

	\brief Return the sine from radians accurate to +-0.000597239

	Calculate sine with 3 iterations. It calculates sine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Sine of fInput

	\sa get_cosine_3_digits(float)

***************************************/

float BURGER_API Burger::get_sine_3_digits(float fInput) BURGER_NOEXCEPT
{
	return get_cosine_3_digits(static_cast<float>(BURGER_PI * 0.5f) - fInput);
}

// Terms for get_cosine_5_digits()
static const Burger::uint32_float_t g_fCos5DigitsValues[10] = {
	{0x00000000}, // 0.0
	{0x40490FDB}, // PI
	{0xC0490FDB}, // -PI
	{0x40C90FDB}, // 2PI
	{0x3E22F983}, // 1 / (PI*2)
	{0x3F22F983}, // 1 / (PI/2)
	{0x3f7fff90}, // 0.9999932946
	{0xbefff486}, // -0.4999124376
	{0x3d29ef0e}, // 0.0414877472
	{0xbaa69eb6}  // -0.0012712095
};

/*! ************************************

	\brief Return the cosine from radians accurate to +- 7.49199e-06

	Calculate cosine with 4 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa get_sine_3_digits(float)

***************************************/

float BURGER_API Burger::get_cosine_5_digits(float fInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	fInput = absolute(fInput);

	// Get the modulo of PI*2
	float fFractional = fInput * g_fCos5DigitsValues[4].f;

	// The integer denotes the quadrant
	int iQuadrant = static_cast<int>(fFractional);

	fFractional =
		fInput - (static_cast<float>(iQuadrant) * g_fCos5DigitsValues[3].f);

	iQuadrant = static_cast<int>(fFractional * g_fCos5DigitsValues[5].f);
	if (iQuadrant & 1) {
		fFractional = -fFractional;
	}
	fFractional = fFractional + g_fCos5DigitsValues[iQuadrant & 3].f;
	const float fFractionalSqr = fFractional * fFractional;
	float fResult = g_fCos5DigitsValues[6].f +
		fFractionalSqr *
			(g_fCos5DigitsValues[7].f +
				fFractionalSqr *
					(g_fCos5DigitsValues[8].f +
						g_fCos5DigitsValues[9].f * fFractionalSqr));
	if ((iQuadrant + 1) & 2) {
		fResult = -fResult;
	}
	return fResult;
}

/*! ************************************

	\brief Return the sine from radians accurate to +-7.49199e-06

	Calculate sine with 4 iterations. It calculates sine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Sine of fInput

	\sa get_cosine_3_digits(float)

***************************************/

float BURGER_API Burger::get_sine_5_digits(float fInput) BURGER_NOEXCEPT
{
	return get_cosine_5_digits(static_cast<float>(BURGER_PI * 0.5f) - fInput);
}

static const Burger::uint32_float_t g_fSinefCosfValues[11] = {
	{0x3FA2F983}, // 4.0 / BURGER_PI (Division by PI/4)
	{0x3F800000}, // 1.0
	{0xBF800000}, // -1.0
	{0xB816C97D}, // -0.35950439e-4f
	{0x3B232F49}, // 0.2490001007e-2f
	{0xBDA55DDD}, // -0.8074543253e-1f
	{0x3F490FDB}, // 0.7853981633f
	{0xB9A71AED}, // -0.31872783e-3f
	{0x3C81D732}, // 0.1584968416e-1f
	{0xBE9DE9C6}, // -0.30842416558f
	{0x3F7FFFFF}  // 0.9999999673f
};

/*! ************************************

	\brief Return the sine from radians accurate to +-1.03892e-06

	Calculate sine with 4 iterations. It calculates sine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Sine of fInput

	\sa get_cosine_6_digits(float)

***************************************/

float BURGER_API Burger::get_sine_6_digits(float fInput) BURGER_NOEXCEPT
{
	// Handle negative arguments
#if defined(BURGER_PPC)
	float fSign = static_cast<float>(
		__fsel(fInput, g_fSinefCosfValues[1].f, g_fSinefCosfValues[2].f));
#else
	float fSign = g_fSinefCosfValues[1].f;
	if (static_cast<uint32_float_t*>(static_cast<void*>(&fInput))->w &
		0x80000000U) {
		fSign = g_fSinefCosfValues[2].f;
	}
#endif

	// Get the modulo of PI/4
	float fFractional = absolute(fInput) * g_fSinefCosfValues[0].f;

	// The integer denotes the quadrant
	const int iQuadrant = static_cast<int>(fFractional);

	fFractional = fFractional - static_cast<float>(iQuadrant);
	// Test to negate the result
	if (iQuadrant & 4) {
		fSign = -fSign;
	}

	if (iQuadrant & 1) {
		// 1.0 - fFractional
		fFractional = g_fSinefCosfValues[1].f - fFractional;
	}

	const float fFractionalSqr = fFractional * fFractional;
	float fResult;
	// Apply the Cosine arc?
	if ((iQuadrant + 1) & 2) {
		fResult = ((g_fSinefCosfValues[7].f * fFractionalSqr +
					   g_fSinefCosfValues[8].f) *
						  fFractionalSqr +
					  g_fSinefCosfValues[9].f) *
				fFractionalSqr +
			g_fSinefCosfValues[10].f;
	} else {
		// Apply the Sine arc
		fResult = (((g_fSinefCosfValues[3].f * fFractionalSqr +
						g_fSinefCosfValues[4].f) *
						   fFractionalSqr +
					   g_fSinefCosfValues[5].f) *
						  fFractionalSqr +
					  g_fSinefCosfValues[6].f) *
			fFractional;
	}

	// Final negation application
	return fResult * fSign;
}

/*! ************************************

	\brief Return the cosine from radians accurate to +-1.1407e-06

	Calculate cosine with 4 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa get_sine_6_digits(float)

***************************************/

float BURGER_API Burger::get_cosine_6_digits(float fInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	fInput = absolute(fInput);

	// Get the modulo of PI/4
	float fFractional = fInput * g_fSinefCosfValues[0].f;

	// The integer denotes the quadrant
	const int iQuadrant = static_cast<int>(fFractional);

	fFractional = fFractional - static_cast<float>(iQuadrant);

	if (iQuadrant & 1) {
		// 1.0 - fFractional
		fFractional = g_fSinefCosfValues[1].f - fFractional;
	}

	const float fFractionalSqr = fFractional * fFractional;
	float fResult;
	if ((iQuadrant + 1) & 2) {
		// Apply the sine arc
		fResult = (((g_fSinefCosfValues[3].f * fFractionalSqr +
						g_fSinefCosfValues[4].f) *
						   fFractionalSqr +
					   g_fSinefCosfValues[5].f) *
						  fFractionalSqr +
					  g_fSinefCosfValues[6].f) *
			fFractional;
	} else {
		// Apply a cosine arc
		fResult = ((g_fSinefCosfValues[7].f * fFractionalSqr +
					   g_fSinefCosfValues[8].f) *
						  fFractionalSqr +
					  g_fSinefCosfValues[9].f) *
				fFractionalSqr +
			g_fSinefCosfValues[10].f;
	}
	// Apply negation
	if ((iQuadrant + 2) & 4) {
		fResult = -fResult;
	}
	return fResult;
}

/*! ************************************

	\fn float BURGER_API Burger::get_sine(float fInput)
	\brief Return the sine from radians

	This is a replacement of sinf() from the C++ libraries

	To calculate sine...
	sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!) ...
	Repeat the pattern by reversing the sign of the addition and adding
	2 to the factors for every iteration

	This function will perform 10 iterations from 3,5,7,...,23

	\note ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5

	\param fInput Value in Radians
	\return Sine of fInput

	\sa get_sine(double) or get_cosine(float)

***************************************/

// 3!, 5!, to 23!
// static const Burger::uint32_float_t g_fSineFactors[11] = {
//	{0xC0C00000},{0x42F00000},{0xC59D8000},{0x48B13000},{0xCC184540},
//	{0x4FB99466},{0xD3983BBC},{0x57A1BF77},{0xDBD815CA},{0x603141DF},
//	{0xE4AF2E1A}
//};

// 1/3!, 1/5!, to 1/23!

static const BURGER_USED Burger::uint32_float_t g_fInverseSineFactors[11] = {
	{0xBE2AAAAB}, {0x3C088889}, {0xB9500D01}, {0x3638EF1D}, {0xB2D7322B},
	{0x2F309231}, {0xAB573F9F}, {0x274A963C}, {0xA317A4DA}, {0x1EB8DC78},
	{0x9A3B0DA1}};

#if defined(BURGER_X86) && \
	(defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || \
		defined(BURGER_MSVC))

BURGER_DECLSPECNAKED float BURGER_API Burger::get_sine(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM
    {
        fld dword ptr[esp + 4]              // Load into the FPU
        fld dword ptr[g_fReciprocalPi2] // Load in 1/2Pi
        fmul st(0), st(1)                   // Multiply
        fadd dword ptr[g_fHalf] // Add half for rounding
        fst dword ptr[esp + 4]  // Round to nearest in 24 bit to force consistent precision
        frndint // Convert to integer
        fcom dword ptr[esp + 4] // Compare the two and get rid of the prerounded
#if defined(BURGER_WATCOM)
        nop // There is an alignment bug in Watcom C. Remove this and
            // [g_fInverseSineFactors+8] vanishes
#endif
        fnstsw ax
        test ah, 0x41                       // ble
        jne SinFExit               // Did it round up?
        fsub dword ptr[g_fOne] // Fixup
    SinFExit:
        fmul dword ptr[g_fPi2] // Mul by 2 pi
        fsubp st(1), st(0) // Subtract and clean up

        fld st(0) // Make a copy for squaring
        fmul st(0), st(1) // Square the input
        fld st(0)
        fmul st(0), st(2)                                // Create the Power of 3
        fld dword ptr[g_fInverseSineFactors] // Start iterating
        fmul st(0), st(1) // fInput*(1/3!)
        faddp st(3), st(0) // fResult + fInput*(1/3!)
        fmul st(0), st(1) // Up the power by 2
        fld dword ptr[g_fInverseSineFactors + 4]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 8]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 12]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 16]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 20]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 24]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 28]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 32]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseSineFactors + 36]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmulp st(1), st(0)
        fmul dword ptr[g_fInverseSineFactors + 40]
        faddp st(1), st(0)
        ret 4 // Clean up and exit
    }
	// clang-format on
}

#elif defined(BURGER_X86) && (defined(BURGER_MACOSX) || defined(BURGER_IOS))
// __ZN6Burger8get_sineEf = float BURGER_API Burger::get_sine(float fInput)
__asm__(
	".globl __ZN6Burger8get_sineEf\n"
	"__ZN6Burger8get_sineEf:\n"
	"	flds	4(%esp)\n"                          // Load into the FPU
	"	flds	(__ZN6Burger16g_fReciprocalPi2E)\n" // Load in 1/2Pi
	"	fmul	%st(1),%st(0)\n"          // Multiply (Really fInput/2Pi)
	"	fadds	(__ZN6Burger7g_fHalfE)\n" // Add half for rounding
	"	fsts	4(%esp)\n" // Round to nearest in 24 bit to force consistent
						   // precision
	"	frndint\n"         // Convert to integer
	"	fcoms	4(%esp)\n" // Compare the two and get rid of the prerounded
	"	fnstsw	%ax\n"
	"	testb	$0x41,%ah\n"             // ble
	"	jne		1f\n"                    // Did it round up?
	"	fsubs	(__ZN6Burger6g_fOneE)\n" // Fixup
	"1:\n"
	"	fmuls	(__ZN6Burger6g_fPi2E)\n" // Mul by 2 pi
	"	fsubrp	%st(1),%st(0)\n"         // Subtract and clean up
	"	fld		%st(0)\n"                // Make a copy for squaring
	"	fmul	%st(1),%st(0)\n"         // Square the input
	"	fld		%st(0)\n"
	"	fmul	%st(2),%st(0)\n"                 // Create the Power of 3
	"	flds	(__ZL21g_fInverseSineFactors)\n" // Start iterating
	"	fmul	%st(1),%st(0)\n"                 // fInput*(1/3!)
	"	faddp	%st(0),%st(3)\n"                 // fResult + fInput*(1/3!)
	"	fmul	%st(1),%st(0)\n"                 // Up the power by 2
	"	flds	(__ZL21g_fInverseSineFactors+4)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+8)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+12)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+16)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+20)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+24)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+28)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+32)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmul	%st(1),%st(0)\n"
	"	flds	(__ZL21g_fInverseSineFactors+36)\n"
	"	fmul	%st(1),%st(0)\n"
	"	faddp	%st(0),%st(3)\n"
	"	fmulp	%st(0),%st(1)\n"
	"	fmuls	(__ZL21g_fInverseSineFactors+40)\n"
	"	faddp	%st(0),%st(1)\n"
	"	ret\n" // Clean up and exit
);

#else

float BURGER_API Burger::get_sine(float fInput) BURGER_NOEXCEPT
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	fInput = modulo_radians(fInput);

	// To calculate sine...
	// Note: ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5
	// sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	const float fInput2 = (fInput * fInput);

	float fInputFactorial = (fInput2 * fInput);

	//- ((x^3)/ 3!)
	float fResult =
		(fInputFactorial * static_cast<float>(g_fInverseSineFactors[0])) +
		fInput;
	//+ ((x^5)/ 5!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[1])) +
		fResult;
	//- ((x^7)/ 7!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[2])) +
		fResult;
	//+ ((x^9)/ 9!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[3])) +
		fResult;
	//- ((x^11)/ 11!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[4])) +
		fResult;
	//+ ((x^13)/ 13!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[5])) +
		fResult;
	//- ((x^15)/ 15!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[6])) +
		fResult;
	//+ ((x^17)/ 17!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[7])) +
		fResult;
	//- ((x^19)/ 19!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[8])) +
		fResult;
	//+ ((x^21)/ 21!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult = (fInputFactorial * static_cast<float>(g_fInverseSineFactors[9])) +
		fResult;
	//- ((x^23)/ 23!)
	fInputFactorial = fInputFactorial * fInput2;
	fResult =
		(fInputFactorial * static_cast<float>(g_fInverseSineFactors[10])) +
		fResult;
	return fResult;
}
#endif

/*! ************************************

	\fn float BURGER_API Burger::get_cosine(float fInput)
	\brief Return the cosine from radians

	This is a replacement of cosf() from the C++ libraries

	\param fInput Value in Radians
	\return Cosine of fInput

	\sa get_cosine(double) or get_sine(float)

***************************************/

// 2!, 4!, to 22!
// static const Burger::uint32_float_t g_fCosineFactors[11] = {
//	{0xC0000000},{0x41C00000},{0xC4340000},{0x471D8000},{0xCA5D7C00},
//	{0x4DE467E0},{0xD1A261D9},{0x55983BBC},{0xD9B5F766},{0x5E070D9E},
//	{0xE273BA93}
//};

// 1/2!, 1/4!, to 1/22!
static const BURGER_USED Burger::uint32_float_t g_fInverseCosineFactors[11] = {
	{0xBF000000}, {0x3D2AAAAB}, {0xBAB60B61}, {0x37D00D01}, {0xB493F27E},
	{0x310F76C7}, {0xAD49CBA5}, {0x29573F9F}, {0xA53413C3}, {0x20F2A15D},
	{0x9C8671CB}};

#if defined(BURGER_X86) && \
	(defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || \
		defined(BURGER_MSVC))
BURGER_DECLSPECNAKED float BURGER_API Burger::get_cosine(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM
    {
        fld1
        fld dword ptr[esp + 4]         // Load into the FPU
        fld dword ptr[g_fReciprocalPi2] // Load in 1/2Pi
        fmul st(0), st(1)                   // Multiply
        fadd dword ptr[g_fHalf] // Add half for rounding
        fst dword ptr[esp + 4]  // Round to nearest in 24 bit to force consistent precision
        frndint // Convert to integer
        fcom dword ptr[esp + 4] // Compare the two and get rid of the prerounded
        fnstsw ax
        test ah, 0x41                       // ble
        jne CosinFExit             // Did it round up?
        fsub dword ptr[g_fOne] // Fixup
    CosinFExit:
        fmul dword ptr[g_fPi2] // Mul by 2 pi
        fsubp st(1), st(0)  // Subtract and clean up

        fmul st(0), st(0)   // Square the input
        fld st(0)           // Copy the Power of 2
        fld dword ptr[g_fInverseCosineFactors] // Start iterating
        fmul st(0), st(1)   // fInput*(1/2!)
        faddp st(3), st(0) // fResult + fInput*(1/2!)
        fmul st(0), st(1) // Up the power by 2
        fld dword ptr[g_fInverseCosineFactors + 4]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 8]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 12]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 16]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 20]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 24]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 28]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 32]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld dword ptr[g_fInverseCosineFactors + 36]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmulp st(1), st(0)
        fmul dword ptr[g_fInverseCosineFactors + 40]
        faddp st(1), st(0)
        ret 4 // Clean up and exit
    }
    // clang-format on 
}
#elif defined(BURGER_X86) && (defined(BURGER_MACOSX) || defined(BURGER_IOS))
// __ZN6Burger10get_cosineEf = float BURGER_API Burger::get_cosine(float fInput)
__asm__(
    ".globl __ZN6Burger10get_cosineEf\n"
    "__ZN6Burger10get_cosineEf:\n"
    "	flds	(__ZN6Burger6g_fOneE)\n"            // Initial 1
    "	flds	4(%esp)\n"                          // Load into the FPU
    "	flds	(__ZN6Burger16g_fReciprocalPi2E)\n" // Load in 1/2Pi
    "	fmul	%st(1),%st(0)\n"          // Multiply (Really fInput/2Pi)
    "	fadds	(__ZN6Burger7g_fHalfE)\n" // Add half for rounding
    "	fsts	4(%esp)\n" // Round to nearest in 24 bit to force consistent
                           // precision
    "	frndint\n"         // Convert to integer
    "	fcoms	4(%esp)\n" // Compare the two and get rid of the prerounded
    "	fnstsw	%ax\n"
    "	testb	$0x41,%ah\n"             // ble
    "	jne		1f\n"                    // Did it round up?
    "	fsubs	(__ZN6Burger6g_fOneE)\n" // Fixup
    "1:\n"
    "	fmuls	(__ZN6Burger6g_fPi2E)\n"      // Mul by 2 pi
    "	fsubrp	%st(1),%st(0)\n"              // Subtract and clean up
    "	fmul	%st(0),%st(0)\n"              // Square the input
    "	fld		%st(0)\n"                     // Copy the Power of 2
    "	flds	(__ZL23g_fInverseCosineFactors)\n" // Start iterating
    "	fmul	%st(1),%st(0)\n"              // fInput*(1/2!)
    "	faddp	%st(0),%st(3)\n"              // fResult + fInput*(1/2!)
    "	fmul	%st(1),%st(0)\n"              // Up the power by 2
    "	flds	(__ZL23g_fInverseCosineFactors+4)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+8)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+12)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+16)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+20)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+24)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+28)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+32)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmul	%st(1),%st(0)\n"
    "	flds	(__ZL23g_fInverseCosineFactors+36)\n"
    "	fmul	%st(1),%st(0)\n"
    "	faddp	%st(0),%st(3)\n"
    "	fmulp	%st(0),%st(1)\n"
    "	fmuls	(__ZL23g_fInverseCosineFactors+40)\n"
    "	faddp	%st(0),%st(1)\n"
    "	ret\n" // Clean up and exit
);

#else
float BURGER_API Burger::get_cosine(float fInput) BURGER_NOEXCEPT
{
    //return xcosf(fInput);

    // Start by rounding the radians to reduce the chance
    // of floating point rounding errors
    fInput = modulo_radians(fInput);

    // To calculate cosine...
    // Note: ! is factoral so 2! = 1*2, and 4! = 1*2*3*4
    // cos(x) = 1 - ((x^2)/ 2!) + ((x^4)/ 4!) - ((x^6)/ 6!) + ((x^8)/ 8!)
    // Repeat the pattern by reversing the sign of the addition and adding
    // 2 for the square for every iteration

    const float fInput2 = (fInput * fInput);

    //- ((x^2)/ 2!)
    float fResult = (fInput2 * static_cast<float>(g_fInverseCosineFactors[0])) + 1.0f;
    //+ ((x^4)/ 4!)
    float fInputFactorial = fInput2 * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[1])) + fResult;
    //- ((x^6)/ 6!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[2])) + fResult;
    //+ ((x^8)/ 8!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[3])) + fResult;
    //- ((x^10)/ 10!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[4])) + fResult;
    //+ ((x^12)/ 12!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[5])) + fResult;
    //- ((x^14)/ 14!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[6])) + fResult;
    //+ ((x^16)/ 16!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[7])) + fResult;
    //- ((x^18)/ 18!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[8])) + fResult;
    //+ ((x^20)/ 20!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[9])) + fResult;
    //- ((x^22)/ 22!)
    fInputFactorial = fInputFactorial * fInput2;
    fResult = (fInputFactorial * static_cast<float>(g_fInverseCosineFactors[10])) + fResult;
    return fResult;
}
#endif
// clang-format on

static const Burger::uint64_double_t g_fxcosf7Values[11] = {
	{0x0000000000000000ULL}, // 0.0
	{0x400921FB54442D18ULL}, // PI
	{0xC00921FB54442D18ULL}, // -PI
	{0x401921FB54442D18ULL}, // 2PI
	{0x3FC45F306DC9C883ULL}, // 1 / (PI*2)
	{0x3FE45F306DC9C883ULL}, // 1 / (PI/2)
	{0x3fefffffe704240fULL}, // 0.999999953464
	{0xbfdfffff9a5d91b4ULL}, // -0.4999999053455
	{0x3fa554edeb3e29beULL}, // 0.0416635846769
	{0xbf56b2aa2b26a495ULL}, // -0.0013853704264
	{0x3ef8475611992c86ULL}  // 0.00002315393167
};

/*! ************************************

	\brief Return the cosine from radians accurate to +-2.05542e-06

	Calculate cosine with 5 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param dInput Value in Radians
	\return Cosine of dInput

	\sa get_sine_7_digits(double)

***************************************/

double BURGER_API Burger::get_cosine_7_digits(double dInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	dInput = absolute(dInput);

	// Get the modulo of PI*2
	double dFractional = dInput * g_fxcosf7Values[4].d;

	// The integer denotes the quadrant
	int iQuadrant = static_cast<int>(dFractional);

	dFractional =
		dInput - (static_cast<double>(iQuadrant) * g_fxcosf7Values[3].d);

	iQuadrant = static_cast<int>(dFractional * g_fxcosf7Values[5].d);
	if (iQuadrant & 1) {
		dFractional = -dFractional;
	}
	dFractional = dFractional + g_fxcosf7Values[iQuadrant & 3].d;
	const double dFractionalSqr = dFractional * dFractional;
	double dResult = g_fxcosf7Values[6].d +
		dFractionalSqr *
			(g_fxcosf7Values[7].d +
				dFractionalSqr *
					(g_fxcosf7Values[8].d +
						dFractionalSqr *
							(g_fxcosf7Values[9].d +
								g_fxcosf7Values[10].d * dFractionalSqr)));
	if ((iQuadrant + 1) & 2) {
		dResult = -dResult;
	}
	return dResult;
}

/*! ************************************

	\brief Return the sine from radians accurate to +-2.05542e-06

	Calculate sine with 5 iterations. It calculates sine but sacrifices some
	accuracy.

	\param dInput Value in Radians
	\return Sine of dInput

	\sa get_cosine_7_digits(double)

***************************************/

double BURGER_API Burger::get_sine_7_digits(double dInput) BURGER_NOEXCEPT
{
	return get_cosine_7_digits((BURGER_PI * 0.5) - dInput);
}

static const Burger::uint64_double_t g_fxcosf12Values[13] = {
	{0x0000000000000000ULL}, // 0.0
	{0x400921FB54442D18ULL}, // PI
	{0xC00921FB54442D18ULL}, // -PI
	{0x401921FB54442D18ULL}, // 2PI
	{0x3FC45F306DC9C883ULL}, // 1 / (PI*2)
	{0x3FE45F306DC9C883ULL}, // 1 / (PI/2)
	{0x3fefffffffffe5adULL}, // 0.99999999999925182
	{0xbfdffffffff7d1d6ULL}, // -0.49999999997024012
	{0x3fa5555553ac4d2cULL}, // 0.041666666473384543
	{0xbf56c16b9551979cULL}, // -0.001388888418000423
	{0x3efa017a892ec93cULL}, // 0.0000248010406484558
	{0xbe9278b4f2f32463ULL}, // -0.0000002752469638432
	{0x3e2119c8b5785b45ULL}  // 0.0000000019907856854
};

/*! ************************************

	\brief Return the cosine from radians accurate to +-7.48669e-13

	Calculate cosine with 6 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param dInput Value in Radians
	\return Cosine of dInput

	\sa get_sine_12_digits(double)

***************************************/

double BURGER_API Burger::get_cosine_12_digits(double dInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	dInput = absolute(dInput);

	// Get the modulo of PI*2
	double dFractional = dInput * g_fxcosf12Values[4].d;

	// The integer denotes the quadrant
	int iQuadrant = static_cast<int>(dFractional);

	dFractional =
		dInput - (static_cast<double>(iQuadrant) * g_fxcosf12Values[3].d);

	iQuadrant = static_cast<int>(dFractional * g_fxcosf12Values[5].d);
	if (iQuadrant & 1) {
		dFractional = -dFractional;
	}
	dFractional = dFractional + g_fxcosf12Values[iQuadrant & 3].d;
	const double dFractionalSqr = dFractional * dFractional;
	double dResult = g_fxcosf12Values[6].d +
		dFractionalSqr *
			(g_fxcosf12Values[7].d +
				dFractionalSqr *
					(g_fxcosf12Values[8].d +
						dFractionalSqr *
							(g_fxcosf12Values[9].d +
								dFractionalSqr *
									(g_fxcosf12Values[10].d +
										dFractionalSqr *
											(g_fxcosf12Values[11].d +
												g_fxcosf12Values[12].d *
													dFractionalSqr)))));
	if ((iQuadrant + 1) & 2) {
		dResult = -dResult;
	}
	return dResult;
}

/*! ************************************

   \brief Return the sine from radians accurate to +-7.48669e-13

   Calculate sine with 6 iterations. It calculates sine but sacrifices some
   accuracy.

   \param dInput Value in Radians
   \return Sine of dInput

   \sa get_cosine_12_digits(double)

***************************************/

double BURGER_API Burger::get_sine_12_digits(double dInput) BURGER_NOEXCEPT
{
	return get_cosine_12_digits((BURGER_PI * 0.5) - dInput);
}

/*! ************************************

	\fn double BURGER_API Burger::get_sine_18_digits(double dInput)
	\brief Return the sine from radians

	This is a replacement of sin() from the C++ libraries

	\param dInput Value in Radians
	\return Sine of dInput

	\sa get_sine(double) or get_cosine_18_digits(double)

***************************************/

// 3!, 5!, to 23!
// static const Burger::uint64_double_t g_dSineFactors[11] = {
//	{0xC018000000000000ULL},{0x405E000000000000ULL},{0xC0B3B00000000000ULL},{0x4116260000000000ULL},{0xC18308A800000000ULL},
//	{0x41F7328CC0000000ULL},{0xC273077775800000ULL},{0x42F437EEECD80000ULL},{0xC37B02B930689000ULL},{0x4406283BE9B5C620ULL},
//	{0xC495E5C335F8A4CEULL}
//};

// 1/3!, 1/5!, to 1/23!
static const Burger::uint64_double_t g_dInverseSineFactors[11] = {
	{0xBFC5555555555555ULL}, {0x3F81111111111111ULL}, {0xBF2A01A01A01A01AULL},
	{0x3EC71DE3A556C734ULL}, {0xBE5AE64567F544E4ULL}, {0x3DE6124613A86D09ULL},
	{0xBD6AE7F3E733B81FULL}, {0x3CE952C77030AD4AULL}, {0xBC62F49B46814157ULL},
	{0x3BD71B8EF6DCF572ULL}, {0xBB4761B413163819ULL}};

#if defined(BURGER_X86) && \
	(defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || \
		defined(BURGER_MSVC))
BURGER_DECLSPECNAKED double BURGER_API Burger::get_sine_18_digits(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM
    {
        fld qword ptr[esp + 4]              // Load into the FPU
        fld qword ptr[g_dReciprocalPi2] // Load in 1/2Pi
        fmul st(0), st(1)                   // Multiply
        fadd qword ptr[g_dHalf] // Add half for rounding
        fst qword ptr[esp + 4]  // Store at double precision
        frndint                 // Remove fraction
        fcom qword ptr[esp + 4] // Compare the two and get rid of the prerounded
#if defined(BURGER_WATCOM)
        nop // There is an alignment bug in Watcom C. Remove this and
            // [g_dInverseSineFactors+16] vanishes
#endif
        fnstsw ax
        test ah, 0x41
        jnz SinFExit          // Did it round up?
        fsub qword ptr[g_dOne] // Fixup
    SinFExit:
        fmul qword ptr[g_dPi2] // Mul by 2 pi
        fsubp st(1), st(0) // Subtract and clean up

        fld st(0) // Make a copy for squaring
        fmul st(0), st(1) // Square the input
        fld st(0)
        fmul st(0), st(2)                                // Create the Power of 3
        fld qword ptr[g_dInverseSineFactors] // Start iterating
        fmul st(0), st(1) // fInput*(1/3!)
        faddp st(3), st(0) // fResult + fInput*(1/3!)
        fmul st(0), st(1) // Up the power by 2
        fld qword ptr[g_dInverseSineFactors + 8]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 16]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 24]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 32]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 40]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 48]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 56]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 64]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseSineFactors + 72]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmulp st(1), st(0)
        fmul qword ptr[g_dInverseSineFactors + 80]
        faddp st(1), st(0)
        ret 8 // Clean up and exit
    }
	// clang-format on
}
// No MacOSX 32 bit intel version assembly version, since SSE builds quite
// nicely

#else

double BURGER_API Burger::get_sine_18_digits(double dInput) BURGER_NOEXCEPT
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	dInput = modulo_radians(dInput);

	// To calculate sine...
	// Note: ! is factoral so 3! = 1*2*3, and 5! = 1*2*3*4*5
	// sin(x) = x - ((x^3)/ 3!) + ((x^5)/ 5!) - ((x^7)/ 7!) + ((x^9)/ 9!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	const double dInput2 = (dInput * dInput);

	double dInputFactorial = (dInput2 * dInput);

	//- ((x^3)/ 3!)
	double dResult = (dInputFactorial * g_dInverseSineFactors[0]) + dInput;
	//+ ((x^5)/ 5!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[1]) + dResult;
	//- ((x^7)/ 7!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[2]) + dResult;
	//+ ((x^9)/ 9!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[3]) + dResult;
	//- ((x^11)/ 11!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[4]) + dResult;
	//+ ((x^13)/ 13!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[5]) + dResult;
	//- ((x^15)/ 15!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[6]) + dResult;
	//+ ((x^17)/ 17!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[7]) + dResult;
	//- ((x^19)/ 19!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[8]) + dResult;
	//+ ((x^21)/ 21!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[9]) + dResult;
	//- ((x^23)/ 23!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseSineFactors[10]) + dResult;
	return dResult;
}
#endif

/*! ************************************

	\fn double BURGER_API Burger::get_cosine_18_digits(double dInput)
	\brief Return the cosine from radians

	This is a replacement of cos() from the C++ libraries

	\param dInput Value in Radians
	\return Cosine of dInput

	\sa get_cosine(float) or get_cosine_18_digits(double)

***************************************/

// 2!, 4!, to 22!
// #if !defined(BURGER_X86) && !defined(BURGER_MACOSX)
// static const
// #endif
// Burger::uint64_double_t g_dCosineFactors[11] = {
//	{0xC000000000000000ULL},{0x4038000000000000ULL},{0xC086800000000000ULL},{0x40E3B00000000000ULL},{0xC14BAF8000000000ULL},
//	{0x41BC8CFC00000000ULL},{0xC2344C3B28000000ULL},{0x42B3077775800000ULL},{0xC336BEECCA730000ULL},{0x43C0E1B3BE415A00ULL},
//	{0xC44E77526159F06CULL}
//};
// 1/2!, 1/4!, to 1/22!

static const Burger::uint64_double_t g_dInverseCosineFactors[11] = {
	{0xBFE0000000000000ULL}, {0x3FA5555555555555ULL}, {0xBF56C16C16C16C17ULL},
	{0x3EFA01A01A01A01AULL}, {0xBE927E4FB7789F5CULL}, {0x3E21EED8EFF8D898ULL},
	{0xBDA93974A8C07C9DULL}, {0x3D2AE7F3E733B81FULL}, {0xBCA6827863B97D97ULL},
	{0x3C1E542BA4020225ULL}, {0xBB90CE396DB7F853ULL}};

#if defined(BURGER_X86) && \
	(defined(BURGER_WATCOM) || defined(BURGER_METROWERKS) || \
		defined(BURGER_MSVC))
BURGER_DECLSPECNAKED double BURGER_API Burger::get_cosine_18_digits(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
    BURGER_ASM
    {
        fld1
        fld qword ptr[esp + 4]         // Load into the FPU
        fld qword ptr[g_dReciprocalPi2] // Load in 1/2Pi
        fmul st(0), st(1)                   // Multiply
        fadd qword ptr[g_dHalf] // Add half for rounding
        fst qword ptr[esp + 4]  // Store at double precision
        frndint                 // Remove fraction
        fcom qword ptr[esp + 4] // Compare the two and get rid of the prerounded
        fnstsw ax
        test ah, 0x41
        jnz SinFExit          // Did it round up?
        fsub qword ptr[g_dOne] // Fixup
    SinFExit:
        fmul qword ptr[g_dPi2] // Mul by 2 pi
        fsubp st(1), st(0) // Subtract and clean up

        fmul st(0), st(0) // Square the input
        fld st(0)                              // Copy the Power of 2
        fld qword ptr[g_dInverseCosineFactors] // Start iterating
        fmul st(0), st(1) // fInput*(1/2!)
        faddp st(3), st(0) // fResult + fInput*(1/2!)
        fmul st(0), st(1) // Up the power by 2
        fld qword ptr[g_dInverseCosineFactors + 8]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 16]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 24]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 32]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 40]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 48]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 56]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 64]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmul st(0), st(1)
        fld qword ptr[g_dInverseCosineFactors + 72]
        fmul st(0), st(1)
        faddp st(3), st(0)
        fmulp st(1), st(0)
        fmul qword ptr[g_dInverseCosineFactors + 80]
        faddp st(1), st(0)
        ret 8 // Clean up and exit
    }
	// clang-format on
}
// No MacOSX 32 bit intel version assembly version, since SSE builds quite
// nicely
#else
double BURGER_API Burger::get_cosine_18_digits(double dInput) BURGER_NOEXCEPT
{
	// Start by rounding the radians to reduce the chance
	// of floating point rounding errors
	dInput = modulo_radians(dInput);

	// To calculate cosine...
	// Note: ! is factoral so 2! = 1*2, and 4! = 1*2*3*4
	// cos(x) = 1 - ((x^2)/ 2!) + ((x^4)/ 4!) - ((x^6)/ 6!) + ((x^8)/ 8!)
	// Repeat the pattern by reversing the sign of the addition and adding
	// 2 for the square for every iteration

	const double dInput2 = (dInput * dInput);

	//- ((x^2)/ 2!)
	double dResult = (dInput2 * g_dInverseCosineFactors[0]) + 1.0;
	//+ ((x^4)/ 4!)
	double dInputFactorial = dInput2 * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[1]) + dResult;
	//- ((x^6)/ 6!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[2]) + dResult;
	//+ ((x^8)/ 8!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[3]) + dResult;
	//- ((x^10)/ 10!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[4]) + dResult;
	//+ ((x^12)/ 12!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[5]) + dResult;
	//- ((x^14)/ 14!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[6]) + dResult;
	//+ ((x^16)/ 16!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[7]) + dResult;
	//- ((x^18)/ 18!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[8]) + dResult;
	//+ ((x^20)/ 20!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[9]) + dResult;
	//- ((x^22)/ 22!)
	dInputFactorial = dInputFactorial * dInput2;
	dResult = (dInputFactorial * g_dInverseCosineFactors[10]) + dResult;
	return dResult;
}
#endif

static const Burger::uint64_double_t g_fxcosf20Values[16] = {
	{0x0000000000000000ULL}, // 0.0
	{0x400921FB54442D18ULL}, // PI
	{0xC00921FB54442D18ULL}, // -PI
	{0x401921FB54442D18ULL}, // 2PI
	{0x3FC45F306DC9C883ULL}, // 1 / (PI*2)
	{0x3FE45F306DC9C883ULL}, // 1 / (PI/2)
	{0x3ff0000000000000ULL}, // 0.9999999999999999999936329
	{0xbfe0000000000000ULL}, // -0.49999999999999999948362843
	{0x3fa5555555555554ULL}, // 0.04166666666666665975670054
	{0xbf56c16c16c16b71ULL}, // -0.00138888888888885302082298
	{0x3efa01a01a013316ULL}, // 0.000024801587301492746422297
	{0xbe927e4fb74f59b3ULL}, // -0.00000027557319209666748555
	{0x3e21eed8dcf143e4ULL}, // 0.0000000020876755667423458605
	{0xbda93969cb094c31ULL}, // -0.0000000000114706701991777771
	{0x3d2ae4331129f8f4ULL}, // 0.0000000000000477687298095717
	{0xbca5ca4196cf0f73ULL}  // -0.00000000000000015119893746887
};

/*! ************************************

	\brief Return the cosine from radians accurate to +-1.34615e-15

	Calculate cosine with 8 iterations. It calculates cosine but sacrifices some
	accuracy.

	\param dInput Value in Radians
	\return Cosine of dInput

	\sa get_sine(double)

***************************************/

double BURGER_API Burger::get_cosine(double dInput) BURGER_NOEXCEPT
{
	// Cosine uses absolute value
	dInput = absolute(dInput);

	// Get the modulo of PI*2
	double dFractional = dInput * g_fxcosf20Values[4].d;

	// The integer denotes the quadrant
	int iQuadrant = static_cast<int>(dFractional);

	dFractional =
		dInput - (static_cast<double>(iQuadrant) * g_fxcosf20Values[3].d);

	iQuadrant = static_cast<int>(dFractional * g_fxcosf20Values[5].d);
	if (iQuadrant & 1) {
		dFractional = -dFractional;
	}
	dFractional = dFractional + g_fxcosf20Values[iQuadrant & 3].d;
	const double dFractionalSqr = dFractional * dFractional;
	double dResult = g_fxcosf20Values[6].d +
		dFractionalSqr *
			(g_fxcosf20Values[7].d +
				dFractionalSqr *
					(g_fxcosf20Values[8].d +
						dFractionalSqr *
							(g_fxcosf20Values[9].d +
								dFractionalSqr *
									(g_fxcosf20Values[10].d +
										dFractionalSqr *
											(g_fxcosf20Values[11].d +
												dFractionalSqr *
													(g_fxcosf20Values[12].d +
														dFractionalSqr *
															(g_fxcosf20Values
																	[13]
																		.d +
																dFractionalSqr *
																	(g_fxcosf20Values
																			[14]
																				.d +
																		dFractionalSqr *
																			g_fxcosf20Values[15]
																				.d))))))));
	if ((iQuadrant + 1) & 2) {
		dResult = -dResult;
	}
	return dResult;
}

/*! ************************************

   \brief Return the sine from radians accurate to +-1.16573e-15

   Calculate sine with 8 iterations. It calculates sine but sacrifices some
   accuracy.

   \param dInput Value in Radians
   \return Sine of dInput

   \sa get_cosine(double)

***************************************/

double BURGER_API Burger::get_sine(double dInput) BURGER_NOEXCEPT
{
	return get_cosine((BURGER_PI * 0.5) - dInput);
}
