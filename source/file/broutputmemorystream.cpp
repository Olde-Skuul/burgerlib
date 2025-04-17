/***************************************

	Simple data stream class for output

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "broutputmemorystream.h"
#include "brendian.h"
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "brnumberstring.h"
#include "brpalette.h"

/*! ************************************

	\class Burger::OutputMemoryStream
	\brief Fast output memory stream.

	When generating a new data file, it's much faster to stream the data into
	memory and once the data is written, store it out to disk.

	If the size of the output file is not known, allocating a buffer large
	enough to hold the data is educated guesswork. This class solves this
	problem twofold, firstly, it allocates in reasonable sized chunks to prevent
	out of memory situations due to allocating very large memory blocks and not
	succeeding due to fragmentation. Secondly, it will allow writing to a byte
	stream so data alignment at the stream level is not necessary and large data
	is always stored in little endian format so files created this way will be
	cross platform.

	\sa InputMemoryStream or Chunk_t

***************************************/

/*! ************************************

	\struct Burger::OutputMemoryStream::Chunk_t
	\brief Data chunk for OutputMemoryStream

	To prevent allocation failures, the OutputMemoryStream class allocates data
	chunks in small sizes so if the memory is fragmented the odds of a
	successful allocation increases.

	Once the OutputMemoryStream is disposed of or a call to
	OutputMemoryStream::Clear() is issued, all chunks are disposed of.

	\sa OutputMemoryStream

***************************************/

/*! ************************************

	\brief Default constructor.

	Initializes a stream for output. No memory is allocated at this time

***************************************/

Burger::OutputMemoryStream::OutputMemoryStream() BURGER_NOEXCEPT
	: m_pRoot(nullptr),
	  m_pCurrent(nullptr),
	  m_uIndex(kChunkSize), // Will force Append(uint8_t) to add a new buffer
	  m_uFileSize(0),
	  m_uError(kErrorNone)
{
}

/*! ************************************

	\brief Destructor.

	Releases all memory. This does NOT save anything to disk

***************************************/

Burger::OutputMemoryStream::~OutputMemoryStream()
{
	Clear();
}

/*! ************************************

	\brief Copy constructor.

	Can default to error condition if out of memory.

***************************************/
Burger::OutputMemoryStream::OutputMemoryStream(
	const OutputMemoryStream& rInput) BURGER_NOEXCEPT
{
	// Now copy the memory buffers into this class
	const Chunk_t* pChunk = rInput.m_pRoot;
	if (pChunk) {
		Chunk_t* pPrev = nullptr;
		do {
			// Make a copy of the chunk.
			Chunk_t* pNew =
				static_cast<Chunk_t*>(alloc_copy(pChunk, sizeof(Chunk_t)));

			// If not the first chunk, add to the linked list
			// or set the root pointer
			if (!pPrev) {
				m_pRoot = pNew;
			} else {
				pPrev->m_pNext = pNew;
			}

			// Was this chunk the "current" chunk?
			if (pChunk == rInput.m_pCurrent) {
				// Use the equivalent copied buffer
				m_pCurrent = pNew;
			}
			// Follow both linked lists
			pChunk = pChunk->m_pNext;
			pPrev = pNew;
			// Reached the end of the list?
		} while (pChunk);
		// Clip off the last entry
		pPrev->m_pNext = nullptr;
	}
	m_uIndex = rInput.m_uIndex;
	m_uFileSize = rInput.m_uFileSize;
	m_uError = rInput.m_uError;
}

/*! ************************************

	\brief Assignment operator.

	Can default to error condition if out of memory.

***************************************/
Burger::OutputMemoryStream& Burger::OutputMemoryStream::operator=(
	const Burger::OutputMemoryStream& rInput) BURGER_NOEXCEPT
{
	if (this != &rInput) {
		// Clear out the previous contents, if any
		Clear();

		// Now copy the memory buffers into this class
		const Chunk_t* pChunk = rInput.m_pRoot;
		if (pChunk) {
			Chunk_t* pPrev = nullptr;
			do {
				// Make a copy of the chunk.
				Chunk_t* pNew =
					static_cast<Chunk_t*>(alloc_copy(pChunk, sizeof(Chunk_t)));

				// If not the first chunk, add to the linked list
				// or set the root pointer
				if (!pPrev) {
					m_pRoot = pNew;
				} else {
					pPrev->m_pNext = pNew;
				}

				// Was this chunk the "current" chunk?
				if (pChunk == rInput.m_pCurrent) {
					// Use the equivalent copied buffer
					m_pCurrent = pNew;
				}
				// Follow both linked lists
				pChunk = pChunk->m_pNext;
				pPrev = pNew;
				// Reached the end of the list?
			} while (pChunk);
			// Clip off the last entry
			pPrev->m_pNext = nullptr;
		}
		m_uIndex = rInput.m_uIndex;
		m_uFileSize = rInput.m_uFileSize;
		m_uError = rInput.m_uError;
	}
	return *this;
}

