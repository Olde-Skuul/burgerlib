/***************************************

	Number to ASCII functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberto.h"
#include "brfixedpoint.h"
#include "brstringfunctions.h"
#include "brstructs.h"
#include <stdio.h>

/*! ************************************

	\brief Convert an 8 bit value into a hex string

	Store two hex digits in the form of 00 to a string buffer and append a
	terminating zero. Returns the pointer to the terminating zero so the calling
	function can continue writing to the output buffer as if it's a stream.

	\param pOutput Pointer to a buffer at least 3 bytes in size.
	\param uInput 8 bit value to print in HEX to a string.
	\return Pointer to terminating zero at the end of the string

	\sa NumberToAsciiHex(char *,uint16_t), NumberToAsciiHex(char *,uint32_t),
		NumberToAsciiHex(char *,uint64_t), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint8_t uInput) BURGER_NOEXCEPT
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

	\sa NumberToAsciiHex(char *,uint8_t), NumberToAsciiHex(char *,uint32_t),
		NumberToAsciiHex(char *,uint64_t), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint16_t uInput) BURGER_NOEXCEPT
{
	// Push the pointer to the highest value byte
	uint_t uCount = 4;
	do {
		pOutput[0] = g_NibbleToAsciiUppercase[uInput >> 12U];
		uInput = static_cast<uint16_t>(uInput << 4U);
		++pOutput;
	} while (--uCount);
	// Insert the zero terminator
	pOutput[0] = static_cast<char>(uCount);
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

	\sa NumberToAsciiHex(char *,uint8_t), NumberToAsciiHex(char *,uint16_t),
		NumberToAsciiHex(char *,uint64_t), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint32_t uInput) BURGER_NOEXCEPT
{
	// Push the pointer to the highest value byte
	uint_t uCount = 8;
	do {
		pOutput[0] = g_NibbleToAsciiUppercase[uInput >> 28U];
		uInput = uInput << 4U;
		++pOutput;
	} while (--uCount);
	// Insert the zero terminator
	pOutput[0] = static_cast<char>(uCount);
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

	\sa NumberToAsciiHex(char *,uint8_t), NumberToAsciiHex(char *,uint16_t),
		NumberToAsciiHex(char *,uint32_t), NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint64_t uInput) BURGER_NOEXCEPT
{
	// Push the pointer to the highest value byte
#if defined(BURGER_64BITCPU)
	uint_t uCount = 16;
	do {
		pOutput[0] = g_NibbleToAsciiUppercase[uInput >> 60U];
		uInput = uInput << 4U;
		++pOutput;
	} while (--uCount);
#else
	// For speed, break it up into two 32 bit loops
	// Metrowerks 68K generated bad code with the
	// 64 bit code above.
	uint_t uCount = 8;
	uint32_t uTemp32 = static_cast<uint32_t>(uInput >> 32U);
	do {
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp32 >> 28U];
		uTemp32 = uTemp32 << 4U;
		++pOutput;
	} while (--uCount);
	uCount = 8;
	uTemp32 = static_cast<uint32_t>(uInput);
	do {
		pOutput[0] = g_NibbleToAsciiUppercase[uTemp32 >> 28U];
		uTemp32 = uTemp32 << 4U;
		++pOutput;
	} while (--uCount);
#endif
	// Insert the zero terminator
	pOutput[0] = static_cast<char>(uCount);
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

	\sa NumberToAsciiHex(char *,uint8_t), NumberToAsciiHex(char *,uint16_t),
		NumberToAsciiHex(char *,uint32_t), NumberToAsciiHex(char *,uint64_t),
		NumberToAsciiHex(char *,double),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, float fInput) BURGER_NOEXCEPT
{
	uint32_float_t Convert;
	Convert.f = fInput;
	return NumberToAsciiHex(pOutput, Convert.w);
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

	\sa NumberToAsciiHex(char *,uint8_t), NumberToAsciiHex(char *,uint16_t),
		NumberToAsciiHex(char *,uint32_t), NumberToAsciiHex(char *,uint64_t),
		NumberToAsciiHex(char *,float),
		NumberToAsciiHex(char *,uint32_t,uint_t), or
		NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, double dInput) BURGER_NOEXCEPT
{
	uint64_double_t Convert;
	Convert.d = dInput;
	return NumberToAsciiHex(pOutput, Convert.w);
}

/*! ************************************

	\fn Burger::NumberToAsciiHex(char* pOutput, T input)
	\brief Convert a integral value into a hex string.

	A template to map an integral data type and calls the appropriate
	NumberToAsciiHex function.

	\note This will intentionally fail on any non-integral data type.

	\tparam T Input data type, tested with is_integral.
	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param input Integral value to print in HEX to a string.

***************************************/

