/***************************************

	XM file importer

	Format is found at http://www.fileformat.info/format/xm/corion.htm

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brimportxm.h"
#include "brendian.h"


/***************************************


	Specification of the FastTracker 2- (.XM)-Format:

	The XM module format description for XM files version $0104.

	By Mr.H of Triton in 1994.

	******************************
	*   The XM file structure:   *
	******************************

	Offset Length Type

      0     17   (char) ID text: 'Extended module: '
     17     20   (char) Module name, padded with zeroes
     37      1   (char) $1a
     38     20   (char) Tracker name
     58      2   (word) Version number, hi-byte major and low-byte minor
                        The current format is version $0103

     60      4  (dword) Header size
     +4      2   (word) Song length (in patten order table)
     +6      2   (word) Restart position
     +8      2   (word) Number of channels (2,4,6,8,10,...,32)
    +10      2   (word) Number of patterns (max 256)
    +12      2   (word) Number of instruments (max 128)
    +14      2   (word) Flags: bit 0: 0 = Amiga frequency table (see below);
                                      1 = Linear frequency table
    +16      2   (word) Default tempo
    +18      2   (word) Default BPM
    +20    256   (byte) Pattern order table

                        Patterns:
                        ---------

      ?      4  (dword) Pattern header length
     +4      1   (byte) Packing type (always 0)
     +5      2   (word) Number of rows in pattern (1..256)
     +7      2   (word) Packed patterndata size

      ?      ?          Packed pattern data

                        Instruments:
                        ------------

      ?      4  (dword) Instrument size
     +4     22   (char) Instrument name
    +26      1   (byte) Instrument type (always 0)
    +27      2   (word) Number of samples in instrument

   If the number of samples > 0, then the this will follow:

   !     +29      4  (dword) Sample header size
   !     +33     96   (byte) Sample number for all notes
   !    +129     48   (byte) Points for volume envelope
   !    +177     48   (byte) Points for panning envelope
   !    +225      1   (byte) Number of volume points
   !    +226      1   (byte) Number of panning points
   !    +227      1   (byte) Volume sustain point
   !    +228      1   (byte) Volume loop start point
   !    +229      1   (byte) Volume loop end point
   !    +230      1   (byte) Panning sustain point
   !    +231      1   (byte) Panning loop start point
   !    +232      1   (byte) Panning loop end point
   !    +233      1   (byte) Volume type: bit 0: On; 1: Sustain; 2: Loop
   !    +234      1   (byte) Panning type: bit 0: On; 1: Sustain; 2: Loop
   !    +235      1   (byte) Vibrato type
   !    +236      1   (byte) Vibrato sweep
   !    +237      1   (byte) Vibrato depth
   !    +238      1   (byte) Vibrato rate
   !    +239      2   (word) Volume fadeout
   !    +241      2   (word) Reserved
   !
   !                         Sample headers:
   !                         ---------------
   !
   !       ?      4  (dword) Sample length
   !      +4      4  (dword) Sample loop start
   !      +8      4  (dword) Sample loop length
   !     +12      1   (byte) Volume
   !     +13      1   (byte) Finetune (signed byte -16..+15)
   !     +14      1   (byte) Type: Bit 0-1: 0 = No loop, 1 = Forward loop,
   !                                        2 = Ping-pong loop;
   !                                     4: 16-bit sampledata
   !     +15      1   (byte) Panning (0-255)
   !     +16      1   (byte) Relative note number (signed byte)
   !     +17      1   (byte) Reserved
   !     +18     22   (char) Sample name
   !
   !                         Sample data:
   !                         ------------
   !
   !       ?      ?          Sample data (signed): The samples are stored
   !                         as delta values. To convert to real data:
   !
   !                         old=0;
   !                         for i=1 to len
   !                            new=sample[i]+old;
   !                            sample[i]=new;
   !                            old=new;

   ***********************
   *   Pattern format:   *
   ***********************

   The patterns are stored as ordinary MOD patterns, except that each
   note is stored as 5 bytes:

      ?      1   (byte) Note (0-71, 0 = C-0)
     +1      1   (byte) Instrument (0-128)
     +2      1   (byte) Volume column byte (see below)
     +3      1   (byte) Effect type
     +4      1   (byte) Effect parameter

   A simle packing scheme is also adopted, so that the patterns not become
   TOO large: Since the MSB in the note value is never used, if is used for
   the compression. If the bit is set, then the other bits are interpreted
   as follows:

      bit 0 set: Note follows
          1 set: Instrument follows
          2 set: Volume column byte follows
          3 set: Effect follows
          4 set: Guess what!

   It is very simple, but far from optimal. If you want a better,
   you can always repack the patterns in your loader.

   ******************************
   *   Volumes and envelopes:   *
   ******************************

   The volume formula:

   FinalVol=(FadeOutVol/65536)*(EnvelopeVol/64)*(GlobalVol/64)*(Vol/64)*Scale;

   The panning formula:

   FinalPan=Pan+(EnvelopePan-32)*(128-Abs(Pan-128))/32;

      Envelope:
      ---------

   The envelopes are processed once per frame, instead of every frame where
   no new notes are read. This is also true for the instrument vibrato and
   the fadeout. Since I am so lazy and the tracker is rather self-explaining
   I am not going to write any more for the moment.


   ********************************
   *   Periods and frequencies:   *
   ********************************

   PatternNote = 0..95 (0 = C-0, 95 = B-7)

   FineTune = -128..+127 (-128 = -1 halftone, +127 = +127/128 halftones)
   RelativeTone = -96..95 (0 => C-4 = C-4)

   RealNote = PatternNote + RelativeTone; (0..118, 0 = C-0, 118 = A#9)

      Linear frequence table:
      -----------------------

   Period = 10*12*16*4 - Note*16*4 - FineTune/2;
   Frequency = 8363*2^((6*12*16*4 - Period) / (12*16*4));

      Amiga frequency table:
      ----------------------

   Period = (PeriodTab[(Note MOD 12)*8 + FineTune/16]*(1-Frac32(FineTune/16)) +
             PeriodTab[(Note MOD 12)*8 + FineTune/16]*(Frac32(FineTune/16)))
            *16/2^(Note DIV 12);
      (The period is interpolated for finer finetune values)
   Frequency = 8363*1712/Period;

   PeriodTab = Array[0..12*8-1] of Word = (
      907,900,894,887,881,875,868,862,856,850,844,838,832,826,820,814,
      808,802,796,791,785,779,774,768,762,757,752,746,741,736,730,725,
      720,715,709,704,699,694,689,684,678,675,670,665,660,655,651,646,
      640,636,632,628,623,619,614,610,604,601,597,592,588,584,580,575,
      570,567,563,559,555,551,547,543,538,535,532,528,524,520,516,513,
      508,505,502,498,494,491,487,484,480,477,474,470,467,463,460,457);


   *************************
   *   Standard effects:   *
   *************************

      0      Arpeggio
      1  (*) Porta up
      2  (*) Porta down
      3  (*) Tone porta
      4  (*) Vibrato
      5  (*) Tone porta+Volume slide
      6  (*) Vibrato+Volume slide
      7  (*) Tremolo
      8      Set panning
      9      Sample offset
      A  (*) Volume slide
      B      Position jump
      C      Set volume
      D      Pattern break
      E1 (*) Fine porta up
      E2 (*) Fine porta down
      E3     Set gliss control
      E4     Set vibrato control
      E5     Set finetune
      E6     Set loop begin/loop
      E7     Set tremolo control
      E9     Retrigger note
      EA (*) Fine volume slide up
      EB (*) Fine volume slide down
      EC     Note cut
      ED     Note delay
      EE     Pattern delay
      F      Set tempo/BPM
      G      Set global volume
      H  (*) Global volume slide
      K      Key off
      L      Set envelope position
      P  (*) Panning slide
      R  (*) Multi retrig note
      T      Tremor
      X1 (*) Extra fine porta up
      X2 (*) Extra fine porta down

      (*) = If the command byte is zero, the last nonzero byte for the
            command should be used.

   *********************************
   *   Effects in volume column:   *
   *********************************

   All effects in the volume column should work as the standard effects.
   The volume column is interpreted before the standard effects, so
   some standard effects may override volume column effects.

   Value      Meaning

      0       Do nothing
    $10-$50   Set volume Value-$10
      :          :        :
      :          :        :
    $60-$6f   Volume slide down
    $70-$7f   Volume slide up
    $80-$8f   Fine volume slide down
    $90-$9f   Fine volume slide up
    $a0-$af   Set vibrato speed
    $b0-$bf   Vibrato
    $c0-$cf   Set panning
    $d0-$df   Panning slide left
    $e0-$ef   Panning slide right
    $f0-$ff   Tone porta

***************************************/