#if defined(BURGER_RVALUE_REFERENCES)

/*! ************************************

	\brief Move assignment operator.

	Move an OutputMemoryStream instance into this instance, only available with
	C++11 or higher

***************************************/

Burger::OutputMemoryStream::OutputMemoryStream(
	OutputMemoryStream&& rInput) BURGER_NOEXCEPT
{
	// Move the data
	m_pRoot = rInput.m_pRoot;
	m_pCurrent = rInput.m_pCurrent;
	m_uIndex = rInput.m_uIndex;
	m_uFileSize = rInput.m_uFileSize;
	m_uError = rInput.m_uError;

	// Zap the data donor
	rInput.m_pRoot = nullptr;
	rInput.m_pCurrent = nullptr;
	rInput.m_uIndex = kChunkSize;
	rInput.m_uFileSize = 0;
	// Note: Leave m_uError intact
}

/*! ************************************

	\brief Move assignment operator.

	Move an OutputMemoryStream instance into this instance, only available with
	C++11 or higher

***************************************/

Burger::OutputMemoryStream& Burger::OutputMemoryStream::operator=(
	OutputMemoryStream&& rInput) BURGER_NOEXCEPT
{
	if (this != &rInput) {
		// Dispose of the data already present
		Clear();

		// Move the data
		m_pRoot = rInput.m_pRoot;
		m_pCurrent = rInput.m_pCurrent;
		m_uIndex = rInput.m_uIndex;
		m_uFileSize = rInput.m_uFileSize;
		m_uError = rInput.m_uError;

		// Zap the data donor
		rInput.m_pRoot = nullptr;
		rInput.m_pCurrent = nullptr;
		rInput.m_uIndex = kChunkSize;
		rInput.m_uFileSize = 0;
		// Note: Leave m_uError intact
	}
	return *this;
}
#endif

/*! ************************************

	\brief Release all allocated memory

	If there was any memory allocated to store the output stream, release it and
	reset the class to a "power up" state. After this function call, this class
	can be recycled to create a  new output stream

***************************************/

void BURGER_API Burger::OutputMemoryStream::Clear(void) BURGER_NOEXCEPT
{
	const Chunk_t* pChunk = m_pRoot;
	if (pChunk) {
		const Chunk_t* pNext;
		do {
			pNext = pChunk->m_pNext;
			Free(pChunk);
			pChunk = pNext;
		} while (pNext);
		m_pRoot = nullptr;
	}
	m_pCurrent = nullptr;
	m_uIndex = kChunkSize;
	m_uFileSize = 0;
	m_uError = kErrorNone; // Clear out the error state
}

/*! ************************************

	\fn Burger::OutputMemoryStream::GetSize(void) const
	\brief Return the amount of data stored in the stream

	Calculate and return the number of bytes of valid data that has been stored
	in the data stream.

	\return Number of bytes stored in the stream.

***************************************/

/*! ************************************

	\fn Burger::OutputMemoryStream::IsEmpty(void) const
	\brief Return \ref TRUE if there is no data in the stream

	If there is any data stored in the data stream, return \ref TRUE, otherwise
	return \ref FALSE

	\return \ref TRUE if empty. \ref FALSE if not.

***************************************/

/*! ************************************

	\fn Burger::OutputMemoryStream::GetError(void) const
	\brief Return non zero if the stream is corrupt.

	During data writing, memory may need to be allocated. If an allocation
	failed, the error state is set and can only be cleared with a call to
	Clear() which resets the state to empty.

	\return Non zero if corrupt. Zero if the stream is okay.

***************************************/

