/***************************************

    Sound manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brsound.h"
#include "brsounddecompress.h"
#include "brendian.h"
#include "brfilelbm.h"
#include "brfixedpoint.h"
#include "brmace.h"
#include "bralaw.h"
#include "brulaw.h"
#include "brmicrosoftadpcm.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)

//
// .WAV file header structure
//

struct WavHeader_t {
	uint8_t szRIFF[4];			// ASCII RIFF
	uint32_t dwFormatLength;		// Length of file contents
	uint8_t szWAVE[4];			// ASCII WAVE
	uint8_t szFMT[4];				// ASCII fmt_
	uint32_t dwWaveFormatLength;	// Size of fmt_ struct (16)
	uint16_t wFormatTag;			// Compression type
	uint16_t wChannels;			// Number of sound channels (1,2)
	uint32_t dwSamplesPerSec;		// Sample rate
	uint32_t dwAvgBytesPerSec;	// Bytes per second	
	uint16_t wBlockAlign;			// Data alignment (2)
	uint16_t wBitsPerSample;		// Bits per sample (8,16)
	uint16_t wExtSize;			// Extra data for compressed formats
	uint16_t wSamplesPerBlock;	// Number of samples in each block
	uint16_t wNumCoefs;			// Number of coefs in tables
};

//
// WAV file chunk info
//

struct WavData_t {
	uint8_t m_DataASCII[4];	// ASCII data 
	uint32_t m_uDataLength;	// Size of the data
	uint8_t m_Data[1];		// Array of byte dwDataLength in size
};

//
// Data for AIFF files (Big endian)
//

struct AIFFHeader_t {		// 12 byte header for ALL IFF files
	char m_FormASCII[4];	// FORM
	uint32_t m_uFileSize;		// Length of the file contents
	char m_AIFFASCII[4];	// AIFF
};

struct AIFFCommon_t {			// COMM chunk
	char m_CommASCII[4];		// COMM
	uint32_t m_uChunkLength;		// Length of the struct
	uint16_t m_uNumChannels;		// Number of channels (1 or 2)
	uint16_t m_uNumSampleFramesHI;	// Number of SAMPLES, not bytes (16 bit aligned)
	uint16_t m_uNumSampleFramesLO;	
	uint16_t m_uSampleSize;		// 8 or 16 bit
	Burger::Float80Bit m_fxSampleRate;	// Extended floating point
	// AIFC specific data (m_uChunkLength>18)
	char m_CompressionType[4];	// 4 character code for compression (16 bit aligned)
	uint8_t m_CompressionString[1];	// Pascal string with the name of the compression algorithm
};

struct AIFFSsnd_t {			// SSND chunk
	char m_SsndASCII[4];	// SSND
	uint32_t m_uChunkLength;	// Length of the data
	uint32_t m_uOffset;		// Offset to the start
	uint32_t m_uBlockSize;	// Size of each unit
	uint8_t m_Data[1];		// Data
};

//
// Data for VOC files (Little endian)
//

#define VOCHHEADERSIZE 26
struct VOCHeader_t {		// Voc file header
	char m_NameASCII[20];	// "Creative Voice File\x1A"
	uint16_t m_uOffset;		// Offset to the header
	uint16_t m_uVersion;		// Version number
	uint16_t m_uChecksum;		// ~(Version - 0x1234)
};

struct VOCChunk_t {
	enum {
		CHUNK_TERMINATOR=0,
		CHUNK_VOICE_DATA=1,
		CHUNK_VOICE_CONTINUATION=2,
		CHUNK_SILENCE=3,
		CHUNK_MARKER=4,
		CHUNK_ASCII_TEXT=5,
		CHUNK_REPEAT_LOOP=6,
		CHUNK_END_REPEAT_LOOP=7,
		CHUNK_EXTENDED=8,
		CHUNK_STEREO=9
	};
	uint8_t m_uChunkType;	// Chunk type
	uint8_t m_uLengthLo;	// 3 bytes for the length
	uint16_t m_uLengthHi;	// upper 2 bytes for length
	BURGER_INLINE uintptr_t GetLength(void) const { return (static_cast<uint32_t>(Burger::LittleEndian::load(&m_uLengthHi))<<8U)+m_uLengthLo; }
};

struct VOCSoundData1_t : public VOCChunk_t {
	enum {
		TYPEBYTE=0,
		TYPE4TO8=1,
		TYPE3TO8=2,
		TYPE2TO8=3,
		TYPEMULTIDAC=4
	};
	uint8_t m_uSampleRate;		// Use the function below to convert to sample rate
	uint8_t m_uCompressionType;	// Type of data
	uint8_t m_Data[1];			// Data
	BURGER_INLINE uint32_t GetSampleRate(void) const { return 1000000U / (256U - m_uSampleRate); }
};

struct VOCSoundData9_t : public VOCChunk_t {
	enum {
		TYPEBYTE=0,
		TYPE8TO4ADPCM=1,
		TYPE8TO3ADPCM=2,
		TYPE8TO2ADPCM=3,
		TYPELSHORT=4,
		TYPEALAW=6,
		TYPEULAW=7,
		TYPE16TO4ADPCM=8192
	};
	uint32_t m_uSampleRate;	// Samples per second
	uint8_t m_uBitsPerSample;	// 8 or 16 bits
	uint8_t m_uChannels;		// 1 or 2 channels
	uint16_t m_uCompression;	// A four
	uint8_t m_Reserved[4];	// Padding
	uint8_t m_Data[1];		// Data
};

BURGER_CREATE_STATICRTTI_PARENT(Burger::SoundManager,Burger::Base);
BURGER_CREATE_STATICRTTI_PARENT(Burger::SoundManager::Buffer,Burger::ReferenceCounter);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::SoundManager::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
/*! ************************************

	\var const Burger::StaticRTTI Burger::SoundManager::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/



/*! ************************************

	\class Burger::SoundManager

	This class manages sound output to the application's speakers
	
	All of the underlying code is system specific

***************************************/


/*! ************************************

	\brief Construct a SoundCardDescription

	Constructor

***************************************/

