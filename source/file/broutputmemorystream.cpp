/***************************************

	Simple data stream class for output

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "broutputmemorystream.h"
#include "brfile.h"
#include "brendian.h"
#include "brnumberstring.h"
#include "brpalette.h"

/*! ************************************

	\class Burger::OutputMemoryStream
	\brief Fast output memory stream.

	When generating a new data file, it's much faster
	to stream the data into memory and once the data is
	written, store it out to disk.
	
	If the size of the output file is not known, allocating a buffer
	large enough to hold the data is educated guesswork.
	This class solves this problem twofold, firstly, it allocates
	in reasonable sized chunks to prevent out of memory
	situations due to allocating very large memory blocks and
	not succeeding due to fragmentation. Secondly, it will
	allow writing to a byte stream so data alignment at the
	stream level is not necessary and large data is always
	stored in little endian format so files created
	this way will be cross platform.

	\sa Burger::InputMemoryStream

***************************************/

/*! ************************************

	\brief Constructor.
	
	Initializes a stream for output. No memory is allocated at this time
	
***************************************/

Burger::OutputMemoryStream::OutputMemoryStream() :
	m_pData(NULL),
	m_pWork(NULL),
	m_uIndex(CHUNKSIZE)
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

	\brief Release all allocated memory

	If there was any memory allocated to store
	the output stream, release it and reset the
	class to a "power up" state. After this function
	call, this class can be recycled to create a
	new output stream

***************************************/

void BURGER_API Burger::OutputMemoryStream::Clear(void)
{
	Word8 *pData = m_pData;
	if (pData) {
		Word8 *pNext;
		do {
			pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
			Free(pData);
			pData = pNext;
		} while (pNext);
		m_pData = NULL;
	}
	m_pWork = NULL;
	m_uIndex = CHUNKSIZE;
}

/*! ************************************

	\brief Return the amount of data stored in the stream

	Calculate and return the number of bytes of valid data
	that has been stored in the data stream.

	\return Number of bytes stored in the stream.

***************************************/

WordPtr BURGER_API Burger::OutputMemoryStream::GetSize(void) const
{
	// If the pointer is NULL, return 0 for size
	WordPtr uSize = 0;
	Word8 *pData = m_pData;
	if (pData) {
		// Start with the size from the last chunk
		uSize = m_uIndex;
		pData = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
		if (pData) {
			do {
				// Every other chunk is full size
				pData = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
				uSize += CHUNKSIZE;
			} while (pData);
		}
	}
	return uSize;
}

/*! ************************************

	\brief Return \ref TRUE if there is no data in the stream

	If there is any data stored in the data stream, return \ref TRUE,
	otherwise return \ref FALSE

	\return \ref TRUE if empty. \ref FALSE if not.

***************************************/

Word BURGER_API Burger::OutputMemoryStream::IsEmpty(void) const
{
	// If the pointer is NULL, we're empty
	Word uResult = TRUE;
	const Word8 *pData = m_pData;
	if (pData) {
		uResult = FALSE;
	}
	return uResult;
}

/*! ************************************

	\brief Save the data into a file

	Open, write and close a file that will contain
	the contents of the entire data stream.

	\param pFilename Burgerlib format filename
	\return Error code with zero being no error, non-zero is an error condition
	\sa SaveFile(Filename *) const

***************************************/

