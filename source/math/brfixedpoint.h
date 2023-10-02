/***************************************

	Fixed point math functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFIXEDPOINT_H__
#define __BRFIXEDPOINT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

/* BEGIN */
#define BURGER_FLOAT_TO_FIXED(x) static_cast<Fixed32>((x)*65536.0f)
#define BURGER_FIXED_TO_FLOAT(x) (static_cast<float>(x) * (1.0f / 65536.0f))
#define BURGER_INT_TO_FIXED(x) static_cast<Fixed32>((x) << 16)
#define BURGER_FIXED_TO_INT(x) \
	static_cast<int>((x + ((x >> 31) & 0xFFFF)) >> 16)

namespace Burger {

/** Minimum value for \ref Frac32 */
BURGER_CONSTEXPR const Frac32 kMinFrac32 = (-0x7FFFFFFF) - 1;

/** Maximum value for \ref Frac32 */
BURGER_CONSTEXPR const Frac32 kMaxFrac32 = 0x7FFFFFFF;

/** Minimum value for \ref Fixed32 */
BURGER_CONSTEXPR const Fixed32 kMinFixed32 = (-0x7FFFFFFF) - 1;

/** Maximum value for \ref Fixed32 */
BURGER_CONSTEXPR const Fixed32 kMaxFixed32 = 0x7FFFFFFF;

/** Pi in \ref Fixed32 format (3.141... * 65536 = 205887.416) */
BURGER_CONSTEXPR const Fixed32 kPiFixed32 = 0x003243F;

BURGER_INLINE uint_t get_low_word(uint32_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(uInput & 0xFFFFU);
}

BURGER_INLINE uint_t get_high_word(uint32_t uInput) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(uInput >> 16U);
}

BURGER_INLINE Fixed32 int_to_fixed(int32_t iInput) BURGER_NOEXCEPT
{
	return static_cast<Fixed32>(iInput << 16);
}

extern Fixed32 BURGER_API int_to_fixed_saturate(int32_t iInput) BURGER_NOEXCEPT;

BURGER_INLINE int32_t fixed_to_int_floor(Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<int32_t>(fInput >> 16);
}

BURGER_INLINE int32_t fixed_to_int(Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<int32_t>((fInput + ((fInput >> 31) & 0xFFFF)) >> 16);
}

BURGER_INLINE int32_t fixed_to_int_ceil(Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<int32_t>((fInput + 0xFFFF) >> 16);
}

BURGER_INLINE int32_t fixed_to_int_nearest(Fixed32 fInput) BURGER_NOEXCEPT
{
	return static_cast<int32_t>(
			   (fInput + 0x8000 - (static_cast<uint32_t>(fInput) >> 31U))) >>
		16;
}