Burger::SoundManager::SoundCardDescription::SoundCardDescription() :
	m_DeviceName(),
	m_uDevNumber(0),
	m_b8Bit(FALSE),
	m_b16Bit(FALSE),
	m_bStereo(FALSE),
	m_bHardwareAccelerated(FALSE),
	m_uMinimumSampleRate(22050),
	m_uMaximumSampleRate(22050)
{
#if defined(BURGER_WINDOWS)
	MemoryClear(&m_GUID,sizeof(m_GUID));
#endif
}

/*! ************************************

	\brief Release a SoundCardDescription

	Destructor

***************************************/

Burger::SoundManager::SoundCardDescription::~SoundCardDescription()
{
}

/*! ************************************

	\brief Construct a BufferDecoder

	Constructor

***************************************/

Burger::SoundManager::BufferDecoder::BufferDecoder()
{
	MemoryClear(this,sizeof(*this));
}

/*! ************************************

	\brief Destruct a BufferDecoder

	Destructor

***************************************/

Burger::SoundManager::BufferDecoder::~BufferDecoder()
{
	Delete(m_pDecompresser);
	m_pDecompresser = NULL;
}

/*! ************************************

	\brief Clear out the BufferDecoder

	Release any previous data this decoder managed

***************************************/

void BURGER_API Burger::SoundManager::BufferDecoder::Clear(void)
{
	// Release any decompresser
	Delete(m_pDecompresser);
	m_pDecompresser = NULL;
	m_uCompressedLength = 0;
	m_uSampleCount = 0;
	MemoryClear(static_cast<BufferDescription_t *>(this),sizeof(BufferDescription_t));
}


