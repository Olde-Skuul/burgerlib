/***************************************

	String handlers for UTF16 support

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brutf16.h"

/*! ************************************

	\class Burger::UTF16
	\brief Conversion routines to the UTF8 format
	
	UTF16 is a data format that allows <a href="http://www.unicode.org">Unicode</a>
	data to be stored in a 16 bit wide "C" string. It is wide enough to contain all of the most
	popular characters for the worlds languages. These functions allow conversion from
	UTF8, which Burgerlib is based on, to UTF16 which some foreign APIs require for
	internationalization. Please note that these functions operate on strings that are
	native endian.

***************************************/

/*! ************************************

	\var Burger::UTF16::BAD
	\brief Value returned if a routine failed.
	
	If a function doesn't return \ref TRUE or \ref FALSE for failure, it will
	return this value instead. Please see the documentation for each function
	to know which ones use true/false pairs or this value.
	
***************************************/

/*! ************************************

	\var Burger::UTF16::ENDIANMARK
	\brief Byte stream token for native endian.
	
	When writing a text file using UTF16, you may need to write this value as
	the first character to mark the endian that the data was saved at. This value
	is the correct value for the native endian of the machine. Use Burger::UTF16::BE or
	Burger::UTF16::LE to test incoming data to determine the endian of data that's unknown.
		
***************************************/

/*! ************************************

	\var Burger::UTF16::BE
	\brief 16 bit Byte Order Mark (BOM) for Big Endian UTF16 data.
	
	If a token was read in the matched this constant, then you must assume that
	all of the following data is Big Endian. It adheres to the Unicode standard for UTF-16
			
***************************************/

/*! ************************************

	\var Burger::UTF16::LE
	\brief 16 bit Byte Order Mark (BOM) for Little Endian UTF16 data.
	
	If a token was read in the matched this constant, then you must assume that
	all of the following data is Little Endian. It adheres to the Unicode standard for UTF-16
			
***************************************/

/*! ************************************

	\var const Word16 Burger::UTF16::FromMacRomanUSTable[128]
	\brief Table to convert MacRomanUS to UTF16

	Since no Unicode token is larger than 16 bits, an array of Word16 values
	holds the 128 high ascii codes for MacRomanUS converted
	to UTF16. All 128 codes are present in Unicode except for code 0xF0
	which is the closed Apple symbol.
	
	The character map for MacRomanUS
	<a href="macromanus.htm">looks like this. Click here.</a>
	
***************************************/

