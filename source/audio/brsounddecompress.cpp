/***************************************

    Sound decompression classes

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brsounddecompress.h"
#include "brendian.h"
#include "brfilelbm.h"
#include "brfixedpoint.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressAudio,Burger::Decompress);
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressUnsigned8BitAudio,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressSigned8BitAudio,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Decompress16BitBEAudio,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Decompress16BitLEAudio,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Decompress32BitBEAudio,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Decompress32BitLEAudio,Burger::DecompressAudio);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressAudio::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressUnsigned8BitAudio::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressSigned8BitAudio::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
/*! ************************************

	\var const Burger::StaticRTTI Burger::Decompress16BitBEAudio::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
/*! ************************************

	\var const Burger::StaticRTTI Burger::Decompress16BitLEAudio::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/



/*! ************************************

	\class Burger::DecompressAudio
	\brief Base class for audio decompression codec

	This is the class used for audio decompression. It only
	differs from a standard \ref Decompress class
	by the addition of a call that will return the
	preferred data type the decompresser will output.

	\sa Decompress

***************************************/

/*! ************************************

	\brief Constructor

	Sets the defaults (Mono)

	\sa SoundManager::eDataType

***************************************/

Burger::DecompressAudio::DecompressAudio(SoundManager::eDataType uDataType) :
	m_eDataType(uDataType),
	m_bStereo(FALSE)
{
}

/*! ************************************

	\fn SoundManager::eDataType Burger::DecompressAudio::GetDataType(void) const
	\brief Returns the uncompressed data type this codec will output.

	\sa SoundManager::eDataType or Decompress

***************************************/

/*! ************************************

	\fn void Burger::DecompressAudio::SetMono(void)
	\brief Alert the decompression codec to create a single audio channel

	\sa SetStereo() or IsStereo() const

***************************************/

/*! ************************************

	\fn void Burger::DecompressAudio::SetStereo(void)
	\brief Alert the decompression codec to create stereo channels

	\sa SetMono() or IsStereo() const

***************************************/

/*! ************************************

	\fn uint_t Burger::DecompressAudio::IsStereo(void)
	\brief Obtain the stereo/mono state of the decompresser
	
	\return \ref TRUE if stereo data is being output
	\sa SetMono() or SetStereo()

***************************************/





