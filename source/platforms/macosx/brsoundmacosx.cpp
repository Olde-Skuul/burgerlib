/***************************************

    Sound manager class

    MacOSX version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

 ***************************************/

#include "brsound.h"

#if defined(BURGER_MACOSX)
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brosstringfunctions.h"
#include "brsounddecompress.h"
#include "brmemoryfunctions.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <AvailabilityMacros.h>
#include <CoreAudio/CoreAudio.h>

#if !defined(DOXYGEN)

//
// Mac OSX 10.5 or earlier uses an older name for the structure
//

#if !defined(MAC_OS_X_VERSION_10_6)
typedef ComponentDescription AudioComponentDescription;
#endif
#if !defined(MAC_OS_X_VERSION_10_7)
typedef UInt32 AudioFormatFlags;
#define kMultiChannelMixerParam_Volume kStereoMixerParam_Volume
#define kMultiChannelMixerParam_Pan kStereoMixerParam_Pan
#define kAudioUnitSubType_MultiChannelMixer kAudioUnitSubType_StereoMixer
BURGER_INLINE float PANFIX(float x)
{
	return (x + 1.0f) * 0.5f;
}
#else
#define PANFIX(x) x
#endif

//
// The structures below are records to query CoreAudio for information about the
// current sound device hardware
//

//
// Structure for finding the default audio device (AudioDeviceID static)
//

static const AudioObjectPropertyAddress g_DefaultOutputProperty = {
	kAudioHardwarePropertyDefaultOutputDevice, // Selector
	kAudioObjectPropertyScopeGlobal,		   // Scope
	kAudioObjectPropertyElementMaster		   // Element
};

//
// Structure for getting the list of hardware output devices (AudioDeviceID[]
// variable)
//

static const AudioObjectPropertyAddress g_GetDeviceList = {
	kAudioHardwarePropertyDevices,	// Selector
	kAudioObjectPropertyScopeGlobal,  // Scope
	kAudioObjectPropertyElementMaster // Element
};

//
// Structure for finding if the device is active (UInt32 static)
//

static const AudioObjectPropertyAddress g_IsAliveProperty = {
	kAudioDevicePropertyDeviceIsAlive, // Selector
	kAudioDevicePropertyScopeOutput,   // Scope
	kAudioObjectPropertyElementMaster  // Element
};

//
// Structure for finding if the device is being exclusively used by another app
// (pid_t static)
//

static const AudioObjectPropertyAddress g_IsBeingHoggedProperty = {
	kAudioDevicePropertyHogMode,	  // Selector
	kAudioDevicePropertyScopeOutput,  // Scope
	kAudioObjectPropertyElementMaster // Element
};

//
// Structure for getting the AudioBufferList of a device from the Stream
// Configuration (AudioBufferList variable)
//

static const AudioObjectPropertyAddress g_GetStreamConfiguration = {
	kAudioDevicePropertyStreamConfiguration, // Selector
	kAudioDevicePropertyScopeOutput,		 // Scope
	kAudioObjectPropertyElementMaster		 // Element
};

//
// Structure for getting a device's name (CFStringRef static)
//

static const AudioObjectPropertyAddress g_GetDeviceName = {
	kAudioObjectPropertyName,		  // Selector
	kAudioDevicePropertyScopeOutput,  // Scope
	kAudioObjectPropertyElementMaster // Element
};

//
// Get the list of audio ranges for sample rates (AudioValueRange[] variable)
//

static const AudioObjectPropertyAddress g_GetAudioSampleRates = {
	kAudioDevicePropertyAvailableNominalSampleRates, // Selector
	kAudioDevicePropertyScopeOutput,				 // Scope
	kAudioObjectPropertyElementMaster				 // Element
};

//
// Get the scalar volume (float static)
//

static const AudioObjectPropertyAddress g_GetScalarVolume = {
	kAudioDevicePropertyVolumeScalar, // Selector
	kAudioDevicePropertyScopeOutput,  // Scope
	kAudioObjectPropertyElementMaster // Element
};

//
// Get the channel indexes for the Left and Right channels (UInt32[2] static)
//

static const AudioObjectPropertyAddress g_GetPreferredChannels = {
	kAudioDevicePropertyPreferredChannelsForStereo, // Selector
	kAudioDevicePropertyScopeOutput,				// Scope
	kAudioObjectPropertyElementMaster				// Element
};

//
// Default output component description
//

static const AudioComponentDescription g_DefaultOutputComponent = {
	kAudioUnitType_Output,			 // componentType
	kAudioUnitSubType_DefaultOutput, // componentSubType
	kAudioUnitManufacturer_Apple,	// componentManufacturer
	0,								 // componentFlags
	0								 // componentFlagsMask
};

//
// Mixer component description
//

static const AudioComponentDescription g_MixerComponent = {
	kAudioUnitType_Mixer,				 // componentType
	kAudioUnitSubType_MultiChannelMixer, // componentSubType
	kAudioUnitManufacturer_Apple,		 // componentManufacturer
	0,									 // componentFlags
	0									 // componentFlagsMask
};

//
// Data format converter component description
//

static const AudioComponentDescription g_ConverterComponent = {
	kAudioUnitType_FormatConverter, // componentType
	kAudioUnitSubType_AUConverter,  // componentSubType
	kAudioUnitManufacturer_Apple,   // componentManufacturer
	0,								// componentFlags
	0								// componentFlagsMask
};

