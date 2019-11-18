/***************************************

	Music Sequencer

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	static const Word cAmigaFrequency = 8363;		///< Default MOD file sample frequency (7159090/856)
	static const Word cInstrumentMaxCount = 255;	///< Maximum number of instruments
	static const Word cSampleMaxCount = 64;			///< Maximum number of samples
	static const Word cPointerMaxCount = 999;		///< Maximum number of track pointers
	static const Word cTrackMaxCount = 256;			///< Maximum number of tracks
	static const Word cPatternMaxCount = 200;		///< Maximum number of patterns
	static const Word cArpeggioMaxCount = 3;		///< Maximum number of arpeggio commands
	static const Word cMaxPlugIns = 16;				///< Maximum number of song format importers
	static const Word cDefaultVolumeFade = 300;		///< Default volume fade speed
	static const Word cAmigaClock = 14317456;		///< Scream Tracker Amiga base clock
	static const Word cFixedPointShift = 8;			///< Number of bits for fixed point math 24.8
	static const Word cMaxVolume = 64;				///< Maximum internal volume for commands
	static const Word cMaxPan = 64;					///< Maximum internal pan for commands
	static const Word cMinimumPitch = 113;			///< Minimum pitch (Samples per second)
	static const Word cMaximumPitch = 27392;		///< Maximum pitch (Samples per second)
	static const Int32 g_VibratoTable[64];			///< Vibrato table

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
		Word8 m_uNote;				///< eNote value
		Word8 m_uInstrument;		///< Instrument attached to the command (0 is no instrument)
		Word8 m_uVolume;			///< Volume command (255 = no command)
		Word8 m_uEffectCommand;		///< eEffect Command enumeration
		Word8 m_uEffectArgument;	///< Argument for the above effect command

		void BURGER_API Clear(void);
		BURGER_INLINE eNote GetNote(void) const { return static_cast<eNote>(m_uNote); }
		BURGER_INLINE eVolumeCommand GetVolume(void) const { return static_cast<eVolumeCommand>(m_uVolume); }
		BURGER_INLINE eEffect GetEffect(void) const { return static_cast<eEffect>(m_uEffectCommand); }
		BURGER_INLINE void SetEffect(eEffect uEffectCommand) { m_uEffectCommand = static_cast<Word8>(uEffectCommand); }
	};

	struct PatternData_t {
		Word32 m_uRowCount;			///< Number of rows in a pattern
		Word32 m_uChannelCount;		///< Number of pattern channels
		char m_Name[32];			///< Name of the sequence pattern
		Command_t m_Commands[1];	///< Array of sequence patterns (m_uPatternSize*m_uChannelCount is the size)
		static PatternData_t * BURGER_API New(Word uRows,Word uChannels);
		Command_t* BURGER_API GetCommand(int iRow,int iChannel);
		BURGER_INLINE void SetName(const char *pName) { StringCopy(m_Name,sizeof(m_Name),pName); }
	};

	class SampleDescription {
	public:
		void *m_pSample;			///< Pointer to the sample data
		Word32 m_uSampleSize;		///< Number of bytes in the sample
		Word32 m_uLoopStart;		///< Byte to start the loop
		Word32 m_uLoopLength;		///< Byte to end the loop
		Word m_uC2SamplesPerSecond;	///< Samples per second to generate C2 (Fine tune adjust)
		Int m_iRelativeNote;		///< Note adjust in semitones
		Word8 m_uVolume;			///< Default volume of the sample
		Word8 m_eLoopType;			///< Default loop behavior
		Word8 m_uBitsPerSample;		///< 8 or 16 bits
		Word8 m_bStereo;			///< Stereo
		char m_Name[32];			///< Sample name
	public:
		~SampleDescription();
		static SampleDescription * BURGER_API New(void);
		BURGER_INLINE void SetName(const char *pName) { StringCopy(m_Name,sizeof(m_Name),pName); }
	};

	struct EnvelopeMarker_t {
		Word16 m_uPosition;	///< Time mark for when this volume is active
		Word16 m_uVolume;	///< Volume at this time mark
		Word BURGER_API Interpolate(const EnvelopeMarker_t *pNext,Word uPosition) const;
	};

	struct InstrData_t {
		enum eEnvelopFlags {
			ENVELOPE_ON			= 1,	///< If set, the envelope is active
			ENVELOPE_SUSTAIN	= 2,	///< If set, the envelope has a sustain section
			ENVELOPE_LOOP		= 4		///< If set, the envelope loops
		};
		Word m_uBaseSampleID;			///< First sample ID in sample list
		Word m_uNumberSamples;			///< Number of samples in instrument
		EnvelopeMarker_t m_VolumeEnvelope[12];	///< Points for volume envelope
		EnvelopeMarker_t m_PanEnvelope[12];		///< Points for panning envelope
		Word8 m_WhichSampleForNote[96];	///< Sample number for all notes

		Word8 m_uVolumeEnvelopeCount;	///< Number of volume points
		Word8 m_uPanEnvelopeCount;		///< Number of panning points

		Word8 m_uVolumeSustainIndex;	///< Volume sustain point
		Word8 m_uVolumeBeginIndex;		///< Volume loop start point
		Word8 m_uVolumeEndIndex;		///< Volume loop end point

		Word8 m_uPanSustainIndex;		///< Panning sustain point
		Word8 m_uPanBeginIndex;			///< Panning loop start point
		Word8 m_uPanEndIndex;			///< Panning loop end point

		Word8 m_uVolumeEnvelopeFlags;	///< Flags for the volume envelope (See eEnvelopFlags)
		Word8 m_uPanEnvelopeFlags;		///< Flags for the pan envelope (See eEnvelopFlags)

		Word16 m_uVolumeFadeSpeed;		///< Volume fadeout speed

		Word8 m_uVibratoDepth;			///< Depth of the vibrato
		Word8 m_uVibratoSpeed;			///< Speed of vibrato oscillation
		Word8 m_uInstrumentNumber;		///< Instrument number
		char m_Name[32];				///< Instrument name
		BURGER_INLINE void SetName(const char *pName) { StringCopy(m_Name,sizeof(m_Name),pName); }
		void BURGER_API Reset(void);
	};

	struct SongDescription_t {
		Word m_uDefaultSpeed;			///< Default song speed
		Word m_uDefaultTempo;			///< Default song tempo
		Word m_uMasterPitch;			///< Master Pitch
		Word m_uMasterSpeed;			///< Master Speed
		Word m_uMasterVolume;			///< Master volume
		Word m_uPatternCount;			///< Number of patterns
		Word m_uChannelCount;			///< Number of channels
		Word m_uPointerCount;			///< Number of pointers in m_PatternPointers
		Word m_uInstrumentCount;		///< Number of instruments
		Word m_uSampleCount;			///< Number of samples
		Word m_PatternPointers[cPointerMaxCount];	///< List of patterns for the song
		Word m_ChannelPans[cTrackMaxCount];		///< Pan settings for each channel
		Word m_ChannelVolumes[cTrackMaxCount];	///< Volume settings for each channel (0-64)
		char m_Name[32];				///< Name of the song
		BURGER_INLINE void Clear(void) { MemoryClear(this,sizeof(*this)); }
		BURGER_INLINE void SetName(const char *pName) { StringCopy(m_Name,sizeof(m_Name),pName); }
	};

	struct Channel_t {
		const char *m_pBeginningOfSample;	///< Beginning of sample data
		const char *m_pEndOfSample;			///< End of the sample data
		const char *m_pCurrent;				///< Current sample being processed
		const char *m_pEchoSample;			///< Sample processed for echo effect
		WordPtr m_uSampleSize;				///< Sample Size in bytes
		Word m_uID;						///< Channel ID - 0 to cTrackMaxCount
		Word m_uBitsPerSample;			///< 8 or 16 bits
		Word m_uLoopBeginning;			///< Loop start position in bytes
		Word m_uLoopSize;				///< Loop size in bytes
		Word m_uCurrentInstrumentID;	///< Current Instrument ID
		Word m_uPreviousInstrumentID;	///< Previous Instrument ID
		Word m_uSampleID;				///< Current Sample ID
		Word m_uAmigaFineTune;			///< Samples per second adjustment
		eNote m_eNote;					///< Musical note number
		Int32 m_iPreviousNote;			///< Previous note number
		Word m_uTimePeriod;			///< Current time period
		Word m_uPreviousTimePeriod;	///< Previous time period
		Word32 m_uVolume;				///< Channel vol (0 to 64)
		Int32 m_iPan;					///< Channel pan (0 to 64)
		Word m_uEffectCommand;			///< See eEffect
		Word m_uEffectArgument;			///< Argument of command
		Word m_uVolumeCommand;			///< Volume Command

		Int32 m_Arpeggios[cArpeggioMaxCount];		///< Array of Arpeggio points
		Word m_uArpeggioIndex;			///< Index to the active arpeggio in the above table

		Int32 m_iVibratoOffset;			///< Base vibrato value
		Int32 m_iVibratoDepth;			///< Range of the vibrato
		Int32 m_iCurrentVibration;		///< Current vibrato value

		Int32 m_iSlide;					///< Adjust value for time period, used for slideUp and slideDown commands

		Int32 m_iPitchGoal;				///< Portamento goal value
		Int32 m_iPitchRate;				///< Speed of Portamento

		Int32 m_iVolumeRate;			// Used for slideVolume command

		Word m_PreviousArguments[16];	///< Arguments for effect commands, Indexed by eEffect

		Word m_uCurrentVolumeEnvelopeIndex;	///< Current index for volume envelopes
		Word m_uNextVolumeEnvelopeIndex;	///< Next index for volume envelopes
		Word m_uCurrentVolumePosition;		///< Current time mark used for interpolation of volume envelopes
		Word m_uCurrentVolumeFromEnvelope;	///< Volume from interpolated volume (0-64)

		Word m_uCurrentPanEnvelopeIndex;	///< Current index for pan envelopes
		Word m_uNextPanEnvelopeIndex;		///< Next index for pan envelopes
		Word m_uCurrentPanPosition;			///< Current time mark used for interpolation of pan envelopes
		Word m_uCurrentPanFromEnvelope;		///< Pan from interpolated pan

		Word32 m_uVolumeFade;				///< Volume fade speed (0-32767)
		Int32 m_iAccumulatorFraction;		///< Accumulator fraction from last pass, 24.8 fixed point

		Int32 m_iLastWordLeft;
		Int32 m_iCurrentLastWordLeft;
		Int32 m_iLastWordRight;
		Int32 m_iCurrentLastWordRight;
		Word m_uCurrentLevelLeft;
		Word m_uCurrentLevelRight;

		Word8 m_bLevelDirectionLeft;
		Word8 m_bLevelDirectionRight;
		Word8 m_bRemoverWorking;
		Word8 m_bKeyOn;

		Word32 m_uPreviousVolumeLeft;
		Word32 m_uPreviousVolumeRight;
		Int32 m_iPreviousOffset;
		Word m_uEffectPartitionIndex;
		Word m_uEffectRowIndex;
		Word8 m_bEffectActive;			///< If \ref TRUE, an audio effect is active
		Word8 m_bStereo;				///< If \ref TRUE, channel is controlling a stereo sample
		Word8 m_eLoopType;				///< See eLoopType
		Word8 m_bPingPongReverse;		///< If \ref TRUE, ping pong is in reverse direction
		Int32 m_iPreviousValue1Left;
		Int32 m_iPreviousValue2Left;
		Int32 m_iPreviousValue1Right;
		Int32 m_iPreviousValue2Right;

		Int32 m_iPreviousValue3Left;
		Int32 m_iPreviousValue4Left;
		Int32 m_iPreviousValue3Right;
		Int32 m_iPreviousValue4Right;
		Word m_uTickRemoveSize;
		void BURGER_API Init(Word uID);
		void BURGER_API Purge(void);
		void BURGER_API VolumeCommand(Word uCall);
		void BURGER_API ParseSlideVolume(Word uArgument);
		void BURGER_API FillBuffers(Int32 *pLeft,Int32 *pRight,WordPtr uCount,Int32 iLeft,Int32 iRight);
		void BURGER_API Tickloop(Int32 *pLeft,Int32 *pRight,WordPtr uCount);
	};

	class SongPackage {
	public:
		PatternData_t *m_pPartitions[cPatternMaxCount];	///< Patterns
		SampleDescription *m_pSampleDescriptions[cInstrumentMaxCount*cSampleMaxCount];	///< Samples
		InstrData_t m_InstrDatas[cInstrumentMaxCount];	///< Instruments
		SongDescription_t m_SongDescription;			///< Description of the entire song
		Word m_bMusicUnderModification;					///< Tell the driver to NOT access music data
		SongPackage();
		~SongPackage();
		void BURGER_API Shutdown(void);
		void BURGER_API RemoveInstrument(Word uInstrumentIndex);
		static SongPackage * BURGER_API New(void);
	};

	typedef Word (BURGER_API *ImportProc)(SongPackage *pOutput,const Word8 *pInput,WordPtr uInputLength);

protected:
	SoundManager *m_pSoundManager;	///< Pointer to the sound manager to send audio data to
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static const Word cMaxEventCount = 2;
	IDirectSoundBuffer8 *m_pDirectSoundBuffer8;	///< DirectSoundBuffer (Windows Only)
	IDirectSoundNotify *m_pDirectSoundNotify;	///< DirectSoundNotify for alert when sound is complete (Windows Only)
	void *m_hCallback;							///< HANDLE of the callback thread (Windows only)
	unsigned long m_uCallbackID;				///< ID of the callback thread (Windows only)
	void *m_hEvents[cMaxEventCount];			///< Events for DirectSound completion functions (Windows Only)
	static unsigned long __stdcall ThreadCallback(void *pThis);
#endif
	SongPackage *m_pSongPackage;	///< Current song being played
	Int32 *m_pAccumBuffer32;		///< 32 bit accumulator buffer (Used for 16 bit samples)
	Int16 *m_pAccumBuffer16;		///< 16 bit accumulator buffer (Used for 8 bit samples)
	void *m_pReverbBuffer;			///< Pointer to the reverb buffer
	WordPtr m_uReverbDelayBufferSize;	///< Length in samples of the reverb buffer
	void *m_pBuffer;				///< Data buffer
	WordPtr m_uBufferSize;			///< Size of the data buffer in bytes
	WordPtr m_uBytesGenerated;		///< Number of bytes generated and uploaded to sound driver
	WordPtr m_uBytesToGenerate;		///< Number of bytes to generate
	WordPtr m_uChunkSize;			///< Number of bytes per time quantum (50Hz)
	WordPtr m_uAccumBufferSize;			///< Size in bytes of the accumulator buffer
	WordPtr m_uMicroDelayBufferSize;	///< Size in bytes of the delay buffer
	Word m_uImporterCount;			///< Number of pointers in m_pImporters
	Word m_uMaxVoices;				///< Active number of track for the currently playing song
	Word m_uMicroDelayDuration;		///< Micro delay duration (min 0 ms, max 1000 ms)
	SoundManager::eDataType m_eOutputDataType;		///< Mix to 8 or 16 bit size
	Word m_uFinalSampleRate;		///< Mix to this many samples per second
	Word m_uReverbSizeMS;			///< Reverb delay duration (min 25 ms, max 1000 ms)
	Word m_uReverbStrength;			///< Reverb strength in % (0 <-> 70, default 20%)
	Word m_uVolume;					///< Burgerlib volume for the song (0-255)
	Word m_uMasterSpeed;			///< Current music speed, 80 = normal
	Word m_uMasterPitch;			///< Current music pitch, 80 = normal
	Word m_uCurrentSpeed;			///< Current speed
	Word m_uFineSpeed;				///< Current fine speed
	Word m_uPatternPosition;		///< Current position in pattern
	Word m_uCurrentIDPattern;		///< Current ID Pattern
	Word m_uPartitionPosition;		///< Current position in partition
	Word m_uSpeedCounter;			///< Current mark for song speed
	Word8 m_bSurroundEnabled;		///< Surround effect active if \ref TRUE
	Word8 m_bReverbEnabled;			///< Reverb effect active if \ref TRUE
	Word8 m_bTickRemoverEnabled;	///< Remove volume/sample/loop ticks.
	Word8 m_bRepeatEnabled;			///< If music finished, repeat if \ref TRUE
	Word8 m_bSongIsPlaying;			///< \ref TRUE if song is in progress
	Word8 m_bSequencingInProgress;	///< \ref TRUE if song is being sequenced
	Word8 m_bPaused;				///< \ref TRUE if song is paused
	ImportProc m_pImporters[cMaxPlugIns];	///< Pointers to the active song importers
	Channel_t m_Channels[cTrackMaxCount];	///< Current note channels
	Word BURGER_API CreateReverbTables(void);
	void BURGER_API DisposeReverbTables(void);
	Word BURGER_API CreateDriverBuffer(void);
	void BURGER_API DisposeDriverBuffer(void);
	Word BURGER_API CreateVolumeBuffer(void);
	void BURGER_API DisposeVolumeBuffer(void);
	void BURGER_API ProcessTick(Channel_t *pChannel,Int32 *pLeft,Int32 *pRight);
	void BURGER_API DetermineSpeed(void);
	void BURGER_API PrepareSequencer(void);
	void BURGER_API ClearSequencer(void);
	void BURGER_API SetChannelCount(void);
	void BURGER_API Sample8To32AddDelay(Channel_t *pChannel,Int32 *pAccumBuffer);
	void BURGER_API Sample16To32AddDelay(Channel_t *pChannel,Int32 *pAccumBuffer);
	void BURGER_API Sample8To16AddDelay(Channel_t *pChannel,Int16 *pAccumBuffer);
	void BURGER_API Sample16To16AddDelay(Channel_t *pChannel,Int16 *pAccumBuffer);
	void BURGER_API Sample8To32AddDelayStereo(Channel_t *pChannel,Int32 *pAccumBuffer);
	void BURGER_API Sample16To32AddDelayStereo(Channel_t *pChannel,Int32 *pAccumBuffer);
	void BURGER_API Sample8To16AddDelayStereo(Channel_t *pChannel,Int16 *pAccumBuffer);
	void BURGER_API Sample16To16AddDelayStereo(Channel_t *pChannel,Int16 *pAccumBuffer);
	void BURGER_API MixTo32(void);
	void BURGER_API MixTo16(void);
	void BURGER_API GenerateSound(void);
	void BURGER_API ApplySurround(void);
	void BURGER_API PurgeChannels(void);
	void BURGER_API SetUpEffect(Channel_t *pChannel);
	void BURGER_API DoEffect(Channel_t *pChannel,Word uStep);
	void BURGER_API ProcessFadeOut(Channel_t *pChannel) const;
	void BURGER_API ProcessEnvelope(Channel_t *pChannel) const;
	void BURGER_API ProcessPanning(Channel_t *pChannel) const;
	void BURGER_API ProcessNote(Channel_t *pChannel,const Command_t *pCommand);
	void BURGER_API PerformSequencing(void);
	Word BURGER_API PlatformInit(void);
	void BURGER_API PlatformShutdown(void);
public:
	Sequencer(SoundManager *pSoundManager);
	~Sequencer();
	Word BURGER_API Init(void);
	void BURGER_API Shutdown(void);
	Word BURGER_API AddImporter(ImportProc pImportProc);
	Word BURGER_API ImportSong(SongPackage **ppOutput,const Word8 *pInput,WordPtr uInputLength);
	BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
	void BURGER_API SetVolume(Word uVolume);
	void BURGER_API ClearChannels(void);
	Word BURGER_API CalculateVolume(const Channel_t *pChannel,Word uSpeaker) const;
	Word BURGER_API SetSong(SongPackage *pSongPackage);
	Word BURGER_API Play(SongPackage *pSongPackage);
	Word BURGER_API Play(const void *pInput,WordPtr uInputLength);
	Word BURGER_API Play(const char *pFilename);
	Word BURGER_API PlayRez(RezFile *pRezFile,Word uRezNum);
	Word BURGER_API StartSequencing(void);
	Word BURGER_API StopSequencing(void);
	void BURGER_API DisposeSong(void);
	void BURGER_API Reset(void);
	void BURGER_API Pause(void);
	void BURGER_API Resume(void);

	BURGER_INLINE Word GetMaxVoices(void) const { return m_uMaxVoices; }
	void BURGER_API SetMaxVoices(Word uMaxVoices);

	BURGER_INLINE SoundManager::eDataType GetOutputDataType(void) const { return m_eOutputDataType; }
	void BURGER_API SetOutputDataType(SoundManager::eDataType uOutputDataType);

	BURGER_INLINE Word GetSampleRate(void) const { return m_uFinalSampleRate; }
	void BURGER_API SetSampleRate(Word uSampleRate);

	BURGER_INLINE Word GetMicroDelayDuration(void) const { return m_uMicroDelayDuration; }
	void BURGER_API SetMicroDelayDuration(Word uMicroDelayDuration);

	BURGER_INLINE Word GetReverbSize(void) const { return m_uReverbSizeMS; }
	BURGER_INLINE Word GetReverbStrength(void) const { return m_uReverbStrength; }
	void BURGER_API SetReverb(Word uReverbSizeMS,Word uReverbStrength);

	BURGER_INLINE Word IsPlaying(void) const { return m_bSongIsPlaying; }
	BURGER_INLINE Word IsLooping(void) const { return m_bRepeatEnabled; }
	BURGER_INLINE void EnableLooping(void) { m_bRepeatEnabled = TRUE; }
	BURGER_INLINE void DisableLooping(void) { m_bRepeatEnabled = FALSE; }
	BURGER_INLINE void SetSoundManager(SoundManager *pSoundManager) { m_pSoundManager = pSoundManager; }
	BURGER_INLINE SoundManager *GetSoundManager(void) const { return m_pSoundManager; }
	static eNote BURGER_API AddNoteSaturate(eNote uNote,Int32 iOffset);
	static Word BURGER_API GetNotePeriod(eNote uNote,Word uC2Speed);
};
}
/* END */

#endif
