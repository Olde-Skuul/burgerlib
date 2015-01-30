/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"
#include "brstringfunctions.h"
#include "brendian.h"
#include "brtick.h"

/*! ************************************

	\typedef GUID
	\brief Globally unique identifier structure

	128 bit value that's generated in a specific way to give a reasonable assurance
	that the value is unique in the whole world. While it's not 100% perfect,
	it's the value system used by many subsystems for Windows.

	Examples of GUIDs

	2F1E4FC0-81FD-11DA-9156-00036A0F876A

	GUID IID_IDirect3DTexture = {0x2CDCD9E0,0x25A0,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };<br/>
	GUID IID_IDirect3D = {0x3BBA0080,0x2421,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };<br/>

	Further reading http://en.wikipedia.org/wiki/Globally_unique_identifier

	\note The first three entries are stored in little endian!!

	\sa _GUID

***************************************/

/*! ************************************

	\struct _GUID
	\brief Globally unique identifier structure

	128 bit value that's generated in a specific way to give a reasonable assurance
	that the value is unique in the whole world. While it's not 100% perfect,
	it's the value system used by many subsystems for Windows.

	Examples of GUIDs

	2F1E4FC0-81FD-11DA-9156-00036A0F876A

	GUID IID_IDirect3DTexture = {0x2CDCD9E0,0x25A0,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };<br/>
	GUID IID_IDirect3D = {0x3BBA0080,0x2421,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };<br/>

	Further reading http://en.wikipedia.org/wiki/Globally_unique_identifier

	\note The first three entries are stored in little endian!!

	\sa GUID

***************************************/

const GUID Burger::g_GUIDZero = {0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } };			///< Empty GUID

/***************************************

	Return TRUE if the ASCII char is 0-9, A-F or a-f or zero
	
***************************************/

static BURGER_INLINE Word GUIDCharIsHex(Word uInput)
{
	Word uResult = FALSE;		// Assume it's bad
	if (uInput<128U) {
		if (!uInput || (Burger::g_AsciiTestTable[uInput]&Burger::ASCII_HEX)) {
			uResult = TRUE;		// It's good!
		}
	}
	return uResult;
}

/***************************************

	Parse a hex strip and return the value in native endian
	(Before I parse, skip over whitespace)
	
***************************************/

static Word32 GUIDFromHex(const char **ppInput,Word uDigits)
{
	const char *pInput = ppInput[0];
	while (!GUIDCharIsHex(reinterpret_cast<const Word8 *>(pInput)[0])) {
		++pInput;
	}
	Word32 uValue = 0;				// Init the value
	do {
		Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		if (!uTemp) {				// End of string???
			break;					// Abort NOW 
		}
		++pInput;					// Accept this char
		uTemp -= '0';				// Convert '0'-'9' to 0-9
		if (uTemp>=10U) {			// Not in range?
			uTemp -= ('A'-'0');		// Convert 'A'-'F' to 0-5
			if (uTemp>=6U) {
				uTemp -= 'a'-'A';	// Convert 'a'-'f' to 0-5
				if (uTemp>=6U) {
					break;			// Not valid!
				}
			}
			uTemp+=10;				// Convert 0-5 to 10-15
		}
		uValue = (uValue << 4U) | uTemp;	// Blend
	} while (--uDigits);
	ppInput[0] = pInput;			// Save the string place
	return uValue;
}

/*! ************************************

	\brief Create a new GUID

	Using a MAC address, current time and deep voodoo, generate a unique 128 bit
	number for labeling a data object.

	http://en.wikipedia.org/wiki/Globally_unique_identifier

	http://tools.ietf.org/html/rfc4122.html
	
	\param pOutput Pointer to an uninitialized GUID buffer
	\sa GUIDFromString() or GUIDToString()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
void BURGER_API Burger::GUIDInit(GUID *pOutput)
{
	// For unsupported platforms, punt
	pOutput->Data1 = LittleEndian::Load(Tick::ReadMicroseconds());
	reinterpret_cast<Word32 *>(pOutput)[1] = LittleEndian::Load(Tick::ReadMicroseconds());
	reinterpret_cast<Word32 *>(pOutput)[2] = LittleEndian::Load(Tick::ReadMicroseconds());
	reinterpret_cast<Word32 *>(pOutput)[3] = LittleEndian::Load(Tick::ReadMicroseconds());
}
#endif

/*! ************************************

	\brief Convert a GUID to a string

	Convert a GUID into a string that looks like 8641FBDE-7F8F-11D4-AAC5-000A27DD93F2
	
	\param pOutput Pointer to a buffer that's at least 36 bytes in length.
	\param pInput Pointer to a GUID to convert to an ASCII string
	\sa GUIDFromString()

***************************************/

void BURGER_API Burger::GUIDToString(char *pOutput,const GUID *pInput)
{
	pOutput = NumberToAsciiHex(pOutput,LittleEndian::Load(&pInput->Data1),LEADINGZEROS|8);
	pOutput[0] = '-';
	pOutput = NumberToAsciiHex(pOutput+1,static_cast<Word32>(LittleEndian::Load(&pInput->Data2)),LEADINGZEROS|4);
	pOutput[0] = '-';
	pOutput = NumberToAsciiHex(pOutput+1,static_cast<Word32>(LittleEndian::Load(&pInput->Data3)),LEADINGZEROS|4);
	pOutput[0] = '-';
	pOutput = NumberToAsciiHex(pOutput+1,static_cast<Word32>(pInput->Data4[0]),LEADINGZEROS|2);
	pOutput = NumberToAsciiHex(pOutput,static_cast<Word32>(pInput->Data4[1]),LEADINGZEROS|2);
	pOutput[0] = '-';
	++pOutput;
	Word i = 0;
	do {
		pOutput = NumberToAsciiHex(pOutput,static_cast<Word32>(pInput->Data4[i+2]),LEADINGZEROS|2);
	} while (++i<6);
	pOutput[0] = 0;			// End the "C" string
}

