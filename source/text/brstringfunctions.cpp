/***************************************

    Stand alone string functions

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brstringfunctions.h"
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brglobalmemorymanager.h"
#include "brisolatin1.h"
#include "brmemoryfunctions.h"
#include <stdio.h>

//
// COMPILER BUG! In Open Watcom 1.8 and 1.9, it can inline an assembly language
// function (StringLength()) even though it's declared as __declspec(naked) This
// means that the code could be inserted in a "C" function and since the
// assembly has a ret instruction inside of it, it will return control too soon
// resulting in a crash.
//

#if defined(BURGER_WATCOM)
#if (BURGER_WATCOM == 1280) || \
    (BURGER_WATCOM == 1290) // Test for Open Watcom 1.8 or 1.9
#pragma inline_depth(0)
#elif (BURGER_WATCOM != 1300)
#error Check if the Open Watcom bug is present in this version of the compiler
#endif
#endif

/*! ************************************

    \enum Burger::eNumericConversionFlags
    \brief Bit flags to control numeric text generation.

    The functions that convert a float or integer into an ASCII string use these
    flags to suppress the output of the terminating zero and/or leading
    zeros. The default behavior is that a zero is output as the last character
    so that the string is a valid "C" string and no leading ASCII zeros.

    \sa NumberToAscii(char *,uint32_t,uint_t)

***************************************/

/*! ************************************

    \enum Burger::eASCIITypeFlag
    \brief Bit flags to quickly test low ASCII character type.

    When \ref g_AsciiTestTable[256] is indexed with a low ASCII character in the
    range of 0 through 127 inclusive, the value will contain one or more flags
    to quickly determine the type of ASCII character.

    \sa g_AsciiTestTable[256]

***************************************/

/*! ************************************

    \struct Burger::FourCharacterCode_t
    \brief Helper structure to create 4 characters codes

    On some platforms and file formats, a four character code is stored in a big
    endian 32 bit integer. To ensure these codes are generated in a platform
    neutral way, this structure is used to statically create a code that can be
    translated directly into a 32 bit value that can be used to directly to a
    single integer comparison for testing codes.

    \code

    const Burger::FourCharacterCode_t AIFFCode = {'A','I','F','F'};

    uint32_t MatchAIFF(const void *pData)
    {
        return static_cast<const uint32_t *>(pData)[0] == AIFFCode;
    }

    \endcode

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::operator const char *() const
    \brief Access the character code as an char array.

    \return Pointer to the first character in the four character code

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::operator char *()
    \brief Access the character code as an char array.

    \return Pointer to the first character in the four character code

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::operator uint32_t()
    \brief Read the four character code as an integer.

    \return The four character code as a 32 bit unsigned integer.
    \sa GetCode(void) const

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::operator = (uint32_t uInput)
    \brief Assign a four character code with an integer.

    \param uInput Four character code in big endian format
    \return A reference to the FourCharacterCode_t
    \sa SetCode(uint32_t uInput)

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::GetCode(void) const
    \brief Read the four character code as an integer.

    \return The four character code as a 32 bit unsigned integer.
    \sa \ref operator uint32_t() const

***************************************/

/*! ************************************

    \fn Burger::FourCharacterCode_t::SetCode(uint32_t uInput)
    \brief Assign a four character code with an integer.

    \param uInput Four character code in big endian format
    \sa \ref operator = (uint32_t uInput)

***************************************/

/*! ************************************

    \brief ""
    This string is hard coded to be an empty string. It saves space by having a
    singular occurrence.

***************************************/

const char Burger::g_EmptyString[1] = {0};

/*! ************************************

    \brief "\r\n"

    This string is hard coded to have the Win32 CR/LF EOL string.
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_CRLFString[3] = {'\r', '\n', 0};

/*! ************************************

    \brief "\r"

    This string is hard coded to have the MacOS CR EOL string.
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_CRString[2] = {'\r', 0};

/*! ************************************

    \brief "\n"

    This string is hard coded to have the Linux/Unix LF EOL string.
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_LFString[2] = {'\n', 0};

/*! ************************************

    \brief "true"

    This string is hard coded to have the string "true".
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_TrueString[5] = {'t', 'r', 'u', 'e', 0};

/*! ************************************

    \brief "false"

    This string is hard coded to have the string "false".
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_FalseString[6] = {'f', 'a', 'l', 's', 'e', 0};

/*! ************************************

    \brief "yes"

    This string is hard coded to have the string "yes".
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_YesString[4] = {'y', 'e', 's', 0};

/*! ************************************

    \brief "no"

    This string is hard coded to have the string "no".
    It saves space by having a singular occurrence.

***************************************/

const char Burger::g_NoString[3] = {'n', 'o', 0};

/*! ************************************

    \brief Days of the week

    An array of seven strings for the seven days of the week. Each string is
    capitalized so they are "Sunday", "Monday", etc...

***************************************/

const char* Burger::g_WeekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"};

/*! ************************************

    \brief Months of the year

    An array of twelve strings for the twelve months of the year. Each string is
    capitalized so they are "January", "February", etc...

***************************************/

const char* Burger::g_Months[12] = {"January", "February", "March", "April",
    "May", "June", "July", "August", "September", "October", "November",
    "December"};

/*! ************************************

    \brief "" in UTF16

    This string is hard coded to be an empty string.
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_EmptyString16[1] = {0};

/*! ************************************

    \brief "\r\n" in UTF16

    This string is hard coded to have the Win32 CR/LF EOL string.
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_CRLFString16[3] = {'\r', '\n', 0};

/*! ************************************

    \brief "\r" in UTF16

    This string is hard coded to have the MacOS CR EOL string.
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_CRString16[2] = {'\r', 0};

/*! ************************************

    \brief "\n" in UTF16

    This string is hard coded to have the Linux/Unix LF EOL string.
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_LFString16[2] = {'\n', 0};

/*! ************************************

    \brief "true" in UTF16

    This string is hard coded to have the string "true".
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_TrueString16[5] = {'t', 'r', 'u', 'e', 0};

/*! ************************************

    \brief "false" in UTF16

    This string is hard coded to have the string "false".
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_FalseString16[6] = {'f', 'a', 'l', 's', 'e', 0};

/*! ************************************

    \brief "yes" in UTF16

    This string is hard coded to have the string "yes".
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_YesString16[4] = {'y', 'e', 's', 0};

/*! ************************************

    \brief "no" in UTF16

    This string is hard coded to have the string "no".
    It saves space by having a singular occurrence.

***************************************/

const uint16_t Burger::g_NoString16[3] = {'n', 'o', 0};

/*! ************************************

    \var const uint8_t Burger::g_AsciiTestTable[256]
    \brief Table to quickly determine the type of low ASCII character.

    This table is used for code to quickly determine if an ASCII code from 0 to
    127 is upper case, lower case, whitespace, etc.

    Entries 128-255 are all zeros, because they are escape codes for UTF-8
    lookup

    \sa eASCIITypeFlag

***************************************/

BURGER_ALIGN(const uint8_t, Burger::g_AsciiTestTable[256], 16) = {
    Burger::ASCII_CONTROL, // 00 (NUL)
    Burger::ASCII_CONTROL, // 01 (SOH)
    Burger::ASCII_CONTROL, // 02 (STX)
    Burger::ASCII_CONTROL, // 03 (ETX)
    Burger::ASCII_CONTROL, // 04 (EOT)
    Burger::ASCII_CONTROL, // 05 (ENQ)
    Burger::ASCII_CONTROL, // 06 (ACK)
    Burger::ASCII_CONTROL, // 07 (BEL)
    Burger::ASCII_CONTROL, // 08 (BS)
    Burger::ASCII_SPACE | Burger::ASCII_CONTROL |
        Burger::ASCII_WHITESPACE,                   // 09 (HT)
    Burger::ASCII_SPACE | Burger::ASCII_CONTROL,    // 0A (LF)
    Burger::ASCII_SPACE | Burger::ASCII_CONTROL,    // 0B (VT)
    Burger::ASCII_SPACE | Burger::ASCII_CONTROL,    // 0C (FF)
    Burger::ASCII_SPACE | Burger::ASCII_CONTROL,    // 0D (CR)
    Burger::ASCII_CONTROL,                          // 0E (SI)
    Burger::ASCII_CONTROL,                          // 0F (SO)
    Burger::ASCII_CONTROL,                          // 10 (DLE)
    Burger::ASCII_CONTROL,                          // 11 (DC1)
    Burger::ASCII_CONTROL,                          // 12 (DC2)
    Burger::ASCII_CONTROL,                          // 13 (DC3)
    Burger::ASCII_CONTROL,                          // 14 (DC4)
    Burger::ASCII_CONTROL,                          // 15 (NAK)
    Burger::ASCII_CONTROL,                          // 16 (SYN)
    Burger::ASCII_CONTROL,                          // 17 (ETB)
    Burger::ASCII_CONTROL,                          // 18 (CAN)
    Burger::ASCII_CONTROL,                          // 19 (EM)
    Burger::ASCII_CONTROL,                          // 1A (SUB)
    Burger::ASCII_CONTROL,                          // 1B (ESC)
    Burger::ASCII_CONTROL,                          // 1C (FS)
    Burger::ASCII_CONTROL,                          // 1D (GS)
    Burger::ASCII_CONTROL,                          // 1E (RS)
    Burger::ASCII_CONTROL,                          // 1F (US)
    Burger::ASCII_SPACE | Burger::ASCII_WHITESPACE, // 20 SPACE
    Burger::ASCII_PUNCTUATION,                      // 21 !
    Burger::ASCII_PUNCTUATION,                      // 22 "
    Burger::ASCII_PUNCTUATION,                      // 23 #
    Burger::ASCII_PUNCTUATION,                      // 24 $
    Burger::ASCII_PUNCTUATION,                      // 25 %
    Burger::ASCII_PUNCTUATION,                      // 26 &
    Burger::ASCII_PUNCTUATION,                      // 27 '
    Burger::ASCII_PUNCTUATION,                      // 28 (
    Burger::ASCII_PUNCTUATION,                      // 29 )
    Burger::ASCII_PUNCTUATION,                      // 2A *
    Burger::ASCII_PUNCTUATION,                      // 2B +
    Burger::ASCII_PUNCTUATION,                      // 2C ,
    Burger::ASCII_PUNCTUATION,                      // 2D -
    Burger::ASCII_PUNCTUATION,                      // 2E .
    Burger::ASCII_PUNCTUATION,                      // 2F /
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 30 0
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 31 1
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 32 2
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 33 3
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 34 4
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 35 5
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 36 6
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 37 7
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 38 8
    Burger::ASCII_DIGIT | Burger::ASCII_HEX,        // 39 9
    Burger::ASCII_PUNCTUATION,                      // 3A :
    Burger::ASCII_PUNCTUATION,                      // 3B ;
    Burger::ASCII_PUNCTUATION,                      // 3C <
    Burger::ASCII_PUNCTUATION,                      // 3D =
    Burger::ASCII_PUNCTUATION,                      // 3E >
    Burger::ASCII_PUNCTUATION,                      // 3F ?
    Burger::ASCII_PUNCTUATION,                      // 40 @
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 41 A
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 42 B
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 43 C
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 44 D
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 45 E
    Burger::ASCII_UPPER | Burger::ASCII_HEX,        // 46 F
    Burger::ASCII_UPPER,                            // 47 G
    Burger::ASCII_UPPER,                            // 48 H
    Burger::ASCII_UPPER,                            // 49 I
    Burger::ASCII_UPPER,                            // 4A J
    Burger::ASCII_UPPER,                            // 4B K
    Burger::ASCII_UPPER,                            // 4C L
    Burger::ASCII_UPPER,                            // 4D M
    Burger::ASCII_UPPER,                            // 4E N
    Burger::ASCII_UPPER,                            // 4F O
    Burger::ASCII_UPPER,                            // 50 P
    Burger::ASCII_UPPER,                            // 51 Q
    Burger::ASCII_UPPER,                            // 52 R
    Burger::ASCII_UPPER,                            // 53 S
    Burger::ASCII_UPPER,                            // 54 T
    Burger::ASCII_UPPER,                            // 55 U
    Burger::ASCII_UPPER,                            // 56 V
    Burger::ASCII_UPPER,                            // 57 W
    Burger::ASCII_UPPER,                            // 58 X
    Burger::ASCII_UPPER,                            // 59 Y
    Burger::ASCII_UPPER,                            // 5A Z
    Burger::ASCII_PUNCTUATION,                      // 5B [
    Burger::ASCII_PUNCTUATION,                      // 5C "\"
    Burger::ASCII_PUNCTUATION,                      // 5D ]
    Burger::ASCII_PUNCTUATION,                      // 5E ^
    Burger::ASCII_PUNCTUATION,                      // 5F _
    Burger::ASCII_PUNCTUATION,                      // 60 `
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 61 a
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 62 b
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 63 c
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 64 d
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 65 e
    Burger::ASCII_LOWER | Burger::ASCII_HEX,        // 66 f
    Burger::ASCII_LOWER,                            // 67 g
    Burger::ASCII_LOWER,                            // 68 h
    Burger::ASCII_LOWER,                            // 69 i
    Burger::ASCII_LOWER,                            // 6A j
    Burger::ASCII_LOWER,                            // 6B k
    Burger::ASCII_LOWER,                            // 6C l
    Burger::ASCII_LOWER,                            // 6D m
    Burger::ASCII_LOWER,                            // 6E n
    Burger::ASCII_LOWER,                            // 6F o
    Burger::ASCII_LOWER,                            // 70 p
    Burger::ASCII_LOWER,                            // 71 q
    Burger::ASCII_LOWER,                            // 72 r
    Burger::ASCII_LOWER,                            // 73 s
    Burger::ASCII_LOWER,                            // 74 t
    Burger::ASCII_LOWER,                            // 75 u
    Burger::ASCII_LOWER,                            // 76 v
    Burger::ASCII_LOWER,                            // 77 w
    Burger::ASCII_LOWER,                            // 78 x
    Burger::ASCII_LOWER,                            // 79 y
    Burger::ASCII_LOWER,                            // 7A z
    Burger::ASCII_PUNCTUATION,                      // 7B {
    Burger::ASCII_PUNCTUATION,                      // 7C |
    Burger::ASCII_PUNCTUATION,                      // 7D }
    Burger::ASCII_PUNCTUATION,                      // 7E ~
    Burger::ASCII_CONTROL,                          // 7F (DEL)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80-8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 90-9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A0-AF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B0-BF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C0-CF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D0-DF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E0-EF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // F0-FF
};

/*! ************************************

    \var const char Burger::g_NibbleToAsciiUppercase[16]
    \brief Table to convert nibbles (0-15) into upper case ASCII characters.

    This table is used for code to quickly map the hex values of 0-15 into the
    digits 0-9 and A-F.

    \sa Burger::g_NibbleToAsciiLowercase[16]

***************************************/

BURGER_ALIGN(const char, Burger::g_NibbleToAsciiUppercase[16], 16) = {'0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/*! ************************************

    \var const char Burger::g_NibbleToAsciiLowercase[16]
    \brief Table to convert nibbles (0-15) into lower case ASCII characters.

    This table is used for code to quickly map the hex values of 0-15 into the
    digits 0-9 and a-f.

    \sa Burger::g_NibbleToAsciiUppercase[16]

***************************************/

BURGER_ALIGN(const char, Burger::g_NibbleToAsciiLowercase[16], 16) = {'0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/*! ************************************

    \var const uint8_t Burger::g_AsciiToWord8Table[256]
    \brief Table to convert ASCII characters into nibbles (0-15).

    This table is used for code to quickly map the ASCII values of '0'-'9',
    'A'-'F' and 'a'-'f' into the hex digits 0x00-0x0F.

***************************************/

BURGER_ALIGN(const uint8_t, Burger::g_AsciiToWord8Table[256], 16) = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x00
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x10
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x20
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x30
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x40
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x50
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x60
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x70
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x80
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0x90
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0xA0
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0xB0
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0xC0
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0xD0
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, // 0xE0
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF // 0xF0
};

/*! ************************************

    \var const uint8_t Burger::g_ReverseBits[256]
    \brief Table to reverse the bit order of a byte.

    This table will take the a uint8_t used as an index and return the bit
    reversed value of that index. 0x80 -> 0x01, 0xAA -> 0x55, 0x32 -> 0x4C.

***************************************/

BURGER_ALIGN(const uint8_t, Burger::g_ReverseBits[256], 16) = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
    0x30, 0xB0, 0x70, 0xF0, // 0x00
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8,
    0x38, 0xB8, 0x78, 0xF8, // 0x10
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4,
    0x34, 0xB4, 0x74, 0xF4, // 0x20
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC, // 0x30
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2,
    0x32, 0xB2, 0x72, 0xF2, // 0x40
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA,
    0x3A, 0xBA, 0x7A, 0xFA, // 0x50
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
    0x36, 0xB6, 0x76, 0xF6, // 0x60
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE,
    0x3E, 0xBE, 0x7E, 0xFE, // 0x70
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1,
    0x31, 0xB1, 0x71, 0xF1, // 0x80
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
    0x39, 0xB9, 0x79, 0xF9, // 0x90
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5,
    0x35, 0xB5, 0x75, 0xF5, // 0xA0
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD,
    0x3D, 0xBD, 0x7D, 0xFD, // 0xB0
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
    0x33, 0xB3, 0x73, 0xF3, // 0xC0
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB,
    0x3B, 0xBB, 0x7B, 0xFB, // 0xD0
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7,
    0x37, 0xB7, 0x77, 0xF7, // 0xE0
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF // 0xF0
};

/*! ************************************

    \brief Table used for testing powers of ten for 32 bit integer to ASCII
        conversion.

    This table is used for code to test a 32 bit integer against a power of ten
    for divisionless conversion of an integer to ASCII text.

***************************************/

