/***************************************

	Stand alone string functions

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brstringfunctions.h"
#include "brglobalmemorymanager.h"
#include "brisolatin1.h"
#include "brfloatingpoint.h"
#include <stdio.h>

//
// COMPILER BUG! In Open Watcom 1.8 and 1.9, it can
// inline an assembly language function (StringLength())
// even though it's declared as __declspec(naked)
// This means that the code could be inserted in a "C"
// function and since the assembly has a ret instruction
// inside of it, it will return control too soon
// resulting in a crash.
//

#if defined(BURGER_WATCOM)
#if (__WATCOMC__==1280) || (__WATCOMC__==1290)	// Test for Open Watcom 1.8 or 1.9
#pragma inline_depth(0)
#else
#error Check if the Open Watcom bug is present in this version of the compiler
#endif
#endif

/*! ************************************

	\var Burger::LEADINGZEROS
	\brief Bit flags to force the output of leading zeros.
	
	The functions that convert a float or integer into an ASCII string use
	this flag to control the output of leading zeros. The default behavior is
	that leading zeros are suppressed. Pass this value in the uDigits parameter
	to enable leading zero output.

	\sa Burger::NumberToAscii(char *,Word32,Word)
	
***************************************/

/*! ************************************

	\var Burger::NOENDINGNULL
	\brief Bit flag to suppress the output of a terminating zero.
	
	The functions that convert a float or integer into an ASCII string use
	this flag to suppress the output of the terminating \ref NULL. The default behavior is
	that a zero is output as the last character so that the string is a valid "C" string.
	Pass this value in the uDigits parameter to disable the terminating zero output.
	
	\sa Burger::NumberToAscii(char *,Word32,Word)

***************************************/

/*! ************************************

	\var Burger::ASCII_LOWER 
	\brief Bit flag to detect a lower case ASCII character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is a lower case character.

	\sa Burger::ASCII_UPPER

***************************************/

/*! ************************************

	\var Burger::ASCII_UPPER
	\brief Bit flag to detect a upper case ASCII character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is a upper case character.

	\sa Burger::ASCII_LOWER

***************************************/

/*! ************************************

	\var Burger::ASCII_CONTROL 
	\brief Bit flag to detect an ASCII control character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is a control character.

***************************************/

/*! ************************************

	\var Burger::ASCII_SPACE
	\brief Bit flag to detect an ASCII space character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is an ASCII space character such as CR, LF, FF, SPACE and TAB
	
	\sa Burger::ASCII_WHITESPACE

***************************************/

/*! ************************************

	\var Burger::ASCII_WHITESPACE
	\brief Bit flag to detect an ASCII whitespace character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is an ASCII whitespace character which is only SPACE or TAB
	
	\sa Burger::ASCII_SPACE

***************************************/

/*! ************************************

	\var Burger::ASCII_PUNCTUATION
	\brief Bit flag to detect an ASCII punctuation character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is an ASCII punctuation character such as [, ], !, ?, etc.

***************************************/

/*! ************************************

	\var Burger::ASCII_HEX
	\brief Bit flag to detect an ASCII hex character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is an ASCII hex character which is A-F, a-f, and 0-9.

	\sa Burger::ASCII_DIGIT

***************************************/

/*! ************************************

	\var Burger::ASCII_DIGIT
	\brief Bit flag to detect an ASCII numeric character

	When Burger::g_AsciiTestTable[128] is indexed with an ASCII character, test it with this
	value to determine if it is an ASCII numeric character which is 0-9.
	
	\sa Burger::ASCII_HEX

***************************************/

/*! ************************************

	\brief ""
	
	This string is hard coded to be an empty string.
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_EmptyString[1] = {0};

/*! ************************************

	\brief "\r\n"
	
	This string is hard coded to have the Win32 CR/LF EOL string.
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_CRLFString[3] = {'\r','\n',0};

/*! ************************************

	\brief "\r"
	
	This string is hard coded to have the MacOS CR EOL string.
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_CRString[2] = {'\r',0};

/*! ************************************

	\brief "\n"
	
	This string is hard coded to have the Linux/Unix LF EOL string.
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_LFString[2] = {'\n',0};

/*! ************************************

	\brief "true"
	
	This string is hard coded to have the string "true".
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_TrueString[5] = {'t','r','u','e',0};

/*! ************************************

	\brief "false"
	
	This string is hard coded to have the string "false".
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_FalseString[6] = {'f','a','l','s','e',0};

/*! ************************************

	\brief "yes"
	
	This string is hard coded to have the string "yes".
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_YesString[4] = {'y','e','s',0};

/*! ************************************

	\brief "no"
	
	This string is hard coded to have the string "no".
	It saves space by having a singular occurrence.

***************************************/

const char Burger::g_NoString[3] = {'n','o',0};

/*! ************************************

	\brief Days of the week
	
	An array of seven strings for the seven days of the week. Each string is capitalized so
	they are "Sunday", "Monday", etc...

***************************************/

