/***************************************

	Handle endian swapping

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRENDIAN_H__
#define __BRENDIAN_H__

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

/* BEGIN */

// Undocumented feature of the Metrowerks compiler.
// Turn on to alert the optimizer that results are static

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm off
#endif

/***************************************

	Swap the endian

	While there are intrinsics for byte swapping, have
	actually seen the code output?

	No?

	Don't. You'll wail in terror.

***************************************/

/***************************************

	uint16_t _swapendian16(uint16_t uInput)

***************************************/

extern "C" {

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint16_t _swapendian16(uint16_t uInput);
#pragma aux _swapendian16 = \
	"ror ax,8" parm[ax] value[ax] modify exact[ax] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

// rorw #8,d0
#pragma parameter __D0 _swapendian16(__D0)
uint16_t _swapendian16(uint16_t) = {0xE058};

#elif (defined(BURGER_METROWERKS) && \
	(defined(BURGER_68K) || defined(BURGER_X86))) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return __ror(uInput, 8);
}

#elif (__has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap16(uInput);
}

#elif ((defined(BURGER_METROWERKS) || defined(BURGER_MRC) || \
		   defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
	defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	register int iResult = __rlwinm(static_cast<int>(uInput), 24, 24, 31);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 16, 23);
	return static_cast<uint16_t>(iResult);
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_68K)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__("rorw $8,%0" : : "=d"(uInput) : "0"(uInput) : "cc");
	return uInput;
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__(
		"rlwinm %0,%1,8,16,23\n"
		"rlwimi %0,%1,24,24,31"
		: "=&r"(uInput)
		: "r"(uInput));
	return uInput;
}

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint16_t>((__builtin_rev(uInput) >> 16));
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM32)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	uint16_t uResult;
	__asm__("rev16 %0,%1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}
#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	uint16_t uResult;
	__asm__("rev16 %w0,%w1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_INTEL)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(register uint16_t uInput) BURGER_NOEXCEPT
{
	__asm__("rorw $8,%0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

// _bswap instead of _byteswap_ulong, because... reasons
BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint16_t>(_bswap(uInput) >> 16);
}

#elif defined(BURGER_MSVC) && !defined(DOXYGEN)

BURGER_INLINE uint16_t _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return _byteswap_ushort(uInput);
}

#else
extern uint16_t BURGER_API _swapendian16(uint16_t uInput) BURGER_NOEXCEPT;
#endif

/***************************************

	uint32_t _swapendian32(uint32_t uInput)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint32_t _swapendian32(uint32_t uInput);
#pragma aux _swapendian32 = \
	"bswap eax" parm[eax] value[eax] modify exact[eax] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

// rorw #8,d0, swap d0, rorw #8,d0
#pragma parameter __D0 _swapendian32(__D0)
uint32_t _swapendian32(uint32_t) = {0xE058, 0x4840, 0xE058};

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

// clang-format off
BURGER_INLINE uint32_t _swapendian32(uint32_t : __D0) :
	__D0 = {
	0xE058,	// rorw #8,d0
	0x4840,	// swap d0
	0xE058	// rorw #8,d0
};
// clang-format on

#elif (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(register uint32_t uInput) BURGER_NOEXCEPT
{
	__asm bswap uInput;
	return uInput;
}

#elif (__has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300) || \
	defined(BURGER_IOS) || defined(BURGER_ARM_COMPILER)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap32(uInput);
}

#elif ((defined(BURGER_METROWERKS) || defined(BURGER_MRC) || \
		   defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS)) && \
	defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput) BURGER_NOEXCEPT
{
	register int iResult = __rlwinm(static_cast<int>(uInput), 24, 0, 31);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 8, 15);
	iResult = __rlwimi(iResult, static_cast<int>(uInput), 8, 24, 31);
	return static_cast<uint32_t>(iResult);
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_68K)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__(
		"rolw #8,%0\n"
		"swap %0\n"
		"rolw #8,%0"
		: "=d"(uInput)
		: "0"(uInput)
		: "cc");
	return uInput;
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__(
		"rlwinm %0,%1,24,0,31\n"
		"rlwimi %0,%1,8,8,15\n"
		"rlwimi %0,%1,8,24,31"
		: "=&r"(uInput)
		: "r"(uInput));
	return uInput;
}

#elif (defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return __builtin_rev(uInput);
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM32)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	uint32_t uResult;
	__asm__("rev %0, %1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	uint32_t uResult;
	__asm__("rev %w0,%w1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_INTEL)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	__asm__("bswap %0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

// _bswap instead of _byteswap_ulong, because... reasons

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return _bswap(uInput);
}

#elif defined(BURGER_MSVC) && !defined(DOXYGEN)

BURGER_INLINE uint32_t _swapendian32(uint32_t uInput)
{
	return _byteswap_ulong(uInput);
}

#else
extern uint32_t BURGER_API _swapendian32(uint32_t uInput) BURGER_NOEXCEPT;
#endif

/***************************************

	uint64_t _swapendian64(uint64_t uInput)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

extern uint64_t _swapendian64(uint64_t uInput);
#pragma aux _swapendian64 = \
	"bswap eax" \
	"bswap edx" \
	"xchg eax,edx" parm[eax edx] value[eax edx] modify \
	exact[eax edx] nomemory;

#elif (defined(BURGER_APPLE_SC) && defined(BURGER_68K)) && !defined(DOXYGEN)

extern uint64_t _swapendian64(const uint64_t& rInput);

#elif (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(register uint64_t uInput)
{
	register uint32_t uLo = static_cast<uint32_t>(uInput);
	register uint32_t uHi = static_cast<uint32_t>(uInput >> 32);
	__asm {
		mov eax, uHi
		mov edx, uLo
		bswap eax
		bswap edx
	}
}

#elif (__has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300) || \
	defined(BURGER_ARM_COMPILER)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return __builtin_bswap64(uInput);
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM32)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	union {
		struct {
			uint32_t l, h;
		} word32;
		uint64_t uWord64;
	} uTemp;

	// Convert to 32 bit registers
	uTemp.uWord64 = uInput;
	uint32_t l = uTemp.word32.l;
	uint32_t h = uTemp.word32.h;

	// Endian swap
#if defined(BURGER_IOS)
	l = __builtin_bswap32(l);
	h = __builtin_bswap32(h);
#else
	__asm__("rev %0,%1" : "=r"(l) : "r"(l));
	__asm__("rev %0,%1" : "=r"(h) : "r"(h));
#endif
	// Restore as 64 bit value
	uTemp.word32.l = h;
	uTemp.word32.h = l;
	return uTemp.uWord64;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_ARM64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	uint64_t uResult;
	__asm__("rev %x0,%x1" : "=r"(uResult) : "r"(uInput));
	return uResult;
}

#elif ( \
	(defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_X86)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	union {
		struct {
			uint32_t l, h;
		} word32;
		uint64_t uWord64;
	} uTemp;

	// Convert to 32 bit registers
	uTemp.uWord64 = uInput;
	uint32_t l = uTemp.word32.l;
	uint32_t h = uTemp.word32.h;

	// Endian swap
	__asm__("bswap %0" : "+r"(l));
	__asm__("bswap %0" : "+r"(h));

	// Restore as 64 bit value
	uTemp.word32.l = h;
	uTemp.word32.h = l;
	return uTemp.uWord64;
}

#elif ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_AMD64)) && \
	!defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	__asm__("bswap %0" : "+r"(uInput));
	return uInput;
}

#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _bswap64(uInput);
}

// Note: This function will fail on Visual Studio 2003, so test for 2005 or
// higher.
#elif (BURGER_MSVC >= 140000000) && !defined(DOXYGEN)

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _byteswap_uint64(uInput);
}

#elif (defined(BURGER_MSVC) && defined(BURGER_X86)) && !defined(DOXYGEN)

// Visual Studio 2003 has a bug that will cause bad code generation
// if using the _byteswap_uint64(). The code below is a workaround

BURGER_INLINE uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _swapendian32(static_cast<uint32_t>(uInput >> 32)) |
		(static_cast<uint64_t>(_swapendian32(static_cast<uint32_t>(uInput)))
			<< 32);
}

#else
extern uint64_t BURGER_API _swapendian64(uint64_t uInput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) || defined(DOXYGEN)
extern void _swapendianstore16(void* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
extern void _swapendianstore32(void* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
extern void _swapendianstore64(void* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif

#if ((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC) && \
	defined(BURGER_MACOSX)) || \
	defined(DOXYGEN)
extern uint64_t _swapendian64ptr(const void* pInput) BURGER_NOEXCEPT;
extern void _swapendianstore64(void* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif
}

namespace Burger {
extern float BURGER_API _swapendianfloat(float fInput) BURGER_NOEXCEPT;
extern double BURGER_API _swapendiandouble(double dInput) BURGER_NOEXCEPT;
extern float BURGER_API _swapendianfloat(
	const float* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern double BURGER_API _swapendiandouble(
	const double* pInput BURGER_68K_A0) BURGER_NOEXCEPT;

extern uint16_t BURGER_API _load_unaligned(
	const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern uint32_t BURGER_API _load_unaligned(
	const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern uint64_t BURGER_API _load_unaligned(
	const uint64_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern float BURGER_API _load_unaligned(const float* pInput) BURGER_NOEXCEPT;
extern double BURGER_API _load_unaligned(const double* pInput) BURGER_NOEXCEPT;

extern void BURGER_API _store_unaligned(uint16_t* pOutput BURGER_68K_A0,
	uint16_t uInput BURGER_68K_D0) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned(uint32_t* pOutput BURGER_68K_A0,
	uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned(
	uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned(
	float* pOutput, float fInput) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned(
	double* pOutput, double dInput) BURGER_NOEXCEPT;

extern uint16_t BURGER_API _load_unaligned_swap(
	const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern uint32_t BURGER_API _load_unaligned_swap(
	const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern uint64_t BURGER_API _load_unaligned_swap(
	const uint64_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT;
extern float BURGER_API _load_unaligned_swap(
	const float* pInput) BURGER_NOEXCEPT;
extern double BURGER_API _load_unaligned_swap(
	const double* pInput) BURGER_NOEXCEPT;

extern void BURGER_API _store_unaligned_swap(uint16_t* pOutput BURGER_68K_A0,
	uint16_t uInput BURGER_68K_D0) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned_swap(uint32_t* pOutput BURGER_68K_A0,
	uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned_swap(
	uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned_swap(
	float* pOutput, float fInput) BURGER_NOEXCEPT;
extern void BURGER_API _store_unaligned_swap(
	double* pOutput, double dInput) BURGER_NOEXCEPT;

/***************************************

	Don't swap endian

***************************************/

#if !defined(DOXYGEN)

// Namespace for native implementations
namespace Native {

// Catchall for NativeEndian::load()
template<typename T, bool is_arithmetic, bool is_pointer>
struct load_dispatch {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated on bad input
		BURGER_STATIC_ASSERT(Input * Input < 0);
		return Input;
	}
};

// Handlers for direct intrinsics (No pointers)
template<typename T>
struct load_dispatch<T, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for arithmetic integral types
		return Input;
	}
};

