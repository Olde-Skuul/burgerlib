/***************************************

	Common code for code pages

	Copyright (c) 2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcodepage.h"

/*! ************************************

	\class Burger::CodePage
	\brief Shared functions for code page handlers.

	Burgerlib uses UTF8 as the default text encoding, but some operating systems
	use other ASCII encodings for text output and / or filenames. This class
	contains common functions shared by all of the code page translators.

	\sa ISOLatin1, Win437, Win1252, or MacRomanUS

***************************************/

/*! ************************************

	\var Burger::CodePage::kInvalid
	\brief Value returned if a routine failed.

	This is the value returned in the event of a function error condition. Test
	for this value to see if a function failed.

***************************************/

/*! ************************************

	\brief Translate a single UTF8 stream character into a mapped 8 bit char.

	UTF8 can be a maximum of 4 bytes per character. Scan a 128 entry table for a
	match and if found, return the index + 128 as the mapped code page. This
	function is used to convert UTF8 to an 8 bit encoding. kInvalid is returned
	if there is no match.

	The table will have 4 bytes per character, with zeros filling all unused
	entries in case the entry is 2 or 3 values in length.

	\note This function will not return the number of bytes decoded. Use
		Burger::UTF8::NextToken(const char *) to get the pointer to the next
		UTF8 entry.

	\param pInput Pointer to UTF8 buffer that contains the 1 to 4 byte buffer to
		convert. nullptr will page fault.
	\param pTranslateTable Pointer to a 128 entry code page lookup table that
		would convert the code page to UTF8 for a reverse lookup.

	\return The unsigned 8 bit character code (0x00-0xFF) or
		Burger::CodePage::kInvalid if the UTF8 value wasn't low ASCII and
		couldn't be mapped to by the translation table.

	\sa Burger::UTF8::NextToken(const char *) or
		Burger::UTF8::GetTokenSize(const char *)

***************************************/