#endif

/*! ************************************

	\brief Private callback routine for sound sampler

	To play waveforms of all types and sample rates, it was necessary to create
	a sampler to perform the conversion before presenting the data to the mixer.

	Instead of keeping decompressed waveforms, this on the fly conversion helps
	keep the memory footprint low.

	\note This function assumes data is interleaved for stereo, there is no
		support for multiple buffer filling for stereo output.

	\macosxonly

	\param pData "this" pointer for the \ref Voice structure for the voice being
		played
	\param pActionFlags Pointer to action flags passed by AudioGraph
	\param pAudioTimeStamp Pointer to time stamp
	\param uBusNumber Number of the data bus
	\param uNumberFrames Number of requested data frames
	\param pAudioBufferList Pointer to a structure describing the audio buffer
		to fill

	\return 0 for no error

***************************************/

#if __LP64__
signed int Burger::SoundManager::Voice::PlayCallback(void* pData,
	unsigned int* /* pActionFlags */,
	const AudioTimeStamp* /* pAudioTimeStamp */, unsigned int /* uBusNumber */,
	unsigned int /* uNumberFrames */, AudioBufferList* pAudioBufferList)
#else
signed long Burger::SoundManager::Voice::PlayCallback(void* pData,
	unsigned long* /* pActionFlags */,
	const AudioTimeStamp* /* pAudioTimeStamp */, unsigned long /* uBusNumber */,
	unsigned long /* uNumberFrames */, AudioBufferList* pAudioBufferList)
#endif
{
	//
	// Get the pointer to the audio buffer to fill
	//
	AudioBuffer* pAudioBuffer = pAudioBufferList->mBuffers;

	uintptr_t uDestLength = pAudioBuffer->mDataByteSize;
	if (uDestLength) {
		uint8_t* pDestBuffer = static_cast<uint8_t*>(pAudioBuffer->mData);

		//
		// Get the "this" pointer
		//

		Voice* pThis = static_cast<Voice*>(pData);

		//
		// Get the voice's buffer
		//

		const Buffer* pBuffer = pThis->m_pBuffer;
		if (pBuffer) {
			// Grab the default settings from the buffer's description
			const BufferDecoder* pDecoder = pBuffer->GetBufferDescription();
			if (pDecoder) {
				const DecompressAudio* pDecompresser =
					pDecoder->GetDecompresser();
				if (pDecompresser) {

					//
					// Silence?
					//

					uint_t bPlaying = pThis->m_bPlaying;
					if (bPlaying) {

						//
						// Was a buffer attached?
						//
						const void* pSourceData = pBuffer->GetAudioBuffer();
						uintptr_t uSoundLength = pBuffer->GetAudioBufferSize();
						if (!pSourceData || !uSoundLength) {
							//
							// Use silence
							//
							bPlaying = FALSE;
						} else {
							//
							// Looping?
							//
							if (pThis->m_uLoopEnd) {
								uSoundLength = pThis->m_uLoopEnd;
							}
							uintptr_t uMark = pThis->m_uCurrentMark;
							do {
								// Get the chunk to upload
								uintptr_t uRemaining = uSoundLength - uMark;
								uintptr_t uChunk = (uDestLength >= uRemaining) ?
									uRemaining :
									uDestLength;

								//
								// Copy from the buffer into the mixer
								//
								MemoryCopy(pDestBuffer,
									static_cast<const uint8_t*>(pSourceData) +
										uMark,
									uChunk);
								pDestBuffer = pDestBuffer + uChunk;
								uDestLength = uDestLength - uChunk;
								uMark = uMark + uChunk;

								//
								// Hit the end of the data?
								//
								if (uMark == uSoundLength) {

									//
									// Loop?
									//
									if (pThis->m_uLoopEnd) {
										uMark = pThis->m_uLoopStart;
									} else {
										//
										// Fill the remainder with silence
										//

										if (!pThis->m_bIsHeld) {

											SoundManager* pManager =
												pThis->m_pManager;
											uint_t uIndex =
												static_cast<uint_t>(pThis -
													pManager->m_ActiveVoices);

											//
											// Disconnect the sound converter
											// from the mixer
											//
											AUGraphDisconnectNodeInput(
												pManager->GetGraph(),
												pManager->GetMixerNode(),
												uIndex);

											//
											// Release the buffer with a smart
											// pointer
											//
											pThis->m_pBuffer = nullptr;
											pThis->m_bAvailable = TRUE;
											pThis->m_bPaused = FALSE;
										}
										pThis->m_bPlaying =
											FALSE; // Turn off playback
										bPlaying = FALSE;
										break;
									}
								}
							} while (uDestLength);

							//
							// Set the new mark
							//

							pThis->m_uCurrentMark = uMark;
						}
					}

					//
					// Erase the output buffer (Or remainder)
					//

					if (!bPlaying) {
						ClearSoundData(pDestBuffer, uDestLength,
							pDecompresser->GetDataType());
					}
				}
			}
		}
	}
	// Return no error
	return 0;
}

/***************************************

	\brief Constructor

	Initialize the structure to defaults

	\sa Shutdown(void)

***************************************/