/*! ************************************

	\brief Convert a GUID string into a GUID

	A string in the format of 8641FBDE-7F8F-11D4-AAC5-000A27DD93F2
	will be parsed and converted into a GUID structure
	
	\param pOutput Pointer to an uninitialized GUID structure
	\param pInput Pointer to a "C" string formatted as above
	\sa GUIDToString()

***************************************/

Word BURGER_API Burger::GUIDFromString(GUID *pOutput,const char *pInput)
{
	pOutput->Data1 = LittleEndian::Load(GUIDFromHex(&pInput,8));	// Get the timestamp
	pOutput->Data2 = LittleEndian::Load(static_cast<Word16>(GUIDFromHex(&pInput,4)));	// The shorts
	pOutput->Data3 = LittleEndian::Load(static_cast<Word16>(GUIDFromHex(&pInput,4)));
	Word i = 0;
	do {
		pOutput->Data4[i] = static_cast<Word8>(GUIDFromHex(&pInput,2));	// The last 8 bytes
	} while (++i<8);
	// Skip over trailing whitespace
	while (!GUIDCharIsHex(reinterpret_cast<const Word8 *>(pInput)[0])) {
		++pInput;
	}
	if (reinterpret_cast<const Word8 *>(pInput)[0]) {
		return TRUE;		// Error
	}
	return FALSE;			// It's ok!
}

/*! ************************************

	\brief Return the 16 bit hash of a GUID

	Using the GUID as a byte array (So the function is
	endian neutral), create a 16 bit hash value.

	\param pInput Pointer to a valid GUID
	\return The 16 bit hash value
	
***************************************/

Word BURGER_API Burger::GUIDHash(const GUID *pInput)
{
	Int iSum = 0;		// Sum of all the entries
	Int iAccum = 0;		// Accumulation
	WordPtr uCount = sizeof(GUID);
	// Using SIGNED math, add all of the byte together
	do {
		iSum = iSum + reinterpret_cast<const char *>(pInput)[0];	// Simple add of all 16 bytes
		iAccum = iAccum + iSum;										// Adjust the hash
		pInput = reinterpret_cast<const GUID *>(reinterpret_cast<const char *>(pInput)+1);
	} while (--uCount);
	
	// Force to 16 bit
	iAccum = static_cast<Int16>(iAccum);
	Int iResult = -iAccum % 255;	// Make a modulo to hash it out
	if (iResult < 0) {
		iResult += 255;				// Force positive
	}
	iResult = iResult&0xFF;			// Force to unsigned char

	iAccum = static_cast<Int16>(iAccum-iSum);	// The hash is 16 bit!
	Int iResult2 = iAccum % 255;	// Make modulo
	if (iResult2 < 0) {
		iResult2 += 255;			// Force positive
	}
	return static_cast<Word>(((iResult2&0xFF) << 8) | iResult);		// Return the hash (Only 16 bits)
}


/*! ************************************

	\brief Compare two GUID structures for equality

	\param pInput1 Pointer to the first GUID to test
	\param pInput2 Pointer to the second GUID to test
	\return \ref TRUE if they are equal, \ref FALSE if not
	
***************************************/

Word BURGER_API Burger::GUIDIsEqual(const GUID *pInput1, const GUID *pInput2)
{
	return (reinterpret_cast<const Word32 *>(pInput1)[0] == reinterpret_cast<const Word32 *>(pInput2)[0]) &&
		(reinterpret_cast<const Word32 *>(pInput1)[1] == reinterpret_cast<const Word32 *>(pInput2)[1]) &&
		(reinterpret_cast<const Word32 *>(pInput1)[2] == reinterpret_cast<const Word32 *>(pInput2)[2]) &&
		(reinterpret_cast<const Word32 *>(pInput1)[3] == reinterpret_cast<const Word32 *>(pInput2)[3]);
}

/*! ************************************

	\brief Test two GUID for equality

	Compare two GUID's for equality. For cross platform consistency,
	the comparisons assume the data is in little endian format and
	will do conversions on big endian machines.

	Will return 0 for equality, -1 if pInput1<pInput2 and 1 if pInput1>pInput2

	\param pInput1 Pointer to the first GUID to test
	\param pInput2 Pointer to the second GUID to test
	\return 1 if pInput1>pInput2, -1 if pInput1<pInput2 and 0 if they are identical

***************************************/

int BURGER_API Burger::GUIDCompare(const GUID *pInput1,const GUID *pInput2)
{
	Word32 uTemp1 = LittleEndian::Load(&pInput1->Data1);
	Word32 uTemp2 = LittleEndian::Load(&pInput2->Data1);
	int iResult;
	if (uTemp1 < uTemp2) {
		iResult = -1;
	} else if (uTemp1 > uTemp2) {
		iResult = 1;
	} else {
		uTemp1 = LittleEndian::Load(&pInput1->Data2);
		uTemp2 = LittleEndian::Load(&pInput2->Data2);
		if (uTemp1 < uTemp2) {
			iResult = -1;
		} else if (uTemp1 > uTemp2) {
			iResult = 1;
		} else {
			uTemp1 = LittleEndian::Load(&pInput1->Data3);
			uTemp2 = LittleEndian::Load(&pInput2->Data3);
			if (uTemp1 < uTemp2) {
				iResult = -1;
			} else if (uTemp1 > uTemp2) {
				iResult = 1;
			} else {
				WordPtr i = 0;
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
				} while (++i<8);
			}
		}
	}
	return iResult;	// Return the result
}

