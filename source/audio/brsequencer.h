/***************************************

    Music Sequencer

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSEQUENCER_H__
#define __BRSEQUENCER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSOUNDDECOMPRESS_H__
#include "brsounddecompress.h"
#endif

#ifndef __BRMEMORYFUNCTIONS_H__
#include "brmemoryfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class Sequencer {
public:
	static const uint_t cAmigaFrequency = 8363;		///< Default MOD file sample frequency (7159090/856)
	static const uint_t cInstrumentMaxCount = 255;	///< Maximum number of instruments
	static const uint_t cSampleMaxCount = 64;			///< Maximum number of samples
	static const uint_t cPointerMaxCount = 999;		///< Maximum number of track pointers
	static const uint_t cTrackMaxCount = 256;			///< Maximum number of tracks
	static const uint_t cPatternMaxCount = 200;		///< Maximum number of patterns
	static const uint_t cArpeggioMaxCount = 3;		///< Maximum number of arpeggio commands
	static const uint_t cMaxPlugIns = 16;				///< Maximum number of song format importers
	static const uint_t cDefaultVolumeFade = 300;		///< Default volume fade speed
	static const uint_t cAmigaClock = 14317456;		///< Scream Tracker Amiga base clock
	static const uint_t cFixedPointShift = 8;			///< Number of bits for fixed point math 24.8
	static const uint_t cMaxVolume = 64;				///< Maximum internal volume for commands
	static const uint_t cMaxPan = 64;					///< Maximum internal pan for commands
	static const uint_t cMinimumPitch = 113;			///< Minimum pitch (Samples per second)
	static const uint_t cMaximumPitch = 27392;		///< Maximum pitch (Samples per second)
	static const int32_t g_VibratoTable[64];			///< Vibrato table

	enum eImporterErrors {
		IMPORT_OKAY,
		IMPORT_UNKNOWN,
		IMPORT_BADFILE,
		IMPORT_TRUNCATION,
		IMPORT_OUTOFMEMORY
	};
	enum eLoopType {
		LOOP_NORMAL = 0,		///< Loop from the beginning to the end
		LOOP_PINGPONG = 1		///< Loop from the beginning to the end and back to the beginning
	};

	enum eNote {
		NOTE_MIN=0,			///< Minimum note value (C natural - ascending)
		NOTE_MID=40,		///< Middle value for defaults
		NOTE_LAST=95,		///< Last valid note
		NOTE_MAX=96,		///< 8 octave range
		NOTE_OFF=254,		///< Silence this channel
		NOTE_UNUSED=255		///< Note value is not used
	};

	struct Command_t {
		enum eEffect {
			EFFECT_ARPEGGIO 	= 0,	// 0x00
			EFFECT_DOWNSLIDE 	= 1,	// 0x01
			EFFECT_UPSLIDE 		= 2,	// 0x02
			EFFECT_PORTAMENTO 	= 3,	// 0x03
			EFFECT_VIBRATO 		= 4,	// 0x04
			EFFECT_PORTASLIDE 	= 5,	// 0x05
			EFFECT_VIBRATOSLIDE	= 6,	// 0x06
			EFFECT_NONE 		= 7,	// 0x07
			EFFECT_PANNING		= 8,	// 0x08
			EFFECT_OFFSET 		= 9,	// 0x09
			EFFECT_SLIDEVOLUME 	= 10,	// 0x0A
			EFFECT_FASTSKIP 	= 11,	// 0x0B
			EFFECT_VOLUME 		= 12,	// 0x0C
			EFFECT_SKIP 		= 13,	// 0x0D
			EFFECT_EXTENDED 	= 14,	// 0x0E
			EFFECT_SPEED 		= 15	// 0x0F
		};
		enum eVolumeCommand {
			VOLUME_MIN = 0,
			VOLUME_MAX = 254,
			VOLUME_UNUSED = 255
		};
		uint8_t m_uNote;				///< eNote value
		uint8_t m_uInstrument;		///< Instrument attached to the command (0 is no instrument)
		uint8_t m_uVolume;			///< Volume command (255 = no command)
		uint8_t m_uEffectCommand;		///< eEffect Command enumeration
		uint8_t m_uEffectArgument;	///< Argument for the above effect command

		void BURGER_API Clear(void);
		BURGER_INLINE eNote GetNote(void) const { return static_cast<eNote>(m_uNote); }
		BURGER_INLINE eVolumeCommand GetVolume(void) const { return static_cast<eVolumeCommand>(m_uVolume); }
		BURGER_INLINE eEffect GetEffect(void) const { return static_cast<eEffect>(m_uEffectCommand); }
		BURGER_INLINE void SetEffect(eEffect uEffectCommand) { m_uEffectCommand = static_cast<uint8_t>(uEffectCommand); }
	};

	struct PatternData_t {
		uint32_t m_uRowCount;			///< Number of rows in a pattern
		uint32_t m_uChannelCount;		///< Number of pattern channels
		char m_Name[32];			///< Name of the sequence pattern
		Command_t m_Commands[1];	///< Array of sequence patterns (m_uPatternSize*m_uChannelCount is the size)
		static PatternData_t * BURGER_API New(uint_t uRows,uint_t uChannels);
		Command_t* BURGER_API GetCommand(int iRow,int iChannel);
		BURGER_INLINE void SetName(const char *pName) { string_copy(m_Name,sizeof(m_Name),pName); }
	};

	class SampleDescription {
	public:
		void *m_pSample;			///< Pointer to the sample data
		uint32_t m_uSampleSize;		///< Number of bytes in the sample
		uint32_t m_uLoopStart;		///< Byte to start the loop
		uint32_t m_uLoopLength;		///< Byte to end the loop
		uint_t m_uC2SamplesPerSecond;	///< Samples per second to generate C2 (Fine tune adjust)
		int_t m_iRelativeNote;		///< Note adjust in semitones
		uint8_t m_uVolume;			///< Default volume of the sample
		uint8_t m_eLoopType;			///< Default loop behavior
		uint8_t m_uBitsPerSample;		///< 8 or 16 bits
		uint8_t m_bStereo;			///< Stereo
		char m_Name[32];			///< Sample name
	public:
		~SampleDescription();
		static SampleDescription * BURGER_API New(void);
		BURGER_INLINE void SetName(const char *pName) { string_copy(m_Name,sizeof(m_Name),pName); }
	};

	struct EnvelopeMarker_t {
		uint16_t m_uPosition;	///< Time mark for when this volume is active
		uint16_t m_uVolume;	///< Volume at this time mark
		uint_t BURGER_API Interpolate(const EnvelopeMarker_t *pNext,uint_t uPosition) const;
	};

	struct InstrData_t {
		enum eEnvelopFlags {
			ENVELOPE_ON			= 1,	///< If set, the envelope is active
			ENVELOPE_SUSTAIN	= 2,	///< If set, the envelope has a sustain section
			ENVELOPE_LOOP		= 4		///< If set, the envelope loops
		};
		uint_t m_uBaseSampleID;			///< First sample ID in sample list
		uint_t m_uNumberSamples;			///< Number of samples in instrument
		EnvelopeMarker_t m_VolumeEnvelope[12];	///< Points for volume envelope
		EnvelopeMarker_t m_PanEnvelope[12];		///< Points for panning envelope
		uint8_t m_WhichSampleForNote[96];	///< Sample number for all notes

		uint8_t m_uVolumeEnvelopeCount;	///< Number of volume points
		uint8_t m_uPanEnvelopeCount;		///< Number of panning points

		uint8_t m_uVolumeSustainIndex;	///< Volume sustain point
		uint8_t m_uVolumeBeginIndex;		///< Volume loop start point
		uint8_t m_uVolumeEndIndex;		///< Volume loop end point

		uint8_t m_uPanSustainIndex;		///< Panning sustain point
		uint8_t m_uPanBeginIndex;			///< Panning loop start point
		uint8_t m_uPanEndIndex;			///< Panning loop end point

		uint8_t m_uVolumeEnvelopeFlags;	///< Flags for the volume envelope (See eEnvelopFlags)
		uint8_t m_uPanEnvelopeFlags;		///< Flags for the pan envelope (See eEnvelopFlags)

		uint16_t m_uVolumeFadeSpeed;		///< Volume fadeout speed

		uint8_t m_uVibratoDepth;			///< Depth of the vibrato
		uint8_t m_uVibratoSpeed;			///< Speed of vibrato oscillation
		uint8_t m_uInstrumentNumber;		///< Instrument number
		char m_Name[32];				///< Instrument name
		BURGER_INLINE void SetName(const char *pName) { string_copy(m_Name,sizeof(m_Name),pName); }
		void BURGER_API Reset(void);
	};

	struct SongDescription_t {
		uint_t m_uDefaultSpeed;			///< Default song speed
		uint_t m_uDefaultTempo;			///< Default song tempo
		uint_t m_uMasterPitch;			///< Master Pitch
		uint_t m_uMasterSpeed;			///< Master Speed
		uint_t m_uMasterVolume;			///< Master volume
		uint_t m_uPatternCount;			///< Number of patterns
		uint_t m_uChannelCount;			///< Number of channels
		uint_t m_uPointerCount;			///< Number of pointers in m_PatternPointers
		uint_t m_uInstrumentCount;		///< Number of instruments
		uint_t m_uSampleCount;			///< Number of samples
		uint_t m_PatternPointers[cPointerMaxCount];	///< List of patterns for the song
		uint_t m_ChannelPans[cTrackMaxCount];		///< Pan settings for each channel
		uint_t m_ChannelVolumes[cTrackMaxCount];	///< Volume settings for each channel (0-64)
		char m_Name[32];				///< Name of the song
		BURGER_INLINE void Clear(void) { MemoryClear(this,sizeof(*this)); }
		BURGER_INLINE void SetName(const char *pName) { string_copy(m_Name,sizeof(m_Name),pName); }
	};

	struct Channel_t {
		const char *m_pBeginningOfSample;	///< Beginning of sample data
		const char *m_pEndOfSample;			///< End of the sample data
		const char *m_pCurrent;				///< Current sample being processed
		const char *m_pEchoSample;			///< Sample processed for echo effect
		uintptr_t m_uSampleSize;				///< Sample Size in bytes
		uint_t m_uID;						///< Channel ID - 0 to cTrackMaxCount
		uint_t m_uBitsPerSample;			///< 8 or 16 bits
		uint_t m_uLoopBeginning;			///< Loop start position in bytes
		uint_t m_uLoopSize;				///< Loop size in bytes
		uint_t m_uCurrentInstrumentID;	///< Current Instrument ID
		uint_t m_uPreviousInstrumentID;	///< Previous Instrument ID
		uint_t m_uSampleID;				///< Current Sample ID
		uint_t m_uAmigaFineTune;			///< Samples per second adjustment
		eNote m_eNote;					///< Musical note number
		int32_t m_iPreviousNote;			///< Previous note number
		uint_t m_uTimePeriod;			///< Current time period
		uint_t m_uPreviousTimePeriod;	///< Previous time period
		uint32_t m_uVolume;				///< Channel vol (0 to 64)
		int32_t m_iPan;					///< Channel pan (0 to 64)
		uint_t m_uEffectCommand;			///< See eEffect
		uint_t m_uEffectArgument;			///< Argument of command
		uint_t m_uVolumeCommand;			///< Volume Command

		int32_t m_Arpeggios[cArpeggioMaxCount];		///< Array of Arpeggio points
		uint_t m_uArpeggioIndex;			///< Index to the active arpeggio in the above table

		int32_t m_iVibratoOffset;			///< Base vibrato value
		int32_t m_iVibratoDepth;			///< Range of the vibrato
		int32_t m_iCurrentVibration;		///< Current vibrato value

		int32_t m_iSlide;					///< Adjust value for time period, used for slideUp and slideDown commands

		int32_t m_iPitchGoal;				///< Portamento goal value
		int32_t m_iPitchRate;				///< Speed of Portamento

		int32_t m_iVolumeRate;			// Used for slideVolume command

		uint_t m_PreviousArguments[16];	///< Arguments for effect commands, Indexed by eEffect

		uint_t m_uCurrentVolumeEnvelopeIndex;	///< Current index for volume envelopes
		uint_t m_uNextVolumeEnvelopeIndex;	///< Next index for volume envelopes
		uint_t m_uCurrentVolumePosition;		///< Current time mark used for interpolation of volume envelopes
		uint_t m_uCurrentVolumeFromEnvelope;	///< Volume from interpolated volume (0-64)

		uint_t m_uCurrentPanEnvelopeIndex;	///< Current index for pan envelopes
		uint_t m_uNextPanEnvelopeIndex;		///< Next index for pan envelopes
		uint_t m_uCurrentPanPosition;			///< Current time mark used for interpolation of pan envelopes
		uint_t m_uCurrentPanFromEnvelope;		///< Pan from interpolated pan

		uint32_t m_uVolumeFade;				///< Volume fade speed (0-32767)
		int32_t m_iAccumulatorFraction;		///< Accumulator fraction from last pass, 24.8 fixed point

		int32_t m_iLastWordLeft;
		int32_t m_iCurrentLastWordLeft;
		int32_t m_iLastWordRight;
		int32_t m_iCurrentLastWordRight;
		uint_t m_uCurrentLevelLeft;
		uint_t m_uCurrentLevelRight;

		uint8_t m_bLevelDirectionLeft;
		uint8_t m_bLevelDirectionRight;
		uint8_t m_bRemoverWorking;
		uint8_t m_bKeyOn;

		uint32_t m_uPreviousVolumeLeft;
		uint32_t m_uPreviousVolumeRight;
		int32_t m_iPreviousOffset;
		uint_t m_uEffectPartitionIndex;
		uint_t m_uEffectRowIndex;
		uint8_t m_bEffectActive;			///< If \ref TRUE, an audio effect is active
		uint8_t m_bStereo;				///< If \ref TRUE, channel is controlling a stereo sample
		uint8_t m_eLoopType;				///< See eLoopType
		uint8_t m_bPingPongReverse;		///< If \ref TRUE, ping pong is in reverse direction
		int32_t m_iPreviousValue1Left;
		int32_t m_iPreviousValue2Left;
		int32_t m_iPreviousValue1Right;
		int32_t m_iPreviousValue2Right;

        int32_t m_iPreviousValue3Left;
        int32_t m_iPreviousValue4Left;
		int32_t m_iPreviousValue3Right;
		int32_t m_iPreviousValue4Right;
		uint_t m_uTickRemoveSize;
		void BURGER_API Init(uint_t uID);
		void BURGER_API Purge(void);
		void BURGER_API VolumeCommand(uint_t uCall);
		void BURGER_API ParseSlideVolume(uint_t uArgument);
		void BURGER_API FillBuffers(int32_t *pLeft,int32_t *pRight,uintptr_t uCount,int32_t iLeft,int32_t iRight);
		void BURGER_API Tickloop(int32_t *pLeft,int32_t *pRight,uintptr_t uCount);
	};

	class SongPackage {
	public:
		PatternData_t *m_pPartitions[cPatternMaxCount];	///< Patterns
		SampleDescription *m_pSampleDescriptions[cInstrumentMaxCount*cSampleMaxCount];	///< Samples
		InstrData_t m_InstrDatas[cInstrumentMaxCount];	///< Instruments
		SongDescription_t m_SongDescription;			///< Description of the entire song
		uint_t m_bMusicUnderModification;					///< Tell the driver to NOT access music data
		SongPackage();
		~SongPackage();
		void BURGER_API Shutdown(void);
		void BURGER_API RemoveInstrument(uint_t uInstrumentIndex);
		static SongPackage * BURGER_API New(void);
	};

	typedef uint_t (BURGER_API *ImportProc)(SongPackage *pOutput,const uint8_t *pInput,uintptr_t uInputLength);

protected:
	SoundManager *m_pSoundManager;	///< Pointer to the sound manager to send audio data to
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static const uint_t cMaxEventCount = 2;
	IDirectSoundBuffer8 *m_pDirectSoundBuffer8;	///< DirectSoundBuffer (Windows Only)
	IDirectSoundNotify *m_pDirectSoundNotify;	///< DirectSoundNotify for alert when sound is complete (Windows Only)
	void *m_hCallback;							///< HANDLE of the callback thread (Windows only)
	unsigned long m_uCallbackID;				///< ID of the callback thread (Windows only)
	void *m_hEvents[cMaxEventCount];			///< Events for DirectSound completion functions (Windows Only)
	static unsigned long __stdcall ThreadCallback(void *pThis);
#endif
	SongPackage *m_pSongPackage;	///< Current song being played
	int32_t *m_pAccumBuffer32;		///< 32 bit accumulator buffer (Used for 16 bit samples)
	int16_t *m_pAccumBuffer16;		///< 16 bit accumulator buffer (Used for 8 bit samples)
	void *m_pReverbBuffer;			///< Pointer to the reverb buffer
	uintptr_t m_uReverbDelayBufferSize;	///< Length in samples of the reverb buffer
	void *m_pBuffer;				///< Data buffer
	uintptr_t m_uBufferSize;			///< Size of the data buffer in bytes
	uintptr_t m_uBytesGenerated;		///< Number of bytes generated and uploaded to sound driver
	uintptr_t m_uBytesToGenerate;		///< Number of bytes to generate
	uintptr_t m_uChunkSize;			///< Number of bytes per time quantum (50Hz)
	uintptr_t m_uAccumBufferSize;			///< Size in bytes of the accumulator buffer
	uintptr_t m_uMicroDelayBufferSize;	///< Size in bytes of the delay buffer
	uint_t m_uImporterCount;			///< Number of pointers in m_pImporters
	uint_t m_uMaxVoices;				///< Active number of track for the currently playing song
	uint_t m_uMicroDelayDuration;		///< Micro delay duration (min 0 ms, max 1000 ms)
	SoundManager::eDataType m_eOutputDataType;		///< Mix to 8 or 16 bit size
	uint_t m_uFinalSampleRate;		///< Mix to this many samples per second
	uint_t m_uReverbSizeMS;			///< Reverb delay duration (min 25 ms, max 1000 ms)
	uint_t m_uReverbStrength;			///< Reverb strength in % (0 <-> 70, default 20%)
	uint_t m_uVolume;					///< Burgerlib volume for the song (0-255)
	uint_t m_uMasterSpeed;			///< Current music speed, 80 = normal
	uint_t m_uMasterPitch;			///< Current music pitch, 80 = normal
	uint_t m_uCurrentSpeed;			///< Current speed
	uint_t m_uFineSpeed;				///< Current fine speed
	uint_t m_uPatternPosition;		///< Current position in pattern
	uint_t m_uCurrentIDPattern;		///< Current ID Pattern
	uint_t m_uPartitionPosition;		///< Current position in partition
	uint_t m_uSpeedCounter;			///< Current mark for song speed
	uint8_t m_bSurroundEnabled;		///< Surround effect active if \ref TRUE
	uint8_t m_bReverbEnabled;			///< Reverb effect active if \ref TRUE
	uint8_t m_bTickRemoverEnabled;	///< Remove volume/sample/loop ticks.
	uint8_t m_bRepeatEnabled;			///< If music finished, repeat if \ref TRUE
	uint8_t m_bSongIsPlaying;			///< \ref TRUE if song is in progress
	uint8_t m_bSequencingInProgress;	///< \ref TRUE if song is being sequenced
	uint8_t m_bPaused;				///< \ref TRUE if song is paused
	ImportProc m_pImporters[cMaxPlugIns];	///< Pointers to the active song importers
	Channel_t m_Channels[cTrackMaxCount];	///< Current note channels
	uint_t BURGER_API CreateReverbTables(void);
	void BURGER_API DisposeReverbTables(void);
	uint_t BURGER_API CreateDriverBuffer(void);
	void BURGER_API DisposeDriverBuffer(void);
	uint_t BURGER_API CreateVolumeBuffer(void);
	void BURGER_API DisposeVolumeBuffer(void);
	void BURGER_API ProcessTick(Channel_t *pChannel,int32_t *pLeft,int32_t *pRight);
	void BURGER_API DetermineSpeed(void);
	void BURGER_API PrepareSequencer(void);
	void BURGER_API ClearSequencer(void);
	void BURGER_API SetChannelCount(void);
	void BURGER_API Sample8To32AddDelay(Channel_t *pChannel,int32_t *pAccumBuffer);
	void BURGER_API Sample16To32AddDelay(Channel_t *pChannel,int32_t *pAccumBuffer);
	void BURGER_API Sample8To16AddDelay(Channel_t *pChannel,int16_t *pAccumBuffer);
	void BURGER_API Sample16To16AddDelay(Channel_t *pChannel,int16_t *pAccumBuffer);
	void BURGER_API Sample8To32AddDelayStereo(Channel_t *pChannel,int32_t *pAccumBuffer);
	void BURGER_API Sample16To32AddDelayStereo(Channel_t *pChannel,int32_t *pAccumBuffer);
	void BURGER_API Sample8To16AddDelayStereo(Channel_t *pChannel,int16_t *pAccumBuffer);
	void BURGER_API Sample16To16AddDelayStereo(Channel_t *pChannel,int16_t *pAccumBuffer);
	void BURGER_API MixTo32(void);
	void BURGER_API MixTo16(void);
	void BURGER_API GenerateSound(void);
	void BURGER_API ApplySurround(void);
	void BURGER_API PurgeChannels(void);
	void BURGER_API SetUpEffect(Channel_t *pChannel);
	void BURGER_API DoEffect(Channel_t *pChannel,uint_t uStep);
	void BURGER_API ProcessFadeOut(Channel_t *pChannel) const;
	void BURGER_API ProcessEnvelope(Channel_t *pChannel) const;
	void BURGER_API ProcessPanning(Channel_t *pChannel) const;
	void BURGER_API ProcessNote(Channel_t *pChannel,const Command_t *pCommand);
	void BURGER_API PerformSequencing(void);
	uint_t BURGER_API PlatformInit(void);
	void BURGER_API PlatformShutdown(void);
public:
	Sequencer(SoundManager *pSoundManager);
	~Sequencer();
	uint_t BURGER_API Init(void);
	void BURGER_API Shutdown(void);
	uint_t BURGER_API AddImporter(ImportProc pImportProc);
	uint_t BURGER_API ImportSong(SongPackage **ppOutput,const uint8_t *pInput,uintptr_t uInputLength);
	BURGER_INLINE uint_t GetVolume(void) const { return m_uVolume; }
	void BURGER_API SetVolume(uint_t uVolume);
	void BURGER_API ClearChannels(void);
	uint_t BURGER_API CalculateVolume(const Channel_t *pChannel,uint_t uSpeaker) const;
	uint_t BURGER_API SetSong(SongPackage *pSongPackage);
	uint_t BURGER_API Play(SongPackage *pSongPackage);
	uint_t BURGER_API Play(const void *pInput,uintptr_t uInputLength);
	uint_t BURGER_API Play(const char *pFilename);
	uint_t BURGER_API PlayRez(RezFile *pRezFile,uint_t uRezNum);
	uint_t BURGER_API StartSequencing(void);
	uint_t BURGER_API StopSequencing(void);
	void BURGER_API DisposeSong(void);
	void BURGER_API Reset(void);
	void BURGER_API Pause(void);
	void BURGER_API Resume(void);

	BURGER_INLINE uint_t GetMaxVoices(void) const { return m_uMaxVoices; }
	void BURGER_API SetMaxVoices(uint_t uMaxVoices);

	BURGER_INLINE SoundManager::eDataType GetOutputDataType(void) const { return m_eOutputDataType; }
	void BURGER_API SetOutputDataType(SoundManager::eDataType uOutputDataType);

	BURGER_INLINE uint_t GetSampleRate(void) const { return m_uFinalSampleRate; }
	void BURGER_API SetSampleRate(uint_t uSampleRate);

	BURGER_INLINE uint_t GetMicroDelayDuration(void) const { return m_uMicroDelayDuration; }
	void BURGER_API SetMicroDelayDuration(uint_t uMicroDelayDuration);

	BURGER_INLINE uint_t GetReverbSize(void) const { return m_uReverbSizeMS; }
	BURGER_INLINE uint_t GetReverbStrength(void) const { return m_uReverbStrength; }
	void BURGER_API SetReverb(uint_t uReverbSizeMS,uint_t uReverbStrength);

	BURGER_INLINE uint_t IsPlaying(void) const { return m_bSongIsPlaying; }
	BURGER_INLINE uint_t IsLooping(void) const { return m_bRepeatEnabled; }
	BURGER_INLINE void EnableLooping(void) { m_bRepeatEnabled = TRUE; }
	BURGER_INLINE void DisableLooping(void) { m_bRepeatEnabled = FALSE; }
	BURGER_INLINE void SetSoundManager(SoundManager *pSoundManager) { m_pSoundManager = pSoundManager; }
	BURGER_INLINE SoundManager *GetSoundManager(void) const { return m_pSoundManager; }
	static eNote BURGER_API AddNoteSaturate(eNote uNote,int32_t iOffset);
	static uint_t BURGER_API GetNotePeriod(eNote uNote,uint_t uC2Speed);
};
}
/* END */

#endif