const Word16 BURGER_ALIGN(Burger::UTF16::FromMacRomanUSTable[128],16) = {
	0x00C4,	// 0x80
	0x00C5,	// 0x81
	0x00C7,	// 0x82
	0x00C9,	// 0x83
	0x00D1,	// 0x84
	0x00D6,	// 0x85
	0x00DC,	// 0x86
	0x00E1,	// 0x87
	0x00E0,	// 0x88
	0x00E2,	// 0x89
	0x00E4,	// 0x8A
	0x00E3,	// 0x8B
	0x00E5,	// 0x8C
	0x00E7,	// 0x8D
	0x00E9,	// 0x8E
	0x00E8,	// 0x8F
	0x00EA,	// 0x90
	0x00EB,	// 0x91
	0x00ED,	// 0x92
	0x00EC,	// 0x93
	0x00EE,	// 0x94
	0x00EF,	// 0x95
	0x00F1,	// 0x96
	0x00F3,	// 0x97
	0x00F2,	// 0x98
	0x00F4,	// 0x99
	0x00F6,	// 0x9A
	0x00F5,	// 0x9B
	0x00FA,	// 0x9C
	0x00F9,	// 0x9D
	0x00FB,	// 0x9E
	0x00FC,	// 0x9F
	0x2020,	// 0xA0
	0x00B0,	// 0xA1
	0x00A2,	// 0xA2
	0x00A3,	// 0xA3
	0x00A7,	// 0xA4
	0x2022,	// 0xA5
	0x00B6,	// 0xA6
	0x00DF,	// 0xA7
	0x00AE,	// 0xA8
	0x00A9,	// 0xA9
	0x2122,	// 0xAA
	0x00B4,	// 0xAB
	0x00A8,	// 0xAC
	0x2260,	// 0xAD
	0x00C6,	// 0xAE
	0x00D8,	// 0xAF
	0x221E,	// 0xB0
	0x00B1,	// 0xB1
	0x2264,	// 0xB2
	0x2265,	// 0xB3
	0x00A5,	// 0xB4
	0x00B5,	// 0xB5
	0x2202,	// 0xB6
	0x2211,	// 0xB7
	0x220F,	// 0xB8
	0x03C0,	// 0xB9
	0x222B,	// 0xBA
	0x00AA,	// 0xBB
	0x00BA,	// 0xBC
	0x03A9,	// 0xBD
	0x00E6,	// 0xBE
	0x00F8,	// 0xBF
	0x00BF,	// 0xC0
	0x00A1,	// 0xC1
	0x00AC,	// 0xC2
	0x221A,	// 0xC3
	0x0192,	// 0xC4
	0x2248,	// 0xC5
	0x2206,	// 0xC6
	0x00AB,	// 0xC7
	0x00BB,	// 0xC8
	0x2026,	// 0xC9
	0x00A0,	// 0xCA
	0x00C0,	// 0xCB
	0x00C3,	// 0xCC
	0x00D5,	// 0xCD
	0x0152,	// 0xCE
	0x0153,	// 0xCF
	0x2013,	// 0xD0
	0x2014,	// 0xD1
	0x201C,	// 0xD2
	0x201D,	// 0xD3
	0x2018,	// 0xD4
	0x2019,	// 0xD5
	0x00F7,	// 0xD6
	0x25CA,	// 0xD7
	0x00FF,	// 0xD8
	0x0178,	// 0xD9
	0x2044,	// 0xDA
	0x20AC,	// 0xDB
	0x2039,	// 0xDC
	0x203A,	// 0xDD
	0xFB01,	// 0xDE
	0xFB02,	// 0xDF
	0x2021,	// 0xE0
	0x00B7,	// 0xE1
	0x201A,	// 0xE2
	0x201E,	// 0xE3
	0x2030,	// 0xE4
	0x00C2,	// 0xE5
	0x00CA,	// 0xE6
	0x00C1,	// 0xE7
	0x00CB,	// 0xE8
	0x00C8,	// 0xE9
	0x00CD,	// 0xEA
	0x00CE,	// 0xEB
	0x00CF,	// 0xEC
	0x00CC,	// 0xED
	0x00D3,	// 0xEE
	0x00D4,	// 0xEF
	0xF8FF,	// 0xF0
	0x00D2,	// 0xF1
	0x00DA,	// 0xF2
	0x00DB,	// 0xF3
	0x00D9,	// 0xF4
	0x0131,	// 0xF5
	0x02C6,	// 0xF6
	0x02DC,	// 0xF7
	0x00AF,	// 0xF8
	0x02D8,	// 0xF9
	0x02D9,	// 0xFA
	0x02DA,	// 0xFB
	0x00B8,	// 0xFC
	0x02DD,	// 0xFD
	0x02DB,	// 0xFE
	0x02C7	// 0xFF
};

/*! ************************************

	\var const Word16 Burger::UTF16::FromWin1252Table[128]
	\brief Table to convert Win1252 to UTF16

	Since no Unicode token is larger than 16 bits, an array of Word16 values
	holds the 128 high ascii codes for Win1252 converted
	to UTF16. Some unprintable codes don't exist and are
	marked as themselves for the pattern. These are
	codes 0x81, 0x8D, 0x8F, 0x90 and 0x9D.

	The character map for Win1252
	<a href="Windows1252.htm">looks like this. Click here.</a>
	
***************************************/