// Catchall for all pointer types
template<typename T>
struct load_dispatch<T, false, true> {
	BURGER_INLINE typename remove_pointer<T>::type operator()(
		T Input) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to arithmetic integral types
		BURGER_STATIC_ASSERT(
			is_arithmetic<typename remove_pointer<T>::type>::value);
		return *Input;
	}
};

// Catchall for NativeEndian::store()
template<typename T, bool is_arithmetic>
struct store_dispatch {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
		*pOutput = Input;
	}
};

// Store arithmetic values
template<typename T>
struct store_dispatch<T, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// Instantiated for arithmetic integral types
		*pOutput = Input;
	}
};

// Implemented on systems that don't support non-aligned reads and writes in
// hardware
#if !(defined(BURGER_INTEL) || defined(BURGER_ARM64))

// Catchall for NativeEndian::load_unaligned()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct load_unaligned_dispatch {
	BURGER_INLINE T operator()(T* pInput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pInput || !pInput);
		return *pInput;
	}
};

// Handlers for NativeEndian::load_unaligned()
template<typename T>
struct load_unaligned_dispatch<T, 1, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to single byte types
		return *pInput;
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 2, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned(reinterpret_cast<const uint16_t*>(pInput)));
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 4, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned(reinterpret_cast<const uint32_t*>(pInput)));
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 8, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned(reinterpret_cast<const uint64_t*>(pInput)));
	}
};