//
// All data is in little endian format
//

#if !defined(DOXYGEN)

struct XMHeader_t {
	char m_ID[17];					///< "Extended Module: "
	char m_Name[21];				///< Name of the module
	char m_TrackerName[20];			///< Tracker name
	Word16 m_uVersion;				///< Version number, hi-byte major and low-byte minor
	Word32 m_uHeaderSize;			///< Header size
	Word16 m_uSongLength;			///< Song length (in patten order table)
	Word16 m_uRestartPosition;		///< Restart position
	Word16 m_uChannelCount;			///< Number of channels (2,4,6,8,10,...,32)
	Word16 m_uPatternCount;			///< Number of patterns (max 256)
	Word16 m_uInstrumentCount;		///< Number of instruments (max 128)
	Word16 m_uFlags;				///< Flags: bit 0: 0 = Amiga frequency table (see below) 1 = Linear frequency table
	Word16 m_uTempo;				///< Default tempo
	Word16 m_uBeatsPerMinute;		///< Default BPM
	Word8 m_PatternPointers[256];	///< Pattern order table
};

struct XMPatHeader_t {
	Word32 m_uSize;			///< Pattern header length
	Word8 m_bPacking;		///< Packing type (always 0)
	Word16 m_uRowCount;		///< Number of rows in pattern (1..256)
	Word16 m_uPackSize;		///< Packed patterndata size
};

