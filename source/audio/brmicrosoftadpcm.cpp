/***************************************

    Microsoft ADPCM decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmicrosoftadpcm.h"
#include "brmemoryfunctions.h"
#include "brfixedpoint.h"
#include "brendian.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressMicrosoftADPCM,Burger::DecompressAudio);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressMicrosoftADPCM::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/


//
// Coefficient tables
// Merged together for ensuring that they are grouped together in the cache
//

#if !defined(DOXYGEN)
static const int32_t g_Table[16+16+7+7] = {
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230,
//static const int32_t g_DeltaCodeTable[16] = {
	0,1,2,3,4,5,6,7,-8,-7,-6,-5,-4,-3,-2,-1,
//static const int32_t g_GainCoef1[7] = {
	256, 512, 0, 192, 240, 460,  392,
//static const int32_t g_GainCoef2[7] = {
	0, -256,  0,  64,   0,-208, -232};
#endif

/*! ************************************

	\brief Given a 4 bit sample, process a sample

	Decode a sample and update the state tables
	Each sample is 4 bits in size

***************************************/

int32_t BURGER_API Burger::ADPCMState_t::Decode(uint_t uDeltaCode)
{
	// Compute next Adaptive Scale Factor (ASF)

	uDeltaCode = uDeltaCode&0x0FU;
	int32_t iIndex = m_iIndex;
	int32_t iNewIndex = (g_Table[uDeltaCode] * iIndex) >> 8;

	// Clamp to table size
	if (iNewIndex < 16) {
		iNewIndex = 16;
	}
	m_iIndex = iNewIndex;

	iIndex = g_Table[uDeltaCode+16]*iIndex;

	// Predict next sample

	int32_t iPredict = (m_iFirstSample * m_iCoef2);	// Get first coefficient
	int32_t iSample = m_iSecondSample;					// Copy to temp
	m_iFirstSample = iSample;						// Move to first
	iSample = ((iSample * m_iCoef1)+iPredict) >> 8;

	iIndex = iIndex + iSample;	// Get the output value

	if (iIndex > 32767) {		// In bounds for a short?
		iIndex = 32767;
	} else if (iIndex <= -32768) {
		iIndex = -32768;
	}
	m_iSecondSample = iIndex;
	return iIndex;
}

/*! ************************************

	\brief Obtain the coefficients from the ADPCM stream

	From a stream of 7 bytes, initialize the coefficient tables
	for mono ADPCM decoding

	\param pInput Pointer to a stream of 7 bytes to extract the coefficients from

***************************************/

void BURGER_API Burger::DecompressMicrosoftADPCM::SetMonoDecoder(const void *pInput)
{
	// Reset the decompresser

	// Which base coefficient
	uint_t uBase = static_cast<const uint8_t *>(pInput)[0];

	if (uBase >= 7) {
		uBase = 6;			// Overflow
	}
	m_Decoders[0].m_iCoef1 = g_Table[uBase+32];
	m_Decoders[0].m_iCoef2 = g_Table[uBase+(32+7)];

	// This data is not guaranteed to be 16 bit aligned
	m_Decoders[0].m_iIndex = LittleEndian::load_unaligned(static_cast<const int16_t *>(static_cast<const void *>(static_cast<const uint8_t *>(pInput)+1)));
	m_Decoders[0].m_iSecondSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(static_cast<const void *>(static_cast<const uint8_t *>(pInput)+3)));
	m_Decoders[0].m_iFirstSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(static_cast<const void *>(static_cast<const uint8_t *>(pInput)+5)));
}

/*! ************************************

	\brief Obtain the coefficients from the ADPCM stream

	From a stream of 14 bytes, initialize the coefficient tables
	for stereo ADPCM decoding

	\param pInput Pointer to a stream of 14 bytes to extract the coefficients from

***************************************/

