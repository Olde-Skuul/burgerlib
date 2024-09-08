/***************************************

	Power of two and other simple math functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brpoweroftwo.h"
#include "brstructs.h"

// Microsoft C++ doesn't have the clang built ins
#if defined(BURGER_MSVC) && defined(BURGER_ARM)
#include <intrin.h>
#define __builtin_clz _CountLeadingZeros
#define __builtin_clzll _CountLeadingZeros64
#endif

/*! ************************************

	\brief Round up an integer to the nearest power of 2

	Take an arbitrary value and round it up to the nearest power of 2 If the
	input is 0x40000001 to 0x7FFFFFFF, the function returns 0x80000000
	0x80000000-0xFFFFFFFF is an overflow and returns zero. Zero will return zero

	\param uInput Integer value to round up

	\return Rounded up value

	\sa power_of_two(uint64_t), power_of_two(float), power_of_two(double)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))

// Assembly functions

#elif (defined(BURGER_MSVC) && defined(BURGER_AMD64)) || \
	((defined(BURGER_GNUC) || defined(BURGER_CLANG)) && defined(BURGER_INTEL))

uint32_t BURGER_API Burger::power_of_two(uint32_t uInput) BURGER_NOEXCEPT
{
	// Use the Intel instruction Bit Scan Reverse to
	// speed up the bit search

	// Force 0x4000 to be 0x3fff so in the end
	// it's changed back to 0x4000
	--uInput;

	// Find the LEFTMOST bit, uInput will have 0x00 to 0x1F
	// Zero will set the zero flag and leave uInput undefined
	unsigned long uResult;
	const uint32_t uZero = _BitScanReverse(&uResult, uInput);

	// This handles all but the 0 case (0x1 is input)
	// In the zero case, uZero is 0, so the result is zero
	uResult = uZero << uResult;

	// I'm one bit shy, so shift up one bit and use
	// an add for Intel efficiency
	// and finally, for the zero case, or with 1
	// if the input was 1 (1-1) -> 0, so, uZero^1 = 1+0 = 1
	return static_cast<uint32_t>(uResult + uResult + (uZero ^ 1));
}

// PowerPC version
#elif (defined(BURGER_MSVC) || defined(BURGER_METROWERKS) || \
	defined(BURGER_SNSYSTEMS) || defined(BURGER_GHS) || \
	defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_PPC)

uint32_t BURGER_API Burger::power_of_two(uint32_t uInput) BURGER_NOEXCEPT
{
	--uInput;
	// Count leading zeros (Reverse the count)
	uint32_t uReverseCount = __cntlzw(uInput);

	// Test for 0 (uReverseCount == 32)
	uint32_t uMask = uReverseCount >> 5U;

	// Convert to value to shift left
	uReverseCount = 32U - uReverseCount;

	// Add in 1 in case of uReverseCount == 32
	return ((uMask ^ 1U) << uReverseCount) + uMask;
}

#elif defined(BURGER_ARM)

uint32_t BURGER_API Burger::power_of_two(uint32_t uInput) BURGER_NOEXCEPT
{
	--uInput;

	// Count leading zeros (Reverse the count)
	uint32_t uReverseCount = __builtin_clz(uInput);

	// Arm cannot shift 32 bits, so early out with the
	// error value of zero
	if (!uReverseCount) {
		return uReverseCount;
	}

	// Test for 0
	uint32_t uMask = uReverseCount >> 5U;

	// Convert to value to shift left
	uReverseCount = 32U - uReverseCount;

	// Add in 1 in case ofuReverseCount == 32
	return ((uMask ^ 1) << uReverseCount) + uMask;
}

#else

uint32_t BURGER_API Burger::power_of_two(uint32_t uInput) BURGER_NOEXCEPT
{
	// To do this, I blend the bits so from the highest will set
	// EVERY bit slot to the right.
	// I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
	// So, by adding one, it will be the next power of 2
	// Since 0 becomes 0xFFFFFFFF at the start, all
	// the or's and shifts do nothing and 0x00 will be
	// returned due to overflow

	--uInput;                // Adjust the input
	uInput |= uInput >> 1U;  // Blend in the odd bits.
	uInput |= uInput >> 2U;  // Now, every 2nd bit
	uInput |= uInput >> 4U;  // Every 4th bit
	uInput |= uInput >> 8U;  // Every 8th bit
	uInput |= uInput >> 16U; // Final pass!
	++uInput;                // Inc and I have the power of 2
	return uInput;
}

#endif

/*! ************************************

	\brief Round up an integer to the nearest power of 2

	Take an arbitrary value and round it up to the nearest power of 2
	If the input is 0x4000000000000001 to 0x7FFFFFFFFFFFFFFF, the function
	returns 0x8000000000000000 0x8000000000000000-0xFFFFFFFFFFFFFFFF is an
	overflow and returns zero. Zero will return zero

	\param uInput Integer value to round up

	\return Rounded up value

	\sa power_of_two(uint32_t), power_of_two(float), or power_of_two(double)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_68K))) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86))

// Assembly functions

// PowerPC version
#elif (defined(BURGER_MSVC) || defined(BURGER_SNSYSTEMS) || \
	defined(BURGER_GNUC) || defined(BURGER_CLANG)) && \
	defined(BURGER_PPC64)

uint64_t BURGER_API Burger::power_of_two(uint64_t uInput) BURGER_NOEXCEPT
{
	--uInput;
	// Count leading zeros (Reverse the count)
	uint64_t uReverseCount = __cntlzd(uInput);

	// Test for 0 (uReverseCount == 64)
	uint64_t uMask = uReverseCount >> 6U;

	// Convert to value to shift left
	uReverseCount = 64U - uReverseCount;

	// Add in 1 in case of uReverseCount == 64
	return ((uMask ^ 1U) << uReverseCount) + uMask;
}

#elif defined(BURGER_ARM64)

uint64_t BURGER_API Burger::power_of_two(uint64_t uInput) BURGER_NOEXCEPT
{
	--uInput;

	// Count leading zeros (Reverse the count)
	uint64_t uReverseCount = __builtin_clzll(uInput);

	// Arm cannot shift 64 bits, so early out with the
	// error value of zero
	if (!uReverseCount) {
		return uReverseCount;
	}

	// Test for 0
	uint64_t uMask = uReverseCount >> 6U;

	// Convert to value to shift left
	uReverseCount = 64U - uReverseCount;

	// Add in 1 in case ofuReverseCount == 64
	return ((uMask ^ 1) << uReverseCount) + uMask;
}

#else
uint64_t BURGER_API Burger::power_of_two(uint64_t uInput) BURGER_NOEXCEPT
{
	// To do this, I blend the bits so from the highest will set
	// EVERY bit slot to the right.
	// I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
	// So, by adding one, it will be the next power of 2
	// Since 0 becomes 0xFFFFFFFF at the start, all
	// the or's and shifts do nothing and 0x00 will be
	// returned due to overflow

	--uInput;
	uInput |= uInput >> 1;  // Blend in the odd bits.
	uInput |= uInput >> 2;  // Now, every 2nd bit
	uInput |= uInput >> 4;  // Every 4th bit
	uInput |= uInput >> 8;  // Every 8th bit
	uInput |= uInput >> 16; // Final pass!
	uInput |= uInput >> 32; // Final pass!
	++uInput;               // Inc and I have the power of 2
	return uInput;
}

#endif

/*! ************************************

	\brief Round up a 32 bit float to the nearest power of 2

	Take an arbitrary value and round it up to the nearest power of 2.

	If the input is 5 to 7, the function returns 8. NaN, infinity, or numbers
	that are already powers of two are returned unchanged. Zero will return NaN.

	\param fInput Float value to round up

	\return Rounded up value

	\sa power_of_two(uint32_t), power_of_two(uint64_t), or power_of_two(double)

***************************************/

