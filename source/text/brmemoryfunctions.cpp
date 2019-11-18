/***************************************

	Simple memory functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemoryfunctions.h"

/*! ************************************

	\brief Copy raw memory from one buffer to another.

	Given a pointer to a byte stream, copy it to a destination buffer using a
	supplied input length.

	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the raw data to copy from.
	\param uCount Size in bytes of the data to copy.

	\note This function will not check if the output buffer is not large enough
		for the input. Caution must be used with this function.

	\note This function uses large chuck data copies, so the input and output
		pointers must point to separate buffers. If they are overlapping
		buffers, the results are undefined.

	\sa MemoryMove(), MemoryFill() and MemoryClear()

***************************************/

#if defined(BURGER_PPC)

/***************************************

	PowerPC optimized version, Must have LOW overhead

	This routine has been VTuned. Don't touch it!!!!!

	Why do I do it this way? doubles on ppc 601's will copy fast under any
	circumstances but the 603 and 604 will cause a processor exception if the
	data is not longword aligned and incurs a 440 cycle penalty PER instruction.

	Pathetic.

***************************************/

void BURGER_API Burger::MemoryCopy(
	void* pOutput, const void* pInput, WordPtr uCount)
{
	if (reinterpret_cast<WordPtr>(pInput) &
		3) {			  // Is the source pointer not long word aligned?
		goto AlignSource; // Align the source
	}
SourceOk:
	// Is the destination aligned?
	if (!(reinterpret_cast<WordPtr>(pOutput) & 3)) {
		// The pointers are double aligned!
		WordPtr uWordCount = uCount >> 5; // double copying (32 byte runs)
		if (uWordCount) {				  // Any longwords?
			do {
				double DTemp = static_cast<const double*>(pInput)[0];
				double DTemp2 = static_cast<const double*>(pInput)[1];
				static_cast<double*>(pOutput)[0] = DTemp;
				static_cast<double*>(pOutput)[1] = DTemp2;
				DTemp = static_cast<const double*>(pInput)[2];
				DTemp2 = static_cast<const double*>(pInput)[3];
				static_cast<double*>(pOutput)[2] = DTemp;
				static_cast<double*>(pOutput)[3] = DTemp2;
				pOutput = static_cast<Word8*>(pOutput) + 32;
				pInput = static_cast<const Word8*>(pInput) + 32;
			} while (--uWordCount);
		}
		if (uCount & 16) { /* 16 byte run? */
			double DTemp = static_cast<const double*>(pInput)[0];
			double DTemp2 = static_cast<const double*>(pInput)[1];
			static_cast<double*>(pOutput)[0] = DTemp;
			static_cast<double*>(pOutput)[1] = DTemp2;
			pOutput = static_cast<Word8*>(pOutput) + 16;
			pInput = static_cast<const Word8*>(pInput) + 16;
		}
		if (uCount & 8) { /* 8 byte run? */
			static_cast<double*>(pOutput)[0] =
				static_cast<const double*>(pInput)[0];
			pOutput = static_cast<Word8*>(pOutput) + 8;
			pInput = static_cast<const Word8*>(pInput) + 8;
		}
		if (uCount & 4) { /* 4 byte run? */
			static_cast<Word32*>(pOutput)[0] =
				static_cast<const Word32*>(pInput)[0];
			pOutput = static_cast<Word8*>(pOutput) + 4;
			pInput = static_cast<const Word8*>(pInput) + 4;
		}
		if (uCount & 2) { /* 2 byte run? */
			static_cast<Word16*>(pOutput)[0] =
				static_cast<const Word16*>(pInput)[0];
			pOutput = static_cast<Word8*>(pOutput) + 2;
			pInput = static_cast<const Word8*>(pInput) + 2;
		}
		if (uCount & 1) { /* 1 byte left? */
			static_cast<Word8*>(pOutput)[0] =
				static_cast<const Word8*>(pInput)[0];
		}
		return; // Done!
	}
	// The pointers are NOT longword aligned!
	{
		// Longword copying
		WordPtr uWordCount = uCount >> 4;
		if (uWordCount) { // Any longwords?
			do {
				Word32 LTemp = static_cast<const Word32*>(pInput)[0];
				Word32 LTemp2 = static_cast<const Word32*>(pInput)[1];
				static_cast<Word32*>(pOutput)[0] = LTemp;
				static_cast<Word32*>(pOutput)[1] = LTemp2;
				LTemp = static_cast<const Word32*>(pInput)[2];
				LTemp2 = static_cast<const Word32*>(pInput)[3];
				static_cast<Word32*>(pOutput)[2] = LTemp;
				static_cast<Word32*>(pOutput)[3] = LTemp2;
				pOutput = static_cast<Word8*>(pOutput) + 16;
				pInput = static_cast<const Word8*>(pInput) + 16;
			} while (--uWordCount);
		}
	}
	if (uCount & 8) {
		Word32 LTemp = static_cast<const Word32*>(pInput)[0];
		Word32 LTemp2 = static_cast<const Word32*>(pInput)[1];
		static_cast<Word32*>(pOutput)[0] = LTemp;
		static_cast<Word32*>(pOutput)[1] = LTemp2;
		pOutput = static_cast<Word8*>(pOutput) + 8;
		pInput = static_cast<const Word8*>(pInput) + 8;
	}
	if (uCount & 4) {
		static_cast<Word32*>(pOutput)[0] =
			static_cast<const Word32*>(pInput)[0];
		pOutput = static_cast<Word8*>(pOutput) + 4;
		pInput = static_cast<const Word8*>(pInput) + 4;
	}
	if (uCount & 2) {
		static_cast<Word16*>(pOutput)[0] =
			static_cast<const Word16*>(pInput)[0];
		pOutput = static_cast<Word8*>(pOutput) + 2;
		pInput = static_cast<const Word8*>(pInput) + 2;
	}
	if (uCount & 1) {
		static_cast<Word8*>(pOutput)[0] = static_cast<const Word8*>(pInput)[0];
	}
	return;

	// Source is not longword aligned, align it

AlignSource:
	if ((reinterpret_cast<WordPtr>(pInput) & 1) && uCount) {
		// Move a single byte to short align the data
		static_cast<Word8*>(pOutput)[0] = static_cast<const Word8*>(pInput)[0];
		pInput = static_cast<const Word8*>(pInput) + 1;
		pOutput = static_cast<Word8*>(pOutput) + 1;
		--uCount; // Remove from the count
	}
	if ((reinterpret_cast<WordPtr>(pInput) & 2) && uCount >= 2) {
		// Move a short to long align the data
		static_cast<Word16*>(pOutput)[0] =
			static_cast<const Word16*>(pInput)[0];
		pInput = static_cast<const Word8*>(pInput) + 2;
		pOutput = static_cast<Word8*>(pOutput) + 2;
		uCount -= 2; // Remove from the count
	}
	goto SourceOk;
}

