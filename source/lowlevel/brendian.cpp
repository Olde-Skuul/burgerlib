/***************************************

	Handle endian swapping

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brendian.h"
#include "brfloatingpoint.h"
#include "brtemplates.h"

/*! ************************************

	\brief Swap endian of a 16 bit integer

	Reverse the endian of a 16 bit integer. Implemented in assembly on some
	platforms.

	\param uInput The value to return endian swapped

	\return Input endian swapped

	\sa _swapendian32(uint32_t), or _swapendian64(uint64_t)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_APPLE_SC) && defined(BURGER_68K)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_68K) || defined(BURGER_X86) || \
			defined(BURGER_PPC))) || \
	((defined(BURGER_MRC) || defined(BURGER_GHS)) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && \
		(defined(BURGER_ARM32) || defined(BURGER_PPC))) || \
	(__has_builtin(__builtin_bswap16) || (BURGER_GNUC >= 40800)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		(defined(BURGER_68K) || defined(BURGER_PPC) || defined(BURGER_ARM) || \
			defined(BURGER_INTEL))) || \
	defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC)) && \
	!defined(DOXYGEN)

#else
uint16_t BURGER_API _swapendian16(uint16_t uInput) BURGER_NOEXCEPT
{
	return ((uInput & 0xFF) << 8) | ((uInput >> 8) & 0xFF);
}
#endif

/*! ************************************

	\brief Swap endian of a 32 bit integer

	Reverse the endian of a 32 bit integer. Implemented in assembly on some
	platforms.

	\param uInput The value to return endian swapped

	\return Input endian swapped

	\sa _swapendian16(uint16_t), or _swapendian64(uint64_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_ARM32)) && !defined(DOXYGEN)

// Nintendo DS ARM doesn't have REV
asm uint32_t BURGER_API _swapendian32(uint32_t uInput) BURGER_NOEXCEPT{
	// clang-format off

	// r1 = 12^56,34^78,12^56,34^78
	eor r1, r0, r0, ror #16

	// r1 = 12^56,0,12^56,34^78
	bic r1, r1, #0x00ff0000

	// r0 = 78,12,34,56
	mov r0, r0, ror #8

	// r0 = 78^0,12^12^56,34^0,56^12^56 (78563412)
	eor r0, r0, r1, lsr #8
	bx lr
	// clang-format on
}

#elif ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_APPLE_SC) && defined(BURGER_68K)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_68K) || defined(BURGER_X86) || \
			defined(BURGER_PPC))) || \
	(__has_builtin(__builtin_bswap32) || (BURGER_GNUC >= 40300) || \
		defined(BURGER_ARM_COMPILER)) || \
	((defined(BURGER_MRC) || defined(BURGER_SNSYSTEMS) || \
		 defined(BURGER_GHS)) && \
		defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && defined(BURGER_ARM32)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		(defined(BURGER_68K) || defined(BURGER_PPC) || \
			defined(BURGER_ARM32) || defined(BURGER_ARM64) || \
			defined(BURGER_INTEL))) || \
	defined(BURGER_INTEL_COMPILER) || defined(BURGER_MSVC)) && \
	!defined(DOXYGEN)

#else
uint32_t BURGER_API _swapendian32(uint32_t uInput) BURGER_NOEXCEPT
{
	return ((uInput & 0xFF000000) >> 24) | ((uInput & 0x00FF0000) >> 8) |
		((uInput & 0x0000FF00) << 8) | ((uInput & 0x000000FF) << 24);
}
#endif

/*! ************************************

	\brief Swap endian of a 64 bit integer.

	64 bit operations for endian swap are specialized on different platforms
	since some forms require the return value to be in a structure.

	\param uInput 64 integer to swap endian.

	\return Input with all 8 bytes reversed.

	\sa _swapendian16(uint16_t), or _swapendian32(uint32_t)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_APPLE_SC) && defined(BURGER_68K)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(__has_builtin(__builtin_bswap64) || (BURGER_GNUC >= 40300) || \
		defined(BURGER_ARM_COMPILER)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		defined(BURGER_ARM32)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		defined(BURGER_ARM64)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		defined(BURGER_X86)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		defined(BURGER_AMD64)) || \
	defined(BURGER_INTEL_COMPILER) || (BURGER_MSVC >= 140000000) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))) && \
	!defined(DOXYGEN)

// Written in assembly (MacOS, PS3, Vita, WiiU)
#elif ((defined(BURGER_METROWERKS) && \
		   (defined(BURGER_68K) || defined(BURGER_PPC))) || \
	(defined(BURGER_GHS) && defined(BURGER_PPC)) || \
	(defined(BURGER_SNSYSTEMS) && \
		(defined(BURGER_PPC) || defined(BURGER_ARM32))) || \
	(defined(BURGER_MACOSX) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)
#else
uint64_t BURGER_API _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
{
	return _swapendian32(static_cast<uint32_t>(uInput >> 32)) |
		(static_cast<uint64_t>(_swapendian32(static_cast<uint32_t>(uInput)))
			<< 32);
}
#endif

/*! ************************************

	\fn _swapendianstore16(void* pOutput, uint16_t uInput)
	\brief Store a 16 bit value endian swapped

	Reverse the endian of a 16 bit integer and store the value to memory

	\note This function only exists on Metrowerks for PowerPC. It's a workaround
		for a pointer propagation bug when using the intrinsic __sthbrx

	\param pOutput Pointer to 16 bit aligned memory
	\param uInput The value to store endian swapped

	\sa _swapendianstore32(void *, uint32_t), or
		_swapendianstore64(void *, uint64_t)

***************************************/

