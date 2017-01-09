/***************************************

	String handlers for UTF32 support

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf32.h"
#include "brutf8.h"

/*! ************************************

	\class Burger::UTF32
	\brief Conversion routines to the UTF32 format
	
	UTF32 is simplest data format for <a href="http://www.unicode.org">Unicode</a>
	data to be stored in a 32 bit wide "C" string. It can easily contain all of the
	characters for the worlds' languages. These functions allow conversion from
	UTF8, which Burgerlib is based on, to UTF32 which some foreign APIs require for
	internationalization. Please note that these functions operate on strings that are
	native endian.

***************************************/

/*! ************************************

	\var Burger::UTF32::BAD
	\brief Value returned if a routine failed.
	
	If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
	return this value instead. Please see the documentation for each function
	to know which ones use true/false pairs or this value.
	
***************************************/

/*! ************************************

	\var Burger::UTF32::ENDIANMARK
	\brief Byte stream token for native endian.
	
	When writing a text file using UTF32, you may need to write this value as
	the first character to mark the endian that the data was saved at. This value
	is the correct value for the native endian of the machine. Use Burger::UTF32::BE or
	Burger::UTF32::LE to test incoming data to determine the endian of data that's unknown.
		
***************************************/

/*! ************************************

	\var Burger::UTF32::BE
	\brief 32 bit Byte Order Mark (BOM) for Big Endian UTF32 data.
	
	If a token was read in the matched this constant, then you must assume that
	all of the following data is Big Endian. It adheres to the Unicode standard for UTF-32
			
***************************************/

/*! ************************************

	\var Burger::UTF32::LE
	\brief 32 bit Byte Order Mark (BOM) for Little Endian UTF32 data.
	
	If a token was read in the matched this constant, then you must assume that
	all of the following data is Little Endian. It adheres to the Unicode standard for UTF-32
			
***************************************/

/*! ************************************

	\brief Validate a UTF32 value.
	
	Return \ref TRUE if a UTF32 character is in the valid
	bounds. (0-0xD7FF) or (0xE000-0x10FFFF).
		
	\param uInput UTF32 encoded character value.
	\return \ref TRUE if in bounds, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF32::IsValid(Word32 uInput)
{
	if ((uInput<0xD800U) || ((uInput>=0xE000U) && (uInput<0x10FFFFU))) {
		return TRUE;
	}
	return FALSE;
}

/*! ************************************

	\brief Check a UTF32 "C" string for validity
	
	Check a "C" string if it's a valid UTF32 stream. Return
	false if there was an error, or true if the bytes represent a valid UTF32
	pattern.
	
	\param pInput Pointer to a zero terminated string. \ref NULL will page fault.
	\return \ref TRUE if the entire string is a valid UTF32 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF32::IsValid(const Word32 *pInput)
{
	Word uFirst = pInput[0];
	if (uFirst) {
		++pInput;					// Accept it
		do {
			if (((uFirst>=0xD800U) && (uFirst<0xE000U)) ||
				(uFirst>=0x110000U)) {
				return FALSE;		// This is a bad UTF32 string
			}
			uFirst = pInput[0];
			++pInput;				// Accept it
		} while (uFirst);
	}
	return TRUE;					// Yes! It's good
}

/*! ************************************

	\brief Check a UTF32 Word32 array for validity.
	
	Check a Word32 array and see if it's a valid UTF32 stream. Return
	false if there was an error, or true if the bytes represent a valid UTF32
	pattern.
	
	\param pInput Pointer to UTF32 data. Can be \ref NULL if uInputSize is zero, otherwise page fault.
	\param uInputSize Length of the data in bytes, if zero, then the function will return true.
		If the length is odd, the low bit will be masked off to force it even.
	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF32::IsValid(const Word32 *pInput,WordPtr uInputSize)
{
	uInputSize>>=2;		// Use longs
	if (uInputSize) {
		do {
			Word uFirst = pInput[0];
			++pInput;					// Accept it
			if (((uFirst>=0xD800U) && (uFirst<0xE000U)) ||
				(uFirst>=0x110000U)) {
				return FALSE;			// This is a bad UTF32 string
			}
		} while (--uInputSize);
	}
	return TRUE;						// Yes! It's good
}


/*! ************************************

	\brief Return a UTF32 code from a UTF8 stream.
	
	Convert from a UTF8 stream into a 32 bit
	Unicode value (0x00 to 0x10FFFF). This function will perform validation
	on the incoming stream and will flag any data that's invalid.
	
	\note This function will not move the pointer forward, use 
		UTF8::NextToken(const char *) instead.
	
	\param pInput Pointer to a valid UTF8 "C" string. \ref NULL will page fault.
	\return The UTF32 code or \ref BAD if invalid. 0x00 is not invalid.
	\sa FromUTF8(const char **), UTF8::GetTokenSize(const char *) or UTF8::NextToken(const char *).
		
***************************************/

