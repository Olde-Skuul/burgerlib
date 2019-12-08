/***************************************

	This is the Burgerlib C++ string
	class. I will not trigger exceptions
	on memory errors. However, I do take great
	care in making sure that the class
	structure is in a valid state at all times

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brstring.h"
#include "brutf8.h"
#include "brnumberto.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::String
	
	\brief UTF 8 bit string class
	
	This commonly used string class was designed for performance in mind. Each
	instance takes 64 bytes to contain data for the string and a pointer
	to allocated memory if the internal string buffer is too large. Since
	most strings rarely exceed 56 bytes in length, this class will be able to
	allocate and free strings with no calls to a memory manager. Only
	when the string exceeds the internal buffer will Burger::Alloc()
	and Burger::Free() be used.

	Functions exist to convert UTF16 and UTF32 data into UTF8 format,
	which this string class uses internally for data storage.

	\sa Burger::String16
	
***************************************/


/*! ************************************

	\fn Burger::String::String(void)

	\brief Creates an empty string

***************************************/

/*! ************************************

	\brief Initialize a Burger::String with a Burger::String
	\param rInput Reference to a Burger::String

***************************************/

Burger::String::String(const Burger::String &rInput) BURGER_NOEXCEPT
{
	WordPtr uInputLength = rInput.m_uLength;	// Get the source length
	char *pWork = m_Raw;
	const char *pInput = rInput.m_pData;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0;
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,uInputLength+1);	// Copy the string and the ending NULL
}

/*! ************************************

	\brief Initialize a Burger::String by using a subsection of a different Burger::String

	Given a starting (inclusive) and ending (exclusive) offset, grab the sub string
	and use it to create a new Burger::String

	\note If uEnd is less than or equal to uStart, the resulting string will be empty.

	\param rInput Burger::String to receive input from
	\param uStart Offset to the first character to read from
	\param uEnd Offset beyond the last character to read from

***************************************/

Burger::String::String(const Burger::String &rInput,WordPtr uStart,WordPtr uEnd) BURGER_NOEXCEPT
{
	WordPtr uInputLength = rInput.m_uLength;	// Get the source length
	if (uEnd>uInputLength) {					// Clamp the end of the string
		uEnd = uInputLength;					// Make sure it fits
	}
	const char *pInput = rInput.m_pData;
	if (uStart>=uEnd) {							// Valid range?
		uInputLength = 0;						// The result will be empty
	} else {
		uInputLength = uEnd-uStart;				// Length of the new string
		pInput += uStart;
	}
	char *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			uInputLength = 0;		// Don't copy anything
		}
	}
	// Since I can't assume the string being created ends with a zero, I'll
	// manually place one in the new string
	pWork[uInputLength] = 0;
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,uInputLength);	// Copy the string
}

/*! ************************************

	\brief Initialize with a "C" string

	Initialize the Burger::String with a copy of the passed string. The original string
	can be discarded after the call returns.

	\param pInput Pointer to a valid "C" string or \ref NULL to create an empty string

***************************************/

Burger::String::String(const char *pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	WordPtr uInputLength = StringLength(pInput);
	char *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,uInputLength+1);	// Copy the string
}

/*! ************************************

	\brief Initialize with a "C" string with padding

	Initialize the Burger::String with a copy of the passed string. The original string
	can be discarded after the call returns. 
	Allocate a buffer that can hold the initialization string + the uPadding number
	of bytes so the programmer can manually append data to the end of the string
	with Burger::StringCopy() or equivalent

	\param pInput Pointer to a valid "C" string or \ref NULL to create an empty string
	\param uPadding Number of bytes to extend the string buffer. The extra bytes are NOT initialized

***************************************/

Burger::String::String(const char *pInput,WordPtr uPadding) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	WordPtr uInputLength = StringLength(pInput);
	char *pWork = m_Raw;
	if ((uInputLength+uPadding)>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+uPadding+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,uInputLength+1);	// Copy the string
}

/*! ************************************

	\brief Initialize with a UTF16 string

	Convert the UTF16 encoded input string into a UFT8 encoded string
	and initialize this class with the UTF8 version. The input
	string can be discarded after this call.

	\param pInput A pointer to a valid UTF16 "C" string

***************************************/

Burger::String::String(const Word16 *pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString16;
	}
	WordPtr uInputLength = UTF8::FromUTF16(NULL,0,pInput);
	char *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString16;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	UTF8::FromUTF16(pWork,uInputLength+1,pInput);	// Copy the string
}

/*! ************************************

	\brief Initialize with a UTF32 string

	Convert the UTF32 encoded input string into a UFT8 encoded string
	and initialize this class with the UTF8 version. The input
	string can be discarded after this call.

	\param pInput A pointer to a valid UTF32 "C" string

***************************************/

Burger::String::String(const Word32 *pInput) BURGER_NOEXCEPT
{
	Word32 uTemp;
	if (!pInput) {
		uTemp = 0;
		pInput = &uTemp;
	}
	WordPtr uInputLength = UTF8::FromUTF32(NULL,0,pInput);
	char *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			uTemp = 0;
			pInput = &uTemp;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	UTF8::FromUTF32(pWork,uInputLength+1,pInput);	// Copy the string
}

/*! ************************************

	\brief Initialize a Burger::String by using a subsection of a different "C" string

	Copy a range of bytes from a "C" string. This will terminate
	early if a zero byte is found in the input stream.

	\param pInput A pointer to a valid "C" string
	\param uStart Offset to the first character to read from
	\param uEnd Offset beyond the last character to read from

***************************************/

Burger::String::String(const char *pInput,WordPtr uStart,WordPtr uEnd) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	WordPtr uInputLength = StringLength(pInput);	// Get the source length
	if (uEnd>uInputLength) {					// Clamp the end of the string
		uEnd = uInputLength;					// Make sure it fits
	}
	if (uStart>=uEnd) {							// Valid range?
		uInputLength = 0;						// The result will be empty
	} else {
		uInputLength = uEnd-uStart;				// Length of the new string
		pInput += uStart;
	}
	char *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			uInputLength = 0;		// Don't copy anything
		}
	}
	// Since I can't assume the string being created ends with a zero, I'll
	// manually place one in the new string
	pWork[uInputLength] = 0;
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,uInputLength);	// Copy the string
}

/*! ************************************

	\brief Initialize a Burger::String with a single character

	If the input character is zero, the resulting string will be empty

	\note It is NOT recommended to use "high ASCII" as input such as 
	values 128-255. They are considered prefix codes as per the UTF8 standard
	and will yield undefined results for code that's expecting a 
	valid UTF8 string

	\param cInput Character to convert to a single byte string

***************************************/