/*! ************************************

	\class Burger::DecompressUnsigned8BitAudio
	\brief Process unsigned 8 bit audio data

	Convert unsigned 8 bit audio data to native
	format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressUnsigned8BitAudio::DecompressUnsigned8BitAudio() :
	DecompressAudio(SoundManager::TYPEBYTE)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::DecompressUnsigned8BitAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using unsigned bytes

	Input data is assumed to be unsigned bytes. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::DecompressUnsigned8BitAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
    // Which is smaller? Input or output?
    uintptr_t uCount = minimum(uInputChunkLength, uOutputChunkLength);

	// Copy the data
	MemoryCopy(pOutput,pInput,uCount);

	// Store the amount of data that was processed

	m_uInputLength = uCount;
	m_uOutputLength = uCount;

	// Add the decompressed data to the totals
	m_uTotalInput += uCount;
	m_uTotalOutput += uCount;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uCount) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength!=uCount) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}

/*! ************************************

	\brief Allocate and initialize a DecompressUnsigned8BitAudio

	\return A pointer to a default DecompressUnsigned8BitAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressUnsigned8BitAudio * BURGER_API Burger::DecompressUnsigned8BitAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressUnsigned8BitAudio))) DecompressUnsigned8BitAudio();
}







/*! ************************************

	\class Burger::DecompressSigned8BitAudio
	\brief Process signed 8 bit audio data

	Convert signed 8 bit audio data to native
	format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressSigned8BitAudio::DecompressSigned8BitAudio() :
	DecompressAudio(SoundManager::TYPEBYTE)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::DecompressSigned8BitAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using signed bytes

	Input data is assumed to be signed bytes. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::DecompressSigned8BitAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
	// Which is smaller? Input or output?
	uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);
	swap_chars_to_bytes(static_cast<uint8_t *>(pOutput),static_cast<const uint8_t *>(pInput),uCount);

	// Store the amount of data that was processed

	m_uInputLength = uCount;
	m_uOutputLength = uCount;

	// Add the decompressed data to the totals
	m_uTotalInput += uCount;
	m_uTotalOutput += uCount;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uCount) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength!=uCount) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}

/*! ************************************

	\brief Allocate and initialize a DecompressSigned8BitAudio

	\return A pointer to a default DecompressSigned8BitAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressSigned8BitAudio * BURGER_API Burger::DecompressSigned8BitAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressSigned8BitAudio))) DecompressSigned8BitAudio();
}




/*! ************************************

	\class Burger::Decompress16BitBEAudio
	\brief Process 16 bit audio data in Big Endian format

	Convert 16 bit big endian audio data to native
	format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::Decompress16BitBEAudio::Decompress16BitBEAudio() :
	DecompressAudio(SoundManager::TYPESHORT)
#if !defined(BURGER_BIGENDIAN)
	,m_uCacheSize(0),
	m_eState(STATE_INIT)
#endif
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::Decompress16BitBEAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
#if !defined(BURGER_BIGENDIAN)
	m_uCacheSize = 0;
	m_eState = STATE_INIT;
#endif
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using big endian 16 bit words

	Input data is assumed to be big endian 16 bit words. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::Decompress16BitBEAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
#if defined(BURGER_BIGENDIAN)

	// Which is smaller? Input or output?
	uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);

	//
	// If there is no conversion, just upload the data as is.
	//

	MemoryCopy(pOutput,pInput,uCount);
	uintptr_t uInputConsumed = uCount;
	uintptr_t uOutputConsumed = uCount;
#else

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
	uint_t bAbort = FALSE;
	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{
				// Copy the data while converting the endian
				uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);
				uInputChunkLength -= uCount;
				uOutputChunkLength -= uCount&(~static_cast<uintptr_t>(1));

				uintptr_t uLength = uCount>>1;
				if (uLength) {
					// Is it aligned?
					if (!((reinterpret_cast<uintptr_t>(pInput)|reinterpret_cast<uintptr_t>(pOutput))&1)) {
						swap_endian(static_cast<uint16_t *>(pOutput),static_cast<const uint16_t *>(pInput),uLength);
						pInput = static_cast<const uint16_t *>(pInput)+uLength;
						pOutput = static_cast<uint16_t *>(pOutput)+uLength;
					} else {

						// You monster.

						do {
							// Convert to endian with unaligned data
							static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[1];
							static_cast<uint8_t *>(pOutput)[1] = static_cast<const uint8_t *>(pInput)[0];
							pInput = static_cast<const uint16_t *>(pInput)+1;
							pOutput = static_cast<uint16_t *>(pOutput)+1;
						} while (--uLength);

					}
				}

				//
				// One extra byte?
				//
				if (uCount&1) {
					// Put it in the cache and go into cache mode
					m_Cache[1] = static_cast<const uint8_t *>(pInput)[0];
					pInput = static_cast<const uint8_t *>(pInput)+1;
					uState = STATE_FILLINGCACHE;
				}
			}
			bAbort = TRUE;
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			if (uOutputChunkLength) {

				// Output 1 or 2 bytes

				uintptr_t uCacheSize = m_uCacheSize;
				uintptr_t uSteps = minimum(uOutputChunkLength,static_cast<uintptr_t>(uCacheSize));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				const uint8_t *pSrc = &m_Cache[sizeof(m_Cache)-uCacheSize];

				// Update the cache size
				uCacheSize = static_cast<uint_t>(uCacheSize-uSteps);
				if (uCacheSize) {
					// Number of bytes remaining in cache
					m_uCacheSize = static_cast<uint_t>(uCacheSize);
				} else {
					// Cache will be empty, so switch to normal mode
					uState = STATE_INIT;
				}

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pSrc[0];
					++pSrc;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);
			} else {
				bAbort = TRUE;
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			if (uInputChunkLength) {
				// Fill in the missing byte (Endian swapped)
				m_Cache[0] = static_cast<const uint8_t *>(pInput)[0];

				// Consume the input byte

				pInput = static_cast<const uint8_t *>(pInput)+1;
				--uInputChunkLength;

				// Change the state to empty the cache
				uState = STATE_CACHEFULL;
				m_uCacheSize = 2;
			} else {
				bAbort = TRUE;
			}
			break;
		}
	} while (!bAbort);

	// Save the state
	m_eState = uState;

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));
#endif

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

	\brief Allocate and initialize a Decompress16BitBEAudio

	\return A pointer to a default Decompress16BitBEAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::Decompress16BitBEAudio * BURGER_API Burger::Decompress16BitBEAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(Decompress16BitBEAudio))) Decompress16BitBEAudio();
}





/*! ************************************

	\class Burger::Decompress16BitLEAudio
	\brief Process 16 bit audio data in Little Endian format

	Convert 16 bit little endian audio data to native
	format

	\sa Decompress16BitBEAudio, \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::Decompress16BitLEAudio::Decompress16BitLEAudio() :
	DecompressAudio(SoundManager::TYPESHORT)
#if defined(BURGER_BIGENDIAN)
	,m_uCacheSize(0),
	m_eState(STATE_INIT)
#endif
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::Decompress16BitLEAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
#if defined(BURGER_BIGENDIAN)
	m_uCacheSize = 0;
	m_eState = STATE_INIT;
#endif
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using little endian 16 bit words

	Input data is assumed to be little endian 16 bit words. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::Decompress16BitLEAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
#if !defined(BURGER_BIGENDIAN)

	// Which is smaller? Input or output?
	uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);

	//
	// If there is no conversion, just upload the data as is.
	//

	MemoryCopy(pOutput,pInput,uCount);
	uintptr_t uInputConsumed = uCount;
	uintptr_t uOutputConsumed = uCount;
#else

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
	uint_t bAbort = FALSE;
	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{
				// Copy the data while converting the endian
				uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);
				uInputChunkLength -= uCount;
				uOutputChunkLength -= uCount&(~static_cast<uintptr_t>(1));

				uintptr_t uLength = uCount>>1;
				if (uLength) {
					// Is it aligned?
					if (!((reinterpret_cast<uintptr_t>(pInput)|reinterpret_cast<uintptr_t>(pOutput))&1)) {
						// Convert to native endian quickly
						swap_endian(static_cast<uint16_t *>(pOutput),static_cast<const uint16_t *>(pInput),uLength);
						pInput = static_cast<const uint16_t *>(pInput)+uLength;
						pOutput = static_cast<uint16_t *>(pOutput)+uLength;

					} else {

						// You monster.

						do {
							// Convert to endian with unaligned data
							static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[1];
							static_cast<uint8_t *>(pOutput)[1] = static_cast<const uint8_t *>(pInput)[0];
							pInput = static_cast<const uint16_t *>(pInput)+1;
							pOutput = static_cast<uint16_t *>(pOutput)+1;
						} while (--uLength);

					}
				}

				//
				// One extra byte?
				//
				if (uCount&1) {
					// Put it in the cache and go into cache mode
					m_Cache[1] = static_cast<const uint8_t *>(pInput)[0];
					pInput = static_cast<const uint8_t *>(pInput)+1;
					uState = STATE_FILLINGCACHE;
				}
			}
			bAbort = TRUE;
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			if (uOutputChunkLength) {

				// Output 1 or 2 bytes

				uintptr_t uCacheSize = m_uCacheSize;
				uintptr_t uSteps = minimum(uOutputChunkLength,static_cast<uintptr_t>(uCacheSize));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				const uint8_t *pSrc = &m_Cache[sizeof(m_Cache)-uCacheSize];

				// Update the cache size
				uCacheSize = static_cast<uint_t>(uCacheSize-uSteps);
				if (uCacheSize) {
					// Number of bytes remaining in cache
					m_uCacheSize = static_cast<uint_t>(uCacheSize);
				} else {
					// Cache will be empty, so switch to normal mode
					uState = STATE_INIT;
				}

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pSrc[0];
					++pSrc;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);
			} else {
				bAbort = TRUE;
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			if (uInputChunkLength) {
				// Fill in the missing byte (Endian swapped)
				m_Cache[0] = static_cast<const uint8_t *>(pInput)[0];

				// Consume the input byte

				pInput = static_cast<const uint8_t *>(pInput)+1;
				--uInputChunkLength;

				// Change the state to empty the cache
				uState = STATE_CACHEFULL;
				m_uCacheSize = 2;
			} else {
				bAbort = TRUE;
			}
			break;
		}
	} while (!bAbort);

	// Save the state
	m_eState = uState;

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));
#endif

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

	\brief Allocate and initialize a Decompress16BitLEAudio

	\return A pointer to a default Decompress16BitLEAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::Decompress16BitLEAudio * BURGER_API Burger::Decompress16BitLEAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(Decompress16BitLEAudio))) Decompress16BitLEAudio();
}




/*! ************************************

	\class Burger::Decompress32BitBEAudio
	\brief Process 32 bit audio data in Big Endian format

	Convert 32 bit big endian audio data to native
	format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::Decompress32BitBEAudio::Decompress32BitBEAudio() :
	DecompressAudio(SoundManager::TYPEFLOAT)
#if !defined(BURGER_BIGENDIAN)
	,m_uCacheCount(0),
	m_eState(STATE_INIT)
#endif
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::Decompress32BitBEAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
#if !defined(BURGER_BIGENDIAN)
	m_uCacheCount = 0;
	m_eState = STATE_INIT;
#endif
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using big endian 32 bit words

	Input data is assumed to be big endian 32 bit words. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::Decompress32BitBEAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
#if defined(BURGER_BIGENDIAN)

	// Which is smaller? Input or output?
	uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);

	//
	// If there is no conversion, just upload the data as is.
	//

	MemoryCopy(pOutput,pInput,uCount);
	uintptr_t uInputConsumed = uCount;
	uintptr_t uOutputConsumed = uCount;
#else

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
	uint_t bAbort = FALSE;
	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{
				// Copy the data while converting the endian
				uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);
				uintptr_t uLength = uCount&(~(3));
				if (uLength) {

					uInputChunkLength -= uLength;
					uOutputChunkLength -= uLength;
					// Is it aligned?
					if (!((reinterpret_cast<uintptr_t>(pInput)|reinterpret_cast<uintptr_t>(pOutput))&3)) {
						swap_endian(static_cast<uint32_t *>(pOutput),static_cast<const uint32_t *>(pInput),uLength>>2U);
						pInput = static_cast<const uint32_t *>(pInput)+uLength;
						pOutput = static_cast<uint32_t *>(pOutput)+uLength;
					} else {

						// You monster.
						uLength = uLength>>2U;
						do {
							// Convert to endian with unaligned data
							static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[3];
							static_cast<uint8_t *>(pOutput)[1] = static_cast<const uint8_t *>(pInput)[2];
							static_cast<uint8_t *>(pOutput)[2] = static_cast<const uint8_t *>(pInput)[1];
							static_cast<uint8_t *>(pOutput)[3] = static_cast<const uint8_t *>(pInput)[0];
							pInput = static_cast<const uint32_t *>(pInput)+1;
							pOutput = static_cast<uint32_t *>(pOutput)+1;
						} while (--uLength);
					}
				}

				//
				// All done with the input?
				//

				if (!uInputChunkLength) {
					bAbort = TRUE;
				} else {
					m_uCacheCount = 0;
					uState = STATE_FILLINGCACHE;
				}
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			bAbort = TRUE;			// Assume data starved
			if (uInputChunkLength) {

				// Get the number of bytes already obtained
				uintptr_t uCacheSize = m_uCacheCount;

				// How many is needed to fill
				uintptr_t uRemaining = sizeof(m_Cache)-uCacheSize;

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
				if (uCacheSize==sizeof(m_Cache)) {
					// Perform the endian swap on the cache
					SwapEndian::fixup(static_cast<uint32_t *>(static_cast<void *>(m_Cache)));
					// Cache is full, send to processing
					bAbort = FALSE;
					uState = STATE_CACHEFULL;
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
				const uint8_t *pInputChunk = &m_Cache[sizeof(m_Cache)-uCacheCount];

				// Update the cache size
				uCacheCount = static_cast<uint_t>(uCacheCount-uSteps);

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pInputChunk[0];
					++pInputChunk;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);

				// Data still in the cache?
				if (uCacheCount) {
					// Update and exit
					m_uCacheCount = static_cast<uint_t>(uCacheCount);
				} else {
					// Cache is empty, so switch to the next state
					uState = STATE_INIT;
					bAbort = FALSE;
				}
			}
			break;

		}
	} while (!bAbort);

	// Save the state
	m_eState = uState;

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));
#endif

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

	\brief Allocate and initialize a Decompress32BitBEAudio

	\return A pointer to a default Decompress32BitBEAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::Decompress32BitBEAudio * BURGER_API Burger::Decompress32BitBEAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(Decompress32BitBEAudio))) Decompress32BitBEAudio();
}





/*! ************************************

	\class Burger::Decompress32BitLEAudio
	\brief Process 32 bit audio data in Little Endian format

	Convert 32 bit little endian audio data to native
	format

	\sa \ref Decompress and \ref DecompressAudio

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::Decompress32BitLEAudio::Decompress32BitLEAudio() :
	DecompressAudio(SoundManager::TYPEFLOAT)
#if defined(BURGER_BIGENDIAN)
	,m_uCacheCount(0),
	m_eState(STATE_INIT)
#endif
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Resets the decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::Decompress32BitLEAudio::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	// No worries!
#if defined(BURGER_BIGENDIAN)
	m_uCacheCount = 0;
	m_eState = STATE_INIT;
#endif
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using little endian 32 bit words

	Input data is assumed to be little endian 32 bit words. There is no
	decompression being performed.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to decompress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

Burger::eError Burger::Decompress32BitLEAudio::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
#if !defined(BURGER_BIGENDIAN)

	// Which is smaller? Input or output?
	uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);

	//
	// If there is no conversion, just upload the data as is.
	//

	MemoryCopy(pOutput,pInput,uCount);
	uintptr_t uInputConsumed = uCount;
	uintptr_t uOutputConsumed = uCount;
#else

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
	uint_t bAbort = FALSE;
	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{
				// Copy the data while converting the endian
				uintptr_t uCount = minimum(uInputChunkLength,uOutputChunkLength);
				uintptr_t uLength = uCount&(~(3));
				if (uLength) {

					uInputChunkLength -= uLength;
					uOutputChunkLength -= uLength;
					// Is it aligned?
					if (!((reinterpret_cast<uintptr_t>(pInput)|reinterpret_cast<uintptr_t>(pOutput))&3)) {
						swap_endian(static_cast<uint32_t *>(pOutput),static_cast<const uint32_t *>(pInput),uLength>>2U);
						pInput = static_cast<const uint32_t *>(pInput)+uLength;
						pOutput = static_cast<uint32_t *>(pOutput)+uLength;
					} else {

						// You monster.
						uLength = uLength>>2U;
						do {
							// Convert to endian with unaligned data
							static_cast<uint8_t *>(pOutput)[0] = static_cast<const uint8_t *>(pInput)[3];
							static_cast<uint8_t *>(pOutput)[1] = static_cast<const uint8_t *>(pInput)[2];
							static_cast<uint8_t *>(pOutput)[2] = static_cast<const uint8_t *>(pInput)[1];
							static_cast<uint8_t *>(pOutput)[3] = static_cast<const uint8_t *>(pInput)[0];
							pInput = static_cast<const uint32_t *>(pInput)+1;
							pOutput = static_cast<uint32_t *>(pOutput)+1;
						} while (--uLength);
					}
				}

				//
				// All done with the input?
				//

				if (!uInputChunkLength) {
					bAbort = TRUE;
				} else {
					m_uCacheCount = 0;
					uState = STATE_FILLINGCACHE;
				}
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			bAbort = TRUE;			// Assume data starved
			if (uInputChunkLength) {

				// Get the number of bytes already obtained
				uintptr_t uCacheSize = m_uCacheCount;

				// How many is needed to fill
				uintptr_t uRemaining = sizeof(m_Cache)-uCacheSize;

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
				if (uCacheSize==sizeof(m_Cache)) {
					// Perform the endian swap on the cache
					SwapEndian::fixup(static_cast<uint32_t *>(static_cast<void *>(m_Cache)));
					// Cache is full, send to processing
					bAbort = FALSE;
					uState = STATE_CACHEFULL;
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
				const uint8_t *pInputChunk = &m_Cache[sizeof(m_Cache)-uCacheCount];

				// Update the cache size
				uCacheCount = static_cast<uint_t>(uCacheCount-uSteps);

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pInputChunk[0];
					++pInputChunk;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);

				// Data still in the cache?
				if (uCacheCount) {
					// Update and exit
					m_uCacheCount = static_cast<uint_t>(uCacheCount);
				} else {
					// Cache is empty, so switch to the next state
					uState = STATE_INIT;
					bAbort = FALSE;
				}
			}
			break;

		}
	} while (!bAbort);

	// Save the state
	m_eState = uState;

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));
#endif

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

	\brief Allocate and initialize a Decompress32BitLEAudio

	\return A pointer to a default Decompress32BitLEAudio class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::Decompress32BitLEAudio * BURGER_API Burger::Decompress32BitLEAudio::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(Decompress32BitLEAudio))) Decompress32BitLEAudio();
}