float BURGER_API Burger::power_of_two(float fInput) BURGER_NOEXCEPT
{
	// Get the binary value of the float
	uint32_float_t Temp;
	Temp.f = fInput;
	uint32_t uTemp = Temp.w;

	// If it's a zero, or negative, return NaN
	if (!uTemp || (uTemp & 0x80000000U)) {
		return g_fNan.f;
	}

	// If positive infinity, NaN, or already a power of two, return that value
	if (uTemp >= 0x7F800000U || !(uTemp & 0x007FFFFFU)) {
		return fInput;
	}

	// Hack off the fraction
	uTemp &= 0xFF800000U;

	// Increase the exponent to get the next power of two
	uTemp += 0x00800000U;

	// Return the value as a float
	Temp.w = uTemp;
	return Temp.f;
}

/*! ************************************

	\brief Round up a 64 bit float to the nearest power of 2

	Take an arbitrary value and round it up to the nearest power of 2.

	If the input is 5 to 7, the function returns 8. NaN, infinity, or numbers
	that are already powers of two are returned unchanged. Zero will return NaN.

	\param dInput Float value to round up

	\return Rounded up value

	\sa power_of_two(uint32_t), power_of_two(uint64_t), or power_of_two(float)

***************************************/

double BURGER_API Burger::power_of_two(double dInput) BURGER_NOEXCEPT
{
	// Get the binary value of the float
	uint64_double_t Temp;
	Temp.d = dInput;
	uint32_t uTemp = Temp.w32[BURGER_ENDIANINDEX_HIGH];

	// If it's a zero, or negative, return NaN
	if (!Temp.w || (uTemp & 0x80000000U)) {
		return g_dNan.d;
	}

	// If positive infinity, NaN, or already a power of two, return that value
	if (uTemp >= 0x7FF00000U ||
		!((uTemp & 0x000FFFFFU) || Temp.w32[BURGER_ENDIANINDEX_LOW])) {
		return dInput;
	}

	// Hack off the fraction
	uTemp &= 0xFFF00000U;

	// Increase the exponent to get the next power of two
	uTemp += 0x00100000U;

	// Return the value as a float
	Temp.w32[BURGER_ENDIANINDEX_HIGH] = uTemp;
	Temp.w32[BURGER_ENDIANINDEX_LOW] = 0;
	return Temp.d;
}