Burger::String::String(char cInput) BURGER_NOEXCEPT
{
	char *pWork = m_Raw;
	pWork[0] = cInput;		// Store the char in the string
	pWork[1] = 0;
	WordPtr uInputLength = 1;
	if (!cInput) {			// Valid string?
		uInputLength = 0;	// 1 char long
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
}

/*! ************************************

	\brief Initialize a Burger::String filled with a single character

	If the input character is zero, the resulting string will be empty

	\param cInput Character to fill the buffer with
	\param uFillSize Size, in bytes, of the resulting string

***************************************/

Burger::String::String(char cInput,WordPtr uFillSize) BURGER_NOEXCEPT
{
	// Fix a logic error. If the character is null, then the string is empty
	// by default. Kill the fill length

	if (!cInput) {
		uFillSize = 0;
	}
	char *pWork = m_Raw;
	if (uFillSize>=BUFFERSIZE) {	// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uFillSize+1));
		if (!pWork) {			// Oh oh...
			pWork = m_Raw;
			uFillSize = 0;		// Don't copy anything
		}
	}
	m_uLength = uFillSize;		// Save the new length
	m_pData = pWork;			// Set the pointer
	pWork[uFillSize] = 0;
	MemoryFill(pWork,static_cast<Word8>(cInput),uFillSize);
}

/*! ************************************

	\brief Initialize a Burger::String with two concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first

***************************************/

Burger::String::String(const char *pInput1,const char *pInput2) BURGER_NOEXCEPT
{
	if (!pInput1) {			// Remove all NULLs
		pInput1 = g_EmptyString;
	}
	if (!pInput2) {
		pInput2 = g_EmptyString;
	}
	WordPtr uLen1 = StringLength(pInput1);
	WordPtr uLen2 = StringLength(pInput2);
	WordPtr uTotal = uLen1+uLen2;		// Size of the finished string

	char *pWork = m_Raw;
	if (uTotal>=BUFFERSIZE) {	// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uTotal+1));
		if (!pWork) {			// Oh oh...
			pWork = m_Raw;
			uTotal = 0;			// Don't copy anything
			uLen1 = 0;
			uLen2 = 0;		
			pInput2 = g_EmptyString;	// Will copy the null character
		}
	}
	m_uLength = uTotal;		// Save the new length
	m_pData = pWork;		// Set the pointer
	MemoryCopy(pWork,pInput1,uLen1);	// Copy the string
	pWork+=uLen1;
	MemoryCopy(pWork,pInput2,uLen2+1);	// Copy the next string
}

/*! ************************************

	\brief Initialize a Burger::String with three concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first
	\param pInput3 String to append after the second

***************************************/

Burger::String::String(const char *pInput1,const char *pInput2,const char *pInput3) BURGER_NOEXCEPT
{
	if (!pInput1) {			// Remove all NULLs
		pInput1 = g_EmptyString;
	}
	if (!pInput2) {
		pInput2 = g_EmptyString;
	}
	if (!pInput3) {
		pInput3 = g_EmptyString;
	}
	WordPtr uLen1 = StringLength(pInput1);
	WordPtr uLen2 = StringLength(pInput2);
	WordPtr uLen3 = StringLength(pInput3);
	WordPtr uTotal = uLen1+uLen2+uLen3;		// Size of the finished string

	char *pWork = m_Raw;
	if (uTotal>=BUFFERSIZE) {	// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uTotal+1));
		if (!pWork) {			// Oh oh...
			pWork = m_Raw;
			uTotal = 0;			// Don't copy anything
			uLen1 = 0;
			uLen2 = 0;		
			uLen3 = 0;		
			pInput3 = g_EmptyString;	// Will copy the null character
		}
	}
	m_uLength = uTotal;		// Save the new length
	m_pData = pWork;		// Set the pointer
	MemoryCopy(pWork,pInput1,uLen1);	// Copy the string
	pWork+=uLen1;
	MemoryCopy(pWork,pInput2,uLen2);	// Copy the next string
	pWork+=uLen2;
	MemoryCopy(pWork,pInput3,uLen3+1);	// Copy the next string
}

/*! ************************************

	\brief Initialize a Burger::String with four concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first
	\param pInput3 String to append after the second
	\param pInput4 String to append after the third

***************************************/

Burger::String::String(const char *pInput1,const char *pInput2,const char *pInput3,const char *pInput4) BURGER_NOEXCEPT
{
	if (!pInput1) {			// Remove all NULLs
		pInput1 = g_EmptyString;
	}
	if (!pInput2) {
		pInput2 = g_EmptyString;
	}
	if (!pInput3) {
		pInput3 = g_EmptyString;
	}
	if (!pInput4) {
		pInput4 = g_EmptyString;
	}
	WordPtr uLen1 = StringLength(pInput1);
	WordPtr uLen2 = StringLength(pInput2);
	WordPtr uLen3 = StringLength(pInput3);
	WordPtr uLen4 = StringLength(pInput4);
	WordPtr uTotal = uLen1+uLen2+uLen3+uLen4;		// Size of the finished string

	char *pWork = m_Raw;
	if (uTotal>=BUFFERSIZE) {	// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uTotal+1));
		if (!pWork) {			// Oh oh...
			pWork = m_Raw;
			uTotal = 0;			// Don't copy anything
			uLen1 = 0;
			uLen2 = 0;		
			uLen3 = 0;		
			uLen4 = 0;		
			pInput4 = g_EmptyString;	// Will copy the null character
		}
	}
	m_uLength = uTotal;		// Save the new length
	m_pData = pWork;		// Set the pointer
	MemoryCopy(pWork,pInput1,uLen1);	// Copy the string
	pWork+=uLen1;
	MemoryCopy(pWork,pInput2,uLen2);	// Copy the next string
	pWork+=uLen2;
	MemoryCopy(pWork,pInput3,uLen3);	// Copy the next string
	pWork+=uLen3;
	MemoryCopy(pWork,pInput4,uLen4+1);	// Copy the next string
}

/*! ************************************

	\fn Burger::String::operator char *()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF8 ASCII string
	\return Pointer to the UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::operator const char *() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF8 ASCII string
	\return Pointer to the const UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::c_str()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF8 ASCII string
	\return Pointer to the UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::c_str() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF8 ASCII string
	\return Pointer to the const UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::operator char () const
	\brief Return the first character of the "C" string

	Helper to access the first character of the "C" string. It can be zero.

	\return The first character of the "C" string

***************************************/

/*! ************************************

	\fn Burger::String::length(void) const
	\brief Return the length of the "C" string

	\return The length in bytes of the UTF8 "C" string

***************************************/

/*! ************************************

	\fn Burger::String::GetLength(void) const
	\brief Return the length of the "C" string

	\return The length in bytes of the UTF8 "C" string

***************************************/

/*! ************************************

	\fn Burger::String::GetPtr()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF8 ASCII string
	\return Pointer to the UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::GetPtr() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF8 ASCII string
	\return Pointer to the const UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::~String()
	\brief Release memory, if any

	Releases any allocated memory for the string. Under most cases, this performs
	no operation.

***************************************/


/*! ************************************

	\brief Copy a "C" string to a Burger::String

	\note This function allows using the tail end of
	the current string as input

	\param pInput Pointer to a UTF8 "C" string. \ref NULL generates an empty string.
	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::Set(const char *pInput)
{
	// Assume no error
	eError uResult = kErrorNone;

	if (!pInput) {
		pInput = g_EmptyString;
	}
	char *pDest = m_Raw;
	WordPtr uInputLength = StringLength(pInput);		// Length of the new string
	if (uInputLength>=BUFFERSIZE) {		// Buffer big enough?
		pDest = static_cast<char *>(Alloc(uInputLength+1));
		if (!pDest) {					// Oh oh...
			pDest = m_Raw;
			uInputLength = 0;			// Don't copy anything
			pInput = g_EmptyString;		// Will copy the null character
			uResult = kErrorOutOfMemory;	// Error!
		}
	}
	char *pOld = m_pData;
	m_uLength = uInputLength;			// Save the new length
	m_pData = pDest;					// Set the pointer
	MemoryMove(pDest,pInput,uInputLength+1);	// Copy the string
	if (pOld!=m_Raw) {					// Discard previous memory
		Free(pOld);
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy an array of characters into a string

	The input data will be converted into a string. If the input data contains
	a zero, it will be truncated to that length. If the array does
	not contain a zero, one will be appended to the resulting string

	\param pInput Pointer to an array of UTF8 characters. \ref NULL generates an empty string.
	\param uLength Length of the input data
	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::Set(const char *pInput,WordPtr uLength)
{
	// Assume no error
	eError uResult = kErrorNone;

	// Failsafe
	if (!pInput) {
		uLength = 0;

	// Determine if the string needs to be truncated
	
	} else if (uLength) {
		const char *pMark = MemoryCharacter(pInput,uLength,0);
		if (pMark) {
			uLength = static_cast<WordPtr>(pMark-pInput);
		}
	}

	char *pDest = m_Raw;
	if (uLength>=BUFFERSIZE) {		// Buffer big enough?
		pDest = static_cast<char *>(Alloc(uLength+1));
		if (!pDest) {					// Oh oh...
			pDest = m_Raw;
			uLength = 0;				// Don't copy anything
			uResult = kErrorOutOfMemory;	// Error!
		}
	}
	char *pOld = m_pData;
	m_uLength = uLength;				// Save the new length
	m_pData = pDest;					// Set the pointer
	MemoryMove(pDest,pInput,uLength);	// Copy the string
	pDest[uLength] = 0;					// Force string termination
	if (pOld!=m_Raw) {					// Discard previous memory
		Free(pOld);
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 16 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF16 "C" string. \ref NULL generates an empty string.
	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::Set(const Word16 *pInput)
{
	// Assume no error
	eError uResult = kErrorNone;

	if (!pInput) {
		pInput = g_EmptyString16;
	}
	char *pDest = m_Raw;
	WordPtr uInputLength = UTF8::FromUTF16(NULL,0,pInput);		// Length of the new string
	if (uInputLength>=BUFFERSIZE) {		// Buffer big enough?
		pDest = static_cast<char *>(Alloc(uInputLength+1));
		if (!pDest) {					// Oh oh...
			pDest = m_Raw;
			uInputLength = 0;			// Don't copy anything
			pInput = g_EmptyString16;	// Will copy the null character
			uResult = kErrorOutOfMemory;	// Error!
		}
	}
	char *pOld = m_pData;
	m_uLength = uInputLength;			// Save the new length
	m_pData = pDest;					// Set the pointer
	UTF8::FromUTF16(pDest,uInputLength+1,pInput);		// Copy the string
	if (pOld!=m_Raw) {					// Discard previous memory
		Free(pOld);
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 16 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF16 "C" string. \ref NULL generates an empty string.
	\param uLength Length of the UTF16 string in characters (sizeof(buffer)/2)
	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::Set(const Word16 *pInput,WordPtr uLength)
{
	// Assume no error
	eError uResult = kErrorNone;

	if (!pInput) {
		pInput = g_EmptyString16;
	}
	char *pDest = m_Raw;
	WordPtr uInputLength = UTF8::FromUTF16(NULL,0,pInput,uLength<<1U);		// Length of the new string
	if (uInputLength>=BUFFERSIZE) {		// Buffer big enough?
		pDest = static_cast<char *>(Alloc(uInputLength+1));
		if (!pDest) {					// Oh oh...
			pDest = m_Raw;
			uInputLength = 0;			// Don't copy anything
			pInput = g_EmptyString16;	// Will copy the null character
			uResult = kErrorOutOfMemory;	// Error!
		}
	}
	char *pOld = m_pData;
	m_uLength = uInputLength;			// Save the new length
	m_pData = pDest;					// Set the pointer
	UTF8::FromUTF16(pDest,uInputLength+1,pInput,uLength<<1U);		// Copy the string
	if (pOld!=m_Raw) {					// Discard previous memory
		Free(pOld);
	}
	// Return error
	return uResult;
}



/*! ************************************

	\brief Force a buffer size

	Set the buffer to a specific size while retaining the existing string.
	If the preexisting string is too long for the new buffer, it will be
	truncated. The buffer size will be padded to reserve one byte for the terminating
	zero.

	\note If a buffer of 100 bytes is requested, 101 bytes will be allocated to hold
	a string up to 100 characters in length with the 101st byte being the terminating
	zero. The output of Burger::StringLength() is acceptable as input for a new string.

	\param uSize Number of bytes to set the buffer to
	\return Zero if no error, error code if the buffer couldn't be resized

***************************************/

Burger::eError BURGER_API Burger::String::SetBufferSize(WordPtr uSize) BURGER_NOEXCEPT
{
	// Assume no error
	eError uResult = kErrorNone;
	if (uSize!=m_uLength) {
		// If no space is requested, clear the buffer
		if (!uSize) {
			Clear();
		} else {
			// Hold the old buffer
			char *pWork = m_pData;
			// Get the new buffer
			char *pDest = m_Raw;
			// Allocate a new buffer if needed
			if (uSize>=BUFFERSIZE) {	// Buffer big enough?
				pDest = static_cast<char *>(Alloc(uSize+1));
				if (!pDest) {			// Oh oh...
					pDest = m_Raw;
					uSize = 0;			// Don't copy anything
					uResult = kErrorOutOfMemory;	// Out of memory error!
				}
			}
			// Get the size of the string
			WordPtr uDestSize = m_uLength;
			if (uDestSize>=uSize) {
				// Truncate the string
				uDestSize = uSize;
			}
			MemoryCopy(pDest,pWork,uDestSize);
			pDest[uDestSize] = 0;		// Ensure the buffer is zero terminated
			if (pWork!=m_Raw) {			// Discard previous memory
				Free(pWork);
			}
			m_pData = pDest;			// Set the pointer
			m_uLength = uSize;			// Save the new length
			pDest[uSize] = 0;			// Ensure the terminating zero is present
		}
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Assign a Burger::String

	Copy the contents of a Burger::String into this Burger::String

	\param rInput Reference to a Burger::String to copy from
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator =(const Burger::String &rInput)
{
	if (this!=&rInput) {		// Am I copying myself?
		char *pWork = m_pData;
		if (pWork!=m_Raw) {		// Discard previous memory
			Free(pWork);
			pWork=m_Raw;
		}

		WordPtr uInputLength = rInput.m_uLength;	// Length of the new string
		const char *pInput = rInput.m_pData;		// Copy the new length
		if (uInputLength>=BUFFERSIZE) {		// Buffer big enough?
			pWork = static_cast<char *>(Alloc(uInputLength+1));
			if (!pWork) {					// Oh oh...
				pWork = m_Raw;
				uInputLength = 0;			// Don't copy anything
				pInput = g_EmptyString;		// Will copy the null character
			}
		}
		m_uLength = uInputLength;			// Save the new length
		m_pData = pWork;					// Set the pointer
		MemoryCopy(pWork,pInput,uInputLength+1);	// Copy the string
	}
	return *this;
}

/*! ************************************

	\brief Assign a "C" string to a Burger::String

	Copy the contents of a UTF8 "C" string into this Burger::String

	\param pInput Pointer to a UTF8 "C" string. \ref NULL generates an empty string.
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator =(const char *pInput)
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	char *pWork = m_pData;
	if (pWork!=m_Raw) {		// Discard previous memory
		Free(pWork);
		pWork = m_Raw;
	}
	WordPtr uInputLength = StringLength(pInput);		// Length of the new string
	if (uInputLength>=BUFFERSIZE) {		// Buffer big enough?
		pWork = static_cast<char *>(Alloc(uInputLength+1));
		if (!pWork) {					// Oh oh...
			pWork = m_Raw;
			uInputLength = 0;			// Don't copy anything
			pInput = g_EmptyString;		// Will copy the null character
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryMove(pWork,pInput,uInputLength+1);	// Copy the string
	return *this;
}

/*! ************************************

	\brief Assign a single character length string to this Burger::String

	If the input is zero, the string is empty. 

	\note Since the default encoding is UFT8, input that's greater than
	127 will yield a possibly invalid string due to UTF8 decoding.

	\param cInput Single low ASCII character to create a string.
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator =(char cInput)
{
	char *pWork = m_pData;
	if (pWork!=m_Raw) {		// Discard previous memory
		Free(pWork);
		pWork = m_Raw;
	}

	pWork[0] = cInput;		// Store the char in the string
	pWork[1] = 0;
	WordPtr uInputLength = 1;
	if (!cInput) {			// Valid string?
		uInputLength = 0;	// 1 char long
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	return *this;
}

/*! ************************************

	\brief Append a Burger::String to the current string.

	Increase the buffer if needed and append the input string to the existing string.
	If the input string is empty, no operation is performed.

	\param rInput Reference to string to append
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator +=(const Burger::String &rInput)
{
	WordPtr uInputLen2 = rInput.m_uLength;
	if (uInputLen2) {							// Do I bother?
		const char *pInput = rInput.m_pData;
		WordPtr uInputLen1 = m_uLength;			// Get the lengths of the two strings
		WordPtr uTotal = uInputLen1+uInputLen2;	// New string size
		char *pWork = m_Raw;
		if (uTotal>=BUFFERSIZE) {				// Buffer big enough?
			pWork = static_cast<char *>(Alloc(uTotal+1));
			if (!pWork) {						// Oh oh...
				pWork = m_Raw;
				pInput = g_EmptyString;
				uInputLen1 = 0;
				uInputLen2 = 0;
				uTotal = 0;						// Don't copy anything
			}
			// Copy the local string into the new buffer
			const char *pAppendTo = m_pData;
			MemoryCopy(pWork,pAppendTo,uInputLen1);
			// Dispose of the old string if needed
			if (pAppendTo!=m_Raw) {
				Free(pAppendTo);
			}
		}
		m_uLength = uTotal;			// Save the new length
		m_pData = pWork;			// Set the pointer

		// If the output buffer is local, it is logical to
		// assume that the original buffer was local.
		// Perform a simple concatenation and be done with it
		MemoryCopy(pWork+uInputLen1,pInput,uInputLen2+1);
	}
	return *this;
}

/*! ************************************

	\brief Append a UTF8 "C" string to the current string.

	Increase the buffer if needed and append the input string to the existing string.
	If the input string is empty, no operation is performed.

	\param pInput Pointer to a UTF8 "C" string to append. \ref NULL performs no operation
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator +=(const char *pInput)
{
	if (pInput) {
		WordPtr uInputLen2 = StringLength(pInput);
		if (uInputLen2) {							// Do I bother?
			WordPtr uInputLen1 = m_uLength;			// Get the lengths of the two strings
			WordPtr uTotal = uInputLen1+uInputLen2;	// New string size
			char *pWork = m_Raw;
			if (uTotal>=BUFFERSIZE) {				// Buffer big enough?
				pWork = static_cast<char *>(Alloc(uTotal+1));
				if (!pWork) {						// Oh oh...
					pWork = m_Raw;
					pInput = g_EmptyString;
					uInputLen1 = 0;
					uInputLen2 = 0;
					uTotal = 0;						// Don't copy anything
				}
				// Copy the local string into the new buffer
				const char *pAppendTo = m_pData;
				MemoryCopy(pWork,pAppendTo,uInputLen1);
				// Dispose of the old string if needed
				if (pAppendTo!=m_Raw) {
					Free(pAppendTo);
				}
			}
			m_uLength = uTotal;			// Save the new length
			m_pData = pWork;			// Set the pointer

			// If the output buffer is local, it is logical to
			// assume that the original buffer was local.
			// Perform a simple concatenation and be done with it
			MemoryCopy(pWork+uInputLen1,pInput,uInputLen2+1);
		}
	}
	return *this;
}

/*! ************************************

	\brief Append a single character to the current string.

	Increase the buffer if needed and append the input character to the existing string.
	If the input character is zero, no operation is performed.

	\param cInput Character to append, zero does nothing.
	\return A reference to the this pointer

***************************************/

Burger::String & Burger::String::operator +=(char cInput)
{
	// Do I bother?
	if (cInput) {
		WordPtr uInputLen1 = m_uLength;		// Get the lengths of the two strings
		WordPtr uTotal = uInputLen1+1;		// New string size
		char *pWork = m_Raw;
		if (uTotal>=BUFFERSIZE) {				// Buffer big enough?
			pWork = static_cast<char *>(Alloc(uTotal+1));
			if (!pWork) {						// Oh oh...
				pWork = m_Raw;
				cInput = 0;
				uInputLen1 = 0;
				uTotal = 0;						// Don't copy anything
			}
			const char *pAppendTo = m_pData;
			// Copy the local string into the new buffer
			MemoryCopy(pWork,pAppendTo,uInputLen1);
			// Dispose of the old string if needed
			if (pAppendTo!=m_Raw) {
				Free(pAppendTo);
			}
		}
		m_uLength = uTotal;			// Save the new length
		m_pData = pWork;			// Set the pointer
		// If the output buffer is local, it is logical to
		// assume that the original buffer was local.
		// Perform a simple concatenation and be done with it
		pWork[uInputLen1] = cInput;
		pWork[uTotal] = 0;
	}
	return *this;
}

/*! ************************************

	\fn Burger::String::operator () (WordPtr uStart,WordPtr uEnd) const
	\brief Create a new Burger::String from a substring

	Given the start and end offsets of a string, create a new string with those
	offsets and return the resulting string

	\param uStart Offset to the starting character of the new string
	\param uEnd Offset to the character AFTER the last character of the new string
	\return A class instance of Burger::String containing the new string.

	\sa Burger::String::String(const Burger::String &,WordPtr,WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String::operator () (WordPtr uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String::operator [] (WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String::operator () (WordPtr uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String::operator [] (WordPtr) const

***************************************/

/*! ************************************

	\fn Burger::String::operator [] (WordPtr uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String::operator () (WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String::operator [] (WordPtr uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String::operator () (WordPtr) const

***************************************/

/*! ************************************

	\brief Return a single character from the string
	
	If the index is beyond the end if the string, the
	function will return a zero.

	\param uWhere Index to fetch a character from
	\return Character found in the string or zero if the index is beyond the end of the string.

***************************************/

char BURGER_API Burger::String::Get(WordPtr uWhere) const
{
	if (uWhere<m_uLength) {			// In bounds?
		return m_pData[uWhere];		// Fetch it
	}
	return 0;						// Return zilch
}

/*! ************************************

	\brief Modify a single character in the string.

	\note This function will NOT increase the size of the
	string.
	\param uWhere Offset into the string to write to.
	\param cInput Character to update the string with.

***************************************/

void BURGER_API Burger::String::Put(WordPtr uWhere,char cInput)
{
	if (uWhere<m_uLength) {			// In bounds?
		m_pData[uWhere] = cInput;	// Store it
	}
}

/*! ************************************

	\brief Scan for a sub string

	\note This function is case sensitive

	\param pInput Pointer to "C" string to scan for.
	\return Pointer to the string found or NULL if not found

***************************************/

char * BURGER_API Burger::String::StringString(const char *pInput) const
{
	// String to scan for
	if (pInput) {
		return Burger::StringString(m_pData,pInput);
	}
	return NULL;
}

/*! ************************************

	\fn Burger::String::ToLowercase(void)
	\brief Convert the string to lower case

	Convert all ASCII uppercase characters to lowercase.

	\return Reference to the string class

***************************************/

/*! ************************************

	\fn Burger::String::ToUppercase(void)
	\brief Convert the string to upper case

	Convert all ASCII lowercase characters to uppercase.

	\return Reference to the string class

***************************************/

/*! ************************************

	\fn Burger::String::Lowercase(void) const
	\brief Return a lowercase copy of the string

	Make a copy of the string with all of the ASCII uppercase characters converted to lowercase.

	\return New instance of the string class in lower case

***************************************/

/*! ************************************

	\fn Burger::String::Uppercase(void) const
	\brief Return an uppercase copy of the string

	Make a copy of the string with all of the ASCII lowercase characters converted to uppercase.

	\return New instance of the string class in upper case

***************************************/

/*! ************************************

	\fn Burger::String::operator ! () const
	\brief Return \ref TRUE if the string is empty

	\return \ref TRUE on an empty string, \ref FALSE if there are characters in the string

***************************************/

/*! ************************************

	\fn Burger::String::IsValid(void) const
	\brief Return \ref TRUE if the string has characters

	\return \ref FALSE on an empty string, \ref TRUE if there are characters in the string

***************************************/

/*! ************************************

	\brief Clear out the data

	Set the string to an empty string

***************************************/

void BURGER_API Burger::String::Clear(void) BURGER_NOEXCEPT
{
	char *pWork = m_pData;		// Old data pointer
	m_pData = m_Raw;			// New pointer
	m_uLength = 0;				// No length
	m_Raw[0] = 0;				// Kill the string
	if (pWork!=m_Raw) {			// Dispose of the data
		Free(pWork);			// Kill the old memory
	}
}

/*! ************************************

	\brief Extract the string into a buffer of a specific size
	
	This will truncate the string if the buffer is too small

	\param pOutput Pointer to a buffer to receive the "C" string
	\param uOutputSize Size in bytes of the output buffer
	\return Length of the string in bytes without the terminating zero

	\sa Burger::String::PCopy(Word8 *,WordPtr) const

***************************************/

WordPtr BURGER_API Burger::String::Copy(char *pOutput,WordPtr uOutputSize) const
{
	if (uOutputSize) {					// Is there an output buffer?
		--uOutputSize;					// Max size of the "C" string and the ending null
		WordPtr uLength = m_uLength;	// Get the size of the current string
		if (uOutputSize>uLength) {		// Set the maximum
			uOutputSize = uLength;		// Truncate
		}
		pOutput[uOutputSize] = 0;		// Zero terminate
		MemoryCopy(pOutput,m_pData,uOutputSize);	// Copy the string
	}
	return uOutputSize;
}

/*! ************************************

	\brief Extract the string into a Pascal buffer of a specific size
	
	This will truncate the string if the buffer is too small

	\note Pascal strings have a hard limit of 256 byte (255 characters). If the
	buffer exceeds this length, it will be truncated

	\note The resulting string will NOT have a terminating zero, so it must
	manually be converted into a "C". It is not a dual "C" string and Pascal string.

	\param pOutput Buffer of up a maximum of 256 bytes in length
	\param uOutputSize Size of the buffer in bytes. If the value is greater than 256 it will be truncated to 256.
	\sa Burger::String::Copy(char *,WordPtr) const

***************************************/

WordPtr BURGER_API Burger::String::PCopy(Word8 *pOutput,WordPtr uOutputSize) const
{
	if (uOutputSize) {				// Is there an output buffer?
		if (uOutputSize>256) {		// Pascal strings maximum length
			uOutputSize = 256;
		}
		--uOutputSize;					// Max size of the "C" string
		WordPtr uLength = m_uLength;	// Get the size of the current string
		if (uOutputSize>uLength) {		// Set the maximum
			uOutputSize = uLength;		// Truncate
		}
		pOutput[0] = static_cast<Word8>(uOutputSize);		// Pascal length
		MemoryCopy(pOutput+1,m_pData,uOutputSize);	// Copy the string
	}
	return uOutputSize;
}

/*! ************************************

	\brief Remove a part of the string.

	This routine will not resize the buffer

	\param uStart Starting index into the string for the section to remove
	\param uLength Number of bytes to remove from the string
	\return Number of bytes of remaining string but not including the terminating zero

***************************************/

WordPtr BURGER_API Burger::String::Remove(WordPtr uStart,WordPtr uLength)
{
	WordPtr uMyLength = m_uLength;		// Get the string's length
	if (uStart < uMyLength) {			// Start off the end?
		char *pWork = &m_pData[uStart];	// Start pointer
		// Real maximum number of bytes I can remove
		WordPtr uRemainingLength = uMyLength - uStart;
		if (uLength >= uRemainingLength) {	// Truncation?
			pWork[0] = 0;					// End the string here
			uMyLength = uStart;				// Return this as the new length
			m_uLength = uStart;				// Save the new length
		} else {
			if (uLength) {					// Am I removing anything?
				uMyLength -= uLength;		// Adjust the length
				m_uLength = uMyLength;
				// +1 for the zero terminator
				uRemainingLength = (uRemainingLength-uLength)+1;		
				// I have the loop here so I'm not calling a subroutine
				do {							
					pWork[0] = pWork[uLength];	// Copy the string
					++pWork;					// Next byte
				} while (--uRemainingLength);	// All done?
			}
		}
	}
	return uMyLength;					// Return the new length
}

/*! ************************************

	\brief Insert a string into a current String

	If uStart is beyond the end of the string, it's
	placed at the end of the string.
	If Text points to a "C" string that is smaller than
	MaxLen, then only the length of the "C" string is used

	\param uStart Index to insert the new characters
	\param pInput Pointer to the string to insert
	\param uLength Number of bytes to insert
	\return Number of bytes of remaining string but not including the terminating zero

***************************************/

WordPtr BURGER_API Burger::String::Insert(WordPtr uStart,const char *pInput,WordPtr uLength)
{
	WordPtr uMyLength = m_uLength;	// Current string size
	if (uStart>uMyLength) {			// Is the start at the END of the string?
		uStart = uMyLength;			// Force appending
	}

	WordPtr uInputLength = StringLength(pInput);	// Maximum insertion
	if (uLength>uInputLength) {
		uLength = uInputLength;		// Duh! Don't insert more that I have
	}

	if (uLength) {					// Am I inserting anything?
		uInputLength = uMyLength+uLength;	// New string length
		char *pWork = m_pData;		// Get the source pointer
		char *pDest;				// Pointer to the new string
		if (uInputLength<BUFFERSIZE) {	// Still in the main buffer?
			pDest = m_Raw;			// Use the raw buffer
		} else {
			// Get a copy of the string
			pDest = static_cast<char *>(Alloc(uInputLength+1));
			if (!pDest) {			// Oh my god!!!
				return uMyLength;			// I'm not touching the string!
			}
			MemoryCopy(pDest,pWork,uMyLength);
		}
		m_uLength = uInputLength;	// Save the new length
		m_pData = pDest;			// Save the new pointer

		if (pDest==pWork) {			// Same buffer?
			pDest = pWork+uStart;	// Start here...
			MemoryMove(pDest+uLength,pDest,(uMyLength-uStart)+1);	// Make room in the center
			MemoryCopy(pDest,pInput,uLength);	// Copy in the string
		} else {
			MemoryCopy(pDest,pWork,uStart);		// Copy the first part
			pDest+=uStart;
			MemoryCopy(pDest,pInput,uLength);	// Insert my string
			pDest+=uLength;
			// Copy the end and the zero
			MemoryCopy(pDest,pWork+uStart,(uMyLength-uStart)+1);
		}
		// Do I dispose of the old one?
		if (pWork!=m_Raw) {
			Free(pWork);
		}
		uMyLength = m_uLength;	// Save the new length
	}
	return uMyLength;			// Return the new string length
}

/*! ************************************

	\brief Append a UTF8 string buffer to the current string.

	Increase the buffer if needed and append the input string to the existing string.
	If the input string is empty, no operation is performed.

	\param pInput Pointer to a UTF8 string to append. \ref NULL performs no operation
	\param uInputSize Number of bytes to append
	\return Number of bytes of the resulting string but not including the terminating zero

***************************************/

WordPtr BURGER_API Burger::String::Append(const char *pInput,WordPtr uInputSize)
{
	WordPtr uTotal = m_uLength;
	// Do I bother?
	if (pInput && uInputSize) {
		WordPtr uInputLen1 = uTotal;			// Get the lengths of the two strings
		uTotal = uTotal+uInputSize;	// New string size
		char *pWork = m_Raw;
		if (uTotal>=BUFFERSIZE) {				// Buffer big enough?
			pWork = static_cast<char *>(Alloc(uTotal+1));
			if (!pWork) {						// Oh oh...
				pWork = m_Raw;
				pInput = g_EmptyString;
				uInputLen1 = 0;
				uInputSize = 0;
				uTotal = 0;						// Don't copy anything
			}
			// Copy the local string into the new buffer
			const char *pAppendTo = m_pData;
			MemoryCopy(pWork,pAppendTo,uInputLen1);
			// Dispose of the old string if needed
			if (pAppendTo!=m_Raw) {
				Free(pAppendTo);
			}
		}
		m_uLength = uTotal;			// Save the new length
		m_pData = pWork;			// Set the pointer
		// If the output buffer is local, it is logical to
		// assume that the original buffer was local.
		// Perform a simple concatenation and be done with it
		MemoryCopy(pWork+uInputLen1,pInput,uInputSize);
		// Zero terminate
		pWork[uInputLen1+uInputSize] = 0;
	}
	return uTotal;
}

/*! ************************************

	\brief Resize a string and either truncate or append with a fill character

	Force the string to be a specific size. If the string
	grows, fill in the extra space with the pad character.
	Retain the preexisting string from index zero to index uNewLength

	\param uNewLength Number of bytes the string will occupy
	\param cPad character to fill the buffer with in case of growth
	\return Reference to the Burger::String

***************************************/

Burger::String & BURGER_API Burger::String::Left(WordPtr uNewLength,char cPad)
{
	WordPtr uOldLen = m_uLength;	// Get the current length
	m_uLength = uNewLength;			// Set the new length
	char *pWork = m_pData;			// Buffer pointer
	if (uNewLength > uOldLen) {		// Did it grow?
		char *pDest;
		if (uNewLength<BUFFERSIZE) {
			pDest = m_Raw;			// Used the internal buffer
		} else {
			// Get a new string
			pDest = static_cast<char *>(Alloc(uNewLength+1));
			if (!pDest) {
				m_uLength = 0;		// No memory!!!
				m_Raw[0] = 0;
				m_pData = m_Raw;
				return *this;
			}
			MemoryCopy(pDest,pWork,uOldLen);
		}
		m_pData = pDest;		// Save the new buffer pointer
		pDest[uNewLength] = 0;	// End it
		// Fill in the extra 
		MemoryFill(pDest + uOldLen, static_cast<Word8>(cPad), uNewLength - uOldLen);
		// Get rid of the old string?
		if (pWork!=m_Raw) {	
			// Bye bye
			Free(pWork);
		}
	} else {
		// Set the end character for shrinkage
		pWork[uNewLength] = 0;
	}
	return *this;
}

/*! ************************************

	\brief Resize a string and either truncate or prepend with a fill character

	Force the string to be a specific size. If the string
	grows, fill in the extra space with the pad character.
	Note: I am padding the string from the "left" side. I.E.
	the string is moved to the right.
	
	\param uNewLength Number of bytes the string will occupy
	\param cPad character to fill the buffer with in case of growth
	\return Reference to the Burger::String

***************************************/

Burger::String & BURGER_API Burger::String::Right(WordPtr uNewLength,char cPad)
{
	WordPtr uOldLen = m_uLength;	// Get the current length
	m_uLength = uNewLength;			// Set the new length
	char *pWork = m_pData;			// Buffer pointer
	if (uNewLength > uOldLen) {		// Did it grow?
		char *pDest;
		if (uNewLength<BUFFERSIZE) {
			pDest = m_Raw;			// Used the internal buffer
		} else {
			pDest = static_cast<char *>(Alloc(uNewLength+1));
			if (!pDest) {
				m_uLength = 0;		// No memory!!!
				m_Raw[0] = 0;
				m_pData = m_Raw;
				return *this;
			}
			// Copy the old string
			MemoryCopy(pDest,pWork,uOldLen);
		}
		m_pData = pDest;		// Save the new buffer pointer
		// Copy over the characters from the right
		MemoryMove(pDest+uNewLength-uOldLen,pDest,uOldLen+1);
		// Fill in the extra
		MemoryFill(pDest, static_cast<Word8>(cPad), uNewLength - uOldLen);
		// Get rid of the old string?
		if (pWork!=m_Raw) {
			// Bye bye
			Free(pWork);
		}
	} else {
		// Copy over the characters from the right
		MemoryMove(pWork,pWork+uOldLen-uNewLength,uNewLength+1);
	}
	return *this;
}

/*! ************************************

	\fn BURGER_INLINE int Burger::String::Compare(const Burger::String &rInput) const
	\brief Compare the string

	Compare two strings for equality using the rules found in the
	ANSI function strcmp(). This is a functional equivalent

	\param rInput Reference to a Burger::String to compare against.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

/*! ************************************

	\brief Compare the string

	Compare two strings for equality using the rules found in the
	ANSI function strcmp(). This is a functional equivalent

	\param pInput Pointer to a "C" string to compare against. \ref NULL will perform as if an empty string was passed in.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

int BURGER_API Burger::String::Compare(const char *pInput) const
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	return StringCompare(m_pData,pInput);
}

/*! ************************************

	\brief Compare the string to a single character

	Compare two strings for equality using the rules found in the
	ANSI function strcmp(). This is a functional equivalent

	\param cInput Character to convert to a string of a single character in length. Zero will perform as if an empty string was passed in.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

int BURGER_API Burger::String::Compare(char cInput) const
{
	char Temp[2];
	Temp[0] = cInput;
	Temp[1] = 0;
	return StringCompare(m_pData,Temp);
}

/*! ************************************

	\brief Remove each and every char in the list

	Given a string containing characters, remove the characters found from the current string.
	Duplicating characters in the input string will reduce performance slightly. The order of
	the characters is not important.

	\param pInput Pointer to a "C" string containing the characters to remove. \ref NULL performs no action
	\return Number of bytes of remaining string but not including the terminating zero
	\sa RemoveChar(char)

***************************************/

WordPtr BURGER_API Burger::String::RemoveChars(const char *pInput)
{
	if (pInput) {
		Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0];
		if (uTemp) {
			do {
				++pInput;				// Accept the char
				RemoveChar(static_cast<char>(uTemp));	// Remove this char 
				// Next char in the list
				uTemp = reinterpret_cast<const Word8 *>(pInput)[0];		
			} while (uTemp);				// Not the end of the list?
		}
	}
	return m_uLength;		// Return the new length
}

/*! ************************************

	\brief Remove a specific character from the string

	Find all occurrences of the character and remove them. The string will be 
	reduced in size if necessary.

	\param cInput Character to remove from the string. Zero will perform no action
	\return Number of bytes of remaining string but not including the terminating zero
	\sa RemoveChars(const char *)

***************************************/

WordPtr BURGER_API Burger::String::RemoveChar(char cInput)
{
	WordPtr uLength = m_uLength;
	// Should I bother?
	if (cInput && uLength) {
		// Get the pointer
		Word8 *pWork = reinterpret_cast<Word8 *>(m_pData);
		Word8 *pDest = pWork;
		do {
			Word uTemp = pWork[0];					// Get the char
			++pWork;								// Accept it
			if (uTemp!=static_cast<Word8>(cInput)) {
				pDest[0] = static_cast<Word8>(uTemp);	// Keep this
				++pDest;
			}
		} while (--uLength);
		pDest[0] = 0;								// Zero terminate
		// Set the new length
		uLength = static_cast<WordPtr>(pDest-reinterpret_cast<Word8 *>(m_pData));
		// Save the new length if needed
		m_uLength = uLength;
	}
	// Return the new length
	return uLength;
}

/*! ************************************

	\brief Normalize Mac and PC line feeds to Linux/Unix format

	Find all occurrences of '\\r' and '\\r\\n' and replace them with a single
	'\\n' character. The string will be reduced in size if necessary.

	\return Number of bytes of remaining string but not including the terminating zero
	\sa RemoveChar(char)

***************************************/

WordPtr BURGER_API Burger::String::NormalizeLineFeeds(void)
{
	WordPtr uLength = m_uLength;
	// Should I bother?
	if (uLength) {
		// Get the pointer
		Word8 *pWork = reinterpret_cast<Word8 *>(m_pData);
		Word8 *pDest = pWork;
		do {
			Word uTemp = pWork[0];					// Get the char
			++pWork;								// Accept it
			if (uTemp=='\r') {						// CR?
				// Is this a CR/LF case?
				if ((uLength>=2) && (pWork[0]=='\n')) {
					++pWork;		// Remove the 2nd characters
					--uLength;
				}
				uTemp = '\n';		// Force a LF into the output
			}
			pDest[0] = static_cast<Word8>(uTemp);	// Keep this
			++pDest;
		} while (--uLength);
		pDest[0] = 0;								// Zero terminate
		// Set the new length
		uLength = static_cast<WordPtr>(pDest-reinterpret_cast<Word8 *>(m_pData));
		// Save the new length if needed
		m_uLength = uLength;
	}
	// Return the new length
	return uLength;
}

/*! ************************************

	\fn BURGER_INLINE Word Burger::String::GetBoolean(Word bDefault) const
	\brief Convert an ASCII string into a boolean.
	
	Convert the string to a \ref TRUE or a \ref FALSE. If the input is
	\ref NULL or invalid, return the default value.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are considered
	\ref FALSE. The comparison is case insensitive.
	
	\param bDefault Default boolean to return in the event of a parsing error.
		
	\return \ref TRUE or \ref FALSE
	
	\sa AsciiToBoolean(const char *,const char **) or AsciiToBoolean(const char *,Word)

***************************************/

/*! ************************************

	\brief Set the string to "true" or "false"

	If the input value is zero, set the string to "false",
	otherwise set the string to "true".

	\param bInput Value to store as a string
	\sa GetBoolean(Word) const or SetYesNo(Word)

***************************************/

void BURGER_API Burger::String::SetTrueFalse(Word bInput)
{
	const char *pSource;
	WordPtr uLength;
	// Choose the string
	if (bInput) {
		pSource = g_TrueString;
		uLength = sizeof(g_TrueString)-1;
	} else {
		pSource = g_FalseString;
		uLength = sizeof(g_FalseString)-1;
	}
	SetFast(pSource,uLength);
}

/*! ************************************

	\brief Set the string to "true" or "false"

	If the input value is zero, set the string to "false",
	otherwise set the string to "true".

	\param bInput Value to store as a string
	\sa GetBoolean(Word) const or SetTrueFalse(Word)

***************************************/

void BURGER_API Burger::String::SetYesNo(Word bInput)
{
	const char *pSource;
	WordPtr uLength;
	// Choose the string
	if (bInput) {
		pSource = g_YesString;
		uLength = sizeof(g_YesString)-1;
	} else {
		pSource = g_NoString;
		uLength = sizeof(g_NoString)-1;
	}
	SetFast(pSource,uLength);
}

/*! ************************************

	\fn BURGER_INLINE Word Burger::String::GetWord(Word uDefault,Word uMin,Word uMax) const
	\brief Return an unsigned integer value

	Scan the value string as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa GetInt(Int,Int,Int) const, SetWord(Word), SetWordHex(Word), or AsciiToWord(const char *,Word,Word,Word)

***************************************/

/*! ************************************

	\brief Set an unsigned integer value

	Convert the input into an unsigned integer UTF-8 string 
	and set the value to this string

	\param uInput Value to store as an unsigned integer string
	\sa GetWord(Word,Word,Word) const or SetWordHex(Word)

***************************************/

void BURGER_API Burger::String::SetWord(Word uInput)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	m_uLength = static_cast<WordPtr>(NumberToAscii(m_Raw,static_cast<Word32>(uInput))-m_Raw);
}

/*! ************************************

	\brief Set an unsigned integer value as hex

	Convert the input into a "C" style hex
	string in the format of 0x12345678 and
	store this string as the value

	\param uValue Value to store as an unsigned integer hex string
	\sa GetWord(Word,Word,Word) const or SetWord(Word)

***************************************/

void BURGER_API Burger::String::SetWordHex(Word uValue)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	m_Raw[0] = '0';
	m_Raw[1] = 'x';
	m_uLength = static_cast<WordPtr>(NumberToAsciiHex(m_Raw+2,static_cast<Word32>(uValue),0)-m_Raw);
}

