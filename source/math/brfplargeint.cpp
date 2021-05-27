/***************************************

    Large integer parsing class.

    Copyright (c) 2016-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
    two main functions are MultiplyReturnOverflow() to parse out digits from the
    fractional side of a fixed point number and DivideReturnRemainder() to
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
    \sa Init(uint_t)

***************************************/

Burger::FPLargeInt::FPLargeInt(uint_t uBitsNeeded) BURGER_NOEXCEPT
{
    Init(uBitsNeeded);
}

/*! ************************************

    \fn uint_t Burger::FPLargeInt::IsZero(void) const
    \brief Returns \ref TRUE if the value is zero

    If the giant integer's value is zero, return \ref TRUE

    \return \ref TRUE if zero
    \sa IsNotZero(void) const

***************************************/

/*! ************************************

    \fn uint_t Burger::FPLargeInt::IsNotZero(void) const
    \brief Returns \ref TRUE if the value is not zero

    If the giant integer's value is not zero, return \ref TRUE

    \return \ref TRUE if not zero
    \sa IsZero(void) const

***************************************/

/*! ************************************

    \brief Initialize the giant integer

    Reset the giant integer to defaults assuming a bit size

    The maximum input value is \ref kTotalBitsInTable -1

    \param uBitsNeeded Number of bits of precision 1 through ( \ref
        kTotalBitsInTable -1)

    \sa FPLargeInt(uint_t)

***************************************/

