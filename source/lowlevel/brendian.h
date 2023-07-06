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

namespace Burger {

extern float BURGER_API _swapendianfloat(float) BURGER_NOEXCEPT;
extern double BURGER_API _swapendiandouble(double) BURGER_NOEXCEPT;
extern float BURGER_API _swapendianfloat(const float*) BURGER_NOEXCEPT;
extern double BURGER_API _swapendiandouble(const double*) BURGER_NOEXCEPT;

#if !defined(DOXYGEN)
// Internal templates
namespace Swap {

// Handlers for SwapEndian::Load()
template<typename T, uintptr_t size, bool is_arithmetic, bool is_pointer>
struct load_dispatch {
	BURGER_INLINE BURGER_CONSTEXPR T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated on bad input
		BURGER_STATIC_ASSERT(Input * Input < 0);
		return Input;
	}
};

// Handlers for direct intrinsics (No pointers)
template<typename T>
struct load_dispatch<T, 1, true, false> {
	BURGER_INLINE BURGER_CONSTEXPR T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for single byte types
		return Input;
	}
};

template<typename T>
struct load_dispatch<T, 2, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for two byte types
		return static_cast<T>(_swapendian16(static_cast<uint16_t>(Input)));
	}
};

template<typename T>
struct load_dispatch<T, 4, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for arithmetic integral types
		return static_cast<T>(_swapendian32(static_cast<uint32_t>(Input)));
	}
};

template<typename T>
struct load_dispatch<T, 8, true, false> {
	BURGER_INLINE T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for arithmetic integral types
		return static_cast<T>(_swapendian64(static_cast<uint64_t>(Input)));
	}
};

template<>
struct load_dispatch<float, sizeof(float), true, false> {
	BURGER_INLINE float operator()(float Input) BURGER_NOEXCEPT
	{
		// Instantiated for float
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

// Handlers for pointer intrinsics
template<typename T>
struct load_dispatch_ptr<T, 1, true> {
	BURGER_INLINE BURGER_CONSTEXPR T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to single byte types
		BURGER_STATIC_ASSERT(is_arithmetic<T>::value);
		return *pInput;
	}
};

template<typename T>
struct load_dispatch_ptr<T, 2, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to two byte types
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

template<typename T>
struct load_dispatch_ptr<T, 4, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to arithmetic integral types
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

template<typename T>
struct load_dispatch_ptr<T, 8, true> {
	BURGER_INLINE T operator()(const T* pInput) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to arithmetic integral types
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
	defined(BURGER_PPC64) && !defined(BURGER_MACOSX)
		T uTemp;
		__asm__ __volatile__("ldbrx %0,0,%1"
							 : "=r"(uTemp)
							 : "r"(pInput), "m"(*pInput));
		return uTemp;
#elif (defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_PPC)
		T uTemp;
		__asm__ __volatile__("lwbrx %L0,0,%1"
							 : "=r"(uTemp)
							 : "r"(pInput), "m"(*pInput));
		__asm__ __volatile__(
			"lwbrx %0,0,%1"
			: "=r"(uTemp)
			: "r"((char*)pInput + 4), "m"(*pInput), "0"(uTemp));
		return uTemp;
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

template<>
struct load_dispatch_ptr<double, sizeof(double), true> {
	BURGER_INLINE double operator()(const double* pInput) BURGER_NOEXCEPT
	{
		// PowerPC has an instruction for this
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
}
#endif

struct SwapEndian {

	template<typename T>
	static BURGER_INLINE BURGER_CONSTEXPR typename remove_pointer<T>::type Load(
		T Input) BURGER_NOEXCEPT
	{
		return Swap::load_dispatch<T, sizeof(T), is_arithmetic<T>::value,
			is_pointer<T>::value>()(Input);
	}

#if (defined(BURGER_METROWERKS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)
	static uint32_t BURGER_API Load(uint32_t uInput) BURGER_NOEXCEPT;
#endif

