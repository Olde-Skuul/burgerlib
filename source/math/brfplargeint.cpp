/***************************************

	Large integer parsing class.

	Copyright (c) 2016-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfplargeint.h"
#include "brassert.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::FPLargeInt
	\brief Class for managing a very large integer for double precision.

	This class is useful in parsing high precision numbers and can help in
	decoding floating point numbers for eventual conversion into a string. A
	double has up to 2048+53 bits of precision with half in the integer range
	and the other half in the fractional range. This class handles 1024+64 bits
	to be able to handle the entire fractional or integer range for ultra high
	precision double to ASCII conversion.

	It can perform modulo and multiplication across all bits of precision. Since
	this class was designed for high precision integer to ASCII conversion, the
	two main functions are multiply_return_overflow() to parse out digits from the
	fractional side of a fixed point number and divide_return_remainder() to
	extract digits from the integer side of the fixed point number.

***************************************/

/*! ************************************

	\brief Default constructor

***************************************/

Burger::FPLargeInt::FPLargeInt(void) BURGER_NOEXCEPT
	: m_uEntryCount(0),
	  m_iHighestNonZeroElement(-1),
	  m_iLowestNonZeroElement(kMaxElements),
	  m_bIsZero(TRUE)
{
}

/*! ************************************

	\brief Constructor that sets to a default

	\param uBitsNeeded Number of bits of precision
	\sa init(uint32_t)

***************************************/

Burger::FPLargeInt::FPLargeInt(uint32_t uBitsNeeded) BURGER_NOEXCEPT
{
	init(uBitsNeeded);
}

/*! ************************************

	\fn Burger::FPLargeInt::is_zero(void) const
	\brief Returns \ref TRUE if the value is zero

	If the giant integer's value is zero, return \ref TRUE

	\return \ref TRUE if zero
	\sa is_not_zero(void) const

***************************************/

/*! ************************************

	\fn Burger::FPLargeInt::is_not_zero(void) const
	\brief Returns \ref TRUE if the value is not zero

	If the giant integer's value is not zero, return \ref TRUE

	\return \ref TRUE if not zero
	\sa is_zero(void) const

***************************************/

/*! ************************************

	\brief Initialize the giant integer

	Reset the giant integer to defaults assuming a bit size

	The maximum input value is \ref kTotalBitsInTable -1

	\param uBitsNeeded Number of bits of precision 1 through ( \ref
		kTotalBitsInTable -1)

	\sa FPLargeInt(uint32_t)

***************************************/

void BURGER_API Burger::FPLargeInt::init(uint32_t uBitsNeeded) BURGER_NOEXCEPT
{
	// Test for error
	BURGER_ASSERT(uBitsNeeded && (uBitsNeeded <= kTotalBitsInTable));

	// Round up to the next ChunkSize_t
	const uint32_t uCount =
		(uBitsNeeded + (kBitsPerElement - 1)) / kBitsPerElement;
	m_uEntryCount = uCount;

	// Zero out all elements we'll use
	MemoryClear(m_uDataTable, uCount * sizeof(m_uDataTable[0]));

	// Init the default found range
	m_iHighestNonZeroElement = -1;
	m_iLowestNonZeroElement = kMaxElements;
	m_bIsZero = TRUE;
}

/*! ************************************

	\brief Insert bits into the buffer

	Insert the bits from value into m_uDataTable, shifted in from the end of the
	array by the specified number of bits. A shift of zero or less means that
	none of the bits will be shifted in. A shift of one means that the high bit
	of value will be in the bottom of the last element of m_uDataTable; the
	least significant bit. A shift of \ref kBitsPerElement means that value will
	be in the least significant element of m_uDataTable, and so on. It's
	possible a large value can be shifted out.

	\param uBits 32 bit data to shift in.
	\param uShiftAmount Number of bits to shift (0 to \ref kTotalBitsInTable)
	\sa insert_bits_at_start(uint32_t, uint32_t) or
		insert_chunk_bits(ChunkSize_t, uint32_t)

***************************************/