extern int32_t BURGER_API float_to_int_floor(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_int_floor(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT;
extern int32_t BURGER_API float_to_int_ceil(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_int_ceil(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT;
extern int32_t BURGER_API float_to_int_round(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_int_round(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT;
extern int32_t BURGER_API float_to_int_round_to_zero(
	float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_int_round_to_zero(
	int32_t* pOutput, float fInput) BURGER_NOEXCEPT;

extern Fixed32 BURGER_API float_to_fixed_floor(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_fixed_floor(
	Fixed32* pOutput, float fInput) BURGER_NOEXCEPT;
extern Fixed32 BURGER_API float_to_fixed_ceil(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_fixed_ceil(
	Fixed32* pOutput, float fInput) BURGER_NOEXCEPT;
extern Fixed32 BURGER_API float_to_fixed_round(float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_fixed_round(
	Fixed32* pOutput, float fInput) BURGER_NOEXCEPT;
extern Fixed32 BURGER_API float_to_fixed_round_to_zero(
	float fInput) BURGER_NOEXCEPT;
extern void BURGER_API float_to_fixed_round_to_zero(
	Fixed32* pOutput, float fInput) BURGER_NOEXCEPT;

#if defined(BURGER_ARM) || defined(BURGER_AMD64) || \
	(defined(BURGER_X86) && !defined(BURGER_WINDOWS)) || defined(DOXYGEN)
BURGER_INLINE int8_t absolute(int8_t iInput)
{
	if (iInput < 0) {
		iInput = -iInput;
	}
	return iInput;
}
BURGER_INLINE int16_t absolute(int16_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = -iInput;
	}
	return iInput;
}
BURGER_INLINE int32_t absolute(int32_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = -iInput;
	}
	return iInput;
}
BURGER_INLINE int64_t absolute(int64_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = -iInput;
	}
	return iInput;
}
BURGER_INLINE int8_t ClampZero(int8_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = 0;
	}
	return iInput;
}
BURGER_INLINE int16_t ClampZero(int16_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = 0;
	}
	return iInput;
}
BURGER_INLINE int32_t ClampZero(int32_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = 0;
	}
	return iInput;
}
BURGER_INLINE int64_t ClampZero(int64_t iInput) BURGER_NOEXCEPT
{
	if (iInput < 0) {
		iInput = 0;
	}
	return iInput;
}
#else
BURGER_INLINE int8_t absolute(int8_t iInput) BURGER_NOEXCEPT
{
	const int8_t iMask = static_cast<int8_t>(iInput >> 7);
	return static_cast<int8_t>((iInput ^ iMask) - iMask);
}
BURGER_INLINE int16_t absolute(int16_t iInput) BURGER_NOEXCEPT
{
	int16_t iMask = static_cast<int16_t>(iInput >> 15);
	return static_cast<int16_t>((iInput ^ iMask) - iMask);
}
BURGER_INLINE int32_t absolute(int32_t iInput) BURGER_NOEXCEPT
{
	int32_t iMask = (iInput >> 31);
	return (iInput ^ iMask) - iMask;
}
BURGER_INLINE int64_t absolute(int64_t iInput) BURGER_NOEXCEPT
{
	int64_t iMask = (iInput >> 63);
	return (iInput ^ iMask) - iMask;
}
BURGER_INLINE BURGER_CONSTEXPR int8_t ClampZero(int8_t iInput) BURGER_NOEXCEPT
{
	return static_cast<int8_t>((~(iInput >> 7)) & iInput);
}
BURGER_INLINE BURGER_CONSTEXPR int16_t ClampZero(int16_t iInput) BURGER_NOEXCEPT
{
	return static_cast<int16_t>((~(iInput >> 15)) & iInput);
}
BURGER_INLINE BURGER_CONSTEXPR int32_t ClampZero(int32_t iInput) BURGER_NOEXCEPT
{
	return (~(iInput >> 31)) & iInput;
}
BURGER_INLINE BURGER_CONSTEXPR int64_t ClampZero(int64_t iInput) BURGER_NOEXCEPT
{
	return (~(iInput >> 63)) & iInput;
}
#endif

BURGER_INLINE BURGER_CONSTEXPR int8_t get_sign(int8_t iInput) BURGER_NOEXCEPT
{
	return static_cast<int8_t>((iInput >> 7) |
		static_cast<int8_t>(static_cast<uint8_t>(-iInput) >> 7U));
}
BURGER_INLINE BURGER_CONSTEXPR int16_t get_sign(int16_t iInput) BURGER_NOEXCEPT
{
	return static_cast<int16_t>((iInput >> 15) |
		static_cast<int16_t>(static_cast<uint16_t>(-iInput) >> 15U));
}
BURGER_INLINE BURGER_CONSTEXPR int32_t get_sign(int32_t iInput) BURGER_NOEXCEPT
{
	return (iInput >> 31) |
		static_cast<int32_t>(static_cast<uint32_t>(-iInput) >> 31U);
}
BURGER_INLINE BURGER_CONSTEXPR int64_t get_sign(int64_t iInput) BURGER_NOEXCEPT
{
	return (iInput >> 63) |
		static_cast<int64_t>(static_cast<uint64_t>(-iInput) >> 63U);
}

#if defined(BURGER_WATCOM)
BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1, Fixed32 fInput2)
{
	return BurgerFixedMathMultiply(fInput1, fInput2);
}
BURGER_INLINE Fixed32 FixedDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator)
{
	return BurgerFixedMathDivide(fInputNumerator, fInputDenominator);
}
BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput)
{
	return BurgerFixedMathReciprocal(fInput);
}
#elif defined(BURGER_X86) && defined(BURGER_METROWERKS)
BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1, Fixed32 fInput2)
{
	// clang-format off
	asm mov eax, fInput1
	asm imul fInput2
	asm shrd eax, edx, 16
	asm mov fInput1, eax
		// clang-format on
		return fInput1;
}
BURGER_INLINE Fixed32 FixedDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator)
{
	// clang-format off
	asm mov eax, fInputNumerator
	asm mov edx, fInputNumerator
	asm shl eax, 16
	asm sar edx, 16
	asm idiv fInputDenominator
	asm mov fInputNumerator, eax
		// clang-format on
		return fInputNumerator;
}
BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput)
{
	// clang-format off
	asm mov ecx, fInput
	asm cmp ecx, -1
	asm mov eax, 080000000H
	asm je Done
	asm dec eax
	asm cmp ecx, 2
	asm jb Done
	asm xor eax, eax
	asm mov edx, 1
	asm idiv ecx
Done:
	asm mov fInput, eax
							 // clang-format on
							 return fInput;
}
#elif defined(BURGER_X86) && defined(BURGER_MSVC)
BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1, Fixed32 fInput2)
{
	// clang-format off
	__asm mov eax, fInput1
	__asm mov edx, fInput2
	__asm imul edx
	__asm shrd eax, edx, 16
	__asm mov fInput1, eax
		// clang-format on
		return fInput1;
}
BURGER_INLINE Fixed32 FixedDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator)
{
	// clang-format off
	__asm mov eax, fInputNumerator
	__asm mov ebx, fInputDenominator
	__asm mov edx, eax
	__asm shl eax, 16
	__asm sar edx, 16
	__asm idiv ebx
	__asm mov fInputNumerator, eax
		// clang-format on
		return fInputNumerator;
}
BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput)
{
	if (fInput == -1) { // Note: Visual C does not like branches in inline asm
		fInput = 0x80000000;
	} else if (static_cast<uint32_t>(fInput) < 2) {
		fInput = 0x7FFFFFFF;
	} else {
		// clang-format off
		__asm mov ecx, fInput
		__asm xor eax, eax
		__asm mov edx, 1
		__asm idiv ecx
		__asm mov fInput, eax
		// clang-format on
	}
	return fInput;
}
#elif defined(BURGER_PPC) || defined(BURGER_64BITCPU) && !defined(DOXYGEN)
BURGER_INLINE Fixed32 FixedMultiply(Fixed32 fInput1, Fixed32 fInput2)
{
	return static_cast<Fixed32>(
		((static_cast<int64_t>(fInput1) * static_cast<int64_t>(fInput2)) >>
			16) &
		0xFFFFFFFF);
}
BURGER_INLINE Fixed32 FixedDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator)
{
	return static_cast<Fixed32>(
		(static_cast<int64_t>(fInputNumerator) << 16) / fInputDenominator);
}
BURGER_INLINE Fixed32 FixedReciprocal(Fixed32 fInput)
{
	if (fInput == -1) {
		fInput = 0x80000000;
	} else if (static_cast<uint32_t>(fInput) < 2) {
		fInput = 0x7FFFFFFF;
	} else {
		fInput =
			static_cast<Fixed32>(0x100000000LL / static_cast<int64_t>(fInput));
	}
	return fInput;
}
#else
extern Fixed32 BURGER_API FixedMultiply(Fixed32 fInput1, Fixed32 fInput2);
extern Fixed32 BURGER_API FixedDivide(
	Fixed32 fInputNumerator, Fixed32 fInputDenominator);