Burger::SoundManager::Voice::Voice() :
	m_uCurrentMark(0), m_pBuffer(), m_pCallBack(nullptr),
	m_pCallBackData(nullptr), m_uLoopStart(0), m_uLoopEnd(0), m_uPausedMark(0),
	m_uSampleRate(BURGER_MAXUINT), m_uPan(BURGER_MAXUINT),
	m_uVolume(BURGER_MAXUINT), m_bPlaying(FALSE), m_bPaused(FALSE),
	m_bAvailable(TRUE), m_bIsHeld(FALSE)
{
}

/*! ************************************

	\brief Initialize a voice for playback

	Set up resources needed for playback, sample rate, pan and volume are
	present.

	The voice is ready for playback. Call Start() to start the sound.

	\param pManager Pointer to the parent SoundManager
	\param pBuffer Pointer to the buffer to use for playback
	\return Zero if no error, non-zero if error
	\sa Start(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Init(
	SoundManager* pManager, Buffer* pBuffer)
{
	m_pManager = pManager;

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

	//
	// Get the voice selection
	//

	AudioStreamBasicDescription StreamDescription;
	MemoryClear(&StreamDescription, sizeof(StreamDescription));

	StreamDescription.mSampleRate = m_uSampleRate;
	StreamDescription.mFormatID = kAudioFormatLinearPCM;

	AudioFormatFlags uFlags =
		kAudioFormatFlagsNativeEndian | kLinearPCMFormatFlagIsPacked;
	uint_t uBytes;
	DecompressAudio* pDecompresser = pDecoder->GetDecompresser();
	switch (pDecompresser->GetDataType()) {
	default:
	// Signed bytes
	case TYPECHAR:
		uFlags |= kAudioFormatFlagIsSignedInteger;
	// Unsigned
	case TYPEBYTE:
		uBytes = 1U; // 1 byte per sample
		break;
	case TYPELSHORT:
	case TYPEBSHORT:
		uFlags |= kAudioFormatFlagIsSignedInteger;
		uBytes = 2U; // 2 bytes per sample
		break;
	case TYPELFLOAT:
	case TYPEBFLOAT:
		uFlags |= kAudioFormatFlagIsFloat;
		uBytes = 4U; // 4 bytes per sample
	}

	eDataType uType = pDecoder->m_eDataType;
	UInt32 uChannels = 1U;
	if (uType & TYPESTEREO) {
		uChannels = 2U;
	}
	StreamDescription.mFormatFlags = uFlags;
	StreamDescription.mBytesPerPacket = uBytes;
	StreamDescription.mFramesPerPacket = 1;
	StreamDescription.mBytesPerFrame = uBytes;
	StreamDescription.mChannelsPerFrame = uChannels;
	StreamDescription.mBitsPerChannel = uBytes * 8;

	uChannels = uBytes * uChannels;
	m_uLoopStart = m_uLoopStart * uChannels;
	m_uLoopEnd = m_uLoopEnd * uChannels;

	uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);
	OSStatus uResult = AudioUnitSetProperty(pManager->m_pSoundUnits[uIndex],
		kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0,
		&StreamDescription, static_cast<UInt32>(sizeof(StreamDescription)));

	if (!uResult) {
		//
		// Set the initial volume and pan
		//

		ComponentInstanceRecord* pMixer = pManager->GetMixerUnit();
		uResult = AudioUnitSetParameter(pMixer, kMultiChannelMixerParam_Volume,
			kAudioUnitScope_Input, uIndex, ConvertToAudioUnitVolume(m_uVolume),
			0);
		if (!uResult) {
			uResult = AudioUnitSetParameter(pMixer, kMultiChannelMixerParam_Pan,
				kAudioUnitScope_Input, uIndex,
				PANFIX(ConvertToAudioUnitPan(m_uPan)), 0);
		}
	}
	return static_cast<uint_t>(uResult);
}

/***************************************

	\brief Stop playback and release all resources

	Release the resources allocated by this voice.

	\sa Stop(void)

***************************************/

void BURGER_API Burger::SoundManager::Voice::Shutdown(void)
{
	//
	// Make sure the voice is not playing any sound
	//

	Stop();

	// The smart pointer will release the buffer if this class is the sole owner
	m_pBuffer = nullptr;

	// Not playing anymore
	m_bPlaying = FALSE;
	m_bPaused = FALSE;
	m_bAvailable = TRUE;
}

/***************************************

	\brief Release the connection to the application

	If a pointer to a Voice is requested, it's marked as unable to be used and
	under application control.

	Calling this function will release the voice back to the free pool once the
	playback has stopped automatically.

	If the sound is a looping sound, it will stop immediately

	\sa Shutdown(void)

***************************************/

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

