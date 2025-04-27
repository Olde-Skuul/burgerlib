/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"
#include "brendian.h"
#include "brnumberto.h"
#include "brstringfunctions.h"
#include "brtick.h"

/*! ************************************

	\typedef GUID
	\brief Globally unique identifier structure

	128 bit value that's generated in a specific way to give a reasonable
	assurance that the value is unique in the whole world. While it's not 100%
	perfect, it's the value system used by many subsystems for Windows.

	Examples of GUIDs

	``2F1E4FC0-81FD-11DA-9156-00036A0F876A``

	\code
	GUID IID_IDirect3DTexture =
		{0x2CDCD9E0,0x25A0,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
	GUID IID_IDirect3D =
		{0x3BBA0080,0x2421,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
	\endcode

	Further reading http://en.wikipedia.org/wiki/Globally_unique_identifier

	\note The first three entries are stored in little endian!!

	\sa _GUID

***************************************/

/*! ************************************

	\struct _GUID
	\brief Globally unique identifier structure

	128 bit value that's generated in a specific way to give a reasonable
	assurance that the value is unique in the whole world. While it's not 100%
	perfect, it's the value system used by many subsystems for Windows.

	Examples of GUIDs

	``2F1E4FC0-81FD-11DA-9156-00036A0F876A``

	\code
	GUID IID_IDirect3DTexture =
		{0x2CDCD9E0,0x25A0,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
	GUID IID_IDirect3D =
		{0x3BBA0080,0x2421,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
	\endcode

	Further reading http://en.wikipedia.org/wiki/Globally_unique_identifier

	\note The first three entries are stored in little endian!!

	\sa \ref GUID

***************************************/

/*! ************************************

	\brief Empty GUID

	A GUID where all values are zero

***************************************/