/*! ************************************

	\brief Set the current write mark.

	Set the write mark. If it's beyond the existing write buffer, expand the
	buffer to accommodate and adjust the buffer length.

	\return Zero if no error, non-zero if memory couldn't be allocated.
	\sa GetMark(void) const

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::SetMark(
	uintptr_t uMark) BURGER_NOEXCEPT
{
	// Only work if the data is intact
	if (m_uError == kErrorNone) {

		// Try the easy way first
		if (m_uFileSize >= uMark) {

			// No need to expand, just find the mark in the
			// existing data and stop there.

			// Zero is a special case
			if (!uMark) {
				// Was there any data allocated?
				if (m_pRoot) {
					// Reset to the first record
					m_pCurrent = m_pRoot;
					m_uIndex = 0;
				}
			} else {

				// Traverse the chunks from the root until the chunk that
				// contains the mark is found

				Chunk_t* pChunk = m_pRoot;
				do {
					// On this chunk?
					if ((pChunk->m_uMark + kChunkSize) >= uMark) {
						// Update the mark information
						m_pCurrent = pChunk;
						// Can be kChunkSize, this is okay
						m_uIndex = uMark - pChunk->m_uMark;
						break;
					}
					pChunk = pChunk->m_pNext;
				} while (pChunk);

				// pChunk should never be NULL here,
				// if it is, data was trashed
			}
		} else {

			// The mark is beyond the end of the buffer. In this case, add
			// chunks until the mark is reached

			Chunk_t* pChunk = m_pCurrent;
			if (!pChunk) {
				// Make sure there is at least one chunk
				Append(static_cast<uint8_t>(0));
				pChunk = m_pCurrent;
			}

			// If it's m_pCurrent==NULL at this point, it means that the
			// Append() above failed and the code should exit immediately.

			if (pChunk) {

				// Traverse to the end of the chain.
				Chunk_t* pNext = pChunk->m_pNext;
				if (pNext) {
					do {
						// Save the last known good
						pChunk = pNext;
						// Next one
						pNext = pChunk->m_pNext;
					} while (pNext);
				}

				// At this point, pChunk is pointing to the last record of the
				// linked list.

				do {
					// On this chunk?
					if ((pChunk->m_uMark + kChunkSize) >= uMark) {

						// Update the mark information
						// Extend the file mark to this point
						m_uFileSize = uMark;
						// Ensure the current record is correct
						m_pCurrent = pChunk;
						// Can be kChunkSize, this is okay
						m_uIndex = uMark - pChunk->m_uMark;
						break;
					}

					// Since pChunk is the end of the list,
					// allocations are performed to extend the list

					// Force an allocation
					m_uIndex = kChunkSize;

					// Add a chunk by appending a byte
					Append(static_cast<uint8_t>(0));

					// Continue where we left off
					pChunk = m_pCurrent;

					// Keep looping if there was no error
				} while (m_uError == Burger::kErrorNone);
			}
		}
	}
	return m_uError;
}

/*! ************************************

	\brief Return the current write mark.

	Calculate the current offset from the beginning of the stream where the next
	data will be written, return that value.

	\return Current write mark
	\sa SetMark(uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::OutputMemoryStream::GetMark(
	void) const BURGER_NOEXCEPT
{
	uintptr_t uMark = 0;
	const Chunk_t* pChunk = m_pCurrent;
	if (pChunk) {
		uMark = pChunk->m_uMark + m_uIndex;
	}
	return uMark;
}

/*! ************************************

	\brief Save the data into a file

	Open, write and close a file that will contain the contents of the entire
	data stream.

	\param pFilename Burgerlib format filename
	\return Error code with zero being no error, non-zero is an error condition
	\sa SaveFile(Filename *) const

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::SaveFile(
	const char* pFilename) const BURGER_NOEXCEPT
{
	// Are we in a good state?
	eError uResult = m_uError;
	if (!uResult) {
		// Try to open the output file
		File FileRef;
		uResult = FileRef.open(pFilename, File::kWriteOnly);
		// File opened fine?
		if (!uResult) {
			// Assume success
			uintptr_t uRemaining = m_uFileSize;
			if (uRemaining) {
				const Chunk_t* pChunk = m_pRoot;
				do {
					uintptr_t uChunkSize = kChunkSize;
					if (uRemaining < uChunkSize) {
						uChunkSize = uRemaining;
					}
					if (FileRef.write(pChunk->m_Buffer, uChunkSize) !=
						uChunkSize) {
						// File error! But not a state error
						// so don't update m_uError
						uResult = kErrorWriteFailure;
						break;
					}
					pChunk = pChunk->m_pNext;
					// All done?
					uRemaining -= uChunkSize;
				} while (uRemaining);
			}
			// Close the file
			FileRef.close();
		}
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Save the data into a file

	Open, write and close a file that will contain the contents of the entire
	data stream.

	\param pFilename Burgerlib filename
	\return Error code with zero being no error, non-zero is an error condition
	\sa SaveFile(const char *) const

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::SaveFile(
	Filename* pFilename) const BURGER_NOEXCEPT
{
	// Are we in a good state?
	eError uResult = m_uError;
	if (!uResult) {
		// Try to open the output file
		File FileRef;
		uResult = FileRef.open(pFilename, File::kWriteOnly);
		// File opened fine?
		if (!uResult) {
			// Assume success
			uintptr_t uRemaining = m_uFileSize;
			if (uRemaining) {
				const Chunk_t* pChunk = m_pRoot;
				do {
					uintptr_t uChunkSize = kChunkSize;
					if (uRemaining < uChunkSize) {
						uChunkSize = uRemaining;
					}
					if (FileRef.write(pChunk->m_Buffer, uChunkSize) !=
						uChunkSize) {
						// File error! But not a state error
						// so don't update m_uError
						uResult = kErrorWriteFailure;
						break;
					}
					pChunk = pChunk->m_pNext;
					// All done?
					uRemaining -= uChunkSize;
				} while (uRemaining);
			}
			// Close the file
			FileRef.close();
		}
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Save the data into a String

	Fill a Burgerlib string with the contents of the byte stream.

	\param pOutput String to copy the data into
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Save(
	String* pOutput) const BURGER_NOEXCEPT
{
	// SetBufferSize() retains the text, disable it by clearing first
	pOutput->clear();
	// Are we in a good state?
	eError uResult = m_uError;
	if (!uResult) {
		// Ensure the buffer is ready
		uintptr_t uRemaining = m_uFileSize;
		uResult = pOutput->resize(uRemaining);
		if (!uResult) {
			// Get the buffer pointer
			char* pWork = pOutput->c_str();
			if (uRemaining) {
				const Chunk_t* pChunk = m_pRoot;
				do {
					uintptr_t uChunkSize = kChunkSize;
					if (uRemaining < uChunkSize) {
						uChunkSize = uRemaining;
					}
					MemoryCopy(pWork, pChunk->m_Buffer, uChunkSize);
					pWork += uChunkSize;
					pChunk = pChunk->m_pNext;
					// All done?
					uRemaining -= uChunkSize;
				} while (uRemaining);
			}

			// Terminate the string
			pWork[0] = 0;
		}
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Flatten the data into a single buffer

	Fill a supplied buffer with the contents of the byte stream

	\note The buffer must be large enough to contain the data or it will return
	an error code and not copy anything.

	\param pOutput Pointer to a buffer to copy the data into
	\param uLength Size of the buffer
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Flatten(
	void* pOutput, uintptr_t uLength) const BURGER_NOEXCEPT
{
	uintptr_t uRemaining = m_uFileSize;
	if (uRemaining > uLength) {
		// Not enough buffer to store in
		return kErrorBufferTooSmall;
	}
	// Assume success
	if (uRemaining) {
		const Chunk_t* pChunk = m_pRoot;
		do {
			uintptr_t uChunkSize = kChunkSize;
			if (uRemaining < uChunkSize) {
				uChunkSize = uRemaining;
			}
			MemoryCopy(pOutput, pChunk->m_Buffer, uChunkSize);
			pOutput = static_cast<uint8_t*>(pOutput) + uChunkSize;
			pChunk = pChunk->m_pNext;
			// All done?
			uRemaining -= uChunkSize;
		} while (uRemaining);
	}
	return kErrorNone;
}

/*! ************************************

	\brief Flatten the data into a single buffer

	Allocate a buffer and fill it with the data in the stream. When the buffer
	is no longer needed, release it with a call to Burger::Free()

	\param pLength Size of the buffer returned
	\return nullptr on error, or a valid pointer to the buffer returned

***************************************/

