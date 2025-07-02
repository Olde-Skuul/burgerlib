/***************************************

	Simple structures

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRUCTS_H__
#define __BRSTRUCTS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
/** Forward declaration for MacOS Point */
struct Point;

/** Forward declaration for MacOS Rect */
struct Rect;

#if defined(BURGER_MAC) || defined(DOXYGEN)
/** Forward declaration for MacOS Queue Header */
struct QHdr;

/** Forward declaration for MacOS Queue Entry */
struct QElem;

/** Forward declaration for MacOS Universal proc */
struct RoutineDescriptor;

/** Forward declaration for MacOS Multiprocessing ID */
struct OpaqueMPTaskID;

/** Forward declaration for MacOS Multiprocessing queue ID */
struct OpaqueMPQueueID;

/** Forward declaration for MacOS Multiprcessing event ID */
struct OpaqueMPEventID;
#endif

namespace Burger {

/** 80 bit float(Extended) */
typedef uint8_t float80_t[10];

union uint32_float_t {

	/** Value in binary */
	uint32_t w;

	/** Value as a 32 bit float */
	float f;

	BURGER_INLINE operator float() const BURGER_NOEXCEPT
	{
		return f;
	}

	BURGER_INLINE uint32_float_t& operator=(float fInput) BURGER_NOEXCEPT
	{
		f = fInput;
		return *this;
	}

	BURGER_INLINE float get_float(void) const BURGER_NOEXCEPT
	{
		return f;
	}

	BURGER_INLINE void set_float(float fInput) BURGER_NOEXCEPT
	{
		f = fInput;
	}

	BURGER_INLINE uint32_t get_uint32(void) const BURGER_NOEXCEPT
	{
		return w;
	}

	BURGER_INLINE void set_uint32(uint32_t uInput) BURGER_NOEXCEPT
	{
		w = uInput;
	}
};

union uint64_double_t {

	/** Value in binary */
	uint64_t w;

	/** Value as a 64 bit float */
	double d;

	/** Array of 2 32 bit words that map on the 64 bit value */
	uint32_t w32[2];

	BURGER_INLINE operator double() const BURGER_NOEXCEPT
	{
		return d;
	}

	BURGER_INLINE uint64_double_t& operator=(double dInput) BURGER_NOEXCEPT
	{
		d = dInput;
		return *this;
	}

	BURGER_INLINE double get_double(void) const BURGER_NOEXCEPT
	{
		return d;
	}

	BURGER_INLINE void set_double(double dInput) BURGER_NOEXCEPT
	{
		d = dInput;
	}

	BURGER_INLINE uint64_t get_uint64(void) const BURGER_NOEXCEPT
	{
		return w;
	}

	BURGER_INLINE void set_uint64(uint64_t uInput) BURGER_NOEXCEPT
	{
		w = uInput;
	}

	BURGER_INLINE uint32_t get_low_uint32(void) const BURGER_NOEXCEPT
	{
		return w32[BURGER_ENDIANINDEX_LOW];
	}

	BURGER_INLINE uint32_t get_high_uint32(void) const BURGER_NOEXCEPT
	{
		return w32[BURGER_ENDIANINDEX_HIGH];
	}

	BURGER_INLINE void set_low_uint32(uint32_t uInput) BURGER_NOEXCEPT
	{
		w32[BURGER_ENDIANINDEX_LOW] = uInput;
	}