template<>
struct load_unaligned_dispatch<float, sizeof(float), true> {
	BURGER_INLINE float operator()(const float* pInput) BURGER_NOEXCEPT
	{
		// Return the float
		return _load_unaligned(pInput);
	}
};

template<>
struct load_unaligned_dispatch<double, sizeof(double), true> {
	BURGER_INLINE double operator()(const double* pInput) BURGER_NOEXCEPT
	{
		// Return the double
		return _load_unaligned(pInput);
	}
};

// Catchall for NativeEndian::store_unaligned()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct store_unaligned_dispatch {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
		return *pOutput = Input;
	}
};

// Store unaligned dispatchers
template<typename T>
struct store_unaligned_dispatch<T, 1, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// Byte doesn't care
		*pOutput = Input;
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 2, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned(
			reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(Input));
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 4, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned(
			reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(Input));
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 8, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned(
			reinterpret_cast<uint64_t*>(pOutput), static_cast<uint64_t>(Input));
	}
};

template<>
struct store_unaligned_dispatch<float, sizeof(float), true> {
	BURGER_INLINE void operator()(float* pOutput, float fInput) BURGER_NOEXCEPT
	{
		// Return the float
		_store_unaligned(pOutput, fInput);
	}
};

template<>
struct store_unaligned_dispatch<double, sizeof(double), true> {
	BURGER_INLINE void operator()(
		double* pOutput, double dInput) BURGER_NOEXCEPT
	{
		// Return the double
		_store_unaligned(pOutput, dInput);
	}
};
#endif
}
#endif

struct NativeEndian {

	template<typename T>
	static BURGER_INLINE BURGER_CONSTEXPR typename remove_pointer<T>::type load(
		T Input) BURGER_NOEXCEPT
	{
		return Native::load_dispatch<T, is_arithmetic<T>::value,
			is_pointer<T>::value>()(Input);
	}

	template<typename T>
	static BURGER_INLINE void store(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		Native::store_dispatch<T, is_arithmetic<T>::value>()(pOutput, Input);
	}

	template<typename T>
	static BURGER_INLINE void store(
		volatile T* pOutput, T Input) BURGER_NOEXCEPT
	{
		store(const_cast<T*>(pOutput), Input);
	}

	template<typename T>
	static BURGER_INLINE T load_unaligned(T* pInput) BURGER_NOEXCEPT
	{
		// These CPUs allow non-aligned reads and writes
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64))
		return Native::load_dispatch<T*, is_arithmetic<T*>::value, true>()(
			pInput);
#else
		typedef typename remove_cv<T>::type real_type;
		return Native::load_unaligned_dispatch<real_type, sizeof(real_type),
			is_arithmetic<real_type>::value>()(const_cast<real_type*>(pInput));
#endif
	}

	template<typename T>
	static BURGER_INLINE void store_unaligned(
		T* pOutput, T Input) BURGER_NOEXCEPT
	{
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64))
		Native::store_dispatch<T, is_arithmetic<T>::value>()(pOutput, Input);
#else
		Native::store_unaligned_dispatch<T, sizeof(T),
			is_arithmetic<T>::value>()(pOutput, Input);
#endif
	}

	template<typename T>
	static BURGER_INLINE void fixup(T*) BURGER_NOEXCEPT
	{
	}

	template<typename T>
	static BURGER_INLINE void fixup_unaligned(T*) BURGER_NOEXCEPT
	{
	}
};

/***************************************

	Swap endian

***************************************/

#if !defined(DOXYGEN)
// Internal templates
namespace Swap {

// Catchall for SwapEndian::load()
template<typename T, uintptr_t size, bool is_arithmetic, bool is_pointer>
struct load_dispatch {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated on bad input
		BURGER_STATIC_ASSERT(Input * Input < 0);
		return Input;
	}
};

// Handlers for direct intrinsics (No pointers)

// Instantiated for single byte types
template<typename T>
struct load_dispatch<T, 1, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		return Input;
	}
};

// Instantiated for two byte types
template<typename T>
struct load_dispatch<T, 2, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{

		return static_cast<T>(_swapendian16(static_cast<uint16_t>(Input)));
	}
};

// Instantiated for four byte types
template<typename T>
struct load_dispatch<T, 4, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{

		return static_cast<T>(_swapendian32(static_cast<uint32_t>(Input)));
	}
};

// Instantiated for eight byte types
template<typename T>
struct load_dispatch<T, 8, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		return static_cast<T>(_swapendian64(static_cast<uint64_t>(Input)));
	}
};

// Instantiated for four byte float
template<>
struct load_dispatch<float, sizeof(float), true, false> {
	BURGER_INLINE float operator()(float Input) BURGER_NOEXCEPT
	{
#if defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code for Playstation 4/5
		uint32_t uTemp;
		__asm__(
			"vmovd %1,%0\n"
			"bswap %0\n"
			"vmovd %0,%1"
			: "=r"(uTemp), "+x"(Input));
		return Input;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		// SSE2 code
		uint32_t uTemp;
		__asm__(
			"movd %1,%0\n"
			"bswap %0\n"
			"movd %0,%1"
			: "=r"(uTemp), "+x"(Input));
		return Input;
#elif defined(BURGER_XBOXONE) || (defined(BURGER_MSVC) && defined(BURGER_SSE2))
		// Cast float to __m128i, __m128i to int, swap endian, convert to
		// __m128i, then cast to __m128 and then to float.

		// Please shoot me.
		return _mm_cvtss_f32(_mm_castsi128_ps(_mm_cvtsi32_si128(_byteswap_ulong(
			_mm_cvtsi128_si32(_mm_castps_si128(_mm_set_ss(Input)))))));
#else
		return _swapendianfloat(Input);
#endif
	}
};