const uint32_t Burger::g_TensTable32[10] = {
    1, // Table to quickly div by a power 10
    10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

/*! ************************************

    \brief Table used for testing powers of ten for 64 bit integer to ASCII
        conversion.

    This table is used for code to test a 64 bit integer against a power of ten
    for divisionless conversion of an integer to ASCII text.

***************************************/

const uint64_t Burger::g_TensTable64[20] = {
    1ULL, // Table to quickly div by a power 10
    10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL,
    100000000ULL, 1000000000ULL, 10000000000ULL, 100000000000ULL,
    1000000000000ULL, 10000000000000ULL, 100000000000000ULL,
    1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL,
    1000000000000000000ULL, 10000000000000000000ULL};

/*! ************************************

    \brief Convert a BCD value into a decimal integer.

    Given an input value of 0x1234, assume it is Binary Coded Decimal (BCD)
    format and convert it to become the decimal integer 1234.

    BCD's encoding rules dictate that only the number 0-9 are valid. Any use of
    the hex digits A-F are considered illegal.

    \param uInput 32-bit hex value in BCD.
    \return Value in decimal format.

    \note The returned value is undefined if the input is not in BCD format.

***************************************/

uint32_t BURGER_API Burger::BCDToWord(uint32_t uInput) BURGER_NOEXCEPT
{
    uint32_t uOutput = 0; // Assume nothing
    uint32_t uMul = 1;    // Default multiplier
    do {
        // Parse the lowest digit
        uOutput = ((uInput & 0xFU) * uMul) + uOutput;
        uInput >>= 4U;     // Shift out the nibble
        uMul = uMul * 10U; // Adjust the multiplier
    } while (uInput);      // Any more?
    return uOutput;
}

/*! ************************************

    \brief Convert a decimal integer into BCD format.

    Given a decimal in the range of 0-99999999, convert it into that value into
    Binary Coded Decimal (BCD) format. Such that 1234 become 0x1234 and 50331
    become 0x00050331. If the input value is greater than 99999999 it is clamped
    to 99999999.

    \param uInput Integer value in the range of 0 to 99999999.
    \return Input encoded in BCD.

***************************************/

uint32_t BURGER_API Burger::WordToBCD(uint32_t uInput) BURGER_NOEXCEPT
{
    uint32_t uOutput = 99999999;

    if (uInput < 99999999) { // Too large?
        uint32_t uTemp;
        uOutput = 0; // Init output
        // Because divides cost so much, Try to skip them with branches
        if (uInput >= 1000000U) {
            uTemp = uInput / 10000000U;
            uOutput = uTemp << 28U;
            uInput -= uTemp * 10000000U;
            uTemp = uInput / 1000000U;
            uOutput |= uTemp << 24U;
            uInput -= uTemp * 1000000U;
        }
        if (uInput >= 10000U) {
            uTemp = uInput / 100000U;
            uOutput |= uTemp << 20U;
            uInput -= uTemp * 100000U;
            uTemp = uInput / 10000U;
            uOutput |= uTemp << 16U;
            uInput -= uTemp * 10000U;
        }
        if (uInput >= 100U) {
            uTemp = uInput / 1000U;
            uOutput |= uTemp << 12U;
            uInput -= uTemp * 1000U;
            uTemp = uInput / 100U;
            uOutput |= uTemp << 8U;
            uInput -= uTemp * 100U;
        }
        // Just do the last two digits.
        uTemp = uInput / 10U;
        uOutput |= uTemp << 4U;
        uInput -= uTemp * 10U;

        uOutput |= uInput;
    }
    return uOutput;
}

/*! ************************************

    \brief Round up an integer to the nearest power of 2

    Take an arbitrary value and round it up to the nearest power of 2 If the
    input is 0x40000001 to 0x7FFFFFFF, the function returns 0x80000000
    0x80000000-0xFFFFFFFF is an overflow and returns zero. Zero will return zero

    \param uInput Integer value to round up
    \return Rounded up value
    \sa PowerOf2(uint64_t)

***************************************/

#if defined(BURGER_X86) && \
    (defined(BURGER_WATCOM) || \
        (defined(BURGER_FASTCALLENABLED) && !defined(BURGER_CLANG) && \
            !defined(BURGER_GNUC)))

// clang-format off

BURGER_DECLSPECNAKED uint32_t BURGER_API Burger::PowerOf2(uint32_t /* uInput */) BURGER_NOEXCEPT
{
	BURGER_ASM
	{
		// Force 0x4000 to be 0x3fff so in the end
		// it's changed back to 0x4000
#if defined(BURGER_WATCOM)
		// Watcom passes uInput in eax
		push ecx
		lea ecx, [eax - 1] 
		push edx  // Don't destroy edx for Watcom
#else
		sub ecx, 1	// Metrowerks and Visual C passes uInput in ecx
#endif
		xor eax, eax
		// Find the LEFTMOST bit, uInput will have 0x00 to 0x1F
		// Zero will set the zero flag and leave uInput undefined
		bsr ecx, ecx
		setnz al
		// This handles all but the 0 case (0x1 is input)
		// In the zero case, uZero is 0, so the result is zero
		mov edx, eax
		shl eax, cl
		xor edx, 1
		// I'm one bit shy, so shift up one bit and use
		// an add for Intel efficiency
		// and finally, for the zero case, or with 1
		// if the input was 1 (1-1) -> 0, so, uZero^1 = 1+0 = 1
		lea eax, [edx + eax * 2]
#if defined(BURGER_WATCOM)
		pop edx // Restore edx and ecx for Watcom
		pop ecx
#endif
		ret
	}
}

// clang-format on

#elif ( \
    (defined(BURGER_AMD64) || defined(BURGER_ARM)) && defined(BURGER_MSVC)) || \
    (defined(BURGER_INTEL) && (defined(BURGER_GNUC) || defined(BURGER_LLVM)))

uint32_t BURGER_API Burger::PowerOf2(uint32_t uInput) BURGER_NOEXCEPT
{
    // Use the Intel instruction Bit Scan Reverse to
    // speed up the bit search

    // Force 0x4000 to be 0x3fff so in the end
    // it's changed back to 0x4000
    --uInput;

    // Find the LEFTMOST bit, uInput will have 0x00 to 0x1F
    // Zero will set the zero flag and leave uInput undefined
    unsigned long uResult;
    const uint32_t uZero = _BitScanReverse(&uResult, uInput);

    // This handles all but the 0 case (0x1 is input)
    // In the zero case, uZero is 0, so the result is zero
    uResult = uZero << uResult;
    // I'm one bit shy, so shift up one bit and use
    // an add for Intel efficiency
    // and finally, for the zero case, or with 1
    // if the input was 1 (1-1) -> 0, so, uZero^1 = 1+0 = 1
    return static_cast<uint32_t>(uResult + uResult + (uZero ^ 1));
}

// PowerPC version
#elif defined(BURGER_XBOX360) || \
    (defined(BURGER_PPC) && \
        (defined(BURGER_METROWERKS) || defined(BURGER_GNUC) || \
            defined(BURGER_LLVM)))

uint32_t BURGER_API Burger::PowerOf2(uint32_t uInput) BURGER_NOEXCEPT
{
    --uInput;
    // Count leading zeros (Reverse the count)
    uint32_t uReverseCount = 32U - __cntlzw(uInput);
    // Test for 0
    uint32_t uMask = ((0 - uInput) | uInput) >> 31U;
    // Add in 1 in case of uReverseCount == 32
    return (uMask << uReverseCount) + (uMask ^ 1);
}

#elif defined(BURGER_ARM)

uint32_t BURGER_API Burger::PowerOf2(uint32_t uInput) BURGER_NOEXCEPT
{
    --uInput;
    // Count leading zeros (Reverse the count)
    uint32_t uReverseCount = 32U - __builtin_clz(uInput);
    // Test for 0
    uint32_t uMask = ((0 - uInput) | uInput) >> 31U;
    // Add in 1 in case ofuReverseCount == 32
    return (uMask << uReverseCount) + (uMask ^ 1);
}

#else

uint32_t BURGER_API Burger::PowerOf2(uint32_t uInput) BURGER_NOEXCEPT
{
    // To do this, I blend the bits so from the highest will set
    // EVERY bit slot to the right.
    // I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
    // So, by adding one, it will be the next power of 2
    // Since 0 becomes 0xFFFFFFFF at the start, all
    // the or's and shifts do nothing and 0x00 will be
    // returned due to overflow

    --uInput;                // Adjust the input
    uInput |= uInput >> 1U;  // Blend in the odd bits.
    uInput |= uInput >> 2U;  // Now, every 2nd bit
    uInput |= uInput >> 4U;  // Every 4th bit
    uInput |= uInput >> 8U;  // Every 8th bit
    uInput |= uInput >> 16U; // Final pass!
    ++uInput;                // Inc and I have the power of 2
    return uInput;
}

#endif

/*! ************************************

    \brief Round up an integer to the nearest power of 2

    Take an arbitrary value and round it up to the nearest power of 2
    If the input is 0x4000000000000001 to 0x7FFFFFFFFFFFFFFF, the function
    returns 0x8000000000000000 0x8000000000000000-0xFFFFFFFFFFFFFFFF is an
    overflow and returns zero. Zero will return zero

    \param uInput Integer value to round up
    \return Rounded up value
    \sa PowerOf2(uint32_t)

***************************************/

// PowerPC version
#if defined(BURGER_XBOX360) || \
    (defined(BURGER_POWERPC64) && \
        (defined(BURGER_METROWERKS) || defined(BURGER_GNUC) || \
            defined(BURGER_LLVM)))

uint64_t BURGER_API Burger::PowerOf2(uint64_t uInput) BURGER_NOEXCEPT
{
    --uInput;
    // Count leading zeros (Reverse the count)
    uint64_t uReverseCount = 64U - __cntlzd(uInput);
    // Test for 0
    uint64_t uMask = ((0 - uInput) | uInput) >> 63U;
    // Add in 1 in case ofuReverseCount == 64
    return (uMask << uReverseCount) + (uMask ^ 1);
}

#else
uint64_t BURGER_API Burger::PowerOf2(uint64_t uInput) BURGER_NOEXCEPT
{
    // To do this, I blend the bits so from the highest will set
    // EVERY bit slot to the right.
    // I.E. 0x05 becomes 0x07 and 0x43 becomes 0x7F
    // So, by adding one, it will be the next power of 2
    // Since 0 becomes 0xFFFFFFFF at the start, all
    // the or's and shifts do nothing and 0x00 will be
    // returned due to overflow

    --uInput;
    uInput |= uInput >> 1;  // Blend in the odd bits.
    uInput |= uInput >> 2;  // Now, every 2nd bit
    uInput |= uInput >> 4;  // Every 4th bit
    uInput |= uInput >> 8;  // Every 8th bit
    uInput |= uInput >> 16; // Final pass!
    uInput |= uInput >> 32; // Final pass!
    ++uInput;               // Inc and I have the power of 2
    return uInput;
}

#endif

/*! ************************************

    \fn Burger::PowerOf2(T input)
    \brief Round up an integer to the nearest power of 2

    A template to take an arbitrary value and round it up to the nearest power
    of 2. Zero will return zero.

    \note This will intentionally fail on any non-integral or signed data type.

    \tparam T Input data type, tested with is_unsigned.
    \param input Integer value to round up

    \return Rounded up value

    \sa PowerOf2(uint32_t) or PowerOf2(uint64_t)

***************************************/

/*! ************************************

    \fn uint32_t Burger::ToLower(uint32_t uInput)
    \brief Convert a character to lower case

    Using ASCII encoding, check if the value is between 'A' to 'Z' inclusive and
    if true, convert to 'a' to 'z'.

    \param uInput ASCII character to convert to lower case
    \return Character converted to lower case using ASCII encoding
    \sa ToUpper(uint32_t)

***************************************/

/*! ************************************

    \fn uint32_t Burger::ToUpper(uint32_t uInput)
    \brief Convert a character to lower case

    Using ASCII encoding, check if the value is between 'a' to 'z' inclusive and
    if true, convert to 'A' to 'Z'.

    \param uInput ASCII character to convert to upper case
    \return Character converted to upper case using ASCII encoding
    \sa ToLower(uint32_t)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsPointerInvalid(const void *pInput)
    \brief Test if a pointer is invalid.

    On windows platforms, memory pointers cannot point to any location less than
    65536, so any pointer less than 65536 is considered broken. All other
    platforms will only consider \ref NULL as an invalid pointer.

    No attempt is made to check if the memory points to a specific data type of
    if the application has permission to access the memory.

    \param pInput Pointer to test
    \return \ref TRUE if it points to invalid memory, \ref FALSE if it's a good
        pointer.

    \sa Burger::IsPointerValid(const void *pInput)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsPointerValid(const void *pInput)
    \brief Test if a pointer is valid.

    On windows platforms, memory pointers cannot point to any location less than
    65536, so any pointer less than 65536 is considered broken. All other
    platforms will only consider \ref NULL as an invalid pointer.

    No attempt is made to check if the memory points to a specific data type of
    if the application has permission to access the memory.

    \param pInput Pointer to test
    \return \ref TRUE if it points to valid memory, \ref FALSE if it's a bad
        pointer.

    \sa Burger::IsPointerInvalid(const void *pInput)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsStringEmpty(const char *pInput)
    \brief Test if a UTF-8 string pointer points to a \ref NULL string.

    Test if the pointer is in a valid address range and if so, test if the first
    byte is a non zero character. If both tests pass, return \ref TRUE.

    \param pInput Pointer to a UTF-8 string to test
    \return \ref TRUE if it points to valid string, \ref FALSE if it's a bad
        pointer or empty string.

    \sa Burger::IsStringEmpty(const uint16_t *pInput)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsStringEmpty(const uint16_t *pInput)
    \brief Test if a UTF-16 string pointer points to a \ref NULL string.

    Test if the pointer is in a valid address range and if so, test if the first
    byte is a non zero character. If both tests pass, return \ref TRUE.

    \param pInput Pointer to a UTF-16 string to test
    \return \ref TRUE if it points to valid string, \ref FALSE if it's a bad
        pointer or empty string.

    \sa Burger::IsStringEmpty(const char *pInput)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsDigit(char iInput)
    \brief Test if a character is 0-9

    If the character is ASCII 0 through 9, return non zero, otherwise return
    zero.

    \param iInput ASCII character
    \return Non zero if the character was 0 through 9, zero if not.
    \sa IsHex(char), IsWhitespace(char), IsLowercase(char) or IsUppercase(char)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsHex(char iInput)
    \brief Test if a character is 0-9, A-F or a-f

    If the character is ASCII 0 through 9, A through F or a through f, return
    non zero, otherwise return zero.

    \param iInput ASCII character
    \return Non zero if the character was 0 through 9, A-F or a-f, zero if not.
    \sa IsDigit(char), IsWhitespace(char), IsLowercase(char) or
        IsUppercase(char)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsWhitespace(char iInput)
    \brief Test if a character is TAB or SPACE

    If the character is ASCII TAB or SPACE, return non zero, otherwise return
    zero.

    \param iInput ASCII character
    \return Non zero if the character was TAB or SPACE, zero if not.
    \sa IsDigit(char), IsHex(char), IsLowercase(char) or IsUppercase(char)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsSpace(int iInput)
    \brief Test if a character is ASCII white space

    If the character is ASCII SPACE, TAB, LF, VT, FF or CR, return non zero, otherwise return
    zero.

	\note This is the same as isspace() from ctype.h.

    \param iInput ASCII character
    \return Non zero if the character was white space, zero if not.
    \sa IsDigit(char), IsHex(char), IsLowercase(char) or IsUppercase(char)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsLowercase(char iInput)
    \brief Test if a character is a-z

    If the character is ASCII a through z, return non zero, otherwise return
    zero.

    \param iInput ASCII character
    \return Non zero if the character was a through z, zero if not.
    \sa IsDigit(char), IsHex(char), IsWhitespace(char) or IsUppercase(char)

***************************************/

/*! ************************************

    \fn uint_t Burger::IsUppercase(char iInput)
    \brief Test if a character is A-Z

    If the character is ASCII A through Z, return non zero, otherwise return
    zero.

    \param iInput ASCII character
    \return Non zero if the character was A through Z, zero if not.
    \sa IsDigit(char), IsHex(char), IsWhitespace(char) or IsLowercase(char)

***************************************/

/*! ************************************

    \brief Reverse the bits in a byte

    Given a bit width (From 1 through 32), reverse the order of the bits within.

    Since this version is variable width, if uBitLength is equal to 4, the truth
    table would look like this...

    <table border="1"
style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
    <tr><th>Input</th><th>Output</th></tr>
    <tr><td>0x00</td><td>0x00</td></tr>
    <tr><td>0x01</td><td>0x02</td></tr>
    <tr><td>0x02</td><td>0x01</td></tr>
    <tr><td>0x03</td><td>0x03</td></tr>
    </table>

    Whereas if uBitLength is equal to 8, the truth table would look like this...

    <table border="1"
style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
    <tr><th>Input</th><th>Output</th></tr>
    <tr><td>0x00</td><td>0x00</td></tr>
    <tr><td>0x01</td><td>0x80</td></tr>
    <tr><td>0x02</td><td>0x40</td></tr>
    <tr><td>0x03</td><td>0xC0</td></tr>
    </table>

    \note This function will set the unused bits to zero, so if a 3 bit number
    is requested, the upper 29 (32-3) bits will be set to zero.

    \param uInput Value to bit reverse
    \param uBitLength Width of the value to reverse (1 through 32)
    \return uInput with the bits reversed
    \sa BitReverse(uint64_t,uint_t)

***************************************/

