/***************************************

	Adler32 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and http://tools.ietf.org/html/rfc1950

	This is based on the algorithm provided from Mark Adler
	in the zlib source archive.

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "bradler32.h"

#if !defined(DOXYGEN)
// Note : Do NOT alter these defines or the checksum
// will not be the same as found in deflate/inflate gzip
// archives. This is a bad thing.

#define LARGESTPRIME 65521U // The largest prime smaller than 65536
#define LARGESTBLOCK 5552U	// This is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#endif

/*! ************************************

	\brief Compute the (Mark) Adler-32 checksum

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and http://tools.ietf.org/html/rfc1950

	The lower 16 bits is a simple additive checksum with
	a starting value of 1.

	The upper 16 bits is a factorial additive checksum based on the
	additive checksum with a starting value of 0

	\param pInput Pointer to a buffer to be checksummed
	\param uInputLength Number of bytes in the buffer to be checksummed
	\param uAdler32 Alder-32 from previous calculations or one if a new checksum is desired

	\return 32 bit Alder-32 checksum of the data

	\sa Burger::CalcCRC32B() or Burger::CalcAdler16()

***************************************/

Word32 BURGER_API Burger::CalcAdler32(const void *pInput,WordPtr uInputLength,Word32 uAdler32)
{
	// Any data to process?
	if (pInput && uInputLength) {
		Word32 uAdditive = static_cast<Word16>(uAdler32);	// Get the additive checksum
		uAdler32 = static_cast<Word16>(uAdler32>>16U);	 	// Get the factorial checksum
		do {
			Word uCount = LARGESTBLOCK;						// Assume maximum
			if (uInputLength<LARGESTBLOCK) {				// Not enough
				uCount = static_cast<Word>(uInputLength);	// Use the length
			}
			// Remove the length
			uInputLength -= uCount;
			do {
				// Add to the additive checksum
				uAdditive += static_cast<const Word8 *>(pInput)[0];
				pInput = static_cast<const Word8 *>(pInput)+1;
				// Add the checksum to the factorial
				uAdler32 += uAdditive;
			} while (--uCount);
			uAdditive %= LARGESTPRIME;	// Force to fit in a short
			uAdler32 %= LARGESTPRIME;	// Force to fit in a short
		} while (uInputLength);			// All done?
		uAdler32 = (uAdler32<<16U)+uAdditive;	// Blend
	}
	// Return the result
	return uAdler32;
}