// Instantiated for eight byte float
template<>
struct load_dispatch<double, sizeof(double), true, false> {
	BURGER_INLINE double operator()(double Input) BURGER_NOEXCEPT
	{
		// Instantiated for double
#if defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code
		uint64_t uTemp;
		__asm__(
			"vmovq %1,%0\n"
			"bswap %0\n"
			"vmovq %0,%1"
			: "=r"(uTemp), "+x"(Input));
		return Input;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		// SSE2 code
		uint64_t uTemp;
		__asm__(
			"movd %1,%0\n"
			"bswap %0\n"
			"movd %0,%1"
			: "=r"(uTemp), "+x"(Input));
		return Input;

#elif (defined(BURGER_XBOXONE) || \
	(defined(BURGER_MSVC) && defined(BURGER_AMD64)))

		// Cast double to __m128i, __m128i to int, swap endian, convert to
		// __m128i, then cast to __m128 and then to double.

		// Please shoot me again.
		return _mm_cvtsd_f64(
			_mm_castsi128_pd(_mm_cvtsi64_si128(_byteswap_uint64(
				_mm_cvtsi128_si64(_mm_castpd_si128(_mm_set_sd(Input)))))));

#else
		return _swapendiandouble(Input);
#endif
	}
};

// Catchall for all pointer types
template<typename T, uintptr_t size, bool bIsArithmetic>
struct load_dispatch_ptr {
	BURGER_INLINE T operator()(T* pInput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pInput || !pInput);
		return *pInput;
	}
};

// Instantiated for pointers to single byte types
template<typename T>
struct load_dispatch_ptr<T, 1, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(is_arithmetic<T>::value);
		return *pInput;
	}
};

// Instantiated for pointers to two byte types
template<typename T>
struct load_dispatch_ptr<T, 2, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(is_arithmetic<T>::value);

		// PowerPC has an instruction for this
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		return static_cast<T>(__lhbrx(const_cast<T*>(pInput), 0));
#elif defined(BURGER_XBOX360)
		return static_cast<T>(__loadshortbytereverse(0, pInput));
#elif defined(BURGER_PS3)
		return static_cast<T>(__lhbrx(const_cast<T*>(pInput)));
#elif defined(BURGER_WIIU)
		return static_cast<T>(*const_cast<__bytereversed T*>(pInput));
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		T uTemp;
		__asm__ __volatile__("lhbrx %0,0,%1"
							 : "=r"(uTemp)
							 : "r"(pInput), "m"(*pInput));
		return uTemp;
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		T uTemp;
		__asm__("movbew %1,%0" : "=r"(uTemp) : "m"(*pInput));
		return uTemp;
#elif defined(BURGER_XBOXONE)
		return static_cast<T>(_load_be_u16(const_cast<T*>(pInput)));
#else
		return static_cast<T>(_swapendian16(static_cast<uint16_t>(*pInput)));
#endif
	}
};

// Instantiated for pointers to four byte types
template<typename T>
struct load_dispatch_ptr<T, 4, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(is_arithmetic<T>::value);

		// PowerPC has an instruction for this
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		return static_cast<T>(__lwbrx(const_cast<T*>(pInput), 0));
#elif defined(BURGER_XBOX360)
		return static_cast<T>(__loadwordbytereverse(0, pInput));
#elif defined(BURGER_PS3)
		return static_cast<T>(__lwbrx(const_cast<T*>(pInput)));
#elif defined(BURGER_WIIU)
		return static_cast<T>(*const_cast<__bytereversed T*>(pInput));
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		T uTemp;
		__asm__ __volatile__("lwbrx %0,0,%1"
							 : "=r"(uTemp)
							 : "r"(pInput), "m"(*pInput));
		return uTemp;
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		T uTemp;
		__asm__ __volatile__("movbel %1,%0" : "=r"(uTemp) : "m"(*pInput));
		return uTemp;
#elif defined(BURGER_XBOXONE)
		return static_cast<T>(_load_be_u32(const_cast<T*>(pInput)));
#else
		return static_cast<T>(_swapendian32(static_cast<uint32_t>(*pInput)));
#endif
	}
};

// Instantiated for pointers to eight byte types
template<typename T>
struct load_dispatch_ptr<T, 8, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(is_arithmetic<T>::value);

		// PowerPC has an instruction for this
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_t uTempHi = __lwbrx(const_cast<T*>(pInput), 0);
		uint32_t uTempLo = __lwbrx(const_cast<T*>(pInput), 4);
		return static_cast<T>(
			(static_cast<uint64_t>(uTempLo) << 32U) | uTempHi);
#elif defined(BURGER_XBOX360)
		return static_cast<T>(__loaddoublewordbytereverse(0, pInput));
#elif defined(BURGER_PS3)
		return static_cast<T>(__ldbrx(const_cast<T*>(pInput)));
#elif defined(BURGER_WIIU)
		const __bytereversed uint32_t* pInput2 =
			reinterpret_cast<const __bytereversed uint32_t*>(pInput);
		uint32_t uTempHi = pInput2[0];
		uint32_t uTempLo = pInput2[1];
		return static_cast<T>(
			(static_cast<uint64_t>(uTempLo) << 32U) | uTempHi);

// PowerMac G5 doesn't have the ldbrx instruction
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_PPC) && defined(BURGER_MACOSX)
		return static_cast<T>(_swapendian64ptr(pInput));
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		T uTemp;
		__asm__ __volatile__("movbeq %1,%0" : "=r"(uTemp) : "m"(*pInput));
		return uTemp;
