/***************************************

	Helper functions for printf and the like

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\param uInput Integer value (Only lowest 4 bits used) to print in HEX to stdout
	\sa PrintHex(Word8), PrintHex(Word16), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHexDigit(Word uInput)
{
	putchar(g_NibbleToAsciiUppercase[uInput&0xF]);		/* Print the char */
}

/*! ************************************

	\brief Print an 8 bit value in hex to standard out

	Print two hex digits in the form of 00 to stdout

	\param uInput 8 bit value to print in HEX to stdout
	\sa PrintHexDigit(Word), PrintHex(Word16), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word8 uInput)
{
	// Convert to a string
	char TempString[(sizeof(Word8)*2)+1];
	NumberToAsciiHex(TempString,uInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 16 bit value in hex to standard out

	Print four hex digits in the form of 0000 to stdout

	\param uInput 16 bit value to print in HEX to stdout
	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word32), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word16 uInput)
{
	// Convert to a string
	char TempString[(sizeof(Word16)*2)+1];
	NumberToAsciiHex(TempString,uInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 32 bit value in hex to standard out

	Print eight hex digits in the form of 00000000 to stdout

	\param uInput 32 bit value to print in HEX to stdout
	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word16), or PrintHex(Word64) 

***************************************/

void BURGER_API Burger::PrintHex(Word32 uInput)
{
	// Convert to a string
	char TempString[(sizeof(Word32)*2)+1];
	NumberToAsciiHex(TempString,uInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 64 bit value in hex to standard out

	Print sixteen hex digits in the form of 0000000000000000 to stdout

	\param uInput 64 bit value to print in HEX to stdout
	\sa PrintHexDigit(Word), PrintHex(Word8), PrintHex(Word16), or PrintHex(Word32) 

***************************************/

void BURGER_API Burger::PrintHex(Word64 uInput)
{
	// Convert to a string
	char TempString[(sizeof(Word64)*2)+1];
	NumberToAsciiHex(TempString,uInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 32 bit floating pointer number in hex to standard out

	Print eight hex digits in the form of 00000000 to stdout

	\param fInput Floating point number to print in HEX to stdout
	\sa PrintHex(Word32), or PrintHex(double) 

***************************************/

void BURGER_API Burger::PrintHex(float fInput)
{
	// Convert to a string
	char TempString[(sizeof(float)*2)+1];
	NumberToAsciiHex(TempString,fInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Print a 64 bit floating point number in hex to standard out

	Print sixteen hex digits in the form of 0000000000000000 to stdout

	\param dInput 64 bit floating point number to print in HEX to stdout
	\sa PrintHex(Word64), or PrintHex(float) 

***************************************/

void BURGER_API Burger::PrintHex(double dInput)
{
	// Convert to a string
	char TempString[(sizeof(double)*2)+1];
	NumberToAsciiHex(TempString,dInput);

	// Send the string to stdout
	fputs(TempString,stdout);
}

/*! ************************************

	\brief Clear the text console

	For platforms that support a text console, clear out
	the	text console by sending a line feed or system
	appropriate set of calls to perform the same action
	the ends up with a clear text screen and the
	output cursor set to the upper left corner.

	This is an equivalent of the Borland C library
	function clrscr().

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
void BURGER_API Burger::ClearConsole(void)
{
	// Send a line feed to clear the screen
	putchar(12);
}
#endif