/***************************************

	\brief Start playback

	Start the voice

	\return Zero if no error, non-zero if error
	\sa Stop(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Start(void)
{
	OSStatus uResult = 0;
	if (!m_bPlaying) {

		//
		// Set up for start
		//
		m_uCurrentMark = m_uLoopStart;

		SoundManager* pManager = m_pManager;
		uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);

		//
		// Attach the sound sampler to the mixer
		//

		uResult = AUGraphConnectNodeInput(pManager->GetGraph(),
			pManager->m_iSoundNodes[uIndex], 0, pManager->GetMixerNode(),
			uIndex);
		if (!uResult) {
			//
			// Tell the sound manager to update
			//
			uResult = AUGraphUpdate(pManager->GetGraph(), nullptr);
			m_bPlaying = TRUE;
			m_bPaused = FALSE;
		}
	}
	return uResult;
}

/***************************************

	\brief Stop playback

	Stop the voice

	\return Zero if no error, non-zero if error
	\sa Start(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Stop(void)
{
	uint_t uResult = 0;
	if (m_bPlaying || m_bPaused) {
		SoundManager* pManager = m_pManager;
		uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);
		//
		// Disconnect the sound converter from the mixer
		//
		uResult = AUGraphDisconnectNodeInput(
			pManager->GetGraph(), pManager->GetMixerNode(), uIndex);
		if (!uResult) {

			//
			// Ensure that the mixer is aware that the removal occurred
			//

			uResult = AUGraphUpdate(pManager->GetGraph(), nullptr);
		}

		//
		// Not playing anymore
		//
		m_bPlaying = FALSE;
		m_bPaused = FALSE;
		m_uPausedMark = 0;
	}
	return uResult;
}

/***************************************

	\brief Pause playback

	Stop the voice and record the location to allow playback to resume with a
	call to Resume()

	\return Zero if no error, non-zero if error
	\sa Resume(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Pause(void)
{
	if (m_bPlaying && !m_bPaused) {
		// Save the playback location
		m_uPausedMark = m_uCurrentMark;
		m_bPlaying = FALSE;
		m_bPaused = TRUE;
	}
	return 0;
}

/***************************************

	\brief Resume playback

	Start the voice at the location recorded from the last call to Pause()

	\return Zero if no error, non-zero if error
	\sa Pause(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Resume(void)
{
	if (!m_bPlaying && m_bPaused) {
		// Restore the playback location
		m_uCurrentMark = m_uPausedMark;
		m_bPlaying = TRUE;
		m_bPaused = FALSE;
	}
	return 0;
}

/***************************************

	\brief Set the volume of the voice

	Immediately change the volume of this specific voice

	\param uVolume Volume from 0 to 255 with 0 being silence and 255 being
		maximum
	\return Zero if no error, non-zero if error \sa GetVolume(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetVolume(uint_t uVolume)
{
	OSStatus uResult = 0;

	//
	// Ensure it's bounds checked
	//

	uVolume = minimum(static_cast<uint32_t>(uVolume), static_cast<uint32_t>(255));

	if (m_uVolume != uVolume) {
		m_uVolume = uVolume;
		//
		// Convert to MacOSX
		//
		float fVolume = ConvertToAudioUnitVolume(uVolume);
		SoundManager* pManager = m_pManager;
		uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);
		uResult = AudioUnitSetParameter(pManager->GetMixerUnit(),
			kMultiChannelMixerParam_Volume, kAudioUnitScope_Input, uIndex,
			fVolume, 0);
	}
	return static_cast<uint_t>(uResult);
}

/***************************************

	\brief Set the pan setting of the voice

	Immediately change the pan setting of this specific voice

	\param uPan Pan setting from 0 to 0xFFFF with 0 being left, 0x8000 being
		center and 0xFFFF being right
	\return Zero if no error, non-zero if error
	\sa GetPan(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetPan(uint_t uPan)
{
	OSStatus uResult = 0;

	//
	// Ensure it's bounds checked
	//

	uPan = minimum(static_cast<uint32_t>(uPan), static_cast<uint32_t>(65535));

	if (m_uPan != uPan) {
		m_uPan = uPan;
		//
		// Convert to MacOSX
		//
		float fPan = PANFIX(ConvertToAudioUnitPan(uPan));
		SoundManager* pManager = m_pManager;
		uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);

		//
		// Set the pan
		//

		uResult = AudioUnitSetParameter(pManager->GetMixerUnit(),
			kMultiChannelMixerParam_Pan, kAudioUnitScope_Input, uIndex, fPan,
			0);
	}
	return static_cast<uint_t>(uResult);
}

/***************************************

	\brief Set the samples per second setting of the voice

	Immediately change the playback speed of this specific voice

	\param uSamplesPerSecond Speed of playback in samples per second
	\return Zero if no error, non-zero if error
	\sa GetSampleRate(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetSampleRate(
	uint_t uSamplesPerSecond)
{
	OSStatus uResult = 0;
	//
	// Did it change?
	//
	if (m_uSampleRate != uSamplesPerSecond) {
		//
		// Set the new rate
		//
		m_uSampleRate = uSamplesPerSecond;

		SoundManager* pManager = m_pManager;
		uint_t uIndex = static_cast<uint_t>(this - pManager->m_ActiveVoices);

		//
		// Convert to a double
		//
		Float64 dRate =
			static_cast<Float64>(static_cast<int>(uSamplesPerSecond));

		// Set the new rate to the sound unit
		//

		uResult = AudioUnitSetProperty(pManager->m_pSoundUnits[uIndex],
			kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &dRate,
			static_cast<UInt32>(sizeof(Float64)));
	}
	return static_cast<uint_t>(uResult);
}

/***************************************

	\brief Internal callback

	Releases the voice automatically upon completion

***************************************/

void BURGER_API Burger::SoundManager::Voice::ReachedEnd(void)
{
	if (!m_bIsHeld) {
		Shutdown();
	}
	m_bPlaying = FALSE;
}

/***************************************

	Initialize the sound manager

***************************************/