#elif defined(BURGER_XBOXONE)
		return static_cast<T>(_load_be_u64(const_cast<T*>(pInput)));
#else
		return static_cast<T>(_swapendian64(static_cast<uint64_t>(*pInput)));
#endif
	}
};

// Instantiated for pointers to four byte floats
template<>
struct load_dispatch_ptr<float, sizeof(float), true> {
	BURGER_INLINE float operator()(const float* pInput) BURGER_NOEXCEPT
	{
		// PowerPC has an instruction for this
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_float_t Temp;
		Temp.w = __lwbrx(const_cast<float*>(pInput), 0);
		return Temp.f;
#elif defined(BURGER_XBOX360)
		uint32_float_t Temp;
		Temp.w = __loadwordbytereverse(0, pInput);
		return Temp.f;
#elif defined(BURGER_PS3)
		uint32_float_t Temp;
		Temp.w = __lwbrx(const_cast<float*>(pInput));
		return Temp.f;
#elif defined(BURGER_WIIU)
		uint32_float_t Temp;
		Temp.w = *static_cast<const __bytereversed uint32_t*>(
			static_cast<const void*>(pInput));
		return Temp.f;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		uint32_float_t Temp;
		uint32_t uTemp;
		__asm__ __volatile__("lwbrx %0,0,%1"
							 : "=r"(uTemp)
							 : "r"(pInput), "m"(*pInput));
		Temp.w = uTemp;
		return Temp.f;
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code
		float fTemp;
		uint32_t uTemp;
		__asm__(
			"movbe %2,%0\n"
			"vmovd %0,%1"
			: "=r"(uTemp), "=x"(fTemp)
			: "m"(*pInput));
		return fTemp;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		// SSE2 code
		float fTemp;
		uint32_t uTemp;
		__asm__(
			"movl %2,%0\n"
			"bswap %0\n"
			"movd %0,%1"
			: "=r"(uTemp), "=x"(fTemp)
			: "m"(*pInput));
		return fTemp;
#elif defined(BURGER_XBOXONE)
		return _mm_cvtss_f32(_mm_castsi128_ps(
			_mm_cvtsi32_si128(_load_be_u32(const_cast<float*>(pInput)))));
#else
		// Return the float
		return _swapendianfloat(pInput);
#endif
	}
};

// Instantiated for pointers to eight byte floats
template<>
struct load_dispatch_ptr<double, sizeof(double), true> {
	BURGER_INLINE double operator()(const double* pInput) BURGER_NOEXCEPT
	{
		// PowerPC 64 has an instruction for this, but not G5
#if defined(BURGER_XBOX360)
		uint64_double_t Temp;
		Temp.w = __loaddoublewordbytereverse(0, pInput);
		return Temp.d;
#elif defined(BURGER_PS3)
		uint64_double_t Temp;
		Temp.w = __ldbrx(const_cast<double*>(pInput));
		return Temp.d;
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code
		double dTemp;
		uint64_t uTemp;
		__asm__(
			"movbe %2,%0\n"
			"vmovq %0,%1"
			: "=r"(uTemp), "=x"(dTemp)
			: "m"(*pInput));
		return dTemp;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		// SSE2 code
		double dTemp;
		uint64_t uTemp;
		__asm__(
			"movq %2,%0\n"
			"bswap %0\n"
			"movd %0,%1"
			: "=r"(uTemp), "=x"(dTemp)
			: "m"(*pInput));
		return dTemp;
#elif defined(BURGER_XBOXONE)
		return _mm_cvtsd_f64(_mm_castsi128_pd(
			_mm_cvtsi64_si128(_load_be_u64(const_cast<double*>(pInput)))));
#else
		return _swapendiandouble(pInput);
#endif
	}
};

// Forward to the pointer type handler
template<typename T>
struct load_dispatch<T, sizeof(void*), false, true> {
	BURGER_INLINE typename remove_pointer<T>::type operator()(
		T pInput) BURGER_NOEXCEPT
	{
		// Use the underlying data type for proper dispatching
		typedef typename remove_pointer<T>::type real_type;
		typedef typename remove_cv<real_type>::type raw_type;

		// Use the remove_cv to convert all pointers to const T * for
		// dispatching so the only overrides needed are const char *, const
		// float *, etc
		return load_dispatch_ptr<raw_type, sizeof(raw_type),
			is_arithmetic<raw_type>::value>()((const raw_type*)pInput);
	}
};

// Catchall for SwapEndian::store()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct store_dispatch {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
		return *pOutput = Input;
	}
};

// Store single byte integer
template<typename T>
struct store_dispatch<T, 1, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		*pOutput = Input;
	}
};

// Store two byte integer
template<typename T>
struct store_dispatch<T, 2, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
#if (defined(BURGER_METROWERKS) && (defined(BURGER_X86) || defined(BURGER_68K)))
		*pOutput = static_cast<T>(__ror(static_cast<uint16_t>(Input), 8));
#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
		// Use this to allow the compiler to know pOutput is volatile
		_swapendianstore16(pOutput, static_cast<uint16_t>(Input));
		// This intrisic allows pOutput to remain "const", so an immediate
		// read will use Input instead, which is bad code
		//__sthbrx(static_cast<uint16_t>(Input), pOutput, 0);
#elif defined(BURGER_XBOX360)
		__storeshortbytereverse(Input, 0, pOutput);
#elif defined(BURGER_PS3)
		__sthbrx(pOutput, Input);
#elif defined(BURGER_WIIU)
		*const_cast<__bytereversed T*>(pOutput) = Input;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		__asm__("sthbrx %0,0,%1" : : "r"(Input), "r"(pOutput) : "memory");
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		__asm__("movbew %0,%1" : : "r"(Input), "m"(*pOutput) : "memory");
#elif defined(BURGER_XBOXONE)
		_store_be_u16(pOutput, static_cast<uint16_t>(Input));
