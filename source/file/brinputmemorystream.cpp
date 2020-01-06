/***************************************

	Simple data stream class for input

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brinputmemorystream.h"
#include "brglobalmemorymanager.h"
#include "brendian.h"
#include "brstringfunctions.h"
#include "brfilemanager.h"
#include "brpalette.h"

/*! ************************************

	\class Burger::InputMemoryStream
	\brief Fast input memory stream.

	When parsing a data file, it's much faster
	to load the entire file into memory and once the data is
	loaded, sequentially read in the data.
	
	At the onset, the size of the input is known, either through
	loading in a data file or being given a pointer to a buffer
	of data and its length, from this, bounds checking is performed
	as subsequent calls to this class will return requested data
	until the data has been exhausted.

	Multibyte data such as \ref Word32 will be assumed to be in Little
	Endian format.

	\sa OutputMemoryStream or InputRezStream

***************************************/

/*! ************************************

	\brief Constructor.
	
	Initializes a stream for input. No memory is allocated at this time
	
***************************************/

Burger::InputMemoryStream::InputMemoryStream() :
	m_pWork(NULL),
	m_pEndOfBuffer(NULL),
	m_pData(NULL),
	m_uBufferSize(0),
	m_bDontFree(FALSE)
{
}

/*! ************************************

	\brief Constructor with a data file
	
	Upon construction, open and load in the data file
	for immediate parsing. Errors can occur and if
	loading fails or memory isn't allocated, the
	class will be given an empty buffer and no
	data will be streamed.

	\param pFilename Pointer to a Burgerlib filename to a file to load into memory
	
***************************************/

Burger::InputMemoryStream::InputMemoryStream(const char *pFilename) :
	m_pData(NULL)
{
	Open(pFilename);
}

/*! ************************************

	\brief Constructor with a data file
	
	Upon construction, open and load in the data file
	for immediate parsing. Errors can occur and if
	loading fails or memory isn't allocated, the
	class will be given an empty buffer and no
	data will be streamed.

	\param pFilename Pointer to a Burgerlib \ref Filename to a file to load into memory
	
***************************************/

Burger::InputMemoryStream::InputMemoryStream(Filename *pFilename) :
	m_pData(NULL)
{
	Open(pFilename);
}

/*! ************************************

	\brief Constructor with a input data buffer.
	
	Upon construction, point to the passed data buffer.
	If the buffer pointer is \ref NULL or there is a zero
	length buffer, the class will be given an empty buffer and no
	data will be streamed.

	\param pBuffer Pointer to an array of bytes to stream from
	\param uBufferSize Length of the array in bytes
	\param bDontFree \ref FALSE (Default) will have the class call Burger::Free(const void *)
		on the input pointer on class shutdown. \ref TRUE will disable this behavior.
	
***************************************/

Burger::InputMemoryStream::InputMemoryStream(const void *pBuffer,WordPtr uBufferSize,Word bDontFree) :
	m_pData(NULL)
{
	Open(pBuffer,uBufferSize,bDontFree);
}

/*! ************************************

	\brief Destructor.
	
	Releases all allocated memory.
	
***************************************/

Burger::InputMemoryStream::~InputMemoryStream()
{
	Clear();
}

/*! ************************************

	\brief Release all allocated memory

	If there was any memory allocated to load
	the input stream, release it and reset the
	class to a "power up" state. After this function
	call, this class will have empty buffers and
	all calls to receive data will return nothing

***************************************/

void BURGER_API Burger::InputMemoryStream::Clear(void)
{
	// Can the buffer be freed?
	if (m_pData && !m_bDontFree) {
		Free(m_pData);
	}
	m_pWork = NULL;
	m_pEndOfBuffer = NULL;
	m_pData = NULL;
	m_uBufferSize = 0;
	m_bDontFree = FALSE;
}