/*! ************************************

	\brief Fill in a BufferDecoder_t from a sound file

	Given a file image of one of several popular file formats,
	determine the type of data contained and initialize the 
	structure to point to the data inside the audio file

	\param pInput Pointer to the image to the audio file
	\param uLength Length of the file image in bytes
	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::SoundManager::BufferDecoder::ParseSoundFileImage(const void *pInput,uintptr_t uLength)
{
	// Clear out the struct
	Clear();

	// Special data for ADPCM
	uint_t uSamplesPerBlock = 0;
	uint_t uBlockAlign = 0;

	// Assume error
	uint_t bResult = TRUE;			

	//
	// Is this a windows .WAV file?
	//

	if ((uLength >= cWAVHeaderSize) &&
		(BigEndian::load(static_cast<const uint32_t *>(pInput)) == cRIFFASCII) &&
		(BigEndian::load(static_cast<const uint32_t *>(pInput)+2) == cWAVEASCII)) {

		// setup a pointer to the wave header
		// set size of the sample and pointer to the sample
		const WavHeader_t *pHeader = static_cast<const WavHeader_t *>(pInput);

		// Get the wav data chunk

		const WavData_t *pWavData = static_cast<const WavData_t *>(FindRIFFChunk(pInput,uLength,cDATAASCII));
		if (pWavData) {
			// Get the pointer to the data and its length
			m_pSoundImage = pWavData->m_Data;
			m_uCompressedLength = LittleEndian::load(&pWavData->m_uDataLength);

			// Samples per second
			m_uSampleRate = LittleEndian::load(&pHeader->dwSamplesPerSec);
		
			uint_t uChannels = LittleEndian::load(&pHeader->wChannels);
			uint_t uWAVFormat = LittleEndian::load(&pHeader->wFormatTag);

			// Each data format needs different handling

			switch (uWAVFormat) {

			// PCM data (Uncompressed audio)
			default:
			case 1:
				// 8 bit data?
				if (LittleEndian::load(&pHeader->wBitsPerSample) == 8) {	
					m_eDataType = TYPEBYTE;
				} else {
				// Little endian 16 bit samples
					m_eDataType = TYPELSHORT;
				}
				m_uSoundLength = m_uCompressedLength;
				bResult = FALSE;		// It's ok
				break;

			// ADPCM data
			case 2:			
				// Only 4 bit data is supported right now
				if (LittleEndian::load(&pHeader->wBitsPerSample) == 4) {

					m_eDataType = TYPEADPCM;
					uSamplesPerBlock = LittleEndian::load(&pHeader->wSamplesPerBlock);
					uBlockAlign = LittleEndian::load(&pHeader->wBlockAlign);

					// Number of packets in the data
					uintptr_t uPackets = m_uCompressedLength / uBlockAlign;

					// Number of bytes generated from the samples
					m_uSoundLength = ((uPackets * uSamplesPerBlock) * uChannels);
				
					// Next, for any partial blocks, subtract overhead from it and it
					// will leave # of samples to read.
				
					m_uSoundLength += ((m_uCompressedLength - (uPackets * uBlockAlign)) - (6 * uChannels)) * uChannels;
					// Double since it decompresses to 16 bit data (Not bytes)
					m_uSoundLength *= 2;
					bResult = FALSE;
				}
				break;

			case 3:
				// Floating point data?
				if (LittleEndian::load(&pHeader->wBitsPerSample) == 32) {	
					m_eDataType = TYPELFLOAT;
					m_uSoundLength = m_uCompressedLength;
					bResult = FALSE;		// It's ok
				}
				break;

			// ALaw data
			case 6:
				// 8 bit data?
				if (LittleEndian::load(&pHeader->wBitsPerSample) == 8) {
					m_eDataType = TYPEALAW;
					// Will decompress into shorts from bytes
					m_uSoundLength = m_uCompressedLength*2;
					bResult = FALSE;
				}
				break;

			// uLaw data
			case 7:
				// 8 bit data?
				if (LittleEndian::load(&pHeader->wBitsPerSample) == 8) {
					m_eDataType = TYPEULAW;
					// Will decompress into shorts from bytes
					m_uSoundLength = m_uCompressedLength*2;
					bResult = FALSE;
				}
				break;
			}

			// Is it stereo?
			if (uChannels==2) {
				m_eDataType = static_cast<eDataType>(m_eDataType|TYPESTEREO);
			}
		}

//
// Is this an AIFF or AIFC file?
//

	} else if ((uLength>=12) &&
		(BigEndian::load(static_cast<const uint32_t *>(pInput)) == cFORMASCII) &&
		((BigEndian::load(static_cast<const uint32_t *>(pInput)+2) == cAIFFASCII) ||
		(BigEndian::load(static_cast<const uint32_t *>(pInput)+2) == cAIFCASCII))) {

		// Get the COMM record
		const AIFFCommon_t *pCommonChunk = static_cast<const AIFFCommon_t *>(FindAIFFChunk(pInput,uLength,cCOMMASCII));
		if (pCommonChunk) {
			// Get the sound data
			const AIFFSsnd_t *pSoundChunk = static_cast<const AIFFSsnd_t *>(FindAIFFChunk(pInput,uLength,cSSNDASCII));
			if (pSoundChunk) {
				// Set up data
				m_pSoundImage = pSoundChunk->m_Data;
				m_uCompressedLength = BigEndian::load(&pSoundChunk->m_uChunkLength)-8;

				// Sample rate is stored as a big endian 80 bit float
				m_uSampleRate = static_cast<uint_t>(static_cast<int>(BigEndianLoadExtended(pCommonChunk->m_fxSampleRate)));

				// 8 bit data?
				uint_t uSampleSize = BigEndian::load(&pCommonChunk->m_uSampleSize);
				if (uSampleSize == 8) {
					m_eDataType = TYPECHAR;
				} else if (uSampleSize == 16) {
					// 16 bit big endian samples
					m_eDataType = TYPEBSHORT;		
				} else {
					m_eDataType = TYPEBFLOAT;
				}
				m_uSoundLength = m_uCompressedLength;

				// Was the data compressed?
				if (BigEndian::load(static_cast<const uint32_t *>(pInput)+2) == cAIFCASCII) {
					// Mace 6?
					uint32_t uCompressionType = BigEndian::load_unaligned(reinterpret_cast<const uint32_t *>(static_cast<const void *>(&pCommonChunk->m_CompressionType)));
					if (uCompressionType==cMAC6ASCII) {
						m_eDataType = TYPEMACE6;
						m_uSoundLength = 6*m_uCompressedLength;

					// Mace 3?
					} else if (uCompressionType==cMAC3ASCII) {
						m_eDataType = TYPEMACE3;
						m_uSoundLength = 3*m_uCompressedLength;

					// alaw
					} else if (uCompressionType==calawASCII) {
						m_eDataType = TYPEALAW;
						m_uSoundLength = 2*m_uCompressedLength;

					// ulaw
					} else if (uCompressionType==culawASCII) {
						m_eDataType = TYPEULAW;
						m_uSoundLength = 2*m_uCompressedLength;
					// float 32
					} else if (uCompressionType==cfl32ASCII) {
						m_eDataType = TYPEBFLOAT;
						m_uSoundLength = m_uCompressedLength;
					} else {
						// Unsupported compression
						return bResult;
					}
				}

				// Handle stereo
				if (BigEndian::load(&pCommonChunk->m_uNumChannels) == 2) {
					m_eDataType = static_cast<eDataType>(m_eDataType|TYPESTEREO);
				}
				bResult = FALSE;
			}
		}

//
// Is this a VOC file?
//

	} else if ((uLength>=(sizeof(VOCChunk_t)+VOCHHEADERSIZE)) &&
		!MemoryCompare(static_cast<const VOCHeader_t*>(pInput)->m_NameASCII,"Creative Voice File\x1A",20) &&
		LittleEndian::load(&static_cast<const VOCHeader_t*>(pInput)->m_uVersion) == static_cast<uint16_t>((~LittleEndian::load(&static_cast<const VOCHeader_t*>(pInput)->m_uChecksum))+0x1234)) {

		//
		// Passed the validations. It's a VOC file
		//

		const VOCChunk_t *pVOCChunk = reinterpret_cast<const VOCChunk_t *>(static_cast<const uint8_t *>(pInput)+LittleEndian::load(&static_cast<const VOCHeader_t*>(pInput)->m_uOffset));

		uintptr_t uTempLength = uLength-VOCHHEADERSIZE;
		uint_t bContinue;
		do {
			// Get the size of the current chunk

			uintptr_t uChunkSize = pVOCChunk->GetLength();
			bContinue = TRUE;
			switch (pVOCChunk->m_uChunkType) {

			// Terminator aborts now
			case VOCChunk_t::CHUNK_TERMINATOR:
				bContinue = FALSE;
				break;

			// Simple format
			case VOCChunk_t::CHUNK_VOICE_DATA:
				{
					const VOCSoundData1_t *pVOCData = static_cast<const VOCSoundData1_t *>(pVOCChunk);
					m_pSoundImage = pVOCData->m_Data;	// Pointer to the wav data
					m_uCompressedLength = uChunkSize-2;
					m_uSampleRate = pVOCData->GetSampleRate();
					if (pVOCData->m_uCompressionType==VOCSoundData1_t::TYPEBYTE) {
						m_eDataType = TYPEBYTE;
						m_uSoundLength = m_uCompressedLength;
						bResult = FALSE;
						bContinue = FALSE;
					}
				}
				break;

			// Version 1.20 or higher
			case VOCChunk_t::CHUNK_STEREO:
				{
					const VOCSoundData9_t *pVOCData = static_cast<const VOCSoundData9_t *>(pVOCChunk);
					m_pSoundImage = pVOCData->m_Data;	// Pointer to the wav data
					m_uCompressedLength = uChunkSize-12;
					m_uSampleRate = LittleEndian::load_unaligned(&pVOCData->m_uSampleRate);
					m_uSoundLength = m_uCompressedLength;
					if (pVOCData->m_uCompression==VOCSoundData9_t::TYPEBYTE) {
						m_eDataType = TYPECHAR;
						bResult = FALSE;
						bContinue = FALSE;
					} else if (pVOCData->m_uCompression==VOCSoundData9_t::TYPELSHORT) {
						// Little endian shorts
						m_eDataType = TYPELSHORT;
						bResult = FALSE;
						bContinue = FALSE;
					}
					if (pVOCData->m_uChannels==2) {
						m_eDataType = static_cast<eDataType>(m_eDataType|TYPESTEREO);
					}
				}
				break;
			// Unknown chunks are ignored
			default:
				break;
			}
			// Off the end of the data?
			uChunkSize+=4;	// Add in sizeof(VOCChunk_t)
			if (uChunkSize>=uTempLength) {
				break;
			}
			uTempLength-=uChunkSize;
			pVOCChunk = reinterpret_cast<const VOCChunk_t *>(reinterpret_cast<const uint8_t *>(pVOCChunk)+uChunkSize);
		} while (bContinue);

		
//
// Is this an OGG/Vorbis file?
//

	} else if ((uLength>=0x2C) &&
		(BigEndian::load(static_cast<const uint32_t *>(pInput)) == cOggSASCII)) {
		m_eDataType = TYPEOGG;
		m_pSoundImage = static_cast<const uint8_t *>(pInput);
		m_uSoundLength = UINTPTR_MAX;		// Can't tell unless I scan the whole thing
		m_uSampleRate = LittleEndian::load(&static_cast<const uint32_t *>(pInput)[0x28/4]);
		if (static_cast<const uint8_t *>(pInput)[0x27]==2) {
			m_eDataType = static_cast<eDataType>(m_eDataType|TYPESTEREO);
		}
		bResult = FALSE;
	}
	
	// Is this valid?
	if (!bResult) {
		
		// Type of data
		DecompressAudio *pDecompresser = NULL;
		switch (m_eDataType&TYPEMASK) {
		case TYPELFLOAT:
			pDecompresser = Decompress32BitLEAudio::New();
			break;

		case TYPEBFLOAT:
			pDecompresser = Decompress32BitBEAudio::New();
			break;
				
		case TYPELSHORT:
			pDecompresser = Decompress16BitLEAudio::New();
			break;

		case TYPEBSHORT:
			pDecompresser = Decompress16BitBEAudio::New();
			break;

		default:
		case TYPEBYTE:
			pDecompresser = DecompressUnsigned8BitAudio::New();
			break;

		case TYPECHAR:
			pDecompresser = DecompressSigned8BitAudio::New();
			break;

		case TYPEMACE3:
			pDecompresser = DecompressMace3::New();
			break;

		case TYPEMACE6:
			pDecompresser = DecompressMace6::New();
			break;

		case TYPEALAW:
			pDecompresser = DecompressALaw::New();
			break;

		case TYPEULAW:
			pDecompresser = DecompressULaw::New();
			break;

		case TYPEADPCM:
			pDecompresser = DecompressMicrosoftADPCM::New();
			static_cast<DecompressMicrosoftADPCM *>(pDecompresser)->SetBlockSize(uBlockAlign);
			static_cast<DecompressMicrosoftADPCM *>(pDecompresser)->SetSamplesPerBlock(uSamplesPerBlock);
			break;

		}

		m_pDecompresser = pDecompresser;

		// Determine the number of SAMPLES from the bytes

		uintptr_t uSamples = m_uSoundLength;	// Number of bytes in the data
		if (m_eDataType&TYPESTEREO) {
			uSamples >>= 1U;				// Cut in half for stereo
			m_pDecompresser->SetStereo();
		}
		eDataType uTempType = m_pDecompresser->GetDataType();
		if ((uTempType==TYPELSHORT) || 
			(uTempType==TYPEBSHORT)) {
			uSamples >>= 1U;			// Cut in half for 16 bit samples
		} else if ((uTempType==TYPELFLOAT) ||
			(uTempType==TYPEBFLOAT)) {
			uSamples >>= 2U;			// Cut in a quarter for 32 bit samples
		}
		m_uSampleCount = uSamples;
	}
	return bResult;
}

/*! ************************************

	\brief Fill in a BufferDecoder_t from a description

	Given a description to raw data, set up the buffer for
	uploading.

	\param pInput Pointer to a description of the audio data
	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::SoundManager::BufferDecoder::Parse(const BufferDescription_t *pInput)
{
	// Assume error
	uint_t bResult = TRUE;

	Clear();
	static_cast<BufferDescription_t *>(this)[0] = pInput[0];
	m_uCompressedLength = 0;

	// Each data format needs different handling

	switch (m_eDataType&TYPEMASK) {
	// PCM data (Uncompressed audio)
	case TYPEBYTE:
	case TYPECHAR:
	case TYPELSHORT:
	case TYPEBSHORT:
	case TYPELFLOAT:
	case TYPEBFLOAT:
		bResult = FALSE;
		break;

	case TYPEMACE3:
		m_uCompressedLength = m_uSoundLength;
		m_uSoundLength *= 3;
		bResult = FALSE;
		break;

	case TYPEMACE6:
		m_uCompressedLength = m_uSoundLength;
		m_uSoundLength *= 6;
		bResult = FALSE;
		break;

	case TYPEULAW:
	case TYPEALAW:
		m_uCompressedLength = m_uSoundLength;	// Compressed data length
		// Will decompress into shorts from bytes
		m_uSoundLength *= 2;
		bResult = FALSE;
		break;

	default:
		break;
	}

	// Is this valid?
	if (!bResult) {

		// Determine the number of SAMPLES from the bytes

		uintptr_t uSamples = m_uSoundLength;	// Number of bytes in the data
		if (m_eDataType&TYPESTEREO) {
			uSamples >>= 1U;				// Cut in half for stereo
		}
		
		// Type of data
		switch (m_eDataType&TYPEMASK) {
		case TYPELFLOAT:
		case TYPEBFLOAT:
			uSamples >>= 2U;			// Cut in 1/4 for 32 bit samples
			break;
		case TYPELSHORT:
		case TYPEBSHORT:
			uSamples >>= 1U;			// Cut in half for 16 bit samples
		default:
			break;
		}
		m_uSampleCount = uSamples;		// Save the count in samples
	}
	return bResult;
}


/*! ************************************

	\class Burger::SoundManager::Buffer
	\brief Audio data class

	Class containing audio data which may or may
	not be uploaded to audio hardware

	\sa \ref Voice or \ref SoundManager

***************************************/

