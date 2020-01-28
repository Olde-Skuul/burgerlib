/***************************************

	String handlers for UTF8 support
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf8.h"
#include "brmacromanus.h"
#include "brwin1252.h"
#include "brisolatin1.h"
#include "brglobalmemorymanager.h"

/***************************************

	Here are the invalid UTF8 patterns that this code
	is using for error detection.
	
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
	
	UTF8 is a format that allows <a href="http://www.unicode.org">Unicode</a>
	data to be stored in a standard "C" string with little modification to
	most existing string managers. All Burgerlib functions accept UTF8 strings
	so that they can properly present international characters in a consistent
	manner across numerous target platforms.

***************************************/

/*! ************************************

	\var Burger::UTF8::BAD
	\brief Value returned if a routine failed.
	
	If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
	return this value instead. Please see the documentation for each function
	to know which ones use true/false pairs or this value.
	
***************************************/


/*! ************************************

	\var const Word8 Burger::UTF8::TokenSizeTable[256]
	\brief Table to determine the size of a UTF8 token stream.
	
	Using the first byte as an index, obtain the size of the stream in 
	bytes from this table. Entries are the numbers 1-4. This table
	shouldn't be used for error checking and it's only for quick look ups
	on valid UTF8 streams.

***************************************/

const Word8 BURGER_ALIGN(Burger::UTF8::TokenSizeTable[256],16) = {
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x00
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x10
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x20
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x30
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x40
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x50
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x60
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x70
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x80
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0x90
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0xA0
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	// 0xB0
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,	// 0xC0
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,	// 0xD0
	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,	// 0xE0
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04		// 0xF0
};

/*! ************************************

	\brief UTF8 text file signature.

	If a raw text file starts with this three byte pattern,
	you're supposed to assume that all of the text that follows is
	encoded with UTF8.
	
	Note: An explanation is found <a href="http://www.unicode.org/faq/utf_bom.html#25">
	here at Unicode.org</a>
	
***************************************/

const Word8 Burger::UTF8::ByteOrderMark[3] = {0xEF,0xBB,0xBF};

/*! ************************************

	\brief Check a single UTF8 byte pattern for validity
	
	Check the next 1 to 4 bytes to see if it comprises a valid UTF8 byte pattern and
	return true if they are, false, if not.
	
	Since UTF8 streams are variable length, there is no function that can take a
	singular value and check it for validity, you must use this function for single
	cases or Burger::UTF8::IsValid(const char *) for multi-character streams.
	
	\note This function is called Burger::UTF8::IsValidSingle(const char *) because of a
	name conflict with Burger::UTF8::IsValid(const char *).
	
	\param pInput Pointer to a stream of 1 to 4 UTF8 encoded bytes. \ref NULL will page fault.
	\return true if the next 1-4 bytes is a valid UTF8 stream, false if not.
		
***************************************/

Word BURGER_API Burger::UTF8::IsValidSingle(const char *pInput)
{
	// Check for the simple case of 0x00-0x7F
	
	Word uResult = TRUE;
	Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];

	if (uFirst>=0x80U) {		// UTF8?
		for (;;) {
		
			// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
		
			// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid codes)
			// So, by checking for 0xC2 instead of 0xC0, I can kick out the invalid
			// codes of 0x00-0x7F by the two byte pattern.
		
			if (uFirst>=0xC2U) {		// Valid prefix code?
			
				// All subsequent bytes MUST be 0x80-0xBF. Anything else is invalid.
				// Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F

				Word uSecond = reinterpret_cast<const Word8*>(pInput)[1]^0x80U;
			
				// Is byte #2 valid?
				
				if (uSecond<0x40U) {
			
				// Check for the 11 bit patterns

					if (uFirst<0xE0U) {
						break;		// It's good!
					}

				// Now, try the 16 bit patterns
				
				// Is byte #3 valid?
					
					Word uTemp = reinterpret_cast<const Word8*>(pInput)[2]^0x80U;
					if (uTemp<0x40) {

						if (uFirst<0xF0U) {
					
				// 0x000-0x7FF and 0xD800-0xDFFF are invalid
							
							if (((uFirst!=0xE0U) || (uSecond>=0x20U)) &&	// Not 0x0000-0x07FF?
								((uFirst!=0xEDU) || (uSecond<0x20U))) {	// Not 0xD800-0xDFFF?
								break;
							}
					
							
				// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
				
				// 0xF5 will make 0x140000, this is out of bounds.
				
						} else if (uFirst<0xF5) {
						
							// Is byte #4 valid?
							
							uTemp = reinterpret_cast<const Word8*>(pInput)[3]^0x80U;
							if (uTemp<0x40) {
								if (((uFirst!=0xF0) || (uSecond>=0x10U)) &&	// Not 0x000000-0x010000?
									((uFirst!=0xF4) || (uSecond<0x10U))) {		// Not 0x110000-0x13FFFF?
									break;
								}
							}
						}
					}
				}
			}
			uResult = FALSE;		// This is a bad UTF8 character
			break;
		}
	}
	return uResult;		// Valid
}