void* BURGER_API Burger::OutputMemoryStream::Flatten(
	uintptr_t* pLength) const BURGER_NOEXCEPT
{
	uintptr_t uRemaining = m_uFileSize;
	void* pResult = Alloc(uRemaining);
	if (!pResult) {
		// Error!
		uRemaining = 0;
	} else {
		// Assume success
		if (uRemaining) {
			void* pOutput = pResult;
			const Chunk_t* pChunk = m_pRoot;
			do {
				uintptr_t uChunkSize = kChunkSize;
				if (uRemaining < uChunkSize) {
					uChunkSize = uRemaining;
				}
				MemoryCopy(pOutput, pChunk->m_Buffer, uChunkSize);
				pOutput = static_cast<uint8_t*>(pOutput) + uChunkSize;
				pChunk = pChunk->m_pNext;
				// All done?
				uRemaining -= uChunkSize;
			} while (uRemaining);
			// Restore the length
			uRemaining = m_uFileSize;
		}
	}
	// Save the length stored
	if (pLength) {
		pLength[0] = uRemaining;
	}
	return pResult;
}

/*! ************************************

	\brief Add a text character to the data stream

	Assume the character is from a text stream, so if it's a '\\n', convert it
	to the proper type for the target operating system such as '\\n' for Linux,
	'\\r' for MacOS and '\\n\\r' for Windows.

	\param iChar Single byte of a UTF-8 text file stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	char iChar) BURGER_NOEXCEPT
{
	// Switch to \r for MacOS classic (OSX is Unix)
#if defined(BURGER_MAC)
	if (iChar == '\n') {
		iChar = '\r';
	}
#elif defined(BURGER_WINDOWS) || defined(BURGER_MSDOS)
	// Insert a \r for windows
	if (iChar == '\n') {
		Append(static_cast<uint8_t>('\r'));
	}
#endif
	// Pass the data to the stream
	return Append(static_cast<uint8_t>(iChar));
}

/*! ************************************

	\brief Add a UTF-8 "C" string to the data stream

	If the string has any '\\n' in it, they will be converted
	to the proper type for the target
	operating system such as '\\n' for Linux, '\\r' for MacOS
	and '\\n\\r' for Windows.

	\note The terminating zero is NOT stored in the stream
	\param pString "C" UTF-8 data string. nullptr will perform no action and
		return no error.
	\return Error code with zero being no error, non-zero is out of
		memory
	\sa AppendPString(const char *)

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const char* pString) BURGER_NOEXCEPT
{
	// Start with no error
	if (pString) {
		// Output a character at a time to allow \n
		// to be converted to \r or \n\r depending
		// on platform
		char Temp = pString[0];
		if (Temp) {
			do {
				++pString;
				Append(Temp);
				Temp = pString[0];
			} while (Temp);
		}
	}
	// Return 0 or error code
	return m_uError;
}

/*! ************************************

	\brief Add a UTF-8 "C" string to the data stream

	The data stream will have a zero terminated "C" string.

	The terminating zero will be included in the stream, if this is not desired,
	call \ref Append(const char *) instead

	\param pString "C" UTF-8 data string. nullptr will insert a zero length
		string
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const char *) or AppendPString(const char *)

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendCString(
	const char* pString) BURGER_NOEXCEPT
{
	// NULL string?
	if (!pString) {
		// Insert a zero length string
		Append(static_cast<uint8_t>(0));
	} else {
		// Get the length
		const uintptr_t uLength = string_length(pString);
		// Insert in one go
		Append(pString, uLength + 1);
	}
	// Return 0 or error code
	return m_uError;
}

/*! ************************************

	\fn Burger::OutputMemoryStream::Append(const String *pString)
	\brief Add a UTF-8 "C" string to the data stream

	The data stream will have a zero terminated "C" string obtained from the
	\ref String class instance.

	The terminating zero will be included in the stream, if this is not desired,
	call \ref Append(const char *) instead with the pointer from the
	String::GetPtr() const function.

	\param pString Pointer to a \ref String class
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const char *) or AppendCString(const char *)

***************************************/

