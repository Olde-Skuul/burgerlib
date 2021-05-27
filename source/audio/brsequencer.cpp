/***************************************

    Music Sequencer

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brsequencer.h"
#include "brendian.h"
#include "brfixedpoint.h"
#include "brfilemanager.h"

#if !defined(DOXYGEN)
// BURGER_CREATE_STATICRTTI_PARENT(Burger::Sequencer,Burger::Base);

//
// One octave's worth of pitches in 28.4 fixed point
//

static const uint_t g_FrequencyTable[12] = {
	1712*16,1616*16,1524*16,1440*16,1356*16,1280*16,
	1208*16,1140*16,1076*16,1016*16,960*16,907*16
};

#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressULaw::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/


/*! ************************************

	\brief Table with values to use for vibrato

***************************************/

const int32_t Burger::Sequencer::g_VibratoTable[64] = {
	   0,  50, 100, 149, 196, 241, 284, 325, 362, 396, 426, 452, 473,490,502,510,512,
	 510, 502, 490, 473, 452, 426, 396, 362, 325, 284, 241, 196, 149,100, 50,  0,-49,
	 -99,-148,-195,-240,-283,-324,-361,-395,-425,-451,-472,-489,-501,
	-509,-511,-509,-501,-489,-472,-451,-425,-395,-361,-324,-283,-240,
	-195,-148, -99, -49
};


/*! ************************************

	\var Burger::Sequencer::cAmigaFrequency

	C one octave above Middle C is 523.25 Hz. To play this on the Amiga with
	16 samples would require a sample rate of 8372 Hz. However, the Amiga 
	has a clock rate of 7,159,090 Hz on NTSC and 7,093,790 Hz for PAL. So, by
	dividing by 855 and 847 respectively, the effective sample rate is actually
	8363 Hz. To reduce the chance of harmonic distortion by sample rate adjusted
	tuning, samples are optimized at 8363 samples per second.

	Hence, this weird magic number used for tuning.

***************************************/


/*! ************************************

	\struct Burger::Sequencer::Command_t
	\brief A buffer describing a single note
	
	For every note played, one of these structures exist to describe it

	\sa PatternData_t

***************************************/


/*! ************************************

	\brief Initialize the structure

	Initialize all entries for an unused command

***************************************/

void BURGER_API Burger::Sequencer::Command_t::Clear(void)
{
	m_uNote = NOTE_UNUSED;
	m_uInstrument = 0;
	m_uVolume = VOLUME_UNUSED;
	m_uEffectCommand = EFFECT_NONE;
	m_uEffectArgument= 0;
}

/*! ************************************

	\fn eNote Burger::Sequencer::Command_t::GetNote(void) const
	\brief Get the note

	\return Returns the eNote constant

***************************************/

/*! ************************************

	\fn eVolume Burger::Sequencer::Command_t::GetVolume(void) const
	\brief Get the volume

	\return Returns the eVolume constant

***************************************/

/*! ************************************

	\fn eEffect Burger::Sequencer::Command_t::GetEffect(void) const
	\brief Get the effect command

	\return Returns the eEffect constant

***************************************/



/*! ************************************

	\struct Burger::Sequencer::PatternData_t
	\brief A buffer describing a group of notes

	This contains a 2 dimensional array of pattern notes
	organized by rows and channels.

	\sa Command_t

***************************************/

/*! ************************************

	\brief Allocate memory for a pattern array

	Given the number of rows and channels, allocate memory
	in one block with space for the Channel_t array

	The array is cleared
	\param uRows Number of commands per row
	\param uChannels Number of channel row
	\return \ref NULL if out of memory, or valid pointer to data

***************************************/

Burger::Sequencer::PatternData_t * BURGER_API Burger::Sequencer::PatternData_t::New(uint_t uRows,uint_t uChannels)
{
	// Number of entries
	uint_t uCount = uRows*uChannels;
	if (!uCount) {
		// Empty?
		uRows = 0;
		uChannels = 0;
	} else {
		// Remove 1 since PatternData_t already has one command
		--uCount;
	}

	// Allocate the memory
	PatternData_t *pResult = static_cast<PatternData_t *>(Alloc(sizeof(PatternData_t)+(uCount*sizeof(Command_t))));
	if (pResult) {
		// Initialize the rows and channels
		pResult->m_uRowCount = uRows;
		pResult->m_uChannelCount = uChannels;
		MemoryClear(pResult->m_Name,sizeof(pResult->m_Name));	// Init the name
		Command_t *pCommand = pResult->m_Commands;

		//
		// Erase every entry
		//
		++uCount;
		do {
			pCommand->Clear();
			++pCommand;
		} while (--uCount);
	}
	// Return the data
	return pResult;
}

/*! ************************************

	\brief Return the requested Command_t

	Do a bounds check on the input values to ensure
	they do not exceed the boundaries of the Command_t array
	and return the pointer to the requested entry.

	\param iRow Index into the row
	\param iChannel Index into the channel
	\return Pointer to the entry in the data
	
***************************************/

Burger::Sequencer::Command_t* BURGER_API Burger::Sequencer::PatternData_t::GetCommand(int iRow,int iChannel)
{
	// Negative numbers become zero
	uint_t uRow = static_cast<uint_t>(ClampZero(static_cast<int32_t>(iRow)));
	uint_t uChannel = static_cast<uint_t>(ClampZero(static_cast<int32_t>(iChannel)));

	// Out of bounds?
	if (uRow >= m_uRowCount) {
		uRow = m_uRowCount;
		if (uRow) {
			--uRow;
		}
	}
	if (uChannel >= m_uChannelCount) {
		uChannel = m_uChannelCount;
		if (uChannel) {
			--uChannel;
		}
	}
	// Index into the table
	return &m_Commands[(m_uRowCount * uChannel) + uRow];
}

/*! ************************************

	\fn void Burger::Sequencer::PatternData_t::SetName(const char *pName)
	\brief Set the name of the pattern

	\note If the string exceeds the size of the buffer, it will be truncated.

	\param pName Pointer to a "C" string for the new name
	
***************************************/



/*! ************************************

	\class Burger::Sequencer::SampleDescription
	\brief A buffer describing digital sample for an instrument

	All the settings for this sample is stored in this structure

***************************************/

/*! ************************************

	\brief Standard destructor

	Release sample data if any had been allocated

***************************************/

Burger::Sequencer::SampleDescription::~SampleDescription()
{
	Free(m_pSample);
	m_pSample = NULL;
}

/*! ************************************

	\brief Allocate a new, initialized SampleDescription

	\return \ref NULL if out of memory or a pointer to an initialized SampleDescription

***************************************/

Burger::Sequencer::SampleDescription * BURGER_API Burger::Sequencer::SampleDescription::New(void)
{
	return static_cast<SampleDescription *>(AllocClear(sizeof(SampleDescription)));
}

/*! ************************************

	\fn void Burger::Sequencer::SampleDescription::SetName(const char *pName)
	\brief Set the name of the sample

	\note If the string exceeds the size of the buffer, it will be truncated.

	\param pName Pointer to a "C" string for the new name
	
***************************************/




/*! ************************************

	\struct Burger::Sequencer::EnvelopeMarker_t
	\brief A single entry for sound envelope marking

***************************************/

/*! ************************************

	\brief Interpolate between two envelope positions

	Given a time mark, determine the interpolated volume between
	the two sound envelope marks

	\param pNext Pointer to the next envelope in the sequence
	\param uPosition Current time mark
	\return Interpolated volume

***************************************/

uint_t BURGER_API Burger::Sequencer::EnvelopeMarker_t::Interpolate(const EnvelopeMarker_t *pNext,uint_t uPosition) const
{
	// Is the first entry okay?
	uint_t uFirstPosition = m_uPosition;
	uint_t uResult = m_uVolume;
	if (uPosition >= uFirstPosition) {
		// Prevent divide by zero
		uint_t uSecondPosition = pNext->m_uPosition;
		if (uFirstPosition != uSecondPosition) {
			// Interpolate (Calculation must be signed for negative value tracking
			uResult = static_cast<uint_t>(uResult + ((static_cast<int_t>(uPosition-uFirstPosition)*static_cast<int_t>(pNext->m_uVolume-uResult)) / static_cast<int_t>(uSecondPosition-uFirstPosition)));
		}
	}
	// Return the new volume
	return uResult;
}



/*! ************************************

	\brief Reset an instrument to defaults

***************************************/

void BURGER_API Burger::Sequencer::InstrData_t::Reset(void)
{
	MemoryClear(m_Name,sizeof(m_Name));
	MemoryClear(m_WhichSampleForNote,sizeof(m_WhichSampleForNote));
	MemoryClear(m_VolumeEnvelope,sizeof(m_VolumeEnvelope));
	MemoryClear(m_PanEnvelope,sizeof(m_PanEnvelope));

	m_uNumberSamples = 0;
	m_uVolumeEnvelopeCount = 0;
	m_uPanEnvelopeCount	= 0;
	
	m_uVolumeSustainIndex = 0;
	m_uVolumeBeginIndex = 0;
	m_uVolumeEndIndex = 0;
	
	m_uPanSustainIndex = 0;
	m_uPanBeginIndex = 0;
	m_uPanEndIndex = 0;
	
	m_uVolumeEnvelopeFlags = 0;
	m_uPanEnvelopeFlags = 0;
	
	m_uVolumeFadeSpeed = cDefaultVolumeFade;
	m_uVibratoDepth	= 0;
	m_uVibratoSpeed = 0;
}




/*! ************************************

	\brief Initialize a channel
	
	Set a channel to defaults

	\param uID Default ID to assign the channel

***************************************/

void BURGER_API Burger::Sequencer::Channel_t::Init(uint_t uID)
{
	m_pBeginningOfSample = NULL;
	m_pEndOfSample = NULL;
	m_pCurrent = NULL;
	m_pEchoSample = NULL;
	m_uSampleSize = 0;
	m_uID = uID;
	m_uBitsPerSample = 8;
	m_uLoopBeginning = 0;
	m_uLoopSize = 0;
	m_uCurrentInstrumentID = 0;
	m_uPreviousInstrumentID = 0;
	m_uSampleID = 0;
	m_uAmigaFineTune = cAmigaFrequency;
	m_eNote = NOTE_UNUSED;
	m_iPreviousNote = NOTE_UNUSED;
	m_uTimePeriod = GetNotePeriod(NOTE_MID,cAmigaFrequency);
	m_uPreviousTimePeriod= GetNotePeriod(NOTE_MID,cAmigaFrequency);
	m_uVolume = cMaxVolume;
	m_iPan = cMaxPan/2;
	m_uEffectCommand = 0;
	m_uEffectArgument = 0;
	m_uVolumeCommand = 0;
	m_uArpeggioIndex = 0;

	MemoryClear(m_Arpeggios,sizeof(m_Arpeggios));
	m_iVibratoOffset = 0;
	m_iVibratoDepth = 0;
	m_iCurrentVibration = 0;
	m_iSlide = 0;
	m_iPitchGoal = 0;
	m_iPitchRate = 0;
	m_iVolumeRate = 0;

	MemoryClear(m_PreviousArguments,sizeof(m_PreviousArguments));
	m_uCurrentVolumeEnvelopeIndex = 0;
	m_uNextVolumeEnvelopeIndex = 1;
	m_uCurrentVolumePosition = 0;
	m_uCurrentVolumeFromEnvelope = cMaxVolume;
	m_uCurrentPanEnvelopeIndex = 0;
	m_uNextPanEnvelopeIndex = 1;
	m_uCurrentPanPosition = 0;
	m_uCurrentPanFromEnvelope = 0;
	m_uVolumeFade = 32767;
	m_iAccumulatorFraction = 0;
	m_iLastWordLeft = 0;
	m_iCurrentLastWordLeft = 0;
	m_iLastWordRight = 0;
	m_iCurrentLastWordRight = 0;
	m_uCurrentLevelLeft = 0;
	m_uCurrentLevelRight = 0;
	m_bLevelDirectionLeft = FALSE;
	m_bLevelDirectionRight = FALSE;
	m_bRemoverWorking = FALSE;
	m_bKeyOn = FALSE;
	m_uPreviousVolumeLeft = 1;
	m_uPreviousVolumeRight = 1;
	m_iPreviousOffset = -1;
	m_uEffectPartitionIndex = 0;
	m_uEffectRowIndex = 0;
	m_bEffectActive = FALSE;
	m_bStereo = FALSE;
	m_eLoopType = LOOP_NORMAL;
	m_bPingPongReverse = FALSE;
	m_iPreviousValue1Left = 0;
	m_iPreviousValue2Left = 0;
	m_iPreviousValue1Right = 0;
	m_iPreviousValue2Right = 0;
	m_iPreviousValue3Left = 0;
	m_iPreviousValue4Left = 0;
	m_iPreviousValue3Right	= 0;
	m_iPreviousValue4Right	= 0;
	m_uTickRemoveSize = 1;
}

/*! ************************************

	\brief Purge a channel from playback
	
	Mark a channel as "completed" to stop playback

***************************************/

void BURGER_API Burger::Sequencer::Channel_t::Purge(void)
{
	m_pEchoSample = NULL;		
	m_pCurrent = m_pEndOfSample;
	m_iAccumulatorFraction = 0;
	m_uLoopBeginning = 0;
	m_uLoopSize = 0;
	m_bRemoverWorking = FALSE;
	m_uTickRemoveSize = 1;
}

/*! ************************************

	\brief Execute a volume command
	
	Process any volume commands from the current command

	\param uCall Sub command

***************************************/

void BURGER_API Burger::Sequencer::Channel_t::VolumeCommand(uint_t uCall)
{
	// Get the command
	uint_t uVolumeCommand = m_uVolumeCommand;

	// Only execute if there's a command
	if (uVolumeCommand) {

		// Get the argument
		uint_t uVolumeArgument = uVolumeCommand & 0xFU;
		switch (uVolumeCommand>>4U) {

		//
		// Volume slide down
		//

		case 0x6:
			m_uVolume = Min(static_cast<uint32_t>(
				ClampZero(static_cast<int32_t>(m_uVolume-uVolumeArgument))),static_cast<uint32_t>(cMaxVolume));
			break;

		//
		// Volume slide up
		//
		case 0x7:
			m_uVolume = Min(static_cast<uint32_t>(
				ClampZero(static_cast<int32_t>(m_uVolume+uVolumeArgument))),static_cast<uint32_t>(cMaxVolume));
			break;

		//
		// Fine volume down
		//

		case 0x8:
			// Only sub call 1 is supported
			if (uCall == 1) {
				m_uVolume = Min(static_cast<uint32_t>(ClampZero(static_cast<int32_t>(m_uVolume-uVolumeArgument))),static_cast<uint32_t>(cMaxVolume));
			}
			break;

		//
		// Fine volume up
		//
		case 0x9:
			// Only sub call 1 is supported
			if (uCall == 1) {
				m_uVolume = Min(static_cast<const uint32_t>(ClampZero(static_cast<int32_t>(m_uVolume+uVolumeArgument))),static_cast<uint32_t>(cMaxVolume));
			}
			break;

		//
		// Not supported yet
		//
		//	case 0xa:                       // set vibrato speed
		//	case 0xb:                       // vibrato
		//	case 0xc:                       // set panning

		//
		// Panning slide left
		//
		case 0xd:
			if (uVolumeCommand & 0xFU) {
				m_iPan = static_cast<int32_t>(Min(
					static_cast<uint32_t>(ClampZero(static_cast<int32_t>(m_iPan-(uVolumeArgument/4)))),
					static_cast<uint32_t>(cMaxPan)));
			}
			break;
		
		//
		// Panning slide right
		//
		case 0xe:
			if (uVolumeCommand & 0xFU) {
				m_iPan = static_cast<int32_t>(Min(
					static_cast<uint32_t>(ClampZero(static_cast<int32_t>(m_iPan+(uVolumeArgument/4)))),
					static_cast<uint32_t>(cMaxPan)));
			}
			break;

		//	case 0xf:// tone porta
		}
	}
}


