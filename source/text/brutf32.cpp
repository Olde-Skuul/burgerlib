/***************************************

	String handlers for UTF32 support

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf32.h"
#include "brutf8.h"

/*! ************************************

	\class Burger::UTF32
	\brief Conversion routines to the UTF32 format

	UTF32 is simplest data format for [Unicode](http://www.unicode.org) data to
	be stored. It is a 32 bit wide "C" string. It can easily contain all of the
	characters for the worlds' languages. These functions allow conversion from
	UTF8, which Burgerlib is based on, to UTF32 which some foreign APIs require
	for internationalization or functions that need UTF32 strings for simplified
	internal code.

	\note These functions operate on strings that are native endian.

***************************************/

/*! ************************************

	\var Burger::UTF32::kEndianMark
	\brief Byte stream token for native endian.

	When writing a text file using UTF32, you may need to write this value as
	the first character to mark the endian that the data was saved at. This
	value is the correct value for the native endian of the machine. Use
	\ref kBigEndianMark or \ref kLittleEndianMark to test incoming data to
	determine the endian of data that is unknown.

***************************************/

/*! ************************************

	\var Burger::UTF32::kBigEndianMark
	\brief 32 bit Byte Order Mark (BOM) for Big Endian UTF32 data.

	If a token was read in the matched this constant, then it is assumed that
	all of the following data is \ref BigEndian. It adheres to the Unicode
	standard for UTF-32 encoding.

***************************************/

/*! ************************************

	\var Burger::UTF32::kLittleEndianMark
	\brief 32 bit Byte Order Mark (BOM) for Little Endian UTF32 data.

	If a token was read in the matched this constant, then it is assumed that
	all of the following data is \ref LittleEndian. It adheres to the Unicode
	standard for UTF-32 encoding.

***************************************/

/*! ************************************

	\brief Validate a UTF32 value.

	Return \ref TRUE if a UTF32 character is in the valid bounds. (0-0xD7FF) or
	(0xE000-0x10FFFF).

	\param uInput UTF32 encoded character value.
	\return \ref TRUE if in bounds, \ref FALSE if not.

	\sa IsValid(const uint32_t *) or IsValid(const uint32_t *, uintptr_t)

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(uint32_t uInput) BURGER_NOEXCEPT
{
	if ((uInput < 0xD800U) || ((uInput >= 0xE000U) && (uInput < 0x110000U))) {
		return TRUE;
	}
	return FALSE;
}

/*! ************************************

	\brief Check a UTF32 "C" string for validity

	Check a "C" string if it's a valid UTF32 stream. Return false if there was
	an error, or true if the bytes represent a valid UTF32 pattern. Parsing will
	end once a zero character is found.

	\param pInput Pointer to a zero terminated string, ``nullptr`` will page
		fault.

	\return \ref TRUE if the entire string is a valid UTF32 stream, \ref FALSE
		if not.

	\sa IsValid(uint32_t) or IsValid(const uint32_t *, uintptr_t)

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(const uint32_t* pInput) BURGER_NOEXCEPT
{
	// Get the first character in the string.
	uint_t uFirst = pInput[0];

	// Only perform the test on a non-empty string.
	if (uFirst) {
		// Accept the initial input
		++pInput;
		do {
			// Check if the character is in UTF32 bounds.
			if (((uFirst >= 0xD800U) && (uFirst < 0xE000U)) ||
				(uFirst >= 0x110000U)) {
				return FALSE;
			}

			// Accept another character from the string.
			uFirst = pInput[0];
			++pInput;
		} while (uFirst);
	}

	// Yes! It's good
	return TRUE;
}

/*! ************************************

	\brief Check a UTF32 uint32_t array for validity.

	Check a uint32_t array and see if it's a valid UTF32 stream. Return \ref
	FALSE if there was an error, or \ref TRUE if the bytes represent a valid
	UTF32 pattern. Zeros are considered valid in the stream.

	\param pInput Pointer to UTF32 data. Can be ``nullptr`` if uInputSize is
		zero, otherwise page fault.
	\param uElementCount Size of the data in elements, if zero, then the
		function will return \ref TRUE and perform no work.

	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if
		not.

	\sa IsValid(uint32_t) or IsValid(const uint32_t *)

***************************************/

