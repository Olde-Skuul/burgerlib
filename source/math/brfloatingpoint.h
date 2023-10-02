/***************************************

	Floating point math functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFLOATINGPOINT_H__
#define __BRFLOATINGPOINT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRUCTS_H__
#include "brstructs.h"
#endif

#ifndef __BRINTRINSICS_H__
#include "brintrinsics.h"
#endif

#ifndef __BRTEMPLATES_H__
#include "brtemplates.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

/* BEGIN */
namespace Burger {

#if defined(BURGER_X86) || defined(DOXYGEN)

/** 0.5f,-0.5f for roundup / rounddown */
extern const uint32_float_t g_X86HalfAndNegHalf[2];

enum e8087Precision {
	/** Use 24 bit / float precision (DirectX default) */
	k8087Precision24Bit = 0,

	/** Invalid value, do not use. */
	k8087PrecisionInvalid = 1,

	/** Use 56 bit / double precision */
	k8087Precision56Bit = 2,

	/** Use 64 bit / long double precision (Power up default) */
	k8087Precision64Bit = 3
};

extern e8087Precision BURGER_API get_8087_precision(void) BURGER_NOEXCEPT;
extern e8087Precision BURGER_API set_8087_precision(
	e8087Precision uInput) BURGER_NOEXCEPT;

enum e8087Rounding {

	/** Use round to nearest integer (Banker's rounding) */
	k8087RoundingNearest = 0,

	/** Use get_floor() rounding (-Infinity) */
	k8087RoundingDown = 1,

	/** Use get_ceiling() rounding (+Infinity) */
	k8087RoundingUp = 2,

	/** Use fraction truncation rounding (To Zero) */
	k8087RoundingTruncate = 3
};

extern e8087Rounding BURGER_API get_8087_rounding(void) BURGER_NOEXCEPT;
extern e8087Rounding BURGER_API set_8087_rounding(
	e8087Rounding uInput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_INTEL) || defined(DOXYGEN)
extern uint32_t BURGER_API set_mxcsr_flags(
	uint32_t uOrFlags, uint32_t uAndFlags) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_PPC) || defined(DOXYGEN)

enum ePowerPCRounding {

	/** Use round to nearest integer (Banker's rounding) */
	kPPCRoundingNearest = 0,

	/** Use fraction truncation rounding (To Zero) */
	kPPCRoundingTruncate = 1,

	/** Use get_ceiling() rounding (+Infinity) */
	kPPCRoundingUp = 2,

	/** Use get_floor() rounding (-Infinity) */
	kPPCRoundingDown = 3
};

extern ePowerPCRounding BURGER_API get_PowerPC_rounding(void);
extern ePowerPCRounding BURGER_API set_PowerPC_rounding(
	ePowerPCRounding uInput);

#endif

#if (defined(BURGER_METROWERKS) && defined(BURGER_68881))
#pragma parameter __D0 get_68881_FPCR
// fmove fpcr,d0
uint32_t get_68881_FPCR(void) = {0xF200, 0xB000};

#pragma parameter set_68881_FPCR(__D0)
// fmove d0,fpcr
void set_68881_FPCR(uint32_t) = {0xF200, 0x9000};
#endif

extern float BURGER_API get_sign(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sign(double dInput) BURGER_NOEXCEPT;

BURGER_INLINE BURGER_CONSTEXPR float square(float fInput) BURGER_NOEXCEPT
{
	return fInput * fInput;
}

BURGER_INLINE BURGER_CONSTEXPR double square(double dInput) BURGER_NOEXCEPT
{
	return dInput * dInput;
}

#if defined(BURGER_WATCOM) && defined(BURGER_X86) && !defined(DOXYGEN)
BURGER_INLINE float absolute(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(::std::fabs(fInput));
}
BURGER_INLINE double absolute(double dInput) BURGER_NOEXCEPT
{
	return ::std::fabs(dInput);
}

#elif ((defined(BURGER_METROWERKS) || defined(BURGER_GHS)) && \
	defined(BURGER_PPC)) && \
	!defined(DOXYGEN)
