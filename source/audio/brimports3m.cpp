/***************************************

    Import an S3M file

    Documentation is found here
    http://www.shikadi.net/moddingwiki/S3M_Format

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brimports3m.h"
#include "brendian.h"

//
// All data is in little endian format
//

//
// Known tracker versions
//
// 0x1300 ScreamTracker 3.00
// 0x1301 ScreamTracker 3.01
// 0x1303 ScreamTracker 3.03
// 0x1320 ScreamTracker 3.20
// 0x2nyy Imago Orpheus x.yy
// 0x3nyy Impulse Tracker x.yy
// 0x4nnn Schism Tracker
// 0x5nyy OpenMPT x.yy
// 0xCA00 Camoto/libgamemusic
//
// Channel setting values
//
// 0 to 7 Left PCM channels 1 to 8
// 8 to 15 Right PCM channels 1 to 8
// 16 to 24 Adlib melody channel 1-9
// 25 Adlib percussion channel: bass drum
// 26 Adlib percussion channel: snare drum
// 27 Adlib percussion channel: tom tom
// 28 Adlib percussion channel: top cymbal
// 29 Adlib percussion channel: hi-hat
// 30 to 127 Unused/invalid
// 128 to 254 Unused/invalid (but channel disabled)
// 255 Channel unused
//

#if !defined(DOXYGEN)
struct S3MHeader_t {
#ifdef BURGER_BIGENDIAN
	static const uint32_t cSignature = 0x5343524D;	// 'SCRM'
#else
	static const uint32_t cSignature = 0x4D524353;
#endif
	enum eFlags {
		FLAG_VIBRATO = 0x01,				// ST2 vibrato [deprecated]
		FLAG_TEMPO = 0x02,					// ST2 tempo [deprecated]
		FLAG_AMIGASLIDES = 0x04,			// Amiga slides [deprecated]
		FLAG_VOLUMEOPTIMIZATIONS = 0x08,	// vol optimizations - automatically turn off looping notes when the volume is zero for more than two rows
		FLAG_AMIGALIMITS = 0x10,			// Amiga limits - ignore notes beyond Amiga hardware limits (as Amiga does.) Sliding up stops at B#5. Also affects other Amiga compatibility issues.
		FLAG_SOUNDBLASTER = 0x20,			// Enable SoundBlaster filter/sfx [deprecated]
		FLAG_VOLUMESLIDES = 0x40,			// ST3.00 volume slides - perform volume slides on first row as well. Set implicitly if trackerVersion is 0x1300.
		FLAG_PTRSPECIAL = 0x80				// ptrSpecial is valid
	};

	char m_Name[28];
	uint8_t m_bSignature;				///< Always 0x1A
	uint8_t m_bSongType;				///< Always 0x10 for S3M
	uint8_t m_Reserved1[2];			///< Always 0
	uint16_t m_uOrderCount;			///< Number of entries in the order table, should be even
	uint16_t m_uInstrumentCount;		///< Number of instruments in the song
	uint16_t m_uPatternCount;			///< Number of pattern parapointers in the song
	uint16_t m_uFlags;				///< See eFlags
	uint16_t m_uTrackerVersion;		///< Upper four bits is tracker ID, lower 12 bits is tracker version
	uint16_t m_uSampleType;			///< 1 = Signed samples, else unsigned
	uint32_t m_Signature;				///< "SCRM"
	uint8_t m_bGlobalVolume;			///< Main volume for the song
	uint8_t m_bDefaultSpeed;			///< Frames per row
	uint8_t m_bDefaultTempo;			///< Frames per second
	uint8_t m_bMasterVolume;			///< Bit 7: 1=stereo, 0=mono, Bits 6-0: volume
	uint8_t m_bUltraClickRemoval;		///< Number of channels to use for click removal on real GUS hardware
	uint8_t m_bDefaultPan;				///< 252=read pan values in header, anything else ignores pan values in header
	uint8_t m_Reserved2[8];			///< Unused, some trackers store data here
	uint16_t m_ptrSpecial;			///< Parapointer to additional data, Only valid if m_uFlags & FLAG_PTRSPECIAL is set
	uint8_t m_uChannelSettings[32];	///< See above table about the values stored here
};

struct S3MPattern_t {
	uint8_t m_uWhat;				///< 0 = End of row, Low 5 bits = channel number	
	uint8_t m_uNote;				///< High nibble = octave, low nibble = note
	uint8_t m_uInstrument;		///< Instrument attached to the command
	uint8_t m_uVolume;			///< Volume command
	uint8_t m_uEffectCommand;		///< Effect command enumeration
	uint8_t m_uEffectArgument;	///< Argument for the above effect command
};

struct S3MInstrument_t {
#ifdef BURGER_BIGENDIAN
	static const uint32_t cSignature = 0x53435253;	// 'SCRS'
#else
	static const uint32_t cSignature = 0x53524353;
#endif
	uint8_t m_bInstrumentType;		///< 0 = Empty, 1 = PCM
	uint8_t m_DOSName[12];			///< MSDOS filename
	uint8_t m_bParapointerHi;			///< Upper 8 bits of the 24 bit offset to the pattern
	uint16_t m_uParapointerLo;		///< Lower 16 bits of the 24 bit offset to the pattern
	uint32_t m_uSampleLength;			///< Length of the sample in bytes
	uint32_t m_uLoopBegin;			///< Start of a loop in bytes
	uint32_t m_uLoopEnd;				///< End of the loop in bytes
	uint8_t m_bVolume;				///< Volume of the instrument (0-64)
	uint8_t m_Reserved1;
	uint8_t m_bPacked;				///< TRUE if ADPCM
	uint8_t m_bFlags;					///< Flags 1=loop on, 2=stereo (data is length bytes for left channel then length bytes for right channel), 4=16-bit little-endian sample
	uint32_t m_uC2Speed;				///< Sample rate for C2
	uint8_t Reserved2[12];			///< Set to zero
	char m_Name[28];				///< Sample title, for display to user. Must be null-terminated.
	uint32_t m_Signature;				///< 'SCRS'
};
#endif

/*! ************************************

	\brief Convert an S3M effect command to a Burgerlib one

	Given an S3M effect command and its argument, translate
	them into a Burgerlib command and argument

	\param pOutput Pointer to the command to receive the translated values
	\param uS3MCommand S3M format effect command
	\param uS3MArgument S3M format effect argument
	\sa ImportS3M(Sequencer::SongPackage *,const uint8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ImportS3MEffect(Sequencer::Command_t *pOutput,uint_t uS3MCommand,uint_t uS3MArgument)
{
	uint_t uS3MArgument0F = uS3MArgument&0xFU;
	uint_t uS3MArgumentF0 = uS3MArgument>>4U;
	Sequencer::Command_t::eEffect uEffectCommand = Sequencer::Command_t::EFFECT_NONE;
	uint_t uEffectArgument = 0;
	
	// Parse out the command (Converted to ASCII)
	switch(uS3MCommand + 0x40) {
	default:
		break;
	// Speed
	case 'A':
		uEffectCommand = Sequencer::Command_t::EFFECT_SPEED;
		uEffectArgument = uS3MArgument;
		break;
	// Tempo
	case 'T':
		uEffectCommand = Sequencer::Command_t::EFFECT_SPEED;
		uEffectArgument = uS3MArgument;
		break;

	case 'B':
		uEffectCommand = Sequencer::Command_t::EFFECT_FASTSKIP;
		uEffectArgument = uS3MArgument;
		break;

	case 'C':
		uEffectCommand = Sequencer::Command_t::EFFECT_SKIP;
		uEffectArgument = uS3MArgument;
		break;

	case 'D':
		if (!uS3MArgument0F || !uS3MArgumentF0)	{	// Slide volume
			uEffectCommand = Sequencer::Command_t::EFFECT_SLIDEVOLUME;
			uEffectArgument = uS3MArgument;
		} else if (uS3MArgumentF0 == 0x0F) {		// Fine Slide volume DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F + (11<<4);
		} else if (uS3MArgument0F == 0x0F) {		// Fine Slide volume UP
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgumentF0+(10<<4);
		}
		break;

	case 'E':
		if (uS3MArgumentF0 == 0x0F) {		// FineSlide DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(2 << 4);
		} else if (uS3MArgumentF0 == 0x0E) {	// ExtraFineSlide DOWN
			//not supported
		} else {					// Slide DOWN
			uEffectCommand = Sequencer::Command_t::EFFECT_UPSLIDE;
			uEffectArgument = uS3MArgument;
		}
		break;

	case 'F':
		// FineSlide UP
		if (uS3MArgumentF0 == 0x0F) {
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(1 << 4);

		// ExtraFineSlide UP
		} else if (uS3MArgumentF0 == 0x0E) {
			// not supported
		} else {
			// Slide UP
			uEffectCommand = Sequencer::Command_t::EFFECT_DOWNSLIDE;
			uEffectArgument = uS3MArgument;
		}
		break;
	case 'G':
		uEffectCommand = Sequencer::Command_t::EFFECT_PORTAMENTO;
		uEffectArgument = uS3MArgument;
		break;
	case 'H':
		uEffectCommand = Sequencer::Command_t::EFFECT_VIBRATO;
		uEffectArgument = uS3MArgument;
		break;
	case 'J':
		uEffectCommand = Sequencer::Command_t::EFFECT_ARPEGGIO;
		uEffectArgument = uS3MArgument;
		break;
	case 'K':
		uEffectCommand = Sequencer::Command_t::EFFECT_VIBRATOSLIDE;
		uEffectArgument = uS3MArgument;
		break;
	case 'L':
		uEffectCommand = Sequencer::Command_t::EFFECT_PORTASLIDE;
		uEffectArgument = uS3MArgument;
		break;
	case 'O':
		uEffectCommand = Sequencer::Command_t::EFFECT_OFFSET;
		uEffectArgument = uS3MArgument;
		break;

	case 'S':		// Special Effects
		switch(uS3MArgumentF0) {
		default:
			break;
		case 2:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(5<<4);
			break;	// FineTune
		case 3:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(4<<4);
			break;	// Set Vibrato WaveForm
		case 4:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(7<<4);
			break;	// Set Tremolo WaveForm
		case 0xB:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(6<<4);
			break;	// Loop pattern
		case 0xC:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(12<<4);
			break;	// Cut sample
		case 0xD:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(13<<4);
			break;	// Delay sample
		case 0xE:
			uEffectCommand = Sequencer::Command_t::EFFECT_EXTENDED;
			uEffectArgument = uS3MArgument0F+(14<<4);
			break;	// Delay pattern
		}
		break;
	}
	// Save off the effect
	pOutput->SetEffect(uEffectCommand);
	pOutput->m_uEffectArgument = static_cast<uint8_t>(uEffectArgument);
}

/*! ************************************

	\brief Import an S3M file

	Convert an S3M music file into a Burgerlib SongPackage

	\param pOutput Pointer to the SongPackage to fill in with music data
	\param pInput Pointer to the S3M file
	\param uInputLength Length of the input data
	\return Zero if no error, non-zero if error
	\sa ImportS3MEffect(Sequencer::Command_t *,uint_t,uint_t)

***************************************/