/*! ************************************

	\brief Move the read pointer forward in the stream

	Move the read pointer forward the number of bytes requested.
	If the offset goes beyond the end of the buffer, it will be
	clamped to the end of the data stream.

	\param uOffset Offset in bytes to skip forward
	\sa GetMark(void) const, SkipBack(WordPtr) or SetMark(WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::SkipForward(WordPtr uOffset)
{
	// Get the current work pointer
	const Word8 *pWork = m_pWork;
	// Calculate the remaining number of bytes
	WordPtr uRemaining = static_cast<WordPtr>(m_pEndOfBuffer-pWork);
	// Overrun?
	if (uOffset>uRemaining) {
		// Clamp
		uOffset = uRemaining;
	}
	// Set the new mark
	m_pWork = pWork+uOffset;
}


/*! ************************************

	\brief Move the read pointer backward in the stream

	Move the read pointer backward the number of bytes requested.
	If the offset goes beyond the start of the buffer, it will be
	clamped to the start of the data stream.

	\param uOffset Offset in bytes to skip backward
	\sa GetMark(void) const, SkipForward(WordPtr) or SetMark(WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::SkipBack(WordPtr uOffset)
{
	// Get the current work pointer
	const Word8 *pWork = m_pWork;
	// Calculate the number of bytes it can reverse
	WordPtr uRemaining = static_cast<WordPtr>(pWork-m_pData);
	// Overrun?
	if (uOffset>uRemaining) {
		// Clamp
		uOffset = uRemaining;
	}
	// Set the new mark
	m_pWork = pWork-uOffset;
}

/*! ************************************

	\brief Move the read pointer to a location in the stream

	Move the read pointer to the passed offset and
	all further read calls will proceed from this location.

	\note If the value is larger than the end of the data, the mark
		will be clamped to the end if the data

	\param uOffset Offset in bytes to read from
	\sa GetMark(void) const, SkipBack(WordPtr) or SkipForward(WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::SetMark(WordPtr uOffset)
{
	// Perform a sanity check
	if (uOffset>m_uBufferSize) {
		uOffset = m_uBufferSize;
	}
	// Can the buffer be freed?
	m_pWork = m_pData+uOffset;
}

/*! ************************************

	\fn const Word8 * Burger::InputMemoryStream::GetPtr(void) const
	\brief Return the current pointer into the data stream

	Return the current read pointer to the input byte stream.

	\return Current read pointer to the input byte stream.
	\sa GetMark(void) const

***************************************/

/*! ************************************

	\fn WordPtr Burger::InputMemoryStream::GetMark(void) const
	\brief Return the current offset into the data stream

	Calculate and return the number of bytes from the start of
	the stream the current read mark is located

	\return Offset in byte from the beginning of the stream.
	\sa GetSize(void) const

***************************************/

/*! ************************************

	\fn WordPtr Burger::InputMemoryStream::GetSize(void) const
	\brief Return the amount of data stored in the stream

	Calculate and return the number of bytes of valid data
	that is stored in the data stream.

	\return Number of bytes stored in the stream.
	\sa GetMark(void) const

***************************************/

/*! ************************************

	\fn Word Burger::InputMemoryStream::IsEmpty(void) const
	\brief Return \ref TRUE if there is no data remaining to parse in the stream

	If there is any data that hasn't been parsed remaining in the data stream, return \ref TRUE,
	otherwise return \ref FALSE

	\return \ref TRUE if no data remains. \ref FALSE if there is more data to parse.

***************************************/

/*! ************************************

	\fn WordPtr Burger::InputMemoryStream::BytesRemaining(void) const
	\brief Return the amount of data remaining to parse in the stream

	If there is any data that hasn't been parsed remaining in the data stream, return
	the size in bytes. Zero if no data remains.

	\return The number of bytes remaining to parse.

***************************************/

