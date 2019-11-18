/***************************************

	Sound manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word8 szRIFF[4];			// ASCII RIFF
	Word32 dwFormatLength;		// Length of file contents
	Word8 szWAVE[4];			// ASCII WAVE
	Word8 szFMT[4];				// ASCII fmt_
	Word32 dwWaveFormatLength;	// Size of fmt_ struct (16)
	Word16 wFormatTag;			// Compression type
	Word16 wChannels;			// Number of sound channels (1,2)
	Word32 dwSamplesPerSec;		// Sample rate
	Word32 dwAvgBytesPerSec;	// Bytes per second	
	Word16 wBlockAlign;			// Data alignment (2)
	Word16 wBitsPerSample;		// Bits per sample (8,16)
	Word16 wExtSize;			// Extra data for compressed formats
	Word16 wSamplesPerBlock;	// Number of samples in each block
	Word16 wNumCoefs;			// Number of coefs in tables
};

//
// WAV file chunk info
//

struct WavData_t {
	Word8 m_DataASCII[4];	// ASCII data 
	Word32 m_uDataLength;	// Size of the data
	Word8 m_Data[1];		// Array of byte dwDataLength in size
};

//
// Data for AIFF files (Big endian)
//

struct AIFFHeader_t {		// 12 byte header for ALL IFF files
	char m_FormASCII[4];	// FORM
	Word32 m_uFileSize;		// Length of the file contents
	char m_AIFFASCII[4];	// AIFF
};

struct AIFFCommon_t {			// COMM chunk
	char m_CommASCII[4];		// COMM
	Word32 m_uChunkLength;		// Length of the struct
	Word16 m_uNumChannels;		// Number of channels (1 or 2)
	Word16 m_uNumSampleFramesHI;	// Number of SAMPLES, not bytes (16 bit aligned)
	Word16 m_uNumSampleFramesLO;	
	Word16 m_uSampleSize;		// 8 or 16 bit
	Burger::Float80Bit m_fxSampleRate;	// Extended floating point
	// AIFC specific data (m_uChunkLength>18)
	char m_CompressionType[4];	// 4 character code for compression (16 bit aligned)
	Word8 m_CompressionString[1];	// Pascal string with the name of the compression algorithm
};

struct AIFFSsnd_t {			// SSND chunk
	char m_SsndASCII[4];	// SSND
	Word32 m_uChunkLength;	// Length of the data
	Word32 m_uOffset;		// Offset to the start
	Word32 m_uBlockSize;	// Size of each unit
	Word8 m_Data[1];		// Data
};

//
// Data for VOC files (Little endian)
//

#define VOCHHEADERSIZE 26
struct VOCHeader_t {		// Voc file header
	char m_NameASCII[20];	// "Creative Voice File\x1A"
	Word16 m_uOffset;		// Offset to the header
	Word16 m_uVersion;		// Version number
	Word16 m_uChecksum;		// ~(Version - 0x1234)
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
	Word8 m_uChunkType;	// Chunk type
	Word8 m_uLengthLo;	// 3 bytes for the length
	Word16 m_uLengthHi;	// upper 2 bytes for length
	BURGER_INLINE WordPtr GetLength(void) const { return (static_cast<Word32>(Burger::LittleEndian::Load(&m_uLengthHi))<<8U)+m_uLengthLo; }
};

struct VOCSoundData1_t : public VOCChunk_t {
	enum {
		TYPEBYTE=0,
		TYPE4TO8=1,
		TYPE3TO8=2,
		TYPE2TO8=3,
		TYPEMULTIDAC=4
	};
	Word8 m_uSampleRate;		// Use the function below to convert to sample rate
	Word8 m_uCompressionType;	// Type of data
	Word8 m_Data[1];			// Data
	BURGER_INLINE Word32 GetSampleRate(void) const { return 1000000U / (256U - m_uSampleRate); }
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
	Word32 m_uSampleRate;	// Samples per second
	Word8 m_uBitsPerSample;	// 8 or 16 bits
	Word8 m_uChannels;		// 1 or 2 channels
	Word16 m_uCompression;	// A four
	Word8 m_Reserved[4];	// Padding
	Word8 m_Data[1];		// Data
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

Word BURGER_API Burger::SoundManager::BufferDecoder::ParseSoundFileImage(const void *pInput,WordPtr uLength)
{
	// Clear out the struct
	Clear();

	// Special data for ADPCM
	Word uSamplesPerBlock = 0;
	Word uBlockAlign = 0;

	// Assume error
	Word bResult = TRUE;			

	//
	// Is this a windows .WAV file?
	//

	if ((uLength >= cWAVHeaderSize) &&
		(BigEndian::Load(static_cast<const Word32 *>(pInput)) == cRIFFASCII) &&
		(BigEndian::Load(static_cast<const Word32 *>(pInput)+2) == cWAVEASCII)) {

		// setup a pointer to the wave header
		// set size of the sample and pointer to the sample
		const WavHeader_t *pHeader = static_cast<const WavHeader_t *>(pInput);

		// Get the wav data chunk

		const WavData_t *pWavData = static_cast<const WavData_t *>(FindRIFFChunk(pInput,uLength,cDATAASCII));
		if (pWavData) {
			// Get the pointer to the data and its length
			m_pSoundImage = pWavData->m_Data;
			m_uCompressedLength = LittleEndian::Load(&pWavData->m_uDataLength);

			// Samples per second
			m_uSampleRate = LittleEndian::Load(&pHeader->dwSamplesPerSec);
		
			Word uChannels = LittleEndian::Load(&pHeader->wChannels);
			Word uWAVFormat = LittleEndian::Load(&pHeader->wFormatTag);

			// Each data format needs different handling

			switch (uWAVFormat) {

			// PCM data (Uncompressed audio)
			default:
			case 1:
				// 8 bit data?
				if (LittleEndian::Load(&pHeader->wBitsPerSample) == 8) {	
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
				if (LittleEndian::Load(&pHeader->wBitsPerSample) == 4) {

					m_eDataType = TYPEADPCM;
					uSamplesPerBlock = LittleEndian::Load(&pHeader->wSamplesPerBlock);
					uBlockAlign = LittleEndian::Load(&pHeader->wBlockAlign);

					// Number of packets in the data
					WordPtr uPackets = m_uCompressedLength / uBlockAlign;

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
				if (LittleEndian::Load(&pHeader->wBitsPerSample) == 32) {	
					m_eDataType = TYPELFLOAT;
					m_uSoundLength = m_uCompressedLength;
					bResult = FALSE;		// It's ok
				}
				break;

			// ALaw data
			case 6:
				// 8 bit data?
				if (LittleEndian::Load(&pHeader->wBitsPerSample) == 8) {
					m_eDataType = TYPEALAW;
					// Will decompress into shorts from bytes
					m_uSoundLength = m_uCompressedLength*2;
					bResult = FALSE;
				}
				break;

			// uLaw data
			case 7:
				// 8 bit data?
				if (LittleEndian::Load(&pHeader->wBitsPerSample) == 8) {
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
		(BigEndian::Load(static_cast<const Word32 *>(pInput)) == cFORMASCII) &&
		((BigEndian::Load(static_cast<const Word32 *>(pInput)+2) == cAIFFASCII) ||
		(BigEndian::Load(static_cast<const Word32 *>(pInput)+2) == cAIFCASCII))) {

		// Get the COMM record
		const AIFFCommon_t *pCommonChunk = static_cast<const AIFFCommon_t *>(FindAIFFChunk(pInput,uLength,cCOMMASCII));
		if (pCommonChunk) {
			// Get the sound data
			const AIFFSsnd_t *pSoundChunk = static_cast<const AIFFSsnd_t *>(FindAIFFChunk(pInput,uLength,cSSNDASCII));
			if (pSoundChunk) {
				// Set up data
				m_pSoundImage = pSoundChunk->m_Data;
				m_uCompressedLength = BigEndian::Load(&pSoundChunk->m_uChunkLength)-8;

				// Sample rate is stored as a big endian 80 bit float
				m_uSampleRate = static_cast<Word>(static_cast<int>(BigEndianLoadExtended(pCommonChunk->m_fxSampleRate)));

				// 8 bit data?
				Word uSampleSize = BigEndian::Load(&pCommonChunk->m_uSampleSize);
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
				if (BigEndian::Load(static_cast<const Word32 *>(pInput)+2) == cAIFCASCII) {
					// Mace 6?
					Word32 uCompressionType = BigEndian::LoadAny(reinterpret_cast<const Word32 *>(static_cast<const void *>(&pCommonChunk->m_CompressionType)));
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
				if (BigEndian::Load(&pCommonChunk->m_uNumChannels) == 2) {
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
		LittleEndian::Load(&static_cast<const VOCHeader_t*>(pInput)->m_uVersion) == static_cast<Word16>((~LittleEndian::Load(&static_cast<const VOCHeader_t*>(pInput)->m_uChecksum))+0x1234)) {

		//
		// Passed the validations. It's a VOC file
		//

		const VOCChunk_t *pVOCChunk = reinterpret_cast<const VOCChunk_t *>(static_cast<const Word8 *>(pInput)+LittleEndian::Load(&static_cast<const VOCHeader_t*>(pInput)->m_uOffset));

		WordPtr uTempLength = uLength-VOCHHEADERSIZE;
		Word bContinue;
		do {
			// Get the size of the current chunk

			WordPtr uChunkSize = pVOCChunk->GetLength();
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
					m_uSampleRate = LittleEndian::LoadAny(&pVOCData->m_uSampleRate);
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
			pVOCChunk = reinterpret_cast<const VOCChunk_t *>(reinterpret_cast<const Word8 *>(pVOCChunk)+uChunkSize);
		} while (bContinue);

		
//
// Is this an OGG/Vorbis file?
//

	} else if ((uLength>=0x2C) &&
		(BigEndian::Load(static_cast<const Word32 *>(pInput)) == cOggSASCII)) {
		m_eDataType = TYPEOGG;
		m_pSoundImage = static_cast<const Word8 *>(pInput);
		m_uSoundLength = UINTPTR_MAX;		// Can't tell unless I scan the whole thing
		m_uSampleRate = LittleEndian::Load(&static_cast<const Word32 *>(pInput)[0x28/4]);
		if (static_cast<const Word8 *>(pInput)[0x27]==2) {
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

		WordPtr uSamples = m_uSoundLength;	// Number of bytes in the data
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

Word BURGER_API Burger::SoundManager::BufferDecoder::Parse(const BufferDescription_t *pInput)
{
	// Assume error
	Word bResult = TRUE;

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

		WordPtr uSamples = m_uSoundLength;	// Number of bytes in the data
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
	up with a call to Init(const void *,WordPtr) or
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

Word BURGER_API Burger::SoundManager::Buffer::Init(const void *pSoundFile,WordPtr uLength)
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
	\sa Init(const void *,WordPtr)

***************************************/