/*! ************************************

	\fn BURGER_INLINE Int Burger::String::GetInt(Int iDefault,Int iMin,Int iMax) const
	\brief Return a signed integer value

	Scan the value string as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa GetWord(Word,Word,Word) const, SetInt(Int) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

/*! ************************************

	\brief Set a signed integer value

	Convert the input into an signed integer UTF-8 string 
	and set the value to this string

	\param iInput Value to store as a signed integer string
	\sa GetInt(Int,Int,Int) const or SetWord(Word)

***************************************/

void BURGER_API Burger::String::SetInt(Int iInput)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	m_uLength = static_cast<WordPtr>(NumberToAscii(m_Raw,static_cast<Int32>(iInput))-m_Raw);
}

/*! ************************************

	\fn BURGER_INLINE float Burger::String::GetFloat(float fDefault) const
	\brief Return a floating point value

	Scan the value string as a 32 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param fDefault Value to return on error
	\return Value or fDefault
	\sa GetDouble(double) const, SetFloat(float) or AsciiToFloat(const char *,float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::String::GetFloat(float fDefault,float fMin,float fMax) const
	\brief Return a floating point value

	Scan the value string as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa GetDouble(double,double,double) const, SetFloat(float) or AsciiToFloat(const char *,float,float,float)

***************************************/


