/***************************************

	Sound manager class

	Windows version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsound.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brendian.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brrunqueue.h"
#include "brsounddecompress.h"
#include "win_dsound.h"
#include "win_winutils.h"

#if !defined(DOXYGEN)

// This crap is needed to get XAudio2.h to include with watcom
#if defined(BURGER_WATCOM)
#include <comdecl.h>
#undef DEFINE_CLSID
#undef DEFINE_IID
#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, \
		0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, \
		0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define KSDATAFORMAT_SUBTYPE_PCM
#define KSDATAFORMAT_SUBTYPE_ADPCM
#define KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
#define CLSID_IXAudio2 IID_IXAudio2
#define __uuidof(x) CLSID_##x
#endif
#endif

// Include in THIS ORDER!

#include "win_windows.h"

#include <xaudio2.h>

#include <dsound.h>

/*! ************************************

	\var Burger::SoundManager::g_DirectSoundVolumes

	Table used for quick Direct sound volume
	conversion from 0-255 to Decibels

	\sa ConvertToDirectSoundVolume(uint_t) of ConvertToDirectSoundVolume(float)

***************************************/

BURGER_ALIGN(const int16_t, Burger::SoundManager::g_DirectSoundVolumes[256],
	16) = {-10000, -8000, -7000, -6415, -6000, -5678, -5415, -5192, -5000,
	-4830, -4678, -4540, -4415, -4299, -4192, -4093, -4000, -3912, -3830, -3752,
	-3678, -3607, -3540, -3476, -3415, -3356, -3299, -3245, -3192, -3142, -3093,
	-3045, -3000, -2955, -2912, -2870, -2830, -2790, -2752, -2714, -2678, -2642,
	-2607, -2573, -2540, -2508, -2476, -2445, -2415, -2385, -2356, -2327, -2299,
	-2272, -2245, -2218, -2192, -2167, -2142, -2117, -2093, -2069, -2045, -2022,
	-2000, -1977, -1955, -1933, -1912, -1891, -1870, -1850, -1830, -1810, -1790,
	-1771, -1752, -1733, -1714, -1696, -1678, -1660, -1642, -1624, -1607, -1590,
	-1573, -1557, -1540, -1524, -1508, -1492, -1476, -1460, -1445, -1430, -1415,
	-1400, -1385, -1370, -1356, -1341, -1327, -1313, -1299, -1285, -1272, -1258,
	-1245, -1231, -1218, -1205, -1192, -1179, -1167, -1154, -1142, -1129, -1117,
	-1105, -1093, -1081, -1069, -1057, -1045, -1034, -1022, -1011, -1000, -988,
	-977, -966, -955, -944, -933, -923, -912, -901, -891, -881, -870, -860,
	-850, -840, -830, -820, -810, -800, -790, -780, -771, -761, -752, -742,
	-733, -723, -714, -705, -696, -687, -678, -669, -660, -651, -642, -633,
	-624, -616, -607, -599, -590, -582, -573, -565, -557, -548, -540, -532,
	-524, -516, -508, -500, -492, -484, -476, -468, -460, -453, -445, -437,
	-430, -422, -415, -407, -400, -392, -385, -377, -370, -363, -356, -348,
	-341, -334, -327, -320, -313, -306, -299, -292, -285, -278, -272, -265,
	-258, -251, -245, -238, -231, -225, -218, -212, -205, -199, -192, -186,
	-179, -173, -167, -160, -154, -148, -142, -135, -129, -123, -117, -111,
	-105, -99, -93, -87, -81, -75, -69, -63, -57, -51, -45, -39, -34, -28, -22,
	-17, -11, 0};

//
// Init the buffer
//

Burger::SoundManager::Buffer::Buffer():
	m_pDirectSoundBuffer8(NULL), m_uPan(cPanCenter), m_uVolume(cMaxVolume)
{
}

//
// Release the direct sound data
//

void Burger::SoundManager::Buffer::Shutdown(void)
{
	IDirectSoundBuffer8* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
	if (pDirectSoundBuffer8) {
		pDirectSoundBuffer8->Stop();
		pDirectSoundBuffer8->Release();
		m_pDirectSoundBuffer8 = NULL;
	}
}

//
// Upload to DirectSound
//