/*! ************************************

	\fn Burger::power_of_two(T input)
	\brief Round up an integer to the nearest power of 2

	A template to take an arbitrary value and round it up to the nearest power
	of 2. Zero will return zero.

	\note This will intentionally fail on any non-integral or signed data type.

	\tparam T Input data type, tested with is_unsigned.
	\param input Integer value to round up

	\return Rounded up value

	\sa power_of_two(uint32_t), power_of_two(uint64_t), power_of_two(float)

***************************************/

/*! ************************************

	\brief Convert a BCD value into a decimal integer.

	Given an input value of 0x1234, assume it is Binary Coded Decimal (BCD)
	format and convert it to become the decimal integer 1234.

	BCD's encoding rules dictate that only the number 0-9 are valid. Any use of
	the hex digits A-F are considered illegal.

	\note The returned value is undefined if the input is not in BCD format.

	\param uInput 32-bit hex value in BCD.

	\return Value in decimal format.

	\sa convert_to_BCD(uint32_t)

***************************************/

uint32_t BURGER_API Burger::convert_from_BCD(uint32_t uInput) BURGER_NOEXCEPT
{
	// Assume nothing
	uint32_t uOutput = 0;

	// Default multiplier
	uint32_t uMul = 1;
	do {
		// Parse the lowest digit
		uOutput = ((uInput & 0xFU) * uMul) + uOutput;

		// Shift out the nibble
		uInput >>= 4U;

		// Adjust the multiplier
		uMul = uMul * 10U;

		// Any more?
	} while (uInput);
	return uOutput;
}

