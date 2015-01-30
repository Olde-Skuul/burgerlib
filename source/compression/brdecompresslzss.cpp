/***************************************

	Decompression manager version of LZSS

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdecompresslzss.h"

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
	reinterpret_cast<Word32 *>(m_uSignature)[0] = Signature;
}

/*! ************************************

	\brief Reset the LZSS decompression

	\return Decompress::DECOMPRESS_OKAY (No error is possible)

***************************************/

Burger::Decompress::eError Burger::DecompressLZSS::Reset(void)
{
	m_uTotalOutput = 0;
	m_uTotalInput = 0;
	m_eState = STATE_INIT;
	m_uBitBucket = 1;
	return DECOMPRESS_OKAY;
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

Burger::Decompress::eError Burger::DecompressLZSS::Process(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength)
{
	m_uInputLength = uInputChunkLength;
	m_uOutputLength = uOutputChunkLength;

	// Default state is assumed

	eState MyState = m_eState;
	if (uInputChunkLength || (MyState==STATE_RUN)) {
		m_eState = STATE_INIT;		// Reset the state
		
		Word uBitBucket = m_uBitBucket;	// Restore the bit bucket
		WordPtr uRunCount = m_uRunCount;

		if (MyState==STATE_16BIT2) {	// Get half token
			goto Get16Bit2;
		}

		// Was a run in progress?

		if (MyState==STATE_RUN) {
			// Get the source pointer
			const Word8 *pSource = static_cast<const Word8*>(pOutput)+m_uOffset;
			if (uOutputChunkLength<uRunCount) {
				// Output data that's in the cache
				if (uOutputChunkLength) {
					uRunCount-=uOutputChunkLength;
					do {
						Word8 Temp = pSource[0];
						++pSource;
						static_cast<Word8*>(pOutput)[0] = Temp;
						pOutput = static_cast<Word8*>(pOutput)+1;
					} while (--uOutputChunkLength);
				}
				m_uRunCount = uRunCount;
				m_eState = STATE_RUN;
				goto Exit;
			}
			uOutputChunkLength-=uRunCount;
			do {
				Word8 Temp = pSource[0];
				++pSource;
				static_cast<Word8*>(pOutput)[0] = Temp;
				pOutput = static_cast<Word8*>(pOutput)+1;
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
				uBitBucket = static_cast<Word>(static_cast<const Word8*>(pInput)[0])|0x100U;
				pInput = static_cast<const Word8*>(pInput)+1;
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
				static_cast<Word8*>(pOutput)[0] = static_cast<const Word8*>(pInput)[0];
				pInput = static_cast<const Word8*>(pInput)+1;
				pOutput = static_cast<Word8*>(pOutput)+1;
				--uInputChunkLength;
				--uOutputChunkLength;

			} else {

				uRunCount = static_cast<const Word8*>(pInput)[0];
				pInput = static_cast<const Word8*>(pInput)+1;
				if (!--uInputChunkLength) {
					m_eState = STATE_16BIT2;
					m_uRunCount = uRunCount;
					break;
				}
Get16Bit2:
				uRunCount = uRunCount | (static_cast<Word>(static_cast<const Word8*>(pInput)[0])<<8);
				pInput = static_cast<const Word8*>(pInput)+1;
				--uInputChunkLength;
				WordPtr uOffset = static_cast<WordPtr>((~static_cast<WordPtr>(0xFFFU))|uRunCount);
				const Word8 *pSource = static_cast<const Word8*>(pOutput)+uOffset;			// Get source pointer
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
							Word8 uTemp = pSource[0];
							++pSource;
							static_cast<Word8*>(pOutput)[0] = uTemp;
							pOutput = static_cast<Word8*>(pOutput)+1;
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
				pOutput = static_cast<Word8*>(pOutput) - uRunCount;
				switch (uRunCount) {
				case 0:
					static_cast<Word8*>(pOutput)[0] = pSource[0];
				case 1:
					static_cast<Word8*>(pOutput)[1] = pSource[1];
				case 2:
					static_cast<Word8*>(pOutput)[2] = pSource[2];
				case 3:
					static_cast<Word8*>(pOutput)[3] = pSource[3];
				case 4:
					static_cast<Word8*>(pOutput)[4] = pSource[4];
				case 5:
					static_cast<Word8*>(pOutput)[5] = pSource[5];
				case 6:
					static_cast<Word8*>(pOutput)[6] = pSource[6];
				case 7:
					static_cast<Word8*>(pOutput)[7] = pSource[7];
				case 8:
					static_cast<Word8*>(pOutput)[8] = pSource[8];
				case 9:
					static_cast<Word8*>(pOutput)[9] = pSource[9];
				case 10:
					static_cast<Word8*>(pOutput)[10] = pSource[10];
				case 11:
					static_cast<Word8*>(pOutput)[11] = pSource[11];
				case 12:
					static_cast<Word8*>(pOutput)[12] = pSource[12];
				case 13:
					static_cast<Word8*>(pOutput)[13] = pSource[13];
				case 14:
					static_cast<Word8*>(pOutput)[14] = pSource[14];
				case 15:
				default:
					static_cast<Word8*>(pOutput)[15] = pSource[15];
					static_cast<Word8*>(pOutput)[16] = pSource[16];
					static_cast<Word8*>(pOutput)[17] = pSource[17];
				}
				pOutput = static_cast<Word8*>(pOutput)+18;
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
		return DECOMPRESS_OUTPUTUNDERRUN;
	}

	// Input data remaining?
	if (uInputChunkLength || (m_eState==STATE_RUN)) {
		return DECOMPRESS_OUTPUTOVERRUN;
	}
	// Decompression is complete
	return DECOMPRESS_OKAY;
}

/***************************************

	Decompress using LZSS.
	I assume I can decompress in one pass.
	This makes for fast and tight code.

***************************************/

Burger::Decompress::eError BURGER_API Burger::SimpleDecompressLZSS(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength)
{
	Burger::DecompressLZSS Local;
	Local.DecompressLZSS::Reset();
	return Local.DecompressLZSS::Process(pOutput,uOutputChunkLength,pInput,uInputChunkLength);
}