const Word16 BURGER_ALIGN(Burger::UTF16::FromWin1252Table[128],16) = {
	0x20AC,	// 0x80
	0x0081,	// 0x81
	0x201A,	// 0x82
	0x0192,	// 0x83
	0x201E,	// 0x84
	0x2026,	// 0x85
	0x2020,	// 0x86
	0x2021,	// 0x87
	0x02C6,	// 0x88
	0x2030,	// 0x89
	0x0160,	// 0x8A
	0x2039,	// 0x8B
	0x0152,	// 0x8C
	0x008D,	// 0x8D
	0x017D,	// 0x8E
	0x008F,	// 0x8F
	0x0090,	// 0x90
	0x2018,	// 0x91
	0x2019,	// 0x92
	0x201C,	// 0x93
	0x201D,	// 0x94
	0x2022,	// 0x95
	0x2013,	// 0x96
	0x2014,	// 0x97
	0x02DC,	// 0x98
	0x2122,	// 0x99
	0x0161,	// 0x9A
	0x203A,	// 0x9B
	0x0153,	// 0x9C
	0x009D,	// 0x9D
	0x017E,	// 0x9E
	0x0178,	// 0x9F
	0x00A0,	// 0xA0
	0x00A1,	// 0xA1
	0x00A2,	// 0xA2
	0x00A3,	// 0xA3
	0x00A4,	// 0xA4
	0x00A5,	// 0xA5
	0x00A6,	// 0xA6
	0x00A7,	// 0xA7
	0x00A8,	// 0xA8
	0x00A9,	// 0xA9
	0x00AA,	// 0xAA
	0x00AB,	// 0xAB
	0x00AC,	// 0xAC
	0x00AD,	// 0xAD
	0x00AE,	// 0xAE
	0x00AF,	// 0xAF
	0x00B0,	// 0xB0
	0x00B1,	// 0xB1
	0x00B2,	// 0xB2
	0x00B3,	// 0xB3
	0x00B4,	// 0xB4
	0x00B5,	// 0xB5
	0x00B6,	// 0xB6
	0x00B7,	// 0xB7
	0x00B8,	// 0xB8
	0x00B9,	// 0xB9
	0x00BA,	// 0xBA
	0x00BB,	// 0xBB
	0x00BC,	// 0xBC
	0x00BD,	// 0xBD
	0x00BE,	// 0xBE
	0x00BF,	// 0xBF
	0x00C0,	// 0xC0
	0x00C1,	// 0xC1
	0x00C2,	// 0xC2
	0x00C3,	// 0xC3
	0x00C4,	// 0xC4
	0x00C5,	// 0xC5
	0x00C6,	// 0xC6
	0x00C7,	// 0xC7
	0x00C8,	// 0xC8
	0x00C9,	// 0xC9
	0x00CA,	// 0xCA
	0x00CB,	// 0xCB
	0x00CC,	// 0xCC
	0x00CD,	// 0xCD
	0x00CE,	// 0xCE
	0x00CF,	// 0xCF
	0x00D0,	// 0xD0
	0x00D1,	// 0xD1
	0x00D2,	// 0xD2
	0x00D3,	// 0xD3
	0x00D4,	// 0xD4
	0x00D5,	// 0xD5
	0x00D6,	// 0xD6
	0x00D7,	// 0xD7
	0x00D8,	// 0xD8
	0x00D9,	// 0xD9
	0x00DA,	// 0xDA
	0x00DB,	// 0xDB
	0x00DC,	// 0xDC
	0x00DD,	// 0xDD
	0x00DE,	// 0xDE
	0x00DF,	// 0xDF
	0x00E0,	// 0xE0
	0x00E1,	// 0xE1
	0x00E2,	// 0xE2
	0x00E3,	// 0xE3
	0x00E4,	// 0xE4
	0x00E5,	// 0xE5
	0x00E6,	// 0xE6
	0x00E7,	// 0xE7
	0x00E8,	// 0xE8
	0x00E9,	// 0xE9
	0x00EA,	// 0xEA
	0x00EB,	// 0xEB
	0x00EC,	// 0xEC
	0x00ED,	// 0xED
	0x00EE,	// 0xEE
	0x00EF,	// 0xEF
	0x00F0,	// 0xF0
	0x00F1,	// 0xF1
	0x00F2,	// 0xF2
	0x00F3,	// 0xF3
	0x00F4,	// 0xF4
	0x00F5,	// 0xF5
	0x00F6,	// 0xF6
	0x00F7,	// 0xF7
	0x00F8,	// 0xF8
	0x00F9,	// 0xF9
	0x00FA,	// 0xFA
	0x00FB,	// 0xFB
	0x00FC,	// 0xFC
	0x00FD,	// 0xFD
	0x00FE,	// 0xFE
	0x00FF	// 0xFF
};