void BURGER_API Burger::FPLargeInt::Init(uint_t uBitsNeeded) BURGER_NOEXCEPT
{
    // Test for error
    BURGER_ASSERT(uBitsNeeded && (uBitsNeeded <= kTotalBitsInTable));

    // Round up to the next ChunkSize_t
    const uint_t uCount =
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
    \sa InsertBitAtStart(uint32_t, uint_t) or InsertChunkBits(ChunkSize_t,
        uint_t)

***************************************/

void BURGER_API Burger::FPLargeInt::InsertBitsAtEnd(
    uint32_t uBits, uint_t uShiftAmount) BURGER_NOEXCEPT
{
    // Sanity check
    if (uBits &&
        (uShiftAmount <
            ((m_uEntryCount + (32 / kBitsPerElement)) * kBitsPerElement))) {

        // Which bit?
        const uint_t uShiftForBit = uShiftAmount & (kBitsPerElement - 1);
        // Which data entry
        const uint_t uShiftForEntry = uShiftAmount / kBitsPerElement;

        // Use an unsigned type so that negative numbers will become large,
        // which makes the range checking below simpler.
        const uint_t uEntryIndex = (m_uEntryCount - 1) - uShiftForEntry;

        // Write the results to the data array. If the index is too large
        // then that means that the data was shifted off the edge.

        if (!uShiftForBit) {
            // If no shifting, use the fast path
#if defined(BURGER_64BITCPU)
            InsertChunkBits(static_cast<ChunkSize_t>(uBits), uEntryIndex + 1);
#else
            // 16 bit version need two writes.
            InsertChunkBits(
                static_cast<ChunkSize_t>(uBits >> 16U), uEntryIndex + 1);
            InsertChunkBits(static_cast<ChunkSize_t>(uBits), uEntryIndex + 2);
#endif
        } else {
#if defined(BURGER_64BITCPU)
            InsertChunkBits(static_cast<ChunkSize_t>(
                                uBits >> (kBitsPerElement - uShiftForBit)),
                uEntryIndex);
            InsertChunkBits(static_cast<ChunkSize_t>(uBits << uShiftForBit),
                uEntryIndex + 1);
#else
            // Copy the 32 bits into 3 16 bit entries
            InsertChunkBits(static_cast<ChunkSize_t>(uBits >>
                                ((16 + kBitsPerElement) - uShiftForBit)),
                uEntryIndex);
            InsertChunkBits(static_cast<ChunkSize_t>(
                                uBits >> (kBitsPerElement - uShiftForBit)),
                uEntryIndex + 1);
            InsertChunkBits(static_cast<ChunkSize_t>(uBits << uShiftForBit),
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
    \sa InsertBitsAtEnd(uint32_t, uint_t) or InsertChunkBits(ChunkSize_t,
        uint_t)

***************************************/

void BURGER_API Burger::FPLargeInt::InsertBitAtStart(
    uint32_t uBits, uint_t uShiftAmount) BURGER_NOEXCEPT
{
    // Reverse the shift from the top end
    InsertBitsAtEnd(uBits,
        ((m_uEntryCount + (32 / kBitsPerElement)) * kBitsPerElement) -
            uShiftAmount);
}

/*! ************************************

    \brief Do the actual work of inserting bits, and updating zero flag, and
        extents

    \param uBits 32 bit data to shift in
    \param uEntryIndex Index into the entry table
    \sa InsertLowBits(uint32_t, uint_t) or InsertTopBits(uint32_t, uint_t)

***************************************/

void BURGER_API Burger::FPLargeInt::InsertChunkBits(
    ChunkSize_t uBits, uint_t uEntryIndex) BURGER_NOEXCEPT
{
    // Sanity check
    if (uEntryIndex < m_uEntryCount) {

        // Anything to or?
        if (uBits) {

            // Or in the bits
            m_uDataTable[uEntryIndex] |= uBits;
            m_bIsZero = FALSE; // Not zero anymore

            // Set the bounds of the range of set bits
            if (static_cast<int>(uEntryIndex) > m_iHighestNonZeroElement) {
                m_iHighestNonZeroElement = static_cast<int>(uEntryIndex);
            }
            if (static_cast<int>(uEntryIndex) < m_iLowestNonZeroElement) {
                m_iLowestNonZeroElement = static_cast<int>(uEntryIndex);
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
    \sa MultiplyReturnOverflow(uint_t)

***************************************/

uint_t Burger::FPLargeInt::DivideReturnRemainder(
    uint_t uDivisor) BURGER_NOEXCEPT
{
    // Preset the result
    uint_t uRemainder = 0;
    if (!m_bIsZero && uDivisor) {

        // Accumulator for zero test
        ChunkSize_t uBitsForZeroTest = 0;

        // Standard long-division algorithm, scan from the first element to the
        // last.
        uint_t i = static_cast<uint_t>(m_iLowestNonZeroElement);
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
    \sa DivideReturnRemainder(uint_t)

***************************************/

uint_t Burger::FPLargeInt::MultiplyReturnOverflow(
    uint_t uMultiplier) BURGER_NOEXCEPT
{
    // Assume zero for answer
    uint_t uOverflow = 0;

    // Only multiply if non-zero
    if (!m_bIsZero) {

        ChunkSize_t uBitsForZeroTest = 0;
        // Scan from the highest entry to the first entry.
        for (int_t i = m_iHighestNonZeroElement; i >= 0; --i) {

            // Perform the multiply using double the bits of precision
            const MathSize_t uMultiplicand =
                (static_cast<MathSize_t>(uMultiplier) * m_uDataTable[i]) +
                uOverflow;

            // Get the bits beyond the precision.
            uOverflow = static_cast<uint_t>(uMultiplicand >> kBitsPerElement);

            // Grab the result and zero test
            const ChunkSize_t uTrimmed =
                static_cast<ChunkSize_t>(uMultiplicand);
            m_uDataTable[i] = uTrimmed;
            uBitsForZeroTest |= uTrimmed;
        }

        // Update zero status
        m_bIsZero = (!uBitsForZeroTest);

        // Did the last 1 bit shift an element to the left? if so, decrement the
        // starting point
        m_iHighestNonZeroElement -= (!m_uDataTable[m_iHighestNonZeroElement]);
    }
    return uOverflow;
}