/*! ************************************

	\brief Load data from a file

	Open, read and close a file that will contain
	the contents of the new input data stream.

	\param pFilename Burgerlib format filename
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Word BURGER_API Burger::InputMemoryStream::Open(const char *pFilename)
{
	Clear();

	void *pInput = FileManager::LoadFile(pFilename,&m_uBufferSize);
	if (pInput) {
		m_pWork = static_cast<Word8 *>(pInput);
		m_pEndOfBuffer = static_cast<Word8 *>(pInput) + m_uBufferSize;
		m_pData = static_cast<Word8 *>(pInput);
		return 0;
	}
	return 10;
}

/*! ************************************

	\brief Load data from a file

	Open, read and close a file that will contain
	the contents of the new input data stream.

	\param pFilename Pointer to a Filename instance
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Word BURGER_API Burger::InputMemoryStream::Open(Filename *pFilename)
{
	Clear();

	void *pInput = FileManager::LoadFile(pFilename,&m_uBufferSize);
	if (pInput) {
		m_pWork = static_cast<Word8 *>(pInput);
		m_pEndOfBuffer = static_cast<Word8 *>(pInput) + m_uBufferSize;
		m_pData = static_cast<Word8 *>(pInput);
		return 0;
	}
	return 10;
}

/*! ************************************

	\brief Use data from a application supplied buffer
	
	If the buffer pointer is \ref NULL or there is a zero
	length buffer, the class will be given an empty buffer and no
	data will be streamed. The data is assumed to be able to be freed
	by Burger::Free(const void *), if this is not the case, pass
	\ref TRUE in bDontFree to disable this feature.

	\param pBuffer Pointer to an array of bytes to stream from
	\param uBufferSize Length of the array in bytes
	\param bDontFree \ref FALSE (Default) will have the class call Burger::Free(const void *)
		on the input pointer on class shutdown. \ref TRUE will disable this behavior.
	
***************************************/

void BURGER_API Burger::InputMemoryStream::Open(const void *pBuffer,WordPtr uBufferSize,Word bDontFree)
{
	Clear();
	m_pWork = static_cast<const Word8 *>(pBuffer);
	m_pEndOfBuffer = static_cast<const Word8 *>(pBuffer)+uBufferSize;
	m_pData = static_cast<const Word8 *>(pBuffer);
	m_uBufferSize = uBufferSize;
	m_bDontFree = bDontFree;
}

