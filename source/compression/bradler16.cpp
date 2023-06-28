/***************************************

	Adler 16 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32 and return a 16 bit version

	This is based on the algorithm provided from Mark Adler in the zlib source
	archive.

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "bradler16.h"

#if !defined(DOXYGEN)
// Note : Do NOT alter these defines or the checksum
// will not be the same as found in deflate/inflate gzip
// archives. This is a bad thing.

// largest prime smaller than 256
#define LARGESTPRIME 251

// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#define LARGESTBLOCK 5802

#endif

/*! ************************************

	\brief Compute the (Mark) Adler-16 checksum

	Create a 16 bit Alder-16 checksum using a 16 bit version the Alder
	algorithm.

	Implemented following the documentation for the 32 bit version found in
	http://en.wikipedia.org/wiki/Adler-32

	The lower 8 bits is a simple additive checksum with a starting value of 1.

	The upper 8 bits is a factorial additive checksum based on the additive
	checksum with a starting value of 0

	\param pInput Pointer to a buffer to be checksummed
	\param uInputLength Number of bytes in the buffer to be checksummed
	\param uAdler16 Alder-16 from previous calculations or one if a new checksum
		is desired

	\return 16 bit Alder-16 checksum of the data

	\sa calc_crc32b(const void *, uintptr_t, uint32_t) or calc_adler32(
		const void *, uintptr_t, uint32_t)

***************************************/

uint_t BURGER_API Burger::calc_adler16(
	const void* pInput, uintptr_t uInputLength, uint_t uAdler16) BURGER_NOEXCEPT
{
	// Anything to process?
	if (pInput && uInputLength) {

		// Get the additive checksum
		uint32_t uAdditive = static_cast<uint8_t>(uAdler16);

		// Get the factorial checksum
		uint32_t uFactorial = static_cast<uint8_t>(uAdler16 >> 8U);
		do {
			// Get the chunk size to process
			uint_t uCount = LARGESTBLOCK;
			if (uInputLength < LARGESTBLOCK) {

				// Truncate to the remainder
				uCount = static_cast<uint_t>(uInputLength);
			}

			// Remove the count from the processed list
			uInputLength -= uCount;
			do {

				// Add to the additive checksum
				uAdditive += static_cast<const uint8_t*>(pInput)[0];
				pInput = static_cast<const uint8_t*>(pInput) + 1;

				// Add the checksum to the factorial
				uFactorial += uAdditive;
			} while (--uCount);

			// Force to fit in a byte
			uAdditive %= LARGESTPRIME;
			uFactorial %= LARGESTPRIME;

			// All done?
		} while (uInputLength);

		// Blend the final 16 bit result
		uAdler16 = static_cast<uint_t>((uFactorial << 8U) + uAdditive);
	}
	return uAdler16;
}