	BURGER_INLINE void set_high_uint32(uint32_t uInput) BURGER_NOEXCEPT
	{
		w32[BURGER_ENDIANINDEX_HIGH] = uInput;
	}
};

// 32 bit floating point constants

/** Constant for 0.0f in the float format. */
extern const uint32_float_t g_fZero;

/** Constant for -0.0f in the float format. */
extern const uint32_float_t g_fNegZero;

/** Constant for 1.0f in the float format. */
extern const uint32_float_t g_fOne;

/** Constant for -1.0f in the float format. */
extern const uint32_float_t g_fNegOne;

/** Constant for 0.5f in the float format. */
extern const uint32_float_t g_fHalf;

/** Constant for -0.5f in the float format. */
extern const uint32_float_t g_fNegHalf;

/** Constant for Infinity in the float format. */
extern const uint32_float_t g_fInf;

/** Constant for -Infinity in the float format. */
extern const uint32_float_t g_fNegInf;

/** Constant for signaling Not a Number (NaN) in the float format. */
extern const uint32_float_t g_fNan;

/** Constant for negative signaling Not a Number (NaN) in the float format. */
extern const uint32_float_t g_fNegNan;

/** Constant for non signaling Not a Number (Nan) in the float format. */
extern const uint32_float_t g_fQNan;

/** Constant for negative non signaling Not a Number in the float format. */
extern const uint32_float_t g_fNegQNan;

/** Constant for smallest number (Non-zero) in the float format. */
extern const uint32_float_t g_fMin;

/** Constant for negative smallest number (Non-zero) in the float format. */
extern const uint32_float_t g_fNegMin;

/** Constant for largest number in the float format. */
extern const uint32_float_t g_fMax;

/** Constant for negative largest number in the float format. */
extern const uint32_float_t g_fNegMax;

/** Constant for the smallest number that cannot have a fraction 8388608.0f */
extern const uint32_float_t g_fMinNoInteger;

/** Constant for the negative smallest number that cannot have a fraction
 * -8388608.0f */
extern const uint32_float_t g_fNegMinNoInteger;

/** Constant for Epsilon in the float format. */
extern const uint32_float_t g_fEpsilon;

/** Constant for negative Epsilon in the float format. */
extern const uint32_float_t g_fNegEpsilon;

/** Constant for Pi*0.25f in the float format. */
extern const uint32_float_t g_fQuarterPi;

/** Constant for -Pi*0.25f in the float format. */
extern const uint32_float_t g_fNegQuarterPi;

/** Constant for Pi*0.5f in the float format. */
extern const uint32_float_t g_fHalfPi;

/** Constant for -Pi*0.5f in the float format. */
extern const uint32_float_t g_fNegHalfPi;

/** Constant for Pi in the float format. */
extern const uint32_float_t g_fPi;

/** Constant for -Pi in the float format. */
extern const uint32_float_t g_fNegPi;

/** Constant for Pi*2 in the float format. */
extern const uint32_float_t g_fPi2;

/** Constant for -Pi*2 in the float format. */
extern const uint32_float_t g_fNegPi2;

/** Constant for Pi*4 in the float format. */
extern const uint32_float_t g_fPi4;

/** Constant for -Pi*4 in the float format. */
extern const uint32_float_t g_fNegPi4;

/** Constant for get_cosine(Pi/6.0) in the float format */
extern const uint32_float_t g_fCosPiDiv6;

/** Constant for -get_cosine(Pi/6.0) in the float format */
extern const uint32_float_t g_fNegCosPiDiv6;

/** Constant for 1/(Pi*0.5f) in the float format. */
extern const uint32_float_t g_fReciprocalHalfPi;

/** Constant for -1/(Pi*0.5f) in the float format. */
extern const uint32_float_t g_fNegReciprocalHalfPi;

/** Constant for 1/Pi in the float format. */
extern const uint32_float_t g_fReciprocalPi;

/** Constant for -1/Pi in the float format. */
extern const uint32_float_t g_fNegReciprocalPi;

/** Constant for 1/(Pi*2.0f) in the float format. */
extern const uint32_float_t g_fReciprocalPi2;

/** Constant for -1/(Pi*2.0f) in the float format. */
extern const uint32_float_t g_fNegReciprocalPi2;

/** Constant for 180/Pi in the float format. */
extern const uint32_float_t g_fRadiansToDegrees;

/** Constant for -180/Pi in the float format. */
extern const uint32_float_t g_fNegRadiansToDegrees;

/** Constant for Pi/180 in the float format. */
extern const uint32_float_t g_fDegreesToRadians;

/** Constant for -Pi/180 in the float format. */
extern const uint32_float_t g_fNegDegreesToRadians;

/** Constant for sqrt(2) in the float format. */
extern const uint32_float_t g_fSqrt2;

/** Constant for -sqrt(2) in the float format. */
extern const uint32_float_t g_fNegSqrt2;

/** Constant for ln(2) in the float format. */
extern const uint32_float_t g_fLN2;

/** Constant for -ln(2) in the float format. */
extern const uint32_float_t g_fNegLN2;

/** Constant for ln(10) in the float format. */
extern const uint32_float_t g_fLN10;

/** Constant for -ln(10) in the float format. */
extern const uint32_float_t g_fNegLN10;

/** Constant for 1.0f/255.f in the float format. */
extern const uint32_float_t g_f1Div255;

/** Constant for 1.0f/3.f in the float format. */
extern const uint32_float_t g_f1Div3;

/** Constant for 2.0f/3.f in the float format. */
extern const uint32_float_t g_f2Div3;

/** Constant for 65536.f in the float float */
extern const uint32_float_t g_f65536;

// 64 bit floating point constants

/** Constant for 0.0 in the double format. */
extern const uint64_double_t g_dZero;

/** Constant for -0.0 in the double format. */
extern const uint64_double_t g_dNegZero;

/** Constant for 1.0 in the double format. */
extern const uint64_double_t g_dOne;

/** Constant for -1.0 in the double format. */
extern const uint64_double_t g_dNegOne;

/** Constant for 0.5 in the double format. */
extern const uint64_double_t g_dHalf;

/** Constant for -0.5 in the double format. */
extern const uint64_double_t g_dNegHalf;

/** Constant for Infinity in the double format. */
extern const uint64_double_t g_dInf;

/** Constant for -Infinity in the double format. */
extern const uint64_double_t g_dNegInf;

/** Constant for signaling Not a Number (NaN) in the double format. */
extern const uint64_double_t g_dNan;

/** Constant for negative signaling Not a Number (NaN) in the double format. */
extern const uint64_double_t g_dNegNan;

/** Constant for non signaling Not a Number (Nan) in the double format. */
extern const uint64_double_t g_dQNan;

/** Constant for negative non signaling Not a Number (Nan) in the double format.
 */
extern const uint64_double_t g_dNegQNan;

/** Constant for the smallest number (Non-zero) in the double format. */
extern const uint64_double_t g_dMin;

/** Constant for the negative smallest number (Non-zero) in the double format.
 */
extern const uint64_double_t g_dNegMin;

/** Constant for the largest number in the double format. */
extern const uint64_double_t g_dMax;

/** Constant for the negative largest number in the double format. */
extern const uint64_double_t g_dNegMax;

/** Constant for the smallest number that cannot have a fraction
 * 4503599627370496.0 */
extern const uint64_double_t g_dMinNoInteger;

/** Constant for the negative smallest number that cannot have a fraction
 * -4503599627370496.0 */
extern const uint64_double_t g_dNegMinNoInteger;

/** Constant for Epsilon in the double format. */
extern const uint64_double_t g_dEpsilon;

/** Constant for negative Epsilon in the double format. */
extern const uint64_double_t g_dNegEpsilon;

/** Constant for Pi*0.25 in the double format. */
extern const uint64_double_t g_dQuarterPi;

/** Constant for -Pi*0.25 in the double format. */
extern const uint64_double_t g_dNegQuarterPi;

/** Constant for Pi*0.5 in the double format. */
extern const uint64_double_t g_dHalfPi;

/** Constant for -Pi*0.5 in the double format. */
extern const uint64_double_t g_dNegHalfPi;

/** Constant for Pi in the double format. */
extern const uint64_double_t g_dPi;

/** Constant for -Pi in the double format. */
extern const uint64_double_t g_dNegPi;

/** Constant for Pi*2 in the double format. */
extern const uint64_double_t g_dPi2;

/** Constant for -Pi*2 in the double format. */
extern const uint64_double_t g_dNegPi2;

/** Constant for Pi*4 in the double format. */
extern const uint64_double_t g_dPi4;

/** Constant for -Pi*4 in the double format. */
extern const uint64_double_t g_dNegPi4;

/** Constant for get_cosine(Pi/6.0) in the double format */
extern const uint64_double_t g_dCosPiDiv6;

/** Constant for -get_cosine(Pi/6.0) in the double format */
extern const uint64_double_t g_dNegCosPiDiv6;

/** Constant for 1/(Pi*0.5) in the double format. */
extern const uint64_double_t g_dReciprocalHalfPi;

/** Constant for -1/(Pi*0.5) in the double format. */
extern const uint64_double_t g_dNegReciprocalHalfPi;

/** Constant for 1/Pi in the double format. */
extern const uint64_double_t g_dReciprocalPi;

/** Constant for -1/Pi in the double format. */
extern const uint64_double_t g_dNegReciprocalPi;

/** Constant for 1/(Pi*2) in the double format. */
extern const uint64_double_t g_dReciprocalPi2;

/** Constant for -1/(Pi*2) in the double format. */
extern const uint64_double_t g_dNegReciprocalPi2;

/** Constant for 180/Pi in the double format. */
extern const uint64_double_t g_dRadiansToDegrees;

/** Constant for -180/Pi in the double format. */
extern const uint64_double_t g_dNegRadiansToDegrees;

/** Constant for Pi/180 in the double format. */
extern const uint64_double_t g_dDegreesToRadians;

/** Constant for -Pi/180 in the double format. */
extern const uint64_double_t g_dNegDegreesToRadians;

/** Constant for sqrt(2) in the double format. */
extern const uint64_double_t g_dSqrt2;

/** Constant for -sqrt(2) in the double format. */
extern const uint64_double_t g_dNegSqrt2;

/** Constant for ln(2) in the double format. */
extern const uint64_double_t g_dLN2;

/** Constant for -ln(2) in the double format. */
extern const uint64_double_t g_dNegLN2;

/** Constant for ln(10) in the double format. */
extern const uint64_double_t g_dLN10;

/** Constant for -ln(10) in the double format. */
extern const uint64_double_t g_dNegLN10;

/** Constant for 1.0/255.0 in the double format. */
extern const uint64_double_t g_d1Div255;

/** Constant for 1.0/3.0 in the double format. */
extern const uint64_double_t g_d1Div3;

/** Constant for 2.0/3.0 in the double format. */
extern const uint64_double_t g_d2Div3;

/** Constant for 65536.0 in the double format. */
extern const uint64_double_t g_d65536;

#if defined(BURGER_X86) || defined(DOXYGEN)

/** 8087 control word to set rounding to zero */
extern const uint16_t g_u8087RoundToZero;

/** 8087 control word to set rounding to +infinity */
extern const uint16_t g_u8087RoundToInfinity;

/** 8087 control word to set rounding to -infinity */
extern const uint16_t g_u8087RoundToNegativeInfinity;

#endif

}
/* END */

#endif