/*! ************************************

	\brief Get the volume slide token

	Sets the volume change rate based on the slide volume
	argument

	\param uArgument Parse slide argument to process
	
***************************************/

void BURGER_API Burger::Sequencer::Channel_t::ParseSlideVolume(uint_t uArgument)
{
	int iTemp = static_cast<int>(uArgument&0x0F);		// Get the low 4 bits
	uArgument>>=4U;
	if (!uArgument) {
		// Negate the volume value
		m_iVolumeRate = -iTemp;
	} else {
		// Use the upper 4 bits
		m_iVolumeRate = static_cast<int32_t>(uArgument);
	}
}

/*! ************************************

	\brief Add constant value to output buffers

	Given a constant value, add it to every other entry to the data buffers

	\param pLeft Pointer to the left channel buffer
	\param pRight Pointer to the right channel buffer
	\param uCount Number of samples in each buffer
	\param iLeft Value to add to every other entry in the left buffer
	\param iRight Value to add to every other entry in the right buffer

***************************************/

void BURGER_API Burger::Sequencer::Channel_t::FillBuffers(int32_t *pLeft,int32_t *pRight,uintptr_t uCount,int32_t iLeft,int32_t iRight)
{
	// Zap the previous pointer for echo
	m_pEchoSample = NULL;
	++uCount;
	do {
		// Get the pair of samples
		int32_t iTempLeft = pLeft[0];
		int32_t iTempRight = pRight[0];
			
		// Add the constants
		iTempLeft = iTempLeft+iLeft;
		iTempRight = iTempRight+iRight;

		// Store the pair
		pLeft[0] = iTempLeft;
		pRight[0] = iTempRight;

		// Process every other entry
		pLeft+=2;
		pRight+=2;
	} while (--uCount);			/* Done? */
}


/*! ************************************

	\brief Remove a channel smoothly

	Iterate over the left and right channels and fade out the sound
	using m_uTickRemoveSize as the period length

	Process every other sample since left and right are interleaved

	\param pLeft Pointer to the left channel buffer
	\param pRight Pointer to the right channel buffer
	\param uCount Number of samples in each buffer
	
***************************************/

void BURGER_API Burger::Sequencer::Channel_t::Tickloop(int32_t *pLeft,int32_t *pRight,uintptr_t uCount)
{
	uint_t uTickRemoveSize = m_uTickRemoveSize;
	
	// Shall I remove something?
	if (uTickRemoveSize && uCount) {

		int32_t iCurrentLastWord = m_iCurrentLastWordLeft;
		uint_t uCurrentLevel = m_uCurrentLevelLeft;
		uintptr_t uTempCount = uCount;

		// Decrement the level?
		if (m_bLevelDirectionLeft) {
			do {
				// Decrement the volume
				if (uCurrentLevel) {
					--uCurrentLevel;
				}
				pLeft[0] = pLeft[0]+((iCurrentLastWord * static_cast<int32_t>(uCurrentLevel)) / static_cast<int32_t>(uTickRemoveSize));
				pLeft += 2;
			} while (--uTempCount);
			if (!uCurrentLevel) {
				m_bRemoverWorking = FALSE;		// Stop this
			}
		} else {

		// Increment the sound level
			do {
				// Tick up until uTickRemoveSize
				if (uCurrentLevel < uTickRemoveSize) {
					++uCurrentLevel;
				}
				pLeft[0] = pLeft[0]+(((iCurrentLastWord * static_cast<int32_t>(uCurrentLevel)) / static_cast<int32_t>(uTickRemoveSize)) - iCurrentLastWord);
				pLeft += 2;
			} while (--uTempCount);
			if (uCurrentLevel >= uTickRemoveSize) {
				m_bRemoverWorking = FALSE;		// Stop this
			}
		}
		// Save the new level
		m_uCurrentLevelLeft = uCurrentLevel;

		// Do the right channel
		iCurrentLastWord = m_iCurrentLastWordRight;
		uCurrentLevel = m_uCurrentLevelRight;
		if (m_bLevelDirectionRight) {
			do {			
				// Decrement the volume
				if (uCurrentLevel) {
					--uCurrentLevel;
				}
				pRight[0] = pRight[0]+((iCurrentLastWord * static_cast<int32_t>(uCurrentLevel)) / static_cast<int32_t>(uTickRemoveSize));
				pRight += 2;
			} while (--uCount);
			if (!uCurrentLevel) {
				m_bRemoverWorking = FALSE;		// Stop this
			}
		} else {
			// Increment the sound level
			do {			
				if (uCurrentLevel < uTickRemoveSize) {
					++uCurrentLevel;
				}
				pRight[0] = pRight[0]+(((iCurrentLastWord * static_cast<int32_t>(uCurrentLevel)) / static_cast<int32_t>(uTickRemoveSize)) - iCurrentLastWord);
				pRight += 2;
			} while (--uCount);
			if (uCurrentLevel >= uTickRemoveSize) {
				m_bRemoverWorking = FALSE;		// Stop this
			}
		}
		m_uCurrentLevelRight = uCurrentLevel;		/* Save the new level */
	}
}

/*! ************************************

	\brief Add a offset to a note and clamp the result

	Add a note shift to a note enumeration and return it clamped
	to be between NOTE_MIN and NOTE_LAST inclusive.

	\param uNote Parse slide argument to process
	\param iOffset Signed value to add to the note
	\return Note enumeration clamped to the valid range
	
***************************************/

Burger::Sequencer::eNote BURGER_API Burger::Sequencer::AddNoteSaturate(eNote uNote,int32_t iOffset)
{
	// Convert less than zero to zero
	uNote = static_cast<eNote>(ClampZero(iOffset+static_cast<int32_t>(uNote)));

	if (uNote>=NOTE_MAX) {
		uNote = NOTE_LAST;
	}
	return uNote;
}


/*! ************************************

	\brief Calculate the sample rate period from a note

	Given a note enumeration and a samples per second fine
	tuning value assuming 8363 is no adjustment, calculate
	the samples per second to play a sample

	\param uNote Note to play
	\param uC2Speed Samples per second to play C2 (C one octave higher than middle C)

***************************************/

uint_t BURGER_API Burger::Sequencer::GetNotePeriod(eNote uNote,uint_t uC2Speed)
{
	uint_t uResult = 4242;		// Default
	if ((uNote != NOTE_UNUSED) &&
		(uNote != NOTE_OFF) &&
		uC2Speed) {
	
		uint_t uOctave = uNote/12U;		// Octave
		uint_t uIndex = uNote-(uOctave*12U);	// Quick modulo for Note

		uResult = ((cAmigaFrequency * g_FrequencyTable[uIndex]) >> uOctave ) / uC2Speed;
		if (!uResult) {		// Failsafe
			uResult = 7242;	// Default
		}
	}
	return uResult;
}



/*! ************************************

	\brief Constructor for the song package

***************************************/

Burger::Sequencer::SongPackage::SongPackage() :
	m_bMusicUnderModification(FALSE)
{
	MemoryClear(m_pSampleDescriptions,sizeof(m_pSampleDescriptions));
	MemoryClear(m_pPartitions,sizeof(m_pPartitions));
	MemoryClear(m_InstrDatas,sizeof(m_InstrDatas));
	m_SongDescription.Clear();
}


/*! ************************************

	\brief Destructor for the song package

***************************************/

Burger::Sequencer::SongPackage::~SongPackage()
{
	Shutdown();
}

/*! ************************************

	\brief Dispose of all resources in the package

	Release all allocated memory and reset the package
	to empty

***************************************/

void BURGER_API Burger::Sequencer::SongPackage::Shutdown(void)
{
	m_bMusicUnderModification = TRUE;

	uintptr_t i = 0;
	do {
		Free(m_pPartitions[i]);
		m_pPartitions[i] = NULL;
	} while (++i<BURGER_ARRAYSIZE(m_pPartitions));

	i = 0;
	do {
		Delete(m_pSampleDescriptions[i]);
		m_pSampleDescriptions[i] = NULL;
	} while (++i<BURGER_ARRAYSIZE(m_pSampleDescriptions));

	MemoryClear(m_InstrDatas,sizeof(m_InstrDatas));
	m_SongDescription.Clear();

	m_bMusicUnderModification = FALSE;
}

/*! ************************************

	\brief Remove an instrument from a song

	\param uInstrumentIndex Which instrument number will be invalidated

***************************************/

void BURGER_API Burger::Sequencer::SongPackage::RemoveInstrument(uint_t uInstrumentIndex)
{
	// Valid instrument index?
	if (uInstrumentIndex<cInstrumentMaxCount) {

		InstrData_t *pInstrData = &m_InstrDatas[uInstrumentIndex];

		// How many samples?
		uint_t uSampleCount = pInstrData->m_uNumberSamples;
		if (uSampleCount) {
			// Dispose of the samples attached to the instrument
			uint_t bOldMusicUnderModification = m_bMusicUnderModification;
			m_bMusicUnderModification = TRUE;
			SampleDescription **ppSampleDescription = &m_pSampleDescriptions[uInstrumentIndex * cSampleMaxCount];
			do {
				Delete(ppSampleDescription[0]);
				ppSampleDescription[0] = NULL;
				++ppSampleDescription;
			} while (--uSampleCount);

			pInstrData->Reset();
			m_bMusicUnderModification = bOldMusicUnderModification;
		}
	}
}

/*! ************************************

	\brief Allocate and initialize a SongPackage

	\return A pointer to a default SongPackage class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::Sequencer::SongPackage * BURGER_API Burger::Sequencer::SongPackage::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(SongPackage))) SongPackage();
}





/*! ************************************

	\brief Constructor for the music sequencer

***************************************/

Burger::Sequencer::Sequencer(SoundManager *pSoundManager) :
	m_pSoundManager(pSoundManager),
#if defined(BURGER_WINDOWS)
	m_pDirectSoundBuffer8(NULL),
	m_pDirectSoundNotify(NULL),
	m_hCallback(NULL),
	m_uCallbackID(0),
#endif
	m_pSongPackage(NULL),
	m_pAccumBuffer32(NULL),
	m_pAccumBuffer16(NULL),
	m_pReverbBuffer(NULL),
	m_uReverbDelayBufferSize(0),
	m_pBuffer(NULL),
	m_uBufferSize(0),
	m_uBytesGenerated(0),
	m_uBytesToGenerate(0),
	m_uChunkSize(0),
	m_uAccumBufferSize(0),
	m_uMicroDelayBufferSize(0),
	m_uImporterCount(0),
	m_uMaxVoices(4),
	m_uMicroDelayDuration(25),
	m_eOutputDataType(SoundManager::TYPESHORT),
	m_uFinalSampleRate(44100),
	m_uReverbSizeMS(100),
	m_uReverbStrength(20),
	m_uVolume(255),
	m_uMasterSpeed(80),
	m_uMasterPitch(80),
	m_uCurrentSpeed(6),
	m_uFineSpeed(125),
	m_uPatternPosition(0),
	m_uCurrentIDPattern(0),
	m_uPartitionPosition(0),
	m_uSpeedCounter(128),
	m_bSurroundEnabled(TRUE),
	m_bReverbEnabled(TRUE),
	m_bTickRemoverEnabled(TRUE),
	m_bRepeatEnabled(TRUE),
	m_bSongIsPlaying(FALSE),
	m_bSequencingInProgress(FALSE),
	m_bPaused(FALSE)
{
	MemoryClear(m_pImporters,sizeof(m_pImporters));
	MemoryClear(m_Channels,sizeof(m_Channels));
#if defined(BURGER_WINDOWS)
	MemoryClear(m_hEvents,sizeof(m_hEvents));
#endif
}

/*! ************************************

	\brief Destructor for the music sequencer

***************************************/

Burger::Sequencer::~Sequencer()
{
	Shutdown();
}

/*! ************************************

	\brief Initialize the music sequencer

***************************************/

