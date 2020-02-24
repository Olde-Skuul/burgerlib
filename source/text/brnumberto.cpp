/***************************************

    Number to ASCII functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberto.h"
#include "brfixedpoint.h"
#include "brstringfunctions.h"
#include <stdio.h>

/*! ************************************

	\brief Convert an 8 bit value into a hex string

	Store two hex digits in the form of 00 to a string buffer and append a
	terminating zero. Returns the pointer to the terminating zero so the calling
	function can continue writing to the output buffer as if it's a stream.

	\param pOutput Pointer to a buffer at least 3 bytes in size.
	\param uInput 8 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word16), NumberToAsciiHex(char *,Word32),
		NumberToAsciiHex(char *,Word64), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double), NumberToAsciiHex(char *,Word32,Word),
		or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, Word8 uInput)
{
	// Convert the string and store in the buffer
	pOutput[0] = g_NibbleToAsciiUppercase[(uInput >> 4U) & 0xFU];
	pOutput[1] = g_NibbleToAsciiUppercase[uInput & 0xFU];
	pOutput[2] = 0;
	// Return the pointer to the terminating zero
	return pOutput + 2;
}

/*! ************************************

	\brief Convert a 16 bit value into a hex string

	Store four hex digits in the form of 0000 to a string buffer and append a
	terminating zero. Returns the pointer to the terminating zero so the calling
	function can continue writing to the output buffer as if it's a stream.

	\param pOutput Pointer to a buffer at least 5 bytes in size.
	\param uInput 16 bit value to print in HEX to a string.

	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word32),
		NumberToAsciiHex(char *,Word64), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double), NumberToAsciiHex(char *,Word32,Word),
		or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, Word16 uInput)
{
	const void* pInput = &uInput;
	// Push the pointer to the highest value byte
	char* pEnd = &pOutput[sizeof(Word16) * 2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8*>(pInput) + (sizeof(Word16) - 1));
#endif
	do {
		Word uTemp = static_cast<const Word8*>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8*>(pInput) - 1);
#else
		pInput = (static_cast<const Word8*>(pInput) + 1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp >> 4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp & 0xFU];
		pOutput += 2;
	} while (pOutput < pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 32 bit value into a hex string

	Store eight hex digits in the form of 00000000 to a string buffer and append
	a terminating zero. Returns the pointer to the terminating zero so the
	calling function can continue writing to the output buffer as if it's a
	stream.

	\param pOutput Pointer to a buffer at least 9 bytes in size.
	\param uInput 32 bit value to print in HEX to a string.

	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16),
		NumberToAsciiHex(char *,Word64), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double), NumberToAsciiHex(char *,Word32,Word),
		or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, Word32 uInput)
{
	const void* pInput = &uInput;
	// Push the pointer to the highest value byte
	char* pEnd = &pOutput[sizeof(Word32) * 2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8*>(pInput) + (sizeof(Word32) - 1));
#endif
	do {
		Word uTemp = static_cast<const Word8*>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8*>(pInput) - 1);
#else
		pInput = (static_cast<const Word8*>(pInput) + 1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp >> 4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp & 0xFU];
		pOutput += 2;
	} while (pOutput < pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 64 bit value into a hex string

	Store sixteen hex digits in the form of 0000000000000000 to a string buffer
	and append a terminating zero. Returns the pointer to the terminating zero
	so the calling function can continue writing to the output buffer as if it's
	a stream.

	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param uInput 64 bit value to print in HEX to a string.

	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16),
		NumberToAsciiHex(char *,Word32), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double), NumberToAsciiHex(char *,Word32,Word),
		or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, Word64 uInput)
{
	const void* pInput = &uInput;
	// Push the pointer to the highest value byte
	char* pEnd = &pOutput[sizeof(Word64) * 2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8*>(pInput) + (sizeof(Word64) - 1));
#endif
	do {
		Word uTemp = static_cast<const Word8*>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8*>(pInput) - 1);
#else
		pInput = (static_cast<const Word8*>(pInput) + 1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp >> 4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp & 0xFU];
		pOutput += 2;
	} while (pOutput < pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 32 bit floating point value into a hex string

	Store eight hex digits in the form of 00000000 to a string buffer and append
	a terminating zero. Returns the pointer to the terminating zero so the
	calling function can continue writing to the output buffer as if it's a
	stream.

	\param pOutput Pointer to a buffer at least 9 bytes in size.
	\param fInput 32 bit value to print in HEX to a string.

	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16),
		NumberToAsciiHex(char *,Word32), NumberToAsciiHex(char *,Word64),
		NumberToAsciiHex(char *,double), NumberToAsciiHex(char *,Word32,Word),
		or NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, float fInput)
{
	const void* pInput = &fInput;
	// Push the pointer to the highest value byte
	char* pEnd = &pOutput[sizeof(float) * 2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8*>(pInput) + (sizeof(float) - 1));
#endif
	do {
		Word uTemp = static_cast<const Word8*>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8*>(pInput) - 1);
#else
		pInput = (static_cast<const Word8*>(pInput) + 1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp >> 4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp & 0xFU];
		pOutput += 2;
	} while (pOutput < pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert a 64 bit floating point value into a hex string

	Store sixteen hex digits in the form of 0000000000000000 to a string buffer
	and append a terminating zero. Returns the pointer to the terminating zero
	so the calling function can continue writing to the output buffer as if it's
	a stream.

	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param dInput 64 bit value to print in HEX to a string.

	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,Word8), NumberToAsciiHex(char *,Word16),
		NumberToAsciiHex(char *,Word32), NumberToAsciiHex(char *,Word64),
		NumberToAsciiHex(char *,float), NumberToAsciiHex(char *,Word32,Word), or
		NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(char* pOutput, double dInput)
{
	const void* pInput = &dInput;
	// Push the pointer to the highest value byte
	char* pEnd = &pOutput[sizeof(double) * 2];
#if defined(BURGER_LITTLEENDIAN)
	pInput = (static_cast<const Word8*>(pInput) + (sizeof(double) - 1));
#endif
	do {
		Word uTemp = static_cast<const Word8*>(pInput)[0];
#if defined(BURGER_LITTLEENDIAN)
		pInput = (static_cast<const Word8*>(pInput) - 1);
#else
		pInput = (static_cast<const Word8*>(pInput) + 1);
#endif
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp >> 4U];
		pOutput[1] = g_NibbleToAsciiUppercase[uTemp & 0xFU];
		pOutput += 2;
	} while (pOutput < pEnd);
	// Insert the zero terminator
	pOutput[0] = 0;
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 32 bit integer into hexadecimal ASCII.

	Convert an unsigned 32 bit integer into a hexadecimal ASCII string. You can
	modify how the data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

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
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput,Word64 uInput,Word uDigits),
		NumberToAscii(char *,Word32,Word) or NumberToAscii(char *,Int32,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, Word32 uInput, Word uDigits)
{
	Word uIndex = uDigits & 0xFFU;		// Get the count of digits to process
	if ((uIndex - 1U) >= 7U) {			// Failsafe
		uIndex = 8;						// Maximum of 8 digits
	} else {							// Won't show certain digits?
		uInput <<= ((8 - uIndex) << 2); // Shift 4,8, -> 28
	}
	if (--uIndex) {
		do {
			Word uLetter = uInput >>
				static_cast<Word>((8 - 1) * 4);		   // Get the highest 4 bits
			if (uLetter || (uDigits & LEADINGZEROS)) { // Already printing?
				uDigits |= LEADINGZEROS;			   // Force future printing
				pOutput[0] =
					g_NibbleToAsciiUppercase[uLetter]; // Print the char
				++pOutput;
			}
			uInput <<= 4;   // Shift to next digit
		} while (--uIndex); // Any more left?
	}
	pOutput[0] = g_NibbleToAsciiUppercase[uInput >>
		static_cast<Word>((8 - 1) * 4)]; // Also must print on last char
	++pOutput;
	if (!(uDigits & NOENDINGNULL)) {
		pOutput[0] = 0; // Terminate the string
	}
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 64 bit integer into hexadecimal ASCII.

	Convert an unsigned 64 bit integer into a hexadecimal ASCII string. You can
	modify how the data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

	Examples:<br>
	uInput = 0x1234BCD, uDigits = 4 -> "4BCD"<br>
	uInput = 0x1234BCD, uDigits = 0 -> "1234BCD"<br>
	uInput = 0x1234BCD, uDigits = Burger::LEADINGZEROS -> "01234BCD"<br>
	uInput = 0x2A, uDigits = Burger::LEADINGZEROS+4 -> "002A"

	\note uDigits is capped at 16 digits. It will not output more than 16
	digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 17 bytes in size Whichever is appropriate.
	\param uInput Unsigned 64 bit integer to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput,Word32 uInput,Word uDigits),
		NumberToAscii(char *,Word32,Word) or NumberToAscii(char *,Int32,Word)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, Word64 uInput, Word uDigits)
{
	Word uIndex = uDigits & 0xFFU;		 // Get the count of digits to process
	if ((uIndex - 1U) >= 15U) {			 // Failsafe
		uIndex = 16;					 // Maximum of 16 digits
	} else {							 // Won't show certain digits?
		uInput <<= ((16 - uIndex) << 2); // Shift 4,8, -> 56
	}
	if (--uIndex) {
		do {
			Word uLetter = static_cast<Word>(uInput >>
				static_cast<Word>((16 - 1) * 4));	  // Get the highest 4 bits
			if (uLetter || (uDigits & LEADINGZEROS)) { // Already printing?
				uDigits |= LEADINGZEROS;			   // Force future printing
				pOutput[0] =
					g_NibbleToAsciiUppercase[uLetter]; // Print the char
				++pOutput;
			}
			uInput <<= 4;   // Shift to next digit
		} while (--uIndex); // Any more left?
	}
	pOutput[0] = g_NibbleToAsciiUppercase[uInput >>
		static_cast<Word>((16 - 1) * 4)]; // Also must print on last char
	++pOutput;
	if (!(uDigits & NOENDINGNULL)) {
		pOutput[0] = 0; // Terminate the string
	}
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 32 bit integer into ASCII.

	Convert an unsigned 32 bit integer into an ASCII string. You can modify how
	the data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"

	\note uDigits is capped at 10 digits. It will not output more than 10
		digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 12 bytes in size Whichever is appropriate.
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa Burger::NumberToAsciiHex(char *,Word32,Word) or
		Burger::NumberToAscii(char *,Int32,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, Word32 uInput, Word uDigits) BURGER_NOEXCEPT
{
	Word uIndex = uDigits & 0xFFU; // How many digits to process?
	if ((uIndex - 1U) >= 9U) { // Out of bounds? Or default? (Allow 1-10 only)
		uIndex = 10;		   // 10 digits to process
	} else {				   // If smaller than the max, hack off the
							   // excess with a modulo
		uInput = uInput % g_TensTable32[uIndex];
	}

	// Let's iterate (Index = 1-10)
	if (--uIndex) { // Only iterate on >=10
		do {
			Word32 uBigNum = g_TensTable32[uIndex]; // Get div value in local
			Word uLetter = '0';						// Init ASCII value
			if (uInput >= uBigNum) {				// Bigger than the min?
				uDigits |= LEADINGZEROS;			// Force future printing
				do {
					uInput -= uBigNum; // Remove value
					++uLetter;		   // Inc ASCII value (Max 9 iterations)
				} while (uInput >= uBigNum); // Keep going?
			}

			// Should I print leading zeros?

			if (uDigits & LEADINGZEROS) {				 // Already printing?
				pOutput[0] = static_cast<char>(uLetter); // Output the digit
				++pOutput;
			}
		} while (--uIndex); /* Any more left? */
	}

	// Handle the last digit (No divide needed)
	// and it's ALWAYS output.

	pOutput[0] = static_cast<char>(uInput + '0');
	++pOutput;

	// Do I need to write a null?

	if (!(uDigits & NOENDINGNULL)) {
		pOutput[0] = 0; // Terminate the string
	}
	return pOutput; // Return the end pointer
}