/*! ************************************

	\fn _swapendianstore32(void* pOutput, uint32_t uInput)
	\brief Store a 32 bit value endian swapped

	Reverse the endian of a 32 bit integer and store the value to memory

	\note This function only exists on Metrowerks for PowerPC. It's a workaround
		for a pointer propagation bug when using the intrinsic __stwbrx

	\param pOutput Pointer to 32 bit aligned memory
	\param uInput The value to store endian swapped

	\sa _swapendianstore16(void *, uint16_t), or
		_swapendianstore64(void *, uint64_t)

***************************************/

/*! ************************************

	\fn _swapendianstore64(void* pOutput, uint64_t uInput)
	\brief Store a 64 bit value endian swapped

	Reverse the endian of a 64 bit integer and store the value to memory

	\note This function only exists on Metrowerks for PowerPC and XCode for
		PowerPC. Both compilers generate bad code when using __stwbrx on 64
		bit integers.

	\param pOutput Pointer to 64 bit aligned memory
	\param uInput The value to store endian swapped

	\sa _swapendianstore16(void *, uint16_t),
		_swapendianstore32(void *, uint32_t), or
		_swapendian64ptr(const void* pInput)

***************************************/

/*! ************************************

	\fn _swapendian64ptr(const void* pInput)
	\brief Load a 64 bit value endian swapped

	Reverse the endian of a 64 bit integer and as it is being read from memory

	\note This function only exists on XCode for PowerPC. The compiler
		generates bad code when using __ldwbrx on 64 bit integers.

	\param pInput Pointer to 64 bit aligned memory

	\return The value to read with endian swapped.

	\sa _swapendianstore64(void *, uint64_t)

***************************************/

/*! ************************************

	\brief Swap endian of a 32 bit float

	Reverse the endian of a 32 bit float. Implemented in assembly on some
	platforms.

	\note It's recommended to use the pointer versions of this function because
		they guarantee bit accurate endian swaps. Some CPUs will modify floating
		point values if endian swapped if they convert to a NaN.

	\param fInput The value to return endian swapped

	\return Input endian swapped

	\sa _swapendianfloat(const float*), SwapEndian::load(T)

***************************************/

// 170000000 == VS 2012

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC) || \
			defined(BURGER_68K))) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86) && !defined(BURGER_SSE2))) && \
	!defined(DOXYGEN)
// These are in assembly
#else

float BURGER_API Burger::_swapendianfloat(float fInput) BURGER_NOEXCEPT
{
	// Convert from float to uint32_t structure
	uint32_float_t Temp;

	// Save the float
	Temp.f = fInput;

	// Endian swap as a word
	Temp.w = SwapEndian::load(Temp.w);

	// Return the float
	return Temp.f;
}
#endif

/*! ************************************

	\brief Swap endian of a 64 bit float

	Reverse the endian of a 64 bit float. Implemented in assembly on some
	platforms.

	\note It's recommended to use the pointer versions of this function because
		they guarantee bit accurate endian swaps. Some CPUs will modify floating
		point values if endian swapped if they convert to a NaN.

	\param dInput The value to return endian swapped

	\return Input endian swapped

	\sa _swapendiandouble(const double*), SwapEndian::load(T)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC) || \
			defined(BURGER_68K))) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86) && !defined(BURGER_SSE2)) || \
	(defined(BURGER_MACOSX) && defined(BURGER_PPC))) && \
	!defined(DOXYGEN)
// These are in assembly
#else

double BURGER_API Burger::_swapendiandouble(double dInput) BURGER_NOEXCEPT
{
	uint64_double_t Temp;

	// 64 bit cpus can do this in one instruction (Mostly)
#if defined(BURGER_64BITCPU)
	Temp.w = SwapEndian::load(
		static_cast<const uint64_t*>(static_cast<const void*>(&dInput)));
#else

	// Make it easy on the 32 bit CPUs
	Temp.w32[0] = SwapEndian::load(
		static_cast<const uint32_t*>(static_cast<const void*>(&dInput)) + 1);
	Temp.w32[1] = SwapEndian::load(
		static_cast<const uint32_t*>(static_cast<const void*>(&dInput)) + 0);
#endif

	// Return the double endian swapped
	return Temp.d;
}
#endif

/*! ************************************

	\brief Swap endian of a 32 bit float from a pointer

	Reverse the endian of a 32 bit float. Implemented in assembly on some
	platforms.

	\param pInput Pointer to the value to return endian swapped

	\return Input endian swapped

	\sa _swapendianfloat(float), SwapEndian::load(T)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC) || \
			defined(BURGER_68K))) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86) && !defined(BURGER_SSE2))) && \
	!defined(DOXYGEN)
// These are in assembly
#else

float BURGER_API Burger::_swapendianfloat(const float* pInput) BURGER_NOEXCEPT
{
	uint32_float_t Temp;
	Temp.w = SwapEndian::load(
		static_cast<const uint32_t*>(static_cast<const void*>(pInput)));

	// Return the float
	return Temp.f;
}

#endif

/*! ************************************

	\brief Swap endian of a 64 bit float

	Reverse the endian of a 64 bit float. Implemented in assembly on some
	platforms.

	\param pInput Pointer to the value to return endian swapped

	\return Input endian swapped

	\sa _swapendiandouble(double), SwapEndian::load(T)

***************************************/

#if ((defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_PPC) || \
			defined(BURGER_68K))) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86) && !defined(BURGER_SSE2))) && \
	!defined(DOXYGEN)
// These are in assembly
#else

double BURGER_API Burger::_swapendiandouble(
	const double* pInput) BURGER_NOEXCEPT
{
	uint64_double_t Temp;
#if defined(BURGER_64BITCPU)
	Temp.w = SwapEndian::load(
		static_cast<const uint64_t*>(static_cast<const void*>(pInput)));
#else
	Temp.w32[0] = SwapEndian::load(
		static_cast<const uint32_t*>(static_cast<const void*>(pInput)) + 1);
	Temp.w32[1] = SwapEndian::load(
		static_cast<const uint32_t*>(static_cast<const void*>(pInput)) + 0);
#endif

	// Return the double
	return Temp.d;
}

