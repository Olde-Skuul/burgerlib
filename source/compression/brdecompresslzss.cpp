/***************************************

    Decompression manager version of LZSS

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdecompresslzss.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressLZSS,Burger::Decompress);
#endif

/*! ************************************

	\class Burger::DecompressLZSS
	\brief Decompress LZSS format
	
	Decompress data in LZSS format (Documented here Burger::CompressLZSS )

	\sa Burger::Decompress and Burger::CompressLZSS

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressLZSS::DecompressLZSS() :
	m_uBitBucket(1),
	m_eState(STATE_INIT)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Reset the LZSS decompression

	\return kErrorNone (No error is possible)

***************************************/

Burger::eError Burger::DecompressLZSS::Reset(void)
{
	m_uTotalOutput = 0;
	m_uTotalInput = 0;
	m_eState = STATE_INIT;
	m_uBitBucket = 1;
	return kErrorNone;
}

/*! ************************************

	\brief Decompress data using LZSS compression

	Using the LZSS compression algorithm, decompress the data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa Burger::SimpleDecompressLZSS()

***************************************/

Burger::eError Burger::DecompressLZSS::Process(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	m_uInputLength = uInputChunkLength;
	m_uOutputLength = uOutputChunkLength;

	// Default state is assumed

	eState MyState = m_eState;
	if (uInputChunkLength || (MyState==STATE_RUN)) {
		m_eState = STATE_INIT;		// Reset the state
		
		uint_t uBitBucket = m_uBitBucket;	// Restore the bit bucket
		uintptr_t uRunCount = m_uRunCount;

		if (MyState==STATE_16BIT2) {	// Get half token
			goto Get16Bit2;
		}

		// Was a run in progress?

		if (MyState==STATE_RUN) {
			// Get the source pointer
			const uint8_t *pSource = static_cast<const uint8_t*>(pOutput)+m_uOffset;
			if (uOutputChunkLength<uRunCount) {
				// Output data that's in the cache
				if (uOutputChunkLength) {
					uRunCount-=uOutputChunkLength;
					do {
						uint8_t Temp = pSource[0];
						++pSource;
						static_cast<uint8_t*>(pOutput)[0] = Temp;
						pOutput = static_cast<uint8_t*>(pOutput)+1;
					} while (--uOutputChunkLength);
				}
				m_uRunCount = uRunCount;
				m_eState = STATE_RUN;
				goto Exit;
			}
			uOutputChunkLength-=uRunCount;
			do {
				uint8_t Temp = pSource[0];
				++pSource;
				static_cast<uint8_t*>(pOutput)[0] = Temp;
				pOutput = static_cast<uint8_t*>(pOutput)+1;
			} while (--uRunCount);
			uBitBucket = uBitBucket>>1;
		}
	
		// Normal decompression entry
		do {
			if (uBitBucket==1) {
				// No input?
				if (!uInputChunkLength) {
					break;
				}
				// Grab the initial bit bucket
				uBitBucket = static_cast<uint_t>(static_cast<const uint8_t*>(pInput)[0])|0x100U;
				pInput = static_cast<const uint8_t*>(pInput)+1;
				--uInputChunkLength;
			}

			// No data to fetch?
			if (!uInputChunkLength) {
				break;
			}

			if (uBitBucket&1U) {			// Get a bit from the input stream
				if (!uOutputChunkLength) {	// No data remaining?
					break;
				}
				// Copy a byte
				static_cast<uint8_t*>(pOutput)[0] = static_cast<const uint8_t*>(pInput)[0];
				pInput = static_cast<const uint8_t*>(pInput)+1;
				pOutput = static_cast<uint8_t*>(pOutput)+1;
				--uInputChunkLength;
				--uOutputChunkLength;

			} else {

				uRunCount = static_cast<const uint8_t*>(pInput)[0];
				pInput = static_cast<const uint8_t*>(pInput)+1;
				if (!--uInputChunkLength) {
					m_eState = STATE_16BIT2;
					m_uRunCount = uRunCount;
					break;
				}
Get16Bit2:
				uRunCount = uRunCount | (static_cast<uint_t>(static_cast<const uint8_t*>(pInput)[0])<<8);
				pInput = static_cast<const uint8_t*>(pInput)+1;
				--uInputChunkLength;
				uintptr_t uOffset = static_cast<uintptr_t>((~static_cast<uintptr_t>(0xFFFU))|uRunCount);
				const uint8_t *pSource = static_cast<const uint8_t*>(pOutput)+uOffset;			// Get source pointer
				uRunCount = (uRunCount>>12)+3;

				// Perform the memory copy
				if (uOutputChunkLength<uRunCount) {
					// The output buffer isn't big enough.
					m_eState = STATE_RUN;
					m_uOffset = uOffset;
					// Can a partial copy be performed?
					if (uOutputChunkLength) {
						// Adjust the run for continuing the copy
						uRunCount-=uOutputChunkLength;
						do {
							uint8_t uTemp = pSource[0];
							++pSource;
							static_cast<uint8_t*>(pOutput)[0] = uTemp;
							pOutput = static_cast<uint8_t*>(pOutput)+1;
						} while (--uOutputChunkLength);
					}
					// Store the remaining run counts
					m_uRunCount = uRunCount;
					break;
				}

				// Fast version!!! :)

				uOutputChunkLength-=uRunCount;
				uRunCount = 18-uRunCount;
				pSource -= uRunCount;
				pOutput = static_cast<uint8_t*>(pOutput) - uRunCount;
				switch (uRunCount) {
				case 0:
					static_cast<uint8_t*>(pOutput)[0] = pSource[0];
				case 1:
					static_cast<uint8_t*>(pOutput)[1] = pSource[1];
				case 2:
					static_cast<uint8_t*>(pOutput)[2] = pSource[2];
				case 3:
					static_cast<uint8_t*>(pOutput)[3] = pSource[3];
				case 4:
					static_cast<uint8_t*>(pOutput)[4] = pSource[4];
				case 5:
					static_cast<uint8_t*>(pOutput)[5] = pSource[5];
				case 6:
					static_cast<uint8_t*>(pOutput)[6] = pSource[6];
				case 7:
					static_cast<uint8_t*>(pOutput)[7] = pSource[7];
				case 8:
					static_cast<uint8_t*>(pOutput)[8] = pSource[8];
				case 9:
					static_cast<uint8_t*>(pOutput)[9] = pSource[9];
				case 10:
					static_cast<uint8_t*>(pOutput)[10] = pSource[10];
				case 11:
					static_cast<uint8_t*>(pOutput)[11] = pSource[11];
				case 12:
					static_cast<uint8_t*>(pOutput)[12] = pSource[12];
				case 13:
					static_cast<uint8_t*>(pOutput)[13] = pSource[13];
				case 14:
					static_cast<uint8_t*>(pOutput)[14] = pSource[14];
				case 15:
				default:
					static_cast<uint8_t*>(pOutput)[15] = pSource[15];
					static_cast<uint8_t*>(pOutput)[16] = pSource[16];
					static_cast<uint8_t*>(pOutput)[17] = pSource[17];
				}
				pOutput = static_cast<uint8_t*>(pOutput)+18;
			}
			// Accept a bit from the bit bucket
			uBitBucket>>=1;
		} while (uOutputChunkLength);
		m_uBitBucket = uBitBucket;
	}
Exit:
	// How did the decompression go?

	m_uOutputLength -= uOutputChunkLength;
	m_uInputLength -= uInputChunkLength;
	m_uTotalOutput += m_uOutputLength;
	m_uTotalInput += m_uInputLength;
	// Output buffer not big enough?
	if (uOutputChunkLength) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength || (m_eState==STATE_RUN)) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}

/***************************************

	Decompress using LZSS.
	I assume I can decompress in one pass.
	This makes for fast and tight code.

***************************************/

Burger::eError BURGER_API Burger::SimpleDecompressLZSS(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	Burger::DecompressLZSS Local;
	Local.DecompressLZSS::Reset();
	return Local.DecompressLZSS::Process(pOutput,uOutputChunkLength,pInput,uInputChunkLength);
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressLZSS::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