struct XMInstrument_t {
	Word32 m_uSize;			///< Instrument size
	char m_Name[22];		///< Instrument name
	Word8 m_bType;			///< Instrument type (always 0)
	Word16 m_uSampleCount;	///< Number of samples in instrument
};

struct XMPatch_t {
	Word8 m_WhichSampleForNote[96];	/// Sample number for all notes
	Word16 m_VolumeEnvelope[24];	/// Points for volume envelope
	Word16 m_PanEnvelope[24];		/// Points for panning envelope
	Word8 m_bVolumeEnvelopeCount;	/// Number of volume points
	Word8 m_bPanEnvelopeCount;		/// Number of panning points
	Word8 m_bVolumeSustainIndex;	/// Volume sustain point
	Word8 m_bVolumeBeginIndex;		/// Volume loop start point
	Word8 m_bVolumeEndIndex;		/// Volume loop end point
	Word8 m_bPanSustainIndex;		/// Panning sustain point
	Word8 m_bPanBeginIndex;			/// Panning loop start point
	Word8 m_bPanEndIndex;			/// Panning loop end point
	Word8 m_bVolumeEnvelopeFlags;	/// Volume type: bit 0: On; 1: Sustain; 2: Loop
	Word8 m_bPanEnvelopeFlags;		/// Panning type: bit 0: On; 1: Sustain; 2: Loop
	Word8 m_bVibratoFlags;			/// Vibrato type
	Word8 m_bVibratoSweep;			/// Vibrato sweep
	Word8 m_bVibratoDepth;			/// Vibrato depth
	Word8 m_bVibratoSpeed;			/// Vibrato rate
	Word16 m_uVolumeFade;			/// Volume fadeout
	Word16 m_Reserved[11];			/// Reserved
};