/*! ************************************

	\brief Add a UTF-8 "P" string to the data stream

	The data stream will be first given a byte with the length of the string,
	followed by the string. If the string is greater than 255 bytes in length,
	it will be truncated in the stream.

	\param pString "C" UTF-8 data string. nullptr will perform no action and
		return no error.
	 \return Error code with zero being no error, non-zero is out of memory
	 \sa Append(const char *) or AppendCString(const char *)

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendPString(
	const char* pString) BURGER_NOEXCEPT
{
	if (pString) {
		uintptr_t uLength = string_length(pString);
		// Truncate to the maximum length of a "P" string
		if (uLength >= 256) {
			uLength = 255;
		}
		Append(static_cast<uint8_t>(uLength));
		Append(pString, uLength);
	}
	// Return 0 or error code
	return m_uError;
}

/*! ************************************

	\brief Add a single byte to the end of the data stream

	Append a single byte to the end of the data stream and allocate memory if
	necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	\param uInput Byte of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	uint8_t uInput) BURGER_NOEXCEPT
{
	// Write the byte into the stream

	uintptr_t uIndex = m_uIndex;
	Chunk_t* pChunk = m_pCurrent;

	// Fast case, space for the data already exists
	if (uIndex < kChunkSize) {
		// Store the data
		pChunk->m_Buffer[uIndex] = uInput;
		++uIndex;
		// Update the index
		m_uIndex = uIndex;

		// Did the end of file move?
		uIndex += pChunk->m_uMark; // Get the mark + the number of bytes used
		if (m_uFileSize < uIndex) {
			// Update it
			m_uFileSize = uIndex;
		}
		// All done!
		return m_uError;
	}

	// Check if there is already another chunk allocated
	// This occurs if the write mark was moved backwards

	if (pChunk) {
		Chunk_t* pNext = pChunk->m_pNext;
		if (pNext) {
			pNext->m_Buffer[0] = uInput;

			// Update the working chunk data
			m_pCurrent = pNext;
			m_uIndex = 1;

			// Did the end of file move?
			// Get the mark + the number of bytes used
			uIndex = pNext->m_uMark + 1;
			if (m_uFileSize < uIndex) {
				// Update it
				m_uFileSize = uIndex;
			}
			// All done!
			return m_uError;
		}
	}

	// Looks like another buffer is needed
	// Make sure it's cleared out in case the mark is skipped ahead
	Chunk_t* pNewChunk = static_cast<Chunk_t*>(alloc_clear(sizeof(Chunk_t)));
	if (!pNewChunk) {
		// Error!!! Data is corrupt from now on!
		m_uError = kErrorOutOfMemory;

	} else {

		// Initialize the buffer with the next link (NULL)
		// pNext->m_pNext = NULL;		// Already cleared

		// Insert the byte to save
		m_uIndex = 1;
		pNewChunk->m_Buffer[0] = uInput;

		// Is this the first buffer? If so, make it the root.
		m_pCurrent = pNewChunk;
		if (!pChunk) {
			// pNext->m_uMark = 0;			// Already cleared
			m_pRoot = pNewChunk;
		} else {
			// Set the new mark
			pNewChunk->m_uMark = pChunk->m_uMark + kChunkSize;
			// Insert this pointer into the linked list for later disposal
			pChunk->m_pNext = pNewChunk;
		}

		// Did the end of file move?
		uIndex = pNewChunk->m_uMark + 1;
		if (m_uFileSize < uIndex) {
			// Update it
			m_uFileSize = uIndex;
		}
	}
	return m_uError;
}

/*! ************************************

	\brief Add a 16 bit value to the end of the data stream

	Append two bytes to the end of the data stream in little endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 16 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	uint16_t uInput) BURGER_NOEXCEPT
{
	Append(static_cast<uint8_t>(uInput));
	return Append(static_cast<uint8_t>(uInput >> 8U));
}

/*! ************************************

	\brief Add a 32 bit value to the end of the data stream

	Append four bytes to the end of the data stream in little endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	uint32_t uInput) BURGER_NOEXCEPT
{
	Append(static_cast<uint8_t>(uInput));
	Append(static_cast<uint8_t>(uInput >> 8U));
	Append(static_cast<uint8_t>(uInput >> 16U));
	return Append(static_cast<uint8_t>(uInput >> 24U));
}

/*! ************************************

	\brief Add a 64 bit value to the end of the data stream

	Append eight bytes to the end of the data stream in little endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	uint64_t uInput) BURGER_NOEXCEPT
{
	uInput = LittleEndian::load(uInput);
	return Append(&uInput, sizeof(uint64_t));
}

/*! ************************************

	\brief Add a 32 bit floating point value to the end of the data stream

	Append four bytes to the end of the data stream in little endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param fInput 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	float fInput) BURGER_NOEXCEPT
{
	LittleEndian::fixup(&fInput);
	return Append(&fInput, sizeof(float));
}

/*! ************************************

	\brief Add a 64 bit floating point value to the end of the data stream

	Append eight bytes to the end of the data stream in little endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param dInput 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	double dInput) BURGER_NOEXCEPT
{
	LittleEndian::fixup(&dInput);
	return Append(&dInput, sizeof(double));
}

/*! ************************************

	\brief Add an R, G, B color to the end of the data stream

	Append three bytes to the end of the data stream and allocate memory if
	necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	\param pInput Pointer to an R, G and B color value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const RGBWord8_t* pInput) BURGER_NOEXCEPT
{
	return Append(pInput, 3);
}

/*! ************************************

	\brief Add an R, G, B, A color to the end of the data stream

	Append four bytes to the end of the data stream and allocate memory if
	necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	\param pInput Pointer to an R, G, B and A color value to append to the
		stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const RGBAWord8_t* pInput) BURGER_NOEXCEPT
{
	return Append(pInput, 4);
}

/*! ************************************

	\brief Add an X, Y, Z floating point value to the end of the data stream

	Append three 32 bit floats to the end of the data stream and allocate memory
	if necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	Data is stored in little endian format

	\param pInput Pointer to an X, Y, and Z value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const RGBFloat_t *)

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const Vector3D_t* pInput) BURGER_NOEXCEPT
{
	Append(pInput->x);
	Append(pInput->y);
	return Append(pInput->z);
}

/*! ************************************

	\brief Add an X, Y, Z, W floating point value to the end of the data stream

	Append four 32 bit floats to the end of the data stream and allocate memory
	if necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	Data is stored in little endian format

	\param pInput Pointer to an X, Y, Z and W value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const RGBAFloat_t *)

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const Vector4D_t* pInput) BURGER_NOEXCEPT
{
	Append(pInput->x);
	Append(pInput->y);
	Append(pInput->z);
	return Append(pInput->w);
}

/*! ************************************

	\fn Burger::OutputMemoryStream::Append(const RGBFloat_t *)
	\brief Add an R, G, B floating point value to the end of the data stream

	Append three 32 bit floats to the end of the data stream and allocate memory
	if necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	Data is stored in little endian format

	\param pInput Pointer to an R, B, and G value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const Vector3D_t *)

***************************************/

