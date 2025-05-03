/***************************************

    Compress using ILBM RLE

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brcompresslbmrle.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::CompressILBMRLE,Burger::Compress);
#endif

/*! ************************************

	\class Burger::CompressILBMRLE
	\brief Compress data using ILBM Run Length Encoding

	This simple format encodes a data stream by finding runs of duplicated data
	and encodes the duplicates into a count/byte pair. Otherwise, it's a
	count followed by a stream of raw data.

	The format is as follows, a byte is taken which is either
	above 128 or less than 128. If higher than 128, it's a repeater,
	if less than 128, it's a count.

	128 is considered a "No Operation" or end of line for the data stream.
	As such, 128 is not allowed to be encoded.

	\code
		uint8_t uToken = pDataStream[0];
		if (uToken>128) {
			uint_t uCount = 257-uToken;
			memory_set(pOutput,pDataStream[1],uCount);
			pDataStream+=2;
		} else if (uToken<128) {
			uint_t uCount = uToken+1;
			memory_copy(pOutput,pDataStream+1,uCount);
			pDataStream+=uCount+1;
		} else if (uToken==128) {
			// Do nothing, as per ILBM specifications
		}
	\endcode

	\sa Burger::DecompressILBMRLE 

***************************************/


/*! ************************************

	\brief Compress the input data using RLE

	Compresses the data using RLE and stores the compressed
	data into an OutputMemoryStream. If there is any excess data,
	record the size in CompressILBMRLE::m_uRemaining
	
***************************************/

Burger::eError Burger::CompressILBMRLE::Compact(const uint8_t *pInput,uintptr_t uInputLength)
{
	// Assume no data remaining
	m_uRemaining = 0;
	if (uInputLength) {
		// Normal compressor

		do {
			// If only one byte, then store and exit
			if (uInputLength==1) {
				m_uRemaining = uInputLength;
				break;
			}
			// Check for repeater
			uint_t uMatchTest = pInput[0];
			// Is there a run?
			if (pInput[1] == uMatchTest) {

				// Maximum loops to try
				uintptr_t uMaximumRun = 129-2;
				if (uInputLength<129) {
					uMaximumRun = uInputLength-2;	// 0-127
					if (!uMaximumRun) {
						m_uRemaining = uInputLength;
						break;
					}
				}

				uintptr_t uRun = 2;
				do {
					// Find end of repeater
					if (pInput[uRun]!=uMatchTest) {
						break;
					}
					// 1 more to run
					++uRun;
				} while (--uMaximumRun);

				// Test for the end of buffer
				if ((uInputLength<129) &&
					(!uMaximumRun)) {
					m_uRemaining = uInputLength;
					break;
				}
				// Perform a run length token Run=2-129
				// Encode 128-255
				m_Output.Append(static_cast<uint8_t>(257-uRun));
				m_Output.Append(static_cast<uint8_t>(uMatchTest));
				uInputLength-=uRun;
				pInput = pInput+uRun;
			} else {
				// Raw run, minimum size of 2 bytes
				uintptr_t uMaximumRun = 128-2;
				if (uInputLength<128) {
					uMaximumRun = uInputLength-2;
					if (!uMaximumRun) {
						m_uRemaining = uInputLength;
						break;
					}
				}
				// Preload the next byte
				uMatchTest = pInput[1];
				uintptr_t uRun = 2;
				do {	
					// Scan for next repeater
					if (pInput[uRun]==uMatchTest) {
						// Remove from the run
						--uRun;
						break;
					}
					// Get the next byte
					uMatchTest = pInput[uRun];
					// +1 to the run
					++uRun;		
				} while (--uMaximumRun);
				if ((uInputLength<128) &&
					(!uMaximumRun)) {
					m_uRemaining = uInputLength;
					break;
				}
				// Perform a raw data transfer
				// Run 1-128
				// Encode 0-127
				m_Output.Append(static_cast<uint8_t>(uRun-1));
				m_Output.Append(pInput,uRun);
				uInputLength-=uRun;
				pInput = pInput+uRun;
			}
		} while (uInputLength);
	}
	return kErrorNone;
}