uint_t BURGER_API Burger::Sequencer::Init(void)
{
	Shutdown();

	m_pSongPackage = NULL;
	m_bSequencingInProgress = FALSE;
	ClearSequencer();
	m_uSpeedCounter = 128;			// Start immediately
	m_uBytesGenerated = 0;
	m_uBytesToGenerate = 0;
	m_uCurrentSpeed = 6;
	m_uFineSpeed = 125;
	m_uMasterSpeed = 80;
	m_uMasterPitch = 80;

	ClearChannels();
	m_uAccumBufferSize = 7500;
	uint_t uResult = CreateDriverBuffer();
	if (!uResult) {
		// Create the timing
		m_uChunkSize = (m_uFinalSampleRate * 125) / 50;
		uResult = CreateReverbTables();
		if (!uResult) {
			uResult = CreateVolumeBuffer();
			if (!uResult) {
				uResult = PlatformInit();
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Dispose of everything in the sequencer

***************************************/

void BURGER_API Burger::Sequencer::Shutdown(void)
{
	// If there is a song, stop it
	DisposeSong();				
	m_bSequencingInProgress = FALSE;
	ClearChannels();
	DisposeDriverBuffer();
	DisposeVolumeBuffer();
	DisposeReverbTables();
	PlatformShutdown();
}


/*! ************************************

	\brief Add a song file importer

	Add a function pointer to the list of file format importers
	so when a song is attempted to be played, there is a method
	for extracting the notes into Burgerlib format

	\param pImportProc Pointer to a function that will parse a song file
	\return Zero if successful, non zero if there table is full.

***************************************/

uint_t BURGER_API Burger::Sequencer::AddImporter(ImportProc pImportProc)
{
	uint_t uResult = 10;
	uint_t uCount = m_uImporterCount;
	// Not full?
	if (uCount<cMaxPlugIns) {
		// Add to the list
		m_pImporters[uCount] = pImportProc;
		++uCount;
		m_uImporterCount=uCount;

		// All good!
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Import a song

	Given a song file in memory, try all the song importers
	until one accepts the file format and converts the
	song into a SongPackage

	\param ppOutput Pointer to a pointer that will have a new SongPackage if parsing is successful
	\param pInput Pointer to a valid song file
	\param uInputLength Length of the valid song file in bytes
	\return Zero if successful, non zero if there was an error

***************************************/

uint_t BURGER_API Burger::Sequencer::ImportSong(SongPackage **ppOutput,const uint8_t *pInput,uintptr_t uInputLength)
{
	// Clear the output pointer
	ppOutput[0] = NULL;

	uint_t uResult = 10;
	uint_t uCount = m_uImporterCount;
	if (uCount) {
		// Allocate the record
		SongPackage *pSong = SongPackage::New();
		if (pSong) {
			ImportProc *ppImporters = m_pImporters;
			do {
				// Use this porter
				uResult = ppImporters[0](pSong,pInput,uInputLength);
				// Processed fine
				if (uResult==IMPORT_OKAY) {
					break;
				}
				// Recognized, but there was an error
				if (uResult!=IMPORT_UNKNOWN) {
					break;
				}
				// Try the next one
				++ppImporters;
			} while (--uCount);
			if (uResult!=IMPORT_OKAY) {
				Delete(pSong);
			} else {
				// Return the pointer to the caller
				ppOutput[0] = pSong;
			}
		}
	}
	// Return the result
	return uResult;
}

/*! ************************************

	\brief Set the volume of the song
	
	Set the volume for song playback, 0 = silence,
	255 = maximum volume

	\param uVolume New volume for the song
	\sa GetVolume(void) const

***************************************/

void BURGER_API Burger::Sequencer::SetVolume(uint_t uVolume)
{
	if (uVolume>=256U) {
		uVolume = 255U;
	}
	m_uVolume = uVolume;
}

/*! ************************************

	\brief Create the reverb table
	\return Zero is no error, non-zero means out of memory

***************************************/

uint_t BURGER_API Burger::Sequencer::CreateReverbTables(void)
{
	// Dispose of any previous tables
	DisposeReverbTables();

	uint_t uResult = 0;
	if (m_bReverbEnabled) {
		// Get the buffer size from MS * sample rate
		uintptr_t uBufferSize = (m_uReverbSizeMS * m_uFinalSampleRate) / 1000U;
		m_uReverbDelayBufferSize = uBufferSize;
		void *pBuffer = NULL;
		switch (m_eOutputDataType) {
		case SoundManager::TYPECHAR:
		case SoundManager::TYPEBYTE:
			// *2 for stereo
			pBuffer = Alloc(uBufferSize*2);
			if (pBuffer) {
				// Initialize to unsigned byte
				MemoryFill(pBuffer,0x80U,uBufferSize*2);
			}
			break;
		default:
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPEBSHORT:
			// *4 for short / Stereo
			pBuffer = AllocClear(uBufferSize*4);
			break;
		}
		// Error?
		if (!pBuffer) {
			uResult = 10;
		}
		m_pReverbBuffer = pBuffer;
	}
	return uResult;
}

/*! ************************************

	\brief Get rid of the reverb tables

***************************************/

void BURGER_API Burger::Sequencer::DisposeReverbTables(void)
{
	Free(m_pReverbBuffer);
	m_pReverbBuffer = NULL;
}

/*! ************************************

	\brief Create the main sound buffer
	\return Zero is no error, non-zero means out of memory

***************************************/

uint_t BURGER_API Burger::Sequencer::CreateDriverBuffer(void)
{
	// Make sure the previous buffer is released
	DisposeDriverBuffer();
    // Double for stereo
    uintptr_t uBufferSize = m_uAccumBufferSize * 2;
	if ((m_eOutputDataType!=SoundManager::TYPECHAR) && 
		(m_eOutputDataType!=SoundManager::TYPEBYTE)) {
		// Double again for shorts
		uBufferSize = uBufferSize*2;
	}

	//
	// Get the buffer
	//
	uint_t uResult = 10;
	m_pBuffer = Alloc(uBufferSize);
	if (m_pBuffer) {
		m_uBufferSize = uBufferSize;
		uResult = 0;		// No error
	}
	return uResult;
}

/*! ************************************

	\brief Get rid of the main sound buffer

***************************************/

void BURGER_API Burger::Sequencer::DisposeDriverBuffer(void)
{
	Free(m_pBuffer);
	m_pBuffer = NULL;
}

/*! ************************************

	\brief Create the main accumulation buffer
	\return Zero is no error, non-zero means out of memory

***************************************/

uint_t BURGER_API Burger::Sequencer::CreateVolumeBuffer(void)
{
	// Make sure the previous buffers were cleared out
	DisposeVolumeBuffer();

	// Get the size of the micro delay buffer
	m_uMicroDelayBufferSize = (m_uMicroDelayDuration * m_uFinalSampleRate) / 1000U;

	void *pBuffer = NULL;
	switch (m_eOutputDataType) {
	case SoundManager::TYPECHAR:
	case SoundManager::TYPEBYTE:
		pBuffer = AllocClear((m_uAccumBufferSize*4) + (m_uMicroDelayBufferSize*2*4));
		if (pBuffer) {
			m_pAccumBuffer16 = static_cast<int16_t *>(pBuffer);
		}
		break;

	default:
	case SoundManager::TYPELSHORT:
	case SoundManager::TYPEBSHORT:
		pBuffer = AllocClear((m_uAccumBufferSize*8) + (m_uMicroDelayBufferSize*2*8));
		if (pBuffer) {
			m_pAccumBuffer32 = static_cast<int32_t *>(pBuffer);
		}
		break;
	}
	// Was there an error?
	uint_t uResult = 0;
	if (!pBuffer) {
		uResult = 10;
	}
	return uResult;
}

/*! ************************************

	\brief Get rid of the accumulator buffers

***************************************/

void BURGER_API Burger::Sequencer::DisposeVolumeBuffer(void)
{
	Free(m_pAccumBuffer32);
	m_pAccumBuffer32 = NULL;
	Free(m_pAccumBuffer16);
	m_pAccumBuffer16 = NULL;
}

/*! ************************************

	\brief Process the fading of a buffer

	\param pChannel Pointer to a channel to process
	\param pLeft Pointer to the left audio buffer
	\param pRight Pointer to the right audio buffer

***************************************/

void BURGER_API Burger::Sequencer::ProcessTick(Channel_t *pChannel,int32_t *pLeft,int32_t *pRight)
{
	// Get the channel volumes
	uint_t uVolumeLeft = CalculateVolume(pChannel,0);
	uint_t uVolumeRight = CalculateVolume(pChannel,1);

	// Do the volumes need updating?
	if ((pChannel->m_pEchoSample != pChannel->m_pBeginningOfSample) ||
		((pChannel->m_pCurrent >= pChannel->m_pEndOfSample) && (!pChannel->m_uLoopSize)) ||
		(pChannel->m_uPreviousVolumeLeft != uVolumeLeft) ||
		(pChannel->m_uPreviousVolumeRight != uVolumeRight)) {

			pChannel->m_bLevelDirectionLeft = TRUE;
			pChannel->m_bLevelDirectionRight = TRUE;

			// No overrides?
			uint_t bOverride = FALSE;

			// Right Channel
			if ((pChannel->m_uPreviousVolumeLeft != uVolumeLeft) && 
				(pChannel->m_pEchoSample == pChannel->m_pBeginningOfSample)) {
					int_t iDifference = static_cast<int_t>(pChannel->m_uPreviousVolumeLeft - uVolumeLeft);
					if (iDifference > 0) {
						// Don't divide by zero
						if (pChannel->m_uPreviousVolumeLeft) {
							pChannel->m_iLastWordRight -= (pChannel->m_iLastWordRight * static_cast<int32_t>(uVolumeLeft)) / static_cast<int32_t>(pChannel->m_uPreviousVolumeLeft);
						}
						pChannel->m_uPreviousVolumeLeft	= uVolumeLeft;
						bOverride = TRUE;
					} else if (iDifference < 0) {
						// Don't divide by zero
						if (pChannel->m_uPreviousVolumeLeft) {
							pChannel->m_iLastWordRight -= (pChannel->m_iLastWordRight * static_cast<int32_t>(uVolumeLeft)) / static_cast<int32_t>(pChannel->m_uPreviousVolumeLeft);
						}
						pChannel->m_iLastWordRight = -pChannel->m_iLastWordRight;
						pChannel->m_uPreviousVolumeLeft = uVolumeLeft;
						pChannel->m_bLevelDirectionRight = FALSE;
						bOverride = TRUE;
					}
			} else if (pChannel->m_pEchoSample == pChannel->m_pBeginningOfSample) {
				pChannel->m_iLastWordRight = 0;
			}

			// Left Channel
			if ((pChannel->m_uPreviousVolumeRight != uVolumeRight) &&
				(pChannel->m_pEchoSample == pChannel->m_pBeginningOfSample)) {
					int_t iDifference = static_cast<int_t>(pChannel->m_uPreviousVolumeRight - uVolumeRight);
					if (iDifference > 0) {
						// Don't divide by zero
						if (pChannel->m_uPreviousVolumeRight) {
							pChannel->m_iLastWordLeft -= (pChannel->m_iLastWordLeft * static_cast<int32_t>(uVolumeRight)) / static_cast<int32_t>(pChannel->m_uPreviousVolumeRight);
						}
						pChannel->m_uPreviousVolumeRight = uVolumeRight;
						bOverride = TRUE;
					} else if (iDifference < 0) {
						// Don't divide by zero
						if (pChannel->m_uPreviousVolumeRight) {
							pChannel->m_iLastWordLeft -= (pChannel->m_iLastWordLeft * static_cast<int32_t>(uVolumeRight)) / static_cast<int32_t>(pChannel->m_uPreviousVolumeRight);
						}
						pChannel->m_iLastWordLeft = -pChannel->m_iLastWordLeft;
						pChannel->m_uPreviousVolumeRight = uVolumeRight;
						pChannel->m_bLevelDirectionLeft = FALSE;
						bOverride = TRUE;
					}
			} else if (pChannel->m_pEchoSample == pChannel->m_pBeginningOfSample) {
				pChannel->m_iLastWordLeft = 0;
			}

			if (pChannel->m_iLastWordLeft || 
				pChannel->m_iLastWordRight || 
				bOverride) {
					uint_t uTickRemoveSize = static_cast<uint_t>(((80*m_uChunkSize)/m_uFineSpeed)/m_uMasterSpeed);
					pChannel->m_uTickRemoveSize = uTickRemoveSize;
					if (pChannel->m_bLevelDirectionRight) {
						pChannel->m_uCurrentLevelRight = uTickRemoveSize;
					} else {
						pChannel->m_uCurrentLevelRight = 0;
					}
					if (pChannel->m_bLevelDirectionLeft) {
						pChannel->m_uCurrentLevelLeft = uTickRemoveSize;
					} else {
						pChannel->m_uCurrentLevelLeft = 0;
					}
					pChannel->m_iCurrentLastWordRight = pChannel->m_iLastWordRight;
					pChannel->m_iCurrentLastWordLeft = pChannel->m_iLastWordLeft;
					pChannel->m_iLastWordRight = 0;
					pChannel->m_iLastWordLeft = 0;	
					pChannel->m_bRemoverWorking = TRUE;
			}
			pChannel->m_pEchoSample = pChannel->m_pBeginningOfSample;
			pChannel->m_uPreviousVolumeLeft = uVolumeLeft;
			pChannel->m_uPreviousVolumeRight = uVolumeRight;
	}

	// Shall I remove ticks?
	if (pChannel->m_bRemoverWorking) {
		// Perform the removal
		pChannel->Tickloop(pLeft,pRight,m_uAccumBufferSize);
	}
}

/*! ************************************

	\brief Determine the starting speed of a song

	Parse the patterns from the end and locate the speed
	and fine speed commands and set the initial speeds to 
	those values to allow the looped speed
	commands to be effective

***************************************/

void BURGER_API Burger::Sequencer::DetermineSpeed(void)
{
	uint_t bCommandSpeedFound = FALSE;
	uint_t bFineSpeedFound = FALSE;
	const SongPackage *pSongPackage = m_pSongPackage;

	uint_t uPatternCount = m_uPartitionPosition+1;
	do {
		uint_t uPatternPosition;
		uint_t i = uPatternCount-1;
		PatternData_t *pPatternData = pSongPackage->m_pPartitions[pSongPackage->m_SongDescription.m_PatternPointers[i]];
		if (i == static_cast<int>(m_uPartitionPosition)) {
			uPatternPosition = static_cast<uint_t>(i);
			if (uPatternPosition >= pPatternData->m_uRowCount) {
				--uPatternPosition;
			}
		} else {
			uPatternPosition = pPatternData->m_uRowCount-1;
		}	

		int x;
		for (x = static_cast<int>(uPatternPosition); x >= 0; x--) {
			int y;
			for (y = static_cast<int>(pSongPackage->m_SongDescription.m_uChannelCount-1); y >= 0 ; y--) {
				const Command_t *pCommand = pPatternData->GetCommand(x,y);

				// Is this a speed command?
				if (pCommand->m_uEffectCommand == Command_t::EFFECT_SPEED) {
					// Is this a song speed command?
					if (pCommand->m_uEffectArgument < 32) {
						// Already found it?
						if (!bCommandSpeedFound) {
							// Do the final test
							if (pCommand->m_uEffectArgument) {
								// Accept the speed
								m_uCurrentSpeed = pCommand->m_uEffectArgument;
								bCommandSpeedFound = TRUE;
								// Found both already?
								if (bFineSpeedFound) {
									return;
								}
							}
						}
					} else if (!bFineSpeedFound) {
						// Set the new fine speed
						m_uFineSpeed = pCommand->m_uEffectArgument;
						bFineSpeedFound = TRUE;
						// Found both already?
						if (bCommandSpeedFound) {
							return;
						}
					}
				}
			}
		}
	} while (--uPatternCount);

	// Use the defaults for those not found

	if (!bCommandSpeedFound) {
		m_uCurrentSpeed = pSongPackage->m_SongDescription.m_uDefaultSpeed;
	}
	if (!bFineSpeedFound) {
		m_uFineSpeed = pSongPackage->m_SongDescription.m_uDefaultTempo;
	}
}

/*! ************************************

	\brief Prepare the sequencer to start music

***************************************/

void BURGER_API Burger::Sequencer::PrepareSequencer(void)
{
	m_bSongIsPlaying = TRUE;
	m_bSequencingInProgress = FALSE;
	ClearChannels();
	DetermineSpeed();
}

/*! ************************************

	\brief Clear the sequencer from music playback

***************************************/

void BURGER_API Burger::Sequencer::ClearSequencer(void)
{
	m_bSongIsPlaying = FALSE;
	ClearChannels();
}

/*! ************************************

	\brief Set up the output channel count

***************************************/

void BURGER_API Burger::Sequencer::SetChannelCount(void)
{
	const SongPackage *pSongPackage = m_pSongPackage;
	if (pSongPackage) {
		uint_t uChannels = pSongPackage->m_SongDescription.m_uChannelCount;
		if (uChannels != m_uMaxVoices) {

			uint_t bSongIsPlaying = m_bSongIsPlaying;
			uint_t bSequencingInProgress = m_bSequencingInProgress;

			ClearSequencer();
			m_uMaxVoices = uChannels;
			CreateVolumeBuffer();
			if (bSongIsPlaying) {
				PrepareSequencer();
			}
			if (bSequencingInProgress) {
				StartSequencing();
			}
		}
	}
}

/*! ************************************

	\brief Add in a channel's 8 bit sample into a 32 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample8To32AddDelay(Channel_t *pChannel,int32_t *pAccumBuffer)
{
	uint_t uLeftVolume = CalculateVolume(pChannel,0);
	uint_t uRightVolume = CalculateVolume(pChannel,1);
	
	// Delay offsets in stereo
	uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
	int32_t *pLeft = pAccumBuffer;
	int32_t *pRight = pAccumBuffer + 1;
	// Even channels are right
	if (!(pChannel->m_uID&1)) {	
		pRight = pRight + uDelayOffset;
	} else {
		pLeft = pLeft + uDelayOffset;
	}

	if (m_bTickRemoverEnabled) {
		ProcessTick(pChannel,pLeft,pRight);
	}

	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;

		// Starting sample
		int iSampleValue = 0;
		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse && 
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}
		int32_t iAccumulatorInteger = 0;
		if (uAccumBufferSize) {
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if (((&pCurrent[iAccumulatorInteger+1] >= pChannel->m_pEndOfSample) && !pChannel->m_bPingPongReverse) ||
							((&pCurrent[iAccumulatorInteger+1] <= (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)) && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
						}
						iPreviousValue1Left = pCurrent[iAccumulatorInteger];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousOffset = iAccumulatorInteger;
						if (&pCurrent[iAccumulatorInteger+1] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift);
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)-1;
							} else {
								pChannel->FillBuffers(pLeft,pRight,uAccumBufferSize,iSampleValue*static_cast<int32_t>(uRightVolume),iSampleValue*static_cast<int32_t>(uLeftVolume));
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[iAccumulatorInteger+1];
					}
				}

				iSampleValue = ((iLeftWeight*iPreviousValue1Left) + (iRightWeight * pCurrent[iAccumulatorInteger+1])) >> cFixedPointShift;
				iAccumulatorFraction += uStepValue;
				pLeft[0] += (iSampleValue * static_cast<int32_t>(uRightVolume));
				pLeft += 2;
				pRight[0] += (iSampleValue * static_cast<int32_t>(uLeftVolume));
				pRight += 2;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {

			if ((iAccumulatorFraction>>cFixedPointShift) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue2Left = pCurrent[iAccumulatorInteger+1];
			pChannel->m_pCurrent = &pCurrent[iAccumulatorFraction>>cFixedPointShift];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
		pChannel->m_iLastWordLeft = (iSampleValue * static_cast<int32_t>(uRightVolume));
		pChannel->m_iLastWordRight = (iSampleValue * static_cast<int32_t>(uLeftVolume));
	}
}

/*! ************************************

	\brief Add in a channel's 16 bit sample into a 32 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample16To32AddDelay(Channel_t *pChannel,int32_t *pAccumBuffer)
{
	uint_t uLeftVolume = CalculateVolume(pChannel,0);
	uint_t uRightVolume = CalculateVolume(pChannel,1);

	// Delay offsets in stereo
	uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
	int32_t *pLeft = pAccumBuffer;
	int32_t *pRight = pAccumBuffer + 1;
	// Even channels are right
	if (!(pChannel->m_uID&1)) {	
		pRight = pRight + uDelayOffset;
	} else {
		pLeft = pLeft + uDelayOffset;
	}

	if (m_bTickRemoverEnabled) {
		ProcessTick(pChannel,pLeft,pRight);
	}

	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		const int16_t *pCurrent = static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pCurrent));
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue3Left = pChannel->m_iPreviousValue3Left;
		int32_t iPreviousValue4Left = pChannel->m_iPreviousValue4Left;

		// Starting sample
		int iSampleValue = 0;
		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse && 
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

		int32_t iAccumulatorInteger = 0;
		if (uAccumBufferSize) {
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift);

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[iAccumulatorInteger+1] >= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pEndOfSample)) && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[iAccumulatorInteger+1] <= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)) && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
						}
						iPreviousValue3Left = pCurrent[iAccumulatorInteger];
					} else {
						iPreviousValue3Left = iPreviousValue4Left;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[iAccumulatorInteger+1] >= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pEndOfSample))) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning))-1;
							} else {
								pChannel->FillBuffers(pLeft,pRight,uAccumBufferSize,(iSampleValue*static_cast<int32_t>(uRightVolume))>>8,(iSampleValue*static_cast<int32_t>(uLeftVolume))>>8);
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue4Left = pCurrent[iAccumulatorInteger+1];
					}
				}

				iSampleValue = ((iLeftWeight*iPreviousValue3Left) + (iRightWeight * pCurrent[iAccumulatorInteger+1])) >> cFixedPointShift;
				iAccumulatorFraction += uStepValue;
				pLeft[0] += (iSampleValue * static_cast<int32_t>(uRightVolume)) >> 8;
				pLeft += 2;
				pRight[0] += (iSampleValue * static_cast<int32_t>(uLeftVolume)) >> 8;
				pRight += 2;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {

			if ((iAccumulatorFraction>>cFixedPointShift) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;	
			}
			pChannel->m_iPreviousValue3Left = iPreviousValue3Left;
			pChannel->m_iPreviousValue4Left = pCurrent[iAccumulatorInteger + 1];
			pChannel->m_pCurrent = static_cast<const char *>(static_cast<const void *>(&pCurrent[iAccumulatorFraction>>cFixedPointShift]));
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
		pChannel->m_iLastWordLeft = (iSampleValue * static_cast<int32_t>(uRightVolume)) >> 8;
		pChannel->m_iLastWordRight = (iSampleValue * static_cast<int32_t>(uLeftVolume)) >> 8;
	}
}

/*! ************************************

	\brief Add in a channel's 8 bit sample into a 16 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample8To16AddDelay(Channel_t *pChannel,int16_t *pAccumBuffer)
{
	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		uint_t uLeftVolume = CalculateVolume(pChannel,0);
		uint_t uRightVolume = CalculateVolume(pChannel,1);

		// Delay offsets in stereo
		uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
		int16_t *pLeft = pAccumBuffer;
		int16_t *pRight = pAccumBuffer + 1;
		// Even channels are right
		if (!(pChannel->m_uID&1)) {	
			pRight = pRight + uDelayOffset;
		} else {
			pLeft = pLeft + uDelayOffset;
		}

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;

		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse &&
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

		int32_t iAccumulatorInteger=0;
		if (uAccumBufferSize) {	
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
							iPreviousOffset = iAccumulatorInteger;
							if ((&pCurrent[iAccumulatorInteger+1] >= pChannel->m_pEndOfSample && !pChannel->m_bPingPongReverse) ||
								(&pCurrent[iAccumulatorInteger+1] <= pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning && pChannel->m_bPingPongReverse)) {
								pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
								iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
								uStepValue = 0-uStepValue;
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
							}
							iPreviousValue1Left = pCurrent[iAccumulatorInteger];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[iAccumulatorInteger+1] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)-1;
							} else {
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[iAccumulatorInteger+1];
					}
				}

				int iSampleValue = ((iLeftWeight * iPreviousValue1Left) + (iRightWeight * pCurrent[iAccumulatorInteger+1])) >> cFixedPointShift;
				iAccumulatorFraction += uStepValue;

				pLeft[0] = static_cast<int16_t>(pLeft[0]+((iSampleValue*static_cast<int32_t>(uRightVolume))>>8));
				pLeft += 2;
				pRight[0] = static_cast<int16_t>(pRight[0]+((iSampleValue*static_cast<int32_t>(uLeftVolume))>>8));
				pRight += 2;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?			
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {

			if ((iAccumulatorFraction>>cFixedPointShift) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue2Left = pCurrent[iAccumulatorInteger + 1];
			pChannel->m_pCurrent = &pCurrent[iAccumulatorFraction>>cFixedPointShift];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
	}
}

/*! ************************************

	\brief Add in a channel's 16 bit sample into a 16 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample16To16AddDelay(Channel_t *pChannel,int16_t *pAccumBuffer)
{
	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		uint_t uLeftVolume = CalculateVolume(pChannel,0);
		uint_t uRightVolume = CalculateVolume(pChannel,1);

		// Delay offsets in stereo
		uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
		int16_t *pLeft = pAccumBuffer;
		int16_t *pRight = pAccumBuffer + 1;
		// Even channels are right
		if (!(pChannel->m_uID&1)) {	
			pRight = pRight + uDelayOffset;
		} else {
			pLeft = pLeft + uDelayOffset;
		}

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;

		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse &&
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

#if defined(BURGER_LITTLEENDIAN)
		++pCurrent;				// Hack for getting the high byte
#endif

		int32_t iAccumulatorInteger=0;
		if (uAccumBufferSize) {
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[(iAccumulatorInteger*2)+2] >= pChannel->m_pEndOfSample && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[(iAccumulatorInteger*2)+2] <= (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning) && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
						}
						iPreviousValue1Left = pCurrent[iAccumulatorInteger*2];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousOffset = iAccumulatorInteger;
						if (&pCurrent[(iAccumulatorInteger*2)+2] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = iAccumulatorFraction>>cFixedPointShift;
								iPreviousOffset = iAccumulatorInteger;

								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)-2;
#if defined(BURGER_LITTLEENDIAN)
								++pCurrent;				// Hack for getting the high byte
#endif
							} else {
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[(iAccumulatorInteger*2)+2];
					}
				}

				int iSampleValue = ((iLeftWeight * iPreviousValue1Left) + (iRightWeight * pCurrent[(iAccumulatorInteger*2)+2])) >> cFixedPointShift;
				iAccumulatorFraction += uStepValue;

				pLeft[0] = static_cast<int16_t>(pLeft[0]+((iSampleValue*static_cast<int32_t>(uRightVolume)) >> 8));
				pLeft += 2;
				pRight[0] = static_cast<int16_t>(pRight[0]+((iSampleValue*static_cast<int32_t>(uLeftVolume)) >> 8));
				pRight += 2;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?			
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {
			if ((iAccumulatorFraction>>cFixedPointShift) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;	
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue2Left = pCurrent[(iAccumulatorInteger*2)+2];
#if defined(BURGER_LITTLEENDIAN)
			--pCurrent;				// Hack for getting the high byte
#endif
			pChannel->m_pCurrent = &pCurrent[(iAccumulatorFraction>>cFixedPointShift)*2];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
	}
}

/*! ************************************

	\brief Add in a channel's 8 bit stereo sample into a 32 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample8To32AddDelayStereo(Channel_t *pChannel,int32_t *pAccumBuffer)
{
	uint_t uLeftVolume = CalculateVolume(pChannel,0);
	uint_t uRightVolume = CalculateVolume(pChannel,1);

	// Delay offsets in stereo
	uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
	int32_t *pLeft = pAccumBuffer;
	int32_t *pRight = pAccumBuffer + 1;
	// Even channels are right
	if (!(pChannel->m_uID&1)) {	
		pRight = pRight + uDelayOffset;
	} else {
		pLeft = pLeft + uDelayOffset;
	}

	if (m_bTickRemoverEnabled) {
		ProcessTick(pChannel,pLeft,pRight);
	}

	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue1Right = pChannel->m_iPreviousValue1Right;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;
		int32_t iPreviousValue2Right = pChannel->m_iPreviousValue2Right;

		// Starting samples
		int iSampleValueRight = 0;
		int iSampleValueLeft = 0;
		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse &&
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

		int32_t iAccumulatorInteger=0;
		if (uAccumBufferSize) {
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[iAccumulatorInteger+3] >= pChannel->m_pEndOfSample && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[iAccumulatorInteger+2] <= (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning) && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
						}
						iPreviousValue1Left = pCurrent[iAccumulatorInteger];
						iPreviousValue1Right = pCurrent[iAccumulatorInteger+1];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousValue1Right = iPreviousValue2Right;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[iAccumulatorInteger+3] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning) - 2;
							} else {
								pChannel->FillBuffers(pLeft,pRight,uAccumBufferSize,iSampleValueLeft*static_cast<int32_t>(uRightVolume),iSampleValueRight*static_cast<int32_t>(uLeftVolume));
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[iAccumulatorInteger+2];
						iPreviousValue2Right = pCurrent[iAccumulatorInteger+3];
					}
				}

				iSampleValueLeft = ((iLeftWeight * iPreviousValue1Left) + (iRightWeight * pCurrent[iAccumulatorInteger+2]))>>cFixedPointShift;
				pLeft[0] += (iSampleValueLeft * static_cast<int32_t>(uRightVolume));
				pLeft += 2;
				iSampleValueRight = ((iLeftWeight * iPreviousValue1Right) + (iRightWeight * pCurrent[iAccumulatorInteger+3]))>>cFixedPointShift;
				pRight[0] += (iSampleValueRight * static_cast<int32_t>(uLeftVolume));
				pRight += 2;
				iAccumulatorFraction += uStepValue;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {
			if (((iAccumulatorFraction>>cFixedPointShift)*2) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue1Right = iPreviousValue1Right;
			pChannel->m_iPreviousValue2Left = pCurrent[iAccumulatorInteger+2];
			pChannel->m_iPreviousValue2Right = pCurrent[iAccumulatorInteger+3];
			pChannel->m_pCurrent = &pCurrent[(iAccumulatorFraction>>cFixedPointShift)*2];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
		pChannel->m_iLastWordLeft = (iSampleValueLeft * static_cast<int32_t>(uRightVolume));
		pChannel->m_iLastWordRight = (iSampleValueRight * static_cast<int32_t>(uLeftVolume));
	}
}

/*! ************************************

	\brief Add in a channel's 16 bit stereo sample into a 32 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample16To32AddDelayStereo(Channel_t *pChannel,int32_t *pAccumBuffer)
{
	uint_t uLeftVolume = CalculateVolume(pChannel,0);
	uint_t uRightVolume = CalculateVolume(pChannel,1);

	// Delay offsets in stereo
	uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
	int32_t *pLeft = pAccumBuffer;
	int32_t *pRight = pAccumBuffer + 1;
	// Even channels are right
	if (!(pChannel->m_uID&1)) {	
		pRight = pRight + uDelayOffset;
	} else {
		pLeft = pLeft + uDelayOffset;
	}

	if (m_bTickRemoverEnabled) {
		ProcessTick(pChannel,pLeft,pRight);
	}

	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		const int16_t *pCurrent = static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pCurrent));
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue3Left = pChannel->m_iPreviousValue3Left;
		int32_t iPreviousValue3Right = pChannel->m_iPreviousValue3Right;
		int32_t iPreviousValue4Left = pChannel->m_iPreviousValue4Left;
		int32_t iPreviousValue4Right = pChannel->m_iPreviousValue4Right;

		// Starting samples
		int iSampleValueRight = 0;
		int iSampleValueLeft = 0;
		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse &&
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

		int32_t iAccumulatorInteger=0;
		if (uAccumBufferSize) {
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[iAccumulatorInteger+3] >= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pEndOfSample)) && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[iAccumulatorInteger+2] <= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)) && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
						}
						iPreviousValue3Left = pCurrent[iAccumulatorInteger];
						iPreviousValue3Right = pCurrent[iAccumulatorInteger+1];
					} else {
						iPreviousValue3Left = iPreviousValue4Left;
						iPreviousValue3Right = iPreviousValue4Right;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[iAccumulatorInteger + 3] >= static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pEndOfSample))) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = static_cast<const int16_t *>(static_cast<const void *>(pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning))-2;
							} else {
								pChannel->FillBuffers(pLeft,pRight,uAccumBufferSize,(iSampleValueLeft*static_cast<int32_t>(uRightVolume))>>8,(iSampleValueRight*static_cast<int32_t>(uLeftVolume))>>8);
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue4Left = pCurrent[iAccumulatorInteger+2];
						iPreviousValue4Right = pCurrent[iAccumulatorInteger+3];
					}
				}

				iSampleValueLeft = ((iLeftWeight * iPreviousValue3Left) + (iRightWeight * pCurrent[iAccumulatorInteger+2])) >> cFixedPointShift;
				pLeft[0] += (iSampleValueLeft * static_cast<int32_t>(uRightVolume)) >> 8;
				pLeft += 2;
				iSampleValueRight = ((iLeftWeight * iPreviousValue3Right) + (iRightWeight * pCurrent[iAccumulatorInteger+3])) >> cFixedPointShift;
				pRight[0] += (iSampleValueRight * static_cast<int32_t>(uLeftVolume)) >> 8;
				pRight += 2;
				iAccumulatorFraction += uStepValue;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {
			if (((iAccumulatorFraction>>cFixedPointShift)*2) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue3Left = iPreviousValue3Left;
			pChannel->m_iPreviousValue3Right = iPreviousValue3Right;
			pChannel->m_iPreviousValue4Left = pCurrent[iAccumulatorInteger+2];
			pChannel->m_iPreviousValue4Right = pCurrent[iAccumulatorInteger+3];
			pChannel->m_pCurrent = static_cast<const char *>(static_cast<const void *>(pCurrent + (iAccumulatorFraction>>cFixedPointShift)*2));
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
		pChannel->m_iLastWordLeft = (iSampleValueLeft * static_cast<int32_t>(uRightVolume)) >> 8;
		pChannel->m_iLastWordRight = (iSampleValueRight * static_cast<int32_t>(uLeftVolume)) >> 8;
	}
}

/*! ************************************

	\brief Add in a channel's 8 bit stereo sample into a 16 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample8To16AddDelayStereo(Channel_t *pChannel,int16_t *pAccumBuffer)
{
	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		uint_t uLeftVolume = CalculateVolume(pChannel,0);
		uint_t uRightVolume	= CalculateVolume(pChannel,1);

		// Delay offsets in stereo
		uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
		int16_t *pLeft = pAccumBuffer;
		int16_t *pRight = pAccumBuffer + 1;
		// Even channels are right
		if (!(pChannel->m_uID&1)) {	
			pRight = pRight + uDelayOffset;
		} else {
			pLeft = pLeft + uDelayOffset;
		}

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue1Right = pChannel->m_iPreviousValue1Right;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;
		int32_t iPreviousValue2Right = pChannel->m_iPreviousValue2Right;

		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse &&
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

		int32_t iAccumulatorInteger=0;
		if (uAccumBufferSize) {			
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[iAccumulatorInteger+3] >= pChannel->m_pEndOfSample && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[iAccumulatorInteger+2] <= pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
						}
						iPreviousValue1Left = pCurrent[iAccumulatorInteger];
						iPreviousValue1Right = pCurrent[iAccumulatorInteger+1];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousValue1Right = iPreviousValue2Right;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[iAccumulatorInteger+3] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize) {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)-2;
							} else {
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[iAccumulatorInteger+2];
						iPreviousValue2Right = pCurrent[iAccumulatorInteger+3];
					}
				}

				int iSampleValueLeft = ((iLeftWeight*iPreviousValue1Left) + (iRightWeight*pCurrent[iAccumulatorInteger+2])) >> cFixedPointShift;
				pLeft[0] = static_cast<int16_t>(pLeft[0]+((iSampleValueLeft*static_cast<int32_t>(uRightVolume))>>8));
				pLeft += 2;
				int iSampleValueRight = ((iLeftWeight*iPreviousValue1Right) + (iRightWeight*pCurrent[iAccumulatorInteger+3])) >> cFixedPointShift;
				pRight[0] = static_cast<int16_t>(pRight[0]+((iSampleValueRight*static_cast<int32_t>(uLeftVolume))>>8));
				pRight += 2;

				iAccumulatorFraction += uStepValue;
			} while (--uAccumBufferSize);
		}			

		// Did it reach the end of the data?			
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {
			if (((iAccumulatorFraction>>cFixedPointShift)*2) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue1Right = iPreviousValue1Right;
			pChannel->m_iPreviousValue2Left = pCurrent[iAccumulatorInteger+2];
			pChannel->m_iPreviousValue2Right = pCurrent[iAccumulatorInteger+3];
			pChannel->m_pCurrent = &pCurrent[2*(iAccumulatorFraction>>cFixedPointShift)];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
	}
}

/*! ************************************

	\brief Add in a channel's 16 bit stereo sample into a 16 bit stereo buffer

	\param pChannel Pointer to channel being added
	\param pAccumBuffer Pointer to stereo buffer to add in the sample

***************************************/

void BURGER_API Burger::Sequencer::Sample16To16AddDelayStereo(Channel_t *pChannel,int16_t *pAccumBuffer)
{
	if ((pChannel->m_pCurrent < pChannel->m_pEndOfSample) ||
		pChannel->m_uLoopSize) {

		uint_t uLeftVolume = CalculateVolume(pChannel,0);
		uint_t uRightVolume = CalculateVolume(pChannel,1);

		// Delay offsets in stereo
		uintptr_t uDelayOffset = m_uMicroDelayBufferSize*2;
		int16_t *pLeft = pAccumBuffer;
		int16_t *pRight = pAccumBuffer + 1;
		// Even channels are right
		if (!(pChannel->m_uID&1)) {	
			pRight = pRight + uDelayOffset;
		} else {
			pLeft = pLeft + uDelayOffset;
		}

		const char *pCurrent = pChannel->m_pCurrent;
		int32_t iPreviousOffset = pChannel->m_iPreviousOffset;
		int32_t iPreviousValue1Left = pChannel->m_iPreviousValue1Left;
		int32_t iPreviousValue1Right = pChannel->m_iPreviousValue1Right;
		int32_t iPreviousValue2Left = pChannel->m_iPreviousValue2Left;
		int32_t iPreviousValue2Right = pChannel->m_iPreviousValue2Right;

		uint_t bKillSample = FALSE;
		uintptr_t uAccumBufferSize = m_uAccumBufferSize;
		// Delta value
		int32_t iAccumulatorFraction = pChannel->m_iAccumulatorFraction;
		// Step value
		uint_t uStepValue = (cAmigaClock << cFixedPointShift) / (pChannel->m_uTimePeriod * m_uFinalSampleRate);

		if (pChannel->m_bPingPongReverse && 
			(pChannel->m_eLoopType == LOOP_PINGPONG)) {
			// Reverse the step
			uStepValue = 0-uStepValue;
		}

#if defined(BURGER_LITTLEENDIAN)
		++pCurrent;
#endif
		int32_t iAccumulatorInteger=0;
		int32_t iSampleValueLeft = 0;
		int32_t iSampleValueRight = 0;
		if (uAccumBufferSize) {	
			do {
				int32_t iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
				int32_t iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
				iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;

				if (iPreviousOffset != iAccumulatorInteger) {
					// Switch direction?
					if ((pChannel->m_eLoopType == LOOP_PINGPONG) &&
						pChannel->m_uLoopSize) {
						iPreviousOffset = iAccumulatorInteger;
						if ((&pCurrent[(iAccumulatorInteger*2)+6] >= pChannel->m_pEndOfSample && !pChannel->m_bPingPongReverse) ||
							(&pCurrent[(iAccumulatorInteger*2)+4] <= pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning && pChannel->m_bPingPongReverse)) {
							pChannel->m_bPingPongReverse = !pChannel->m_bPingPongReverse;
							iAccumulatorFraction = static_cast<int32_t>(iAccumulatorFraction-uStepValue);
							uStepValue = 0-uStepValue;
							iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
							iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
							iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
						}
						iPreviousValue1Left = pCurrent[iAccumulatorInteger*2];
						iPreviousValue1Right = pCurrent[(iAccumulatorInteger*2)+2];
					} else {
						iPreviousValue1Left = iPreviousValue2Left;
						iPreviousValue1Right = iPreviousValue2Right;
						iPreviousOffset = iAccumulatorInteger;

						if (&pCurrent[(iAccumulatorInteger*2) + 6] >= pChannel->m_pEndOfSample) {
							if (pChannel->m_uLoopSize)  {
								iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);	
								iRightWeight = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
								iLeftWeight = (1 << cFixedPointShift) - iRightWeight;
								iAccumulatorInteger = (iAccumulatorFraction>>cFixedPointShift)*2;
								iPreviousOffset = iAccumulatorInteger;
								pCurrent = (pChannel->m_pBeginningOfSample + pChannel->m_uLoopBeginning)-4;
#if defined(BURGER_LITTLEENDIAN)
								++pCurrent;
#endif
							} else {
								bKillSample = TRUE;
								break;
							}
						}
						iPreviousValue2Left = pCurrent[(iAccumulatorInteger*2)+4];
						iPreviousValue2Right = pCurrent[(iAccumulatorInteger*2)+6];
					}
				}

				iSampleValueLeft = ((iLeftWeight * iPreviousValue1Left) + (iRightWeight * pCurrent[(iAccumulatorInteger*2)+4])) >> cFixedPointShift;
				pLeft[0] = static_cast<int16_t>(pLeft[0] + ((iSampleValueLeft*static_cast<int32_t>(uRightVolume))>>8));
				pLeft += 2;
				iSampleValueRight = ((iLeftWeight * iPreviousValue1Right) + (iRightWeight * pCurrent[(iAccumulatorInteger*2)+6])) >> cFixedPointShift;
				pRight[0] = static_cast<int16_t>(pRight[0] + ((iSampleValueRight*static_cast<int32_t>(uLeftVolume))>>8));
				pRight += 2;
				iAccumulatorFraction += uStepValue;
			} while (--uAccumBufferSize);
		}

		// Did it reach the end of the data?			
		if (bKillSample) {
			pChannel->m_pCurrent = pChannel->m_pEndOfSample;
		} else {
			if (((iAccumulatorFraction>>cFixedPointShift)*2) == iPreviousOffset) {
				pChannel->m_iPreviousOffset = 0;
			} else {
				pChannel->m_iPreviousOffset = -1;
			}
			pChannel->m_iPreviousValue1Left = iPreviousValue1Left;
			pChannel->m_iPreviousValue1Right = iPreviousValue1Right;
			pChannel->m_iPreviousValue2Left = pCurrent[(iAccumulatorInteger*2) + 4];
			pChannel->m_iPreviousValue2Right = pCurrent[(iAccumulatorInteger*2) + 6];
#if defined(BURGER_LITTLEENDIAN)
			--pCurrent;
#endif
			pChannel->m_pCurrent = &pCurrent[(iAccumulatorFraction>>cFixedPointShift)*4];
		}
		pChannel->m_iAccumulatorFraction = iAccumulatorFraction & ((1 << cFixedPointShift) - 1);
	}
}

/*! ************************************

	\brief Mix all channels into the 32 bit buffer

***************************************/

void BURGER_API Burger::Sequencer::MixTo32(void)
{
	uint_t uMaxVoices = m_uMaxVoices;
	if (uMaxVoices) {
		Channel_t *pChannel = m_Channels;
		do {
			int32_t *pAccumBuffer = m_pAccumBuffer32;
			if (pChannel->m_bStereo) {
				if (pChannel->m_uBitsPerSample == 16) {
					Sample16To32AddDelayStereo(pChannel,pAccumBuffer);
				} else {
					Sample8To32AddDelayStereo(pChannel,pAccumBuffer);
				}
			} else {
				if (pChannel->m_uBitsPerSample == 16) {
					Sample16To32AddDelay(pChannel,pAccumBuffer);
				} else {
					Sample8To32AddDelay(pChannel,pAccumBuffer);
				}
			}
			++pChannel;
		} while (--uMaxVoices);
	}

	// Copy the final buffer
	{
		int32_t *pAccumBuffer = m_pAccumBuffer32;
		int16_t *pBuffer = static_cast<int16_t*>(m_pBuffer);
		uintptr_t uAccumBufferSize = m_uAccumBufferSize*2;
		if (uAccumBufferSize) {
			do {
				int32_t iTemp = pAccumBuffer[0];
				pAccumBuffer[0] = 0;
				++pAccumBuffer;
				if (iTemp > 0x7FFF) {
					iTemp = 0x7FFF;
				} else if (iTemp < -0x7FFF) {
					iTemp = -0x7FFF;
				}
				pBuffer[0] = static_cast<int16_t>(iTemp);
				++pBuffer;
			} while (--uAccumBufferSize);
		}
	}
}

/*! ************************************

	\brief Mix all channels into the 16 bit buffer

***************************************/

void BURGER_API Burger::Sequencer::MixTo16(void)
{
	uint_t uMaxVoices = m_uMaxVoices;
	if (uMaxVoices) {
		Channel_t *pChannel = m_Channels;
		do {
			int16_t *pAccumBuffer = m_pAccumBuffer16;
			if (pChannel->m_bStereo) {
				if (pChannel->m_uBitsPerSample == 16) {
					Sample16To16AddDelayStereo(pChannel,pAccumBuffer);
				} else {
					Sample8To16AddDelayStereo(pChannel,pAccumBuffer);
				}
			} else {
				if (pChannel->m_uBitsPerSample == 16) {
					Sample16To16AddDelay(pChannel,pAccumBuffer);
				} else {
					Sample8To16AddDelay(pChannel,pAccumBuffer);
				}
			}
			++pChannel;
		} while (--uMaxVoices);
	}
	{
		int16_t *pAccumBuffer = m_pAccumBuffer16;
		uint8_t *pBuffer = static_cast<uint8_t *>(m_pBuffer);
		uintptr_t uAccumBufferSize = m_uAccumBufferSize*2;
		if (uAccumBufferSize) {
			do {
				int32_t iTemp = pAccumBuffer[0]+128;
				pAccumBuffer[0] = 0;
				++pAccumBuffer;
				iTemp = ClampZero(iTemp);
				iTemp = Min(iTemp,static_cast<int32_t>(255));
				pBuffer[0] = static_cast<uint8_t>(iTemp);
				++pBuffer;
			} while (--uAccumBufferSize);
		}
	}
}

/*! ************************************

	\brief Mix all the channels into the output buffer

***************************************/

void BURGER_API Burger::Sequencer::GenerateSound(void)
{
	uintptr_t uAccumBufferSize;
	switch(m_eOutputDataType) {
	case SoundManager::TYPECHAR:
	case SoundManager::TYPEBYTE:
		MixTo16();
		uAccumBufferSize = m_uAccumBufferSize*2;
		m_pBuffer = static_cast<uint8_t *>(m_pBuffer)+uAccumBufferSize;
		m_pAccumBuffer16 += uAccumBufferSize;
		break;
	default:
	case SoundManager::TYPELSHORT:
	case SoundManager::TYPEBSHORT:
		MixTo32();
		uAccumBufferSize = m_uAccumBufferSize*2;
		m_pBuffer = static_cast<uint8_t *>(m_pBuffer)+uAccumBufferSize*2;
		m_pAccumBuffer32 += uAccumBufferSize;
		break;
	}
}

/*! ************************************

	\brief Apply surround sound to the output buffer

	Assuming the output buffer is stereo, and phase reverse the
	left channel

***************************************/

void BURGER_API Burger::Sequencer::ApplySurround(void)
{
	uintptr_t uAccumBufferSize = m_uAccumBufferSize;
	if (uAccumBufferSize) {
		switch(m_eOutputDataType) {
		case SoundManager::TYPECHAR:
		case SoundManager::TYPEBYTE:
			{
				char *pBuffer = static_cast<char*>(m_pBuffer);
				do {
					pBuffer[0] = static_cast<char>(-1-pBuffer[0]);
					pBuffer += 2;
				} while (--uAccumBufferSize);
			}
			break;

		default:
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPEBSHORT:
			{
				int16_t *pBuffer16 = static_cast<int16_t*>(m_pBuffer);
				do {
					pBuffer16[0] = static_cast<int16_t>(-1-pBuffer16[0]);
					pBuffer16 += 2;
				} while (--uAccumBufferSize);
			}
			break;
		}
	}
}

/*! ************************************

	\brief Purge all of the channels from playback

***************************************/

void BURGER_API Burger::Sequencer::PurgeChannels(void)
{
	uint_t uMaxVoices = m_uMaxVoices;
	if (uMaxVoices) {
		Channel_t *pChannel = m_Channels;
		do {
			pChannel->Purge();
		} while (--uMaxVoices);
	}
}

/*! ************************************

	\brief Process the current active effect

***************************************/

void BURGER_API Burger::Sequencer::SetUpEffect(Channel_t *pChannel)
{
	// Get the command token
	uint_t uEffectArgument = pChannel->m_uEffectArgument;
	uint_t uEffectCommand = pChannel->m_uEffectCommand;
	uint_t uTemp;

	// Any argument?
	if (!uEffectArgument) {
		switch(uEffectCommand) {
		default:
			uEffectArgument = pChannel->m_PreviousArguments[uEffectCommand];
			pChannel->m_uEffectArgument = uEffectArgument;
		case Command_t::EFFECT_ARPEGGIO:
		case Command_t::EFFECT_NONE:
		case Command_t::EFFECT_FASTSKIP:
		case Command_t::EFFECT_VOLUME:
		case Command_t::EFFECT_PANNING:
		case Command_t::EFFECT_SKIP:
		case Command_t::EFFECT_EXTENDED:
		case Command_t::EFFECT_SPEED:
			break;
		}
	} else {
		// Save the previous argument
		pChannel->m_PreviousArguments[uEffectCommand] = uEffectArgument;
	}

	switch (uEffectCommand) {
	case Command_t::EFFECT_UPSLIDE:
	case Command_t::EFFECT_DOWNSLIDE:
		if (uEffectArgument) {
			pChannel->m_iSlide = static_cast<int32_t>(uEffectArgument);
		}
		break;

	case Command_t::EFFECT_VIBRATO:
		uTemp = uEffectArgument>>4;
		if (uTemp) {
			pChannel->m_iCurrentVibration = static_cast<int32_t>(uTemp);
		}
		uTemp = uEffectArgument&0x0F;
		if (uTemp) {
			pChannel->m_iVibratoDepth = static_cast<int32_t>(uTemp);
		}
		pChannel->m_uPreviousTimePeriod = pChannel->m_uTimePeriod;
		break;

	case Command_t::EFFECT_ARPEGGIO:
		if (!uEffectArgument) {
			pChannel->m_Arpeggios[0] = 0;
		} else {
			if (pChannel->m_eNote != NOTE_UNUSED) {
				eNote uNote = static_cast<eNote>(pChannel->m_eNote + (uEffectArgument>>4));
				if (uNote < NOTE_MAX) {
					pChannel->m_Arpeggios[1] = static_cast<int32_t>(GetNotePeriod(uNote,cAmigaFrequency));
				}
				uNote = static_cast<eNote>(pChannel->m_eNote + (uEffectArgument&0x0F));
				if (uNote < NOTE_MAX) {
					pChannel->m_Arpeggios[2] = static_cast<int32_t>(GetNotePeriod(uNote,cAmigaFrequency));
				}
				pChannel->m_uArpeggioIndex = 0;
				pChannel->m_Arpeggios[0] = static_cast<int32_t>(pChannel->m_uTimePeriod);
			}
		}
		break;

	case Command_t::EFFECT_SLIDEVOLUME:
		pChannel->ParseSlideVolume(uEffectArgument);
		break;

	case Command_t::EFFECT_EXTENDED:
		switch((uEffectArgument>>4)) {
		case 0:		// Turn On/Off filter
			break;

		case 1:		// Fineslide up
			uTemp = uEffectArgument&0x0F;
			pChannel->m_uTimePeriod -= uTemp*4;
			break;

		case 2:		// Fineslide down
			uTemp = uEffectArgument&0x0F;
			pChannel->m_uTimePeriod += uTemp*4;
			break;

		case 3:		// Set glissando on/off
			break;

		case 4:		// Set vibrato waveform
			break;

		case 5:		// Set finetune value
		//	pChannel->m_iAmigaFineTune	= finetune[arg&0x0F];
		//	pChannel->m_uTimePeriod	= GetNotePeriod(pChannel->m_eNote,pChannel->m_iAmigaFineTune);
			break;

		case 6:		// Loop pattern
			break;

		case 7:		// Set tremolo waveform
			break;

		case 8:		// Unused
			break;

		case 9:
			break;

		case 10:	// Fine volume slide up
			uTemp = pChannel->m_uVolume;
			uTemp += (uEffectArgument&0x0F);
			if (uTemp > cMaxVolume) {
				uTemp = cMaxVolume;
			}
			pChannel->m_uVolume = uTemp;
			break;

		case 11:	// Fine volume slide down
			uTemp = pChannel->m_uVolume;
			uTemp -= (uEffectArgument&0x0F);
			if (static_cast<int_t>(uTemp) < 0) {
				uTemp = 0;
			} else if (uTemp > cMaxVolume) {
				uTemp = cMaxVolume;
			}
			pChannel->m_uVolume = uTemp;
			break;

		case 12:	// Cut sample
			break;

		case 13:	// Delay sample
			break;

		case 14:	// Delay pattern
			break;

		case 15:	// Invert loop
			break;
		}
		break;

	case Command_t::EFFECT_PORTAMENTO:
		pChannel->m_iPitchRate = static_cast<int32_t>(uEffectArgument);

		if (pChannel->m_eNote != NOTE_UNUSED) {
			pChannel->m_iPitchGoal = static_cast<int32_t>(GetNotePeriod(pChannel->m_eNote,pChannel->m_uAmigaFineTune));
		} else if (!uEffectArgument) {
			pChannel->m_iPitchGoal = static_cast<int32_t>(pChannel->m_uTimePeriod);
		}
		break;

	case Command_t::EFFECT_PORTASLIDE:
		if (pChannel->m_eNote != NOTE_UNUSED) {
			pChannel->m_iPitchGoal = static_cast<int32_t>(GetNotePeriod(pChannel->m_eNote,pChannel->m_uAmigaFineTune));
		} else if (!pChannel->m_iPitchGoal) {
			pChannel->m_iPitchGoal = static_cast<int32_t>(pChannel->m_uTimePeriod);
		}
		pChannel->ParseSlideVolume(uEffectArgument);
		break;

	case Command_t::EFFECT_VIBRATOSLIDE:
		pChannel->m_uPreviousTimePeriod = pChannel->m_uTimePeriod;
		pChannel->ParseSlideVolume(uEffectArgument);
		break;

	case Command_t::EFFECT_SPEED:
		if (uEffectArgument < 32) {
			if (uEffectArgument) {
				m_uCurrentSpeed = uEffectArgument;
			}
		} else {
			m_uFineSpeed = uEffectArgument;
		}
		break;

	case Command_t::EFFECT_SKIP:
		break;

	case Command_t::EFFECT_FASTSKIP:
		break;

	case Command_t::EFFECT_OFFSET:
		pChannel->m_pCurrent = pChannel->m_pBeginningOfSample+(uEffectArgument*256);
		break;

	case Command_t::EFFECT_PANNING:
		uEffectArgument = (uEffectArgument * cMaxPan) / 0xFF;
		if (uEffectArgument > cMaxPan) {
			uEffectArgument = cMaxPan;
		}
		pChannel->m_iPan = static_cast<int32_t>(uEffectArgument);
		break;

	case Command_t::EFFECT_VOLUME:
		if (uEffectArgument > cMaxVolume) {
			uEffectArgument = cMaxVolume;
		}
		pChannel->m_uVolume = uEffectArgument;
		break;
	}
}

/*! ************************************

	\brief Process the current active effect

***************************************/

void BURGER_API Burger::Sequencer::DoEffect(Channel_t *pChannel,uint_t uStep)
{
	switch (pChannel->m_uEffectCommand) {
		// I surrender
	default:
		pChannel->m_uEffectCommand = 0;
		pChannel->m_uEffectArgument = 0;
		return;

	case Command_t::EFFECT_ARPEGGIO:
		if (pChannel->m_uEffectArgument && pChannel->m_Arpeggios[0]) {
			uint_t uArpeggioIndex = pChannel->m_uArpeggioIndex;
			// Adjust the index
			if (++uArpeggioIndex >= cArpeggioMaxCount) {		
				// Back to the beginning
				uArpeggioIndex = 0;
			}
			pChannel->m_uArpeggioIndex = uArpeggioIndex;
			pChannel->m_uTimePeriod = static_cast<uint_t>(pChannel->m_Arpeggios[uArpeggioIndex]);
		}
		break;

	case Command_t::EFFECT_SKIP:
		if (uStep == (m_uCurrentSpeed - 1)) {
			// Get the partition index
			uint_t uPartitionPosition = m_uPartitionPosition;
			if (m_uPatternPosition) {
				++uPartitionPosition;
				m_uPartitionPosition = uPartitionPosition;
				// Next pattern
				m_uCurrentIDPattern = m_pSongPackage->m_SongDescription.m_PatternPointers[uPartitionPosition];
			}

			uint_t uEffectArgument = pChannel->m_uEffectArgument;
			// Convert from BCD
			m_uPatternPosition = ((uEffectArgument>>4) * 10) + (uEffectArgument&0x0f);

			if (uPartitionPosition >= m_pSongPackage->m_SongDescription.m_uPointerCount) {
				m_uPartitionPosition = 0;
				m_uCurrentIDPattern = m_pSongPackage->m_SongDescription.m_PatternPointers[0];
				ClearChannels();
				if (!m_bRepeatEnabled) {
					m_bSequencingInProgress = FALSE;
				}
			}
			pChannel->m_uEffectCommand = 0;
			pChannel->m_uEffectArgument = 0;
		}
		break;

	case Command_t::EFFECT_FASTSKIP:
		if (uStep == (m_uCurrentSpeed - 1)) {
			if (m_uPartitionPosition > pChannel->m_uEffectArgument) {
				if (!m_bRepeatEnabled) {
					m_bSequencingInProgress = FALSE;
				}
			}

			uint_t uEffectArgument = pChannel->m_uEffectArgument;
			m_uPartitionPosition = uEffectArgument;
			m_uCurrentIDPattern = m_pSongPackage->m_SongDescription.m_PatternPointers[uEffectArgument];

			if (uEffectArgument >= m_pSongPackage->m_SongDescription.m_uPointerCount) {
				m_uPartitionPosition = 0;
				m_uCurrentIDPattern = m_pSongPackage->m_SongDescription.m_PatternPointers[0];

				ClearChannels();
				if (!m_bRepeatEnabled) {
					m_bSequencingInProgress = FALSE;
				}
			}

			m_uPatternPosition = 0;
			pChannel->m_uEffectCommand = 0;
			pChannel->m_uEffectArgument = 0;
		}
		break;

	case Command_t::EFFECT_DOWNSLIDE:
		if (pChannel->m_uTimePeriod > cMinimumPitch) {
			pChannel->m_uTimePeriod -= pChannel->m_iSlide*4;
		}
		break;

	case Command_t::EFFECT_UPSLIDE:
		if (pChannel->m_uTimePeriod < cMaximumPitch) {
			pChannel->m_uTimePeriod += pChannel->m_iSlide*4;
		}
		break;

	case Command_t::EFFECT_VIBRATO:
		{
			int32_t iVibratoOffset = (pChannel->m_iVibratoOffset+pChannel->m_iCurrentVibration)&0x3F;
			pChannel->m_iVibratoOffset = iVibratoOffset;
			iVibratoOffset = (g_VibratoTable[iVibratoOffset] * pChannel->m_iVibratoDepth) / 512;
			pChannel->m_uTimePeriod = pChannel->m_uPreviousTimePeriod + iVibratoOffset*4;
		}
		break;

	case Command_t::EFFECT_SLIDEVOLUME:
		{
			uint_t uVolume = pChannel->m_uVolume + pChannel->m_iVolumeRate;
			if (static_cast<int32_t>(uVolume) < 0) {
				uVolume = 0;
			} else if (uVolume > cMaxVolume) {
				uVolume = cMaxVolume;
			}
			pChannel->m_uVolume = uVolume;
		}
		break;

	case Command_t::EFFECT_PORTAMENTO:
		{
			int32_t iPitchGoal = pChannel->m_iPitchGoal;
			int32_t iTimePeriod = static_cast<int32_t>(pChannel->m_uTimePeriod);
			if (iTimePeriod != iPitchGoal) {
				if (iTimePeriod < iPitchGoal) {
					iTimePeriod += pChannel->m_iPitchRate*4;
					if (iTimePeriod > iPitchGoal) {
						pChannel->m_uEffectCommand = 0;
						pChannel->m_uEffectArgument = 0;
						iTimePeriod = iPitchGoal;
					}
					pChannel->m_uTimePeriod = static_cast<uint_t>(iTimePeriod);
				} else if (iTimePeriod > iPitchGoal) {
					iTimePeriod -= pChannel->m_iPitchRate*4;
					if (iTimePeriod < iPitchGoal) {
						pChannel->m_uEffectCommand = 0;
						pChannel->m_uEffectArgument = 0;
						iTimePeriod = iPitchGoal;
					}
					pChannel->m_uTimePeriod = static_cast<uint_t>(iTimePeriod);
				}
			}
		}
		break;

	case Command_t::EFFECT_PORTASLIDE:
		// Fake commands
		pChannel->m_uEffectCommand = Command_t::EFFECT_PORTAMENTO;
		DoEffect(pChannel,uStep);
		pChannel->m_uEffectCommand = Command_t::EFFECT_SLIDEVOLUME;
		DoEffect(pChannel,uStep);
		// Restore the token
		pChannel->m_uEffectCommand = Command_t::EFFECT_PORTASLIDE;
		break;

	case Command_t::EFFECT_VIBRATOSLIDE:
		// Fake commands
		pChannel->m_uEffectCommand = Command_t::EFFECT_VIBRATO;
		DoEffect(pChannel,uStep);
		pChannel->m_uEffectCommand = Command_t::EFFECT_SLIDEVOLUME;
		DoEffect(pChannel,uStep);
		// Restore the token
		pChannel->m_uEffectCommand = Command_t::EFFECT_VIBRATOSLIDE;
		break;

	case Command_t::EFFECT_EXTENDED:
		if ((pChannel->m_uEffectArgument&0xF0)==(12<<4)) {
			if (uStep>= (pChannel->m_uEffectArgument&0x0F)) {
				pChannel->m_uVolume = 0;
			}
		}
		break;
	}

	// At the end?
	if (uStep == (m_uCurrentSpeed - 1)) {
		// Zap the command
		pChannel->m_uEffectArgument = 0;
		pChannel->m_uEffectCommand = 0;
	}
}

/*! ************************************

	\brief Handle the volume fading

***************************************/

void BURGER_API Burger::Sequencer::ProcessFadeOut(Channel_t *pChannel) const
{
	if (!pChannel->m_bKeyOn) {
		SongPackage *pSongPackage = m_pSongPackage;
		if (pSongPackage) {
			int32_t iTemp = static_cast<int32_t>(pChannel->m_uVolumeFade - pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID].m_uVolumeFadeSpeed);
			if (iTemp < 0) {
				pChannel->m_uLoopBeginning = 0;
				pChannel->m_uLoopSize = 0;
				iTemp = 0;
			}
			pChannel->m_uVolumeFade = static_cast<uint32_t>(iTemp);
		}
	}
}

/*! ************************************

	\brief Handle the volume envelope

***************************************/

void BURGER_API Burger::Sequencer::ProcessEnvelope(Channel_t *pChannel) const
{
	pChannel->m_uCurrentVolumeFromEnvelope = cMaxVolume;
	const InstrData_t *pInstrData = &m_pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID];
	if (pInstrData->m_uVolumeEnvelopeCount) {
		if (pInstrData->m_uVolumeEnvelopeFlags & InstrData_t::ENVELOPE_ON) {

			// Using an envelope, check if it's only one entry
			if (pInstrData->m_uVolumeEnvelopeCount == 1) {
				// Use the default value
				pChannel->m_uCurrentVolumeFromEnvelope = pInstrData->m_VolumeEnvelope[0].m_uVolume;
				pChannel->m_uCurrentVolumePosition = pInstrData->m_VolumeEnvelope[0].m_uPosition;
			} else {

				// Interpolate from the envelope
				uint_t uCurrentVolumeEnvelopeIndex=pChannel->m_uCurrentVolumeEnvelopeIndex;
				uint_t uNextVolumeEnvelopeIndex=pChannel->m_uNextVolumeEnvelopeIndex;
				uint_t uCurrentVolumePosition=pChannel->m_uCurrentVolumePosition;
				pChannel->m_uCurrentVolumeFromEnvelope = pInstrData->m_VolumeEnvelope[uCurrentVolumeEnvelopeIndex].Interpolate(&pInstrData->m_VolumeEnvelope[uNextVolumeEnvelopeIndex],uCurrentVolumePosition);

				// If on sustain, stay until key up has been pressed

				if (!(pInstrData->m_uVolumeEnvelopeFlags & InstrData_t::ENVELOPE_SUSTAIN) || 
					!pChannel->m_bKeyOn || 
					(uCurrentVolumeEnvelopeIndex!=pInstrData->m_uVolumeSustainIndex) || 
					(uCurrentVolumePosition!=pInstrData->m_VolumeEnvelope[uCurrentVolumeEnvelopeIndex].m_uPosition)) {
						if (++uCurrentVolumePosition >= pInstrData->m_VolumeEnvelope[uNextVolumeEnvelopeIndex].m_uPosition) {
							uCurrentVolumeEnvelopeIndex=uNextVolumeEnvelopeIndex;
							++uNextVolumeEnvelopeIndex;

							// Handle the loop
							if (pInstrData->m_uVolumeEnvelopeFlags & InstrData_t::ENVELOPE_LOOP) {
								if (uNextVolumeEnvelopeIndex > pInstrData->m_uVolumeEndIndex) {
									uCurrentVolumeEnvelopeIndex=pInstrData->m_uVolumeBeginIndex;
									uNextVolumeEnvelopeIndex=uCurrentVolumeEnvelopeIndex+1;
									uCurrentVolumePosition=pInstrData->m_VolumeEnvelope[uCurrentVolumeEnvelopeIndex].m_uPosition;
								}
							} else {
								// End of the record?
								if (uNextVolumeEnvelopeIndex >= pInstrData->m_uVolumeEnvelopeCount) {
									--uNextVolumeEnvelopeIndex;
									--uCurrentVolumePosition;
								}
							}
						}
						pChannel->m_uCurrentVolumeEnvelopeIndex=uCurrentVolumeEnvelopeIndex;
						pChannel->m_uNextVolumeEnvelopeIndex=uNextVolumeEnvelopeIndex;
						pChannel->m_uCurrentVolumePosition=uCurrentVolumePosition;
				}
			}
		}
	}
}

/*! ************************************

	\brief Handle the pan envelope

***************************************/

void BURGER_API Burger::Sequencer::ProcessPanning(Channel_t *pChannel) const
{
	pChannel->m_uCurrentPanFromEnvelope = static_cast<uint_t>(pChannel->m_iPan);
	const InstrData_t *pInstrData = &m_pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID];
	if (pInstrData->m_uPanEnvelopeCount) {
		if (pInstrData->m_uPanEnvelopeFlags & InstrData_t::ENVELOPE_ON) {
			// Only one point? Use it
			if (pInstrData->m_uPanEnvelopeCount == 1) {
				pChannel->m_uCurrentPanFromEnvelope = pInstrData->m_PanEnvelope[0].m_uVolume;
				pChannel->m_uCurrentPanPosition = pInstrData->m_PanEnvelope[0].m_uPosition;
			} else {
				uint_t uCurrentPanEnvelopeIndex = pChannel->m_uCurrentPanEnvelopeIndex;
				uint_t uNextPanEnvelopeIndex = pChannel->m_uNextPanEnvelopeIndex;
				uint_t uCurrentPanPosition = pChannel->m_uCurrentPanPosition;
				pChannel->m_uCurrentPanFromEnvelope = pInstrData->m_PanEnvelope[uCurrentPanEnvelopeIndex].Interpolate(&pInstrData->m_PanEnvelope[uNextPanEnvelopeIndex],uCurrentPanPosition);
				if (++uCurrentPanPosition >= pInstrData->m_PanEnvelope[uNextPanEnvelopeIndex].m_uPosition) {
					uCurrentPanEnvelopeIndex=uNextPanEnvelopeIndex;
					++uNextPanEnvelopeIndex;
					if (pInstrData->m_uPanEnvelopeFlags & InstrData_t::ENVELOPE_LOOP) {
						if (uNextPanEnvelopeIndex > pInstrData->m_uPanEndIndex) {
							uCurrentPanEnvelopeIndex=pInstrData->m_uPanBeginIndex;
							uNextPanEnvelopeIndex=uCurrentPanEnvelopeIndex+1;
							uCurrentPanPosition=pInstrData->m_PanEnvelope[uCurrentPanEnvelopeIndex].m_uPosition;
						}
					} else {
						if (uNextPanEnvelopeIndex >= pInstrData->m_uPanEnvelopeCount) {
							--uNextPanEnvelopeIndex;
							--uCurrentPanPosition;
						}
					}
					pChannel->m_uCurrentPanEnvelopeIndex=uCurrentPanEnvelopeIndex;
					pChannel->m_uNextPanEnvelopeIndex=uNextPanEnvelopeIndex;
				}
				pChannel->m_uCurrentPanPosition=uCurrentPanPosition;
			}
		}
	}
}

/*! ************************************

	\brief Process a note on/off command

***************************************/

void BURGER_API Burger::Sequencer::ProcessNote(Channel_t *pChannel,const Command_t *pCommand)
{
	Command_t::eEffect uEffect = pCommand->GetEffect();
	uint_t uEffectArgument = pCommand->m_uEffectArgument;

	//
	// Handle extended effects
	//
	if ((uEffect == Command_t::EFFECT_EXTENDED) &&
		((uEffectArgument >> 4) == 0x0D)) {
		if ((!m_uSpeedCounter) && 
			!pChannel->m_bEffectActive) {
			// Process in NoteAnalyze()
			pChannel->m_bEffectActive = TRUE;
			pChannel->m_uEffectPartitionIndex = m_uCurrentIDPattern;
			pChannel->m_uEffectRowIndex = m_uPatternPosition;
		}
		// Wait until the effect is done.
		if (m_uSpeedCounter < (uEffectArgument & 0x0F)) {
			return;
		}
	}
	pChannel->m_bEffectActive = FALSE;

	//
	// Process the note command
	//

	uint_t uInstrument = pCommand->m_uInstrument;
	eNote uNote = pCommand->GetNote();
	if (uInstrument ||
		((uNote != NOTE_UNUSED) && (uNote != NOTE_OFF))) {
		// Use the previous instrument?
		if (!uInstrument) {
			uInstrument = pChannel->m_uPreviousInstrumentID;
		} else {
			// Use this instrument
			pChannel->m_uPreviousInstrumentID = uInstrument;
		}

		if (uInstrument && 
			((uNote != NOTE_UNUSED) && (uNote != NOTE_OFF))) {

			// uInstrument is greater than zero here
			uint_t uInstrumentIndex = uInstrument - 1;
			if (uInstrumentIndex >= cInstrumentMaxCount) {
				uInstrumentIndex = cInstrumentMaxCount-1;
			}
			uint_t uSampleIndex = m_pSongPackage->m_InstrDatas[uInstrumentIndex].m_WhichSampleForNote[uNote];

			// Is there a digital sample for this?
			if (uSampleIndex < m_pSongPackage->m_InstrDatas[uInstrumentIndex].m_uNumberSamples) {

				const SampleDescription *pSampleDescription = m_pSongPackage->m_pSampleDescriptions[m_pSongPackage->m_InstrDatas[uInstrumentIndex].m_uBaseSampleID + uSampleIndex];
				pChannel->m_uCurrentInstrumentID = uInstrumentIndex;
				pChannel->m_uBitsPerSample = pSampleDescription->m_uBitsPerSample;
				pChannel->m_bStereo = pSampleDescription->m_bStereo;
				pChannel->m_uSampleID = uSampleIndex;
				pChannel->m_eLoopType= pSampleDescription->m_eLoopType;

				if ((uEffect != Command_t::EFFECT_PORTAMENTO) &&
					(uEffect != Command_t::EFFECT_PORTASLIDE)) {
					pChannel->m_pEchoSample = NULL;
					const void *pSample = pSampleDescription->m_pSample;
					pChannel->m_pCurrent = static_cast<const char *>(pSample);
					pChannel->m_pBeginningOfSample = static_cast<const char *>(pSample);
					uintptr_t uSampleSize = pSampleDescription->m_uSampleSize;
					pChannel->m_pEndOfSample = static_cast<const char *>(pSample) + uSampleSize;
					pChannel->m_uSampleSize = uSampleSize;
					pChannel->m_iAccumulatorFraction = 0;
					pChannel->m_bPingPongReverse = FALSE;
					pChannel->m_iPreviousOffset = -1;
					pChannel->m_iPreviousValue1Left = 0;
					pChannel->m_iPreviousValue3Left = 0;
					pChannel->m_iPreviousValue2Left = static_cast<const char *>(pSample)[0];
					if (pChannel->m_uBitsPerSample == 8) {
						pChannel->m_iPreviousValue2Right = static_cast<const char *>(pSample)[1];
					} else {
						pChannel->m_iPreviousValue2Right = static_cast<const char *>(pSample)[2];
					}
					pChannel->m_iPreviousValue4Left = static_cast<const int16_t *>(pSample)[0];
					pChannel->m_iPreviousValue4Right = static_cast<const int16_t *>(pSample)[1];

					if (pSampleDescription->m_uLoopLength > 2) {
						pChannel->m_uLoopBeginning = pSampleDescription->m_uLoopStart;
						pChannel->m_uLoopSize = pSampleDescription->m_uLoopLength;
						pChannel->m_pEndOfSample = static_cast<const char *>(pSample) + pSampleDescription->m_uLoopStart + pSampleDescription->m_uLoopLength;
					} else {
						pChannel->m_uLoopBeginning = 0;
						pChannel->m_uLoopSize = 0;
					}
					pChannel->m_iVibratoOffset = 0;

					if (uEffect != Command_t::EFFECT_VOLUME) {
						pChannel->m_uVolume = pSampleDescription->m_uVolume;
						if (pChannel->m_uVolume > cMaxVolume) {
							pChannel->m_uVolume = cMaxVolume;
						}
						pChannel->m_uVolumeFade = 32767;
					}

					if (uEffect != Command_t::EFFECT_PANNING) {
						pChannel->m_iPan = static_cast<int32_t>(m_pSongPackage->m_SongDescription.m_ChannelPans[pChannel->m_uID]);
						if (pChannel->m_iPan > cMaxPan) {
							pChannel->m_iPan = cMaxPan;
						}
					}
					// Set the pan envelope
					pChannel->m_uCurrentPanPosition=0;
					pChannel->m_uCurrentPanEnvelopeIndex=0;
					pChannel->m_uNextPanEnvelopeIndex=1;

					// Set the volume envelope
					pChannel->m_uCurrentVolumePosition=0;
					pChannel->m_uCurrentVolumeEnvelopeIndex=0;
					pChannel->m_uNextVolumeEnvelopeIndex=1;
				}
			}
		} else if (uInstrument &&
			(uNote == NOTE_UNUSED)) {
			if (pChannel->m_uSampleID < m_pSongPackage->m_InstrDatas[ pChannel->m_uCurrentInstrumentID].m_uNumberSamples) {

				const SampleDescription *pSampleDescription = m_pSongPackage->m_pSampleDescriptions[m_pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID].m_uBaseSampleID + pChannel->m_uSampleID];
				if (pSampleDescription) {
					if (uEffect != Command_t::EFFECT_VOLUME) {
						pChannel->m_uVolume = pSampleDescription->m_uVolume;
						if (pChannel->m_uVolume > cMaxVolume) {
							pChannel->m_uVolume = cMaxVolume;
						}
						pChannel->m_uVolumeFade = 32767;
					}

					if (uEffect != Command_t::EFFECT_PANNING) {
						pChannel->m_iPan = static_cast<int32_t>(m_pSongPackage->m_SongDescription.m_ChannelPans[pChannel->m_uID]);
						if (pChannel->m_iPan > cMaxPan) {
							pChannel->m_iPan = cMaxPan;
						}
					}
				}
			}
		}

		//
		// Handle note on events
		//

		if ((uNote != NOTE_UNUSED) &&
			(uNote != NOTE_OFF)) {
			const SongPackage *pSongPackage = m_pSongPackage;
			uint_t uSampleIndex = pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID].m_WhichSampleForNote[uNote];
			if (uSampleIndex < pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID].m_uNumberSamples) {

				const SampleDescription *pSampleDescription = pSongPackage->m_pSampleDescriptions[pSongPackage->m_InstrDatas[pChannel->m_uCurrentInstrumentID].m_uBaseSampleID + uSampleIndex];
				pChannel->m_eNote = static_cast<eNote>(uNote + pSampleDescription->m_iRelativeNote);
				pChannel->m_uAmigaFineTune = pSampleDescription->m_uC2SamplesPerSecond;
				pChannel->m_bKeyOn = TRUE;

				if ((uEffect != Command_t::EFFECT_PORTAMENTO) &&
					(uEffect != Command_t::EFFECT_PORTASLIDE)) {
					pChannel->m_uTimePeriod = GetNotePeriod(pChannel->m_eNote,pChannel->m_uAmigaFineTune);
					uint_t uTimePeriod = (pChannel->m_uTimePeriod*m_uMasterPitch) / 80U;
					pChannel->m_uTimePeriod = uTimePeriod;
					pChannel->m_uPreviousTimePeriod = uTimePeriod;
				}
			}
		}
	} else {
		pChannel->m_eNote = NOTE_UNUSED;
	}

	//
	// Process a volume command
	//

	Command_t::eVolumeCommand uVolumeCommand = pCommand->GetVolume();
	if (uVolumeCommand != Command_t::VOLUME_UNUSED) {
		// Commands 0x10-0x50 is normalized to 0-64
		if (uVolumeCommand >= 0x10 && uVolumeCommand <= 0x50) {
			pChannel->m_uVolume = uVolumeCommand - 0x10U;
			pChannel->m_uVolumeCommand = 0;		// No command
		} else {
			// Save the volume command (0-15)
			pChannel->m_uVolumeCommand = uVolumeCommand;
		}
	} else {
		// No volume command in progress
		pChannel->m_uVolumeCommand = 0;
	}

	//
	// Process the effect
	//
	pChannel->m_uEffectCommand = uEffect;
	pChannel->m_uEffectArgument = uEffectArgument;
	SetUpEffect(pChannel);

	//
	// Handle the key off event
	//

	if (uNote == NOTE_OFF) {
		pChannel->m_bKeyOn = FALSE;
	}
}

