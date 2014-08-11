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

#if defined(BURGER_WINDOWS)

#include "brglobals.h"
#include "brrunqueue.h"
#include "brendian.h"

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <MMReg.h>
#include <dsound.h>

/*! ************************************

	\var Burger::Sound::g_DirectSoundVolumes

	Table used for Direct sound volume
	conversion from 0-255 to Decibels

***************************************/

const Int16 BURGER_ALIGN(Burger::Sound::g_DirectSoundVolumes[256],16) = {
	-10000, -8000, -7000, -6415, -6000, -5678, -5415, -5192,
	 -5000, -4830, -4678, -4540, -4415, -4299, -4192, -4093,
	 -4000, -3912, -3830, -3752, -3678, -3607, -3540, -3476,
	 -3415, -3356, -3299, -3245, -3192, -3142, -3093, -3045,
	 -3000, -2955, -2912, -2870, -2830, -2790, -2752, -2714,
	 -2678, -2642, -2607, -2573, -2540, -2508, -2476, -2445,
	 -2415, -2385, -2356, -2327, -2299, -2272, -2245, -2218,
	 -2192, -2167, -2142, -2117, -2093, -2069, -2045, -2022,
	 -2000, -1977, -1955, -1933, -1912, -1891, -1870, -1850,
	 -1830, -1810, -1790, -1771, -1752, -1733, -1714, -1696,
	 -1678, -1660, -1642, -1624, -1607, -1590, -1573, -1557,
	 -1540, -1524, -1508, -1492, -1476, -1460, -1445, -1430,
	 -1415, -1400, -1385, -1370, -1356, -1341, -1327, -1313,
	 -1299, -1285, -1272, -1258, -1245, -1231, -1218, -1205,
	 -1192, -1179, -1167, -1154, -1142, -1129, -1117, -1105,
	 -1093, -1081, -1069, -1057, -1045, -1034, -1022, -1011,
	 -1000,  -988,  -977,  -966,  -955,  -944,  -933,  -923,
	  -912,  -901,  -891,  -881,  -870,  -860,  -850,  -840,
	  -830,  -820,  -810,  -800,  -790,  -780,  -771,  -761,
	  -752,  -742,  -733,  -723,  -714,  -705,  -696,  -687,
	  -678,  -669,  -660,  -651,  -642,  -633,  -624,  -616,
	  -607,  -599,  -590,  -582,  -573,  -565,  -557,  -548,
	  -540,  -532,  -524,  -516,  -508,  -500,  -492,  -484,
	  -476,  -468,  -460,  -453,  -445,  -437,  -430,  -422,
	  -415,  -407,  -400,  -392,  -385,  -377,  -370,  -363,
	  -356,  -348,  -341,  -334,  -327,  -320,  -313,  -306,
	  -299,  -292,  -285,  -278,  -272,  -265,  -258,  -251,
	  -245,  -238,  -231,  -225,  -218,  -212,  -205,  -199,
	  -192,  -186,  -179,  -173,  -167,  -160,  -154,  -148,
	  -142,  -135,  -129,  -123,  -117,  -111,  -105,   -99,
	   -93,   -87,   -81,   -75,   -69,   -63,   -57,   -51,
	   -45,   -39,   -34,   -28,   -22,   -17,   -11,     0
};

/***************************************

	Upload sound data and convert if needed

***************************************/

static void CopySoundData(Word8 *pOutput,const Word8 *pInput,WordPtr uLength,Burger::Sound::DataType eType)
{
	if (uLength) {
		switch (eType) {
		case Burger::Sound::TYPEBYTE:
		case Burger::Sound::TYPELSHORT:
			Burger::MemoryCopy(pOutput,pInput,uLength);
			break;
		case Burger::Sound::TYPECHAR:
			do {
				pOutput[0] = pInput[0]^0x80U;
				++pInput;
				++pOutput;
			} while (--uLength);
			break;
		case Burger::Sound::TYPEBSHORT:
			uLength>>=1U;
			if (uLength) {
				do {
					reinterpret_cast<Word16 *>(pOutput)[0] = Burger::SwapEndian::Load(reinterpret_cast<const Word16 *>(pInput));
					pInput+=2;
					pOutput+=2;
				} while (--uLength);
			}
		}
	}
}