uint_t Burger::SoundManager::Buffer::Upload(SoundManager* pSoundManager)
{
	uint_t uResult = 0;
	// Not already uploaded?
	if (!m_pDirectSoundBuffer8) {
		// Create a direct sound buffer for the sound

		WAVEFORMATEX SampleRecord;
		SampleRecord.wFormatTag = WAVE_FORMAT_PCM;
		if (m_Decoder.m_eDataType & TYPESTEREO) {
			SampleRecord.nChannels = 2;
		} else {
			SampleRecord.nChannels = 1;
		}
		SampleRecord.nSamplesPerSec = m_Decoder.m_uSampleRate;
		DecompressAudio* pDecompresser = m_Decoder.GetDecompresser();
		switch (pDecompresser->GetDataType()) {
		default:
			// case TYPEBYTE:
			// case TYPECHAR:
			SampleRecord.wBitsPerSample = 8;
			break;
		case TYPELSHORT:
		case TYPEBSHORT:
			SampleRecord.wBitsPerSample = 16;
			break;
		case TYPELFLOAT:
		case TYPEBFLOAT:
			SampleRecord.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			SampleRecord.wBitsPerSample = 32;
			break;
		}
		SampleRecord.cbSize = 0;

		SampleRecord.nBlockAlign = static_cast<uint16_t>(
			(SampleRecord.wBitsPerSample / 8) * SampleRecord.nChannels);
		SampleRecord.nAvgBytesPerSec =
			SampleRecord.nSamplesPerSec * SampleRecord.nBlockAlign;

		DSBUFFERDESC BufferStats;
		MemoryClear(&BufferStats, sizeof(BufferStats));
		BufferStats.dwSize = sizeof(BufferStats);
		BufferStats.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 |
			DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_STICKYFOCUS;
		BufferStats.dwBufferBytes =
			static_cast<DWORD>(m_Decoder.m_uSoundLength);
		//	BufferStats.dwReserved = 0;
		BufferStats.lpwfxFormat = &SampleRecord;
		//	MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));

		IDirectSoundBuffer* pIDirectSoundBuffer = NULL;
		uResult = static_cast<uint_t>(
			pSoundManager->m_pDirectSound8Device->CreateSoundBuffer(
				&BufferStats, &pIDirectSoundBuffer, NULL));
		if (uResult == DS_OK) {
			// Get the DirectSoundBuffer8 pointer
			uResult = static_cast<uint_t>(
				pIDirectSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,
					reinterpret_cast<void**>(&m_pDirectSoundBuffer8)));
			// Release the old pointer
			pIDirectSoundBuffer->Release();
			if (uResult == DS_OK) {
				// We're good!! Put some data in it!
				pDecompresser->Reset();
				uResult = Burger::Upload(m_pDirectSoundBuffer8, 0, &m_Decoder,
					m_Decoder.m_uSoundLength);
			}
		}
	}
	return uResult;
}

//
// Initialize the voice
//

Burger::SoundManager::Voice::Voice():
	m_pDirectSoundBuffer8(),
	m_pDirectSoundNotify(NULL),
	m_pBuffer(NULL),
	m_pCallBack(NULL),
	m_pCallBackData(NULL),
	m_uLoopStart(0),
	m_uLoopEnd(0),
	m_uSampleRate(BURGER_MAXUINT),
	m_uPan(BURGER_MAXUINT),
	m_uVolume(BURGER_MAXUINT),
	m_bPlaying(FALSE),
	m_bAvailable(TRUE)
{
}

uint_t BURGER_API Burger::SoundManager::Voice::Init(
	SoundManager* pManager, Buffer* pBuffer)
{
	// Attach to the buffer (Obtain a reference via a smart pointer)
	m_pBuffer = pBuffer;

	// Grab the default settings from the buffer's description
	const BufferDecoder* pDecoder = pBuffer->GetBufferDescription();

	// Looping?
	m_uLoopStart = pDecoder->m_uLoopStart;
	m_uLoopEnd = pDecoder->m_uLoopEnd;
	m_uPausedMark = 0;

	// Starting settings for playback

	m_uSampleRate = pDecoder->m_uSampleRate;
	m_uPan = pBuffer->GetPan();
	m_uVolume = pBuffer->GetVolume();

	// Not playing yet
	m_bPlaying = FALSE;
	m_bPaused = FALSE;
	m_bAvailable = FALSE;
	m_bIsHeld = FALSE;

	// Assume failure
	uint_t uResult = 10;

	// Is there a buffer?
	IDirectSoundBuffer8* pDirectSoundBuffer8 = pBuffer->GetDirectSoundBuffer8();
	if (pDirectSoundBuffer8) {

		// Get a reference to the direct sound buffer
		// so I can have multi-playback from a single buffer
		IDirectSound8* pDevice = pManager->m_pDirectSound8Device;
		IDirectSoundBuffer* pDirectSoundBuffer;

		uResult = static_cast<uint_t>(pDevice->DuplicateSoundBuffer(
			pDirectSoundBuffer8, &pDirectSoundBuffer));
		if (uResult == DS_OK) {
			// Get DirectSound8
			uResult = static_cast<uint_t>(
				pDirectSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,
					reinterpret_cast<void**>(&m_pDirectSoundBuffer8)));
			// Release the old pointer (Not needed anymore)
			pDirectSoundBuffer->Release();

			if (uResult == DS_OK) {
				pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
				// Attach a Notify event
				uResult = static_cast<uint_t>(
					pDirectSoundBuffer8->QueryInterface(IID_IDirectSoundNotify,
						reinterpret_cast<void**>(&m_pDirectSoundNotify)));
				if (uResult == DS_OK) {
					DSBPOSITIONNOTIFY Notify;
					Notify.dwOffset = DSBPN_OFFSETSTOP;
					Notify.hEventNotify =
						pManager->m_hEvents[this - pManager->m_ActiveVoices];
					uResult = static_cast<uint_t>(
						m_pDirectSoundNotify->SetNotificationPositions(
							1, &Notify));

					// Set the speed and other settings
					pDirectSoundBuffer8->SetVolume(
						ConvertToDirectSoundVolume(m_uVolume));
					pDirectSoundBuffer8->SetPan(
						ConvertToDirectSoundPan(m_uPan));
					pDirectSoundBuffer8->SetFrequency(m_uSampleRate);
				}
			}
		}
	}

	// If failed, release everything
	if (uResult != DS_OK) {
		Shutdown();
	}
	return uResult;
}

