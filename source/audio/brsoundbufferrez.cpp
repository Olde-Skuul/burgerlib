/***************************************

    Sound manager buffer from resource

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brsoundbufferrez.h"

#if !defined(DOXYGEN)

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

BURGER_CREATE_STATICRTTI_PARENT(Burger::SoundBufferRez,Burger::SoundManager::Buffer);
#endif

/*! ************************************

	\class Burger::SoundBufferRez
	\brief Convenience class for sound files in resources

	When this class is created, only it's variables are initialized
	but data isn't actually loaded, to upload the sound to
	the audio hardware, call SoundManager::Play() or
	call Upload() directly.

	This is a reference counted class, as such it can
	only be created with a call to New() and disposed
	of by a call to Release(). Delete() will likely
	asset with a "refcount is not zero" error.

***************************************/

/*! ************************************

	\brief Default constructor

	This creates an incomplete class, follow up
	with a call to Set(RezFile *,uint_t) to 
	have the class access a valid sound file

	\sa SoundBufferRez(RezFile *,uint_t) or Set(RezFile *,uint_t)

***************************************/

Burger::SoundBufferRez::SoundBufferRez(void) :
	Buffer(),
	m_pRezFile(NULL),
	m_uRezNum(0)
{
}

/*! ************************************

	\brief Default constructor

	Sets up the RezFile and resource number of the
	sound file this buffer derives the audio data from.

	\param pRezFile Pointer to a RezFile to load data from
	\param uRezNum Resource number of the sound file
	\sa SoundBufferRez() or Set(RezFile *,uint_t)

***************************************/

Burger::SoundBufferRez::SoundBufferRez(Burger::RezFile *pRezFile,uint_t uRezNum) :
	Buffer(),
	m_pRezFile(pRezFile),
	m_uRezNum(uRezNum)
{
}

/*! ************************************

	\brief Load resource and upload

	If the sound hardware already has the data uploaded,
	this function will do nothing. Otherwise, it will
	call the RezFile class to load in the sound data and
	it will be passed to the audio hardware. After
	the upload is complete, the RezFile data chunk
	is marked "released" so it can be purged.

	\param pSoundManager A pointer to the parent SoundManager that manages the audio hardware
	\return Zero if successful (Or already loaded) or non-zero if an error occurred.

***************************************/

uint_t Burger::SoundBufferRez::Upload(SoundManager *pSoundManager)
{
	// Assume success
	uint_t uResult = 0;
	// Don't upload again if not needed
	if (!IsUploaded()) {

		// Change to failure assumption
		uResult = 10;
		// Load if there's a valid resource
		RezFile *pRezFile = m_pRezFile;
		uint_t uRezNum = m_uRezNum;
		if (pRezFile && uRezNum) {

			// Load in the data
			void *pData = pRezFile->Load(uRezNum);
			if (pData) {
				// Get the data's size
				uintptr_t uLength = pRezFile->GetSize(uRezNum);

				// Initialize my description with file image
				uResult = Init(pData,uLength);
				if (!uResult) {
					// Perform the upload with the platform specific code
					uResult = Buffer::Upload(pSoundManager);
				}
				// Regardless of success or failure, release the resource
				pRezFile->Release(uRezNum);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Set the resource to track

	Set the resource number to upload into the sound buffer.

	It will purge the previous buffer. 

	\note Do not call this function if this buffer is currently being used
	for sound playback. Behavior is undefined if an audio buffer is manually
	purged while still in use by audio hardware.

	\param pRezFile Pointer to a RezFile to load data from
	\param uRezNum Resource number of the sound file
	
***************************************/

void Burger::SoundBufferRez::Set(RezFile *pRezFile,uint_t uRezNum)
{
	// Purge anything if present

	Shutdown();

	// Set the new resource to track
	m_pRezFile = pRezFile;
	m_uRezNum = uRezNum;
}

/*! ************************************

	\brief Create a new sound buffer

	\return Pointer to a SoundBufferRez class with a refcount of 1 or \ref NULL if out of memory.

***************************************/

Burger::SoundBufferRez * BURGER_API Burger::SoundBufferRez::New(void)
{
	SoundBufferRez *pBuffer = new (Alloc(sizeof(SoundBufferRez))) SoundBufferRez();
	if (pBuffer) {
		pBuffer->AddRef();
	}
	return pBuffer;
}

/*! ************************************

	\brief Create a new sound buffer

	\param pRezFile Pointer to a RezFile to load data from
	\param uRezNum Resource number of the sound file
	\return Pointer to a SoundBufferRez class with a refcount of 1 or \ref NULL if out of memory.

***************************************/

Burger::SoundBufferRez * BURGER_API Burger::SoundBufferRez::New(RezFile *pRezFile,uint_t uRezNum)
{
	SoundBufferRez *pBuffer = new (Alloc(sizeof(SoundBufferRez))) SoundBufferRez(pRezFile,uRezNum);
	if (pBuffer) {
		pBuffer->AddRef();
	}
	return pBuffer;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::SoundBufferRez::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