	static BURGER_INLINE void Store(char* pOutput, char iInput) BURGER_NOEXCEPT
	{
		pOutput[0] = iInput;
	}
	static BURGER_INLINE void Store(
		uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = uInput;
	}
	static BURGER_INLINE void Store(
		int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT
	{
		pOutput[0] = iInput;
	}
#if (defined(BURGER_METROWERKS) && defined(BURGER_X86)) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = __ror(uInput, 8);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, register uint32_t uInput) BURGER_NOEXCEPT
	{
		__asm bswap uInput;
		pOutput[0] = uInput;
	}

#elif (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = __ror(uInput, 8);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, register uint32_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian32(uInput);
	}

#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		__sthbrx(uInput, pOutput, 0);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		__stwbrx(uInput, pOutput, 0);
	}
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		__storeshortbytereverse(uInput, 0, pOutput);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		__storewordbytereverse(uInput, 0, pOutput);
	}
#elif defined(BURGER_PS3) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		__sthbrx(pOutput, uInput);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		__stwbrx(pOutput, uInput);
	}
#elif defined(BURGER_INTEL_COMPILER) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian16(uInput);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian32(uInput);
	}

// Byteswap intrinsics were defined above
#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	defined(BURGER_MSVC) || (defined(BURGER_GNUC) && !defined(__SNC__))) && \
	!defined(DOXYGEN)

	static BURGER_INLINE void Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = Load(uInput);
	}
	static BURGER_INLINE void Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = Load(uInput);
	}
