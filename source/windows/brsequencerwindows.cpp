/***************************************

	Music Sequencer

	Windows version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brsequencer.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(DIRECTSOUND_VERSION)
#define DIRECTSOUND_VERSION 0x800
#endif

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

Word BURGER_API Burger::Sequencer::PlatformInit(void)
{
	m_hCallback = CreateThread(NULL,0,ThreadCallback,this,0,&m_uCallbackID);
	HRESULT uResult = 10;
	if (m_hCallback) {
		// Make sure this is the highest priority to keep the sound going
		SetThreadPriority(m_hCallback,THREAD_PRIORITY_HIGHEST);

		// Create the events for the 32 voices to track
		Word i = 0;
		do {
			m_hEvents[i] = CreateEventW(NULL,FALSE,FALSE,NULL);
			if (!m_hEvents[i]) {
				break;
			}
		} while (++i<cMaxEventCount);

		m_pDirectSoundBuffer8 = 0;
		WAVEFORMATEX SampleRecord;

		// Set up wave format structure.
		MemoryClear(&SampleRecord,sizeof(PCMWAVEFORMAT));
		SampleRecord.wFormatTag = WAVE_FORMAT_PCM;
		SampleRecord.nChannels = 2;
		SampleRecord.nSamplesPerSec = m_uFinalSampleRate;
		switch (m_eOutputDataType) {
		case SoundManager::TYPECHAR:
		case SoundManager::TYPEBYTE:
			SampleRecord.wBitsPerSample = 8;
			break;
		default:
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPEBSHORT:
			SampleRecord.wBitsPerSample = 16;
		}
		SampleRecord.nBlockAlign = static_cast<WORD>(SampleRecord.nChannels * (SampleRecord.wBitsPerSample/8));
		SampleRecord.nAvgBytesPerSec = SampleRecord.nSamplesPerSec * SampleRecord.nBlockAlign;

		// Set up DSBUFFERDESC structure.
		DSBUFFERDESC dsbdesc;
		MemoryClear(&dsbdesc,sizeof(DSBUFFERDESC));	// Zero it out.
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);

		dsbdesc.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_STICKYFOCUS;
		dsbdesc.dwBufferBytes = static_cast<DWORD>(m_uBufferSize*2);
		dsbdesc.dwReserved = 0;
		dsbdesc.lpwfxFormat = &SampleRecord;
		IDirectSoundBuffer *pDirectSoundBuffer = NULL;
		uResult = m_pSoundManager->GetDirectSound8()->CreateSoundBuffer(&dsbdesc,&pDirectSoundBuffer,NULL);
		if (uResult == DS_OK) {
			// Get DirectSound8
			uResult = pDirectSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,reinterpret_cast<void **>(&m_pDirectSoundBuffer8));
			// Release the old pointer (Not needed anymore)
			pDirectSoundBuffer->Release();

			if (m_pDirectSoundBuffer8) {
				// Attach a Notify event
				uResult = m_pDirectSoundBuffer8->QueryInterface(IID_IDirectSoundNotify,reinterpret_cast<void **>(&m_pDirectSoundNotify));
				if (uResult==DS_OK) {
					DSBPOSITIONNOTIFY Notify[cMaxEventCount];
					Notify[0].dwOffset = static_cast<DWORD>(m_uBufferSize);
					Notify[0].hEventNotify = m_hEvents[0];
					Notify[1].dwOffset = static_cast<DWORD>(m_uBufferSize/2);
					Notify[1].hEventNotify = m_hEvents[1];
					uResult = m_pDirectSoundNotify->SetNotificationPositions(cMaxEventCount,Notify);
				}
				m_pDirectSoundBuffer8->Play(0,0,DSBPLAY_LOOPING);
				return 0;		/* I am good to go! */
			}
		} else {
			m_pDirectSoundBuffer8 = NULL;
		}
	}
	// Return error if any
	return static_cast<Word>(uResult);
}

/***************************************

	Shut down audio hardware

***************************************/

void BURGER_API Burger::Sequencer::PlatformShutdown(void)
{
	if (m_pDirectSoundBuffer8) {			/* Was a DirectSound buffer allocated? */
		m_pDirectSoundBuffer8->Stop();		/* Stop audio output */
		m_pDirectSoundBuffer8->Release();	/* Kill the buffer */
		m_pDirectSoundBuffer8 = NULL;		/* Gone! */
	}

	// Was there a notification attached?
	IDirectSoundNotify *pDirectSoundNotify = m_pDirectSoundNotify;
	if (pDirectSoundNotify) {
		pDirectSoundNotify->Release();
		m_pDirectSoundNotify = NULL;
	}

	// Shut down the worker thread
	if (m_hCallback) {
		// Send a quit command to nicely ask it to shut down
		PostThreadMessageW(m_uCallbackID,WM_QUIT,0,0);
		WaitForSingleObject(m_hCallback,INFINITE);
		// Dispose of the thread's handle
		CloseHandle(m_hCallback);
		m_hCallback = NULL;
		m_uCallbackID = 0;
	}

	// Release all the pending thread events
	Word i = cMaxEventCount;
	void **pEvents = m_hEvents;
	do {
		if (pEvents[0]) {
			CloseHandle(pEvents[0]);
			pEvents[0] = NULL;
		}
		++pEvents;
	} while (--i);
}


//
// This callback thread monitors all of the sound channels
// for data completion. If a voice completes, it will
// allow it to automatically self-destruct
//

unsigned long __stdcall Burger::Sequencer::ThreadCallback(void *pThis)
{
	MSG WindowsMessage;
	Sequencer *pSequencer = static_cast<Sequencer*>(pThis);
	Word bQuit = FALSE;
	do {
		DWORD uResult = MsgWaitForMultipleObjects(cMaxEventCount,pSequencer->m_hEvents,FALSE,INFINITE,QS_ALLEVENTS);
		if (uResult == (WAIT_OBJECT_0+cMaxEventCount)) {
			while (PeekMessageW(&WindowsMessage,NULL,0,0,PM_REMOVE)) {
				if (WindowsMessage.message == WM_QUIT) {
					bQuit = TRUE;
					break;
				}
			}
		} else {
			if (/* uResult >= WAIT_OBJECT_0 && */ uResult < (WAIT_OBJECT_0+cMaxEventCount)) {
				// Is music playing now?

				Word bSilence = TRUE;
				if (pSequencer->m_bSequencingInProgress) {
					pSequencer->PerformSequencing();
					if ((!pSequencer->m_pSongPackage) ||
						(pSequencer->m_uPartitionPosition < pSequencer->m_pSongPackage->m_SongDescription.m_uPointerCount)) {
						bSilence = FALSE;
					}
				}
				if (bSilence) {
					ClearSoundData(pSequencer->m_pBuffer,pSequencer->m_uBufferSize,pSequencer->m_eOutputDataType);
				}
				Upload(pSequencer->m_pDirectSoundBuffer8,uResult*(pSequencer->m_uBufferSize),static_cast<const Word8*>(pSequencer->m_pBuffer),pSequencer->m_uBufferSize);
			}
		}
	} while (!bQuit);
	return 0;
}

#endif