uint_t BURGER_API Burger::ImportS3M(Sequencer::SongPackage *pOutput,const uint8_t *pInput,uintptr_t uInputLength)
{
	uint_t uResult = Sequencer::IMPORT_UNKNOWN;
	const S3MHeader_t *pS3MHeader = static_cast<const S3MHeader_t*>(static_cast<const void *>(pInput));
	if ((uInputLength>=96) && 
		(pS3MHeader->m_Signature == S3MHeader_t::cSignature)) {

		// Assume data starvation error
		uResult = Sequencer::IMPORT_TRUNCATION;

		// Let's attempt the conversion by consuming the header
		const uint8_t *pWork = pInput+96;
		uInputLength-=96;

		// Set up the pointer to the orders
		uint_t uOrderCount = LittleEndian::load(&pS3MHeader->m_uOrderCount);
		if (uInputLength>=uOrderCount) {

			// Mark the data and consume it
			const uint8_t *pOrders = pWork;
			pWork += uOrderCount;
			uInputLength-=uOrderCount;
			
			// Instruments (16 bit aligned)
			uint_t uInstrumentCount = LittleEndian::load(&pS3MHeader->m_uInstrumentCount);
			if (uInputLength>=(uInstrumentCount*2)) {

				// Mark the instrument sizes
				const uint16_t *pInstrumentOffsets = static_cast<const uint16_t *>(static_cast<const void *>(pWork));
				pWork += uInstrumentCount * 2;
				uInputLength -= uInstrumentCount * 2;

				// Pointers to pattern
				uint_t uPatternCount = LittleEndian::load(&pS3MHeader->m_uPatternCount);
				if (uInputLength>=(uPatternCount*2)) {
					// Mark the pattern offsets
					const uint16_t *pPatternOffsets = static_cast<const uint16_t *>(static_cast<const void *>(pWork));
					//pWork += uPatternCount*2;
					uInputLength -= uPatternCount*2;

					// Get the pointer to the S3M instruments
					if (uInputLength>=(uInstrumentCount*sizeof(S3MInstrument_t))) {
					
						uResult = Sequencer::IMPORT_OKAY;
						// Clamp the instrument count for Burgerlib
						if (uInstrumentCount > Sequencer::cInstrumentMaxCount) {
							uInstrumentCount = Sequencer::cInstrumentMaxCount;
						}
						
						// Clamp pattern pointers
						if (uOrderCount>=BURGER_ARRAYSIZE(pOutput->m_SongDescription.m_PatternPointers)) {
							uOrderCount = static_cast<uint_t>(BURGER_ARRAYSIZE(pOutput->m_SongDescription.m_PatternPointers));
						}

						// Begin the data extraction!

						pOutput->Shutdown();
						pOutput->m_SongDescription.SetName(pS3MHeader->m_Name);
						pOutput->m_SongDescription.m_uPatternCount = uPatternCount;
						pOutput->m_SongDescription.m_uPointerCount = uOrderCount;
						pOutput->m_SongDescription.m_uDefaultSpeed = pS3MHeader->m_bDefaultSpeed;
						pOutput->m_SongDescription.m_uDefaultTempo = pS3MHeader->m_bDefaultTempo;
						pOutput->m_SongDescription.m_uMasterVolume = 64;
						pOutput->m_SongDescription.m_uMasterSpeed = 80;
						pOutput->m_SongDescription.m_uMasterPitch = 80;
						pOutput->m_SongDescription.m_uInstrumentCount = uInstrumentCount;

						// Set default sample IDs
						uint_t i = 0;
						do {
							pOutput->m_InstrDatas[i].m_uBaseSampleID = (i * Sequencer::cSampleMaxCount);
						} while (++i < BURGER_ARRAYSIZE(pOutput->m_InstrDatas));

						// Init the pattern pointers
						if (uOrderCount) {
							i = 0;
							do {
								uint_t uOrder = pOrders[i];
								if (uOrder>=uPatternCount) {
									uOrder = 0;
								}
								pOutput->m_SongDescription.m_PatternPointers[i] = uOrder;
							} while (++i<uOrderCount);
						}

						// Init the panning for each channel
						i = 0;
						do {
							// Use the truth table for setting pans
							// 1 0 0 1 1 0 0 1 1 0 0 1 1 0

							// i - 1 = the table below
							// 1 2 3 0 1 2 3 0 1 2 3 0 1 2

							pOutput->m_SongDescription.m_ChannelPans[i] = Sequencer::cMaxPan/4 + ((((i+1)>>1)&1)*(Sequencer::cMaxPan/2));
							pOutput->m_SongDescription.m_ChannelVolumes[i] = Sequencer::cMaxVolume;
						} while (++i < BURGER_ARRAYSIZE(pOutput->m_SongDescription.m_ChannelPans));

						// Determine the number of channels

						uint_t uChannelCount = 0;
						i = 0;
						do {
							// Less than 32 is valid
							if (pS3MHeader->m_uChannelSettings[i] < 32) {
								++uChannelCount;
							}
						} while (++i<32);
						// Round up to even
						uChannelCount = (uChannelCount+1)&(~1);
						pOutput->m_SongDescription.m_uChannelCount = uChannelCount;

						// Process the instruments

						uint_t uSampleCount = 0;		// No samples found yet

						if (uInstrumentCount) {
							i = 0;
							Sequencer::InstrData_t *pInstrData = pOutput->m_InstrDatas;
							do {
								const S3MInstrument_t *pS3MInstruments = static_cast<const S3MInstrument_t *>(static_cast<const void *>(pInput+(LittleEndian::load(&pInstrumentOffsets[i])*16)));

								// Get the name
								pInstrData->SetName(pS3MInstruments->m_Name);

								if ((pS3MInstruments->m_bInstrumentType == 1) && 
									(pS3MInstruments->m_bPacked == 0) &&
									(pS3MInstruments->m_Signature == S3MInstrument_t::cSignature)) {
										const uint8_t *pSample = pInput + ((static_cast<uint_t>(pS3MInstruments->m_bParapointerHi)<<20U)|(static_cast<uint_t>(LittleEndian::load(&pS3MInstruments->m_uParapointerLo))<<4U));
										++uSampleCount;
										pInstrData->m_uNumberSamples = 1;
										pInstrData->m_uVolumeFadeSpeed = Sequencer::cDefaultVolumeFade;

										Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::new_object();
										pOutput->m_pSampleDescriptions[i*Sequencer::cSampleMaxCount] = pSampleDescription;
										if (!pSampleDescription) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}
										pSampleDescription->m_uSampleSize = LittleEndian::load(&pS3MInstruments->m_uSampleLength);
										if (pS3MInstruments->m_bFlags&1) {
											pSampleDescription->m_uLoopStart = LittleEndian::load(&pS3MInstruments->m_uLoopBegin);
											pSampleDescription->m_uLoopLength = LittleEndian::load(&pS3MInstruments->m_uLoopEnd) - pSampleDescription->m_uLoopStart;
										} else {
											pSampleDescription->m_uLoopStart = 0;
											pSampleDescription->m_uLoopLength = 0;
										}
										pSampleDescription->m_uVolume = pS3MInstruments->m_bVolume;
										pSampleDescription->m_uC2SamplesPerSecond = LittleEndian::load(&pS3MInstruments->m_uC2Speed);
										pSampleDescription->m_eLoopType = Sequencer::LOOP_NORMAL;
										pSampleDescription->m_uBitsPerSample = 8;
										if (pS3MInstruments->m_bFlags&4) {
											pSampleDescription->m_uBitsPerSample= 16;
										}
										pSampleDescription->m_iRelativeNote = 0;

										if (pSampleDescription->m_uBitsPerSample == 16) {
											pSampleDescription->m_uSampleSize *= 2;
											pSampleDescription->m_uLoopStart *= 2;
											pSampleDescription->m_uLoopLength *= 2;
										}

										// Import the digital sample
										pSampleDescription->m_pSample = allocate_memory_copy(pSample,pSampleDescription->m_uSampleSize);
										if (!pSampleDescription->m_pSample) {
											// Uh oh...
											uResult = Sequencer::IMPORT_OUTOFMEMORY;
											break;
										}
										uint_t uSampleType = LittleEndian::load(&pS3MHeader->m_uSampleType);
										if (pSampleDescription->m_uBitsPerSample==16) {
											uintptr_t uLength = pSampleDescription->m_uSampleSize/2;
											if (uLength) {
												if (uSampleType != 1) {
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
										} else {
											if (uSampleType != 1) {
												// Convert to signed chars
												swap_chars_to_bytes(pSampleDescription->m_pSample,pSampleDescription->m_uSampleSize);
											}
										}
								} else {
									pInstrData->m_uNumberSamples = 0;
								}
								++pInstrData;
							} while (++i<uInstrumentCount);
						}
						pOutput->m_SongDescription.m_uSampleCount = uSampleCount;
	
						// Process the musical notes
	
						if (uPatternCount) {
							i = 0;
							do {
								Sequencer::PatternData_t *pPatternData = Sequencer::PatternData_t::new_object(64,uChannelCount);
								if (!pPatternData) {
									// Uh oh...
									uResult = Sequencer::IMPORT_OUTOFMEMORY;
									break;
								}
								pOutput->m_pPartitions[i] = pPatternData;
								uint_t uPatternOffset = LittleEndian::load(&pPatternOffsets[i]);
								if (uPatternOffset) {
									pWork = pInput + (uPatternOffset*16) + 2;

									uint_t uRowIndex = 0;
									do {
										//
										// 0 = End the row
										// 0x1F = Switch channel
										// 0x20 = Note and Instrument
										// 0x40 = Volume command
										// 0x80 = Command / Argument
										//

										uint_t uChannelFlags = pWork[0];
										++pWork;

										if (!uChannelFlags) {
											++uRowIndex;
										} else {
											// Channel

											uint_t uChannel = uChannelFlags&0x1FU;
											Sequencer::Command_t *pCommand;
											if (uChannel < uChannelCount) {
												pCommand = pPatternData->GetCommand(static_cast<int>(uRowIndex),static_cast<int>(uChannel));
											} else {
												pCommand = NULL;
											}
										
											// Note

											if (uChannelFlags & 0x20) {
												if (pCommand) {
													uint_t uNoteTemp = pWork[0];
													uNoteTemp = (((uNoteTemp>>4U)&0xFU)*12U) + (uNoteTemp & 0x0FU);
													if (uNoteTemp >= Sequencer::NOTE_MAX) {
														uNoteTemp = 0xFF;
													}
													pCommand->m_uNote = static_cast<uint8_t>(uNoteTemp);
													pCommand->m_uInstrument = pWork[1];
												}
												pWork += 2;
											}

											// Volume command?

											if (uChannelFlags & 0x40) {
												if (pCommand) {
													uint_t uVolume = pWork[0];
													if (uVolume > 64) {
														uVolume = 64;
													}
													pCommand->m_uVolume = static_cast<uint8_t>(uVolume+0x10U);
												}
												++pWork;
											} else {
												if (pCommand) {
													pCommand->m_uVolume = 255;
												}
											}
			
											// Special effect?
											if (uChannelFlags & 0x80) {
												if (pCommand) {
													if (pWork[0] != 255) {
														ImportS3MEffect(pCommand,pWork[0],pWork[1]);
													}
												}
												pWork += 2;
											}
										}
									} while (uRowIndex<64);
								}
							} while (++i<uPatternCount);
						}

						// If there were parsing errors, take off and nuke the site from orbit.

						if (uResult!=Sequencer::IMPORT_OKAY) {
							pOutput->Shutdown();
						}
					}
				}
			}
		}
	}
	return uResult;		
}

