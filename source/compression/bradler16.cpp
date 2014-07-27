/***************************************

	Adler 16 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and return a 16 bit version

	This is based on the algorithm provided from Mark Adler
	in the zlib source archive.

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "bradler16.h"

#if !defined(DOXYGEN)
// Note : Do NOT alter these defines or the checksum
// will not be the same as found in deflate/inflate gzip
// archives. This is a bad thing.

#define LARGESTPRIME 251	// largest prime smaller than 256
#define LARGESTBLOCK 5802	// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#endif

/*! ************************************

	\brief Compute the (Mark) Adler-16 checksum

	Create a 16 bit Alder-16 checksum using a 16 bit version
	the Alder algorithm.

	Implemented following the documentation for the 32
	bit version found in
	http://en.wikipedia.org/wiki/Adler-32
	
	The lower 8 bits is a simple additive checksum with
	a starting value of 1.

	The upper 8 bits is a factorial additive checksum based on the
	additive checksum with a starting value of 0

	\param pInput Pointer to a buffer to be checksummed
	\param uInputLength Number of bytes in the buffer to be checksummed
	\param uAdler16 Alder-16 from previous calculations or one if a new checksum is desired

	\return 16 bit Alder-16 checksum of the data

	\sa Burger::CalcCRC32B() or Burger::CalcAdler32()

***************************************/

Word BURGER_API Burger::CalcAdler16(const void *pInput,WordPtr uInputLength,Word uAdler16)
{
	// Anything to process?
	if (pInput && uInputLength) {

		Word32 uAdditive = static_cast<Word8>(uAdler16);		// Get the additive checksum
		Word32 uFactorial = static_cast<Word8>(uAdler16>>8U); 	// Get the factorial checksum
		do {
			// Get the chunk size to process
			Word uCount = LARGESTBLOCK;
			if (uInputLength<LARGESTBLOCK) {
				// Truncate to the remainder
				uCount = static_cast<Word>(uInputLength);
			}
			// Remove the count from the processed list
			uInputLength -= uCount;
			do {
				// Add to the additive checksum
				uAdditive += static_cast<const Word8 *>(pInput)[0];
				pInput = static_cast<const Word8 *>(pInput)+1;
				// Add the checksum to the factorial
				uFactorial += uAdditive;
			} while (--uCount);
			uAdditive %= LARGESTPRIME;		// Force to fit in a byte
			uFactorial %= LARGESTPRIME;		// Force to fit in a byte
		} while (uInputLength);				// All done?
		// Blend the final 16 bit result
		uAdler16 = static_cast<Word>((uFactorial<<8U)+uAdditive);
	}
	return uAdler16;
}

