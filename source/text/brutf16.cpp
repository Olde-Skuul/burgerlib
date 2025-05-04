/***************************************

	String handlers for UTF16 support

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf16.h"

/*! ************************************

	\class Burger::UTF16
	\brief Conversion routines to the UTF8 format

	UTF16 is a data format that allows [Unicode](http://www.unicode.org) data to
	be stored in a 16 bit wide "C" string. It is wide enough to contain all of
	the most popular characters for the worlds languages. These functions allow
	conversion from UTF8, which Burgerlib is based on, to UTF16 which some
	foreign APIs require for internationalization. Please note that these
	functions operate on strings that are native endian.

***************************************/

/*! ************************************

	\var Burger::UTF16::kInvalid
	\brief Value returned if a routine failed.

	If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
	return this value instead. Please see the documentation for each function
	to know which ones use true/false pairs or this value.

***************************************/

/*! ************************************

	\var Burger::UTF16::kEndianMark
	\brief Byte stream token for native endian.

	When writing a text file using UTF16, you may need to write this value as
	the first character to mark the endian that the data was saved at. This
	value is the correct value for the native endian of the machine. Use
	Burger::UTF16::BE or Burger::UTF16::LE to test incoming data to determine
	the endian of data that's unknown.

***************************************/

/*! ************************************

	\var Burger::UTF16::kBigEndianMark
	\brief 16 bit Byte Order Mark (BOM) for Big Endian UTF16 data.

	If a token was read in the matched this constant, then you must assume that
	all of the following data is Big Endian. It adheres to the Unicode standard
	for UTF-16

***************************************/

/*! ************************************

	\var Burger::UTF16::kLittleEndianMark
	\brief 16 bit Byte Order Mark (BOM) for Little Endian UTF16 data.

	If a token was read in the matched this constant, then you must assume that
	all of the following data is Little Endian. It adheres to the Unicode
	standard for UTF-16

***************************************/

/*! ************************************

	\brief Validate a UTF16 value.

	\note Use of this function is not recommended because it considers escape
		values as invalid. Use is_valid(const uint16_t*) instead.

	Return \ref TRUE if a UTF16 character is in the valid bounds. (0-0xD7FF) or
	(0xE000-0xFFFF).

	\param uInput UTF16 encoded character value.
	\return \ref TRUE if in bounds, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::UTF16::is_valid(uint16_t uInput) BURGER_NOEXCEPT
{
	if ((uInput < 0xD800U) || (uInput >= 0xE000U)) {
		return TRUE;
	}
	return FALSE;
}

/*! ************************************

	\brief Check a UTF16 "C" string for validity

	Check a "C" string if it's a valid UTF16 stream. Return \ref FALSE if there
	was an error, or \ref TRUE if the bytes represent a valid UTF16 pattern.

	\param pInput Pointer to a zero terminated string, ``nullptr`` will page
		fault.
	\return \ref TRUE if the entire string is a valid UTF16 stream, \ref FALSE
		if not.

***************************************/