const char *Burger::g_WeekDays[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

/*! ************************************

	\brief Months of the year
	
	An array of twelve strings for the twelve months of the year. Each string is capitalized so
	they are "January", "February", etc...

***************************************/

const char *Burger::g_Months[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

/*! ************************************

	\brief "" in UTF16
	
	This string is hard coded to be an empty string.
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_EmptyString16[1] = {0};

/*! ************************************

	\brief "\r\n" in UTF16
	
	This string is hard coded to have the Win32 CR/LF EOL string.
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_CRLFString16[3] = {'\r','\n',0};

/*! ************************************

	\brief "\r" in UTF16
	
	This string is hard coded to have the MacOS CR EOL string.
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_CRString16[2] = {'\r',0};

/*! ************************************

	\brief "\n" in UTF16
	
	This string is hard coded to have the Linux/Unix LF EOL string.
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_LFString16[2] = {'\n',0};

/*! ************************************

	\brief "true" in UTF16
	
	This string is hard coded to have the string "true".
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_TrueString16[5] = {'t','r','u','e',0};

/*! ************************************

	\brief "false" in UTF16
	
	This string is hard coded to have the string "false".
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_FalseString16[6] = {'f','a','l','s','e',0};

/*! ************************************

	\brief "yes" in UTF16
	
	This string is hard coded to have the string "yes".
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_YesString16[4] = {'y','e','s',0};

/*! ************************************

	\brief "no" in UTF16
	
	This string is hard coded to have the string "no".
	It saves space by having a singular occurrence.

***************************************/

const Word16 Burger::g_NoString16[3] = {'n','o',0};

/*! ************************************

	\var const Word8 Burger::g_AsciiTestTable[128]
	\brief Table to quickly determine the type of low ASCII character.
	
	This table is used for code to quickly determine if an
	ASCII code from 0 to 127 is upper case, lower case, whitespace, etc.

	\sa Burger::ASCII_CONTROL,Burger::ASCII_UPPER,Burger::ASCII_LOWER or Burger::ASCII_PUNCTUATION

***************************************/

const Word8 BURGER_ALIGN(Burger::g_AsciiTestTable[128],16) = {
	Burger::ASCII_CONTROL,								// 00 (NUL)
	Burger::ASCII_CONTROL,								// 01 (SOH)
	Burger::ASCII_CONTROL,								// 02 (STX)
	Burger::ASCII_CONTROL,								// 03 (ETX)
	Burger::ASCII_CONTROL,								// 04 (EOT)
	Burger::ASCII_CONTROL,								// 05 (ENQ)
	Burger::ASCII_CONTROL,								// 06 (ACK)
	Burger::ASCII_CONTROL,								// 07 (BEL)
	Burger::ASCII_CONTROL,								// 08 (BS)
	Burger::ASCII_SPACE|Burger::ASCII_CONTROL|Burger::ASCII_WHITESPACE,	// 09 (HT)
	Burger::ASCII_SPACE|Burger::ASCII_CONTROL,			// 0A (LF)
	Burger::ASCII_SPACE|Burger::ASCII_CONTROL,			// 0B (VT)
	Burger::ASCII_SPACE|Burger::ASCII_CONTROL,			// 0C (FF)
	Burger::ASCII_SPACE|Burger::ASCII_CONTROL,			// 0D (CR)
	Burger::ASCII_CONTROL,								// 0E (SI)
	Burger::ASCII_CONTROL,								// 0F (SO)
	Burger::ASCII_CONTROL,								// 10 (DLE)
	Burger::ASCII_CONTROL,								// 11 (DC1)
	Burger::ASCII_CONTROL,								// 12 (DC2)
	Burger::ASCII_CONTROL,								// 13 (DC3)
	Burger::ASCII_CONTROL,								// 14 (DC4)
	Burger::ASCII_CONTROL,								// 15 (NAK)
	Burger::ASCII_CONTROL,								// 16 (SYN)
	Burger::ASCII_CONTROL,								// 17 (ETB)
	Burger::ASCII_CONTROL,								// 18 (CAN)
	Burger::ASCII_CONTROL,								// 19 (EM)
	Burger::ASCII_CONTROL,								// 1A (SUB)
	Burger::ASCII_CONTROL,								// 1B (ESC)
	Burger::ASCII_CONTROL,								// 1C (FS)
	Burger::ASCII_CONTROL,								// 1D (GS)
	Burger::ASCII_CONTROL,								// 1E (RS)
	Burger::ASCII_CONTROL,								// 1F (US)
	Burger::ASCII_SPACE|Burger::ASCII_WHITESPACE,		// 20 SPACE
	Burger::ASCII_PUNCTUATION,							// 21 !
	Burger::ASCII_PUNCTUATION,							// 22 "
	Burger::ASCII_PUNCTUATION,							// 23 #
	Burger::ASCII_PUNCTUATION,							// 24 $
	Burger::ASCII_PUNCTUATION,							// 25 %
	Burger::ASCII_PUNCTUATION,							// 26 &
	Burger::ASCII_PUNCTUATION,							// 27 '
	Burger::ASCII_PUNCTUATION,							// 28 (
	Burger::ASCII_PUNCTUATION,							// 29 )
	Burger::ASCII_PUNCTUATION,							// 2A *
	Burger::ASCII_PUNCTUATION,							// 2B +
	Burger::ASCII_PUNCTUATION,							// 2C ,
	Burger::ASCII_PUNCTUATION,							// 2D -
	Burger::ASCII_PUNCTUATION,							// 2E .
	Burger::ASCII_PUNCTUATION,							// 2F /
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 30 0
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 31 1
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 32 2
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 33 3
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 34 4
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 35 5
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 36 6
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 37 7
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 38 8
	Burger::ASCII_DIGIT|Burger::ASCII_HEX,				// 39 9
	Burger::ASCII_PUNCTUATION,							// 3A :
	Burger::ASCII_PUNCTUATION,							// 3B ;
	Burger::ASCII_PUNCTUATION,							// 3C <
	Burger::ASCII_PUNCTUATION,							// 3D =
	Burger::ASCII_PUNCTUATION,							// 3E >
	Burger::ASCII_PUNCTUATION,							// 3F ?
	Burger::ASCII_PUNCTUATION,							// 40 @
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 41 A
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 42 B
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 43 C
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 44 D
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 45 E
	Burger::ASCII_UPPER|Burger::ASCII_HEX,				// 46 F
	Burger::ASCII_UPPER,								// 47 G
	Burger::ASCII_UPPER,								// 48 H
	Burger::ASCII_UPPER,								// 49 I
	Burger::ASCII_UPPER,								// 4A J
	Burger::ASCII_UPPER,								// 4B K
	Burger::ASCII_UPPER,								// 4C L
	Burger::ASCII_UPPER,								// 4D M
	Burger::ASCII_UPPER,								// 4E N
	Burger::ASCII_UPPER,								// 4F O
	Burger::ASCII_UPPER,								// 50 P
	Burger::ASCII_UPPER,								// 51 Q
	Burger::ASCII_UPPER,								// 52 R
	Burger::ASCII_UPPER,								// 53 S
	Burger::ASCII_UPPER,								// 54 T
	Burger::ASCII_UPPER,								// 55 U
	Burger::ASCII_UPPER,								// 56 V
	Burger::ASCII_UPPER,								// 57 W
	Burger::ASCII_UPPER,								// 58 X
	Burger::ASCII_UPPER,								// 59 Y
	Burger::ASCII_UPPER,								// 5A Z
	Burger::ASCII_PUNCTUATION,							// 5B [
	Burger::ASCII_PUNCTUATION,							// 5C "\"
	Burger::ASCII_PUNCTUATION,							// 5D ]
	Burger::ASCII_PUNCTUATION,							// 5E ^
	Burger::ASCII_PUNCTUATION,							// 5F _
	Burger::ASCII_PUNCTUATION,							// 60 `
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 61 a
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 62 b
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 63 c
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 64 d
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 65 e
	Burger::ASCII_LOWER|Burger::ASCII_HEX,				// 66 f
	Burger::ASCII_LOWER,								// 67 g
	Burger::ASCII_LOWER,								// 68 h
	Burger::ASCII_LOWER,								// 69 i
	Burger::ASCII_LOWER,								// 6A j
	Burger::ASCII_LOWER,								// 6B k
	Burger::ASCII_LOWER,								// 6C l
	Burger::ASCII_LOWER,								// 6D m
	Burger::ASCII_LOWER,								// 6E n
	Burger::ASCII_LOWER,								// 6F o
	Burger::ASCII_LOWER,								// 70 p
	Burger::ASCII_LOWER,								// 71 q
	Burger::ASCII_LOWER,								// 72 r
	Burger::ASCII_LOWER,								// 73 s
	Burger::ASCII_LOWER,								// 74 t
	Burger::ASCII_LOWER,								// 75 u
	Burger::ASCII_LOWER,								// 76 v
	Burger::ASCII_LOWER,								// 77 w
	Burger::ASCII_LOWER,								// 78 x
	Burger::ASCII_LOWER,								// 79 y
	Burger::ASCII_LOWER,								// 7A z
	Burger::ASCII_PUNCTUATION,							// 7B {
	Burger::ASCII_PUNCTUATION,							// 7C |
	Burger::ASCII_PUNCTUATION,							// 7D }
	Burger::ASCII_PUNCTUATION,							// 7E ~
	Burger::ASCII_CONTROL								// 7F (DEL)
};

/*! ************************************

	\var const char Burger::g_NibbleToAsciiUppercase[16]
	\brief Table to convert nibbles (0-15) into upper case ASCII characters.
	
	This table is used for code to quickly map the hex
	values of 0-15 into the digits 0-9 and A-F.

	\sa Burger::g_NibbleToAsciiLowercase[16]

***************************************/

const char BURGER_ALIGN(Burger::g_NibbleToAsciiUppercase[16],16) = {
	'0','1','2','3','4','5','6','7',
	'8','9','A','B','C','D','E','F'};

/*! ************************************

	\var const char Burger::g_NibbleToAsciiLowercase[16]
	\brief Table to convert nibbles (0-15) into lower case ASCII characters.
	
	This table is used for code to quickly map the hex
	values of 0-15 into the digits 0-9 and a-f.

	\sa Burger::g_NibbleToAsciiUppercase[16]

***************************************/

const char BURGER_ALIGN(Burger::g_NibbleToAsciiLowercase[16],16) = {'0','1','2','3','4','5','6','7',
	'8','9','a','b','c','d','e','f'};

/*! ************************************

	\var const Word8 Burger::g_AsciiToWord8Table[256]
	\brief Table to convert ASCII characters into nibbles (0-15).
	
	This table is used for code to quickly map the ASCII
	values of '0'-'9', 'A'-'F' and 'a'-'f' into the hex digits 0x00-0x0F.

***************************************/

const Word8 BURGER_ALIGN(Burger::g_AsciiToWord8Table[256],16) = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x00
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x10
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x20
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x30
	0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x40
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x50
	0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x60
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x70
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x80
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0x90
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0xA0
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0xB0
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0xC0
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0xD0
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// 0xE0
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF		// 0xF0
};

/*! ************************************

	\var const Word8 Burger::g_ReverseBits[256]
	\brief Table to reverse the bit order of a byte.
	
	This table will take the a Word8 used as an index and return the bit reversed value
	of that index. 0x80 -> 0x01, 0xAA -> 0x55, 0x32 -> 0x4C.

***************************************/

const Word8 BURGER_ALIGN(Burger::g_ReverseBits[256],16) = {
	0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,	// 0x00
	0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,	// 0x10
	0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,	// 0x20
	0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,	// 0x30
	0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,	// 0x40
	0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,	// 0x50
	0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,	// 0x60
	0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,	// 0x70
	0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,	// 0x80
	0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,	// 0x90
	0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,	// 0xA0
	0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,	// 0xB0
	0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,	// 0xC0
	0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,	// 0xD0
	0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,	// 0xE0
	0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF		// 0xF0
};

/*! ************************************

	\brief Table used for testing powers of ten for 32 bit integer to ASCII conversion.
	
	This table is used for code to test a 32 bit integer against a
	power of ten for divisionless conversion of an integer
	to ASCII text.

***************************************/

const Word32 Burger::g_TensTable32[10] = {
1,				// Table to quickly div by a power 10
10,
100,
1000,
10000,
100000,
1000000,
10000000,
100000000,
1000000000
};

/*! ************************************

	\brief Table used for testing powers of ten for 64 bit integer to ASCII conversion.
	
	This table is used for code to test a 64 bit integer against a
	power of ten for divisionless conversion of an integer
	to ASCII text.

***************************************/

const Word64 Burger::g_TensTable64[20] = {
1ULL,				// Table to quickly div by a power 10
10ULL,
100ULL,
1000ULL,
10000ULL,
100000ULL,
1000000ULL,
10000000ULL,
100000000ULL,
1000000000ULL,
10000000000ULL,
100000000000ULL,
1000000000000ULL,
10000000000000ULL,
100000000000000ULL,
1000000000000000ULL,
10000000000000000ULL,
100000000000000000ULL,
1000000000000000000ULL,
10000000000000000000ULL
};

/*! ************************************

	\brief Convert a BCD value into a decimal integer.
	
	Given an input value of 0x1234, assume it is 
	Binary Coded Decimal (BCD) format and convert it
	to become the decimal integer 1234.
	
	BCD's encoding rules dictate that only the number 0-9
	are valid. Any use of the hex digits A-F are considered
	illegal.

	\param uInput 32-bit hex value in BCD.
	\return Value in decimal format.
	
	\note The returned value is undefined if the input is not
	in BCD format.
	
***************************************/

Word32 BURGER_API Burger::BCDToWord(Word32 uInput)
{
	Word32 uOutput = 0;		// Assume nothing
	Word32 uMul = 1;		// Default multiplier
	do {
		// Parse the lowest digit
		uOutput = ((uInput&0xFU)*uMul)+uOutput;
		uInput>>=4U;		// Shift out the nibble
		uMul = uMul*10U;	// Adjust the multiplier		
	} while (uInput);		// Any more?
	return uOutput;
}

/*! ************************************

	\brief Convert a decimal integer into BCD format.
	
	Given a decimal in the range of 0-99999999, convert
	it into that value into Binary Coded Decimal (BCD)
	format. Such that 1234 become 0x1234 and 50331 become
	0x00050331. If the input value is greater than 99999999
	it is clamped to 99999999.
	
	\param uInput Integer value in the range of 0 to 99999999.
	\return Input encoded in BCD.
	
***************************************/

Word32 BURGER_API Burger::WordToBCD(Word32 uInput)
{
	Word32 uOutput=99999999;

	if (uInput<99999999) {		// Too large?
		Word32 uTemp;
		uOutput = 0;			// Init output
		// Because divides cost so much, Try to skip them with branches
		if (uInput>=1000000) {
			uTemp = uInput/10000000;
			uOutput = uTemp<<28;
			uInput -= uTemp*10000000;
			uTemp = uInput/1000000;
			uOutput |= uTemp<<24;
			uInput -= uTemp*1000000;
		}
		if (uInput>=10000) {
			uTemp = uInput/100000;
			uOutput |= uTemp<<20;
			uInput -= uTemp*100000;
			uTemp = uInput/10000;
			uOutput |= uTemp<<16;
			uInput -= uTemp*10000;
		}
		if (uInput>=100) {
			uTemp = uInput/1000;
			uOutput |= uTemp<<12;
			uInput -= uTemp*1000;
			uTemp = uInput/100;
			uOutput |= uTemp<<8;
			uInput -= uTemp*100;
		}
		// Just do the last two digits.
		uTemp = uInput/10;
		uOutput |= uTemp<<4;
		uInput -= uTemp*10;	

		uOutput |= uInput;
	}
	return uOutput;
}

/*! ************************************

	\brief Round up an integer to the nearest power of 2
	
	Take an arbitrary value and round it up to
	the nearest power of 2
	If the input is 0x40000001 to 0x7FFFFFFF, the function returns 0x80000000
	0x80000000-0xFFFFFFFF is an overflow and returns zero.
	Zero will return zero
	
	\param uInput Integer value to round up
	\return Rounded up value
	\sa PowerOf2(Word64)

***************************************/

// Watcom, Metrowerks and Visual C version (Note, assembly is only allowed for 32 bit mode)

#if ((defined(BURGER_WATCOM) || defined(BURGER_FASTCALLENABLED)) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED Word32 BURGER_API Burger::PowerOf2(Word32 /* uInput */)
{
	BURGER_ASM {
	// Force 0x4000 to be 0x3fff so in the end
	// it's changed back to 0x4000
#if defined(BURGER_WATCOM)
	push ecx
	lea ecx,[eax-1]		// Watcom passes uInput in eax
	push edx			// Don't destroy edx for Watcom
#else
	sub ecx,1			// Metrowerks and Visual C passes uInput in ecx
#endif
	xor eax,eax
	// Find the LEFTMOST bit, uInput will have 0x00 to 0x1F
	// Zero will set the zero flag and leave uInput undefined
	bsr ecx,ecx
	setnz al
	// This handles all but the 0 case (0x1 is input)
	// In the zero case, uZero is 0, so the result is zero
	mov edx,eax
	shl eax,cl
	xor edx,1
	// I'm one bit shy, so shift up one bit and use
	// an add for Intel efficiency
	// and finally, for the zero case, or with 1
	// if the input was 1 (1-1) -> 0, so, uZero^1 = 1+0 = 1
	lea eax,[edx+eax*2]
#if defined(BURGER_WATCOM)
	pop edx				// Restore edx and ecx for Watcom
	pop ecx
#endif
	ret
	}
}

// Metrowerks PowerPC version
#elif (defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)) && !defined(DOXYGEN)
asm Word32 BURGER_API Burger::PowerOf2(register Word32 uInput)
{
	subi	r4,r3,1			// Adjust the input
	neg		r5,r4			// Test r4 for zero
	cntlzw	r3,r4			// Count the leading zeros
	or		r5,r5,r4
	subfic	r0,r3,32		// I want the reverse to the count
	srwi	r5,r5,31		// if r4 was zero, r5 = 0, else 1
	xori	r4,r5,1			// Get the reverse of r5
	slw		r3,r5,r0		// Shift 1<<count or 0<<count (Can overflow which is okay)
	add		r3,r3,r4		// Add in 1 in case of 0<<count
	blr
}

// GNU version for MacOSX
#elif defined(BURGER_INTELARCHITECTURE) && defined(BURGER_GNUC) && !defined(DOXYGEN)
Word32 BURGER_API Burger::PowerOf2(Word32 uInput)
{
	// Use the Intel instruction Bit Scan Reverse to 
	// speed up the bit search
	Word8 bZero;
	// Force 0x4000 to be 0x3fff so in the end
	// it's changed back to 0x4000
	--uInput;
	// Find the LEFTMOST bit, uInput will have 0x00 to 0x1F
	// Zero will set the zero flag and leave uInput undefined
	__asm__("bsr %1,%1 \n \
			setne %0" : "=a" (bZero),
			 "+r" (uInput)
			);
	// Convert the byte to a 32 bit value
	Word uZero = bZero;
	// This handles all but the 0 case (0x1 is input)
	// In the zero case, uZero is 0, so the result is zero
	uInput = uZero<<uInput;
	// I'm one bit shy, so shift up one bit and use
	// an add for Intel efficiency
	// and finally, for the zero case, or with 1
	// if the input was 1 (1-1) -> 0, so, uZero^1 = 1+0 = 1
	return uInput+uInput+(uZero^1);
}

#else

Word32 BURGER_API Burger::PowerOf2(Word32 uInput)
{
	// To do this, I blend the bits so from the highest will set
	// EVERY bit slot to the right.
	// I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
	// So, by adding one, it will be the next power of 2
	// Since 0 becomes 0xFFFFFFFF at the start, all
	// the or's and shifts do nothing and 0x00 will be
	// returned due to overflow
	
	--uInput;
	uInput |= uInput>>1;	// Blend in the odd bits.
	uInput |= uInput>>2;	// Now, every 2nd bit
	uInput |= uInput>>4;	// Every 4th bit
	uInput |= uInput>>8;	// Every 8th bit
	uInput |= uInput>>16;	// Final pass!
	++uInput;				// Inc and I have the power of 2
	return uInput;
	
}
#endif

/*! ************************************

	\brief Round up an integer to the nearest power of 2
	
	Take an arbitrary value and round it up to
	the nearest power of 2
	If the input is 0x4000000000000001 to 0x7FFFFFFFFFFFFFFF, the function returns 0x8000000000000000
	0x8000000000000000-0xFFFFFFFFFFFFFFFF is an overflow and returns zero.
	Zero will return zero
	
	\param uInput Integer value to round up
	\return Rounded up value
	\sa PowerOf2(Word32)
	
***************************************/

Word64 BURGER_API Burger::PowerOf2(Word64 uInput)
{
	// To do this, I blend the bits so from the highest will set
	// EVERY bit slot to the right.
	// I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
	// So, by adding one, it will be the next power of 2
	// Since 0 becomes 0xFFFFFFFF at the start, all
	// the or's and shifts do nothing and 0x00 will be
	// returned due to overflow

	--uInput;
	uInput |= uInput>>1;	// Blend in the odd bits.
	uInput |= uInput>>2;	// Now, every 2nd bit
	uInput |= uInput>>4;	// Every 4th bit
	uInput |= uInput>>8;	// Every 8th bit
	uInput |= uInput>>16;	// Final pass!
	uInput |= uInput>>32;	// Final pass!
	++uInput;				// Inc and I have the power of 2
	return uInput;
}


/*! ************************************

	\fn Word32 Burger::ToLower(Word32 uInput)
	\brief Convert a character to lower case

	Using ASCII encoding, check if the value is between 'A' to 'Z' inclusive
	and if true, convert to 'a' to 'z'.
	
	\param uInput ASCII character to convert to lower case
	\return Character converted to lower case using ASCII encoding
	\sa ToUpper(Word32)
	
***************************************/

/*! ************************************

	\fn Word32 Burger::ToUpper(Word32 uInput)
	\brief Convert a character to lower case

	Using ASCII encoding, check if the value is between 'a' to 'z' inclusive
	and if true, convert to 'A' to 'Z'.
	
	\param uInput ASCII character to convert to upper case
	\return Character converted to upper case using ASCII encoding
	\sa ToLower(Word32)
	
***************************************/


/*! ************************************

	\brief Reverse the bits in a byte

	Given a bit width (From 1 through 32), reverse the
	order of the bits within.

	Since this version is variable width, if uBitLength is equal to 4, the truth table
	would look like this...

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>Input</th><th>Output</th></tr>
	<tr><td>0x00</td><td>0x00</td></tr>
	<tr><td>0x01</td><td>0x02</td></tr>
	<tr><td>0x02</td><td>0x01</td></tr>
	<tr><td>0x03</td><td>0x03</td></tr>
	</table>

	Whereas if uBitLength is equal to 8, the truth table
	would look like this...

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>Input</th><th>Output</th></tr>
	<tr><td>0x00</td><td>0x00</td></tr>
	<tr><td>0x01</td><td>0x80</td></tr>
	<tr><td>0x02</td><td>0x40</td></tr>
	<tr><td>0x03</td><td>0xC0</td></tr>
	</table>

	\note This function will set the unused bits to zero, so if
	a 3 bit number is requested, the upper 29 (32-3) bits
	will be set to zero.

	\param uInput Value to bit reverse
	\param uBitLength Width of the value to reverse (1 through 32)
	\return uInput with the bits reversed
	\sa BitReverse(Word64,Word)

***************************************/


// Watcom, Metrowerks and Visual C version (Note, assembly is only allowed for 32 bit mode)

#if ((defined(BURGER_WATCOM) || defined(BURGER_FASTCALLENABLED)) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED Word32 BURGER_API Burger::BitReverse(Word32 /* uInput */,Word /* uBitLength */)
{
	BURGER_ASM {
#if defined(BURGER_WATCOM)
		push ecx		// ecx needs to be saved for Watcom
		mov	ecx,eax		// Watcom passes in eax
#endif
		xor eax,eax		// Initialize the result
BitReverse32:
		shr	ecx,1		// Shift out a bit
		rcl eax,1		// Shift into the result
		sub edx,1		// Count down the iterator
		jne BitReverse32
#if defined(BURGER_WATCOM)
		pop ecx			// Restore ecx for Watcom
#endif
		ret
	}
}

#else

Word32 BURGER_API Burger::BitReverse(Word32 uInput,Word uBitLength)
{
	Word32 uResult = 0;		// Initialize the result
	do {
		uResult = uResult+uResult;
		uResult += (uInput & 1U);
		uInput >>= 1U;
	} while (--uBitLength);
	return uResult;
}
#endif

/*! ************************************

	\brief Reverse the bits in a byte

	Given a bit width (From 1 through 64), reverse the
	order of the bits within.

	Since this version is variable width, if uBitLength is equal to 4, the truth table
	would look like this...

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>Input</th><th>Output</th></tr>
	<tr><td>0x00</td><td>0x00</td></tr>
	<tr><td>0x01</td><td>0x02</td></tr>
	<tr><td>0x02</td><td>0x01</td></tr>
	<tr><td>0x03</td><td>0x03</td></tr>
	</table>

	Whereas if uBitLength is equal to 8, the truth table
	would look like this...

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>Input</th><th>Output</th></tr>
	<tr><td>0x00</td><td>0x00</td></tr>
	<tr><td>0x01</td><td>0x80</td></tr>
	<tr><td>0x02</td><td>0x40</td></tr>
	<tr><td>0x03</td><td>0xC0</td></tr>
	</table>

	\note This function will set the unused bits to zero, so if
	a 3 bit number is requested, the upper 61 (64-3) bits
	will be set to zero.

	\param uInput Value to bit reverse
	\param uBitLength Width of the value to reverse (1 through 64)
	\return uInput with the bits reversed
	\sa BitReverse(Word32,Word)

***************************************/

Word64 BURGER_API Burger::BitReverse(Word64 uInput,Word uBitLength)
{
	Word64 uResult = 0;		// Initialize the result
	do {
		uResult = uResult+uResult;
		uResult += (uInput & 1U);
		uInput >>= 1U;
	} while (--uBitLength);
	return uResult;
}

/*! ************************************

	\brief Calculate the number of set bits

	Given a 32 bit integer, count the number
	of bits set and return the value
	from zero to thirty two.

	\param uInput Integer to count the bits from
	\return 0 to 32
	\sa BitSetCount(Word64)

***************************************/

Word BURGER_API Burger::BitSetCount(Word32 uInput)
{
	// Use vector adding to count the bits
	// Stage 1, add 16 pairs of 1 bit numbers
	uInput = uInput - ((uInput >> 1) & 0x55555555U);
	// Stage 2, add 8 pairs of 2 bit numbers
	uInput = (uInput & 0x33333333U) + ((uInput >> 2U) & 0x33333333U);
	// Stage 3, add 4 pairs of 4 bit numbers and then sum
	// them with a vector multiply in which the upper 8 bits is the count
	// Neat, eh?
	return (((uInput + (uInput >> 4U)) & 0x0F0F0F0FU) * 0x01010101U) >> 24U;
}

/*! ************************************

	\brief Calculate the number of set bits

	Given a 64 bit integer, count the number
	of bits set and return the value
	from zero to sixty four.

	\param uInput Integer to count the bits from
	\return 0 to 64
	\sa BitSetCount(Word32)

***************************************/

Word BURGER_API Burger::BitSetCount(Word64 uInput)
{
	// If the CPU is a 64 bit one, do it the fast way
#if defined(BURGER_64BITCPU) && !defined(BURGER_METROWERKS)
	// Use vector adding to count the bits
	// Stage 1, add 16 pairs of 1 bit numbers
	uInput = uInput - ((uInput >> 1) & 0x5555555555555555UL);
	// Stage 2, add 8 pairs of 2 bit numbers
	uInput = (uInput & 0x3333333333333333U) + ((uInput >> 2U) & 0x3333333333333333ULL);
	// Stage 3, add 4 pairs of 4 bit numbers and then sum
	// them with a vector multiply in which the upper 8 bits is the count
	// Neat, eh?
	uInput = (((uInput + (uInput >> 4U)) & 0x0F0F0F0F0F0F0F0FULL) * 0x0101010101010101ULL) >> 56U;
	return static_cast<Word>(uInput);
#else
	// For 32 bit CPUs, break it into two 32 bit chunks to make the code simpler
	// (Note, the 64 bit code breaks on CodeWarrior in debugging mode due to a broken 64 bit multiply
	// call)
	return BitSetCount(static_cast<Word32>(uInput))+BitSetCount(static_cast<Word32>(uInput>>32U));
#endif
}

/*! ************************************

	\brief Convert a "C" string into a Pascal string.
	
	Copy a "C" string and convert it into a Pascal style string. A
	byte will denote the length, followed by the text itself. There
	will be no terminating zero in the new string.
	
	The destination and the source pointer can contain the same value. This
	function allows in buffer data conversion. 
	
	\note If the input string is greater than 255 characters in length,
	it will be truncated at 255 characters.
	
	\note While the input and output strings can be the same value for
	in place conversions, they cannot be pointing to overlapping sections
	of the same buffer. Either the buffers must be independent of
	each other, or they are pointing to the exact same buffer. Overlap
	will cause undefined behavior.

	\param pOutput Pointer to a buffer (max 256 bytes) to receive the new string. \ref NULL will page fault.
	\param pInput Pointer to the "C" string to copy. \ref NULL will page fault.
	\sa Burger::PStringToCString(char *,const Word8*);
	
***************************************/

void BURGER_API Burger::CStringToPString(Word8 *pOutput,const char *pInput)
{
	// Note, empty "C" and Pascal strings are a single 0,
	// so if the "C" string is empty, no action is needed!
	
	// Prefetch first char
	Word uTemp1 = reinterpret_cast<const Word8*>(pInput)[0];		
	if (uTemp1) {			// End of string?
		++pInput;
		Word8 *pWork = pOutput;	// Set the work pointer
		++pWork;				// Index past the length byte
		Word uLength = 0;		// Set the length of the string
		do {
			// Grab the next character to prevent an overwrite if the source
			// and dest buffers are the same

			Word uTemp2 = reinterpret_cast<const Word8*>(pInput)[0];	
			++pInput;
			pWork[0] = static_cast<Word8>(uTemp1);	// Save to dest string
			++pWork;
			if (++uLength==255) {
				break;		// Overflow!
			}
			uTemp1 = uTemp2;	// Get source string
		} while (uTemp1);	// Still more?
		pOutput[0] = static_cast<Word8>(uLength);	// Save the length byte for PString
	}
}

/*! ************************************

	\brief Convert a Pascal string into a "C" string.
	
	Copy a Pascal string and convert it into a "C" style string. A
	byte will denote the length, followed by the text itself. There
	will be text followed by a terminating zero in the new string.
	
	The destination and the source pointer can contain the same value. This
	function allows in buffer data conversion.

	\note While the input and output strings can be the same value for
	in place conversions, they cannot be pointing to overlapping sections
	of the same buffer. Either the buffers must be independent of
	each other, or they are pointing to the exact same buffer. Overlap
	will cause undefined behavior.

	\param pOutput Pointer to a buffer (max 256 bytes) to receive the new string. \ref NULL will page fault.
	\param pInput Pointer to the Pascal string to copy. \ref NULL will page fault.
	\sa Burger::CStringToPString(Word8 *,const char *);

***************************************/

void BURGER_API Burger::PStringToCString(char *pOutput,const Word8 *pInput)
{
	Word uCount = pInput[0];		// Get the string length
	if (uCount) {
		++pInput;
		do {
			reinterpret_cast<Word8*>(pOutput)[0] = pInput[0];	// Copy a byte
			++pInput;
			++pOutput;
		} while (--uCount);	// Count down
		// Note, it's here because a zero length
		// Pascal string is by default, null terminated
		pOutput[0] = 0;			// Terminating zero
	}
}

/*! ************************************

	\brief Parse a "C" string until a non-white space character is found.
	
	Skip over any space (32) or tab (9) character and return the pointer
	to the first character that doesn't match. 
	
	\param pInput Pointer to a "C" string to parse. \ref NULL will page fault.
	\return Pointer to the first non-white space character .
	\sa Burger::ParseToDelimiter(const char *pInput)

***************************************/

char * BURGER_API Burger::ParseBeyondWhiteSpace(const char *pInput)
{
	Word uTemp;		// Temp storage
	do {
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Get a byte of input
		++pInput;
	} while ((uTemp==32) || (uTemp==9));	// Space or TAB?
	return const_cast<char*>(--pInput);		// Return the result pointer
}

/*! ************************************

	\brief Return a pointer to whitespace, CR, LF or zero

	Parse a string until a whitespace,
	CR, LF or zero is found. Return the pointer at the
	point where the requested character is. Useful in
	parsing a token and finding the end of it.

	\param pInput Pointer to a "C" string to parse. \ref NULL will page fault.

	\return Pointer to a TAB, Space, CR, LF or zero.
	\sa Burger::ParseBeyondWhiteSpace(const char *)

***************************************/

char * BURGER_API Burger::ParseToDelimiter(const char *pInput)
{
	Word uTemp;		// Temp storage
	do {
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Get a byte of input
		++pInput;
	} while (uTemp && (uTemp!=9) && (uTemp!=10) && (uTemp!=13) && (uTemp!=32));		// Space or TAB?
	return const_cast<char*>(--pInput);		// Return the result pointer
}

/*! ************************************

	\brief Parse a "C" string until a zero or EOL.
	
	Follow a stream of text input until either a zero is found
	or an End Of Line is found.
	
	If a zero is found, return a pointer to the ZERO.
	
	If an EOL is found, return a pointer to the text BEYOND
	the EOL which is a "\n" (10) or a "\r" (13). In the case of a CR/LF
	combination, found in PC style text files, return the pointer
	beyond the pair.

	\param pInput Pointer to a "C" string to parse. \ref NULL will page fault.
	\return Pointer to the zero, or EOL character.
	
***************************************/

char * BURGER_API Burger::ParseBeyondEOL(const char *pInput)
{
	Word uTemp;		// Temp storage
	do {
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Get a byte of input 
		if (!uTemp) {		// End now?
			break;
		}
		++pInput;			// Accept the char
		if (uTemp==13) {	// Mac or PC style EOL?
			if (reinterpret_cast<const Word8 *>(pInput)[0]==10) {	// Followed by a PC LF?
				++pInput;		// Accept the LF as well
			}
			break;		// Exit now
		}
	} while (uTemp!=10);		// Unix EOL?
	return const_cast<char*>(pInput);		// Return the result pointer
}

/*! ************************************

	\brief Parse a "C" string until a zero or EOL or out of data.
	
	Follow a stream of text input until either a zero is found,
	an End Of Line is found or data ran out.
	
	If a zero is found, return a pointer to the ZERO.
	
	If an EOL is found, return a pointer to the text BEYOND
	the EOL which is a "\n" (10) or a "\r" (13). In the case of a CR/LF
	combination, found in PC style text files, return the pointer
	beyond the pair.

	\param pInput Pointer to a string to parse. \ref NULL will page fault.
	\param uLength Length of the string buffer.
	\return Pointer to the zero, or EOL character.
	
***************************************/

char * BURGER_API Burger::ParseBeyondEOL(const char *pInput,WordPtr uLength)
{
	Word uTemp;		// Temp storage
	if (uLength) {
		do {
			uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Get a byte of input 
			if (!uTemp) {		// End now?
				break;
			}
			++pInput;			// Accept the char
			// Out of data?
			if (!--uLength) {
				break;
			}
			if (uTemp==13) {	// Mac or PC style EOL?
				if (reinterpret_cast<const Word8 *>(pInput)[0]==10) {	// Followed by a PC LF?
					++pInput;		// Accept the LF as well
				}
				break;		// Exit now
			}
		} while (uTemp!=10);		// Unix EOL?
	}
	return const_cast<char*>(pInput);		// Return the result pointer
}

/*! ************************************

	\brief Parse out a quoted string.
	
	If the first character of the string is a quote "\"" (0x22) or "'",
	then parse out a quote delimited string. This function will
	parse double quotes ("" or '') as a single quote to be inserted into the string.
	Whichever was the first quote character found, that will be the quote character
	to be used for the double quote and end quote parser making the alternate
	quote treated as a normal character.

	If the first character is 0 or whitespace, return pInput to signal an error condition.

	If the first character isn't a quote, 0 or whitespace, parse the string until
	another whitespace character is found.

	All tabs (\\t) in a quoted string are converted into spaces.

	All strings terminate if a 0 or EOL (\\r, \\r\\n, \\n) sequence is found. If this
	type of terminator is found, the returned pointer will point to the terminating
	character (Except in the case of \\r\\n, where it will point at the \\n character).

	If the output buffer isn't large enough to hold the resulting string, truncate
	the output to fit. The output will always be zero terminated in all cases.

	\code 
		char output[256];
		const char *pInput = "Hello";
		char *pError = ParseQuotedString(output,sizeof(output),pInput);
		// Determining if output is really an empty string or a parse error
		if (pError==pInput) {
			printf("Nothing parsed, not an empty string");
		}
		
	\endcode 

	\param pOutput Pointer to the output string. \ref NULL is acceptable if uOutputSize is zero.
	\param uOutputSize Size of the output string buffer in bytes.
	\param pInput Pointer to a "C" string that has a '"' as the first character.
		\ref NULL will page fault.
	\return Pointer to the character that ended parsing. +1 beyond '"', or at CR, at LF or at zero.

***************************************/

char * BURGER_API Burger::ParseQuotedString(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	Word uLetter = reinterpret_cast<const Word8 *>(pInput)[0];		// Get the first char
	Word uQuote = ' ';

	// Is the first character even allowed to be parsed?
	if ((uLetter!=' ') && 
		uLetter && 
		(uLetter!=10) &&
		(uLetter!=13) && 
		(uLetter!=9)) {
		if (uLetter=='"' || uLetter=='\'') { // Does it start with a quote?
			uQuote = uLetter;			// Whichever one it is, use it as the delimiter
		} else {
			--pInput;		// Undo the future ++pInput
		}
		
		WordPtr uMax = uOutputSize;	// Get the size of the buffer to write to
		if (uMax) {
			--uMax;					// Leave space for the end zero
		}
		for (;;) {
			++pInput;			// Accept a character
			uLetter = reinterpret_cast<const Word8 *>(pInput)[0];
			
			if (uLetter==9) {	// Tabs become spaces
				uLetter = ' ';
			}
			
			// Did I hit a delimiter?
			if (uLetter==uQuote) {	// Double quote?
				if (uLetter==' ') {		// If it's a space, exit immediately
					break;
				}
				++pInput;			// Accept the quote
				// Double quote?
				if (reinterpret_cast<const Word8 *>(pInput)[0]!=uQuote) {
					break;		// Nope, exit then
				}
			} else if ((!uLetter) || (uLetter==10)) {		// End of line?
				break;
			} else if (uLetter==13) {
				if (reinterpret_cast<const Word8 *>(pInput)[0]==10) {
					++pInput;
				}
				break;
			}
			// If I got here, it means the character is acceptable.
			if (uMax) {		// Can it fit?
				pOutput[0] = static_cast<char>(uLetter);		// Save resulting ASCII
				++pOutput;
				--uMax;
			}
		}
	}
	if (uOutputSize) {		// Is there an output buffer?
		pOutput[0] = 0;		// Zero terminate
	}
	return const_cast<char *>(pInput);		// Return dest buffer pointer
}



/*! ************************************

	\brief Read a stream of text chars until a \ref NULL, LF, CR or CR/LF is found.
	
	Copy from the input stream of text characters until either a zero, 
	a LF (Unix), CR/LF (Windows), or CR (Mac) is found. The output
	buffer will be zero terminated and will truncate excess
	data.
	
	The value returned is the number of bytes that was processed.
	This number is how many bytes from the original input was used.
	
	\param pOutput Pointer to the output buffer (Can be \ref NULL if uOutputSize is zero).
	\param uOutputSize Size in bytes of the output buffer.
	\param pInput Pointer to the input buffer.
	\param uInputSize Size in bytes of the maximum number of bytes to process.
	
	\return The number of input bytes processed.
	
***************************************/

WordPtr BURGER_API Burger::CopyUpToEOL(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	const Word8 *pWork = reinterpret_cast<const Word8*>(pInput);
	if (uInputSize) {		// Any input data?

		WordPtr uOutSize = uOutputSize;	// Make a copy of the output buffer size

		// Remove 1 byte to ensure space for the terminating zero

		if (uOutSize) {
			--uOutSize;					
		}

		// Let's copy the data
		
		do {
			Word Temp = pWork[0];		// Grab a char from the input stream

			if (!Temp) {				// Do NOT accept NULL's
				break;
			}
			++pWork;					// Accept the char
			if (Temp==10) {				// Unix LF?
				break;					// Accept and end
			}
			
			if (Temp==13) {				// Look for the CR/LF case
				if (uInputSize>=2) {		// It there data left?

				// Yank out the LF from the stream in the CR/LF case.

					if (pWork[0]==10) {
						++pWork;		// Accept the char
					}
				}
				break;		// Bye!
			}

			// Save the character to the output
			
			if (uOutSize) {			// Any space left in the output buffer?
				pOutput[0] = static_cast<char>(Temp);		// Store the char
				++pOutput;		// Accept it
				--uOutSize;			// Reduce the output buffer size
			}
		} while (--uInputSize);
	}

	// If the output buffer had even one char, I can write the terminating zero.

	if (uOutputSize) {
		pOutput[0] = 0;		// Terminate the output
	}
	
	// Calculate the number of bytes parsed and exit.
	return static_cast<Word32>(reinterpret_cast<const char *>(pWork)-pInput);
}

/*! ************************************

	\brief Remove spaces from the beginning of a string.

	Starting from the beginning of a string, see if the character is a space (32). If
	so, then the character is removed by copying the rest of the string up. This is
	repeated until there are no more spaces at the beginning of the string or the
	string is empty.

	Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes "Foo", &quot;Foo&nbsp;&nbsp;&nbsp;&quot; remains &quot;Foo&nbsp;&nbsp;&nbsp;&quot;, &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes &quot;Foo&nbsp;&nbsp;&nbsp;&quot;.

	\param pInput Pointer to the "C" string to remove beginning spaces from. \ref NULL will page fault.
	\sa Burger::StripLeadingWhiteSpace(char *), Burger::StripTrailingSpaces(char *), Burger::StripLeading(char *,const char *).

***************************************/

void BURGER_API Burger::StripLeadingSpaces(char* pInput)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];
	if (uTemp==' ') {						// Is there a leading space?
		char *pTemp = pInput;				// Save the starting point of the string
		++pInput;							// Accept the char
		do {
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Fetch the next char
			++pInput;
		} while (uTemp==' ');				// Look for the end of the spaces

		if (uTemp) {						// End of the string?
			do {
				pTemp[0] = static_cast<char>(uTemp);	// Now, copy the string to the beginning of
				++pTemp;					// the buffer
				uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Accept the next char
				++pInput;
			} while (uTemp);					// Did I copy the ending zero?
		}
		// uTemp == 0 here
		pTemp[0] = static_cast<char>(uTemp);	// Store the final zero
	}
}

/*! ************************************

	\brief Remove spaces from the end of a string.

	Starting from the end of a string but before the ending zero, see if the last
	character is a space. If so, then the character is removed by zeroing it out and the
	process begins again until the string is empty or a non space character is at the
	end of the "C" string. For speed purposes, only a single zero is ever written to the
	"C" string. Do not assume that all the spaces that were removed were actually
	overwritten with zeros.

	Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; remains &quot;&nbsp;&nbsp;&nbsp;Foo&quot;, &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes "Foo", &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes &quot;&nbsp;&nbsp;&nbsp;Foo&quot;.

	\param pInput Pointer to the "C" string to remove ending spaces from. \ref NULL will page fault.
	\sa Burger::StripTrailingWhiteSpace(char *), Burger::StripLeadingSpaces(char *), Burger::StripTrailing(char *).

***************************************/

void BURGER_API Burger::StripTrailingSpaces(char* pInput)
{
	Word Temp = reinterpret_cast<Word8 *>(pInput)[0];	// Get the first character
	if (Temp) {								// Is there a string?
		char *pTemp = pInput;				// Init the zap pointer
		do {
			++pInput;						// Accept the char
			if (Temp!=' ') {				// Not a space?
				pTemp=pInput;				// Last VALID char
			}
			Temp = reinterpret_cast<Word8 *>(pInput)[0];
		} while (Temp);						// All done?
		// Temp == 0 here
		pTemp[0] = static_cast<char>(Temp);	// Zap the final char
	}
}

/*! ************************************

	\brief Remove whitespace from the beginning of a string.

	Starting from the beginning of a string, see if the character is a space or a tab. If
	so, then the character is removed by copying the rest of the string up. This is
	repeated until there are no more spaces or tabs at the beginning of the string or the
	string is empty.

	Example: &quot;&nbsp;\\t&nbsp;Foo&quot; becomes "Foo", &quot;Foo&nbsp;\\t&nbsp;&quot; remains &quot;Foo&nbsp;\\t&nbsp;&quot;, &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes &quot;Foo&nbsp;\\t&nbsp;&quot;.

	\param pInput Pointer to the "C" string to remove beginning whitespace from. \ref NULL will page fault.
	\sa Burger::StripLeadingSpaces(char *), Burger::StripTrailingSpaces(char *), Burger::StripLeading(char *,const char *)

***************************************/

