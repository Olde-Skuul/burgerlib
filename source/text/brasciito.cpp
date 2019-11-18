/***************************************

	ASCII to number functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brasciito.h"
#include "brfloatingpoint.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

/*! ************************************

	\brief Convert an ASCII string into an integer.

	Take a string formatted as a simple integer number a hex number of the form
	$5123 or 0x1234 with or without a preceding '-' sign and convert it into a
	32 bit integer value.

	Overflow is detected and is considered an error condition. 0xFFFFFFFFU will
	be returned in this case.

	\param pInput Pointer to the string to convert. NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput
		(Error) of pInput moved ahead until a non-numeric character was found.
		This can be NULL if the application doesn't require the location of the
		string after the integer was parsed.

	\return A 32 bit integer that represents the ASCII string. Will be
		0xFFFFFFFF if an overflow occurred and 0 if nothing could be parsed.

	\sa Burger::AsciiToFloat(const char *,const char **) or
		Burger::AsciiToDouble(const char *,const char **)

***************************************/

Word32 BURGER_API Burger::AsciiToInteger(const char* pInput, const char** pDest) BURGER_NOEXCEPT
{
	if (pDest) {
		pDest[0] = pInput; // Assume I don't accept any input (Error condition)
	}

	uint_t uLetter; // Temp ASCII char

	// Parse away whitespace and the '-' flag.

    uint_t uNegate = 0; // Don't negate the result (Assume positive)
	do {
		uLetter = reinterpret_cast<const uint8_t*>(pInput)[0]; // Get char
		++pInput;
		// Negate it?
		if (uLetter == '-') {
			// Toggle it, so - - works. (Foo^-1)-(-1) = -Foo
			uNegate ^= static_cast<uint_t>(-1);
			uLetter = ' '; // Force staying in the loop
		}
	} while ((uLetter == ' ') || (uLetter == '\t') ||
		(uLetter == '+')); // Eat white space

	if (uLetter != '$') { // Hex input?
		if ((uLetter == '0') &&
			!((reinterpret_cast<const uint8_t*>(pInput)[0] ^ 'X') &
				(~0x20U))) { // 0X or 0x for "C" style input
			++pInput;		 // Skip the x and go to the hex parser
		} else {

			// Here's the base 10 code. Common case

            uint32_t uValue10 = g_AsciiToWord8Table[uLetter];
			if (uValue10 < 10) { // First char valid?
                uint_t uAscii10 = reinterpret_cast<const uint8_t*>(pInput)[0];
				uLetter = g_AsciiToWord8Table[uAscii10];
				if (uLetter < 10) { // Second char valid?
					do {
						++pInput;
						// Check for overflow
						if ((uValue10 >
								0x19999999) || // 0xFFFFFFFFU/10 = 0x19999999
							((uValue10 == 0x19999999) && (uLetter >= 6))) {
							return 0xFFFFFFFFU; // Tilt!
						}
						// Process into the total
						uValue10 = (uValue10 * 10) + uLetter;
						// Convert char to value
						uAscii10 = reinterpret_cast<const uint8_t*>(pInput)[0];
						uLetter = g_AsciiToWord8Table[uAscii10];
					} while (uLetter < 10);
				}

				// Okay, I've got a valid value

				uValue10 = (uValue10 ^ uNegate) - uNegate; // Perform negation?

				// Does the caller want the end address?

				if (pDest) {
					// Skip past trailing white space

					if (uAscii10 == ' ' || uAscii10 == '\t') {
						do {
							++pInput;
							// Remove whitespace
							uAscii10 =
								reinterpret_cast<const uint8_t*>(pInput)[0];
						} while ((uAscii10 == ' ') || (uAscii10 == '\t'));
					}
					pDest[0] = pInput; // Store the ASCII address
				}
				return uValue10; // Return the result
			}
			return 0; // No input, error
		}
	}

	// Base 16

	{
		uint32_t uValue16 =
			g_AsciiToWord8Table[reinterpret_cast<const uint8_t*>(pInput)[0]];
		if (uValue16 < 16) {
			++pInput;
            uint_t uAscii16 = reinterpret_cast<const uint8_t*>(pInput)[0];
			uLetter = g_AsciiToWord8Table[uAscii16];
			if (uLetter < 16) { // Second char valid?
				do {
					++pInput;
					if (uValue16 >= 0x10000000) {
						return 0xFFFFFFFFU; // Tilt
					}
					uValue16 = (uValue16 << 4) + uLetter;
					// Convert char to value
					uAscii16 = reinterpret_cast<const uint8_t*>(pInput)[0];
					uLetter = g_AsciiToWord8Table[uAscii16];
				} while (uLetter < 16);
			}

			// Perform negation?
			uValue16 = (uValue16 ^ uNegate) - uNegate;

			// Does the caller want the end address?

			if (pDest) {
				// Skip past trailing white space

				if (uAscii16 == ' ' || uAscii16 == '\t') {
					do {
						++pInput;
						uAscii16 = reinterpret_cast<const uint8_t*>(
							pInput)[0]; // Remove whitespace
					} while ((uAscii16 == ' ') || (uAscii16 == '\t'));
				}
				pDest[0] = pInput; // Store the ASCII address
			}
			return uValue16; // Return the result
		}
	}
	return 0; // Bad value
}

/*! ************************************

	\brief Return a signed integer value

	Scan the value string as a 32 bit signed integer or hex value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form of $1234 and 0x1234. 0xFFFFFFFF
	will be converted to -1.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value

	\return Value in between iMin and iMax or iDefault

	\sa AsciiToInteger(const char *,const char **) or AsciiToWord(const char
		*,Word,Word,Word)

***************************************/

