/***************************************

    Library to handle mp3 files (MP3)

    Copyright (c) 2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemp3.h"

/*! ************************************

	\brief Parses the ID3V2 tag length
	
	A ID3V2 tag is 28 bits pulled out of 4 bytes with
	the highest bit masked off.

	\param pInput Pointer to a raw ID3V2 length stream (4 bytes)
	\return The converted tag length (28 bits max)

***************************************/

uint32_t Burger::GetID3V2TagLength(const void *pInput)
{
	uint_t uResult = (static_cast<const uint8_t *>(pInput)[0] & 0x7FU)<<21U;
	uResult += (static_cast<const uint8_t *>(pInput)[1] & 0x7FU)<<14U;
	uResult += (static_cast<const uint8_t *>(pInput)[2] & 0x7FU)<<7U;
	uResult += (static_cast<const uint8_t *>(pInput)[3] & 0x7FU);
	return uResult;
}

/*! ************************************

	\class Burger::FileMP3
	\brief Reads and writes MP3 files

	This class will read and write out MP3 text files

***************************************/

/*! ************************************

	\brief Initialize a MP3 file object

***************************************/

Burger::FileMP3::FileMP3(void)
{
}

/*! ************************************

	\brief Dispose of an MP3 file object

	Release all objects.

	Calling Shutdown() performs this action manually.

	\sa Shutdown(void)

***************************************/

Burger::FileMP3::~FileMP3()
{
}