// Watcom, Metrowerks and Visual C version (Note, assembly is only allowed for
// 32 bit mode)

#if ((defined(BURGER_WATCOM) || \
         (defined(BURGER_FASTCALLENABLED) && !defined(BURGER_CLANG) && \
             !defined(BURGER_GNUC))) && \
    defined(BURGER_X86)) && \
    !defined(DOXYGEN)

// clang-format off

BURGER_DECLSPECNAKED uint32_t BURGER_API Burger::BitReverse(
	uint32_t /* uInput */, uint_t /* uBitLength */)
{
	BURGER_ASM
	{
#if defined(BURGER_WATCOM)
	push ecx // ecx needs to be saved for Watcom
	mov ecx, eax // Watcom passes in eax
#endif
	xor eax, eax // Initialize the result
BitReverse32:
	shr ecx, 1 // Shift out a bit
	rcl eax, 1 // Shift into the result
	sub edx, 1 // Count down the iterator
	jne BitReverse32
#if defined(BURGER_WATCOM)
	pop ecx // Restore ecx for Watcom
#endif
	ret
	}
}
// clang-format on

#else

uint32_t BURGER_API Burger::BitReverse(uint32_t uInput, uint_t uBitLength)
{
    uint32_t uResult = 0; // Initialize the result
    do {
        uResult = uResult + uResult;
        uResult += (uInput & 1U);
        uInput >>= 1U;
    } while (--uBitLength);
    return uResult;
}
#endif

/*! ************************************

    \brief Reverse the bits in a byte

    Given a bit width (From 1 through 64), reverse the order of the bits within.

    Since this version is variable width, if uBitLength is equal to 4, the truth
    table would look like this...

    <table border="1"
style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
    <tr><th>Input</th><th>Output</th></tr>
    <tr><td>0x00</td><td>0x00</td></tr>
    <tr><td>0x01</td><td>0x02</td></tr>
    <tr><td>0x02</td><td>0x01</td></tr>
    <tr><td>0x03</td><td>0x03</td></tr>
    </table>

    Whereas if uBitLength is equal to 8, the truth table would look like this...

    <table border="1"
style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
    <tr><th>Input</th><th>Output</th></tr>
    <tr><td>0x00</td><td>0x00</td></tr>
    <tr><td>0x01</td><td>0x80</td></tr>
    <tr><td>0x02</td><td>0x40</td></tr>
    <tr><td>0x03</td><td>0xC0</td></tr>
    </table>

    \note This function will set the unused bits to zero, so if	a 3 bit number
        is requested, the upper 61 (64-3) bits will be set to zero.

    \param uInput Value to bit reverse
    \param uBitLength Width of the value to reverse (1 through 64)
    \return uInput with the bits reversed
    \sa BitReverse(uint32_t,uint_t)

***************************************/

uint64_t BURGER_API Burger::BitReverse(uint64_t uInput, uint_t uBitLength)
{
    uint64_t uResult = 0; // Initialize the result
    do {
        uResult = uResult + uResult;
        uResult += (uInput & 1U);
        uInput >>= 1U;
    } while (--uBitLength);
    return uResult;
}

/*! ************************************

    \brief Calculate the number of set bits

    Given a 32 bit integer, count the number of bits set and return the value
    from zero to thirty two.

    \param uInput Integer to count the bits from
    \return 0 to 32
    \sa BitSetCount(uint64_t)

***************************************/

uint_t BURGER_API Burger::BitSetCount(uint32_t uInput)
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

    Given a 64 bit integer, count the number of bits set and return the value
    from zero to sixty four.

    \param uInput Integer to count the bits from
    \return 0 to 64
    \sa BitSetCount(uint32_t)

***************************************/

uint_t BURGER_API Burger::BitSetCount(uint64_t uInput)
{
    // If the CPU is a 64 bit one, do it the fast way
#if defined(BURGER_64BITCPU) && !defined(BURGER_METROWERKS)
    // Use vector adding to count the bits
    // Stage 1, add 16 pairs of 1 bit numbers
    uInput = uInput - ((uInput >> 1) & 0x5555555555555555ULL);
    // Stage 2, add 8 pairs of 2 bit numbers
    uInput = (uInput & 0x3333333333333333ULL) +
        ((uInput >> 2U) & 0x3333333333333333ULL);
    // Stage 3, add 4 pairs of 4 bit numbers and then sum
    // them with a vector multiply in which the upper 8 bits is the count
    // Neat, eh?
    uInput = (((uInput + (uInput >> 4U)) & 0x0F0F0F0F0F0F0F0FULL) *
                 0x0101010101010101ULL) >>
        56U;
    return static_cast<uint_t>(uInput);
#else
    // For 32 bit CPUs, break it into two 32 bit chunks to make the code simpler
    // (Note, the 64 bit code breaks on CodeWarrior in debugging mode due to a
    // broken 64 bit multiply call)
    return BitSetCount(static_cast<uint32_t>(uInput)) +
        BitSetCount(static_cast<uint32_t>(uInput >> 32U));
#endif
}

/*! ************************************

    \brief Convert a "C" string into a Pascal string.

    Copy a "C" string and convert it into a Pascal style string. A byte will
    denote the length, followed by the text itself. There will be no terminating
    zero in the new string.

    The destination and the source pointer can contain the same value. This
    function allows in buffer data conversion.

    \note If the input string is greater than 255 characters in length, it will
        be truncated at 255 characters.

    \note While the input and output strings can be the same value for
    in place conversions, they cannot be pointing to overlapping sections
    of the same buffer. Either the buffers must be independent of
    each other, or they are pointing to the exact same buffer. Overlap
    will cause undefined behavior.

    \param pOutput Pointer to a buffer (max 256 bytes) to receive the new
        string. \ref NULL will page fault.
    \param pInput Pointer to the "C" string to copy. \ref NULL will page fault.

    \sa Burger::PStringToCString(char *,const uint8_t*);

***************************************/

void BURGER_API Burger::CStringToPString(uint8_t* pOutput, const char* pInput)
{
    // Note, empty "C" and Pascal strings are a single 0,
    // so if the "C" string is empty, no action is needed!

    // Prefetch first char
    uint_t uTemp1 = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp1) { // End of string?
        ++pInput;
        uint8_t* pWork = pOutput; // Set the work pointer
        ++pWork;                // Index past the length byte
        uint_t uLength = 0;       // Set the length of the string
        do {
            // Grab the next character to prevent an overwrite if the source
            // and destination buffers are the same

            uint_t uTemp2 = reinterpret_cast<const uint8_t*>(pInput)[0];
            ++pInput;
            pWork[0] = static_cast<uint8_t>(uTemp1); // Save to destination string
            ++pWork;
            if (++uLength == 255) {
                break; // Overflow!
            }
            uTemp1 = uTemp2; // Get source string
        } while (uTemp1);    // Still more?
        // Save the length byte for PString
        pOutput[0] = static_cast<uint8_t>(uLength);
    }
}

/*! ************************************

    \brief Convert a Pascal string into a "C" string.

    Copy a Pascal string and convert it into a "C" style string. A byte will
    denote the length, followed by the text itself. There will be text followed
    by a terminating zero in the new string.

    The destination and the source pointer can contain the same value. This
    function allows in buffer data conversion.

    \note While the input and output strings can be the same value for in place
    conversions, they cannot be pointing to overlapping sections of the same
    buffer. Either the buffers must be independent of each other, or they are
    pointing to the exact same buffer. Overlap will cause undefined behavior.

    \param pOutput Pointer to a buffer (max 256 bytes) to receive the new
        string. \ref NULL will page fault.
    \param pInput Pointer to the Pascal string to copy. \ref NULL will page
        fault.

    \sa Burger::CStringToPString(uint8_t *,const char *);

***************************************/

void BURGER_API Burger::PStringToCString(char* pOutput, const uint8_t* pInput)
{
    uint_t uCount = pInput[0]; // Get the string length
    if (uCount) {
        ++pInput;
        do {
            reinterpret_cast<uint8_t*>(pOutput)[0] = pInput[0]; // Copy a byte
            ++pInput;
            ++pOutput;
        } while (--uCount); // Count down
        // Note, it's here because a zero length
        // Pascal string is by default, null terminated
        pOutput[0] = 0; // Terminating zero
    }
}

/*! ************************************

    \brief Parse a "C" string until a non-white space character is found.

    Skip over any space (32) or tab (9) character and return the pointer to the
    first character that doesn't match.

    \param pInput Pointer to a "C" string to parse. \ref NULL will page fault.
    \return Pointer to the first non-white space character .
    \sa Burger::ParseToDelimiter(const char *pInput)

***************************************/

char* BURGER_API Burger::ParseBeyondWhiteSpace(const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp; // Temp storage
    do {
        uTemp =
            reinterpret_cast<const uint8_t*>(pInput)[0]; // Get a byte of input
        ++pInput;
    } while ((uTemp == 32) || (uTemp == 9)); // Space or TAB?
    return const_cast<char*>(--pInput);      // Return the result pointer
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

char* BURGER_API Burger::ParseToDelimiter(const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp; // Temp storage
    do {
        uTemp =
            reinterpret_cast<const uint8_t*>(pInput)[0]; // Get a byte of input
        ++pInput;
    } while (uTemp && (uTemp != 9) && (uTemp != 10) && (uTemp != 13) &&
        (uTemp != 32));                 // Space or TAB?
    return const_cast<char*>(--pInput); // Return the result pointer
}

/*! ************************************

    \brief Parse a "C" string until a zero or EOL.

    Follow a stream of text input until either a zero is found or an End Of Line
    is found.

    If a zero is found, return a pointer to the ZERO.

    If an EOL is found, return a pointer to the text BEYOND the EOL which is a
    "\n" (10) or a "\r" (13). In the case of a CR/LF combination, found in PC
    style text files, return the pointer beyond the pair.

    \param pInput Pointer to a "C" string to parse. \ref NULL will page fault.
    \return Pointer to the zero, or EOL character.

***************************************/

char* BURGER_API Burger::ParseBeyondEOL(const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp; // Temp storage
    do {
        uTemp =
            reinterpret_cast<const uint8_t*>(pInput)[0]; // Get a byte of input
        if (!uTemp) {                                  // End now?
            break;
        }
        ++pInput;          // Accept the char
        if (uTemp == 13) { // Mac or PC style EOL?
            if (reinterpret_cast<const uint8_t*>(pInput)[0] ==
                10) {     // Followed by a PC LF?
                ++pInput; // Accept the LF as well
            }
            break; // Exit now
        }
    } while (uTemp != 10);            // Unix EOL?
    return const_cast<char*>(pInput); // Return the result pointer
}

/*! ************************************

    \brief Parse a "C" string until a zero or EOL or out of data.

    Follow a stream of text input until either a zero is found, an End Of Line
    is found or data ran out.

    If a zero is found, return a pointer to the ZERO.

    If an EOL is found, return a pointer to the text BEYOND the EOL which is a
    "\n" (10) or a "\r" (13). In the case of a CR/LF combination, found in PC
    style text files, return the pointer beyond the pair.

    \param pInput Pointer to a string to parse. \ref NULL will page fault.
    \param uLength Length of the string buffer.
    \return Pointer to the zero, or EOL character.

***************************************/

char* BURGER_API Burger::ParseBeyondEOL(const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
    uint_t uTemp; // Temp storage
    if (uLength) {
        do {
            uTemp = reinterpret_cast<const uint8_t*>(
                pInput)[0]; // Get a byte of input
            if (!uTemp) {   // End now?
                break;
            }
            ++pInput; // Accept the char
            // Out of data?
            if (!--uLength) {
                break;
            }
            if (uTemp == 13) { // Mac or PC style EOL?
                if (reinterpret_cast<const uint8_t*>(pInput)[0] ==
                    10) {     // Followed by a PC LF?
                    ++pInput; // Accept the LF as well
                }
                break; // Exit now
            }
        } while (uTemp != 10); // Unix EOL?
    }
    return const_cast<char*>(pInput); // Return the result pointer
}

/*! ************************************

    \brief Parse out a quoted string.

    If the first character of the string is a quote "\"" (0x22) or "'", then
    parse out a quote delimited string. This function will parse double quotes
    ("" or '') as a single quote to be inserted into the string. Whichever was
    the first quote character found, that will be the quote character to be used
    for the double quote and end quote parser making the alternate quote treated
    as a normal character.

    If the first character is 0 or whitespace, return pInput to signal an error
    condition.

    If the first character isn't a quote, 0 or whitespace, parse the string
    until another whitespace character is found.

    All tabs (\\t) in a quoted string are converted into spaces.

    All strings terminate if a 0 or EOL (\\r, \\r\\n, \\n) sequence is found. If
    this type of terminator is found, the returned pointer will point to the
    terminating character (Except in the case of \\r\\n, where it will point at
    the \\n character).

    If the output buffer isn't large enough to hold the resulting string,
    truncate the output to fit. The output will always be zero terminated in all
    cases.

    \code
    char output[256];
    const char *pInput = "Hello";
    char *pError = ParseQuotedString(output,sizeof(output),pInput);
    // Determining if output is really an empty string or a parse error
    if (pError==pInput) {
        printf("Nothing parsed, not an empty string");
    }

    \endcode

    \param pOutput Pointer to the output string. \ref NULL is acceptable if
        uOutputSize is zero.
    \param uOutputSize Size of the output string buffer in bytes.
    \param pInput Pointer to a "C" string that has a '"' as the first
        character. \ref NULL will page fault.

    \return Pointer to the character that ended parsing. +1 beyond '"', or at
        CR, at LF or at zero.

***************************************/

char* BURGER_API Burger::ParseQuotedString(
    char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
    uint_t uLetter =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Get the first char
    uint_t uQuote = ' ';

    // Is the first character even allowed to be parsed?
    if ((uLetter != ' ') && uLetter && (uLetter != 10) && (uLetter != 13) &&
        (uLetter != 9)) {
        if (uLetter == '"' || uLetter == '\'') { // Does it start with a quote?
            uQuote = uLetter; // Whichever one it is, use it as the delimiter
        } else {
            --pInput; // Undo the future ++pInput
        }

        uintptr_t uMax = uOutputSize; // Get the size of the buffer to write to
        if (uMax) {
            --uMax; // Leave space for the end zero
        }
        for (;;) {
            ++pInput; // Accept a character
            uLetter = reinterpret_cast<const uint8_t*>(pInput)[0];

            if (uLetter == 9) { // Tabs become spaces
                uLetter = ' ';
            }

            // Did I hit a delimiter?
            if (uLetter == uQuote) {  // Double quote?
                if (uLetter == ' ') { // If it's a space, exit immediately
                    break;
                }
                ++pInput; // Accept the quote
                // Double quote?
                if (reinterpret_cast<const uint8_t*>(pInput)[0] != uQuote) {
                    break; // Nope, exit then
                }
            } else if ((!uLetter) || (uLetter == 10)) { // End of line?
                break;
            } else if (uLetter == 13) {
                if (reinterpret_cast<const uint8_t*>(pInput)[0] == 10) {
                    ++pInput;
                }
                break;
            }
            // If I got here, it means the character is acceptable.
            if (uMax) {                                  // Can it fit?
                pOutput[0] = static_cast<char>(uLetter); // Save resulting ASCII
                ++pOutput;
                --uMax;
            }
        }
    }
    if (uOutputSize) {  // Is there an output buffer?
        pOutput[0] = 0; // Zero terminate
    }
    return const_cast<char*>(pInput); // Return dest buffer pointer
}

/*! ************************************

    \brief Read a stream of text chars until a \ref NULL, LF, CR or CR/LF is
        found.

    Copy from the input stream of text characters until either a zero, a LF
    (Unix), CR/LF (Windows), or CR (Mac) is found. The output buffer will be
    zero terminated and will truncate excess data.

    The value returned is the number of bytes that was processed. This number is
    how many bytes from the original input was used.

    \param pOutput Pointer to the output buffer (Can be \ref NULL if uOutputSize
        is zero).
    \param uOutputSize Size in bytes of the output buffer.
    \param pInput Pointer to the input buffer.
    \param uInputSize Size in bytes of the maximum number of bytes to process.

    \return The number of input bytes processed.

***************************************/

uintptr_t BURGER_API Burger::CopyUpToEOL(
    char* pOutput, uintptr_t uOutputSize, const char* pInput, uintptr_t uInputSize)
{
    const uint8_t* pWork = reinterpret_cast<const uint8_t*>(pInput);
    if (uInputSize) { // Any input data?

        uintptr_t uOutSize = uOutputSize; // Make a copy of the output buffer size

        // Remove 1 byte to ensure space for the terminating zero

        if (uOutSize) {
            --uOutSize;
        }

        // Let's copy the data

        do {
            uint_t Temp = pWork[0]; // Grab a char from the input stream

            if (!Temp) { // Do NOT accept NULL's
                break;
            }
            ++pWork;          // Accept the char
            if (Temp == 10) { // Unix LF?
                break;        // Accept and end
            }

            if (Temp == 13) {          // Look for the CR/LF case
                if (uInputSize >= 2) { // It there data left?

                    // Yank out the LF from the stream in the CR/LF case.

                    if (pWork[0] == 10) {
                        ++pWork; // Accept the char
                    }
                }
                break; // Bye!
            }

            // Save the character to the output

            if (uOutSize) { // Any space left in the output buffer?
                pOutput[0] = static_cast<char>(Temp); // Store the char
                ++pOutput;                            // Accept it
                --uOutSize; // Reduce the output buffer size
            }
        } while (--uInputSize);
    }

    // If the output buffer had even one char, I can write the terminating zero.

    if (uOutputSize) {
        pOutput[0] = 0; // Terminate the output
    }

    // Calculate the number of bytes parsed and exit.
    return static_cast<uint32_t>(reinterpret_cast<const char*>(pWork) - pInput);
}

/*! ************************************

    \brief Remove spaces from the beginning of a string.

    Starting from the beginning of a string, see if the character is a space
    (32). If so, then the character is removed by copying the rest of the string
    up. This is repeated until there are no more spaces at the beginning of the
    string or the string is empty.

    Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes "Foo",
    &quot;Foo&nbsp;&nbsp;&nbsp;&quot; remains &quot;Foo&nbsp;&nbsp;&nbsp;&quot;,
    &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes
    &quot;Foo&nbsp;&nbsp;&nbsp;&quot;.

    \param pInput Pointer to the "C" string to remove beginning spaces from.
        \ref NULL will page fault.

    \sa Burger::StripLeadingWhiteSpace(char *), Burger::StripTrailingSpaces(char
        *), Burger::StripLeading(char *,const char *).

***************************************/

void BURGER_API Burger::StripLeadingSpaces(char* pInput)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if (uTemp == ' ') {       // Is there a leading space?
        char* pTemp = pInput; // Save the starting point of the string
        ++pInput;             // Accept the char
        do {
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Fetch the next char
            ++pInput;
        } while (uTemp == ' '); // Look for the end of the spaces

        if (uTemp) { // End of the string?
            do {
                pTemp[0] = static_cast<char>(
                    uTemp); // Now, copy the string to the beginning of
                ++pTemp;    // the buffer
                uTemp =
                    reinterpret_cast<uint8_t*>(pInput)[0]; // Accept the next char
                ++pInput;
            } while (uTemp); // Did I copy the ending zero?
        }
        // uTemp == 0 here
        pTemp[0] = static_cast<char>(uTemp); // Store the final zero
    }
}