//
// Release the sound effect
//

void BURGER_API Burger::SoundManager::Voice::Shutdown(void)
{
	// Was there a notification attached?
	IDirectSoundNotify* pDirectSoundNotify = m_pDirectSoundNotify;
	if (pDirectSoundNotify) {
		pDirectSoundNotify->Release();
		m_pDirectSoundNotify = NULL;
	}

	// Is there a sound buffer?
	IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
	if (pDirectSoundBuffer8) {
		pDirectSoundBuffer8->Stop();
		pDirectSoundBuffer8->Release();
		m_pDirectSoundBuffer8 = NULL;
	}

	// The smart pointer will release the buffer if
	// this class is the sole owner
	m_pBuffer = NULL;

	// Not playing anymore
	m_bPlaying = FALSE;
	m_bPaused = FALSE;
	m_bAvailable = TRUE;
}

//
// Release this voice to the free pool
//

void BURGER_API Burger::SoundManager::Voice::Release(void)
{
	// Looping? Stopped? Paused?
	if (m_uLoopEnd || !m_bPlaying || m_bPaused) {
		// Kill the sound
		Shutdown();
	}
	// Allow this voice to auto shutdown
	m_bIsHeld = FALSE;
}

uint_t BURGER_API Burger::SoundManager::Voice::Start(void)
{
	uint_t uResult = 0;
	if (!m_bPlaying) {
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {
			// Make sure it's at the beginning
			uResult =
				static_cast<uint_t>(pDirectSoundBuffer8->SetCurrentPosition(0));
			if (uResult == DS_OK) {
				// Not looping
				DWORD uFlags = 0;
				if (m_uLoopEnd) {
					uFlags = DSBPLAY_LOOPING;
				}
				// Play
				uResult = static_cast<uint_t>(
					pDirectSoundBuffer8->Play(0, 0, uFlags));
				if (uResult == DS_OK) {
					m_bPlaying = TRUE;
					m_bPaused = FALSE;
				}
			}
		}
	}
	return uResult;
}

//
// Stop playback
//

uint_t BURGER_API Burger::SoundManager::Voice::Stop(void)
{
	uint_t uResult = 0;
	if (m_bPlaying || m_bPaused) {
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {
			uResult = static_cast<uint_t>(pDirectSoundBuffer8->Stop());
			if (uResult == DS_OK) {
				m_bPlaying = FALSE;
				m_bPaused = FALSE;
				m_uPausedMark = 0;
			}
		}
	}
	return uResult;
}

//
// Pause playback
//

uint_t BURGER_API Burger::SoundManager::Voice::Pause(void)
{
	uint_t uResult = 0;
	if (m_bPlaying && !m_bPaused) {
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {

			// Get the location the buffer is at
			DWORD uPlayCursor;
			DWORD uWriteCursor;
			uResult =
				static_cast<uint_t>(pDirectSoundBuffer8->GetCurrentPosition(
					&uPlayCursor, &uWriteCursor));
			if (uResult == DS_OK) {
				uResult = static_cast<uint_t>(pDirectSoundBuffer8->Stop());
				if (uResult == DS_OK) {
					// Save the playback location
					m_uPausedMark = uPlayCursor;
					m_bPlaying = FALSE;
					m_bPaused = TRUE;
				}
			}
		}
	}
	return uResult;
}

//
// Resume from pause
//