/*! ************************************

	\brief Convert a decimal integer into BCD format.

	Given a decimal in the range of 0-99999999, convert it into that value into
	Binary Coded Decimal (BCD) format. Such that 1234 become 0x1234 and 50331
	become 0x00050331. If the input value is greater than 99999999 it is clamped
	to 99999999.

	\param uInput Integer value in the range of 0 to 99999999.

	\return Input encoded in BCD.

	\sa convert_from_BCD(uint32_t)

***************************************/

uint32_t BURGER_API Burger::convert_to_BCD(uint32_t uInput) BURGER_NOEXCEPT
{
	// Assume 99999999 in BCD
	uint32_t uOutput = 0x99999999;

	// Too large?
	if (uInput < 99999999) {
		uint32_t uTemp;

		// Init output
		uOutput = 0;

		// Because divides cost so much, Try to skip them with branches
		if (uInput >= 1000000U) {
			uTemp = uInput / 10000000U;
			uOutput = uTemp << 28U;
			uInput -= uTemp * 10000000U;
			uTemp = uInput / 1000000U;
			uOutput |= uTemp << 24U;
			uInput -= uTemp * 1000000U;
		}

		if (uInput >= 10000U) {
			uTemp = uInput / 100000U;
			uOutput |= uTemp << 20U;
			uInput -= uTemp * 100000U;
			uTemp = uInput / 10000U;
			uOutput |= uTemp << 16U;
			uInput -= uTemp * 10000U;
		}

		if (uInput >= 100U) {
			uTemp = uInput / 1000U;
			uOutput |= uTemp << 12U;
			uInput -= uTemp * 1000U;
			uTemp = uInput / 100U;
			uOutput |= uTemp << 8U;
			uInput -= uTemp * 100U;
		}

		// Just do the last two digits.
		uTemp = uInput / 10U;
		uOutput |= uTemp << 4U;
		uInput -= uTemp * 10U;

		uOutput |= uInput;
	}
	return uOutput;
}

/*! ************************************

	\brief Reverse the bits in a byte

	Given a bit width (From 1 through 32), reverse the order of the bits within.

	Since this version is variable width, if uBitLength is equal to 4, the truth
	table would look like this...

	| Input | Output |
	| :---: | :---: |
	| 0x00 | 0x00 |
	| 0x01 | 0x02 |
	| 0x02 | 0x01 |
	| 0x03 | 0x03 |

	Whereas if uBitLength is equal to 8, the truth table would look like this...

	| Input | Output |
	| :---: | :---: |
	| 0x00 | 0x00 |
	| 0x01 | 0x00 |
	| 0x02 | 0x40 |
	| 0x03 | 0xC0 |

	\note This function will set the unused bits to zero, so if a 3 bit number
	is requested, the upper 29 (32-3) bits will be set to zero.

	\param uInput Value to bit reverse
	\param uBitLength Width of the value to reverse (1 through 32)

	\return uInput with the bits reversed

	\sa bit_reverse(uint64_t,uint_t)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_68K)))
// In assembly

#else

uint32_t BURGER_API Burger::bit_reverse(
	uint32_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
{
	// Initialize the result
	uint32_t uResult = 0;
	do {
		uResult = uResult + uResult;
		uResult += (uInput & 1U);
		uInput >>= 1U;
	} while (--uBitLength);
	return uResult;
}
#endif

/*! ************************************

	\brief Reverse the bits in a byte

	Given a bit width (From 1 through 64), reverse the order of the bits within.

	Since this version is variable width, if uBitLength is equal to 4, the truth
	table would look like this...

	| Input | Output |
	| :---: | :---: |
	| 0x00 | 0x00 |
	| 0x01 | 0x02 |
	| 0x02 | 0x01 |
	| 0x03 | 0x03 |

	Whereas if uBitLength is equal to 8, the truth table would look like this...

	| Input | Output |
	| :---: | :---: |
	| 0x00 | 0x00 |
	| 0x01 | 0x80 |
	| 0x02 | 0x40 |
	| 0x03 | 0xC0 |

	\note This function will set the unused bits to zero, so if	a 3 bit number
		is requested, the upper 61 (64-3) bits will be set to zero.

	\param uInput Value to bit reverse
	\param uBitLength Width of the value to reverse (1 through 64)

	\return uInput with the bits reversed

	\sa bit_reverse(uint32_t,uint_t)

***************************************/