/*! ************************************

	\brief Check a UTF8 "C" string for validity
	
	Check a "C" string to see if it's a valid UTF8 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a valid UTF8
	pattern.
	
	\param pInput Pointer to a zero terminated string. \ref NULL will page fault.
	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF8::IsValid(const char *pInput)
{
	// Check for the simple case of 0x00-0x7F
	
	Word uFirst;
	do {
		uFirst = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;					// Accept it
		if (uFirst<0x80U) {
			continue;
		}	
	// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
		
	// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid codes)
	// So, by checking for 0xC2 instead of 0xC0, I can kick out the invalid
	// codes of 0x00-0x7F by the two byte pattern.
		
		if (uFirst<0xC2U) {		// Valid prefix code?
			break;
		}
			
	// I now am at least 2 bytes long.
	// All subsequent bytes MUST be 0x80-0xBF. Anything else is invalid.
			
	// Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F
		Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
						
	// Is byte #2 valid?
			
		if (uSecond>=0x40U) {
			break;
		}

	// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF

		if (uFirst<0xE0U) {
			continue;		// It's good!
		}

		// Is byte #3 valid?
			
		Word uTemp = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
		if (uTemp>=0x40) {
			break;
		}
			
	// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF

		if (uFirst<0xF0U) {
					
			// 0x000-0x7FF and 0xD800-0xDFFF are invalid
			// Check for them
						
			if (((uFirst==0xE0U) && (uSecond<0x20U)) ||	// Is 0x0000-0x07FF?
				((uFirst==0xEDU) && (uSecond>=0x20U))) {	// Is 0xD800-0xDFFF?
				break;
			}
			continue;
		}
			
		// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
			
		// 0xF5 will make 0x140000, this is out of bounds.
			
		if (uFirst>=0xF5) {
			break;
		}
		// Is byte #4 valid?
						
		uTemp = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
		if (uTemp>=0x40) {
			break;
		}
		if (((uFirst==0xF0) && (uSecond<0x10U)) ||		// Is 0x000000-0x010000?
			((uFirst==0xF4) && (uSecond>=0x10U))) {	// Is 0x110000-0x13FFFF?
			break;
		}
	} while (uFirst);
	return !uFirst;		// If non-zero, then I aborted
}

/*! ************************************

	\brief Check a UTF8 byte array for validity.
	
	Check a byte array and see if it's a valid UTF8 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a valid UTF8
	pattern.
	
	\param pInput Pointer to UTF8 data. Can be \ref NULL if uInputSize is zero, otherwise page fault.
	\param uInputSize Length of the data in bytes, if zero, then the function will return \ref TRUE.
	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF8::IsValid(const char *pInput,WordPtr uInputSize)
{
	const char *pEndPtr = pInput+uInputSize;		// End mark
	Word uTemp=0;
	do {
		// Out of data at a stopping point?
		if (pEndPtr==pInput) {
			return TRUE;			// Valid string
		}
		Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;					// Accept it
	
		// Check for the simple case of 0x00-0x7F
		if (uFirst<0x80U) {
			continue;
		}
			
	// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			
	// 0xC0 generates 0x00-0x3F and 0xC1 generates 0x40-0x7F (Invalid codes)
	// So, by checking for 0xC2 instead of 0xC0, I can kick out the invalid
	// codes of 0x00-0x7F by the two byte pattern.
		
		if (uFirst<0xC2U) {		// Valid prefix code?
			break;
		}
				
	// I now am at least 2 bytes long.
	// All subsequent bytes MUST be 0x80-0xBF. Anything else is invalid.
			
	// Use xor with 0x80 to convert 0x80-0xBF to 0x00-0x3F
		if (pEndPtr==pInput) {
			break;
		}
		Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
							
	// Is byte #2 valid?
				
		if (uSecond>=0x40U) {
			break;
		}
	
	// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
		if (uFirst<0xE0U) {
			continue;		// It's good!
		}

		// Is byte #3 valid?
				
		if (pEndPtr==pInput) {
			break;
		}
		uTemp = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
		if (uTemp>=0x40) {
			break;
		}
				
	// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF

		if (uFirst<0xF0U) {
					
			// 0x000-0x7FF and 0xD800-0xDFFF are invalid
			// Check for them
							
			if (((uFirst==0xE0U) && (uSecond<0x20U)) ||	// Is 0x0000-0x07FF?
				((uFirst==0xEDU) && (uSecond>=0x20U))) {	// Is 0xD800-0xDFFF?
				break;
			}
			continue;
		}
				
		// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
				
		// 0xF5 will make 0x140000, this is out of bounds.
				
		if (uFirst>=0xF5) {
			break;
		}
		// Is byte #4 valid?
							
		if (pEndPtr==pInput) {
			break;
		}
		uTemp = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;
		++pInput;
		if (((uFirst==0xF0) && (uSecond<0x10U)) ||		// Is 0x000000-0x010000?
			((uFirst==0xF4) && (uSecond>=0x10U))) {	// Is 0x110000-0x13FFFF?
			break;
		}
	} while (uTemp<0x40);
	return FALSE;	// This is a bad UTF8 string (\ref NULL point)
}

/*! ************************************

	\brief Return the number of bytes a UTF8 stream occupies.
	
	Check the UTF8 stream and determine if it's 1-4 bytes in length. No
	invalid data checking is performed. Use Burger::UTF8::IsValidSingle(const char *)
	instead.
	
	\param pInput Pointer to UTF8 data. \ref NULL will page fault.
	\return 1-4 for the token size. 
		
***************************************/

