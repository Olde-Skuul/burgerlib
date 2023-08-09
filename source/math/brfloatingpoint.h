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

#ifndef __BRINTRINSICS_H__
#include "brintrinsics.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

#ifndef __BRSNSYSTEMS_H__
#include "brsnsystems.h"
#endif

#ifndef __BRINTELCOMPILER_H__
#include "brintelcompiler.h"
#endif

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

#if defined(BURGER_PS3) && !defined(__BRPS3TYPES_H__)
#include "brps3types.h"
#endif

#if defined(BURGER_PS4) && !defined(__BRPS4TYPES_H__)
#include "brps4types.h"
#endif

#if defined(BURGER_WIIU) && !defined(__BRWIIUTYPES_H__)
#include "brwiiutypes.h"
#endif

#ifndef __BRALGORITHM_H__
#include "bralgorithm.h"
#endif

#ifndef __BRSTRUCTS_H__
#include "brstructs.h"
#endif

/* BEGIN */
namespace Burger {

#if defined(BURGER_X86) || defined(DOXYGEN)

/** 8087 control word to round down with 64 bit precision */
extern const uint16_t g_X86RoundDownFlag;

/** 1.0f,-1.0f for roundup / rounddown */
extern const uint32_float_t g_X86OneAndNegOne[2];

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

	/** Use Floor() rounding (-Infinity) */
	k8087RoundingDown = 1,

	/** Use Ceil() rounding (+Infinity) */
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

	/** Use Ceil() rounding (+Infinity) */
	kPPCRoundingUp = 2,

	/** Use Floor() rounding (-Infinity) */
	kPPCRoundingDown = 3
};

extern ePowerPCRounding BURGER_API get_PowerPC_rounding(void);
extern ePowerPCRounding BURGER_API set_PowerPC_rounding(
	ePowerPCRounding uInput);

#endif

#if defined(BURGER_PPC) && !defined(DOXYGEN)
// Zero and -Zero will cancel out. Otherwise, adding two 0.5 will become 1.0
// This code is branchless for performance
BURGER_INLINE float get_sign(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(
		__fsel(fInput, 0.5f, -0.5f) + __fsel(-fInput, -0.5f, 0.5f));
}
BURGER_INLINE double get_sign(double dInput) BURGER_NOEXCEPT
{
	return __fsel(dInput, 0.5, -0.5) + __fsel(-dInput, -0.5, 0.5);
}

#elif (defined(BURGER_68K) && defined(BURGER_METROWERKS) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5)) && \
	!defined(DOXYGEN)
extern float BURGER_API get_sign(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sign(double dInput) BURGER_NOEXCEPT;

#else
BURGER_INLINE float get_sign(float fInput) BURGER_NOEXCEPT
{
	return ((fInput > 0.0f) ? 1.0f : ((fInput < 0.0f) ? -1.0f : 0.0f));
}
BURGER_INLINE double get_sign(double dInput) BURGER_NOEXCEPT
{
	return ((dInput > 0.0) ? 1.0 : ((dInput < 0.0) ? -1.0 : 0.0));
}
#endif

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

BURGER_INLINE float clamp(float fIn, float fMin, float fMax) BURGER_NOEXCEPT
{
	return minimum(maximum(fIn, fMin), fMax);
}

BURGER_INLINE double clamp(double dIn, double dMin, double dMax) BURGER_NOEXCEPT
{
	return minimum(maximum(dIn, dMin), dMax);
}

extern uint_t BURGER_API is_NaN(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_NaN(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsInf(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsInf(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsFinite(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsFinite(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsNormal(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsNormal(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API SignBit(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API SignBit(double dInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API EqualWithEpsilon(
	float fInput1, float fInput2) BURGER_NOEXCEPT;
extern uint_t BURGER_API EqualWithEpsilon(
	double dInput1, double dInput2) BURGER_NOEXCEPT;
extern uint_t BURGER_API EqualWithEpsilon(
	float fInput1, float fInput2, float fEpsilon) BURGER_NOEXCEPT;
extern uint_t BURGER_API EqualWithEpsilon(
	double dInput1, double dInput2, double dEpsilon) BURGER_NOEXCEPT;
extern float BURGER_API Floor(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Floor(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Ceil(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Ceil(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Round(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Round(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API RoundToZero(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API RoundToZero(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API ModuloRadians(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API ModuloRadians(double dInput) BURGER_NOEXCEPT;
#if defined(BURGER_X86) || defined(DOXYGEN)
extern float BURGER_API Sin387(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Sin387(double fInput) BURGER_NOEXCEPT;
extern float BURGER_API Cos387(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Cos387(double fInput) BURGER_NOEXCEPT;
#endif
extern float BURGER_API Sin3Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Cos3Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Sin5Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Cos5Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Sin6Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Cos6Digits(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Sin(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API Cos(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Sin7Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Cos7Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Sin12Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Cos12Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Sin18Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Cos18Digits(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Sin(double dInput) BURGER_NOEXCEPT;
extern double BURGER_API Cos(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Tan(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Tan(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API ASin(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API ASin(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API ACos(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API ACos(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API ATan(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API ATan(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API ATan2(float fSin, float fCos) BURGER_NOEXCEPT;
extern double BURGER_API ATan2(double dSin, double dCos) BURGER_NOEXCEPT;
extern float BURGER_API Pow(float fX, float fY) BURGER_NOEXCEPT;
extern double BURGER_API Pow(double dX, double dY) BURGER_NOEXCEPT;
extern float BURGER_API Exp(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Exp(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Log(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Log(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Log2(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Log2(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Log10(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Log10(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Modf(float fInput, float* pInteger) BURGER_NOEXCEPT;
extern double BURGER_API Modf(double dInput, double* pInteger) BURGER_NOEXCEPT;
extern float BURGER_API Fmod(float fInput, float fDivisor) BURGER_NOEXCEPT;
extern double BURGER_API Fmod(double dInput, double dDivisor) BURGER_NOEXCEPT;
extern double BURGER_API LittleEndianLoadExtended(
	const Float80Bit pInput) BURGER_NOEXCEPT;
extern double BURGER_API BigEndianLoadExtended(
	const Float80Bit pInput) BURGER_NOEXCEPT;
extern long BURGER_API ConvertToDirectSoundVolume(
	uint_t uInput) BURGER_NOEXCEPT;
extern long BURGER_API ConvertToDirectSoundVolume(float fInput) BURGER_NOEXCEPT;
extern long BURGER_API ConvertToDirectSoundPan(uint_t uInput) BURGER_NOEXCEPT;
extern long BURGER_API ConvertToDirectSoundPan(float fInput) BURGER_NOEXCEPT;
extern float BURGER_API ConvertToAudioUnitVolume(uint_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API ConvertToAudioUnitPan(uint_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API ConvertToAudioUnitVolume(uint_t uInput) BURGER_NOEXCEPT;
extern float BURGER_API DecibelsToAmplitudeRatio(
	float fDecibels) BURGER_NOEXCEPT;
extern float BURGER_API AmplitudeRatioToDecibels(float fVolume) BURGER_NOEXCEPT;

}

/* END */

#endif