/*! ************************************

    \brief Remove spaces from the end of a string.

    Starting from the end of a string but before the ending zero, see if the
    last character is a space. If so, then the character is removed by zeroing
    it out and the process begins again until the string is empty or a non space
    character is at the end of the "C" string. For speed purposes, only a single
    zero is ever written to the "C" string. Do not assume that all the spaces
    that were removed were actually overwritten with zeros.

    Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; remains
    &quot;&nbsp;&nbsp;&nbsp;Foo&quot;, &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes
    "Foo", &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes
    &quot;&nbsp;&nbsp;&nbsp;Foo&quot;.

    \param pInput Pointer to the "C" string to remove ending spaces from. \ref
        NULL will page fault.

    \sa Burger::StripTrailingWhiteSpace(char *), Burger::StripLeadingSpaces(char
        *), Burger::StripTrailing(char *).

***************************************/

void BURGER_API Burger::StripTrailingSpaces(char* pInput) BURGER_NOEXCEPT
{
    uint_t Temp = reinterpret_cast<uint8_t*>(pInput)[0]; // Get the first character
    if (Temp) {                                      // Is there a string?
        char* pTemp = pInput;                        // Init the zap pointer
        do {
            ++pInput;           // Accept the char
            if (Temp != ' ') {  // Not a space?
                pTemp = pInput; // Last VALID char
            }
            Temp = reinterpret_cast<uint8_t*>(pInput)[0];
        } while (Temp); // All done?
        // Temp == 0 here
        pTemp[0] = static_cast<char>(Temp); // Zap the final char
    }
}

/*! ************************************

    \brief Remove whitespace from the beginning of a string.

    Starting from the beginning of a string, see if the character is a space or
    a tab. If so, then the character is removed by copying the rest of the
    string up. This is repeated until there are no more spaces or tabs at the
    beginning of the string or the string is empty.

    Example: &quot;&nbsp;\\t&nbsp;Foo&quot; becomes "Foo",
    &quot;Foo&nbsp;\\t&nbsp;&quot; remains &quot;Foo&nbsp;\\t&nbsp;&quot;,
    &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes
    &quot;Foo&nbsp;\\t&nbsp;&quot;.

    \param pInput Pointer to the "C" string to remove beginning whitespace from.
        \ref NULL will page fault.

    \sa Burger::StripLeadingSpaces(char *), Burger::StripTrailingSpaces(char *),
        Burger::StripLeading(char *,const char *)

***************************************/

void BURGER_API Burger::StripLeadingWhiteSpace(char* pInput)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if ((uTemp == ' ') || (uTemp == '\t')) { // Is there a leading whitespace?
        char* pTemp = pInput; // Save the starting point of the string
        ++pInput;             // Accept the char
        do {
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Fetch the next char
            ++pInput;
        } while ((uTemp == ' ') ||
            (uTemp == '\t')); // Look for the end of the whitespace

        if (uTemp) { // End of the string?
            do {
                pTemp[0] = static_cast<char>(
                    uTemp); // Now, copy the string to the beginning of
                ++pTemp;    // the buffer
                uTemp =
                    reinterpret_cast<uint8_t *>(pInput)[0]; // Accept the next char
                ++pInput;
            } while (uTemp); // Did I copy the ending zero?
        }
        // Temp == 0 here
        pTemp[0] = static_cast<char>(uTemp); // Store the final zero
    }
}

/*! ************************************

    \brief Remove whitespace from the end of a string

    Starting from the end of a string but before the ending zero, see if the
    last character is a space or tab. If so, then the character is removed by
    zeroing it out and the process begins again until the string is empty or a
    non space or tab character is at the end of the "C" string. For speed
    purposes, only a single zero is ever written to the "C" string. Do not
    assume that all the spaces and tabs that were removed were actually
    overwritten with zeros.

    Example: &quot;&nbsp;\\t&nbsp;Foo&quot; remains
    &quot;&nbsp;\\t&nbsp;Foo&quot;, &quot;Foo&nbsp;\\t&nbsp;&quot; becomes
    "Foo", &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes
    &quot;&nbsp;\\t&nbsp;Foo&quot;.

    \param pInput Pointer to the "C" string to remove ending spaces and tabs
        from. \ref NULL will page fault.

    \sa Burger::StripTrailingSpaces(char *), Burger::StripLeadingSpaces(char *),
        Burger::StripTrailing(char *,const char*)

***************************************/

void BURGER_API Burger::StripTrailingWhiteSpace(char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Get the first character
    if (uTemp) {                                      // Is there a string?
        char* pTemp = pInput;                         // Init the zap pointer
        do {
            ++pInput;                                // Accept the char
            if ((uTemp != ' ') && (uTemp != '\t')) { // Not whitespace?
                pTemp = pInput;                      // Last VALID char
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
        } while (uTemp); // All done?
        // Temp == 0 here
        pTemp[0] = static_cast<char>(uTemp); // Zap the final char
    }
}

/*! ************************************

    \brief Remove spaces from the beginning and end of a string.

    Starting from the beginning of a string, see if the character is a space. If
    so then the character is removed by copying the rest of the string up. This
    is repeated until there are no more spaces at the beginning of the string or
    the string is empty. Then the process is repeated but from the end of the
    string. The resulting string will not have any space characters at the
    beginning or the end.

    Example: &quot;&nbsp;&nbsp;&nbsp;Foo&quot; becomes "Foo",
    &quot;Foo&nbsp;&nbsp;&nbsp;&quot; becomes "Foo",
    &quot;&nbsp;&nbsp;&nbsp;Foo&nbsp;&nbsp;&nbsp;&quot; becomes "Foo".

    \param pInput Pointer to the "C" string to remove beginning and ending
        spaces from. \ref NULL will page fault.

    \sa Burger::StripTrailingSpaces(char *), Burger::StripLeadingSpaces(char *),
        Burger::StripLeadingAndTrailingWhiteSpace(char *).

***************************************/

void BURGER_API Burger::StripLeadingAndTrailingSpaces(char* pInput)
{
    char* pEnd = pInput; // Save the starting point of the string
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    ++pInput;           // Accept the char
    if (uTemp == ' ') { // Is there a leading space?
        do {
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Fetch the next char
            ++pInput;
        } while (uTemp == ' '); // Look for the end of the spaces
    }

    if (uTemp) {            // End of the string?
        char* pTemp = pEnd; // Begin storing here
        do {
            pTemp[0] = static_cast<char>(
                uTemp);         // Now, copy the string to the beginning of
            ++pTemp;            // the buffer
            if (uTemp != ' ') { // Is this a forbidden last char?
                pEnd = pTemp;
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Accept the next char
            ++pInput;
        } while (uTemp); // No more string?
    }
    // Temp == 0 here
    pEnd[0] = static_cast<char>(uTemp); // Store the final zero
}

/*! ************************************

    \brief Remove whitespace from the beginning and end of a string.

    Starting from the beginning of a string, see if the character is whitespace.
    If so then the character is removed by copying the rest of the string up.
    This is repeated until there are no more whitespace at the beginning of the
    string or the string is empty. Then the process is repeated but from the end
    of the string. The resulting string will not have any whitespace characters
    at the beginning or the end.

    Example: &quot;&nbsp;\\t&nbsp;Foo&quot; becomes "Foo",
    &quot;Foo&nbsp;\\t&nbsp;&quot; becomes "Foo",
    &quot;&nbsp;\\t&nbsp;Foo&nbsp;\\t&nbsp;&quot; becomes "Foo".

    \param pInput Pointer to the "C" string to remove beginning and ending
        whitespace from. \ref NULL will page fault.

    \sa Burger::StripTrailingWhiteSpace(char *),
        Burger::StripLeadingWhiteSpace(char *),
        Burger::StripLeadingAndTrailingSpaces(char *)

***************************************/

void BURGER_API Burger::StripLeadingAndTrailingWhiteSpace(char* pInput)
{
    char* pEnd = pInput; // Save the starting point of the string
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    ++pInput;                                // Accept the char
    if ((uTemp == ' ') || (uTemp == '\t')) { // Is there a leading space?
        do {
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Fetch the next char
            ++pInput;
        } while ((uTemp == ' ') ||
            (uTemp == '\t')); // Look for the end of the spaces
    }

    if (uTemp) {            // End of the string?
        char* pTemp = pEnd; // Begin storing here
        do {
            pTemp[0] = static_cast<char>(
                uTemp); // Now, copy the string to the beginning of
            ++pTemp;    // the buffer
            if ((uTemp != ' ') &&
                (uTemp != '\t')) { // Is this a forbidden last char?
                pEnd = pTemp;
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Accept the next char
            ++pInput;
        } while (uTemp); // No more string?
    }
    pEnd[0] = static_cast<char>(uTemp); // Store the final zero
}

/*! ************************************

    \brief Remove all characters that match those in a list

    The Input string is scanned and every character that is in the pList "C"
    string will be removed and compacted. The resulting string consists only of
    characters that are not found in the pList "C" string.

    \note This code is case sensitive.

    \param pInput Pointer to the "C" string to purge. \ref NULL will page fault.
    \param pList Pointer to the "C" string that contains the characters to
        remove. Do not pass a \ref NULL pointer.

    \sa Burger::StripAllButList(char*,const char*),
        Burger::StripLeadingSpaces(char*), Burger::StripTrailingSpaces(char*)

***************************************/

void BURGER_API Burger::StripAllFromList(char* pInput, const char* pList)
{
    // Get a character of the input
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if (uTemp && reinterpret_cast<const uint8_t*>(pList)[0]) { // Any input?
        char* pTemp = pInput; // Destination pointer
        ++pInput;             // Accept the fetched character
        do {
            pTemp[0] = static_cast<char>(uTemp); // Accept the char
            ++pTemp;
            uint_t uTest = reinterpret_cast<const uint8_t*>(pList)[0];
            const uint8_t* pWork = reinterpret_cast<const uint8_t*>(pList);
            do {
                ++pWork;
                if (uTest == uTemp) { // Is it in the list?
                    --pTemp; // Reverse the output pointer to "undo" the
                             // acceptance
                    break;
                }
                uTest = pWork[0]; // Next one in the list.
            } while (uTest);      // Any more?
            uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
            ++pInput;
        } while (uTemp);
        pTemp[0] = 0; // Terminate the string
    }
}

/*! ************************************

    \brief Remove all characters except those in a list

    The Input string is scanned and every character that is not in the pList "C"
    string will be removed and compacted. The resulting string consists only of
    characters that are found in the pList "C" string.

    \note This code is case sensitive.

    \param pInput Pointer to the "C" string to purge. \ref NULL will page fault.
    \param pList Pointer to the "C" string that contains the valid characters to
        allow.

    \sa Burger::StripAllFromList(char *,const char *),
        Burger::StripLeadingSpaces(char *), Burger::StripTrailingSpaces(char *)

***************************************/

void BURGER_API Burger::StripAllButList(char* pInput, const char* pList)
{
    if (reinterpret_cast<uint8_t*>(pInput)[0]) { // Any input
        char* pTemp = pInput;                  // Destination pointer
        if (reinterpret_cast<const uint8_t*>(
                pList)[0]) { // Is there a keep pointer?
            uint_t uTemp =
                reinterpret_cast<uint8_t*>(pInput)[0]; // Grab an input char
            do {
                ++pInput;
                uint_t uTemp2 = reinterpret_cast<const uint8_t*>(pList)[0];
                const uint8_t* pTempList = reinterpret_cast<const uint8_t*>(pList);
                do {
                    ++pTempList;
                    if (uTemp2 == uTemp) {
                        pTemp[0] = static_cast<char>(uTemp); // Accept the char
                        ++pTemp;
                        break;
                    }
                    uTemp2 = pTempList[0];
                } while (uTemp2); // Any more?
                uTemp =
                    reinterpret_cast<uint8_t*>(pInput)[0]; // Grab an input char
            } while (uTemp);
        }
        pTemp[0] = 0; // Terminate the string
    }
}

/*! ************************************

    \brief Remove characters from the end of a string

    Starting from the end of a string but before the ending zero, see if the
    character found is inside the string passed in pList. If so, then the
    character is removed by zeroing it out and the process begins again until
    the string is empty or a character that is not in the list is found.

    \note This code is case sensitive.

    \param pInput Pointer to the "C" string to remove ending characters from. Do
        not pass a \ref NULL pointer.
    \param pList Pointer to the "C" string that contains the characters to
        remove. Do not pass a \ref NULL pointer.

    \sa Burger::StripAllButList(char *,const char *),
        Burger::StripLeadingSpaces(char *) or Burger::StripTrailingSpaces(char
        *)

***************************************/

void BURGER_API Burger::StripTrailing(char* pInput, const char* pList)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) { // Is there a string?
        uint_t uTest = reinterpret_cast<const uint8_t*>(pList)[0];
        if (uTest) {

            char* pEnd = pInput; // Init the end of string pointer
            do {
                ++pInput; // Point to where a zero would be if uTemp is
                          // acceptable
                // Iterate over the test list
                uTest = reinterpret_cast<const uint8_t*>(pList)[0];
                const uint8_t* pListTemp = reinterpret_cast<const uint8_t*>(pList);
                do {
                    if (uTemp == uTest) { // In the list?
                        goto SkipIt;      // Last VALID char
                    }
                    // Next in the list
                    uTest = pListTemp[1];
                    ++pListTemp;
                } while (uTest);
                // Not in the list, mark the new location for terminator
                pEnd = pInput; // Terminate AFTER this character (++pInput is
                               // above)
            SkipIt:
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
            } while (uTemp); // All done?
            pEnd[0] = 0;     // Zap the final char
        }
    }
}

/*! ************************************

    \brief Remove characters from the beginning of a string

    Starting from the beginning of a string, see if the character is in the
    supplied list. If so, then the character is removed by copying the rest of
    the string up. This is repeated until there are no more characters from the
    list at the beginning of the string or the string is empty.

    \note This code is case sensitive.

    \param pInput Pointer to the "C" string to remove beginning characters from.
        Do not pass a \ref NULL pointer.
    \param pList Pointer to the "C" string that contains the characters to
        remove. Do not pass a \ref NULL pointer.

    \sa Burger::StripLeadingWhiteSpace(char *), Burger::StripTrailingSpaces(char
        *), Burger::StripLeading(char *,const char *)

***************************************/

void BURGER_API Burger::StripLeading(char* pInput, const char* pList)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp && reinterpret_cast<const uint8_t*>(pList)[0]) { // Any input
        char* pTemp = pInput; // Destination pointer
        ++pInput;

        // First, iterate over the string until the first character
        // that's not in the list shows up.
        uint_t uTest = reinterpret_cast<const uint8_t*>(pList)[0];
        const uint8_t* pListTemp = reinterpret_cast<const uint8_t*>(pList);
        do {
            if (uTest == uTemp) { // Found one?
                // Step to the next character in the input string
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
                ++pInput;
                if (!uTemp) { // End of string?
                    break;
                }
                pListTemp = reinterpret_cast<const uint8_t*>(
                    pList - 1); // Reset the list pointer
            }
            uTest = pListTemp[1];
            ++pListTemp;
        } while (uTest); // Any more?
        --pInput;
        if (pInput != pTemp) { // Did I remove anything?
            StringCopy(pTemp, pInput);
        }
    }
}

/*! ************************************

    \brief Remove a character from the end of a string if present.

    Parse to the end of the string. If the last character is a match to uRemove,
    it's zeroed out, effectively removing it. This function is a convenience
    routine used to get rid of a trailing ':' or '/' mark from a directory path.

    \param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
    \param uRemove character to test the last char with (Range 0-255).
    \sa Burger::ForceTrailingChar(char *,uint_t)

***************************************/

void BURGER_API Burger::RemoveTrailingChar(char* pInput, uint_t uRemove) BURGER_NOEXCEPT
{
    uintptr_t uLength = StringLength(pInput); // Index to the last char
    if (uLength) {                          // Should I bother?
        if (reinterpret_cast<uint8_t*>(pInput)[uLength - 1] == uRemove) {
            pInput[uLength - 1] = 0;
        }
    }
}