uint_t BURGER_API Burger::CodePage::TranslateFromUTF8(
	const char* pInput, const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT
{
	// Get the first UTF8 character code
	uint_t uResult = reinterpret_cast<const uint8_t*>(pInput)[0];

	// If it's less than 128, no further work is needed.
	if (uResult >= 0x80U) {

		// It's not low ASCII. Do it the hard way.
		uintptr_t uCounter = 128;

		// Cache the second char (The table is 2 to 4 valid tokens only)
		const uint_t uFirst = uResult;
		const uint_t uSecond = reinterpret_cast<const uint8_t*>(pInput)[1];

		// Assume failure
		uResult = kInvalid;
		do {
			// So far, so good? All tables have valid first two entries.
			if ((uFirst == pTranslateTable[0][0]) &&
				(uSecond == pTranslateTable[0][1])) {

				// Is this a 3 byte pattern?
				uint_t uTemp = pTranslateTable[0][2];
				// No? Then the code was matched.
				if (!uTemp) {
					uResult = static_cast<uint_t>(256 - uCounter);
					break;
				}

				// Try matching a 3 byte pattern.
				if (reinterpret_cast<const uint8_t*>(pInput)[2] == uTemp) {

					// Is this a 4 byte pattern?
					uTemp = pTranslateTable[0][3];
					if (!uTemp ||
						(reinterpret_cast<const uint8_t*>(pInput)[3] ==
							uTemp)) {
						// Match a 3 or 4 byte pattern.
						uResult = static_cast<uint_t>(256 - uCounter);
						break;
					}
				}
			}
			// Next 4 byte entry
			++pTranslateTable;
		} while (--uCounter);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF8 stream into a ISOLatin1 "C" string.

	Take a "C" string that is using UTF8 encoding and convert it into a
	ISOLatin1 encoded "C" string. The function will return the size of the
	string after encoding. This size is valid, even if it exceeded the output
	buffer size. The output pointer and size can be nullptr to have this
	routine calculate the size of the possible output so the application can
	allocate a buffer large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
		even if truncation is necessary to get it to fit in the output buffer.
		Under no circumstances will the output buffer be overrun.

	\param pOutput Pointer to byte buffer to receive the ISOLatin1 encoded
		string. nullptr is okay if uOutputSize is zero, otherwise it will page
		fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput A UTF8 encoded "C" string. nullptr will page fault.
	\param pTranslateTable Pointer to a 128 entry code page lookup table that
		would convert the code page to UTF8 for a reverse lookup.

	\return Burger::StringLength() of the potential output. It is valid, even if
		the output buffer wasn't large enough to contain everything.

***************************************/

uint_t BURGER_API Burger::CodePage::TranslateFromUTF8(char* pOutput,
	uintptr_t uOutputSize, const char* pInput,
	const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// If nonzero, then I append a 0 to the string.
	const uintptr_t bAddZero = uOutputSize;
	// Valid?
	if (uOutputSize) {
		--uOutputSize;
	}

	// This is the end of the valid buffer
	const uint8_t* pEndPtr = pWorkPtr + uOutputSize;

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

				// Valid multibyte opcode?
			} else if (uFirst >= 0xC2U) {

				// Make a copy of the pointer
				const uint8_t(*pTranslateTable2)[4] = pTranslateTable;

				// Great, it's not low ASCII. Do it the hard way.

				// Number of attempts
				uint_t uCounter = 128;
				const uint_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0];
				do {
					if ((uFirst == pTranslateTable2[0][0]) &&
						(uSecond == pTranslateTable2[0][1])) {
						const uint_t uTemp = pTranslateTable2[0][2];
						// 2 byte code?
						if (!uTemp) {
							++pInput;
							break;
						}
						// 3 byte code?
						if ((reinterpret_cast<const uint8_t*>(pInput)[1] ==
								uTemp)) {
							pInput += 2;
							break;
						}
					}

					// Next 4 byte entry
					++pTranslateTable2;
				} while (--uCounter);

				// Did I get a valid code?
				if (uCounter) {

					// Write it out
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
	if (bAddZero) {

		// Write it, but don't add it to the strlen()
		pWorkPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uint_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a ISOLatin1 byte array.

	Take a byte array that is using UTF8 encoding and convert it to a ISOLatin1
	encoded "C" string. The function will return the size of the string after
	encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be nullptr to have this routine calculate
	the size of the possible output so the application can allocate a buffer
	large enough to hold it.

	\note This function will ensure that the string is always zero terminated,
		even if truncation is necessary to get it to fit in the output buffer.
		Under no circumstances will the output buffer be overrun.

	\note Zeros can be encoded into the stream. This function will not early out
		if a zero was parsed. Zeros will be placed in the ISOLatin1 stream as
		is.

	\param pOutput Pointer to a byte buffer to receive the ISOLatin1 string.
		nullptr is okay if uOutputSize is zero, otherwise a page fault will
		occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF8 encoded byte array. nullptr is okay if uInputSize is
		zero.
	\param uInputSize Size of the input byte array.
	\param pTranslateTable Pointer to a 128 entry code page lookup table that
		would convert the code page to UTF8 for a reverse lookup.

	\return Burger::StringLength() of the potential output. It is valid, even if
		the output buffer wasn't large enough to contain everything.

***************************************/

uint_t BURGER_API Burger::CodePage::TranslateFromUTF8(char* pOutput,
	uintptr_t uOutputSize, const char* pInput, uintptr_t uInputSize,
	const uint8_t pTranslateTable[128][4]) BURGER_NOEXCEPT
{
	uint8_t* pWorkPtr = reinterpret_cast<uint8_t*>(pOutput);

	// Can I save a zero?
	const uintptr_t bAddZero = uOutputSize;

	// Valid for anything?
	if (uOutputSize) {

		// Make room for the zero
		--uOutputSize;
	}

	// Can't write, ever
	const uint8_t* pEndPtr = pWorkPtr + uOutputSize;

	// Let's convert the string

	// Sanity check
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
				// Must have at least 2 bytes pending
				++pWorkPtr;

			} else if ((uFirst >= 0xC2U) && (uInputSize >= 2)) {

				// Make a copy of the pointer
				const uint8_t(*pTranslateTable2)[4] = pTranslateTable;

				// Make a copy of the pointer
				const uint_t uSecond =
					reinterpret_cast<const uint8_t*>(pInput)[0];

				// Great, it's not low ASCII. Do it the hard way.
				// Number of attempts
				uint_t uCounter = 128;
				do {
					if ((uFirst == pTranslateTable2[0][0]) &&
						(uSecond == pTranslateTable2[0][1])) {
						const uint_t uTemp = pTranslateTable2[0][2];
						if (!uTemp) {
							// Accept it
							++pInput;
							--uInputSize;
							break;
						}
						if ((uInputSize >= 3) &&
							(reinterpret_cast<const uint8_t*>(pInput)[1] ==
								uTemp)) {
							// Accept the three bytes
							pInput += 2;
							uInputSize -= 2;
							break;
						}
					}
					// Next 4 byte entry
					++pTranslateTable2;
				} while (--uCounter);

				// Did I get a valid code?
				if (uCounter) {

					// Write it out
					if (pWorkPtr < pEndPtr) {
						pWorkPtr[0] = static_cast<uint8_t>(256 - uCounter);
					}
					++pWorkPtr;
				}
			}
		} while (--uInputSize);
	}

	// Can I add a trailing zero?
	if (bAddZero) {

		// Write it, but don't add it to the strlen()
		pWorkPtr[0] = 0;
	}

	// Return the equivalent of strlen()
	return static_cast<uint_t>(reinterpret_cast<char*>(pWorkPtr) - pOutput);
}