WordPtr BURGER_API Burger::UTF8::GetTokenSize(const char *pInput)
{
	return UTF8::TokenSizeTable[reinterpret_cast<const Word8*>(pInput)[0]];
}

/*! ************************************

	\brief Return the pointer to the next UTF8 token.
	
	Check the UTF8 stream and determine if it's 1-4 bytes in length, then return
	the supplied pointer incremented by that length. No
	invalid data checking is performed. Use Burger::UTF8::IsValidSingle(const char *)
	instead.
	
	\param pInput Pointer to UTF8 data. \ref NULL will page fault.
	\return pInput + the number of bytes the current token occupies.
		
***************************************/

const char * BURGER_API Burger::UTF8::NextToken(const char *pInput)
{
	return pInput+UTF8::TokenSizeTable[reinterpret_cast<const Word8*>(pInput)[0]];
}

/*! ************************************

	\brief Convert a MacRomanUS 8 bit char into a UTF8 stream.
	
	Take the unsigned 8 bit value of the MacRomanUS character and convert it to
	a 1,2 or 3 byte UTF8 stream.
	
	\note This function will write a <b>zero</b> after the stream so you can assume
	that it's a valid "C" string.
	
	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size. \ref NULL will page fault.
	\param uInput MacRomanUS encoded 8 bit character
	\return The number of bytes written to the stream. 1, 2, 3 or 0 if 
		uInput is >=256.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromMacRomanUS(char *pOutput,Word uInput)
{
	if (uInput<0x80U) {		// ASCII?
		pOutput[0] = static_cast<char>(uInput);
		pOutput[1] = 0;
		return 1;
	}
	if (uInput<0x100U) {		// Still valid?
		
		// The MacRomanUS table only has 2 or 3 byte tokens
			
		const Word8 *pSrc = MacRomanUS::ToUTF8Table[uInput-0x80U];
		reinterpret_cast<Word8*>(pOutput)[0] = pSrc[0];
		reinterpret_cast<Word8*>(pOutput)[1] = pSrc[1];
		
		// Temp = zero or char
		Word uTemp = pSrc[2];
		reinterpret_cast<Word8*>(pOutput)[2] = static_cast<Word8>(uTemp);
		if (uTemp) {
			pOutput[3] = 0;		// It's a char
			return 3;
		}
		return 2;
	}
	pOutput[0] = 0;		// No good
	return 0;
}

/*! ************************************

	\brief Convert a MacRomanUS "C" string into a UTF8 stream.
	
	Take a "C" string that is using MacRomanUS encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput MacRomanUS encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromMacRomanUS(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uTemp) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The MacRomanUS table only has 2 or 3 byte tokens
			
				const Word8 *pSrc = MacRomanUS::ToUTF8Table[uTemp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			
				// Write the optional third character

				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(uTemp);
					}
					++pWorkPtr;
				}
			}
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it
		} while (uTemp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a MacRomanUS byte array into a UTF8 stream.
	
	Take a byte array that is using MacRomanUS encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput MacRomanUS encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromMacRomanUS(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	
	if (uInputSize) {
		do {
	
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The MacRomanUS table only has 2 or 3 byte tokens
			
				const Word8 *pSrc = MacRomanUS::ToUTF8Table[uTemp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			
				// Write the optional third character

				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(uTemp);
					}
					++pWorkPtr;
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a Win1252 8 bit char into a UTF8 stream.
	
	Take the unsigned 8 bit value of the Win1252 character and convert it to
	a 1,2 or 3 byte UTF8 stream.
	
	\note This function will write a <b>zero</b> after the stream so you can assume
	that it's a valid "C" string.
	
	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size. \ref NULL will page fault.
	\param uInput Win1252 encoded 8 bit character
	\return The number of bytes written to the stream. 1, 2, 3 or 0 if
		uInput is >=256.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromWin1252(char *pOutput,Word uInput)
{
	if (uInput<0x80U) {		// ASCII?
		pOutput[0] = static_cast<char>(uInput);
		pOutput[1] = 0;
		return 1;
	}
	if (uInput<0x100U) {		// Still valid?
		
		// The Win1252 table only has 2 or 3 byte tokens
			
		const Word8 *pSrc = Win1252::ToUTF8Table[uInput-0x80U];
		reinterpret_cast<Word8*>(pOutput)[0] = pSrc[0];
		reinterpret_cast<Word8*>(pOutput)[1] = pSrc[1];
		Word uTemp = pSrc[2];
		reinterpret_cast<Word8*>(pOutput)[2] = static_cast<Word8>(uTemp);
		if (uTemp) {
			pOutput[3] = 0;
			return 3;
		}
		return 2;
	}
	pOutput[0] = 0;
	return 0;
}

/*! ************************************

	\brief Convert a Win1252 "C" string into a UTF8 stream.
	
	Take a "C" string that is using Win1252 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput Win1252 encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromWin1252(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word Temp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (Temp) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (Temp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(Temp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The Win1252 table only has 2 or 3 byte tokens
			
				const Word8 *pSrc = Win1252::ToUTF8Table[Temp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			
				// Write the optional third character

				Temp = pSrc[2];
				if (Temp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(Temp);
					}
					++pWorkPtr;
				}
			}
			Temp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it
		} while (Temp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a Win1252 byte array into a UTF8 stream.
	
	Take a byte array that is using Win1252 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput Win1252 encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromWin1252(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	
	if (uInputSize) {
		do {
	
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The Win1252 table only has 2 or 3 byte tokens
			
				const Word8 *pSrc = Win1252::ToUTF8Table[uTemp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			
				// Write the optional third character

				uTemp = pSrc[2];
				if (uTemp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(uTemp);
					}
					++pWorkPtr;
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a ISOLatin1 8 bit char into a UTF8 stream.
	
	Take the unsigned 8 bit value of the ISOLatin1 character and convert it to
	a 1 or 2 byte UTF8 stream.
	
	\note This function will write a <b>zero</b> after the stream so you can assume
	that it's a valid "C" string.
	
	\param pOutput Pointer to UTF8 buffer that's a minimum 3 bytes in size. \ref NULL will page fault.
	\param uInput ISOLatin1 encoded 8 bit character
	\return The number of bytes written to the stream. 1, 2 or 0 if 
		uInput is >=256.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromISOLatin1(char *pOutput,Word uInput)
{
	if (uInput<0x80U) {		// ASCII?
		pOutput[0] = static_cast<char>(uInput);
		pOutput[1] = 0;
		return 1;
	}
	if (uInput<0x100U) {		// Still valid?
		
		// The ISOLatin1 table only has 2 byte tokens
			
		const Word8 *pSrc = ISOLatin1::ToUTF8Table[uInput-0x80U];
		reinterpret_cast<Word8*>(pOutput)[0] = pSrc[0];
		reinterpret_cast<Word8*>(pOutput)[1] = pSrc[1];
		pOutput[2] = 0;
		return 2;
	}
	pOutput[0] = 0;
	return 0;
}

/*! ************************************

	\brief Convert a ISOLatin1 "C" string into a UTF8 stream.
	
	Take a "C" string that is using ISOLatin1 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput ISOLatin1 encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromISOLatin1(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uTemp) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The ISOLatin1 table only has 2 byte tokens
			
				const Word8 *pSrc = ISOLatin1::ToUTF8Table[uTemp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			}
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it
		} while (uTemp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a ISOLatin1 byte array into a UTF8 stream.
	
	Take a byte array that is using ISOLatin1 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput ISOLatin1 encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromISOLatin1(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string

	if (uInputSize) {
		do {
	
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
			
				// The Win1252 table only has 2 byte tokens
			
				const Word8 *pSrc = ISOLatin1::ToUTF8Table[uTemp-0x80U];
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[0];		// Write the first character
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = pSrc[1];		// Write the second character
				}
				++pWorkPtr;
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a generic 8 bit char into a UTF8 stream.
	
	Take the unsigned 8 bit value of the generic character and convert it to
	a 1 to 4 byte UTF8 stream. Codes 0 through 0x7f are considered ASCII while codes
	0x80 through 0xFF will be found in the supplied table.
	
	The user supplied must contain valid UTF8 byte patterns. This routine will not
	perform validation on the contents of the table and if the table has bad data,
	the UTF8 stream produced by this function will be error prone.
	
	\note This function will write a <b>zero</b> after the stream so you can assume
	that it's a valid "C" string.
	
	\param pOutput Pointer to UTF8 buffer that's a minimum 5 bytes in size. \ref NULL will page fault.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\param uInput Generic encoded 8 bit character
	\return The number of bytes written to the stream. 1, 2, 3, 4 or 0 if
		uInput is >=256.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromGeneric(char *pOutput,const Word8 pTranslateTable[128][4],Word uInput)
{
	Word uResult;
	
	if (uInput>=0x80U) {		// ASCII?
		uResult = 0;
		if (uInput>=0x100U) {		// Still valid?
			pOutput[0] = 0;			// Nope
		} else {
			// I need to get the size from the table
			
			const Word8 *pSrc = pTranslateTable[uInput-0x80U];
			Word uTemp = pSrc[0];
			reinterpret_cast<Word8*>(pOutput)[0] = static_cast<Word8>(uTemp);
			if (uTemp) {
				uResult = 1;
				uTemp = pSrc[1];
				reinterpret_cast<Word8*>(pOutput)[1] = static_cast<Word8>(uTemp);
				if (uTemp) {
					uResult = 2;
					uTemp = pSrc[2];
					reinterpret_cast<Word8*>(pOutput)[2] = static_cast<Word8>(uTemp);
					if (uTemp) {
						uResult = 3;
						uTemp = pSrc[3];
						reinterpret_cast<Word8*>(pOutput)[3] = static_cast<Word8>(uTemp);
						if (uTemp) {
							uResult = 4;
							pOutput[4] = 0;		// I must create the trailing zero manually
						}
					}
				}
			}
		}
	} else {
		pOutput[0] = static_cast<char>(uInput);
		pOutput[1] = 0;
		uResult = 1;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a generic "C" string into a UTF8 stream.
	
	Take a "C" string that is using generic encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	The user supplied must contain valid UTF8 byte patterns. This routine will not
	perform validation on the contents of the table and if the table has bad data,
	the UTF8 stream produced by this function will be error prone.

	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\param pInput A generic encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uTemp) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
		
				// The Generic table can have 0 to 4 bytes
			
				const Word8 *pSrc = pTranslateTable[uTemp-0x80U];
				uTemp = pSrc[0];
				if (uTemp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(uTemp);
					}
					++pWorkPtr;
					uTemp = pSrc[1];
					if (uTemp) {
						if (pWorkPtr<pEndPtr) {
							pWorkPtr[0] = static_cast<Word8>(uTemp);
						}
						++pWorkPtr;
						uTemp = pSrc[2];
						if (uTemp) {
							if (pWorkPtr<pEndPtr) {
								pWorkPtr[0] = static_cast<Word8>(uTemp);
							}
							++pWorkPtr;
							uTemp = pSrc[3];
							if (uTemp) {
								if (pWorkPtr<pEndPtr) {
									pWorkPtr[0] = static_cast<Word8>(uTemp);
								}
								++pWorkPtr;
							}
						}
					}
				}
			}
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it
		} while (uTemp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a generic byte array into a UTF8 stream.
	
	Take a byte array that is using generic encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	The user supplied must contain valid UTF8 byte patterns. This routine will not
	perform validation on the contents of the table and if the table has bad data,
	the UTF8 stream produced by this function will be error prone.

	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\param pInput Generic encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string

	if (uInputSize) {
		do {
	
			Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else {
			
				// The generic table has 0 to 4 bytes
				
				const Word8 *pSrc = pTranslateTable[uTemp-0x80U];
				uTemp = pSrc[0];
				if (uTemp) {
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(uTemp);		// Write the first character
					}
					++pWorkPtr;
					uTemp = pSrc[1];
					if (uTemp) {
						if (pWorkPtr<pEndPtr) {
							pWorkPtr[0] = static_cast<Word8>(uTemp);		// Write the second character
						}
						++pWorkPtr;
				
						// Write the optional third character

						uTemp = pSrc[2];
						if (uTemp) {
							if (pWorkPtr<pEndPtr) {
								pWorkPtr[0] = static_cast<Word8>(uTemp);
							}
							++pWorkPtr;
							uTemp = pSrc[3];
							if (uTemp) {
								if (pWorkPtr<pEndPtr) {
									pWorkPtr[0] = static_cast<Word8>(uTemp);
								}
								++pWorkPtr;
							}
						}
					}
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF16 char into a UTF8 stream.
	
	Take the unsigned 16 bit value of the UTF16 character and convert it to
	a 1,2 or 3 byte UTF8 stream.
	
	\note This will NOT parse word pairs. It will return a 0 and not process the
	value if it's 0xD800-0xDFFF which is an escape token for UTF16.
	
	\note This function will write a <b>zero</b> after the stream so you can assume
	that it's a valid "C" string.
	
	\param pOutput Pointer to UTF8 buffer that's a minimum 4 bytes in size. \ref NULL will page fault.
	\param uInput UTF16 encoded 16 bit character
	\return The number of bytes written to the stream. 1, 2, 3 or 0 if
		uInput is >=0xD800 and <=0xDFFF.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromUTF16(char *pOutput,Word16 uInput)
{
	Word uTemp = static_cast<Word>(uInput);	// Convert to native data size
	if (uTemp<0x80U) {		// ASCII?
		pOutput[0] = static_cast<char>(uTemp);
		pOutput[1] = 0;
		return 1;
	}

	if (uTemp < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
		pOutput[0] = static_cast<char>((uTemp>>6) | 0xC0U);		// Encode the upper 6 bits
		pOutput[1] = static_cast<char>((uTemp & 0x3FU) | 0x80U);	// Encode the lower 6 bits
		pOutput[2] = 0;
		return 2;
	}
	if ((uTemp>=0xD800U) && (uTemp<0xE000U)) {
		pOutput[0] = 0;
		return 0;			// 0xD800-0xDFFF or >=0x110000 are bad.
	}
	// 16 bit encoding?
	pOutput[0] = static_cast<char>((uTemp >> 12U) | 0xE0U);
	pOutput[1] = static_cast<char>(((uTemp >> 6U) & 0x3FU) | 0x80U);
	pOutput[2] = static_cast<char>((uTemp & 0x3FU) | 0x80U);
	pOutput[3] = 0;
	return 3;
}

/*! ************************************

	\brief Convert a UTF16 "C" string into a UTF8 stream.
	
	Take a "C" string that is using UTF16 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note If invalid UTF16 surrogate pair(s) are found, they will be skipped.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF16 encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

uintptr_t BURGER_API Burger::UTF8::FromUTF16(char *pOutput,uintptr_t uOutputSize,const uint16_t *pInput) BURGER_NOEXCEPT
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	uintptr_t uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {					// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uTemp = pInput[0];
	++pInput;			// Accept it
	if (uTemp) {		// Can I start the loop?
		do {
	
		// Get a char
	
			if (uTemp<0x80U) {				// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;						// Increment the output
			} else if (uTemp < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp>>6) | 0xC0U);		// Encode the upper 6 bits
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);	// Encode the lower 6 bits
				}
				++pWorkPtr;
			} else if ((uTemp<0xD800U) || (uTemp>=0xE000U)) {
				// 16 bit encoding? (0x800-0xFFFF)
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp >> 12U) | 0xE0U);
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>(((uTemp >> 6U) & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
			} else if (uTemp<0xDC00U) {		// Valid start token?
				// Special case for 21 bit encoding? (0x10000-0x10FFFF)
				Word uTemp2 = pInput[0]^0xDC00U;		// Get the second token
				if (uTemp2<0x400U) {					// Valid?
					++pInput;						// Accept the second char
					uTemp -= (0xD800-0x40);			// Add in the 0x10000>>10 and remove 0xD800
					if (pWorkPtr<pEndPtr) {			// Top 3 bits
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 8U) | 0xF0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// Next 6 bits
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 2U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// 2 bits from Temp, 4 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>(((uTemp << 4U)&0x30) | (uTemp2 >> 6U) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// The last 6 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp2 & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
			uTemp = pInput[0];	// Get the next 16 bits
			++pInput;			// Accept it
		} while (uTemp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF16 Word16 array into a UTF8 stream.
	
	Take a Word16 array that is using UTF16 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF16 encoded Word16 array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input Word16 array in bytes
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromUTF16(char *pOutput,WordPtr uOutputSize,const Word16 *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	uInputSize>>=1;		// Convert to shorts
	if (uInputSize) {
		do {
			Word uTemp = pInput[0];	// Get the next 16 bits
			++pInput;			// Accept it
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else if (uTemp < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp>>6) | 0xC0U);		// Encode the upper 6 bits
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);	// Encode the lower 6 bits
				}
				++pWorkPtr;
			} else if ((uTemp<0xD800U) || (uTemp>=0xE000U)) {
				// 16 bit encoding? (0x800-0xFFFF)
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp >> 12U) | 0xE0U);
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>(((uTemp >> 6U) & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
				}
				++pWorkPtr;
			} else if (uTemp<0xDC00U && uInputSize>=2) {		// Valid start token?
				// Special case for 21 bit encoding? (0x10000-0x10FFFF)
				Word uTemp2 = pInput[0]^0xDC00U;		// Get the second token
				if (uTemp2<0x400U) {					// Valid?
					++pInput;						// Accept the second char
					--uInputSize;
					uTemp -= (0xD800-0x40);			// Add in the 0x10000>>10 and remove 0xD800
					if (pWorkPtr<pEndPtr) {			// Top 3 bits
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 8U) | 0xF0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// Next 6 bits
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 2U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// 2 bits from uTemp, 4 bits from uTemp2
						pWorkPtr[0] = static_cast<Word8>(((uTemp << 4U)&0x30) | (uTemp2 >> 6U) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// The last 6 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp2 & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF16 "C" string into an allocated UTF8 "C" string.
	
	Take a "C" string that is using UTF16 encoding and convert it
	to a UTF8 encoded "C" string. The function will allocate a buffer large enough
	to store the string. When the string isn't needed anymore, release
	the memory with a call to Burger::Free(const void *)
	
	UTF16 surrogate pairs will be properly parsed and encoded into their UTF8
	equivalents.
	
	\note If invalid UTF16 surrogate pair(s) are found, they will be skipped.
	
	\param pInput UTF16 encoded "C" string. \ref NULL will page fault.
	\return A valid pointer to a UTF-8 version of the "C" string. 
		\ref NULL if memory allocation failure.

	\sa Burger::Free(const void *)

***************************************/