void BURGER_API Burger::FPLargeInt::insert_bits_at_end(
	uint32_t uBits, uint32_t uShiftAmount) BURGER_NOEXCEPT
{
	// Sanity check
	if (uBits &&
		(uShiftAmount <
			((m_uEntryCount + (32 / kBitsPerElement)) * kBitsPerElement))) {

		// Which bit?
		const uint32_t uShiftForBit = uShiftAmount & (kBitsPerElement - 1);

		// Which data entry
		const uint32_t uShiftForEntry = uShiftAmount / kBitsPerElement;

		// Use an unsigned type so that negative numbers will become large,
		// which makes the range checking below simpler.
		const uint32_t uEntryIndex = (m_uEntryCount - 1) - uShiftForEntry;

		// Write the results to the data array. If the index is too large
		// then that means that the data was shifted off the edge.

		if (!uShiftForBit) {
			// If no shifting, use the fast path

#if defined(BURGER_64BITCPU)
			insert_chunk_bits(static_cast<ChunkSize_t>(uBits), uEntryIndex + 1);
#else
			// 16 bit version need two writes.
			insert_chunk_bits(
				static_cast<ChunkSize_t>(uBits >> 16U), uEntryIndex + 1);
			insert_chunk_bits(static_cast<ChunkSize_t>(uBits), uEntryIndex + 2);
#endif

		} else {

#if defined(BURGER_64BITCPU)
			insert_chunk_bits(static_cast<ChunkSize_t>(
								uBits >> (kBitsPerElement - uShiftForBit)),
				uEntryIndex);
			insert_chunk_bits(static_cast<ChunkSize_t>(uBits << uShiftForBit),
				uEntryIndex + 1);
#else
			// Copy the 32 bits into 3 16 bit entries
			insert_chunk_bits(static_cast<ChunkSize_t>(uBits >>
								((16 + kBitsPerElement) - uShiftForBit)),
				uEntryIndex);
			insert_chunk_bits(static_cast<ChunkSize_t>(
								uBits >> (kBitsPerElement - uShiftForBit)),
				uEntryIndex + 1);
			insert_chunk_bits(static_cast<ChunkSize_t>(uBits << uShiftForBit),
				uEntryIndex + 2);
#endif
		}
	}
}

/*! ************************************

	\brief Insert bits into the buffer from the top

	Insert the bits from value into m_uDataTable, shifted in from the beginning
	of the array by the specified number of bits. A shift of zero or less means
	that none of the bits will be shifted in. A shift of one means that the low
	bit of value will be in the top of the first element of m_uDataTable; the
	most significant bit. A shift of \ref kBitsPerElement means that value will
	be in the most significant element of m_uDataTable, and so on.

	\param uBits 32 bit data to shift in
	\param uShiftAmount Number of bits to shift (0 to \ref kTotalBitsInTable)

	\sa insert_bits_at_end(uint32_t, uint32_t) or
		insert_chunk_bits(ChunkSize_t, uint32_t)

***************************************/

void BURGER_API Burger::FPLargeInt::insert_bits_at_start(
	uint32_t uBits, uint32_t uShiftAmount) BURGER_NOEXCEPT
{
	// Reverse the shift from the top end
	insert_bits_at_end(uBits,
		((m_uEntryCount + (32U / kBitsPerElement)) * kBitsPerElement) -
			uShiftAmount);
}

/*! ************************************

	\brief Do the actual work of inserting bits, and updating zero flag, and
		extents

	\param uBits 32 bit data to shift in
	\param uEntryIndex Index into the entry table
	\sa InsertLowBits(uint32_t, uint32_t) or InsertTopBits(uint32_t, uint32_t)

***************************************/

void BURGER_API Burger::FPLargeInt::insert_chunk_bits(
	ChunkSize_t uBits, uint32_t uEntryIndex) BURGER_NOEXCEPT
{
	// Sanity check
	if (uEntryIndex < m_uEntryCount) {

		// Anything to or?
		if (uBits) {

			// Or in the bits
			m_uDataTable[uEntryIndex] |= uBits;

			// Not zero anymore
			m_bIsZero = FALSE;

			// Set the bounds of the range of set bits
			if (static_cast<int32_t>(uEntryIndex) > m_iHighestNonZeroElement) {
				m_iHighestNonZeroElement = static_cast<int32_t>(uEntryIndex);
			}
			if (static_cast<int32_t>(uEntryIndex) < m_iLowestNonZeroElement) {
				m_iLowestNonZeroElement = static_cast<int32_t>(uEntryIndex);
			}
		}
	}
}

/*! ************************************

	\brief Divide the long number and return the remainder.

	Given a value to perform a modulo, perform the modulo and return the
	remainder and leave the dividend in the data array

	If uDivisor was 0, this function does nothing and returns zero

	\param uDivisor 32 bit unsigned number to divide the long integer

	\return Value from 0 to uDivisor-1.

	\sa multiply_return_overflow(uint32_t)

***************************************/

uint32_t Burger::FPLargeInt::divide_return_remainder(
	uint32_t uDivisor) BURGER_NOEXCEPT
{
	// Preset the result
	uint32_t uRemainder = 0;
	if (!m_bIsZero && uDivisor) {

		// Accumulator for zero test
		ChunkSize_t uBitsForZeroTest = 0;

		// Standard long-division algorithm, scan from the first element to the
		// last.
		uint32_t i = static_cast<uint32_t>(m_iLowestNonZeroElement);
		if (i < m_uEntryCount) {
			do {
				// Perform a high precision divide
				const MathSize_t uDividend =
					(static_cast<MathSize_t>(uRemainder) << kBitsPerElement) +
					m_uDataTable[i];

				const MathSize_t uDivisionResult = uDividend / uDivisor;
				uRemainder = static_cast<ChunkSize_t>(uDividend % uDivisor);

				// Get the new result.
				const ChunkSize_t uDivisionResult32 =
					static_cast<ChunkSize_t>(uDivisionResult);
				m_uDataTable[i] = uDivisionResult32;
				// Accumulate bits for zero test
				uBitsForZeroTest |= uDivisionResult32;
			} while (++i < m_uEntryCount);
		}

		// Update zero status
		m_bIsZero = !uBitsForZeroTest;

		// Did the first 1 bit shift an element to the right? if so, increment
		// starting point
		m_iLowestNonZeroElement += (!m_uDataTable[m_iLowestNonZeroElement]);
	}
	return uRemainder;
}

