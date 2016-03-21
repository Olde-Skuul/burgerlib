/***************************************

	IT file importer

	Format is found at http://schismtracker.org/wiki/ITTECH.TXT

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brimportit.h"
#include "brendian.h"

//
// All data is in little endian format
//

#if !defined(DOXYGEN)

struct ITHeader_t {
#if defined(BURGER_BIGENDIAN)
	static const Word32 cSignature=0x494D504D;	///< 'IMPM'
#else
	static const Word32 cSignature=0x4D504D49;	///< 'IMPM'
#endif
	enum eFlags {
		FLAG_STEREO = 0x01,			// On = Stereo, Off = Mono
		FLAG_VOL0MIX = 0x02,		// If on, no mixing occurs if the volume at mixing time is 0
		FLAG_USEINSTRUMENTS = 0x04,	// On = Use instruments, Off = Use samples.
		FLAG_LINEARSLIDES = 0x08,	// On = Linear slides, Off = Amiga slides.
		FLAG_OLDEFFECTS = 0x10,		// On = Old Effects, Off = IT Effects
		FLAG_LINKEFFECTS = 0x20,	// On = Link Effect G's memory with Effect E/F
		FLAG_USEMIDIPITCH = 0x40,	// Use MIDI pitch controller, Pitch depth given by PWD.
		FLAG_REQUESTMIDI = 0x80		// Request embedded MIDI configuration
	};
	enum eSpecialFlags {
		SPECIAL_SONGMESSAGE = 0x01,	// On = song message attached.
		SPECIAL_MIDICONFIG = 0x08	// MIDI configuration embedded
	};

	Word32 m_Signature;			///< 'IMPM'
	char m_Name[26];			///< Song name
	Word16 m_uPatternHilight;	///< Palette/Row highlight info (For editing)
	Word16 m_uOrderCount;		///< Number of orders in the song
	Word16 m_uInstrumentCount;	///< Number of instruments in the song
	Word16 m_uSampleCount;		///< Number of samples in the song
	Word16 m_uPatternCount;		///< Number of patterns in the song
	Word16 m_uTrackerVersion;	///< Tracker version
	Word16 m_uFileVersion;		///< File version
	Word16 m_uFlags;			///< Flags
	Word16 m_uSpecialFlags;		///< Special flags
	Word8 m_bGlobalVolume;		///< Global volume. (0->128) All volumes are adjusted by this
	Word8 m_bMixVolume;			///< Mix volume (0->128) During mixing, this value controls the magnitude of the wave being mixed.
	Word8 m_bInitialSpeed;		///< Initial Speed of song.
	Word8 m_bInitialTempo;		///< Initial Tempo of song
	Word8 m_bPanSeparation;		///< Panning separation between channels (0->128, 128 is max sep.)
	Word8 m_bPitchWheelDepth;	///< Pitch wheel depth for MIDI controllers
	Word16 m_uMessageLength;	///< Size of internal message
	Word32 m_uMessageOffset;	///< Where is the message
	Word32 Reserved;			///< Not used
	Word8 m_bChannelPans[64];	///< Pan data
	Word8 m_bChannelVolumes[64];	///< Volume data
};

struct ITPatternFormat_t {
	Word16 m_uLength;		///< Length of the data
	Word16 m_uRows;			///< Number of rows
	Word8 Reserved[4];		///< Not used
	Word8 m_Data[1];			///< Start of the pattern data
};

struct ITNode_t {
	Word8 m_bY;			///< Y for node
	Word8 m_bTickLow;	///< Low byte tick value
	Word8 m_bTickHi;	///< High byte tick value
};

struct ITEnvelopeLayout_t {
	enum eFlags {
		FLAG_ENVELOPE = 0x01,		///< Envelope on/off, 1 = on, 0 = off
		FLAG_LOOP = 0x02,			///< Loop on/off, 1 = on, 0 = off
		FLAG_SUSTAINLOOP = 0x04,	///< SusLoop on/off, 1 = on, 0 = off
		FLAG_USEASFILTER = 0x80		///< Use pitch envelope as filter envelope instead.
	};
	Word8 m_bFlags;				/// See above
	Word8 m_bNodeCount;			/// Number of node points
	Word8 m_bLoopBeginning;		/// Loop beginning
	Word8 m_bLoopEnding;		/// Loop end
	Word8 m_bSustainBeginning;	/// Sustain loop beginning
	Word8 m_bSustainEnding;		/// Sustain loop end
	ITNode_t m_Nodes[25];		/// Array of points
};

struct ITKeyMap_t {
	Word8 m_bNote;		///< Note to play
	Word8 m_bSample;	///< Sample to use for the note
};

struct ITSampleFormat_t {
#if defined(BURGER_BIGENDIAN)
	static const Word32 cSignature=0x494D5053;	///< 'IMPS'
#else
	static const Word32 cSignature=0x53504D49;	///< 'IMPS'
#endif
	enum eFlags {
		FLAG_SAMPLE = 0x01,			/// On = sample associated with header
		FLAG_16BIT = 0x02,			/// On = 16 bit, Off = 8 bit.
		FLAG_STEREO = 0x04,			/// On = stereo, Off = mono. Stereo samples not supported yet
		FLAG_COMPRESSED = 0x08,		/// compressed samples.
		FLAG_LOOP = 0x10,			/// Use loop
		FLAG_SUSTAIN = 0x20,		/// Use sustain loop
		FLAG_PINGPONG = 0x40,		/// Ping Pong loop, Off = Forwards loop
		FLAG_PINGPONGSUSTAIN = 0x80	/// On = Ping Pong Sustain loop, Off = Forwards Sustain loop
	};
	enum eConvertFlags {
		CONVERT_SIGNED = 0x01,		/// Signed sampled (Not unsigned)
		CONVERT_BIGENDIAN = 0x02,	/// Big endian instead of little endian
		CONVERT_DELTA = 0x04,		/// Delta values instead of PCM
		CONVERT_BYTEDELTA = 0x08,	/// Byte delta for PTM loader
		CONVERT_TXWAVE = 0x10,		/// 12 bit TX-Wave data
		CONVERT_LEFTRIGHTSTEREO = 0x20	/// Prompt for Left/Right/All stereo
	};
	Word32 m_Signature;			///< 'IMPS'
	char m_DOSName[12];			///< Dos filename
	Word8 Reserved1;			///< Zero (To end the filename)
	Word8 m_bGlobalVolume;		///< Global volume for the instrument
	Word8 m_bFlags;				///< Flags
	Word8 m_bDefaultVolume;		///< Starting volume
	char m_Name[26];			///< Name of the sample
	Word8 m_bConversionFlags;	///< Flags for conversion
	Word8 m_bDefaultPan;		///< Starting Pan
	Word32 m_uSampleCount;		///< Size of the sample in samples, not bytes
	Word32 m_uLoopBegin;		///< Start of loop (no of samples in, not bytes)
	Word32 m_uLoopEnd;			///< Sample no. AFTER end of loop
	Word32 m_uC5Speed;			///< Number of bytes a second for C-5 (ranges from 0->9999999)
	Word32 m_uSustainLoopBegin;	///< Start of sustain loop
	Word32 m_uSustainLoopEnd;	///< Sample no. AFTER end of sustain loop
	Word32 m_uSampleOffset;		///< Offset of sample in file.
	Word8 m_bVibratoSpeed;		///< Vibrato Speed, ranges from 0->64
	Word8 m_bVibratoDepth;		///< Vibrato Depth, ranges from 0->64
	Word8 m_bVibratoRate;		///< Vibrato Rate, rate at which vibrato is applied (0->64)
	Word8 m_bVibratoWaveform;	///< 0=Sine wave, 1=Ramp down, 2=Square wave, 3=Random 
};

struct ITInstrumentFormat_t {
#if defined(BURGER_BIGENDIAN)
	static const Word32 cSignature=0x494D5049;	///< 'IMPI'
#else
	static const Word32 cSignature=0x49504D49;	///< 'IMPI'
#endif
	Word32 m_Signature;			///< 'IMPI'
	char m_DOSName[12];			///< Dos filename
	Word8 Reserved1;			///< Zero (To end the filename)
	Word8 m_bNewNoteAction;		///< 0 = Cut, 1 = Continue, 2 = Note off, 3 = Note fade
	Word8 m_bDuplicateCheckType;	///< 0 = Off, 1 = Note, 2 = Sample, 3 = Instrument
	Word8 m_bDuplicateCheckAction;	///< 0 = Cut, 1 = Note off, 2 = Note of
	Word16 m_uFadeOut;			///< Ranges between 0 and 128, but the fadeout "Count" is 1024
	Int8 m_bPitchPanSeperation;	///< Range -32 -> +32
	Word8 m_bPitchPanCenter;	///< C-0 to B-9 represented as 0->119 inclusive
	Word8 m_bGlobalVolume;		///< 0->128
	Word8 m_bDefaultPan;		///< 0->64, 0x80 => Don't use
	Word8 m_bRandomVolume;		///< Percentage
	Word8 m_bRandomPanning;		///< Percentage
	Word16 m_uTrackerVersion;	///< Tracker version used to save the instrument
	Word8 m_bSampleCount;		///< Number of samples associated with instrument
	Word8 Reserved2;
	char m_Name[26];			///< Instrument name
	Word8 m_bInitialFilterCutoff;	///< Starting filter cutoff
	Word8 m_bInitialFilterResonance;	///< Starting filter resonance
	Word8 m_bMIDIChannel;		///< Which midi channel
	Word8 m_bMIDIProgram;		///< Which midi program
	Word16 m_uMIDIBank;			///< MIDI instrument bank
	ITKeyMap_t m_KeyMaps[120];	///< Sample/keyboard table

	/// Envelopes for ADSR
	ITEnvelopeLayout_t m_VolumeEnvelope;
	ITEnvelopeLayout_t m_PanEnvelope;
	ITEnvelopeLayout_t m_PitchEnvelope;
};

struct ITOldInsForm	{
	Word32 m_Signature;			///< 'IMPI'
	char m_DOSName[12];			///< Dos filename
	Word8 Reserved1;			///< Zero (To end the filename)
	Word8 m_bNewNoteAction;		///< 0 = Cut, 1 = Continue, 2 = Note off, 3 = Note fade
	Word8 m_bDuplicateCheckType;	///< 0 = Off, 1 = Note, 2 = Sample, 3 = Instrument
	Word8 m_bDuplicateCheckAction;	///< 0 = Cut, 1 = Note off, 2 = Note of
	Word16 m_uFadeOut;			///< Ranges between 0 and 128, but the fadeout "Count" is 1024
	Int8 m_bPitchPanSeperation;	///< Range -32 -> +32
	Word8 m_bPitchPanCenter;	///< C-0 to B-9 represented as 0->119 inclusive
	Word8 m_bGlobalVolume;		///< 0->128
	Word8 m_bDefaultPan;		///< 0->64, 0x80 => Don't use
	Word8 m_bRandomVolume;		///< Percentage
	Word8 m_bRandomPanning;		///< Percentage
	Word16 m_uTrackerVersion;	///< Tracker version used to save the instrument
	Word8 m_bSampleCount;		///< Number of samples associated with instrument
	Word8 Reserved2;
	char m_Name[26];			///< Instrument name
	Word8 m_bInitialFilterCutoff;	///< Starting filter cutoff
	Word8 m_bInitialFilterResonance;	///< Starting filter resonance
	Word8 m_bMIDIChannel;		///< Which midi channel
	Word8 m_bMIDIProgram;		///< Which midi program
	Word16 m_uMIDIBank;			///< MIDI instrument bank
	ITKeyMap_t m_KeyMaps[120];	///< Sample/keyboard table

	ITKeyMap_t m_VolumeEnvelopes[100];
};

#endif

/*! ************************************

	\brief Convert an IT effect command to a Burgerlib one

	Given an IT effect command and its argument, translate
	them into a Burgerlib command and argument

	\param pOutput Pointer to the command to receive the translated values
	\param uITCommand IT format effect command
	\param uITArgument IT format effect argument
	\sa ImportIT(Sequencer::SongPackage *,const Word8 *,WordPtr)

***************************************/