/*! ************************************

	\fn Burger::OutputMemoryStream::Append(const RGBAFloat_t *)
	\brief Add an R, G, B, A floating point value to the end of the data stream

	Append four 32 bit floats to the end of the data stream and allocate memory
	if necessary. If the memory allocation fails, a non-zero error code will be
	returned.

	Data is stored in little endian format

	\param pInput Pointer to an R, G, B and A value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const Vector4D_t *)

***************************************/

/*! ************************************

	\brief Add an array of bytes to the data stream

	Given a buffer pointer and a length, append the byte stream to the output
	stream.

	\param pData Pointer to the data to append to the stream. nullptr performs
		no operation
	\param uSize Number of bytes to append to the stream. Zero performs
		no operation
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Append(
	const void* pData, uintptr_t uSize) BURGER_NOEXCEPT
{
	// Start with no error
	if (pData && uSize) {
		// Let's add the data
		do {
			Append(static_cast<const uint8_t*>(pData)[0]);
			pData = static_cast<const uint8_t*>(pData) + 1;
			// Continue?
		} while (--uSize);
	}
	// Return 0 or error code
	return m_uError;
}

/*! ************************************

	\brief Add a 16 bit value to the end of the data stream

	Append two bytes to the end of the data stream in big endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 16 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::BigEndianAppend(
	uint16_t uInput) BURGER_NOEXCEPT
{
	Append(static_cast<uint8_t>(uInput >> 8U));
	return Append(static_cast<uint8_t>(uInput));
}

/*! ************************************

	\brief Add a 32 bit value to the end of the data stream

	Append four bytes to the end of the data stream in big endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::BigEndianAppend(
	uint32_t uInput) BURGER_NOEXCEPT
{
	Append(static_cast<uint8_t>(uInput >> 24U));
	Append(static_cast<uint8_t>(uInput >> 16U));
	Append(static_cast<uint8_t>(uInput >> 8U));
	return Append(static_cast<uint8_t>(uInput));
}

/*! ************************************

	\brief Add a 64 bit value to the end of the data stream

	Append eight bytes to the end of the data stream in big endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param uInput 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::BigEndianAppend(
	uint64_t uInput) BURGER_NOEXCEPT
{
	uInput = BigEndian::load(uInput);
	return Append(&uInput, sizeof(uint64_t));
}

/*! ************************************

	\brief Add a 32 bit floating point value to the end of the data stream

	Append four bytes to the end of the data stream in big endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param fInput 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::BigEndianAppend(
	float fInput) BURGER_NOEXCEPT
{
	BigEndian::fixup(&fInput);
	return Append(&fInput, sizeof(float));
}

/*! ************************************

	\brief Add a 64 bit floating point value to the end of the data stream

	Append eight bytes to the end of the data stream in big endian format and
	allocate memory if necessary. If the memory allocation fails, a non-zero
	error code will be returned.

	\param dInput 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::BigEndianAppend(
	double dInput) BURGER_NOEXCEPT
{
	BigEndian::fixup(&dInput);
	return Append(&dInput, sizeof(double));
}

/*! ************************************

	\brief Convert a 16 bit unsigned integer to text and append it

	Convert a 16 bit unsigned integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param uInput 16 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	uint16_t uInput) BURGER_NOEXCEPT
{
	const NumberString Text(static_cast<uint32_t>(uInput));
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 32 bit unsigned integer to text and append it

	Convert a 32 bit unsigned integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param uInput 32 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	uint32_t uInput) BURGER_NOEXCEPT
{
	const NumberString Text(uInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 64 bit unsigned integer to text and append it

	Convert a 64 bit unsigned integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param uInput 64 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	uint64_t uInput) BURGER_NOEXCEPT
{
	const NumberString Text(uInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 16 bit signed integer to text and append it

	Convert a 16 bit signed integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param iInput 16 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	int16_t iInput) BURGER_NOEXCEPT
{
	const NumberString Text(static_cast<int32_t>(iInput));
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 32 bit signed integer to text and append it

	Convert a 32 bit signed integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param iInput 32 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	int32_t iInput) BURGER_NOEXCEPT
{
	const NumberString Text(iInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 64 bit signed integer to text and append it

	Convert a 64 bit signed integer into ASCII and append the resulting string
	to the end of the stream. If the memory allocation fails, a non-zero error
	code will be returned.

	\param iInput 64 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	int64_t iInput) BURGER_NOEXCEPT
{
	const NumberString Text(iInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 32 bit floating point number to text and append it

	Convert a 32 bit floating pointer number into ASCII and append the resulting
	string to the end of the stream. If the memory allocation fails, a non-zero
	error code will be returned.

	\param fInput 32 bit floating point value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	float fInput) BURGER_NOEXCEPT
{
	const NumberString Text(fInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Convert a 64 bit floating point number to text and append it

	Convert a 64 bit floating pointer number into ASCII and append the resulting
	string to the end of the stream. If the memory allocation fails, a non-zero
	error code will be returned.

	\param dInput 64 bit floating point value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendAscii(
	double dInput) BURGER_NOEXCEPT
{
	const NumberString Text(dInput);
	return Append(Text.c_str());
}

/*! ************************************

	\brief Insert a number of tabs into the data stream

	For some text files, tabs are needed for formatting. Insert a number of tabs
	into the stream.

	\param uTabCount 0 or more tabs to insert
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::AppendTabs(
	uint_t uTabCount) BURGER_NOEXCEPT
{
	if (uTabCount) {
		do {
			Append('\t');      // Insert the tabs
		} while (--uTabCount); // Count down
	}
	return m_uError;
}

/*! ************************************

	\brief Compare data to the data in the stream

	Given a buffer of data, do a byte for byte compare with the data in the
	stream and return FALSE if the data matches and non-zero if the data does
	not.

	\param pInput Pointer to the buffer to match
	\param uLength Size of the buffer to match
	\return \ref FALSE if the data matches, \ref TRUE if not

***************************************/