/*! ************************************

	\var const Word16 Burger::UTF16::FromISOLatin1Table[128]
	\brief Table to convert ISOLatin1 to UTF16

	Since no Unicode token is larger than 16 bits, an array of Word16 values
	holds the 128 high ascii codes for ISOLatin1 converted
	to UTF16. All 128 codes are present since they are
	a 1-to-1 mapping to Unicode.
	
	The character map for ISOLatin1
	<a href="ISOLatin1.htm">looks like this. Click here.</a>

***************************************/

const Word16 BURGER_ALIGN(Burger::UTF16::FromISOLatin1Table[128],16) = {
	0x0080,	// 0x80
	0x0081,	// 0x81
	0x0082,	// 0x82
	0x0083,	// 0x83
	0x0084,	// 0x84
	0x0085,	// 0x85
	0x0086,	// 0x86
	0x0087,	// 0x87
	0x0088,	// 0x88
	0x0089,	// 0x89
	0x008A,	// 0x8A
	0x008B,	// 0x8B
	0x008C,	// 0x8C
	0x008D,	// 0x8D
	0x008E,	// 0x8E
	0x008F,	// 0x8F
	0x0090,	// 0x90
	0x0091,	// 0x91
	0x0092,	// 0x92
	0x0093,	// 0x93
	0x0094,	// 0x94
	0x0095,	// 0x95
	0x0096,	// 0x96
	0x0097,	// 0x97
	0x0098,	// 0x98
	0x0099,	// 0x99
	0x009A,	// 0x9A
	0x009B,	// 0x9B
	0x009C,	// 0x9C
	0x009D,	// 0x9D
	0x009E,	// 0x9E
	0x009F,	// 0x9F
	0x00A0,	// 0xA0
	0x00A1,	// 0xA1
	0x00A2,	// 0xA2
	0x00A3,	// 0xA3
	0x00A4,	// 0xA4
	0x00A5,	// 0xA5
	0x00A6,	// 0xA6
	0x00A7,	// 0xA7
	0x00A8,	// 0xA8
	0x00A9,	// 0xA9
	0x00AA,	// 0xAA
	0x00AB,	// 0xAB
	0x00AC,	// 0xAC
	0x00AD,	// 0xAD
	0x00AE,	// 0xAE
	0x00AF,	// 0xAF
	0x00B0,	// 0xB0
	0x00B1,	// 0xB1
	0x00B2,	// 0xB2
	0x00B3,	// 0xB3
	0x00B4,	// 0xB4
	0x00B5,	// 0xB5
	0x00B6,	// 0xB6
	0x00B7,	// 0xB7
	0x00B8,	// 0xB8
	0x00B9,	// 0xB9
	0x00BA,	// 0xBA
	0x00BB,	// 0xBB
	0x00BC,	// 0xBC
	0x00BD,	// 0xBD
	0x00BE,	// 0xBE
	0x00BF,	// 0xBF
	0x00C0,	// 0xC0
	0x00C1,	// 0xC1
	0x00C2,	// 0xC2
	0x00C3,	// 0xC3
	0x00C4,	// 0xC4
	0x00C5,	// 0xC5
	0x00C6,	// 0xC6
	0x00C7,	// 0xC7
	0x00C8,	// 0xC8
	0x00C9,	// 0xC9
	0x00CA,	// 0xCA
	0x00CB,	// 0xCB
	0x00CC,	// 0xCC
	0x00CD,	// 0xCD
	0x00CE,	// 0xCE
	0x00CF,	// 0xCF
	0x00D0,	// 0xD0
	0x00D1,	// 0xD1
	0x00D2,	// 0xD2
	0x00D3,	// 0xD3
	0x00D4,	// 0xD4
	0x00D5,	// 0xD5
	0x00D6,	// 0xD6
	0x00D7,	// 0xD7
	0x00D8,	// 0xD8
	0x00D9,	// 0xD9
	0x00DA,	// 0xDA
	0x00DB,	// 0xDB
	0x00DC,	// 0xDC
	0x00DD,	// 0xDD
	0x00DE,	// 0xDE
	0x00DF,	// 0xDF
	0x00E0,	// 0xE0
	0x00E1,	// 0xE1
	0x00E2,	// 0xE2
	0x00E3,	// 0xE3
	0x00E4,	// 0xE4
	0x00E5,	// 0xE5
	0x00E6,	// 0xE6
	0x00E7,	// 0xE7
	0x00E8,	// 0xE8
	0x00E9,	// 0xE9
	0x00EA,	// 0xEA
	0x00EB,	// 0xEB
	0x00EC,	// 0xEC
	0x00ED,	// 0xED
	0x00EE,	// 0xEE
	0x00EF,	// 0xEF
	0x00F0,	// 0xF0
	0x00F1,	// 0xF1
	0x00F2,	// 0xF2
	0x00F3,	// 0xF3
	0x00F4,	// 0xF4
	0x00F5,	// 0xF5
	0x00F6,	// 0xF6
	0x00F7,	// 0xF7
	0x00F8,	// 0xF8
	0x00F9,	// 0xF9
	0x00FA,	// 0xFA
	0x00FB,	// 0xFB
	0x00FC,	// 0xFC
	0x00FD,	// 0xFD
	0x00FE,	// 0xFE
	0x00FF	// 0xFF
};