BURGER_INLINE float absolute(float fInput) BURGER_NOEXCEPT
{
	return __fabsf(fInput);
}
BURGER_INLINE double absolute(double dInput) BURGER_NOEXCEPT
{
	return __fabs(dInput);
}

#elif ((defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
	defined(BURGER_SNSYSTEMS)) && \
	!defined(DOXYGEN)
BURGER_INLINE float absolute(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fabsf(fInput);
}
BURGER_INLINE double absolute(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fabs(dInput);
}

#elif (defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER)) && \
	!defined(DOXYGEN)

BURGER_INLINE float absolute(float fInput) BURGER_NOEXCEPT
{
	// Only available on ARM and Itanium, PPC and Intel uses the double version
#if defined(BURGER_ARM) || defined(BURGER_ITANIUM)
	return fabsf(fInput);
#else
	return static_cast<float>(fabs(fInput));
#endif
}

BURGER_INLINE double absolute(double dInput) BURGER_NOEXCEPT
{
	return fabs(dInput);
}

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68881)) && !defined(DOXYGEN)
// Use the fabs 68881 instruction
// clang-format off
static float BURGER_API absolute(float fInput:__FP0):__FP0 = {0xF200, 0x0018};
static double BURGER_API absolute(double dInput:__FP0):__FP0 = {0xF200, 0x0018};
// clang-format on
#else
extern float BURGER_API absolute(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API absolute(double dInput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(::std::sqrt(fInput));
}
BURGER_INLINE double square_root(double dInput) BURGER_NOEXCEPT
{
	return ::std::sqrt(dInput);
}

#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
	return __fsqrts(fInput);
}
BURGER_INLINE double square_root(double dInput) BURGER_NOEXCEPT
{
	return __fsqrt(dInput);
}

#elif defined(BURGER_PS3) && !defined(DOXYGEN)
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fsqrts(fInput);
}
BURGER_INLINE double square_root(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fsqrt(dInput);
}

#elif ((defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	defined(BURGER_GNUC) || defined(BURGER_CLANG) || \
	defined(BURGER_SNSYSTEMS)) && \
	!defined(DOXYGEN)
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
#if defined(BURGER_METROWERKS) && defined(BURGER_X86)
	return static_cast<float>(__builtin_sqrt(fInput));
#else
	return __builtin_sqrtf(fInput);
#endif
}
BURGER_INLINE double square_root(double dInput) BURGER_NOEXCEPT
{
	return __builtin_sqrt(dInput);
}

#elif (defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER)) && \
	(defined(BURGER_INTEL) || defined(BURGER_ARM))

#if defined(BURGER_AMD64) || defined(BURGER_ARM)
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
	return sqrtf(fInput);
}
#else
BURGER_INLINE float square_root(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(sqrt(static_cast<double>(fInput)));
}
#endif

BURGER_INLINE double square_root(double dInput) BURGER_NOEXCEPT
{
	return sqrt(dInput);
}

#else
extern float BURGER_API square_root(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API square_root(double dInput) BURGER_NOEXCEPT;
#endif

BURGER_INLINE BURGER_CONSTEXPR float int_to_float(
	int32_t iInput) BURGER_NOEXCEPT
{
	return static_cast<float>(iInput);
}

BURGER_INLINE float int_to_float(const int32_t* pInput) BURGER_NOEXCEPT
{
	return static_cast<float>(pInput[0]);
}

BURGER_INLINE void int_to_float(float* pOutput, int32_t iInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(iInput);
}

BURGER_INLINE BURGER_CONSTEXPR float fixed_to_float(
	Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(fInput) * (1.0f / 65536.0f);
}
BURGER_INLINE float fixed_to_float(const Fixed32* pInput) BURGER_NOEXCEPT
{
	return static_cast<float>(pInput[0]) * (1.0f / 65536.0f);
}
BURGER_INLINE void fixed_to_float(
	float* pOutput, Fixed32 fInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(fInput) * (1.0f / 65536.0f);
}

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)