uint_t BURGER_API Burger::SoundManager::Voice::Resume(void)
{
	uint_t uResult = 0;
	if (!m_bPlaying && m_bPaused) {
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {

			// Resume where we left off
			uResult =
				static_cast<uint_t>(pDirectSoundBuffer8->SetCurrentPosition(
					static_cast<DWORD>(m_uPausedMark)));
			if (uResult == DS_OK) {

				// Not looping
				DWORD uFlags = 0;
				if (m_uLoopEnd) {
					uFlags = DSBPLAY_LOOPING;
				}
				// Play
				uResult = static_cast<uint_t>(
					pDirectSoundBuffer8->Play(0, 0, uFlags));
				if (uResult == DS_OK) {
					m_bPlaying = TRUE;
					m_bPaused = FALSE;
				}
			}
		}
	}
	return uResult;
}

//
// Set the sound volume
//

uint_t BURGER_API Burger::SoundManager::Voice::SetVolume(uint_t uVolume)
{
	uint_t uResult = 0;
	if (m_uVolume != uVolume) {
		m_uVolume = uVolume; // Get the new volume
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {
			uResult = static_cast<uint_t>(pDirectSoundBuffer8->SetVolume(
				ConvertToDirectSoundVolume(uVolume)));
		}
	}
	return uResult;
}

//
// Set the sound pan value
//

uint_t BURGER_API Burger::SoundManager::Voice::SetPan(uint_t uPan)
{
	uint_t uResult = 0;
	if (m_uPan != uPan) {
		m_uPan = uPan; // Get the pan position value
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {
			uResult = static_cast<uint_t>(
				pDirectSoundBuffer8->SetPan(ConvertToDirectSoundPan(m_uPan)));
		}
	}
	return uResult;
}

//
// Set the sound's sample rate
//

uint_t BURGER_API Burger::SoundManager::Voice::SetSampleRate(
	uint_t uSamplesPerSecond)
{
	uint_t uResult = 0;
	if (m_uSampleRate != uSamplesPerSecond) {
		m_uSampleRate = uSamplesPerSecond; // Get the pan position value
		IDirectSoundBuffer* pDirectSoundBuffer8 = m_pDirectSoundBuffer8;
		if (pDirectSoundBuffer8) {
			uResult = static_cast<uint_t>(
				pDirectSoundBuffer8->SetFrequency(uSamplesPerSecond));
		}
	}
	return uResult;
}

void BURGER_API Burger::SoundManager::Voice::ReachedEnd(void)
{
	m_bPlaying = FALSE;
	if (!m_bIsHeld) {
		Shutdown();
	}
}

/***************************************

	Initialize the sound manager

***************************************/

Burger::SoundManager::SoundManager(GameApp* pGameApp):
	m_pGameApp(pGameApp),
	m_pIXAudio2(NULL),
	m_pIXAudio2MasteringVoice(NULL),
	m_pDirectSound8Device(NULL),
	m_pDirectSound3DListener(NULL),
	m_pDirectSoundBuffer(NULL),
	m_hCallback(NULL),
	m_uCallbackID(0),
	m_uBufferAllocationType(DSBCAPS_STATIC),
	m_uMaxVoices(cMaxVoiceCount),
	m_uVolume(cMaxVolume),
	m_bStereoAvailable(FALSE),
	m_bMasterVolumeAvailable(FALSE),
	m_uBufferDepth(16),
	m_uOutputSamplesPerSecond(22050)
{
	MemoryClear(m_hEvents, sizeof(m_hEvents));
}

/***************************************

	Init the sound manager

***************************************/