const GUID Burger::g_GUID_zero = {0x00000000, 0x0000, 0x0000,
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

/***************************************

	Parse a hex strip and return the value in native endian

	Before parsing, skip over whitespace

***************************************/

static uint32_t BURGER_API GUID_from_hex(
	const char** ppInput, uint_t uDigits) BURGER_NOEXCEPT
{
	const char* pInput = ppInput[0];

	// Init the value
	uint32_t uValue = 0;

	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {

		// Skip all non-hex characters, like {} and -
		do {
			if (Burger::is_hex(static_cast<char>(uTemp))) {
				break;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);

		// Convert the hex to integer
		do {
			// End of string???
			if (!uTemp) {

				// Abort NOW
				break;
			}

			// Convert '0'-'9' to 0-9
			uTemp -= '0';

			// Not in range?
			if (uTemp >= 10U) {

				// Convert 'A'-'F' to 0-5
				uTemp -= static_cast<uint_t>('A' - '0');
				if (uTemp >= 6U) {

					// Convert 'a'-'f' to 0-5
					uTemp -= static_cast<uint_t>('a' - 'A');
					if (uTemp >= 6U) {
						// Not valid!
						break;
					}
				}

				// Convert 0-5 to 10-15
				uTemp += 10U;
			}

			// Accept this char
			++pInput;

			// Blend
			uValue = (uValue << 4U) | uTemp;

			// Fetch the next character
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (--uDigits);
	}

	// Save the location of the end of string
	ppInput[0] = pInput;
	return uValue;
}

/*! ************************************

	\brief Create a new GUID

	Using a MAC address, current time and deep voodoo, generate a unique 128 bit
	number for labeling a data object.

	http://en.wikipedia.org/wiki/Globally_unique_identifier

	http://tools.ietf.org/html/rfc4122.html

	\param pOutput Pointer to an uninitialized GUID buffer
	\sa GUID_from_string() or GUID_to_string()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_DARWIN) || \
	defined(BURGER_MAC) || defined(BURGER_ANDROID) || defined(BURGER_PS4) || \
	defined(BURGER_PS5) || defined(BURGER_XBOXONE) || defined(BURGER_LINUX) || \
	defined(BURGER_WIIU) || defined(BURGER_SWITCH)) || \
	defined(DOXYGEN)

void BURGER_API Burger::GUID_init(GUID* pOutput) BURGER_NOEXCEPT
{
	uint32_t* pTemp = reinterpret_cast<uint32_t*>(pOutput);
	// For unsupported platforms, punt
	LittleEndian::store(&pTemp[0], Tick::read_us());

	// Make sure it's marked as version 4, variant 1
	// which marks it as a random number
	LittleEndian::store(
		&pTemp[1], (Tick::read_us() & 0xFFF0FFFFU) | 0x00040000U);
	LittleEndian::store(
		&pTemp[2], (Tick::read_us() & 0xFFFFFFF3U) | 0x00000008U);
	LittleEndian::store(&pTemp[3], Tick::read_us());
}
#endif

/*! ************************************

	\brief Convert a GUID to a string

	Convert a GUID into a string that looks like
	`8641FBDE-7F8F-11D4-AAC5-000A27DD93F2`

	\note This function assumes the GUID is stored in Microsoft mixed endian
		format where the first three groups are little endian and the remaining
		2 groups as big endian.

	\param pOutput Pointer to a buffer that's at least 36 bytes in length.
	\param pInput Pointer to a GUID to convert to an ASCII string

	\sa GUID_from_string()

***************************************/

void BURGER_API Burger::GUID_to_string(
	char* pOutput, const GUID* pInput) BURGER_NOEXCEPT
{
	pOutput = to_hex_ascii(
		pOutput, LittleEndian::load(&pInput->Data1), kEnableLeadingZeros | 8U);

	pOutput[0] = '-';

	pOutput = to_hex_ascii(pOutput + 1, LittleEndian::load(&pInput->Data2),
		kEnableLeadingZeros | 4U);

	pOutput[0] = '-';

	pOutput = to_hex_ascii(pOutput + 1, LittleEndian::load(&pInput->Data3),
		kEnableLeadingZeros | 4U);

	pOutput[0] = '-';

	// Print as big endian 16 bit
	pOutput =
		to_hex_ascii(pOutput + 1, pInput->Data4[0], kEnableLeadingZeros | 2U);
	pOutput = to_hex_ascii(pOutput, pInput->Data4[1], kEnableLeadingZeros | 2U);

	pOutput[0] = '-';
	++pOutput;

	uintptr_t i = 0;
	do {
		pOutput = to_hex_ascii(
			pOutput, pInput->Data4[i + 2], kEnableLeadingZeros | 2U);
	} while (++i < 6U);

	// End the "C" string
	pOutput[0] = 0;
}

/*! ************************************

	\brief Convert a GUID string into a GUID

	A string in the format of `8641FBDE-7F8F-11D4-AAC5-000A27DD93F2`
	will be parsed and converted into a GUID structure

	\param pOutput Pointer to an uninitialized GUID structure
	\param pInput Pointer to a "C" string formatted as above

	\return Zero if no error, \ref kErrorInvalidArgument on bad input
	\sa GUID_to_string()

***************************************/

Burger::eError BURGER_API Burger::GUID_from_string(
	GUID* pOutput, const char* pInput) BURGER_NOEXCEPT
{
	// Get the timestamp
	LittleEndian::store(&pOutput->Data1, GUID_from_hex(&pInput, 8U));

	// The shorts
	LittleEndian::store(
		&pOutput->Data2, static_cast<uint16_t>(GUID_from_hex(&pInput, 4U)));
	LittleEndian::store(
		&pOutput->Data3, static_cast<uint16_t>(GUID_from_hex(&pInput, 4U)));

	uintptr_t i = 0;
	do {
		// The last 8 bytes
		pOutput->Data4[i] = static_cast<uint8_t>(GUID_from_hex(&pInput, 2U));
	} while (++i < 8U);

	// Skip over trailing non hex characters
	eError uResult = kErrorNone;
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {
		do {
			if (is_hex(static_cast<char>(uTemp))) {
				// Error
				uResult = kErrorInvalidArgument;
				break;
			}
			++pInput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		} while (uTemp);
	}

	// It's ok!
	return uResult;
}

/*! ************************************

	\brief Return the 16 bit hash of a GUID

	Using the GUID as a byte array (So the function is endian neutral), create a
	16 bit hash value.

	\param pInput Pointer to a valid GUID

	\return The 16 bit hash value

***************************************/

uint_t BURGER_API Burger::GUID_hash(const GUID* pInput) BURGER_NOEXCEPT
{
	// Sum of all the entries
	int_t iSum = 0;

	// Accumulation
	int_t iAccum = 0;
	uintptr_t uCount = sizeof(GUID);

	// Using SIGNED math, add all of the byte together
	do {
		// Simple add of all 16 bytes
		iSum = iSum + reinterpret_cast<const char*>(pInput)[0];

		// Adjust the hash
		iAccum = iAccum + iSum;
		pInput = reinterpret_cast<const GUID*>(
			reinterpret_cast<const char*>(pInput) + 1);
	} while (--uCount);

	// Force to 16 bit
	iAccum = static_cast<int16_t>(iAccum);

	// Make a modulo to hash it out
	int_t iResult = -iAccum % 255;
	if (iResult < 0) {
		// Force positive
		iResult += 255;
	}

	// Force to unsigned char
	iResult = iResult & 0xFF;

	// The hash is 16 bit!
	iAccum = static_cast<int16_t>(iAccum - iSum);

	// Make modulo
	int_t iResult2 = iAccum % 255;
	if (iResult2 < 0) {

		// Force positive
		iResult2 += 255;
	}

	// Return the hash (Only 16 bits)
	return static_cast<uint_t>(((iResult2 & 0xFFU) << 8U) | iResult);
}

/*! ************************************

	\brief Compare two GUID structures for equality

	\param pInput1 Pointer to the first GUID to test
	\param pInput2 Pointer to the second GUID to test

	\return \ref TRUE if they are equal, \ref FALSE if not

	\sa GUID_compare(const GUID*, const GUID*)

***************************************/

uint_t BURGER_API Burger::GUID_is_equal(
	const GUID* pInput1, const GUID* pInput2) BURGER_NOEXCEPT
{
	return (reinterpret_cast<const uint32_t*>(pInput1)[0] ==
			   reinterpret_cast<const uint32_t*>(pInput2)[0]) &&
		(reinterpret_cast<const uint32_t*>(pInput1)[1] ==
			reinterpret_cast<const uint32_t*>(pInput2)[1]) &&
		(reinterpret_cast<const uint32_t*>(pInput1)[2] ==
			reinterpret_cast<const uint32_t*>(pInput2)[2]) &&
		(reinterpret_cast<const uint32_t*>(pInput1)[3] ==
			reinterpret_cast<const uint32_t*>(pInput2)[3]);
}

/*! ************************************

	\brief Test two GUID for equality

	Compare two GUID's for equality. For cross platform consistency,
	the comparisons assume the data is in little endian format and
	will do conversions on big endian machines.

	Will return 0 for equality, -1 if pInput1<pInput2 and 1 if pInput1>pInput2

	\param pInput1 Pointer to the first GUID to test
	\param pInput2 Pointer to the second GUID to test

	\return 1 if pInput1>pInput2, -1 if pInput1<pInput2 and 0 if they are
		identical

	\sa GUID_is_equal(const GUID*, const GUID*)

***************************************/

int BURGER_API Burger::GUID_compare(
	const GUID* pInput1, const GUID* pInput2) BURGER_NOEXCEPT
{

	uint32_t uTemp1 = LittleEndian::load(&pInput1->Data1);
	uint32_t uTemp2 = LittleEndian::load(&pInput2->Data1);

	int iResult;
	if (uTemp1 < uTemp2) {
		iResult = -1;
	} else if (uTemp1 > uTemp2) {
		iResult = 1;
	} else {

		uTemp1 = LittleEndian::load(&pInput1->Data2);
		uTemp2 = LittleEndian::load(&pInput2->Data2);
		if (uTemp1 < uTemp2) {
			iResult = -1;
		} else if (uTemp1 > uTemp2) {
			iResult = 1;
		} else {

			uTemp1 = LittleEndian::load(&pInput1->Data3);
			uTemp2 = LittleEndian::load(&pInput2->Data3);
			if (uTemp1 < uTemp2) {
				iResult = -1;
			} else if (uTemp1 > uTemp2) {
				iResult = 1;
			} else {

				// Scan from 0 to 7 to simulate big endian
				uintptr_t i = 0;
				iResult = 0;
				do {
					uTemp1 = pInput1->Data4[i];
					uTemp2 = pInput2->Data4[i];
					if (uTemp1 < uTemp2) {
						iResult = -1;
						break;
					}
					if (uTemp1 > uTemp2) {
						iResult = 1;
						break;
					}
				} while (++i < 8);
			}
		}
	}

	// Return the result
	return iResult;
}
