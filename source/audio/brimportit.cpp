/***************************************

    IT file importer

    Format is found at http://schismtracker.org/wiki/ITTECH.TXT

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	static const uint32_t cSignature=0x494D504D;	///< 'IMPM'
#else
	static const uint32_t cSignature=0x4D504D49;	///< 'IMPM'
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

	uint32_t m_Signature;			///< 'IMPM'
	char m_Name[26];			///< Song name
	uint16_t m_uPatternHilight;	///< Palette/Row highlight info (For editing)
	uint16_t m_uOrderCount;		///< Number of orders in the song
	uint16_t m_uInstrumentCount;	///< Number of instruments in the song
	uint16_t m_uSampleCount;		///< Number of samples in the song
	uint16_t m_uPatternCount;		///< Number of patterns in the song
	uint16_t m_uTrackerVersion;	///< Tracker version
	uint16_t m_uFileVersion;		///< File version
	uint16_t m_uFlags;			///< Flags
	uint16_t m_uSpecialFlags;		///< Special flags
	uint8_t m_bGlobalVolume;		///< Global volume. (0->128) All volumes are adjusted by this
	uint8_t m_bMixVolume;			///< Mix volume (0->128) During mixing, this value controls the magnitude of the wave being mixed.
	uint8_t m_bInitialSpeed;		///< Initial Speed of song.
	uint8_t m_bInitialTempo;		///< Initial Tempo of song
	uint8_t m_bPanSeparation;		///< Panning separation between channels (0->128, 128 is max sep.)
	uint8_t m_bPitchWheelDepth;	///< Pitch wheel depth for MIDI controllers
	uint16_t m_uMessageLength;	///< Size of internal message
	uint32_t m_uMessageOffset;	///< Where is the message
	uint32_t Reserved;			///< Not used
	uint8_t m_bChannelPans[64];	///< Pan data
	uint8_t m_bChannelVolumes[64];	///< Volume data
};

struct ITPatternFormat_t {
	uint16_t m_uLength;		///< Length of the data
	uint16_t m_uRows;			///< Number of rows
	uint8_t Reserved[4];		///< Not used
	uint8_t m_Data[1];			///< Start of the pattern data
};

struct ITNode_t {
	uint8_t m_bY;			///< Y for node
	uint8_t m_bTickLow;	///< Low byte tick value
	uint8_t m_bTickHi;	///< High byte tick value
};

struct ITEnvelopeLayout_t {
	enum eFlags {
		FLAG_ENVELOPE = 0x01,		///< Envelope on/off, 1 = on, 0 = off
		FLAG_LOOP = 0x02,			///< Loop on/off, 1 = on, 0 = off
		FLAG_SUSTAINLOOP = 0x04,	///< SusLoop on/off, 1 = on, 0 = off
		FLAG_USEASFILTER = 0x80		///< Use pitch envelope as filter envelope instead.
	};
	uint8_t m_bFlags;				/// See above
	uint8_t m_bNodeCount;			/// Number of node points
	uint8_t m_bLoopBeginning;		/// Loop beginning
	uint8_t m_bLoopEnding;		/// Loop end
	uint8_t m_bSustainBeginning;	/// Sustain loop beginning
	uint8_t m_bSustainEnding;		/// Sustain loop end
	ITNode_t m_Nodes[25];		/// Array of points
};

struct ITKeyMap_t {
	uint8_t m_bNote;		///< Note to play
	uint8_t m_bSample;	///< Sample to use for the note
};

struct ITSampleFormat_t {
#if defined(BURGER_BIGENDIAN)
	static const uint32_t cSignature=0x494D5053;	///< 'IMPS'
#else
	static const uint32_t cSignature=0x53504D49;	///< 'IMPS'
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
	uint32_t m_Signature;			///< 'IMPS'
	char m_DOSName[12];			///< Dos filename
	uint8_t Reserved1;			///< Zero (To end the filename)
	uint8_t m_bGlobalVolume;		///< Global volume for the instrument
	uint8_t m_bFlags;				///< Flags
	uint8_t m_bDefaultVolume;		///< Starting volume
	char m_Name[26];			///< Name of the sample
	uint8_t m_bConversionFlags;	///< Flags for conversion
	uint8_t m_bDefaultPan;		///< Starting Pan
	uint32_t m_uSampleCount;		///< Size of the sample in samples, not bytes
	uint32_t m_uLoopBegin;		///< Start of loop (no of samples in, not bytes)
	uint32_t m_uLoopEnd;			///< Sample no. AFTER end of loop
	uint32_t m_uC5Speed;			///< Number of bytes a second for C-5 (ranges from 0->9999999)
	uint32_t m_uSustainLoopBegin;	///< Start of sustain loop
	uint32_t m_uSustainLoopEnd;	///< Sample no. AFTER end of sustain loop
	uint32_t m_uSampleOffset;		///< Offset of sample in file.
	uint8_t m_bVibratoSpeed;		///< Vibrato Speed, ranges from 0->64
	uint8_t m_bVibratoDepth;		///< Vibrato Depth, ranges from 0->64
	uint8_t m_bVibratoRate;		///< Vibrato Rate, rate at which vibrato is applied (0->64)
	uint8_t m_bVibratoWaveform;	///< 0=Sine wave, 1=Ramp down, 2=Square wave, 3=Random 
};

struct ITInstrumentFormat_t {
#if defined(BURGER_BIGENDIAN)
	static const uint32_t cSignature=0x494D5049;	///< 'IMPI'
#else
	static const uint32_t cSignature=0x49504D49;	///< 'IMPI'
#endif
	uint32_t m_Signature;			///< 'IMPI'
	char m_DOSName[12];			///< Dos filename
	uint8_t Reserved1;			///< Zero (To end the filename)
	uint8_t m_bNewNoteAction;		///< 0 = Cut, 1 = Continue, 2 = Note off, 3 = Note fade
	uint8_t m_bDuplicateCheckType;	///< 0 = Off, 1 = Note, 2 = Sample, 3 = Instrument
	uint8_t m_bDuplicateCheckAction;	///< 0 = Cut, 1 = Note off, 2 = Note of
	uint16_t m_uFadeOut;			///< Ranges between 0 and 128, but the fadeout "Count" is 1024
	int8_t m_bPitchPanSeperation;	///< Range -32 -> +32
	uint8_t m_bPitchPanCenter;	///< C-0 to B-9 represented as 0->119 inclusive
	uint8_t m_bGlobalVolume;		///< 0->128
	uint8_t m_bDefaultPan;		///< 0->64, 0x80 => Don't use
	uint8_t m_bRandomVolume;		///< Percentage
	uint8_t m_bRandomPanning;		///< Percentage
	uint16_t m_uTrackerVersion;	///< Tracker version used to save the instrument
	uint8_t m_bSampleCount;		///< Number of samples associated with instrument
	uint8_t Reserved2;
	char m_Name[26];			///< Instrument name
	uint8_t m_bInitialFilterCutoff;	///< Starting filter cutoff
	uint8_t m_bInitialFilterResonance;	///< Starting filter resonance
	uint8_t m_bMIDIChannel;		///< Which midi channel
	uint8_t m_bMIDIProgram;		///< Which midi program
	uint16_t m_uMIDIBank;			///< MIDI instrument bank
	ITKeyMap_t m_KeyMaps[120];	///< Sample/keyboard table

	/// Envelopes for ADSR
	ITEnvelopeLayout_t m_VolumeEnvelope;
	ITEnvelopeLayout_t m_PanEnvelope;
	ITEnvelopeLayout_t m_PitchEnvelope;
};

struct ITOldInsForm	{
	uint32_t m_Signature;			///< 'IMPI'
	char m_DOSName[12];			///< Dos filename
	uint8_t Reserved1;			///< Zero (To end the filename)
	uint8_t m_bNewNoteAction;		///< 0 = Cut, 1 = Continue, 2 = Note off, 3 = Note fade
	uint8_t m_bDuplicateCheckType;	///< 0 = Off, 1 = Note, 2 = Sample, 3 = Instrument
	uint8_t m_bDuplicateCheckAction;	///< 0 = Cut, 1 = Note off, 2 = Note of
	uint16_t m_uFadeOut;			///< Ranges between 0 and 128, but the fadeout "Count" is 1024
	int8_t m_bPitchPanSeperation;	///< Range -32 -> +32
	uint8_t m_bPitchPanCenter;	///< C-0 to B-9 represented as 0->119 inclusive
	uint8_t m_bGlobalVolume;		///< 0->128
	uint8_t m_bDefaultPan;		///< 0->64, 0x80 => Don't use
	uint8_t m_bRandomVolume;		///< Percentage
	uint8_t m_bRandomPanning;		///< Percentage
	uint16_t m_uTrackerVersion;	///< Tracker version used to save the instrument
	uint8_t m_bSampleCount;		///< Number of samples associated with instrument
	uint8_t Reserved2;
	char m_Name[26];			///< Instrument name
	uint8_t m_bInitialFilterCutoff;	///< Starting filter cutoff
	uint8_t m_bInitialFilterResonance;	///< Starting filter resonance
	uint8_t m_bMIDIChannel;		///< Which midi channel
	uint8_t m_bMIDIProgram;		///< Which midi program
	uint16_t m_uMIDIBank;			///< MIDI instrument bank
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
	\sa ImportIT(Sequencer::SongPackage *,const uint8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ImportITEffect(Sequencer::Command_t *pOutput,uint_t uITCommand,uint_t uITArgument)
{
	uint_t uITArgument0F = uITArgument&0xFU;
	uint_t uITArgumentF0 = uITArgument>>4U;
	Sequencer::Command_t::eEffect uEffectCommand = Sequencer::Command_t::EFFECT_NONE;
	uint_t uEffectArgument = 0;
	
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
	pOutput->m_uEffectArgument = static_cast<uint8_t>(uEffectArgument);
}

/*! ************************************

	\brief Import an IT file

	Convert an IT music file into a Burgerlib SongPackage

	\param pOutput Pointer to the SongPackage to fill in with music data
	\param pInput Pointer to the IT file
	\param uInputLength Length of the input data
	\return Zero if no error, non-zero if error
	\sa ImportITEffect(Sequencer::Command_t *,uint_t,uint_t)

***************************************/