char *BURGER_API Burger::UTF8::FromUTF16(const Word16 *pInput)
{
	WordPtr uInputLength = FromUTF16(NULL,0,pInput);
	char *pWork = static_cast<char *>(Alloc(uInputLength+1));
	if (pWork) {				// Oh oh...
		FromUTF16(pWork,uInputLength+1,pInput);	// Copy the string
	}
	return pWork;
}

/*! ************************************

	\brief Convert a UTF32 value into a UTF8 stream.
	
	Given a valid UTF32 value (0-0xD7FF / 0xE000-0x10FFFF),
	encode it into a valid UTF8 stream. If the value is invalid, it will NOT
	be encoded.
	
	The output buffer must have at least 5 bytes available.
	
	\param pOutput Pointer to a char buffer of a minimum of 5 bytes in size. \ref NULL is invalid.
	\param uInput UTF32 encoded character value.
	\return Number of bytes used to store the UTF8 stream, zero if the value can't be encoded.
		
***************************************/

WordPtr BURGER_API Burger::UTF8::FromUTF32(char *pOutput,Word32 uInput)
{
	WordPtr uResult;
	if (uInput >= 0x80U) {			// Not ASCII?
		if (uInput < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
			pOutput[0] = static_cast<char>((uInput >> 6U) | 0xC0U);	// Encode the upper 6 bits
			pOutput[1] = static_cast<char>((uInput & 0x3FU) | 0x80U);	// Encode the lower 6 bits
			pOutput[2] = 0;
			uResult = 2;
		} else if (((uInput>=0xD800U) && (uInput<0xE000U)) || (uInput>=0x110000U)) {
			pOutput[0] = 0;
			uResult = 0;				// 0xD800-0xDFFF or >=0x110000 are bad.
		} else if (uInput<0x10000U) {	// 16 bit encoding?
			pOutput[0] = static_cast<char>((uInput >> 12U) | 0xE0U);
			pOutput[1] = static_cast<char>(((uInput >> 6U) & 0x3FU) | 0x80U);
			pOutput[2] = static_cast<char>((uInput & 0x3FU) | 0x80U);
			pOutput[3] = 0;
			uResult = 3;
		} else {						// 21 bit encoding
			pOutput[0] = static_cast<char>((uInput >> 18U) | 0xF0U);
			pOutput[1] = static_cast<char>(((uInput >> 12U) & 0x3FU) | 0x80U);
			pOutput[2] = static_cast<char>(((uInput >> 6U) & 0x3FU) | 0x80U);
			pOutput[3] = static_cast<char>((uInput & 0x3FU) | 0x80U);
			pOutput[4] = 0;
			uResult = 4;
		}
	} else {
	
	// This is the most common case, save it and exit.
	
		pOutput[0] = static_cast<char>(uInput);		// Easy!
		pOutput[1] = 0;
		uResult = 1;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF32 "C" string into a UTF8 stream.
	
	Take a "C" string that is using UTF32 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note If invalid UTF32 values are found, they will be skipped.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromUTF32(char *pOutput,WordPtr uOutputSize,const Word32 *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word32 uTemp = pInput[0];
	++pInput;			// Accept it
	if (uTemp) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else if (uTemp < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp>>6) | 0xC0U);		// Encode the upper 6 bits
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);	// Encode the lower 6 bits
				}
				++pWorkPtr;
			} else if (((uTemp<0xD800U) || (uTemp>=0xE000U)) && (uTemp<0x110000U)) {
				if (uTemp<0x10000U) {
					// 16 bit encoding? (0x800-0xFFFF)
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 12U) | 0xE0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
				} else {
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)
					if (pWorkPtr<pEndPtr) {			// Top 3 bits
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 18U) | 0xF0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// Next 6 bits
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 12U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// 2 bits from Temp, 4 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 6U) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// The last 6 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
			uTemp = pInput[0];	// Get the next 32 bits
			++pInput;			// Accept it
		} while (uTemp);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF32 Word32 array into a UTF8 stream.
	
	Take a Word32 array that is using UTF32 encoding and convert it
	to a UTF8 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF8 stream as is.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded Word32 array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input Word16 array in bytes
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::FromUTF32(char *pOutput,WordPtr uOutputSize,const Word32 *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	uInputSize>>=2;		// Convert to Word32s
	if (uInputSize) {
		do {
			Word32 uTemp = pInput[0];	// Get the next 16 bits
			++pInput;			// Accept it
			if (uTemp<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uTemp);
				}
				++pWorkPtr;			// Increment the output
			} else if (uTemp < 0x800U) {		// 0x80-0x7FF (11 bit encoding) ?
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp>>6) | 0xC0U);		// Encode the upper 6 bits
				}
				++pWorkPtr;
				if (pWorkPtr<pEndPtr) {
					pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);	// Encode the lower 6 bits
				}
				++pWorkPtr;
			} else if (((uTemp<0xD800U) || (uTemp>=0xE000U)) && (uTemp<0x110000U)) {
				if (uTemp<0x10000U) {
					// 16 bit encoding? (0x800-0xFFFF)
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 12U) | 0xE0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 6U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
				} else {		// Valid start token?
					// Special case for 21 bit encoding? (0x10000-0x10FFFF)
					if (pWorkPtr<pEndPtr) {			// Top 3 bits
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 18U) | 0xF0U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// Next 6 bits
						pWorkPtr[0] = static_cast<Word8>(((uTemp >> 12U) & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// 2 bits from Temp, 4 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp >> 6U) | 0x80U);
					}
					++pWorkPtr;
					if (pWorkPtr<pEndPtr) {			// The last 6 bits from Temp2
						pWorkPtr[0] = static_cast<Word8>((uTemp & 0x3FU) | 0x80U);
					}
					++pWorkPtr;
					// All 21 are encoded now
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic 8 bit char.
	
	Take a 1 to 4 byte UTF8 stream and look up the unsigned 8 bit value of the
	generic character. Codes 0 through 0x7f are considered ASCII while codes
	0x80 through 0xFF will be found in the supplied table.
	
	\note This function does a linear search from the table, as a result, it's not
	very fast on strings with a lot of high ascii characters
	
	\param pInput Pointer to UTF8 buffer that contains tha stream valid to convert. \ref NULL will page fault.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\return The unsigned 8 bit character code (0x00-0xFF) or Burger::UTF8::BAD if the
		UTF8 value wasn't low ASCII and couldn't be found in the table.
		
***************************************/

Word BURGER_API Burger::UTF8::ToGeneric(const char *pInput,const Word8 pTranslateTable[128][4])
{
	Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];	// Get a UTF-8 char
	if (uFirst<0x80U) {
		return uFirst;			// Easy!!! I like it!
	}
	// Great, it's not low ASCII. Do it the hard way.
	Word uCounter = 128;			// Number of attempts
	do {
		if (uFirst==pTranslateTable[0][0]) {
			Word uTemp = pTranslateTable[0][1];
			if (!uTemp) {
				break;
			}
			if (reinterpret_cast<const Word8*>(pInput)[1] == uTemp) {
				uTemp = pTranslateTable[0][2];
				if (!uTemp) {
					break;
				}
				if (reinterpret_cast<const Word8*>(pInput)[2] == uTemp) {
					uTemp = pTranslateTable[0][3];
					if (!uTemp) {
						break;
					}
					if (reinterpret_cast<const Word8*>(pInput)[3] == uTemp) {
						break;
					}
				}
			}
		}
		++pTranslateTable;			// Next 4 byte entry
	} while (--uCounter);
	if (uCounter) {
		return 256-uCounter;			// Return 0x80-0xFF
	}
	return static_cast<Word>(UTF8::BAD);		// Crud
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic "C" string.
	
	Take a "C" string that is using UTF8 encoding and convert it
	to a generic encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\param pOutput Pointer to byte buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\param pInput A UTF8 encoded "C" string. \ref NULL will page fault.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::ToGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);
	
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uFirst) {			// Can I start the loop?
		do {
	
		// Get a char
	
			if (uFirst<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uFirst);
				}
				++pWorkPtr;			// Increment the output
			} else if (uFirst>=0xC2U) {
				// Make a copy of the pointer
				const Word8 (*pTranslateTable2)[4] = pTranslateTable;

				// Great, it's not low ASCII. Do it the hard way.
				Word uCounter = 128;			// Number of attempts
				do {
					if (uFirst==pTranslateTable2[0][0]) {
						Word uTemp = pTranslateTable2[0][1];
						if (!uTemp) {
							break;			// Only one char?
						}
						if (reinterpret_cast<const Word8*>(pInput)[0] == uTemp) {
							uTemp = pTranslateTable2[0][2];
							if (!uTemp) {
								++pInput;		// I took two chars
								break;
							}
							if (reinterpret_cast<const Word8*>(pInput)[1] == uTemp) {
								uTemp = pTranslateTable2[0][3];
								if (!uTemp) {
									pInput+=2;	// I took three chars
									break;
								}
								if (reinterpret_cast<const Word8*>(pInput)[2] == uTemp) {
									pInput+=3;	// I took four chars
									break;
								}
							}
						}
					}
					++pTranslateTable2;			// Next 4 byte entry
				} while (--uCounter);
		
				if (uCounter) {		// Found it?
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(256-uCounter);
					}
					++pWorkPtr;
				}
			}
			uFirst = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it
		} while (uFirst);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}