#else
		*pOutput = static_cast<T>(_swapendian16(static_cast<uint16_t>(Input)));
#endif
	}
};

template<typename T>
struct store_dispatch<T, 4, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
		// Use this to allow the compiler to know pOutput is volatile
		_swapendianstore32(pOutput, static_cast<uint32_t>(Input));
		// This intrisic allows pOutput to remain "const", so an immediate
		// read will use Input instead, which is bad code
		// __stwbrx(static_cast<unsigned int>(Input), pOutput, 0);
#elif defined(BURGER_XBOX360)
		__storewordbytereverse(Input, 0, pOutput);
#elif defined(BURGER_PS3)
		__stwbrx(pOutput, Input);
#elif defined(BURGER_WIIU)
		*const_cast<__bytereversed T*>(pOutput) = Input;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		__asm__("stwbrx %0,0,%1" : : "r"(Input), "r"(pOutput) : "memory");
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		__asm__("movbel %0,%1" : : "r"(Input), "m"(*pOutput) : "memory");
#elif defined(BURGER_XBOXONE)
		_store_be_u32(pOutput, static_cast<uint32_t>(Input));
#else
		*pOutput = static_cast<T>(_swapendian32(static_cast<uint32_t>(Input)));
#endif
	}
};

template<typename T>
struct store_dispatch<T, 8, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		_swapendianstore64(pOutput, static_cast<uint64_t>(Input));
#elif defined(BURGER_XBOX360)
		__storedoublewordbytereverse(Input, 0, pOutput);
#elif defined(BURGER_PS3)
		__stdbrx(pOutput, Input);
#elif defined(BURGER_WIIU)
		__bytereversed uint32_t* pOutput2 =
			reinterpret_cast<__bytereversed uint32_t*>(pOutput);
		pOutput2[0] = static_cast<uint32_t>(Input);
		pOutput2[1] = static_cast<uint32_t>(Input >> 32U);

// PowerMac G5 doesn't have this stdbrx
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_PPC) && defined(BURGER_MACOSX)
		_swapendianstore64(pOutput, Input);
#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		__asm__("movbeq %0,%1" : : "r"(Input), "m"(*pOutput) : "memory");
#elif defined(BURGER_XBOXONE)
		_store_be_u64(pOutput, static_cast<uint64_t>(Input));
#else
		*pOutput = static_cast<T>(_swapendian64(static_cast<uint64_t>(Input)));
#endif
	}
};

template<>
struct store_dispatch<float, sizeof(float), true> {
	BURGER_INLINE void operator()(float* pOutput, float fInput) BURGER_NOEXCEPT
	{
#if defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code for Playstation 4/5
		// Note, split to two operations, since the compiler barfs
		// if combined into one
		uint32_t uTemp;
		__asm__("vmovd %1,%0\n" : "=r"(uTemp) : "x"(fInput));
		__asm__("movbel %0,%1\n" : : "r"(uTemp), "m"(*pOutput) : "memory");
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		uint32_t uTemp;
		__asm__(
			"movd %1,%0\n"
			"bswapl %0\n"
			: "=r"(uTemp)
			: "x"(fInput));
		__asm__("movl %0,%1\n" : : "r"(uTemp), "m"(*pOutput) : "memory");
#elif defined(BURGER_XBOXONE) && 0
		// This generates an internal compiler error
		_store_be_u32(
			pOutput, _mm_cvtsi128_si32(_mm_castps_si128(_mm_set_ss(fInput))));
#elif (defined(BURGER_MSVC) && defined(BURGER_SSE2))
		*reinterpret_cast<uint32_t*>(pOutput) = _byteswap_ulong(
			_mm_cvtsi128_si32(_mm_castps_si128(_mm_set_ss(fInput))));
#else
		// Swap the float without losing bits
		uint32_float_t Temp;
		Temp.f = fInput;
		*static_cast<uint32_t*>(static_cast<void*>(pOutput)) =
			_swapendian32(Temp.w);
#endif
	}
};

template<>
struct store_dispatch<double, sizeof(double), true> {
	BURGER_INLINE void operator()(
		double* pOutput, double dInput) BURGER_NOEXCEPT
	{
#if defined(BURGER_PS4) || defined(BURGER_PS5)
		// VMX code for Playstation 4/5
		// Note, split to two operations, since the compiler barfs
		// if combined into one
		uint64_t uTemp;
		__asm__("vmovd %1,%0\n" : "=r"(uTemp) : "x"(dInput));
		__asm__("movbeq %0,%1\n" : : "r"(uTemp), "m"(*pOutput) : "memory");
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_AMD64)
		uint64_t uTemp;
		__asm__(
			"movd %1,%0\n"
			"bswapq %0\n"
			: "=r"(uTemp)
			: "x"(dInput));
		__asm__("movq %0,%1\n" : : "r"(uTemp), "m"(*pOutput) : "memory");
#elif defined(BURGER_XBOXONE) && 0
		// This generates an internal compiler error
		_store_be_u64(
			pOutput, _mm_cvtsi128_si64(_mm_castpd_si128(_mm_set_sd(dInput))));
#elif (defined(BURGER_MSVC) && defined(BURGER_AMD64))
		*reinterpret_cast<uint64_t*>(pOutput) = _byteswap_uint64(
			_mm_cvtsi128_si64(_mm_castpd_si128(_mm_set_sd(dInput))));
#else
		// Swap the double without losing bits
		uint64_double_t Temp;
		Temp.d = dInput;
		*static_cast<uint64_t*>(static_cast<void*>(pOutput)) =
			_swapendian64(Temp.w);
#endif
	}
};

// Implemented on systems that don't support non-aligned reads and writes in
// hardware
#if !(defined(BURGER_INTEL) || defined(BURGER_ARM64))

// Catchall for SwapEndian::load_unaligned()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct load_unaligned_dispatch {
	BURGER_INLINE T operator()(T* pInput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pInput || !pInput);
		return *pInput;
	}
};