/*! ************************************

	\brief Set a 32 bit floating point value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param fValue Value to store as a floating point string
	\sa GetFloat(float,float,float) const or SetDouble(double)

***************************************/

void BURGER_API Burger::String::SetFloat(float fValue)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	m_uLength = static_cast<WordPtr>(NumberToAscii(m_Raw,fValue)-m_Raw);
}

/*! ************************************

	\fn BURGER_INLINE double Burger::String::GetDouble(double dDefault) const
	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param dDefault Value to return on error
	\return Value or dDefault
	\sa GetFloat(float) const, SetDouble(double) or AsciiToDouble(const char *,double)

***************************************/

/*! ************************************

	\fn BURGER_INLINE double Burger::String::GetDouble(double dDefault,double dMin,double dMax) const
	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa GetFloat(float,float,float) const, SetDouble(double) or AsciiToDouble(const char *,double,double,double)

***************************************/


/*! ************************************

	\brief Set a 64 bit floating point value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param dValue Value to store as a 64 bit floating point string
	\sa GetDouble(double,double,double) const or SetFloat(float)

***************************************/

void BURGER_API Burger::String::SetDouble(double dValue)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	m_uLength = static_cast<WordPtr>(NumberToAscii(m_Raw,dValue)-m_Raw);
}

/*! ************************************

	\brief Copy a "C" string to a String with shortcuts

	If the string is under BUFFERSIZE-1 bytes in length AND it's not
	using the a buffer where a call to MemoryCopy() would fail, then
	call this function to update the string since it performs
	some optimizations that cannot be done if either case
	of the input data were true.

	\note Due to optimizations, some bounds checking is not performed. Do not call
		this function if the input string could be larger than BUFFERSIZE-1 bytes in length!!!
	\param pInput Pointer to a UTF8 "C" string that's less than BUFFERSIZE in length. \ref NULL will page fault.

***************************************/