/*! ************************************

	\brief Constructor

	Initialize the variables to defaults. Follow
	up with a call to Init(const void *,uintptr_t) or
	Init(const BufferDescription_t *).

	\sa Shutdown()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)

Burger::SoundManager::Buffer::Buffer() :
	m_pBufferData(NULL),
	m_uBufferSize(0),
	m_uPan(cPanCenter),
	m_uVolume(cMaxVolume)
{
}

#endif

/*! ************************************

	\brief Destructor

	Releases all platform specific data and
	any buffers that were allocated by this class

	\sa Shutdown()

***************************************/

Burger::SoundManager::Buffer::~Buffer()
{
	Shutdown();
}

/*! ************************************

	\brief Attach sound data to buffer

	Pass a pointer to a sound file loaded into memory
	and this function will parse the file and determine
	how to play the file and sets up some defaults

	SampleRate, Pan and Volume are set to defaults in this function

	\note This function assumes the audio data is persistent

	\param pSoundFile Pointer to the sound file in memory
	\param uLength Length of the sound file in bytes
	\return Zero if no error, non-zero on error
	\sa Init(const BufferDescription_t *)

***************************************/

uint_t BURGER_API Burger::SoundManager::Buffer::Init(const void *pSoundFile,uintptr_t uLength)
{
	m_uPan = cPanCenter;
	m_uVolume = cMaxVolume;
	return m_Decoder.ParseSoundFileImage(pSoundFile,uLength);
}