uint_t BURGER_API Burger::UTF32::IsValid(
	const uint32_t* pInput, uintptr_t uElementCount) BURGER_NOEXCEPT
{
	if (uElementCount) {
		do {
			const uint32_t uFirst = pInput[0];
			// Accept it
			++pInput;
			if (((uFirst >= 0xD800U) && (uFirst < 0xE000U)) ||
				(uFirst >= 0x110000U)) {
				// This is a bad UTF32 string
				return FALSE;
			}
		} while (--uElementCount);
	}
	// Yes! It's good
	return TRUE;
}

/*! ************************************

	\brief Return a UTF32 code from a UTF8 stream.

	Convert from a UTF8 stream into a 32 bit Unicode value (0x00 to 0x10FFFF).
	This function will perform validation on the incoming stream and will flag
	any data that is invalid.

	\note This function will not move the pointer forward, use
		UTF8::NextToken(const char *) instead.

	\param pInput Pointer to a valid UTF8 "C" string, ``nullptr`` will page
		fault.

	\return The UTF32 code or \ref kInvalid if invalid. 0x00 is not invalid.

	\sa TranslateFromUTF8(const char **), UTF8::GetTokenSize(const char *) or
		UTF8::NextToken(const char *).

***************************************/

uint32_t BURGER_API Burger::UTF32::TranslateFromUTF8(
	const char* pInput) BURGER_NOEXCEPT
{
	// Check for the simple case of 0x00-0x7F
	const uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];

	// Return ASCII now (0x00-0x7F)
	if (uFirst < 0x80U) {
		return uFirst;
	}

	// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
	if (uFirst >= 0xC2U) {

		// Trick to convert 0x80-0xBF to 0x00-0x3F
		const uint32_t uSecond =
			reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
		if (uSecond < 0x40U) {

			// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
			if (uFirst < 0xE0U) {
				// Get the upper 5 bits
				return ((uFirst & 0x1FU) << 6U) | uSecond;
			}

			const uint32_t uThird =
				reinterpret_cast<const uint8_t*>(pInput)[2] ^ 0x80U;
			if (uThird < 0x40U) {

				// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
				// 0x80-0xBF
				if (uFirst < 0xF0U) {
					// Get the upper 4 bits
					const uint32_t uResult =
						((uFirst & 0xFU) << 12U) | (uSecond << 6U) | uThird;
					// Only 0x0800-0xFFFF are valid
					if ((uResult >= 0x800U) &&
						((uResult < 0xD800U) || (uResult >= 0xE000U))) {
						// It's good!
						return uResult;
					}
				} else {
					const uint32_t uFourth =
						reinterpret_cast<const uint8_t*>(pInput)[3] ^ 0x80U;
					if (uFourth < 0x40U) {

						// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF /
						// 0x80-0xBF / 0x80-0xBF
						// Get the upper 4 bits
						const uint32_t uResult = ((uFirst & 0xFU) << 18U) |
							(uSecond << 12U) | (uThird << 6U) | uFourth;

						// Only 0x10000-0x10FFFF are valid
						if ((uResult >= 0x10000U) && (uResult < 0x110000U)) {
							// It's good!
							return uResult;
						}
					}
				}
			}
		}
	}
	// This is a bad stream
	return kInvalid;
}

/*! ************************************

	\brief Return a UTF32 code from a UTF8 stream and update the pointer

	Convert from a UTF8 stream into a 32 bit Unicode value (0x00 to 0x10FFFF).
	This function will perform validation on the incoming stream and will flag
	any data that's invalid.

	\param ppInput Pointer to a valid UTF8 "C" string pointer, ``nullptr`` will
		page fault.

	\return The UTF32 code or \ref kInvalid if invalid. 0x00 is not invalid.
	\sa FromUTF8(const char *), UTF8::GetTokenSize(const char *) or
		UTF8::NextToken(const char *).

***************************************/