/*! ************************************

	\brief Validate a UTF16 value.
	
	Return \ref TRUE if a UTF16 character is in the valid
	bounds. (0-0xD7FF) or (0xE000-0xFFFF).
		
	\param uInput UTF16 encoded character value.
	\return \ref TRUE if in bounds, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF16::IsValid(Word16 uInput)
{
	if ((uInput<0xD800) || (uInput>=0xE000)) {
		return TRUE;
	}
	return FALSE;
}

/*! ************************************

	\brief Check a UTF16 "C" string for validity
	
	Check a "C" string if it's a valid UTF16 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a valid UTF16
	pattern.
	
	\param pInput Pointer to a zero terminated string. \ref NULL will page fault.
	\return \ref TRUE if the entire string is a valid UTF16 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF16::IsValid(const Word16 *pInput)
{
	Word uFirst = pInput[0];
	++pInput;					// Accept it
	if (uFirst) {
		do {
			if ((uFirst>=0xD800U) && (uFirst<0xE000)) {
				if (uFirst>=0xDC00U) {	// Invalid? 0xDC00-0xDFFF
					return FALSE;		// This is a bad UTF16 string
				}
				Word uSecond = pInput[0]^0xDC00U;	// Get the next value
				++pInput;				// The surrogate pair is okay.
				if (uSecond>=0x400U) {	// Out of bounds?
					return FALSE;		// This is a bad UTF16 string
				}
			}
			uFirst = pInput[0];
			++pInput;					// Accept it
		} while (uFirst);
	}
	return TRUE;		// Yes! It's good
}

/*! ************************************

	\brief Check a UTF16 Word16 array for validity.
	
	Check a Word16 array and see if it's a valid UTF16 stream. Return
	\ref FALSE if there was an error, or \ref TRUE if the bytes represent a valid UTF16
	pattern.
	
	\param pInput Pointer to UTF16 data. Can be \ref NULL if uInputSize is zero,
		otherwise page fault.
	\param uInputSize Length of the data in bytes, if zero, then the function will return \ref TRUE.
		If the length is odd, the low bit will be masked off to force it even.
	\return \ref TRUE if the entire string is a valid UTF8 stream, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::UTF16::IsValid(const Word16 *pInput,WordPtr uInputSize)
{
	uInputSize>>=1;		// Use shorts
	if (uInputSize) {
		do {
			Word uFirst = pInput[0];
			++pInput;					// Accept it
			if ((uFirst>=0xD800U) && (uFirst<0xE000U)) {
				if ((uInputSize<2) || (uFirst>=0xDC00U)) {	// Invalid? 0xDC00-0xDFFF
					return FALSE;		// This is a bad UTF16 string
				}
				Word uSecond = pInput[0];	// Get the next value
				--uInputSize;
				++pInput;				// The surrogate pair is okay.
				if ((uSecond<0xDC00U) || (uSecond>=0xE000U)) {	// Out of bounds?
					return FALSE;		// This is a bad UTF16 string
				}
			}
		} while (--uInputSize);
	}
	return TRUE;		// Yes! It's good
}

/*! ************************************

	\brief Return a UTF16 code from a UTF8 stream.
	
	Convert from a UTF8 stream into a 16 bit
	Unicode value (0x00 to 0FFFF). This function will perform validation
	on the incoming stream and will flag any data that's invalid. It will not parse
	unicode values in the range of 0xD800-0xDFFF and greater than 0xFFFF, these
	do not fit in a single 16 bit quantity and will return an error.
	
	\note This function will not move the pointer forward, use 
	Burger::UTF8::NextToken(const char *) instead.
	
	\param pInput Pointer to a valid UTF8 "C" string.
	\return The UTF16 code or Burger::UTF16::BAD if invalid. 0x00 is not invalid.
	\sa Burger::UTF8::GetTokenSize(const char *) or Burger::UTF8::NextToken(const char *).
		
***************************************/

