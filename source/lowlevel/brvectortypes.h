/***************************************

	Handle all the vector types for all CPUs and compilers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVECTORTYPES_H__
#define __BRVECTORTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
/***************************************

	Vector data types and vector intrinsics

***************************************/

#if defined(BURGER_PS2) || defined(BURGER_PSP)
typedef unsigned int Vector_128 __attribute__((mode(TI)));

#elif defined(BURGER_PS3)
#include <altivec.h>
#include <ppu_intrinsics.h>
#ifndef __VEC_TYPES_H__
#include <vec_types.h>
#endif
typedef vec_float4 Vector_128;

#elif defined(BURGER_PS4) || defined(BURGER_PS5)
#ifndef __XMMINTRIN_H
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_INTEL) && \
	(defined(BURGER_MSVC) || defined(BURGER_INTEL_COMPILER) || \
		defined(BURGER_GNUC) || defined(BURGER_CLANG))
#ifndef _INCLUDED_EMM
#include <emmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_XBOX360)
#ifndef __PPCINTRINSICS_H__
#include <ppcintrinsics.h>
#endif
#ifndef __VECTORINTRINSICS_H__
#include <vectorintrinsics.h>
#endif
typedef __vector4 Vector_128;

#elif defined(BURGER_METROWERKS) && defined(BURGER_X86)
#ifndef _XMMINTRIN_H
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_DARWIN) && defined(BURGER_INTEL)
#ifndef __EMMINTRIN_H
#include <emmintrin.h>
#endif
typedef __m128 Vector_128;

#elif defined(BURGER_ANDROID) || defined(BURGER_SWITCH)
#ifndef __ARM_FP16_H
#include <arm_fp16.h>
#endif

#ifndef __ARM_NEON_H
#include <arm_neon.h>
#endif
typedef float32x4_t Vector_128;

#elif defined(BURGER_WIIU)
#include <ppc_ghs.h>
#define __fsel __FSEL
#define __lwarx __LWARX
#define __stwcx __STWCX
struct Vector_128 {
	/** Opaque contents to the 128 bit vector register */
	BURGER_ALIGN(float, m128_f32[4], 16);
};

#elif defined(BURGER_MINGW)
#ifndef _XMMINTRIN_H_INCLUDED
#include <xmmintrin.h>
#endif
typedef __m128 Vector_128;

#else

#if defined(BURGER_WATCOM)
#ifndef _MMINTRIN_H_INCLUDED
#include <mmintrin.h>
#endif
#endif

struct Vector_128 {
	/** Opaque contents to the 128 bit vector register */
	BURGER_ALIGN(float, m128_f32[4], 16);
};
#endif

namespace Burger {
struct Vector_128Float32 {
	union {
		/** Value as 4 32 bit floats */
		BURGER_ALIGN(float, f[4], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Float64 {
	union {
		/** Value as 2 64 bit floats */
		BURGER_ALIGN(double, d[2], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Int32 {
	union {
		/** Value as 4 signed 32 bit integers */
		BURGER_ALIGN(int32_t, i[4], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Word64 {
	union {
		/** Value as 2 unsigned 64 bit integers */
		BURGER_ALIGN(uint64_t, u[2], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Word32 {
	union {
		/** Value as 4 unsigned 32 bit integers */
		BURGER_ALIGN(uint32_t, u[4], 16);
		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Word8 {
	union {
		/** Value as 16 unsigned 8 bit integers */
		BURGER_ALIGN(uint8_t, u[16], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};

struct Vector_128Word16 {
	union {
		/** Value as 8 unsigned 16 bit integers */
		BURGER_ALIGN(uint16_t, u[8], 16);

		/** Value as a 128 bit vector */
		Vector_128 v;
	};

	BURGER_INLINE operator Vector_128() const BURGER_NOEXCEPT
	{
		return v;
	}
};
}

/* END */

#endif