/*! ************************************

    \brief Force a character to be at the end of a string if missing.

    Parse to the end of the string. If the last character is a match to uLast,
    then do nothing. Otherwise, append this single character to the end of the
    string and write a terminating zero after it. This function is a convenience
    routine used to ensure that a trailing ':' or '/' mark is at the end of a
    directory path.

    \note This function may grow the string by a single character. It is the
    caller's responsibility to ensure that the string buffer is large enough to
    accept a string that grows by 1 byte.

    \param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
    \param uLast 8 bit character to test the last char with.
    \sa Burger::RemoveTrailingChar(char *,uint_t)

***************************************/

void BURGER_API Burger::ForceTrailingChar(char* pInput, uint_t uLast) BURGER_NOEXCEPT
{
    uintptr_t uLength = StringLength(pInput); // Index to the last char
    if (!uLength ||                         // Always do it on an empty string
        (reinterpret_cast<uint8_t*>(pInput)[uLength - 1] != uLast)) {
        pInput[uLength] = static_cast<char>(uLast);
        pInput[uLength + 1] = 0;
    }
}

/*! ************************************

    \brief Convert all forward and back slashes to colons.

    Burgerlib uses colons as directory separators. This function will take
    Unix ('\') and Windows ('/') style slashes and convert them into colons.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.

    \sa SlashesToColons(char *,const char *), SlashesToWindowsSlashes(char *) or
        SlashesToLinuxSlashes(char *)

***************************************/

void BURGER_API Burger::SlashesToColons(char* pInput)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Get the first char
    if (uTemp) {                                      // Do I bother?
        do {
            if ((uTemp == '\\') || (uTemp == '/')) { // Is it a slash?
                pInput[0] = ':';                     // It's a colon now
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1]; // Next char
            ++pInput;                                    // Inc the pointer
        } while (uTemp);                                 // More?
    }
}

/*! ************************************

    \brief Convert all forward and back slashes to colons.

    Burgerlib uses colons as directory separators. This function will take
    Unix ('\') and Windows ('/') style slashes and convert them into colons.

    \param pOutput Pointer to a buffer large enough to contain the new string
    \param pInput Pointer to the "C" string to perform the fix up. \ref NULL
        will page fault.

    \sa SlashesToColons(char *), SlashesToWindowsSlashes(char
        *,const char *) or SlashesToLinuxSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::SlashesToColons(char* pOutput, const char* pInput)
{
    uint_t uTemp =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Get the first char
    if (uTemp) {                                   // Do I bother?
        do {
            if ((uTemp == '\\') || (uTemp == '/')) { // Is it a slash?
                uTemp = ':';                         // It's a colon now
            }
            pOutput[0] = static_cast<char>(uTemp); // Store the new character
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1]; // Next char
            ++pInput;    // Inc the pointer
        } while (uTemp); // More?
    }
    pOutput[0] = static_cast<char>(uTemp); // Store the zero terminator
}

/*! ************************************

    \brief Convert all forward slashes ('/') to back slashes ('\').

    Windows uses backslashes as directory separators. This function will take
    Linux style slashes and convert them into Windows slashes.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.

    \sa SlashesToWindowsSlashes(char *,const char *),
        SlashesToColons(char *) or SlashesToLinuxSlashes(char *)

***************************************/

void BURGER_API Burger::SlashesToWindowsSlashes(char* pInput)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == '/') { // Change the slash
                pInput[0] = '\\';
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
}

/*! ************************************

    \brief Convert all forward slashes ('/') to back slashes ('\').

    Windows uses backslashes as directory separators. This function will take
    Unix style slashes and convert them into Windows slashes.

    \param pOutput Pointer to a buffer large enough to hold the converted "C"
        string.
    \param pInput Pointer to the "C" string to perform the fix up on. \ref
        NULL will page fault.

    \sa SlashesToWindowsSlashes(char *), SlashesToColons(char
        *,const char *) or SlashesToLinuxSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::SlashesToWindowsSlashes(
    char* pOutput, const char* pInput)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == '/') { // Change the slash
                uTemp = '\\';
            }
            pOutput[0] = static_cast<char>(uTemp);
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
    pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

    \brief Force the last character of a string to be '\'

    Windows uses backslashes as directory separators. This function will take
    Linux style slashes and convert them into Windows slashes. It will also
    append a '\' character to the end of the string if a slash wasn't already
    there.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.
    \sa EndWithWindowsSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::EndWithWindowsSlashes(char* pInput)
{
    // Get the initial character
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    uint_t uLastChar = 0;
    if (uTemp) {
        // Start the loop
        do {
            if (uTemp == '/') { // Change the Linux slash to windows
                uTemp = '\\';   // Set last char
                pInput[0] = '\\';
            }
            uLastChar = uTemp; // Record the last valid character
            // Get the next character
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }

    // If the last character wasn't a slash, add one
    if (uLastChar != '\\') {
        pInput[0] = '\\';
        pInput[1] = 0;
    }
}

/*! ************************************

    \brief Copy a string and force the last character of a string to be '\'

    Windows uses backslashes as directory separators. This function will take
    Linux style slashes and convert them into Windows slashes. It will also
    append a '\' character to the end of the string if a slash wasn't already
    there.

    \param pOutput Pointer to a buffer large enough to hold the converted "C"
        string.
    \param pInput Pointer to the "C" string to perform the fix up on. \ref
        NULL will page fault.

    \sa EndWithWindowsSlashes(char *)

***************************************/