uint_t BURGER_API Burger::ImportIT(Sequencer::SongPackage *pOutput,const uint8_t *pInput,uintptr_t uInputLength)
{
	uint_t uResult = Sequencer::IMPORT_UNKNOWN;
	const ITHeader_t *pITHeader = static_cast<const ITHeader_t*>(static_cast<const void *>(pInput));
	if ((uInputLength>=192) && 
		(pITHeader->m_Signature == ITHeader_t::cSignature)) {

		// Assume data starvation error
		uResult = Sequencer::IMPORT_TRUNCATION;

		// Let's attempt the conversion by consuming the header
		const uint8_t *pWork = pInput+192;
		uInputLength-=192;

		// Set up the pointer to the orders

		uint_t uOrderCount = LittleEndian::load(&pITHeader->m_uOrderCount);
		if (uInputLength>=uOrderCount) {

			// Mark the data and consume it
			const uint8_t *pOrders = pWork;
			pWork += uOrderCount;
			uInputLength-=uOrderCount;

			// Instruments (Not 32 bit aligned)
			uint_t uInstrumentCount = LittleEndian::load(&pITHeader->m_uInstrumentCount);
			if (uInputLength>=(uInstrumentCount*4)) {

				// Mark the instrument sizes (May not be 32 bit aligned!!)
				const uint32_t *pInstrumentOffsets = static_cast<const uint32_t *>(static_cast<const void *>(pWork));
				pWork += uInstrumentCount * 4;
				uInputLength -= uInstrumentCount * 4;

				// Samples (Not 32 bit aligned)
				uint_t uSampleCount = LittleEndian::load(&pITHeader->m_uSampleCount);
				if (uInputLength>=(uSampleCount*4)) {

					// Mark the sample sizes (May not be 32 bit aligned!!)
					const uint32_t *pSampleOffsets = static_cast<const uint32_t *>(static_cast<const void *>(pWork));
					pWork += uSampleCount * 4;
					uInputLength -= uSampleCount * 4;

					// Pointers to pattern
					uint_t uPatternCount = LittleEndian::load(&pITHeader->m_uPatternCount);
					if (uInputLength>=(uPatternCount*4)) {
						// Mark the pattern offsets
						const uint32_t *pPatternOffsets = static_cast<const uint32_t *>(static_cast<const void *>(pWork));
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

						uint_t i;
						if (uOrderCount) {
							i = 0;
							do {
								uint_t uOrder = pOrders[i];
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

						uint_t uHeaderFlags = LittleEndian::load(&pITHeader->m_uFlags);
						if (uHeaderFlags & ITHeader_t::FLAG_USEINSTRUMENTS) {
							if (uInstrumentCount) {
								i = 0;
								Sequencer::InstrData_t *pInstrData = pOutput->m_InstrDatas;
								do {
									const ITInstrumentFormat_t *pInstrumentFormat = static_cast<const ITInstrumentFormat_t*>(static_cast<const void *>(pInput+LittleEndian::load_unaligned(&pInstrumentOffsets[i])));
									if (pInstrumentFormat->m_Signature != ITInstrumentFormat_t::cSignature) {
										uResult = Sequencer::IMPORT_BADFILE;
										break;
									}

									// Instrument conversion

									pInstrData->m_uNumberSamples = 0;
									pInstrData->SetName(pInstrumentFormat->m_Name);

									memory_clear(pInstrData->m_WhichSampleForNote,sizeof(pInstrData->m_WhichSampleForNote));

									uint_t uMinSamp = 200;
									uint_t j = 0;
									do {
										uint_t uNewSample = pInstrumentFormat->m_KeyMaps[j].m_bSample;
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
										uint_t uNewSample = pInstrumentFormat->m_KeyMaps[j].m_bSample;
										if (uNewSample) {
											if (uNewSample>uSampleCount) {
												uNewSample = uSampleCount;
											}
											if (pInstrumentFormat->m_KeyMaps[j].m_bNote < 96) {
												pInstrData->m_WhichSampleForNote[pInstrumentFormat->m_KeyMaps[j].m_bNote] = static_cast<uint8_t>((uNewSample-1) - uMinSamp);
											}
										}
									} while (++j<120);

									// Samples conversion

									int prevSamp = -1;
									uint_t uMapIndex = 0;
									do {
										if (pInstrumentFormat->m_KeyMaps[uMapIndex].m_bNote<96) {
											int newsamp = pInstrumentFormat->m_KeyMaps[uMapIndex].m_bSample;
											if (newsamp) {
												--newsamp;
												if (static_cast<uint_t>(newsamp)>=uSampleCount) {
													newsamp = static_cast<int>(uSampleCount-1);
												}
												if (prevSamp != newsamp) {
													prevSamp = newsamp;
													Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::new_object();
													if (!pSampleDescription) {
														// Uh oh...
														uResult = Sequencer::IMPORT_OUTOFMEMORY;
														break;
													}
													pOutput->m_pSampleDescriptions[i*Sequencer::cSampleMaxCount + pInstrData->m_uNumberSamples] = pSampleDescription;

													const ITSampleFormat_t *pSampleFormat = static_cast<const ITSampleFormat_t*>(static_cast<const void *>(pInput+LittleEndian::load_unaligned(&pSampleOffsets[prevSamp])));

													pSampleDescription->m_uSampleSize = LittleEndian::load_unaligned(&pSampleFormat->m_uSampleCount);
													if (pSampleFormat->m_bFlags&ITSampleFormat_t::FLAG_LOOP) {
														pSampleDescription->m_uLoopStart = LittleEndian::load_unaligned(&pSampleFormat->m_uLoopBegin);
														pSampleDescription->m_uLoopLength = LittleEndian::load_unaligned(&pSampleFormat->m_uLoopEnd) - pSampleDescription->m_uLoopStart;
													} else {
														pSampleDescription->m_uLoopStart = 0;
														pSampleDescription->m_uLoopLength = 0;
													}

													pSampleDescription->m_uVolume = pSampleFormat->m_bGlobalVolume;
													pSampleDescription->m_uC2SamplesPerSecond = LittleEndian::load_unaligned(&pSampleFormat->m_uC5Speed);
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
														const void *pDigital = pInput+LittleEndian::load_unaligned(&pSampleFormat->m_uSampleOffset);
														pSampleDescription->m_pSample = allocate_memory_copy(pDigital,pSampleDescription->m_uSampleSize);
														if (!pSampleDescription->m_pSample) {
															// Uh oh...
															uResult = Sequencer::IMPORT_OUTOFMEMORY;
															break;
														}
													}
													if (pSampleDescription->m_pSample) {
														if(!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED) &&
															(pSampleDescription->m_uBitsPerSample == 8)) {
															swap_chars_to_bytes(pSampleDescription->m_pSample,pSampleDescription->m_uSampleSize);
														}

														if( pSampleDescription->m_uBitsPerSample == 16) {
															uintptr_t uLength = pSampleDescription->m_uSampleSize/2;
															if (uLength) {
																if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED)) {
																	uint16_t *pSampleTemp = static_cast<uint16_t *>(pSampleDescription->m_pSample);
																	do {
																		pSampleTemp[0] = static_cast<uint16_t>(LittleEndian::load(pSampleTemp)^0x8000U);
																		++pSampleTemp;
																	} while (--uLength);
																}
#if defined(BURGER_BIGENDIAN)
																else {
																	swap_endian(static_cast<uint16_t *>(pSampleDescription->m_pSample),uLength);
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
									const ITSampleFormat_t *pSampleFormat = static_cast<const ITSampleFormat_t*>(static_cast<const void *>(pInput+LittleEndian::load_unaligned(&pSampleOffsets[i])));
									if (pSampleFormat->m_Signature != ITSampleFormat_t::cSignature) {
										uResult = Sequencer::IMPORT_BADFILE;
										break;
									}
									uint_t uSampCount = LittleEndian::load_unaligned(&pSampleFormat->m_uSampleCount);
									if (uSampCount) {
										const void *pDigitalSample = pInput+LittleEndian::load_unaligned(&pSampleFormat->m_uSampleOffset);
										pInstrData->m_uNumberSamples = 1;
										pInstrData->m_uVolumeFadeSpeed = Sequencer::cDefaultVolumeFade;

										Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::new_object();
										if (!pSampleDescription) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}
										pOutput->m_pSampleDescriptions[i*Sequencer::cSampleMaxCount] = pSampleDescription;

										pSampleDescription->m_uSampleSize = LittleEndian::load_unaligned(&pSampleFormat->m_uSampleCount);
										pSampleDescription->m_uLoopStart = LittleEndian::load_unaligned(&pSampleFormat->m_uLoopBegin);
										pSampleDescription->m_uLoopLength = LittleEndian::load_unaligned(&pSampleFormat->m_uLoopEnd) - pSampleDescription->m_uLoopStart;
										pSampleDescription->m_uVolume = pSampleFormat->m_bGlobalVolume;
										pSampleDescription->m_uC2SamplesPerSecond = LittleEndian::load_unaligned(&pSampleFormat->m_uC5Speed);
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
										pSampleDescription->m_pSample = allocate_memory_copy(pDigitalSample,pSampleDescription->m_uSampleSize);
										if (!pSampleDescription->m_pSample) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}

										if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED) &&
											(pSampleDescription->m_uBitsPerSample==8)) {
											swap_chars_to_bytes(pSampleDescription->m_pSample,pSampleDescription->m_uSampleSize);
										}

										if (pSampleDescription->m_uBitsPerSample == 16) {
											uintptr_t uLength = pSampleDescription->m_uSampleSize/2;
											if (uLength) {
												if (!(pSampleFormat->m_bConversionFlags & ITSampleFormat_t::CONVERT_SIGNED)) {
													uint16_t *pSampleTemp = static_cast<uint16_t *>(pSampleDescription->m_pSample);
													do {
														pSampleTemp[0] = static_cast<uint16_t>(LittleEndian::load(pSampleTemp)^0x8000U);
														++pSampleTemp;
													} while (--uLength);
												}
#if defined(BURGER_BIGENDIAN)
												else {
													swap_endian(static_cast<uint16_t *>(pSampleDescription->m_pSample),uLength);
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

						uint_t uChannelCount = 0;

						for (i = 0; i < uPatternCount ; i++) {
							uint_t uPatternOffset = LittleEndian::load_unaligned(&pPatternOffsets[i]);
							if (uPatternOffset) {
								uint8_t PreviousVariables[Sequencer::cTrackMaxCount];
								memory_clear(PreviousVariables,sizeof(PreviousVariables));

								const ITPatternFormat_t *pPatternFormat = static_cast<const ITPatternFormat_t*>(static_cast<const void *>(pInput + uPatternOffset));
								uint_t uPatternLength = LittleEndian::load_unaligned(&pPatternFormat->m_uLength);

								pWork = pPatternFormat->m_Data;
								uint8_t bMask = 0;
								uint_t bNeedChannelToRead = TRUE;

								uint_t uRow = 0;
								uint_t uRows = LittleEndian::load_unaligned(&pPatternFormat->m_uRows);
								uint_t uChannelTemp = 0;
								while (uRow < uRows) {
									uint8_t uTemp = pWork[0];
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

								if (static_cast<uintptr_t>(pWork - pPatternFormat->m_Data) != uPatternLength) {
									uResult = Sequencer::IMPORT_BADFILE;
									break;
								}
							}
						}

						uChannelCount = (uChannelCount+2)&(~1);

						pOutput->m_SongDescription.m_uChannelCount = uChannelCount;

						for( i = 0; i < uPatternCount ; i++) {
							
							uint_t uPatternOffset = LittleEndian::load_unaligned(&pPatternOffsets[i]);
							if (!uPatternOffset) {
								
								// No Data for this pattern - Clear Pattern

								pOutput->m_pPartitions[i] = Sequencer::PatternData_t::new_object(1,uChannelCount);
								if (!pOutput->m_pPartitions[i]) {
									uResult = Sequencer::IMPORT_OUTOFMEMORY;
									break;
								}
								pOutput->m_pPartitions[i]->SetName("Unnamed pattern");
							} else {

								const ITPatternFormat_t *pPatternFormat = static_cast<const ITPatternFormat_t*>(static_cast<const void *>(pInput + uPatternOffset));
								uint_t uRows = LittleEndian::load_unaligned(&pPatternFormat->m_uRows);

								pOutput->m_pPartitions[i] = Sequencer::PatternData_t::new_object(uRows,uChannelCount);
								if (!pOutput->m_pPartitions[i]) {
									uResult = Sequencer::IMPORT_OUTOFMEMORY;
									break;
								}
								uint8_t PreviousVariables[Sequencer::cTrackMaxCount];
								uint8_t PreviousNotes[Sequencer::cTrackMaxCount];
								uint8_t PreviousInstruments[Sequencer::cTrackMaxCount];
								uint8_t PreviousVolumes[Sequencer::cTrackMaxCount];
								uint8_t PreviousArguments[Sequencer::cTrackMaxCount];
								uint8_t PreviousCommands[Sequencer::cTrackMaxCount];
								memory_clear(PreviousVariables,sizeof(PreviousVariables));

								uint_t uPatternLength = LittleEndian::load_unaligned(&pPatternFormat->m_uLength);
								pWork = pPatternFormat->m_Data;
								uint8_t bMask = 0;
								uint_t bNeedChannelToRead = TRUE;

								uint_t uRow = 0;
								uint_t uTempChannel = 0;
								Sequencer::Command_t *pCommand = NULL;
								while (uRow < uRows) {
									uint8_t uTemp = pWork[0];
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
											uint8_t bNote;

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
											uint8_t uInstrumentTemp;

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
											uint8_t bVolume;

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
											uint8_t uITCommand;
											uint8_t uITArgument;

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

								if (static_cast<uintptr_t>(pWork - pPatternFormat->m_Data) != uPatternLength) {
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