uint_t BURGER_API Burger::SoundManager::Init(void)
{

	// First, create the background thread needed to handle asynchronous audio
	// processing

	// Assume error
	m_hCallback =
		CreateThread(NULL, 0, ThreadCallback, this, 0, &m_uCallbackID);
	const char* pError = "Could not create background thread";
	HRESULT uResult = 10;
	if (m_hCallback) {

		// Make sure this is the highest priority to keep the sound going
		SetThreadPriority(m_hCallback, THREAD_PRIORITY_HIGHEST);

		// Create the events for the 32 voices to track
		uint_t i = 0;
		do {
			m_hEvents[i] = CreateEventW(NULL, FALSE, FALSE, NULL);
			if (!m_hEvents[i]) {
				break;
			}
		} while (++i < cMaxVoiceCount);

		pError = "Could not create events";
		if (i == cMaxVoiceCount) {
			// Open the sound device by creating an DirectSound object

			uResult = static_cast<HRESULT>(
				Win32::DirectSoundCreate8(NULL, &m_pDirectSound8Device));
			pError = "Direct sound could not be started";
			if (uResult == DS_OK) {

				// In case I don't have a game window, I'll take the frontmost
				// active window If one doesn't exist, then you are so
				// screwed!!!

				HWND pWindow = Win32::get_window();
				if (!pWindow) {
					pWindow = GetActiveWindow();
				}

				pError = "Direct sound could not set the priority";
				// Set lowest level of cooperation
				if (((uResult = m_pDirectSound8Device->SetCooperativeLevel(
						  pWindow, DSSCL_PRIORITY)) == DS_OK) ||
					((uResult = m_pDirectSound8Device->SetCooperativeLevel(
						  pWindow, DSSCL_NORMAL)) == DS_OK)) {

					// Obtain the caps for the sound device, and nab some of the
					// settings from it

					DSCAPS Caps;
					MemoryClear(&Caps, sizeof(Caps));
					Caps.dwSize = sizeof(Caps);
					uResult = m_pDirectSound8Device->GetCaps(&Caps);
					pError = "Could not get the device CAPS data";
					if (uResult == DS_OK) {

						// Get the flags to determine stereo and 16 bit
						// compatibility

						uint_t uFlags = Caps.dwFlags;
						m_bStereoAvailable = static_cast<uint_t>(
							(uFlags & DSCAPS_PRIMARYSTEREO) != 0);
						m_uBufferDepth = static_cast<uint_t>(
							(uFlags & DSCAPS_PRIMARY16BIT) ? 16 : 8);

						// 22050 is the base and widely supported sample rate
						uint_t uSampleRate = 22050;

						// If 44100 is supported, use that rate instead.
						if (uFlags & DSCAPS_CONTINUOUSRATE) {
							if ((Caps.dwMaxSecondarySampleRate >= 44100) &&
								(Caps.dwMinSecondarySampleRate <= 44100)) {
								uSampleRate = 44100;
							}
						}
						m_uOutputSamplesPerSecond = uSampleRate;

						// Note: If the audio driver reports that it has
						// hardware mixing buffers but no streaming buffer, it's
						// likely to be an old ISA card with on-board memory. Do
						// NOT use the LOCDEFER flag on these cards!!! It is
						// really slow so use the STATIC flag instead. If the
						// card reports that it has streaming buffers, it's
						// likely a high speed PCI card. :)

						if (Caps.dwMaxHwMixingStaticBuffers > 0 &&
							Caps.dwMaxHwMixingStreamingBuffers == 0) {
							m_uBufferAllocationType = DSBCAPS_STATIC;
						} else {
							m_uBufferAllocationType = DSBCAPS_LOCDEFER;
						}

						// Create a primary buffer for audio playback (First try
						// one that volume can be changed)

						// Note, I ask for this buffer, even though the CAPS may
						// not say it's available for a simple reason. Buggy
						// drivers!! This function will fail if it REALLY didn't
						// support the mode

						m_bMasterVolumeAvailable =
							TRUE; // Assume volume can be controlled
						DSBUFFERDESC BufferStats;
						MemoryClear(&BufferStats, sizeof(BufferStats));
						BufferStats.dwSize = sizeof(BufferStats);
						BufferStats.dwFlags = DSBCAPS_PRIMARYBUFFER |
							DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D |
							DSBCAPS_STICKYFOCUS;
						//						BufferStats.dwBufferBytes = 0;
						//// Must be zero for a primary buffer
						//						BufferStats.dwReserved = 0;
						//						BufferStats.lpwfxFormat = NULL;
						//						MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));

						// Allocate the composite buffer
						m_pDirectSoundBuffer = NULL;
						uResult = m_pDirectSound8Device->CreateSoundBuffer(
							&BufferStats, &m_pDirectSoundBuffer, NULL);
						if (uResult != DS_OK) {

							// Let's try again but no sound volume control

							m_bMasterVolumeAvailable =
								FALSE; // There is no master volume support
							MemoryClear(&BufferStats, sizeof(BufferStats));
							BufferStats.dwSize = sizeof(BufferStats);
							BufferStats.dwFlags = DSBCAPS_PRIMARYBUFFER;
							//							BufferStats.dwBufferBytes
							//= 0; BufferStats.dwReserved = 0;
							//							BufferStats.lpwfxFormat
							//= NULL;
							//							MemoryCopy(BufferStats.guid3DAlgorithm,Algorithm,sizeof(GUID));
							uResult = m_pDirectSound8Device->CreateSoundBuffer(
								&BufferStats, &m_pDirectSoundBuffer, NULL);
						}
						IDirectSoundBuffer* pDirectSoundBuffer =
							m_pDirectSoundBuffer;
						// Was a primary sound buffer allocated?
						pError = "Could not create a primary sound buffer";
						if (uResult == DS_OK) {

							// Get current format settings
							WAVEFORMATEX BufferFormat;
							DWORD bytesWritten;
							MemoryClear(&BufferFormat, sizeof(BufferFormat));
							pDirectSoundBuffer->GetFormat(&BufferFormat,
								sizeof(BufferFormat), &bytesWritten);
							pError =
								"Could not call GetFormat() on the primary sound buffer";
							if (uResult == DS_OK) {

								// Set the type of final output required

								// PCM is the ONLY supported format
								BufferFormat.wFormatTag = WAVE_FORMAT_PCM;

								// Set for stereo if supported
								BufferFormat.nChannels = static_cast<uint16_t>(
									m_bStereoAvailable ? 2 : 1);
								BufferFormat.nSamplesPerSec =
									m_uOutputSamplesPerSecond;
								BufferFormat.wBitsPerSample =
									static_cast<uint16_t>(m_uBufferDepth);
								BufferFormat.nBlockAlign =
									static_cast<uint16_t>(
										(m_uBufferDepth >> 3U) *
										BufferFormat.nChannels);
								BufferFormat.nAvgBytesPerSec =
									BufferFormat.nSamplesPerSec *
									BufferFormat.nBlockAlign;

								// Set the new format, but don't die if was
								// unable to take

								pDirectSoundBuffer->SetFormat(&BufferFormat);

								// Get the 3D audio listener and set it to a
								// default

								uResult = pDirectSoundBuffer->QueryInterface(
									IID_IDirectSound3DListener,
									reinterpret_cast<void**>(
										&m_pDirectSound3DListener));
								pError =
									"Could not call QueryInterface() a DirectSound3DListener on the primary sound buffer";
								if (uResult == DS_OK) {
									m_pDirectSound3DListener->SetRolloffFactor(
										1.0f, DS3D_DEFERRED);
									// Change listener's orientation to 0,0,1
									// front and 0,1,0 top
									m_pDirectSound3DListener->SetOrientation(
										0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
										DS3D_DEFERRED);
									// Commit the changes to Rolloff Factor and
									// orientation
									m_pDirectSound3DListener
										->CommitDeferredSettings();

									// Start up XAudio2
									uResult = Win32::XAudio2Create(&m_pIXAudio2, 0,
										static_cast<uint_t>(XAUDIO2_DEFAULT_PROCESSOR));
									if (uResult == S_OK) {
										uResult =
											m_pIXAudio2->CreateMasteringVoice(
												&m_pIXAudio2MasteringVoice);
										if (uResult == S_OK) {
											return 0;
										}
										m_pIXAudio2->Release();
										m_pIXAudio2 = nullptr;
									}
								}
							}
							pDirectSoundBuffer->Release();
							m_pDirectSoundBuffer = NULL;
						}
					}
				}
			}
		}
	}
	Shutdown();
	// Report the error condition
	Debug::Message("%s\n", pError);
	char OhGreat[256];
	StringCopy(OhGreat, pError);
	StringConcatenate(OhGreat, ", sound is disabled");
	OkAlertMessage(OhGreat, "Direct sound error");
	Globals::SetErrorCode(static_cast<Burger::eError>(uResult));
	return static_cast<uint_t>(uResult);
}