#else

//
// Generic version assuming a 32 bit CPU
//

void BURGER_API Burger::MemoryCopy(
	void* pOutput, const void* pInput, WordPtr uCount)
{
	// Do anything?
	if (uCount && pOutput) {

		if ((reinterpret_cast<WordPtr>(pInput) ^
				reinterpret_cast<WordPtr>(pOutput)) &
			3) {
			// Data is not aligned, copy the slow way
			do {
				static_cast<Word8*>(pOutput)[0] =
					static_cast<const Word8*>(pInput)[0];
				pInput = static_cast<const Word8*>(pInput) + 1;
				pOutput = static_cast<Word8*>(pOutput) + 1;
			} while (--uCount);
		} else {

			// Is the source pointer not long word aligned?
			if (reinterpret_cast<WordPtr>(pInput) & 3) {
				// Align the source
				if (reinterpret_cast<WordPtr>(pInput) & 1) {
					// Move a single byte to be short aligned
					static_cast<Word8*>(pOutput)[0] =
						static_cast<const Word8*>(pInput)[0];
					pInput = static_cast<const Word8*>(pInput) + 1;
					pOutput = static_cast<Word8*>(pOutput) + 1;
					// Remove from the count
					--uCount;
				}
				if ((reinterpret_cast<WordPtr>(pInput) & 2) && (uCount >= 2)) {
					// Move a short to be 32 bit aligned
					static_cast<Word16*>(pOutput)[0] =
						static_cast<const Word16*>(pInput)[0];
					pInput = static_cast<const Word8*>(pInput) + 2;
					pOutput = static_cast<Word8*>(pOutput) + 2;
					// Remove from the count
					uCount -= 2;
				}
			}

			// Longword copying
			WordPtr uWordCount = uCount >> 4;
			// Any longwords?
			if (uWordCount) {
				do {
					Word32 LTemp = static_cast<const Word32*>(pInput)[0];
					Word32 LTemp2 = static_cast<const Word32*>(pInput)[1];
					static_cast<Word32*>(pOutput)[0] = LTemp;
					static_cast<Word32*>(pOutput)[1] = LTemp2;
					LTemp = static_cast<const Word32*>(pInput)[2];
					LTemp2 = static_cast<const Word32*>(pInput)[3];
					static_cast<Word32*>(pOutput)[2] = LTemp;
					static_cast<Word32*>(pOutput)[3] = LTemp2;
					pOutput = static_cast<Word8*>(pOutput) + 16;
					pInput = static_cast<const Word8*>(pInput) + 16;
				} while (--uWordCount);
			}
			if (uCount & 8) {
				Word32 LTemp = static_cast<const Word32*>(pInput)[0];
				Word32 LTemp2 = static_cast<const Word32*>(pInput)[1];
				static_cast<Word32*>(pOutput)[0] = LTemp;
				static_cast<Word32*>(pOutput)[1] = LTemp2;
				pOutput = static_cast<Word8*>(pOutput) + 8;
				pInput = static_cast<const Word8*>(pInput) + 8;
			}
			if (uCount & 4) {
				static_cast<Word32*>(pOutput)[0] =
					static_cast<const Word32*>(pInput)[0];
				pOutput = static_cast<Word8*>(pOutput) + 4;
				pInput = static_cast<const Word8*>(pInput) + 4;
			}
			if (uCount & 2) {
				static_cast<Word16*>(pOutput)[0] =
					static_cast<const Word16*>(pInput)[0];
				pOutput = static_cast<Word8*>(pOutput) + 2;
				pInput = static_cast<const Word8*>(pInput) + 2;
			}
			if (uCount & 1) {
				static_cast<Word8*>(pOutput)[0] =
					static_cast<const Word8*>(pInput)[0];
			}
		}
	}
}