extern Fixed32 BURGER_API FixedReciprocal(Fixed32 fInput);
#endif

extern uint32_t BURGER_API square_root(uint32_t uInput);
extern uint32_t BURGER_API SqrtFixedToWord32(Fixed32 fInput);
extern Fixed32 BURGER_API square_root(Fixed32 uInput);

#if defined(BURGER_MSVC) || defined(BURGER_WATCOM)
BURGER_INLINE uint32_t RotateLeft(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(_rotl(uInput, uShiftCount));
}
BURGER_INLINE uint32_t RotateRight(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(_rotr(uInput, uShiftCount));
}
#elif (defined(BURGER_METROWERKS) && \
	(defined(BURGER_X86) || defined(BURGER_68K)))
BURGER_INLINE uint32_t RotateLeft(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(__rol(uInput, uShiftCount));
}
BURGER_INLINE uint32_t RotateRight(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(__ror(uInput, uShiftCount));
}
#elif (defined(BURGER_METROWERKS) && defined(BURGER_PPC))
BURGER_INLINE uint32_t RotateLeft(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(
		__builtin___rotate_left32(uInput, static_cast<int>(uShiftCount)));
}
BURGER_INLINE uint32_t RotateRight(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(
		__builtin___rotate_right32(uInput, static_cast<int>(uShiftCount)));
}
#elif defined(BURGER_VITA)
BURGER_INLINE uint32_t RotateLeft(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return __builtin_rol(uInput, static_cast<uint32_t>(uShiftCount));
}
BURGER_INLINE uint32_t RotateRight(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return __builtin_ror(uInput, static_cast<uint32_t>(uShiftCount));
}
#else
BURGER_INLINE uint32_t RotateLeft(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return ((uInput << uShiftCount) | (uInput >> (32 - uShiftCount)));
}
BURGER_INLINE uint32_t RotateRight(
	uint32_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return ((uInput >> uShiftCount) | (uInput << (32 - uShiftCount)));
}
#endif

#if defined(BURGER_MSVC)
BURGER_INLINE uint64_t RotateLeft(
	uint64_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return _rotl64(uInput, uShiftCount);
}
BURGER_INLINE uint64_t RotateRight(
	uint64_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return _rotr64(uInput, uShiftCount);
}
#else
BURGER_INLINE uint64_t RotateLeft(
	uint64_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return ((uInput << uShiftCount) | (uInput >> (64 - uShiftCount)));
}
BURGER_INLINE uint64_t RotateRight(
	uint64_t uInput, uint_t uShiftCount) BURGER_NOEXCEPT
{
	return ((uInput >> uShiftCount) | (uInput << (64 - uShiftCount)));
}
#endif

}
/* END */

#endif