uint32_t BURGER_API BURGER_API Burger::UTF32::TranslateFromUTF8(
	const char** ppInput) BURGER_NOEXCEPT
{
	const char* pInput = ppInput[0];
	// Decode the data
	const uint32_t uResult = TranslateFromUTF8(pInput);
	if (uResult != kInvalid) {
		ppInput[0] = UTF8::NextToken(pInput);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF8 "C" string into a UTF32 stream.

	Take a "C" string that is using UTF8 encoding and convert it to a UTF32
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be null to have this routine calculate the
	size of the possible output so the application can allocate a buffer large
	enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note If invalid UTF8 data is found, it will be skipped.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded "C" string, ``nullptr`` will page fault.

	\return The number of bytes of the potential output without the trailing
		uint32_t zero. It is valid, even if the output buffer wasn't large
		enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::TranslateFromUTF8(uint32_t* pOutput,
	uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	// Make a copy, since pOutput is needed for determining size
	uint32_t* pWorkPtr = pOutput;

	// This is the end of the valid buffer
	uint32_t* pEndPtr = pWorkPtr + uOutputSize;

	// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {
		--pEndPtr;
	}

	// Let's convert the string
	uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
	++pInput;

	// Can I start the loop?
	if (uFirst) {
		do {
			// Check for the simple case of 0x00-0x7F
			if (uFirst < 0x80U) {
				// Write out the value, if possible
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = uFirst;
				}
				++pWorkPtr;

				// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			} else if (uFirst >= 0xC2U) {

				// Trick to convert 0x80-0xBF to 0x00-0x3F
				const uint32_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;

				if (uSecond < 0x40U) {

					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst < 0xE0U) {
						// Write out the value, if possible
						if (pWorkPtr < pEndPtr) {
							pWorkPtr[0] = ((uFirst & 0x1FU) << 6U) | uSecond;
						}
						++pWorkPtr;

						// Accept the second byte
						++pInput;

						// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
						// 0x80-0xBF
					} else {
						const uint32_t uThird =
							reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
						if (uThird < 0x40U) {
							if (uFirst < 0xF0U) {
								// Get the upper 4 bits
								const uint32_t uResult =
									((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
									uThird;
								if ((uResult >= 0x800U) &&
									((uResult < 0xD800U) ||
										// Only 0x0800-0xFFFF are valid
										(uResult >= 0xE000U))) {
									// Write out the value, if possible
									if (pWorkPtr < pEndPtr) {
										pWorkPtr[0] = uResult;
									}
									++pWorkPtr;
								}
								// I took three chars
								pInput += 2;

								// Check for the 21 bit code of 0xF0-0xFF /
								// 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else {
								const uint32_t uFourth =
									reinterpret_cast<const uint8_t*>(
										pInput)[2] ^
									0x80U;
								if (uFourth < 0x40U) {
									// Get the upper 4 bits
									const uint32_t uResult =
										((uFirst & 0xFU) << 18U) |
										(uSecond << 12U) | (uThird << 6U) |
										uFourth;
									// Only 0x10000-0x10FFFF are valid
									if ((uResult >= 0x10000U) &&
										(uResult < 0x110000U)) {
										// Write out the value, if possible
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = uResult;
										}
										++pWorkPtr;
									}
									pInput += 3;
								}
							}
						}
					}
				}
			}

			// Fetch a new start UTF8 character
			uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
			++pInput;
		} while (uFirst);
	}

	// Can I add a trailing zero? Don't modify pWorkPtr
	if (uOutputSize) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(pWorkPtr - pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a UTF32 uint32_t array.

	Take a byte array that is using UTF8 encoding and convert it to a UTF32
	uint32_t encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer
	size. The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
		if a zero was parsed. Zeros will be placed in the UTF32 stream as is.

	\param pOutput Pointer to a byte buffer to receive the UTF32 string
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF8 encoded byte array, ``nullptr`` is okay if uInputSize is
		zero.
	\param uInputSize Size of the input byte array.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::TranslateFromUTF8(uint32_t* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	// Make a copy, since pOutput is needed for determining size
	uint32_t* pWorkPtr = pOutput;

	// This is the end of the valid buffer
	uint32_t* pEndPtr = pWorkPtr + uOutputSize;

	// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {
		--pEndPtr;
	}

	// Let's convert the string
	if (uInputSize) {
		do {
			// Accept the first character
			const uint32_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
			++pInput;

			// Check for the simple case of 0x00-0x7F
			if (uFirst < 0x80U) {
				// Write out the value, if possible
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = uFirst;
				}
				++pWorkPtr;

				// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			} else if ((uInputSize >= 2) && uFirst >= 0xC2U) {
				// Trick to convert 0x80-0xBF to 0x00-0x3F
				const uint32_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
				if (uSecond < 0x40U) {

					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst < 0xE0U) {
						// Write out the value, if possible
						if (pWorkPtr < pEndPtr) {
							pWorkPtr[0] = ((uFirst & 0x1FU) << 6U) | uSecond;
						}
						++pWorkPtr;

						// Accept the second byte
						++pInput;
						--uInputSize;

						// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
						// 0x80-0xBF
					} else if (uInputSize >= 3) {
						const uint32_t uThird =
							reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
						if (uThird < 0x40U) {
							if (uFirst < 0xF0U) {
								// Get the upper 4 bits
								const uint32_t uResult =
									((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
									uThird;
								// Only 0x0800-0xFFFF are valid
								if ((uResult >= 0x800U) &&
									((uResult < 0xD800U) ||
										(uResult >= 0xE000U))) {
									// Write out the value, if possible
									if (pWorkPtr < pEndPtr) {
										pWorkPtr[0] = uResult;
									}
									++pWorkPtr;
								}
								// I took three chars
								pInput += 2;
								uInputSize -= 2;

								// Check for the 21 bit code of 0xF0-0xFF /
								// 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else if (uInputSize >= 4) {
								const uint32_t uFourth =
									reinterpret_cast<const uint8_t*>(
										pInput)[2] ^
									0x80U;
								if (uFourth < 0x40U) {
									// Get the upper 4 bits
									const uint32_t uResult =
										((uFirst & 0xFU) << 18U) |
										(uSecond << 12U) | (uThird << 6U) |
										uFourth;
									// Only 0x10000-0x10FFFF are valid
									if ((uResult >= 0x10000U) &&
										(uResult < 0x110000U)) {

										// Write out the value, if possible
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = uResult;
										}
										++pWorkPtr;
									}
								}
								pInput += 3;
								uInputSize -= 3;
							}
						}
					}
				}
			}
		} while (--uInputSize);
	}

	// Can I add a trailing zero? Don't modify pWorkPtr
	if (uOutputSize) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(pWorkPtr - pOutput);
}