void BURGER_API Burger::DecompressMicrosoftADPCM::SetStereoDecoder(const void *pInput)
{
	uint_t uBase = static_cast<const uint8_t *>(pInput)[0];	// Left
	if (uBase >= 7) {
		uBase = 6;			// Overflow
	}
	m_Decoders[0].m_iCoef1 = g_Table[uBase+32];
	m_Decoders[0].m_iCoef2 = g_Table[uBase+(32+7)];

	uBase = static_cast<const uint8_t *>(pInput)[1];	// Right
	if (uBase >= 7) {
		uBase = 6;			// Overflow
	}
	m_Decoders[1].m_iCoef1 = g_Table[uBase+32];
	m_Decoders[1].m_iCoef2 = g_Table[uBase+(32+7)];

	
	m_Decoders[0].m_iIndex = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+1);
	m_Decoders[1].m_iIndex = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+2);

	m_Decoders[0].m_iSecondSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+3);
	m_Decoders[1].m_iSecondSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+4);

	m_Decoders[0].m_iFirstSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+5);
	m_Decoders[1].m_iFirstSample = LittleEndian::load_unaligned(static_cast<const int16_t *>(pInput)+6);
}

/*! ************************************

	\brief Decode the a block of ADPCM samples

	Function that will process a buffer of ADPCM samples and stores them
	into the output buffer.

	\note This function will only process a single block of ADPCM data, note
	the entire input buffer

	\param pOutput Buffer to get the decompressed sound data
	\param pInput Pointer to ADPCM data
	\param uInputLength Number of bytes of ADPCM data
	\return Number of bytes output

***************************************/

uint_t BURGER_API Burger::DecompressMicrosoftADPCM::ADPCMDecodeBlock(int16_t *pOutput,const uint8_t *pInput,uintptr_t uInputLength)
{
	// Pull in the packet and check the header

	uint32_t Temp = m_uBlockSize;
	if (uInputLength<Temp) {
		Temp = static_cast<uint_t>(uInputLength);
	}
	uInputLength-=Temp;
	pInput=&pInput[Temp];			// Adjust the source pointer

	uint_t uSamplesThisBlock;
	if (Temp < m_uBlockSize) {		// Partial block?

	// If it looks like a valid header is around then try and
	// work with partial blocks. Specs say it should be null
	// padded but I guess this is better then trailing quiet.
		uint_t uChannels = m_bStereo ? 2U : 1U;
		if (Temp < (7 * uChannels)) {
			return 0;		// No bytes decoded!
		}
		uSamplesThisBlock = (m_uBlockSize - (6 * uChannels));
	} else {
		uSamplesThisBlock = m_uSamplesPerBlock;
	}

	// Now, I decompress differently for mono or stereo

	if (!m_bStereo) {
		// Mono
		// Read the four-byte header for each channel

		// Reset the decompresser
		Temp = pInput[0];		// Left
		// 7 should be variable from AVI/WAV header
		if (Temp < 7) {
			m_Decoders[0].m_iCoef1 = g_Table[Temp+32];
			m_Decoders[0].m_iCoef2 = g_Table[Temp+(32+7)];

			m_Decoders[0].m_iIndex = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+1));
			m_Decoders[0].m_iSecondSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+3));
			m_Decoders[0].m_iFirstSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+5));

			/* Decode two samples for the header */
			pOutput[0] = static_cast<int16_t>(m_Decoders[0].m_iFirstSample);
			pOutput[1] = static_cast<int16_t>(m_Decoders[0].m_iSecondSample);

			pInput += 7;
			pOutput += 2;

			// Decompress nibbles. Minus 2 included in header

			if (uSamplesThisBlock>2) {
				uint_t uRemaining = (uSamplesThisBlock-2)>>1;
				do {
					Temp = pInput[0];
					++pInput;
					pOutput[0] = static_cast<int16_t>(m_Decoders[0].Decode(Temp>>4));
					pOutput[1] = static_cast<int16_t>(m_Decoders[0].Decode(Temp));
					pOutput+=2;
				} while (--uRemaining);
			}
			return uSamplesThisBlock*2;
		}
		return 0;
	}

	//
	// Stereo
	//

	// Read the four-byte header for each channel

	// Reset the decompresser
	Temp = pInput[0];	// Left
	// 7 should be variable from AVI/WAV header
	if (Temp < 7) {
		m_Decoders[0].m_iCoef1 = g_Table[Temp+32];
		m_Decoders[0].m_iCoef2 = g_Table[Temp+(32+7)];

		Temp = pInput[1];	// Right
		if (Temp < 7) {
			m_Decoders[1].m_iCoef1 = g_Table[Temp+32];
			m_Decoders[1].m_iCoef2 = g_Table[Temp+(32+7)];

			m_Decoders[0].m_iIndex = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+2));
			m_Decoders[1].m_iIndex = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+4));

			m_Decoders[0].m_iSecondSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+6));
			m_Decoders[1].m_iSecondSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+8));

			m_Decoders[0].m_iFirstSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+10));
			m_Decoders[1].m_iFirstSample = LittleEndian::load_unaligned(reinterpret_cast<const int16_t *>(pInput+12));

			// Decode two samples for the header
			pOutput[0] = static_cast<int16_t>(m_Decoders[0].m_iFirstSample);
			pOutput[1] = static_cast<int16_t>(m_Decoders[1].m_iFirstSample);
			pOutput[2] = static_cast<int16_t>(m_Decoders[0].m_iSecondSample);
			pOutput[3] = static_cast<int16_t>(m_Decoders[1].m_iSecondSample);
			pOutput+=4;
			pInput+=14;
			
			// Decompress nibbles. Minus 2 included in header
			if (uSamplesThisBlock>2) {
				uint_t uRemaining = uSamplesThisBlock-2;
				do {
					Temp = pInput[0];
					++pInput;
					pOutput[0] = static_cast<int16_t>(m_Decoders[0].Decode(Temp>>4));
					pOutput[1] = static_cast<int16_t>(m_Decoders[1].Decode(Temp));
					pOutput+=2;
				} while (--uRemaining);
			}
			return uSamplesThisBlock*4;
		}
	}
	return 0;
}