/*! ************************************

	\brief Parse a UTF-8 "C" string from the data stream

	Parse the input until an end of line is found. End of lines
	are either a '\\0' or a '\\n'. For compatibility with cross 
	platform text files, '\\r' and '\\r\\n' are acceptable end of line
	tokens.

	The output buffer is guaranteed to have a terminating zero. If
	the line is too large, it will be truncated.

	\param pOutput Buffer to receive the "C" style UTF-8 data string. \ref NULL will disable
		the string copy, however the string will be "parsed" from the input
	\param uOutputSize Size of the buffer pOutput
	\sa GetCString(char *,WordPtr) or GetPString(char *,WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::GetString(char *pOutput,WordPtr uOutputSize)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		// Is there output?
		if (uOutputSize) {
			// Reduce the buffer by 1
			--uOutputSize;
			pOutput[uOutputSize] = 0;	// Force the last char to be zero
		}
		// Parse out the string
		do {
			// Get a char from input
			Word uTemp = pWork[0];
			++pWork;
			// Is this a Carriage Return?
			if (uTemp==13) {
				// Is it followed by a Line Feed?
				if ((pWork<pEndOfBuffer) && pWork[0]==10) {
					// Discard the Line Feed!
					++pWork;
				}
				break;
			}
			// Terminate on a Line Feed or a NULL
			if ((uTemp==10) || !uTemp) {
				break;
			}

			// Any room in the dest buffer?
			if (uOutputSize) {
				// Remove space
				--uOutputSize;
				// Output the character
				pOutput[0] = static_cast<char>(uTemp);
				++pOutput;
			}
			// Scan for more data
		} while (pWork<pEndOfBuffer);

		// Did I underrun?
		if (uOutputSize) {
			pOutput[0] = 0;		// Force a "C" string.
		}
		m_pWork = pWork;		// Consume the input
	}
}

/*! ************************************

	\brief Parse a UTF-8 "C" string from the data stream

	Parse the input until an end of line is found. End of lines
	are either a '\\0' or end of data.

	The output String will be sized to contain the new string.

	\param pOutput Buffer to receive the "C" style UTF-8 data string.
	\sa GetCString(char *,WordPtr),GetPString(char *,WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::GetString(String *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;

	// Set the size of the new string
	WordPtr uOutputSize = 0;
	if (pWork<pEndOfBuffer) {
		// Parse out the string
		do {
			// Get a char from input
			Word uTemp = pWork[0];
			++pWork;
			// Terminate at a NULL
			if (!uTemp) {
				break;
			}
			++uOutputSize;
			// Scan for more data
		} while (pWork<pEndOfBuffer);
	}
	// Set the output string size
	pOutput->SetBufferSize(uOutputSize);
	if (uOutputSize) {
		// Copy the string into the new buffer
		// SetBufferSize() sets the null terminator
		MemoryCopy(pOutput->GetPtr(),m_pWork,uOutputSize);
	}
	m_pWork = pWork;		// Consume the input
}

/*! ************************************

	\brief Parse a UTF-8 "C" string from the data stream

	Parse the input until an end of line is found. End of lines
	are either a '\\0' or end of data.

	The output buffer is guaranteed to have a terminating zero. If
	the line is too large, it will be truncated.

	\param pOutput Buffer to receive the "C" style UTF-8 data string. \ref NULL will disable
		the string copy, however the string will be "parsed" from the input
	\param uOutputSize Size of the buffer pOutput
	\sa GetPString(char *,WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::GetCString(char *pOutput,WordPtr uOutputSize)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		// Is there output?
		if (uOutputSize) {
			// Reduce the buffer by 1
			--uOutputSize;
			pOutput[uOutputSize] = 0;	// Force the last char to be zero
		}
		// Parse out the string
		do {
			// Get a char from input
			Word uTemp = pWork[0];
			++pWork;
			// Terminate on a NULL
			if (!uTemp) {
				break;
			}

			// Any room in the destination buffer?
			if (uOutputSize) {
				// Remove space
				--uOutputSize;
				// Output the character
				pOutput[0] = static_cast<char>(uTemp);
				++pOutput;
			}
			// Scan for more data
		} while (pWork<pEndOfBuffer);

		// Did I underrun?
		if (uOutputSize) {
			pOutput[0] = 0;		// Force a "C" string.
		}
		m_pWork = pWork;		// Consume the input
	}
}

/*! ************************************

	\brief Parse a UTF-8 "P" string from the data stream

	Read a single byte from the stream to get the length of the
	string and then copy those bytes into the buffer.

	The output buffer is guaranteed to have a terminating zero. If
	the line is too large, it will be truncated.

	\param pOutput Buffer to receive the "C" style UTF-8 data string. \ref NULL will disable
		the string copy, however the string will be "parsed" from the input
	\param uOutputSize Size of the buffer pOutput
	\sa GetCString(char *,WordPtr)

***************************************/

void BURGER_API Burger::InputMemoryStream::GetPString(char *pOutput,WordPtr uOutputSize)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		// Is there output?
		if (uOutputSize) {
			// Reduce the buffer by 1
			--uOutputSize;
			pOutput[uOutputSize] = 0;	// Force the last char to be zero
		}

		// Get the length of the pascal string
		WordPtr uLength = pWork[0];
		++pWork;
		// Any data?
		if (uLength && (pWork<pEndOfBuffer)) {
			// Parse out the string
			do {
				// Get a char from input
				Word uTemp = pWork[0];
				++pWork;
				// Any room in the destination buffer?
				if (uOutputSize) {
					// Remove space
					--uOutputSize;
					// Output the character
					pOutput[0] = static_cast<char>(uTemp);
					++pOutput;
				}
				// Terminate on end of input
				if (!--uLength) {
					break;
				}
				// Scan for more data
			} while (pWork<pEndOfBuffer);
		}

		// Did I underrun?
		if (uOutputSize) {
			pOutput[0] = 0;		// Force a "C" string.
		}
		m_pWork = pWork;		// Consume the input
	}
}