void BURGER_API Burger::String::SetFast(const char *pInput)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	// Copy the new string
	WordPtr uLength = StringLength(pInput);
	m_uLength = uLength;
	m_Raw[uLength] = 0;
	MemoryCopy(m_Raw,pInput,uLength);
}

/*! ************************************

	\brief Copy an array of characters to a String with shortcuts

	If the string is under BUFFERSIZE-1 bytes in length AND it's not
	using the a buffer where a call to MemoryCopy() would fail, then
	call this function to update the string since it performs
	some optimizations that cannot be done if either case
	of the input data were true. A zero will be appended to the final
	string. The input string should not contain a zero.

	\note Due to optimizations, some bounds checking is not performed. Do not call
		this function if the input string could be larger than BUFFERSIZE-1 bytes in length!!!
	\param pInput Pointer to a UTF8 "C" string that's less than BUFFERSIZE in length. \ref NULL will page fault.
	\param uLength Number of bytes to copy.

***************************************/

void BURGER_API Burger::String::SetFast(const char *pInput,WordPtr uLength)
{
	// Remove any previously allocated buffer
	char *pDest = m_pData;
	if (pDest!=m_Raw) {
		Free(pDest);
		m_pData = m_Raw;
	}
	// Copy the new string
	m_uLength = uLength;
	m_Raw[uLength] = 0;
	MemoryCopy(m_Raw,pInput,uLength);
}