Word32 BURGER_API Burger::UTF16::FromUTF8(const char *pInput)
{
	if (pInput) {				// Is the pointer valid?
	
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
				
				// Check for the 16 bit code of 0xE0-0xEF / 0x80-0xBF / 0x80-0xBF
				}
				if (uFirst<0xF0U) {
					Word uResult = ((uFirst&0xFU)<<12U)|(uSecond<<6U);		// Get the upper 4 bits
					Word uThird = reinterpret_cast<const Word8*>(pInput)[2]^0x80U;
					if (uThird<0x40U) {
						uResult |= uThird;
						if ((uResult>=0x800U) &&
							((uResult<0xD800U) || (uResult>=0xE000U))) {	// Only 0x0800-0xFFFF are valid
							return uResult;		// I'm good!
						}
					}
				}
			}
			// Don't bother with the 21 bit code, it won't fit in UTF16
			// 0xF0-0xFF / 0x80-0xBF / 0x80-0xBF / 0x80-0xBF
		}
	}
	// If I got here, it's bad
	return static_cast<Word32>(UTF16::BAD);		// This is a bad stream
}

/*! ************************************

	\brief Convert a UTF8 "C" string into a UTF16 stream.
	
	Take a "C" string that is using UTF8 encoding and convert it
	to a UTF16 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note If invalid UTF8 data is found, it will be skipped.
	
	\param pOutput Pointer to UTF8 buffer to receive the converted string. \ref NULL is okay if uOutputSize is zero, otherwise it will page fault.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF16 encoded "C" string. \ref NULL will page fault.
	\return The number of bytes of the potential output without the trailing Word16 zero. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF16::FromUTF8(Word16 *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word16 *pWorkPtr = pOutput;
	
	uOutputSize>>=1;				// Use shorts
	WordPtr uAddZero = uOutputSize;		// If nonzero, then I append a 0 to the string.
	if (uOutputSize) {				// Valid?
		--uOutputSize;
	}
	Word16 *pEndPtr = pWorkPtr+uOutputSize;	// This is the end of the valid buffer
	
	// Let's convert the string
	
	Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;			// Accept it
	if (uFirst) {			// Can I start the loop?
		do {
			// Check for the simple case of 0x00-0x7F
		
			if (uFirst<0x80U) {
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word16>(uFirst);
				}
				++pWorkPtr;			// Increment the output
		
		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			
			} else if (uFirst>=0xC2U) {
				Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;	// Trick to convert 0x80-0xBF to 0x00-0x3F
				if (uSecond<0x40U) {
		
					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst<0xE0U) {
						if (pWorkPtr<pEndPtr) {		// Can I write?
							pWorkPtr[0] = static_cast<Word16>(((uFirst&0x1FU)<<6U)|uSecond);
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
										pWorkPtr[0] = static_cast<Word16>(uResult);
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

					// Since UTF16 can't take 32 bit values, I'll make a surrogate pair
										
										if (pWorkPtr<pEndPtr) {		// Can I write?
											pWorkPtr[0] = static_cast<Word16>((uResult>>10U)+(0xD800-0x40));
										}
										++pWorkPtr;			// Increment the output
										if (pWorkPtr<pEndPtr) {		// Can I write?
											pWorkPtr[0] = static_cast<Word16>((uResult&0x3FFU)|0xDC00U);
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
		pWorkPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<Word8*>(pWorkPtr)-reinterpret_cast<Word8*>(pOutput));
}

/*! ************************************

	\brief Convert a UTF8 stream into a UTF16 Word16 array.
	
	Take a byte array that is using UTF8 encoding and convert it
	to a UTF16 Word16 encoded "C" string. The function will return the size of the string
	after encoding. This size is valid, even if it exceeded the output buffer size.
	The output pointer and size can be \ref NULL to have this routine calculate the size
	of the possible output so the application can allocate a buffer large enough
	to hold it.
	
	\note This function will ensure that the string is always zero terminated, 
	even if truncation is necessary to get it to fit in the output buffer. Under
	no circumstances will the output buffer be overrun.
	
	\note Zeros can be encoded into the stream. This function will not early out if a zero
	was parsed. Zeros will be placed in the UTF16 stream as is.
	
	\param pOutput Pointer to a \ref Word16 buffer to receive the UTF16 string. \ref NULL is okay
		if uOutputSize is zero, outwise a page fault will occur.
	\param uOutputSize Size of the output buffer in bytes.
	\param pInput UTF8 encoded byte array. \ref NULL is okay if uInputSize is zero.
	\param uInputSize Size of the input byte array.
	\return Byte count of the potential output. It is valid, even if the output buffer wasn't large
	enough to contain everything.

***************************************/