struct XMSample_t {
	Word32 m_uLength;		/// Sample length
	Word32 m_uLoopStart;	/// Sample loop start
	Word32 m_uLoopLength;	/// Sample loop length
	Word8 m_bVolume;		/// Volume
	char m_bFineTune;		/// Finetune (signed byte -128..+127)
	Word8 m_bType;			/// Type: Bit 0-1: 0 = No loop, 1 = Forward loop,
	// 2 = Ping-pong loop;
	// 4: 16-bit sampledata
	Word8 m_bPanning;		/// Panning (0-255)
	char m_bRelNote;		/// Relative note number (signed byte)
	Word8 m_bReserved;		/// Reserved
	char m_Name[22];		/// Sample name
};

struct XMNote_t {
	Word8 m_uNote;
	Word8 m_uInstrument;
	Word8 m_uVolume;
	Word8 m_uEffectCommand;
	Word8 m_uEffectArgument;
};

static const Word g_FineTune[16] = {
	7895,7941,7985,8046,8107,8169,8232,8280,
	8363,8413,8463,8529,8581,8651,8723,8757
};
#endif

/*! ************************************

	\brief Convert an XM effect command to a Burgerlib one

	Given an data stream with an XM effect encoded into it,
	parse the stream, convert the command and its arguments, translate
	them into a Burgerlib command and argument

	\param pOutput Pointer to the command to receive the translated values
	\param pInput Pointer to the input stream
	\return Pointer to the data stream after parsing out a single command
	\sa ImportXM(Sequencer::SongPackage *,const Word8 *,WordPtr)

***************************************/

const Word8 * BURGER_API Burger::ImportXMEffect(Sequencer::Command_t *pOutput,const Word8 *pInput)
{
	//
	// First step, decompress the note packet from the stream
	//

	Word uNote;
	Word uInstrument;
	Word uVolume;
	Word uEffectCommand;
	Word uEffectArgument;

	Word uFlags = pInput[0];
	if (uFlags&0x80U) {
		++pInput;

		uNote = 0xFF;
		uInstrument = 0;
		uVolume  = 0xFF;
		uEffectCommand = 0xFF;
		uEffectArgument = 0xFF;

		if (uFlags&1U) {
			uNote = pInput[0];
			++pInput;
		} 

		if (uFlags&2U) {
			uInstrument = pInput[0];
			++pInput;
		}

		if (uFlags&4U) {
			uVolume = pInput[0];
			++pInput;
		}

		if (uFlags&8U) {
			uEffectCommand = pInput[0];
			++pInput;
		}

		if (uFlags&0x10U) {
			uEffectArgument = pInput[0];
			++pInput;
		}
	} else {
		// Grab 5 uncompressed bytes from XMNote_t
		uNote = uFlags;
		uInstrument = pInput[1];
		uVolume = pInput[2];
		uEffectCommand = pInput[3];
		uEffectArgument = pInput[4];
		pInput+=5;
	}

	//
	// Pass 2, convert to burgerlib
	//


	// Fix the note on/off commands
	if (!uNote || (uNote == 0xFF)) {
		uNote = 0xFF;
	} else {
		--uNote;
		if (uNote == 96) {
			uNote = 0xFE;
		}
	}

	// Fix the effects
	if (uEffectCommand < 0x10) {
		if (uEffectArgument == 0xFF) {
			uEffectArgument = 0;
		}

		switch(uEffectCommand) {
		//	case 'G'-55:	// G - set global volume
		//	break;

		//	case 'H'-55:	// H - global volume slide
		//	break;

		case 'K'-55:		// K - keyoff	
			uInstrument = 0;
			uNote = 0xFE;
			break;

		//	case 'L'-55:	// L - set envelope position
		//	break;

		//	case 'P'-55:	// P - panning slide
		//	break;

		//	case 'R'-55:	// R - multi retrig note
		//	break;

		//	case 'T'-55:
		//	break;

		//	case 'X'-55:
		//	if((dat>>4)==1) {	// X1 extra fine porta up
		//	} else {			// X2 extra fine porta down
		//	}
		//	break;

		default:
			break;
		}
	} else {
		uEffectCommand = 0;
		uEffectArgument = 0;
	}

	//
	// Store in the output
	//
	pOutput->m_uInstrument = static_cast<Word8>(uInstrument);
	pOutput->m_uNote = static_cast<Word8>(uNote);
	pOutput->m_uVolume = static_cast<Word8>(uVolume);
	pOutput->m_uEffectCommand = static_cast<Word8>(uEffectCommand);
	pOutput->m_uEffectArgument = static_cast<Word8>(uEffectArgument);

	// Return the adjusted input pointer

	return pInput;
}