Word BURGER_API Burger::OutputMemoryStream::SaveFile(const char *pFilename) const
{
	// Try to open the output file
	File FileRef;
	Word uResult = FileRef.Open(pFilename,File::WRITEONLY);
	// File opened fine?
	if (!uResult) {
		// Assume success
		Word8 *pData = m_pData;
		if (pData) {
			Word8 *pNext;
			do {
				pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
				WordPtr uChunk = CHUNKSIZE;
				if (!pNext) {
					uChunk = m_uIndex;
				}
				if (FileRef.Write(pData,uChunk)!=uChunk) {
					uResult = 5;		// File error!
					break;
				}
				pData = pNext;
			} while (pNext);
		}
		// Close the file
		FileRef.Close();
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Save the data into a file

	Open, write and close a file that will contain
	the contents of the entire data stream.

	\param pFilename Burgerlib filename
	\return Error code with zero being no error, non-zero is an error condition
	\sa SaveFile(const char *) const

***************************************/

Word BURGER_API Burger::OutputMemoryStream::SaveFile(Filename *pFilename) const
{
	// Try to open the output file
	File FileRef;
	Word uResult = FileRef.Open(pFilename,File::WRITEONLY);
	// File opened fine?
	if (!uResult) {
		// Assume success
		Word8 *pData = m_pData;
		if (pData) {
			Word8 *pNext;
			do {
				pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
				WordPtr uChunk = CHUNKSIZE;
				if (!pNext) {
					uChunk = m_uIndex;
				}
				if (FileRef.Write(pData,uChunk)!=uChunk) {
					uResult = 5;		// File error!
					break;
				}
				pData = pNext;
			} while (pNext);
		}
		// Close the file
		FileRef.Close();
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Save the data into a String

	Fill a burgerlib string with the contents of the byte stream.

	\param pOutput String to copy the data into
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Save(Burger::String *pOutput) const
{
	// SetBufferSize() retains the text, disable it by clearing first
	pOutput->Clear();
	WordPtr uSize = GetSize();
	pOutput->SetBufferSize(uSize);
	// Assume success
	Word8 *pData = m_pData;
	char *pWork = pOutput->GetPtr();
	if (pData) {
		Word8 *pNext;
		do {
			pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
			WordPtr uChunk = CHUNKSIZE;
			if (!pNext) {
				uChunk = m_uIndex;
			}
			MemoryCopy(pWork,pData,uChunk);
			pWork+=uChunk;
			pData = pNext;
		} while (pNext);
	}
	pWork[0] = 0;
	// Return the error code
	return 0;
}

/*! ************************************

	\brief Flatten the data into a single buffer

	Fill a supplied buffer with the contents of the byte stream

	\note The buffer MUST be an exact match to the number of bytes in
	the stream or an error will occur
	
	\param pOutput Pointer to a buffer to copy the data into
	\param uLength Size of the buffer
	\return Error code with zero being no error, non-zero is an error condition

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Flatten(void *pOutput,WordPtr uLength) const
{
	WordPtr uSize = GetSize();
	if (uSize!=uLength) {
		return TRUE;
	}
	// Assume success
	Word8 *pData = m_pData;
	if (pData) {
		Word8 *pNext;
		do {
			pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
			WordPtr uChunk = CHUNKSIZE;
			if (!pNext) {
				uChunk = m_uIndex;
			}
			MemoryCopy(pOutput,pData,uChunk);
			pOutput = static_cast<Word8 *>(pOutput) + uChunk;
			pData = pNext;
		} while (pNext);
	}
	return 0;
}

/*! ************************************

	\brief Flatten the data into a single buffer

	Allocate a buffer and fill it with the data in the stream. 
	When the buffer is no longer needed, release it with a call
	to Burger::Free()

	\param pLength Size of the buffer returned
	\return \ref NULL on error, or a valid pointer to the buffer returned

***************************************/

void * BURGER_API Burger::OutputMemoryStream::Flatten(WordPtr *pLength) const
{
	WordPtr uSize = GetSize();
	void *pResult = Alloc(uSize);
	if (!pResult) {
		uSize = 0;
	} else {
		// Assume success
		Word8 *pData = m_pData;
		if (pData) {
			void *pOutput = pResult;
			Word8 *pNext;
			do {
				pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
				WordPtr uChunk = CHUNKSIZE;
				if (!pNext) {
					uChunk = m_uIndex;
				}
				MemoryCopy(pOutput,pData,uChunk);
				pOutput = static_cast<Word8 *>(pOutput) + uChunk;
				pData = pNext;
			} while (pNext);
		}
	}
	if (pLength) {
		pLength[0] = uSize;
	}
	return pResult;
}

/*! ************************************

	\brief Add a text character to the data stream

	Assume the character is from a text stream, so if it's
	a '\\n', convert it to the proper type for the target
	operating system such as '\\n' for Linux, '\\r' for MacOS
	and '\\n\\r' for Windows.

	\param iChar Single byte of a UTF-8 text file stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(char iChar)
{
	// Switch to \r for macos classic (OSX is Unix)
#if defined(BURGER_MAC)
	if (iChar=='\n') {
		iChar = '\r';
	}
#elif defined(BURGER_WINDOWS) || defined(BURGER_MSDOS)
	// Insert a \r for windows
	if (iChar=='\n') {
		Append(static_cast<Word8>('\r'));
	}
#endif
	// Pass the data to the stream
	return Append(static_cast<Word8>(iChar));
}

/*! ************************************

	\brief Add a UTF-8 "C" string to the data stream

	If the string has any '\\n' in it, they will be converted
	to the proper type for the target
	operating system such as '\\n' for Linux, '\\r' for MacOS
	and '\\n\\r' for Windows.

	\note The terminating zero is NOT stored in the stream
	\param pString "C" UTF-8 data string. \ref NULL will perform no action and return no error.
	\return Error code with zero being no error, non-zero is out of memory
	\sa AppendPString(const char *)

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const char *pString)
{
	// Start with no error
	Word uResult=0;
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
	return uResult;
}

/*! ************************************

	\brief Add a UTF-8 "C" string to the data stream

	The data stream will have a zero terminated "C" string.

	The terminating zero will be included in the stream, if this is
	not desired, call \ref Append(const char *) instead

	\param pString "C" UTF-8 data string. \ref NULL will insert a zero length string
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const char *) or AppendPString(const char *)

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendCString(const char *pString)
{
	Word uResult;

	// NULL string?
	if (!pString) {
		// Insert a zero length string
		uResult = Append(static_cast<Word8>(0));
	} else {
		// Get the length
		WordPtr uLength = StringLength(pString);
		// Insert in one go
		uResult = Append(pString,uLength+1);
	}
	// Return 0 or error code
	return uResult;
}

/*! ************************************

	\fn Word Burger::OutputMemoryStream::Append(const String *pString)
	\brief Add a UTF-8 "C" string to the data stream

	The data stream will have a zero terminated "C" string obtained
	from the \ref String class instance.

	The terminating zero will be included in the stream, if this is
	not desired, call \ref Append(const char *) instead with the
	pointer from the String::GetPtr() const function.

	\param pString Pointer to a \ref String class
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const char *) or AppendCString(const char *)

***************************************/

/*! ************************************

	\brief Add a UTF-8 "P" string to the data stream

	The data stream will be first given a byte with the length of
	the string, followed by the string. If the string is greater than
	255 bytes in length, it will be truncated in the stream.

	\param pString "C" UTF-8 data string. \ref NULL will perform no action and return no error.
	\return Error code with zero being no error, non-zero is out of memory
	\sa Append(const char *) or AppendCString(const char *)

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendPString(const char *pString)
{
	// Start with no error
	Word uResult=0;
	if (pString) {
		WordPtr uLength = StringLength(pString);
		if (uLength>=256) {
			uLength=255;
		}
		uResult = Append(static_cast<Word8>(uLength));
		if (!uResult) {
			uResult = Append(pString,uLength);
		}
	}
	// Return 0 or error code
	return uResult;
}

/*! ************************************

	\brief Add a single byte to the end of the data stream

	Append a single byte to the end of the data stream and allocate
	memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param uByte Byte of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(Word8 uByte)
{
	// Write the byte into the stream

	WordPtr uIndex = m_uIndex;
	// Fast case, space for the data already exists
	if (uIndex<CHUNKSIZE) {
		// Store the data
		Word8 *pWork = m_pWork;
		pWork[uIndex] = uByte;
		++uIndex;
		m_uIndex = uIndex;
		// All done!
		return 0;
	}

	// Looks like another buffer is needed
	Word8 *pNewData = static_cast<Word8*>(Alloc(CHUNKSIZE+sizeof(Word8*)));
	if (!pNewData) {
		return 10;
	}
	// Initialize the buffer with the next link (NULL)
	// and insert the byte to save
	m_uIndex = 1;
	pNewData[0] = uByte;
	reinterpret_cast<Word8 **>(pNewData+CHUNKSIZE)[0] = NULL;
	// Is this the first buffer? If so, make it the root.
	Word8 *pOldWork = m_pWork;
	m_pWork = pNewData;
	if (!pOldWork) {
		m_pData = pNewData;
	} else {
		// Insert this pointer into the linked list for later disposal
		reinterpret_cast<Word8 **>(pOldWork+CHUNKSIZE)[0] = pNewData;
	}
	return 0;
}

/*! ************************************

	\brief Add a 16 bit value to the end of the data stream

	Append two bytes to the end of the data stream in little endian format 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param uShort 16 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(Word16 uShort)
{
	Word uError = Append(static_cast<Word8>(uShort));
	uError |= Append(static_cast<Word8>(uShort>>8U));
	return uError;
}

/*! ************************************

	\brief Add a 32 bit value to the end of the data stream

	Append four bytes to the end of the data stream in little endian format 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param uWord 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(Word32 uWord)
{
	Word uError = Append(static_cast<Word8>(uWord));
	uError |= Append(static_cast<Word8>(uWord>>8U));
	uError |= Append(static_cast<Word8>(uWord>>16U));
	uError |= Append(static_cast<Word8>(uWord>>24U));
	return uError;
}

/*! ************************************

	\brief Add a 64 bit value to the end of the data stream

	Append eight bytes to the end of the data stream in little endian format 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param uWord 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(Word64 uWord)
{
#if defined(BURGER_BIGENDIAN)
	uWord = SwapEndian::Load(uWord);
#endif
	return Append(&uWord,sizeof(Word64));
}

/*! ************************************

	\brief Add a 32 bit floating point value to the end of the data stream

	Append four bytes to the end of the data stream in little endian format 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param fInput 32 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(float fInput)
{
#if defined(BURGER_BIGENDIAN)
	SwapEndian::Fixup(&fInput);
#endif
	return Append(&fInput,sizeof(float));
}

/*! ************************************

	\brief Add a 64 bit floating point value to the end of the data stream

	Append eight bytes to the end of the data stream in little endian format 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param dInput 64 bits of data to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(double dInput)
{
#if defined(BURGER_BIGENDIAN)
	SwapEndian::Fixup(&dInput);
#endif
	return Append(&dInput,sizeof(double));
}

/*! ************************************

	\brief Add an R, G, B color to the end of the data stream

	Append three bytes to the end of the data stream 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param pInput Pointer to an R, G and B color value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const RGBWord8_t *pInput)
{
	return Append(pInput,3);
}

/*! ************************************

	\brief Add an R, G, B, A color to the end of the data stream

	Append four bytes to the end of the data stream 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	\param pInput Pointer to an R, G, B and A color value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const RGBAWord8_t *pInput)
{
	return Append(pInput,4);
}

/*! ************************************

	\brief Add an X, Y, Z floating point value to the end of the data stream

	Append three 32 bit floats to the end of the data stream 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	Data is stored in little endian format

	\param pInput Pointer to an X, Y, and Z value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const Vector3D_t *pInput)
{
	Word uResult = Append(pInput->x);
	if (!uResult) {
		uResult = Append(pInput->y);
		if (!uResult) {
			uResult = Append(pInput->z);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Add an X, Y, Z, W floating point value to the end of the data stream

	Append four 32 bit floats to the end of the data stream 
	and allocate memory if necessary. If the memory allocation fails, a
	non-zero error code will be returned.

	Data is stored in little endian format

	\param pInput Pointer to an X, Y, Z and W value to append to the stream
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const Vector4D_t *pInput)
{
	Word uResult = Append(pInput->x);
	if (!uResult) {
		uResult = Append(pInput->y);
		if (!uResult) {
			uResult = Append(pInput->z);
			if (!uResult) {
				uResult = Append(pInput->w);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Add an array of bytes to the data stream

	Given a buffer pointer and a length, append the byte stream to
	the output stream.

	\param pData Pointer to the data to append to the stream. \ref NULL performs no operation
	\param uSize Number of bytes to append to the stream. Zero performs no operation
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Append(const void *pData,WordPtr uSize)
{
	// Start with no error
	Word uResult=0;
	if (pData && uSize) {
		// Let's add the data
		do {
			uResult = Append(static_cast<const Word8 *>(pData)[0]);
			// Error?
			if (uResult) {
				break;
			}
			pData = static_cast<const Word8 *>(pData)+1;
			// Continue?
		} while (--uSize);
	}
	// Return 0 or error code
	return uResult;
}

/*! ************************************

	\brief Convert a 32 bit unsigned integer to text and append it

	Convert a 32 bit unsigned integer into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param uInput 32 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(Word32 uInput)
{
	NumberString Text(uInput);
	return Append(Text);
}

/*! ************************************

	\brief Convert a 64 bit unsigned integer to text and append it

	Convert a 64 bit unsigned integer into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param uInput 64 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(Word64 uInput)
{
	NumberString Text(uInput);
	return Append(Text);
}

/*! ************************************

	\brief Convert a 32 bit signed integer to text and append it

	Convert a 32 bit signed integer into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param iInput 32 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(Int32 iInput)
{
	NumberString Text(iInput);
	return Append(Text);
}

/*! ************************************

	\brief Convert a 64 bit signed integer to text and append it

	Convert a 64 bit signed integer into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param iInput 64 bit value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(Int64 iInput)
{
	NumberString Text(iInput);
	return Append(Text);
}

/*! ************************************

	\brief Convert a 32 bit floating point number to text and append it

	Convert a 32 bit floating pointer number into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param fInput 32 bit floating point value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(float fInput)
{
	NumberString Text(fInput);
	return Append(Text);
}

/*! ************************************

	\brief Convert a 64 bit floating point number to text and append it

	Convert a 64 bit floating pointer number into ASCII and append
	the resulting string to the end of the stream.
	If the memory allocation fails, a non-zero error code will be returned.

	\param dInput 64 bit floating point value to append to the stream as ASCII
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendAscii(double dInput)
{
	NumberString Text(dInput);
	return Append(Text);
}

/*! ************************************

	\brief Insert a number of tabs into the data stream

	For some text files, tabs are needed for formatting. Insert
	a number of tabs into the stream.

	\param uTabCount 0 or more tabs to insert
	\return Error code with zero being no error, non-zero is out of memory

***************************************/

Word BURGER_API Burger::OutputMemoryStream::AppendTabs(Word uTabCount)
{
	Word uResult = FALSE;		// Assume no error
	if (uTabCount) {
		do {
			uResult = Append('\t');	// Insert the tabs
			if (uResult) {			// Abort on error
				break;
			}
		} while (--uTabCount);		// Count down
	}
	return uResult;
}

/*! ************************************

	\brief Compare data to the data in the stream

	Given a buffer of data, do a byte for byte compare with
	the data in the stream and return FALSE if the
	data matches and non-zero if the data does not.

	\param pInput Pointer to the buffer to match
	\param uLength Size of the buffer to match
	\return \ref FALSE if the data matches, \ref TRUE if not

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Compare(const void *pInput,WordPtr uLength) const
{
	// Assume failure
	Word uResult = TRUE;
	const Word8 *pData = m_pData;
	// Checking for empty?
	if (!uLength) {
		if (!pData) {
			// Match!
			uResult = FALSE;
		}
	} else {
		if (pData) {
			const Word8 *pNext;
			do {
				pNext = reinterpret_cast<Word8* const *>(pData+CHUNKSIZE)[0];
				// Get the number of valid bytes
				WordPtr uChunk = CHUNKSIZE;
				if (!pNext) {
					uChunk = m_uIndex;
				}
				// Are there less bytes to check than in the buffer?
				if (uLength<uChunk) {
					// Size mismatch!
					break;
				}
				// Compare this chunk
				if (MemoryCompare(pData,pInput,uChunk)) {
					// Data mismatch!
					break;
				}
				// Consume this input data
				pInput=static_cast<const Word8 *>(pInput)+uChunk;
				uLength-=uChunk;
				// No more data from input and stream?
				if (!uLength && !pNext) {
					// The data is a match!
					uResult = FALSE;
					break;
				}
				pData = pNext;
			} while (pNext);
		}
	}
	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Overwrite data in the stream

	Given a buffer of data, replace the data in the 
	stream and return zero if no error and non-zero
	if there is an attempted buffer overrun.

	\param pInput Pointer to the buffer to match
	\param uLength Size of the buffer to match
	\param uOffset Offset into the data to begin the overwrite
	\return Zero if there was no overflow, non-zero if so

***************************************/

Word BURGER_API Burger::OutputMemoryStream::Overwrite(const void *pInput,WordPtr uLength,WordPtr uOffset)
{
	// Assume success
	Word uResult = 0;
	// Checking for empty?
	if (uLength) {
		// Now assume failure
		uResult = 1;
		Word8 *pData = m_pData;
		if (pData) {
			Word8 *pNext;
			// Set the write mark
			WordPtr uMark = 0;
			do {
				pNext = reinterpret_cast<Word8**>(pData+CHUNKSIZE)[0];
				// Get the number of valid bytes in this chunk
				WordPtr uChunk = CHUNKSIZE;
				if (!pNext) {
					uChunk = m_uIndex;
				}
				// Is the offset beyond this chunk?

				WordPtr uEndMark = uMark + uChunk;
				if (uEndMark>uOffset) {
					// How many bytes to skip?
					WordPtr uSkip = uOffset-uMark;
					pData += uSkip;
					uChunk -= uSkip;
					if (uLength<uChunk) {
						uChunk = uLength;
					}

					// Overwrite this chunk
					MemoryCopy(pData,pInput,uChunk);

					// Consume this input data
					pInput=static_cast<const Word8 *>(pInput)+uChunk;
					uLength-=uChunk;
					if (!uLength) {
						uResult = 0;
						break;
					}
					uOffset = uEndMark;
				}
				// Update the data mark
				uMark = uEndMark;
				pData = pNext;
			} while (pNext);
		}
	}
	// Return the error code
	return uResult;
}