/*! ************************************

	\brief Convert an unsigned 32 bit integer into hexadecimal ASCII.

	Convert an unsigned 32 bit integer into a hexadecimal ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
	without accidentally truncating it.

	Examples:<br>
	uInput = 0x1234BCD, uDigits = 4 \-> "4BCD"<br>
	uInput = 0x1234BCD, uDigits = 0 \-> "1234BCD"<br>
	uInput = 0x1234BCD, uDigits = Burger::LEADINGZEROS \-> "01234BCD"<br>
	uInput = 0x2A, uDigits = Burger::LEADINGZEROS+4 \-> "002A"

	\note uDigits is capped at 8 digits. It will not output more than 8 digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 9 bytes in size, whichever is appropriate.
	\param uInput Unsigned 32 bit integer to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput,uint64_t uInput,uint_t uDigits),
		NumberToAscii(char *,uint32_t,uint_t) or
		NumberToAscii(char *,int32_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint32_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// Adjust the input for truncated digits
	uint_t uCount = uDigits & 0xFFU;    // Get the count of digits to process
	if ((uCount - 1U) >= 7U) {          // Test if it's not 1-7
		uCount = 8;                     // 0 and 8+ use the maximum
	} else {                            // Won't show certain digits?
		uInput <<= ((8 - uCount) << 2); // Shift out the unused digits
	}

	// Print the optional characters.
	if ((uCount != 1) && !(uDigits & LEADINGZEROS)) {
		do {
			// Get the highest 4 bits
			if (uInput & (0xFU << 28U)) {
				break;
			}
			// Shift to next digit
			uInput <<= 4;
		} while (--uCount != 1);
	}

	do {
		// Get the highest 4 bits
		const uint_t uLetter = uInput >> 28U;
		// Print the char
		pOutput[0] = g_NibbleToAsciiUppercase[uLetter];
		++pOutput;
		uInput <<= 4;   // Shift to next digit
	} while (--uCount); // Any more left?

	// Add terminating null?
	if (!(uDigits & NOENDINGNULL)) {
		// Terminate the string
		pOutput[0] = static_cast<char>(uCount);
	}
	return pOutput;
}

/*! ************************************

	\brief Convert an unsigned 64 bit integer into hexadecimal ASCII.

	Convert an unsigned 64 bit integer into a hexadecimal ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
	without accidentally truncating it.

	Examples:<br>
	uInput = 0x1234BCD, uDigits = 4 \-> "4BCD"<br>
	uInput = 0x1234BCD, uDigits = 0 \-> "1234BCD"<br>
	uInput = 0x1234BCD, uDigits = Burger::LEADINGZEROS \-> "01234BCD"<br>
	uInput = 0x2A, uDigits = Burger::LEADINGZEROS+4 \-> "002A"

	\note uDigits is capped at 16 digits. It will not output more than 16
	digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 17 bytes in size, whichever is appropriate.
	\param uInput Unsigned 64 bit integer to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput, uint32_t uInput, uint_t uDigits),
		NumberToAscii(char *, uint32_t, uint_t) or
		NumberToAscii(char *, int32_t, uint_t)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, uint64_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// Adjust the input for truncated digits
	uint_t uCount = uDigits & 0xFFU;     // Get the count of digits to process
	if ((uCount - 1U) >= 15U) {          // Test if it's not 1-15
		uCount = 16;                     // 0 and 8+ use the maximum
	} else {                             // Won't show certain digits?
		uInput <<= ((16 - uCount) << 2); // Shift out the unused digits
	}

	// Print the optional characters.
	if ((uCount != 1) && !(uDigits & LEADINGZEROS)) {
		do {
			// Get the highest 4 bits
			if (uInput & (0xFULL << 60U)) {
				break;
			}
			// Shift to next digit
			uInput <<= 4;
		} while (--uCount != 1);
	}

	do {
		// Get the highest 4 bits
		const uint_t uLetter = static_cast<uint_t>(uInput >> 60U);
		// Print the char
		pOutput[0] = g_NibbleToAsciiUppercase[uLetter];
		++pOutput;
		uInput <<= 4;   // Shift to next digit
	} while (--uCount); // Any more left?

	// Add terminating null?
	if (!(uDigits & NOENDINGNULL)) {
		// Terminate the string
		pOutput[0] = static_cast<char>(uCount);
	}
	return pOutput;
}

/*! ************************************

	\brief Convert a 32 bit float into hexadecimal ASCII.

	Convert a 32 bit float into a hexadecimal ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
	without accidentally truncating it.

	\note uDigits is capped at 8 digits. It will not output more than 8 digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 9 bytes in size, whichever is appropriate.
	\param fInput 32 bit float to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput,uint32_t uInput,uint_t uDigits)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, float fInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// Convert the float to bitwise 32 bit integer
	uint32_float_t Convert;
	Convert = fInput;
	return NumberToAsciiHex(pOutput, Convert.w, uDigits);
}

/*! ************************************

	\brief Convert a 64 bit float into hexadecimal ASCII.

	Convert a 64 bit float into a hexadecimal ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
	without accidentally truncating it.

	\note uDigits is capped at 16 digits. It will not output more than 16
		digits.

	\param pOutput Pointer to a buffer to hold the converted integer. It has
		to be with uDigits+1 or 17 bytes in size, whichever is appropriate.
	\param dInput 64 bit float to convert to hexadecimal ASCII.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output. Most
		cases, this is a pointer to the terminating zero.

	\sa NumberToAsciiHex(char *pOutput,uint64_t uInput,uint_t uDigits)

***************************************/