/*! ************************************

	\class Burger::DecompressMicrosoftADPCM
	\brief Decompress Microsoft ADPCM format
	
	Decompress audio data in Microsoft ADPCM format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressMicrosoftADPCM::DecompressMicrosoftADPCM() :
	DecompressAudio(SoundManager::TYPESHORT)
{
	m_uSignature = Signature;
	// Clear out the state
	DecompressMicrosoftADPCM::Reset();
}

/*! ************************************

	\brief Resets the MAC6 decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::DecompressMicrosoftADPCM::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	m_eState = STATE_INIT;
	m_uCacheCount = 0;
	m_uCacheSize = 0;
	// No worries!
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using Microsoft ADPCM compression

	Using the Microsoft version of ADPCM compression algorithm, decompress the audio data data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa DecompressAudio

***************************************/

Burger::eError Burger::DecompressMicrosoftADPCM::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
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

	const void *pInputChunk = NULL;
	void *pDest = NULL;
	eState uState = m_eState;
	uint_t bAbort = FALSE;
	do {
		switch (uState) {

		// Init the decoder for mono or stereo decoding
		case STATE_INIT:
			if (!m_bStereo) {
				uState = STATE_INITMONO;
			} else {
				uState = STATE_INITSTEREO;
			}
			break;

		//
		// Obtain the coefficients from the input stream
		//
		case STATE_INITMONO:
			if (uInputChunkLength<7) {
				// Put the data into the cache
				m_eNextState = STATE_GETDECODERMONO;
				m_uCacheSize = 7;
				m_uCacheCount = 0;
				uState = STATE_FILLINGCACHE;
				break;
			}
			// Consume the input chunk directly
			pInputChunk = pInput;
			pInput = static_cast<const uint8_t *>(pInput)+7;
			uInputChunkLength-=7;
			uState = STATE_GETDECODERMONO;

		//
		// Process the table and then write out the first two samples
		//
		case STATE_GETDECODERMONO:
			// Initialize the decoder
			SetMonoDecoder(pInputChunk);

			// Write out the samples to the stream or the cache
			if (uOutputChunkLength<4) {
				// Store into the cache
				m_uCacheCount = 4;
				m_uCacheSize = 4;
				m_eNextState = STATE_WRITESAMPLESMONO;
				uState = STATE_CACHEFULL;
				pDest = m_Cache;
			} else {
				pDest = pOutput;
				pOutput = static_cast<uint8_t *>(pOutput)+4;
				uOutputChunkLength-=4;
				uState = STATE_WRITESAMPLESMONO;
			}
			static_cast<int16_t *>(pDest)[0] = static_cast<int16_t>(m_Decoders[0].m_iFirstSample);
			static_cast<int16_t *>(pDest)[1] = static_cast<int16_t>(m_Decoders[0].m_iSecondSample);
			break;

		//
		// Write out the rest of the samples, decoded from 4 bits to 16
		//
		case STATE_WRITESAMPLESMONO:
			{
				uint_t uSteps = m_uSamplesPerBlock;
				if (uSteps<=2) {
					// Nothing to do, reset
					uState = STATE_INITMONO;
					break;
				}

				// Take into account the samples processed and begin writing
				m_uSamplesRemaining = uSteps-2;
				uState = STATE_WRITINGSAMPLESMONO;
			}

		case STATE_WRITINGSAMPLESMONO:

			// This state cannot continue without input data
			if (!uInputChunkLength) {
				bAbort = TRUE;
			
			} else {
				uint_t uSamplesRemaining = m_uSamplesRemaining;

				//
				// Quickly process the data in the most common case where
				// enough input and output data exist
				//

				// Clamp to input
				uint_t uCounter = static_cast<uint_t>(minimum(static_cast<uintptr_t>(uSamplesRemaining>>1U),uInputChunkLength));
				// Clamp to output
				uCounter = static_cast<uint_t>(minimum(static_cast<uintptr_t>(uCounter),uOutputChunkLength>>2U));

				// Write out the fast chunks
				if (uCounter) {
					// Consume
					uInputChunkLength -= uCounter;			// Bytes
					uOutputChunkLength -= uCounter*4;		// Bytes -> 2 shorts
					uSamplesRemaining -= uCounter*2;		// Bytes -> nibbles
					do {
						uint_t uNibbles = static_cast<const uint8_t *>(pInput)[0];
						pInput = static_cast<const uint8_t *>(pInput)+1;
						static_cast<int16_t *>(pOutput)[0] = static_cast<int16_t>(m_Decoders[0].Decode(uNibbles>>4U));
						static_cast<int16_t *>(pOutput)[1] = static_cast<int16_t>(m_Decoders[0].Decode(uNibbles));
						pOutput = static_cast<int16_t *>(pOutput)+2;
					} while (--uCounter);
					// Processed everything?
					if (!uSamplesRemaining) {
						// Nothing more to do, reset
						uState = STATE_INITMONO;
						break;
					}
				}

				//
				// If the code made it here, it means that the cache needs to
				// get involved. Do it the slow way
				//

				// No data to read?
				if (!uInputChunkLength) {
					// Stay in this state
					bAbort = TRUE;
				} else {
					//
					// Looks like it's got an output problem. 
					// Let the output cache deal with it.
					//
					uint_t uNibble = static_cast<const uint8_t *>(pInput)[0];
					pInput = static_cast<const uint8_t *>(pInput)+1;
					--uInputChunkLength;

					// Write into the cache
					pDest = m_Cache;
					static_cast<int16_t *>(pDest)[0] = static_cast<int16_t>(m_Decoders[0].Decode(uNibble>>4U));
					static_cast<int16_t *>(pDest)[1] = static_cast<int16_t>(m_Decoders[0].Decode(uNibble));

					// The special case where there's an odd number of samples.

					if (uSamplesRemaining<2) {
						uSamplesRemaining = 0;
						m_uCacheCount = 2;
						m_uCacheSize = 2;
					} else {
						// Mark as processed
						uSamplesRemaining -= 2;
						m_uCacheCount = 4;
						m_uCacheSize = 4;
					}
					// Where to go after the cache is flushed?
					if (uSamplesRemaining) {
						m_eNextState = STATE_WRITINGSAMPLESMONO;
					} else {
						m_eNextState = STATE_INITMONO;
					}
					// Output the cache
					uState = STATE_CACHEFULL;
				}
				// Update the samples
				m_uSamplesRemaining = uSamplesRemaining;
			}
			break;

		//
		// Obtain the coefficients from the input stream
		//
		case STATE_INITSTEREO:
			if (uInputChunkLength<14) {
				// Put the data into the cache
				m_eNextState = STATE_GETDECODERSTEREO;
				m_uCacheSize = 14;
				m_uCacheCount = 0;
				uState = STATE_FILLINGCACHE;
				break;
			}
			// Consume the input chunk directly
			pInputChunk = pInput;
			pInput = static_cast<const uint8_t *>(pInput)+14;
			uInputChunkLength-=14;
			uState = STATE_GETDECODERSTEREO;

		//
		// Process the table and then write out the first four samples
		//
		case STATE_GETDECODERSTEREO:
			// Initialize the decoder
			SetStereoDecoder(pInputChunk);

			// Write out the samples to the stream or the cache
			if (uOutputChunkLength<8) {
				// Store into the cache
				m_uCacheCount = 8;
				m_uCacheSize = 8;
				m_eNextState = STATE_WRITESAMPLESSTEREO;
				uState = STATE_CACHEFULL;
				pDest = m_Cache;
			} else {
				pDest = pOutput;
				pOutput = static_cast<uint8_t *>(pOutput)+8;
				uOutputChunkLength-=8;
				uState = STATE_WRITESAMPLESSTEREO;
			}
			static_cast<int16_t *>(pDest)[0] = static_cast<int16_t>(m_Decoders[0].m_iFirstSample);
			static_cast<int16_t *>(pDest)[1] = static_cast<int16_t>(m_Decoders[0].m_iFirstSample);
			static_cast<int16_t *>(pDest)[2] = static_cast<int16_t>(m_Decoders[0].m_iSecondSample);
			static_cast<int16_t *>(pDest)[3] = static_cast<int16_t>(m_Decoders[0].m_iSecondSample);
			break;
			
		//
		// Write out the rest of the samples, decoded from 4 bits to 16
		//
		case STATE_WRITESAMPLESSTEREO:
			{
				uint_t uSteps = m_uSamplesPerBlock;
				if (uSteps<=2) {
					// Nothing to do, reset
					uState = STATE_INITSTEREO;
					break;
				}

				// Take into account the samples processed and begin writing
				m_uSamplesRemaining = uSteps-2;
				uState = STATE_WRITINGSAMPLESSTEREO;
			}

		case STATE_WRITINGSAMPLESSTEREO:

			// This state cannot continue without input data
			if (!uInputChunkLength) {
				bAbort = TRUE;

			} else {
				uint_t uSamplesRemaining = m_uSamplesRemaining;

				//
				// Quickly process the data in the most common case where
				// enough input and output data exist
				//

				// Clamp to input
				uint_t uCounter = static_cast<uint_t>(minimum(static_cast<uintptr_t>(uSamplesRemaining),uInputChunkLength));
				// Clamp to output
				uCounter = static_cast<uint_t>(minimum(static_cast<uintptr_t>(uCounter),uOutputChunkLength>>2U));

				// Write out the fast chunks
				if (uCounter) {
					// Consume
					uInputChunkLength -= uCounter;			// Bytes
					uOutputChunkLength -= uCounter*4;		// Bytes -> 2 shorts
					uSamplesRemaining -= uCounter;			// Bytes -> 2 nibbles
					do {
						uint_t uNibbles = static_cast<const uint8_t *>(pInput)[0];
						pInput = static_cast<const uint8_t *>(pInput)+1;
						// Left
						static_cast<int16_t *>(pOutput)[0] = static_cast<int16_t>(m_Decoders[0].Decode(uNibbles>>4U));
						// Right
						static_cast<int16_t *>(pOutput)[1] = static_cast<int16_t>(m_Decoders[1].Decode(uNibbles));
						pOutput = static_cast<int16_t *>(pOutput)+2;
					} while (--uCounter);
					// Processed everything?
					if (!uSamplesRemaining) {
						// Nothing more to do, reset
						uState = STATE_INITSTEREO;
						break;
					}
				}

				//
				// If the code made it here, it means that the cache needs to
				// get involved. Do it the slow way
				//

				// No data to read?
				if (!uInputChunkLength) {
					// Stay in this state
					bAbort = TRUE;
				} else {
					//
					// Looks like it's got an output problem. 
					// Let the output cache deal with it.
					//
					uint_t uNibble = static_cast<const uint8_t *>(pInput)[0];
					pInput = static_cast<const uint8_t *>(pInput)+1;
					--uInputChunkLength;

					// Write into the cache
					pDest = m_Cache;
					static_cast<int16_t *>(pDest)[0] = static_cast<int16_t>(m_Decoders[0].Decode(uNibble>>4U));
					static_cast<int16_t *>(pDest)[1] = static_cast<int16_t>(m_Decoders[1].Decode(uNibble));

					// The special case where there's an odd number of samples.

					// Mark as processed
					--uSamplesRemaining;
					m_uCacheCount = 4;
					m_uCacheSize = 4;

					// Where to go after the cache is flushed?
					if (uSamplesRemaining) {
						m_eNextState = STATE_WRITINGSAMPLESSTEREO;
					} else {
						m_eNextState = STATE_INITSTEREO;
					}
					// Output the cache
					uState = STATE_CACHEFULL;
				}
				// Update the samples
				m_uSamplesRemaining = uSamplesRemaining;
			}
			break;


	
		//
		// Fill up the cache for later processing
		//
		case STATE_FILLINGCACHE:
			bAbort = TRUE;			// Assume data starved
			if (uInputChunkLength) {

				// Get the number of bytes already obtained
				uintptr_t uCacheSize = m_uCacheCount;

				// How many is needed to fill
				uintptr_t uRemaining = m_uCacheSize-uCacheSize;

				// Number of bytes to process
				uintptr_t uChunk = minimum(uRemaining,uInputChunkLength);

				// Fill in the cache
				MemoryCopy(&m_Cache[uCacheSize],pInput,uChunk);

				// Consume the input bytes

				pInput = static_cast<const uint8_t *>(pInput)+uChunk;
				uInputChunkLength-=uChunk;

				// Did the cache fill up?
				uCacheSize += uChunk;
				m_uCacheCount = static_cast<uint_t>(uCacheSize);
				if (uCacheSize==m_uCacheSize) {
					// Cache is full, send to processing
					bAbort = FALSE;
					uState = m_eNextState;
					pInputChunk = m_Cache;
				}
			}
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			bAbort = TRUE;			// Assume output buffer is full
			if (uOutputChunkLength) {

				// Output data from the cache

				uintptr_t uCacheCount = m_uCacheCount;
				uintptr_t uSteps = minimum(uOutputChunkLength,static_cast<uintptr_t>(uCacheCount));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				pInputChunk = &m_Cache[m_uCacheSize-uCacheCount];

				// Update the cache size
				uCacheCount = static_cast<uint_t>(uCacheCount-uSteps);

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t*>(pInputChunk)[0];
					pInputChunk = static_cast<const uint8_t *>(pInputChunk)+1;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);

				// Data still in the cache?
				if (uCacheCount) {
					// Update and exit
					m_uCacheCount = static_cast<uint_t>(uCacheCount);
				} else {
					// Cache is empty, so switch to the next state
					uState = m_eNextState;
					bAbort = FALSE;
				}
			}
			break;
		}
	} while (!bAbort);

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));

	// Store the amount of data that was processed

	m_uInputLength = uInputConsumed;
	m_uOutputLength = uOutputConsumed;

	// Add the decompressed data to the totals
	m_uTotalInput += uInputConsumed;
	m_uTotalOutput += uOutputConsumed;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uOutputConsumed) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength!=uInputConsumed) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}


/*! ************************************

	\brief Allocate and initialize a DecompressMicrosoftADPCM

	\return A pointer to a default DecompressMicrosoftADPCM class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressMicrosoftADPCM * BURGER_API Burger::DecompressMicrosoftADPCM::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressMicrosoftADPCM))) DecompressMicrosoftADPCM();
}


/*! ************************************

	\fn void Burger::DecompressMicrosoftADPCM::SetBlockSize(uint_t uBlockSize)
	\brief Set the block size for decompressing ADPCM data

	\param uBlockSize Size, in bytes, of each ADPCM block

***************************************/

/*! ************************************

	\fn void Burger::DecompressMicrosoftADPCM::SetSamplesPerBlock(uint_t uSamplesPerBlock)
	\brief Set the number of samples in a block of ADPCM data

	When decompressing ADPCM data, it's necessary to have the samples per block
	value since this value is not stored in the data stream.

	\param uSamplesPerBlock Number of audio samples in a block

***************************************/