#else
	static void BURGER_API Store(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
	static void BURGER_API Store(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
	{
		return __storedoublewordbytereverse(uInput, 0, pOutput);
	}
#elif defined(BURGER_PS3) && !defined(DOXYGEN)
	static BURGER_INLINE void Store(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
	{
		__stdbrx(pOutput, uInput);
	}
#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_INTEL) && defined(BURGER_MSVC)) || \
	((defined(BURGER_GNUC) && !defined(__SNC__)) && \
		(defined(BURGER_AMD64) || defined(BURGER_ARM64) || \
			defined(BURGER_PPC)))) && \
	!defined(DOXYGEN)

	static BURGER_INLINE void Store(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = Load(uInput);
	}
#else
	static void BURGER_API Store(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif
	static void BURGER_API Store(float* pOutput, float fInput) BURGER_NOEXCEPT;
	static void BURGER_API Store(
		double* pOutput, double dInput) BURGER_NOEXCEPT;

	static BURGER_INLINE BURGER_CONSTEXPR char LoadAny(
		const char* pInput) BURGER_NOEXCEPT
	{
		return pInput[0];
	}
	static BURGER_INLINE BURGER_CONSTEXPR uint8_t LoadAny(
		const uint8_t* pInput) BURGER_NOEXCEPT
	{
		return pInput[0];
	}
	static BURGER_INLINE BURGER_CONSTEXPR int8_t LoadAny(
		const int8_t* pInput) BURGER_NOEXCEPT
	{
		return pInput[0];
	}
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

	static BURGER_INLINE uint16_t LoadAny(
		const uint16_t* pInput) BURGER_NOEXCEPT
	{
		return _swapendian16(pInput[0]);
	}
	static BURGER_INLINE uint32_t LoadAny(
		const uint32_t* pInput) BURGER_NOEXCEPT
	{
		return _swapendian32(pInput[0]);
	}
	static BURGER_INLINE uint64_t LoadAny(
		const uint64_t* pInput) BURGER_NOEXCEPT
	{
		return _swapendian64(pInput[0]);
	}
#else
	static uint16_t BURGER_API LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT;
	static uint32_t BURGER_API LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT;
	static uint64_t BURGER_API LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT;
#endif
	static float BURGER_API LoadAny(const float* pInput) BURGER_NOEXCEPT;
	static double BURGER_API LoadAny(const double* pInput) BURGER_NOEXCEPT;

	static BURGER_INLINE void StoreAny(
		char* pOutput, char iInput) BURGER_NOEXCEPT
	{
		pOutput[0] = iInput;
	}
	static BURGER_INLINE void StoreAny(
		uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = uInput;
	}
	static BURGER_INLINE void StoreAny(
		int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT
	{
		pOutput[0] = iInput;
	}
#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) && !defined(DOXYGEN)

	static BURGER_INLINE void StoreAny(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian16(uInput);
	}
	static BURGER_INLINE void StoreAny(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian32(uInput);
	}
	static BURGER_INLINE void StoreAny(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
	{
		pOutput[0] = _swapendian64(uInput);
	}
#else
	static void BURGER_API StoreAny(
		uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
	static void BURGER_API StoreAny(
		uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
	static void BURGER_API StoreAny(
		uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif

	static void BURGER_API StoreAny(
		float* pOutput, float fInput) BURGER_NOEXCEPT;
	static void BURGER_API StoreAny(
		double* pOutput, double dInput) BURGER_NOEXCEPT;

	static BURGER_INLINE void Fixup(char*) BURGER_NOEXCEPT {}
	static BURGER_INLINE void Fixup(uint8_t*) BURGER_NOEXCEPT {}
	static BURGER_INLINE void Fixup(int8_t*) BURGER_NOEXCEPT {}
	static BURGER_INLINE void Fixup(uint16_t* pInput) BURGER_NOEXCEPT
	{
		pInput[0] = Load(pInput);
	}
	static BURGER_INLINE void Fixup(uint32_t* pInput) BURGER_NOEXCEPT
	{
		pInput[0] = Load(pInput);
	}
#if defined(BURGER_64BITCPU) && !defined(DOXYGEN)
	static BURGER_INLINE void Fixup(uint64_t* pInput) BURGER_NOEXCEPT
	{
		pInput[0] = Load(pInput);
	}
#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	static BURGER_INLINE void Fixup(uint64_t* pInput) BURGER_NOEXCEPT
	{
		uint32_t uLow = __lwbrx(pInput, 0);
		uint32_t uHigh = __lwbrx(pInput, 4);
		reinterpret_cast<uint32_t*>(pInput)[0] = uHigh;
		reinterpret_cast<uint32_t*>(pInput)[1] = uLow;
	}
#else
	static void BURGER_API Fixup(uint64_t* pInput) BURGER_NOEXCEPT;
#endif
	static BURGER_INLINE void FixupAny(char*) BURGER_NOEXCEPT {}
	static BURGER_INLINE void FixupAny(uint8_t*) BURGER_NOEXCEPT {}
	static BURGER_INLINE void FixupAny(int8_t*) BURGER_NOEXCEPT {}
	static void BURGER_API FixupAny(uint16_t* pInput) BURGER_NOEXCEPT;
	static void BURGER_API FixupAny(uint32_t* pInput) BURGER_NOEXCEPT;
	static void BURGER_API FixupAny(uint64_t* pInput) BURGER_NOEXCEPT;

	static BURGER_INLINE void Store(
		int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint16_t*>(pOutput),
			static_cast<uint16_t>(iInput));
	}
	static BURGER_INLINE void Store(
		int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint32_t*>(pOutput),
			static_cast<uint32_t>(iInput));
	}
	static BURGER_INLINE void Store(
		int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint64_t*>(pOutput),
			static_cast<uint64_t>(iInput));
	}

	static BURGER_INLINE int16_t LoadAny(const int16_t* pInput) BURGER_NOEXCEPT
	{
		return static_cast<int16_t>(
			LoadAny(reinterpret_cast<const uint16_t*>(pInput)));
	}
	static BURGER_INLINE int32_t LoadAny(const int32_t* pInput) BURGER_NOEXCEPT
	{
		return static_cast<int32_t>(
			LoadAny(reinterpret_cast<const uint32_t*>(pInput)));
	}
	static BURGER_INLINE int64_t LoadAny(const int64_t* pInput) BURGER_NOEXCEPT
	{
		return static_cast<int64_t>(
			LoadAny(reinterpret_cast<const uint64_t*>(pInput)));
	}

	static BURGER_INLINE void StoreAny(
		int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint16_t*>(pOutput),
			static_cast<uint16_t>(iInput));
	}
	static BURGER_INLINE void StoreAny(
		int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint32_t*>(pOutput),
			static_cast<uint32_t>(iInput));
	}
	static BURGER_INLINE void StoreAny(
		int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint64_t*>(pOutput),
			static_cast<uint64_t>(iInput));
	}

	static BURGER_INLINE void Fixup(int16_t* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint16_t*>(pInput));
	}
	static BURGER_INLINE void Fixup(int32_t* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint32_t*>(pInput));
	}
	static BURGER_INLINE void Fixup(int64_t* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint64_t*>(pInput));
	}
	static BURGER_INLINE void Fixup(float* pInput) BURGER_NOEXCEPT
	{
		Fixup(static_cast<uint32_t*>(static_cast<void*>(pInput)));
	}
	static BURGER_INLINE void Fixup(double* pInput) BURGER_NOEXCEPT
	{
		Fixup(static_cast<uint64_t*>(static_cast<void*>(pInput)));
	}

	static BURGER_INLINE void FixupAny(int16_t* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint16_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(int32_t* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint32_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(int64_t* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint64_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(float* pInput) BURGER_NOEXCEPT
	{
		FixupAny(static_cast<uint32_t*>(static_cast<void*>(pInput)));
	}
	static BURGER_INLINE void FixupAny(double* pInput) BURGER_NOEXCEPT
	{
		FixupAny(static_cast<uint64_t*>(static_cast<void*>(pInput)));
	}

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
	static BURGER_INLINE void Store(
		wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint16_t*>(pOutput),
			static_cast<uint16_t>(uInput));
	}
	static BURGER_INLINE wchar_t LoadAny(const wchar_t* pInput) BURGER_NOEXCEPT
	{
		return static_cast<wchar_t>(
			LoadAny(reinterpret_cast<const uint16_t*>(pInput)));
	}
	static BURGER_INLINE void StoreAny(
		wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint16_t*>(pOutput), uInput);
	}
	static BURGER_INLINE void Fixup(wchar_t* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint16_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(wchar_t* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint16_t*>(pInput));
	}
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
	static BURGER_INLINE void Store(
		unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint2uint_t*>(pOutput),
			static_cast<uint2uint_t>(uInput));
	}
	static BURGER_INLINE void Store(
		signed int* pOutput, signed int iInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<uint2uint_t*>(pOutput),
			static_cast<uint2uint_t>(iInput));
	}
	static BURGER_INLINE unsigned int LoadAny(
		const unsigned int* pInput) BURGER_NOEXCEPT
	{
		return static_cast<unsigned int>(
			LoadAny(reinterpret_cast<const uint2uint_t*>(pInput)));
	}
	static BURGER_INLINE signed int LoadAny(
		const signed int* pInput) BURGER_NOEXCEPT
	{
		return static_cast<signed int>(
			LoadAny(reinterpret_cast<const uint2uint_t*>(pInput)));
	}
	static BURGER_INLINE void StoreAny(
		unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint2uint_t*>(pOutput),
			static_cast<uint2uint_t>(uInput));
	}
	static BURGER_INLINE void StoreAny(
		signed int* pOutput, signed int iInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<uint2uint_t*>(pOutput),
			static_cast<uint2uint_t>(iInput));
	}
	static BURGER_INLINE void Fixup(unsigned int* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint2uint_t*>(pInput));
	}
	static BURGER_INLINE void Fixup(signed int* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<uint2uint_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(unsigned int* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint2uint_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(signed int* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<uint2uint_t*>(pInput));
	}
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
	static BURGER_INLINE void Store(
		unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<ulong2uint_t*>(pOutput),
			static_cast<ulong2uint_t>(uInput));
	}
	static BURGER_INLINE void Store(
		signed long* pOutput, signed long iInput) BURGER_NOEXCEPT
	{
		Store(reinterpret_cast<ulong2uint_t*>(pOutput),
			static_cast<ulong2uint_t>(iInput));
	}
	static BURGER_INLINE unsigned long LoadAny(
		const unsigned long* pInput) BURGER_NOEXCEPT
	{
		return static_cast<unsigned long>(
			LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput)));
	}
	static BURGER_INLINE signed long LoadAny(
		const signed long* pInput) BURGER_NOEXCEPT
	{
		return static_cast<signed long>(
			LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput)));
	}
	static BURGER_INLINE void StoreAny(
		unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput),
			static_cast<ulong2uint_t>(uInput));
	}
	static BURGER_INLINE void StoreAny(
		signed long* pOutput, signed long iInput) BURGER_NOEXCEPT
	{
		StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput),
			static_cast<ulong2uint_t>(iInput));
	}
	static BURGER_INLINE void Fixup(unsigned long* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<ulong2uint_t*>(pInput));
	}
	static BURGER_INLINE void Fixup(signed long* pInput) BURGER_NOEXCEPT
	{
		Fixup(reinterpret_cast<ulong2uint_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(unsigned long* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<ulong2uint_t*>(pInput));
	}
	static BURGER_INLINE void FixupAny(signed long* pInput) BURGER_NOEXCEPT
	{
		FixupAny(reinterpret_cast<ulong2uint_t*>(pInput));
	}
#endif
	// clang-format on
};