BURGER_INLINE void int_to_float(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	// Load in VMX128
	__vector4 vTemp = __lvlx(pInput, 0);
	// Convert to float
	vTemp = __vcfsx(vTemp, 0);
	// Splat for non-aligned memory writes
	vTemp = __vspltw(vTemp, 0);
	// Store 32 bit float
	__stvewx(vTemp, pOutput, 0);
}

BURGER_INLINE void fixed_to_float(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	// Load in VMX128
	__vector4 vTemp = __lvlx(pInput, 0);
	// Use 16.16 fixed point conversion
	vTemp = __vcfsx(vTemp, 16);
	// Splat for non-aligned memory writes
	vTemp = __vspltw(vTemp, 0);
	// Store 32 bit float
	__stvewx(vTemp, pOutput, 0);
}

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

BURGER_INLINE void int_to_float(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	// Load in VMX128
	vector int vTemp = vec_lvlx(0, pInput);
	// Convert to float
	vector float vfTemp = vec_vcfsx(vTemp, 0);
	// Splat for non-aligned memory writes
	vfTemp = vec_vspltw(vfTemp, 0);
	// Store 32 bit float
	vec_stvewx(vfTemp, 0, pOutput);
}

BURGER_INLINE void fixed_to_float(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	// Load in VMX128
	vector int vTemp = vec_lvlx(0, pInput);
	// Use 16.16 fixed point conversion
	vector float vfTemp = vec_vcfsx(vTemp, 16);
	// Splat for non-aligned memory writes
	vfTemp = vec_vspltw(vfTemp, 0);
	// Store 32 bit float
	vec_stvewx(vfTemp, 0, pOutput);
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_POWERPC64)) && \
	!defined(DOXYGEN)
BURGER_INLINE void int_to_float(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	// Step one, load the integer into the vector
	// It can be in different slots based on the low 4 bit of the address
	__vector int vTemp = __builtin_altivec_lvewx(0, pInput);

	// Determine where to slide the result
	__vector unsigned char vPermute = __builtin_altivec_lvsl(0,
		reinterpret_cast<const char*>(pInput) -
			reinterpret_cast<uintptr_t>(pOutput));

	// Next, convert the value into a float
	vTemp = (__vector int)__builtin_altivec_vcfsx(vTemp, 0);

	// Next, move the value to the slot of the output pointer
	vTemp = __builtin_altivec_vperm_4si(vTemp, vTemp, vPermute);

	// Store 32 bit float (Premoved to proper location)
	__builtin_altivec_stvewx(vTemp, 0, pOutput);
}

BURGER_INLINE void fixed_to_float(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	// Step one, load the integer into the vector
	// It can be in different slots based on the low 4 bit of the address
	__vector int vTemp = __builtin_altivec_lvewx(0, pInput);

	// Determine where to slide the result
	__vector unsigned char vPermute = __builtin_altivec_lvsl(0,
		reinterpret_cast<const char*>(pInput) -
			reinterpret_cast<uintptr_t>(pOutput));

	// Next, convert the fixed point 16.16 into a float
	vTemp = (__vector int)__builtin_altivec_vcfsx(vTemp, 16);

	// Next, move the value to the slot of the output pointer
	vTemp = __builtin_altivec_vperm_4si(vTemp, vTemp, vPermute);

	// Store 32 bit float (Premoved to proper location)
	__builtin_altivec_stvewx(vTemp, 0, pOutput);
}

#else
BURGER_INLINE void int_to_float(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(pInput[0]);
}
BURGER_INLINE void fixed_to_float(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(pInput[0]) * (1.0f / 65536.0f);
}
#endif

BURGER_INLINE BURGER_CONSTEXPR float interpolate(
	float fFrom, float fTo, float fFactor) BURGER_NOEXCEPT
{
	return ((fTo - fFrom) * fFactor) + fFrom;
}

BURGER_INLINE BURGER_CONSTEXPR double interpolate(
	double dFrom, double dTo, double dFactor) BURGER_NOEXCEPT
{
	return ((dTo - dFrom) * dFactor) + dFrom;
}