/*! ************************************

	\brief Return an 8 bit value from the data stream

	\return Next 8 bit value from the stream. Zero if there is no data remaining.

***************************************/

uint8_t BURGER_API Burger::InputMemoryStream::GetByte(void) BURGER_NOEXCEPT
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word8 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		uTemp = pWork[0];
		++pWork;
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 16 bit little endian value from the data stream

	\return Next 16 bit value from the stream. Zero if there is no data remaining.
	\sa GetBigShort(void) or GetWord32(void)

***************************************/

uint16_t BURGER_API Burger::InputMemoryStream::GetShort(void) BURGER_NOEXCEPT
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word16 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+1)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = LittleEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
			pWork+=2;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 16 bit big endian value from the data stream

	\return Next 16 bit value from the stream. Zero if there is no data remaining.
	\sa GetShort(void) or GetBigWord32(void)

***************************************/

uint16_t BURGER_API Burger::InputMemoryStream::GetBigShort(void) BURGER_NOEXCEPT
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word16 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+1)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = BigEndian::LoadAny(reinterpret_cast<const Word16 *>(pWork));
			pWork+=2;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 32 bit little endian value from the data stream

	\return Next 32 bit value from the stream. Zero if there is no data remaining.
	\sa GetBigWord32(void) or GetShort(void)

***************************************/

uint32_t BURGER_API Burger::InputMemoryStream::GetWord32(void) BURGER_NOEXCEPT
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word32 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+3)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = LittleEndian::LoadAny(reinterpret_cast<const Word32 *>(pWork));
			pWork+=4;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 32 bit big endian value from the data stream

	\return Next 32 bit value from the stream. Zero if there is no data remaining.
	\sa GetWord32(void) or GetBigShort(void)

***************************************/

uint32_t BURGER_API Burger::InputMemoryStream::GetBigWord32(void) BURGER_NOEXCEPT
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word32 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+3)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = BigEndian::LoadAny(reinterpret_cast<const Word32 *>(pWork));
			pWork+=4;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 64 bit little endian value from the data stream

	\return Next 64 bit value from the stream. Zero if there is no data remaining.
	\sa GetBigWord64(void) or GetWord32(void)

***************************************/

Word64 BURGER_API Burger::InputMemoryStream::GetWord64(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word64 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+7)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = LittleEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
			pWork+=8;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 64 bit big endian value from the data stream

	\return Next 64 bit value from the stream. Zero if there is no data remaining.
	\sa GetWord64(void) or GetBigWord32(void)

***************************************/

Word64 BURGER_API Burger::InputMemoryStream::GetBigWord64(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word64 uTemp = 0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+7)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			uTemp = BigEndian::LoadAny(reinterpret_cast<const Word64 *>(pWork));
			pWork+=8;
		}
		m_pWork = pWork;
	}
	return uTemp;
}

/*! ************************************

	\brief Return a 32 bit little endian float from the data stream

	\return Next 32 bit value from the stream. Zero if there is no data remaining.
	\sa GetBigFloat(void) or GetDouble(void)

***************************************/

float BURGER_API Burger::InputMemoryStream::GetFloat(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	float fTemp = 0.0f;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+3)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			fTemp = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork));
			pWork+=4;
		}
		m_pWork = pWork;
	}
	return fTemp;
}