#endif

/*! ************************************

	\brief Fetch a 16 bit unsigned value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time and
	reconstruct it into a 16 bit value in native endian.

	\param pInput Pointer to a 16 bit value.

	\return The 16 bit unsigned value.

	\sa _load_unaligned_swap(const uint16_t*)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else

uint16_t BURGER_API Burger::_load_unaligned(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Grab 16 bits
	uint32_t uLow = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint32_t uHigh = reinterpret_cast<const uint8_t*>(pInput)[1];

	// Reconstruct using native endian

#if defined(BURGER_BIGENDIAN)
	// PPC likes bit or
	uint16_t uResult = static_cast<uint16_t>((uLow << 8U) | uHigh);

#else
	// Intel likes addition
	uint16_t uResult = static_cast<uint16_t>(uLow + (uHigh << 8U));
#endif
	return uResult;
}

#endif

/*! ************************************

	\brief Fetch a 32 bit unsigned value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 32 bit value in native endian.

	\param pInput Pointer to a 32 bit value.

	\return The 32 bit unsigned value.

	\sa _load_unaligned_swap(const uint32_t*)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else

uint32_t BURGER_API Burger::_load_unaligned(
	const uint32_t* pInput) BURGER_NOEXCEPT
{
	// Grab 32 bits
	uint32_t uLow1 = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint32_t uLow2 = reinterpret_cast<const uint8_t*>(pInput)[1];
	uint32_t uLow3 = reinterpret_cast<const uint8_t*>(pInput)[2];
	uint32_t uLow4 = reinterpret_cast<const uint8_t*>(pInput)[3];

	// Reconstruct using native endian

#if defined(BURGER_BIGENDIAN)
	// PPC likes bit or
	uint32_t uResult = static_cast<uint32_t>(
		(uLow1 << 24) | (uLow2 << 16) | (uLow3 << 8) | uLow4);
#else
	// Intel likes addition
	uint32_t uResult = static_cast<uint32_t>(
		uLow1 + (uLow2 << 8) + (uLow3 << 16) + (uLow4 << 24));
#endif
	return uResult;
}

#endif

/*! ************************************

	\brief Fetch a 64 bit unsigned value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in native endian.

	\param pInput Pointer to a 64 bit value.

	\return The 64 bit unsigned value.

	\sa _load_unaligned_swap(const uint64_t*)

***************************************/

#if ((defined(BURGER_METROWERKS) && \
		 (defined(BURGER_68K) || defined(BURGER_PPC))) || \
	(defined(BURGER_MSVC) && defined(BURGER_PPC)) || defined(BURGER_PS3) || \
	defined(BURGER_WIIU) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
		defined(BURGER_PPC))) && \
	!defined(DOXYGEN)

#else
uint64_t BURGER_API Burger::_load_unaligned(
	const uint64_t* pInput) BURGER_NOEXCEPT
{
	// Use a memory variable
	uint64_t uResult;

	// Memcpy
	reinterpret_cast<uint8_t*>(&uResult)[0] =
		reinterpret_cast<const uint8_t*>(pInput)[0];
	reinterpret_cast<uint8_t*>(&uResult)[1] =
		reinterpret_cast<const uint8_t*>(pInput)[1];
	reinterpret_cast<uint8_t*>(&uResult)[2] =
		reinterpret_cast<const uint8_t*>(pInput)[2];
	reinterpret_cast<uint8_t*>(&uResult)[3] =
		reinterpret_cast<const uint8_t*>(pInput)[3];
	reinterpret_cast<uint8_t*>(&uResult)[4] =
		reinterpret_cast<const uint8_t*>(pInput)[4];
	reinterpret_cast<uint8_t*>(&uResult)[5] =
		reinterpret_cast<const uint8_t*>(pInput)[5];
	reinterpret_cast<uint8_t*>(&uResult)[6] =
		reinterpret_cast<const uint8_t*>(pInput)[6];
	reinterpret_cast<uint8_t*>(&uResult)[7] =
		reinterpret_cast<const uint8_t*>(pInput)[7];

	// Return the 64 bit value in a register
	return uResult;
}
#endif

/*! ************************************

	\brief Fetch a float value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a float value in native endian.

	\param pInput Pointer to a float value.

	\return The float value.

	\sa _load_unaligned_swap(const float*), or _load_unaligned(const uint32_t*)

***************************************/

float BURGER_API Burger::_load_unaligned(const float* pInput) BURGER_NOEXCEPT
{
	uint32_float_t uTemp;
	uTemp.w = _load_unaligned(
		static_cast<const uint32_t*>(static_cast<const void*>(pInput)));
	return uTemp.f;
}

/*! ************************************

	\brief Fetch a double value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a double value in native endian.

	\param pInput Pointer to a double value.

	\return The double value.

	\sa _load_unaligned_swap(const double*), or _load_unaligned(const uint64_t*)

***************************************/

double BURGER_API Burger::_load_unaligned(const double* pInput) BURGER_NOEXCEPT
{
	uint64_double_t uTemp;
	uTemp.w = _load_unaligned(
		static_cast<const uint64_t*>(static_cast<const void*>(pInput)));
	return uTemp.d;
}