void BURGER_API Burger::String::InitFormattedString(const char* pFormat,WordPtr uArgCount,const SafePrintArgument **ppArgs)
{
	// Remove any previously allocated buffer
	Clear();
	if ((pFormat == NULL) || (*pFormat == 0x00)) {
		return;
	}

	if (uArgCount && ppArgs) {
		if (!SprintfUserAlloc(FormattedAllocCallback,this,TRUE,pFormat,uArgCount,ppArgs)) {
			Clear();
		}
	} else {
		Set(pFormat);
	}
}

 Word BURGER_API Burger::String::FormattedAllocCallback(Word bNoErrors,WordPtr uRequestedSize,void **ppOutputBuffer,void *pContext)
{
	if (!bNoErrors || !pContext || !uRequestedSize) {
		return FALSE;	// Abort
	}
	String* theString = static_cast<String*>(pContext);
	theString->SetBufferSize(uRequestedSize);
	ppOutputBuffer[0] = theString->m_pData;
	return TRUE;		// Proceed
}


/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are equal

***************************************/



/*! ************************************

	\fn Burger::String::operator != (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the strings are not equal

***************************************/



/*! ************************************

	\fn Burger::String::operator < (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the first is less than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than the second (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the first is less than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than the second (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than the second

***************************************/



/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/


/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the first is greater than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than the second (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the first is greater than the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than the second (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than the second

***************************************/


/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,const char *pInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (const char *pInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second (Case sensitive)
	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,char cInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second (Case sensitive)
	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (char cInput1,Burger::String const &rInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second (Case sensitive)
	\param cInput1 Single character as a string to test 
	\param rInput2 Burger::String to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/