WordPtr BURGER_API Burger::UTF16::FromUTF8(Word16 *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	Word16 *pWorkPtr = pOutput;
	uOutputSize>>=1;				// Convert to shorts
	WordPtr uAddZero = uOutputSize;		// Can I save a zero?
	if (uOutputSize) {		// Valid for anything?
		--uOutputSize;		// Make room for the zero
	}
	Word16 *pEndPtr = pWorkPtr+uOutputSize;				// Can't write, ever
	
	// Let's convert the string
	
	if (uInputSize) {			// Sanity check
		do {
			Word uFirst = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;			// Accept it

			// Check for the simple case of 0x00-0x7F
		
			if (uFirst<0x80U) {
				if (pWorkPtr<pEndPtr) {		// Can I write?
					pWorkPtr[0] = static_cast<Word16>(uFirst);
				}
				++pWorkPtr;			// Increment the output
		
		// UTF8 prefix codes start at 0xC0, 0x80-0xBF are invalid
			
			} else if ((uInputSize>=2) && uFirst>=0xC2U) {
				Word uSecond = reinterpret_cast<const Word8*>(pInput)[0]^0x80U;	// Trick to convert 0x80-0xBF to 0x00-0x3F
				if (uSecond<0x40U) {
		
					// Check for the 11 bit code of 0xC0-0xDF / 0x80-0xBF
					if (uFirst<0xE0U) {
						if (pWorkPtr<pEndPtr) {		// Can I write?
							pWorkPtr[0] = static_cast<Word16>(((uFirst&0x1FU)<<6U)|uSecond);
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
										pWorkPtr[0] = static_cast<Word16>(uResult);
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
											pWorkPtr[0] = static_cast<Word16>((uResult>>10U)+(0xD800-0x40));
										}
										++pWorkPtr;			// Increment the output
										if (pWorkPtr<pEndPtr) {		// Can I write?
											pWorkPtr[0] = static_cast<Word16>((uResult&0x3FFU)|0xDC00U);
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
		pWorkPtr[0] = 0;	// Write it, but don't add it to the strlen()
	}

	// Return the equivalent of strlen()
	return static_cast<WordPtr>(reinterpret_cast<Word8*>(pWorkPtr)-reinterpret_cast<Word8*>(pOutput));
}