/***************************************

	Don't swap endian

***************************************/

#if !defined(DOXYGEN)

// Namespace for native implementations
namespace Native {

// Catchall for load()
template<typename T, bool is_arithmetic, bool is_pointer>
struct load_dispatch {
	BURGER_INLINE BURGER_CONSTEXPR T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated on bad input
		BURGER_STATIC_ASSERT(Input * Input < 0);
		return Input;
	}
};

// Load arithmetic values
template<typename T>
struct load_dispatch<T, true, false> {
	BURGER_INLINE BURGER_CONSTEXPR T operator()(T Input) BURGER_NOEXCEPT
	{
		// Instantiated for arithmetic integral types
		return Input;
	}
};

// Load from a pointer to an arithmetic value
template<typename T>
struct load_dispatch<T, false, true> {
	BURGER_INLINE BURGER_CONSTEXPR typename remove_pointer<T>::type operator()(
		T Input) BURGER_NOEXCEPT
	{
		// Instantiated for pointers to arithmetic integral types
		BURGER_STATIC_ASSERT(
			is_arithmetic<typename remove_pointer<T>::type>::value);
		return *Input;
	}
};
}
#endif

struct NativeEndian {

	template<typename T>
	static BURGER_INLINE BURGER_CONSTEXPR typename remove_pointer<T>::type Load(
		T Input) BURGER_NOEXCEPT
	{
		return Native::load_dispatch<T, is_arithmetic<T>::value,
			is_pointer<T>::value>()(Input);
	}