Burger::SoundManager::SoundManager(GameApp* pGameApp) :
	m_pGameApp(pGameApp), m_pGraph(nullptr), m_pOutputUnit(nullptr),
	m_pMixerUnit(nullptr), m_iOutputNode(0), m_iMixerNode(0), m_uDeviceID(0),
	m_uMaxVoices(cMaxVoiceCount), m_uVolume(cMaxVolume),
	m_bStereoAvailable(TRUE), m_bMasterVolumeAvailable(TRUE),
	m_uBufferDepth(16), m_uOutputSamplesPerSecond(44100)
{
	MemoryClear(m_pSoundUnits, sizeof(m_pSoundUnits));
	MemoryClear(m_iSoundNodes, sizeof(m_iSoundNodes));
}

/***************************************

	\brief Initialize a SoundManager

	\return Zero if no error, non-zero if an error occurred

	For the Mac, an AUGraph is generated which audio is mixed in a mixer object
	that will take all the channels and output in a single output.

	Each Voice will have it's own AudioUnit to output sound data

***************************************/

uint_t BURGER_API Burger::SoundManager::Init(void)
{
	// Get the device ID of the default audio device
	AudioDeviceID uDeviceID;
	UInt32 uSize = static_cast<UInt32>(sizeof(uDeviceID));
	OSStatus uResult = AudioObjectGetPropertyData(kAudioObjectSystemObject,
		&g_DefaultOutputProperty, 0, nullptr, &uSize, &uDeviceID);

	// Got the device?

	if (!uResult) {

		// Test if the requested device is alive (or disconnected, like
		// headphones)
		UInt32 uIsAlive = 0;
		uSize = static_cast<UInt32>(sizeof(uIsAlive));
		uResult = AudioObjectGetPropertyData(
			uDeviceID, &g_IsAliveProperty, 0, nullptr, &uSize, &uIsAlive);
		// If no error and the device is active, continue
		if (!uResult && uIsAlive) {
			pid_t HoggingThreadID = 0;
			uSize = static_cast<UInt32>(sizeof(HoggingThreadID));
			uResult = AudioObjectGetPropertyData(uDeviceID,
				&g_IsBeingHoggedProperty, 0, nullptr, &uSize, &HoggingThreadID);

			//
			// If there's an error, it means that this isn't supported by the
			// device driver, so consider it "good"
			//

			// No one is claiming exclusive access to the device?
			if (uResult || (HoggingThreadID == -1)) {

				// Set the device ID for the audio device
				m_uDeviceID = uDeviceID;

				//
				// Use breaks to abort the flow, it prevents indentation
				//

				for (;;) {

					//
					// Let's make the audio graph!
					//

					uResult = NewAUGraph(&m_pGraph);
					if (uResult) {
						break;
					}

					//
					// Open the graph
					//

					uResult = AUGraphOpen(m_pGraph);
					if (uResult) {
						break;
					}

					//
					// Start with the output device
					//

					SInt32 iTemp;
					uResult = AUGraphAddNode(
						m_pGraph, &g_DefaultOutputComponent, &iTemp);
					if (uResult) {
						break;
					}
					m_iOutputNode = iTemp;

					//
					// Create the mixer for the voices
					//

					uResult =
						AUGraphAddNode(m_pGraph, &g_MixerComponent, &iTemp);
					if (uResult) {
						break;
					}
					m_iMixerNode = iTemp;

					//
					// Connect the mixer to the output hardware
					//

					uResult = AUGraphConnectNodeInput(
						m_pGraph, m_iMixerNode, 0, m_iOutputNode, 0);
					if (uResult) {
						break;
					}

					//
					// Get the units for the mixer and the output device
					//

					uResult = AUGraphNodeInfo(
						m_pGraph, m_iMixerNode, nullptr, &m_pMixerUnit);
					if (uResult) {
						break;
					}

					uResult = AUGraphNodeInfo(
						m_pGraph, m_iOutputNode, nullptr, &m_pOutputUnit);
					if (uResult) {
						break;
					}

					//
					// Set the number of voices in the mixer for Burgerlib's
					// maximum count (It's usually 64, but set it to 32 to
					// reduce overhead
					//
					uSize = cMaxVoiceCount;
					uResult = AudioUnitSetProperty(m_pMixerUnit,
						kAudioUnitProperty_ElementCount, kAudioUnitScope_Input,
						0, &uSize, static_cast<UInt32>(sizeof(uSize)));
					if (uResult) {
						break;
					}

					AURenderCallbackStruct MyCallback;
					MyCallback.inputProc = Voice::PlayCallback;
					uintptr_t i = 0;
					do {
						uResult = AUGraphAddNode(
							m_pGraph, &g_ConverterComponent, &iTemp);
						if (uResult) {
							break;
						}
						m_iSoundNodes[i] = iTemp;
						uResult = AUGraphNodeInfo(m_pGraph, m_iSoundNodes[i],
							nullptr, &m_pSoundUnits[i]);
						if (uResult) {
							break;
						}
						MyCallback.inputProcRefCon = &m_ActiveVoices[i];
						uResult = AUGraphSetNodeInputCallback(
							m_pGraph, m_iSoundNodes[i], 0, &MyCallback);
						if (uResult) {
							break;
						}
					} while (++i < cMaxVoiceCount);
					if (uResult) {
						break;
					}

					//
					// For iOS and Mac OSX, allow the mixer to use LARGE chunks
					// of data in the event audio is playing when a screen save
					// (OSX) or a lock screen (iOS) without glitching
					//

					uSize = 4096;
					uResult = AudioUnitSetProperty(m_pMixerUnit,
						kAudioUnitProperty_MaximumFramesPerSlice,
						kAudioUnitScope_Global, 0, &uSize,
						static_cast<UInt32>(sizeof(uSize)));
					if (uResult) {
						break;
					}

					//
					// Set the initial pan and volume to the sound system
					//

					uResult = AudioUnitSetParameter(m_pMixerUnit,
						kMultiChannelMixerParam_Volume, kAudioUnitScope_Output,
						0, ConvertToAudioUnitVolume(m_uVolume), 0);
					if (uResult) {
						break;
					}

					uResult = AudioUnitSetParameter(m_pMixerUnit,
						kMultiChannelMixerParam_Pan, kAudioUnitScope_Output, 0,
						PANFIX(0.0f), 0);
					if (uResult) {
						break;
					}

					uResult = AUGraphInitialize(m_pGraph);
					if (uResult) {
						break;
					}
					uResult = AUGraphStart(m_pGraph);
					break;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Shut down a SoundManager

***************************************/

void BURGER_API Burger::SoundManager::Shutdown(void)
{
	//
	// Make sure every voice is turned off
	//
	uint_t i = cMaxVoiceCount;
	Voice* pWork = m_ActiveVoices;
	do {
		// Shut down every voice
		pWork->Shutdown();
		++pWork;
	} while (--i);

	//
	// If there is a AUGraph, make sure it's turned off
	//

	OSStatus uResult;
	if (m_pGraph) {

		//
		// Stop it right now
		//

		uResult = AUGraphStop(m_pGraph);
		uResult = AUGraphUninitialize(m_pGraph);
		uResult = AUGraphClose(m_pGraph);

		//
		// Get rid of all of the sound nodes
		//

		i = 0;
		do {
			uResult = AUGraphRemoveNode(m_pGraph, m_iSoundNodes[i]);
		} while (++i < cMaxVoiceCount);
		//
		// Clear the memory internally
		//
		MemoryClear(m_pSoundUnits, sizeof(m_pSoundUnits));
		MemoryClear(m_iSoundNodes, sizeof(m_iSoundNodes));

		//
		// Was there a mixer?
		//

		if (m_iMixerNode) {

			//
			// Disconnect from output
			//

			uResult = AUGraphDisconnectNodeInput(m_pGraph, m_iOutputNode, 0);

			//
			// Dispose of the mixer
			//

			uResult = AUGraphRemoveNode(m_pGraph, m_iMixerNode);
			m_pMixerUnit = nullptr;
			m_iMixerNode = 0;
		}

		//
		// Was there an output node?
		//

		if (m_iOutputNode) {
			uResult = AUGraphRemoveNode(m_pGraph, m_iOutputNode);
			m_pOutputUnit = nullptr;
			m_iOutputNode = 0;
		}

		//
		// Dispose of the main node
		//

		uResult = DisposeAUGraph(m_pGraph);
		m_pGraph = nullptr;
	}
}

/***************************************

	\brief Set the master volume

	\param uVolume Master volume 0-255

***************************************/

void BURGER_API Burger::SoundManager::SetVolume(uint_t uVolume)
{
	if (uVolume >= 255U) {
		uVolume = 255;
	}
	// Did it change?
	if (uVolume != m_uVolume) {
		m_uVolume = uVolume;
		// Only update the volume if the audio system is running
		if (m_pMixerUnit) {
			AudioUnitSetParameter(m_pMixerUnit, kMultiChannelMixerParam_Volume,
				kAudioUnitScope_Output, 0, ConvertToAudioUnitVolume(uVolume),
				0);
		}
	}
}

/***************************************

	\brief Get a list of available audio modes

	\param pOutput Pointer to array of SoundCardDescription entries
	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::SoundManager::GetAudioModes(
	ClassArray<SoundCardDescription>* pOutput)
{
	// Clear out the list
	pOutput->clear();

	// How much memory is needed for the device list?
	UInt32 uSize = 0;
	OSStatus uResult = AudioObjectGetPropertyDataSize(
		kAudioObjectSystemObject, &g_GetDeviceList, 0, nullptr, &uSize);
	if (!uResult && uSize) {

		// Use a local buffer to avoid memory allocation in 99% of the time this
		// function is called

		AudioDeviceID MyDeviceList[16];
		AudioDeviceID* pDeviceList = MyDeviceList;
		if (uSize > sizeof(MyDeviceList)) {
			pDeviceList = static_cast<AudioDeviceID*>(alloc_clear(uSize));
			// What?
			if (!pDeviceList) {
				pDeviceList = MyDeviceList;
				uSize = static_cast<UInt32>(sizeof(MyDeviceList));
			}
		}

		// Get the list of DeviceIDs

		uResult = AudioObjectGetPropertyData(kAudioObjectSystemObject,
			&g_GetDeviceList, 0, nullptr, &uSize, pDeviceList);
		if (!uResult) {

			// Iterate over the list

			uintptr_t i = 0;
			uintptr_t uCount = uSize / static_cast<uint32_t>(sizeof(AudioDeviceID));
			do {
				AudioDeviceID uDeviceID = pDeviceList[i];

				// Since this is a list of ALL audio devices, test it if it has
				// an OUTPUT stream to rule out input devices like microphones.

				uResult = AudioObjectGetPropertyDataSize(
					uDeviceID, &g_GetStreamConfiguration, 0, nullptr, &uSize);
				if (uResult) {
					continue;
				}

				// Get space for the audio buffer list
				AudioBufferList* pBufferList =
					static_cast<AudioBufferList*>(Alloc(uSize));
				uint_t bOutputDevice = FALSE;
				if (pBufferList) {
					// Grab the list
					uResult = AudioObjectGetPropertyData(uDeviceID,
						&g_GetStreamConfiguration, 0, nullptr, &uSize,
						pBufferList);
					if (!uResult) {
						// Any entries?
						UInt32 uBufferCount = pBufferList->mNumberBuffers;
						if (uBufferCount) {
							const AudioBuffer* pAudioBuffer =
								pBufferList->mBuffers;
							do {
								// Does this buffer accept output data?
								if (pAudioBuffer->mNumberChannels) {
									// It's an output device
									bOutputDevice = 1;
									break;
								}
								++pAudioBuffer;
							} while (--uBufferCount);
						}
					}
					Free(pBufferList);
				}

				// Was it an output device?
				if (!bOutputDevice) {
					// Skip input devices or unknown devices
					continue;
				}

				// Initialize an entry to add to the list
				SoundCardDescription Entry;

				// Set the device ID
				Entry.m_uDevNumber = uDeviceID;

				// Let's get the devices' name

				uSize = static_cast<UInt32>(sizeof(CFStringRef));
				CFStringRef pCFString = nullptr;
				uResult = AudioObjectGetPropertyData(uDeviceID,
					&g_GetDeviceName, 0, nullptr, &uSize, &pCFString);
				// What? No name?!?!
				if (uResult) {
					continue;
				}
				StringCopy(&Entry.m_DeviceName, pCFString);
				CFRelease(pCFString);

				// Some devices have whitespace, remove it
				StripLeadingAndTrailingSpaces(Entry.m_DeviceName.c_str());
				Entry.m_DeviceName.resize(
					StringLength(Entry.m_DeviceName.c_str()));

				// Mac OSX performs the translation in software
				Entry.m_b8Bit = TRUE;
				Entry.m_b16Bit = TRUE;
				Entry.m_bStereo = TRUE;

				// As of 2015, no mac uses a sound accelerator card
				Entry.m_bHardwareAccelerated = FALSE;

				// Get the list of sample rates the hardware can support
				uResult = AudioObjectGetPropertyDataSize(
					uDeviceID, &g_GetAudioSampleRates, 0, nullptr, &uSize);
				if (!uResult && uSize) {
					// Get space for the audio buffer list
					AudioValueRange* pRangeList =
						static_cast<AudioValueRange*>(Alloc(uSize));
					if (pRangeList) {
						uResult = AudioObjectGetPropertyData(uDeviceID,
							&g_GetAudioSampleRates, 0, nullptr, &uSize,
							pRangeList);
						if (!uResult) {

							// Iterate over the table to get the true minimum
							// and maximum
							uintptr_t uCountRange = uSize /
								static_cast<uint32_t>(sizeof(AudioValueRange));
							const AudioValueRange* pRange = pRangeList;
							Float64 fMin = g_dInf;
							Float64 fMax = 0.0;
							do {
								fMin = minimum(pRange->mMinimum, fMin);
								fMax = maximum(pRange->mMaximum, fMax);
								++pRange;
							} while (--uCountRange);
							Entry.m_uMinimumSampleRate =
								static_cast<uint_t>(static_cast<int_t>(fMin));
							Entry.m_uMaximumSampleRate =
								static_cast<uint_t>(static_cast<int_t>(fMax));
						}
						Free(pRangeList);
					}
				}

				// Save the audio device to the list
				pOutput->push_back(Entry);
			} while (++i < uCount);
		}

		// Release the device list buffer
		if (pDeviceList != MyDeviceList) {
			Free(pDeviceList);
		}
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Gets the audio device's master volume

	Query the device if it has mono volume control and if so, return the value
	it's set at. If it has stereo control, get the average volume of the left
	and right channels. If there is no control, or some other error occurred,
	return NaN instead of the volume.

	\macosxonly
	\param uDeviceID ID of the MacOSX audio device
	\return 0.0 for silence, 1.0f for maximum or a number in between or NaN on
		error.
	\sa AudioDeviceSetMasterVolume(uint32_t,float)

***************************************/

float BURGER_API Burger::AudioDeviceGetMasterVolume(uint32_t uDeviceID)
{
	UInt32 uSize; // Data size
	float fTemp;  // Temp float

	// Start off by checking if it a volume control is supported in mono
	Boolean bHasIt = AudioObjectHasProperty(uDeviceID, &g_GetScalarVolume);

	OSStatus uResult = 666;
	float fResult = g_fNan; // Error is NaN
	// If mono volume is present, just use that
	if (bHasIt) {
		uSize = static_cast<UInt32>(sizeof(fTemp));
		// Get the volume (0-1.0)
		uResult = AudioObjectGetPropertyData(
			uDeviceID, &g_GetScalarVolume, 0, nullptr, &uSize, &fTemp);
		if (!uResult) {
			// We are golden!
			fResult = fTemp;
		}
	}

	// If either it's not mono or the mono call failed, try stereo Stereo is for
	// iMac or other Macs with twin speakers built into the cabinet

	if (uResult) {
		// Request the channels for left and right
		bHasIt = AudioObjectHasProperty(uDeviceID, &g_GetPreferredChannels);
		if (bHasIt) {
			// Obtain the channel indexes for left and right
			UInt32 ChannelIndexes[2];
			uSize = static_cast<UInt32>(sizeof(ChannelIndexes));
			uResult = AudioObjectGetPropertyData(uDeviceID,
				&g_GetPreferredChannels, 0, nullptr, &uSize, ChannelIndexes);
			if (!uResult) {

				// Got the indexes for the left and right channels.
				// Extract the data for the channels
				uSize = static_cast<UInt32>(sizeof(fTemp));
				AudioObjectPropertyAddress MyPropertyAddress;
				MyPropertyAddress.mSelector = kAudioDevicePropertyVolumeScalar;
				MyPropertyAddress.mScope = kAudioDevicePropertyScopeOutput;
				MyPropertyAddress.mElement = ChannelIndexes[0];
				Boolean bGotLeft =
					AudioObjectHasProperty(uDeviceID, &MyPropertyAddress);
				float fLeft = 0.0f;
				if (bGotLeft) {
					uResult = AudioObjectGetPropertyData(uDeviceID,
						&MyPropertyAddress, 0, nullptr, &uSize, &fTemp);
					if (!uResult) {
						fLeft = fTemp;
					}
				}
				MyPropertyAddress.mElement = ChannelIndexes[1];
				Boolean bGotRight =
					AudioObjectHasProperty(uDeviceID, &MyPropertyAddress);
				float fRight = 0.0f;
				if (bGotRight) {
					uResult = AudioObjectGetPropertyData(uDeviceID,
						&MyPropertyAddress, 0, nullptr, &uSize, &fTemp);
					if (!uResult) {
						fRight = fTemp;
					}
				}
				if (bGotLeft && bGotRight) {
					fResult = (fLeft + fRight) * 0.5f;
				}
			}
		}
	}
	return fResult;
}

/*! ************************************

	\brief Sets an audio device's master volume

	Given a volume from 0.0f to 1.0f, set the master volume for the requested
	audio device. It will first query the device if mono volume control is
	possible and sets that. If mono volume control is not present it will then
	check if there are independent left and right controls and will set both to
	the input volume.

	\macosxonly
	\param uDeviceID ID of the MacOSX audio device
	\param fInput Volume to set from 0.0f to 1.0f
	\return Zero if no error, non-zero if error
	\sa AudioDeviceGetMasterVolume(uint32_t)

***************************************/

uint_t BURGER_API Burger::AudioDeviceSetMasterVolume(
	uint32_t uDeviceID, float fInput)
{
	float fTemp = fInput; // Temp float

	// Start off by checking if it a volume control is supported in mono
	Boolean bCanSetIt;
	OSStatus uResult = AudioObjectIsPropertySettable(
		uDeviceID, &g_GetScalarVolume, &bCanSetIt);
	if (!uResult) {
		if (!bCanSetIt) {
			uResult = -44; // Write protected error
		} else {
			// Get the volume (0-1.0)
			uResult = AudioObjectSetPropertyData(uDeviceID, &g_GetScalarVolume,
				0, nullptr, static_cast<UInt32>(sizeof(fTemp)), &fTemp);
		}
	}

	// If it couldn't set the mono volume, try the stereo volume for iMacs that
	// have speakers built in

	if (uResult) {

		// Request the channels for left and right (Fail if can't find it)
		Boolean bHasIt =
			AudioObjectHasProperty(uDeviceID, &g_GetPreferredChannels);
		if (bHasIt) {

			// Obtain the channel indexes for left and right
			UInt32 ChannelIndexes[2];
			UInt32 uSize = static_cast<UInt32>(sizeof(ChannelIndexes));
			uResult = AudioObjectGetPropertyData(uDeviceID,
				&g_GetPreferredChannels, 0, nullptr, &uSize, ChannelIndexes);
			if (!uResult) {

				// Got the indexes for the left and right channels.
				// Set the volume for the channels (if applicable)

				AudioObjectPropertyAddress MyPropertyAddress;
				MyPropertyAddress.mSelector = kAudioDevicePropertyVolumeScalar;
				MyPropertyAddress.mScope = kAudioDevicePropertyScopeOutput;
				MyPropertyAddress.mElement = ChannelIndexes[0];
				OSStatus uLeftResult = AudioObjectIsPropertySettable(
					uDeviceID, &MyPropertyAddress, &bCanSetIt);
				if (!uLeftResult && bCanSetIt) {
					uResult = AudioObjectSetPropertyData(uDeviceID,
						&MyPropertyAddress, 0, nullptr,
						static_cast<UInt32>(sizeof(fTemp)), &fTemp);
				}
				MyPropertyAddress.mElement = ChannelIndexes[1];
				OSStatus uRightResult = AudioObjectIsPropertySettable(
					uDeviceID, &MyPropertyAddress, &bCanSetIt);
				if (!uRightResult && bCanSetIt) {
					uResult = AudioObjectSetPropertyData(uDeviceID,
						&MyPropertyAddress, 0, nullptr,
						static_cast<UInt32>(sizeof(fTemp)), &fTemp);
				}
				//
				// Return the error, if any
				//

				if (!uResult) {
					if (uLeftResult) {
						uResult = uLeftResult;
					} else if (uRightResult) {
						uResult = uRightResult;
					}
				}
			}
		}
	}
	return static_cast<uint_t>(uResult);
}

#endif
