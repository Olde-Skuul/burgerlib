/***************************************

	uLaw decompresser

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brulaw.h"
#include "brendian.h"
#include "brfixedpoint.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressULaw,Burger::DecompressAudio);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressULaw::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\brief ULaw decompression lookup table

***************************************/

const Int16 Burger::DecompressULaw::g_Table[256] = {
	-32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
	-23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
	-15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
	-11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
	-7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
	-5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
	-3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
	-2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
	-1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
	-1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,
	-876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,
	-620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,
	-372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,
	-244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,
	-120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,
	-56,   -48,   -40,   -32,   -24,   -16,    -8,     0,
	32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
	23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
	15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
	11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,
	7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,
	5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,
	3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,
	2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,
	1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,
	1372,  1308,  1244,  1180,  1116,  1052,   988,   924,
	876,   844,   812,   780,   748,   716,   684,   652,
	620,   588,   556,   524,   492,   460,   428,   396,
	372,   356,   340,   324,   308,   292,   276,   260,
	244,   228,   212,   196,   180,   164,   148,   132,
	120,   112,   104,    96,    88,    80,    72,    64,
	56,    48,    40,    32,    24,    16,     8,     0
};


/*! ************************************

	\class Burger::DecompressULaw
	\brief Process audio data compressed with ULaw

	Convert audio data compressed with ULaw to native
	16 bit format

	\sa DecompressALaw, \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressULaw::DecompressULaw() :
	DecompressAudio(SoundManager::TYPESHORT),
	m_uCacheSize(0),
	m_eState(STATE_INIT)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref DECOMPRESS_OKAY if successful

***************************************/

Burger::Decompress::eError Burger::DecompressULaw::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
	m_uCacheSize = 0;
	m_eState = STATE_INIT;
	return DECOMPRESS_OKAY;
}

/*! ************************************

	\brief Decompress audio data using ulaw

	Input data is assumed to be ulaw compressed bytes.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::Decompress::eError Burger::DecompressULaw::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
	//
	// Handle data "decompression"
	//

	// Save the pointers to determine consumption
	void *pOldOutput = pOutput;
	const void *pOldInput = pInput;

	//
	// Process based on state
	//

	eState uState = m_eState;
	Word bAbort = FALSE;
	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{
				// Copy the data while converting the endian
				WordPtr uPacketCount = Min(uInputChunkLength,uOutputChunkLength/2);
				uInputChunkLength -= uPacketCount;
				uOutputChunkLength -= uPacketCount*2;

				WordPtr uLength = uPacketCount;
				if (uLength) {
					// Is it aligned?
					if (!(reinterpret_cast<WordPtr>(pOutput)&1)) {
						do {
							// Convert to native endian quickly
							static_cast<Int16 *>(pOutput)[0] = g_Table[static_cast<const Word8 *>(pInput)[0]];
							pInput = static_cast<const Word8 *>(pInput)+1;
							pOutput = static_cast<Word16 *>(pOutput)+1;
						} while (--uLength);
					} else {

						// You monster.

						do {
							// Convert to endian with unaligned data
							NativeEndian::StoreAny(static_cast<int16_t*>(pOutput),g_Table[static_cast<const Word8 *>(pInput)[0]]);
							pInput = static_cast<const Word8 *>(pInput)+1;
							pOutput = static_cast<Word16 *>(pOutput)+1;
						} while (--uLength);
					}
				}

				//
				// One extra byte?
				//
				if (uInputChunkLength) {
					// Put it in the cache and go into cache mode
					m_iCache = g_Table[static_cast<const Word8 *>(pInput)[0]];
					pInput = static_cast<const Word8 *>(pInput)+1;
					--uInputChunkLength;
					m_uCacheSize = 2;
					uState = STATE_CACHEFULL;
				} else {
					bAbort = TRUE;
				}
			}
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			if (uOutputChunkLength) {

				// Output 1 or 2 bytes

				WordPtr uCacheSize = m_uCacheSize;
				WordPtr uSteps = Min(uOutputChunkLength,static_cast<WordPtr>(uCacheSize));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				const Word8 *pSrc = &reinterpret_cast<const Word8 *>(&m_iCache)[2-uCacheSize];

				// Update the cache size
				uCacheSize = static_cast<Word>(uCacheSize-uSteps);
				if (uCacheSize) {
					// Number of bytes remaining in cache
					m_uCacheSize = static_cast<Word>(uCacheSize);
				} else {
					// Cache will be empty, so switch to normal mode
					uState = STATE_INIT;
				}

				//
				// Copy out the cache data
				//
				do {
					static_cast<Word8 *>(pOutput)[0] = pSrc[0];
					++pSrc;
					pOutput = static_cast<Word8 *>(pOutput)+1;
				} while (--uSteps);
			} else {
				bAbort = TRUE;
			}
			break;

		}
	} while (!bAbort);

	// Save the state
	m_eState = uState;

	// Return the number of bytes actually consumed
	WordPtr uInputConsumed = static_cast<WordPtr>(static_cast<const Word8 *>(pInput)-static_cast<const Word8 *>(pOldInput));
	WordPtr uOutputConsumed = static_cast<WordPtr>(static_cast<const Word8 *>(pOutput)-static_cast<const Word8 *>(pOldOutput));

	// Store the amount of data that was processed

	m_uInputLength = uInputConsumed;
	m_uOutputLength = uOutputConsumed;

	// Add the decompressed data to the totals
	m_uTotalInput += uInputConsumed;
	m_uTotalOutput += uOutputConsumed;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uOutputConsumed) {
		return DECOMPRESS_OUTPUTUNDERRUN;
	}

	// Input data remaining?
	if (uInputChunkLength!=uInputConsumed) {
		return DECOMPRESS_OUTPUTOVERRUN;
	}
	// Decompression is complete
	return DECOMPRESS_OKAY;
}

/*! ************************************

	\brief Allocate and initialize a DecompressULaw

	\return A pointer to a default DecompressULaw class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressULaw * BURGER_API Burger::DecompressULaw::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressULaw))) DecompressULaw();
}
