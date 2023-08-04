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

extern e8087Precision BURGER_API set_8087_precision(
	e8087Precision uInput) BURGER_NOEXCEPT;
extern e8087Precision BURGER_API get_8087_precision(void) BURGER_NOEXCEPT;

enum e8087Rounding {
	ROUNDTONEAREST = 0, ///< Use round to nearest integer (Banker's rounding)
	ROUNDDOWN = 1,      ///< Use Floor() rounding (-Infinity)
	ROUNDUP = 2,        ///< Use Ceil() rounding (+Infinity)
	ROUNDTRUNCATE = 3   ///< Use fraction truncation rounding (To Zero)
};
extern e8087Rounding BURGER_API Set8087Rounding(e8087Rounding eInput);
extern e8087Rounding BURGER_API Get8087Rounding(void);

#endif

#if defined(BURGER_PPC) || defined(DOXYGEN)

enum ePowerPCRounding {
	ROUNDTONEAREST = 0, ///< Use round to nearest integer (Banker's rounding)
	ROUNDTRUNCATE = 1,  ///< Use fraction truncation rounding (To Zero)
	ROUNDUP = 2,        ///< Use Ceil() rounding (+Infinity)
	ROUNDDOWN = 3       ///< Use Floor() rounding (-Infinity)
};

extern ePowerPCRounding BURGER_API SetPowerPCRounding(ePowerPCRounding eInput);
extern ePowerPCRounding BURGER_API GetPowerPCRounding(void);

#endif

#if defined(BURGER_68K) && defined(BURGER_METROWERKS)
extern float BURGER_API get_sign(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API get_sign(double dInput) BURGER_NOEXCEPT;

#elif defined(BURGER_PPC)
BURGER_INLINE float get_sign(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(
		__fsel(fInput, 0.5f, -0.5f) + __fsel(-fInput, -0.5f, 0.5f));
}
BURGER_INLINE double get_sign(double dInput) BURGER_NOEXCEPT
{
	return __fsel(dInput, 0.5, -0.5) + __fsel(-dInput, -0.5, 0.5);
}
#else
BURGER_INLINE float get_sign(float fInput) BURGER_NOEXCEPT
{
#if defined(BURGER_PS4) || defined(BURGER_PS5)
	uint32_t uTemp;
	uint32_t uTemp2;
	__asm__(
		"vmovd %2,%0\n"          // Move to an integer register
		"movl $0x7F000000, %1\n" // Load 1.0, shifted up by one bit
		"shll $1,%0\n"           // Obtain the sign bit in carry
		"rcrl $1,%1\n"           // Convert 1.0 to 1.0 or -1.0 depending on sign
		"negl %0\n"              // Set carry if not zero
		"sbbl %0, %0\n"          // Temp is mask, zero or 0xFFFFFFFF
		"andl %1, %0\n"          // Mask 1.0/-1.0f to zero if zero
		"movd %0,%2"             // Move result to float
		: "=r"(uTemp), "=r"(uTemp2), "+x"(fInput));
	return fInput;
#else
	return ((fInput > 0.0f) ? 1.0f : ((fInput < 0.0f) ? -1.0f : 0.0f));
#endif
}
BURGER_INLINE double get_sign(double dInput) BURGER_NOEXCEPT
{
#if defined(BURGER_PS4) || defined(BURGER_PS5)
	uint64_t uTemp;
	uint64_t uTemp2;
	__asm__(
		"vmovd %2,%0\n"                  // Move to an integer register
		"movq $0x7FE0000000000000, %1\n" // Load 1.0, shifted up by one bit
		"shlq $1,%0\n"                   // Obtain the sign bit in carry
		"rcrq $1,%1\n"  // Convert 1.0 to 1.0 or -1.0 depending on sign
		"negq %0\n"     // Set carry if not zero
		"sbbq %0, %0\n" // Temp is mask, zero or 0xFFFFFFFF
		"andq %1, %0\n" // Mask 1.0/-1.0f to zero if zero
		"movd %0,%2"    // Move result to float
		: "=r"(uTemp), "=r"(uTemp2), "+x"(dInput));
	return dInput;
#else
	return ((dInput > 0.0) ? 1.0 : ((dInput < 0.0) ? -1.0 : 0.0));
#endif
}
#endif

BURGER_INLINE BURGER_CONSTEXPR float Sqr(float fInput) BURGER_NOEXCEPT
{
	return fInput * fInput;
}
BURGER_INLINE BURGER_CONSTEXPR double Sqr(double dInput) BURGER_NOEXCEPT
{
	return dInput * dInput;
}

#if defined(BURGER_WATCOM)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(::std::fabs(fInput));
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return ::std::fabs(dInput);
}
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(::std::sqrt(fInput));
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return ::std::sqrt(dInput);
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fabsf(fInput);
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fabs(dInput);
}
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(__builtin_sqrt(fInput));
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return __builtin_sqrt(dInput);
}

#elif defined(BURGER_METROWERKS) && defined(BURGER_PPC)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return __fabsf(fInput);
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return __fabs(dInput);
}
extern float BURGER_API Sqrt(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Sqrt(double dInput) BURGER_NOEXCEPT;

#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(fabs(fInput));
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return fabs(dInput);
}
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return __fsqrts(fInput);
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return __fsqrt(dInput);
}

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fabsf(fInput);
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fabs(dInput);
}
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return sqrtf(fInput);
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return sqrt(dInput);
}