/*! ************************************

	\brief Perform the sequencing and fill the output buffer

***************************************/

void BURGER_API Burger::Sequencer::PerformSequencing(void)
{
	// If there is no song, or it's blocked, fill with silence
	if (!m_pSongPackage || 
		m_pSongPackage->m_bMusicUnderModification) {
		ClearSoundData(m_pBuffer,m_uAccumBufferSize,m_eOutputDataType);
		return;
	}

	//
	// Process notes and then generate the audio data
	//

	uintptr_t uRemaining = m_uAccumBufferSize;
	if (uRemaining) {

		// Copy these values for later restoration
		void *pBufferCopy = m_pBuffer;
		int32_t *pAccumBuffer32Copy = m_pAccumBuffer32;
		int16_t *pAccumBuffer16Copy = m_pAccumBuffer16;
		uintptr_t uAccumBufferSizeCopy = m_uAccumBufferSize;

		do {
			uintptr_t uBytesLeft = m_uBytesToGenerate - m_uBytesGenerated;
			uint_t bNoteProcessing;
			if (uBytesLeft > uRemaining) {
				uBytesLeft = uRemaining;
				bNoteProcessing = FALSE;
			} else {
				bNoteProcessing = TRUE;
			}

			// Set the size
			m_uAccumBufferSize = uBytesLeft;
			if (uBytesLeft) {
				// Generate sound to fill the buffer
				GenerateSound();
				uBytesLeft = m_uAccumBufferSize;
				m_uBytesGenerated += uBytesLeft;
				uRemaining -= uBytesLeft;
			}

			//
			// At this point, notes need to be played to adjust the instrument playback
			//

			if (!bNoteProcessing) {
				break;
			}

			//
			// Handle the special effect notes
			//

			if (m_pSongPackage && m_bSequencingInProgress) {
				uint_t uChannelCount = m_pSongPackage->m_SongDescription.m_uChannelCount;
				if (uChannelCount) {
					Channel_t *pChannel = m_Channels;
					uint_t uChannel = 0;
					do {
						if (pChannel->m_bEffectActive) {
							ProcessNote(pChannel,m_pSongPackage->m_pPartitions[pChannel->m_uEffectPartitionIndex]->GetCommand(static_cast<int>(pChannel->m_uEffectRowIndex),static_cast<int>(uChannel)));
						}
						++pChannel;
					} while (++uChannel<uChannelCount);
				}
			}

			//
			// Handle notes and the instrument effects (ADSR, vibrato, etc)
			//

			if (++m_uSpeedCounter >= m_uCurrentSpeed) {
				// Note
				m_uSpeedCounter = 0;
				const SongPackage *pSongPackage = m_pSongPackage;
				if (pSongPackage) {
					uint_t uChannelCount = pSongPackage->m_SongDescription.m_uChannelCount;
					if (uChannelCount) {
						Sequencer::Channel_t *pChannel = m_Channels;
						uint_t uChannel = 0;
						do {
							if (m_bSequencingInProgress) {
								ProcessNote(pChannel,pSongPackage->m_pPartitions[m_uCurrentIDPattern]->GetCommand(static_cast<int>(m_uPatternPosition),static_cast<int>(uChannel)));
							}
							ProcessEnvelope(pChannel);
							ProcessPanning(pChannel);
							ProcessFadeOut(pChannel);
							++pChannel;
						} while (++uChannel<uChannelCount);
					}

					if (m_bSequencingInProgress) {
						if (++m_uPatternPosition >= pSongPackage->m_pPartitions[m_uCurrentIDPattern]->m_uRowCount) {
							m_uPatternPosition = 0;

							uint_t uPartitionPosition = m_uPartitionPosition+1;
							m_uPartitionPosition = uPartitionPosition;
							m_uCurrentIDPattern = pSongPackage->m_SongDescription.m_PatternPointers[uPartitionPosition];

							if ((m_uCurrentSpeed == 1) &&
								(uPartitionPosition >= pSongPackage->m_SongDescription.m_uPointerCount)) {
								m_uPartitionPosition = 0;
								m_uCurrentIDPattern = pSongPackage->m_SongDescription.m_PatternPointers[0];
								ClearChannels();
								// Repeat the song?
								if (!m_bRepeatEnabled) {
									// Stop playing
									m_bSequencingInProgress = FALSE;
								}
							}
						}
					}
				}
			} else {
				//
				// Only process the instrument effects
				//
				const Sequencer::SongPackage *pSongPackage = m_pSongPackage;
				if (pSongPackage) {
					uint_t uMaxVoices = m_uMaxVoices;
					if (uMaxVoices) {
						Sequencer::Channel_t *pChannel = m_Channels;
						do {
							pChannel->VolumeCommand(m_uSpeedCounter);
							DoEffect(pChannel,m_uSpeedCounter);
							ProcessEnvelope(pChannel);
							ProcessPanning(pChannel);
							ProcessFadeOut(pChannel);
							++pChannel;
						} while (--uMaxVoices);
					}

					if (m_bSequencingInProgress) {
						if (m_uSpeedCounter == (m_uCurrentSpeed - 1)) {
							if (m_uPartitionPosition >= pSongPackage->m_SongDescription.m_uPointerCount) {
								m_uPartitionPosition = 0;
								m_uCurrentIDPattern = pSongPackage->m_SongDescription.m_PatternPointers[0];

								ClearChannels();
								// Repeating?
								if (!m_bRepeatEnabled) {
									// Stop the song
									m_bSequencingInProgress = FALSE;
								}
							}
						}
					}
				}
			}

			uintptr_t uThisChunk = ((80*m_uChunkSize)/m_uFineSpeed)/m_uMasterSpeed;
			m_uBytesToGenerate += uThisChunk;
		} while (uRemaining);

		// Restore the values
		m_uAccumBufferSize = uAccumBufferSizeCopy;
		m_pBuffer = pBufferCopy;
		m_pAccumBuffer32 = pAccumBuffer32Copy;
		m_pAccumBuffer16 = pAccumBuffer16Copy;
	}

	//
	// Apply the micro delay?
	//

	if (m_uMicroDelayDuration) {
		switch (m_eOutputDataType) {
		default:
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPEBSHORT:
			MemoryCopy(m_pAccumBuffer32, m_pAccumBuffer32 + m_uAccumBufferSize*2,m_uMicroDelayBufferSize*8);
			MemoryClear((m_pAccumBuffer32 + m_uMicroDelayBufferSize*2),m_uAccumBufferSize*8);
			break;

		case SoundManager::TYPECHAR:
		case SoundManager::TYPEBYTE:
			if (m_uMicroDelayBufferSize&1) {
				MemoryCopy(m_pAccumBuffer16,m_pAccumBuffer16 + m_uAccumBufferSize*2,m_uMicroDelayBufferSize*4 + 1);
				MemoryClear((m_pAccumBuffer16 + m_uMicroDelayBufferSize*2),(m_uAccumBufferSize+1)*4);
			} else {
				MemoryCopy(m_pAccumBuffer16,m_pAccumBuffer16 + m_uAccumBufferSize*2,m_uMicroDelayBufferSize*4);
				MemoryClear((m_pAccumBuffer16 + m_uMicroDelayBufferSize*2),m_uAccumBufferSize*4);
			}
			break;
		}
	}

	//
	// Apply surround sound
	//

	if (m_bSurroundEnabled) {
		ApplySurround();
	}

	//
	// Apply reverb
	//

	if (m_bReverbEnabled && 
		(m_uAccumBufferSize < m_uReverbDelayBufferSize)) {
		switch (m_eOutputDataType) {
		case SoundManager::TYPECHAR:
		case SoundManager::TYPEBYTE:
			ComputeReverb(static_cast<uint8_t*>(m_pBuffer),static_cast<const uint8_t*>(m_pReverbBuffer),m_uAccumBufferSize*2,m_uReverbStrength);
			MemoryCopy(m_pReverbBuffer,static_cast<const uint8_t*>(m_pReverbBuffer) + m_uAccumBufferSize*2,(m_uReverbDelayBufferSize-m_uAccumBufferSize)*2);
			MemoryCopy(static_cast<uint8_t*>(m_pReverbBuffer) + m_uReverbDelayBufferSize*2 - m_uAccumBufferSize*2,m_pBuffer,m_uAccumBufferSize*2);
			break;

		default:
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPEBSHORT:
			ComputeReverb(static_cast<int16_t*>(m_pBuffer),static_cast<const int16_t *>(m_pReverbBuffer),m_uAccumBufferSize*2,m_uReverbStrength);
			MemoryCopy(m_pReverbBuffer,static_cast<const uint8_t*>(m_pReverbBuffer) + m_uAccumBufferSize*4,(m_uReverbDelayBufferSize-m_uAccumBufferSize)*4);
			MemoryCopy(static_cast<uint8_t*>(m_pReverbBuffer) + m_uReverbDelayBufferSize*4 - m_uAccumBufferSize*4,m_pBuffer,m_uAccumBufferSize*4);
			break;
		}
	}
}