/*! ************************************

	\brief Attach sound data to buffer

	Pass a pointer to a structure that describes audio data
	in memory and set up this buffer to use this data.

	\note This function assumes the audio data is persistent

	SampleRate, Pan and Volume are set to defaults in this function

	\param pRawData Pointer to the description of the audio data
	\return Zero if no error, non-zero on error
	\sa Init(const void *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::SoundManager::Buffer::Init(const BufferDescription_t *pRawData)
{
	m_uPan = cPanCenter;
	m_uVolume = cMaxVolume;
	return m_Decoder.Parse(pRawData);
}

/*! ************************************

	\brief Release resources

	Releases all platform specific data and any
	ownership of audio data passed in.

	\sa Init(const void *,uintptr_t) or Init(const void *,uintptr_t)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
void Burger::SoundManager::Buffer::Shutdown(void)
{
	Free(m_pBufferData);
	m_pBufferData = NULL;
	m_uBufferSize = 0;
}
#endif

/*! ************************************

	\brief Upload audio data to hardware

	For some platforms, audio data must be uploaded to
	specific memory dedicated for audio playback. This
	function will determine if the memory has not been
	uploaded and if it hasn't, it will perform
	the upload.

	\param pSoundManager Pointer to the parent sound manager class
	\return Zero if no error, non-zero on error

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
uint_t Burger::SoundManager::Buffer::Upload(SoundManager * /* pSoundManager */)
{
	uint_t uResult = 0;
	// Not already uploaded?
	if (!m_pBufferData) {
		// Create a buffer for the sound
		
		uintptr_t uBufferSize = m_Decoder.m_uSoundLength;
		void *pBuffer = Alloc(uBufferSize);
		if (pBuffer) {
			m_pBufferData = pBuffer;
			m_uBufferSize = uBufferSize;
			DecompressAudio *pDecompresser = m_Decoder.GetDecompresser();
			pDecompresser->Reset();
			// Decompress the data
			pDecompresser->Process(pBuffer,uBufferSize,m_Decoder.m_pSoundImage,m_Decoder.GetCompressedSize());
		} else {
			uResult = 10;
		}
	}
	return uResult;
}
#endif

/*! ************************************

	\brief Create a new sound buffer

***************************************/

Burger::SoundManager::Buffer * BURGER_API Burger::SoundManager::Buffer::New(void)
{
	Buffer *pBuffer = new (Alloc(sizeof(Buffer))) Buffer();
	if (pBuffer) {
		pBuffer->AddRef();
	}
	return pBuffer;
}

/*! ************************************

	\fn BufferDecoder_t *Burger::SoundManager::Buffer::GetBufferDescription(void)
	\brief Returns the contained BufferDecoder_t state

	Accessor for the BufferDecoder_t

	\return Pointer to the contained BufferDecoder_t

***************************************/

/*! ************************************

	\fn const BufferDecoder_t *Burger::SoundManager::Buffer::GetBufferDescription(void) const
	\brief Returns the contained BufferDecoder_t state

	Accessor for the BufferDecoder_t

	\return Pointer to the contained BufferDecoder_t as const

***************************************/

/*! ************************************

	\fn void Burger::SoundManager::Buffer::SetSampleRate(uint_t uSampleRate)
	\brief Sets the buffer's sample rate

	Sets the default sample rate to playback the audio data.

	\param uSampleRate New sample rate
	\sa GetSampleRate(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SoundManager::Buffer::GetSampleRate(void) const
	\brief Gets the buffer's sample rate

	Gets the default sample rate for the audio data.

	\return The buffer's default sample rate
	\sa SetSampleRate(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::SoundManager::Buffer::SetPan(uint_t uPan)
	\brief Sets the buffer's stereo pan setting

	Sets the default pan setting for audio data playback.

	\param uPan New default stereo pan setting
	\sa GetPan(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SoundManager::Buffer::GetPan(void) const
	\brief Gets the buffer's stereo pan setting

	Gets the default pan setting for the audio data.

	\return The buffer's default pan setting
	\sa SetPan(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::SoundManager::Buffer::SetVolume(uint_t uVolume)
	\brief Sets the buffer's default volume

	Sets the default volume setting for audio data playback.

	\param uVolume New default volume setting
	\sa GetVolume(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SoundManager::Buffer::GetVolume(void) const
	\brief Gets the buffer's default volume setting

	Gets the default volume setting for the audio data.

	\return The buffer's default volume setting
	\sa SetVolume(uint_t)

***************************************/

/*! ************************************

	\fn IDirectSoundBuffer8 *Burger::SoundManager::Buffer::GetDirectSoundBuffer8(void) const
	\brief Gets the buffer's DirectSoundBuffer8

	If a buffer was allocated, returns the pointer to the attached
	DirectSoundBuffer8

	\windowsonly
	\return The attached DirectSoundBuffer8

***************************************/





/*! ************************************

	\class Burger::SoundManager::Voice
	\brief Audio playback class

	Class containing audio playback state
	for a single voice. This is the class
	that controls the playback of a single
	channel

	\sa \ref Buffer or \ref SoundManager

***************************************/