#elif (defined(BURGER_PS4) || defined(BURGER_VITA)) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fabsf(fInput);
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fabs(dInput);
}
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return __builtin_sqrtf(fInput);
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return __builtin_sqrt(dInput);
}

#elif (defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER)) && \
	(defined(BURGER_INTEL) || defined(BURGER_ARM))

#if defined(BURGER_ARM)
BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return fabsf(fInput);
}
#else
BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(fabs(fInput));
}
#endif

BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return fabs(dInput);
}

#if defined(BURGER_AMD64) || defined(BURGER_ARM)
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return sqrtf(fInput);
}
#else
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(sqrt(static_cast<double>(fInput)));
}
#endif

BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return sqrt(dInput);
}

#elif defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_LINUX)

BURGER_INLINE float Abs(float fInput) BURGER_NOEXCEPT
{
	return __builtin_fabsf(fInput);
}
BURGER_INLINE double Abs(double dInput) BURGER_NOEXCEPT
{
	return __builtin_fabs(dInput);
}

#if defined(BURGER_POWERPC64)
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return __sqrt(fInput);
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return __sqrt(dInput);
}
#else
BURGER_INLINE float Sqrt(float fInput) BURGER_NOEXCEPT
{
	return __builtin_sqrtf(fInput);
}
BURGER_INLINE double Sqrt(double dInput) BURGER_NOEXCEPT
{
	return __builtin_sqrt(dInput);
}
#endif

#else

extern float BURGER_API Abs(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Abs(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API Sqrt(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API Sqrt(double dInput) BURGER_NOEXCEPT;

#endif

BURGER_INLINE BURGER_CONSTEXPR float IntToFloat(int32_t iInput) BURGER_NOEXCEPT
{
	return static_cast<float>(iInput);
}
BURGER_INLINE float IntToFloat(const int32_t* pInput) BURGER_NOEXCEPT
{
	return static_cast<float>(pInput[0]);
}
BURGER_INLINE void IntToFloat(float* pOutput, int32_t iInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(iInput);
}
BURGER_INLINE BURGER_CONSTEXPR float FixedToFloat(
	Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<float>(fInput) * (1.0f / 65536.0f);
}
BURGER_INLINE float FixedToFloat(const Fixed32* pInput) BURGER_NOEXCEPT
{
	return static_cast<float>(pInput[0]) * (1.0f / 65536.0f);
}
BURGER_INLINE void FixedToFloat(float* pOutput, Fixed32 fInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(fInput) * (1.0f / 65536.0f);
}

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)

BURGER_INLINE void IntToFloat(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	__vector4 vTemp = __lvlx(pInput, 0); // Load in VMX128
	vTemp = __vcfsx(vTemp, 0);           // Convert
	vTemp = __vspltw(vTemp, 0);          // Splat
	__stvewx(vTemp, pOutput, 0);         // Store 32 bit
}

BURGER_INLINE void FixedToFloat(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	__vector4 vTemp = __lvlx(pInput, 0); // Load in VMX128
	vTemp = __vcfsx(vTemp, 16);          // Use 16.16 fixed point conversion
	vTemp = __vspltw(vTemp, 0);          // Splat
	__stvewx(vTemp, pOutput, 0);         // Store 32 bit
}

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

BURGER_INLINE void IntToFloat(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	vector int vTemp = vec_lvlx(0, pInput);    // Load in VMX128
	vector float vfTemp = vec_vcfsx(vTemp, 0); // Convert to float
	vfTemp = vec_vspltw(vfTemp, 0);            // Splat
	vec_stvewx(vfTemp, 0, pOutput);            // Store 32 bit
}

BURGER_INLINE void FixedToFloat(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	vector int vTemp = vec_lvlx(0, pInput); // Load in VMX128
	vector float vfTemp =
		vec_vcfsx(vTemp, 16);               // Use 16.16 fixed point conversion
	vfTemp = vec_vspltw(vfTemp, 0);         // Splat
	vec_stvewx(vfTemp, 0, pOutput);         // Store 32 bit
}

#else
BURGER_INLINE void IntToFloat(
	float* pOutput, const int32_t* pInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(pInput[0]);
}
BURGER_INLINE void FixedToFloat(
	float* pOutput, const Fixed32* pInput) BURGER_NOEXCEPT
{
	pOutput[0] = static_cast<float>(pInput[0]) * (1.0f / 65536.0f);
}
#endif

BURGER_INLINE BURGER_CONSTEXPR float Interpolate(
	float fFrom, float fTo, float fFactor) BURGER_NOEXCEPT
{
	return ((fTo - fFrom) * fFactor) + fFrom;
}
BURGER_INLINE BURGER_CONSTEXPR double Interpolate(
	double dFrom, double dTo, double dFactor) BURGER_NOEXCEPT
{
	return ((dTo - dFrom) * dFactor) + dFrom;
}
BURGER_INLINE BURGER_CONSTEXPR float Square(float fInput) BURGER_NOEXCEPT
{
	return fInput * fInput;
}
BURGER_INLINE BURGER_CONSTEXPR double Square(double dInput) BURGER_NOEXCEPT
{
	return dInput * dInput;
}

BURGER_INLINE float Clamp(float fIn, float fMin, float fMax) BURGER_NOEXCEPT
{
	return Min(Max(fIn, fMin), fMax);
}
BURGER_INLINE double Clamp(double dIn, double dMin, double dMax) BURGER_NOEXCEPT
{
	return Min(Max(dIn, dMin), dMax);
}

extern uint_t BURGER_API IsNan(float fInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API IsNan(double dInput) BURGER_NOEXCEPT;
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