template<typename T>
BURGER_INLINE T clamp(T Input, T Min, T Max) BURGER_NOEXCEPT
{
	return minimum(maximum(Input, Min), Max);
}

extern uint_t BURGER_API is_NaN(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_NaN(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_infinite(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_infinite(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_finite(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_finite(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_normal(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_normal(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API get_sign_bit(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API get_sign_bit(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API equal_with_epsilon(
	float fInput1, float fInput2) BURGER_NOEXCEPT;
extern uint_t BURGER_API equal_with_epsilon(
	double dInput1, double dInput2) BURGER_NOEXCEPT;
extern uint_t BURGER_API equal_with_epsilon(
	float fInput1, float fInput2, float fEpsilon) BURGER_NOEXCEPT;
extern uint_t BURGER_API equal_with_epsilon(
	double dInput1, double dInput2, double dEpsilon) BURGER_NOEXCEPT;
extern float BURGER_API get_floor(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_floor(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_ceiling(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_ceiling(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_round(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_round(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API round_to_zero(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API round_to_zero(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API modulo_radians(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API modulo_radians(double dInput) BURGER_NOEXCEPT;

#if defined(BURGER_X86) || defined(DOXYGEN)
extern float BURGER_API sine_387(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API sine_387(double fInput) BURGER_NOEXCEPT;
extern float BURGER_API cosine_387(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API cosine_387(double fInput) BURGER_NOEXCEPT;
#endif

extern float BURGER_API get_sine_3_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_cosine_3_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_sine_5_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_cosine_5_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_sine_6_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_cosine_6_digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_sine(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API get_cosine(float fInput) BURGER_NOEXCEPT;

extern double BURGER_API get_sine_7_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_cosine_7_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sine_12_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_cosine_12_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sine_18_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_cosine_18_digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sine(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API get_cosine(double dInput) BURGER_NOEXCEPT;

extern float BURGER_API get_tangent(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_tangent(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_arcsine(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_arcsine(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_arccosine(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_arccosine(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_arctangent(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_arctangent(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_arctangent2(float fSin, float fCos) BURGER_NOEXCEPT;
extern double BURGER_API get_arctangent2(
	double dSin, double dCos) BURGER_NOEXCEPT;
extern float BURGER_API get_power(float fX, float fY) BURGER_NOEXCEPT;
extern double BURGER_API get_power(double dX, double dY) BURGER_NOEXCEPT;
extern float BURGER_API get_exponent(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_exponent(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_logarithm(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_logarithm(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_logarithm2(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_logarithm2(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_logarithm10(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_logarithm10(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API get_fraction(
	float fInput, float* pInteger) BURGER_NOEXCEPT;
extern double BURGER_API get_fraction(
	double dInput, double* pInteger) BURGER_NOEXCEPT;
extern float BURGER_API get_modulo(
	float fInput, float fDivisor) BURGER_NOEXCEPT;
extern double BURGER_API get_modulo(
	double dInput, double dDivisor) BURGER_NOEXCEPT;

extern double BURGER_API little_endian_load_extended(
	const Float80Bit pInput) BURGER_NOEXCEPT;
extern double BURGER_API big_endian_load_extended(
	const Float80Bit pInput) BURGER_NOEXCEPT;

extern long BURGER_API convert_to_DirectSound_volume(
	uint_t uInput) BURGER_NOEXCEPT;
extern long BURGER_API convert_to_DirectSound_volume(
	float fInput) BURGER_NOEXCEPT;
extern long BURGER_API convert_to_DirectSound_pan(
	uint_t uInput) BURGER_NOEXCEPT;
extern long BURGER_API convert_to_DirectSound_pan(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API convert_to_AudioUnit_volume(
	uint_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API convert_to_AudioUnit_pan(uint_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API decibels_to_amplitude_ratio(
	float fDecibels) BURGER_NOEXCEPT;
extern float BURGER_API amplitude_ratio_to_decibels(
	float fVolume) BURGER_NOEXCEPT;

}

/* END */

#endif