/***************************************

	Init

***************************************/

Burger::Sound::Sound(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_pDirectSound8Device(NULL),
	m_pDirectSound3DListener(NULL),
	m_hCallback(NULL),
	m_uCallbackID(0),
	m_uDeviceFlags(0),
	m_uBufferAllocationType(DSBCAPS_STATIC),
	m_uActiveVoices(0),
	m_uMaxVoices(MAXVOICECOUNT),
	m_uVolume(255),
	m_bStereoAvailable(FALSE),
	m_bBufferDepth(16),
	m_uOutputSamplesPerSecond(22050)
{
	MemoryClear(m_hEvents,sizeof(m_hEvents));
}

/***************************************

	Init the sound manager

***************************************/

Word Burger::Sound::Init(void)
{

	// First, create the background thread needed to handle async audio
	// processing

	// Assume error
	m_hCallback = CreateThread(NULL,0,ThreadCallback,this,0,&m_uCallbackID);
	const char *pError ="Could not create background thread";
	HRESULT uResult = 10;
	if (m_hCallback) {
		SetThreadPriority(m_hCallback,THREAD_PRIORITY_HIGHEST);

		// Create the events
		Word i = 0;
		do {
			m_hEvents[i] = CreateEventW(NULL,FALSE,FALSE,NULL);
			if (!m_hEvents[i]) {
				break;
			}
		} while (++i<MAXVOICECOUNT);

		pError ="Could not create events";
		if (i==MAXVOICECOUNT) {
			// Open the sound device by creating an DirectSound object

			uResult = Globals::DirectSoundCreate8(NULL,&m_pDirectSound8Device);
			pError = "Direct sound could not be started";
			if (uResult==DS_OK) {

				// In case I don't have a game window, I'll take the frontmost active window
				// If one doesn't exist, then you are so screwed!!!

				HWND pWindow = Globals::GetWindow();
				if (!pWindow) {
					pWindow = GetActiveWindow();
				}

				pError = "Direct sound could not set the priority";
				// Set lowest level of cooperation
				if (((uResult = m_pDirectSound8Device->SetCooperativeLevel(pWindow,DSSCL_PRIORITY))==DS_OK) ||
					((uResult = m_pDirectSound8Device->SetCooperativeLevel(pWindow,DSSCL_NORMAL))==DS_OK)) {

					// Obtain the caps for the sound device, and nab some of the settings from it

					DSCAPS Caps;
					MemoryClear(&Caps,sizeof(Caps));
					Caps.dwSize = sizeof(Caps);
					uResult = m_pDirectSound8Device->GetCaps(&Caps);
					pError = "Could not get the device CAPS data";
					if (uResult==DS_OK) {

						// Get the flags to determine stereo and 16 bit compatibility

						Word uFlags = Caps.dwFlags;
						m_uDeviceFlags = uFlags;
						m_bStereoAvailable = (uFlags & DSCAPS_PRIMARYSTEREO)!=0;
						m_bBufferDepth = (uFlags & DSCAPS_PRIMARY16BIT) ? 16 : 8;

						// 22050 is the base and widely supported sample rate
						Word uSampleRate = 22050;

						// If 44100 is supported, use that rate instead.
						if (uFlags & DSCAPS_CONTINUOUSRATE) {
							if ((Caps.dwMaxSecondarySampleRate>=44100) &&
								(Caps.dwMinSecondarySampleRate<=44100)) {
									uSampleRate = 44100;
							}
						}
						m_uOutputSamplesPerSecond = uSampleRate;

						// Note: If the audio driver reports that it has hardware mixing buffers
						// but no streaming buffer, it's likely to be an old ISA card with on-board
						// memory. Do NOT use the LOCDEFER flag on these cards!!! It is really slow
						// so use the STATIC flag instead.
						// If the card reports that it has streaming buffers, it's likely
						// a high speed PCI card. :)

						if (Caps.dwMaxHwMixingStaticBuffers > 0 &&
							Caps.dwMaxHwMixingStreamingBuffers == 0) {
							m_uBufferAllocationType = DSBCAPS_STATIC;
						} else {
							m_uBufferAllocationType = DSBCAPS_LOCDEFER;
						}

						// Create a primary buffer for audio playback (First try one that volume can be changed)
						DSBUFFERDESC BufferStats;
						MemoryClear(&BufferStats,sizeof(BufferStats));
						BufferStats.dwSize = sizeof(BufferStats);
						BufferStats.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
//						BufferStats.dwBufferBytes = 0;			// Must be zero for a primary buffer
//						BufferStats.dwReserved = 0;
//						BufferStats.lpwfxFormat = NULL;
//						MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));

						// Allocate the composite buffer
						IDirectSoundBuffer *pDirectSoundBuffer = NULL;
						uResult = m_pDirectSound8Device->CreateSoundBuffer(&BufferStats,&pDirectSoundBuffer,NULL);
						if (uResult!=DS_OK) {

							// Let's try again but no sound volume control

							MemoryClear(&BufferStats,sizeof(BufferStats));
							BufferStats.dwSize = sizeof(BufferStats);
							BufferStats.dwFlags = DSBCAPS_PRIMARYBUFFER;
//							BufferStats.dwBufferBytes = 0;
//							BufferStats.dwReserved = 0;
//							BufferStats.lpwfxFormat = NULL;
//							MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));
							uResult = m_pDirectSound8Device->CreateSoundBuffer(&BufferStats,&pDirectSoundBuffer,NULL);
						}

						// Was a primary sound buffer allocated?
						pError = "Could not create a primary sound buffer";
						if (uResult==DS_OK) {

							// Get current format settings
							WAVEFORMATEX BufferFormat;
							DWORD bytesWritten;
							MemoryClear(&BufferFormat,sizeof(BufferFormat));
							pDirectSoundBuffer->GetFormat(&BufferFormat,sizeof(BufferFormat),&bytesWritten);
							pError = "Could not call GetFormat() on the primary sound buffer";
							if (uResult==DS_OK) {

								// Set the type of final output required

								BufferFormat.wFormatTag = WAVE_FORMAT_PCM;
								// Set for stereo if supported
								BufferFormat.nChannels = m_bStereoAvailable ? 2 : 1;
								BufferFormat.nSamplesPerSec = m_uOutputSamplesPerSecond;
								BufferFormat.wBitsPerSample = static_cast<Word16>(m_bBufferDepth);
								BufferFormat.nBlockAlign = static_cast<Word16>((m_bBufferDepth >> 3U) * BufferFormat.nChannels);
								BufferFormat.nAvgBytesPerSec = BufferFormat.nSamplesPerSec * BufferFormat.nBlockAlign;

								// Set the new format, but don't die if was unable to take

								pDirectSoundBuffer->SetFormat(&BufferFormat);

								// Get the 3D audio listener and set it to a default

								uResult = pDirectSoundBuffer->QueryInterface(IID_IDirectSound3DListener,reinterpret_cast<void **>(&m_pDirectSound3DListener));
								pError = "Could not call QueryInterface() a DirectSound3DListener on the primary sound buffer";
								if (uResult == DS_OK) {
									m_pDirectSound3DListener->SetRolloffFactor(1.0f,DS3D_DEFERRED);
									// Change listener's orientation to 0,0,1 front and 0,1,0 top
									m_pDirectSound3DListener->SetOrientation(0.0f,0.0f,1.0f,0.0f,1.0f,0.0f,DS3D_DEFERRED);
									// Commit the changes to Rolloff Factor and orientation
									m_pDirectSound3DListener->CommitDeferredSettings();

									pDirectSoundBuffer->Release();

									return 0;
								}
							}
							pDirectSoundBuffer->Release();
						}
					}
				}
			}
		}
	}
	Shutdown();
	// Report the error condition
	Debug::Message("%s\n",pError);
	char OhGreat[256];
	StringCopy(OhGreat,pError);
	StringConcatenate(OhGreat,", sound is disabled");
	OkAlertMessage(OhGreat,"Direct sound error");
	Globals::SetErrorCode(uResult);
	return uResult;
}

/***************************************

	Shut down the main sound manager

***************************************/

void Burger::Sound::Shutdown(void)
{
	// Shut down the worker thread
	if (m_hCallback) {
		PostThreadMessage(m_uCallbackID, WM_QUIT, 0, 0);
		WaitForSingleObject(m_hCallback, INFINITE);
		CloseHandle(m_hCallback);
		m_hCallback = NULL;
		m_uCallbackID = 0;
	}

	// Release all the pending thread events
	Word i = MAXVOICECOUNT;
	void **pEvents = m_hEvents;
	do {
		if (pEvents[0]) {
			CloseHandle(pEvents[0]);
			pEvents[0] = NULL;
		}
		++pEvents;
	} while (--i);

	// Shut down the 3d mixer
	if (m_pDirectSound3DListener) {
		m_pDirectSound3DListener->Release();
		m_pDirectSound3DListener = NULL;
	}
	// Shut down DirectSound
	if (m_pDirectSound8Device) {
		m_pDirectSound8Device->Release();
		m_pDirectSound8Device = NULL;
	}
}

Word Burger::Sound::Play(Buffer *pInput,Voice *pOutput)
{
	IDirectSoundBuffer *m_pDirectSoundBuffer8;
	// Create a primary buffer for audio playback (First try one that volume can be changed)
	DSBUFFERDESC BufferStats;
	MemoryClear(&BufferStats,sizeof(BufferStats));
	BufferStats.dwSize = sizeof(BufferStats);
	BufferStats.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
//	BufferStats.dwBufferBytes = 0;			// Must be zero for a primary buffer
//	BufferStats.dwReserved = 0;
//	BufferStats.lpwfxFormat = NULL;
//	MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));
	m_pDirectSound8Device->CreateSoundBuffer(&BufferStats,&m_pDirectSoundBuffer8,NULL);
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

//
// This callback thread monitors all of the sound channels
// for data completion. If a voice completes, it will
// allow it to automatically self-destruct
//

unsigned long __stdcall Burger::Sound::ThreadCallback(void *pThis)
{
	MSG WindowsMessage;
	Sound *pSound = static_cast<Sound*>(pThis);
	Word bQuit = FALSE;
	do {
		DWORD result = MsgWaitForMultipleObjects(MAXVOICECOUNT,pSound->m_hEvents,FALSE,INFINITE,QS_ALLEVENTS);
		if (result == (WAIT_OBJECT_0+MAXVOICECOUNT)) {
			while (PeekMessage(&WindowsMessage,NULL,0,0,PM_REMOVE)) {
				if (WindowsMessage.message == WM_QUIT) {
					bQuit = TRUE;
					break;
				}
			}
		} else {
			if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0+MAXVOICECOUNT)) {
			}
		}
	} while (!bQuit);
	return 0;
}