Word BURGER_API Burger::SoundManager::Buffer::Init(const BufferDescription_t *pRawData)
{
	m_uPan = cPanCenter;
	m_uVolume = cMaxVolume;
	return m_Decoder.Parse(pRawData);
}

/*! ************************************

	\brief Release resources

	Releases all platform specific data and any
	ownership of audio data passed in.

	\sa Init(const void *,WordPtr) or Init(const void *,WordPtr)

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
Word Burger::SoundManager::Buffer::Upload(SoundManager * /* pSoundManager */)
{
	Word uResult = 0;
	// Not already uploaded?
	if (!m_pBufferData) {
		// Create a buffer for the sound
		
		WordPtr uBufferSize = m_Decoder.m_uSoundLength;
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

	\fn void Burger::SoundManager::Buffer::SetSampleRate(Word uSampleRate)
	\brief Sets the buffer's sample rate

	Sets the default sample rate to playback the audio data.

	\param uSampleRate New sample rate
	\sa GetSampleRate(void) const

***************************************/

/*! ************************************

	\fn Word Burger::SoundManager::Buffer::GetSampleRate(void) const
	\brief Gets the buffer's sample rate

	Gets the default sample rate for the audio data.

	\return The buffer's default sample rate
	\sa SetSampleRate(Word)

***************************************/

/*! ************************************

	\fn void Burger::SoundManager::Buffer::SetPan(Word uPan)
	\brief Sets the buffer's stereo pan setting

	Sets the default pan setting for audio data playback.

	\param uPan New default stereo pan setting
	\sa GetPan(void) const

***************************************/

/*! ************************************

	\fn Word Burger::SoundManager::Buffer::GetPan(void) const
	\brief Gets the buffer's stereo pan setting

	Gets the default pan setting for the audio data.

	\return The buffer's default pan setting
	\sa SetPan(Word)

***************************************/

/*! ************************************

	\fn void Burger::SoundManager::Buffer::SetVolume(Word uVolume)
	\brief Sets the buffer's default volume

	Sets the default volume setting for audio data playback.

	\param uVolume New default volume setting
	\sa GetVolume(void) const

***************************************/

/*! ************************************

	\fn Word Burger::SoundManager::Buffer::GetVolume(void) const
	\brief Gets the buffer's default volume setting

	Gets the default volume setting for the audio data.

	\return The buffer's default volume setting
	\sa SetVolume(Word)

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
Word BURGER_API Burger::SoundManager::Voice::Init(SoundManager *pManager,Buffer *pBuffer)
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
	Word uResult = 10;
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

Word BURGER_API Burger::SoundManager::Voice::Start(void)
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

Word BURGER_API Burger::SoundManager::Voice::Stop(void)
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

Word BURGER_API Burger::SoundManager::Voice::Pause(void)
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

Word BURGER_API Burger::SoundManager::Voice::Resume(void)
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

Word BURGER_API Burger::SoundManager::Voice::SetVolume(Word uVolume)
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

Word BURGER_API Burger::SoundManager::Voice::SetPan(Word uPan)
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

Word BURGER_API Burger::SoundManager::Voice::SetSampleRate(Word uSamplesPerSecond)
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
Word BURGER_API Burger::SoundManager::Init(void)
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

Word BURGER_API Burger::SoundManager::Play(Buffer *pBuffer,Voice **ppCookie)
{
	if (ppCookie) {
		ppCookie[0] = NULL;
	}
	
	Word uResult = pBuffer->Upload(this);
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

Word BURGER_API Burger::SoundManager::Stop(void)
{
	Word uResult = 0;
	// Start at the max
	Word i = cMaxVoiceCount;
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

Word BURGER_API Burger::SoundManager::Pause(void)
{
	Word uResult = 0;
	// Start at the max
	Word i = cMaxVoiceCount;
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

Word BURGER_API Burger::SoundManager::Resume(void)
{
	Word uResult = 0;
	// Start at the max
	Word i = cMaxVoiceCount;
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

void BURGER_API Burger::SoundManager::SetMaxVoices(Word uMax)
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

Word BURGER_API Burger::SoundManager::GetActiveVoiceCount(void) const
{
	Word uResult = 0;
	// Start at the max
	Word i = cMaxVoiceCount;
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
void BURGER_API Burger::SoundManager::SetVolume(Word uVolume)
{
	m_uVolume = uVolume;
}
#endif

/*! ************************************

	\brief Create a new sound buffer

***************************************/

Burger::SoundManager::Buffer * BURGER_API Burger::SoundManager::NewBuffer(void *pWaveFile,WordPtr uLength)
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
Word BURGER_API Burger::SoundManager::GetAudioModes(ClassArray<SoundCardDescription> *pOutput)
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
	Word i = m_uMaxVoices;
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

void BURGER_API Burger::CopySoundData(void *pOutput,const void *pInput,WordPtr uLength,SoundManager::eDataType eType)
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
			SwapCharsToBytes(pOutput,pInput,uLength);
			break;

		// Endian swapped shorts?
#if defined(BURGER_BIGENDIAN)
		case SoundManager::TYPELSHORT:
#else
		case SoundManager::TYPEBSHORT:
#endif
			ConvertEndian(static_cast<Word16 *>(pOutput),static_cast<const Word16 *>(pInput),uLength>>1U);
			break;

		// Endian swapped floats?
#if defined(BURGER_BIGENDIAN)
		case SoundManager::TYPELFLOAT:
#else
		case SoundManager::TYPEBFLOAT:
#endif
			ConvertEndian(static_cast<Word32 *>(pOutput),static_cast<const Word32 *>(pInput),uLength>>2U);
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

void BURGER_API Burger::ClearSoundData(void *pOutput,WordPtr uLength,SoundManager::eDataType eType)
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

	\sa CopyStereoInterleaved(void *,const Word16 *,const Word16 *,WordPtr)

***************************************/

void BURGER_API Burger::CopyStereoInterleaved(void *pOutput,const Word8 *pLeft,const Word8 *pRight,WordPtr uOutputSize)
{
	if (uOutputSize) {
		do {
			// Copy a byte from the left and then the right buffers
			static_cast<Word8 *>(pOutput)[0] = pLeft[0];
			static_cast<Word8 *>(pOutput)[1] = pRight[0];

			// Increment the pointers
			pOutput = static_cast<Word8 *>(pOutput)+2;
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
	\param uOutputSize Size in entries (Word16) of the left or right channel (Both must be the same size)

	\sa CopyStereoInterleaved(void *,const Word8 *,const Word8 *,WordPtr)

***************************************/

void BURGER_API Burger::CopyStereoInterleaved(void *pOutput,const Word16 *pLeft,const Word16 *pRight,WordPtr uOutputSize)
{
	if (uOutputSize) {
		do {
			// Copy a byte from the left and then the right buffers
			static_cast<Word16 *>(pOutput)[0] = pLeft[0];
			static_cast<Word16 *>(pOutput)[1] = pRight[0];

			// Increment the pointers
			pOutput = static_cast<Word16 *>(pOutput)+2;
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

void BURGER_API Burger::ComputeReverb(Word8 *pOutput,const Word8 *pInput,WordPtr uCount,Word uPercentage)
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
			iTemp = Min(iTemp,static_cast<int32_t>(255));

			// Store in the reverb buffer
			pOutput[0] = static_cast<Word8>(iTemp);
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

void BURGER_API Burger::ComputeReverb(Int16 *pOutput,const Int16 *pInput,WordPtr uCount,Word uPercentage)
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
			iTemp = Max(iTemp,static_cast<int32_t>(-32767));
			// Overflow?
			iTemp = Min(iTemp,static_cast<int32_t>(32767));

			// Store in the reverb buffer
			pOutput[0] = static_cast<Int16>(iTemp);
			++pOutput;
		} while (--uCount);
	}
}