/*! ************************************

	\brief Return a 32 bit big endian float from the data stream

	\return Next 32 bit value from the stream. Zero if there is no data remaining.
	\sa GetFloat(void) or GetBigDouble(void)

***************************************/

float BURGER_API Burger::InputMemoryStream::GetBigFloat(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	float fTemp = 0.0f;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+3)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			fTemp = BigEndian::LoadAny(reinterpret_cast<const float *>(pWork));
			pWork+=4;
		}
		m_pWork = pWork;
	}
	return fTemp;
}

/*! ************************************

	\brief Return a 64 bit little endian float from the data stream

	\return Next 64 bit value from the stream. Zero if there is no data remaining.
	\sa GetBigDouble(void) or GetFloat(void)

***************************************/

double BURGER_API Burger::InputMemoryStream::GetDouble(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	double dTemp = 0.0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+7)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			dTemp = LittleEndian::LoadAny(reinterpret_cast<const double *>(pWork));
			pWork+=8;
		}
		m_pWork = pWork;
	}
	return dTemp;
}

/*! ************************************

	\brief Return a 64 bit big endian float from the data stream

	\return Next 64 bit value from the stream. Zero if there is no data remaining.
	\sa GetDouble(void) or GetBigFloat(void)

***************************************/

double BURGER_API Burger::InputMemoryStream::GetBigDouble(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	double dTemp = 0.0;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+7)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
		} else {
			dTemp = BigEndian::LoadAny(reinterpret_cast<const double *>(pWork));
			pWork+=8;
		}
		m_pWork = pWork;
	}
	return dTemp;
}

/*! ************************************

	\brief Return an unmodified array of bytes from the input stream

	\param pOutput Pointer to buffer to receive data. \ref NULL will perform no data transfer
	\param uOutputSize Number of bytes to transfer, ignored if pOutput is \ref NULL
	\return The number of bytes transferred. Is equal to uOutputSize of BytesRemaining(), whichever is less.

***************************************/

WordPtr BURGER_API Burger::InputMemoryStream::Get(void *pOutput,WordPtr uOutputSize)
{
	WordPtr uResult = 0;
	if (pOutput && uOutputSize) {
		const Word8 *pWork = m_pWork;
		WordPtr uRemaining = static_cast<WordPtr>(m_pEndOfBuffer-pWork);
		if (uRemaining<uOutputSize) {
			uOutputSize = uRemaining;
		}
		MemoryCopy(pOutput,pWork,uOutputSize);
		pWork+=uOutputSize;
		m_pWork = pWork;
		uResult = uOutputSize;
	}
	return uResult;
}