void BURGER_API Burger::EndWithWindowsSlashes(char* pOutput, const char* pInput)
{
    uint_t uTemp = reinterpret_cast<const uint8_t *>(pInput)[0];
    uint_t uLastChar = 0;
    if (uTemp) {
        do {
            if (uTemp == '/') { // Change the slash
                uTemp = '\\';
            }
            uLastChar = uTemp; // Record the last valid character
            pOutput[0] = static_cast<char>(uTemp);
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }

    // If the last character wasn't a slash, add one
    if (uLastChar != '\\') {
        pOutput[0] = '\\';
        ++pOutput;
    }
    pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

    \brief Convert all back slashes ('\') to forward slashes ('/').

    Linux and MacOSX uses forward slashes as directory separators. This function
    will take Windows slashes and convert them into Linux style slashes.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.
    \sa SlashesToLinuxSlashes(char *,const char *),SlashesToColons(char *) or
        SlashesToWindowsSlashes(char *)

***************************************/

void BURGER_API Burger::SlashesToLinuxSlashes(char* pInput)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == '\\') { // Change the slash
                pInput[0] = '/';
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
}

/*! ************************************

    \brief Convert all back slashes ('\') to forward slashes ('/').

    Linux and MacOSX uses forward slashes as directory separators. This function
    will take Windows slashes and convert them into Unix style slashes.

    \param pOutput Pointer to a buffer large enough to hold the converted "C"
        string.
    \param pInput Pointer to the "C" string to perform the fix up on. \ref
        NULL will page fault.

    \sa SlashesToLinuxSlashes(char *),SlashesToColons(char *,const char *) or
        SlashesToWindowsSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::SlashesToLinuxSlashes(char* pOutput, const char* pInput)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == '\\') { // Change the slash
                uTemp = '/';
            }
            pOutput[0] = static_cast<char>(uTemp);
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
    pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

    \brief Force the last character of a string to be '/'

    Linux uses forward slashes as directory separators. This function will take
    Windows style slashes and convert them into Linux slashes. It will also
    append a '/' character to the end of the string if a slash wasn't already
    there.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.

    \sa EndWithLinuxSlashes(char *,const char *)

***************************************/

void BURGER_API Burger::EndWithLinuxSlashes(char* pInput)
{
    // Get the initial character
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    uint_t uLastChar = 0;
    if (uTemp) {
        // Start the loop
        do {
            if (uTemp == '\\') { // Change the Windows slash to Linux
                uTemp = '/';     // Set last char
                pInput[0] = '/';
            }
            uLastChar = uTemp; // Record the last valid character
            // Get the next character
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }

    // If the last character wasn't a slash, add one
    if (uLastChar != '/') {
        pInput[0] = '/';
        pInput[1] = 0;
    }
}

/*! ************************************

    \brief Copy a string and force the last character of a string to be '/'

    Linux uses forward slashes as directory separators. This function will take
    Windows style slashes and convert them into Linux slashes. It will also
    append a '/' character to the end of the string if a slash wasn't already
    there.

    \param pOutput Pointer to a buffer large enough to hold the converted "C"
        string.
    \param pInput Pointer to the "C" string to perform the fix up on. \ref
        NULL will page fault.

    \sa EndWithLinuxSlashes(char *)

***************************************/

void BURGER_API Burger::EndWithLinuxSlashes(char* pOutput, const char* pInput)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    uint_t uLastChar = 0;
    if (uTemp) {
        do {
            if (uTemp == '\\') { // Change the slash
                uTemp = '/';
            }
            uLastChar = uTemp; // Record the last valid character
            pOutput[0] = static_cast<char>(uTemp);
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }

    // If the last character wasn't a slash, add one
    if (uLastChar != '/') {
        pOutput[0] = '/';
        ++pOutput;
    }
    pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

    \brief Convert characters in a string from one to another

    Convert any character that matches the uFrom value into the uTo value.

    \param pInput Pointer to the "C" string to perform the fix up on. \ref NULL
        will page fault.
    \param uFrom Value to compare with.
    \param uTo Value to replace matching values with.

    \sa Replace(char *,const char *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::Replace(char* pInput, uint_t uFrom, uint_t uTo)
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == uFrom) { // Change the slash
                pInput[0] = static_cast<char>(uTo);
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
}

/*! ************************************

    \brief Convert characters in a string from one to another

    Convert any character that matches the uFrom value into the uTo value.

    \param pOutput Pointer to a buffer large enough to hold the converted "C"
        string.
    \param pInput Pointer to the "C" string to perform the fix up on. \ref
        NULL will page fault.
    \param uFrom Value to compare with.
    \param uTo Value to replace matching values with.

    \sa Replace(char *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::Replace(
    char* pOutput, const char* pInput, uint_t uFrom, uint_t uTo)
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) {
        do {
            if (uTemp == uFrom) { // Test the value
                uTemp = uTo;      // Value to change with
            }
            pOutput[0] = static_cast<char>(uTemp);
            ++pOutput;
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
            ++pInput;
        } while (uTemp); // End of string?
    }
    pOutput[0] = static_cast<char>(uTemp);
}

/*! ************************************

    \brief Get a pointer to the beginning of the file extension.

    A string is scanned until the last period is found. A pointer to the string
    fragment following the period is returned. If no period is found then a
    pointer to the terminating zero is returned. This function is a quick way to
    extract the file extension from a PC style filename.

    Example filenames expected are: "Foo", "ReadMe.txt","ArtFile.gif" or
    "MyData.c".

    These examples will return pointers to: "", "txt", "gif" or "c".

    The pointer returned is a pointer within the string. You do not need to
    release the memory in any way.

    \param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
    \return A pointer to the filename extension of terminating zero.

    \sa Burger::SetFileExtension(char *,const char *).

***************************************/

char* BURGER_API Burger::GetFileExtension(const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) {
		// No string found
        const char* pWork = pInput;
        pInput = nullptr;
        do {
            ++pWork;
			// Hit a period?
            if (uTemp == '.') {
				// Save the place (+1 beyond the '.')
                pInput = pWork;
            }
            uTemp = reinterpret_cast<const uint8_t*>(pWork)[0];
        } while (uTemp);    // End of the string

		// No '.' found?
		if (!pInput) {
			// Point to the terminating zero
            pInput = pWork;
        }
    }

	// Return end of string (Remove the const)
    return const_cast<char*>(pInput);
}

/*! ************************************

    \brief Replace the text after the last period for filename extensions.

    A string is scanned until the last period is found. The text beyond the
    final period is discarded and the string pointed by pNewExtension is
    appended to the Input filename. If no final period is found, then a period
    is appended and then the new extension is added. If NewExtension has a
    period as the first character, it will be ignored to prevent a double period
    from occurring in the final string.

    You must guarantee that the Input buffer has enough space to accommodate the
    new extension. This routine will not check for buffer overruns.

    Examples: "Foo.txt" + "gif" = "Foo.gif", "Foo" + "gif" = "Foo.gif",
    "Foo.txt" + \ref NULL = "Foo", "Foo.bin" + ".txt" = "Foo.txt".

    \param pInput Pointer to the "C" string to scan. \ref NULL will page fault.
        This will be modified with the new file extension. NewExtension =
        Pointer to a "C" string with the extension to apply.

    \param pNewExtension Pointer to a "C" string that represents the new
        extension. A \ref NULL or empty string will have the existing extension
        stripped.

    \sa Burger::GetFileExtension(const char *).

***************************************/

void BURGER_API Burger::SetFileExtension(
    char* pInput, const char* pNewExtension) BURGER_NOEXCEPT
{
    char* pWork =
        GetFileExtension(pInput); // Get pointer to the char after the '.'

    // If pWork[0] == 0, then no extension is present.

    if (reinterpret_cast<uint8_t*>(pWork)[0]) { // End of string? (No extension)
        --pWork;                              // Point to the period
        pWork[0] = 0;                         // Truncate the string
    }

    // At this point, the file extension is stripped and WorkPtr points
    // to the zero at the end of the string.

    if (pNewExtension) { // Don't add a new extension?
        const uint_t uTemp = reinterpret_cast<const uint8_t*>(
            pNewExtension)[0];  // Is there a new extension?
        if (uTemp) {            // Valid string?
            if (uTemp != '.') { // Does the new extension have a period?
                pWork[0] = '.'; // Put the period back manually
                ++pWork;
            }
            StringCopy(pWork, pNewExtension); // Overwrite the extension
        }
    }
}

/*! ************************************

    \fn uintptr_t BURGER_API Burger::StringLength(const char *pInput)
    \brief Perform an ANSI compatible strlen().

    Determine the length of a "C" string in memory. A "C" string is a random
    string of bytes that terminates with a zero.

    This function works by first forcing 32-bit alignment and then grabbing four
    bytes at a time, and uses a pseudo vector operation to test all four of them
    at once.

    Here's how it works. For each byte, perform an operation that will clear the
    high bit if it is not in a suitable range. The two tests are as follows.

    #1) Negate the high bit. This eliminates 0x80-0xFF, because	if the byte was
    0xD5, then it would become 0x55 which has a clear high bit. (0xD5^0x80)&0x80
    == 0. This test is performed by performing a "~" operation in "C".

    #2) Add 0xFF to the byte. This converts 0x01-0x80 into 0x00-0x7F. This range
    of values all have their high bits clear. This test is performed by adding
    the constant 0xFEFEFEFF to the uint32_t.

    Take the output of these two tests and and them together, then and with 0x80
    to test the high bit. If it is set, it must be zero since zero is the only
    value that passes both tests.

    You may ask, why add with 0xFEFEFEFF, when I want to add 0xFF to each byte?
    Why wasn't it 0xFFFFFFFF? Because when I add 0xFF to the bottommost byte, it
    will carry a one to each upper byte. To compensate, I add 0xFE instead of
    0xFF so the carried one is accounted for.

    \param pInput Pointer to "C" string to determine the length
    \return Length in bytes of the "C" string sans the terminating zero.
    \sa Burger::StringCopy(char *,const char *)

***************************************/

#if ((defined(BURGER_WATCOM) || \
         (defined(BURGER_FASTCALLENABLED) && !defined(BURGER_CLANG) && \
             !defined(BURGER_GNUC))) && \
    defined(BURGER_X86)) && \
    !defined(DOXYGEN)

// clang-format off
BURGER_DECLSPECNAKED uintptr_t BURGER_API Burger::StringLength(
	const char* /* pInput */) BURGER_NOEXCEPT
{
	BURGER_ASM
	{

#if defined(BURGER_WATCOM)
	push edx
	push ecx
	push eax		// Save the original pointer
	mov ecx,eax		// Copy the pointer to ecx
#else
	push ecx		// Save the original pointer
	mov eax,ecx		// Copy the pointer to eax
#endif
	and ecx,3		// Already aligned?
	jnz short DoAlign // Nope, preflight (Let's hope data's always aligned)

	// Here's the meat and potatoes
MainLoop:
	mov edx,[eax]		// Fetch 4 bytes
MainLoop2:
	add eax,4			// Inc my pointer
	lea ecx,[edx + 0xFEFEFEFF] // Do the vector addition for 0x01-0x80
	not edx				// Perform the xor test for 0x80-0xFF
	and ecx,edx			// Blend the two tests
	and ecx,0x80808080	// Test the high bits to find any zeros
	jz short MainLoop	// All bytes are non-zero, loop

	// Note to self, I really hate using the bsf instruction
	// since it's 10+3n (386) or 6-42 (486) clocks, but the
	// penalty for mis-predicted branches by testing and exiting is
	// worse.

	// Addendum, P4 and Xeon made mis-predicted branches even
	// worse, at least bsf sucks less now

	// BSF searches for the first set bit by testing
	// from the lowest bit to the highest bit in that
	// order BSF is undefined if ecx == 0, which won't
	// happen in this code

	bsf ecx,ecx		// ecx returns 7,15,23 or 31
	shr ecx,3		// Convert to 0-3
	sub eax,4		// Undo the last +4 prefetch

#if defined(BURGER_WATCOM)
	mov edx,[esp + 8]
	add eax,ecx		// Add 0-3 to get the pointer to the zero byte
	mov ecx,[esp + 4]
	sub eax,[esp]	// Subtract from the original pointer to get the length
	add esp,12
#else
	sub eax,[esp]	// Subtract from the original pointer to get the length
	add esp,4
	add eax,ecx		// Add 0-3 to get the pointer to the zero byte
#endif
	ret

DoAlign:			// Pre-align
	shl ecx,3		// 1,2,3 -> 8,16,24
	or edx,-1		// Create a bit mask
	shl edx,cl		// 0xFFFFFF00, 0xFFFF0000, 0xFF000000
	and eax,0xFFFFFFFC // Align to 32-bits
	not edx			// 0x000000FF, 0x0000FFFF, 0x00FFFFFF
	or edx,[eax]	// Fetch 4 bytes (Some are invalid)
	jmp short MainLoop2
	}
}
// clang-format on

#else

//
// Generic "C" version that's endian neutral
// And it compiles to excellent ARM code
//

uintptr_t BURGER_API Burger::StringLength(const char* pInput) BURGER_NOEXCEPT
{
    uint32_t uSample;  // Needed for the endian neutral version
    uint32_t uAddTemp; // Temp for the vector add test

    const char* pWork = pInput;

    // Get the address and see if it is already aligned
    uintptr_t uCount = reinterpret_cast<uintptr_t>(pInput) & 3;
    if (uCount) { // Nope, perform a phony first fetch
        pWork = pWork -
            uCount;   // Adjust the start pointer to align by 4 (Backwards)
        uCount <<= 3; // 1,2,3 -> 8,16,24
        uSample = reinterpret_cast<const uint32_t*>(
            pWork)[0]; // Get the first longword
        pWork += 4;
        // Make 0xFFFFFF00, 0xFFFF0000 or 0xFF000000 for 1,2,3 (little)
#if defined(BURGER_LITTLEENDIAN)
        uAddTemp = 0xFFFFFFFFU << uCount; // Create mask for unused bytes
#else
        uAddTemp = 0xFFFFFFFFU >> uCount; // Handle reverse endian
#endif
        // Convert to 0x000000FF, 0x0000FFFF or 0x00FFFFFF for 1,2,3 (little
        // endian) For the input, ensure these bytes are non-zero
        uSample |= (~uAddTemp);

        // Let's get to work.
        uAddTemp =
            uSample + 0xFEFEFEFFU;    // Do the vector addition for 0x01-0x80
        uAddTemp &= (~uSample);       // Perform the xor test for 0x80-0xFF
        if (uAddTemp & 0x80808080U) { // All bytes are non-zero, loop
            goto Skip1;               // If TRUE then a transition occurred
        }
    }

    // This is the main loop
    do {
        uSample =
            reinterpret_cast<const uint32_t*>(pWork)[0]; // Fetch the longword
        pWork += 4;                                      // Accept the 4 bytes
        uAddTemp =
            uSample + 0xFEFEFEFFU;       // Do the vector addition for 0x01-0x80
        uAddTemp &= (~uSample);          // Perform the xor test for 0x80-0xFF
    } while (!(uAddTemp & 0x80808080U)); // All bytes are non-zero, loop
Skip1:;
    uCount = static_cast<uintptr_t>(pWork - pInput);
#if defined(BURGER_LITTLEENDIAN)
    if (!(uSample & 0xFF)) { // Was the first byte the zero one?
        return uCount - 4;   // Adjust result and exit
    }
    if (!(uSample & 0xFF00)) { // uint8_t #2 zero?
        return uCount - 3;
    }
    if (!(uSample & 0xFF0000)) { // uint8_t #3 zero?
        return uCount - 2;
    }
#else
    if (!(uSample & 0xFF000000)) {
        return uCount - 4;
    }
    if (!(uSample & 0xFF0000)) {
        return uCount - 3;
    }
    if (!(uSample & 0xFF00)) {
        return uCount - 2;
    }
#endif
    return uCount - 1; // I'll just assume byte #4 is zero
}
#endif

/*! ************************************

    \fn uintptr_t BURGER_API Burger::StringLength(const uint16_t *pInput)
    \brief Perform an ANSI compatible strlen() for UTF16 strings..

    Determine the length of a UTF16 "C" string in memory. A UTF16 "C" string is
    a random string of shorts that terminates with a zero.

    \note While the string L"Foobar" takes 14 bytes of memory to store, this
    function will return 6 to denote the number of values that are present. Due
    to UTF16 encoding, do not assume that this value represents the number of
    visible characters since some encodings take 2 samples instead of one.

    \param pInput Pointer to a UTF16 "C" string to determine the length
    \return Length in values of the UTF16 "C" string sans the terminating zero.
    \sa Burger::StringLength(const char *)

***************************************/

uintptr_t BURGER_API Burger::StringLength(
    const uint16_t* pInput) BURGER_NOEXCEPT
{
    uintptr_t uResult = 0; // Nothing found yet.
    if (pInput) {
        uint_t uTemp = pInput[0];
        ++pInput;
        if (uTemp) { // Is there any string yet?
            do {
                uTemp = pInput[0];
                ++pInput;
                ++uResult; // Add to the count
            } while (uTemp);
        }
    }
    return uResult; // Exit with the count
}

/*! ************************************

    \brief Copy a "C" string

    Given a pointer to a "C" string, copy it to a destination buffer. This is a
    functional equivalent to strcpy().

    \param pOutput Pointer to the buffer to get the copy.
    \param pInput Pointer to the buffer with the "C" to copy from.

    \note This function does not check for buffer overruns on the destination
    buffer. Use with caution.

    \sa Burger::StringCopy(char *,uintptr_t,const char *) or
        Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringCopy(
    char* pOutput, const char* pInput) BURGER_NOEXCEPT
{
    //
    // First, if the two pointers can't both be long word aligned
    // I'm stuck doing a simple byte copy
    //

    if (!((reinterpret_cast<uintptr_t>(pInput) ^
              reinterpret_cast<uintptr_t>(pOutput)) &
            3)) {
        // Okay, they CAN be aligned. See if they are already.

        uintptr_t uAlign = reinterpret_cast<uintptr_t>(pInput) & 3U;
        if (uAlign) { // Not already aligned
            uint8_t bChar1;
            uAlign = 4 - uAlign; // Number of bytes to parse to force alignment
            do {
                bChar1 = reinterpret_cast<const uint8_t*>(pInput)[0];
                ++pInput;
                pOutput[0] = static_cast<char>(bChar1);
                ++pOutput;
                if (!bChar1) { // End of string (Small string)
                    return;    // Leave now
                }
            } while (--uAlign); // Continue until the pointer is 32-bit aligned
        }

        // At this point, both input and output are 32-bit aligned
        // Let's hit the accelerator

        // Grab 32 bits of data
        uint32_t uTemp = reinterpret_cast<const uint32_t*>(pInput)[0];
        uint32_t uTest = uTemp + 0xFEFEFEFFU;
        uTest &= ~uTemp;
        if (!(uTest & 0x80808080U)) {
            do {
                pInput += 4; // Only now, adjust the input pointer
                // Store the last 32-bit value that's known
                // to contain no zeros
                reinterpret_cast<uint32_t*>(pOutput)[0] = uTemp;
                pOutput += 4;
                // Fetch the next 32 bit input
                uTemp = reinterpret_cast<const uint32_t*>(pInput)[0];
                uTest = uTemp + 0xFEFEFEFFU;
                uTest &= ~uTemp;
            } while (!(uTest & 0x80808080U));
        }
        // I found the zero byte. It's in the last
        // fetched uint32_t.
    }

    //
    // This is the byte copy version of StringCopy
    // Used for the slow path and to write the final byte and ensuring I only
    // write a BYTE. No buffer overruns are possible by accidentally writing a
    // uint32_t when I mean to write a uint8_t
    //

    {
        uint8_t bChar2;
        do {
            bChar2 = reinterpret_cast<const uint8_t*>(pInput)[0];
            ++pInput;
            pOutput[0] = static_cast<char>(bChar2);
            ++pOutput;
        } while (bChar2);
    }
}

/*! ************************************

    \brief Copy a "C" string with bounds checking

    Given a pointer to a "C" string, copy it to a destination buffer. If the
    destination buffer isn't big enough for the input string, truncate it.

    \param pOutput Pointer to the buffer to get the copy.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the "C" to copy from.

    \note This function will always zero terminate the output string and perform
    nothing at all if the input size is zero.

    \sa Burger::StringCopy(char *,const char *) or Burger::StringLength(const
        char *)

***************************************/

void BURGER_API Burger::StringCopy(
    char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
    if (uOutputSize) {
        uintptr_t uFinalLength = StringLength(pInput) + 1;
        if (uFinalLength > uOutputSize) {
            uFinalLength = uOutputSize - 1;
            pOutput[uFinalLength] = 0;
        }
        MemoryCopy(pOutput, pInput, uFinalLength);
    }
}

/*! ************************************

    \brief Copy a text buffer into a "C" string with bounds checking

    Given a pointer to a buffer of text, copy it to a destination buffer and
    append it with a trailing zero. If the destination buffer isn't big enough
    for the input string, truncate it.

    \param pOutput Pointer to the buffer to get the copy.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the text to copy from.
    \param uInputSize Size in byte of the text to copy from

    \note This function will always zero terminate the output string and perform
    nothing at all if the output size is zero.

    \sa Burger::StringCopy(char *,const char *) or Burger::StringLength(const
        char *)

***************************************/

void BURGER_API Burger::StringCopy(char* pOutput, uintptr_t uOutputSize,
    const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
    if (uOutputSize) {
        // How much can be copied?
        if (uInputSize >= uOutputSize) {
            uInputSize = uOutputSize - 1;
        }
        // Store the ending zero
        pOutput[uInputSize] = 0;
        MemoryCopy(pOutput, pInput, uInputSize);
    }
}

/*! ************************************

    \brief Copy a 16 bit "C" string

    Given a pointer to a 16 bit "C" string, copy it to a destination buffer.
    This is a functional equivalent to wstrcpy().

    \param pOutput Pointer to the buffer to get the copy.
    \param pInput Pointer to the buffer with the 16 bit "C" to copy from.

    \note This function does not check for buffer overruns on the destination
        buffer. Use with caution.

    \sa Burger::StringCopy(uint16_t *,uintptr_t,const uint16_t *) or
        Burger::StringLength(const uint16_t *)

***************************************/

void BURGER_API Burger::StringCopy(
    uint16_t* pOutput, const uint16_t* pInput) BURGER_NOEXCEPT
{
    uint16_t bChar2;
    do {
        bChar2 = pInput[0];
        ++pInput;
        pOutput[0] = bChar2;
        ++pOutput;
    } while (bChar2);
}

/*! ************************************

    \brief Copy a 16 bit "C" string with bounds checking

    Given a pointer to a 16 bit "C" string, copy it to a destination buffer. If
    the destination buffer isn't big enough for the input string, truncate it.

    \param pOutput Pointer to the buffer to get the copy.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the 16 bit "C" to copy from.

    \note This function will always zero terminate the output string and perform
        nothing at all if the input size is zero.

    \sa Burger::StringCopy(uint16_t *,const uint16_t *) or
        Burger::StringLength(const uint16_t *)

***************************************/

void BURGER_API Burger::StringCopy(
    uint16_t* pOutput, uintptr_t uOutputSize, const uint16_t* pInput) BURGER_NOEXCEPT
{
    if (uOutputSize >= 2 && pOutput) {
        uintptr_t uFinalLength = (StringLength(pInput) * 2) + 2;
        if (uFinalLength > uOutputSize) {
            uFinalLength = uOutputSize - 2;
            pOutput[uFinalLength / 2] = 0;
        }
        MemoryCopy(pOutput, pInput, uFinalLength);
    }
}

/*! ************************************

    \brief Copy a text buffer into a 16 bit "C" string with bounds checking

    Given a pointer to a buffer of text, copy it to a destination buffer and
    append it with a trailing zero. If the destination buffer isn't big enough
    for the input string, truncate it.

    \param pOutput Pointer to the buffer to get the copy.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the text to copy from.
    \param uInputSize Size in byte of the text to copy from

    \note This function will always zero terminate the output string and perform
        nothing at all if the output size is zero.

    \sa Burger::StringCopy(uint16_t *,const uint16_t *) or
        Burger::StringLength(const uint16_t *)

***************************************/

void BURGER_API Burger::StringCopy(uint16_t* pOutput, uintptr_t uOutputSize,
    const uint16_t* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
    if (uOutputSize) {
        // How much can be copied?
        if (uInputSize >= uOutputSize) {
            uInputSize = uOutputSize - 2;
        }
        // Store the ending zero
        pOutput[uInputSize / 2] = 0;
        MemoryCopy(pOutput, pInput, uInputSize);
    }
}

/*! ************************************

    \brief Make a copy of a "C" string.

    The "C" string passed by pInput will be copied into a buffer allocated by
    \ref Burger::Alloc(uintptr_t). The buffer is exactly the same size of the
    string. You must eventually dispose of the string with a call to
    Burger::StringDelete(const char *).

    \param pInput Pointer to the "C" string to copy. A \ref NULL pointer will
        page fault.

    \return A pointer to the copy of the string. Or \ref NULL if a memory error
        occurs.

    \sa Burger::StringDuplicate(const char *,uintptr_t) or
        Burger::StringDelete(const char *).

***************************************/

char* BURGER_API Burger::StringDuplicate(const char* pInput) BURGER_NOEXCEPT
{
    const uintptr_t uLength = StringLength(pInput) + 1; // Get the length
    return static_cast<char*>(
        AllocCopy(pInput, uLength)); // Allocate the memory
}

/*! ************************************

    \brief Make a copy of a "C" string with some padding.

    The "C" string passed by pInput will be copied into a buffer allocated by
    Burger::Alloc(uintptr_t). The buffer is the same size of the string plus the
    padding value. The extra memory is not initialized but the string does
    terminate with a zero. You must eventually dispose of the string with a call
    to Burger::StringDelete(const char *).

    \param pInput Pointer to the "C" string to copy. A null pointer will page
        fault.
    \param uPadding Number of bytes to extend the buffer.

    \return A pointer to the copy of the string. Or \ref NULL if a memory error
        occurred.

    \sa Burger::StringDuplicate(const char *) or Burger::StringDelete(const char
        *)

***************************************/

char* BURGER_API Burger::StringDuplicate(
    const char* pInput, uintptr_t uPadding) BURGER_NOEXCEPT
{
    const uintptr_t uLength = StringLength(pInput) + 1; // Get the length
    char* pResult =
        static_cast<char*>(Alloc(uLength + uPadding)); // Allocate the memory
    if (pResult) {
        MemoryCopy(pResult, pInput, uLength); // Copy the string
    }
    return pResult; // Return the string or NULL on error
}

/*! ************************************

    \brief Delete an allocated string.

    If a string was allocated with Burger::StringDuplicate(const char *) or
    Burger::StringDuplicate(const char *,uintptr_t) then you must dispose of it
    with this function.

    \param pInput Pointer to the "C" string to delete. A \ref NULL pointer will
        do nothing and is okay to pass.

    \sa Burger::StringDuplicate(const char *) or Burger::StringDuplicate(const
        char *,uintptr_t)

***************************************/

void BURGER_API Burger::StringDelete(const char* pInput) BURGER_NOEXCEPT
{
    Free(pInput);
}

/*! ************************************

    \brief Concatenate a "C" string with another "C" string.

    Given a pointer to a "C" string, append it to a destination buffer that
    contains a valid "C" string. No bounds checking is performed.

    This is the equivalent to strcat()

    \param pOutput Pointer to the buffer of a valid "C" string to be appended.
    \param pInput Pointer to the buffer with the "C" to copy from.

    \sa Burger::StringConcatenate(char *,uintptr_t,const char *) or
        Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(
    char* pOutput, const char* pInput) BURGER_NOEXCEPT
{
    // Get the end of the first string
    uintptr_t uLength = StringLength(pOutput);
    // Copy the string to the end of the first one
    StringCopy(pOutput + uLength, pInput);
}

/*! ************************************

    \brief Concatenate a "C" string with another "C" string bounds checked.

    Given a pointer to a "C" string, append it to a destination buffer that
    contains a valid "C" string. If the destination buffer isn't big enough for
    the input string, truncate it.

    \param pOutput Pointer to the buffer of a valid "C" string to be appended.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the "C" to copy from.

    \sa Burger::StringConcatenate(char *,const char *) or
        Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(
    char* pOutput, uintptr_t uOutputSize, const char* pInput) BURGER_NOEXCEPT
{
    // Get the end of the first string
    uintptr_t uLength = StringLength(pOutput);
    // Already out of bounds?
    if (uLength < uOutputSize) {
        // Adjust the output to all the data that won't be touched.
        uOutputSize -= uLength;
        // Copy the rest with bounds checking
        StringCopy(pOutput + uLength, uOutputSize, pInput);
    }
}

/*! ************************************

    \brief Concatenate a "C" string with a string buffer, bounds checked.

    Given a pointer to a string buffer, append it to a destination buffer that
    contains a valid "C" string. If the destination buffer isn't big enough for
    the input string, truncate it.

    \param pOutput Pointer to the buffer of a valid "C" string to be appended.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the string to copy from.
    \param uInputSize Size in bytes of the input buffer

    \sa Burger::StringConcatenate(char *,const char *) or
        Burger::StringLength(const char *)

***************************************/

void BURGER_API Burger::StringConcatenate(char* pOutput, uintptr_t uOutputSize,
    const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
    // Get the end of the first string
    uintptr_t uLength = StringLength(pOutput);
    // Already out of bounds?
    if (uLength < uOutputSize) {
        // Adjust the output to all the data that won't be touched.
        uOutputSize -= uLength;
        // Copy the rest with bounds checking
        StringCopy(pOutput + uLength, uOutputSize, pInput, uInputSize);
    }
}

/*! ************************************

    \brief Concatenate a 16 bit "C" string with another 16 bit "C" string.

    Given a pointer to a 16 bit "C" string, append it to a destination buffer
    that contains a valid 16 bit "C" string. No bounds checking is performed.

    This is the equivalent to wstrcat()

    \param pOutput Pointer to the buffer of a valid 16 bit "C" string to be
        appended.
    \param pInput Pointer to the buffer with the 16 bit "C" to copy from.

    \sa Burger::StringConcatenate(uint16_t *,uintptr_t,const uint16_t *) or
        Burger::StringLength(const uint16_t *)

***************************************/

void BURGER_API Burger::StringConcatenate(
    uint16_t* pOutput, const uint16_t* pInput) BURGER_NOEXCEPT
{
    // Get the end of the first string
    uintptr_t uLength = StringLength(pOutput);
    // Copy the string to the end of the first one
    StringCopy(pOutput + uLength, pInput);
}

/*! ************************************

    \brief Concatenate a 16 bit "C" string with another 16 bit "C" string bounds
        checked.

    Given a pointer to a 16 bit "C" string, append it to a destination buffer
    that contains a valid 16 bit "C" string. If the destination buffer isn't big
    enough for the input string, truncate it.

    \param pOutput Pointer to the buffer of a valid 16 bit "C" string to be
        appended.
    \param uOutputSize Size in bytes of the output buffer
    \param pInput Pointer to the buffer with the 16 bit "C" to copy from.

    \sa Burger::StringConcatenate(uint16_t *,const uint16_t *) or
        Burger::StringLength(const uint16_t *)

***************************************/

void BURGER_API Burger::StringConcatenate(
    uint16_t* pOutput, uintptr_t uOutputSize, const uint16_t* pInput) BURGER_NOEXCEPT
{
    // Get the end of the first string
    uintptr_t uLength = StringLength(pOutput);
    // Already out of bounds?
    if ((uLength * 2) < uOutputSize) {
        // Adjust the output to all the data that won't be touched.
        uOutputSize -= (uLength * 2);
        // Copy the rest with bounds checking
        StringCopy(pOutput + uLength, uOutputSize, pInput);
    }
}

/*! ************************************

    \brief Compare two "C" strings for equality

    Given pointers to two "C" strings, compare them for equality by using a
    subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero.

    \param pInput1 Pointer to the first "C" string to subtract from.
    \param pInput2 Pointer to the second "C" string to subtract with.

    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

    \sa Burger::StringCompare(const char *,const char *,uintptr_t)

***************************************/

int BURGER_API Burger::StringCompare(
    const char* pInput1, const char* pInput2) BURGER_NOEXCEPT
{
    int iTemp;
    uint_t uTemp1;
    do {
        uTemp1 = reinterpret_cast<const uint8_t*>(pInput1)[0]; // First byte
        ++pInput1;
        uint_t uTemp2 = reinterpret_cast<const uint8_t*>(pInput2)[0]; // Second byte
        ++pInput2;
        iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
        if (iTemp) {
            break;
        }
    } while (uTemp1); // End of string? (And match!!)
    return iTemp;     // Perfect match!
}

/*! ************************************

    \brief Compare two "C" strings for equality, case sensitive, length
        delimited

    Given pointers to two "C" strings, compare them for equality by using a
    subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero.

    The function determines equality by hitting a zero first or the maximum
    allowed length.

    \param pInput1 Pointer to the first "C" string to subtract from.
    \param pInput2 Pointer to the second "C" string to subtract with.
    \param uMaxLength Maximum number of bytes to check

    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

    \sa Burger::StringCompare(const char *,const char *)

***************************************/

int BURGER_API Burger::StringCompare(const char* pInput1, const char* pInput2,
    uintptr_t uMaxLength) BURGER_NOEXCEPT
{
    int iTemp = 0;
    uint_t uTemp1;
    if (uMaxLength) {
        do {
            uTemp1 = reinterpret_cast<const uint8_t*>(pInput1)[0]; // First byte
            ++pInput1;
            uint_t uTemp2 =
                reinterpret_cast<const uint8_t*>(pInput2)[0]; // Second byte
            ++pInput2;
            iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
            if (iTemp) {
                break;
            }
        } while (--uMaxLength && uTemp1); // End of string? (And match!!)
    }
    // Return the match!
    return iTemp;
}

/*! ************************************

    \brief Compare two 16 bit "C" strings for equality

    Given pointers to two 16 bit "C" strings, compare them for equality by using
    a subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero.

    \param pInput1 Pointer to the first 16 bit "C" string to subtract from.
    \param pInput2 Pointer to the second 16 bit "C" string to subtract with.

    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

    \sa Burger::StringCompare(const uint16_t *,const uint16_t *,uintptr_t)

***************************************/

int BURGER_API Burger::StringCompare(
    const uint16_t* pInput1, const uint16_t* pInput2) BURGER_NOEXCEPT
{
    int iTemp;
    uint_t uTemp1;
    do {
        uTemp1 = pInput1[0]; // First byte
        ++pInput1;
        uint_t uTemp2 = pInput2[0]; // Second byte
        ++pInput2;
        iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
        if (iTemp) {
            break;
        }
    } while (uTemp1); // End of string? (And match!!)
    return iTemp;     // Perfect match!
}

/*! ************************************

    \brief Compare two 16 bit "C" strings for equality, case sensitive, length
        delimited

    Given pointers to two 16 bit "C" strings, compare them for equality by using
    a subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero.

    The function determines equality by hitting a zero first or the maximum
    allowed length.

    \param pInput1 Pointer to the first 16 bit "C" string to subtract from.
    \param pInput2 Pointer to the second 16 bit "C" string to subtract with.
    \param uMaxLength Maximum number of bytes to check
    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

    \sa Burger::StringCompare(const uint16_t *,const uint16_t *)

***************************************/

int BURGER_API Burger::StringCompare(const uint16_t* pInput1,
    const uint16_t* pInput2, uintptr_t uMaxLength) BURGER_NOEXCEPT
{
    int iTemp = 0;
    uint_t uTemp1;
    uMaxLength <<= 1;
    if (uMaxLength) {
        do {
            uTemp1 = pInput1[0]; // First byte
            ++pInput1;
            uint_t uTemp2 = pInput2[0]; // Second byte
            ++pInput2;
            iTemp = static_cast<int>(uTemp1 - uTemp2); // Compare
            if (iTemp) {
                break;
            }
        } while (--uMaxLength && uTemp1); // End of string? (And match!!)
    }
    // Return the match!
    return iTemp;
}

/*! ************************************

    \brief Compare two "C" strings for equality, case insensitive

    Given pointers to two "C" strings, compare them for equality by using a
    subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero. If the characters in the string are
    upper case, convert them to ASCII lowercase before the subtraction.

    \param pInput1 Pointer to the first "C" string to subtract from.
    \param pInput2 Pointer to the second "C" string to subtract with.

    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

    \sa Burger::StringCompare(const char *,const char *),
        Burger::StringCaseCompare(const char *,const char *,uintptr_t)

***************************************/

int BURGER_API Burger::StringCaseCompare(
    const char* pInput1, const char* pInput2) BURGER_NOEXCEPT
{
    int iTemp;
    uint_t uTemp1;
    do {
        uTemp1 = reinterpret_cast<const uint8_t*>(pInput1)[0]; // First byte
        ++pInput1;
        uint_t uTemp2 =
            reinterpret_cast<const uint8_t*>(pInput2)[0]; // Second byte
        ++pInput2;
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
    } while (uTemp1); // End of string? (And match!!)
    return iTemp;     // Perfect match!
}

/*! ************************************

    \brief Compare two "C" strings for equality, case insensitive, length
        delimited

    Given pointers to two "C" strings, compare them for equality by using a
    subtraction of the first buffer to the second. If the subtraction's result
    is non-zero, sign extend the result and return it immediately. If both
    buffers are identical, return zero. If the characters in the string are
    upper case, convert them to ASCII lowercase before the subtraction.

    The function determines equality by hitting a zero first or the maximum
    allowed length.

    \param pInput1 Pointer to the first "C" string to subtract from.
    \param pInput2 Pointer to the second "C" string to subtract with.
    \param uMaxLength Maximum number of bytes to check

    \return Zero if identical, <0 if pInput1<pInput2 and >0 if pInput1>pInput2

***************************************/

int BURGER_API Burger::StringCaseCompare(const char* pInput1,
    const char* pInput2, uintptr_t uMaxLength) BURGER_NOEXCEPT
{
    int iTemp = 0;
    uint_t uTemp1;
    if (uMaxLength) {
        do {
            uTemp1 = reinterpret_cast<const uint8_t*>(pInput1)[0]; // First byte
            ++pInput1;
            uint_t uTemp2 =
                reinterpret_cast<const uint8_t*>(pInput2)[0]; // Second byte
            ++pInput2;
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
        } while (--uMaxLength && uTemp1); // End of string? (And match!!)
    }
    return iTemp; // Perfect match!
}

/*! ************************************

    \brief Perform a string comparison using the wild card system.

    An asterisk "*" denotes that the input is skipped until the character
    following is found, then the compare continues. If a "*" is the final
    character, then the input string is considered a match.

    A question mark "?" denotes that a single, non-zero character must occupy
    this location.

    Examples for wild cards:
    *.* = All strings that have a single period anywhere in them.
    * = All strings match.
    *.??? = All strings that have a exactly three characters at the end after a
        period.

    \note Comparisons are case insensitive.

    \param pInput Pointer to a "C" string to compare against. \ref NULL will
        page fault.
    \param pWildcard Pointer to a "C" string that has wild card information.
        \ref NULL will page fault.

    \return \ref FALSE if the string matches according to the wild card rules,
        \ref TRUE if not.

    \sa HasWildcard(const char*)

***************************************/

uint_t BURGER_API Burger::Wildcardcmp(
    const char* pInput, const char* pWildcard) BURGER_NOEXCEPT
{
    uint_t uTemp;
    uint_t uResult = TRUE;
    do {
        // Main entry, assume no previous state
        // Grab an input char
        uTemp = ISOLatin1::UpperCaseTable[reinterpret_cast<const uint8_t*>(
            pInput)[0]];
        // Get a wildcard char
        uint_t uWild = ISOLatin1::UpperCaseTable[reinterpret_cast<const uint8_t*>(
            pWildcard)[0]];

        // Is the wildcard a "?"
        // If so, then I'm expecting a single, non-zero character

        if (uWild == '?') { // Single wildcard
            // If uTemp is zero, exit with TRUE
            ++pInput;
            ++pWildcard;
            continue;
        }

        // The magic "*" wildcard character?

        if (uWild == '*') {
            ++pWildcard;
            uWild = reinterpret_cast<const uint8_t*>(pWildcard)[0];
            if (!uWild) { // Was the '*' at the end of the wildcard string?
                uResult = FALSE;
                break; // Assume it's a full match.
            }
            // If there is string remaining?
            if (uTemp) {
                // Recursively call myself until a match of the pattern is found
                // or I give up
                uWild = ISOLatin1::UpperCaseTable[uWild]; // Upper case
                do {
                    if (uTemp ==
                        uWild) { // Follow the input until the marker is found
                        uResult = Wildcardcmp(pInput, pWildcard);
                        if (!uResult) {
                            return FALSE;
                        }
                    }
                    ++pInput;
                    uTemp = ISOLatin1::UpperCaseTable
                        [reinterpret_cast<const uint8_t*>(pInput)[0]];
                } while (uTemp);
            }
        } else {
            // Simple character test
            ++pInput;
            ++pWildcard;
            if (uTemp != uWild) { // No match?
                break;            // Exit with TRUE
            }
            // Match!
            if (!uTemp) {
                uResult = FALSE; // W00t!
            }
        }
    } while (uTemp); // Keep going?
    return uResult;  // Return the answer
}

/*! ************************************

    \brief Return \ref TRUE if the string contains a wild card character

    To determine if a string contains a '*' or a '?' wild card character, call
    this function and it will return \ref TRUE if so.

    \param pInput Pointer to a "C" string to test. \ref NULL will return \ref
        FALSE

    \return \ref TRUE if the string contains wild card characters, \ref FALSE if
        not.

    \sa Wildcardcmp(const char *,const char *)

***************************************/

uint_t BURGER_API Burger::HasWildcard(const char* pInput) BURGER_NOEXCEPT
{
    // Assume not a wild card
    uint_t uResult = FALSE;
    if (pInput) {
        uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
        if (uTemp) {
            ++pInput;
            do {
                // Wild card character
                if ((uTemp == '?') || (uTemp == '*')) {
                    uResult = TRUE;
                    break;
                }
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
                ++pInput;
            } while (uTemp);
        }
    }
    return uResult;
}

/*! ************************************

    \brief Convert a string to upper case

    Convert all 'a'-'z' characters to 'A'-'Z'. No localization is performed and
    no other characters are altered.

    \param pInput Pointer to "C" string to modify.

    \sa Burger::StringUppercase(char *,const char *) and
        Burger::StringLowercase(char *)

***************************************/

void BURGER_API Burger::StringUppercase(char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Prefetch first char
    if (uTemp) {                                      // End of string?
        do {
            if ((uTemp - 'a') < 26) { // Is it 'a' to 'z' inclusive?
                uTemp = uTemp - 32;
                pInput[0] = static_cast<char>(uTemp); // Convert to upper case
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1]; // Get the next char
            ++pInput;
        } while (uTemp); // Still more?
    }
}

/*! ************************************

    \brief Convert a string to upper case into a new buffer

    Copy a string and convert all 'a'-'z' characters to 'A'-'Z'. No localization
    is performed and no other characters are altered.

    \param pOutput Pointer to a buffer large enough to hold the input "C"
        string.
    \param pInput Pointer to "C" string to modify.

    \sa Burger::StringUppercase(char *) and Burger::StringLowercase(char *,const
        char *)

***************************************/

void BURGER_API Burger::StringUppercase(char* pOutput, const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Prefetch first char
    if (uTemp) {                                   // End of string?
        do {
            if ((uTemp - 'a') < 26) { // Is it 'a' to 'z' inclusive?
                uTemp = uTemp - 32;   // Convert to upper case
            }
            pOutput[0] = static_cast<char>(uTemp); // Store output
            ++pOutput;
            uTemp =
                reinterpret_cast<const uint8_t*>(pInput)[1]; // Get the next char
            ++pInput;
        } while (uTemp); // Still more?
    }
    pOutput[0] = static_cast<char>(uTemp); // Store zero terminator
}

/*! ************************************

    \brief Convert a string to lower case

    Convert all 'A'-'Z' characters to 'a'-'z'. No localization is performed and
    no other characters are altered.

    \param pInput Pointer to "C" string to modify.

    \sa Burger::StringLowercase(char *,const char *) and
        Burger::StringUppercase(char *)

***************************************/

void BURGER_API Burger::StringLowercase(char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp = reinterpret_cast<uint8_t*>(pInput)[0]; // Prefetch first char
    if (uTemp) {                                      // End of string?
        do {
            if ((uTemp - 'A') < 26) {
                uTemp = uTemp + 32;
                pInput[0] = static_cast<char>(uTemp); // Convert to lower case
            }
            uTemp = reinterpret_cast<uint8_t*>(pInput)[1]; // Get the next char
            ++pInput;
        } while (uTemp); // Still more?
    }
}

/*! ************************************

    \brief Convert a string to lower case into a new buffer

    Copy a string and convert all 'A'-'Z' characters to 'a'-'z'. No localization
    is performed and no other characters are altered.

    \param pOutput Pointer to a buffer large enough to hold the input "C"
        string.
    \param pInput Pointer to "C" string to modify.

    \sa Burger::StringLowercase(char *) and Burger::StringUppercase(char *,const
        char *)

***************************************/

void BURGER_API Burger::StringLowercase(char* pOutput, const char* pInput) BURGER_NOEXCEPT
{
    uint_t uTemp =
        reinterpret_cast<const uint8_t*>(pInput)[0]; // Prefetch first char
    if (uTemp) {                                   // End of string?
        do {
            if ((uTemp - 'A') < 26) { // Is it 'A' to 'Z' inclusive?
                uTemp = uTemp + 32;   // Convert to lower case
            }
            pOutput[0] = static_cast<char>(uTemp); // Store output
            ++pOutput;
            uTemp =
                reinterpret_cast<const uint8_t*>(pInput)[1]; // Get the next char
            ++pInput;
        } while (uTemp); // Still more?
    }
    pOutput[0] = static_cast<char>(uTemp); // Store zero terminator
}

/*! ************************************

    \brief Scan for the first occurrence of a specific character

    Returns a pointer to the first occurrence of the character iChar in the
    string pInput or \ref NULL if the character was not found

    \param pInput Pointer to a "C" string to scan
    \param iChar Character to scan for

    \return Pointer inside the input string to the first matching character or
        \ref NULL if not found

    \sa Burger::StringCharacter(const uint16_t *,uint_t) and
        Burger::StringCharacterReverse(const char *,int)

***************************************/

char* BURGER_API Burger::StringCharacter(
    const char* pInput, int iChar) BURGER_NOEXCEPT
{
    // Valid pointer?
    if (pInput) {
        // Get the first character
        uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
        // Valid?
        if (uTemp) {
            do {
                // Perform the test
                if (static_cast<uint_t>(iChar) == uTemp) {
                    // Found the first one!
                    return const_cast<char*>(pInput);
                }
                // Get the next character
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
                // Increment afterward to avoid Address Generation Interlock
                ++pInput;
                // Continue?
            } while (uTemp);
        }
    }
    // Not found
    return nullptr;
}

/*! ************************************

    \brief Scan for the first occurrence of a specific character

    Returns a pointer to the first occurrence of the character uChar in the
    string pInput or \ref NULL if the character was not found

    \param pInput Pointer to a "C" string to scan
    \param uChar Character to scan for

    \return Pointer inside the input string to the first matching character or
        \ref NULL if not found

    \sa Burger::StringCharacter(const char *,int) and
        Burger::StringCharacterReverse(const uint16_t *,uint_t)

***************************************/

uint16_t* BURGER_API Burger::StringCharacter(
    const uint16_t* pInput, uint_t uChar) BURGER_NOEXCEPT
{
    // Valid pointer?
    if (pInput) {
        // Get the first character
        uint_t uTemp = pInput[0];
        // Valid?
        if (uTemp) {
            do {
                // Perform the test
                if (uChar == uTemp) {
                    // Found the first one!
                    return const_cast<uint16_t*>(pInput);
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
    return nullptr;
}

/*! ************************************

    \brief Scan for the last occurrence of a specific character

    Returns a pointer to the last occurrence of the character iChar in the
    string pInput or \ref NULL if the character was not found

    \param pInput Pointer to a "C" string to scan
    \param iChar Character to scan for

    \return Pointer inside the input string to the last matching character or
        \ref NULL if not found

    \sa Burger::StringCharacterReverse(const uint16_t *,uint_t),
        Burger::MemoryCharacterReverse(const char *,uintptr_t,int) and
        Burger::StringCharacter(const char *,int)

***************************************/

char* BURGER_API Burger::StringCharacterReverse(
    const char* pInput, int iChar) BURGER_NOEXCEPT
{
    // Assume it's not found
    char* pFound = nullptr;
    // Valid pointer?
    if (pInput) {
        // Get the first character
        uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
        // Valid?
        if (uTemp) {
            do {
                // Perform the test
                if (static_cast<uint_t>(iChar) == uTemp) {
                    // Found another one!
                    pFound = const_cast<char*>(pInput);
                }
                // Get the next character
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[1];
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

    Returns a pointer to the last occurrence of the character uChar in the
    string pInput or \ref NULL if the character was not found

    \param pInput Pointer to a "C" string to scan
    \param uChar Character to scan for

    \return Pointer inside the input string to the last matching character or
        \ref NULL if not found

    \sa Burger::StringCharacterReverse(const char *,int),
        Burger::MemoryCharacterReverse(const uint16_t *,uintptr_t,uint_t) and
        Burger::StringCharacter(const uint16_t *,uint_t)

***************************************/

uint16_t* BURGER_API Burger::StringCharacterReverse(
    const uint16_t* pInput, uint_t uChar) BURGER_NOEXCEPT
{
    // Assume it's not found
    uint16_t* pFound = nullptr;
    // Valid pointer?
    if (pInput) {
        // Get the first character
        uint_t uTemp = pInput[0];
        // Valid?
        if (uTemp) {
            do {
                // Perform the test
                if (uChar == uTemp) {
                    // Found another one!
                    pFound = const_cast<uint16_t*>(pInput);
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

    \brief Locate the offset of the first non-delimiter in a string

    Calculate the length of a span in pInput, which consists entirely of
    characters in pDelimiters. If no delimiters are found, the returned value is
    zero.

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the list of delimiters to look for

    \return Index of the first non-delimiter found, or zero in no delimiter was
        found.

    \sa Burger::StringStopAt(const uint16_t *,const uint16_t *)

***************************************/

uintptr_t BURGER_API Burger::StringSkipOver(
    const char* pInput, const char* pDelimiters) BURGER_NOEXCEPT
{
    // Result value
    uintptr_t uResult;
    if (!pInput || !pDelimiters || !pDelimiters[0]) {
        // If there are no delimiters, return length of the string immediately
        uResult = 0;
    } else {

        // Scan the string
        const char* pWork = pInput;
        uint_t uTemp = pWork[0];
        if (uTemp) {
            do {
                // Scan the character if it's in the delimiter list
                const char* pWorkLimit = pDelimiters;
                uint_t uDelimiter = pWorkLimit[0];
                do {
                    // Match a delimiter?
                    if (uDelimiter == uTemp) {
                        break;
                    }
                    ++pWorkLimit;
                    // End of the delimiter list?
                    uDelimiter = pWorkLimit[0];
                } while (uDelimiter);

                // Exit if no delimiter was found
                if (!uDelimiter) {
                    break;
                }

                ++pWork;
                uTemp = pWork[0];
            } while (uTemp);
        }
        // Use the offset
        uResult = static_cast<uintptr_t>(pWork - pInput);
    }
    // Return the offset
    return uResult;
}

/*! ************************************

    \brief Locate the offset of the first non-delimiter in a string

    Calculate the length of a span in pInput, which consists entirely of
    characters in pDelimiters. If no delimiters are found, the returned value is
    zero.

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the list of delimiters to look for

    \return Index of the first non-delimiter found, or zero in no delimiter was
        found.

    \sa Burger::StringSkipOver(const char *,const char *)

***************************************/

uintptr_t BURGER_API Burger::StringSkipOver(
    const uint16_t* pInput, const uint16_t* pDelimiters) BURGER_NOEXCEPT
{
    // Result value
    uintptr_t uResult;
    if (!pInput || !pDelimiters || !pDelimiters[0]) {
        // If there are no delimiters, return zero immediately
        uResult = 0;
    } else {

        // Scan the string
        const uint16_t* pWork = pInput;
        uint_t uTemp = pWork[0];
        if (uTemp) {
            do {
                // Scan the character if it's in the delimiter list
                const uint16_t* pWorkLimit = pDelimiters;
                uint_t uDelimiter = pWorkLimit[0];
                do {
                    // Match a delimiter?
                    if (uDelimiter == uTemp) {
                        break;
                    }
                    ++pWorkLimit;
                    // End of the delimiter list?
                    uDelimiter = pWorkLimit[0];
                } while (uDelimiter);

                // Exit if no delimiter was found
                if (!uDelimiter) {
                    break;
                }

                ++pWork;
                uTemp = pWork[0];
            } while (uTemp);
        }
        // Use the offset
        uResult = static_cast<uintptr_t>(pWork - pInput);
    }
    // Return the offset
    return uResult;
}

/*! ************************************

    \brief Locate the offset of the first delimiter in a string

    Scan pInput until a delimiter is found. Return the offset from the beginning
    of the string where the delimiter was located. If any of the delimiters are
    not found, the length of the input string is returned.

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the list of delimiters to look for

    \return Index of the first delimiter found, or length of the string if no
        delimiter found.

    \sa Burger::StringStopAt(const uint16_t *,const uint16_t *)

***************************************/

uintptr_t BURGER_API Burger::StringStopAt(
    const char* pInput, const char* pDelimiters) BURGER_NOEXCEPT
{
    // Result value
    uintptr_t uResult;
    if (!pInput) {
        // No input, return nothing
        uResult = 0;
    } else if (!pDelimiters || !pDelimiters[0]) {
        // If there are no delimiters, return length of the string immediately
        uResult = StringLength(pInput);
    } else {

        // Scan the string
        const char* pWork = pInput;
        uint_t uTemp = pWork[0];
        if (uTemp) {
            do {
                // Scan the character if it's in the delimiter list
                const char* pWorkLimit = pDelimiters;
                uint_t uDelimiter = pWorkLimit[0];
                do {
                    // Match a delimiter?
                    if (uDelimiter == uTemp) {
                        break;
                    }
                    ++pWorkLimit;
                    // End of the delimiter list?
                    uDelimiter = pWorkLimit[0];
                } while (uDelimiter);

                // Exit if delimiter was found
                if (uDelimiter) {
                    break;
                }

                ++pWork;
                uTemp = pWork[0];
            } while (uTemp);
        }
        // Use the offset
        uResult = static_cast<uintptr_t>(pWork - pInput);
    }
    // Return the offset
    return uResult;
}

/*! ************************************

    \brief Locate the offset of a delimiter in a string

    Scan pInput until a delimiter is found. Return the offset from the beginning
    of the string where the delimiter was located. If any of the delimiters are
    not found, the length of the input string is returned.

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the list of delimiters to look for

    \return Index of the first delimiter found, or length of the string if no
        delimiter found.

    \sa Burger::StringStopAt(const char *,const char *)

***************************************/

uintptr_t BURGER_API Burger::StringStopAt(
    const uint16_t* pInput, const uint16_t* pDelimiters) BURGER_NOEXCEPT
{
    // Result value
    uintptr_t uResult;
    if (!pInput) {
        // No input, return nothing
        uResult = 0;
    } else if (!pDelimiters || !pDelimiters[0]) {
        // If there are no delimiters, return length of the string immediately
        uResult = StringLength(pInput);
    } else {

        // Scan the string
        const uint16_t* pWork = pInput;
        uint_t uTemp = pWork[0];
        if (uTemp) {
            do {
                // Scan the character if it's in the delimiter list
                const uint16_t* pWorkLimit = pDelimiters;
                uint_t uDelimiter = pWorkLimit[0];
                do {
                    // Match a delimiter?
                    if (uDelimiter == uTemp) {
                        break;
                    }
                    ++pWorkLimit;
                    // End of the delimiter list?
                    uDelimiter = pWorkLimit[0];
                } while (uDelimiter);

                // Exit if delimiter was found
                if (uDelimiter) {
                    break;
                }

                ++pWork;
                uTemp = pWork[0];
            } while (uTemp);
        }
        // Use the offset
        uResult = static_cast<uintptr_t>(pWork - pInput);
    }
    // Return the offset
    return uResult;
}

/*! ************************************

    \brief Locate a substring

    Returns a pointer to the first occurrence of pTest in pInput, or a \ref NULL
    pointer if pTest is not part of pInput.

    The matching process does not include the terminating null-characters, but
    it stops there.

    \param pInput Pointer to the string to scan
    \param pTest Pointer to the substring to look for
    \return \ref NULL if substring was not found or the pointer to the first
        matching string

    \sa Burger::StringCaseString(const char *,const char *)

***************************************/

char* BURGER_API Burger::StringString(const char* pInput, const char* pTest)
{
    // Get the first character
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) { // Do I even bother?
        do {
            uintptr_t i = 0; // Init the index
            uint_t uTemp2;
            do {
                // Get the first char of the test string
                uTemp2 = reinterpret_cast<const uint8_t*>(pTest)[i];
                if (!uTemp2) {                        // Match?
                    return const_cast<char*>(pInput); // I matched here!
                }
                // Get the source string
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[i];
                ++i;                   // Ack the char
            } while (uTemp == uTemp2); // Match?
            ++pInput;                  // Next main string char
            // Next entry
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
            // Source string still ok?
        } while (uTemp);
    }
    // No string match
    return nullptr;
}

/*! ************************************

    \brief Locate a substring (UTF-16 version)

    Returns a pointer to the first occurrence of pTest in pInput, or a \ref NULL
    pointer if pTest is not part of pInput.

    The matching process does not include the terminating null-characters, but
    it stops there.

    \param pInput Pointer to the string to scan
    \param pTest Pointer to the substring to look for

    \return \ref NULL if substring was not found or the pointer to the first
        matching string

    \sa Burger::StringCaseString(const uint16_t *,const uint16_t *)

***************************************/

uint16_t* BURGER_API Burger::StringString(
    const uint16_t* pInput, const uint16_t* pTest)
{
    // Get the first character
    uint_t uTemp = pInput[0];
    if (uTemp) { // Do I even bother?
        do {
            uintptr_t i = 0; // Init the index
            uint_t uTemp2;
            do {
                // Get the first char of the test string
                uTemp2 = pTest[i];
                if (!uTemp2) {                          // Match?
                    return const_cast<uint16_t*>(pInput); // I matched here!
                }
                // Get the source string
                uTemp = pInput[i];
                ++i;                   // Ack the char
            } while (uTemp == uTemp2); // Match?
            ++pInput;                  // Next main string char
            // Next entry
            uTemp = pInput[0];
            // Source string still ok?
        } while (uTemp);
    }
    // No string match
    return nullptr;
}

/*! ************************************

    \brief Locate a substring, case insensitive

    Returns a pointer to the first occurrence of pTest in pInput, or a \ref NULL
    pointer if pTest is not part of pInput.

    The case insensitive matching process does not include the terminating
    null-characters, but it stops there.

    \param pInput Pointer to the string to scan
    \param pTest Pointer to the substring to look for

    \return \ref NULL if substring was not found or the pointer to the first
        matching string

    \sa Burger::StringString(const char *,const char *)

***************************************/

char* BURGER_API Burger::StringCaseString(const char* pInput, const char* pTest)
{
    // Get the first character
    uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
    if (uTemp) { // Do I even bother?
        do {
            uintptr_t i = 0; // Init the index
            uint_t uTemp2;
            do {
                // Get the first char of the test string
                uTemp2 = reinterpret_cast<const uint8_t*>(pTest)[i];
                if (!uTemp2) {                        // Match?
                    return const_cast<char*>(pInput); // I matched here!
                }
                // Get the source string
                uTemp = reinterpret_cast<const uint8_t*>(pInput)[i];
                ++i;                                  // Ack the char
                if (uTemp2 >= 'A' && uTemp2 <= 'Z') { // Convert to lower case
                    uTemp2 += 32;
                }
                if (uTemp >= 'A' && uTemp <= 'Z') { // Convert to lower case
                    uTemp += 32;
                }
            } while (uTemp == uTemp2); // Match?
            ++pInput;                  // Next main string char
            // Next entry
            uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
            // Source string still ok?
        } while (uTemp);
    }
    // No string match
    return nullptr;
}

/*! ************************************

    \brief Locate a substring, case insensitive (UTF-16 version)

    Returns a pointer to the first occurrence of pTest in pInput, or a \ref NULL
    pointer if pTest is not part of pInput.

    The case insensitive matching process does not include the terminating
    null-characters, but it stops there.

    \param pInput Pointer to the string to scan
    \param pTest Pointer to the substring to look for

    \return \ref NULL if substring was not found or the pointer to the first
        matching string

    \sa Burger::StringString(const uint16_t *,const uint16_t *)

***************************************/

uint16_t* BURGER_API Burger::StringCaseString(
    const uint16_t* pInput, const uint16_t* pTest)
{
    // Get the first character
    uint_t uTemp = pInput[0];
    if (uTemp) { // Do I even bother?
        do {
            uintptr_t i = 0; // Init the index
            uint_t uTemp2;
            do {
                // Get the first char of the test string
                uTemp2 = pTest[i];
                if (!uTemp2) {                          // Match?
                    return const_cast<uint16_t*>(pInput); // I matched here!
                }
                // Get the source string
                uTemp = pInput[i];
                ++i;                                  // Ack the char
                if (uTemp2 >= 'A' && uTemp2 <= 'Z') { // Convert to lower case
                    uTemp2 += 32;
                }
                if (uTemp >= 'A' && uTemp <= 'Z') { // Convert to lower case
                    uTemp += 32;
                }
            } while (uTemp == uTemp2); // Match?
            ++pInput;                  // Next main string char
            // Next entry
            uTemp = pInput[0];
            // Source string still ok?
        } while (uTemp);
    }
    // No string match
    return nullptr;
}

/*! ************************************

    \brief Split a string into tokens

    On a first call, the function expects a C string as argument for pInput,
    whose first character is used as the starting location to scan for tokens.
    In subsequent calls, the function expects a null pointer and uses the
    position right after the end of the last token as the new starting location
    for scanning.

    \note This function will destroy the initial input of pInput. pInput must
    remain in scope while this function and its output are in use. Replaces
    strtok_r().

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the string of delimiters to scan
    \param ppSave Pointer to a char * to save the state

    \return \ref NULL if not more tokens are remaining, or a pointer to the
    first character of the token found which is zero terminated at the delimiter
    value.

    \sa Burger::StringToken(uint16_t *,const uint16_t *, uint16_t **)

***************************************/

char* BURGER_API Burger::StringToken(
    char* pInput, const char* pDelimiters, char** ppSave) BURGER_NOEXCEPT
{
    // If no input string, use the saved pointer
    if (!pInput) {
        pInput = ppSave[0];
    }

    char* pResult;

    // Skip over "whitespace"
    pInput += StringSkipOver(pInput, pDelimiters);
    if (!pInput[0]) {
        pResult = nullptr;
    } else {
        // Return the start of the token
        pResult = pInput;
        // Find the end of this string
        pInput += StringStopAt(pInput, pDelimiters);
        if (pInput[0]) {
            // Mark the end of the string and save past the marker
            pInput[0] = 0;
            ++pInput;
        }
    }
    // Save the state
    *ppSave = pInput;
    return pResult;
}

/*! ************************************

    \brief Split a string into tokens

    On a first call, the function expects a C string as argument for pInput,
    whose first character is used as the starting location to scan for tokens.
    In subsequent calls, the function expects a null pointer and uses the
    position right after the end of the last token as the new starting location
    for scanning.

    \note This function will destroy the initial input of pInput. pInput must
    remain in scope while this function and its output are in use. Replaces
    strtok_r().

    \param pInput Pointer to the string to scan
    \param pDelimiters Pointer to the string of delimiters to scan
    \param ppSave Pointer to a char * to save the state

    \return \ref NULL if not more tokens are remaining, or a pointer to the
    first character of the token found which is zero terminated at the delimiter
    value.

    \sa Burger::StringToken(char *,const char *, char **)

***************************************/

uint16_t* BURGER_API Burger::StringToken(
    uint16_t* pInput, const uint16_t* pDelimiters, uint16_t** ppSave) BURGER_NOEXCEPT
{
    // If no input string, use the saved pointer
    if (!pInput) {
        pInput = ppSave[0];
    }

    uint16_t* pResult;

    // Skip over "whitespace"
    pInput += StringSkipOver(pInput, pDelimiters);
    if (!pInput[0]) {
        pResult = nullptr;
    } else {
        // Return the start of the token
        pResult = pInput;
        // Find the end of this string
        pInput += StringStopAt(pInput, pDelimiters);
        if (pInput[0]) {
            // Mark the end of the string and save past the marker
            pInput[0] = 0;
            ++pInput;
        }
    }
    // Save the state
    *ppSave = pInput;
    return pResult;
}