// Handlers for NativeEndian::load_unaligned()
template<typename T>
struct load_unaligned_dispatch<T, 1, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to single byte types
		return *pInput;
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 2, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned_swap(reinterpret_cast<const uint16_t*>(pInput)));
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 4, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned_swap(reinterpret_cast<const uint32_t*>(pInput)));
	}
};

template<typename T>
struct load_unaligned_dispatch<T, 8, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		return static_cast<T>(
			_load_unaligned_swap(reinterpret_cast<const uint64_t*>(pInput)));
	}
};

template<>
struct load_unaligned_dispatch<float, sizeof(float), true> {
	BURGER_INLINE float operator()(const float* pInput) BURGER_NOEXCEPT
	{
		// Return the float
		return _load_unaligned_swap(pInput);
	}
};

template<>
struct load_unaligned_dispatch<double, sizeof(double), true> {
	BURGER_INLINE double operator()(const double* pInput) BURGER_NOEXCEPT
	{
		// Return the double
		return _load_unaligned_swap(pInput);
	}
};

// Catchall for NativeEndian::store_unaligned()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct store_unaligned_dispatch {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
		return *pOutput = Input;
	}
};

// Store unaligned dispatchers
template<typename T>
struct store_unaligned_dispatch<T, 1, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		// Byte doesn't care
		*pOutput = Input;
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 2, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned_swap(
			reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(Input));
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 4, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned_swap(
			reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(Input));
	}
};

template<typename T>
struct store_unaligned_dispatch<T, 8, true> {
	BURGER_INLINE void operator()(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		_store_unaligned_swap(
			reinterpret_cast<uint64_t*>(pOutput), static_cast<uint64_t>(Input));
	}
};

template<>
struct store_unaligned_dispatch<float, sizeof(float), true> {
	BURGER_INLINE void operator()(float* pOutput, float fInput) BURGER_NOEXCEPT
	{
		// Return the float
		_store_unaligned_swap(pOutput, fInput);
	}
};

template<>
struct store_unaligned_dispatch<double, sizeof(double), true> {
	BURGER_INLINE void operator()(
		double* pOutput, double dInput) BURGER_NOEXCEPT
	{
		// Return the double
		_store_unaligned_swap(pOutput, dInput);
	}
};
#endif

// Catchall for SwapEndian::fixup()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct fixup_dispatch {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
	}
};

// Fixup single byte integer (No swapping)
template<typename T>
struct fixup_dispatch<T, 1, true> {
	BURGER_INLINE void operator()(T* /* pOutput */) BURGER_NOEXCEPT {}
};

// Fixup two byte integer
template<typename T>
struct fixup_dispatch<T, 2, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint16_t uTemp = __lhbrx(pOutput, 0);
		reinterpret_cast<uint16_t*>(pOutput)[0] = uTemp;
#else
		*pOutput =
			Swap::load_dispatch<T*, sizeof(void*), false, true>()(pOutput);
#endif
	}
};

// Fixup four byte integer
template<typename T>
struct fixup_dispatch<T, 4, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_t uTemp = __lwbrx(pOutput, 0);
		reinterpret_cast<uint32_t*>(pOutput)[0] = uTemp;
#else
		*pOutput =
			Swap::load_dispatch<T*, sizeof(void*), false, true>()(pOutput);
#endif
	}
};

// Fixup eight byte integer
template<typename T>
struct fixup_dispatch<T, 8, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_t uLow = __lwbrx(pOutput, 0);
		uint32_t uHigh = __lwbrx(pOutput, 4);
		reinterpret_cast<uint32_t*>(pOutput)[0] = uHigh;
		reinterpret_cast<uint32_t*>(pOutput)[1] = uLow;
#else
		*pOutput =
			Swap::load_dispatch<T*, sizeof(void*), false, true>()(pOutput);
#endif
	}
};

// Fixup four byte float (Use integer instructions)
template<>
struct fixup_dispatch<float, sizeof(float), true> {
	BURGER_INLINE void operator()(float* pOutput) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_t uTemp = __lwbrx(pOutput, 0);
		reinterpret_cast<uint32_t*>(pOutput)[0] = uTemp;
#else
		uint32_t* p32 = reinterpret_cast<uint32_t*>(pOutput);
		*p32 =
			Swap::load_dispatch<uint32_t*, sizeof(void*), false, true>()(p32);
#endif
	}
};

// Fixup eight byte float (Use integer instructions)
template<>
struct fixup_dispatch<double, sizeof(double), true> {
	BURGER_INLINE void operator()(double* pOutput) BURGER_NOEXCEPT
	{
#if defined(BURGER_METROWERKS) && defined(BURGER_PPC)
		uint32_t uLow = __lwbrx(pOutput, 0);
		uint32_t uHigh = __lwbrx(pOutput, 4);
		reinterpret_cast<uint32_t*>(pOutput)[0] = uHigh;
		reinterpret_cast<uint32_t*>(pOutput)[1] = uLow;
#else
		uint64_t* p64 = reinterpret_cast<uint64_t*>(pOutput);
		*p64 =
			Swap::load_dispatch<uint64_t*, sizeof(void*), false, true>()(p64);
#endif
	}
};

// Implemented on systems that don't support non-aligned reads and writes in
// hardware
#if !(defined(BURGER_INTEL) || defined(BURGER_ARM64))

// Catchall for SwapEndian::fixup_unaligned()
template<typename T, uintptr_t size, bool bIsArithmetic>
struct fixup_unaligned_dispatch {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
		// If this is triggered, a pointer was passed to an unsupported data
		// type
		BURGER_STATIC_ASSERT(pOutput || !pOutput);
	}
};

// Fixup single byte integer (No swapping)
template<typename T>
struct fixup_unaligned_dispatch<T, 1, true> {
	BURGER_INLINE void operator()(T* /* pOutput */) BURGER_NOEXCEPT {}
};