uint_t BURGER_API Burger::OutputMemoryStream::Compare(
	const void* pInput, uintptr_t uLength) const BURGER_NOEXCEPT
{
	// Assume failure
	uint_t uResult = TRUE;
	if (!m_uError) {

		uintptr_t uRemaining = m_uFileSize;
		// Checking for empty?
		if (!uLength) {
			if (!uRemaining) {
				// Match!
				uResult = FALSE;
			}
		} else {
			if (uRemaining) {
				const Chunk_t* pChunk = m_pRoot;
				do {
					// Get the number of valid bytes
					uintptr_t uChunkSize = kChunkSize;
					if (uRemaining < uChunkSize) {
						uChunkSize = uRemaining;
					}
					// Are there less bytes to check than in the buffer?
					if (uLength < uChunkSize) {
						// Size mismatch!
						break;
					}

					// Memory error in the stream!
					if (!pChunk) {
						break;
					}

					// Compare this chunk
					if (MemoryCompare(pChunk->m_Buffer, pInput, uChunkSize)) {
						// Data mismatch!
						break;
					}

					// Consume this input data
					pInput = static_cast<const uint8_t*>(pInput) + uChunkSize;
					uLength -= uChunkSize;
					uRemaining -= uChunkSize;

					// No more data from input and stream?
					if (!uLength && !uRemaining) {
						// The data is a match!
						uResult = FALSE;
						break;
					}
					pChunk = pChunk->m_pNext;
				} while (uRemaining);
			}
		}
	}

	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Overwrite data in the stream

	Given a buffer of data, replace the data in the stream and return zero if no
	error and non-zero if there is an attempted buffer overrun.

	\param pInput Pointer to the buffer to match
	\param uLength Size of the buffer to match
	\param uOffset Offset into the data to begin the overwrite
	\return Zero if there was no overflow, non-zero if so

***************************************/

Burger::eError BURGER_API Burger::OutputMemoryStream::Overwrite(
	const void* pInput, uintptr_t uLength, uintptr_t uOffset) BURGER_NOEXCEPT
{
	// Assume success
	eError uResult = kErrorNone;

	// Checking for empty?
	if (uLength) {

		// Now assume failure
		uResult = kErrorWriteFailure;
		uintptr_t uRemaining = m_uFileSize;
		if (uRemaining) {

			// Test if there was a memory issue, if so, return error
			Chunk_t* pChunk = m_pRoot;
			do {
				if (!pChunk) {
					break;
				}

				// Set the write mark
				const uintptr_t uMark = pChunk->m_uMark;

				// Get the number of valid bytes
				uintptr_t uChunkSize = kChunkSize;
				if (uRemaining < uChunkSize) {
					uChunkSize = uRemaining;
				}

				// Is the offset beyond this chunk?
				// If so, skip this chunk since it's before the
				// requested file mark offset
				const uintptr_t uEndMark = uMark + uChunkSize;
				if (uOffset < uEndMark) {

					// How many bytes to skip?
					uintptr_t uSkip = uOffset - uMark;
					uChunkSize -= uSkip;

					// Apply the same adjustment to uRemaining from
					// uChunkSize to allow the uRemaining -= uChunkSize
					// below to stay correct
					uRemaining -= uSkip;

					if (uLength < uChunkSize) {
						uChunkSize = uLength;
					}

					// Overwrite this chunk
					MemoryCopy(&pChunk->m_Buffer[uSkip], pInput, uChunkSize);

					// Consume this input data
					pInput = static_cast<const uint8_t*>(pInput) + uChunkSize;
					uLength -= uChunkSize;

					// All done?
					if (!uLength) {
						uResult = kErrorNone;
						break;
					}
					// Set the new mark
					uOffset = uEndMark;
				}
				// Next chunk
				pChunk = pChunk->m_pNext;
				uRemaining -= uChunkSize;
			} while (uRemaining);
		}
	}

	// Return the error code
	return uResult;
}