/*! ************************************

	\brief Allocate the platform specific resources
	
***************************************/

#if !(defined(BURGER_WINDOWS)) ||  defined(DOXYGEN)
uint_t BURGER_API Burger::Sequencer::PlatformInit(void)
{
	return 0;
}

/*! ************************************

	\brief Release the platform specific resources

***************************************/

void BURGER_API Burger::Sequencer::PlatformShutdown(void)
{
}
#endif

/*! ************************************

	\brief Clear all of the channels

***************************************/

void BURGER_API Burger::Sequencer::ClearChannels(void)
{
	m_uBytesGenerated = 0;
	m_uBytesToGenerate = 0;
	uint_t i = 0;
	Channel_t *pChannel = m_Channels;
	do {
		pChannel->Init(i);
		++pChannel;
	} while (++i<cTrackMaxCount);
}

/*! ************************************

	\brief Calculate the final volume for a voice

	Given a voice and a requested speaker channel, return the volume
	for the current state of the channel

	\param pChannel
	\param uSpeaker Which speaker to get the volume for (0=left, 1 = right)
	\return 0-256, volume of the channel at this moment in time

***************************************/

uint_t BURGER_API Burger::Sequencer::CalculateVolume(const Channel_t *pChannel,uint_t uSpeaker) const
{
	//
	// Ranges in order, 64 * 64 * 32767 / (16*32767) = 256
	// This way the resulting volume is from 0 to 256
	//
	
	uint_t uResult = (pChannel->m_uVolume * pChannel->m_uCurrentVolumeFromEnvelope * pChannel->m_uVolumeFade) / (16*32767);

	if (m_pSongPackage) {
		//
		// Adjust to channel volume based on the channel volumes
		//
		uResult = (uResult * m_pSongPackage->m_SongDescription.m_ChannelVolumes[pChannel->m_uID]) / cMaxVolume;
	}

	//
	// Compute Panning
	//

	if (uSpeaker<2) {
		uint_t uPanAdjust = pChannel->m_uCurrentPanFromEnvelope;
		// Right speaker?
		if (uSpeaker == 1) {
			// Flip the pan
			uPanAdjust = cMaxPan - uPanAdjust;
		}
		uResult = (uResult * uPanAdjust) / cMaxPan;
	}

	//
	// Apply the global volume
	//

	uResult = (uResult * m_uVolume) / 255;

	if (uResult > 256) {
		uResult = 256U;
	}
	return uResult;
}

