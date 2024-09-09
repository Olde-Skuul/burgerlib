/***************************************

	String handlers for UTF8 support

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf8.h"
#include "brglobalmemorymanager.h"
#include "brisolatin1.h"
#include "brmacromanus.h"
#include "brwin1252.h"
#include "brwin437.h"

/***************************************

	Here are the invalid UTF8 patterns that this code is using for error
	detection.

	Unicode pattern
	0x00-0x7F ASCII
	0x80-0xBF transports (Invalid without a prefix)

	11 bit / 2 byte pattern
	Valid range of 0x80-0x7FF
	110xxxxx 10yyyyyy = xxx xxyyyyyy
	0xC0-0xC1 are no good. All others are fine

	16 bit / 3 byte pattern
	Valid ranges of 0x800-0xD7FF, 0xE000-0xFFFF
	1110xxxx 10yyyyyy 10zzzzzz = xxxxyyyy yyzzzzzz
	0xE0/0x80-0x9F are invalid (They produce 0x0000-0x07FF)
	0xED/0xA0-0xBF are invalid (They produce 0xD800-0xDFFF)

	21 bit / 4 byte pattern
	Valid range of 0x10000-0x10FFFF
	1111xxxx 10yyyyyy 10zzzzzz 10wwwwww = xxxxyy yyyyzzzz zzwwwwww
	0xF0/0x80-0x8F are invalid (They produce 0x0000-0xFFFF)
	0xF4/0x90-0xBF are invalid (They produce 0x110000 and up)
	0xF5-0xFF invalid

***************************************/

/*! ************************************

	\class Burger::UTF8
	\brief Conversion routines to support the UTF8 format.

	UTF8 is a format that allows [Unicode](http://www.unicode.org)
	data to be stored in a standard "C" string with little modification to most
	existing string managers. All Burgerlib functions accept UTF8 strings so
	that they can properly present international characters in a consistent
	manner across numerous target platforms.

***************************************/

/*! ************************************

	\var Burger::UTF8::kInvalid
	\brief Value returned if a routine failed.

	If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
	return this value instead. Please see the documentation for each function
	to know which ones use true/false pairs or this value.

***************************************/

/*! ************************************

	\var const uint8_t Burger::UTF8::TokenSizeTable[256]
	\brief Table to determine the size of a UTF8 token stream.

	Using the first byte as an index, obtain the size of the stream in
	bytes from this table. Entries are the numbers 1-4. This table
	shouldn't be used for error checking and it's only for quick look ups
	on valid UTF8 streams.

***************************************/

BURGER_ALIGN(const uint8_t, Burger::UTF8::TokenSizeTable[256], 16) = {
	// 0x00
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x10
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x20
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x30
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x40
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x50
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x60
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x70
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x80
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0x90
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0xA0
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0xB0
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01,
	// 0xC0
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02,
	// 0xD0
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02,
	// 0xE0
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03,
	// 0xF0
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04};

/*! ************************************

	\brief UTF8 text file signature.

	If a raw text file starts with this three byte pattern, you're supposed to
	assume that all of the text that follows is encoded with UTF8.

	Note: An explanation is found [here at
	Unicode.org](http://www.unicode.org/faq/utf_bom.html#25)

***************************************/

const uint8_t Burger::UTF8::ByteOrderMark[3] = {0xEF, 0xBB, 0xBF};

/*! ************************************

	\brief Check a single UTF8 byte pattern for validity

	Check the next 1 to 4 bytes to see if it comprises a valid UTF8 byte pattern
	and return true if they are, false, if not.

	Since UTF8 streams are variable length, there is no function that can take a
	singular value and check it for validity, you must use this function for
	single cases or Burger::UTF8::is_valid(const char *) for multi-character
	streams.

	\note This function is called Burger::UTF8::is_valid_single(const char *)
	because of a name conflict with Burger::UTF8::is_valid(const char *).

	\param pInput Pointer to a stream of 1 to 4 UTF8 encoded bytes, ``nullptr``
		will page fault.
	\return true if the next 1-4 bytes is a valid UTF8 stream, false if not.

***************************************/

uint_t BURGER_API Burger::UTF8::is_valid_single(
	const char* pInput) BURGER_NOEXCEPT
{
	// Check for the simple case of 0x00-0x7F
	uint_t uResult = TRUE;
	const uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];

	// UTF8?
	if (uFirst >= 0x80U) {

		// For fake gotos.
		for (;;) {

			// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

			// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid
			// codes) So, by checking for 0xC2 instead of 0xC0, I can kick out
			// the invalid codes of 0x00-0x7F by the two byte pattern.

			// Valid prefix code?
			if (uFirst >= 0xC2U) {

				// All subsequent bytes MUST be 0x80-0xBF. Anything else is
				// invalid. Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F
				const uint_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[1] ^ 0x80U;

				// Is byte #2 valid?
				if (uSecond < 0x40U) {

					// Check for the 11 bit patterns
					if (uFirst < 0xE0U) {
						// It's good!
						break;
					}

					// Now, try the 16 bit patterns

					// Is byte #3 valid?
					uint_t uTemp =
						reinterpret_cast<const uint8_t*>(pInput)[2] ^ 0x80U;
					if (uTemp < 0x40U) {
						if (uFirst < 0xF0U) {
							// 0x000-0x7FF and 0xD800-0xDFFF are invalid
							// Not 0x0000-0x07FF?
							if (((uFirst != 0xE0U) || (uSecond >= 0x20U)) &&
								// Not 0xD800-0xDFFF?
								((uFirst != 0xEDU) || (uSecond < 0x20U))) {
								break;
							}

							// Check for the 21 bit code of 0xF0-0xFF /
							// 0x80-0xBF / 0x80-0xBF / 0x80-0xBF

							// 0xF5 will make 0x140000, this is out of bounds.
						} else if (uFirst < 0xF5U) {

							// Is byte #4 valid?
							uTemp =
								reinterpret_cast<const uint8_t*>(pInput)[3] ^
								0x80U;
							if (uTemp < 0x40) {
								// Not 0x000000-0x010000?
								if (((uFirst != 0xF0) || (uSecond >= 0x10U)) &&
									// Not 0x110000-0x13FFFF?
									((uFirst != 0xF4) || (uSecond < 0x10U))) {
									break;
								}
							}
						}
					}
				}
			}
			// This is a bad UTF8 character
			uResult = FALSE;
			break;
		}
	}
	// Valid
	return uResult;
}