Word32 BURGER_API Burger::UTF32::FromUTF8(const char *pInput)
{
	// Check for the simple case of 0x00-0x7F
		
	Word32 uFirst = reinterpret_cast<const Word8*>(pInput)[0];
	if (uFirst<0x80U) {
		return uFirst;		// Return ASCII now (0x00-0x7F)
	}
		
	// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
		
	if (uFirst>=0xC2U) {
		Word uSecond = reinterpret_cast<const Word8*>(pInput)[1]^0x80U;	// Trick to convert 0x80-0xBF to 0x00-0x3F
		if (uSecond<0x40U) {
			// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF

			if (uFirst<0xE0U) {
				return ((uFirst&0x1FU)<<6U)|uSecond;	// Get the upper 5 bits
			}
				
			Word uThird = reinterpret_cast<const Word8*>(pInput)[2]^0x80U;
			if (uThird<0x40U) {
												
			// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
				if (uFirst<0xF0U) {
					Word uResult = ((uFirst&0xFU)<<12U)|(uSecond<<6U)|uThird;		// Get the upper 4 bits
					if ((uResult>=0x800U) &&
						((uResult<0xD800U) || (uResult>=0xE000U))) {	// Only 0x0800-0xFFFF are valid
						return uResult;		// I'm good!
					}
				} else {
					Word uFourth = reinterpret_cast<const Word8*>(pInput)[3]^0x80U;
					if (uFourth<0x40U) {
				
			// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
						Word uResult = ((uFirst&0xFU)<<18U)|(uSecond<<12U)|(uThird<<6U)|uFourth;		// Get the upper 4 bits
						if ((uResult>=0x10000U) &&
							(uResult<0x110000U)) {	// Only 0x10000-0x10FFFF are valid
							return uResult;			// I'm good!
						}
					}
				}
			}
		}
	}
	// If I got here, it's bad
	return static_cast<Word>(BAD);		// This is a bad stream
}

/*! ************************************

	\brief Return a UTF32 code from a UTF8 stream and update the pointer
	
	Convert from a UTF8 stream into a 32 bit
	Unicode value (0x00 to 0x10FFFF). This function will perform validation
	on the incoming stream and will flag any data that's invalid.
	
	\param ppInput Pointer to a valid UTF8 "C" string pointer. \ref NULL will page fault.
	\return The UTF32 code or \ref BAD if invalid. 0x00 is not invalid.
	\sa FromUTF8(const char *), UTF8::GetTokenSize(const char *) or UTF8::NextToken(const char *).
		
***************************************/

Word32 BURGER_API BURGER_API Burger::UTF32::FromUTF8(const char **ppInput)
{
	const char *pInput = ppInput[0];
	// Decode the data
	Word32 uResult = FromUTF8(pInput);
	if (uResult!=static_cast<Word32>(BAD)) {
		ppInput[0] = UTF8::NextToken(pInput);
	}
	return uResult;
}