/*! ************************************

	\brief Set the song to play

	\return Zero is no error, non-zero means package install failure

***************************************/

uint_t BURGER_API Burger::Sequencer::SetSong(SongPackage *pSongPackage)
{
	uint_t uResult = 10;
	if (pSongPackage) {
		DisposeSong();
		m_pSongPackage = pSongPackage;
		m_uVolume = pSongPackage->m_SongDescription.m_uMasterVolume*4;
		if (!m_uVolume || (m_uVolume>=256U)) {
			m_uVolume = 255;
		}
		m_uMasterSpeed = pSongPackage->m_SongDescription.m_uMasterSpeed;
		if (m_uMasterSpeed <= 0) {
			m_uMasterSpeed = 80;
		}
		m_uMasterPitch = pSongPackage->m_SongDescription.m_uMasterPitch;
		if (m_uMasterPitch <= 0) {
			m_uMasterPitch = 80;
		}
		Reset();
		SetChannelCount();
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Play a song

	\return Zero is no error, non-zero means no song present

***************************************/

uint_t BURGER_API Burger::Sequencer::Play(SongPackage *pSongPackage)
{
	uint_t uResult = SetSong(pSongPackage);
	if (!uResult) {
		PrepareSequencer();			// Turn interrupt driver function ON
		StartSequencing();			// Read the current partition in memory
	}
	return uResult;
}

/*! ************************************

	\brief Play a song

	Run the data through an importer and then play the song if
	there were no importation errors

	\return Zero is no error, non-zero means no song present

***************************************/

uint_t BURGER_API Burger::Sequencer::Play(const void *pInput,uintptr_t uInputLength)
{
	SongPackage *pSongPackage;
	uint_t uResult = ImportSong(&pSongPackage,static_cast<const uint8_t*>(pInput),uInputLength);
	if (!uResult) {
		uResult = Play(pSongPackage);
	}
	return uResult;
}

/*! ************************************

	\brief Play a song

	Load a song file and then
	run the data through an importer and then play the song if
	there were no importation errors

	\return Zero is no error, non-zero means no song present

***************************************/

uint_t BURGER_API Burger::Sequencer::Play(const char *pFilename)
{
	uintptr_t uInputLength;
	uint_t uResult = 10;
	const void *pInput = FileManager::LoadFile(pFilename,&uInputLength);
	if (pInput) {
		uResult = Play(pInput,uInputLength);
		Free(pInput);
	}
	return uResult;
}

uint_t BURGER_API Burger::Sequencer::PlayRez(RezFile *pRezFile,uint_t uRezNum)
{
	uint_t uResult = 10;
	const void *pInput = pRezFile->Load(uRezNum);
	if (pInput) {
		uResult = Play(pInput,pRezFile->GetSize(uRezNum));
		pRezFile->Release(uRezNum);
	}
	return uResult;
}

/*! ************************************

	\brief Start music sequencer

	\return Zero is no error, non-zero means no song present

***************************************/

uint_t BURGER_API Burger::Sequencer::StartSequencing(void)
{
	uint_t uResult = 10;
	if (m_pSongPackage) {
		m_bSequencingInProgress = TRUE;
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Stop music sequencer

	\return Zero is no error, non-zero means no song present

***************************************/

uint_t BURGER_API Burger::Sequencer::StopSequencing(void)
{
	uint_t uResult = 10;
	if (m_pSongPackage) {
		m_bSequencingInProgress = FALSE;
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Stop music sequencer and release the song

***************************************/

void BURGER_API Burger::Sequencer::DisposeSong(void)
{
	StopSequencing();		// Stop reading current partition
	ClearSequencer();		// Stop driver interrupt function
	Delete(m_pSongPackage);	// Dispose of the music
	m_pSongPackage = NULL;
	m_bPaused = FALSE;
}

/*! ************************************

	\brief Reset music sequencer

***************************************/

void BURGER_API Burger::Sequencer::Reset(void)
{
	ClearChannels();
	m_uBytesGenerated = 0;
	m_uBytesToGenerate = 0;
	m_uSpeedCounter = 128;

	m_uPartitionPosition = 0;
	m_uPatternPosition = 0;
	const SongPackage *pPackage = m_pSongPackage;
	if (pPackage) {
		m_uCurrentIDPattern = pPackage->m_SongDescription.m_PatternPointers[0];
		m_uCurrentSpeed = pPackage->m_SongDescription.m_uDefaultSpeed;
		m_uFineSpeed = pPackage->m_SongDescription.m_uDefaultTempo;
	}
}

/*! ************************************

	\brief Pause music sequencer

***************************************/

void BURGER_API Burger::Sequencer::Pause(void)
{
	if (!m_bPaused) {
		StopSequencing();		// Pause the song
		ClearChannels();		// Stop all instruments
		m_bPaused = TRUE;
	}
}

/*! ************************************

	\brief Resume music sequencer

***************************************/

void BURGER_API Burger::Sequencer::Resume(void)
{
	if (m_bPaused) {	
		StartSequencing();		// Resume the song
		m_bPaused = FALSE;
	}
}

/*! ************************************

	\brief Set the maximum number of voices

	\param uMaxVoices Maximum number of voices to use

***************************************/

void BURGER_API Burger::Sequencer::SetMaxVoices(uint_t uMaxVoices)
{
	uMaxVoices = (uMaxVoices+1)&(~1);
	if (uMaxVoices<2) {
		uMaxVoices = 2;
	} else if (uMaxVoices>=cTrackMaxCount) {
		uMaxVoices = cTrackMaxCount;
	}
	m_uMaxVoices = uMaxVoices;
}

/*! ************************************

	\brief Set the bit size for the final sound output

	\param uOutputDataType 8 or 16 to set the bit size.

***************************************/

void BURGER_API Burger::Sequencer::SetOutputDataType(SoundManager::eDataType uOutputDataType)
{
	m_eOutputDataType = uOutputDataType;
}

/*! ************************************

	\brief Set the sample rate for the final sound output

	\param uSampleRate 5000 to 48000

***************************************/

void BURGER_API Burger::Sequencer::SetSampleRate(uint_t uSampleRate)
{
	if (uSampleRate<5000) {
		uSampleRate = 5000;
	} else if (uSampleRate>=48000) {
		uSampleRate = 48000;
	}
	m_uFinalSampleRate = uSampleRate;
}

/*! ************************************

	\brief Set the delay duration

	\param uMicroDelayDuration 0 turns it off and 1000 is one second (In milliseconds)

***************************************/

void BURGER_API Burger::Sequencer::SetMicroDelayDuration(uint_t uMicroDelayDuration)
{
	if (uMicroDelayDuration>=1000) {
		uMicroDelayDuration = 1000;
	}
	m_uMicroDelayDuration = uMicroDelayDuration;
}

/*! ************************************

	\brief Set the reverb settings

	\param uReverbSizeMS 24 or less turns it off and 1000 is one second (In milliseconds)
	\param uReverbStrength 0 turns it off to 70 in percentage (0-70)

***************************************/

void BURGER_API Burger::Sequencer::SetReverb(uint_t uReverbSizeMS,uint_t uReverbStrength)
{
	if ((uReverbSizeMS<25) || !uReverbStrength) {
		m_bReverbEnabled = FALSE;
		uReverbSizeMS = 0;
		uReverbStrength = 0;
	} else {
		if (uReverbStrength>=70) {
			uReverbStrength = 70;
		}
		if (uReverbSizeMS>=1000) {
			uReverbSizeMS = 1000;
		}
		m_bReverbEnabled = TRUE;
		m_uReverbSizeMS = uReverbSizeMS;
		m_uReverbStrength = uReverbStrength;
	}
}