/*! ************************************

	\brief Check a UTF8 "C" string for validity

	Check a "C" string to see if it's a valid UTF8 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a
		valid UTF8 pattern.

	\param pInput Pointer to a zero terminated string, ``nullptr`` will page
		fault.
	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::UTF8::is_valid(const char* pInput) BURGER_NOEXCEPT
{
	// Check for the simple case of 0x00-0x7F
	uint_t uFirst;
	do {
		uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
		// Accept it
		++pInput;
		if (uFirst < 0x80U) {
			continue;
		}
		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

		// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid codes)
		// So, by checking for 0xC2 instead of 0xC0, I can kick out the invalid
		// codes of 0x00-0x7F by the two byte pattern.
		if (uFirst < 0xC2U) {
			// Valid prefix code?
			break;
		}

		// I now am at least 2 bytes long.
		// All subsequent bytes MUST be 0x80-0xBF. Anything else is invalid.

		// Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F
		const uint_t uSecond =
			reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;

		// Is byte #2 valid?

		if (uSecond >= 0x40U) {
			break;
		}

		// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
		if (uFirst < 0xE0U) {
			// It's good!
			continue;
		}

		// Is byte #3 valid?
		uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;
		if (uTemp >= 0x40U) {
			break;
		}

		// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
		if (uFirst < 0xF0U) {

			// 0x000-0x7FF and 0xD800-0xDFFF are invalid
			// Check for them
			// Is 0x0000-0x07FF?
			if (((uFirst == 0xE0U) && (uSecond < 0x20U)) ||
				// Is 0xD800-0xDFFF?
				((uFirst == 0xEDU) && (uSecond >= 0x20U))) {
				break;
			}
			continue;
		}

		// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF /
		// 0x80-0xBF

		// 0xF5 will make 0x140000, this is out of bounds.
		if (uFirst >= 0xF5U) {
			break;
		}

		// Is byte #4 valid?
		uTemp = reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;
		if (uTemp >= 0x40U) {
			break;
		}
		// Is 0x000000-0x010000?
		if (((uFirst == 0xF0) && (uSecond < 0x10U)) ||
			// Is 0x110000-0x13FFFF?
			((uFirst == 0xF4) && (uSecond >= 0x10U))) {
			break;
		}
	} while (uFirst);
	// If non-zero, then I aborted
	return !uFirst;
}

/*! ************************************

	\brief Check a UTF8 byte array for validity.

	Check a byte array and see if it's a valid UTF8 stream. Return \ref FALSE if
	there was an error, or \ref TRUE if the bytes represent a valid UTF8
	pattern.

	\param pInput Pointer to UTF8 data. Can be ``nullptr`` if uInputSize is
		zero, otherwise page fault.
	\param uInputSize Length of the data in bytes, if zero, then the function
		will return \ref TRUE.

	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::UTF8::is_valid(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	// End mark
	const char* pEndPtr = pInput + uInputSize;
	uint_t uTemp = 0;
	do {
		// Out of data at a stopping point?
		if (pEndPtr == pInput) {
			// Valid string
			return TRUE;
		}
		const uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
		// Accept it
		++pInput;

		// Check for the simple case of 0x00-0x7F
		if (uFirst < 0x80U) {
			continue;
		}

		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid

		// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid codes)
		// So, by checking for 0xC2 instead of 0xC0, I can kick out the invalid
		// codes of 0x00-0x7F by the two byte pattern.
		// Valid prefix code?
		if (uFirst < 0xC2U) {
			break;
		}

		// I now am at least 2 bytes long.
		// All subsequent bytes MUST be 0x80-0xBF. Anything else is invalid.

		// Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F
		if (pEndPtr == pInput) {
			break;
		}
		const uint_t uSecond =
			reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;

		// Is byte #2 valid?
		if (uSecond >= 0x40U) {
			break;
		}

		// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
		if (uFirst < 0xE0U) {
			// It's good!
			continue;
		}

		// Is byte #3 valid?
		if (pEndPtr == pInput) {
			break;
		}
		uTemp = reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;
		if (uTemp >= 0x40U) {
			break;
		}

		// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
		if (uFirst < 0xF0U) {

			// 0x000-0x7FF and 0xD800-0xDFFF are invalid
			// Check for them
			// Is 0x0000-0x07FF?
			if (((uFirst == 0xE0U) && (uSecond < 0x20U)) ||
				// Is 0xD800-0xDFFF?
				((uFirst == 0xEDU) && (uSecond >= 0x20U))) {
				break;
			}
			continue;
		}

		// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF /
		// 0x80-0xBF

		// 0xF5 will make 0x140000, this is out of bounds.
		if (uFirst >= 0xF5U) {
			break;
		}

		// Is byte #4 valid?
		if (pEndPtr == pInput) {
			break;
		}
		uTemp = reinterpret_cast<const uint8_t*>(pInput)[0] ^ 0x80U;
		++pInput;
		// Is 0x000000-0x010000?
		if (((uFirst == 0xF0U) && (uSecond < 0x10U)) ||
			// Is 0x110000-0x13FFFF?
			((uFirst == 0xF4U) && (uSecond >= 0x10U))) {
			break;
		}
	} while (uTemp < 0x40U);
	// This is a bad UTF8 string (nullptr point)
	return FALSE;
}

/*! ************************************

	\brief Return the number of bytes a UTF8 stream occupies.

	Check the UTF8 stream and determine if it's 1-4 bytes in length. No
	invalid data checking is performed. Use Burger::UTF8::IsValidSingle(const
	char *) instead.

	\param pInput Pointer to UTF8 data, ``nullptr`` will page fault.
	\return 1-4 for the token size.

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetTokenSize(
	const char* pInput) BURGER_NOEXCEPT
{
	return UTF8::TokenSizeTable[reinterpret_cast<const uint8_t*>(pInput)[0]];
}

/*! ************************************

	\brief Return the pointer to the next UTF8 token.

	Check the UTF8 stream and determine if it's 1-4 bytes in length, then return
	the supplied pointer incremented by that length. No invalid data checking is
	performed. Use Burger::UTF8::IsValidSingle(const char *) instead.

	\param pInput Pointer to UTF8 data, ``nullptr`` will page fault.
	\return pInput + the number of bytes the current token occupies.

***************************************/