/*! ************************************

	\brief Import an XM file

	Convert an XM music file into a Burgerlib SongPackage

	\param pOutput Pointer to the SongPackage to fill in with music data
	\param pInput Pointer to the XM file
	\param uInputLength Length of the input data
	\return Zero if no error, non-zero if error
	\sa ImportXMEffect(Sequencer::Command_t *,const Word8 *)

***************************************/

Word BURGER_API Burger::ImportXM(Sequencer::SongPackage *pOutput,const Word8 *pInput,WordPtr uInputLength)
{
	Word uResult = Sequencer::IMPORT_UNKNOWN;
	const XMHeader_t *pXMHeader = static_cast<const XMHeader_t*>(static_cast<const void *>(pInput));
	if ((uInputLength>=sizeof(XMHeader_t)) &&
		(!MemoryCompare(pXMHeader->m_ID,"Extended Module: ",17)) &&
		(LittleEndian::Load(&pXMHeader->m_uVersion) == 0x104)) {

		// Assume data starvation error
		uResult = Sequencer::IMPORT_TRUNCATION;

		// Get the counts
			
		Word uInstrumentCount = LittleEndian::Load(&pXMHeader->m_uInstrumentCount);
		if (uInstrumentCount>Sequencer::cInstrumentMaxCount) {
			uInstrumentCount = Sequencer::cInstrumentMaxCount;
		}

		// Begin the conversion

		pOutput->Shutdown();
		pOutput->m_SongDescription.SetName(pXMHeader->m_Name);
		Word uOrderCount = LittleEndian::Load(&pXMHeader->m_uSongLength);
		Word uPatternCount = LittleEndian::Load(&pXMHeader->m_uPatternCount);
		pOutput->m_SongDescription.m_uPatternCount = uPatternCount;
		pOutput->m_SongDescription.m_uPointerCount = uOrderCount;
		pOutput->m_SongDescription.m_uDefaultSpeed = LittleEndian::Load(&pXMHeader->m_uTempo);
		pOutput->m_SongDescription.m_uDefaultTempo = LittleEndian::Load(&pXMHeader->m_uBeatsPerMinute);
		pOutput->m_SongDescription.m_uMasterVolume = 64;
		pOutput->m_SongDescription.m_uMasterSpeed = 80;
		pOutput->m_SongDescription.m_uMasterPitch = 80;
		pOutput->m_SongDescription.m_uInstrumentCount = uInstrumentCount;
		pOutput->m_SongDescription.m_uChannelCount = LittleEndian::Load(&pXMHeader->m_uChannelCount);
		if (uOrderCount > 128) {
			pOutput->m_SongDescription.m_uPointerCount = 128;
		}

		//
		// Create the pattern pointers
		//

		if (uOrderCount) {
			Word uIndex = 0;
			do {
				Word uPattern = pXMHeader->m_PatternPointers[uIndex];
				if (uPattern >= uPatternCount) {
					uPattern = uPatternCount-1;
				}
				pOutput->m_SongDescription.m_PatternPointers[uIndex] = uPattern;
			} while (++uIndex<uOrderCount);
		}

		//
		// Initialize the pans and volumes
		//

		Word i = 0;
		do {
			// Use the truth table for setting pans
			// 1 0 0 1 1 0 0 1 1 0 0 1 1 0

			// i - 1 = the table below
			// 1 2 3 0 1 2 3 0 1 2 3 0 1 2

			pOutput->m_SongDescription.m_ChannelPans[i] = Sequencer::cMaxPan/4 + ((((i+1)>>1)&1)*(Sequencer::cMaxPan/2));
			pOutput->m_SongDescription.m_ChannelVolumes[i] = Sequencer::cMaxVolume;
		} while (++i<Sequencer::cTrackMaxCount);

		//
		// Index to the data
		//
		
		pInput+=sizeof(XMHeader_t);
		uInputLength -= sizeof(XMHeader_t);
	
		//
		// Iterate over an array of XMPatHeader_t
		// Do it manually since the structure is NOT aligned
		//

		uResult = Sequencer::IMPORT_OKAY;
		if (uPatternCount) {
			Word uChannelCount = pOutput->m_SongDescription.m_uChannelCount;
			i = 0;
			do {
				Word uSize = LittleEndian::LoadAny(static_cast<const Word32 *>(static_cast<const void *>(pInput)));	// m_uSize
//				Word bPacking = pInput[4];		// m_bPacking
				Word uRowCount = LittleEndian::LoadAny(static_cast<const Word16 *>(static_cast<const void *>(pInput+5)));	// m_uRowCount
				Word uPackSize = LittleEndian::LoadAny(static_cast<const Word16 *>(static_cast<const void *>(pInput+7)));		// m_uPackSize
				if (uInputLength<uSize) {
					uResult = Sequencer::IMPORT_TRUNCATION;
					break;
				}
				// Consume the data
				pInput += uSize;
				uInputLength -= uSize;

				// If uPackSize is zero, there's no pattern

				Sequencer::PatternData_t *pPatternData;
				if (uPackSize) {
					pPatternData = Sequencer::PatternData_t::New(uRowCount,uChannelCount);
					if (!pPatternData) {
						// Uh oh...
						uResult = Sequencer::IMPORT_OUTOFMEMORY;
						break;
					}
					pOutput->m_pPartitions[i] = pPatternData;
					
					if (uRowCount && uChannelCount) {
						const Word8 *pTempInput = pInput;
						Word uRowIndex = 0;
						do {
							Word uChannel = 0;
							do {
								pInput = ImportXMEffect(pPatternData->GetCommand(static_cast<int>(uRowIndex),static_cast<int>(uChannel)),pInput);
							} while (++uChannel<uChannelCount);
						} while (++uRowIndex<uRowCount);
						WordPtr uConsumed = static_cast<WordPtr>(pInput-pTempInput);
						if (uInputLength<uConsumed) {
							uResult = Sequencer::IMPORT_TRUNCATION;
							break;
						}
						uInputLength -= uConsumed;
					}
				} else {
					pPatternData = Sequencer::PatternData_t::New(1,pOutput->m_SongDescription.m_uChannelCount);
					if (!pPatternData) {
						// Uh oh...
						uResult = Sequencer::IMPORT_OUTOFMEMORY;
					}
					pOutput->m_pPartitions[i] = pPatternData;
				}
			} while (++i<uPatternCount);
		}

		//
		// Process the instruments
		//

		if (!uResult) {

			//
			// Initialize the default sample ID
			//

			i = 0;
			do {
				pOutput->m_InstrDatas[i].m_uBaseSampleID = i * Sequencer::cSampleMaxCount;
			} while (++i<Sequencer::cInstrumentMaxCount);

			if (uInstrumentCount) {
				Sequencer::InstrData_t *pInstrData = pOutput->m_InstrDatas;
				Word uInstrument = 0;
				do {
					//
					// Is there data?
					//
					if (uInputLength<29) {
						uResult = Sequencer::IMPORT_TRUNCATION;
						break;
					}

					const Word8 *pTempInput = pInput;
					// XMInstrument_t is not native aligned, manually extract the data

					Word uSize = LittleEndian::LoadAny((Word32 *)pInput);
					pInstrData->SetName(static_cast<const char *>(static_cast<const void *>(pInput+4)));
					//Word bType = pInput[26];
					Word uSampleCount = LittleEndian::LoadAny(static_cast<const Word16 *>(static_cast<const void *>(pInput+27)));
					pInstrData->m_uNumberSamples = uSampleCount;
					// Consume the structure
					pInput += 29;

					//
					// Was there a patch?
					//
					Word uSampleSize = 0;
					if (uSampleCount) {
						// Get the size of the data
						uSampleSize = LittleEndian::LoadAny(static_cast<const Word32*>(static_cast<const void *>(pInput)));

						// Get the pointer to the patch
						const XMPatch_t *pPatch = static_cast<const XMPatch_t *>(static_cast<const void *>(pInput+4));
						//pInput += sizeof(XMPatch_t);
						i = 0;
						do {
							pInstrData->m_WhichSampleForNote[i] = pPatch->m_WhichSampleForNote[i];
						} while (++i<96);

						//
						// Volume envelopes
						//

						i = 0;
						do {
							pInstrData->m_VolumeEnvelope[i].m_uPosition = LittleEndian::LoadAny(&pPatch->m_VolumeEnvelope[i*2]); 
							pInstrData->m_VolumeEnvelope[i].m_uVolume = LittleEndian::LoadAny(&pPatch->m_VolumeEnvelope[i*2+1]);
						} while (++i<12);

						pInstrData->m_uVolumeEnvelopeCount = pPatch->m_bVolumeEnvelopeCount;
						pInstrData->m_uVolumeEnvelopeFlags = pPatch->m_bVolumeEnvelopeFlags;
						pInstrData->m_uVolumeSustainIndex = pPatch->m_bVolumeSustainIndex;
						pInstrData->m_uVolumeBeginIndex	= pPatch->m_bVolumeBeginIndex;
						pInstrData->m_uVolumeEndIndex = pPatch->m_bVolumeEndIndex;

						if (pInstrData->m_uVolumeBeginIndex >= pInstrData->m_uVolumeEnvelopeCount) {
							pInstrData->m_uVolumeBeginIndex = static_cast<Word8>(pInstrData->m_uVolumeEnvelopeCount-1U);
						}
						if (pInstrData->m_uVolumeEndIndex >= pInstrData->m_uVolumeEnvelopeCount) {
							pInstrData->m_uVolumeEndIndex = static_cast<Word8>(pInstrData->m_uVolumeEnvelopeCount-1U);
						}
						pInstrData->m_uVolumeFadeSpeed = LittleEndian::LoadAny(&pPatch->m_uVolumeFade);

						//
						// Pan envelopes
						//

						i = 0;
						do {
							pInstrData->m_PanEnvelope[i].m_uPosition = LittleEndian::LoadAny(&pPatch->m_PanEnvelope[i*2]);
							pInstrData->m_PanEnvelope[i].m_uVolume = LittleEndian::LoadAny(&pPatch->m_PanEnvelope[i*2+1]);
						} while (++i<12);

						pInstrData->m_uPanEnvelopeCount = pPatch->m_bPanEnvelopeCount;
						pInstrData->m_uPanEnvelopeFlags = pPatch->m_bPanEnvelopeFlags;
						pInstrData->m_uPanSustainIndex = pPatch->m_bPanSustainIndex;
						pInstrData->m_uPanBeginIndex = pPatch->m_bPanBeginIndex;
						pInstrData->m_uPanEndIndex = pPatch->m_bPanEndIndex;

						if (pInstrData->m_uPanBeginIndex >= pInstrData->m_uPanEnvelopeCount) {
							pInstrData->m_uPanBeginIndex = static_cast<Word8>(pInstrData->m_uPanEnvelopeCount-1);
						}
						if (pInstrData->m_uPanEndIndex >= pInstrData->m_uPanEnvelopeCount) {
							pInstrData->m_uPanEndIndex = static_cast<Word8>(pInstrData->m_uPanEnvelopeCount-1);
						}
					}

					//
					// Was their enough data?
					//

					if (uInputLength<uSize) {
						uResult = Sequencer::IMPORT_TRUNCATION;
						break;
					}
					pInput = pTempInput + uSize;
					uInputLength -= uSize;

					//
					// Get the samples
					//

					if (uSampleCount) {
						Word uSampleIndex = 0;
						do {
							const XMSample_t *pXMSample = static_cast<const XMSample_t *>(static_cast<const void *>(pInput));
							if (uInputLength<uSampleSize) {
								uResult = Sequencer::IMPORT_TRUNCATION;
								break;
							}
							pInput = pInput + uSampleSize;
							uInputLength -= uSampleSize;
							Sequencer::SampleDescription *pSampleDescription = Sequencer::SampleDescription::New();
							pOutput->m_pSampleDescriptions[uInstrument*Sequencer::cSampleMaxCount + uSampleIndex] = pSampleDescription;
							pSampleDescription->m_uSampleSize = LittleEndian::LoadAny(&pXMSample->m_uLength);
							pSampleDescription->m_uLoopStart = LittleEndian::LoadAny(&pXMSample->m_uLoopStart);
							pSampleDescription->m_uLoopLength = LittleEndian::LoadAny(&pXMSample->m_uLoopLength);

							pSampleDescription->m_uVolume = pXMSample->m_bVolume;
							pSampleDescription->m_uC2SamplesPerSecond = g_FineTune[(pXMSample->m_bFineTune + 128)/16];
							pSampleDescription->m_eLoopType = Sequencer::LOOP_NORMAL;
							pSampleDescription->m_uBitsPerSample = 8;

							// Ping pong loop?
							if (pXMSample->m_bType & 0x02U) {
								pSampleDescription->m_eLoopType = Sequencer::LOOP_PINGPONG;
							}

							// 16 bit samples?
							if (pXMSample->m_bType & 0x10U)	{
								pSampleDescription->m_uBitsPerSample = 16;
							}

							// Not looping?
							if (!(pXMSample->m_bType & 0x03U)) {
								pSampleDescription->m_uLoopStart = 0;
								pSampleDescription->m_uLoopLength = 0;
							}

							pSampleDescription->m_iRelativeNote = pXMSample->m_bRelNote;
							pSampleDescription->SetName(pXMSample->m_Name);
						} while (++uSampleIndex<uSampleCount);
					}

					//
					// Exit on error
					//

					if (uResult) {
						break;
					}

					//
					// Process the samples for the instrument
					//

					if (uSampleCount) {
						Word uSampleIndex = 0;
						do {

							Sequencer::SampleDescription *pSampleDescription = pOutput->m_pSampleDescriptions[uInstrument*Sequencer::cSampleMaxCount + uSampleIndex];
							void *pDestination = Alloc(pSampleDescription->m_uSampleSize);
							if (!pDestination) {
								uResult = Sequencer::IMPORT_OUTOFMEMORY;
								break;
							}
							pSampleDescription->m_pSample = pDestination;

							// Convert the delta compressed waveforms into standard samples

							if (uInputLength<pSampleDescription->m_uSampleSize) {
								uResult = Sequencer::IMPORT_TRUNCATION;
								break;
							}
							const Word8 *pSource = pInput;
							pInput += pSampleDescription->m_uSampleSize;
							uInputLength -= pSampleDescription->m_uSampleSize;

							if (pSampleDescription->m_uBitsPerSample == 16) {				
								// Delta to Real
								WordPtr uCounter = pSampleDescription->m_uSampleSize>>1U;
								if (uCounter) {
									Word uTemp = 0;
									do {
										uTemp = LittleEndian::LoadAny(static_cast<const Word16 *>(static_cast<const void *>(pSource))) + uTemp;
										static_cast<Word16 *>(pDestination)[0] = static_cast<Word16>(uTemp);
										pSource+=2;
										pDestination=static_cast<Word16 *>(pDestination)+1;
									} while (--uCounter);
								}
							} else {
								// Delta to Real
								WordPtr uCounter = pSampleDescription->m_uSampleSize;
								if (uCounter) {
									Word uTemp = 0;
									do {
										uTemp = pSource[0] + uTemp;
										static_cast<Word8 *>(pDestination)[0] = static_cast<Word8>(uTemp);
										++pSource;
										pDestination=static_cast<Word8 *>(pDestination)+1;
									} while (--uCounter);
								}
							}
						} while (++uSampleIndex<uSampleCount);
					}

					//
					// Abort on error
					//

					if (uResult) {
						break;
					}
					++pInstrData;
				} while (++uInstrument<uInstrumentCount);
			}			
		}
	}

	if (uResult) {
		pOutput->Shutdown();
	}
	// Return the result
	return uResult;		
}
