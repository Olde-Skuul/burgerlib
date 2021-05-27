/***************************************

    UnPackBytes clone from the Apple IIgs

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brunpackbytes.h"

/*! ************************************

	\brief Decompress data compressed with PackBytes from the Apple IIgs

	This routine will decompress data compressed by the algorithm
	used by PackBytes() from the Apple IIgs. This function is almost
	a drop in replacement, with the exception that the handle to
	the source data is replaced with a pointer and remaining bytes.

	The value returned is the number of input bytes consumed, or zero if
	an error occurred.

	\param pInput Pointer to the compressed data
	\param uInputLength Number of bytes of compressed data
	\param ppOutput Pointer to the uint8_t buffer to receive the decompressed data
	\param pOutputLength Pointer to the number of compressed packets to decompress
	\return Zero on error, or number of compressed bytes consumed

***************************************/

uintptr_t BURGER_API Burger::UnpackBytes(const uint8_t *pInput,uintptr_t uInputLength,uint8_t **ppOutput,uintptr_t *pOutputLength)
{
	const uint8_t *pResult = pInput;
	// Check for valid input
	if (pInput && uInputLength && ppOutput && pOutputLength) {

		// Fetch the output buffer pointer
		uint8_t *pOutput = *ppOutput;
		uintptr_t uOutputLength = pOutputLength[0];

		// Check for secondary valid input
		if (pOutput && uOutputLength) {
			do {
				// Grab a token byte
				uint_t uToken = pInput[0];
				++pInput;
				--uInputLength;

				// Get the counter value
				uint_t uCounter = (uToken & 0x3F) + 1;

				// There are 4 difference decompression types
				uToken = (uToken & 0xC0);

				// 0x00 = no compression, raw data
				if (!uToken) {
					// Overrun?
					if (uOutputLength<uCounter) {
						break;		// Error!
					}
					if (uInputLength<uCounter) {
						break;		// Error!
					}
					// Adjust the remainders
					uInputLength-=uCounter;
					uOutputLength-=uCounter;
					do {
						pOutput[0] = pInput[0];
						++pInput;
						++pOutput;
					} while (--uCounter);

				// 0x40 byte fill
				} else if (uToken==0x40) {
					// Overrun?
					if (uOutputLength<uCounter) {
						break;		// Error!
					}
					if (!uInputLength) {
						break;		// Error!
					}
					uint_t uTemp = pInput[0];
					++pInput;
					--uInputLength;
					uOutputLength-=uCounter;
					do {
						pOutput[0] = static_cast<uint8_t>(uTemp);
						++pOutput;
					} while (--uCounter);

				// 0x80 32 bit fill
				} else if (uToken==0x80) {
					// Overrun?
					if (uOutputLength<(uCounter<<2U)) {
						break;		// Error!
					}
					if (uInputLength<4) {
						break;		// Error!
					}
					uint_t uTemp1 = pInput[0];
					uint_t uTemp2 = pInput[1];
					uint_t uTemp3 = pInput[2];
					uint_t uTemp4 = pInput[3];
					pInput+=4;
					uInputLength-=4;
					uOutputLength-=(uCounter<<2U);
					do {
						pOutput[0] = static_cast<uint8_t>(uTemp1);
						pOutput[1] = static_cast<uint8_t>(uTemp2);
						pOutput[2] = static_cast<uint8_t>(uTemp3);
						pOutput[3] = static_cast<uint8_t>(uTemp4);
						pOutput+=4;
					} while (--uCounter);

				// 0xC0 byte fill expanded by a factor of four
				} else {
					// Overrun?
					if (uOutputLength<(uCounter<<2U)) {
						break;		// Error!
					}
					if (!uInputLength) {
						break;		// Error!
					}

					uint_t uTemp = pInput[0];
					++pInput;
					--uInputLength;
					uOutputLength-=(uCounter<<2U);
					do {
						pOutput[0] = static_cast<uint8_t>(uTemp);
						pOutput[1] = static_cast<uint8_t>(uTemp);
						pOutput[2] = static_cast<uint8_t>(uTemp);
						pOutput[3] = static_cast<uint8_t>(uTemp);
						pOutput+=4;
					} while (--uCounter);
				}
			} while (uInputLength);

			// Return the updated output pointers
			ppOutput[0] = pOutput;
			pOutputLength[0] = uOutputLength;
		}
	}
	return static_cast<uintptr_t>(pInput-pResult);
}