#endif

/*! ************************************

	\brief Copy raw memory from one buffer to another in any direction.

	Given a pointer to a byte stream, copy it to a destination buffer using a
	supplied input length. This differs from Burger::MemoryCopy() in that it is
	safe to use this function for up or down memory copies. It will determine
	which direction to perform the copy in a safe manner. As such, there is a
	small performance penalty for using this function instead of
	Burger::MemoryCopy().

	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the raw data to copy from.
	\param uCount Size in bytes of the data to copy.

	\sa MemoryCopy(), MemoryClear() and MemoryFill()

***************************************/

void BURGER_API Burger::MemoryMove(
	void* pOutput, const void* pInput, WordPtr uCount)
{
	if (pOutput <= pInput) {
		MemoryCopy(pOutput, pInput, uCount);
	} else if (uCount) {
		// Copy in reverse
		pInput = static_cast<const Word8*>(pInput) + (uCount - 1);
		pOutput = static_cast<Word8*>(pOutput) + (uCount - 1);
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<const Word8*>(pInput)[0];
			pInput = static_cast<const Word8*>(pInput) - 1;
			pOutput = static_cast<Word8*>(pOutput) - 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Set a buffer to all zeros

	Given a pointer to a byte array, erase it to all zeros

	\param pOutput Pointer to the buffer to erase.
	\param uCount Size in bytes of the buffer.

	\note This function is not recommended for non-cached or special memories.
		On some platforms, it will use special cache clear instructions to
		perform the operation quickly in which cache-disabled sections of memory
		would generate a page fault.

	\sa MemoryFill() and MemoryCopy()

***************************************/

void BURGER_API Burger::MemoryClear(void* pOutput, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] = 0;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Set a buffer to a specific value

	Given a pointer to a byte array, fill it with a specific value

	\param pOutput Pointer to the buffer to erase.
	\param uFill 8 bit value to fill memory with
	\param uCount Size in bytes of the buffer.

	\note This function is not recommended for non-cached or special memories.
		On some platforms, it will use special cache clear instructions to
		perform the operation quickly in which cache-disabled sections of memory
		would generate a page fault.

	\sa MemoryClear() and MemoryCopy()

***************************************/

void BURGER_API Burger::MemoryFill(void* pOutput, Word8 uFill, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] = uFill;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with 0xFF

	Given a pointer to a byte array, toggle all the zeros to ones and vice
	versa.

	\param pOutput Pointer to the buffer to toggle.
	\param uCount Size in bytes of the buffer.

	\sa MemoryNot(void *,const void *,WordPtr) and MemoryXor(void
		*,Word8,WordPtr)

***************************************/

void BURGER_API Burger::MemoryNot(void* pOutput, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<Word8>(~static_cast<const Word8*>(pOutput)[0]);
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Copy a buffer that's been exclusive or'd with 0xFF

	Given a pointer to a byte array, toggle all the zeros to ones and vice versa
	and store the result into another buffer

	\param pOutput Pointer to the buffer to store the result.
	\param pInput Pointer to the buffer to toggle.
	\param uCount Size in bytes of the buffer.

	\sa MemoryNot(void *,WordPtr) and MemoryXor(void *,const void
		*,Word8,WordPtr)

***************************************/

void BURGER_API Burger::MemoryNot(
	void* pOutput, const void* pInput, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<Word8>(~static_cast<const Word8*>(pInput)[0]);
			pInput = static_cast<const Word8*>(pInput) + 1;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with a value

	Given a pointer to a byte array, exclusive or all of the values with a given
	8 bit value.

	\param pOutput Pointer to the buffer to toggle.
	\param uXor Value to exclusive or the buffer with
	\param uCount Size in bytes of the buffer.

	\sa MemoryXor(void *,const void *,Word8,WordPtr) and MemoryNot(void
		*,WordPtr)

***************************************/

void BURGER_API Burger::MemoryXor(void* pOutput, Word8 uXor, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] = static_cast<Word8>(
				static_cast<const Word8*>(pOutput)[0] ^ uXor);
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with a value and store the result in another
		buffer

	Given a pointer to a byte array, exclusive or all of the values with a given
	8 bit value and store the result elsewhere

	\param pOutput Pointer to the buffer to store the result.
	\param pInput Pointer to the buffer to toggle.
	\param uXor Value to exclusive or the buffer with
	\param uCount Size in bytes of the buffer.

	\sa MemoryXor(void *,Word8,WordPtr) and MemoryNot(void *,const void
		*,WordPtr)

***************************************/

void BURGER_API Burger::MemoryXor(
	void* pOutput, const void* pInput, Word8 uXor, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<Word8>(static_cast<const Word8*>(pInput)[0] ^ uXor);
			pInput = static_cast<const Word8*>(pInput) + 1;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with another buffer

	Given pointers to two byte arrays, exclusive or all of the values of each
	buffer with each other and store the result in the first buffer

	\param pOutput Pointer to the buffer to exclusive or and store the result.
	\param pInput Pointer to the buffer to exclusive or with.
	\param uCount Size in bytes of the buffers. Both buffers must be the same
		size.

	\sa MemoryXor(void *,const void *,const void *,WordPtr) and MemoryXor(void
		*,Word8,WordPtr)

***************************************/

void BURGER_API Burger::MemoryXor(
	void* pOutput, const void* pInput, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<Word8>(static_cast<Word8*>(pOutput)[0] ^
					static_cast<const Word8*>(pInput)[0]);
			pInput = static_cast<const Word8*>(pInput) + 1;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with another buffer and store the result in a
		third buffer

	Given pointers to two byte arrays, exclusive or all of the values of each
	buffer with each other and store the result in a third buffer

	\param pOutput Pointer to the buffer to store the result.
	\param pInput1 Pointer to the first buffer to exclusive or with.
	\param pInput2 Pointer to the second buffer to exclusive or with.
	\param uCount Size in bytes of the buffers. All three buffers must be the
		same size.

	\sa MemoryXor(void *,const void *,WordPtr) and MemoryXor(void *,const void
		*,Word8,WordPtr)

***************************************/

void BURGER_API Burger::MemoryXor(
	void* pOutput, const void* pInput1, const void* pInput2, WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8*>(pOutput)[0] =
				static_cast<Word8>(static_cast<const Word8*>(pInput1)[0] ^
					static_cast<const Word8*>(pInput2)[0]);
			pInput1 = static_cast<const Word8*>(pInput1) + 1;
			pInput2 = static_cast<const Word8*>(pInput2) + 1;
			pOutput = static_cast<Word8*>(pOutput) + 1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Compare two byte buffers for equality

	Given pointers to two unsigned byte arrays, compare them for equality by
	using a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately. If
	both buffers are identical, return zero.

	This is equivalent to memcmp()

	\param pInput1 Pointer to the first buffer to subtract from.
	\param pInput2 Pointer to the second buffer to subtract with.
	\param uCount Size in bytes of both of the buffers.

	\return Zero on equality, negative on less than, positive on greater than.

	\sa MemoryCaseCompare(const void *,const void *,WordPtr)

***************************************/

int BURGER_API Burger::MemoryCompare(
	const void* pInput1, const void* pInput2, WordPtr uCount)
{
	int iTemp = 0;
	if (uCount) {
		do {
			Word uTemp1 = reinterpret_cast<const Word8*>(pInput1)[0];
			pInput1 = reinterpret_cast<const Word8*>(pInput1) + 1;
			Word uTemp2 = reinterpret_cast<const Word8*>(pInput2)[0];
			pInput2 = reinterpret_cast<const Word8*>(pInput2) + 1;
			iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
			if (iTemp) {
				break;
			}
		} while (--uCount); // End of string? (And match!!)
	}
	return iTemp; // Perfect match!
}

/*! ************************************

	\brief Compare two byte buffers for equality, case insensitive

	Given pointers to two unsigned byte arrays, compare them for equality by
	using a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately. If
	both buffers are identical, return zero. If the byte is an upper case ASCII
	character, convert it to lower case before the subtraction is performed

	\param pInput1 Pointer to the first buffer to subtract from.
	\param pInput2 Pointer to the second buffer to subtract with.
	\param uCount Size in bytes of both of the buffers.

	\return Zero on equality, negative on less than, positive on greater than.

	\sa MemoryCompare(const void *,const void *,WordPtr)

***************************************/

int BURGER_API Burger::MemoryCaseCompare(
	const void* pInput1, const void* pInput2, WordPtr uCount) BURGER_NOEXCEPT
{
	int iTemp = 0;
	if (uCount) {
		do {
			Word uTemp1 = reinterpret_cast<const Word8*>(pInput1)[0];
			pInput1 = reinterpret_cast<const Word8*>(pInput1) + 1;
			Word uTemp2 = reinterpret_cast<const Word8*>(pInput2)[0];
			pInput2 = reinterpret_cast<const Word8*>(pInput2) + 1;
			if ((uTemp1 - 'A') < 26) { // Convert to lower case
				uTemp1 = uTemp1 + 32;
			}
			if ((uTemp2 - 'A') < 26) {
				uTemp2 = uTemp2 + 32;
			}
			iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
			if (iTemp) {
				break;
			}
		} while (--uCount); // End of string? (And match!!)
	}
	return iTemp; // Perfect match!
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character in a byte array

	Returns a pointer to the first occurrence of the character iChar in the 8
	bit pInput or \ref NULL if the character was not found

	\param pInput Pointer to an array of bytes to scan
	\param uInputSize Size of the buffer in bytes
	\param iChar Character to scan for

	\return Pointer inside the input string to the first matching character or
		\ref NULL if not found

	\sa Burger::MemoryCharacter(const Word16 *,WordPtr,Word)

***************************************/

char* BURGER_API Burger::MemoryCharacter(
	const char* pInput, WordPtr uInputSize, int iChar)
{
	// Valid pointer?
	if (pInput && uInputSize) {
		// Valid?
		do {
			// Get the first character
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			// Perform the test
			if (static_cast<Word>(iChar) == uTemp) {
				// Found the first one!
				return const_cast<char*>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Not found
	return NULL;
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character in a short
		array

	Returns a pointer to the first occurrence of the character uChar in the 16
	bit pInput or \ref NULL if the character was not found

	\param pInput Pointer to an array of 16 bit shorts to scan
	\param uInputSize Size of the buffer in bytes
	\param uChar Character to scan for

	\return Pointer inside the input string to the first matching character or
		\ref NULL if not found

	\sa Burger::MemoryCharacter(const char *,WordPtr,int)

***************************************/

Word16* BURGER_API Burger::MemoryCharacter(
	const Word16* pInput, WordPtr uInputSize, Word uChar)
{
	// Valid pointer?
	uInputSize >>= 1U;
	if (pInput && uInputSize) {
		do {
			// Get the first character
			Word uTemp = pInput[0];
			// Perform the test
			if (uChar == uTemp) {
				// Found the first one!
				return const_cast<Word16*>(pInput);
			}
			// Get the next character
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Not found
	return NULL;
}

/*! ************************************

	\brief Scan for the last occurrence of a specific character in a buffer

	Returns a pointer to the last occurrence of the character iChar in the
	memory buffer pInput or \ref NULL if the character was not found

	\note This function can be used to scan for zeros in the data array.

	\param pInput Pointer to a char buffer string to scan
	\param uInputSize Number of bytes contained in the buffer
	\param iChar Character to scan for

	\return Pointer inside the input string to the last matching character or
		\ref NULL if not found

	\sa Burger::MemoryCharacterReverse(const Word16 *,WordPtr,Word) and
		Burger::StringCharacterReverse(const char *,int)

***************************************/

char* BURGER_API Burger::MemoryCharacterReverse(
	const char* pInput, WordPtr uInputSize, int iChar)
{
	// Assume it's not found
	char* pFound = NULL;
	// Valid pointer?
	if (pInput && uInputSize) {
		do {
			// Get the next character
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			// Perform the test
			if (static_cast<Word>(iChar) == uTemp) {
				// Found another one!
				pFound = const_cast<char*>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Return the result
	return pFound;
}

/*! ************************************

	\brief Scan for the last occurrence of a specific character in a Word16
		buffer

	Returns a pointer to the last occurrence of the character uChar in the
	Word16 string buffer pInput or \ref NULL if the character was not found

	\note This function can be used to scan for zeros in the data array.

	\param pInput Pointer to a Word16 string buffer to scan
	\param uInputSize Number of BYTES in the buffer
	\param uChar Character to scan for

	\return Pointer inside the input string to the last matching character or
		\ref NULL if not found

	\sa Burger::MemoryCharacterReverse(const char *,WordPtr,int) and
		Burger::StringCharacterReverse(const Word16 *,Word)

***************************************/

Word16* BURGER_API Burger::MemoryCharacterReverse(
	const Word16* pInput, WordPtr uInputSize, Word uChar)
{
	// Assume it's not found
	Word16* pFound = NULL;
	uInputSize <<= 1; // Convert to Word16 sizes
	// Valid pointer?
	if (pInput && uInputSize) {
		do {
			// Get the next character
			Word uTemp = pInput[0];
			// Perform the test
			if (uChar == uTemp) {
				// Found another one!
				pFound = const_cast<Word16*>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Return the result
	return pFound;
}