/*! ************************************

	\brief Store a 16 bit unsigned value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 16 bit value in native endian.

	\param pOutput Pointer to a 16 bit value.

	\param uInput The 16 bit unsigned value.

	\sa _store_unaligned_swap(uint16_t*,uint16_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned(
	uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput);
#else
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
#endif
}
#endif

/*! ************************************

	\brief Store a 32 bit unsigned value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 32 bit value in native endian.

	\param pOutput Pointer to a 32 bit value.

	\param uInput The 32 bit unsigned value.

	\sa _store_unaligned_swap(uint32_t*,uint32_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned(
	uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		static_cast<uint8_t>(uInput >> 24U);
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		static_cast<uint8_t>(uInput >> 16U);
	reinterpret_cast<uint8_t*>(pOutput)[2] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[3] = static_cast<uint8_t>(uInput);
#else
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		static_cast<uint8_t>(uInput >> 16U);
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		static_cast<uint8_t>(uInput >> 24U);
#endif
}
#endif

/*! ************************************

	\brief Store a 64 bit unsigned value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 64 bit value in native endian.

	\param pOutput Pointer to a 64 bit value.

	\param uInput The 64 bit unsigned value.

	\sa _store_unaligned_swap(uint64_t*,uint64_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned(
	uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&uInput)[0];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&uInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&uInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&uInput)[3];
	reinterpret_cast<uint8_t*>(pOutput)[4] =
		reinterpret_cast<const uint8_t*>(&uInput)[4];
	reinterpret_cast<uint8_t*>(pOutput)[5] =
		reinterpret_cast<const uint8_t*>(&uInput)[5];
	reinterpret_cast<uint8_t*>(pOutput)[6] =
		reinterpret_cast<const uint8_t*>(&uInput)[6];
	reinterpret_cast<uint8_t*>(pOutput)[7] =
		reinterpret_cast<const uint8_t*>(&uInput)[7];
}
#endif

/*! ************************************

	\brief Store a float value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 32 bit float value in native endian.

	\param pOutput Pointer to a float value.

	\param fInput The float value.

	\sa _store_unaligned_swap(float*,float)

***************************************/

void BURGER_API Burger::_store_unaligned(
	float* pOutput, float fInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&fInput)[0];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&fInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&fInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&fInput)[3];
}

/*! ************************************

	\brief Store a double value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 64 bit float value in native endian.

	\param pOutput Pointer to a double value.

	\param dInput The double value.

	\sa _store_unaligned_swap(double*,double).

***************************************/

void BURGER_API Burger::_store_unaligned(
	double* pOutput, double dInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&dInput)[0];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&dInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&dInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&dInput)[3];
	reinterpret_cast<uint8_t*>(pOutput)[4] =
		reinterpret_cast<const uint8_t*>(&dInput)[4];
	reinterpret_cast<uint8_t*>(pOutput)[5] =
		reinterpret_cast<const uint8_t*>(&dInput)[5];
	reinterpret_cast<uint8_t*>(pOutput)[6] =
		reinterpret_cast<const uint8_t*>(&dInput)[6];
	reinterpret_cast<uint8_t*>(pOutput)[7] =
		reinterpret_cast<const uint8_t*>(&dInput)[7];
}

/*! ************************************

	\brief Fetch a 16 bit unsigned reverse endian integer from memory with byte
		alignment.

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to a value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint16_t *)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
uint16_t BURGER_API Burger::_load_unaligned_swap(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Grab 16 bits
	uint32_t uLow = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint32_t uHigh = reinterpret_cast<const uint8_t*>(pInput)[1];
#if defined(BURGER_LITTLEENDIAN)
	// Intel likes +
	uint16_t uResult = static_cast<uint16_t>((uLow << 8U) + uHigh);
#else
	// PPC likes |
	uint16_t uResult = static_cast<uint16_t>(uLow | (uHigh << 8U));
#endif
	return uResult;
}
#endif

/*! ************************************

	\brief Fetch a 32 bit unsigned reverse endian integer from memory with byte
		alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint32_t *)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
uint32_t BURGER_API Burger::_load_unaligned_swap(
	const uint32_t* pInput) BURGER_NOEXCEPT
{
	// Grab 32 bits
	uint32_t uLow1 = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint32_t uLow2 = reinterpret_cast<const uint8_t*>(pInput)[1];
	uint32_t uLow3 = reinterpret_cast<const uint8_t*>(pInput)[2];
	uint32_t uLow4 = reinterpret_cast<const uint8_t*>(pInput)[3];
#if defined(BURGER_LITTLEENDIAN)
	// Intel likes +
	uint32_t uResult = static_cast<uint32_t>(
		(uLow1 << 24U) + (uLow2 << 16U) + (uLow3 << 8U) + uLow4);
#else
	// PPC likes |
	uint32_t uResult = static_cast<uint32_t>(
		uLow1 | (uLow2 << 8U) | (uLow3 << 16U) | (uLow4 << 24U));
#endif
	return uResult;
}
#endif

/*! ************************************

	\brief Fetch a 64 bit unsigned reverse endian integer from memory with byte
		alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in reverse endian. The pointer does
	not have to be 64-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to a 64 bit value.

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint64_t*)

***************************************/

#if (defined(BURGER_METROWERKS) && \
	(defined(BURGER_PPC) || defined(BURGER_68K))) && \
	!defined(DOXYGEN)

#else
uint64_t BURGER_API Burger::_load_unaligned_swap(
	const uint64_t* pInput) BURGER_NOEXCEPT
{
	// Make sure it's aligned
	uint64_t Result;

	reinterpret_cast<uint8_t*>(&Result)[7] =
		reinterpret_cast<const uint8_t*>(pInput)[0];
	reinterpret_cast<uint8_t*>(&Result)[6] =
		reinterpret_cast<const uint8_t*>(pInput)[1];
	reinterpret_cast<uint8_t*>(&Result)[5] =
		reinterpret_cast<const uint8_t*>(pInput)[2];
	reinterpret_cast<uint8_t*>(&Result)[4] =
		reinterpret_cast<const uint8_t*>(pInput)[3];
	reinterpret_cast<uint8_t*>(&Result)[3] =
		reinterpret_cast<const uint8_t*>(pInput)[4];
	reinterpret_cast<uint8_t*>(&Result)[2] =
		reinterpret_cast<const uint8_t*>(pInput)[5];
	reinterpret_cast<uint8_t*>(&Result)[1] =
		reinterpret_cast<const uint8_t*>(pInput)[6];
	reinterpret_cast<uint8_t*>(&Result)[0] =
		reinterpret_cast<const uint8_t*>(pInput)[7];

	// Return the 64 bit value in a register
	return Result;
}
#endif