/*! ************************************

	\brief Convert a signed 32 bit integer into ASCII.

	Convert a signed 32 bit integer into an ASCII string. You can modify how the
	data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = -1234567, uDigits = 4 -> "-4567"<br>
	uInput = -1234567, uDigits = 0 -> "-1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"<br>
	uInput = -22, uDigits = Burger::LEADINGZEROS+4 -> "-0022"

	\note uDigits is capped at 10 digits. It will not output more than 10
		digits. The negative sign does NOT count as part of the numeric string
		length.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 12 bytes in size Whichever is appropriate.
	\param iInput Signed 32 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa Burger::NumberToAscii(char *,Word32,Word) or
		Burger::NumberToAsciiHex(char *,Word32,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, Int32 iInput, Word uDigits) BURGER_NOEXCEPT
{
	if (iInput < 0) {	 // Negative?
		iInput = -iInput; // Convert to positive
		pOutput[0] = '-'; // Output the '-' sign.
		++pOutput;
	}
	return NumberToAscii(pOutput, static_cast<Word32>(iInput), uDigits);
}

/*! ************************************

	\brief Convert an unsigned 64 bit integer into ASCII.

	Convert an unsigned 64 bit integer into an ASCII string. You can modify how
	the data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"

	\note uDigits is capped at 20 digits. It will not output more than 20
		digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 24 bytes in size Whichever is appropriate.
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa Burger::NumberToAsciiHex(char *,Word64,Word) or
		Burger::NumberToAscii(char *,Int64,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, Word64 uInput, Word uDigits) BURGER_NOEXCEPT
{
	Word uIndex = uDigits & 0xFFU; // How many digits to process?
	if ((uIndex - 1U) >= 19U) { // Out of bounds? Or default? (Allow 1-10 only)
		uIndex = 20;			// 10 digits to process
	} else {					// If smaller than the max, hack off the
								// excess with a modulo
		uInput = uInput % g_TensTable64[uIndex];
	}

	// Let's iterate (Index = 1-20)
	if (--uIndex) { // Only iterate on >=20
		do {
			Word64 uBigNum = g_TensTable64[uIndex]; // Get div value in local
			Word uLetter = '0';						// Init ASCII value
			if (uInput >= uBigNum) {				// Bigger than the min?
				uDigits |= LEADINGZEROS;			// Force future printing
				do {
					uInput -= uBigNum; // Remove value
					++uLetter;		   // Inc ASCII value (Max 9 iterations)
				} while (uInput >= uBigNum); // Keep going?
			}

			// Should I print leading zeros?

			if (uDigits & LEADINGZEROS) {				 // Already printing?
				pOutput[0] = static_cast<char>(uLetter); // Output the digit
				++pOutput;
			}
		} while (--uIndex); /* Any more left? */
	}

	// Handle the last digit (No divide needed)
	// and it's ALWAYS output.

	pOutput[0] = static_cast<char>(uInput + '0');
	++pOutput;

	// Do I need to write a null?

	if (!(uDigits & NOENDINGNULL)) {
		pOutput[0] = 0; // Terminate the string
	}
	return pOutput; // Return the end pointer
}

