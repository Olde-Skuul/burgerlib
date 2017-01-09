/***************************************

	Fixed point math functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRFLOATINGPOINT_H__
#define __BRFLOATINGPOINT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
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

/* BEGIN */
namespace Burger {

#define	BURGER_PI 3.1415926535897932384626	///< Pretty accurate, eh?

typedef Word8 Float80Bit[10];				///< 80 bit float (Extended)

struct Word32ToFloat {
	union {
		Word32 w;		///< Value in binary
		float f;		///< Value as a 32 bit float
	};
	BURGER_INLINE operator float() const { return f; }
	BURGER_INLINE Word32ToFloat & operator = (float fInput) { f = fInput; return *this; }
	BURGER_INLINE Word32 GetWord(void) const { return w; }
	BURGER_INLINE void SetWord(Word32 uInput) { w = uInput; }
};

struct Word64ToDouble {
	union {
		Word64 w;		///< Value in binary
		double d;		///< Value as a 64 bit float
		Word32 w32[2];	///< Array of 2 32 bit words that map on the 64 bit value
	};
	BURGER_INLINE operator double() const { return d; }
	BURGER_INLINE Word64ToDouble & operator = (double dInput) { d = dInput; return *this; }
	BURGER_INLINE Word64 GetWord(void) const { return w; }
	BURGER_INLINE void SetWord(Word64 uInput) { w = uInput; }
#if defined(BURGER_BIGENDIAN)
	BURGER_INLINE Word32 GetLowWord(void) const { return w32[1]; }
	BURGER_INLINE Word32 GetHiWord(void) const { return w32[0]; }
	BURGER_INLINE void SetLowWord(Word32 uInput) { w32[1] = uInput; }
	BURGER_INLINE void SetHiWord(Word32 uInput) { w32[0] = uInput; }
#else
	BURGER_INLINE Word32 GetLowWord(void) const { return w32[0]; }
	BURGER_INLINE Word32 GetHiWord(void) const { return w32[1]; }
	BURGER_INLINE void SetLowWord(Word32 uInput) { w32[0] = uInput; }
	BURGER_INLINE void SetHiWord(Word32 uInput) { w32[1] = uInput; }
#endif
};

struct Vector_128Float32 {
	union {
		float BURGER_ALIGN(f[4],16);		///< Value as 4 32 bit floats
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Float64 {
	union {
		double BURGER_ALIGN(d[2],16);		///< Value as 2 64 bit floats
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Int32 {
	union {
		Int32 BURGER_ALIGN(i[4],16);		///< Value as 4 signed 32 bit integers
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Word64 {
	union {
		Word64 BURGER_ALIGN(u[2],16);		///< Value as 2 unsigned 64 bit integers
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Word32 {
	union {
		Word32 BURGER_ALIGN(u[4],16);		///< Value as 4 unsigned 32 bit integers
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Word8 {
	union {
		Word8 BURGER_ALIGN(u[16],16);		///< Value as 16 unsigned 8 bit integers
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

struct Vector_128Word16 {
	union {
		Word16 BURGER_ALIGN(u[8],16);		///< Value as 8 unsigned 16 bit integers
		Vector_128 v;	///< Value as a 128 bit vector
	};
	BURGER_INLINE operator Vector_128() const { return v; }
};

extern const Word64ToDouble g_dOne;			///< Constant for 1.0 in the double format.
extern const Word64ToDouble g_dNegOne;		///< Constant for -1.0 in the double format.
extern const Word64ToDouble g_dHalf;		///< Constant for 0.5 in the double format.
extern const Word64ToDouble g_dNegHalf;		///< Constant for -0.5 in the double format.
extern const Word64ToDouble g_dInf;			///< Constant for Infinity in the double format.
extern const Word64ToDouble g_dNegInf;		///< Constant for -Infinity in the double format.
extern const Word64ToDouble g_dNan;			///< Constant for signaling Not a Number (NaN) in the double format.
extern const Word64ToDouble g_dNegNan;		///< Constant for negative signaling Not a Number (NaN) in the double format.
extern const Word64ToDouble g_dQNan;		///< Constant for non signaling Not a Number (Nan) in the double format.
extern const Word64ToDouble g_dNegQNan;		///< Constant for negative non signaling Not a Number (Nan) in the double format.
extern const Word64ToDouble g_dMin;			///< Constant for the smallest number (Non-zero) in the double format.
extern const Word64ToDouble g_dNegMin;		///< Constant for the negative smallest number (Non-zero) in the double format.
extern const Word64ToDouble g_dMax;			///< Constant for the largest number in the double format.
extern const Word64ToDouble g_dNegMax;		///< Constant for the negative largest number in the double format.
extern const Word64ToDouble g_dEpsilon;		///< Constant for Epsilon in the double format.
extern const Word64ToDouble g_dNegEpsilon;	///< Constant for negative Epsilon in the double format.
extern const Word64ToDouble g_dQuarterPi;	///< Constant for Pi*0.25 in the double format.
extern const Word64ToDouble g_dNegQuarterPi;///< Constant for -Pi*0.25 in the double format.
extern const Word64ToDouble g_dHalfPi;		///< Constant for Pi*0.5 in the double format.
extern const Word64ToDouble g_dNegHalfPi;	///< Constant for -Pi*0.5 in the double format.
extern const Word64ToDouble g_dPi;			///< Constant for Pi in the double format.
extern const Word64ToDouble g_dNegPi;		///< Constant for -Pi in the double format.
extern const Word64ToDouble g_dPi2;			///< Constant for Pi*2 in the double format.
extern const Word64ToDouble g_dNegPi2;		///< Constant for -Pi*2 in the double format.
extern const Word64ToDouble g_dPi4;			///< Constant for Pi*4 in the double format.
extern const Word64ToDouble g_dNegPi4;		///< Constant for -Pi*4 in the double format.
extern const Word64ToDouble g_dReciprocalHalfPi;	///< Constant for 1/(Pi*0.5) in the double format.
extern const Word64ToDouble g_dReciprocalPi;		///< Constant for 1/Pi in the double format.
extern const Word64ToDouble g_dReciprocalPi2;		///< Constant for 1/(Pi*2) in the double format.
extern const Word64ToDouble g_dRadiansToDegrees;	///< Constant for 180/Pi in the double format.
extern const Word64ToDouble g_dDegreesToRadians;	///< Constant for Pi/180 in the double format.

extern const Word32ToFloat g_fOne;				///< Constant for 1.0f in the float format.
extern const Word32ToFloat g_fNegOne;			///< Constant for -1.0f in the float format.
extern const Word32ToFloat g_fHalf;				///< Constant for 0.5f in the float format.
extern const Word32ToFloat g_fNegHalf;			///< Constant for -0.5f in the float format.
extern const Word32ToFloat g_fInf;				///< Constant for Infinity in the float format.
extern const Word32ToFloat g_fNegInf;			///< Constant for -Infinity in the float format.
extern const Word32ToFloat g_fNan;				///< Constant for signaling Not a Number (NaN) in the float format.
extern const Word32ToFloat g_fNegNan;			///< Constant for negative signaling Not a Number (NaN) in the float format.
extern const Word32ToFloat g_fQNan;				///< Constant for non signaling Not a Number (Nan) in the float format.
extern const Word32ToFloat g_fNegQNan;			///< Constant for negative non signaling Not a Number (Nan) in the float format.
extern const Word32ToFloat g_fMin;				///< Constant for smallest number (Non-zero) in the float format.
extern const Word32ToFloat g_fNegMin;			///< Constant for negative smallest number (Non-zero) in the float format.
extern const Word32ToFloat g_fMax;				///< Constant for largest number in the float format.
extern const Word32ToFloat g_fNegMax;			///< Constant for negative largest number in the float format.
extern const Word32ToFloat g_fEpsilon;			///< Constant for Epsilon in the float format.
extern const Word32ToFloat g_fNegEpsilon;		///< Constant for negative Epsilon in the float format.
extern const Word32ToFloat g_fQuarterPi;		///< Constant for Pi*0.25f in the float format.
extern const Word32ToFloat g_fNegQuarterPi;		///< Constant for -Pi*0.25f in the float format.
extern const Word32ToFloat g_fHalfPi;			///< Constant for Pi*0.5f in the float format.
extern const Word32ToFloat g_fNegHalfPi;		///< Constant for -Pi*0.5f in the float format.
extern const Word32ToFloat g_fPi;				///< Constant for Pi in the float format.
extern const Word32ToFloat g_fNegPi;			///< Constant for -Pi in the float format.
extern const Word32ToFloat g_fPi2;				///< Constant for Pi*2 in the float format.
extern const Word32ToFloat g_fNegPi2;			///< Constant for -Pi*2 in the float format.
extern const Word32ToFloat g_fPi4;				///< Constant for Pi*4 in the float format.
extern const Word32ToFloat g_fNegPi4;			///< Constant for -Pi*4 in the float format.
extern const Word32ToFloat g_fReciprocalHalfPi;	///< Constant for 1/(Pi*0.5f) in the float format.
extern const Word32ToFloat g_fReciprocalPi;		///< Constant for 1/Pi in the float format.
extern const Word32ToFloat g_fReciprocalPi2;	///< Constant for 1/(Pi*2.0f) in the float format.
extern const Word32ToFloat g_fRadiansToDegrees;	///< Constant for 180/Pi in the float format.
extern const Word32ToFloat g_fDegreesToRadians;	///< Constant for Pi/180 in the float format.

#if defined(BURGER_X86) || defined(DOXYGEN)

enum e8087Precision {
	PRECISION24=0,	///< Use 24 bit / float precision (DirectX default)
	PRECISION56=2,	///< Use 56 bit / double precision
	PRECISION64=3	///< Use 64 bit / long double precision (Power up default)
};
extern e8087Precision BURGER_API Set8087Precision(e8087Precision eInput);

enum e8087Rounding {
	ROUNDTONEAREST=0,	///< Use round to nearest integer
	ROUNDDOWN=1,		///< Use Floor() rounding
	ROUNDUP=2,			///< Use Ceil() rounding
	ROUNDTRUNCATE=3		///< Use fraction truncation rounding
};
extern e8087Rounding BURGER_API Set8087Rounding(e8087Rounding eInput);
#endif

BURGER_INLINE float Sqr(float fInput) { return fInput*fInput; }
BURGER_INLINE double Sqr(double dInput) { return dInput*dInput; }

#if defined(BURGER_WATCOM)

BURGER_INLINE float Abs(float fInput) { return static_cast<float>(::std::fabs(fInput)); }
BURGER_INLINE double Abs(double dInput) { return ::std::fabs(dInput); }
BURGER_INLINE float Sqrt(float fInput) { return static_cast<float>(::std::sqrt(fInput)); }
BURGER_INLINE double Sqrt(double dInput) { return ::std::sqrt(dInput); }

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)

BURGER_INLINE float Abs(float fInput) { return ::std::__builtin_fabsf(fInput); }
BURGER_INLINE double Abs(double dInput) { return ::std::__builtin_fabs(dInput); }
BURGER_INLINE float Sqrt(float fInput) { return static_cast<float>(::std::__builtin_sqrt(fInput)); }
BURGER_INLINE double Sqrt(double dInput) { return ::std::__builtin_sqrt(dInput); }

#elif defined(BURGER_METROWERKS) && defined(BURGER_POWERPC)

BURGER_INLINE float Abs(float fInput) { return __fabsf(fInput); }
BURGER_INLINE double Abs(double dInput) { return __fabs(dInput); }
extern float BURGER_API Sqrt(float fInput);
extern double BURGER_API Sqrt(double dInput);

#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) { return static_cast<float>(fabs(fInput)); }
BURGER_INLINE double Abs(double dInput) { return fabs(dInput); }
BURGER_INLINE float Sqrt(float fInput) { return __fsqrts(fInput); }
BURGER_INLINE double Sqrt(double dInput) { return __fsqrt(dInput); }

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) { return __builtin_fabsf(fInput); }
BURGER_INLINE double Abs(double dInput) { return __builtin_fabs(dInput); }
BURGER_INLINE float Sqrt(float fInput) { return sqrtf(fInput); }
BURGER_INLINE double Sqrt(double dInput) { return sqrt(dInput); }

#elif (defined(BURGER_PS4) || defined(BURGER_VITA)) && !defined(DOXYGEN)

BURGER_INLINE float Abs(float fInput) { return __builtin_fabsf(fInput); }
BURGER_INLINE double Abs(double dInput) { return __builtin_fabs(dInput); }
BURGER_INLINE float Sqrt(float fInput) { return __builtin_sqrtf(fInput); }
BURGER_INLINE double Sqrt(double dInput) { return __builtin_sqrt(dInput); }

#elif defined(BURGER_MSVC) && (defined(BURGER_INTELARCHITECTURE) || defined(BURGER_ARM))

#if defined(BURGER_ARM)
BURGER_INLINE float Abs(float fInput) { return fabsf(fInput); }
#else
BURGER_INLINE float Abs(float fInput) { return static_cast<float>(fabs(fInput)); }
#endif

BURGER_INLINE double Abs(double dInput) { return fabs(dInput); }

#if defined(BURGER_AMD64) || defined(BURGER_ARM)
BURGER_INLINE float Sqrt(float fInput) { return sqrtf(fInput); }
#else
BURGER_INLINE float Sqrt(float fInput) { return static_cast<float>(sqrt(static_cast<double>(fInput))); }
#endif

BURGER_INLINE double Sqrt(double dInput) { return sqrt(dInput); }

#elif defined(BURGER_MACOSX) || defined(BURGER_IOS)

BURGER_INLINE float Abs(float fInput) { return __builtin_fabsf(fInput); }
BURGER_INLINE double Abs(double dInput) { return __builtin_fabs(dInput); }

#if defined(BURGER_POWERPC) && defined(BURGER_64BITCPU)
BURGER_INLINE float Sqrt(float fInput) { return __sqrt(fInput); }
BURGER_INLINE double Sqrt(double dInput) { return __sqrt(dInput); }
#else
BURGER_INLINE float Sqrt(float fInput) { return __builtin_sqrtf(fInput); }
BURGER_INLINE double Sqrt(double dInput) { return __builtin_sqrt(dInput); }
#endif

#else

extern float BURGER_API Abs(float fInput);
extern double BURGER_API Abs(double dInput);
extern float BURGER_API Sqrt(float fInput);
extern double BURGER_API Sqrt(double dInput);

#endif

BURGER_INLINE float IntToFloat(Int32 iInput) { return static_cast<float>(iInput); }
BURGER_INLINE float IntToFloat(const Int32 *pInput) { return static_cast<float>(pInput[0]); }
BURGER_INLINE void IntToFloat(float *pOutput,Int32 iInput) { pOutput[0] = static_cast<float>(iInput); }
BURGER_INLINE float FixedToFloat(Fixed32 fInput) { return static_cast<float>(fInput)*(1.0f/65536.0f); }
BURGER_INLINE float FixedToFloat(const Fixed32 *pInput) { return static_cast<float>(pInput[0])*(1.0f/65536.0f); }
BURGER_INLINE void FixedToFloat(float *pOutput,Fixed32 fInput) { pOutput[0] = static_cast<float>(fInput)*(1.0f/65536.0f); }

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)

BURGER_INLINE void IntToFloat(float *pOutput,const Int32 *pInput) {
	__vector4 vTemp = __lvlx(pInput,0);	// Load in VMX128
	vTemp = __vcfsx(vTemp,0);			// Convert
	vTemp = __vspltw(vTemp,0);			// Splat
	__stvewx(vTemp,pOutput,0);			// Store 32 bit
}		

BURGER_INLINE void FixedToFloat(float *pOutput,const Fixed32 *pInput) {
	__vector4 vTemp = __lvlx(pInput,0);	// Load in VMX128
	vTemp = __vcfsx(vTemp,16);		// Use 16.16 fixed point conversion
	vTemp = __vspltw(vTemp,0);		// Splat
	__stvewx(vTemp,pOutput,0);		// Store 32 bit
}

#elif defined(BURGER_PS3) && !defined(DOXYGEN)

BURGER_INLINE void IntToFloat(float *pOutput,const Int32 *pInput) {
	vector int vTemp = vec_lvlx(0,pInput);	// Load in VMX128
	vector float vfTemp = vec_vcfsx(vTemp,0);	// Convert to float
	vfTemp = vec_vspltw(vfTemp,0);			// Splat
	vec_stvewx(vfTemp,0,pOutput);			// Store 32 bit
}		

BURGER_INLINE void FixedToFloat(float *pOutput,const Fixed32 *pInput) {
	vector int vTemp = vec_lvlx(0,pInput);	// Load in VMX128
	vector float vfTemp = vec_vcfsx(vTemp,16);		// Use 16.16 fixed point conversion
	vfTemp = vec_vspltw(vfTemp,0);		// Splat
	vec_stvewx(vfTemp,0,pOutput);		// Store 32 bit
}	

#else
BURGER_INLINE void IntToFloat(float *pOutput,const Int32 *pInput) { pOutput[0] = static_cast<float>(pInput[0]); }
BURGER_INLINE void FixedToFloat(float *pOutput,const Fixed32 *pInput) { pOutput[0] = static_cast<float>(pInput[0])*(1.0f/65536.0f); }
#endif

BURGER_INLINE float Interpolate(float fFrom,float fTo,float fFactor) { return ((fTo - fFrom) * fFactor) + fFrom; }
BURGER_INLINE double Interpolate(double dFrom,double dTo,double dFactor) { return ((dTo - dFrom) * dFactor) + dFrom; }
BURGER_INLINE float Square(float fInput) { return fInput*fInput; }
BURGER_INLINE double Square(double dInput) { return dInput*dInput; }

#if defined(BURGER_POWERPC)
BURGER_INLINE float Sign(float fInput) { return static_cast<float>(__fsel(fInput,0.5f,-0.5f)+__fsel(-fInput,-0.5f,0.5f)); }
BURGER_INLINE double Sign(double dInput) { return __fsel(dInput,0.5,-0.5)+__fsel(-dInput,-0.5,0.5); }
#else
BURGER_INLINE float Sign(float fInput) { return ((fInput > 0.0f) ? 1.0f : ((fInput < 0.0f) ? -1.0f : 0.0f)); }
BURGER_INLINE double Sign(double dInput) { return ((dInput > 0.0) ? 1.0 : ((dInput < 0.0) ? -1.0 : 0.0)); }
#endif

#if defined(BURGER_POWERPC)
BURGER_INLINE float Min(float fA,float fB) { return static_cast<float>(__fsel((fA-fB),fB,fA)); }
BURGER_INLINE double Min(double dA,double dB) { return __fsel((dA-dB),dB,dA); }
BURGER_INLINE float Max(float fA,float fB) { return static_cast<float>(__fsel((fA-fB),fA,fB)); }
BURGER_INLINE double Max(double dA,double dB) { return __fsel((dA-dB),dA,dB); }

#elif (defined(BURGER_MSVC) && defined(BURGER_AMD64)) || ((defined(BURGER_MACOSX) || defined(BURGER_IOS)) && defined(BURGER_INTELARCHITECTURE))
BURGER_INLINE float Min(float fA,float fB) { return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(fA),_mm_set_ss(fB))); }
BURGER_INLINE double Min(double dA,double dB) { return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(dA),_mm_set_sd(dB))); }
BURGER_INLINE float Max(float fA,float fB) { return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(fA),_mm_set_ss(fB))); }
BURGER_INLINE double Max(double dA,double dB) { return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(dA),_mm_set_sd(dB))); }

#else
BURGER_INLINE float Min(float fA,float fB) { return ((fA < fB) ? fA : fB); }
BURGER_INLINE double Min(double dA,double dB) { return ((dA < dB) ? dA : dB); }
BURGER_INLINE float Max(float fA,float fB) { return ((fA > fB) ? fA : fB); }
BURGER_INLINE double Max(double dA,double dB) { return ((dA > dB) ? dA : dB); }
#endif

BURGER_INLINE float Clamp(float fIn,float fMin,float fMax) { return Min(Max(fIn,fMin),fMax); }
BURGER_INLINE double Clamp(double dIn,double dMin,double dMax) { return Min(Max(dIn,dMin),dMax); }

extern Word BURGER_API IsNan(float fInput);
extern Word BURGER_API IsNan(double dInput);
extern Word BURGER_API IsInf(float fInput);
extern Word BURGER_API IsInf(double dInput);
extern Word BURGER_API IsFinite(float fInput);
extern Word BURGER_API IsFinite(double dInput);
extern Word BURGER_API IsNormal(float fInput);
extern Word BURGER_API IsNormal(double dInput);
extern Word BURGER_API SignBit(float fInput);
extern Word BURGER_API SignBit(double dInput);
extern float BURGER_API Floor(float fInput);
extern double BURGER_API Floor(double dInput);
extern float BURGER_API Ceil(float fInput);
extern double BURGER_API Ceil(double dInput);
extern float BURGER_API Round(float fInput);
extern double BURGER_API Round(double dInput);
extern float BURGER_API RoundToZero(float fInput);
extern double BURGER_API RoundToZero(double dInput);
extern float BURGER_API ModuloRadians(float fInput);
extern double BURGER_API ModuloRadians(double dInput);
extern float BURGER_API Sin(float fInput);
extern double BURGER_API Sin(double dInput);
extern float BURGER_API Cos(float fInput);
extern double BURGER_API Cos(double dInput);
extern float BURGER_API Tan(float fInput);
extern double BURGER_API Tan(double dInput);
extern float BURGER_API ASin(float fInput);
extern double BURGER_API ASin(double dInput);
extern float BURGER_API ACos(float fInput);
extern double BURGER_API ACos(double dInput);
extern float BURGER_API ATan(float fInput);
extern double BURGER_API ATan(double dInput);
extern float BURGER_API ATan2(float fSin,float fCos);
extern double BURGER_API ATan2(double dSin,double dCos);
extern float BURGER_API Pow(float fX,float fY);
extern double BURGER_API Pow(double dX,double dY);
extern float BURGER_API Exp(float fInput);
extern double BURGER_API Exp(double dInput);
extern float BURGER_API Log(float fInput);
extern double BURGER_API Log(double dInput);
extern float BURGER_API Log2(float fInput);
extern double BURGER_API Log2(double dInput);
extern float BURGER_API Log10(float fInput);
extern double BURGER_API Log10(double dInput);
extern float BURGER_API Modf(float fInput,float *pInteger);
extern double BURGER_API Modf(double dInput,double *pInteger);
extern float BURGER_API Fmod(float fInput,float fDivisor);
extern double BURGER_API Fmod(double dInput,double dDivisor);
extern double BURGER_API LittleEndianLoadExtended(const Float80Bit pInput);
extern double BURGER_API BigEndianLoadExtended(const Float80Bit pInput);
extern long BURGER_API ConvertToDirectSoundVolume(Word uInput);
extern long BURGER_API ConvertToDirectSoundVolume(float fInput);
extern long BURGER_API ConvertToDirectSoundPan(Word uInput);
extern long BURGER_API ConvertToDirectSoundPan(float fInput);
extern float BURGER_API ConvertToAudioUnitVolume(Word uInput);
extern float BURGER_API ConvertToAudioUnitPan(Word uInput);
extern float BURGER_API ConvertToAudioUnitVolume(Word uInput);
extern float BURGER_API DecibelsToAmplitudeRatio(float fDecibels);
extern float BURGER_API AmplitudeRatioToDecibels(float fVolume);

}

/* END */

#endif