/*! ************************************

	\brief Default constructor

	Initializes the cache buffer.

***************************************/

Burger::CompressILBMRLE::CompressILBMRLE() :
	Compress(),
	m_uCacheUsed(0),
	m_uRemaining(0)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Reset the RLE compressor

	Resets the cache buffer.

***************************************/

Burger::eError Burger::CompressILBMRLE::Init(void)
{
	m_Output.Clear();
	m_uRemaining = 0;
	m_uCacheUsed = 0;
	return kErrorNone;
}

/*! ************************************

	\brief Compress the input data using RLE

	Compresses the data using RLE and stores the compressed
	data into an OutputMemoryStream

***************************************/

Burger::eError Burger::CompressILBMRLE::Process(const void *pInput,uintptr_t uInputLength)
{
	eError Error = kErrorNone;
	if (uInputLength) {
		uintptr_t uRemaining;

		// Was there data remaining to be processed from the last call?
		uintptr_t uCacheUsed = m_uCacheUsed;
		if (uCacheUsed) {
			do {
				// Process the cache first
				// Get the maximum number of bytes that can be appended to the cache
				uintptr_t uInputChunk = sizeof(m_Cache)-uCacheUsed;
				// Is there enough input to fill the cache?
				if (uInputLength<uInputChunk) {
					uInputChunk = uInputLength;
				}
				// Append the input to the cache
				memory_copy(&m_Cache[uCacheUsed],pInput,uInputChunk);
				// Bytes to process
				uintptr_t uTotal = uCacheUsed+uInputChunk;
				// Apply compression
				Error = Compact(m_Cache,uTotal);

				// Check for the simple case first which is a full cache flush
				// and data remains in the pInput buffer
				
				uRemaining = m_uRemaining;
				// Number of bytes actually processed
				uintptr_t uActual = uTotal-uRemaining;

				if (uActual>=uCacheUsed) {
					// The cache is flushed. Process the rest
					uintptr_t uRemoved = uActual-uCacheUsed;
					pInput = static_cast<const uint8_t *>(pInput)+uRemoved;
					uInputLength -= uRemoved;
					m_uCacheUsed = 0;		// Clear the cache
					break;
				}
				// The cache wasn't flushed. Could be because of
				// a strange run or not enough new input.

				// Accept all parsed input as cached

				pInput = static_cast<const uint8_t *>(pInput)+uInputChunk;
				uInputLength -= uInputChunk;

				// Update the cache
				memory_copy(m_Cache,&m_Cache[uActual],uRemaining);
				uCacheUsed = uRemaining;
				m_uCacheUsed = uCacheUsed;

				// uInputLength could be zero if there was not enough to fill the
				// cache, in that case, the work is done.
			} while (uInputLength);
		}

		// If there is any input remaining, it's okay to assume the cache is clear
		// Perform the final compression using the remaining buffer

		if (uInputLength) {
			// Compress the remaining data
			Error = Compact(static_cast<const uint8_t *>(pInput),uInputLength);
			// If any data remains. Store in the cache for later processing
			uRemaining = m_uRemaining;
			m_uCacheUsed = uRemaining;
			if (uRemaining) {
				memory_copy(m_Cache,static_cast<const uint8_t *>(pInput)+(uInputLength-uRemaining),uRemaining);
			}
		}
	}
	return Error;
}

/*! ************************************

	\brief Finalize RLE compression

	If any data has been cached from the compression stream,
	flush it into the output

	\return Zero if no error, non-zero on error

***************************************/

Burger::eError Burger::CompressILBMRLE::Finalize(void)
{
	// Are there bytes in the cache?
	uintptr_t uCached = m_uCacheUsed;
	if (uCached) {
		// Handle the single byte as a 1 byte memory copy
		if ((uCached!=1) &&
			(m_Cache[0]==m_Cache[1])) {
			m_Output.Append(static_cast<uint8_t>(257-uCached));
			m_Output.Append(m_Cache[0]);
		} else {
			m_Output.Append(static_cast<uint8_t>(uCached-1));
			m_Output.Append(m_Cache,uCached);
		}
		m_uCacheUsed = 0;
	}
	return kErrorNone;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::CompressILBMRLE::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