/*! ************************************

	\brief Constructor

	Initialize the structure to defaults

	\sa Shutdown(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
Burger::SoundManager::Voice::Voice() :
	m_pBuffer(),
	m_pCallBack(NULL),
	m_pCallBackData(NULL),
	m_uLoopStart(0),
	m_uLoopEnd(0),
	m_uPausedMark(0),
	m_uSampleRate(BURGER_MAXUINT),
	m_uPan(BURGER_MAXUINT),
	m_uVolume(BURGER_MAXUINT),
	m_bPlaying(FALSE),
	m_bPaused(FALSE),
	m_bAvailable(TRUE),
	m_bIsHeld(FALSE)
{
}
#endif

/*! ************************************

	\brief Destructor

	Release all resources

	\sa Shutdown(void)

***************************************/

Burger::SoundManager::Voice::~Voice()
{
	Shutdown();
}


/*! ************************************

	\brief Initialize a voice for playback

	Set up resources needed for playback,
	sample rate, pan and volume are present.

	The voice is ready for playback. Call Start() to
	start the sound.

	\param pManager Pointer to the parent SoundManager
	\param pBuffer Pointer to the buffer to use for playback
	\return Zero if no error, non-zero if error
	\sa Start(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
uint_t BURGER_API Burger::SoundManager::Voice::Init(SoundManager *pManager,Buffer *pBuffer)
{
	BURGER_UNUSED(pManager);

	// Attach to the buffer (Obtain a reference via a smart pointer)
	m_pBuffer = pBuffer;

	// Grab the default settings from the buffer's description
	const BufferDecoder *pDecoder = pBuffer->GetBufferDescription();

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
	return uResult;
}

/*! ************************************

	\brief Stop playback and release all resources

	Release the resources allocated by this voice.

	\sa Stop(void)

***************************************/

void BURGER_API Burger::SoundManager::Voice::Shutdown(void)
{
}

/*! ************************************

	\brief Release the connection to the application

	If a pointer to a Voice is requested, it's marked
	as unable to be used and under application control.

	Calling this function will release the voice back to the
	free pool once the playback has stopped automatically.

	If the sound is a looping sound, it will stop immediately

	\sa Shutdown(void)

***************************************/

void BURGER_API Burger::SoundManager::Voice::Release(void)
{
	m_bIsHeld = FALSE;
	m_bAvailable = TRUE;
}


/*! ************************************

	\brief Start playback

	Start the voice

	\return Zero if no error, non-zero if error
	\sa Stop(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Start(void)
{
	m_bPlaying = FALSE;		// TRUE;
	m_bPaused = FALSE;
	return 10;
}

/*! ************************************

	\brief Stop playback

	Stop the voice

	\return Zero if no error, non-zero if error
	\sa Start(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Stop(void)
{
	m_bPlaying = FALSE;
	m_bPaused = FALSE;
	return 10;
}

/*! ************************************

	\brief Pause playback

	Stop the voice and record the location to allow
	playback to resume with a call to Resume()

	\return Zero if no error, non-zero if error
	\sa Resume(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Pause(void)
{
	m_uPausedMark = 0;
	m_bPlaying = FALSE;
	m_bPaused = TRUE;
	return 10;
}

/*! ************************************

	\brief Resume playback

	Start the voice at the location recorded
	from the last call to Pause()

	\return Zero if no error, non-zero if error
	\sa Pause(void)

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::Resume(void)
{
	m_bPlaying = FALSE;
	m_bPaused = FALSE;
	return 10;
}

/*! ************************************

	\brief Set the volume of the voice

	Immediately change the volume of this specific voice

	\param uVolume Volume from 0 to 255 with 0 being silence and 255 being maximum
	\return Zero if no error, non-zero if error
	\sa GetVolume(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetVolume(uint_t uVolume)
{
	m_uVolume = uVolume;
	return 10;
}

/*! ************************************

	\brief Set the pan setting of the voice

	Immediately change the pan setting of this specific voice

	\param uPan Pan setting from 0 to 0xFFFF with 0 being left, 0x8000 being center and 0xFFFF being right
	\return Zero if no error, non-zero if error
	\sa GetPan(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetPan(uint_t uPan)
{
	m_uPan = uPan;
	return 10;
}

/*! ************************************

	\brief Set the samples per second setting of the voice

	Immediately change the playback speed of this specific voice

	\param uSamplesPerSecond Speed of playback in samples per second
	\return Zero if no error, non-zero if error
	\sa GetSampleRate(void) const

***************************************/

uint_t BURGER_API Burger::SoundManager::Voice::SetSampleRate(uint_t uSamplesPerSecond)
{
	m_uSampleRate = uSamplesPerSecond;
	return 10;
}

/*! ************************************

	\brief Internal callback

	Releases the voice automatically upon completion

***************************************/

void BURGER_API Burger::SoundManager::Voice::ReachedEnd(void) 
{
	m_bPlaying = FALSE;
	if (!m_bIsHeld) {
		Shutdown();
	}
}

#endif

/*! ************************************

	\brief Constructor

	Initialize the variables to defaults. Follow
	up with a call to Init().

	\param pGameApp Pointer to the parent game application
	\sa Shutdown()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
Burger::SoundManager::SoundManager(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_uMaxVoices(0),
	m_uVolume(0),
	m_bStereoAvailable(FALSE),
	m_bMasterVolumeAvailable(FALSE),
	m_uBufferDepth(0),
	m_uOutputSamplesPerSecond(0)
{
}
#endif

/*! ************************************

	\brief Release all resources

	Shut down the Sound class by calling Shutdown() to
	release all of the resources.

	\sa Shutdown()

***************************************/

Burger::SoundManager::~SoundManager()
{
	Shutdown();
}

/*! ************************************

	\brief Allocate and initialize a SoundManager

	\param pGameApp Pointer to the parent game application
	\return A pointer to a default SoundManager class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::SoundManager * BURGER_API Burger::SoundManager::New(GameApp *pGameApp)
{
	// Allocate the memory
	return new (Alloc(sizeof(SoundManager))) SoundManager(pGameApp);
}

/*! ************************************

	\brief Initialize a SoundManager

	\return Zero if no error, non-zero if an error occurred

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
uint_t BURGER_API Burger::SoundManager::Init(void)
{
	return 1;
}

/*! ************************************

	\brief Shut down a SoundManager

***************************************/

void BURGER_API Burger::SoundManager::Shutdown(void)
{
}
#endif

