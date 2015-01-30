/***************************************

	Helper functions for printf and the like

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brstdouthelpers.h"
#include "brstringfunctions.h"
#include <stdio.h>

/*! ************************************

	\brief Print a hex character to standard out

	Print a single hex digit from the lowest 4 bits of the input
	value to stdout

	\sa PrintHex(Word8), PrintHex(Word16), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHexDigit(Word uInput)
{
	putchar(g_NibbleToAsciiUppercase[uInput&0xF]);		/* Print the char */
}

/*! ************************************

	\brief Print an 8 bit value in hex to standard out

	Print two hex digits in the form of 00 to stdout

	\sa PrintHexDigit(Word), PrintHex(Word16), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word8 uInput)
{
	char TempString[2+1];
	TempString[0] = g_NibbleToAsciiUppercase[(uInput>>4)&0xF];
	TempString[1] = g_NibbleToAsciiUppercase[uInput&0xF];
	TempString[2] = 0;
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 16 bit value in hex to standard out

	Print four hex digits in the form of 0000 to stdout

	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word16 uInput)
{
	char TempString[4+1];
	TempString[0] = g_NibbleToAsciiUppercase[(uInput>>12)&0xF];
	TempString[1] = g_NibbleToAsciiUppercase[(uInput>>8)&0xF];
	TempString[2] = g_NibbleToAsciiUppercase[(uInput>>4)&0xF];
	TempString[3] = g_NibbleToAsciiUppercase[uInput&0xF];
	TempString[4] = 0;
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 32 bit value in hex to standard out

	Print eight hex digits in the form of 00000000 to stdout

	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word16), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word32 uInput)
{
	char TempString[8+1];
	TempString[0] = g_NibbleToAsciiUppercase[(uInput>>28)&0xF];
	TempString[1] = g_NibbleToAsciiUppercase[(uInput>>24)&0xF];
	TempString[2] = g_NibbleToAsciiUppercase[(uInput>>20)&0xF];
	TempString[3] = g_NibbleToAsciiUppercase[(uInput>>16)&0xF];
	TempString[4] = g_NibbleToAsciiUppercase[(uInput>>12)&0xF];
	TempString[5] = g_NibbleToAsciiUppercase[(uInput>>8)&0xF];
	TempString[6] = g_NibbleToAsciiUppercase[(uInput>>4)&0xF];
	TempString[7] = g_NibbleToAsciiUppercase[uInput&0xF];
	TempString[8] = 0;
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 64 bit value in hex to standard out

	Print sixteen hex digits in the form of 0000000000000000 to stdout

	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word16), or PrintHex(Word32) 

***************************************/

void BURGER_API Burger::PrintHex(Word64 uInput)
{
	char TempString[16+1];
	TempString[0] = g_NibbleToAsciiUppercase[(uInput>>60)&0xF];
	TempString[1] = g_NibbleToAsciiUppercase[(uInput>>56)&0xF];
	TempString[2] = g_NibbleToAsciiUppercase[(uInput>>52)&0xF];
	TempString[3] = g_NibbleToAsciiUppercase[(uInput>>48)&0xF];
	TempString[4] = g_NibbleToAsciiUppercase[(uInput>>44)&0xF];
	TempString[5] = g_NibbleToAsciiUppercase[(uInput>>40)&0xF];
	TempString[6] = g_NibbleToAsciiUppercase[(uInput>>36)&0xF];
	TempString[7] = g_NibbleToAsciiUppercase[(uInput>>32)&0xF];
	TempString[8] = g_NibbleToAsciiUppercase[(uInput>>28)&0xF];
	TempString[9] = g_NibbleToAsciiUppercase[(uInput>>24)&0xF];
	TempString[10] = g_NibbleToAsciiUppercase[(uInput>>20)&0xF];
	TempString[11] = g_NibbleToAsciiUppercase[(uInput>>16)&0xF];
	TempString[12] = g_NibbleToAsciiUppercase[(uInput>>12)&0xF];
	TempString[13] = g_NibbleToAsciiUppercase[(uInput>>8)&0xF];
	TempString[14] = g_NibbleToAsciiUppercase[(uInput>>4)&0xF];
	TempString[15] = g_NibbleToAsciiUppercase[uInput&0xF];
	TempString[16] = 0;
	fputs(TempString,stdout);
}