/*! ************************************

	\brief Fetch a 32 bit reverse endian float from memory with byte alignment.

	Given a pointer to a 32-bit floating point value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const float *)

***************************************/

float BURGER_API Burger::_load_unaligned_swap(
	const float* pInput) BURGER_NOEXCEPT
{
	// This MUST be cast as a float to be float aligned!
	float fInput;

	uint8_t bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[1];
	// Perform the swap
	reinterpret_cast<uint8_t*>(&fInput)[3] = bTemp1;
	reinterpret_cast<uint8_t*>(&fInput)[2] = bTemp2;

	bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[2];
	bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[3];
	reinterpret_cast<uint8_t*>(&fInput)[1] = bTemp1;
	reinterpret_cast<uint8_t*>(&fInput)[0] = bTemp2;

	// Return the float
	return fInput;
}

/*! ************************************

	\brief Fetch a 64 bit reverse endian float from memory with byte alignment.

	Given a pointer to a 64-bit floating point value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not
	have to be 64-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const double *)

***************************************/

double BURGER_API Burger::_load_unaligned_swap(
	const double* pInput) BURGER_NOEXCEPT
{
	// This MUST be cast as a double to be double aligned!
	double dTemp;

	uint8_t bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[0];
	uint8_t bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[1];

	// Perform the swap
	reinterpret_cast<uint8_t*>(&dTemp)[7] = bTemp1;
	reinterpret_cast<uint8_t*>(&dTemp)[6] = bTemp2;
	bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[2];
	bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[3];
	reinterpret_cast<uint8_t*>(&dTemp)[5] = bTemp1;
	reinterpret_cast<uint8_t*>(&dTemp)[4] = bTemp2;
	bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[4];
	bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[5];
	reinterpret_cast<uint8_t*>(&dTemp)[3] = bTemp1;
	reinterpret_cast<uint8_t*>(&dTemp)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const uint8_t*>(pInput)[6];
	bTemp2 = reinterpret_cast<const uint8_t*>(pInput)[7];
	reinterpret_cast<uint8_t*>(&dTemp)[1] = bTemp1;
	reinterpret_cast<uint8_t*>(&dTemp)[0] = bTemp2;

	// Return the double
	return dTemp;
}

/*! ************************************

	\brief Store a 16 bit unsigned value with endian swapping to memory with
		byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 16 bit value in reversed endian.

	\param pOutput Pointer to a 16 bit value.

	\param uInput The 16 bit unsigned value.

	\sa _store_unaligned(uint16_t*,uint16_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned_swap(
	uint16_t* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
#else
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput);
#endif
}
#endif

/*! ************************************

	\brief Store a 32 bit unsigned value with endian swapping to memory with
		byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 32 bit value in reversed endian.

	\param pOutput Pointer to a 32 bit value.

	\param uInput The 32 bit unsigned value.

	\sa _store_unaligned(uint32_t*, uint32_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_68K)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned_swap(
	uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
#if defined(BURGER_BIGENDIAN)
	reinterpret_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(uInput);
	reinterpret_cast<uint8_t*>(pOutput)[1] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		static_cast<uint8_t>(uInput >> 16U);
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		static_cast<uint8_t>(uInput >> 24U);
#else
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		static_cast<uint8_t>(uInput >> 24U);
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		static_cast<uint8_t>(uInput >> 16U);
	reinterpret_cast<uint8_t*>(pOutput)[2] = static_cast<uint8_t>(uInput >> 8U);
	reinterpret_cast<uint8_t*>(pOutput)[3] = static_cast<uint8_t>(uInput);
#endif
}
#endif

/*! ************************************

	\brief Store a 64 bit unsigned value with endian swapping to memory with
		byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 64 bit value in reversed endian.

	\param pOutput Pointer to a 64 bit value.

	\param uInput The 64 bit unsigned value.

	\sa _store_unaligned(uint64_t*,uint64_t)

***************************************/

#if (defined(BURGER_METROWERKS) && defined(BURGER_PPC)) && !defined(DOXYGEN)

#else
void BURGER_API Burger::_store_unaligned_swap(
	uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&uInput)[7];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&uInput)[6];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&uInput)[5];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&uInput)[4];
	reinterpret_cast<uint8_t*>(pOutput)[4] =
		reinterpret_cast<const uint8_t*>(&uInput)[3];
	reinterpret_cast<uint8_t*>(pOutput)[5] =
		reinterpret_cast<const uint8_t*>(&uInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[6] =
		reinterpret_cast<const uint8_t*>(&uInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[7] =
		reinterpret_cast<const uint8_t*>(&uInput)[0];
}
#endif

/*! ************************************

	\brief Store a float value to memory with endian swapping with byte
		alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 32 bit float value in reversed endian.

	\param pOutput Pointer to a float value.

	\param fInput The float value.

	\sa _store_unaligned(float*, float)

***************************************/

void BURGER_API Burger::_store_unaligned_swap(
	float* pOutput, float fInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&fInput)[3];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&fInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&fInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&fInput)[0];
}

/*! ************************************

	\brief Store a double value to memory with endian swapping with byte
		alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 64 bit float value in reversed endian.

	\param pOutput Pointer to a double value.

	\param dInput The double value.

	\sa _store_unaligned(double*,double)

***************************************/