Int BURGER_API Burger::AsciiToInteger(
	const char* pInput, Int iDefault, Int iMin, Int iMax)
{
	if (pInput) {
		const char* pDest;
		// Parse as signed
		Int iValue = static_cast<Int>(AsciiToInteger(pInput, &pDest));
		if (pDest != pInput) {
			// Do a signed bounds check
			if (iValue < iMin) {
				iDefault = iMin;
			} else if (iValue > iMax) {
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

	Scan the value string as a 32 bit integer or hex value and if successful,
	return \ref TRUE.

	Hex strings are acceptable input in the form of $1234 and 0x1234. 0xFFFFFFFF
	will be converted to -1.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the
		default

	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string
		was not a number

	\sa AsciiToInteger(const char *,const char **) or AsciiToInteger(const
		char *,Int,Int,Int)

***************************************/

Word BURGER_API Burger::AsciiToInteger(Word32* pOutput, const char* pInput)
{
	const char* pDest;
	// Convert the text
	Word32 uOutput = AsciiToInteger(pInput, &pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest == pInput) {
		uResult = FALSE;
		uOutput = 0; // Clear the output
	}
	pOutput[0] = uOutput;
	return uResult; // Return the result
}

/*! ************************************

	\brief Return an unsigned integer value

	Scan the value string as a 32 bit unsigned integer or hex value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form of $1234 and 0x1234

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value

	\return Value in between uMin and uMax or uDefault

	\sa AsciiToInteger(const char *,const char **) or AsciiToInteger(const char
		*,Int,Int,Int)

***************************************/

Word BURGER_API Burger::AsciiToWord(
	const char* pInput, Word uDefault, Word uMin, Word uMax)
{
	if (pInput) {
		// Convert from UTF-8
		const char* pDest;
		Word32 uValue = AsciiToInteger(pInput, &pDest);
		// Anything parsed?
		if (pDest != pInput) {
			// Bounds test
			if (uValue < uMin) {
				uDefault = uMin;
			} else if (uValue > uMax) {
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

	Take a string formatted as a simple integer number, a hex number of the form
	$5123 or 0x1234 with or without a preceding '-' sign and convert it into a
	64 bit integer value.

	Overflow is detected and is considered an error condition.
	0xFFFFFFFFFFFFFFFFU will be returned in this case.

	\param pInput Pointer to the string to convert. NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput
		(Error) of pInput moved ahead until a non-numeric character was found.
		This can be NULL if the application doesn't require the location of the
		string after the integer was parsed.

	\return A 64 bit integer that represents the ASCII string. Will be
		0xFFFFFFFF if an overflow occurred and 0 if nothing could be parsed.

	\sa Burger::AsciiToFloat(const char *,const char **) or
		Burger::AsciiToDouble(const char *,const char **)

***************************************/

Word64 BURGER_API Burger::AsciiToInteger64(
	const char* pInput, const char** pDest)
{
	if (pDest) {
		pDest[0] = pInput; // Assume I don't accept any input (Error condition)
	}

	Word uLetter; // Temp ASCII char

	// Parse away whitespace and the '-' flag.

	Word64 uNegate = 0; // Don't negate the result (Assume positive)
	do {
		uLetter = reinterpret_cast<const Word8*>(pInput)[0]; // Get char
		++pInput;
		if (uLetter == '-') {			 // Negate it?
			uNegate ^= BURGER_MAXUINT64; // Toggle it, so - - works.
										 // (Foo^-1)-(-1) = -Foo
			uLetter = ' ';				 // Force staying in the loop
		}
		// Eat white space
	} while ((uLetter == ' ') || (uLetter == '\t') || (uLetter == '+'));

	if (uLetter != '$') { // Hex input?
		if ((uLetter == '0') &&
			!((reinterpret_cast<const Word8*>(pInput)[0] ^ 'X') &
				(~0x20U))) { // 0X or 0x for "C" style input
			++pInput;		 // Skip the x and go to the hex parser
		} else {

			// Here's the base 10 code. Common case

			Word64 uValue10 = g_AsciiToWord8Table[uLetter];
			if (uValue10 < 10) { // First char valid?
				Word uAscii10 = reinterpret_cast<const Word8*>(pInput)[0];
				uLetter = g_AsciiToWord8Table[uAscii10];
				if (uLetter < 10) { // Second char valid?
					do {
						++pInput;
						// Check for overflow
						if ((uValue10 >
								0x1999999999999999ULL) || // 0xFFFFFFFFFFFFFFFFULL/10
														  // =
														  // 0x1999999999999999
							((uValue10 == 0x1999999999999999ULL) &&
								(uLetter >= 6))) {
							return BURGER_MAXUINT64; // Tilt!
						}
						uValue10 =
							(uValue10 * 10) + uLetter; // Process into the total
													   // Convert char to value
						uAscii10 = reinterpret_cast<const Word8*>(pInput)[0];
						uLetter = g_AsciiToWord8Table[uAscii10];
					} while (uLetter < 10);
				}

				// Okay, I've got a valid value

				uValue10 = (uValue10 ^ uNegate) - uNegate; // Perform negation?

				// Does the caller want the end address?

				if (pDest) {
					// Skip past trailing white space

					if (uAscii10 == ' ' || uAscii10 == '\t') {
						do {
							++pInput;
							uAscii10 = reinterpret_cast<const Word8*>(
								pInput)[0]; // Remove whitespace
						} while ((uAscii10 == ' ') || (uAscii10 == '\t'));
					}
					pDest[0] = pInput; // Store the ASCII address
				}
				return uValue10; // Return the result
			}
			return 0; // No input, error
		}
	}

	// Base 16

	{
		Word64 uValue16 =
			g_AsciiToWord8Table[reinterpret_cast<const Word8*>(pInput)[0]];
		if (uValue16 < 16) {
			++pInput;
			Word uAscii16 = reinterpret_cast<const Word8*>(pInput)[0];
			uLetter = g_AsciiToWord8Table[uAscii16];
			if (uLetter < 16) { // Second char valid?
				do {
					++pInput;
					if (uValue16 >= 0x1000000000000000ULL) {
						return BURGER_MAXUINT64; // Tilt
					}
					uValue16 = (uValue16 << 4U) + uLetter;
					// Convert char to value
					uAscii16 = reinterpret_cast<const Word8*>(pInput)[0];
					uLetter = g_AsciiToWord8Table[uAscii16];
				} while (uLetter < 16);
			}

			// Perform negation?
			uValue16 = (uValue16 ^ uNegate) - uNegate;

			// Does the caller want the end address?

			if (pDest) {
				// Skip past trailing white space

				if (uAscii16 == ' ' || uAscii16 == '\t') {
					do {
						++pInput;
						uAscii16 = reinterpret_cast<const Word8*>(
							pInput)[0]; // Remove whitespace
					} while ((uAscii16 == ' ') || (uAscii16 == '\t'));
				}
				pDest[0] = pInput; // Store the ASCII address
			}
			return uValue16; // Return the result
		}
	}
	return 0; // Bad value
}

/*! ************************************

	\brief Return a signed integer value

	Scan the value string as a 64 bit signed integer or hex value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form of $1234 and 0x1234.
	0xFFFFFFFFFFFFFFFF will be converted to -1.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value

	\return Value in between iMin and iMax or iDefault

	\sa AsciiToInteger64(const char *,const char **) or AsciiToWord64(const char
		*,Word64,Word64,Word64)

***************************************/

Int64 BURGER_API Burger::AsciiToInteger64(
	const char* pInput, Int64 iDefault, Int64 iMin, Int64 iMax)
{
	if (pInput) {
		const char* pDest;
		// Parse as signed
		Int64 iValue = static_cast<Int64>(AsciiToInteger64(pInput, &pDest));
		if (pDest != pInput) {
			// Do a signed bounds check
			if (iValue < iMin) {
				iDefault = iMin;
			} else if (iValue > iMax) {
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

	Scan the value string as a 64 bit integer or hex value and if successful,
	return \ref TRUE.

	Hex strings are acceptable input in the form of $1234 and 0x1234.
	0xFFFFFFFFFFFFFFFF will be converted to -1.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the
		default

	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string
		was not a number

	\sa AsciiToInteger64(const char *,const char **) or AsciiToInteger64(const
		char *,Int64,Int64,Int64)

***************************************/

Word BURGER_API Burger::AsciiToInteger64(Word64* pOutput, const char* pInput)
{
	const char* pDest;
	// Convert the text
	Word64 uOutput = AsciiToInteger64(pInput, &pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest == pInput) {
		uResult = FALSE;
		uOutput = 0; // Clear the output
	}
	pOutput[0] = uOutput;
	return uResult; // Return the result
}

/*! ************************************

	\brief Return an unsigned integer value

	Scan the value string as a 64 bit unsigned integer or hex value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form of $1234 and 0x1234

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value

	\return Value in between uMin and uMax or uDefault

	\sa AsciiToInteger64(const char *,const char **) or AsciiToInteger64(const
		char *,Int64,Int64,Int64)

***************************************/

Word64 BURGER_API Burger::AsciiToWord64(
	const char* pInput, Word64 uDefault, Word64 uMin, Word64 uMax)
{
	if (pInput) {
		// Convert from UTF-8
		const char* pDest;
		Word64 uValue = AsciiToInteger64(pInput, &pDest);
		// Anything parsed?
		if (pDest != pInput) {
			// Bounds test
			if (uValue < uMin) {
				uDefault = uMin;
			} else if (uValue > uMax) {
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

static const float PowerOf10FltTable[6] = { // 6 entries, since 38 is the max
	1e1f, 1e2f, 1e4f, 1e8f, 1e16f, 1e32f};

/*! ************************************

	\brief Convert an ASCII string into a floating point number.

	Take a floating point number of the format (+/-)303.030e(+/-)1 and turn it
	into a float that is the equivalent.

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
	\param pDest Pointer to receive the character beyond the last character
		parsed. Will equal pInput on error.

	\return The number as a float.

	\sa Burger::AsciiToDouble(const char *,const char **) or
		Burger::AsciiToInteger(const char *,const char **)

***************************************/

float BURGER_API Burger::AsciiToFloat(const char* pInput, const char** pDest)
{
	if (pDest) {
		pDest[0] = pInput; // Assume an error occurs
	}

	// Skip white space

	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;
	if ((uTemp == ' ') || (uTemp == '\t')) {
		do {
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;
		} while ((uTemp == ' ') || (uTemp == '\t'));
	}

	// Now, see if the first character is the sign (+, -)

	float fNegative = 1.0f; // Assume positive
	if (uTemp == '+') {		// Just eat the + character
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	} else if (uTemp == '-') { // Ah, it's negative
		fNegative = -1.0f;	 // Negate the answer
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	}

	// Is this NAN or INF?

	if (!((uTemp ^ 'N') & (~0x20))) { // n or N
		Word uT2 = reinterpret_cast<const Word8*>(pInput)[0];
		if (!((uT2 ^ 'A') & (~0x20))) { // a or A
			uT2 = reinterpret_cast<const Word8*>(pInput)[1];
			if (!((uT2 ^ 'N') & (~0x20))) { // n or N
				float fResultNan = Burger::g_fNan;

				// Any math operation on a NAN will do nothing.
				// So, I use the negate instruction, which will work.
				if (fNegative < 0) {
					fResultNan = -fResultNan;
				}
				if (pDest) {
					pDest[0] = pInput + 2;
				}
				return fResultNan;
			}
		}
	} else if (!((uTemp ^ 'I') & (~0x20))) { // I or i
		Word uT3 = reinterpret_cast<const Word8*>(pInput)[0];
		if (!((uT3 ^ 'N') & (~0x20))) { // N or n
			uT3 = reinterpret_cast<const Word8*>(pInput)[1];
			if (!((uT3 ^ 'F') & (~0x20))) { // F or f
				float ResultInf = Burger::g_fInf;
				ResultInf *= fNegative;
				if (pDest) {
					pDest[0] = pInput + 2;
				}
				return ResultInf;
			}
		}
	}

	// Now, lets parse xxx.xxx
	// I'm going to make it easy on myself.
	// I'll copy the digits, sans the period and all leading and trailing zeros.
	// I will parse a maximum of 9 digits

	Word uDigitCount = 0;	 // Number of digits in my buffer
	Word uFractionDigits = 0; // Nothing parsed after the period
	Word bNumberFound =
		FALSE; // Set to true if a number of any kind (Like all zeros) was found
	Word8 DigitBuffer[9]; // Buffer for digits converted to 0-9 (DO NOT CHANGE
						  // THE SIZE!)
	for (;;) {
		if (uTemp == '.') {		   // Is this a period?
			if (uFractionDigits) { // Already got one?
				break;			   // Can't do double periods!
			}
			uFractionDigits =
				1; // Start the counter by setting this to non-zero
		} else {
			Word uVal = uTemp - '0'; // Precompute
			if (uVal >=
				10) { // Only take numbers (Also breaks on null terminator)
				break;
			}
			bNumberFound = TRUE;   // I got something
			if (uFractionDigits) { // After the period?
				++uFractionDigits; // Add to total (Note, all digits AFTER the
								   // period count
			}
			if (uDigitCount || uVal) { // Suppress leading zeros
				if (uDigitCount < sizeof(DigitBuffer)) { // Buffer overrun? Bad.
					DigitBuffer[uDigitCount] = static_cast<Word8>(uVal);
				}
				++uDigitCount;
			}
		}
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	}

	// Did I parse ANY number?
	// -.e1 is NOT considered a floating point number
	// Accept NOTHING and return 0.0f as my failure condition

	if (!bNumberFound) {
		return 0.0f; // Forget it. Error.
	}

	// At this point, I've parsed up to 9 digits
	// I need to get the exponent

	int iExponent = 0;				  // Assume no extra shifting of the digits
	if (!((uTemp ^ 'E') & (~0x20))) { // E or e Exponent token?
		const char* pInput2 = pInput; // Save the mark

		uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
		++pInput2;
		Word NegExponent = FALSE; // Assume a positive exponent
		if (uTemp == '+') {
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		} else if (uTemp == '-') {
			NegExponent = TRUE;
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		}
		uTemp -= '0';
		if (uTemp < 10) {
			do {
				if (iExponent < 100) { // Prevent overflow
					iExponent = (iExponent * 10) + static_cast<int>(uTemp);
				}
				uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
				++pInput2;
				uTemp -= '0';
			} while (uTemp < 10);

			// Since I got an e102 or e-1, I'll accept this input
			pInput = pInput2;
			// This is the only place where exponent could be non-zero
			if (NegExponent) {
				iExponent = -iExponent;
			}
		}
	}

	// Mark the requested ASCII marker

	if (pDest) {
		pDest[0] = pInput -
			1; // Back step 1 char, but this is where I stopped parsing.
	}

	// Since I've got an exponent and up to 9 digits of mantissa,
	// let's make a floating point number.

	if (uFractionDigits) { // Did I find a period?
		// Adjust the exponent for how many digits
		// there were to the right of the period
		iExponent -=
			uFractionDigits - 1; // (The +1 was the flag that I got something)
	}

	// I counted the number of digits, but the buffer may not
	// hold them all. Add the truncated count to the exponent

	if (uDigitCount > sizeof(DigitBuffer)) { // Clipped?
		iExponent += uDigitCount -
			static_cast<Word>(sizeof(DigitBuffer)); // Add the clipped count

		// Force my counter for parsing to the right amount
		uDigitCount = static_cast<Word>(sizeof(DigitBuffer));
	}

	// Dispose of trailing zeros (It helps with precision)

	if (uDigitCount) {
		do {
			if (DigitBuffer[uDigitCount - 1]) { // Last digit, non-zero?
				break;							// Don't touch it
			}
			++iExponent;		 // Adjust the exponent
		} while (--uDigitCount); // Count down
		if (!uDigitCount) {		 // Did I remove all of the digits?
			return 0.0f;		 // 0 exponent anything, is still zero
		}
	}

	// Since we've got something to be converted into
	// a number, let's parse it.

	const Word8* pBuffer = DigitBuffer;

	Int32 iLow9 = 0; // Low 9 digits
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

	if (iExponent) { // Do I bother?
		if (iExponent < 0) {

			// I'm going to shrink the number via division

			// I'm performing the division operation instead of
			// the reciprocal multiplies because I want to retain
			// as much precision as I can.

			Word uCount1 = static_cast<Word>(-iExponent); // Get abs value

			// This is for really stupid numbers

			if (uCount1 >= 38) {
				fResult /= 1e38f;
				uCount1 -= 38;
			}
			if (uCount1) {
				const float* pPower1 = PowerOf10FltTable;
				float fScalar1 = 1.0f;
				do {
					if (uCount1 & 1) { // Using a double table, I'll get the
									   // right power of 10
						fScalar1 *= pPower1[0];
					}
					++pPower1;
					uCount1 >>= 1;
				} while (uCount1);   // Keep going until I'm done
				fResult /= fScalar1; // Shrink the number
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
				const float* pPower2 = PowerOf10FltTable;
				float fScalar2 = 1.0f;
				do {
					if (uCount2 & 1) {
						fScalar2 *= pPower2[0];
					}
					++pPower2;
					uCount2 >>= 1;
				} while (uCount2);
				fResult *= fScalar2;
			}
		}
	}

	// Okay, I've got the number. Detect overflow

	iExponent += uDigitCount;
	if (iExponent > (38 + 1)) {   // Was the exponent nuts?
		fResult = Burger::g_fMax; // Grab a constant
	} else if (iExponent <
		((-38) + 1)) { // Was the exponent nuts in the other direction?
		fResult = Burger::g_fMin; // Grab a constant
	}
	return fResult * fNegative; // Return the answer
}

/***************************************

	Power of 10 table with the powers doubling with each entry

	I use it to perform a binary multiply to cut down on the number of
	multiplies I need to do before I'm done scaling the number

	The "e" is the number of zeros following the 1
	1e1 = 10, 1e2 = 100, 1e4 = 10000 etc...

***************************************/

static const double PowerOf10DblTable[9] = { // 9 entries, since 308 is the max
	1e1, 1e2, 1e4, 1e8, 1e16, 1e32, 1e64, 1e128, 1e256};

/*! ************************************

	\brief Convert an ASCII string into a floating point number.

	Take a floating point number of the format (+/-)303.030e(+/-)1 and turn it
	into a double that is the equivalent.

	If a number is parsed, the character at the end of the string after
	whitespace is removed is the one pointed by pDest.

	Examples:<br>
	1.0<br>
	1.666<br>
	1666.4<br>
	1e3 -> 1000<br>
	NAN -> NAN<br>
	INF -> INF<br>

	\note A 64 bit floating point number is of the range 1.797693134862316e+308
		(Max) to 2.225073858507201e-308 (min).

	\note This function is not recommended for use on the PlayStation 2 due to
		its heavy use of double operations. Use Burger::AsciiToFloat(const char
		*,const char **) instead.

	\param pInput Pointer to the ASCII string with the number to parse.
	\param pDest Pointer to receive the character beyond the last character
		parsed. Will equal pInput on error.

	\return The number as a double.

	\sa Burger::AsciiToFloat(const char *,const char **) or
		Burger::AsciiToInteger(const char *,const char **)

***************************************/

double BURGER_API Burger::AsciiToDouble(const char* pInput, const char** pDest)
{
	if (pDest) {
		pDest[0] = pInput; // Assume an error occurs
	}

	// Skip white space

	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	++pInput;
	if ((uTemp == ' ') || (uTemp == '\t')) {
		do {
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];
			++pInput;
		} while ((uTemp == ' ') || (uTemp == '\t'));
	}

	// Now, see if the first character is the sign (+, -)

	double dNegative = 1.0; // Assume positive
	if (uTemp == '+') {		// Just eat the + character
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	} else if (uTemp == '-') { // Ah, it's negative
		dNegative = -1.0;	  // Negate the answer
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	}

	// Is this NAN or INF?

	if (!((uTemp ^ 'N') & (~0x20))) { // N or n
		Word uT2 = reinterpret_cast<const Word8*>(pInput)[0];
		if (!((uT2 ^ 'A') & (~0x20))) { // A or a
			uT2 = reinterpret_cast<const Word8*>(pInput)[1];
			if (!((uT2 ^ 'N') & (~0x20))) { // N or n
				double dResultNan = Burger::g_dNan;

				// Any math operation on a NAN will do nothing.
				// So, I use the negate instruction, which will work.
				if (dNegative < 0) {
					dResultNan = -dResultNan;
				}
				if (pDest) {
					pDest[0] = pInput + 2;
				}
				return dResultNan;
			}
		}
	} else if (!((uTemp ^ 'I') & (~0x20))) { // I or i
		Word uT3 = reinterpret_cast<const Word8*>(pInput)[0];
		if (!((uT3 ^ 'N') & (~0x20))) { // N or n
			uT3 = reinterpret_cast<const Word8*>(pInput)[1];
			if (!((uT3 ^ 'F') & (~0x20))) { // F or f
				double dResultInf = Burger::g_dInf;
				dResultInf *= dNegative;
				if (pDest) {
					pDest[0] = pInput + 2;
				}
				return dResultInf;
			}
		}
	}

	// Now, lets parse xxx.xxx
	// I'm going to make it easy on myself.
	// I'll copy the digits, sans the period and all leading and trailing zeros.
	// I will parse a maximum of 18 digits

	Word uDigitCount = 0;	 // Number of digits in my buffer
	Word uFractionDigits = 0; // Nothing parsed after the period
	Word bNumberFound =
		FALSE; // Set to true if a number of any kind (Like all zeros) was found
	Word8 DigitBuffer[18]; // Buffer for digits converted to 0-9 (DO NOT CHANGE
						   // THE SIZE!)
	for (;;) {
		if (uTemp == '.') {		   // Is this a period?
			if (uFractionDigits) { // Already got one?
				break;			   // Can't do double periods!
			}
			uFractionDigits =
				1; // Start the counter by setting this to non-zero
		} else {
			Word uVal = uTemp - '0'; // Precompute
			if (uVal >=
				10) { // Only take numbers (Also breaks on null terminator)
				break;
			}
			bNumberFound = TRUE;   // I got something
			if (uFractionDigits) { // After the period?
				++uFractionDigits; // Add to total (Note, all digits AFTER the
								   // period count
			}
			if (uDigitCount || uVal) { // Suppress leading zeros
				if (uDigitCount < sizeof(DigitBuffer)) { // Buffer overrun? Bad.
					DigitBuffer[uDigitCount] = static_cast<Word8>(uVal);
				}
				++uDigitCount;
			}
		}
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
		++pInput;
	}

	// Did I parse ANY number?
	// -.e1 is NOT considered a floating point number
	// Accept NOTHING and return 0.0 as my failure condition

	if (!bNumberFound) {
		return 0.0; // Forget it. Error.
	}

	// At this point, I've parsed up to 18 digits
	// I need to get the exponent

	int iExponent = 0;				  // Assume no extra shifting of the digits
	if (!((uTemp ^ 'E') & (~0x20))) { // E or e Exponent token?
		const char* pInput2 = pInput; // Save the mark

		uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
		++pInput2;
		Word bNegExponent = FALSE; // Assume a positive exponent
		if (uTemp == '+') {
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		} else if (uTemp == '-') {
			bNegExponent = TRUE;
			uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
			++pInput2;
		}
		uTemp -= '0';
		if (uTemp < 10) {
			do {
				if (iExponent < 1000) { // Prevent overflow
					iExponent = (iExponent * 10) + static_cast<int>(uTemp);
				}
				uTemp = reinterpret_cast<const Word8*>(pInput2)[0];
				++pInput2;
				uTemp -= '0';
			} while (uTemp < 10);

			pInput =
				pInput2; // Since I got an e102 or e-1, I'll accept this input
			// This is the only place where exponent could be non-zero
			if (bNegExponent) {
				iExponent = -iExponent;
			}
		}
	}

	// Mark the requested ASCII marker

	if (pDest) {
		pDest[0] = pInput -
			1; // Back step 1 char, but this is where I stopped parsing.
	}

	// Since I've got an exponent and up to 18 digits of mantissa,
	// let's make a floating point number.

	if (uFractionDigits) { // Did I find a period?
		// Adjust the exponent for how many digits
		// there were to the right of the period
		iExponent -=
			uFractionDigits - 1; // (The +1 was the flag that I got something)
	}

	// I counted the number of digits, but the buffer may not
	// hold them all. Add the truncated count to the exponent

	if (uDigitCount > sizeof(DigitBuffer)) { // Clipped?
		iExponent += uDigitCount -
			static_cast<Word>(sizeof(DigitBuffer)); // Add the clipped count

		// Force my counter for parsing to the right amount
		uDigitCount = static_cast<Word>(sizeof(DigitBuffer));
	}

	// Dispose of trailing zeros (It helps with precision)

	if (uDigitCount) {
		do {
			if (DigitBuffer[uDigitCount - 1]) { // Last digit, non-zero?
				break;							// Don't touch it
			}
			++iExponent;		 // Adjust the exponent
		} while (--uDigitCount); // Count down
		if (!uDigitCount) {		 // Did I remove all of the digits?
			return 0.0;			 // 0 exponent anything, is still zero
		}
	}

	// Since we've got something to be converted into a number, let's parse it.
	// PROBLEM: A 32 bit long can only store 9 digits safely, so I take the 18
	// digits I have and split them into two longs of 9 digits each. Then I'll
	// merge the two using the double unit.

	const Word8* pBuffer = DigitBuffer;
	Int32 iHigh9 = 0;	  // High 9 digits
	if (uDigitCount > 9) { // 1-18
		// Parse up to 9 digits
		Word i1 = uDigitCount - 9; // 1-9
		do {
			iHigh9 = (iHigh9 * 10) + static_cast<Int32>(pBuffer[0]);
			++pBuffer;
		} while (--i1);
	}

	Int32 iLow9 = 0; // Low 9 digits
	Word i2 = uDigitCount;
	if (i2 > 9) {
		i2 = 9; // Clamp
	}
	if (i2) {
		do {
			iLow9 = (iLow9 * 10) + static_cast<Int32>(pBuffer[0]);
			++pBuffer;
		} while (--i2);
	}

	// Merge the two 32 bit integers into a 64 bit float

	double dResult =
		(static_cast<double>(iHigh9) * 1e9) + static_cast<double>(iLow9);

	// Apply the exponent, if any.

	if (iExponent) { // Do I bother?
		if (iExponent < 0) {

			// I'm going to shrink the number via division

			// I'm performing the division operation instead of the reciprocal
			// multiplies because I want to retain as much precision as I can.

			Word uCount1 = static_cast<Word>(-iExponent); // Get abs value

			// This is for really stupid numbers

			if (uCount1 >= 308) {
				dResult /= 1e308;
				uCount1 -= 308;
			}
			if (uCount1) {
				const double* pPower1 = PowerOf10DblTable;
				double dScalar1 = 1.0;
				do {
					if (uCount1 & 1) { // Using a double table, I'll get the
									   // right power of 10
						dScalar1 *= pPower1[0];
					}
					++pPower1;
					uCount1 >>= 1;
				} while (uCount1);   // Keep going until I'm done
				dResult /= dScalar1; // Shrink the number
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
				const double* pPower2 = PowerOf10DblTable;
				double dScalar2 = 1.0;
				do {
					if (uCount2 & 1) {
						dScalar2 *= pPower2[0];
					}
					++pPower2;
					uCount2 >>= 1;
				} while (uCount2);
				dResult *= dScalar2;
			}
		}
	}

	// Okay, I've got the number. Detect overflow

	iExponent += uDigitCount;
	if (iExponent > (308 + 1)) { // Was the exponent nuts?
		dResult = g_dMax;		 // Grab a constant
	} else if (iExponent <
		((-308) + 1)) {   // Was the exponent nuts in the other direction?
		dResult = g_dMin; // Grab a constant
	}
	return dResult * dNegative; // Return the answer
}

/*! ************************************

	\brief Convert an ASCII string into a boolean.

	If the case insensitive string matches yes, no, true or false, return
	1,0,1,0 in that order. Otherwise, parse it as a number and return 1 if it's
	non-zero and 0 if it's zero or invalid.

	String matching does not look for ending whitespace, so true and true value
	will both match as "true" and return 1. The pDest return value will be moved
	forward by four characters in both cases.

	\param pInput Pointer to the string to convert. \ref NULL will page fault.
	\param pDest Pointer to a const char * that will either be given pInput
		(Error) of pInput moved ahead until a non-numeric character was found.
		This can be \ref NULL if the application doesn't require the location of
		the string after the integer was parsed.

	\return 0 if no, false, ASCII 0 or invalid input. 1 if yes, true or a
		non-zero numeric value.

	\sa AsciiToInteger(const char *,const char **) or AsciiToBoolean(const char
		*,Word)

***************************************/

#if !defined(DOXYGEN)
struct BooleanLookup_t {
	const char* m_pASCII;
	Word32 m_uSize;
	Word32 m_bValue;
};

static const BooleanLookup_t g_BooleanLookup[] = {
	{Burger::g_TrueString,
		static_cast<Word32>(sizeof(Burger::g_TrueString) - 1), TRUE},
	{Burger::g_YesString, static_cast<Word32>(sizeof(Burger::g_YesString) - 1),
		TRUE},
	{Burger::g_FalseString,
		static_cast<Word32>(sizeof(Burger::g_FalseString) - 1), FALSE},
	{Burger::g_NoString, static_cast<Word32>(sizeof(Burger::g_NoString) - 1),
		FALSE}};
#endif

Word BURGER_API Burger::AsciiToBoolean(const char* pInput, const char** pDest)
{
	const BooleanLookup_t* pBooleanLookup = g_BooleanLookup;
	WordPtr i = BURGER_ARRAYSIZE(g_BooleanLookup);
	do {
		if (!MemoryCaseCompare(
				pBooleanLookup->m_pASCII, pInput, pBooleanLookup->m_uSize)) {
			if (pDest) {
				pDest[0] = pInput + pBooleanLookup->m_uSize;
			}
			return pBooleanLookup->m_bValue;
		}
		++pBooleanLookup;
	} while (--i);
	// Return 1 if non-zero
	return AsciiToInteger(pInput, pDest) != 0;
}

/*! ************************************

	\brief Convert an ASCII string into a boolean.

	Convert the string to a \ref TRUE or a \ref FALSE. If the input is \ref NULL
	or invalid, return the default value.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are
	considered \ref FALSE. The comparison is case insensitive.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param bDefault Default boolean to return in the event of a parsing error.

	\return \ref TRUE or \ref FALSE

	\sa AsciiToBoolean(const char *,const char **)

***************************************/

Word BURGER_API Burger::AsciiToBoolean(const char* pInput, Word bDefault)
{
	if (pInput) {
		const char* pDest;
		// Anything parsed?
		Word bValue = AsciiToBoolean(pInput, &pDest);
		if (pDest != pInput) {
			bDefault = bValue;
		}
	}
	return bDefault;
}

/*! ************************************

	\brief Convert an ASCII string into a boolean and signal if successful

	Convert the string to a \ref TRUE or a \ref FALSE. If the input is \ref NULL
	or invalid, return \ref FALSE.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are
	considered \ref FALSE. The comparison is case insensitive.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the
		default

	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string
		was not a boolean

	\sa AsciiToBoolean(const char *,const char **) or AsciiToBoolean(const char
		*,Word)

***************************************/

Word BURGER_API Burger::AsciiToBoolean(Word* pOutput, const char* pInput)
{
	const char* pDest;
	// Convert the text
	Word uOutput = AsciiToBoolean(pInput, &pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest == pInput) {
		uResult = FALSE;
		uOutput = 0; // Clear the output
	}
	pOutput[0] = uOutput;
	return uResult; // Return the result
}

/*! ************************************

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point numeric value and if
	successful, return it. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param fDefault Value to return on error

	\return Value parsed or fDefault

	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(const char
		*,double)

***************************************/

float BURGER_API Burger::AsciiToFloat(const char* pInput, float fDefault)
{
	if (pInput) {
		const char* pDest;
		// Anything parsed?
		float fValue = AsciiToFloat(pInput, &pDest);
		if (pDest != pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(fValue)) {
				fDefault = 0.0f; // Nope, nope, nope...
			} else {
				fDefault = fValue;
			}
		}
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point numeric value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value

	\return Value in between fMin and fMax or fDefault

	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(const char
		*,double,double,double)

***************************************/

float BURGER_API Burger::AsciiToFloat(
	const char* pInput, float fDefault, float fMin, float fMax)
{
	if (pInput) {
		const char* pDest;
		// Anything parsed?
		float fValue = AsciiToFloat(pInput, &pDest);
		if (pDest != pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(fValue)) {
				fDefault = 0.0f; // Nope, nope, nope...
			} else if (fValue < fMin) {
				fDefault = fMin;
			} else if (fValue > fMax) {
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

	Scan the value string as a 32 bit floating point numeric value and if
	successful, return \ref TRUE. If it's not a number, return \ref FALSE.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the
		default

	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string was
		not a 32 bit float

	\sa AsciiToFloat(const char *,const char **) or AsciiToDouble(double *,const
		char *)

***************************************/

Word BURGER_API Burger::AsciiToFloat(float* pOutput, const char* pInput)
{
	const char* pDest;
	// Convert the text
	float fOutput = AsciiToFloat(pInput, &pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest == pInput) {
		uResult = FALSE;
		fOutput = 0.0f; // Clear the output
	}
	pOutput[0] = fOutput;
	return uResult; // Return the result
}

/*! ************************************

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point numeric value and if
	successful, return it. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param dDefault Value to return on error

	\return Value or dDefault

	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(const char
		*,float)

***************************************/

double BURGER_API Burger::AsciiToDouble(const char* pInput, double dDefault)
{
	if (pInput) {
		const char* pDest;
		double dValue = AsciiToDouble(pInput, &pDest);
		if (pDest != pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(dValue)) {
				dDefault = 0.0f; // Nope, nope, nope...
			} else {
				dDefault = dValue;
			}
		}
	}
	return dDefault;
}

/*! ************************************

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point numeric value and if
	successful, test it against the valid range and return the value clamped to
	that range. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param pInput Pointer to the string to convert. \ref NULL will force the
		default
	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value

	\return Value in between dMin and dMax or dDefault

	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(const char
		*,float,float,float)

***************************************/

double BURGER_API Burger::AsciiToDouble(
	const char* pInput, double dDefault, double dMin, double dMax)
{
	if (pInput) {
		const char* pDest;
		double dValue = AsciiToDouble(pInput, &pDest);
		if (pDest != pInput) {
			// Is it NaN or Inf?
			if (!IsFinite(dValue)) {
				dDefault = 0.0f; // Nope, nope, nope...
			} else if (dValue < dMin) {
				dDefault = dMin;
			} else if (dValue > dMax) {
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

	Scan the value string as a 64 bit floating point numeric value and if
	successful, return \ref TRUE. If it's not a number, return \ref FALSE.

	\param pOutput Pointer to the value to return
	\param pInput Pointer to the string to convert. \ref NULL will force the
		default

	\return \ref TRUE if a value was parsed, \ref FALSE if the ASCII string
		was not a 64 bit float

	\sa AsciiToDouble(const char *,const char **) or AsciiToFloat(float *,const
		char *)

***************************************/

Word BURGER_API Burger::AsciiToDouble(double* pOutput, const char* pInput)
{
	const char* pDest;
	// Convert the text
	double dOutput = AsciiToDouble(pInput, &pDest);
	Word uResult = TRUE;
	// Was anything parsed?
	if (pDest == pInput) {
		uResult = FALSE;
		dOutput = 0.0; // Clear the output
	}
	pOutput[0] = dOutput;
	return uResult; // Return the result
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

Word32 BURGER_API Burger::AsciiHexToInteger(const char* pInput, WordPtr uLength)
{
	Word32 uResult = 0;
	// Input valid?
	if (pInput && uLength) {
		do {
			// Look up the character
			Word32 uValue16 =
				g_AsciiToWord8Table[reinterpret_cast<const Word8*>(pInput)[0]];
			// Not 0-9, A-F?
			if (uValue16 >= 16) {
				// Exit
				break;
			}
			++pInput;
			// Overflow?
			if (uResult >= 0x10000000) {
				uResult = BURGER_MAXUINT; // Tilt
				break;
			}
			// Fold in the number
			uResult = (uResult << 4U) + uValue16;
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

Word32 BURGER_API Burger::AsciiHexToInteger(
	const Word16* pInput, WordPtr uLength)
{
	Word32 uResult = 0;
	// Input valid?
	if (pInput && uLength) {
		do {
			// Get a character
			Word uTemp = pInput[0];
			// End now?
			if (uTemp >= 256) {
				break;
			}
			// Look up the character
			Word32 uValue16 = g_AsciiToWord8Table[uTemp];
			// Not 0-9, A-F?
			if (uValue16 >= 16) {
				// Exit
				break;
			}
			++pInput;
			// Overflow?
			if (uResult >= 0x10000000) {
				uResult = BURGER_MAXUINT; // Tilt
				break;
			}
			// Fold in the number
			uResult = (uResult << 4U) + uValue16;
		} while (--uLength);
	}
	return uResult;
}