/*! ************************************

	\brief Extract an R,G,B color from the input stream

	Use the next three bytes as Red, Green and Blue (In
	that order) and store it in the output RGBWord8_t
	structure.

	\param pOutput Pointer to a buffer to store the output
	\return Zero if no error, non zero if there isn't enough data in the stream for the output.
	\sa Get(RGBAWord8_t *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::Get(RGBWord8_t *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word uResult = TRUE;		// Error!
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+2)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
			pOutput->m_uRed = 0;
			pOutput->m_uGreen = 0;
			pOutput->m_uBlue = 0;
		} else {
			pOutput->m_uRed = pWork[0];
			pOutput->m_uGreen = pWork[1];
			pOutput->m_uBlue = pWork[2];
			pWork+=3;
			uResult = FALSE;
		}
		m_pWork = pWork;
	}
	return uResult;
}

/*! ************************************

	\brief Extract an R,G,B,A color from the input stream

	Use the next four bytes as Red, Green, Blue and Alpha (In
	that order) and store it in the output RGBAWord8_t
	structure.

	\param pOutput Pointer to a buffer to store the output
	\return Zero if no error, non zero if there isn't enough data in the stream for the output.
	\sa Get(RGBWord8_t *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::Get(RGBAWord8_t *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word uResult = TRUE;		// Error!
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+3)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
			pOutput->m_uRed = 0;
			pOutput->m_uGreen = 0;
			pOutput->m_uBlue = 0;
			pOutput->m_uAlpha = 255;
		} else {
			pOutput->m_uRed = pWork[0];
			pOutput->m_uGreen = pWork[1];
			pOutput->m_uBlue = pWork[2];
			pOutput->m_uAlpha = pWork[3];
			pWork+=4;
			uResult = FALSE;
		}
		m_pWork = pWork;
	}
	return uResult;
}

/*! ************************************

	\brief Extract a Vector2D_t from the input stream

	Use the next eight bytes as a little endian pair
	of 32 bit floating point values and store it in
	the output Vector2D_t structure.

	\param pOutput Pointer to a buffer to store the output
	\return Zero if no error, non zero if there isn't enough data in the stream for the output.
	\sa Get(Vector3D_t *) or Get(Vector4D_t *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::Get(Vector2D_t *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word uResult = TRUE;		// Error!
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+7)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
			pOutput->Zero();
		} else {
			pOutput->x = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork));
			pOutput->y = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+4));
			pWork+=8;
			uResult = FALSE;
		}
		m_pWork = pWork;
	}
	return uResult;
}

/*! ************************************

	\brief Extract a Vector3D_t from the input stream

	Use the next twelve bytes as a little endian triplet
	of 32 bit floating point values and store it in
	the output Vector3D_t structure.

	\param pOutput Pointer to a buffer to store the output
	\return Zero if no error, non zero if there isn't enough data in the stream for the output.
	\sa Get(Vector2D_t *) or Get(Vector4D_t *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::Get(Vector3D_t *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word uResult = TRUE;		// Error!
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+11)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
			pOutput->Zero();
		} else {
			pOutput->x = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork));
			pOutput->y = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+4));
			pOutput->z = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+8));
			pWork+=12;
			uResult = FALSE;
		}
		m_pWork = pWork;
	}
	return uResult;
}


/*! ************************************

	\brief Extract a Vector4D_t from the input stream

	Use the next sixteen bytes as a little endian quad
	of 32 bit floating point values and store it in
	the output Vector4D_t structure.

	\param pOutput Pointer to a buffer to store the output
	\return Zero if no error, non zero if there isn't enough data in the stream for the output.
	\sa Get(Vector2D_t *) or Get(Vector3D_t *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::Get(Vector4D_t *pOutput)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	Word uResult = TRUE;		// Error!
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		if ((pWork+15)>=pEndOfBuffer) {
			pWork = pEndOfBuffer;
			pOutput->Zero();
		} else {
			pOutput->x = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork));
			pOutput->y = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+4));
			pOutput->z = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+8));
			pOutput->w = LittleEndian::LoadAny(reinterpret_cast<const float *>(pWork+12));
			pWork+=16;
			uResult = FALSE;
		}
		m_pWork = pWork;
	}
	return uResult;
}

/*! ************************************

	\brief Skip past white space in the input stream

	If the bytes in the input stream are spaces or tabs, skip
	over them.

	\note This function only skips over data in the stream,
	if may set the stream to the end if there are only
	spaces and tabs remaining.

***************************************/

void BURGER_API Burger::InputMemoryStream::ParseBeyondWhiteSpace(void)
{
	const Word8 *pEndOfBuffer = m_pEndOfBuffer;
	const Word8 *pWork = m_pWork;
	// Is there any input?
	if (pWork<pEndOfBuffer) {
		do {
			Word uTemp = pWork[0];
			if ((uTemp!=' ') &&
				(uTemp!='\t')) {
				break;
			}
		} while (++pWork<pEndOfBuffer);
		m_pWork = pWork;
	}
}