void BURGER_API Burger::_store_unaligned_swap(
	double* pOutput, double dInput) BURGER_NOEXCEPT
{
	reinterpret_cast<uint8_t*>(pOutput)[0] =
		reinterpret_cast<const uint8_t*>(&dInput)[7];
	reinterpret_cast<uint8_t*>(pOutput)[1] =
		reinterpret_cast<const uint8_t*>(&dInput)[6];
	reinterpret_cast<uint8_t*>(pOutput)[2] =
		reinterpret_cast<const uint8_t*>(&dInput)[5];
	reinterpret_cast<uint8_t*>(pOutput)[3] =
		reinterpret_cast<const uint8_t*>(&dInput)[4];
	reinterpret_cast<uint8_t*>(pOutput)[4] =
		reinterpret_cast<const uint8_t*>(&dInput)[3];
	reinterpret_cast<uint8_t*>(pOutput)[5] =
		reinterpret_cast<const uint8_t*>(&dInput)[2];
	reinterpret_cast<uint8_t*>(pOutput)[6] =
		reinterpret_cast<const uint8_t*>(&dInput)[1];
	reinterpret_cast<uint8_t*>(pOutput)[7] =
		reinterpret_cast<const uint8_t*>(&dInput)[0];
}

/*! ************************************

	\struct Burger::NativeEndian
	\brief Loads a 16, 32 or 64 bit value with no byte swapping.

	The classes \ref LittleEndian and \ref BigEndian either map to \ref
	NativeEndian or \ref SwapEndian. If the machine's endian matches the class,
	then it maps to this class.

	This class does nothing for most functions by design. It is meant to vanish
	when the program is asking for no endian swapping since the data being read
	is the same endian as the machine.

	The only functions that do not disappear are the load_unaligned(T*) or
	store_unaligned(T*, T) group of calls since they have the ability to fetch a
	16, 32 or 64 bit value regardless of the alignment of the data pointer.
	These are useful in grabbing data from a byte stream and won't trigger an
	alignment access fault.

	Under most circumstances, you will not call this class directly.

	\sa \ref SwapEndian, \ref LittleEndian and \ref BigEndian

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::load(T Input)
	\brief Load an arithmetic value

	Given either an arithmetic type, or a pointer to an arithmetic type, load
	the value and return it without modification.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is passed to this template, the pointer is assumed to be
		aligned to the data type's granularity.

	\tparam T arithmetic type or pointer to arithmetic type.

	\param Input The value to load from pointer or return as is.

	\return The input value or the loaded value from pointer.

	\sa SwapEndian::load(T)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::store(T* pOutput, T Input)
	\brief Store an arithmetic value

	Given either an arithmetic type and its pointer, store the value without
	modification.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is assumed to be aligned to the data type's granularity.

	\note \ref nullptr is illegal and will page fault.

	\tparam T arithmetic type.

	\param pOutput Pointer to an arithmetic value.
	\param Input The arithmetic value.

	\sa store(volatile T*,T), load(T), SwapEndian::store(T*, T)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::store(volatile T* pOutput, T Input)
	\brief Store an arithmetic value to a volatile variable

	Given either an arithmetic type and its pointer, store the value without
	modification.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is assumed to be aligned to the data type's granularity.

	\note \ref nullptr is illegal and will page fault.

	\tparam T arithmetic type.

	\param pOutput Volatile pointer to an arithmetic value.
	\param Input The arithmetic value.

	\sa store(T*, T), load(T), SwapEndian::store(T*, T)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::load_unaligned(const T* pInput)
	\brief Fetch a value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a value in native endian.

	\tparam T arithmetic type.

	\param pInput Pointer to a value.

	\return The loaded value

	\sa store_unaligned(T*, T), or SwapEndian::load_unaligned(T*)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::store_unaligned(T*, T)
	\brief Store a 16 bit unsigned value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 16 bit value in native endian.

	\tparam T arithmetic type.

	\param pOutput Pointer to a 16 bit value.

	\param Input The 16 bit unsigned value.

	\sa load_unaligned(T*), or SwapEndian::store_unaligned(T*, T)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::fixup(T* pOutput)
	\brief Does nothing.

	Native endian value do not need to be endian swapped, so this function will
	do nothing.

	\tparam T Pointer to a value to not endian swap.

	\param pOutput Pointer to the variable

	\sa SwapEndian::fixup(T*)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::fixup_unaligned(T* pOutput)
	\brief Does nothing.

	The \ref SwapEndian class would swap the endian of a non aligned variable,
	but this class performs no operation since the endian is already a match for
	what the machine expects.

	\tparam T Pointer to a value to not endian swap.

	\param pOutput Pointer to the variable

	\sa SwapEndian::fixup_unaligned(T*)

***************************************/

