/***************************************

	Adler32 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and http://tools.ietf.org/html/rfc1950

	This is based on the algorithm provided from Mark Adler in the zlib source
	archive.

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "bradler32.h"

#if !defined(DOXYGEN)
// Note : Do NOT alter these defines or the checksum
// will not be the same as found in deflate/inflate gzip
// archives. This is a bad thing.

// The largest prime smaller than 65536
#define LARGESTPRIME 65521U

// This is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#define LARGESTBLOCK 5552U

#endif

/*! ************************************

	\brief Compute the (Mark) Adler-32 checksum

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and http://tools.ietf.org/html/rfc1950

	The lower 16 bits is a simple additive checksum with a starting value of 1.

	The upper 16 bits is a factorial additive checksum based on the additive
	checksum with a starting value of 0

	\param pInput Pointer to a buffer to be checksummed
	\param uInputLength Number of bytes in the buffer to be checksummed
	\param uAdler32 Alder-32 from previous calculations or one if a new checksum
		is desired

	\return 32 bit Alder-32 checksum of the data

	\sa calc_crc32b(const void *, uintptr_t, uint32_t) or calc_adler16(
		const void *, uintptr_t, uint_t)

***************************************/

uint32_t BURGER_API Burger::calc_adler32(const void* pInput,
	uintptr_t uInputLength, uint32_t uAdler32) BURGER_NOEXCEPT
{
	// Any data to process?
	if (pInput && uInputLength) {

		// Get the additive checksum
		uint32_t uAdditive = static_cast<uint16_t>(uAdler32);

		// Get the factorial checksum
		uAdler32 = static_cast<uint16_t>(uAdler32 >> 16U);

		do {
			// Assume maximum
			uint_t uCount = LARGESTBLOCK;

			// Not enough
			if (uInputLength < LARGESTBLOCK) {
				// Use the length
				uCount = static_cast<uint_t>(uInputLength);
			}

			// Remove the length
			uInputLength -= uCount;
			do {
				// Add to the additive checksum
				uAdditive += static_cast<const uint8_t*>(pInput)[0];
				pInput = static_cast<const uint8_t*>(pInput) + 1;

				// Add the checksum to the factorial
				uAdler32 += uAdditive;
			} while (--uCount);

			// Force to fit in a short
			uAdditive %= LARGESTPRIME;
			uAdler32 %= LARGESTPRIME;

			// All done?
		} while (uInputLength);

		// Blend
		uAdler32 = (uAdler32 << 16U) + uAdditive;
	}
	// Return the result
	return uAdler32;
}