// Fixup two byte integer
template<typename T>
struct fixup_unaligned_dispatch<T, 2, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
		_store_unaligned(reinterpret_cast<uint16_t*>(pOutput),
			_load_unaligned_swap(reinterpret_cast<uint16_t*>(pOutput)));
	}
};

// Fixup four byte integer
template<typename T>
struct fixup_unaligned_dispatch<T, 4, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
		_store_unaligned(reinterpret_cast<uint32_t*>(pOutput),
			_load_unaligned_swap(reinterpret_cast<uint32_t*>(pOutput)));
	}
};

// Fixup eight byte integer
template<typename T>
struct fixup_unaligned_dispatch<T, 8, true> {
	BURGER_INLINE void operator()(T* pOutput) BURGER_NOEXCEPT
	{
		_store_unaligned(reinterpret_cast<uint64_t*>(pOutput),
			_load_unaligned_swap(reinterpret_cast<uint64_t*>(pOutput)));
	}
};

// Fixup four byte float (Use integer instructions)
template<>
struct fixup_unaligned_dispatch<float, sizeof(float), true> {
	BURGER_INLINE void operator()(float* pOutput) BURGER_NOEXCEPT
	{
		uint32_t* p32 = reinterpret_cast<uint32_t*>(pOutput);
		_store_unaligned(p32, _load_unaligned_swap(p32));
	}
};

// Fixup eight byte float (Use integer instructions)
template<>
struct fixup_unaligned_dispatch<double, sizeof(double), true> {
	BURGER_INLINE void operator()(double* pOutput) BURGER_NOEXCEPT
	{
		uint64_t* p64 = reinterpret_cast<uint64_t*>(pOutput);
		_store_unaligned(p64, _load_unaligned_swap(p64));
	}
};

#endif

}
#endif

struct SwapEndian {

	template<typename T>
	static BURGER_INLINE BURGER_CONSTEXPR typename remove_pointer<T>::type load(
		T Input) BURGER_NOEXCEPT
	{
		return Swap::load_dispatch<T, sizeof(T), is_arithmetic<T>::value,
			is_pointer<T>::value>()(Input);
	}

	template<typename T>
	static BURGER_INLINE void store(T* pOutput, T Input) BURGER_NOEXCEPT
	{
		Swap::store_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(
			pOutput, Input);
	}

	template<typename T>
	static BURGER_INLINE void store(
		volatile T* pOutput, T Input) BURGER_NOEXCEPT
	{
		store(const_cast<T*>(pOutput), Input);
	}

	template<typename T>
	static BURGER_INLINE T load_unaligned(T* pInput) BURGER_NOEXCEPT
	{
		// These CPUs allow non-aligned reads and writes
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64))
		return Swap::load_dispatch<T*, sizeof(void*), is_arithmetic<T*>::value,
			true>()(pInput);
#else
		typedef typename remove_cv<T>::type real_type;
		return Swap::load_unaligned_dispatch<real_type, sizeof(real_type),
			is_arithmetic<real_type>::value>()(const_cast<real_type*>(pInput));
#endif
	}

	template<typename T>
	static BURGER_INLINE void store_unaligned(
		T* pOutput, T Input) BURGER_NOEXCEPT
	{
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64))
		Swap::store_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(
			pOutput, Input);
#else
		Swap::store_unaligned_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(
			pOutput, Input);
#endif
	}

	template<typename T>
	static BURGER_INLINE void fixup(T* pOutput) BURGER_NOEXCEPT
	{
		Swap::fixup_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(pOutput);
	}

	template<typename T>
	static BURGER_INLINE void fixup(volatile T* pOutput) BURGER_NOEXCEPT
	{
		fixup(const_cast<T*>(pOutput));
	}

	template<typename T>
	static BURGER_INLINE void fixup_unaligned(T* pOutput) BURGER_NOEXCEPT
	{
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64))
		Swap::fixup_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(pOutput);
#else
		Swap::fixup_unaligned_dispatch<T, sizeof(T), is_arithmetic<T>::value>()(
			pOutput);
#endif
	}

	template<typename T>
	static BURGER_INLINE void fixup_unaligned(
		volatile T* pOutput) BURGER_NOEXCEPT
	{
		fixup_unaligned(const_cast<T*>(pOutput));
	}
};

#if (defined(BURGER_METROWERKS) && defined(BURGER_X86))
#pragma volatile_asm reset
#endif

// Which endian to swap, which to not.

#if defined(BURGER_LITTLEENDIAN)

struct LittleEndian: public NativeEndian {};
struct BigEndian: public SwapEndian {};
#define BURGER_BIGENDIAN16(x) (((x & 0xFFU) << 8U) + ((x >> 8U) & 0xFFU))
#define BURGER_BIGENDIAN32(x) \
	(((x & 0xFFU) << 24U) + ((x << 8U) & 0xFF0000U) + ((x >> 8U) & 0xFF00U) + \
		((x >> 24U) & 0xFFU))
#define BURGER_LITTLEENDIAN16(x) (x)
#define BURGER_LITTLEENDIAN32(x) (x)
#else

struct LittleEndian: public SwapEndian {};
struct BigEndian: public NativeEndian {};
#define BURGER_LITTLEENDIAN16(x) (((x & 0xFFU) << 8U) + ((x >> 8U) & 0xFFU))
#define BURGER_LITTLEENDIAN32(x) \
	(((x & 0xFFU) << 24U) + ((x << 8U) & 0xFF0000U) + ((x >> 8U) & 0xFF00U) + \
		((x >> 24U) & 0xFFU))
#define BURGER_BIGENDIAN16(x) (x)
#define BURGER_BIGENDIAN32(x) (x)

#endif

extern void BURGER_API swap_endian(
	uint16_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_endian(uint16_t* pOutput, const uint16_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_endian(
	uint32_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_endian(uint32_t* pOutput, const uint32_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_endian(
	uint64_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_endian(uint64_t* pOutput, const uint64_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT;
extern void BURGER_API swap_chars_to_bytes(
	void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
extern void BURGER_API swap_chars_to_bytes(
	void* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
}

/* END */

#endif