void BURGER_API Burger::StripLeadingWhiteSpace(char* pInput)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];
	if ((uTemp==' ') || (uTemp=='\t')) {	// Is there a leading whitespace?
		char *pTemp = pInput;				// Save the starting point of the string
		++pInput;							// Accept the char
		do {
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Fetch the next char
			++pInput;
		} while ((uTemp==' ') || (uTemp=='\t'));	// Look for the end of the whitespace

		if (uTemp) {							// End of the string?
			do {
				pTemp[0] = static_cast<char>(uTemp);	// Now, copy the string to the beginning of
				++pTemp;					// the buffer
				uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Accept the next char
				++pInput;
			} while (uTemp);					// Did I copy the ending zero?
		}
		// Temp == 0 here
		pTemp[0] = static_cast<char>(uTemp);			// Store the final zero
	}
}

/*! ************************************

	\brief Remove whitespace from the end of a string

	Starting from the end of a string but before the ending zero, see if the last
	character is a space or tab. If so, then the character is removed by zeroing it
	out and the process begins again until the string is empty or a non space or tab
	character is at the end of the "C" string. For speed purposes, only a single zero
	is ever written to the "C" string. Do not assume that all the spaces and tabs that
	were removed were actually overwritten with zeros.

	Example: &quot;&nbsp;\\t&nbsp;Foo&quot; remains &quot;&nbsp;\\t&nbsp;Foo&quot;, &quot;Foo&nbsp;\\t&nbsp;&quot; becomes "Foo", &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes &quot;&nbsp;\\t&nbsp;Foo&quot;.

	\param pInput Pointer to the "C" string to remove ending spaces and tabs from. \ref NULL will page fault.

	\sa Burger::StripTrailingSpaces(char *), Burger::StripLeadingSpaces(char *), Burger::StripTrailing(char *,const char*)

***************************************/

void BURGER_API Burger::StripTrailingWhiteSpace(char* pInput)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Get the first character
	if (uTemp) {								// Is there a string?
		char *pTemp = pInput;					// Init the zap pointer
		do {
			++pInput;							// Accept the char
			if ((uTemp!=' ') && (uTemp!='\t')) {	// Not whitespace?
				pTemp=pInput;					// Last VALID char
			}
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];
		} while (uTemp);						// All done?
		// Temp == 0 here
		pTemp[0] = static_cast<char>(uTemp);	// Zap the final char
	}
}

/*! ************************************

	\brief Remove spaces from the beginning and end of a string.

	Starting from the beginning of a string, see if the character is a space. If so
	then the character is removed by copying the rest of the string up. This is repeated
	until there are no more spaces at the beginning of the string or the string is empty.
	Then the process is repeated but from the end of the string. The resulting string will
	not have any space characters at the beginning or the end.

	Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes "Foo", &quot;Foo&nbsp;&nbsp;&nbsp;&quot; becomes "Foo", &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes "Foo".

	\param pInput Pointer to the "C" string to remove beginning and ending spaces from. \ref NULL will page fault.

	\sa Burger::StripTrailingSpaces(char *), Burger::StripLeadingSpaces(char *), Burger::StripLeadingAndTrailingWhiteSpace(char *).

***************************************/

void BURGER_API Burger::StripLeadingAndTrailingSpaces(char* pInput)
{
	char *pEnd = pInput;					// Save the starting point of the string
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];
	++pInput;								// Accept the char
	if (uTemp==' ') {						// Is there a leading space?
		do {
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Fetch the next char
			++pInput;
		} while (uTemp==' ');				// Look for the end of the spaces
	}

	if (uTemp) {							// End of the string?
		char *pTemp = pEnd;					// Begin storing here
		do {
			pTemp[0] = static_cast<char>(uTemp);	// Now, copy the string to the beginning of
			++pTemp;						// the buffer
			if (uTemp!=' ') {				// Is this a forbidden last char?
				pEnd = pTemp;
			}
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Accept the next char
			++pInput;
		} while (uTemp);					// No more string?
	}
	// Temp == 0 here
	pEnd[0] = static_cast<char>(uTemp);		// Store the final zero
}

/*! ************************************

	\brief Remove whitespace from the beginning and end of a string.

	Starting from the beginning of a string, see if the character is whitespace. If so
	then the character is removed by copying the rest of the string up. This is repeated
	until there are no more whitespace at the beginning of the string or the string is empty.
	Then the process is repeated but from the end of the string. The resulting string will
	not have any whitespace characters at the beginning or the end.

	Example: &quot;&nbsp;\\t&nbsp;Foo&quot; becomes "Foo", &quot;Foo&nbsp;\\t&nbsp;&quot; becomes "Foo", &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes "Foo".

	\param pInput Pointer to the "C" string to remove beginning and ending whitespace from. \ref NULL will page fault.

	\sa Burger::StripTrailingWhiteSpace(char *), Burger::StripLeadingWhiteSpace(char *), Burger::StripLeadingAndTrailingSpaces(char *)

***************************************/

void BURGER_API Burger::StripLeadingAndTrailingWhiteSpace(char* pInput)
{
	char *pEnd = pInput;							// Save the starting point of the string
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];
	++pInput;								// Accept the char
	if ((uTemp==' ') || (uTemp =='\t')) {		// Is there a leading space?
		do {
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Fetch the next char
			++pInput;
		} while ((uTemp==' ') || (uTemp=='\t'));	// Look for the end of the spaces
	}

	if (uTemp) {							// End of the string?
		char *pTemp = pEnd;					// Begin storing here
		do {
			pTemp[0] = static_cast<char>(uTemp);		//Now, copy the string to the beginning of
			++pTemp;							// the buffer
			if ((uTemp!=' ') && (uTemp!='\t')) {	// Is this a forbidden last char?
				pEnd = pTemp;
			}
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];		// Accept the next char
			++pInput;
		} while (uTemp);					// No more string?
	}
	pEnd[0] = static_cast<char>(uTemp);		// Store the final zero
}

/*! ************************************

	\brief Remove all characters that match those in a list

	The Input string is scanned and every character that is in the pList "C"
	string will be removed and compacted. The resulting string consists only of
	characters that are not found in the pList "C" string.

	\note This code is case sensitive.

	\param pInput Pointer to the "C" string to purge. \ref NULL will page fault.
	\param pList Pointer to the "C" string that contains the characters to remove. Do not pass a \ref NULL pointer.

	\sa Burger::StripAllButList(char*,const char*), Burger::StripLeadingSpaces(char*), Burger::StripTrailingSpaces(char*)

***************************************/

void BURGER_API Burger::StripAllFromList(char* pInput,const char* pList)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Get a character of the input
	if (uTemp &&
		reinterpret_cast<const Word8 *>(pList)[0]) {	// Any input?
		char *pTemp = pInput;							// Destination pointer
		++pInput;										// Accept the fetched character
		do {
			pTemp[0] = static_cast<char>(uTemp);		// Accept the char
			++pTemp;
			Word uTest = reinterpret_cast<const Word8 *>(pList)[0];
			const Word8 *pWork = reinterpret_cast<const Word8*>(pList);
			do {
				++pWork;
				if (uTest==uTemp) {		// Is it in the list?
					--pTemp;			// Reverse the output pointer to "undo" the acceptance
					break;
				}
				uTest = pWork[0];		// Next one in the list.
			} while (uTest);			// Any more?
			uTemp = reinterpret_cast<Word8 *>(pInput)[0];
			++pInput;
		} while (uTemp);
		pTemp[0] = 0;		// Terminate the string
	}
}

/*! ************************************

	\brief Remove all characters except those in a list

	The Input string is scanned and every character that is not in the pList "C"
	string will be removed and compacted. The resulting string consists only of
	characters that are found in the pList "C" string.

	\note This code is case sensitive.

	\param pInput Pointer to the "C" string to purge. \ref NULL will page fault.
	\param pList Pointer to the "C" string that contains the valid characters to allow.

	\sa Burger::StripAllFromList(char *,const char *), Burger::StripLeadingSpaces(char *), Burger::StripTrailingSpaces(char *)

***************************************/

void BURGER_API Burger::StripAllButList(char* pInput,const char* pList)
{
	if (reinterpret_cast<Word8 *>(pInput)[0]) {		// Any input
		char *pTemp = pInput;						// Destination pointer
		if (reinterpret_cast<const Word8*>(pList)[0]) {		// Is there a keep pointer?
			Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Grab an input char
			do {
				++pInput;
				Word uTemp2 = reinterpret_cast<const Word8*>(pList)[0];
				const Word8 *pTempList = reinterpret_cast<const Word8*>(pList);
				do {
					++pTempList;
					if (uTemp2==uTemp) {
						pTemp[0] = static_cast<char>(uTemp);		// Accept the char
						++pTemp;
						break;
					}
					uTemp2 = pTempList[0];
				} while (uTemp2);	// Any more?
				uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Grab an input char
			} while (uTemp);
		}
		pTemp[0] = 0;		// Terminate the string
	}
}

/*! ************************************

	\brief Remove characters from the end of a string

	Starting from the end of a string but before the ending zero, see if the
	character found is inside the string passed in pList. If so, then the character
	is removed by zeroing it out and the process begins again until the string is
	empty or a character that is not in the list is found.

	\note This code is case sensitive.

	\param pInput Pointer to the "C" string to remove ending characters from. Do not pass
		a \ref NULL pointer.
	\param pList Pointer to the "C" string that contains the characters to remove.
		Do not pass a \ref NULL pointer.

	\sa Burger::StripAllButList(char *,const char *), Burger::StripLeadingSpaces(char *) or Burger::StripTrailingSpaces(char *)

***************************************/

void BURGER_API Burger::StripTrailing(char* pInput,const char* pList)
{
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	if (uTemp) {				// Is there a string?
		Word uTest = reinterpret_cast<const Word8 *>(pList)[0];
		if (uTest) {

			char *pEnd = pInput;	// Init the end of string pointer
			do {
				++pInput;			// Point to where a zero would be if uTemp is acceptable
				// Iterate over the test list
				uTest = reinterpret_cast<const Word8 *>(pList)[0];
				const Word8 *pListTemp = reinterpret_cast<const Word8 *>(pList);
				do {
					if (uTemp==uTest) {		// In the list?
						goto SkipIt;		// Last VALID char
					}
					// Next in the list
					uTest = pListTemp[1];
					++pListTemp;
				} while (uTest);
				// Not in the list, mark the new location for terminator
				pEnd = pInput;	// Terminate AFTER this character (++pInput is above)
SkipIt:
				uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			} while (uTemp);		// All done?
			pEnd[0] = 0;			// Zap the final char
		}
	}
}

/*! ************************************

	\brief Remove characters from the beginning of a string

	Starting from the beginning of a string, see if the character is in the supplied
	list. If so, then the character is removed by copying the rest of the string up.
	This is repeated until there are no more characters from the list at the
	beginning of the string or the string is empty.

	\note This code is case sensitive.

	\param pInput Pointer to the "C" string to remove beginning characters from. Do not pass
		a \ref NULL pointer.
	\param pList Pointer to the "C" string that contains the characters to remove.
		Do not pass a \ref NULL pointer.

	\sa Burger::StripLeadingWhiteSpace(char *), Burger::StripTrailingSpaces(char *), Burger::StripLeading(char *,const char *)

***************************************/

void BURGER_API Burger::StripLeading(char* pInput,const char* pList)
{
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	if (uTemp && reinterpret_cast<const Word8 *>(pList)[0]) {	// Any input
		char *pTemp = pInput;			// Destination pointer
		++pInput;

		// First, iterate over the string until the first character
		// that's not in the list shows up.
		Word uTest = reinterpret_cast<const Word8 *>(pList)[0];
		const Word8 *pListTemp = reinterpret_cast<const Word8 *>(pList);
		do {
			if (uTest==uTemp) {		// Found one?
				// Step to the next character in the input string
				uTemp = reinterpret_cast<const Word8*>(pInput)[0];
				++pInput;
				if (!uTemp) {	// End of string?
					break;
				}
				pListTemp = reinterpret_cast<const Word8 *>(pList-1);		// Reset the list pointer
			}
			uTest = pListTemp[1];
			++pListTemp;
		} while (uTest);			// Any more?
		--pInput;
		if (pInput!=pTemp) {		// Did I remove anything?
			StringCopy(pTemp,pInput);
		}
	}
}

/*! ************************************

	\brief Remove a character from the end of a string if present.
	
	Parse to the end of the string. If the last character is a match to uRemove,
	it's zero'd out, effectively removing it. This function is a convenience routine
	used to get rid of a trailing ':' or '/' mark from a directory path.
	
	\param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
	\param uRemove character to test the last char with (Range 0-255).
	\sa Burger::ForceTrailingChar(char *,Word)
	
***************************************/

void BURGER_API Burger::RemoveTrailingChar(char *pInput,Word uRemove)
{
	WordPtr uLength = StringLength(pInput);		// Index to the last char
	if (uLength) {							// Should I bother?
		if (reinterpret_cast<Word8*>(pInput)[uLength-1] == uRemove) {
			pInput[uLength-1] = 0;
		}	
	}
}

/*! ************************************

	\brief Force a character to be at the end of a string if missing.
	
	Parse to the end of the string. If the last character is a match to uLast,
	then do nothing. Otherwise, append this single character to the end of the string
	and write a terminating zero after it. This function is a convenience routine
	used to ensure that a trailing ':' or '/' mark is at the end of a directory path.
	
	\note This function may grow the string by a single character. It is the
	caller's responsibility to ensure that the string buffer is large enough to
	accept a string that grows by 1 byte.
	
	\param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
	\param uLast 8 bit character to test the last char with.
	\sa Burger::RemoveTrailingChar(char *,Word)

***************************************/

void BURGER_API Burger::ForceTrailingChar(char *pInput,Word uLast)
{
	WordPtr uLength = StringLength(pInput);				// Index to the last char
	if (!uLength ||										// Always do it on an empty string
		(reinterpret_cast<Word8*>(pInput)[uLength-1] != uLast)) {
		pInput[uLength] = static_cast<char>(uLast);
		pInput[uLength+1] = 0;
	}
}

/*! ************************************

	\brief Convert all forward and back slashes to colons.
	
	Burgerlib uses colons as directory separators. This function will take
	unix ('\') and Windows ('/') style slashes and convert them into colons.
		
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\sa SlashesToColons(char *,const char *), SlashesToWindowsSlashes(char *) or SlashesToLinuxSlashes(char *)
	
***************************************/

void BURGER_API Burger::SlashesToColons(char *pInput)
{
	Word uTemp = reinterpret_cast<Word8*>(pInput)[0];		// Get the first char
	if (uTemp) {											// Do I bother?
		do {
			if ((uTemp=='\\') || (uTemp=='/')) {			// Is it a slash?
				pInput[0] = ':';							// It's a colon now
			}
			uTemp = reinterpret_cast<Word8*>(pInput)[1];	// Next char
			++pInput;										// Inc the pointer
		} while (uTemp);									// More?
	}
}

/*! ************************************

	\brief Convert all forward and back slashes to colons.
	
	Burgerlib uses colons as directory separators. This function will take
	unix ('\') and Windows ('/') style slashes and convert them into colons.
		
	\param pOutput Pointer to a buffer large enough to contain the new string
	\param pInput Pointer to the "C" string to perform the fix up. \ref NULL will page fault.
	\sa SlashesToColons(char *), SlashesToWindowsSlashes(char *,const char *) or SlashesToLinuxSlashes(char *,const char *)
	
***************************************/

void BURGER_API Burger::SlashesToColons(char *pOutput,const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];		// Get the first char
	if (uTemp) {											// Do I bother?
		do {
			if ((uTemp=='\\') || (uTemp=='/')) {			// Is it a slash?
				uTemp = ':';								// It's a colon now
			}
			pOutput[0] = static_cast<char>(uTemp);			// Store the new character
			++pOutput;
			uTemp = reinterpret_cast<const Word8*>(pInput)[1];	// Next char
			++pInput;										// Inc the pointer
		} while (uTemp);									// More?
	}
	pOutput[0] = static_cast<char>(uTemp);					// Store the zero terminator
}

/*! ************************************

	\brief Convert all forward slashes ('/') to back slashes ('\').
	
	Windows uses backslashes as directory separators. This function will take
	unix style slashes and convert them into Windows slashes.
		
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\sa SlashesToWindowsSlashes(char *,const char *), SlashesToColons(char *) or SlashesToLinuxSlashes(char *)

***************************************/

void BURGER_API Burger::SlashesToWindowsSlashes(char *pInput)
{
	Word uTemp = reinterpret_cast<Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp=='/') {	// Change the slash
				pInput[0] = '\\';
			}
			uTemp = reinterpret_cast<Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
}

/*! ************************************

	\brief Convert all forward slashes ('/') to back slashes ('\').
	
	Windows uses backslashes as directory separators. This function will take
	unix style slashes and convert them into Windows slashes.
		
	\param pOutput Pointer to a buffer large enough to hold the converted "C" string.
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\sa SlashesToWindowsSlashes(char *), SlashesToColons(char *,const char *) or SlashesToLinuxSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::SlashesToWindowsSlashes(char *pOutput,const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp=='/') {	// Change the slash
				uTemp = '\\';
			}
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
			uTemp = reinterpret_cast<const Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
	pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

	\brief Convert all back slashes ('\') to forward slashes ('/').
	
	Linux and MacOSX uses forward slashes as directory separators. This function will take
	Windows slashes and convert them into unix style slashes.
		
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\sa SlashesToLinuxSlashes(char *,const char *),SlashesToColons(char *) or SlashesToWindowsSlashes(char *)

***************************************/

void BURGER_API Burger::SlashesToLinuxSlashes(char *pInput)
{
	Word uTemp = reinterpret_cast<Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp=='\\') {	// Change the slash
				pInput[0] = '/';
			}
			uTemp = reinterpret_cast<Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
}

/*! ************************************

	\brief Convert all back slashes ('\') to forward slashes ('/').

	Linux and MacOSX uses forward slashes as directory separators. This function will take
	Windows slashes and convert them into unix style slashes.
		
	\param pOutput Pointer to a buffer large enough to hold the converted "C" string.
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\sa SlashesToLinuxSlashes(char *),SlashesToColons(char *,const char *) or SlashesToWindowsSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::SlashesToLinuxSlashes(char *pOutput,const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp=='\\') {	// Change the slash
				uTemp = '/';
			}
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
			uTemp = reinterpret_cast<const Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
	pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

	\brief Convert characters in a string from one to another
	
	Convert any character that matches the uFrom value
	into the uTo value.
		
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\param uFrom Value to compare with.
	\param uTo Value to replace matching values with.
	\sa Replace(char *,const char *,Word,Word)

***************************************/

void BURGER_API Burger::Replace(char *pInput,Word uFrom,Word uTo)
{
	Word uTemp = reinterpret_cast<Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp==uFrom) {	// Change the slash
				pInput[0] = static_cast<char>(uTo);
			}
			uTemp = reinterpret_cast<Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
}

/*! ************************************

	\brief Convert characters in a string from one to another
	
	Convert any character that matches the uFrom value
	into the uTo value.
		
	\param pOutput Pointer to a buffer large enough to hold the converted "C" string.
	\param pInput Pointer to the "C" string to perform the fix up on. \ref NULL will page fault.
	\param uFrom Value to compare with.
	\param uTo Value to replace matching values with.
	\sa Replace(char *,Word,Word)

***************************************/