#if (defined(BURGER_WATCOM) && defined(BURGER_X86)) || \
	(defined(BURGER_MSVC) && defined(BURGER_X86)) || \
	(defined(BURGER_METROWERKS) && \
		(defined(BURGER_X86) || defined(BURGER_68K)))
// In assembly

#else
uint64_t BURGER_API Burger::bit_reverse(
	uint64_t uInput, uint_t uBitLength) BURGER_NOEXCEPT
{
	// Initialize the result
	uint64_t uResult = 0;
	do {
		uResult = uResult + uResult;
		uResult += (uInput & 1U);
		uInput >>= 1U;
	} while (--uBitLength);
	return uResult;
}
#endif

/*! ************************************

	\brief Calculate the number of set bits

	Given a 32 bit integer, count the number of bits set and return the value
	from zero to thirty two.

	\param uInput Integer to count the bits from
	\return 0 to 32
	\sa count_set_bits(uint64_t)

***************************************/

uint_t BURGER_API Burger::count_set_bits(uint32_t uInput) BURGER_NOEXCEPT
{
	// Use vector adding to count the bits
	// Stage 1, add 16 pairs of 1 bit numbers
	uInput = uInput - ((uInput >> 1) & 0x55555555U);

	// Stage 2, add 8 pairs of 2 bit numbers
	uInput = (uInput & 0x33333333U) + ((uInput >> 2U) & 0x33333333U);

	// Stage 3, add 4 pairs of 4 bit numbers and then sum
	// them with a vector multiply in which the upper 8 bits is the count
	// Neat, eh?
	return (((uInput + (uInput >> 4U)) & 0x0F0F0F0FU) * 0x01010101U) >> 24U;
}

/*! ************************************

	\brief Calculate the number of set bits

	Given a 64 bit integer, count the number of bits set and return the value
	from zero to sixty four.

	\param uInput Integer to count the bits from
	\return 0 to 64
	\sa count_set_bits(uint32_t)

***************************************/

uint_t BURGER_API Burger::count_set_bits(uint64_t uInput) BURGER_NOEXCEPT
{
	// If the CPU is a 64 bit one, do it the fast way
#if defined(BURGER_64BITCPU) && !defined(BURGER_METROWERKS)
	// Use vector adding to count the bits
	// Stage 1, add 16 pairs of 1 bit numbers
	uInput = uInput - ((uInput >> 1) & 0x5555555555555555ULL);

	// Stage 2, add 8 pairs of 2 bit numbers
	uInput = (uInput & 0x3333333333333333ULL) +
		((uInput >> 2U) & 0x3333333333333333ULL);

	// Stage 3, add 4 pairs of 4 bit numbers and then sum
	// them with a vector multiply in which the upper 8 bits is the count
	// Neat, eh?
	uInput = (((uInput + (uInput >> 4U)) & 0x0F0F0F0F0F0F0F0FULL) *
				 0x0101010101010101ULL) >>
		56U;
	return static_cast<uint_t>(uInput);
#else
	// For 32 bit CPUs, break it into two 32 bit chunks to make the code simpler
	// (Note, the 64 bit code breaks on CodeWarrior in debugging mode due to a
	// broken 64 bit multiply call)
	return count_set_bits(static_cast<uint32_t>(uInput)) +
		count_set_bits(static_cast<uint32_t>(uInput >> 32U));
#endif
}