Burger::Sound::Buffer::Buffer() :
	m_pDirectSoundBuffer8(NULL),
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
	if (m_pDirectSoundBuffer8) {
		m_pDirectSoundBuffer8->Stop();
		m_pDirectSoundBuffer8->Release();
		m_pDirectSoundBuffer8 = NULL;
	}
}

Burger::Sound::Voice::Voice() :
	m_pDirectSoundBuffer8(NULL)
{
}

Burger::Sound::Voice::~Voice()
{
	Shutdown();
}

void Burger::Sound::Voice::Shutdown(void)
{
	if (m_pDirectSoundBuffer8) {
		m_pDirectSoundBuffer8->Stop();
		m_pDirectSoundBuffer8->Release();
		m_pDirectSoundBuffer8 = NULL;
	}
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
		if (m_pDirectSoundBuffer8) {
			m_pDirectSoundBuffer8->SetVolume(uVolume);
		}
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

		if (m_pDirectSoundBuffer8) {
			int WinPan;
			if (uPan<0x8000) {
				WinPan = g_DirectSoundVolumes[uPan>>7];
			} else {
				WinPan = -g_DirectSoundVolumes[(0xFFFF-uPan)>>7];
			}
			m_pDirectSoundBuffer8->SetPan(WinPan);
		}
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

		if (m_pDirectSoundBuffer8) {
			m_pDirectSoundBuffer8->SetFrequency(uSamplesPerSecond);
		}
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
					//message("Index is : %d", idx);
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