void BURGER_API Burger::ImportITEffect(Sequencer::Command_t *pOutput,Word uITCommand,Word uITArgument)
{
	Word uITArgument0F = uITArgument&0xFU;
	Word uITArgumentF0 = uITArgument>>4U;
	Sequencer::Command_t::eEffect uEffectCommand = Sequencer::Command_t::EFFECT_NONE;
	Word uEffectArgument = 0;
	
	// Parse out the command (Converted to ASCII)
	switch (uITCommand + 0x40) {
	default:
		break;
	// Speed
	case 'A':
		uEffectCommand = Sequencer::Command_t::EFFECT_SPEED;
		uEffectArgument = uITArgument;
		break;
	// Tempo
	case 'T':
		uEffectCommand = Sequencer::Command_t::EFFECT_SPEED;
		uEffectArgument = uITArgument;
		break;

	case 'B':
		uEffectCommand = Sequencer::Command_t::EFFECT_FASTSKIP;
		uEffectArgument = uITArgument;
		break;

	case 'C':
		uEffectCommand = Sequencer::Command_t::EFFECT_SKIP;
		uEffectArgument = uITArgument;
		break;

	case 'D':
		if (!uITArgument0F || !uITArgumentF0) {	// Slide volume
			uEffectCommand = Sequencer::Command_t::EFFECT_SLIDEVOLUME;
			uEffectArgument = uITArgument;
		} else if (uITArgumentF0 == 0x0F) {		// Fine Slide volume DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = (uITArgument0F+(11<<4));
		} else if (uITArgument0F == 0x0F) {		// Fine Slide volume UP
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = (uITArgumentF0+(10<<4));
		}
		break;

	case 'E':
		if (uITArgumentF0 == 0x0F) {		// FineSlide DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(2<<4);
		} else if( uITArgumentF0 == 0x0E) {	// ExtraFineSlide DOWN
			//not supported
		} else {					// Slide DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_UPSLIDE;
			uEffectArgument = uITArgument;
		}
		break;

	case 'F':
		if (uITArgumentF0 == 0x0F) {		// FineSlide UP
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(1<<4);
		} else if (uITArgumentF0 == 0x0E) {	// ExtraFineSlide UP
			//not supported
		} else {					// Slide UP
			uEffectCommand = Sequencer::Command_t::EFFECT_DOWNSLIDE;
			uEffectArgument = uITArgument;
		}
		break;

	case 'G':
		uEffectCommand = Sequencer::Command_t::EFFECT_PORTAMENTO;
		uEffectArgument = uITArgument;
		break;
	case 'H':
		uEffectCommand = Sequencer::Command_t::EFFECT_VIBRATO;
		uEffectArgument = uITArgument;
		break;
	case 'J':
		uEffectCommand = Sequencer::Command_t::EFFECT_ARPEGGIO;
		uEffectArgument = uITArgument;
		break;
	case 'K':
		uEffectCommand = Sequencer::Command_t::EFFECT_VIBRATOSLIDE;
		uEffectArgument = uITArgument;
		break;
	case 'L':
		uEffectCommand = Sequencer::Command_t::EFFECT_PORTASLIDE;
		uEffectArgument = uITArgument;
		break;
	case 'O':
		uEffectCommand = Sequencer::Command_t::EFFECT_OFFSET;	
		uEffectArgument = uITArgument;
		break;

	case 'S':		// Special Effects
		switch (uITArgumentF0) {
		default:
			break;
		case 2:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(5<<4);
			break;	// FineTune
		case 3:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(4<<4);
			break;	// Set Vibrato WaveForm
		case 4:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(7<<4);
			break;	// Set Tremolo WaveForm
		case 0xB:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(6<<4);
			break;	// Loop pattern
		case 0xC:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(12<<4);
			break;	// Cut sample
		case 0xD:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(13<<4);
			break;	// Delay sample
		case 0xE:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uITArgument0F+(14<<4);
			break;	// Delay pattern
		}
		break;
	}
	// Save off the effect
	pOutput->SetEffect(uEffectCommand);
	pOutput->m_uEffectArgument = static_cast<Word8>(uEffectArgument);
}