/*! ************************************

	\brief Convert a UTF8 "C" string into a UTF32 stream.
	
	Take a "C" string that is using UTF8 encoding and convert it
	to a UTF32 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be null to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note If invalid UTF8 data is found, it will be skipped.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF32 encoded "C" string. \ref NULL will page fault.
	\return The number of bytes of the potential output without the trailing Word32 zero. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF32::FromUTF8(Word32 *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word32 *pWorkPtr = pOutput;
	
	uOutputSize>>=2;				// Use longs
	WordPtr uAddZero = uOutputSize;	// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word32 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uFirst) {		// Can I start the loop?
		do {
			// Check for the simple case of 0x00-0x7F
		
			if (uFirst<0x80U) {
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word32>(uFirst);
				}
				++pWorkPtr;			// Increment the output
		
		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			
			} else if (uFirst>=0xC2U) {
				Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;	// Trick to convert 0x80-0xBF to 0x00-0x3F
				if (uSecond<0x40U) {
		
					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst<0xE0U) {
						if (pWorkPtr<pEndPtr) {		// Can I write?
							pWorkPtr[0] = static_cast<Word32>(((uFirst&0x1FU)<<6U)|uSecond);
						}
						++pWorkPtr;			// Increment the output
						++pInput;			// Accept the second byte
					
					// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
					} else {
						Word uThird = reinterpret_cast<const Word8*>(pInput)[1]^0x80U;
						if (uThird<0x40U) {
							if (uFirst<0xF0U) {
								Word uResult = ((uFirst&0xFU)<<12U)|(uSecond<<6U)|uThird;		// Get the upper 4 bits
								if ((uResult>=0x800U) &&
									((uResult<0xD800U) || (uResult>=0xE000U))) {	// Only 0x0800-0xFFFF are valid
									if (pWorkPtr<pEndPtr) {		// Can I write?
										pWorkPtr[0] = static_cast<Word32>(uResult);
									}
									++pWorkPtr;			// Increment the output
								}
								pInput+=2;			// I took three chars
								
				// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else {
								Word uFourth = reinterpret_cast<const Word8*>(pInput)[2]^0x80U;
								if (uFourth<0x40U) {
									Word uResult = ((uFirst&0xFU)<<18U)|(uSecond<<12U)|(uThird<<6U)|uFourth;		// Get the upper 4 bits
									if ((uResult>=0x10000U) &&
										(uResult<0x110000U)) {	// Only 0x10000-0x10FFFF are valid
										
										if (pWorkPtr<pEndPtr) {		// Can I write?
											pWorkPtr[0] = static_cast<Word32>(uResult);
										}
										++pWorkPtr;			// Increment the output
									}
									pInput+=3;
								}
							}
						}
					}
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
	return static_cast<WordPtr>(reinterpret_cast<Word8*>(pWorkPtr)-reinterpret_cast<Word8*>(pOutput));
}


/*! ************************************

	\brief Convert a UTF8 stream into a UTF32 Word32 array.
	
	Take a byte array that is using UTF8 encoding and convert it
	to a UTF32 Word32 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF32 stream as is.
	
	\param pOutput Pointer to a byte buffer to receive the UTF32 string. \ref NULL is okay
		if uOutputSize is zero, otherwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF8 encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF32::FromUTF8(Word32 *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	Word32 *pWorkPtr = pOutput;	
	uOutputSize>>=2;				// Convert to longs
	WordPtr uAddZero = uOutputSize;	// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word32 *pEndPtr = pWorkPtr+uOutputSize;		// Can't write past this point, ever
	
	// Let's convert the string
	
	if (uInputSize) {			// Sanity check
		do {
			Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Check for the simple case of 0x00-0x7F
		
			if (uFirst<0x80U) {
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word32>(uFirst);
				}
				++pWorkPtr;			// Increment the output
		
		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			
			} else if ((uInputSize>=2) && uFirst>=0xC2U) {
				Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;	// Trick to convert 0x80-0xBF to 0x00-0x3F
				if (uSecond<0x40U) {
		
					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst<0xE0U) {
						if (pWorkPtr<pEndPtr) {		// Can I write?
							pWorkPtr[0] = static_cast<Word32>(((uFirst&0x1FU)<<6U)|uSecond);
						}
						++pWorkPtr;			// Increment the output
						++pInput;			// Accept the second byte
						--uInputSize;
					
					// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
					} else if (uInputSize>=3) {
						Word uThird = reinterpret_cast<const Word8*>(pInput)[1]^0x80U;
						if (uThird<0x40U) {
							if (uFirst<0xF0U) {
								Word uResult = ((uFirst&0xFU)<<12U)|(uSecond<<6U)|uThird;		// Get the upper 4 bits
								if ((uResult>=0x800U) &&
									((uResult<0xD800U) || (uResult>=0xE000U))) {	// Only 0x0800-0xFFFF are valid
									if (pWorkPtr<pEndPtr) {		// Can I write?
										pWorkPtr[0] = static_cast<Word32>(uResult);
									}
									++pWorkPtr;			// Increment the output
								}
								pInput+=2;			// I took three chars
								uInputSize-=2;
								
				// Check for the 21 bit code of 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
							} else if (uInputSize>=4) {
								Word uFourth = reinterpret_cast<const Word8*>(pInput)[2]^0x80U;
								if (uFourth<0x40U) {
									Word uResult = ((uFirst&0xFU)<<18U)|(uSecond<<12U)|(uThird<<6U)|uFourth;		// Get the upper 4 bits
									if ((uResult>=0x10000U) &&
										(uResult<0x110000U)) {	// Only 0x10000-0x10FFFF are valid

					// Since UTF16 can't take 32 bit values, I'll make a surrogate pair
										
										if (pWorkPtr<pEndPtr) {		// Can I write?
											pWorkPtr[0] = static_cast<Word32>(uResult);
										}
										++pWorkPtr;			// Increment the output
									}
								}
								pInput+=3;
								uInputSize-=3;
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
	return static_cast<WordPtr>(reinterpret_cast<Word8*>(pWorkPtr)-reinterpret_cast<Word8*>(pOutput));
}