void BURGER_API Burger::Replace(char *pOutput,const char *pInput,Word uFrom,Word uTo)
{
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	if (uTemp) {
		do {
			if (uTemp==uFrom) {	// Test the value
				uTemp = uTo;	// Value to change with
			}
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
			uTemp = reinterpret_cast<const Word8*>(pInput)[1];
			++pInput;
		} while (uTemp);		// End of string?
	}
	pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

	\brief Get a pointer to the beginning of the file extension.

	A string is scanned until the last period is found. A pointer to the string
	fragment following the period is returned. If no period is found then a pointer
	to the terminating zero is returned. This function is a quick way to extract the
	file extension from a pc style filename.
	
	Example filenames expected are: "Foo", "ReadMe.txt","ArtFile.gif" or "MyData.c".
	
	These examples will return pointers to: "", "txt", "gif" or "c".
	
	The pointer returned is a pointer within the string. You do not need to release the memory
	in any way.

	\param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
	\return A pointer to the filename extension of terminating zero.
	\sa Burger::SetFileExtension(char *,const char *).

***************************************/

char* BURGER_API Burger::GetFileExtension(const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	if (uTemp) {
		const char *pWork = pInput;		// No string found
		pInput = NULL;
		do {
			++pWork;
			if (uTemp=='.') {			// Hit a period?
				pInput = pWork;			// Save the place (+1 beyond the '.')
			}
			uTemp = reinterpret_cast<const Word8*>(pWork)[0];
		} while (uTemp);				// End of the string
		if (!pInput) {					// No '.' found?
			pInput = pWork;				// Point to the terminating zero
		}
	}
	return const_cast<char *>(pInput);	// Return end of string (Remove the const)
}

/*! ************************************

	\brief Replace the text after the last period for filename extensions.

	A string is scanned until the last period is found. The text beyond the final
	period is discarded and the string pointed by pNewExtension is appended to the Input
	filename. If no final period is found, then a period is appended and then
	the new extension is added. If NewExtension has a period as the first character,
	it will be ignored to prevent a double period from occurring in the final string.

	You must guarantee that the Input buffer has enough space to accommodate the new
	extension. This routine will not check for buffer overruns.
	
	Examples: "Foo.txt" + "gif" = "Foo.gif", "Foo" + "gif" = "Foo.gif", "Foo.txt" + \ref NULL = "Foo",
		"Foo.bin" + ".txt" = "Foo.txt".

	\param pInput Pointer to the "C" string to scan. \ref NULL will page fault. This will be modified with the new file extension.
		NewExtension = Pointer to a "C" string with the extension to apply.
	\param pNewExtension Pointer to a "C" string that represents the new extension. A \ref NULL or empty string will have the existing extension stripped.
	\sa Burger::GetFileExtension(const char *).

***************************************/

void BURGER_API Burger::SetFileExtension(char* pInput,const char* pNewExtension)
{
	char *pWork = GetFileExtension(pInput);	// Get pointer to the char after the '.'

	// If pWork[0] == 0, then no extension is present.

	if (reinterpret_cast<Word8*>(pWork)[0]) {	// End of string? (No extension)
		--pWork;		// Point to the period
		pWork[0] = 0;	// Truncate the string
	}
		
	// At this point, the file extension is stripped and WorkPtr points
	// to the zero at the end of the string.
		
	if (pNewExtension) {		// Don't add a new extension?
		Word uTemp = reinterpret_cast<const Word8*>(pNewExtension)[0];	// Is there a new extension?
		if (uTemp) {			// Valid string?
			if (uTemp!='.') {		// Does the new extension have a period?
				pWork[0] = '.';		// Put the period back manually
				++pWork;
			}
			StringCopy(pWork,pNewExtension);		// Overwrite the extension
		}
	}
}

/*! ************************************

	\brief Copy raw memory from one buffer to another.
	
	Given a pointer to a byte stream, copy it to
	a destination buffer using a supplied input length.
	
	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the raw data to copy from.
	\param uCount Size in bytes of the data to copy.
	
	\note This function will not check if the output buffer is
	not large enough for the input. Caution must be used with
	this function.
	
	\note This function uses large chuck data copies, so the input
	and output pointers must point to separate buffers. If they
	are overlapping buffers, the results are undefined.
	
	\sa MemoryMove(), MemoryFill() and MemoryClear()
	
***************************************/

#if defined(BURGER_POWERPC)

/***************************************

	PowerPC optimized version, Must have LOW overhead
	This routine has been VTuned. Don't touch it!!!!!
	Why do I do it this way? doubles on ppc 601's will copy fast
	under any circumstances but the 603 and 604 will cause
	a processor exception if the data is not longword aligned and incurs
	a 440 cycle penalty PER instruction. Pathetic.

***************************************/

void BURGER_API Burger::MemoryCopy(void *pOutput,const void *pInput,WordPtr uCount)
{
	if (reinterpret_cast<WordPtr>(pInput)&3) {		// Is the source pointer not long word aligned?
		goto AlignSource;							// Align the source
	}
SourceOk:
	// Is the dest aligned?
	if (!(reinterpret_cast<WordPtr>(pOutput)&3)) {
	// The pointers are double aligned!
		WordPtr uWordCount = uCount>>5;		// double copying (32 byte runs)
		if (uWordCount) {					// Any longwords?
			do {
				double DTemp = static_cast<const double *>(pInput)[0];
				double DTemp2 = static_cast<const double *>(pInput)[1];
				static_cast<double *>(pOutput)[0] = DTemp;
				static_cast<double *>(pOutput)[1] = DTemp2;
				DTemp = static_cast<const double *>(pInput)[2];
				DTemp2 = static_cast<const double *>(pInput)[3];
				static_cast<double *>(pOutput)[2] = DTemp;
				static_cast<double *>(pOutput)[3] = DTemp2;
				pOutput=static_cast<Word8 *>(pOutput)+32;
				pInput=static_cast<const Word8 *>(pInput)+32;
			} while (--uWordCount);
		}
		if (uCount&16) {			/* 16 byte run? */
			double DTemp = static_cast<const double *>(pInput)[0];
			double DTemp2 = static_cast<const double *>(pInput)[1];
			static_cast<double *>(pOutput)[0] = DTemp;
			static_cast<double *>(pOutput)[1] = DTemp2;
			pOutput=static_cast<Word8 *>(pOutput)+16;
			pInput=static_cast<const Word8 *>(pInput)+16;
		}
		if (uCount&8) {				/* 8 byte run? */
			static_cast<double *>(pOutput)[0] = static_cast<const double *>(pInput)[0];
			pOutput=static_cast<Word8 *>(pOutput)+8;
			pInput=static_cast<const Word8 *>(pInput)+8;
		}
		if (uCount&4) {				/* 4 byte run? */
			static_cast<Word32 *>(pOutput)[0] = static_cast<const Word32 *>(pInput)[0];
			pOutput=static_cast<Word8 *>(pOutput)+4;
			pInput=static_cast<const Word8 *>(pInput)+4;
		}
		if (uCount&2) {				/* 2 byte run? */
			static_cast<Word16 *>(pOutput)[0] = static_cast<const Word16 *>(pInput)[0];
			pOutput=static_cast<Word8 *>(pOutput)+2;
			pInput=static_cast<const Word8 *>(pInput)+2;
		}
		if (uCount&1) {				/* 1 byte left? */
			static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
		}
		return;			// Done!
	}
	// The pointers are NOT longword aligned!
	{
		// Longword copying
		WordPtr uWordCount = uCount>>4;
		if (uWordCount) {	// Any longwords?
			do {
				Word32 LTemp = static_cast<const Word32 *>(pInput)[0];
				Word32 LTemp2 = static_cast<const Word32 *>(pInput)[1];
				static_cast<Word32 *>(pOutput)[0] = LTemp;
				static_cast<Word32 *>(pOutput)[1] = LTemp2;
				LTemp = static_cast<const Word32 *>(pInput)[2];
				LTemp2 = static_cast<const Word32 *>(pInput)[3];
				static_cast<Word32 *>(pOutput)[2] = LTemp;
				static_cast<Word32 *>(pOutput)[3] = LTemp2;
				pOutput=static_cast<Word8 *>(pOutput)+16;
				pInput=static_cast<const Word8 *>(pInput)+16;
			} while (--uWordCount);
		}
	}
	if (uCount&8) {
		Word32 LTemp = static_cast<const Word32 *>(pInput)[0];
		Word32 LTemp2 = static_cast<const Word32 *>(pInput)[1];
		static_cast<Word32 *>(pOutput)[0] = LTemp;
		static_cast<Word32 *>(pOutput)[1] = LTemp2;
		pOutput=static_cast<Word8 *>(pOutput)+8;
		pInput=static_cast<const Word8 *>(pInput)+8;
	}
	if (uCount&4) {
		static_cast<Word32 *>(pOutput)[0] = static_cast<const Word32 *>(pInput)[0];
		pOutput=static_cast<Word8 *>(pOutput)+4;
		pInput=static_cast<const Word8 *>(pInput)+4;
	}
	if (uCount&2) {
		static_cast<Word16 *>(pOutput)[0] = static_cast<const Word16 *>(pInput)[0];
		pOutput=static_cast<Word8 *>(pOutput)+2;
		pInput=static_cast<const Word8 *>(pInput)+2;
	}
	if (uCount&1) {
		static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
	}
	return;

// Source is not longword aligned, align it

AlignSource:
	if ((reinterpret_cast<WordPtr>(pInput)&1) && uCount) {
		// Move a single byte to short align the data
		static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
		pInput=static_cast<const Word8 *>(pInput)+1;
		pOutput=static_cast<Word8 *>(pOutput)+1;
		--uCount;					// Remove from the count
	}
	if ((reinterpret_cast<WordPtr>(pInput)&2) && uCount>=2) {
		// Move a short to long align the data
		static_cast<Word16 *>(pOutput)[0] = static_cast<const Word16 *>(pInput)[0];
		pInput=static_cast<const Word8 *>(pInput)+2;
		pOutput=static_cast<Word8 *>(pOutput)+2;
		uCount-=2;					// Remove from the count
	}
	goto SourceOk;
}

#else

//
// Generic version assuming a 32 bit CPU
//

void BURGER_API Burger::MemoryCopy(void *pOutput,const void *pInput,WordPtr uCount)
{
	// Do anything?
	if (uCount && pOutput) {

		if ((reinterpret_cast<WordPtr>(pInput)^reinterpret_cast<WordPtr>(pOutput))&3) {
			// Data is not aligned, copy the slow way
			do {
				static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
				pInput=static_cast<const Word8 *>(pInput)+1;
				pOutput=static_cast<Word8 *>(pOutput)+1;
			} while (--uCount);
		} else {

			// Is the source pointer not long word aligned?
			if (reinterpret_cast<WordPtr>(pInput)&3) {
				// Align the source
				if (reinterpret_cast<WordPtr>(pInput)&1) {
					// Move a single byte to be short aligned
					static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
					pInput=static_cast<const Word8 *>(pInput)+1;
					pOutput=static_cast<Word8 *>(pOutput)+1;
					// Remove from the count
					--uCount;
				}
				if ((reinterpret_cast<WordPtr>(pInput)&2) && (uCount>=2)) {
					// Move a short to be 32 bit aligned
					static_cast<Word16 *>(pOutput)[0] = static_cast<const Word16 *>(pInput)[0];
					pInput=static_cast<const Word8 *>(pInput)+2;
					pOutput=static_cast<Word8 *>(pOutput)+2;
					// Remove from the count
					uCount-=2;
				}
			}

			// Longword copying
			WordPtr uWordCount = uCount>>4;
			// Any longwords? 
			if (uWordCount) {
				do {
					Word32 LTemp = static_cast<const Word32 *>(pInput)[0];
					Word32 LTemp2 = static_cast<const Word32 *>(pInput)[1];
					static_cast<Word32 *>(pOutput)[0] = LTemp;
					static_cast<Word32 *>(pOutput)[1] = LTemp2;
					LTemp = static_cast<const Word32 *>(pInput)[2];
					LTemp2 = static_cast<const Word32 *>(pInput)[3];
					static_cast<Word32 *>(pOutput)[2] = LTemp;
					static_cast<Word32 *>(pOutput)[3] = LTemp2;
					pOutput=static_cast<Word8 *>(pOutput)+16;
					pInput=static_cast<const Word8 *>(pInput)+16;
				} while (--uWordCount);
			}
			if (uCount&8) {
				Word32 LTemp = static_cast<const Word32 *>(pInput)[0];
				Word32 LTemp2 = static_cast<const Word32 *>(pInput)[1];
				static_cast<Word32 *>(pOutput)[0] = LTemp;
				static_cast<Word32 *>(pOutput)[1] = LTemp2;
				pOutput=static_cast<Word8 *>(pOutput)+8;
				pInput=static_cast<const Word8 *>(pInput)+8;
			}
			if (uCount&4) {
				static_cast<Word32 *>(pOutput)[0] = static_cast<const Word32 *>(pInput)[0];
				pOutput=static_cast<Word8 *>(pOutput)+4;
				pInput=static_cast<const Word8 *>(pInput)+4;
			}
			if (uCount&2) {
				static_cast<Word16 *>(pOutput)[0] = static_cast<const Word16 *>(pInput)[0];
				pOutput=static_cast<Word8 *>(pOutput)+2;
				pInput=static_cast<const Word8 *>(pInput)+2;
			}
			if (uCount&1) {
				static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
			}
		}
	}
}

#endif

/*! ************************************

	\brief Copy raw memory from one buffer to another in any direction.
	
	Given a pointer to a byte stream, copy it to
	a destination buffer using a supplied input length.
	This differs from Burger::MemoryCopy() in that it is safe
	to use this function for up or down memory copies. It
	will determine which direction to perform the copy in 
	a safe manner. As such, there is a small performance
	penalty for using this function instead of Burger::MemoryCopy().
	
	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the raw data to copy from.
	\param uCount Size in bytes of the data to copy.
	
	\sa MemoryCopy(), MemoryClear() and MemoryFill()
	
***************************************/

void BURGER_API Burger::MemoryMove(void *pOutput,const void *pInput,WordPtr uCount)
{
	if (pOutput<=pInput) {
		MemoryCopy(pOutput,pInput,uCount);
	} else if (uCount) {
		// Copy in reverse
		pInput = static_cast<const Word8 *>(pInput)+(uCount-1);
		pOutput = static_cast<Word8 *>(pOutput)+(uCount-1);
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<const Word8 *>(pInput)[0];
			pInput=static_cast<const Word8 *>(pInput)-1;
			pOutput=static_cast<Word8 *>(pOutput)-1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Set a buffer to all zeros
	
	Given a pointer to a byte array, erase it to all zeros
	
	\param pOutput Pointer to the buffer to erase.
	\param uCount Size in bytes of the buffer.
	
	\note This function is not recommended for non-cached or special
	memories. On some platforms, it will use special cache clear instructions
	to perform the operation quickly in which cache-disabled sections of 
	memory would generate a page fault.
	
	\sa MemoryFill() and MemoryCopy()
		
***************************************/

void BURGER_API Burger::MemoryClear(void *pOutput,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = 0;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Set a buffer to a specific value
	
	Given a pointer to a byte array, fill it with a specific value
	
	\param pOutput Pointer to the buffer to erase.
	\param uFill 8 bit value to fill memory with
	\param uCount Size in bytes of the buffer.
	
	\note This function is not recommended for non-cached or special
	memories. On some platforms, it will use special cache clear instructions
	to perform the operation quickly in which cache-disabled sections of 
	memory would generate a page fault.

	\sa MemoryClear() and MemoryCopy()
	
***************************************/

void BURGER_API Burger::MemoryFill(void *pOutput,Word8 uFill,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = uFill;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with 0xFF
	
	Given a pointer to a byte array, toggle all the zeros to ones and
	vice versa.
	
	\param pOutput Pointer to the buffer to toggle.
	\param uCount Size in bytes of the buffer.

	\sa MemoryNot(void *,const void *,WordPtr) and MemoryXor(void *,Word8,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryNot(void *pOutput,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(~static_cast<const Word8 *>(pOutput)[0]);
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Copy a buffer that's been exclusive or'd with 0xFF 
	
	Given a pointer to a byte array, toggle all the zeros to ones and
	vice versa and store the result into another buffer
	
	\param pOutput Pointer to the buffer to store the result.
	\param pInput Pointer to the buffer to toggle.
	\param uCount Size in bytes of the buffer.

	\sa MemoryNot(void *,WordPtr) and MemoryXor(void *,const void *,Word8,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryNot(void *pOutput,const void *pInput,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(~static_cast<const Word8 *>(pInput)[0]);
			pInput = static_cast<const Word8 *>(pInput)+1;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with a value
	
	Given a pointer to a byte array, exclusive or all of the values
	with a given 8 bit value.
	
	\param pOutput Pointer to the buffer to toggle.
	\param uXor Value to exclusive or the buffer with
	\param uCount Size in bytes of the buffer.

	\sa MemoryXor(void *,const void *,Word8,WordPtr) and MemoryNot(void *,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryXor(void *pOutput,Word8 uXor,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(static_cast<const Word8 *>(pOutput)[0]^uXor);
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with a value and store the result in another buffer
	
	Given a pointer to a byte array, exclusive or all of the values
	with a given 8 bit value and store the result elsewhere
	
	\param pOutput Pointer to the buffer to store the result.
	\param pInput Pointer to the buffer to toggle.
	\param uXor Value to exclusive or the buffer with
	\param uCount Size in bytes of the buffer.

	\sa MemoryXor(void *,Word8,WordPtr) and MemoryNot(void *,const void *,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryXor(void *pOutput,const void *pInput,Word8 uXor,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(static_cast<const Word8 *>(pInput)[0]^uXor);
			pInput = static_cast<const Word8 *>(pInput)+1;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with another buffer
	
	Given pointers to two byte arrays, exclusive or all of the values
	of each buffer with each other and store the result in the first buffer
	
	\param pOutput Pointer to the buffer to exclusive or and store the result.
	\param pInput Pointer to the buffer to exclusive or with.
	\param uCount Size in bytes of the buffers. Both buffers must be the same size.

	\sa MemoryXor(void *,const void *,const void *,WordPtr) and MemoryXor(void *,Word8,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryXor(void *pOutput,const void *pInput,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(static_cast<Word8 *>(pOutput)[0]^static_cast<const Word8 *>(pInput)[0]);
			pInput = static_cast<const Word8 *>(pInput)+1;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\brief Exclusive or a buffer with another buffer and store the result in a third buffer
	
	Given pointers to two byte arrays, exclusive or all of the values
	of each buffer with each other and store the result in a third buffer
	
	\param pOutput Pointer to the buffer to store the result.
	\param pInput1 Pointer to the first buffer to exclusive or with.
	\param pInput2 Pointer to the second buffer to exclusive or with.
	\param uCount Size in bytes of the buffers. All three buffers must be the same size.

	\sa MemoryXor(void *,const void *,WordPtr) and MemoryXor(void *,const void *,Word8,WordPtr)
	
***************************************/

void BURGER_API Burger::MemoryXor(void *pOutput,const void *pInput1,const void *pInput2,WordPtr uCount)
{
	if (uCount) {
		do {
			static_cast<Word8 *>(pOutput)[0] = static_cast<Word8>(static_cast<const Word8 *>(pInput1)[0]^static_cast<const Word8 *>(pInput2)[0]);
			pInput1 = static_cast<const Word8 *>(pInput1)+1;
			pInput2 = static_cast<const Word8 *>(pInput2)+1;
			pOutput = static_cast<Word8 *>(pOutput)+1;
		} while (--uCount);
	}
}

/*! ************************************

	\fn WordPtr BURGER_API Burger::StringLength(const char *pInput)
	\brief Perform an ANSI compatible strlen().

	Determine the length of a "C" string in memory. A "C" string is a random
	string of bytes that terminates with a zero.
	 
	This function works by first forcing 32-bit alignment and then grabbing four bytes at a time,
	and uses a pseudo vector operation to test all four of them at once.

	Here's how it works. For each byte, perform an operation that will clear
	the high bit if it is not in a suitable range. The two tests are as follows.
	
	#1) Negate the high bit. This eliminates 0x80-0xFF, because
	if the byte was 0xD5, then it would become 0x55 which has a clear high bit.
	(0xD5^0x80)&0x80 == 0. This test is performed by performing a "~" operation in "C".
	
	#2) Add 0xFF to the byte. This converts 0x01-0x80 into 0x00-0x7F. This range
	of values all have their high bits clear. This test is performed by adding the constant
	0xFEFEFEFF to the \ref Word32.
	
	Take the output of these two tests and and them together, then and with 0x80 to
	test the high bit. If it is set, it must be zero since zero is the only value
	that passes both tests.
	
	You may ask, why add with 0xFEFEFEFF, when I want to add 0xFF to each byte? Why wasn't it 0xFFFFFFFF?
	Because when I add 0xFF to the bottommost byte, it will carry a one to each upper byte. To compensate, I 
	add 0xFE instead of 0xFF so the carried one is accounted for.
	
	\param pInput Pointer to "C" string to determine the length
	\return Length in bytes of the "C" string sans the terminating zero.
	\sa Burger::StringCopy(char *,const char *)
	
***************************************/

#if ((defined(BURGER_WATCOM) || defined(BURGER_FASTCALLENABLED)) && defined(BURGER_X86)) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED WordPtr BURGER_API Burger::StringLength(const char * /* pInput */)
{
	BURGER_ASM {

#if defined(BURGER_WATCOM)	
	push edx
	push ecx
	push eax			// Save the original pointer
	mov ecx,eax			// Copy the pointer to ecx
#else
	push ecx			// Save the original pointer
	mov eax,ecx			// Copy the pointer to eax
#endif
	and ecx,3			// Already aligned?
	jnz short DoAlign	// Nope, preflight (Let's hope data's always aligned)
	
	// Here's the meat and potatoes
MainLoop:
	mov edx,[eax]				// Fetch 4 bytes
MainLoop2:
	add eax,4					// Inc my pointer
	lea ecx,[edx+0xFEFEFEFF]	// Do the vector addition for 0x01-0x80
	not edx						// Perform the xor test for 0x80-0xFF
	and ecx,edx					// Blend the two tests
	and ecx,0x80808080			// Test the high bits to find any zeros
	jz short MainLoop			// All bytes are non-zero, loop
	
	// Note to self, I really hate using the bsf instruction 
	// since it's 10+3n (386) or 6-42 (486) clocks, but the penalty for mispredicted
	// branches by testing and exiting is worse.

	// Addendum, P4 and Xeon made mispredicted branches even worse,
	// at least bsf sucks less now
	
	// BSF searches for the first set bit by testing
	// from the lowest bit to the highest bit in that order
	// BSF is undefined if ecx == 0, which won't happen in this code
	
	bsf	ecx,ecx			// ecx returns 7,15,23 or 31

	shr ecx,3			// Convert to 0-3
	sub	eax,4			// Undo the last +4 prefetch

#if defined(BURGER_WATCOM)
	mov edx,[esp+8]
	add eax,ecx			// Add 0-3 to get the pointer to the zero byte
	mov ecx,[esp+4]
	sub eax,[esp]		// Subtract from the original pointer to get the length
	add esp,12
#else
	sub eax,[esp]		// Subtract from the original pointer to get the length
	add	esp,4
	add eax,ecx			// Add 0-3 to get the pointer to the zero byte
#endif
	ret

DoAlign:				// Prealign
	
	shl	ecx,3			// 1,2,3 -> 8,16,24
	or edx,-1			// Create a bit mask

	shl edx,cl			// 0xFFFFFF00, 0xFFFF0000, 0xFF000000
	and	eax,0xFFFFFFFC	// Align to 32-bits

	not edx				// 0x000000FF, 0x0000FFFF, 0x00FFFFFF
	or edx,[eax]		// Fetch 4 bytes (Some are invalid)
	jmp short MainLoop2
	}
}

#else

//
// Generic "C" version that's endian neutral
// And it compiles to excellent ARM code
//

WordPtr BURGER_API Burger::StringLength(const char *pInput)
{
	Word32 uSample;		// Needed for the endian neutral version
	Word32 uAddTemp;	// Temp for the vector add test

	const char *pWork = pInput;

	// Get the address and see if it is already aligned
	WordPtr uCount = reinterpret_cast<WordPtr>(pInput)&3;
	if (uCount) {				// Nope, perform a phony first fetch
		pWork = pWork-uCount;	// Adjust the start pointer to align by 4 (Backwards)
		uCount <<= 3;			// 1,2,3 -> 8,16,24
		uSample = reinterpret_cast<const Word32 *>(pWork)[0];	// Get the first longword
		pWork+=4;
		// Make 0xFFFFFF00, 0xFFFF0000 or 0xFF000000 for 1,2,3 (little)
#if defined(BURGER_LITTLEENDIAN)
		uAddTemp = 0xFFFFFFFFU<<uCount;		// Create mask for unused bytes
#else
		uAddTemp = 0xFFFFFFFFU>>uCount;		// Handle reverse endian
#endif
		// Convert to 0x000000FF, 0x0000FFFF or 0x00FFFFFF for 1,2,3 (little endian)
		// For the input, ensure these bytes are non-zero
		uSample |= (~uAddTemp);
		
		// Let's get to work.
		uAddTemp = uSample+0xFEFEFEFFU;	// Do the vector addition for 0x01-0x80
		uAddTemp &= (~uSample);			// Perform the xor test for 0x80-0xFF
		if (uAddTemp&0x80808080U) {		// All bytes are non-zero, loop
			goto Skip1;					// If TRUE then a transition occurred
		}
	}

	// This is the main loop
	do {
		uSample = reinterpret_cast<const Word32 *>(pWork)[0];	// Fetch the longword
		pWork += 4;					// Accept the 4 bytes
		uAddTemp = uSample+0xFEFEFEFFU;	// Do the vector addition for 0x01-0x80
		uAddTemp &= (~uSample);			// Perform the xor test for 0x80-0xFF
	} while (!(uAddTemp&0x80808080U));	// All bytes are non-zero, loop
Skip1:;
	uCount = static_cast<WordPtr>(pWork-pInput);
#if defined(BURGER_LITTLEENDIAN)
	if (!(uSample&0xFF)) {				// Was the first byte the zero one?
		return uCount-4;				// Adjust result and exit
	}
	if (!(uSample&0xFF00)) {			// Word8 #2 zero?
		return uCount-3;
	}
	if (!(uSample&0xFF0000)) {			// Word8 #3 zero?
		return uCount-2;
	}
#else
	if (!(uSample&0xFF000000)) {
		return uCount-4;
	}
	if (!(uSample&0xFF0000)) {
		return uCount-3;
	}
	if (!(uSample&0xFF00)) {
		return uCount-2;
	}
#endif
	return uCount-1;		// I'll just assume byte #4 is zero
}
#endif

/*! ************************************

	\fn WordPtr BURGER_API Burger::StringLength(const Word16 *pInput)
	\brief Perform an ANSI compatible strlen() for UTF16 strings..

	Determine the length of a UTF16 "C" string in memory. A UTF16 "C" string is a random
	string of shorts that terminates with a zero.
	 	
	\note While the string L"Foobar" takes 14 bytes of memory to store, this function will
	return 6 to denote the number of values that are present. Due to UTF16 encoding, do not 
	assume that this value represents the number of visible characters since some encodings
	take 2 samples instead of one.

	\param pInput Pointer to a UTF16 "C" string to determine the length
	\return Length in values of the UTF16 "C" string sans the terminating zero.
	\sa Burger::StringLength(const char *)
	
***************************************/

WordPtr BURGER_API Burger::StringLength(const Word16 *pInput)
{
	WordPtr uResult = 0;		// Nothing found yet.
	if (pInput) {
		Word uTemp = pInput[0];
		++pInput;
		if (uTemp) {			// Is there any string yet?
			do {
				uTemp = pInput[0];
				++pInput;
				++uResult;		// Add to the count
			} while (uTemp);
		}
	}
	return uResult;				// Exit with the count
}

/*! ************************************

	\brief Copy a "C" string
	
	Given a pointer to a "C" string, copy it to
	a destination buffer. This is a functional equivalent to strcpy().
	
	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the "C" to copy from.

	\note This function does not check for buffer
	overruns on the destination buffer. Use with caution.
	
	\sa Burger::StringCopy(char *,WordPtr,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringCopy(char *pOutput,const char *pInput)
{
	//
	// First, if the two pointers can't both be long word aligned
	// I'm stuck doing a simple byte copy
	//
	
	if (!((reinterpret_cast<WordPtr>(pInput)^reinterpret_cast<WordPtr>(pOutput))&3)) {
		// Okay, they CAN be aligned. See if they are already.
		
		WordPtr uAlign = reinterpret_cast<WordPtr>(pInput)&3U;
		if (uAlign) {			// Not already aligned
			Word8 bChar1;
			uAlign = 4-uAlign;	// Number of bytes to parse to force alignment
			do {
				bChar1 = reinterpret_cast<const Word8 *>(pInput)[0];
				++pInput;
				pOutput[0] = static_cast<char>(bChar1);
				++pOutput;
				if (!bChar1) {		// End of string (Small string)
					return;			// Leave now
				}
			} while (--uAlign);		// Continue until the pointer is 32-bit aligned
		}

		// At this point, both input and output are 32-bit aligned
		// Let's hit the accelerator
	
		// Grab 32 bits of data	
		Word32 uTemp = reinterpret_cast<const Word32*>(pInput)[0];
		Word32 uTest = uTemp + 0xFEFEFEFFU;
		uTest &= ~uTemp;
		if (!(uTest&0x80808080U)) {
			do {
				pInput+=4;			// Only now, adjust the input pointer
				// Store the last 32-bit value that's known
				// to contain no zeros
				reinterpret_cast<Word32 *>(pOutput)[0] = uTemp;
				pOutput+=4;
				// Fetch the next 32 bit input
				uTemp = reinterpret_cast<const Word32 *>(pInput)[0];
				uTest = uTemp + 0xFEFEFEFFU;
				uTest &= ~uTemp;
			} while (!(uTest&0x80808080U));
		}
		// I found the zero byte. It's in the last
		// fetched Word32.
	}

	//
	// This is the byte copy version of StringCopy
	// Used for the slow path and to write the final byte
	// and ensuring I only write a BYTE. No buffer
	// overruns are possible by accidentally writing a Word32
	// when I mean to write a Word8
	//
	
	{
		Word8 bChar2;
		do {
			bChar2 = reinterpret_cast<const Word8 *>(pInput)[0];
			++pInput;
			pOutput[0] = static_cast<char>(bChar2);
			++pOutput;
		} while (bChar2);
	}
}

/*! ************************************

	\brief Copy a "C" string with bounds checking
	
	Given a pointer to a "C" string, copy it to
	a destination buffer. If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer to get the copy.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the "C" to copy from.
	
	\note This function will always zero terminate the output string and
	perform nothing at all if the input size is zero.
	
	\sa Burger::StringCopy(char *,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringCopy(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	if (uOutputSize) {
		WordPtr uFinalLength = StringLength(pInput)+1;
		if (uFinalLength>uOutputSize) {
			uFinalLength = uOutputSize-1;
			pOutput[uFinalLength] = 0;
		}
		MemoryCopy(pOutput,pInput,uFinalLength);
	}
}

/*! ************************************

	\brief Copy a text buffer into a "C" string with bounds checking
	
	Given a pointer to a buffer of text, copy it to
	a destination buffer and append it with a trailing zero. If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer to get the copy.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the text to copy from.
	\param uInputSize Size in byte of the text to copy from

	\note This function will always zero terminate the output string and
	perform nothing at all if the output size is zero.
	
	\sa Burger::StringCopy(char *,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringCopy(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	if (uOutputSize) {
		// How much can be copied?
		if (uInputSize>=uOutputSize) {
			uInputSize = uOutputSize-1;
		}
		// Store the ending zero
		pOutput[uInputSize] = 0;
		MemoryCopy(pOutput,pInput,uInputSize);
	}
}

/*! ************************************

	\brief Copy a 16 bit "C" string
	
	Given a pointer to a 16 bit "C" string, copy it to
	a destination buffer. This is a functional equivalent to wstrcpy().
	
	\param pOutput Pointer to the buffer to get the copy.
	\param pInput Pointer to the buffer with the 16 bit "C" to copy from.

	\note This function does not check for buffer
	overruns on the destination buffer. Use with caution.
	
	\sa Burger::StringCopy(Word16 *,WordPtr,const Word16 *) or Burger::StringLength(const Word16 *)

***************************************/

void BURGER_API Burger::StringCopy(Word16 *pOutput,const Word16 *pInput)
{
	Word16 bChar2;
	do {
		bChar2 = pInput[0];
		++pInput;
		pOutput[0] = bChar2;
		++pOutput;
	} while (bChar2);
}

/*! ************************************

	\brief Copy a 16 bit "C" string with bounds checking
	
	Given a pointer to a 16 bit "C" string, copy it to
	a destination buffer. If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer to get the copy.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the 16 bit "C" to copy from.
	
	\note This function will always zero terminate the output string and
	perform nothing at all if the input size is zero.
	
	\sa Burger::StringCopy(Word16 *,const Word16 *) or Burger::StringLength(const Word16 *)

***************************************/

void BURGER_API Burger::StringCopy(Word16 *pOutput,WordPtr uOutputSize,const Word16 *pInput)
{
	if (uOutputSize>=2 && pOutput) {
		WordPtr uFinalLength = (StringLength(pInput)*2)+2;
		if (uFinalLength>uOutputSize) {
			uFinalLength = uOutputSize-2;
			pOutput[uFinalLength/2] = 0;
		}
		MemoryCopy(pOutput,pInput,uFinalLength);
	}
}

/*! ************************************

	\brief Copy a text buffer into a 16 bit "C" string with bounds checking
	
	Given a pointer to a buffer of text, copy it to
	a destination buffer and append it with a trailing zero. If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer to get the copy.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the text to copy from.
	\param uInputSize Size in byte of the text to copy from

	\note This function will always zero terminate the output string and
	perform nothing at all if the output size is zero.
	
	\sa Burger::StringCopy(Word16 *,const Word16 *) or Burger::StringLength(const Word16 *)

***************************************/

void BURGER_API Burger::StringCopy(Word16 *pOutput,WordPtr uOutputSize,const Word16 *pInput,WordPtr uInputSize)
{
	if (uOutputSize) {
		// How much can be copied?
		if (uInputSize>=uOutputSize) {
			uInputSize = uOutputSize-2;
		}
		// Store the ending zero
		pOutput[uInputSize/2] = 0;
		MemoryCopy(pOutput,pInput,uInputSize);
	}
}

/*! ************************************

	\brief Make a copy of a "C" string.
	
	The "C" string passed by pInput will be copied into a buffer allocated
	by \ref Burger::Alloc(WordPtr). The buffer is exactly the same size of the string.
	You must eventually dispose of the string with a call to
	Burger::StringDelete(const char *).

	\param pInput Pointer to the "C" string to copy. A \ref NULL pointer will page fault.
	\return A pointer to the copy of the string. Or \ref NULL if a memory error occurs.
	\sa Burger::StringDuplicate(const char *,WordPtr) or Burger::StringDelete(const char *).
	
***************************************/

char * BURGER_API Burger::StringDuplicate(const char *pInput)
{
	WordPtr uLength = StringLength(pInput)+1;				// Get the length
	return static_cast<char *>(AllocCopy(pInput,uLength));	// Allocate the memory
}

/*! ************************************

	\brief Make a copy of a "C" string with some padding.

	The "C" string passed by pInput will be copied into a buffer allocated
	by Burger::Alloc(WordPtr). The buffer is the same size of the string plus the padding value.
	The extra memory is not initialized but the string does terminate with a zero.
	You must eventually dispose of the string with a call to
	Burger::StringDelete(const char *).

	\param pInput Pointer to the "C" string to copy. A null pointer will page fault.
	\param uPadding Number of bytes to extend the buffer.
	\return A pointer to the copy of the string. Or \ref NULL if a memory error occurred.
	\sa Burger::StringDuplicate(const char *) or Burger::StringDelete(const char *)

***************************************/

char * BURGER_API Burger::StringDuplicate(const char *pInput,WordPtr uPadding)
{
	WordPtr uLength = StringLength(pInput)+1;			// Get the length
	char *pResult = static_cast<char *>(Alloc(uLength+uPadding));		// Allocate the memory
	if (pResult) {
		MemoryCopy(pResult,pInput,uLength);		// Copy the string
	}
	return pResult;								// Return the string or NULL on error
}

/*! ************************************

	\brief Delete an allocated string.

	If a string was allocated with Burger::StringDuplicate(const char *) or
	Burger::StringDuplicate(const char *,WordPtr) then you must dispose of
	it with this function.

	\param pInput Pointer to the "C" string to delete. A \ref NULL pointer will do nothing and is okay to pass.
	\sa Burger::StringDuplicate(const char *) or Burger::StringDuplicate(const char *,WordPtr)

***************************************/

void BURGER_API Burger::StringDelete(const char *pInput)
{
	Free(pInput);
}

/*! ************************************

	\brief Concatenate a "C" string with another "C" string.

	Given a pointer to a "C" string, append it to
	a destination buffer that contains a valid "C" string.
	No bounds checking is performed.
	
	This is the equivalent to strcat()
	
	\param pOutput Pointer to the buffer of a valid "C" string to be appended.
	\param pInput Pointer to the buffer with the "C" to copy from.
		
	\sa Burger::StringConcatenate(char *,WordPtr,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(char *pOutput,const char *pInput)
{
	// Get the end of the first string
	WordPtr uLength = StringLength(pOutput);
	// Copy the string to the end of the first one
	StringCopy(pOutput+uLength,pInput);
}

/*! ************************************

	\brief Concatenate a "C" string with another "C" string bounds checked.

	Given a pointer to a "C" string, append it to
	a destination buffer that contains a valid "C" string.
	If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer of a valid "C" string to be appended.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the "C" to copy from.
		
	\sa Burger::StringConcatenate(char *,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(char *pOutput,WordPtr uOutputSize,const char *pInput)
{
	// Get the end of the first string
	WordPtr uLength = StringLength(pOutput);
	// Already out of bounds?
	if (uLength<uOutputSize) {
		// Adjust the output to all the data that won't be touched.
		uOutputSize-=uLength;
		// Copy the rest with bounds checking
		StringCopy(pOutput+uLength,uOutputSize,pInput);
	}
}

/*! ************************************

	\brief Concatenate a "C" string with a string buffer, bounds checked.

	Given a pointer to a string buffer, append it to
	a destination buffer that contains a valid "C" string.
	If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer of a valid "C" string to be appended.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the string to copy from.
	\param uInputSize Size in bytes of the input buffer
		
	\sa Burger::StringConcatenate(char *,const char *) or Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize)
{
	// Get the end of the first string
	WordPtr uLength = StringLength(pOutput);
	// Already out of bounds?
	if (uLength<uOutputSize) {
		// Adjust the output to all the data that won't be touched.
		uOutputSize-=uLength;
		// Copy the rest with bounds checking
		StringCopy(pOutput+uLength,uOutputSize,pInput,uInputSize);
	}
}

/*! ************************************

	\brief Concatenate a 16 bit "C" string with another 16 bit "C" string.

	Given a pointer to a 16 bit "C" string, append it to
	a destination buffer that contains a valid 16 bit "C" string.
	No bounds checking is performed.
	
	This is the equivalent to wstrcat()
	
	\param pOutput Pointer to the buffer of a valid 16 bit "C" string to be appended.
	\param pInput Pointer to the buffer with the 16 bit "C" to copy from.
		
	\sa Burger::StringConcatenate(Word16 *,WordPtr,const Word16 *) or Burger::StringLength(const Word16 *)

***************************************/

void BURGER_API Burger::StringConcatenate(Word16 *pOutput,const Word16 *pInput)
{
	// Get the end of the first string
	WordPtr uLength = StringLength(pOutput);
	// Copy the string to the end of the first one
	StringCopy(pOutput+uLength,pInput);
}

/*! ************************************

	\brief Concatenate a 16 bit "C" string with another 16 bit "C" string bounds checked.

	Given a pointer to a 16 bit "C" string, append it to
	a destination buffer that contains a valid 16 bit "C" string.
	If the destination buffer isn't
	big enough for the input string, truncate it.
	
	\param pOutput Pointer to the buffer of a valid 16 bit "C" string to be appended.
	\param uOutputSize Size in bytes of the output buffer
	\param pInput Pointer to the buffer with the 16 bit "C" to copy from.
		
	\sa Burger::StringConcatenate(Word16 *,const Word16 *) or Burger::StringLength(const Word16 *)

***************************************/

void BURGER_API Burger::StringConcatenate(Word16 *pOutput,WordPtr uOutputSize,const Word16 *pInput)
{
	// Get the end of the first string
	WordPtr uLength = StringLength(pOutput);
	// Already out of bounds?
	if ((uLength*2)<uOutputSize) {
		// Adjust the output to all the data that won't be touched.
		uOutputSize-=(uLength*2);
		// Copy the rest with bounds checking
		StringCopy(pOutput+uLength,uOutputSize,pInput);
	}
}

/*! ************************************

	\brief Compare two "C" strings for equality
	
	Given pointers to two "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero.
	
	\param pInput1 Pointer to the first "C" string to subtract from.
	\param pInput2 Pointer to the second "C" string to subtract with.
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2
	
	\sa Burger::StringCompare(const char *,const char *,WordPtr)
	
***************************************/

int BURGER_API Burger::StringCompare(const char *pInput1,const char *pInput2)
{
	int iTemp;
	Word uTemp1;
	do {
		uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];		// First byte
		++pInput1;
		Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];	// Second byte
		++pInput2;
		iTemp = static_cast<int>(uTemp1-uTemp2);			// Compare
		if (iTemp) {
			break;
		}			
	} while (uTemp1);		// End of string? (And match!!)
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Compare two "C" strings for equality, case sensitive, length delimited
	
	Given pointers to two "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero.
	
	The function determines equality by hitting a zero first or the maximum allowed length.
	
	\param pInput1 Pointer to the first "C" string to subtract from.
	\param pInput2 Pointer to the second "C" string to subtract with.
	\param uMaxLength Maximum number of bytes to check
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

	\sa Burger::StringCompare(const char *,const char *)

***************************************/

int BURGER_API Burger::StringCompare(const char *pInput1,const char *pInput2,WordPtr uMaxLength)
{
	int iTemp=0;
	Word uTemp1;
	if (uMaxLength) {
		do {
			uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];		// First byte
			++pInput1;
			Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];	// Second byte
			++pInput2;
			iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
			if (iTemp) {
				break;
			}			
		} while (--uMaxLength && uTemp1);				// End of string? (And match!!)
	}
	// Return the match!
	return iTemp;
}

/*! ************************************

	\brief Compare two 16 bit "C" strings for equality
	
	Given pointers to two 16 bit "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero.
	
	\param pInput1 Pointer to the first 16 bit "C" string to subtract from.
	\param pInput2 Pointer to the second 16 bit "C" string to subtract with.
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2
	
	\sa Burger::StringCompare(const Word16 *,const Word16 *,WordPtr)
	
***************************************/

int BURGER_API Burger::StringCompare(const Word16 *pInput1,const Word16 *pInput2)
{
	int iTemp;
	Word uTemp1;
	do {
		uTemp1 = pInput1[0];		// First byte
		++pInput1;
		Word uTemp2 = pInput2[0];	// Second byte
		++pInput2;
		iTemp = static_cast<int>(uTemp1-uTemp2);			// Compare
		if (iTemp) {
			break;
		}			
	} while (uTemp1);		// End of string? (And match!!)
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Compare two 16 bit "C" strings for equality, case sensitive, length delimited
	
	Given pointers to two 16 bit "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero.
	
	The function determines equality by hitting a zero first or the maximum allowed length.
	
	\param pInput1 Pointer to the first 16 bit "C" string to subtract from.
	\param pInput2 Pointer to the second 16 bit "C" string to subtract with.
	\param uMaxLength Maximum number of bytes to check
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

	\sa Burger::StringCompare(const Word16 *,const Word16 *)

***************************************/

int BURGER_API Burger::StringCompare(const Word16 *pInput1,const Word16 *pInput2,WordPtr uMaxLength)
{
	int iTemp=0;
	Word uTemp1;
	uMaxLength<<=1;
	if (uMaxLength) {
		do {
			uTemp1 = pInput1[0];		// First byte
			++pInput1;
			Word uTemp2 = pInput2[0];	// Second byte
			++pInput2;
			iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
			if (iTemp) {
				break;
			}			
		} while (--uMaxLength && uTemp1);				// End of string? (And match!!)
	}
	// Return the match!
	return iTemp;
}

/*! ************************************

	\brief Compare two "C" strings for equality, case insensitive
	
	Given pointers to two "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero. If the characters in the string
	are upper case, convert them to ASCII lowercase before the subtraction.
	
	\param pInput1 Pointer to the first "C" string to subtract from.
	\param pInput2 Pointer to the second "C" string to subtract with.
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2
	\sa Burger::StringCompare(const char *,const char *), Burger::StringCaseCompare(const char *,const char *,WordPtr)
	
***************************************/

int BURGER_API Burger::StringCaseCompare(const char *pInput1,const char *pInput2)
{
	int iTemp;
	Word uTemp1;
	do {
		uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];		// First byte
		++pInput1;
		Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];	// Second byte
		++pInput2;
		if ((uTemp1-'A')<26) {		// Convert to lower case
			uTemp1 = uTemp1+32;
		}
		if ((uTemp2-'A')<26) {
			uTemp2 = uTemp2+32;
		}
		iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
		if (iTemp) {
			break;
		}			
	} while (uTemp1);		// End of string? (And match!!)
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Compare two "C" strings for equality, case insensitive, length delimited
	
	Given pointers to two "C" strings, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero. If the characters in the string
	are upper case, convert them to ASCII lowercase before the subtraction.
	
	The function determines equality by hitting a zero first or the maximum allowed length.
	
	\param pInput1 Pointer to the first "C" string to subtract from.
	\param pInput2 Pointer to the second "C" string to subtract with.
	\param uMaxLength Maximum number of bytes to check
	\return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

***************************************/

int BURGER_API Burger::StringCaseCompare(const char *pInput1,const char *pInput2,WordPtr uMaxLength)
{
	int iTemp=0;
	Word uTemp1;
	if (uMaxLength) {
		do {
			uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];		// First byte
			++pInput1;
			Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];	// Second byte
			++pInput2;
			if ((uTemp1-'A')<26) {		// Convert to lower case
				uTemp1 = uTemp1+32;
			}
			if ((uTemp2-'A')<26) {
				uTemp2 = uTemp2+32;
			}
			iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
			if (iTemp) {
				break;
			}			
		} while (--uMaxLength && uTemp1);		// End of string? (And match!!)
	}
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Compare two byte buffers for equality
	
	Given pointers to two unsiged byte arrays, compare them for equalty by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero.
	
	This is equivalent to memcmp()
	
	\param pInput1 Pointer to the first buffer to subtract from.
	\param pInput2 Pointer to the second buffer to subtract with.
	\param uCount Size in bytes of both of the buffers.
	
	\sa MemoryCaseCompare(const void *,const void *,WordPtr)
	
***************************************/

int BURGER_API Burger::MemoryCompare(const void *pInput1,const void *pInput2,WordPtr uCount)
{
	int iTemp=0;
	if (uCount) {
		do {
			Word uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];
			pInput1 = reinterpret_cast<const Word8 *>(pInput1)+1;
			Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];
			pInput2 = reinterpret_cast<const Word8 *>(pInput2)+1;
			iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
			if (iTemp) {
				break;
			}			
		} while (--uCount);		// End of string? (And match!!)
	}
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Compare two byte buffers for equality, case insensitive
	
	Given pointers to two unsigned byte arrays, compare them for equality by using
	a subtraction of the first buffer to the second. If the subtraction's
	result is non-zero, sign extend the result and return it immediately.
	If both buffers are identical, return zero. If the byte is an upper case ASCII
	character, convert it to lower case before the subtraction is performed
	
	\param pInput1 Pointer to the first buffer to subtract from.
	\param pInput2 Pointer to the second buffer to subtract with.
	\param uCount Size in bytes of both of the buffers.
	
	\sa MemoryCompare(const void *,const void *,WordPtr)
	
***************************************/

int BURGER_API Burger::MemoryCaseCompare(const void *pInput1,const void *pInput2,WordPtr uCount)
{
	int iTemp=0;
	if (uCount) {
		do {
			Word uTemp1 = reinterpret_cast<const Word8 *>(pInput1)[0];
			pInput1 = reinterpret_cast<const Word8 *>(pInput1)+1;
			Word uTemp2 = reinterpret_cast<const Word8 *>(pInput2)[0];
			pInput2 = reinterpret_cast<const Word8 *>(pInput2)+1;
			if ((uTemp1-'A')<26) {		// Convert to lower case
				uTemp1 = uTemp1+32;
			}
			if ((uTemp2-'A')<26) {
				uTemp2 = uTemp2+32;
			}
			iTemp = static_cast<int>(uTemp1-uTemp2);	// Compare
			if (iTemp) {
				break;
			}			
		} while (--uCount);		// End of string? (And match!!)
	}
	return iTemp;			// Perfect match!
}

/*! ************************************

	\brief Perform a string comparison using the wild card system.

	An asterisk "*" denotes that the input is skipped until the character following
	is found, then the compare continues. If a "*" is the final character,
	then the input string is considered a match.

	A question mark "?" denotes that a single, non-zero character must occupy this location.

	Examples for wild cards:
	*.* = All strings that have a single period anywhere in them.
	* = All strings match.
	*.??? = All strings that have a exactly three characters at the end after a period.

	\note Comparisons are case insensitive.
	\param pInput Pointer to a "C" string to compare against. \ref NULL will page fault.
	\param pWildcard Pointer to a "C" string that has wild card information. \ref NULL will page fault.
	\return \ref FALSE if the string matches according to the wild card rules, \ref TRUE if not.
	\sa HasWildcard(const char *)

***************************************/

Word BURGER_API Burger::Wildcardcmp(const char *pInput,const char *pWildcard)
{
	Word uTemp;
	Word uResult = TRUE;
	do {
		// Main entry, assume no previous state
		// Grab an input char
		uTemp = ISOLatin1::UpperCaseTable[reinterpret_cast<const Word8*>(pInput)[0]];	
		// Get a wildcard char
		Word uWild = ISOLatin1::UpperCaseTable[reinterpret_cast<const Word8*>(pWildcard)[0]];	

		// Is the wildcard a "?"
		// If so, then I'm expecting a single, non-zero character

		if (uWild=='?') {		// Single wildcard
			// If uTemp is zero, exit with TRUE
			++pInput;
			++pWildcard;
			continue;
		}

		// The magic "*" wildcard character?

		if (uWild=='*') {
			++pWildcard;
			uWild = reinterpret_cast<const Word8*>(pWildcard)[0];
			if (!uWild) {			// Was the '*' at the end of the wildcard string?
				uResult = FALSE;
				break;				// Assume it's a full match.
			}
			// If there is string remaining?
			if (uTemp) {
				// Recursively call myself until a match of the pattern is found or
				// I give up
				uWild = ISOLatin1::UpperCaseTable[uWild];		// Upper case
				do {
					if (uTemp==uWild) {			// Follow the input until the marker is found
						uResult = Wildcardcmp(pInput,pWildcard);
						if (!uResult) {
							return FALSE;
						}
					}
					++pInput;
					uTemp = ISOLatin1::UpperCaseTable[reinterpret_cast<const Word8*>(pInput)[0]];	
				} while (uTemp);
			}
		} else {
			// Simple character test
			++pInput;
			++pWildcard;
			if (uTemp!=uWild) {			// No match?
				break;					// Exit with TRUE
			}
			// Match!
			if (!uTemp) {
				uResult = FALSE;		// W00t!
			}
		}
	} while (uTemp);				// Keep going?
	return uResult;					// Return the answer
}

/*! ************************************

	\brief Return \ref TRUE if the string contains a wild card character

	To determine if a string contains a '*' or a '?' wild card character,
	call this function and it will return \ref TRUE if so.

	\param pInput Pointer to a "C" string to test. \ref NULL will return \ref FALSE
	\return \ref TRUE if the string contains wild card characters, \ref FALSE if not.
	\sa Wildcardcmp(const char *,const char *)

***************************************/

Word BURGER_API Burger::HasWildcard(const char *pInput)
{
	// Assume not a wild card
	Word uResult = FALSE;
	if (pInput) {
		Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		if (uTemp) {
			++pInput;
			do {
				// Wild card character
				if ((uTemp=='?') || (uTemp=='*')) {
					uResult = TRUE;
					break;
				}
				uTemp = reinterpret_cast<const Word8*>(pInput)[0];
				++pInput;
			} while (uTemp);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Convert a string to upper case
	
	Convert all 'a'-'z' characters to 'A'-'Z'. No localization
	is performed and no other characters are altered.
	
	\param pInput Pointer to "C" string to modify.
	\sa Burger::StringUppercase(char *,const char *) and Burger::StringLowercase(char *)

***************************************/

void BURGER_API Burger::StringUppercase(char *pInput)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Prefetch first char
	if (uTemp) {										// End of string?
		do {
			if ((uTemp-'a')<26) {						// Is it 'a' to 'z' inclusive?
				uTemp = uTemp-32;
				pInput[0] = static_cast<char>(uTemp);	// Convert to upper case
			}
			uTemp = reinterpret_cast<Word8 *>(pInput)[1];	// Get the next char
			++pInput;
		} while (uTemp);								// Still more?
	}
}

/*! ************************************

	\brief Convert a string to upper case into a new buffer
	
	Copy a string and convert all 'a'-'z' characters to 'A'-'Z'. No localization
	is performed and no other characters are altered.
	
	\param pOutput Pointer to a buffer large enough to hold the input "C" string.
	\param pInput Pointer to "C" string to modify.
	\sa Burger::StringUppercase(char *) and Burger::StringLowercase(char *,const char *)

***************************************/

void BURGER_API Burger::StringUppercase(char *pOutput,const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Prefetch first char
	if (uTemp) {									// End of string?
		do {
			if ((uTemp-'a')<26) {					// Is it 'a' to 'z' inclusive?
				uTemp = uTemp-32;					// Convert to upper case
			}
			pOutput[0] = static_cast<char>(uTemp);	// Store output
			++pOutput;
			uTemp = reinterpret_cast<const Word8 *>(pInput)[1];	// Get the next char
			++pInput;
		} while (uTemp);							// Still more?
	}
	pOutput[0] = static_cast<char>(uTemp);			// Store zero terminator
}


/*! ************************************

	\brief Convert a string to lower case
	
	Convert all 'A'-'Z' characters to 'a'-'z'. No localization
	is performed and no other characters are altered.
	
	\param pInput Pointer to "C" string to modify.
	\sa Burger::StringLowercase(char *,const char *) and Burger::StringUppercase(char *)

***************************************/

void BURGER_API Burger::StringLowercase(char *pInput)
{
	Word uTemp = reinterpret_cast<Word8 *>(pInput)[0];	// Prefetch first char
	if (uTemp) {										// End of string?
		do {
			if ((uTemp-'A')<26) {
				uTemp = uTemp+32;
				pInput[0] = static_cast<char>(uTemp);	// Convert to lower case
			}
			uTemp = reinterpret_cast<Word8 *>(pInput)[1];	// Get the next char
			++pInput;
		} while (uTemp);		// Still more?
	}
}

/*! ************************************

	\brief Convert a string to lower case into a new buffer
	
	Copy a string and convert all 'A'-'Z' characters to 'a'-'z'. No localization
	is performed and no other characters are altered.
	
	\param pOutput Pointer to a buffer large enough to hold the input "C" string.
	\param pInput Pointer to "C" string to modify.
	\sa Burger::StringLowercase(char *) and Burger::StringUppercase(char *,const char *)

***************************************/

void BURGER_API Burger::StringLowercase(char *pOutput,const char *pInput)
{
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];	// Prefetch first char
	if (uTemp) {									// End of string?
		do {
			if ((uTemp-'A')<26) {					// Is it 'A' to 'Z' inclusive?
				uTemp = uTemp+32;					// Convert to lower case
			}
			pOutput[0] = static_cast<char>(uTemp);	// Store output
			++pOutput;
			uTemp = reinterpret_cast<const Word8 *>(pInput)[1];	// Get the next char
			++pInput;
		} while (uTemp);							// Still more?
	}
	pOutput[0] = static_cast<char>(uTemp);			// Store zero terminator
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character

	Returns a pointer to the first occurrence of the
	character iChar in the string pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to a "C" string to scan
	\param iChar Character to scan for
	\return Pointer inside the input string to the first matching character or \ref NULL if not found
	\sa Burger::StringCharacter(const Word16 *,Word) and Burger::StringCharacterReverse(const char *,int)

***************************************/

char * BURGER_API Burger::StringCharacter(const char *pInput,int iChar)
{
	// Valid pointer?
	if (pInput) {
		// Get the first character
		Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		// Valid?
		if (uTemp) {
			do {
			// Perform the test
				if (static_cast<Word>(iChar)==uTemp) {
					// Found the first one!
					return const_cast<char *>(pInput);
				}
				// Get the next character
				uTemp = reinterpret_cast<const Word8 *>(pInput)[1];
				// Increment afterward to avoid Address Generation Interlock
				++pInput;
				// Continue?
			} while (uTemp);
		}
	}
	// Not found
	return NULL;
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character

	Returns a pointer to the first occurrence of the
	character uChar in the string pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to a "C" string to scan
	\param uChar Character to scan for
	\return Pointer inside the input string to the first matching character or \ref NULL if not found
	\sa Burger::StringCharacter(const char *,int) and Burger::StringCharacterReverse(const Word16 *,Word)

***************************************/

Word16 * BURGER_API Burger::StringCharacter(const Word16 *pInput,Word uChar)
{
	// Valid pointer?
	if (pInput) {
		// Get the first character
		Word uTemp = pInput[0];
		// Valid?
		if (uTemp) {
			do {
				// Perform the test
				if (uChar==uTemp) {
					// Found the first one!
					return const_cast<Word16 *>(pInput);
				}
				// Get the next character
				uTemp = pInput[1];
				// Increment afterward to avoid Address Generation Interlock
				++pInput;
				// Continue?
			} while (uTemp);
		}
	}
	// Not found
	return NULL;
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character in a byte array

	Returns a pointer to the first occurrence of the
	character iChar in the 8 bit pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to an array of bytes to scan
	\param uInputSize Size of the buffer in bytes
	\param iChar Character to scan for
	\return Pointer inside the input string to the first matching character or \ref NULL if not found
	\sa Burger::MemoryCharacter(const Word16 *,WordPtr,Word)

***************************************/

char * BURGER_API Burger::MemoryCharacter(const char *pInput,WordPtr uInputSize,int iChar)
{
	// Valid pointer?
	if (pInput && uInputSize) {
		// Valid?
		do {
			// Get the first character
			Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			// Perform the test
			if (static_cast<Word>(iChar)==uTemp) {
			// Found the first one!
				return const_cast<char *>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Not found
	return NULL;
}

/*! ************************************

	\brief Scan for the first occurrence of a specific character in a short array

	Returns a pointer to the first occurrence of the
	character uChar in the 16 bit pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to an array of 16 bit shorts to scan
	\param uInputSize Size of the buffer in bytes
	\param uChar Character to scan for
	\return Pointer inside the input string to the first matching character or \ref NULL if not found
	\sa Burger::MemoryCharacter(const char *,WordPtr,int)

***************************************/

Word16 * BURGER_API Burger::MemoryCharacter(const Word16 *pInput,WordPtr uInputSize,Word uChar)
{
	// Valid pointer?
	uInputSize>>=1U;
	if (pInput && uInputSize) {
		do {
			// Get the first character
			Word uTemp = pInput[0];
			// Perform the test
			if (uChar==uTemp) {
			// Found the first one!
				return const_cast<Word16 *>(pInput);
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

	\brief Scan for the last occurrence of a specific character

	Returns a pointer to the last occurrence of the
	character iChar in the string pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to a "C" string to scan
	\param iChar Character to scan for
	\return Pointer inside the input string to the last matching character or \ref NULL if not found
	\sa Burger::StringCharacterReverse(const Word16 *,Word), Burger::MemoryCharacterReverse(const char *,WordPtr,int) and Burger::StringCharacter(const char *,int)

***************************************/

char * BURGER_API Burger::StringCharacterReverse(const char *pInput,int iChar)
{
	// Assume it's not found
	char *pFound = NULL;
	// Valid pointer?
	if (pInput) {
		// Get the first character
		Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		// Valid?
		if (uTemp) {
			do {
				// Perform the test
				if (static_cast<Word>(iChar)==uTemp) {
					// Found another one!
					pFound = const_cast<char *>(pInput);
				}
				// Get the next character
				uTemp = reinterpret_cast<const Word8 *>(pInput)[1];
				// Increment afterward to avoid Address Generation Interlock
				++pInput;
				// Continue?
			} while (uTemp);
		}
	}
	// Return the result
	return pFound;
}

/*! ************************************

	\brief Scan for the last occurrence of a specific character

	Returns a pointer to the last occurrence of the
	character uChar in the string pInput or \ref NULL if
	the character was not found

	\param pInput Pointer to a "C" string to scan
	\param uChar Character to scan for
	\return Pointer inside the input string to the last matching character or \ref NULL if not found
	\sa Burger::StringCharacterReverse(const char *,int), Burger::MemoryCharacterReverse(const Word16 *,WordPtr,Word) and Burger::StringCharacter(const Word16 *,Word)

***************************************/

Word16 * BURGER_API Burger::StringCharacterReverse(const Word16 *pInput,Word uChar)
{
	// Assume it's not found
	Word16 *pFound = NULL;
	// Valid pointer?
	if (pInput) {
		// Get the first character
		Word uTemp = pInput[0];
		// Valid?
		if (uTemp) {
			do {
				// Perform the test
				if (uChar==uTemp) {
					// Found another one!
					pFound = const_cast<Word16 *>(pInput);
				}
				// Get the next character
				uTemp = pInput[1];
				// Increment afterward to avoid Address Generation Interlock
				++pInput;
				// Continue?
			} while (uTemp);
		}
	}
	// Return the result
	return pFound;
}

/*! ************************************

	\brief Scan for the last occurrence of a specific character in a buffer

	Returns a pointer to the last occurrence of the
	character iChar in the memory buffer pInput or \ref NULL if
	the character was not found

	\note This function can be used to scan for zeros in the data array.

	\param pInput Pointer to a char buffer string to scan
	\param uInputSize Number of bytes contained in the buffer
	\param iChar Character to scan for
	\return Pointer inside the input string to the last matching character or \ref NULL if not found
	\sa Burger::MemoryCharacterReverse(const Word16 *,WordPtr,Word) and Burger::StringCharacterReverse(const char *,int)

***************************************/

char * BURGER_API Burger::MemoryCharacterReverse(const char *pInput,WordPtr uInputSize,int iChar)
{
	// Assume it's not found
	char *pFound = NULL;
	// Valid pointer?
	if (pInput && uInputSize) {
		do {
			// Get the next character
			Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			// Perform the test
			if (static_cast<Word>(iChar)==uTemp) {
				// Found another one!
				pFound = const_cast<char *>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Return the result
	return pFound;
}

/*! ************************************

	\brief Scan for the last occurrence of a specific character in a Word16 buffer

	Returns a pointer to the last occurrence of the
	character uChar in the Word16 string buffer pInput or \ref NULL if
	the character was not found

	\note This function can be used to scan for zeros in the data array.

	\param pInput Pointer to a Word16 string buffer to scan
	\param uInputSize Number of BYTES in the buffer
	\param uChar Character to scan for
	\return Pointer inside the input string to the last matching character or \ref NULL if not found
	\sa Burger::MemoryCharacterReverse(const char *,WordPtr,int) and Burger::StringCharacterReverse(const Word16 *,Word)

***************************************/

Word16 * BURGER_API Burger::MemoryCharacterReverse(const Word16 *pInput,WordPtr uInputSize,Word uChar)
{
	// Assume it's not found
	Word16 *pFound = NULL;
	uInputSize<<=1;			// Convert to Word16 sizes
	// Valid pointer?
	if (pInput && uInputSize) {
		do {
			// Get the next character
			Word uTemp = pInput[0];
			// Perform the test
			if (uChar==uTemp) {
				// Found another one!
				pFound = const_cast<Word16 *>(pInput);
			}
			++pInput;
			// Continue?
		} while (--uInputSize);
	}
	// Return the result
	return pFound;
}

/*! ************************************

	\brief Locate a substring

	Returns a pointer to the first occurrence of pTest in pInput,
	or a \ref NULL pointer if pTest is not part of pInput.

	The matching process does not include the terminating null-characters,
	but it stops there.

	\param pInput Pointer to the string to scan
	\param pTest Pointer to the substring to look for
	\return \ref NULL if substring was not found or the pointer to the first matching string

	\sa Burger::StringCaseString(const char *,const char *)

***************************************/

char *BURGER_API Burger::StringString(const char *pInput,const char *pTest)
{
	// Get the first character
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	if (uTemp) {						// Do I even bother?
		do {
			WordPtr i = 0;					// Init the index
			Word uTemp2;
			do {
				// Get the first char of the test string
				uTemp2 = reinterpret_cast<const Word8 *>(pTest)[i];
				if (!uTemp2) {					// Match?
					return const_cast<char *>(pInput);		// I matched here!
				}
				// Get the source string
				uTemp = reinterpret_cast<const Word8 *>(pInput)[i];
				++i;							// Ack the char
			} while (uTemp == uTemp2);			// Match?
			++pInput;							// Next main string char
			// Next entry
			uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			// Source string still ok?
		} while (uTemp);
	}
	// No string match
	return NULL;
}

/*! ************************************

	\brief Locate a substring (UTF-16 version)

	Returns a pointer to the first occurrence of pTest in pInput,
	or a \ref NULL pointer if pTest is not part of pInput.

	The matching process does not include the terminating null-characters,
	but it stops there.

	\param pInput Pointer to the string to scan
	\param pTest Pointer to the substring to look for
	\return \ref NULL if substring was not found or the pointer to the first matching string

	\sa Burger::StringCaseString(const Word16 *,const Word16 *)

***************************************/

Word16 *BURGER_API Burger::StringString(const Word16 *pInput,const Word16 *pTest)
{
	// Get the first character
	Word uTemp = pInput[0];
	if (uTemp) {						// Do I even bother?
		do {
			WordPtr i = 0;					// Init the index
			Word uTemp2;
			do {
				// Get the first char of the test string
				uTemp2 = pTest[i];
				if (!uTemp2) {					// Match?
					return const_cast<Word16 *>(pInput);		// I matched here!
				}
				// Get the source string
				uTemp = pInput[i];
				++i;							// Ack the char
			} while (uTemp == uTemp2);			// Match?
			++pInput;							// Next main string char
			// Next entry
			uTemp = pInput[0];
			// Source string still ok?
		} while (uTemp);
	}
	// No string match
	return NULL;
}


/*! ************************************

	\brief Locate a substring, case insensitive

	Returns a pointer to the first occurrence of pTest in pInput,
	or a \ref NULL pointer if pTest is not part of pInput.

	The case insensitive matching process does not include the terminating null-characters,
	but it stops there.

	\param pInput Pointer to the string to scan
	\param pTest Pointer to the substring to look for
	\return \ref NULL if substring was not found or the pointer to the first matching string

	\sa Burger::StringString(const char *,const char *)

***************************************/

char *BURGER_API Burger::StringCaseString(const char *pInput,const char *pTest)
{
	// Get the first character
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	if (uTemp) {						// Do I even bother?
		do {
			WordPtr i = 0;					// Init the index
			Word uTemp2;
			do {
				// Get the first char of the test string
				uTemp2 = reinterpret_cast<const Word8 *>(pTest)[i];
				if (!uTemp2) {					// Match?
					return const_cast<char *>(pInput);		// I matched here!
				}
				// Get the source string
				uTemp = reinterpret_cast<const Word8 *>(pInput)[i];
				++i;							// Ack the char
				if (uTemp2>='A' && uTemp2<='Z') {	// Convert to lower case
					uTemp2 += 32;
				}
				if (uTemp>='A' && uTemp<='Z') {	// Convert to lower case
					uTemp += 32;
				}
			} while (uTemp == uTemp2);			// Match?
			++pInput;							// Next main string char
			// Next entry
			uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			// Source string still ok?
		} while (uTemp);
	}
	// No string match
	return NULL;
}

/*! ************************************

	\brief Locate a substring, case insensitive (UTF-16 version)

	Returns a pointer to the first occurrence of pTest in pInput,
	or a \ref NULL pointer if pTest is not part of pInput.

	The case insensitive matching process does not include the terminating null-characters,
	but it stops there.

	\param pInput Pointer to the string to scan
	\param pTest Pointer to the substring to look for
	\return \ref NULL if substring was not found or the pointer to the first matching string

	\sa Burger::StringString(const Word16 *,const Word16 *)

***************************************/

Word16 *BURGER_API Burger::StringCaseString(const Word16 *pInput,const Word16 *pTest)
{
	// Get the first character
	Word uTemp = pInput[0];
	if (uTemp) {						// Do I even bother?
		do {
			WordPtr i = 0;					// Init the index
			Word uTemp2;
			do {
				// Get the first char of the test string
				uTemp2 = pTest[i];
				if (!uTemp2) {					// Match?
					return const_cast<Word16 *>(pInput);		// I matched here!
				}
				// Get the source string
				uTemp = pInput[i];
				++i;							// Ack the char
				if (uTemp2>='A' && uTemp2<='Z') {	// Convert to lower case
					uTemp2 += 32;
				}
				if (uTemp>='A' && uTemp<='Z') {	// Convert to lower case
					uTemp += 32;
				}
			} while (uTemp == uTemp2);			// Match?
			++pInput;							// Next main string char
			// Next entry
			uTemp = pInput[0];
			// Source string still ok?
		} while (uTemp);
	}
	// No string match
	return NULL;
}

/*! ************************************

	\brief Convert an 8 bit value into a hex string

	Store two hex digits in the form of 00 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 3 bytes in size.
	\param uInput 8 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word32), NumberToAsciiHex(char *,Word64),
		NumberToAsciiHex(char *,float), NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word8 uInput)
{
	// Convert the string and store in the buffer
	pOutput[0] = g_NibbleToAsciiUppercase[(uInput>>4U)&0xFU];
	pOutput[1] = g_NibbleToAsciiUppercase[uInput&0xFU];
	pOutput[2] = 0;
	// Return the pointer to the terminating zero
	return pOutput+2;
}

/*! ************************************

	\brief Convert a 16 bit value into a hex string

	Store four hex digits in the form of 0000 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 5 bytes in size.
	\param uInput 16 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word32), NumberToAsciiHex(char *,Word64),
		NumberToAsciiHex(char *,float), NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word16 uInput)
{
	const void *pInput = &uInput;
	// Push the pointer to the highest value byte
	char *pEnd = &pOutput[sizeof(Word16)*2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8 *>(pInput)+(sizeof(Word16)-1));
#endif
	do {
		Word uTemp = static_cast<const Word8 *>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8 *>(pInput)-1);
#else
		pInput = (static_cast<const Word8 *>(pInput)+1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp>>4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp&0xFU];
		pOutput+=2;
	} while (pOutput<pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 32 bit value into a hex string

	Store eight hex digits in the form of 00000000 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 9 bytes in size.
	\param uInput 32 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word64),
		NumberToAsciiHex(char *,float), NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word32 uInput)
{
	const void *pInput = &uInput;
	// Push the pointer to the highest value byte
	char *pEnd = &pOutput[sizeof(Word32)*2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8 *>(pInput)+(sizeof(Word32)-1));
#endif
	do {
		Word uTemp = static_cast<const Word8 *>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8 *>(pInput)-1);
#else
		pInput = (static_cast<const Word8 *>(pInput)+1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp>>4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp&0xFU];
		pOutput+=2;
	} while (pOutput<pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 64 bit value into a hex string

	Store sixteen hex digits in the form of 0000000000000000 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param uInput 64 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word32),
		NumberToAsciiHex(char *,float), NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word64 uInput)
{
	const void *pInput = &uInput;
	// Push the pointer to the highest value byte
	char *pEnd = &pOutput[sizeof(Word64)*2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8 *>(pInput)+(sizeof(Word64)-1));
#endif
	do {
		Word uTemp = static_cast<const Word8 *>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8 *>(pInput)-1);
#else
		pInput = (static_cast<const Word8 *>(pInput)+1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp>>4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp&0xFU];
		pOutput+=2;
	} while (pOutput<pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 32 bit floating point value into a hex string

	Store eight hex digits in the form of 00000000 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 9 bytes in size.
	\param fInput 32 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word32),
		NumberToAsciiHex(char *,Word64), NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,float fInput)
{
	const void *pInput = &fInput;
	// Push the pointer to the highest value byte
	char *pEnd = &pOutput[sizeof(float)*2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8 *>(pInput)+(sizeof(float)-1));
#endif
	do {
		Word uTemp = static_cast<const Word8 *>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8 *>(pInput)-1);
#else
		pInput = (static_cast<const Word8 *>(pInput)+1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp>>4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp&0xFU];
		pOutput+=2;
	} while (pOutput<pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 64 bit floating point value into a hex string

	Store sixteen hex digits in the form of 0000000000000000 to a string buffer
	and append a terminating zero. Returns the pointer
	to the terminating zero so the calling function
	can continue writing to the output buffer
	as if it's a stream.

	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param dInput 64 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word32),
		NumberToAsciiHex(char *,Word64), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,Word32,Word), or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char * BURGER_API Burger::NumberToAsciiHex(char *pOutput,double dInput)
{
	const void *pInput = &dInput;
	// Push the pointer to the highest value byte
	char *pEnd = &pOutput[sizeof(double)*2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8 *>(pInput)+(sizeof(double)-1));
#endif
	do {
		Word uTemp = static_cast<const Word8 *>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8 *>(pInput)-1);
#else
		pInput = (static_cast<const Word8 *>(pInput)+1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp>>4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp&0xFU];
		pOutput+=2;
	} while (pOutput<pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 32 bit integer into hexadecimal ASCII.
	
	Convert an unsigned 32 bit integer into a hexadecimal ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 0x1234BCD, uDigits = 4 -> "4BCD"<br>
	uInput = 0x1234BCD, uDigits = 0 -> "1234BCD"<br>
	uInput = 0x1234BCD, uDigits = Burger::LEADINGZEROS -> "01234BCD"<br>
	uInput = 0x2A, uDigits = Burger::LEADINGZEROS+4 -> "002A"
	
	\note uDigits is capped at 8 digits. It will not output more than 8 digits.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 9 bytes in size Whichever is appropriate.
	\param uInput Unsigned 32 bit integer to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa NumberToAsciiHex(char *pOutput,Word64 uInput,Word uDigits), 
		NumberToAscii(char *,Word32,Word) or NumberToAscii(char *,Int32,Word)

***************************************/

char *BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word32 uInput,Word uDigits)
{
	Word uIndex = uDigits&0xFFU;// Get the count of digits to process
	if ((uIndex-1U)>=7U) {		// Failsafe
		uIndex = 8;				// Maximum of 8 digits
	} else {					// Won't show certain digits?
		uInput <<= ((8-uIndex)<<2);	// Shift 4,8, -> 28
	}
	if (--uIndex) {
		do {
			Word uLetter = uInput>>static_cast<Word>((8-1)*4);		// Get the highest 4 bits
			if (uLetter || (uDigits&LEADINGZEROS)) {	// Already printing?
				uDigits |= LEADINGZEROS;				// Force future printing
				pOutput[0] = g_NibbleToAsciiUppercase[uLetter];	// Print the char
				++pOutput;
			}
			uInput<<=4;			// Shift to next digit
		} while (--uIndex);		// Any more left?
	}
	pOutput[0] = g_NibbleToAsciiUppercase[uInput>>static_cast<Word>((8-1)*4)];	// Also must print on last char
	++pOutput;
	if (!(uDigits&NOENDINGNULL)) {
		pOutput[0] = 0;		// Terminate the string
	}
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 64 bit integer into hexadecimal ASCII.
	
	Convert an unsigned 64 bit integer into a hexadecimal ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 0x1234BCD, uDigits = 4 -> "4BCD"<br>
	uInput = 0x1234BCD, uDigits = 0 -> "1234BCD"<br>
	uInput = 0x1234BCD, uDigits = Burger::LEADINGZEROS -> "01234BCD"<br>
	uInput = 0x2A, uDigits = Burger::LEADINGZEROS+4 -> "002A"
	
	\note uDigits is capped at 16 digits. It will not output more than 16 digits.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 17 bytes in size Whichever is appropriate.
	\param uInput Unsigned 64 bit integer to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa NumberToAsciiHex(char *pOutput,Word32 uInput,Word uDigits), 
		NumberToAscii(char *,Word32,Word) or NumberToAscii(char *,Int32,Word)

***************************************/

char *BURGER_API Burger::NumberToAsciiHex(char *pOutput,Word64 uInput,Word uDigits)
{
	Word uIndex = uDigits&0xFFU;// Get the count of digits to process
	if ((uIndex-1U)>=15U) {		// Failsafe
		uIndex = 16;			// Maximum of 16 digits
	} else {					// Won't show certain digits?
		uInput <<= ((16-uIndex)<<2);	// Shift 4,8, -> 56
	}
	if (--uIndex) {
		do {
			Word uLetter = static_cast<Word>(uInput>>static_cast<Word>((16-1)*4));		// Get the highest 4 bits
			if (uLetter || (uDigits&LEADINGZEROS)) {	// Already printing?
				uDigits |= LEADINGZEROS;				// Force future printing
				pOutput[0] = g_NibbleToAsciiUppercase[uLetter];	// Print the char
				++pOutput;
			}
			uInput<<=4;			// Shift to next digit
		} while (--uIndex);		// Any more left?
	}
	pOutput[0] = g_NibbleToAsciiUppercase[uInput>>static_cast<Word>((16-1)*4)];	// Also must print on last char
	++pOutput;
	if (!(uDigits&NOENDINGNULL)) {
		pOutput[0] = 0;		// Terminate the string
	}
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 32 bit integer into ASCII.
	
	Convert an unsigned 32 bit integer into an ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"
	
	\note uDigits is capped at 10 digits. It will not output more than 10 digits.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 12 bytes in size Whichever is appropriate.
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa Burger::NumberToAsciiHex(char *,Word32,Word) or Burger::NumberToAscii(char *,Int32,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,Word32 uInput,Word uDigits)
{
	Word uIndex = uDigits&0xFFU;	// How many digits to process?
	if ((uIndex-1U)>=9U) {			// Out of bounds? Or default? (Allow 1-10 only)
		uIndex = 10;		 		// 10 digits to process
	} else {						// If smaller than the max, hack off the
									// excess with a modulo
		uInput = uInput%g_TensTable32[uIndex];
	}
	
	// Let's iterate (Index = 1-10)
	if (--uIndex) {			// Only iterate on >=10
		do {
			Word32 uBigNum = g_TensTable32[uIndex];	// Get div value in local
			Word uLetter = '0';				// Init ASCII value
			if (uInput>=uBigNum) {			// Bigger than the min?
				uDigits |= LEADINGZEROS;	// Force future printing
				do {
					uInput-=uBigNum;		// Remove value
					++uLetter;				// Inc ASCII value (Max 9 iterations)
				} while (uInput>=uBigNum);	// Keep going?
			}
			
			// Should I print leading zeros?
			
			if (uDigits&LEADINGZEROS) {	// Already printing?
				pOutput[0] = static_cast<char>(uLetter);	// Output the digit
				++pOutput;
			}
		} while (--uIndex);		/* Any more left? */
	}

	// Handle the last digit (No divide needed)
	// and it's ALWAYS output.
	
	pOutput[0] = static_cast<char>(uInput+'0');
	++pOutput;
	
	// Do I need to write a null?
	
	if (!(uDigits&NOENDINGNULL)) {
		pOutput[0] = 0;		// Terminate the string
	}
	return pOutput;			// Return the end pointer
}

/*! ************************************

	\brief Convert a signed 32 bit integer into ASCII.
	
	Convert a signed 32 bit integer into an ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = -1234567, uDigits = 4 -> "-4567"<br>
	uInput = -1234567, uDigits = 0 -> "-1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"<br>
	uInput = -22, uDigits = Burger::LEADINGZEROS+4 -> "-0022"
	
	\note uDigits is capped at 10 digits. It will not output more than 10 digits.
	The negative sign does NOT count as part of the numeric string length.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 12 bytes in size Whichever is appropriate.
	\param iInput Signed 32 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa Burger::NumberToAscii(char *,Word32,Word) or Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,Int32 iInput,Word uDigits)
{
	if (iInput<0) {			// Negative?
		iInput = -iInput;	// Convert to positive
		pOutput[0] = '-';	// Output the '-' sign.
		++pOutput;
	}
	return NumberToAscii(pOutput,static_cast<Word32>(iInput),uDigits);
}

/*! ************************************

	\brief Convert an unsigned 64 bit integer into ASCII.
	
	Convert an unsigned 64 bit integer into an ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"
	
	\note uDigits is capped at 20 digits. It will not output more than 20 digits.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 24 bytes in size Whichever is appropriate.
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa Burger::NumberToAsciiHex(char *,Word64,Word) or Burger::NumberToAscii(char *,Int64,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,Word64 uInput,Word uDigits)
{
	Word uIndex = uDigits&0xFFU;	// How many digits to process?
	if ((uIndex-1U)>=19U) {			// Out of bounds? Or default? (Allow 1-10 only)
		uIndex = 20;		 		// 10 digits to process
	} else {						// If smaller than the max, hack off the
									// excess with a modulo
		uInput = uInput%g_TensTable64[uIndex];
	}
	
	// Let's iterate (Index = 1-20)
	if (--uIndex) {			// Only iterate on >=20
		do {
			Word64 uBigNum = g_TensTable64[uIndex];	// Get div value in local
			Word uLetter = '0';				// Init ASCII value
			if (uInput>=uBigNum) {			// Bigger than the min?
				uDigits |= LEADINGZEROS;	// Force future printing
				do {
					uInput-=uBigNum;		// Remove value
					++uLetter;				// Inc ASCII value (Max 9 iterations)
				} while (uInput>=uBigNum);	// Keep going?
			}
			
			// Should I print leading zeros?
			
			if (uDigits&LEADINGZEROS) {	// Already printing?
				pOutput[0] = static_cast<char>(uLetter);	// Output the digit
				++pOutput;
			}
		} while (--uIndex);		/* Any more left? */
	}

	// Handle the last digit (No divide needed)
	// and it's ALWAYS output.
	
	pOutput[0] = static_cast<char>(uInput+'0');
	++pOutput;
	
	// Do I need to write a null?
	
	if (!(uDigits&NOENDINGNULL)) {
		pOutput[0] = 0;		// Terminate the string
	}
	return pOutput;			// Return the end pointer
}

/*! ************************************

	\brief Convert a signed 64 bit integer into ASCII.
	
	Convert a signed 64 bit integer into an ASCII string.
	You can modify how the data is formatted by passing a third parameter.
	
	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero character
	at the end of the string so you can modify the an existing string without accidentally
	truncating it.
	
	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = -1234567, uDigits = 4 -> "-4567"<br>
	uInput = -1234567, uDigits = 0 -> "-1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"<br>
	uInput = -22, uDigits = Burger::LEADINGZEROS+4 -> "-0022"
	
	\note uDigits is capped at 20 digits. It will not output more than 20 digits.
	The negative sign does NOT count as part of the numeric string length.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 24 bytes in size Whichever is appropriate.
	\param iInput Signed 64 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect output.
	
	\return Pointer to the char immediately after the last char output. Most cases,
		this is a pointer to the terminating zero.
	\sa Burger::NumberToAscii(char *,Word64,Word) or Burger::NumberToAsciiHex(char *,Word64,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,Int64 iInput,Word uDigits)
{
	if (iInput<0) {			// Negative?
		iInput = -iInput;	// Convert to positive
		pOutput[0] = '-';	// Output the '-' sign.
		++pOutput;
	}
	return NumberToAscii(pOutput,static_cast<Word64>(iInput),uDigits);
}

/*! ************************************

	\brief Convert a 32 bit float into ASCII.
	
	Convert a 32 bit float into an ASCII string.
	
	Examples:<br>
	fInput = 123.4567 -> "123.4567"<br>
	fInput = 123456.7 -> "123456.7"<br>
	fInput = -1.234567 -> "-1.234567"<br>
	fInput = -1234.567 -> "-1234.567"<br>

	\note The string is capped at 30 digits. It will not output more than 30 digits.
	The negative sign does NOT count as part of the numeric string length.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be a minimum of 32 bytes in size.
	\param fInput 32 bit float to convert to ASCII.
	
	\return Pointer to the char immediately after the last char output.
	\sa Burger::NumberToAscii(char *,Word32,Word) or Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,float fInput)
{
#if defined(BURGER_XBOX360) || (defined(BURGER_WINDOWS) && defined(_MSC_VER))
	int iLength = sprintf_s(pOutput,32,"%f",static_cast<double>(fInput));
#else
	int iLength = sprintf(pOutput,"%f",static_cast<double>(fInput));
#endif
	if (iLength<0) {
		iLength = 0;
	}
	return pOutput+iLength;
}

/*! ************************************

	\brief Convert a 64 bit float into ASCII.
	
	Convert a 64 bit float into an ASCII string.
	
	Examples:<br>
	dInput = 123.4567 -> "123.4567"<br>
	dInput = 123456.7 -> "123456.7"<br>
	dInput = -1.234567 -> "-1.234567"<br>
	dInput = -1234.567 -> "-1234.567"<br>

	\note The string is capped at 30 digits. It will not output more than 30 digits.
	The negative sign does NOT count as part of the numeric string length.
	
	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be a minimum of 32 bytes in size.
	\param dInput 64 bit float to convert to ASCII.
	
	\return Pointer to the char immediately after the last char output.
	\sa Burger::NumberToAscii(char *,Word32,Word) or Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

char *BURGER_API Burger::NumberToAscii(char *pOutput,double dInput)
{
#if defined(BURGER_XBOX360) || (defined(BURGER_WINDOWS) && defined(_MSC_VER))
	int iLength = sprintf_s(pOutput,32,"%f",dInput);
#else
	int iLength = sprintf(pOutput,"%f",dInput);
#endif
	if (iLength<0) {
		iLength = 0;
	}
	return pOutput+iLength;
}

/*! ************************************

	\brief Convert an ASCII string into an integer.
	
	Take a string formatted as a simple integer number, 
	a hex number of the form $5123 or 0x1234 with or
	without a preceding '-' sign and convert it into
	a 32 bit integer value.
	
	Overflow is detected and is considered an error condition. 0xFFFFFFFFU
	will be returned in this case.
	
	\param pInput Pointer to the string to convert. NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput (Error)
		of pInput moved ahead until a non-numeric character was found. This can
		be NULL if the application doesn't require the location of the string after
		the integer was parsed.
		
	\return A 32 bit integer that represents the ASCII string. Will be 0xFFFFFFFF if
		an overflow occurred and 0 if nothing could be parsed.
	
	\sa Burger::AsciiToFloat(const char *,const char **) or Burger::AsciiToDouble(const char *,const char **) 

***************************************/

Word32 BURGER_API Burger::AsciiToInteger(const char *pInput,const char **pDest)
{
	if (pDest) {
		pDest[0] = pInput;	// Assume I don't accept any input (Error condition)
	}
	
	Word uLetter;			// Temp ASCII char

	// Parse away whitespace and the '-' flag.
	
	Word uNegate = 0;		// Don't negate the result (Assume positive)
	do {
		uLetter = reinterpret_cast<const Word8*>(pInput)[0];			// Get char
		++pInput;
		if (uLetter == '-') {	// Negate it?
			uNegate ^= static_cast<Word>(-1);	// Toggle it, so - - works. (Foo^-1)-(-1) = -Foo
			uLetter = ' ';			// Force staying in the loop
		}
	} while ((uLetter == ' ') || (uLetter == '\t') || (uLetter == '+'));		// Eat white space

	if (uLetter!='$') {			// Hex input?
		if ((uLetter=='0') &&
			!((reinterpret_cast<const Word8 *>(pInput)[0]^'X')&(~0x20U))) {		// 0X or 0x for "C" style input
			++pInput;	// Skip the x and go to the hex parser
		} else {

	// Here's the base 10 code. Common case
	
			Word32 uValue10 = g_AsciiToWord8Table[uLetter];
			if (uValue10<10) {		// First char valid?
				Word uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];
				uLetter = g_AsciiToWord8Table[uAscii10];
				if (uLetter<10) {	// Second char valid?
					do {
						++pInput;
						// Check for overflow
						if ((uValue10>0x19999999) ||		// 0xFFFFFFFFU/10 = 0x19999999
							((uValue10==0x19999999) && (uLetter>=6))) {
							return 0xFFFFFFFFU;			// Tilt!
						}
						uValue10 = (uValue10*10)+uLetter;		// Process into the total
					// Convert char to value
						uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];
						uLetter = g_AsciiToWord8Table[uAscii10];
					} while (uLetter<10);
				}
				
				// Okay, I've got a valid value
				
				uValue10 = (uValue10^uNegate)-uNegate;		// Perform negation?

				// Does the caller want the end address?
				
				if (pDest) {
					// Skip past trailing white space
					
					if (uAscii10==' ' || uAscii10=='\t') {
						do {
							++pInput;
							uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];	// Remove whitespace
						} while ((uAscii10==' ') || (uAscii10=='\t'));
					}
					pDest[0] = pInput;		// Store the ASCII address
				}
				return uValue10;			// Return the result
			}
			return 0;		// No input, error
		}
	}

	// Base 16
	
	
	{
		Word32 uValue16 = g_AsciiToWord8Table[reinterpret_cast<const Word8 *>(pInput)[0]];
		if (uValue16<16) {
			++pInput;
			Word uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];
			uLetter = g_AsciiToWord8Table[uAscii16];
			if (uLetter<16) {		// Second char valid?
				do {
					++pInput;
					if (uValue16>=0x10000000) {
						return 0xFFFFFFFFU;		// Tilt
					}
					uValue16 = (uValue16<<4)+uLetter;
					// Convert char to value
					uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];
					uLetter = g_AsciiToWord8Table[uAscii16];
				} while (uLetter<16);
			}
			
			// Perform negation?
			uValue16 = (uValue16^uNegate)-uNegate;

			// Does the caller want the end address?
				
			if (pDest) {
				// Skip past trailing white space
					
				if (uAscii16==' ' || uAscii16=='\t') {
					do {
						++pInput;
						uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];	// Remove whitespace
					} while ((uAscii16==' ') || (uAscii16=='\t'));
				}
				pDest[0] = pInput;		// Store the ASCII address
			}
			return uValue16;			// Return the result
		}
	}
	return 0;				// Bad value
}

/*! ************************************

	\brief Return a signed integer value

	Scan the value string as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa AsciiToInteger(const char *,const char **) or AsciiToWord(const char *,Word,Word,Word)

***************************************/

Int BURGER_API Burger::AsciiToInteger(const char *pInput,Int iDefault,Int iMin,Int iMax)
{
	if (pInput) {
		const char *pDest;
		// Parse as signed
		Int iValue = static_cast<Int>(AsciiToInteger(pInput,&pDest));
		if (pDest!=pInput) {
			// Do a signed bounds check
			if (iValue<iMin) {
				iDefault = iMin;
			} else if (iValue>iMax) {
				iDefault = iMax;
			} else {
				iDefault = iValue;
			}
		}
	}
	return iDefault;
}

/*! ************************************

	\brief Convert a 32 bit integer and signal if successful

	Scan the value string as a 32 bit integer or
	hex value and if successful, return \ref TRUE.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was not a number
	\sa AsciiToInteger(const char *,const char **) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

Word BURGER_API Burger::AsciiToInteger(Word32 *pOutput,const char *pInput)
{
	const char *pDest;
	// Convert the text
	Word32 uOutput = AsciiToInteger(pInput,&pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest==pInput) {
		uResult = FALSE;
		uOutput = 0;	// Clear the output
	}
	pOutput[0] = uOutput;
	return uResult;		// Return the result
}

/*! ************************************

	\brief Return an unsigned integer value

	Scan the value string as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa AsciiToInteger(const char *,const char **) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

Word BURGER_API Burger::AsciiToWord(const char *pInput,Word uDefault,Word uMin,Word uMax)
{
	if (pInput) {
		// Convert from UTF-8
		const char *pDest;
		Word32 uValue = AsciiToInteger(pInput,&pDest);
		// Anything parsed?
		if (pDest!=pInput) {
			// Bounds test
			if (uValue<uMin) {
				uDefault = uMin;
			} else if (uValue>uMax) {
				uDefault = uMax;
			} else {
				uDefault = uValue;
			}
		}
	}
	return uDefault;
}

/*! ************************************

	\brief Convert an ASCII string into a 64 bit integer.
	
	Take a string formatted as a simple integer number, 
	a hex number of the form $5123 or 0x1234 with or
	without a preceding '-' sign and convert it into
	a 64 bit integer value.
	
	Overflow is detected and is considered an error condition. 0xFFFFFFFFFFFFFFFFU
	will be returned in this case.
	
	\param pInput Pointer to the string to convert. NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput (Error)
		of pInput moved ahead until a non-numeric character was found. This can
		be NULL if the application doesn't require the location of the string after
		the integer was parsed.
		
	\return A 64 bit integer that represents the ASCII string. Will be 0xFFFFFFFF if
		an overflow occurred and 0 if nothing could be parsed.
	
	\sa Burger::AsciiToFloat(const char *,const char **) or Burger::AsciiToDouble(const char *,const char **) 

***************************************/

Word64 BURGER_API Burger::AsciiToInteger64(const char *pInput,const char **pDest)
{
	if (pDest) {
		pDest[0] = pInput;	// Assume I don't accept any input (Error condition)
	}
	
	Word uLetter;			// Temp ASCII char

	// Parse away whitespace and the '-' flag.
	
	Word64 uNegate = 0;		// Don't negate the result (Assume positive)
	do {
		uLetter = reinterpret_cast<const Word8*>(pInput)[0];			// Get char
		++pInput;
		if (uLetter == '-') {	// Negate it?
			uNegate ^= BURGER_MAXUINT64;	// Toggle it, so - - works. (Foo^-1)-(-1) = -Foo
			uLetter = ' ';			// Force staying in the loop
		}
	} while ((uLetter == ' ') || (uLetter == '\t') || (uLetter == '+'));		// Eat white space

	if (uLetter!='$') {			// Hex input?
		if ((uLetter=='0') &&
			!((reinterpret_cast<const Word8 *>(pInput)[0]^'X')&(~0x20U))) {		// 0X or 0x for "C" style input
			++pInput;	// Skip the x and go to the hex parser
		} else {

	// Here's the base 10 code. Common case
	
			Word64 uValue10 = g_AsciiToWord8Table[uLetter];
			if (uValue10<10) {		// First char valid?
				Word uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];
				uLetter = g_AsciiToWord8Table[uAscii10];
				if (uLetter<10) {	// Second char valid?
					do {
						++pInput;
						// Check for overflow
						if ((uValue10>0x1999999999999999ULL) ||		// 0xFFFFFFFFFFFFFFFFULL/10 = 0x1999999999999999
							((uValue10==0x1999999999999999ULL) && (uLetter>=6))) {
							return BURGER_MAXUINT64;			// Tilt!
						}
						uValue10 = (uValue10*10)+uLetter;		// Process into the total
					// Convert char to value
						uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];
						uLetter = g_AsciiToWord8Table[uAscii10];
					} while (uLetter<10);
				}
				
				// Okay, I've got a valid value
				
				uValue10 = (uValue10^uNegate)-uNegate;		// Perform negation?

				// Does the caller want the end address?
				
				if (pDest) {
					// Skip past trailing white space
					
					if (uAscii10==' ' || uAscii10=='\t') {
						do {
							++pInput;
							uAscii10 = reinterpret_cast<const Word8 *>(pInput)[0];	// Remove whitespace
						} while ((uAscii10==' ') || (uAscii10=='\t'));
					}
					pDest[0] = pInput;		// Store the ASCII address
				}
				return uValue10;			// Return the result
			}
			return 0;		// No input, error
		}
	}

	// Base 16
	
	
	{
		Word64 uValue16 = g_AsciiToWord8Table[reinterpret_cast<const Word8 *>(pInput)[0]];
		if (uValue16<16) {
			++pInput;
			Word uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];
			uLetter = g_AsciiToWord8Table[uAscii16];
			if (uLetter<16) {		// Second char valid?
				do {
					++pInput;
					if (uValue16>=0x1000000000000000ULL) {
						return BURGER_MAXUINT64;		// Tilt
					}
					uValue16 = (uValue16<<4U)+uLetter;
					// Convert char to value
					uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];
					uLetter = g_AsciiToWord8Table[uAscii16];
				} while (uLetter<16);
			}
			
			// Perform negation?
			uValue16 = (uValue16^uNegate)-uNegate;

			// Does the caller want the end address?
				
			if (pDest) {
				// Skip past trailing white space
					
				if (uAscii16==' ' || uAscii16=='\t') {
					do {
						++pInput;
						uAscii16 = reinterpret_cast<const Word8 *>(pInput)[0];	// Remove whitespace
					} while ((uAscii16==' ') || (uAscii16=='\t'));
				}
				pDest[0] = pInput;		// Store the ASCII address
			}
			return uValue16;			// Return the result
		}
	}
	return 0;				// Bad value
}

/*! ************************************

	\brief Return a signed integer value

	Scan the value string as a 64 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFFFFFFFFFF will be converted
	to -1.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa AsciiToInteger64(const char *,const char **) or AsciiToWord64(const char *,Word64,Word64,Word64)

***************************************/

Int64 BURGER_API Burger::AsciiToInteger64(const char *pInput,Int64 iDefault,Int64 iMin,Int64 iMax)
{
	if (pInput) {
		const char *pDest;
		// Parse as signed
		Int64 iValue = static_cast<Int64>(AsciiToInteger64(pInput,&pDest));
		if (pDest!=pInput) {
			// Do a signed bounds check
			if (iValue<iMin) {
				iDefault = iMin;
			} else if (iValue>iMax) {
				iDefault = iMax;
			} else {
				iDefault = iValue;
			}
		}
	}
	return iDefault;
}

/*! ************************************

	\brief Convert a 64 bit integer and signal if successful

	Scan the value string as a 64 bit integer or
	hex value and if successful, return \ref TRUE.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFFFFFFFFFF will be converted
	to -1.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was not a number
	\sa AsciiToInteger64(const char *,const char **) or AsciiToInteger64(const char *,Int64,Int64,Int64)

***************************************/

Word BURGER_API Burger::AsciiToInteger64(Word64 *pOutput,const char *pInput)
{
	const char *pDest;
	// Convert the text
	Word64 uOutput = AsciiToInteger64(pInput,&pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest==pInput) {
		uResult = FALSE;
		uOutput = 0;	// Clear the output
	}
	pOutput[0] = uOutput;
	return uResult;		// Return the result
}

/*! ************************************

	\brief Return an unsigned integer value

	Scan the value string as a 64 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa AsciiToInteger64(const char *,const char **) or AsciiToInteger64(const char *,Int64,Int64,Int64)

***************************************/

Word64 BURGER_API Burger::AsciiToWord64(const char *pInput,Word64 uDefault,Word64 uMin,Word64 uMax)
{
	if (pInput) {
		// Convert from UTF-8
		const char *pDest;
		Word64 uValue = AsciiToInteger64(pInput,&pDest);
		// Anything parsed?
		if (pDest!=pInput) {
			// Bounds test
			if (uValue<uMin) {
				uDefault = uMin;
			} else if (uValue>uMax) {
				uDefault = uMax;
			} else {
				uDefault = uValue;
			}
		}
	}
	return uDefault;
}

/***************************************

	Power of 10 table with the powers doubling with each entry
	I use it to perform a binary multiply to cut down on the number
	of muls I need to do before I'm done scaling the number

	The "e" is the number of zeros following the 1
	1e1 = 10, 1e2 = 100, 1e4 = 10000 etc...

***************************************/

static const float PowerOf10FltTable[6] = {		// 6 entries, since 38 is the max
	1e1f, 1e2f, 1e4f, 1e8f, 1e16f, 1e32f
};

/*! ************************************

	\brief Convert an ASCII string into a floating point number.
	
	Take a floating point number of the format (+/-)303.030e(+/-)1 
	and turn it into a float that is the equivalent.
	
	If a number is parsed, the character at the end of the string after
	whitespace is removed is the one pointed by pDest.
	
	Examples:<br>
	1.0<br>
	1.666<br>
	1666.4<br>
	1e3 -> 1000<br>
	NAN -> NAN<br>
	INF -> INF<br>

	\note A 32 bit floating point number is of the range 3.402823e+38 (Max) to 
		1.175494e-38 (min).

	\param pInput Pointer to the ASCII string with the number to parse.
	\param pDest Pointer to receive the character beyond the last character parsed. Will equal pInput on error.
	\return The number as a float.
	
	\sa Burger::AsciiToDouble(const char *,const char **) or Burger::AsciiToInteger(const char *,const char **)
	
***************************************/

float BURGER_API Burger::AsciiToFloat(const char *pInput,const char **pDest)
{
	if (pDest) {
		pDest[0] = pInput;				// Assume an error occurs
	}
	
	// Skip white space
	
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	++pInput;
	if ((uTemp==' ') || (uTemp=='\t')) {
		do {
			uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			++pInput;
		} while ((uTemp==' ') || (uTemp=='\t'));
	}

	// Now, see if the first character is the sign (+, -)
	
	float fNegative = 1.0f;		// Assume positive
	if (uTemp == '+') {				// Just eat the + character
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	} else if (uTemp == '-') {		// Ah, it's negative
		fNegative = -1.0f;		// Negate the answer
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	}

	// Is this NAN or INF?
	
	if (!((uTemp^'N')&(~0x20))) {		// n or N
		Word uT2 = reinterpret_cast<const Word8 *>(pInput)[0];
		if (!((uT2^'A')&(~0x20))) {		// a or A
			uT2 = reinterpret_cast<const Word8 *>(pInput)[1];
			if (!((uT2^'N')&(~0x20))) {	// n or N
				float fResultNan = Burger::g_fNan;
				
				// Any math operation on a NAN will do nothing.
				// So, I use the negate instruction, which will work.
				if (fNegative<0) {
					fResultNan = -fResultNan;
				}
				if (pDest) {
					pDest[0] = pInput+2;
				}
				return fResultNan;
			}
		}
	} else if (!((uTemp^'I')&(~0x20))) {		// I or i
		Word uT3 = reinterpret_cast<const Word8 *>(pInput)[0];
		if (!((uT3^'N')&(~0x20))) {				// N or n
			uT3 = reinterpret_cast<const Word8 *>(pInput)[1];
			if (!((uT3^'F')&(~0x20))) {			// F or f
				float ResultInf = Burger::g_fInf;
				ResultInf *= fNegative;
				if (pDest) {
					pDest[0] = pInput+2;
				}
				return ResultInf;
			}
		}
	}
	
	// Now, lets parse xxx.xxx
	// I'm going to make it easy on myself.
	// I'll copy the digits, sans the period and all leading
	// and trailing zeros.
	// I will parse a maximum of 9 digits
	
	Word uDigitCount = 0;			// Number of digits in my buffer
	Word uFractionDigits = 0;		// Nothing parsed after the period
	Word bNumberFound = FALSE;		// Set to true if a number of any kind (Like all zeros) was found
	Word8 DigitBuffer[9];			// Buffer for digits converted to 0-9 (DO NOT CHANGE THE SIZE!)
	for (;;) {
		if (uTemp == '.' ) {		// Is this a period?
			if (uFractionDigits) {	// Already got one?
				break;				// Can't do double periods!
			}
			uFractionDigits = 1;		// Start the counter by setting this to non-zero
		} else {
			Word uVal = uTemp-'0';	// Precompute
			if (uVal >= 10) {		// Only take numbers (Also breaks on null terminator)
				break;
			}
			bNumberFound = TRUE;		// I got something
			if (uFractionDigits) {	// After the period?
				++uFractionDigits;	// Add to total (Note, all digits AFTER the period count
			}
			if (uDigitCount || uVal) {	// Suppress leading zeros
				if (uDigitCount < sizeof(DigitBuffer)) {		// Buffer overrun? Bad.
					DigitBuffer[uDigitCount] = static_cast<Word8>(uVal);
				}
				++uDigitCount;
			}
		}
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	}

	// Did I parse ANY number?
	// -.e1 is NOT considered a floating point number
	// Accept NOTHING and return 0.0f as my failure condition
	
	if (!bNumberFound) {
		return 0.0f;			// Forget it. Error.
	}
	
	// At this point, I've parsed up to 9 digits
	// I need to get the exponent
	
	int iExponent = 0;			// Assume no extra shifting of the digits
	if (!((uTemp^'E')&(~0x20))) {		// E or e Exponent token?
		const char *pInput2 = pInput;		// Save the mark
		
		uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
		++pInput2;
		Word NegExponent = FALSE;		// Assume a positive exponent
		if (uTemp == '+') {
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		} else if (uTemp == '-' ) {
			NegExponent = TRUE;
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		}
		uTemp-='0';
		if (uTemp<10) {
			do {
				if (iExponent < 100) {		// Prevent overflow
					iExponent = (iExponent * 10) + static_cast<int>(uTemp);
				}
				uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
				++pInput2;
				uTemp -= '0';
			} while (uTemp<10);
			
			pInput = pInput2;		// Since I got an e102 or e-1, I'll accept this input
			// This is the only place where exponent could be non-zero
			if (NegExponent) {
				iExponent = -iExponent;
			}
		}
	}

	// Mark the requested ASCII marker
	
	if (pDest) {
		pDest[0] = pInput-1;		// Backstep 1 char, but this is where I stopped parsing.
	}
	
	// Since I've got an exponent and up to 9 digits of mantissa,
	// let's make a floating point number.
	
	if (uFractionDigits) {		// Did I find a period?
		// Adjust the exponent for how many digits
		// there were to the right of the period
		iExponent -= uFractionDigits-1;	// (The +1 was the flag that I got something)
	}
	
	// I counted the number of digits, but the buffer may not
	// hold them all. Add the truncated count to the exponent
	
	if (uDigitCount > sizeof(DigitBuffer) ) {		// Clipped?
		iExponent += uDigitCount - static_cast<Word>(sizeof(DigitBuffer));	// Add the clipped count

		// Force my counter for parsing to the right amount
		uDigitCount = static_cast<Word>(sizeof(DigitBuffer));
	}

	// Dispose of trailing zeros (It helps with precision)
	
	if (uDigitCount) {
		do {
			if (DigitBuffer[uDigitCount-1]) {	// Last digit, non-zero?
				break;				// Don't touch it
			}
			++iExponent;			// Adjust the exponent
		} while (--uDigitCount);		// Count down
		if (!uDigitCount) {			// Did I remove all of the digits?
			return 0.0f;			// 0 exponent anything, is still zero
		}
	}
	
	// Since we've got something to be converted into
	// a number, let's parse it.
	
	const Word8 *pBuffer = DigitBuffer;
	
	Int32 iLow9 = 0;					// Low 9 digits
	Word i2 = uDigitCount;
	if (i2) {
		do {
			iLow9 = (iLow9 * 10) + static_cast<Int32>(pBuffer[0]);
			++pBuffer;
		} while (--i2);
	}

	// Merge the convert the 32 bit int into a float
	
	float fResult = static_cast<float>(iLow9);

	// Apply the exponent, if any.
	
	if (iExponent) {		// Do I bother?
		if (iExponent<0) {
		
			// I'm going to shrink the number via division
		
			// I'm performing the division operation instead of
			// the reciprocal multiplies because I want to retain
			// as much precision as I can.
			
			Word uCount1 = static_cast<Word>(-iExponent);		// Get abs value
		
			// This is for really stupid numbers
		
			if (uCount1 >= 38) {
				fResult /= 1e38f;
				uCount1 -= 38;
			}
			if (uCount1) {
				const float *pPower1 = PowerOf10FltTable;
				float fScalar1 = 1.0f;
				do {
					if (uCount1&1) {			// Using a double table, I'll get the right power of 10
						fScalar1 *= pPower1[0];
					}
					++pPower1;
					uCount1>>=1;
				} while (uCount1);		// Keep going until I'm done
				fResult /= fScalar1;		// Shrink the number
			}
		} else {
		
			// Same as above, but I'm growing the number this time
			
			Word uCount2 = static_cast<Word>(iExponent);
		
			// This is for really stupid numbers
		
			if (uCount2 >= 38) {
				fResult *= 1e38f;
				uCount2 -= 38;
			}
			if (uCount2) {
				const float *pPower2 = PowerOf10FltTable;
				float fScalar2 = 1.0f;
				do {
					if (uCount2&1) {
						fScalar2 *= pPower2[0];
					}
					++pPower2;
					uCount2>>=1;
				} while (uCount2);
				fResult *= fScalar2;
			}
		}
	}
	
	// Okay, I've got the number. Detect overflow
	
	iExponent+=uDigitCount;
	if (iExponent > (38+1) ) {		// Was the exponent nuts?
		fResult = Burger::g_fMax;	// Grab a constant
	} else if (iExponent < ((-38)+1) ) { 	// Was the exponent nuts in the other direction?
		fResult = Burger::g_fMin;	// Grab a constant
	}
	return fResult*fNegative;			// Return the answer
}

/***************************************

	Power of 10 table with the powers doubling with each entry
	I use it to perform a binary multiply to cut down on the number
	of muls I need to do before I'm done scaling the number

	The "e" is the number of zeros following the 1
	1e1 = 10, 1e2 = 100, 1e4 = 10000 etc...

***************************************/

static const double PowerOf10DblTable[9] = {		// 9 entries, since 308 is the max
	1e1, 1e2, 1e4, 1e8, 1e16, 1e32, 1e64, 1e128, 1e256
};

/*! ************************************

	\brief Convert an ASCII string into a floating point number.
	
	Take a floating point number of the format (+/-)303.030e(+/-)1 
	and turn it into a double that is the equivalent.
	
	If a number is parsed, the character at the end of the string after
	whitespace is removed is the one pointed by pDest.
	
	Examples:<br>
	1.0<br>
	1.666<br>
	1666.4<br>
	1e3 -> 1000<br>
	NAN -> NAN<br>
	INF -> INF<br>

	\note A 64 bit floating point number is of the range 1.797693134862316e+308 (Max) to 
		2.225073858507201e-308 (min).
		
	\note This function is not recommended for use on the PlayStation 2 due to its heavy
	use of double operations. Use Burger::AsciiToFloat(const char *,const char **)
	instead.

	\param pInput Pointer to the ASCII string with the number to parse.
	\param pDest Pointer to receive the character beyond the last character parsed. Will equal pInput on error.
	\return The number as a double.
	\sa Burger::AsciiToFloat(const char *,const char **) or Burger::AsciiToInteger(const char *,const char **)
	
***************************************/

double BURGER_API Burger::AsciiToDouble(const char *pInput,const char **pDest)
{
	if (pDest) {
		pDest[0] = pInput;				// Assume an error occurs
	}
	
	// Skip white space
	
	Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
	++pInput;
	if ((uTemp==' ') || (uTemp=='\t')) {
		do {
			uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
			++pInput;
		} while ((uTemp==' ') || (uTemp=='\t'));
	}

	// Now, see if the first character is the sign (+, -)
	
	double dNegative = 1.0;		// Assume positive
	if (uTemp == '+') {				// Just eat the + character
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	} else if (uTemp == '-') {		// Ah, it's negative
		dNegative = -1.0;		// Negate the answer
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	}

	// Is this NAN or INF?
	
	if (!((uTemp^'N')&(~0x20))) {		// N or n
		Word uT2 = reinterpret_cast<const Word8 *>(pInput)[0];
		if (!((uT2^'A')&(~0x20))) {		// A or a
			uT2 = reinterpret_cast<const Word8 *>(pInput)[1];
			if (!((uT2^'N')&(~0x20))) {	// N or n
				double dResultNan = Burger::g_dNan;
				
				// Any math operation on a NAN will do nothing.
				// So, I use the negate instruction, which will work.
				if (dNegative<0) {
					dResultNan = -dResultNan;
				}
				if (pDest) {
					pDest[0] = pInput+2;
				}
				return dResultNan;
			}
		}
	} else if (!((uTemp^'I')&(~0x20))) {	// I or i
		Word uT3 = reinterpret_cast<const Word8 *>(pInput)[0];
		if (!((uT3^'N')&(~0x20))) {			// N or n
			uT3 = reinterpret_cast<const Word8 *>(pInput)[1];
			if (!((uT3^'F')&(~0x20))) {		// F or f
				double dResultInf = Burger::g_dInf;
				dResultInf *= dNegative;
				if (pDest) {
					pDest[0] = pInput+2;
				}
				return dResultInf;
			}
		}
	}
	
	// Now, lets parse xxx.xxx
	// I'm going to make it easy on myself.
	// I'll copy the digits, sans the period and all leading
	// and trailing zeros.
	// I will parse a maximum of 18 digits
	
	Word uDigitCount = 0;			// Number of digits in my buffer
	Word uFractionDigits = 0;		// Nothing parsed after the period
	Word bNumberFound = FALSE;		// Set to true if a number of any kind (Like all zeros) was found
	Word8 DigitBuffer[18];			// Buffer for digits converted to 0-9 (DO NOT CHANGE THE SIZE!)
	for (;;) {
		if (uTemp == '.' ) {		// Is this a period?
			if (uFractionDigits) {	// Already got one?
				break;				// Can't do double periods!
			}
			uFractionDigits = 1;		// Start the counter by setting this to non-zero
		} else {
			Word uVal = uTemp-'0';	// Precompute
			if (uVal >= 10) {		// Only take numbers (Also breaks on null terminator)
				break;
			}
			bNumberFound = TRUE;		// I got something
			if (uFractionDigits) {	// After the period?
				++uFractionDigits;	// Add to total (Note, all digits AFTER the period count
			}
			if (uDigitCount || uVal) {	// Suppress leading zeros
				if (uDigitCount < sizeof(DigitBuffer)) {		// Buffer overrun? Bad.
					DigitBuffer[uDigitCount] = static_cast<Word8>(uVal);
				}
				++uDigitCount;
			}
		}
		uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		++pInput;
	}

	// Did I parse ANY number?
	// -.e1 is NOT considered a floating point number
	// Accept NOTHING and return 0.0 as my failure condition
	
	if (!bNumberFound) {
		return 0.0;			// Forget it. Error.
	}
	
	// At this point, I've parsed up to 18 digits
	// I need to get the exponent
	
	int iExponent = 0;			// Assume no extra shifting of the digits
	if (!((uTemp^'E')&(~0x20))) {		// E or e Exponent token?
		const char *pInput2 = pInput;		// Save the mark
		
		uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
		++pInput2;
		Word bNegExponent = FALSE;		// Assume a positive exponent
		if (uTemp == '+') {
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		} else if (uTemp == '-' ) {
			bNegExponent = TRUE;
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		}
		uTemp-='0';
		if (uTemp<10) {
			do {
				if (iExponent < 1000) {		// Prevent overflow
					iExponent = (iExponent * 10) + static_cast<int>(uTemp);
				}
				uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
				++pInput2;
				uTemp -= '0';
			} while (uTemp<10);
			
			pInput = pInput2;		// Since I got an e102 or e-1, I'll accept this input
			// This is the only place where exponent could be non-zero
			if (bNegExponent) {
				iExponent = -iExponent;
			}
		}
	}

	// Mark the requested ASCII marker
	
	if (pDest) {
		pDest[0] = pInput-1;		// Backstep 1 char, but this is where I stopped parsing.
	}
	
	// Since I've got an exponent and up to 18 digits of mantissa,
	// let's make a floating point number.
	
	if (uFractionDigits) {		// Did I find a period?
		// Adjust the exponent for how many digits
		// there were to the right of the period
		iExponent -= uFractionDigits-1;	// (The +1 was the flag that I got something)
	}
	
	// I counted the number of digits, but the buffer may not
	// hold them all. Add the truncated count to the exponent
	
	if (uDigitCount > sizeof(DigitBuffer) ) {		// Clipped?
		iExponent += uDigitCount - static_cast<Word>(sizeof(DigitBuffer));	// Add the clipped count

		// Force my counter for parsing to the right amount
		uDigitCount = static_cast<Word>(sizeof(DigitBuffer));
	}

	// Dispose of trailing zeros (It helps with precision)
	
	if (uDigitCount) {
		do {
			if (DigitBuffer[uDigitCount-1]) {	// Last digit, non-zero?
				break;				// Don't touch it
			}
			++iExponent;			// Adjust the exponent
		} while (--uDigitCount);		// Count down
		if (!uDigitCount) {			// Did I remove all of the digits?
			return 0.0;				// 0 exponent anything, is still zero
		}
	}
	
	// Since we've got something to be converted into
	// a number, let's parse it.
	// PROBLEM: A 32 bit long can only store 9 digits safely, so I take
	// the 18 digits I have and split them into two longs of 9 digits each.
	// Then I'll merge the two using the double unit.
	
	const Word8 *pBuffer = DigitBuffer;
	Int32 iHigh9 = 0;					// High 9 digits
	if (uDigitCount>9) {					// 1-18
		// Parse up to 9 digits
		Word i1 = uDigitCount-9;			// 1-9
		do {
			iHigh9 = (iHigh9 * 10) + static_cast<Int32>(pBuffer[0]);
			++pBuffer;
		} while (--i1);
	}
	
	Int32 iLow9 = 0;					// Low 9 digits
	Word i2 = uDigitCount;
	if (i2>9) {
		i2 = 9;							// Clamp
	}
	if (i2) {
		do {
			iLow9 = (iLow9 * 10) + static_cast<Int32>(pBuffer[0]);
			++pBuffer;
		} while (--i2);
	}

	// Merge the two 32 bit integers into a 64 bit float
	
	double dResult = (static_cast<double>(iHigh9) * 1e9) + static_cast<double>(iLow9);

	// Apply the exponent, if any.
	
	if (iExponent) {		// Do I bother?
		if (iExponent<0) {
		
			// I'm going to shrink the number via division
		
			// I'm performing the division operation instead of
			// the reciprocal multiplies because I want to retain
			// as much precision as I can.
			
			Word uCount1 = static_cast<Word>(-iExponent);		// Get abs value
		
			// This is for really stupid numbers
		
			if (uCount1 >= 308) {
				dResult /= 1e308;
				uCount1 -= 308;
			}
			if (uCount1) {
				const double *pPower1 = PowerOf10DblTable;
				double dScalar1 = 1.0;
				do {
					if (uCount1&1) {			// Using a double table, I'll get the right power of 10
						dScalar1 *= pPower1[0];
					}
					++pPower1;
					uCount1>>=1;
				} while (uCount1);		// Keep going until I'm done
				dResult /= dScalar1;		// Shrink the number
			}
		} else {
		
			// Same as above, but I'm growing the number this time
			
			Word uCount2 = static_cast<Word>(iExponent);
		
			// This is for really stupid numbers
		
			if (uCount2 >= 308) {
				dResult *= 1e308;
				uCount2 -= 308;
			}
			if (uCount2) {
				const double *pPower2 = PowerOf10DblTable;
				double dScalar2 = 1.0;
				do {
					if (uCount2&1) {
						dScalar2 *= pPower2[0];
					}
					++pPower2;
					uCount2>>=1;
				} while (uCount2);
				dResult *= dScalar2;
			}
		}
	}
	
	// Okay, I've got the number. Detect overflow
	
	iExponent+=uDigitCount;
	if (iExponent > (308+1) ) {		// Was the exponent nuts?
		dResult = g_dMax;	// Grab a constant
	} else if (iExponent < ((-308)+1) ) { 	// Was the exponent nuts in the other direction?
		dResult = g_dMin;	// Grab a constant
	}
	return dResult*dNegative;			// Return the answer
}

/*! ************************************

	\brief Convert an ASCII string into a boolean.
	
	If the case insensitive string matches yes, no, true or false, return 1,0,1,0
	in that order. Otherwise, parse it as a number and return 1 if it's non-zero
	and 0 if it's zero or invalid.

	String matching does not look for ending whitespace, so true and truevalue will both
	match as "true" and return 1. The pDest return value will be moved forward by
	four characters in both cases.
	
	\param pInput Pointer to the string to convert. \ref NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput (Error)
		of pInput moved ahead until a non-numeric character was found. This can
		be \ref NULL if the application doesn't require the location of the string after
		the integer was parsed.
		
	\return 0 if no, false, ASCII 0 or invalid input. 1 if yes, true or a non-zero numeric value.
	
	\sa AsciiToInteger(const char *,const char **) or AsciiToBoolean(const char *,Word)

***************************************/

#if !defined(DOXYGEN)
struct BooleanLookup_t {
	const char *m_pASCII;
	Word32 m_uSize;
	Word32 m_bValue;
};

static const BooleanLookup_t g_BooleanLookup[] = {
	{Burger::g_TrueString,static_cast<Word32>(sizeof(Burger::g_TrueString)-1),TRUE},
	{Burger::g_YesString,static_cast<Word32>(sizeof(Burger::g_YesString)-1),TRUE},
	{Burger::g_FalseString,static_cast<Word32>(sizeof(Burger::g_FalseString)-1),FALSE},
	{Burger::g_NoString,static_cast<Word32>(sizeof(Burger::g_NoString)-1),FALSE}
};
#endif

Word BURGER_API Burger::AsciiToBoolean(const char *pInput,const char **pDest)
{
	const BooleanLookup_t *pBooleanLookup = g_BooleanLookup;
	WordPtr i = BURGER_ARRAYSIZE(g_BooleanLookup);
	do {
		if (!MemoryCaseCompare(pBooleanLookup->m_pASCII,pInput,pBooleanLookup->m_uSize)) {
			if (pDest) {
				pDest[0] = pInput+pBooleanLookup->m_uSize;
			}
			return pBooleanLookup->m_bValue;
		}
		++pBooleanLookup;
	} while (--i);
	// Return 1 if non-zero
	return AsciiToInteger(pInput,pDest)!=0;
}

/*! ************************************

	\brief Convert an ASCII string into a boolean.
	
	Convert the string to a \ref TRUE or a \ref FALSE. If the input is
	\ref NULL or invalid, return the default value.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are considered
	\ref FALSE. The comparison is case insensitive.
	
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param bDefault Default boolean to return in the event of a parsing error.
		
	\return \ref TRUE or \ref FALSE
	
	\sa AsciiToBoolean(const char *,const char **)

***************************************/

Word BURGER_API Burger::AsciiToBoolean(const char *pInput,Word bDefault)
{
	if (pInput) {
		const char *pDest;
		// Anything parsed?
		Word bValue = AsciiToBoolean(pInput,&pDest);
		if (pDest!=pInput) {
			bDefault = bValue;
		}
	}
	return bDefault;
}

/*! ************************************

	\brief Convert an ASCII string into a boolean and signal if successful
	
	Convert the string to a \ref TRUE or a \ref FALSE. If the input is
	\ref NULL or invalid, return \ref FALSE.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are considered
	\ref FALSE. The comparison is case insensitive.
	
	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was not a boolean
	
	\sa AsciiToBoolean(const char *,const char **) or AsciiToBoolean(const char *,Word)

***************************************/

Word BURGER_API Burger::AsciiToBoolean(Word *pOutput,const char *pInput)
{
	const char *pDest;
	// Convert the text
	Word uOutput = AsciiToBoolean(pInput,&pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest==pInput) {
		uResult = FALSE;
		uOutput = 0;	// Clear the output
	}
	pOutput[0] = uOutput;
	return uResult;		// Return the result
}

/*! ************************************

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param fDefault Value to return on error
	\return Value parsed or fDefault
	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(const char *,double)

***************************************/

float BURGER_API Burger::AsciiToFloat(const char *pInput,float fDefault)
{
	if (pInput) {
		const char *pDest;
		// Anything parsed?
		float fValue = AsciiToFloat(pInput,&pDest);
		if (pDest!=pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(fValue)) {
				fDefault = 0.0f;		// Nope, nope, nope...
			} else {
				fDefault = fValue;
			}
		}
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(const char *,double,double,double)

***************************************/

float BURGER_API Burger::AsciiToFloat(const char *pInput,float fDefault,float fMin,float fMax)
{
	if (pInput) {
		const char *pDest;
		// Anything parsed?
		float fValue = AsciiToFloat(pInput,&pDest);
		if (pDest!=pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(fValue)) {
				fDefault = 0.0f;		// Nope, nope, nope...
			} else if (fValue<fMin) {
				fDefault = fMin;
			} else if (fValue>fMax) {
				fDefault = fMax;
			} else {
				fDefault = fValue;
			}
		}
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value and signal if successful

	Scan the value string as a 32 bit floating point
	numeric value and if successful, return \ref TRUE.
	If it's not a number, return \ref FALSE.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was not a 32 bit float
	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(double *,const char *)

***************************************/

Word BURGER_API Burger::AsciiToFloat(float *pOutput,const char *pInput)
{
	const char *pDest;
	// Convert the text
	float fOutput = AsciiToFloat(pInput,&pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest==pInput) {
		uResult = FALSE;
		fOutput = 0.0f;	// Clear the output
	}
	pOutput[0] = fOutput;
	return uResult;		// Return the result
}

/*! ************************************

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param dDefault Value to return on error
	\return Value or dDefault
	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(const char *,float)

***************************************/

double BURGER_API Burger::AsciiToDouble(const char *pInput,double dDefault)
{
	if (pInput) {
		const char *pDest;
		double dValue = AsciiToDouble(pInput,&pDest);
		if (pDest!=pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(dValue)) {
				dDefault = 0.0f;		// Nope, nope, nope...
			} else {
				dDefault = dValue;
			}
		}
	}
	return dDefault;
}

/*! ************************************

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(const char *,float,float,float)

***************************************/

double BURGER_API Burger::AsciiToDouble(const char *pInput,double dDefault,double dMin,double dMax)
{
	if (pInput) {
		const char *pDest;
		double dValue = AsciiToDouble(pInput,&pDest);
		if (pDest!=pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(dValue)) {
				dDefault = 0.0f;		// Nope, nope, nope...
			} else if (dValue<dMin) {
				dDefault = dMin;
			} else if (dValue>dMax) {
				dDefault = dMax;
			} else {
				dDefault = dValue;
			}
		}
	}
	return dDefault;
}

/*! ************************************

	\brief Return a 64 bit floating point value and signal if successful

	Scan the value string as a 64 bit floating point
	numeric value and if successful, return \ref TRUE.
	If it's not a number, return \ref FALSE.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the default
	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was not a 64 bit float
	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(float *,const char *)

***************************************/

Word BURGER_API Burger::AsciiToDouble(double *pOutput,const char *pInput)
{
	const char *pDest;
	// Convert the text
	double dOutput = AsciiToDouble(pInput,&pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest==pInput) {
		uResult = FALSE;
		dOutput = 0.0;	// Clear the output
	}
	pOutput[0] = dOutput;
	return uResult;		// Return the result
}

/*! ************************************

	\brief Convert hex ASCII string to an integer

	Scan a hex string and return a 32 bit unsigned integer

	Parsing ends either when characters are exhausted of if a non-ASCII
	character is found. Overflow returns \ref BURGER_MAXUINT

	\param pInput Pointer to the ASCII string
	\param uLength Length of the string in bytes
	\return 0 if the string is invalid, or the hex value
	\sa AsciiHexToInteger(const Word16 *,WordPtr)

***************************************/

Word32 BURGER_API Burger::AsciiHexToInteger(const char *pInput,WordPtr uLength)
{
	Word32 uResult = 0;
	// Input valid?
	if (pInput && uLength) {
		do {
			// Look up the character
			Word32 uValue16 = g_AsciiToWord8Table[reinterpret_cast<const Word8 *>(pInput)[0]];
			// Not 0-9, A-F?
			if (uValue16>=16) {
				// Exit
				break;
			}
			++pInput;
			// Overflow?
			if (uResult>=0x10000000) {
				uResult = BURGER_MAXUINT;		// Tilt
				break;
			}
			// Fold in the number
			uResult = (uResult<<4U)+uValue16;
		} while (--uLength);
	}
	return uResult;
}

/*! ************************************

	\brief Convert hex UTF-16 string to an integer

	Scan a hex string and return a 32 bit unsigned integer

	Parsing ends either when characters are exhausted of if a non-ASCII
	character is found. Overflow returns \ref BURGER_MAXUINT

	\param pInput Pointer to the UTF-16 string
	\param uLength Length of the string in bytes
	\return 0 if the string is invalid, or the hex value
	\sa AsciiHexToInteger(const char *,WordPtr)

***************************************/

Word32 BURGER_API Burger::AsciiHexToInteger(const Word16 *pInput,WordPtr uLength)
{
	Word32 uResult = 0;
	// Input valid?
	if (pInput && uLength) {
		do {
			// Get a character
			Word uTemp = pInput[0];
			// End now?
			if (uTemp>=256) {
				break;
			}
			// Look up the character
			Word32 uValue16 = g_AsciiToWord8Table[uTemp];
			// Not 0-9, A-F?
			if (uValue16>=16) {
				// Exit
				break;
			}
			++pInput;
			// Overflow?
			if (uResult>=0x10000000) {
				uResult = BURGER_MAXUINT;		// Tilt
				break;
			}
			// Fold in the number
			uResult = (uResult<<4U)+uValue16;
		} while (--uLength);
	}
	return uResult;
}