/*! ************************************

	\brief Convert a signed 64 bit integer into ASCII.

	Convert a signed 64 bit integer into an ASCII string. You can modify how the
	data is formatted by passing a third parameter.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so you can modify the an existing string
	without accidentally truncating it.

	Examples:<br>
	uInput = 1234567, uDigits = 4 -> "4567"<br>
	uInput = 1234567, uDigits = 0 -> "1234567"<br>
	uInput = -1234567, uDigits = 4 -> "-4567"<br>
	uInput = -1234567, uDigits = 0 -> "-1234567"<br>
	uInput = 1234567, uDigits = Burger::LEADINGZEROS -> "0001234567"<br>
	uInput = 22, uDigits = Burger::LEADINGZEROS+4 -> "0022"<br>
	uInput = -22, uDigits = Burger::LEADINGZEROS+4 -> "-0022"

	\note uDigits is capped at 20 digits. It will not output more than 20
		digits. The negative sign does NOT count as part of the numeric string
		length.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 24 bytes in size Whichever is appropriate.
	\param iInput Signed 64 bit integer to convert to ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa Burger::NumberToAscii(char *,Word64,Word) or
		Burger::NumberToAsciiHex(char *,Word64,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, Int64 iInput, Word uDigits) BURGER_NOEXCEPT
{
	if (iInput < 0) {	 // Negative?
		iInput = -iInput; // Convert to positive
		pOutput[0] = '-'; // Output the '-' sign.
		++pOutput;
	}
	return NumberToAscii(pOutput, static_cast<Word64>(iInput), uDigits);
}

/*! ************************************

	\brief Convert a 32 bit float into ASCII.

	Convert a 32 bit float into an ASCII string.

	Examples:<br>
	fInput = 123.4567 -> "123.4567"<br>
	fInput = 123456.7 -> "123456.7"<br>
	fInput = -1.234567 -> "-1.234567"<br>
	fInput = -1234.567 -> "-1234.567"<br>

	\note The string is capped at 30 digits. It will not output more than 30
		digits. The negative sign does NOT count as part of the numeric string
		length.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be a minimum of 32 bytes in size.
	\param fInput 32 bit float to convert to ASCII.

	\return Pointer to the char immediately after the last char output.
	\sa Burger::NumberToAscii(char *,Word32,Word) or
		Burger::NumberToAsciiHex(char *,Word32,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(char* pOutput, float fInput) BURGER_NOEXCEPT
{
#if defined(BURGER_XBOX360) || (defined(BURGER_WINDOWS) && (defined(_MSC_VER) && (_MSC_VER>=1400)))
	int iLength = sprintf_s(pOutput, 32, "%f", static_cast<double>(fInput));
#else
	int iLength = sprintf(pOutput, "%f", static_cast<double>(fInput));
#endif
	if (iLength < 0) {
		iLength = 0;
	}
	return pOutput + iLength;
}

/*! ************************************

	\brief Convert a 64 bit float into ASCII.

	Convert a 64 bit float into an ASCII string.

	Examples:<br>
	dInput = 123.4567 -> "123.4567"<br>
	dInput = 123456.7 -> "123456.7"<br>
	dInput = -1.234567 -> "-1.234567"<br>
	dInput = -1234.567 -> "-1234.567"<br>

	\note The string is capped at 30 digits. It will not output more than 30
		digits. The negative sign does NOT count as part of the numeric string
		length.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be a minimum of 32 bytes in size.
	\param dInput 64 bit float to convert to ASCII.

	\return Pointer to the char immediately after the last char output.

	\sa Burger::NumberToAscii(char *,Word32,Word) or
Burger::NumberToAsciiHex(char *,Word32,Word)

***************************************/