/*! ************************************

	\brief Multiply by multiplier and return the overflow, from 0 to div-1

	Multiply the giant integer with a number and return the number that
	overflowed out.

	\param uMultiplier Value to multiply the giant integer with

	\return Overflow value, or zero if there was no overflow or the giant
		integer was zero.

	\sa divide_return_remainder(uint32_t)

***************************************/

uint32_t Burger::FPLargeInt::multiply_return_overflow(
	uint32_t uMultiplier) BURGER_NOEXCEPT
{
	// Assume zero for answer
	uint32_t uOverflow = 0;

	// Only multiply if non-zero
	if (!m_bIsZero) {

		ChunkSize_t uBitsForZeroTest = 0;

		// Scan from the highest entry to the first entry.
		for (int32_t i = m_iHighestNonZeroElement; i >= 0; --i) {

			// Perform the multiply using double the bits of precision
			const MathSize_t uMultiplicand =
				(static_cast<MathSize_t>(uMultiplier) * m_uDataTable[i]) +
				uOverflow;

			// Get the bits beyond the precision.
			uOverflow = static_cast<uint32_t>(uMultiplicand >> kBitsPerElement);

			// Grab the result and zero test
			const ChunkSize_t uTrimmed =
				static_cast<ChunkSize_t>(uMultiplicand);
			m_uDataTable[i] = uTrimmed;
			uBitsForZeroTest |= uTrimmed;
		}

		// Update zero status
		m_bIsZero = !uBitsForZeroTest;

		// Did the last 1 bit shift an element to the left? if so, decrement the
		// starting point
		m_iHighestNonZeroElement -= (!m_uDataTable[m_iHighestNonZeroElement]);
	}
	return uOverflow;
}

/*! ************************************

	\brief Separate integer and fractional values from floating point number.

	Given an \ref FPInfo class instance, extract the integer and the fractional
	components of a floating point number and store the values into separate
	\ref FPLargeInt classes.

	\param pIntegerPart Pointer to the output integer record
	\param pFractionalPart Pointer to the output fractional record
	\param pFPInfo Pointer to the input \ref FPInfo record

***************************************/

void BURGER_API Burger::separate_integer_fraction(FPLargeInt* pIntegerPart,
	FPLargeInt* pFractionalPart, const FPInfo* pFPInfo) BURGER_NOEXCEPT
{
	// Better be valid!
	BURGER_ASSERT(pFPInfo->is_valid());

	// Assuming the normal pattern of exponent bias, for n bits of exponent,
	// the range of exponents is:  max exponent = + 2^( n-1 ) -1
	// min exponent = - ( 2^( n-1 ) - 2 )

	// Need 1 + max exponent bits to hold highest value (implied mantissa 1.0
	// bit << max_exponent )
	const uint32_t uNumberIntBitsNeeded = 1U
		<< (pFPInfo->get_exponent_bit_count() - 1);
	pIntegerPart->init(uNumberIntBitsNeeded);

	// Need min exponent bits plus all the binary fraction bits from the
	// mantissa
	const uint32_t uNumberFracBitsNeeded =
		(1U << (pFPInfo->get_exponent_bit_count() - 1)) - 2 +
		(pFPInfo->get_mantissa_bit_count() - 1);
	pFractionalPart->init(uNumberFracBitsNeeded);

	// Integer and fractional have been set to their bit sizes and zeroed out,
	// is there more work to do?

	if (!pFPInfo->is_zero()) {

		// Copy up the lower 32 bits
		const uint32_t uLow32Bits =
			static_cast<uint32_t>(pFPInfo->get_mantissa());
		pIntegerPart->insert_bits_at_end(uLow32Bits,
			(32U + 1U) - pFPInfo->get_mantissa_bit_count() +
				pFPInfo->get_exponent());
		pFractionalPart->insert_bits_at_start(uLow32Bits,
			(pFPInfo->get_mantissa_bit_count() - 1U) - pFPInfo->get_exponent());

		// Double?
		if (pFPInfo->get_mantissa_bit_count() > 32U) {
			const uint32_t uHigh32Bits =
				static_cast<uint32_t>(pFPInfo->get_mantissa() >> 32U);
			pIntegerPart->insert_bits_at_end(uHigh32Bits,
				(64U + 1U) - pFPInfo->get_mantissa_bit_count() +
					pFPInfo->get_exponent());
			pFractionalPart->insert_bits_at_start(uHigh32Bits,
				(pFPInfo->get_mantissa_bit_count() - 1U) -
					pFPInfo->get_exponent() - 32U);
		}
	}
}