/*! ************************************

	\struct Burger::SwapEndian
	\brief Swap the byte order of 16, 32 and 64 bit values.

	When reading data from machines that are using different microprocessors,
	the byte order could be reversed. These functions will swap the byte order.

	Whenever possible, these functions will use native CPU endian swap
	instructions for performance.

	\note These functions will always swap the bytes, \ref LittleEndian
	or \ref BigEndian to let the compiler determine whether byte
	swapping is needed.

	\sa \ref NativeEndian, \ref LittleEndian and \ref BigEndian

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::load(T Input)
	\brief Load an arithmetic value endian swapped

	Given either an arithmetic type, or a pointer to an arithmetic type, load
	the value and return it with endian swapped.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is passed to this template, the pointer is assumed to be
		aligned to the data type's granularity.

	\tparam T Arithmetic type or pointer to arithmetic type.
	\param Input The value to load from pointer or return endian swapped

	\return The input value or the loaded value from pointer.

	\sa NativeEndian::load(T)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::store(T* pOutput, T Input)
	\brief Reverse the endian of stored arithmetic value

	Given either an arithmetic type and its pointer, store the value endian
	swapped.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is assumed to be aligned to the data type's granularity.

	\note \ref nullptr is illegal and will page fault.

	\tparam T Arithmetic type

	\param pOutput Pointer to a value to store as endian swapped
	\param Input The arithmetic value to store endian swapped

	\sa store(volatile T*,T), load(T), NativeEndian::store(T*, T)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::store(volatile T* pOutput, T Input)
	\brief Reverse the endian of volatile stored arithmetic value

	Given either an arithmetic type and its pointer, store the value endian
	swapped.

	If a non-arithmetic or pointer to non-arithmetic type is used, this template
	will throw a static assert.

	\note If a pointer is assumed to be aligned to the data type's granularity.

	\note \ref nullptr is illegal and will page fault.

	\tparam T Arithmetic type

	\param pOutput Pointer to a value to store as endian swapped
	\param Input The arithmetic value to store endian swapped

	\sa store(volatile T*,T), load(T), NativeEndian::store(T*, T)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::load_unaligned(const T* pInput)
	\brief Fetch a byte swapped value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a value in swapped endian.

	\tparam T arithmetic type.

	\param pInput Pointer to a value.

	\return The loaded value

	\sa store_unaligned(T*, T), or NativeEndian::load_unaligned(T*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::store_unaligned(T *pOutput,T Input)
	\brief Reverse the endian of unaligned stored arithmetic value

	It assumes that the output pointer is unaligned.

	\note If a pointer is assumed to be aligned to the data type's granularity.

	\note \ref nullptr is illegal and will page fault.

	\tparam T Pointer to a value to endian swap.

	\param pOutput Pointer to a value to store as endian swapped
	\param Input The arithmetic value to store endian swapped

	\sa store(T*,T), NativeEndian::store_unaligned(T*,T)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::fixup(T* pOutput)
	\brief Swap the endian of a variable

	Read in the variable, swap the endian, and then store the result back in the
	original variable.

	\note Single byte variables can't be endian swapped. So no operation is
		performed.

	\tparam T Pointer to a value to endian swap.

	\param pOutput Pointer to the variable to endian swap

	\sa NativeEndian::fixup(T*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::fixup(volatile T* pOutput)
	\brief Swap the endian of a volatile variable

	Read in the variable, swap the endian, and then store the result back in the
	original variable.

	\note Single byte variables can't be endian swapped. So no operation is
		performed.

	\tparam T Pointer to a value to endian swap.

	\param pOutput Pointer to the variable to endian swap

	\sa NativeEndian::fixup(T*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::fixup_unaligned(T* pOutput)
	\brief Swap the endian of a variable that is byte aligned

	Read in the variable, swap the endian, and then store the result back in the
	original variable. This function works with values that are byte aligned.

	\note Single byte variables can't be endian swapped. So no operation is
		performed.

	\tparam T Pointer to a value to endian swap.

	\param pOutput Pointer to the variable to endian swap

	\sa NativeEndian::fixup_unaligned(T*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::fixup_unaligned(volatile T* pOutput)
	\brief Swap the endian of a volatile variable that is byte aligned

	Read in the variable, swap the endian, and then store the result back in the
	original variable. This function works with values that are byte aligned.

	\note Single byte variables can't be endian swapped. So no operation is
		performed.

	\tparam T Pointer to a value to endian swap.

	\param pOutput Pointer to the variable to endian swap

	\sa NativeEndian::fixup_unaligned(T*)

***************************************/

/*! ************************************

	\struct Burger::BigEndian
	\brief Loads a 16, 32 or 64 bit value with byte swapping if needed.

	This class will map to either to \ref NativeEndian or \ref SwapEndian
	depending on if this is a big endian machine or not. Use of this class will
	allow the programmer to write code that is endian neutral since the compiler
	will perform the proper mapping depending on the target's settings..

	Big endian is considered true if the uint32_t value 0x12345678 is stored
	in memory as 0x12, 0x34, 0x56, 0x78.

	Examples of use:
	\code
	uint32_t LoadedInt;

	// Load 4 bytes from a file
	fread(fp,1,4,&LoadedInt);

	// Fetch the big endian data
	uint32_t foo = Burger::BigEndian::load(&LoadedInt);
	\endcode

	\note The documentation will describe the behavior of \ref SwapEndian, be
		warned that it will map to \ref NativeEndian on a big endian machine.

	\sa \ref NativeEndian, \ref LittleEndian and \ref SwapEndian

***************************************/

/*! ************************************

	\struct Burger::LittleEndian
	\brief Loads a 16, 32 or 64 bit value with byte swapping if needed.

	This class will map to either to \ref NativeEndian or \ref SwapEndian
	depending on if this is a little endian machine or not. Use of this class
	will allow the programmer to write code that is endian neutral since the
	compiler will perform the proper mapping depending on the target's settings.

	Little endian is considered true if the uint32_t value 0x12345678 is
	stored in memory as 0x78, 0x56, 0x34, 0x12.

	Examples of use:
	\code
	uint32_t LoadedInt;

	// Load 4 bytes from a file
	fread(fp,1,4,&LoadedInt);

	// Fetch the little endian data
	uint32_t foo = Burger::LittleEndian::load(&LoadedInt);
	\endcode

	\note The documentation will describe the behavior of \ref NativeEndian, be
		warned that it will map to \ref SwapEndian on a big endian machine.

	\sa \ref NativeEndian, \ref BigEndian and \ref SwapEndian

***************************************/

/*! ************************************

	\def BURGER_BIGENDIAN16(x)
	\brief Endian swaps a 16 bit constant on little endian machines.

	If a constant is required to exist as big endian, this macro will perform
	the endian swap at compile time if the machine is little endian.

	\param x 16 bit constant
	\return Constant in big endian format.
	\sa BURGER_BIGENDIAN32() or BURGER_LITTLEENDIAN16()

***************************************/