	// clang-format off
    static BURGER_INLINE void Store(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void Store(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(float* pOutput, float fInput) BURGER_NOEXCEPT { pOutput[0] = fInput; }
    static BURGER_INLINE void Store(double* pOutput, double dInput) BURGER_NOEXCEPT { pOutput[0] = dInput; }

    static BURGER_INLINE BURGER_CONSTEXPR char LoadAny(const char* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint8_t LoadAny(const uint8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR int8_t LoadAny(const int8_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)
    static BURGER_INLINE BURGER_CONSTEXPR uint16_t LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint32_t LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR uint64_t LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR float LoadAny(const float* pInput) BURGER_NOEXCEPT { return pInput[0]; }
    static BURGER_INLINE BURGER_CONSTEXPR double LoadAny(const double* pInput) BURGER_NOEXCEPT { return pInput[0]; }
#else
    static uint16_t BURGER_API LoadAny(const uint16_t* pInput) BURGER_NOEXCEPT;
    static uint32_t BURGER_API LoadAny(const uint32_t* pInput) BURGER_NOEXCEPT;
    static uint64_t BURGER_API LoadAny(const uint64_t* pInput) BURGER_NOEXCEPT;
    static float BURGER_API LoadAny(const float* pInput) BURGER_NOEXCEPT;
    static double BURGER_API LoadAny(const double* pInput) BURGER_NOEXCEPT;
#endif
    static BURGER_INLINE int16_t LoadAny(const int16_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int16_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput)));}
    static BURGER_INLINE int32_t LoadAny(const int32_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int32_t>(LoadAny(reinterpret_cast<const uint32_t*>(pInput))); }
    static BURGER_INLINE int64_t LoadAny(const int64_t* pInput) BURGER_NOEXCEPT {
        return static_cast<int64_t>(LoadAny(reinterpret_cast<const uint64_t*>(pInput))); }

    static BURGER_INLINE void StoreAny(char* pOutput, char iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE void StoreAny(uint8_t* pOutput, uint8_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void StoreAny(int8_t* pOutput, int8_t iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
#if (defined(BURGER_INTEL) || defined(BURGER_ARM64)) && !defined(DOXYGEN)
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static void BURGER_API StoreAny(float* pOutput, float fInput) BURGER_NOEXCEPT { pOutput[0] = fInput; }
    static void BURGER_API StoreAny(double* pOutput, double dInput) BURGER_NOEXCEPT { pOutput[0] = dInput; }
#else
    static void BURGER_API StoreAny(uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(float* pOutput, float fInput) BURGER_NOEXCEPT;
    static void BURGER_API StoreAny(double* pOutput, double dInput) BURGER_NOEXCEPT;
#endif

    static BURGER_INLINE void StoreAny(int16_t* pOutput, int16_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), static_cast<uint16_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int32_t* pOutput, int32_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint32_t*>(pOutput), static_cast<uint32_t>(iInput)); }
    static BURGER_INLINE void StoreAny(int64_t* pOutput, int64_t iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint64_t*>(pOutput),  static_cast<uint64_t>(iInput)); }

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
	static BURGER_INLINE void Store(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE wchar_t LoadAny(const wchar_t* pInput) BURGER_NOEXCEPT {
        return static_cast<wchar_t>(LoadAny(reinterpret_cast<const uint16_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(wchar_t* pOutput, wchar_t uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint16_t*>(pOutput), uInput);
    }
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
	static BURGER_INLINE void Store(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE BURGER_CONSTEXPR unsigned int LoadAny(const unsigned int* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput)));}
    static BURGER_INLINE BURGER_CONSTEXPR signed int LoadAny(const signed int* pInput) BURGER_NOEXCEPT {
        return static_cast<signed int>(LoadAny(reinterpret_cast<const uint2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned int* pOutput, unsigned int uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed int* pOutput, signed int iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<uint2uint_t*>(pOutput), static_cast<uint2uint_t>(iInput)); }
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
	static BURGER_INLINE void Store(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT { pOutput[0] = uInput; }
    static BURGER_INLINE void Store(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT { pOutput[0] = iInput; }
    static BURGER_INLINE unsigned long LoadAny(const unsigned long* pInput) BURGER_NOEXCEPT {
        return static_cast<unsigned long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE signed long LoadAny(const signed long* pInput) BURGER_NOEXCEPT {
        return static_cast<signed long>(LoadAny(reinterpret_cast<const ulong2uint_t*>(pInput))); }
    static BURGER_INLINE void StoreAny(unsigned long* pOutput, unsigned long uInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(uInput)); }
    static BURGER_INLINE void StoreAny(signed long* pOutput, signed long iInput) BURGER_NOEXCEPT {
        StoreAny(reinterpret_cast<ulong2uint_t*>(pOutput), static_cast<ulong2uint_t>(iInput)); }
#endif
	// clang-format on

	template<typename T>
	static BURGER_INLINE void Fixup(T) BURGER_NOEXCEPT
	{
	}

	template<typename T>
	static BURGER_INLINE void FixupAny(T) BURGER_NOEXCEPT
	{
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

extern void BURGER_API ConvertEndian(uint16_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
	uint16_t* pOutput, const uint16_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(uint32_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
	uint32_t* pOutput, const uint32_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(uint64_t* pInput, uintptr_t uCount);
extern void BURGER_API ConvertEndian(
	uint64_t* pOutput, const uint64_t* pInput, uintptr_t uCount);
extern void BURGER_API SwapCharsToBytes(void* pInput, uintptr_t uLength);
extern void BURGER_API SwapCharsToBytes(
	void* pOutput, const void* pInput, uintptr_t uLength);
}

/* END */

#endif