uint_t BURGER_API Burger::UTF16::is_valid(const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Get the first character in the string.
	uint_t uFirst = pInput[0];

	// Only perform the test on a non-empty string.
	if (uFirst) {
		// Accept the initial input
		++pInput;
		do {
			// Invalid? 0xDC00-0xDFFF
			if ((uFirst >= 0xD800U) && (uFirst < 0xE000)) {
				if (uFirst >= 0xDC00U) {
					// This is a bad UTF16 string
					return FALSE;
				}

				// Get the next value of the surrogate pair
				const uint_t uSecond = pInput[0] ^ 0xDC00U;
				++pInput;
				// In bounds (DC00-E000)?
				if (uSecond >= 0x400U) {
					// This is a bad UTF16 string
					return FALSE;
				}
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

	\brief Check a UTF16 uint16_t array for validity.

	Check a uint16_t array and see if it's a valid UTF16 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a
	valid UTF16 pattern.

	\param pInput Pointer to UTF16 data. Can be ``nullptr`` if uInputSize is
		zero, otherwise page fault.
	\param uElementCount Size of the data in elements, if zero, then the
		function will return true.

	 \return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE
		if not.

***************************************/

uint_t BURGER_API Burger::UTF16::is_valid(
	const uint16_t* pInput, uintptr_t uElementCount) BURGER_NOEXCEPT
{
	if (uElementCount) {
		do {
			const uint_t uFirst = pInput[0];
			// Accept it
			++pInput;
			// Invalid? 0xDC00-0xDFFF
			if ((uFirst >= 0xD800U) && (uFirst < 0xE000U)) {
				if ((uElementCount < 2) || (uFirst >= 0xDC00U)) {
					// This is a bad UTF16 string
					return FALSE;
				}
				// Get the next value of the surrogate pair
				const uint_t uSecond = pInput[0] ^ 0xDC00U;
				--uElementCount;
				++pInput;

				// In bounds (DC00-E000)?
				if (uSecond >= 0x400U) {
					// This is a bad UTF16 string
					return FALSE;
				}
			}
		} while (--uElementCount);
	}
	// Yes! It's good
	return TRUE;
}

/*! ************************************

	\brief Return a UTF16 code from a UTF8 stream.

	Convert from a UTF8 stream into a 16 bit Unicode value (0x00 to 0FFFF). This
	function will perform validation on the incoming stream and will flag any
	data that's invalid. It will not parse Unicode values in the range of
	0xD800-0xDFFF and greater than 0xFFFF, these do not fit in a single 16 bit
	quantity and will return an error.

	\note This function will not move the pointer forward, use
	Burger::UTF8::next_token(const char *) instead.

	\param pInput Pointer to a valid UTF8 "C" string.
	\return The UTF16 code or Burger::UTF16::kInvalid if invalid. 0x00 is not
		invalid.

	\sa Burger::UTF8::get_token_size(const char *) or
		Burger::UTF8::next_token(const char *).

***************************************/

uint16_t BURGER_API Burger::UTF16::translate_from_UTF8(
	const char* pInput) BURGER_NOEXCEPT
{

	// Check for the simple case of 0x00-0x7F

	const uint16_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uFirst < 0x80U) {
		// Return ASCII now (0x00-0x7F)
		return uFirst;
	}

	// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
	if (uFirst >= 0xC2U) {
		// Trick to convert 0x80-0xBF to 0x00-0x3F
		const uint16_t uSecond = static_cast<uint16_t>(
			reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U);
		if (uSecond < 0x40U) {

			// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
			if (uFirst < 0xE0U) {
				// Get the upper 5 bits
				return static_cast<uint16_t>(
					((uFirst & 0x1FU) << 6U) | uSecond);

				// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
				// 0x80-0xBF
			}
			if (uFirst < 0xF0U) {

				// 3 byte code
				// Get the upper 4 bits
				uint16_t uResult = static_cast<uint16_t>(
					((uFirst & 0xFU) << 12U) | (uSecond << 6U));
				const uint16_t uThird = static_cast<uint16_t>(
					reinterpret_cast<const uint8_t*>(pInput)[2] ^ 0x80U);

				if (uThird < 0x40U) {
					uResult |= uThird;
					// Only 0x0800-0xFFFF are valid
					if ((uResult >= 0x800U) &&
						((uResult < 0xD800U) || (uResult >= 0xE000U))) {
						// I'm good!
						return uResult;
					}
				}
			}
		}
		// Don't bother with the 21 bit code, it won't fit in UTF16
		// 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
	}

	// This is a bad stream
	return kInvalid;
}

/*! ************************************

	\brief Convert a UTF8 "C" string into a UTF16 stream.

	Take a "C" string that is using UTF8 encoding and convert it to a UTF16
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate
	the size of the possible output so the application can allocate a buffer
	large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note If invalid UTF8 data is found, it will be skipped.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
	``nullptr`` is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in elements.
	\param pInput UTF16 encoded "C"string, ``nullptr`` will page fault.

	\return The number of elements of the potential output without the trailing
	uint16_t zero. It is valid, even if the output buffer wasn't large enough to
	contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF16::translate_from_UTF8(uint16_t* pOutput,
	uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	uint16_t* pWorkPtr = pOutput;

	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;

	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint16_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
	// Accept it
	++pInput;
	// Can I start the loop?
	if (uFirst) {
		do {
			// Check for the simple case of 0x00-0x7F
			if (uFirst < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint16_t>(uFirst);
				}
				// Increment the output
				++pWorkPtr;

				// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

			} else if (uFirst >= 0xC2U) {
				// Trick to convert 0x80-0xBF to 0x00-0x3F
				const uint_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
				if (uSecond < 0x40U) {

					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst < 0xE0U) {
						// Can I write?
						if (pWorkPtr < pEndPtr) {
							pWorkPtr[0] = static_cast<uint16_t>(
								((uFirst & 0x1FU) << 6U) | uSecond);
						}
						// Increment the output
						++pWorkPtr;
						// Accept the second byte
						++pInput;

						// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
						// 0x80-0xBF
					} else {
						const uint_t uThird =
							reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
						if (uThird < 0x40U) {
							if (uFirst < 0xF0U) {
								// Get the upper 4 bits
								const uint_t uResult =
									((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
									uThird;
								// Only 0x0800-0xFFFF are valid
								if ((uResult >= 0x800U) &&
									((uResult < 0xD800U) ||
										(uResult >= 0xE000U))) {
									// Can I write?
									if (pWorkPtr < pEndPtr) {
										pWorkPtr[0] =
											static_cast<uint16_t>(uResult);
									}
									// Increment the output
									++pWorkPtr;
								}
								// Accepted three chars
								pInput += 2;

								// Check for the 21 bit code of 0xF0-0xFF /
								// 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else {
								const uint_t uFourth =
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

										// Since UTF16 can't take 32 bit values,
										// Make a surrogate pair
										// Can I write?
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = static_cast<uint16_t>(
												(uResult >> 10U) +
												(0xD800 - 0x40));
										}
										// Increment the output
										++pWorkPtr;
										// Can I write?
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = static_cast<uint16_t>(
												(uResult & 0x3FFU) | 0xDC00U);
										}
										// Increment the output
										++pWorkPtr;
									}
									pInput += 3;
								}
							}
						}
					}
				}
			}
			uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (uFirst);
	}
	// Can I add a trailing zero?
	if (uAddZero) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return number of characters
	return static_cast<uintptr_t>(pWorkPtr - pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a UTF16 uint16_t array.

	Take a byte array that is using UTF8 encoding and convert it to a UTF16
	uint16_t encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer
	size. The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
		if a zero was parsed. Zeros will be placed in the UTF16 stream as is.

	\param pOutput Pointer to a uint16_t buffer to receive the UTF16 string,
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in elements.
	\param pInput UTF8 encoded byte array, ``nullptr`` is okay if uInputSize is
		zero.
	\param uInputSize Size of the input byte array.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF16::translate_from_UTF8(uint16_t* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint16_t* pWorkPtr = pOutput;

	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;

	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint16_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string

	// Sanity check
	if (uInputSize) {
		do {
			const uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Check for the simple case of 0x00-0x7F
			if (uFirst < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint16_t>(uFirst);
				}
				// Increment the output
				++pWorkPtr;

				// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

			} else if ((uInputSize >= 2) && uFirst >= 0xC2U) {
				// Trick to convert 0x80-0xBF to 0x00-0x3F
				const uint_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
				if (uSecond < 0x40U) {

					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst < 0xE0U) {
						// Can I write?
						if (pWorkPtr < pEndPtr) {
							pWorkPtr[0] = static_cast<uint16_t>(
								((uFirst & 0x1FU) << 6U) | uSecond);
						}
						++pWorkPtr; // Increment the output
						++pInput;   // Accept the second byte
						--uInputSize;

						// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF /
						// 0x80-0xBF
					} else if (uInputSize >= 3) {
						const uint_t uThird =
							reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;
						if (uThird < 0x40U) {
							if (uFirst < 0xF0U) {
								// Get the upper 4 bits
								const uint_t uResult =
									((uFirst & 0xFU) << 12U) | (uSecond << 6U) |
									uThird;
								// Only 0x0800-0xFFFF are valid
								if ((uResult >= 0x800U) &&
									((uResult < 0xD800U) ||
										(uResult >= 0xE000U))) {
									// Can I write?
									if (pWorkPtr < pEndPtr) {
										pWorkPtr[0] =
											static_cast<uint16_t>(uResult);
									}
									++pWorkPtr; // Increment the output
								}
								pInput += 2; // I took three chars
								uInputSize -= 2;

								// Check for the 21 bit code of 0xF0-0xFF /
								// 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else if (uInputSize >= 4) {
								const uint_t uFourth =
									reinterpret_cast<const uint8_t*>(
										pInput)[2] ^
									0x80U;
								if (uFourth < 0x40U) {
									// Get the upper 4 bits
									const uint_t uResult =
										((uFirst & 0xFU) << 18U) |
										(uSecond << 12U) | (uThird << 6U) |
										uFourth;
									// Only 0x10000-0x10FFFF are valid
									if ((uResult >= 0x10000U) &&
										(uResult < 0x110000U)) {

										// Since UTF16 can't take 32 bit values,
										// Make a surrogate pair
										// Can I write?
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = static_cast<uint16_t>(
												(uResult >> 10U) +
												(0xD800 - 0x40));
										}
										// Increment the output
										++pWorkPtr;
										// Can I write?
										if (pWorkPtr < pEndPtr) {
											pWorkPtr[0] = static_cast<uint16_t>(
												(uResult & 0x3FFU) | 0xDC00U);
										}
										// Increment the output
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
	// Can I add a trailing zero?
	if (uAddZero) {
		if (pWorkPtr < pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		// Write it, but don't add it to the strlen()
		pEndPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(pWorkPtr - pOutput);
}