/*! ************************************

	\brief Play a sound

	\return Zero if no error, non-zero if an error occurred

***************************************/

uint_t BURGER_API Burger::SoundManager::Play(Buffer *pBuffer,Voice **ppCookie)
{
	if (ppCookie) {
		ppCookie[0] = NULL;
	}
	
	uint_t uResult = pBuffer->Upload(this);
	if (!uResult) {
		Voice *pVoice = AllocVoice();
		if (!pVoice) {
			uResult = 99;
		} else {
			uResult = pVoice->Init(this,pBuffer);
			if (!uResult) {
				uResult = pVoice->Start();
				if (!uResult) {
					if (ppCookie) {
						pVoice->SetHeld();
						ppCookie[0] = pVoice;
					}
				}
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Stop all sound

	\return Zero if no error, non-zero if an error occurred

***************************************/

uint_t BURGER_API Burger::SoundManager::Stop(void)
{
	uint_t uResult = 0;
	// Start at the max
	uint_t i = cMaxVoiceCount;
	Voice *pWork = m_ActiveVoices;
	do {
		// Check for a free slot
		uResult |= pWork->Stop();
		++pWork;
	} while (--i);
	return uResult;
}

/*! ************************************

	\brief Pause sound

	\return Zero if no error, non-zero if an error occurred

***************************************/

uint_t BURGER_API Burger::SoundManager::Pause(void)
{
	uint_t uResult = 0;
	// Start at the max
	uint_t i = cMaxVoiceCount;
	Voice *pWork = m_ActiveVoices;
	do {
		// Check for a free slot
		uResult |= pWork->Pause();
		++pWork;
	} while (--i);
	return uResult;
}

/*! ************************************

	\brief Resume sound

	\return Zero if no error, non-zero if an error occurred

***************************************/

uint_t BURGER_API Burger::SoundManager::Resume(void)
{
	uint_t uResult = 0;
	// Start at the max
	uint_t i = cMaxVoiceCount;
	Voice *pWork = m_ActiveVoices;
	do {
		// Check for a free slot
		uResult |= pWork->Resume();
		++pWork;
	} while (--i);
	return uResult;
}

/*! ************************************

	\brief Set the maximum number of voices to mix

	\param uMax Number of voices to allow to use
	\sa cMaxVoiceCount

***************************************/

void BURGER_API Burger::SoundManager::SetMaxVoices(uint_t uMax)
{
	if (uMax>=cMaxVoiceCount) {
		uMax = cMaxVoiceCount;
	}
	m_uMaxVoices = uMax;
}

/*! ************************************

	\brief Return the number of free voices

	\return Number of unallocated voices
	\sa cMaxVoiceCount

***************************************/

uint_t BURGER_API Burger::SoundManager::GetActiveVoiceCount(void) const
{
	uint_t uResult = 0;
	// Start at the max
	uint_t i = cMaxVoiceCount;
	const Voice *pWork = m_ActiveVoices;
	do {
		// Check for a free slot
		if (pWork->IsAvailable()) {
			++uResult;
		}
		++pWork;
	} while (--i);
	return uResult;
}

/*! ************************************

	\brief Set the master volume
	
	\param uVolume Master volume 0-255

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
void BURGER_API Burger::SoundManager::SetVolume(uint_t uVolume)
{
	m_uVolume = uVolume;
}
#endif

/*! ************************************

	\brief Create a new sound buffer

***************************************/

Burger::SoundManager::Buffer * BURGER_API Burger::SoundManager::NewBuffer(void *pWaveFile,uintptr_t uLength)
{
	Buffer *pBuffer = Buffer::New();
	if (pBuffer) {
		BufferDecoder *pDecoder = pBuffer->GetBufferDescription();
		if (pDecoder->ParseSoundFileImage(pWaveFile,uLength)) {
			pBuffer->Release();
			pBuffer = NULL;
		}
	}
	return pBuffer;
}

/*! ************************************

	\brief Get a list of available audio modes
	
	\param pOutput Pointer to array of SoundCardDescription entries
	\return Zero if no error, non-zero on error

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
uint_t BURGER_API Burger::SoundManager::GetAudioModes(ClassArray<SoundCardDescription> *pOutput)
{
	pOutput->clear();
	return 10;
}
#endif


/*! ************************************

	\brief Find a free voice channel

	Scan the array of voices and locate a free slot

	\return Pointer to a free slot or \ref NULL if no voices are available

***************************************/

Burger::SoundManager::Voice * BURGER_API Burger::SoundManager::AllocVoice(void)
{
	// Start at the max
	uint_t i = m_uMaxVoices;
	Voice *pResult = NULL;
	if (i) {
		Voice *pWork = m_ActiveVoices;
		do {
			// Check for a free slot
			if (pWork->IsAvailable()) {
				// Found an available voice!
				pResult = pWork;
				break;
			}
			++pWork;
		} while (--i);
	}
	return pResult;
}



/*! ************************************

	\brief Upload sound data

	This function will take signed 8 bit, unsigned 8 bit, big
	and little endian signed 16 bit sound data and copy it
	into either an 8 or 16 bit sound buffer using native
	endian. This is a helper function for the sound manager.

	If the data type is not one of the four types, it will perform
	a simple memory copy

	\param pOutput Pointer to the buffer to receive the converted data
	\param pInput Pointer to the sound data
	\param uLength Length of the sound data in bytes
	\param eType Type of the input sound data 

***************************************/

void BURGER_API Burger::CopySoundData(void *pOutput,const void *pInput,uintptr_t uLength,SoundManager::eDataType eType)
{
	if (uLength) {
		switch (eType) {
		default:
		case SoundManager::TYPEBYTE:
#if defined(BURGER_BIGENDIAN)
		case SoundManager::TYPEBSHORT:
		case SoundManager::TYPEBFLOAT:
#else
		case SoundManager::TYPELSHORT:
		case SoundManager::TYPELFLOAT:
#endif
			// Copy as is
			MemoryCopy(pOutput,pInput,uLength);
			break;
		case SoundManager::TYPECHAR:
			swap_chars_to_bytes(pOutput,pInput,uLength);
			break;

		// Endian swapped shorts?
#if defined(BURGER_BIGENDIAN)
		case SoundManager::TYPELSHORT:
#else
		case SoundManager::TYPEBSHORT:
#endif
			swap_endian(static_cast<uint16_t *>(pOutput),static_cast<const uint16_t *>(pInput),uLength>>1U);
			break;

		// Endian swapped floats?
#if defined(BURGER_BIGENDIAN)
		case SoundManager::TYPELFLOAT:
#else
		case SoundManager::TYPEBFLOAT:
#endif
			swap_endian(static_cast<uint32_t *>(pOutput),static_cast<const uint32_t *>(pInput),uLength>>2U);
			break;
		}
	}
}

/*! ************************************

	\brief Clear sound data

	Fill the sound buffer with data that represents silence.
	For unsigned char, fill with 0x80U, all other formats will fill
	with zero.

	\param pOutput Pointer to the buffer to receive the converted data
	\param uLength Length of the sound data in bytes
	\param eType Type of the input sound data 

***************************************/

void BURGER_API Burger::ClearSoundData(void *pOutput,uintptr_t uLength,SoundManager::eDataType eType)
{
	switch (eType) {
	default:
		MemoryClear(pOutput,uLength);
		break;
	case SoundManager::TYPEBYTE:
		MemoryFill(pOutput,0x80U,uLength);
		break;
	}
}

/*! ************************************

	\brief Merge a left and right audio channel into a single buffer
	
	Given two input buffers of equal size, interleave the buffers
	with the left channel written first and then the right second into
	a buffer that's twice as large as uOutputSize to contain the 
	merged data.

	\param pOutput Pointer to receive the interleaved data (Must be uOutputSize*2 bytes in size)
	\param pLeft Pointer to the left channel data
	\param pRight Pointer to the right channel data
	\param uOutputSize Size in bytes of the left or right channel (Both must be the same size)

	\sa CopyStereoInterleaved(void *,const uint16_t *,const uint16_t *,uintptr_t)

***************************************/

void BURGER_API Burger::CopyStereoInterleaved(void *pOutput,const uint8_t *pLeft,const uint8_t *pRight,uintptr_t uOutputSize)
{
	if (uOutputSize) {
		do {
            // Copy a byte from the left and then the right buffers
            static_cast<uint8_t *>(pOutput)[0] = pLeft[0];
			static_cast<uint8_t *>(pOutput)[1] = pRight[0];

			// Increment the pointers
			pOutput = static_cast<uint8_t *>(pOutput)+2;
			++pLeft;
			++pRight;
		} while (--uOutputSize);
	}
}

/*! ************************************

	\brief Merge a left and right audio channel into a single buffer
	
	Given two input buffers of equal size, interleave the buffers
	with the left channel written first and then the right second into
	a buffer that's twice as large as uOutputSize to contain the 
	merged data.

	\param pOutput Pointer to receive the interleaved data (Must be uOutputSize*4 bytes in size)
	\param pLeft Pointer to the left channel data
	\param pRight Pointer to the right channel data
	\param uOutputSize Size in entries (uint16_t) of the left or right channel (Both must be the same size)

	\sa CopyStereoInterleaved(void *,const uint8_t *,const uint8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::CopyStereoInterleaved(void *pOutput,const uint16_t *pLeft,const uint16_t *pRight,uintptr_t uOutputSize)
{
	if (uOutputSize) {
		do {
			// Copy a byte from the left and then the right buffers
			static_cast<uint16_t *>(pOutput)[0] = pLeft[0];
			static_cast<uint16_t *>(pOutput)[1] = pRight[0];

			// Increment the pointers
			pOutput = static_cast<uint16_t *>(pOutput)+2;
			++pLeft;
			++pRight;
		} while (--uOutputSize);
	}
}

/*! ************************************

	\brief Add in an 8 bit reverb

	Given a scale in the format of a percentage of 0 to 100,
	apply the scaled input buffer to the output buffer via
	addition to generate reverb

	\param pOutput Pointer to the buffer which will have reverb applied
	\param pInput Pointer to the buffer which the reverb comes from
	\param uCount Number of bytes to process
	\param uPercentage Scale of reverb 0 is none, 100 is full volume

***************************************/

void BURGER_API Burger::ComputeReverb(uint8_t *pOutput,const uint8_t *pInput,uintptr_t uCount,uint_t uPercentage)
{
	if (uCount) {
		// Convert the percentage from 100 to 0.0 to 1.0 in 25.7 fixed point
		int iPercentage = static_cast<int>((uPercentage*128U)/100U);
		do {
			// Do a signed scale from 0 (Unsigned zero is 128)
			// using 25.7 fixed point
			int32_t iTemp = pOutput[0] + (((pInput[0] - 128)*iPercentage)>>7);
			++pInput;

			// Underflow?
			iTemp = ClampZero(iTemp);
			// Overflow?
			iTemp = minimum(iTemp,static_cast<int32_t>(255));

			// Store in the reverb buffer
			pOutput[0] = static_cast<uint8_t>(iTemp);
			++pOutput;
		} while (--uCount);
	}
}


/*! ************************************

	\brief Add in an 16 bit reverb

	Given a scale in the format of a percentage of 0 to 100,
	apply the scaled input buffer to the output buffer via
	addition to generate reverb

	\param pOutput Pointer to the buffer which will have reverb applied
	\param pInput Pointer to the buffer which the reverb comes from
	\param uCount Number of bytes to process
	\param uPercentage Scale of reverb 0 is none, 100 is full volume

***************************************/

void BURGER_API Burger::ComputeReverb(int16_t *pOutput,const int16_t *pInput,uintptr_t uCount,uint_t uPercentage)
{
	if (uCount) {
		// Convert the percentage from 100 to 0.0 to 1.0 in 25.7 fixed point
		int iPercentage = static_cast<int>((uPercentage*128U)/100U);
		do {
			// Do a signed scale
			// using 25.7 fixed point
			int32_t iTemp = pOutput[0] + ((pInput[0]*iPercentage)>>7);
			++pInput;

			// Underflow?
			iTemp = maximum(iTemp,static_cast<int32_t>(-32767));
			// Overflow?
			iTemp = minimum(iTemp,static_cast<int32_t>(32767));

			// Store in the reverb buffer
			pOutput[0] = static_cast<int16_t>(iTemp);
			++pOutput;
		} while (--uCount);
	}
}
