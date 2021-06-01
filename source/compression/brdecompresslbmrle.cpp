/***************************************

    Decompression manager version of RLE

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdecompresslbmrle.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressILBMRLE,Burger::Decompress);
#endif

/*! ************************************

	\class Burger::DecompressILBMRLE
	\brief Decompress RLE format
	
	Decompress data in RLE format (Documented here Burger::CompressRLE )
	Token >=0x80 = 0x101-Token Repeat (Min 2, Max 129)
	Token < 0x80 = Token+1 Data[Token+1] (Min 1, Max 128)

	\sa Burger::Decompress and Burger::CompressILBMRLE

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressILBMRLE::DecompressILBMRLE(void) :
	Decompress(),
	m_uRun(0),
	m_uFill(0),
	m_eState(STATE_INIT)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Reset the RLE decompression

	\return kErrorNone (No error is possible)

***************************************/

Burger::eError Burger::DecompressILBMRLE::Reset(void)
{
	m_uTotalOutput = 0;
	m_uTotalInput = 0;
	m_eState = STATE_INIT;
	return kErrorNone;
}

/*! ************************************

	\brief Decompress data using RLE compression

	Using the RLE compression algorithm, decompress the data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa Burger::SimpleDecompressILBMRLE()

***************************************/

Burger::eError Burger::DecompressILBMRLE::Process(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	m_uInputLength = uInputChunkLength;
	m_uOutputLength = uOutputChunkLength;

	// Default state is assumed

	eState MyState = m_eState;
	if (uInputChunkLength || (MyState!=STATE_INIT)) {
		m_eState = STATE_INIT;		// Reset the state

		// Restore the state variables

		uintptr_t uRunLength = m_uRun;	// Restore the run length
		uint_t uFillTemp = m_uFill;		// Fill value

		if (MyState==STATE_FILLTOKEN) {
			goto FillToken;
		}
		if (MyState==STATE_FILL) {
			goto Fill;
		}
		if (MyState==STATE_RUN) {
			goto Run; 
		}

		do {
			// Get the run token
			uRunLength = static_cast<const uint8_t *>(pInput)[0];
			pInput = static_cast<const uint8_t *>(pInput)+1;
			--uInputChunkLength;

			if (uRunLength>=128) {		// Run length?
				uRunLength = 257-uRunLength;	// Count the run (2-129)

				if (!uInputChunkLength) {
					m_eState = STATE_FILLTOKEN;
					m_uRun = uRunLength;
					break;
				}
FillToken:
				// Filler value
				uFillTemp = static_cast<const uint8_t *>(pInput)[0];
				pInput = static_cast<const uint8_t *>(pInput)+1;
				--uInputChunkLength;

				// Perform a memory fill
Fill:
				if (uOutputChunkLength<uRunLength) {
					// Save the memory fill state
					m_eState = STATE_FILL;
					m_uRun = uRunLength-uOutputChunkLength;
					m_uFill = uFillTemp;
					if (uOutputChunkLength) {
						do {
							static_cast<uint8_t *>(pOutput)[0] = static_cast<uint8_t>(uFillTemp);
							pOutput = static_cast<uint8_t *>(pOutput)+1;
						} while (--uOutputChunkLength);
					}
					break;
				}
				// Perform the memory fill
				uOutputChunkLength -= uRunLength;
				do {
					static_cast<uint8_t *>(pOutput)[0] = static_cast<uint8_t>(uFillTemp);
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uRunLength);

			} else {

				// Handle a memory copy
				++uRunLength;		// +1 to the count
Run:
				if ((uOutputChunkLength<uRunLength) ||
					(uInputChunkLength<uRunLength)) {
					// Use the smaller run
					uintptr_t uMaxRun = uRunLength;
					if (uInputChunkLength<uMaxRun) {
						uMaxRun = uInputChunkLength;
					}
					if (uOutputChunkLength<uMaxRun) {
						uMaxRun = uOutputChunkLength;
					}
					// Store the remainder for the next time
					uInputChunkLength -= uMaxRun;
					uOutputChunkLength -= uMaxRun;
					m_eState = STATE_RUN;
					m_uRun = uRunLength-uMaxRun;
					if (uMaxRun) {
						do {
							static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[0];
							pInput = static_cast<const uint8_t *>(pInput)+1;
							pOutput = static_cast<uint8_t *>(pOutput)+1;
						} while (--uMaxRun);
					}
					break;
				}
				// Perform the memory copy
				uOutputChunkLength -= uRunLength;
				uInputChunkLength -= uRunLength;
				do {
					static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[0];
					pInput = static_cast<const uint8_t *>(pInput)+1;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uRunLength);
			}
		} while (uInputChunkLength);	// More?
	}

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
	if (uInputChunkLength || (m_eState!=STATE_INIT)) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}

/*! ************************************

	\brief Decompress data using RLE compression

	Using the RLE compression algorithm, decompress the data in one pass

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa DecompressILBMRLE::Process()

***************************************/

Burger::eError BURGER_API Burger::SimpleDecompressILBMRLE(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	Burger::DecompressILBMRLE Local;
	Local.DecompressILBMRLE::Reset();
	return Local.DecompressILBMRLE::Process(pOutput,uOutputChunkLength,pInput,uInputChunkLength);
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressILBMRLE::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