char* BURGER_API Burger::NumberToAsciiHex(
	char* pOutput, double dInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// Convert the double to bitwise 64 bit integer
	uint64_double_t Convert;
	Convert = dInput;
	return NumberToAsciiHex(pOutput, Convert.w, uDigits);
}

/*! ************************************

	\fn Burger::NumberToAsciiHex(char* pOutput, T input, uint_t uDigits)
	\brief Convert a integral value into a hex string.

	A template to map an integral data type and calls the appropriate
	NumberToAsciiHex(char *, T, uint_t uDigits) function.

	\note This will intentionally fail on any non-integral data type.

	\tparam T Input data type, tested with is_integral.
	\param pOutput Pointer to a buffer at least 17 bytes in size.
	\param input Integral value to print in HEX to a string.
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS
		and Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

***************************************/

/*! ************************************

	\brief Convert an unsigned 32 bit integer into ASCII.

	Convert an unsigned 32 bit integer into an ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
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

	\sa Burger::NumberToAsciiHex(char *,uint32_t,uint_t) or
		Burger::NumberToAscii(char *,int32_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, uint32_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// How many digits to process?
	uint_t uIndex = uDigits & 0xFFU;

	// Out of bounds? Or default? (Allow 1-10 only)
	if ((uIndex - 1U) >= 9U) {
		// 10 max digits to process
		uIndex = 10;
	} else {
		// If smaller than the max, hack off the excess with a modulo
		uInput = uInput % g_TensTable32[uIndex];
	}

	// Let's iterate (Index = 1-10)
	if (--uIndex) { // Only iterate on >=10
		do {
			// Get div value in local
			const uint32_t uBigNum = g_TensTable32[uIndex];
			uint_t uLetter = '0';        // Init ASCII value
			if (uInput >= uBigNum) {     // Bigger than the min?
				uDigits |= LEADINGZEROS; // Force future printing
				do {
					uInput -= uBigNum; // Remove value
					++uLetter;         // Inc ASCII value (Max 9 iterations)
				} while (uInput >= uBigNum); // Keep going?
			}

			// Should I print leading zeros?

			if (uDigits & LEADINGZEROS) {                // Already printing?
				pOutput[0] = static_cast<char>(uLetter); // Output the digit
				++pOutput;
			}
			// Any more left?
		} while (--uIndex);
	}

	// Handle the last digit (No divide needed) and it's ALWAYS output.
	pOutput[0] = static_cast<char>(uInput + '0');
	++pOutput;

	// Do I need to write a null?
	if (!(uDigits & NOENDINGNULL)) {
		// Terminate the string
		pOutput[0] = 0;
	}
	// Return the end pointer
	return pOutput;
}

/*! ************************************

	\brief Convert a signed 32 bit integer into ASCII.

	Convert a signed 32 bit integer into an ASCII string. The
	third parameter modifies how the data is formatted.

	Burger::LEADINGZEROS will force leading zeros to be printed.
	Burger::NOENDINGNULL will disable the writing of a terminating zero
	character at the end of the string so an existing string can be modified
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

	\sa Burger::NumberToAscii(char *,uint32_t,uint_t) or
		Burger::NumberToAsciiHex(char *,uint32_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, int32_t iInput, uint_t uDigits) BURGER_NOEXCEPT
{
	if (iInput < 0) {     // Negative?
		pOutput[0] = '-'; // Output the '-' sign.
		++pOutput;
		iInput = -iInput; // Convert to positive
	}
	return NumberToAscii(pOutput, static_cast<uint32_t>(iInput), uDigits);
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

	\sa Burger::NumberToAsciiHex(char *,uint64_t,uint_t) or
		Burger::NumberToAscii(char *,int64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, uint64_t uInput, uint_t uDigits) BURGER_NOEXCEPT
{
	// How many digits to process?
	uint_t uIndex = uDigits & 0xFFU;

	// Out of bounds? Or default? (Allow 1-20 only)
	if ((uIndex - 1U) >= 19U) {
		// 10 max digits to process
		uIndex = 20;
	} else {
		// If smaller than the max, hack off the excess with a modulo
		uInput = uInput % g_TensTable64[uIndex];
	}

	// Let's iterate (Index = 1-20)
	if (--uIndex) { // Only iterate on >=20
		do {
			const uint64_t uBigNum =
				g_TensTable64[uIndex];   // Get div value in local
			uint_t uLetter = '0';        // Init ASCII value
			if (uInput >= uBigNum) {     // Bigger than the min?
				uDigits |= LEADINGZEROS; // Force future printing
				do {
					uInput -= uBigNum; // Remove value
					++uLetter;         // Inc ASCII value (Max 9 iterations)
				} while (uInput >= uBigNum); // Keep going?
			}

			// Should I print leading zeros?

			if (uDigits & LEADINGZEROS) {                // Already printing?
				pOutput[0] = static_cast<char>(uLetter); // Output the digit
				++pOutput;
			}
			// Any more left ?
		} while (--uIndex);
	}

	// Handle the last digit (No divide needed) and it's ALWAYS output.
	pOutput[0] = static_cast<char>(uInput + '0');
	++pOutput;

	// Do I need to write a null?
	if (!(uDigits & NOENDINGNULL)) {
		// Terminate the string
		pOutput[0] = 0;
	}
	// Return the end pointer
	return pOutput;
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

	\sa Burger::NumberToAscii(char *,uint64_t,uint_t) or
		Burger::NumberToAsciiHex(char *,uint64_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, int64_t iInput, uint_t uDigits) BURGER_NOEXCEPT
{
	if (iInput < 0) {     // Negative?
		iInput = -iInput; // Convert to positive
		pOutput[0] = '-'; // Output the '-' sign.
		++pOutput;
	}
	return NumberToAscii(pOutput, static_cast<uint64_t>(iInput), uDigits);
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
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS and
		Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output.
	\sa Burger::NumberToAscii(char *,uint32_t,uint_t) or
		Burger::NumberToAsciiHex(char *,uint32_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, float fInput, uint_t) BURGER_NOEXCEPT
{
#if defined(BURGER_XBOX360) || \
	(defined(BURGER_WINDOWS) && (defined(_MSC_VER) && (_MSC_VER >= 1400)))
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
	\param uDigits Number of digits to convert. Flags Burger::LEADINGZEROS
		and Burger::NOENDINGNULL can be added or or'd to this value to affect
		output.

	\return Pointer to the char immediately after the last char output.

	\sa Burger::NumberToAscii(char *,uint32_t,uint_t) or
	Burger::NumberToAsciiHex(char *,uint32_t,uint_t)

***************************************/