const char* BURGER_API Burger::UTF8::NextToken(
	const char* pInput) BURGER_NOEXCEPT
{
	return pInput +
		UTF8::TokenSizeTable[reinterpret_cast<const uint8_t*>(pInput)[0]];
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a MacRomanUS char

	Take the unsigned 8 bit value of the MacRomanUS character and return the
	number of UTF8 bytes it will occupy. The answer is either 1, 2 or 3 bytes.
	If the input is greater than 0xFF, the returned value is zero.

	\param uInput MacRomanUS encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2, 3 or 0 if uInput is
		>=256.

	\sa GetMacRomanUSSize(const char*) or GetMacRomanUSSize(
		const char*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetMacRomanUSSize(
	uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uResult = 0U;
		} else {
			// The MacRomanUS table only has 2 or 3 byte tokens
			const uint8_t* pTable = MacRomanUS::g_ToUTF8Table[uInput - 0x80U];
			uResult = 2U;
			// Is there a third character?
			if (pTable[2]) {
				uResult = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a MacRomanUS "C" string.

	Take a "C" string, encoded with MacRomanUS, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pInput Pointer to a "C" string encoded in MacRomanUS

	\return The number of bytes the string would require if converted.

	\sa GetMacRomanUSSize(uint_t uInput) or GetMacRomanUSSize(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetMacRomanUSSize(
	const char* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			// ASCII?
			if (uTemp >= 0x80U) {
				// The MacRomanUS table only has 2 or 3 byte tokens
				const uint8_t* pTable = MacRomanUS::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a MacRomanUS buffer.

	Take a buffer, encoded with MacRomanUS, and determine the length in bytes
	this buffer would require if encoded in UTF8.

	\param pInput Pointer to a buffer encoded in MacRomanUS
	\param uInputSize Number of bytes in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetMacRomanUSSize(uint_t uInput) or GetMacRomanUSSize(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetMacRomanUSSize(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// ASCII?
			if (uTemp >= 0x80U) {
				// The MacRomanUS table only has 2 or 3 byte tokens
				const uint8_t* pTable = MacRomanUS::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a MacRomanUS 8 bit char into a UTF8 stream.

	Take the unsigned 8 bit value of the MacRomanUS character and convert it to
	a 1, 2 or 3 byte UTF8 stream. Only the UTF8 characters are written.

	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size,
	``nullptr`` will page fault.
	\param uInput MacRomanUS encoded 8 bit character

	\return The number of bytes written to the stream. 1, 2, 3 or 0 if uInput is
		>=256.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromMacRomanUS(
	char* pOutput, uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uSize;
	// ASCII?
	if (uInput >= 0x80U) {

		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uSize = 0U;
		} else {
			// The MacRomanUS table only has 2 or 3 byte tokens
			const uint8_t* pSrc = MacRomanUS::g_ToUTF8Table[uInput - 0x80U];
			reinterpret_cast<uint8_t*>(pOutput)[0] = pSrc[0];
			reinterpret_cast<uint8_t*>(pOutput)[1] = pSrc[1];

			// Temp = zero or char
			uint_t uTemp = pSrc[2];
			uSize = 2U;
			if (uTemp) {
				// It's 3 bytes in size
				reinterpret_cast<uint8_t*>(pOutput)[2] =
					static_cast<uint8_t>(uTemp);
				uSize = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		pOutput[0] = static_cast<char>(uInput);
		uSize = 1U;
	}
	return uSize;
}

/*! ************************************

	\brief Convert a MacRomanUS "C" string into a UTF8 stream.

	Take a "C" string that is using MacRomanUS encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
	``nullptr`` is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput MacRomanUS encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromMacRomanUS(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;
	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	++pInput;    // Accept it
	if (uTemp) { // Can I start the loop?
		do {

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The MacRomanUS table only has 2 or 3 byte tokens
				const uint8_t* pSrc = MacRomanUS::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
				}
			}
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (uTemp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a MacRomanUS byte array into a UTF8 stream.

	Take a byte array that is using MacRomanUS encoding and convert it to a UTF8
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
	\param pInput MacRomanUS encoded byte array, ``nullptr`` is okay if
		uInputSize is zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromMacRomanUS(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The MacRomanUS table only has 2 or 3 byte tokens
				const uint8_t* pSrc = MacRomanUS::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win1252 char

	Take the unsigned 8 bit value of the Win1252 character and return the
	number of UTF8 bytes it will occupy. The answer is either 1, 2 or 3 bytes.
	If the input is greater than 0xFF, the returned value is zero.

	\param uInput Win1252 encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2, 3 or 0 if uInput is
		>=256.

	\sa GetWin1252Size(const char*) or GetWin1252Size(
		const char*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin1252Size(uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uResult = 0U;
		} else {
			// The Win1252 table only has 2 or 3 byte tokens
			const uint8_t* pTable = Win1252::g_ToUTF8Table[uInput - 0x80U];
			uResult = 2U;
			// Is there a third character?
			if (pTable[2]) {
				uResult = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win1252 "C" string.

	Take a "C" string, encoded with Win1252, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pInput Pointer to a "C" string encoded in Win1252

	\return The number of bytes the string would require if converted.

	\sa GetWin1252Size(uint_t uInput) or GetWin1252Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin1252Size(
	const char* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			// ASCII?
			if (uTemp >= 0x80U) {
				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pTable = Win1252::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win1252 buffer.

	Take a buffer, encoded with Win1252, and determine the length in bytes
	this buffer would require if encoded in UTF8.

	\param pInput Pointer to a buffer encoded in Win1252
	\param uInputSize Number of bytes in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetWin1252Size(uint_t uInput) or GetWin1252Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin1252Size(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// ASCII?
			if (uTemp >= 0x80U) {
				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pTable = Win1252::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Win1252 8 bit char into a UTF8 stream.

	Take the unsigned 8 bit value of the Win1252 character and convert it to
	a 1, 2 or 3 byte UTF8 stream.

	\note This function will write a *zero* after the stream so you can
	assume that it's a valid "C" string.

	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size,
		``nullptr`` will page fault.
	\param uInput Win1252 encoded 8 bit character

	\return The number of bytes written to the stream. 1, 2, 3 or 0 if uInput is
		>=256.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin1252(
	char* pOutput, uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uSize;
	// ASCII?
	if (uInput >= 0x80U) {

		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uSize = 0U;
		} else {
			// The Win1252 table only has 2 or 3 byte tokens
			const uint8_t* pSrc = Win1252::g_ToUTF8Table[uInput - 0x80U];
			reinterpret_cast<uint8_t*>(pOutput)[0] = pSrc[0];
			reinterpret_cast<uint8_t*>(pOutput)[1] = pSrc[1];

			// Temp = zero or char
			uint_t uTemp = pSrc[2];
			uSize = 2U;
			if (uTemp) {
				// It's 3 bytes in size
				reinterpret_cast<uint8_t*>(pOutput)[2] =
					static_cast<uint8_t>(uTemp);
				uSize = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		pOutput[0] = static_cast<char>(uInput);
		uSize = 1U;
	}
	return uSize;
}

/*! ************************************

	\brief Convert a Win1252 "C" string into a UTF8 stream.

	Take a "C" string that is using Win1252 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.

	\param uOutputSize Size of the output buffer in bytes.
	\param pInput Win1252 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin1252(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;

	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t Temp = reinterpret_cast<const uint8_t*>(pInput)[0];
	++pInput; // Accept it
	// Can I start the loop?
	if (Temp) {
		do {

			// Get a char
			// ASCII?
			if (Temp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(Temp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pSrc = Win1252::g_ToUTF8Table[Temp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				Temp = pSrc[2];
				if (Temp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(Temp);
					}
					++pWorkPtr;
				}
			}
			Temp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (Temp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a Win1252 byte array into a UTF8 stream.

	Take a byte array that is using Win1252 encoding and convert it to a UTF8
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
	\param pInput Win1252 encoded byte array, ``nullptr`` is okay if uInputSize
		is zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin1252(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pSrc = Win1252::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win437 char

	Take the unsigned 8 bit value of the Win437 character and return the
	number of UTF8 bytes it will occupy. The answer is either 1, 2 or 3 bytes.
	If the input is greater than 0xFF, the returned value is zero.

	\param uInput Win437 encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2, 3 or 0 if uInput is
		>=256.

	\sa GetWin437Size(const char*) or GetWin437Size(
		const char*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin437Size(uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uResult = 0U;
		} else {
			// The Win1252 table only has 2 or 3 byte tokens
			const uint8_t* pTable = Win437::g_ToUTF8Table[uInput - 0x80U];
			uResult = 2U;
			// Is there a third character?
			if (pTable[2]) {
				uResult = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win437 "C" string.

	Take a "C" string, encoded with Win437, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pInput Pointer to a "C" string encoded in Win437

	\return The number of bytes the string would require if converted.

	\sa GetWin437Size(uint_t uInput) or GetWin437Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin437Size(
	const char* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			// ASCII?
			if (uTemp >= 0x80U) {
				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pTable = Win437::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win437 buffer.

	Take a buffer, encoded with Win437, and determine the length in bytes
	this buffer would require if encoded in UTF8.

	\param pInput Pointer to a buffer encoded in Win437
	\param uInputSize Number of bytes in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetWin437Size(uint_t uInput) or GetWin437Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetWin437Size(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// ASCII?
			if (uTemp >= 0x80U) {
				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pTable = Win437::g_ToUTF8Table[uTemp - 0x80U];
				uResult += 2U;
				// Is there a third character?
				if (pTable[2]) {
					++uResult;
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a Win437 8 bit char into a UTF8 stream.

	Take the unsigned 8 bit value of the Win437 character and convert it to
	a 1,2 or 3 byte UTF8 stream.

	\note This function will write a *zero* after the stream so you can
	assume that it's a valid "C" string.

	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size,
		``nullptr`` will page fault.
	\param uInput Win437 encoded 8 bit character

	\return The number of bytes written to the stream. 1, 2, 3 or 0 if uInput is
		>=256.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin437(
	char* pOutput, uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uSize;
	// ASCII?
	if (uInput >= 0x80U) {

		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uSize = 0U;
		} else {
			// The Win437 table only has 2 or 3 byte tokens
			const uint8_t* pSrc = Win437::g_ToUTF8Table[uInput - 0x80U];
			reinterpret_cast<uint8_t*>(pOutput)[0] = pSrc[0];
			reinterpret_cast<uint8_t*>(pOutput)[1] = pSrc[1];

			// Temp = zero or char
			uint_t uTemp = pSrc[2];
			uSize = 2U;
			if (uTemp) {
				// It's 3 bytes in size
				reinterpret_cast<uint8_t*>(pOutput)[2] =
					static_cast<uint8_t>(uTemp);
				uSize = 3U;
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		pOutput[0] = static_cast<char>(uInput);
		uSize = 1U;
	}
	return uSize;
}

/*! ************************************

	\brief Convert a Win437 "C" string into a UTF8 stream.

	Take a "C" string that is using Win437 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.

	\param uOutputSize Size of the output buffer in bytes.
	\param pInput Win437 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin437(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;

	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t Temp = reinterpret_cast<const uint8_t*>(pInput)[0];
	++pInput; // Accept it
	// Can I start the loop?
	if (Temp) {
		do {

			// Get a char
			// ASCII?
			if (Temp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(Temp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Win437 table only has 2 or 3 byte tokens
				const uint8_t* pSrc = Win437::g_ToUTF8Table[Temp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				Temp = pSrc[2];
				if (Temp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(Temp);
					}
					++pWorkPtr;
				}
			}
			Temp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (Temp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a Win437 byte array into a UTF8 stream.

	Take a byte array that is using Win437 encoding and convert it to a UTF8
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
	\param pInput Win437 encoded byte array, ``nullptr`` is okay if uInputSize
		is zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromWin437(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Win437 table only has 2 or 3 byte tokens
				const uint8_t* pSrc = Win437::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;

				// Write the optional third character
				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a ISOLatin1 char

	Take the unsigned 8 bit value of the ISOLatin1 character and return the
	number of UTF8 bytes it will occupy. The answer is either 1 or 2 bytes.
	If the input is greater than 0xFF, the returned value is zero.

	\param uInput ISOLatin1 encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2 or 0 if uInput is
		>=256.

	\sa GetISOLatin1Size(const char*) or GetISOLatin1Size(
		const char*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetISOLatin1Size(
	uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uResult = 0U;
		} else {
			uResult = 2U;
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a ISOLatin1 "C" string.

	Take a "C" string, encoded with ISOLatin1, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pInput Pointer to a "C" string encoded in ISOLatin1

	\return The number of bytes the string would require if converted.

	\sa GetISOLatin1Size(uint_t uInput) or GetISOLatin1Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetISOLatin1Size(
	const char* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			// ASCII?
			if (uTemp >= 0x80U) {
				uResult += 2U;
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a ISOLatin1 buffer.

	Take a buffer, encoded with ISOLatin1, and determine the length in bytes
	this buffer would require if encoded in UTF8.

	\param pInput Pointer to a buffer encoded in ISOLatin1
	\param uInputSize Number of bytes in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetISOLatin1Size(uint_t uInput) or GetISOLatin1Size(
		const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetISOLatin1Size(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// ASCII?
			if (uTemp >= 0x80U) {
				uResult += 2U;
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a ISOLatin1 8 bit char into a UTF8 stream.

	Take the unsigned 8 bit value of the ISOLatin1 character and convert it to
	a 1 or 2 byte UTF8 stream.

	\note This function will write a *zero* after the stream so you can
	assume that it's a valid "C" string.

	\param pOutput Pointer to UTF8 buffer that's a minimum 3 bytes in size,
		``nullptr`` will page fault.
	\param uInput ISOLatin1 encoded 8 bit character

	\return The number of bytes written to the stream. 1, 2 or 0 if uInput is
		>=256.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromISOLatin1(
	char* pOutput, uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uSize;
	// ASCII?
	if (uInput >= 0x80U) {

		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uSize = 0U;
		} else {
			// The ISOLatin1 table only has 2 byte tokens
			const uint8_t* pSrc = ISOLatin1::g_ToUTF8Table[uInput - 0x80U];
			reinterpret_cast<uint8_t*>(pOutput)[0] = pSrc[0];
			reinterpret_cast<uint8_t*>(pOutput)[1] = pSrc[1];
			uSize = 2U;
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		pOutput[0] = static_cast<char>(uInput);
		uSize = 1U;
	}
	return uSize;
}

/*! ************************************

	\brief Convert a ISOLatin1 "C" string into a UTF8 stream.

	Take a "C" string that is using ISOLatin1 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput ISOLatin1 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromISOLatin1(
	char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;
	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	// Accept it
	++pInput;
	// Can I start the loop?
	if (uTemp) {
		do {

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The ISOLatin1 table only has 2 byte tokens
				const uint8_t* pSrc = ISOLatin1::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;
			}
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (uTemp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a ISOLatin1 byte array into a UTF8 stream.

	Take a byte array that is using ISOLatin1 encoding and convert it to a UTF8
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
	\param pInput ISOLatin1 encoded byte array, ``nullptr`` is okay if
		uInputSize is zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromISOLatin1(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			const uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Win1252 table only has 2 byte tokens
				const uint8_t* pSrc = ISOLatin1::g_ToUTF8Table[uTemp - 0x80U];
				if (pWorkPtr < pEndPtr) {
					// Write the first character
					pWorkPtr[0] = pSrc[0];
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					// Write the second character
					pWorkPtr[0] = pSrc[1];
				}
				++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Determine the size of the UTF8 stream from a char

	Take the unsigned 8 bit value of the supplied character lookup table and
	return the number of UTF8 bytes it will occupy. The answer is either 1, 2, 3
	or 4 bytes. If the input is greater than 0xFF, the returned value is zero.

	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param uInput Table encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2, 3, 4 or 0 if uInput
		is >=256.

	\sa GetGenericSize(const uint8_t [128][4], const char*) or GetGenericSize(
		const uint8_t [128][4], const char*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetGenericSize(
	const uint8_t pTranslateTable[128][4], uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// 0x0100+ is bogus
			uResult = 0U;
		} else {
			// Scan the table if there is a translation
			const uint8_t* pTable = pTranslateTable[uInput - 0x80U];

			// Table marked as invalid entry?
			uResult = 0U;
			if (pTable[0]) {
				uResult = 1U;
				if (pTable[1]) {
					uResult = 2U;
					// Is there a third character?
					if (pTable[2]) {
						uResult = 3U;
						// Forth character?
						if (pTable[3]) {
							uResult = 4U;
						}
					}
				}
			}
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win1252 "C" string.

	Take a "C" string, encoded with Win1252, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput Pointer to a "C" string encoded in Win1252

	\return The number of bytes the string would require if converted.

	\sa GetGenericSize(const uint8_t [128][4], uint_t uInput) or GetGenericSize(
		const uint8_t [128][4], const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetGenericSize(
	const uint8_t pTranslateTable[128][4], const char* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			// ASCII?
			if (uTemp >= 0x80U) {
				// Scan the table if there is a translation
				const uint8_t* pTable = pTranslateTable[uTemp - 0x80U];

				// Table marked as invalid entry?
				if (pTable[0]) {
					++uResult;
					if (pTable[1]) {
						++uResult;
						// Is there a third character?
						if (pTable[2]) {
							++uResult;
							// Forth character?
							if (pTable[3]) {
								++uResult;
							}
						}
					}
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a Win1252 buffer.

	Take a buffer, encoded with Win1252, and determine the length in bytes
	this buffer would require if encoded in UTF8.


	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput Pointer to a buffer encoded in Win1252
	\param uInputSize Number of bytes in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetGenericSize(const uint8_t [128][4], uint_t uInput) or GetGenericSize(
		const uint8_t [128][4], const char *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetGenericSize(
	const uint8_t pTranslateTable[128][4], const char* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// ASCII?
			if (uTemp >= 0x80U) {
				// The Win1252 table only has 2 or 3 byte tokens
				const uint8_t* pTable = pTranslateTable[uTemp - 0x80U];
				// Table marked as invalid entry?
				if (pTable[0]) {
					++uResult;
					if (pTable[1]) {
						++uResult;
						// Is there a third character?
						if (pTable[2]) {
							++uResult;
							// Forth character?
							if (pTable[3]) {
								++uResult;
							}
						}
					}
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			++pInput;
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a generic 8 bit char into a UTF8 stream.

	Take the unsigned 8 bit value of the generic character and convert it to
	a 1 to 4 byte UTF8 stream. Codes 0 through 0x7f are considered ASCII while
	codes 0x80 through 0xFF will be found in the supplied table.

	The user supplied must contain valid UTF8 byte patterns. This routine will
	not perform validation on the contents of the table and if the table has bad
	data, the UTF8 stream produced by this function will be error prone.

	\note This function will write a *zero* after the stream so you can assume
	that it's a valid "C" string.

	\param pOutput Pointer to UTF8 buffer that's a minimum 5 bytes in size,
		``nullptr`` will page fault.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param uInput Generic encoded 8 bit character

	\return The number of bytes written to the stream. 1, 2, 3, 4 or 0 if uInput
		is >=256.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromGeneric(char* pOutput,
	const uint8_t pTranslateTable[128][4], uint_t uInput) BURGER_NOEXCEPT
{
	uint_t uResult = 0;
	// ASCII?
	if (uInput >= 0x80U) {
		// Still valid?
		if (uInput >= 0x100U) {
			// Nope
			pOutput[0] = 0;
		} else {

			// I need to get the size from the table
			const uint8_t* pSrc = pTranslateTable[uInput - 0x80U];
			uint_t uTemp = pSrc[0];
			if (uTemp) {
				reinterpret_cast<uint8_t*>(pOutput)[0] =
					static_cast<uint8_t>(uTemp);
				uResult = 1;

				uTemp = pSrc[1];
				if (uTemp) {
					reinterpret_cast<uint8_t*>(pOutput)[1] =
						static_cast<uint8_t>(uTemp);
					uResult = 2;

					uTemp = pSrc[2];
					if (uTemp) {
						reinterpret_cast<uint8_t*>(pOutput)[2] =
							static_cast<uint8_t>(uTemp);
						uResult = 3;

						uTemp = pSrc[3];
						if (uTemp) {
							reinterpret_cast<uint8_t*>(pOutput)[3] =
								static_cast<uint8_t>(uTemp);
							uResult = 4;
						}
					}
				}
			}
		}
	} else {
		pOutput[0] = static_cast<char>(uInput);
		uResult = 1;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a generic "C" string into a UTF8 stream.

	Take a "C" string that is using generic encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	The user supplied must contain valid UTF8 byte patterns. This routine will
	not perform validation on the contents of the table and if the table has bad
	data, the UTF8 stream produced by this function will be error prone.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput A generic encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromGeneric(char* pOutput,
	uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
	const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;
	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	// Accept it
	++pInput;
	// Can I start the loop?
	if (uTemp) {
		do {

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The Generic table can have 0 to 4 bytes
				const uint8_t* pSrc = pTranslateTable[uTemp - 0x80U];
				uTemp = pSrc[0];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
					uTemp = pSrc[1];
					if (uTemp) {
						if (pWorkPtr < pEndPtr) {
							pWorkPtr[0] = static_cast<uint8_t>(uTemp);
						}
						++pWorkPtr;
						uTemp = pSrc[2];
						if (uTemp) {
							if (pWorkPtr < pEndPtr) {
								pWorkPtr[0] = static_cast<uint8_t>(uTemp);
							}
							++pWorkPtr;
							uTemp = pSrc[3];
							if (uTemp) {
								if (pWorkPtr < pEndPtr) {
									pWorkPtr[0] = static_cast<uint8_t>(uTemp);
								}
								++pWorkPtr;
							}
						}
					}
				}
			}
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;
		} while (uTemp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a generic byte array into a UTF8 stream.

	Take a byte array that is using generic encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	The user supplied must contain valid UTF8 byte patterns. This routine will
	not perform validation on the contents of the table and if the table has bad
	data, the UTF8 stream produced by this function will be error prone.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
		if a zero was parsed. Zeros will be placed in the UTF8 stream as is.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput Generic encoded byte array, ``nullptr`` is okay if uInputSize
		is zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromGeneric(char* pOutput,
	uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
			} else {

				// The generic table has 0 to 4 bytes
				const uint8_t* pSrc = pTranslateTable[uTemp - 0x80U];
				uTemp = pSrc[0];
				if (uTemp) {
					if (pWorkPtr < pEndPtr) {
						// Write the first character
						pWorkPtr[0] = static_cast<uint8_t>(uTemp);
					}
					++pWorkPtr;
					uTemp = pSrc[1];
					if (uTemp) {
						if (pWorkPtr < pEndPtr) {
							// Write the second character
							pWorkPtr[0] = static_cast<uint8_t>(uTemp);
						}
						++pWorkPtr;

						// Write the optional third character
						uTemp = pSrc[2];
						if (uTemp) {
							if (pWorkPtr < pEndPtr) {
								pWorkPtr[0] = static_cast<uint8_t>(uTemp);
							}
							++pWorkPtr;
							uTemp = pSrc[3];
							if (uTemp) {
								if (pWorkPtr < pEndPtr) {
									pWorkPtr[0] = static_cast<uint8_t>(uTemp);
								}
								++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a UTF16 value

	Take the unsigned 16 bit value of the UTF16 character and return the
	number of UTF8 bytes it will occupy. The answer is either 1, 2 or 3 bytes.
	If the input is greater than 0xFF, the returned value is zero.

	\param uInput UTF16 encoded 8 bit character

	\return The number of bytes needed to UTF8 encode. 1, 2, 3 or 0 if uInput is
		>=256.

	\sa GetUTF16Size(const uint16_t*) or GetUTF16Size(
		const uint16_t*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetUTF16Size(uint_t uInput) BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// ASCII?
	if (uInput >= 0x80U) {
		// 0x80-0x7FF (11 bit encoding) ?
		if (uInput < 0x800U) {
			uResult = 2U;
		} else if ((uInput >= 0xD800U) && (uInput < 0xE000U)) {
			// 0xD800-0xDFFF or >=0x110000 are bad.
			uResult = 0U;
		} else {
			// 24 bit encoding
			uResult = 3U;
		}
	} else {
		// 0x00 - 0x7F is literal ASCII
		uResult = 1U;
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a UTF16 "C" string.

	Take a "C" string, encoded with UTF16, and determine the length in
	bytes this string would require if encoded in UTF8.

	\param pInput Pointer to a "C" string encoded in UTF16

	\return The number of bytes the string would require if converted.

	\sa GetUTF16Size(uint_t uInput) or GetUTF16Size(
		const uint16_t*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetUTF16Size(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	uint_t uTemp = pInput[0];
	if (uTemp) {
		do {
			++pInput;
			// ASCII?
			if (uTemp >= 0x80U) {
				// 0x80-0x7FF (11 bit encoding) ?
				if (uTemp < 0x800U) {
					uResult += 2U;
				} else if ((uTemp < 0xD800U) || (uTemp >= 0xE000U)) {
					// 24 bit encoding
					uResult += 3U;
					// Valid start token?
				} else if (uTemp < 0xDC00U) {
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)
					// Get the second token
					const uint_t uTemp2 = pInput[0] ^ 0xDC00U;
					// Valid?
					if (uTemp2 < 0x400U) {
						// Accept the second char and assume 32 bit encoding
						++pInput;
						uResult += 4U;
					}
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
			uTemp = pInput[0];
		} while (uTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Determine the size of the UTF8 stream for a UTF16 buffer.

	Take a buffer, encoded with UTF16, and determine the length in bytes
	this buffer would require if encoded in UTF8.

	\param pInput Pointer to a buffer encoded in UTF16
	\param uInputSize Number of elements in the buffer

	\return The number of bytes the buffer would require if converted.

	\sa GetUTF16Size(uint_t uInput) or GetUTF16Size(
		const uint16_t *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::UTF8::GetUTF16Size(
	const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;

	if (uInputSize) {
		do {
			uint_t uTemp = pInput[0];
			++pInput;
			// ASCII?
			if (uTemp >= 0x80U) {
				// 0x80-0x7FF (11 bit encoding) ?
				if (uTemp < 0x800U) {
					uResult += 2U;
				} else if ((uTemp < 0xD800U) || (uTemp >= 0xE000U)) {
					// 24 bit encoding
					uResult += 3U;
					// Valid start token?
				} else if (uTemp < 0xDC00U) {
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)
					// Get the second token
					const uint_t uTemp2 = pInput[0] ^ 0xDC00U;
					// Valid?
					if (uTemp2 < 0x400U) {
						// Accept the second char and assume 32 bit encoding
						++pInput;
						uResult += 4U;
					}
				}
			} else {
				// 0x00 - 0x7F is literal ASCII
				++uResult;
			}
		} while (--uInputSize);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF16 char into a UTF8 stream.

	Take the unsigned 16 bit value of the UTF16 character and convert it to
	a 1,2 or 3 byte UTF8 stream.

	\note This will NOT parse word pairs. It will return a 0 and not process the
	value if it's 0xD800-0xDFFF which is an escape token for UTF16.

	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size,
		``nullptr`` will page fault.
	\param uInput UTF16 encoded 16 bit character

	\return The number of bytes written to the stream. 1, 2, 3 or 0 if uInput is
		>=0xD800 and <=0xDFFF.

	\sa from_UTF16(char *,uintptr_t, const uint16_t *)

***************************************/

uintptr_t BURGER_API Burger::UTF8::from_UTF16(
	char* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
	// Convert to native data size
	const uint_t uTemp = static_cast<uint_t>(uInput);
	// ASCII?
	if (uTemp < 0x80U) {
		pOutput[0] = static_cast<char>(uTemp);
		return 1;
	}

	// 0x80-0x7FF (11 bit encoding) ?
	if (uTemp < 0x800U) {
		// Encode the upper 6 bits
		pOutput[0] = static_cast<char>((uTemp >> 6) | 0xC0U);
		// Encode the lower 6 bits
		pOutput[1] = static_cast<char>((uTemp & 0x3FU) | 0x80U);
		return 2;
	}
	if ((uTemp >= 0xD800U) && (uTemp < 0xE000U)) {
		// 0xD800-0xDFFF or >=0x110000 are bad.
		return 0;
	}
	// 16 bit encoding?
	pOutput[0] = static_cast<char>((uTemp >> 12U) | 0xE0U);
	pOutput[1] = static_cast<char>(((uTemp >> 6U) & 0x3FU) | 0x80U);
	pOutput[2] = static_cast<char>((uTemp & 0x3FU) | 0x80U);
	return 3;
}

/*! ************************************

	\brief Convert a UTF16 "C" string into a UTF8 stream.

	Take a "C" string that is using UTF16 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate
	the size of the possible output so the application can allocate a buffer
	large enough to hold it.

	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note If invalid UTF16 surrogate pair(s) are found, they will be skipped.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF16 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::from_UTF16(char* pOutput,
	uintptr_t uOutputSize, const uint16_t* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;
	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}
	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uTemp = pInput[0];
	// Accept it
	++pInput;
	// Can I start the loop?
	if (uTemp) {
		do {
			// Get a char
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
				++pWorkPtr;
				// 0x80-0x7FF (11 bit encoding)?
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
			} else if ((uTemp < 0xD800U) || (uTemp >= 0xE000U)) {
				// 16 bit encoding? (0x800-0xFFFF)
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>((uTemp >> 12U) | 0xE0U);
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] =
						static_cast<uint8_t>(((uTemp >> 6U) & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				// Valid start token?
			} else if (uTemp < 0xDC00U) {
				// Special case for 21 bit encoding? (0x10000-0x10FFFF)
				// Get the second token
				const uint_t uTemp2 = pInput[0] ^ 0xDC00U;
				// Valid?
				if (uTemp2 < 0x400U) {
					// Accept the second char
					++pInput;
					// Add in the 0x10000>>10 and remove 0xD800
					uTemp -= (0xD800 - 0x40);
					// Top 3 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 8U) | 0xF0U);
					}
					++pWorkPtr;
					// Next 6 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 2U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// 2 bits from uTemp, 4 bits from uTemp2
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp << 4U) & 0x30) | (uTemp2 >> 6U) | 0x80U);
					}
					++pWorkPtr;
					// The last 6 bits from uTemp2
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp2 & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
			// Get the next 16 bits
			uTemp = pInput[0];
			// Accept it
			++pInput;
		} while (uTemp);
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a UTF16 uint16_t array into a UTF8 stream.

	Take a uint16_t array that is using UTF16 encoding and convert it to a UTF8
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
		if a zero was parsed. Zeros will be placed in the UTF8 stream as is.

	\param pOutput Pointer to UTF8 buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF16 encoded uint16_t array, ``nullptr`` is okay if
		uInputSize is zero.
	\param uInputSize Size of the input uint16_t array in elements

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::from_UTF16(char* pOutput,
	uintptr_t uOutputSize, const uint16_t* pInput,
	uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);
	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;
	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}
	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	// Convert to shorts
	if (uInputSize) {
		do {
			// Get the next 16 bits
			uint_t uTemp = pInput[0];
			// Accept it
			++pInput;
			// ASCII?
			if (uTemp < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uTemp);
				}
				// Increment the output
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
			} else if ((uTemp < 0xD800U) || (uTemp >= 0xE000U)) {
				// 16 bit encoding? (0x800-0xFFFF)
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>((uTemp >> 12U) | 0xE0U);
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] =
						static_cast<uint8_t>(((uTemp >> 6U) & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				// Valid start token?
			} else if ((uTemp < 0xDC00U) && (uInputSize >= 2)) {
				// Special case for 21 bit encoding? (0x10000-0x10FFFF)
				// Get the second token
				const uint_t uTemp2 = pInput[0] ^ 0xDC00U;
				// Valid?
				if (uTemp2 < 0x400U) {
					// Accept the second char
					++pInput;
					--uInputSize;
					// Add in the 0x10000>>10 and remove 0xD800
					uTemp -= (0xD800 - 0x40);
					// Top 3 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp >> 8U) | 0xF0U);
					}
					++pWorkPtr;
					// Next 6 bits
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp >> 2U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// 2 bits from uTemp, 4 bits from uTemp2
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(
							((uTemp << 4U) & 0x30) | (uTemp2 >> 6U) | 0x80U);
					}
					++pWorkPtr;
					// The last 6 bits from uTemp2
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] =
							static_cast<uint8_t>((uTemp2 & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a UTF16 "C" string into an allocated UTF8 "C" string.

	Take a "C" string that is using UTF16 encoding and convert it to a UTF8
	encoded "C" string. The function will allocate a buffer large enough to
	store the string. When the string isn't needed anymore, release the memory
	with a call to Burger::Free(const void *)

	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.

	\note If invalid UTF16 surrogate pair(s) are found, they will be skipped.

	\param pInput UTF16 encoded "C" string, ``nullptr`` will page fault.
	\return A valid pointer to a UTF-8 version of the "C" string. ``nullptr`` if
		memory allocation failure.

	\sa Burger::Free(const void *)

***************************************/

char* BURGER_API Burger::UTF8::from_UTF16(const uint16_t* pInput) BURGER_NOEXCEPT
{
	const uintptr_t uInputLength = GetUTF16Size(pInput);
	char* pWork = static_cast<char*>(Alloc(uInputLength + 1));
	if (pWork) {
		// Copy the string
		from_UTF16(pWork, uInputLength + 1, pInput);
	}
	return pWork;
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

uintptr_t BURGER_API Burger::UTF8::from_UTF32(
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

uintptr_t BURGER_API Burger::UTF8::from_UTF32(char* pOutput,
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

uintptr_t BURGER_API Burger::UTF8::from_UTF32(char* pOutput,
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

/*! ************************************

	\brief Convert a UTF32 "C" string into an allocated UTF8 "C" string.

	Take a "C" string that is using UTF32 encoding and convert it to a UTF8
	encoded "C" string. The function will allocate a buffer large enough to
	store the string. When the string isn't needed anymore, release the memory
	with a call to Burger::Free(const void *)

	\note If invalid UTF32 codes are found, they will be skipped.

	\param pInput UTF32 encoded "C" string, ``nullptr`` will page fault.
	\return A valid pointer to a UTF-8 version of the "C" string. ``nullptr`` if
		memory allocation failure.

	\sa Burger::Free(const void *)

***************************************/

char* BURGER_API Burger::UTF8::from_UTF32(const uint32_t* pInput) BURGER_NOEXCEPT
{
	const uintptr_t uInputLength = from_UTF32(nullptr, 0, pInput);
	char* pWork = static_cast<char*>(Alloc(uInputLength + 1));
	if (pWork) {
		// Copy the string
		from_UTF32(pWork, uInputLength + 1, pInput);
	}
	return pWork;
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic 8 bit char.

	Take a 1 to 4 byte UTF8 stream and look up the unsigned 8 bit value of the
	generic character. Codes 0 through 0x7f are considered ASCII while codes
	0x80 through 0xFF will be found in the supplied table.

	\note This function does a linear search from the table, as a result, it's
		not very fast on strings with a lot of high ascii characters

	\param pInput Pointer to UTF8 buffer that contains the valid stream to
		convert, ``nullptr`` will page fault.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.

	\return The unsigned 8 bit character code (0x00-0xFF) or
		Burger::UTF8::kInvalid if the UTF8 value wasn't low ASCII and couldn't
		be found in the table.

***************************************/

uint_t BURGER_API Burger::UTF8::ToGeneric(
	const char* pInput, const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT
{
	// Get a UTF-8 char
	const uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uFirst < 0x80U) {
		// Easy!!! I like it!
		return uFirst;
	}
	// Great, it's not low ASCII. Do it the hard way.
	// Number of attempts
	uint_t uCounter = 128;
	do {
		if (uFirst == pTranslateTable[0][0]) {
			uint_t uTemp = pTranslateTable[0][1];
			if (!uTemp) {
				break;
			}
			if (reinterpret_cast<const uint8_t*>(pInput)[1] == uTemp) {
				uTemp = pTranslateTable[0][2];
				if (!uTemp) {
					break;
				}
				if (reinterpret_cast<const uint8_t*>(pInput)[2] == uTemp) {
					uTemp = pTranslateTable[0][3];
					if (!uTemp) {
						break;
					}
					if (reinterpret_cast<const uint8_t*>(pInput)[3] == uTemp) {
						break;
					}
				}
			}
		}
		// Next 4 byte entry
		++pTranslateTable;
	} while (--uCounter);
	if (uCounter) {
		// Return 0x80-0xFF
		return 256U - uCounter;
	}
	// Crud
	return kInvalid;
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic "C" string.

	Take a "C" string that is using UTF8 encoding and convert it to a generic
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\param pOutput Pointer to byte buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput A UTF8 encoded "C" string, ``nullptr`` will page fault.

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::ToGeneric(char* pOutput,
	uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
	const char* pInput) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// If nonzero, then I append a 0 to the string.
	const uintptr_t uAddZero = uOutputSize;

	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}

	// This is the end of the valid buffer
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
	// Accept it
	++pInput;
	// Can I start the loop?
	if (uFirst) {
		do {

			// Get a char
			// ASCII?
			if (uFirst < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uFirst);
				}
				// Increment the output
				++pWorkPtr;
			} else if (uFirst >= 0xC2U) {
				// Make a copy of the pointer
				const uint8_t(*pTranslateTable2)[4] = pTranslateTable;

				// Great, it's not low ASCII. Do it the hard way.
				// Number of attempts
				uintptr_t uCounter = 128;
				do {
					if (uFirst == pTranslateTable2[0][0]) {
						uint_t uTemp = pTranslateTable2[0][1];
						if (!uTemp) {
							// Only one char?
							break;
						}
						if (reinterpret_cast<const uint8_t*>(pInput)[0] ==
							uTemp) {
							uTemp = pTranslateTable2[0][2];
							if (!uTemp) {
								// I took two chars
								++pInput;
								break;
							}
							if (reinterpret_cast<const uint8_t*>(pInput)[1] ==
								uTemp) {
								uTemp = pTranslateTable2[0][3];
								if (!uTemp) {
									// I took three chars
									pInput += 2;
									break;
								}
								if (reinterpret_cast<const uint8_t*>(
										pInput)[2] == uTemp) {
									// I took four chars
									pInput += 3;
									break;
								}
							}
						}
					}
					++pTranslateTable2; // Next 4 byte entry
				} while (--uCounter);

				if (uCounter) { // Found it?
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(256 - uCounter);
					}
					++pWorkPtr;
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

	// Return the equivalent of strlen()
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic byte array.

	Take a byte array that is using UTF8 encoding and convert it to a generic
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be ``nullptr`` to have this routine
	calculate the size of the possible output so the application can allocate a
	buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
if a zero was parsed. Zeros will be placed in the generic stream as is.

	\param pOutput Pointer to a byte buffer to receive the converted string,
		``nullptr`` is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion
		table.
	\param pInput UTF8 encoded byte array, ``nullptr`` is okay if uInputSize is
		zero.
	\param uInputSize Size of the input byte array

	\return Byte count of the potential output. It is valid, even if the output
		buffer wasn't large enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::ToGeneric(char* pOutput,
	uintptr_t uOutputSize, const uint8_t pTranslateTable[128][4],
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// Can I save a zero?
	const uintptr_t uAddZero = uOutputSize;

	// Valid for anything?
	if (uOutputSize) {
		// Make room for the zero
		--uOutputSize;
	}

	// Can't write, ever
	uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string
	if (uInputSize) {
		do {
			const uint_t uFirst = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Accept it
			++pInput;

			// Get a char
			// ASCII?
			if (uFirst < 0x80U) {
				// Can I write?
				if (pWorkPtr < pEndPtr) {
					pWorkPtr[0] = static_cast<uint8_t>(uFirst);
				}
				// Increment the output
				++pWorkPtr;
			} else if (uFirst >= 0xC2U) {

				// Make a copy of the pointer
				const uint8_t(*pTranslateTable2)[4] = pTranslateTable;

				// Great, it's not low ASCII. Do it the hard way.
				// Number of attempts
				uint_t uCounter = 128;
				do {
					if (uFirst == pTranslateTable2[0][0]) {
						uint_t uTemp = pTranslateTable2[0][1];
						if (!uTemp) {
							break;
						}
						if ((uInputSize >= 2) &&
							(reinterpret_cast<const uint8_t*>(pInput)[0] ==
								uTemp)) {
							uTemp = pTranslateTable2[0][2];
							if (!uTemp) {
								++pInput;
								--uInputSize;
								break;
							}
							if ((uInputSize >= 3) &&
								(reinterpret_cast<const uint8_t*>(pInput)[1] ==
									uTemp)) {
								uTemp = pTranslateTable2[0][3];
								if (!uTemp) {
									pInput += 2;
									uInputSize -= 2;
									break;
								}
								if ((uInputSize >= 4) &&
									(reinterpret_cast<const uint8_t*>(
										 pInput)[2] == uTemp)) {
									pInput += 3;
									uInputSize -= 3;
									break;
								}
							}
						}
					}
					// Next 4 byte entry
					++pTranslateTable2;
				} while (--uCounter);

				// Found it?
				if (uCounter) {
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(256 - uCounter);
					}
					++pWorkPtr;
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
	return static_cast<uintptr_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}