/*! ************************************

	\brief Import an IT file

	Convert an IT music file into a Burgerlib SongPackage

	\param pOutput Pointer to the SongPackage to fill in with music data
	\param pInput Pointer to the IT file
	\param uInputLength Length of the input data
	\return Zero if no error, non-zero if error
	\sa ImportITEffect(Sequencer::Command_t *,Word,Word)

***************************************/

Word BURGER_API Burger::ImportIT(Sequencer::SongPackage *pOutput,const Word8 *pInput,WordPtr uInputLength)
{
	Word uResult = Sequencer::IMPORT_UNKNOWN;
	const ITHeader_t *pITHeader = static_cast<const ITHeader_t*>(static_cast<const void *>(pInput));
	if ((uInputLength>=192) && 
		(pITHeader->m_Signature == ITHeader_t::cSignature)) {

		// Assume data starvation error
		uResult = Sequencer::IMPORT_TRUNCATION;

		// Let's attempt the conversion by consuming the header
		const Word8 *pWork = pInput+192;
		uInputLength-=192;

		// Set up the pointer to the orders

		Word uOrderCount = LittleEndian::Load(&pITHeader->m_uOrderCount);
		if (uInputLength>=uOrderCount) {

			// Mark the data and consume it
			const Word8 *pOrders = pWork;
			pWork += uOrderCount;
			uInputLength-=uOrderCount;

			// Instruments (Not 32 bit aligned)
			Word uInstrumentCount = LittleEndian::Load(&pITHeader->m_uInstrumentCount);
			if (uInputLength>=(uInstrumentCount*4)) {

				// Mark the instrument sizes (May not be 32 bit aligned!!)
				const Word32 *pInstrumentOffsets = static_cast<const Word32 *>(static_cast<const void *>(pWork));
				pWork += uInstrumentCount * 4;
				uInputLength -= uInstrumentCount * 4;

				// Samples (Not 32 bit aligned)
				Word uSampleCount = LittleEndian::Load(&pITHeader->m_uSampleCount);
				if (uInputLength>=(uSampleCount*4)) {

					// Mark the sample sizes (May not be 32 bit aligned!!)
					const Word32 *pSampleOffsets = static_cast<const Word32 *>(static_cast<const void *>(pWork));
					pWork += uSampleCount * 4;
					uInputLength -= uSampleCount * 4;

					// Pointers to pattern
					Word uPatternCount = LittleEndian::Load(&pITHeader->m_uPatternCount);
					if (uInputLength>=(uPatternCount*4)) {
						// Mark the pattern offsets
						const Word32 *pPatternOffsets = static_cast<const Word32 *>(static_cast<const void *>(pWork));
						//pWork += uPatternCount*4;
						//uInputLength -= uPatternCount*4;

						// Begin the conversion

						uResult = Sequencer::IMPORT_OKAY;
						pOutput->Shutdown();
						pOutput->m_SongDescription.SetName(pITHeader->m_Name);
						pOutput->m_SongDescription.m_uPatternCount = uPatternCount;
						pOutput->m_SongDescription.m_uPointerCount = uOrderCount;
						pOutput->m_SongDescription.m_uDefaultSpeed = pITHeader->m_bInitialSpeed;
						pOutput->m_SongDescription.m_uDefaultTempo = pITHeader->m_bInitialTempo;
						pOutput->m_SongDescription.m_uMasterVolume = 64;
						pOutput->m_SongDescription.m_uMasterSpeed = 80;
						pOutput->m_SongDescription.m_uMasterPitch = 80;
						pOutput->m_SongDescription.m_uInstrumentCount = uInstrumentCount;
						pOutput->m_SongDescription.m_uSampleCount = uSampleCount;

						//
						// Create the pattern pointers
						//

						Word i;
						if (uOrderCount) {
							i = 0;
							do {
								Word uOrder = pOrders[i];
								if (uOrder >= uPatternCount) {
									uOrder = 0;
								}
								pOutput->m_SongDescription.m_PatternPointers[i] = uOrder;
							} while (++i<uOrderCount);
						}

						//
						// Initialize the pans and volumes
						//

						i = 0;
						do {
							pOutput->m_SongDescription.m_ChannelPans[i] = pITHeader->m_bChannelPans[i];
							pOutput->m_SongDescription.m_ChannelVolumes[i] = pITHeader->m_bChannelVolumes[i];
						} while (++i<64);

						// i = 64;
						do {
							pOutput->m_SongDescription.m_ChannelPans[i] = Sequencer::cMaxPan/4 + ((i&1)*(Sequencer::cMaxPan/2));
							pOutput->m_SongDescription.m_ChannelVolumes[i] = Sequencer::cMaxVolume;
						} while (++i<Sequencer::cTrackMaxCount);

						i = 0;
						do {
							pOutput->m_InstrDatas[i].m_uBaseSampleID = (i * Sequencer::cSampleMaxCount);
						} while (++i<Sequencer::cInstrumentMaxCount);

						//
						// Use the instrument records or sample record
						//

						Word uHeaderFlags = LittleEndian::Load(&pITHeader->m_uFlags);
						if (uHeaderFlags & ITHeader_t::FLAG_USEINSTRUMENTS) {
							if (uInstrumentCount) {
								i = 0;
								Sequencer::InstrData_t *pInstrData = pOutput->m_InstrDatas;
								do {
									const ITInstrumentFormat_t *pInstrumentFormat = static_cast<const ITInstrumentFormat_t*>(static_cast<const void *>(pInput+LittleEndian::LoadAny(&pInstrumentOffsets[i])));
									if (pInstrumentFormat->m_Signature != ITInstrumentFormat_t::cSignature) {
										uResult = Sequencer::IMPORT_BADFILE;
										break;
									}

									// Instrument conversion

									pInstrData->m_uNumberSamples = 0;
									pInstrData->SetName(pInstrumentFormat->m_Name);

									MemoryClear(pInstrData->m_WhichSampleForNote,sizeof(pInstrData->m_WhichSampleForNote));

									Word uMinSamp = 200;
									Word j = 0;
									do {
										Word uNewSample = pInstrumentFormat->m_KeyMaps[j].m_bSample;
										if (uNewSample) {
											if (uNewSample>uSampleCount) {
												uNewSample = uSampleCount;
											}
											if (uNewSample < (uMinSamp+1)) {
												uMinSamp = uNewSample-1;
											}
										}
									} while (++j<120);

									j = 0;
									do {
										Word uNewSample = pInstrumentFormat->m_KeyMaps[j].m_bSample;
										if (uNewSample) {
											if (uNewSample>uSampleCount) {
												uNewSample = uSampleCount;
											}
											if (pInstrumentFormat->m_KeyMaps[j].m_bNote < 96) {
												pInstrData->m_WhichSampleForNote[pInstrumentFormat->m_KeyMaps[j].m_bNote] = static_cast<Word8>((uNewSample-1) - uMinSamp);
											}
										}
									} while (++j<120);

									// Samples conversion

									int prevSamp = -1;
									Word uMapIndex = 0;
									do {
										if (pInstrumentFormat->m_KeyMaps[uMapIndex].m_bNote<96) {
											int newsamp = pInstrumentFormat->m_KeyMaps[uMapIndex].m_bSample;
											if (newsamp) {
												--newsamp;
												if (static_cast<Word>(newsamp)>=uSampleCount) {
													newsamp = static_cast<int>(uSampleCount-1);
												}
												if (prevSamp != newsamp) {
													prevSamp = newsamp;
													Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::New();
													if (!pSampleDescription) {
														// Uh oh...
														uResult = Sequencer::IMPORT_OUTOFMEMORY;
														break;
													}
													pOutput->m_pSampleDescriptions[i*Sequencer::cSampleMaxCount + pInstrData->m_uNumberSamples] = pSampleDescription;

													const ITSampleFormat_t *pSampleFormat = static_cast<const ITSampleFormat_t*>(static_cast<const void *>(pInput+LittleEndian::LoadAny(&pSampleOffsets[prevSamp])));

													pSampleDescription->m_uSampleSize = LittleEndian::LoadAny(&pSampleFormat->m_uSampleCount);
													if (pSampleFormat->m_bFlags&ITSampleFormat_t::FLAG_LOOP) {
														pSampleDescription->m_uLoopStart = LittleEndian::LoadAny(&pSampleFormat->m_uLoopBegin);
														pSampleDescription->m_uLoopLength = LittleEndian::LoadAny(&pSampleFormat->m_uLoopEnd) - pSampleDescription->m_uLoopStart;
													} else {
														pSampleDescription->m_uLoopStart = 0;
														pSampleDescription->m_uLoopLength = 0;
													}

													pSampleDescription->m_uVolume = pSampleFormat->m_bGlobalVolume;
													pSampleDescription->m_uC2SamplesPerSecond = LittleEndian::LoadAny(&pSampleFormat->m_uC5Speed);
													pSampleDescription->m_eLoopType	= Sequencer::LOOP_NORMAL;
													pSampleDescription->m_uBitsPerSample = 8;

													if (pSampleFormat->m_bFlags&ITSampleFormat_t::FLAG_16BIT) {
														pSampleDescription->m_uBitsPerSample = 16;
														pSampleDescription->m_uSampleSize *= 2;
														pSampleDescription->m_uLoopStart *= 2;
														pSampleDescription->m_uLoopLength *= 2;
													}

													pSampleDescription->m_iRelativeNote	= -12;
													pSampleDescription->SetName(pSampleFormat->m_Name);
													if (pSampleDescription->m_uSampleSize) {
														const void *pDigital = pInput+LittleEndian::LoadAny(&pSampleFormat->m_uSampleOffset);
														pSampleDescription->m_pSample = AllocCopy(pDigital,pSampleDescription->m_uSampleSize);
														if (!pSampleDescription->m_pSample) {
															// Uh oh...
															uResult = Sequencer::IMPORT_OUTOFMEMORY;
															break;
														}
													}
													if (pSampleDescription->m_pSample) {
														if(!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED) &&
															(pSampleDescription->m_uBitsPerSample == 8)) {
															SwapCharsToBytes(pSampleDescription->m_pSample,pSampleDescription->m_uSampleSize);
														}

														if( pSampleDescription->m_uBitsPerSample == 16) {
															WordPtr uLength = pSampleDescription->m_uSampleSize/2;
															if (uLength) {
																if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED)) {
																	Word16 *pSampleTemp = static_cast<Word16 *>(pSampleDescription->m_pSample);
																	do {
																		pSampleTemp[0] = static_cast<Word16>(LittleEndian::Load(pSampleTemp)^0x8000U);
																		++pSampleTemp;
																	} while (--uLength);
																}
#if defined(BURGER_BIGENDIAN)
																else {
																	ConvertEndian(static_cast<Word16 *>(pSampleDescription->m_pSample),uLength);
																}
#endif
															}
														}
													}
													pInstrData->m_uNumberSamples++;
												}
											}
										}
									} while (++uMapIndex<120);
									++pInstrData;
								} while (++i<uInstrumentCount);
							}
						} else {

							// Samples are instruments
							if (uSampleCount) {
								i = 0;
								Sequencer::InstrData_t *pInstrData = pOutput->m_InstrDatas;
								do {
									const ITSampleFormat_t *pSampleFormat = static_cast<const ITSampleFormat_t*>(static_cast<const void *>(pInput+LittleEndian::LoadAny(&pSampleOffsets[i])));
									if (pSampleFormat->m_Signature != ITSampleFormat_t::cSignature) {
										uResult = Sequencer::IMPORT_BADFILE;
										break;
									}
									Word uSampCount = LittleEndian::LoadAny(&pSampleFormat->m_uSampleCount);
									if (uSampCount) {
										const void *pDigitalSample = pInput+LittleEndian::LoadAny(&pSampleFormat->m_uSampleOffset);
										pInstrData->m_uNumberSamples = 1;
										pInstrData->m_uVolumeFadeSpeed = Sequencer::cDefaultVolumeFade;

										Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::New();
										if (!pSampleDescription) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}
										pOutput->m_pSampleDescriptions[i*Sequencer::cSampleMaxCount] = pSampleDescription;

										pSampleDescription->m_uSampleSize = LittleEndian::LoadAny(&pSampleFormat->m_uSampleCount);
										pSampleDescription->m_uLoopStart = LittleEndian::LoadAny(&pSampleFormat->m_uLoopBegin);
										pSampleDescription->m_uLoopLength = LittleEndian::LoadAny(&pSampleFormat->m_uLoopEnd) - pSampleDescription->m_uLoopStart;
										pSampleDescription->m_uVolume = pSampleFormat->m_bGlobalVolume;
										pSampleDescription->m_uC2SamplesPerSecond = LittleEndian::LoadAny(&pSampleFormat->m_uC5Speed);
										pSampleDescription->m_eLoopType	= Sequencer::LOOP_NORMAL;
										pSampleDescription->m_uBitsPerSample = 8;

										if (pSampleFormat->m_bFlags&ITSampleFormat_t::FLAG_16BIT) {
											pSampleDescription->m_uBitsPerSample = 16;
											pSampleDescription->m_uSampleSize *= 2;
											pSampleDescription->m_uLoopStart *= 2;
											pSampleDescription->m_uLoopLength *= 2;
										}

										pSampleDescription->m_iRelativeNote	= -12;
										pInstrData->SetName(pSampleFormat->m_Name);
										pSampleDescription->m_pSample = AllocCopy(pDigitalSample,pSampleDescription->m_uSampleSize);
										if (!pSampleDescription->m_pSample) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}

										if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED) &&
											(pSampleDescription->m_uBitsPerSample==8)) {
											SwapCharsToBytes(pSampleDescription->m_pSample,pSampleDescription->m_uSampleSize);
										}

										if (pSampleDescription->m_uBitsPerSample == 16) {
											WordPtr uLength = pSampleDescription->m_uSampleSize/2;
											if (uLength) {
												if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED)) {
													Word16 *pSampleTemp = static_cast<Word16 *>(pSampleDescription->m_pSample);
													do {
														pSampleTemp[0] = static_cast<Word16>(LittleEndian::Load(pSampleTemp)^0x8000U);
														++pSampleTemp;
													} while (--uLength);
												}
#if defined(BURGER_BIGENDIAN)
												else {
													ConvertEndian(static_cast<Word16 *>(pSampleDescription->m_pSample),uLength);
												}
#endif
											}
										}
									} else {
										pInstrData->m_uNumberSamples = 0;
									}
									++pInstrData;
								} while (++i<uSampleCount);
							}
						}

						//
						// Scan the tracks to determine the number of channels
						//

						Word uChannelCount = 0;

						for (i = 0; i < uPatternCount ; i++) {
							Word uPatternOffset = LittleEndian::LoadAny(&pPatternOffsets[i]);
							if (uPatternOffset) {
								Word8 PreviousVariables[Sequencer::cTrackMaxCount];
								MemoryClear(PreviousVariables,sizeof(PreviousVariables));

								const ITPatternFormat_t *pPatternFormat = static_cast<const ITPatternFormat_t*>(static_cast<const void *>(pInput + uPatternOffset));
								Word uPatternLength = LittleEndian::LoadAny(&pPatternFormat->m_uLength);

								pWork = pPatternFormat->m_Data;
								Word8 bMask = 0;
								Word bNeedChannelToRead = TRUE;

								Word uRow = 0;
								Word uRows = LittleEndian::LoadAny(&pPatternFormat->m_uRows);
								Word uChannelTemp = 0;
								while (uRow < uRows) {
									Word8 uTemp = pWork[0];
									++pWork;
									if (!uTemp) {
										++uRow;
									} else {
										if (bNeedChannelToRead) {
											// Channel
											uChannelTemp = (uTemp-1) & 0x3FU;

											if (uChannelTemp > uChannelCount) {
												uChannelCount = uChannelTemp;
											}
										}

										if (uTemp & 0x80U) {
											bMask = pWork[0];
											++pWork;
											PreviousVariables[uChannelTemp] = bMask;
										} else {
											bMask = PreviousVariables[uChannelTemp];
										}

										// Consume the bytes in the stream
										if (bMask & 1) {
											++pWork;
										}
										if (bMask & 2) {
											++pWork;
										}
										if (bMask & 4) {
											++pWork;
										}
										if (bMask & 8) {
											pWork += 2;
										}
									}
								}

								if (static_cast<WordPtr>(pWork - pPatternFormat->m_Data) != uPatternLength) {
									uResult = Sequencer::IMPORT_BADFILE;
									break;
								}
							}
						}

						uChannelCount = (uChannelCount+2)&(~1);

						pOutput->m_SongDescription.m_uChannelCount = uChannelCount;

						for( i = 0; i < uPatternCount ; i++) {
							
							Word uPatternOffset = LittleEndian::LoadAny(&pPatternOffsets[i]);
							if (!uPatternOffset) {
								
								// No Data for this pattern - Clear Pattern

								pOutput->m_pPartitions[i] = Sequencer::PatternData_t::New(1,uChannelCount);
								if (!pOutput->m_pPartitions[i]) {
									uResult = Sequencer::IMPORT_OUTOFMEMORY;
									break;
								}
								pOutput->m_pPartitions[i]->SetName("Unnamed pattern");
							} else {

								const ITPatternFormat_t *pPatternFormat = static_cast<const ITPatternFormat_t*>(static_cast<const void *>(pInput + uPatternOffset));
								Word uRows = LittleEndian::LoadAny(&pPatternFormat->m_uRows);

								pOutput->m_pPartitions[i] = Sequencer::PatternData_t::New(uRows,uChannelCount);
								if (!pOutput->m_pPartitions[i]) {
									uResult = Sequencer::IMPORT_OUTOFMEMORY;
									break;
								}
								Word8 PreviousVariables[Sequencer::cTrackMaxCount];
								Word8 PreviousNotes[Sequencer::cTrackMaxCount];
								Word8 PreviousInstruments[Sequencer::cTrackMaxCount];
								Word8 PreviousVolumes[Sequencer::cTrackMaxCount];
								Word8 PreviousArguments[Sequencer::cTrackMaxCount];
								Word8 PreviousCommands[Sequencer::cTrackMaxCount];
								MemoryClear(PreviousVariables,sizeof(PreviousVariables));

								Word uPatternLength = LittleEndian::LoadAny(&pPatternFormat->m_uLength);
								pWork = pPatternFormat->m_Data;
								Word8 bMask = 0;
								Word bNeedChannelToRead = TRUE;

								Word uRow = 0;
								Word uTempChannel = 0;
								Sequencer::Command_t *pCommand = NULL;
								while (uRow < uRows) {
									Word8 uTemp = pWork[0];
									++pWork;

									if (!uTemp) {
										++uRow;
									} else {
										if (bNeedChannelToRead) {
											// Channel
											uTempChannel = (uTemp-1) & 0x3FU;
											if (uTempChannel < uChannelCount) {
												pCommand = pOutput->m_pPartitions[i]->GetCommand(static_cast<int>(uRow),static_cast<int>(uTempChannel));
											} else {
												uResult = Sequencer::IMPORT_BADFILE;
												break;
											}
										}

										if (uTemp & 0x80U) {
											bMask = pWork[0];
											++pWork;
											PreviousVariables[uTempChannel] = bMask;
										} else {
											bMask = PreviousVariables[uTempChannel];
										}

										// NOTE
										if ((bMask & 1) || (bMask & 16)) {
											Word8 bNote;

											if (bMask & 1) {
												bNote = pWork[0];
												++pWork;
												PreviousNotes[uTempChannel] = bNote;
											} else {
												bNote = PreviousNotes[uTempChannel];
											}
											if (pCommand) {
												if (bNote == 255) {
													pCommand->m_uNote = 0xFE;
												} else if (bNote >= Sequencer::NOTE_MAX) {
													pCommand->m_uNote = 0xFF;
												} else {
													pCommand->m_uNote = bNote;
												}
											}
										}

										//
										// Instrument extraction
										//
										if ((bMask & 2) || (bMask & 32)) {
											Word8 uInstrumentTemp;

											if (bMask & 2) {
												uInstrumentTemp = pWork[0];
												++pWork;
												PreviousInstruments[uTempChannel] = uInstrumentTemp;
											} else {
												uInstrumentTemp = PreviousInstruments[uTempChannel];
											}
											if (pCommand) {
												pCommand->m_uInstrument = uInstrumentTemp;
											}
										}

										// VOLUME
										if ((bMask & 4) || (bMask & 64)) {
											Word8 bVolume;

											if (bMask & 4) {
												bVolume = pWork[0];
												++pWork;
												PreviousVolumes[uTempChannel] = bVolume;
											} else {
												bVolume = PreviousVolumes[uTempChannel];
											}

											if (pCommand) {
												if (bVolume>64) {
													bVolume=64;
												}
												bVolume+=0x10;
												pCommand->m_uVolume = bVolume;
											}
										} else {
											if (pCommand) {
												pCommand->m_uVolume = 255;
											}
										}
										
										//
										// Effects
										//

										if ((bMask & 8) || (bMask & 128)) {
											Word8 uITCommand;
											Word8 uITArgument;

											if (bMask & 8) {
												uITCommand = pWork[0];
												uITArgument = pWork[1];
												pWork += 2;
												PreviousCommands[uTempChannel] = uITCommand;
												PreviousArguments[uTempChannel] = uITArgument;
											} else {
												uITCommand = PreviousCommands[uTempChannel];
												uITArgument = PreviousArguments[uTempChannel];
											}

											if (pCommand) {
												ImportITEffect(pCommand,uITCommand,uITArgument);
											}
										}
									}
								}

								if (static_cast<WordPtr>(pWork - pPatternFormat->m_Data) != uPatternLength) {
									uResult = Sequencer::IMPORT_BADFILE;
									break;
								}
							}
						}

						// If there was an error, dispose of what was created

						if (uResult) {
							pOutput->Shutdown();
						}
					}
				}
			}
		}
	}
	return uResult;
}