/*! ************************************

	\brief Test if the next bytes in the stream match

	Given a "C" string, test if the string is the next value in
	the stream and if true, remove the string and return \ref TRUE

	\param pInput Pointer to a "C" string to match
	\return \ref TRUE if it was a match, \ref FALSE if not
	\sa IsStringMatchCase(const char *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::IsStringMatch(const char *pInput)
{
	// Assume a match on a NULL string
	Word uResult = TRUE;
	if (pInput) {
		Word uInput = reinterpret_cast<const Word8 *>(pInput)[0];
		if (uInput) {
			// No match
			uResult = FALSE;
			const Word8 *pEndOfBuffer = m_pEndOfBuffer;
			const Word8 *pWork = m_pWork;
			// Is there any input?
			if (pWork<pEndOfBuffer) {
				// Get a match character
				do {
					Word uTemp1 = pWork[0];
					// No match?
					if (uTemp1!=uInput) {
						break;
					}
					// Accept the input
					++pInput;
					++pWork;
					// End of the test string?
					uInput = reinterpret_cast<const Word8 *>(pInput)[0];
					if (!uInput) {
						// Full match, remove the input and exit
						uResult = TRUE;
						m_pWork = pWork;
						break;
					}
				} while (pWork<pEndOfBuffer);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Case insensitive test if the next bytes in the stream match

	Given a "C" string, test if the string with case insensitivity is the next value in
	the stream and if true, remove the string and return \ref TRUE

	\param pInput Pointer to a "C" string to match
	\return \ref TRUE if it was a match, \ref FALSE if not
	\sa IsStringMatch(const char *)

***************************************/

Word BURGER_API Burger::InputMemoryStream::IsStringMatchCase(const char *pInput)
{
	// Assume a match on a NULL string
	Word uResult = TRUE;
	if (pInput) {
		Word uInput = reinterpret_cast<const Word8 *>(pInput)[0];
		if (uInput) {
			// No match
			uResult = FALSE;
			const Word8 *pEndOfBuffer = m_pEndOfBuffer;
			const Word8 *pWork = m_pWork;
			// Is there any input?
			if (pWork<pEndOfBuffer) {
				// Get a match character
				do {
					Word uTemp1 = pWork[0];
					// No match?
					if (uTemp1>='A' && uTemp1<='Z') {	// Convert to lower case
						uTemp1 += 32;
					}
					if (uInput>='A' && uInput<='Z') {	// Convert to lower case
						uInput += 32;
					}
					if (uTemp1!=uInput) {
						break;
					}
					// Accept the input
					++pInput;
					++pWork;
					// End of the test string?
					uInput = reinterpret_cast<const Word8 *>(pInput)[0];
					if (!uInput) {
						// Full match, remove the input and exit
						uResult = TRUE;
						m_pWork = pWork;
						break;
					}
				} while (pWork<pEndOfBuffer);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Test if the next bytes in the stream match

	Given an array of bytes, test if the data is the next value in
	the stream and if true, remove the data from the stream and return \ref TRUE

	\note A length of 0 will return \ref TRUE in all cases and remove no data from the stream.

	\param pInput Pointer to an array of bytes to test against
	\param uLength Length of the data in bytes.
	\return \ref TRUE if it was a match, \ref FALSE if not

***************************************/

Word BURGER_API Burger::InputMemoryStream::IsDataMatch(const Word8 *pInput,WordPtr uLength)
{
	// Assume a match on a NULL string
	Word uResult = TRUE;
	if (uLength) {
		// No match
		uResult = FALSE;
		const Word8 *pEndOfBuffer = m_pEndOfBuffer;
		const Word8 *pWork = m_pWork;
		// Is there any input?
		if (pWork<pEndOfBuffer) {
			// Get a match character
			do {
				// No match?
				if (pWork[0]!=pInput[0]) {
					break;
				}
				// Accept the input
				++pInput;
				++pWork;
				// End of the test data?
				if (!--uLength) {
					// Full match, remove the input and exit
					uResult = TRUE;
					m_pWork = pWork;
					break;
				}
			} while (pWork<pEndOfBuffer);
		}
	}
	return uResult;
}