char* BURGER_API Burger::NumberToAscii(
	char* pOutput, double dInput, uint_t) BURGER_NOEXCEPT
{
#if defined(BURGER_XBOX360) || \
	(defined(BURGER_WINDOWS) && (defined(_MSC_VER) && (_MSC_VER >= 1400)))
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
	of 10 characters which could be "4294967295".

	Example: 12345 will return 5, 1000 will return 4, 0 through 9 will return 1.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 10 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(uint64_t) or NumberStringLength(int32_t)

***************************************/

uint_t BURGER_API Burger::NumberStringLength(uint32_t uInput) BURGER_NOEXCEPT
{
	// Perform a binary search for performance

	uint_t uResult;
	if (uInput < 10000U) {      // 1-4 digits?
		if (uInput < 100U) {    // 1-2 digits
			if (uInput < 10U) { // 1 digit?
				uResult = 1;    // 0-9
			} else {
				uResult = 2; // 10-99
			}
		} else if (uInput < 1000U) {
			uResult = 3; // 100-999
		} else {
			uResult = 4; // 1000-9999
		}
	} else if (uInput < 100000000U) { // 5-8 digits?
		if (uInput < 1000000U) {      // 5-6 digits?
			if (uInput < 100000U) {
				uResult = 5; // 10000-99999
			} else {
				uResult = 6; // 100000-999999
			}
		} else if (uInput < 10000000U) { // 7 digits?
			uResult = 7;                 // 1000000-9999999
		} else {
			uResult = 8; // 10000000-99999999
		}
	} else if (uInput < 1000000000U) { // 9-10 digits?
		uResult = 9;                   // 100000000-999999999
	} else {
		uResult = 10; // 1000000000-4294967295
	}
	return uResult;
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 32 bit integers have a minimum of one character to a maximum
	of 10 characters and an extra character for a '-' for negative numbers which
	could be "-2147483648".

	Example: 12345 will return 5, 1000 will return 4, 0 through 9 will return 1.

	\param iInput 32 bit signed integer to test.

	\return 1 through 10 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(uint32_t) or NumberStringLength(int64_t)

***************************************/

uint_t BURGER_API Burger::NumberStringLength(int32_t iInput) BURGER_NOEXCEPT
{
	const uint_t uSign =
		static_cast<uint32_t>(iInput) >> 31U; // 1 if negative, 0 positive
	return uSign + NumberStringLength(static_cast<uint32_t>(absolute(iInput)));
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 64 bit integers have a minimum of one character to a maximum
	of 20 characters "18446744073709551615".

	Example: 12345 will return 5, 1000 will return 4, 0 through 9 will return 1.

	\param uInput 32 bit unsigned integer to test.
	\return 1 through 21 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(int64_t) or NumberStringLength(uint32_t)

***************************************/

uint_t BURGER_API Burger::NumberStringLength(uint64_t uInput) BURGER_NOEXCEPT
{
	// Assume no extra digits
	uint_t uExtraDigits = 0;

	// Is this REALLY a 64 bit value?
	if (uInput >= 0x100000000ULL) {
		// Divide by 1 billion (remove 9 digits)
		uInput = uInput / 1000000000U;
		uExtraDigits = 9;

		// Edge case. Very close to max_int64 will still be > 32 bits after
		// division
		if (uInput >= 0x100000000ULL) {
			uInput = uInput / 100U;
			uExtraDigits = 11;
		}
	}
	// Get the rest of the digit count
	return uExtraDigits + NumberStringLength(static_cast<uint32_t>(uInput));
}

/*! ************************************

	\brief Calculate the length of a string that represents this integer.

	Determine the number of characters are needed to convert the input value
	into a string. 64 bit integers have a minimum of one character to a maximum
	of 20 characters and an extra character for a '-' for negative numbers which
	could be "-9223372036854775808"

	Example: 12345 will return 5, 1000 will return 4, 0 through 9 will return 1.

	\param iInput 64 bit signed integer to test.

	\return 1 through 20 for the number of digits this number requires for
		string conversion.

	\sa NumberStringLength(uint32_t) or NumberStringLength(int64_t)

***************************************/

uint_t BURGER_API Burger::NumberStringLength(int64_t iInput) BURGER_NOEXCEPT
{
	// 1 if negative, 0 positive
	const uint_t uSign =
		static_cast<uint_t>(static_cast<uint64_t>(iInput) >> 63U);
	return uSign + NumberStringLength(static_cast<uint64_t>(absolute(iInput)));
}

/*! ************************************

	\fn Burger::NumberStringLength(T input)
	\brief Calculate the length of a string that represents this integer.

	A template to map an integral data type and calls the appropriate
	NumberStringLength function.

	\note This will intentionally fail on any non-integral data type.

	\tparam T Input data type, tested with is_integral.
	\param input Integral value to determine the string length if converted to
		ASCII.

	\sa NumberStringLength(uint32_t) or NumberStringLength(int32_t)

***************************************/

/*! ************************************

	\brief Calculate the length of a string that represents this hex integer.

	Determine the number of characters are needed to convert the input value
	into a hexadecimal string. 32 bit integers have a minimum of one character
	to a maximum of 8 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 8 for the number of digits this number requires for string
		conversion.

	\sa NumberHexStringLength(uint64_t)

***************************************/

uint_t BURGER_API Burger::NumberHexStringLength(uint32_t uInput) BURGER_NOEXCEPT
{
	uint_t uDigitCount = 1;

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

	\sa NumberHexStringLength(uint32_t)

***************************************/

uint_t BURGER_API Burger::NumberHexStringLength(uint64_t uInput) BURGER_NOEXCEPT
{
	uint_t uDigitCount = 1;

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

	\fn Burger::NumberHexStringLength(T input)
	\brief Calculate the length of a string that represents this hex integer.

	A template to map an integral data type and calls the appropriate
	NumberHexStringLength function.

	\note This will intentionally fail on any non-integral data type.

	\tparam T Input data type, tested with is_integral.
	\param input Integral value to determine the string length if converted to
		hex ASCII.

	\sa NumberHexStringLength(uint32_t) or NumberHexStringLength(uint64_t)

***************************************/

/*! ************************************

	\brief Calculate the length of a string that represents this octal integer.

	Determine the number of characters are needed to convert the input value
	into a octal string. 32 bit integers have a minimum of one character to a
	maximum of 11 characters.

	\param uInput 32 bit unsigned integer to test.

	\return 1 through 11 for the number of digits this number requires for
		string conversion.

	\sa NumberOctalStringLength(uint64_t)

***************************************/

uint_t BURGER_API Burger::NumberOctalStringLength(
	uint32_t uInput) BURGER_NOEXCEPT
{
	uint_t uDigitCount = 1;

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

	\sa NumberOctalStringLength(uint32_t)

***************************************/

uint_t BURGER_API Burger::NumberOctalStringLength(
	uint64_t uInput) BURGER_NOEXCEPT
{
	uint_t uDigitCount = 1;

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

/*! ************************************

	\fn Burger::NumberOctalStringLength(T input)
	\brief Calculate the length of a string that represents this octal integer.

	A template to map an integral data type and calls the appropriate
	NumberOctalStringLength function.

	\note This will intentionally fail on any non-integral data type.

	\tparam T Input data type, tested with is_integral.
	\param input Integral value to determine the string length if converted to
		octal ASCII.

	\sa NumberOctalStringLength(uint32_t) or NumberOctalStringLength(uint64_t)

***************************************/