char* BURGER_API Burger::NumberToAscii(char* pOutput, double dInput) BURGER_NOEXCEPT
{
#if defined(BURGER_XBOX360) || (defined(BURGER_WINDOWS) && (defined(_MSC_VER) && (_MSC_VER>=1400)))
	int iLength = sprintf_s(pOutput, 32, "%f", dInput);
#else
	int iLength = sprintf(pOutput, "%f", dInput);
#endif
	if (iLength < 0) {
		iLength = 0;
	}
	return pOutput + iLength;
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 32 bit integers have a minimum of one character to a maximum
	of 10 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 10 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(Word64) or NumberStringLength(Int32)

***************************************/

Word BURGER_API Burger::NumberStringLength(Word32 uInput)
{
	// Perform a binary search for performance

	Word uResult;
	if (uInput < 10000U) {		// 1-4 digits?
		if (uInput < 100U) {	// 1-2 digits
			if (uInput < 10U) { // 1 digit?
				uResult = 1;	// 0-9
			} else {
				uResult = 2; // 10-99
			}
		} else if (uInput < 1000U) {
			uResult = 3; // 100-999
		} else {
			uResult = 4; // 1000-9999
		}
	} else if (uInput < 100000000U) { // 5-8 digits?
		if (uInput < 1000000U) {	  // 5-6 digits?
			if (uInput < 100000U) {
				uResult = 5; // 10000-99999
			} else {
				uResult = 6; // 100000-999999
			}
		} else if (uInput < 10000000U) { // 7 digits?
			uResult = 7;				 // 1000000-9999999
		} else {
			uResult = 8; // 10000000-99999999
		}
	} else if (uInput < 1000000000U) { // 9-10 digits?
		uResult = 9;				   // 100000000-999999999
	} else {
		uResult = 10; // 1000000000-4294967295
	}
	return uResult;
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 32 bit integers have a minimum of one character to a maximum
	of 10 characters and an extra character for a '-' for negative numbers

	\param iInput 32 bit signed integer to test.

	\return 1 through 10 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(Word32) or NumberStringLength(Int64)

***************************************/

Word BURGER_API Burger::NumberStringLength(Int32 iInput)
{
	Word uSign =
		static_cast<Word32>(iInput) >> 31U; // 1 if negative, 0 positive
	return uSign + NumberStringLength(static_cast<Word32>(Abs(iInput)));
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 64 bit integers have a minimum of one character to a maximum
	of 21 characters.

	\param uInput 32 bit unsigned integer to test.
	\return 1 through 21 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(Int64) or NumberStringLength(Word32)

***************************************/

Word BURGER_API Burger::NumberStringLength(Word64 uInput)
{
	// Assume no extra digits
	Word uExtraDigits = 0;

	// Is this REALLY a 64 bit value?
	if (uInput >= 0x100000000ULL) {
		// Divide by 1 billion (remove 9 digits)
		uInput = uInput / 1000000000U;
		uExtraDigits = 9;

		// edge case.. very close to max_int64 will still be > 32 bits after
		// division
		if (uInput >= 0x100000000ULL) {
			uInput = uInput / 100U;
			uExtraDigits = 11;
		}
	}
	// Get the rest of the digit count
	return uExtraDigits + NumberStringLength(static_cast<Word32>(uInput));
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 64 bit integers have a minimum of one character to a maximum
	of 21 characters and an extra character for a '-' for negative numbers

	\param iInput 64 bit signed integer to test.

	\return 1 through 2 for the number of digits this number requires for string
		conversion.

	\sa NumberStringLength(Word32) or NumberStringLength(Int64)

***************************************/

Word BURGER_API Burger::NumberStringLength(Int64 iInput)
{
	// 1 if negative, 0 positive
	Word uSign = static_cast<Word>(static_cast<Word64>(iInput) >> 63U);
	return uSign + NumberStringLength(static_cast<Word64>(Abs(iInput)));
}

/*! ************************************

	\brief Calculate the length of a string that represents this hex integer.

	Determine the number of characters are needed to convert the input value
	into a hexadecimal string. 32 bit integers have a minimum of one character
	to a maximum of 8 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 8 for the number of digits this number requires for string
		conversion.

	\sa NumberHexStringLength(Word64)

***************************************/

Word BURGER_API Burger::NumberHexStringLength(Word32 uInput)
{
	Word uDigitCount = 1;

	// Any other digits?
	if (uInput & 0xFFFFFFF0U) {
		do {
			// Add another digit
			++uDigitCount;
			uInput >>= 4U;
		} while (uInput & 0xFFFFFFF0U);
	}
	return uDigitCount;
}

/*! ************************************

	\brief Calculate the length of a string that represents this hex integer.

	Determine the number of characters are needed to convert the input value
	into a hexadecimal string. 64 bit integers have a minimum of one character
	to a maximum of 16 characters.

	\param uInput 64 bit unsigned integer to test.

	\return 1 through 16 for the number of digits this number requires for
		string conversion.

	\sa NumberHexStringLength(Word32)

***************************************/

Word BURGER_API Burger::NumberHexStringLength(Word64 uInput)
{
	Word uDigitCount = 1;

	// Any other digits?
	if (uInput & 0xFFFFFFFFFFFFFFF0ULL) {
		do {
			// Add another digit
			++uDigitCount;
			uInput >>= 4U;
		} while (uInput & 0xFFFFFFFFFFFFFFF0ULL);
	}
	return uDigitCount;
}

/*! ************************************

	\brief Calculate the length of a string that represents this octal integer.

	Determine the number of characters are needed to convert the input value
	into a octal string. 32 bit integers have a minimum of one character to a
	maximum of 11 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 11 for the number of digits this number requires for
		string conversion.

	\sa NumberOctalStringLength(Word64)

***************************************/

Word BURGER_API Burger::NumberOctalStringLength(Word32 uInput)
{
	Word uDigitCount = 1;

	// Any other digits?
	if (uInput & 0xFFFFFFF8U) {
		do {
			// Add another digit
			++uDigitCount;
			uInput >>= 3U;
		} while (uInput & 0xFFFFFFF8U);
	}
	return uDigitCount;
}

/*! ************************************

	\brief Calculate the length of a string that represents this octal integer.

	Determine the number of characters are needed to convert the input value
	into a octal string. 64 bit integers have a minimum of one character to a
	maximum of 22 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 22 for the number of digits this number requires for
		string conversion.

	\sa NumberOctalStringLength(Word32)

***************************************/

Word BURGER_API Burger::NumberOctalStringLength(Word64 uInput)
{
	Word uDigitCount = 1;

	// Any other digits?
	if (uInput & 0xFFFFFFFFFFFFFFF8ULL) {
		do {
			// Add another digit
			++uDigitCount;
			uInput >>= 3U;
		} while (uInput & 0xFFFFFFFFFFFFFFF8ULL);
	}
	return uDigitCount;
}