/***************************************

	Shut down the main sound manager

***************************************/

void BURGER_API Burger::SoundManager::Shutdown(void)
{
	Voice* pVoice = m_ActiveVoices;
	uint_t i = cMaxVoiceCount;
	do {
		pVoice->Shutdown();
		++pVoice;
	} while (--i);

	// Stop XAudio2
	if (m_pIXAudio2MasteringVoice) {
		m_pIXAudio2MasteringVoice->DestroyVoice();
		m_pIXAudio2MasteringVoice = NULL;
	}

	if (m_pIXAudio2) {
		m_pIXAudio2->Release();
		m_pIXAudio2 = NULL;
	}

	// Shut down the worker thread
	if (m_hCallback) {
		// Send a quit command to nicely ask it to shut down
		PostThreadMessageW(m_uCallbackID, WM_QUIT, 0, 0);
		WaitForSingleObject(m_hCallback, INFINITE);
		// Dispose of the thread's handle
		CloseHandle(m_hCallback);
		m_hCallback = NULL;
		m_uCallbackID = 0;
	}

	// Release all the pending thread events
	i = cMaxVoiceCount;
	void** pEvents = m_hEvents;
	do {
		if (pEvents[0]) {
			CloseHandle(pEvents[0]);
			pEvents[0] = NULL;
		}
		++pEvents;
	} while (--i);

	if (m_pDirectSoundBuffer) {
		m_pDirectSoundBuffer->Release();
		m_pDirectSoundBuffer = NULL;
	}
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

void BURGER_API Burger::SoundManager::SetVolume(uint_t uVolume)
{
	if (uVolume != m_uVolume) {
		m_uVolume = uVolume;
		if (m_pDirectSoundBuffer) {
			m_pDirectSoundBuffer->SetVolume(
				ConvertToDirectSoundVolume(uVolume));
		}
	}
}

//
// This callback thread monitors all of the sound channels
// for data completion. If a voice completes, it will
// allow it to automatically self-destruct
//

unsigned long __stdcall Burger::SoundManager::ThreadCallback(void* pThis)
{
	MSG WindowsMessage;
	SoundManager* pSound = static_cast<SoundManager*>(pThis);
	uint_t bQuit = FALSE;
	do {
		DWORD uResult = MsgWaitForMultipleObjects(
			cMaxVoiceCount, pSound->m_hEvents, FALSE, INFINITE, QS_ALLEVENTS);
		if (uResult == (WAIT_OBJECT_0 + cMaxVoiceCount)) {
			while (PeekMessageW(&WindowsMessage, NULL, 0, 0, PM_REMOVE)) {
				if (WindowsMessage.message == WM_QUIT) {
					bQuit = TRUE;
					break;
				}
			}
		} else {
			if (/* uResult >= WAIT_OBJECT_0 && */ uResult <
				(WAIT_OBJECT_0 + cMaxVoiceCount)) {
				Voice* pVoice = &pSound->m_ActiveVoices[uResult];
				pVoice->ReachedEnd();
			}
		}
	} while (!bQuit);
	return 0;
}

//
// Get the list of audio modes available
//

static BOOL CALLBACK EnumerateAudioDevices(GUID* pGUID,
	const uint16_t* pDescription, const uint16_t* /* pModule */, void* pInput)
{
	// Ignore NULL GUID (Primary)
	if (pGUID) {
		// Get the "this" pointer
		Burger::ClassArray<Burger::SoundManager::SoundCardDescription>*
			pOutput = static_cast<Burger::ClassArray<
				Burger::SoundManager::SoundCardDescription>*>(pInput);
		IDirectSound8* pDirectSound8;

		// Get the direct sound device for this specific GUID
		if (Burger::Win32::DirectSoundCreate8(pGUID, &pDirectSound8) == DS_OK) {

			// Entry to create
			Burger::SoundManager::SoundCardDescription Entry;

			// Get the audio card GUID
			Burger::MemoryCopy(&Entry.m_GUID, pGUID, sizeof(GUID));

			// Get the audio card name
			Entry.m_uDevNumber = static_cast<uint_t>(pOutput->size());
			Entry.m_DeviceName.assign(pDescription);

			// Is it hardware accelerated?
			DSCAPS HardwareCaps;
			Burger::MemoryClear(&HardwareCaps, sizeof(HardwareCaps));
			HardwareCaps.dwSize = sizeof(HardwareCaps);
			if (pDirectSound8->GetCaps(&HardwareCaps) == DS_OK) {
				// Get the sample rate values
				if (HardwareCaps.dwFlags & DSCAPS_CONTINUOUSRATE) {
					Entry.m_uMinimumSampleRate =
						HardwareCaps.dwMinSecondarySampleRate;
					Entry.m_uMaximumSampleRate =
						HardwareCaps.dwMaxSecondarySampleRate;
				}

				// Stereo allowed?
				if (HardwareCaps.dwFlags & DSCAPS_SECONDARYSTEREO) {
					Entry.m_bStereo = TRUE;
				}

				// 8 Bit audio allowed?
				if (HardwareCaps.dwFlags & DSCAPS_SECONDARY8BIT) {
					Entry.m_b8Bit = TRUE;
				}

				// 16 bit audio allowed?
				if (HardwareCaps.dwFlags & DSCAPS_SECONDARY16BIT) {
					Entry.m_b16Bit = TRUE;
				}

				// It's accelerated ONLY if there are more than 1
				// hardware buffers for mixing
				if (HardwareCaps.dwMaxHwMixingAllBuffers >= 2) {
					Entry.m_bHardwareAccelerated = TRUE;
				}
			}

			// Release the device obtained
			pDirectSound8->Release();
			// Save the device found
			pOutput->push_back(Entry);
		}
	}
	return TRUE;
}

/***************************************

	\brief Get a list of available audio modes

	\param pOutput Pointer to array of SoundCardDescription entries
	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::SoundManager::GetAudioModes(
	ClassArray<SoundCardDescription>* pOutput)
{
	// Erase the old list
	pOutput->clear();
	uint_t uResult = 10;
	// Enumerate all devices
	if (Win32::DirectSoundEnumerateW(EnumerateAudioDevices, pOutput) == DS_OK) {
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Upload sound data into a DirectSoundBuffer

	Given a buffer, lock the DirectSoundBuffer and upload the raw data
	into it. If the DirectSoundBuffer was lost, attempt to restore it
	and the upload the data.

	\note This function will handle buffer wraparound

	\windowsonly
	\param pBuffer Pointer to a DirectSoundBuffer
	\param uOffset Offset into the start of the buffer
	\param pInput Pointer to the data to upload
	\param uInputLength Number of bytes to upload
	\return Zero if successful, HRESULT from DirectSound for any error

***************************************/

uint_t BURGER_API Burger::Upload(IDirectSoundBuffer* pBuffer, uintptr_t uOffset,
	const uint8_t* pInput, uintptr_t uInputLength)
{
	HRESULT uResult = DSERR_INVALIDPARAM;

	// DirectSound is only 32 bits wide, so parameters that exceed this, nip it
	// in the bud

#if defined(BURGER_64BITCPU)
	if ((uOffset < 0x100000000ULL) && (uInputLength < 0x100000000ULL))
#endif
	{

		// Pointers set up by the lock command
		void* pBuffer1;
		DWORD uBufferSize1;
		void* pBuffer2;
		DWORD uBufferSize2;

		// Lock down the memory
		uResult = pBuffer->Lock(static_cast<DWORD>(uOffset),
			static_cast<DWORD>(uInputLength), &pBuffer1, &uBufferSize1,
			&pBuffer2, &uBufferSize2, 0);

		//
		// If the buffer was lost, try to restore it
		//
		if (uResult == DSERR_BUFFERLOST) {
			// Restore the sound card buffer
			pBuffer->Restore();
			// Try the lock again
			uResult = pBuffer->Lock(static_cast<DWORD>(uOffset),
				static_cast<DWORD>(uInputLength), &pBuffer1, &uBufferSize1,
				&pBuffer2, &uBufferSize2, 0);
		}

		// Lock was obtained, upload!
		if (uResult == DS_OK) {

			// Write the first chunk
			MemoryCopy(pBuffer1, pInput, uBufferSize1);
			// Was wrap around involved?
			if (pBuffer2) {
				// Upload the second chunk
				MemoryCopy(pBuffer2, pInput + uBufferSize1, uBufferSize2);
			}
			// Perform the unlock
			uResult =
				pBuffer->Unlock(pBuffer1, uBufferSize1, pBuffer2, uBufferSize2);
		}
	}
	// Return the result
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Upload compressed sound data into a DirectSoundBuffer

	Given a buffer, lock the DirectSoundBuffer, decompress data
	from the compressed stream and upload the newly decompressed
	data into the DirectSoundBuffer. If the DirectSoundBuffer
	was lost, attempt to restore it and the upload the data.

	\note This function will handle buffer wraparound

	\windowsonly
	\param pBuffer Pointer to a DirectSoundBuffer
	\param uOffset Offset into the start of the buffer
	\param pBufferDecoder Pointer to the compressed data stream to decode from
	\param uInputLength Number of bytes to upload
	\return Zero if successful, HRESULT from DirectSound for any error

***************************************/

uint_t BURGER_API Burger::Upload(IDirectSoundBuffer* pBuffer, uintptr_t uOffset,
	SoundManager::BufferDecoder* pBufferDecoder, uintptr_t uInputLength)
{
	HRESULT uResult = DSERR_INVALIDPARAM;

	// DirectSound is only 32 bits wide, so parameters that exceed this, nip it
	// in the bud

#if defined(BURGER_64BITCPU)
	if ((uOffset < 0x100000000ULL) && (uInputLength < 0x100000000ULL))
#endif
	{

		// Pointers set up by the lock command
		void* pBuffer1;
		DWORD uBufferSize1;
		void* pBuffer2;
		DWORD uBufferSize2;

		// Lock down the memory
		uResult = pBuffer->Lock(static_cast<DWORD>(uOffset),
			static_cast<DWORD>(uInputLength), &pBuffer1, &uBufferSize1,
			&pBuffer2, &uBufferSize2, 0);

		//
		// If the buffer was lost, try to restore it
		//
		if (uResult == DSERR_BUFFERLOST) {
			// Restore the sound card buffer
			pBuffer->Restore();
			// Try the lock again
			uResult = pBuffer->Lock(static_cast<DWORD>(uOffset),
				static_cast<DWORD>(uInputLength), &pBuffer1, &uBufferSize1,
				&pBuffer2, &uBufferSize2, 0);
		}

		// Lock was obtained, upload!
		if (uResult == DS_OK) {
			DecompressAudio* pDecompresser = pBufferDecoder->GetDecompresser();
			uintptr_t uMarker = pDecompresser->GetTotalInputSize();

			// Write the first chunk
			pDecompresser->Process(pBuffer1, uBufferSize1,
				pBufferDecoder->m_pSoundImage + uMarker,
				pBufferDecoder->GetCompressedSize() - uMarker);

			// Was wrap around involved?
			if (pBuffer2) {
				// Upload the second chunk
				uMarker = pDecompresser->GetTotalInputSize();
				pDecompresser->Process(pBuffer2, uBufferSize2,
					pBufferDecoder->m_pSoundImage + uMarker,
					pBufferDecoder->GetCompressedSize() - uMarker);
			}
			// Perform the unlock
			uResult =
				pBuffer->Unlock(pBuffer1, uBufferSize1, pBuffer2, uBufferSize2);
		}
	}
	// Return the result
	return static_cast<uint_t>(uResult);
}

#endif
