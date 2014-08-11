/***************************************

	Sound manager class

	Windows version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brsound.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

#include "brglobals.h"
#include "brrunqueue.h"
#include "brendian.h"

/***************************************

	Init

***************************************/

Burger::Sound::Sound(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_uActiveVoices(0),
	m_uMaxVoices(MAXVOICECOUNT),
	m_uVolume(255),
	m_bStereoAvailable(FALSE),
	m_bBufferDepth(16),
	m_uOutputSamplesPerSecond(22050)
{
}

/***************************************

	Init the sound manager

***************************************/

Word Burger::Sound::Init(void)
{
	Word uResult = 0;
	return uResult;
}

/***************************************

	Shut down the main sound manager

***************************************/

void Burger::Sound::Shutdown(void)
{
}

Word Burger::Sound::Play(Buffer *pInput,Voice *pOutput)
{
	return 10;
}

Word Burger::Sound::Stop(void)
{
	return 10;
}

Word Burger::Sound::Pause(void)
{
	return 10;
}

Word Burger::Sound::Resume(void)
{
	return 10;
}

void Burger::Sound::SetMaxVoices(Word uMax)
{
}

void Burger::Sound::SetVolume(Word uVolume)
{
}

Burger::Sound::Buffer::Buffer() :
	m_uDefaultSampleRate(22050),
	m_uDefaultPan(0x8000),
	m_uDefaultVolume(255),
	m_bPlaying(FALSE)
{
}

Burger::Sound::Buffer::~Buffer()
{
	Shutdown();
}

void Burger::Sound::Buffer::Shutdown(void)
{
}

Burger::Sound::Voice::Voice()
{
}

Burger::Sound::Voice::~Voice()
{
	Shutdown();
}

void Burger::Sound::Voice::Shutdown(void)
{
}

//
// Set the sound volume
//

Word Burger::Sound::Voice::SetVolume(Word uVolume)
{
	if (m_uVolume!=uVolume) {
		m_uVolume = uVolume;			// Get the pan position value
//		uVolume *= MasterSoundVolume;
//		uVolume /= 255;		/* Set the new scaled volume */
	}
	return FALSE;
}

//
// Set the sound pan value
//

Word Burger::Sound::Voice::SetPan(Word uPan)
{
	if (m_uPan!=uPan) {
		m_uPan = uPan;			// Get the pan position value
	}
	return FALSE;
}

//
// Set the sound's sample rate
//

Word Burger::Sound::Voice::SetSampleRate(Word uSamplesPerSecond)
{
	if (m_uSampleRate!=uSamplesPerSecond) {
		m_uSampleRate = uSamplesPerSecond;			// Get the pan position value
	}
	return FALSE;
}


Burger::SoundBufferRez::SoundBufferRez(void) :
	Burger::Sound::Buffer(),
	m_pRezFile(NULL),
	m_ppHandle(NULL),
	m_uRezNum(0)
{
}

Burger::SoundBufferRez::SoundBufferRez(Burger::RezFile *pRezFile,Word uRezNum) :
	Burger::Sound::Buffer(),
	m_pRezFile(pRezFile),
	m_ppHandle(NULL),
	m_uRezNum(uRezNum)
{
}

Burger::SoundBufferRez::~SoundBufferRez()
{
	Shutdown();
}

void Burger::SoundBufferRez::Shutdown(void)
{
}

void Burger::SoundBufferRez::Set(RezFile *pRezFile,Word uRezNum)
{
	m_pRezFile = pRezFile;
	m_uRezNum = uRezNum;
}

void Burger::SoundBufferRez::Set(Word uRezNum)
{
	m_uRezNum = uRezNum;
}

#if 0


// notify thread
DWORD WINAPI ds8_notification_proc(LPVOID param)
{
	MSG		msg;
	DWORD		result;
	int		offset = 0;
	ds8_device_t	*snd_dev = (ds8_device_t*)param;
	while (1) {
		result = MsgWaitForMultipleObjects(MAX_STREAMS, &ds8_notification_event[offset], FALSE, INFINITE, QS_ALLEVENTS);
		if (result == (WAIT_OBJECT_0+MAX_STREAMS)) {
			while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					goto end_thread;
				}
			}
		}
		else {
			//message("result= %d", result);
			if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0+MAX_STREAMS)) {
				if (snd_dev) {
					snd_element_t *snd;
					int idx = (result - WAIT_OBJECT_0);
					if (offset > 0) {
						idx += offset;
						idx %= MAX_STREAMS;
					}
					//message("Index is :              %d", idx);
					snd = ds8_streams[idx];
					if (snd) {
						EnterCriticalSection(&stream_section);
						snd_dev->update_stream_buffer(snd);
						LeaveCriticalSection(&stream_section);
					}
					offset = (result - WAIT_OBJECT_0) + 1;
					offset %= MAX_STREAMS;
					//message("Offset is : %d", offset);
				}
			}
		}
	}
end_thread:
	return 666;
}

#endif

#endif