/*! ************************************

	\def BURGER_BIGENDIAN32(x)
	\brief Endian swaps a 32 bit constant on little endian machines.

	If a constant is required to exist as big endian, this macro will perform
	the endian swap at compile time if the machine is little endian.

	\param x 32 bit constant
	\return Constant in big endian format.
	\sa BURGER_BIGENDIAN16() or BURGER_LITTLEENDIAN32()

***************************************/

/*! ************************************

	\def BURGER_LITTLEENDIAN16(x)
	\brief Endian swaps a 16 bit constant on big endian machines.

	If a constant is required to exist as little endian, this macro will perform
	the endian swap at compile time if the machine is big endian.

	\param x 16 bit constant
	\return Constant in little endian format.
	\sa BURGER_LITTLEENDIAN32() or BURGER_BIGENDIAN16()

***************************************/

/*! ************************************

	\def BURGER_LITTLEENDIAN32(x)
	\brief Endian swaps a 32 bit constant on big endian machines.

	If a constant is required to exist as little endian, this macro will perform
	the endian swap at compile time if the machine is big endian.

	\param x 32 bit constant
	\return Constant in little endian format.
	\sa BURGER_LITTLEENDIAN16() or BURGER_BIGENDIAN32()

***************************************/

/*! ************************************

	\brief Reverse the endian of an array of 16-bit integers

	Given a pointer to an array of 16-bit integers, swap the endian of every
	entry

	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint16_t *,const uint16_t *,uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(
	uint16_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap in place
			SwapEndian::fixup(pInput);
			++pInput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Reverse the endian of a copied array of 16-bit integers

	Given a pointer to an array of 16-bit integers, swap the endian of every
	entry and store the result into another array of equal or greater size

	\param pOutput Pointer to the array to receive the swapped data
	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint16_t *,uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(uint16_t* pOutput, const uint16_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap and copy
			pOutput[0] = SwapEndian::load(pInput);
			++pInput;
			++pOutput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Reverse the endian of an array of 32-bit integers

	Given a pointer to an array of 32-bit integers, swap the endian of every
	entry

	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint32_t *,const uint32_t *,uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(
	uint32_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap in place
			SwapEndian::fixup(pInput);
			++pInput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Reverse the endian of a copied array of 32-bit integers

	Given a pointer to an array of 32-bit integers, swap the endian of every
	entry and store the result into another array of equal or greater size

	\param pOutput Pointer to the array to receive the swapped data
	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint32_t *,uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(uint32_t* pOutput, const uint32_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap and copy
			pOutput[0] = SwapEndian::load(pInput);
			++pInput;
			++pOutput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Reverse the endian of an array of 64-bit integers

	Given a pointer to an array of 64-bit integers, swap the endian of every
	entry

	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint64_t*, const uint64_t *,uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(
	uint64_t* pInput, uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap in place
			SwapEndian::fixup(pInput);
			++pInput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Reverse the endian of a copied array of 64-bit integers

	Given a pointer to an array of 64-bit integers, swap the endian of every
	entry and store the result into another array of equal or greater size

	\param pOutput Pointer to the array to receive the swapped data
	\param pInput Pointer to the array to endian swap
	\param uElements Number of elements in the array (Not bytes)

	\sa swap_endian(uint64_t*, uintptr_t)

***************************************/

void BURGER_API Burger::swap_endian(uint64_t* pOutput, const uint64_t* pInput,
	uintptr_t uElements) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uElements) {
		do {
			// Swap and copy
#if defined(BURGER_64BITCPU)
			pOutput[0] = SwapEndian::load(pInput);
#else
			// For non 64 bit CPUs, do the swap in 32 bit chunks
			uint32_t uLow = SwapEndian::load(
				static_cast<const uint32_t*>(static_cast<const void*>(pInput)));
			uint32_t uHigh = SwapEndian::load(
				static_cast<const uint32_t*>(static_cast<const void*>(pInput)) +
				1);
			static_cast<uint32_t*>(static_cast<void*>(pOutput))[0] = uHigh;
			static_cast<uint32_t*>(static_cast<void*>(pOutput))[1] = uLow;
#endif
			++pInput;
			++pOutput;
		} while (--uElements);
	}
}

/*! ************************************

	\brief Add 128 to every byte to convert a char to a byte or vice versa

	Given a pointer to an array of bytes, add 128 to every entry

	\param pInput Pointer to the array to bytes
	\param uLength Number of bytes in the array

	\sa swap_chars_to_bytes(void*, const void*, uintptr_t)

***************************************/

void BURGER_API Burger::swap_chars_to_bytes(
	void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uLength) {
		do {

			// Flip the high bit
			static_cast<uint8_t*>(pInput)[0] =
				static_cast<uint8_t>(static_cast<uint8_t*>(pInput)[0] ^ 0x80U);

			pInput = static_cast<uint8_t*>(pInput) + 1;
		} while (--uLength);
	}
}

/*! ************************************

	\brief Add 128 to every byte to convert a char to a byte or vice versa

	Given a pointer to an array of bytes, add 128 to every entry and store the
	result into another array of equal or greater size

	\param pOutput Pointer to the array to receive the converted data
	\param pInput Pointer to the array to bytes
	\param uLength Number of bytes in the array

	\sa swap_chars_to_bytes(void*, uintptr_t)

***************************************/

void BURGER_API Burger::swap_chars_to_bytes(
	void* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Any data to process?
	if (uLength) {
		do {

			// Flip the high bit
			static_cast<uint8_t*>(pOutput)[0] = static_cast<uint8_t>(
				static_cast<const uint8_t*>(pInput)[0] ^ 0x80U);

			pInput = static_cast<const uint8_t*>(pInput) + 1;
			pOutput = static_cast<uint8_t*>(pOutput) + 1;
		} while (--uLength);
	}
}