/*! ************************************

	\brief Convert a UTF32 value into a UTF8 stream.

	Given a valid UTF32 value (0-0xD7FF / 0xE000-0x10FFFF), encode it into a
	valid UTF8 stream. If the value is invalid, it will NOT be encoded.

	The output buffer must have at least 5 bytes available.

	\note The function will zero terminate the stream. The zero isn't counted on
		the data length.

	\param pOutput Pointer to a char buffer of a minimum of 5 bytes in size,
		``nullptr`` is invalid.
	\param uInput UTF32 encoded character value.

	\return Number of bytes used to store the UTF8 stream, zero if the value
		can't be encoded.

***************************************/

uintptr_t BURGER_API Burger::UTF32::TranslateToUTF8(
	char* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
	// Not ASCII?
	uintptr_t uResult;
	if (uInput >= 0x80U) {
		// 0x80-0x7FF (11 bit encoding)?
		if (uInput < 0x800U) {
			// Encode the upper 6 bits
			pOutput[0] = static_cast<char>((uInput >> 6U) | 0xC0U);
			// Encode the lower 6 bits
			pOutput[1] = static_cast<char>((uInput & 0x3FU) | 0x80U);
			pOutput[2] = 0;
			uResult = 2;

		} else if (((uInput >= 0xD800U) && (uInput < 0xE000U)) ||
			(uInput >= 0x110000U)) {
			pOutput[0] = 0;
			uResult = 0;
			// 0xD800-0xDFFF or >=0x110000 are bad.
			// 16 bit encoding?
		} else if (uInput < 0x10000U) {
			pOutput[0] = static_cast<char>((uInput >> 12U) | 0xE0U);
			pOutput[1] = static_cast<char>(((uInput >> 6U) & 0x3FU) | 0x80U);
			pOutput[2] = static_cast<char>((uInput & 0x3FU) | 0x80U);
			pOutput[3] = 0;
			uResult = 3;
			// 21 bit encoding
		} else {
			pOutput[0] = static_cast<char>((uInput >> 18U) | 0xF0U);
			pOutput[1] = static_cast<char>(((uInput >> 12U) & 0x3FU) | 0x80U);
			pOutput[2] = static_cast<char>(((uInput >> 6U) & 0x3FU) | 0x80U);
			pOutput[3] = static_cast<char>((uInput & 0x3FU) | 0x80U);
			pOutput[4] = 0;
			uResult = 4;
		}
	} else {

		// This is the most common case, save it and exit.
		// Easy!
		pOutput[0] = static_cast<char>(uInput);
		pOutput[1] = 0;
		uResult = 1;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF32 "C" string into a UTF8 stream.

	Take a "C" string that is using UTF32 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note If invalid UTF32 values are found, they will be skipped.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::TranslateToUTF8(char* pOutput,
	uintptr_t uOutputSize, const uint32_t* pInput) BURGER_NOEXCEPT
{
	// Make a copy, since pOutput is needed for determining size
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {
		--pEndPtr;
	}

	// Let's convert the string
	uint32_t uTemp = pInput[0];
	++pInput;

	// Can I start the loop?
	if (uTemp) {
		do {

			// Get an ASCII char?
			if (uTemp < 0x80U) {
				// Write out the value, if possible
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				++pWorkPtr;

				// 0x80-0x7FF (11 bit encoding)?
			} else if (uTemp < 0x800U) {

				// Write out the value, if possible
				if (pWorkPtr < pEndPtr) {
					// Encode the upper 6 bits
					pWorkPtr[0] = static_cast<uint8_t>((uTemp >> 6) | 0xC0U);
				}
				++pWorkPtr;

				if (pWorkPtr < pEndPtr) {
					// Encode the lower 6 bits
					pWorkPtr[0] = static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;

			} else if (((uTemp < 0xD800U) || (uTemp >= 0xE000U)) &&
				(uTemp < 0x110000U)) {
				if (uTemp < 0x10000U) {

					// 16 bit encoding? (0x800-0xFFFF)
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 12U) | 0xE0U);
					}
					++pWorkPtr;

					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
				} else {
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)

					if (pWorkPtr < pEndPtr) {
						// Top 3 bits
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 18U) | 0xF0U);
					}
					++pWorkPtr;

					// Next 6 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 12U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					// Next 6 bits from uTemp
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					// The last 6 bits from uTemp
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
			// Get the next 32 bits
			uTemp = pInput[0];
			++pInput;
		} while (uTemp);
	}
	// Can I add a trailing zero?
	if (uOutputSize) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a UTF32 uint32_t array into a UTF8 stream.

	Take a uint32_t array that is using UTF32 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
	if a zero was parsed. Zeros will be placed in the UTF8 stream as is.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded uint32_t array, ``nullptr`` is okay if
		uInputSize is zero.
	\param uInputSize Size of the input uint32_t array in bytes

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF32::TranslateToUTF8(char* pOutput,
	uintptr_t uOutputSize, const uint32_t* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	// Make a copy, since pOutput is needed for determining size
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--pEndPtr;
	}

	// Let's convert the string
	// Convert to uint32_t's
	if (uInputSize) {
		do {
			// Get the next 32 bits
			const uint32_t uTemp = pInput[0];
			++pInput;

			// ASCII?
			if (uTemp < 0x80U) {
				// Write out the value, if possible
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				++pWorkPtr;

				// 0x80-0x7FF (11 bit encoding) ?
			} else if (uTemp < 0x800U) {
				if (pWorkPtr < pEndPtr) {
					// Encode the upper 6 bits
					pWorkPtr[0] = static_cast<uint8_t>((uTemp >> 6) | 0xC0U);
				}
				++pWorkPtr;

				if (pWorkPtr < pEndPtr) {
					// Encode the lower 6 bits
					pWorkPtr[0] = static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;

			} else if (((uTemp < 0xD800U) || (uTemp >= 0xE000U)) &&
				(uTemp < 0x110000U)) {
				if (uTemp < 0x10000U) {
					// 16 bit encoding? (0x800-0xFFFF)
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 12U) | 0xE0U);
					}
					++pWorkPtr;

					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					// Valid start token?
				} else {
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)
					if (pWorkPtr < pEndPtr) {
						// Top 3 bits
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 18U) | 0xF0U);
					}
					++pWorkPtr;

					// Next 6 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 12U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					// Next 6 bits from uTemp
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;

					// The last 6 bits from uTemp
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
		} while (--uInputSize);
	}

	// Can I add a trailing zero?
	if (uOutputSize) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}