/*! ************************************

	\brief Convert a UTF8 stream into a generic byte array.
	
	Take a byte array that is using UTF8 encoding and convert it
	to a generic encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the generic stream as is.
	
	\param pOutput Pointer to a byte buffer to receive the converted string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pTranslateTable Pointer to a 128x4 array to use as a UTF8 conversion table.
	\param pInput UTF8 encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF8::ToGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput,WordPtr uInputSize)
{
	Word8 *pWorkPtr = reinterpret_cast<Word8 *>(pOutput);	
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word8 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	
	if (uInputSize) {
		do {
	
			Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Get a char
	
			if (uFirst<0x80U) {		// ASCII?
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word8>(uFirst);
				}
				++pWorkPtr;			// Increment the output
			} else if (uFirst>=0xC2U) {
			
				// Make a copy of the pointer
				const Word8 (*pTranslateTable2)[4] = pTranslateTable;

				// Great, it's not low ASCII. Do it the hard way.
				Word uCounter = 128;			// Number of attempts
				do {
					if (uFirst==pTranslateTable2[0][0]) {
						Word uTemp = pTranslateTable2[0][1];
						if (!uTemp) {
							break;
						}
						if ((uInputSize>=2) && 
							(reinterpret_cast<const Word8*>(pInput)[0] == uTemp)) {
							uTemp = pTranslateTable2[0][2];
							if (!uTemp) {
								++pInput;
								--uInputSize;
								break;
							}
							if ((uInputSize>=3) &&
								(reinterpret_cast<const Word8*>(pInput)[1] == uTemp)) {
								uTemp = pTranslateTable2[0][3];
								if (!uTemp) {
									pInput+=2;
									uInputSize-=2;
									break;
								}
								if ((uInputSize>=4) &&
									(reinterpret_cast<const Word8*>(pInput)[2] == uTemp)) {
									pInput+=3;
									uInputSize-=3;
									break;
								}
							}
						}
					}
					++pTranslateTable2;			// Next 4 byte entry
				} while (--uCounter);
		
				if (uCounter) {		// Found it?
					if (pWorkPtr<pEndPtr) {
						pWorkPtr[0] = static_cast<Word8>(256-uCounter);
					}
					++pWorkPtr;
				}
			}
		} while (--uInputSize);
	}
	if (uAddZero) {			// Can I add a trailing zero?
		if (pWorkPtr<pEndPtr) {
			pEndPtr = pWorkPtr;
		}
		pEndPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<char *>(pWorkPtr)-pOutput);
}


