/***************************************

	C++ ANSI "C" string class for UTF16
	This is optimized for performance

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brstring16.h"
#include "brstringfunctions.h"
#include "brutf16.h"

/*! ************************************

	\class Burger::String16
	
	\brief UTF 16 bit string class
	
	This commonly used string class was designed for performance in mind. Each
	instance takes 128 bytes to contain data for the string and a pointer
	to allocated memory if the internal string buffer is too large. Since
	most strings rarely exceed 120 bytes in length, this class will be able to
	allocate and free strings with no calls to a memory manager. Only
	when the string exceeds the internal buffer will Burger::Alloc()
	and Burger::Free() be used.

	Functions exist to convert UTF8 and UTF32 data into UTF16 format,
	which this string class uses internally for data storage.

	\sa Burger::String
	
***************************************/

/*! ************************************

	\fn Burger::String16::String16(void)

	\brief Creates an empty string

***************************************/

/*! ************************************

	\brief Initialize a Burger::String16 with a Burger::String16
	\param rInput Reference to a Burger::String16

***************************************/

Burger::String16::String16(const Burger::String16 &rInput)
{
	WordPtr uInputLength = rInput.m_uLength;	// Get the source length
	Word16 *pWork = m_Raw;
	const Word16 *pInput = rInput.m_pData;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<Word16 *>(Alloc((uInputLength+1)*sizeof(Word16)));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString16;
			uInputLength = 0;
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,(uInputLength+1)*sizeof(Word16));	// Copy the string and the ending NULL
}

/*! ************************************

	\brief Initialize a Burger::String16 by using a subsection of a different Burger::String16

	Given a starting (inclusive) and ending (exclusive) offset, grab the sub string
	and use it to create a new Burger::String16

	\note If uEnd is less than or equal to uStart, the resulting string will be empty.

	\param rInput Burger::String16 to receive input from
	\param uStart Offset to the first character to read from
	\param uEnd Offset beyond the last character to read from

***************************************/

Burger::String16::String16(const Burger::String16 &rInput,WordPtr uStart,WordPtr uEnd)
{
	WordPtr uInputLength = rInput.m_uLength;	// Get the source length
	if (uEnd>uInputLength) {					// Clamp the end of the string
		uEnd = uInputLength;					// Make sure it fits
	}
	const Word16 *pInput = rInput.m_pData;
	if (uStart>=uEnd) {							// Valid range?
		uInputLength = 0;						// The result will be empty
	} else {
		uInputLength = uEnd-uStart;				// Length of the new string
		pInput += uStart;
	}
	Word16 *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<Word16 *>(Alloc((uInputLength+1)*sizeof(Word16)));
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
	MemoryCopy(pWork,pInput,uInputLength*sizeof(Word16));	// Copy the string
}

/*! ************************************

	\brief Initialize with a UTF8 "C" string

	Initialize the Burger::String16 with a copy of the passed string. The original string
	can be discarded after the call returns.

	\note This function will perform a conversion from UTF8 to UTF16

	\param pInput Pointer to a valid UTF8 "C" string or \ref NULL to create an empty string

***************************************/

Burger::String16::String16(const char *pInput)
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	WordPtr uInputLength = UTF16::FromUTF8(NULL,0,pInput)/sizeof(Word16);
	Word16 *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<Word16 *>(Burger::Alloc((uInputLength+1)*sizeof(Word16)));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	UTF16::FromUTF8(pWork,(uInputLength+1)*sizeof(Word16),pInput);	// Copy the string
}

/*! ************************************

	\brief Initialize with a UTF8 "C" string with padding

	Initialize the Burger::String16 with a copy of the passed string. The original string
	can be discarded after the call returns. 
	Allocate a buffer that can hold the initialization string + the uPadding number
	of bytes so the programmer can manually append data to the end of the string
	with Burger::StringCopy() or equivalent

	\param pInput Pointer to a valid UTF8 "C" string or \ref NULL to create an empty string
	\param uPadding Number of bytes to extend the string buffer. The extra bytes are NOT initialized

***************************************/

Burger::String16::String16(const char *pInput,WordPtr uPadding)
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	WordPtr uInputLength = UTF16::FromUTF8(NULL,0,pInput)/sizeof(Word16);
	Word16 *pWork = m_Raw;
	if ((uInputLength+uPadding)>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<Word16 *>(Alloc((uInputLength+uPadding+1)*sizeof(Word16)));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	UTF16::FromUTF8(pWork,(uInputLength+1)*sizeof(Word16),pInput);	// Copy the string
}

/*! ************************************

	\brief Initialize with a UTF16 string

	Copy the UTF16 encoded input string 
	and initialize this class with it. The input
	string can be discarded after this call.

	\param pInput A pointer to a valid UTF16 "C" string

***************************************/

Burger::String16::String16(const Word16 *pInput)
{
	if (!pInput) {
		pInput = g_EmptyString16;
	}
	WordPtr uInputLength = StringLength(pInput);
	Word16 *pWork = m_Raw;
	if (uInputLength>=BUFFERSIZE) {				// Buffer big enough?
		pWork = static_cast<Word16 *>(Alloc((uInputLength+1)*sizeof(Word16)));
		if (!pWork) {				// Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString16;
			uInputLength = 0;		// Don't copy anything
		}
	}
	m_uLength = uInputLength;			// Save the new length
	m_pData = pWork;					// Set the pointer
	MemoryCopy(pWork,pInput,(uInputLength+1)*sizeof(Word16));	// Copy the string
}

/*! ************************************

	\fn Burger::String16::~String16()
	\brief Release memory, if any

	Releases any allocated memory for the string. Under most cases, this performs
	no operation.

***************************************/

/*! ************************************

	\fn Burger::String16::operator Word16 *()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF16 string
	\return Pointer to the UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String16::operator const Word16 *() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF16 string
	\return Pointer to the const UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String16::c_str()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF16 string
	\return Pointer to the UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String16::c_str() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF16 string
	\return Pointer to the const UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String16::operator Word16 () const
	\brief Return the first character of the UTF16 "C" string

	Helper to access the first character of the UTF16 "C" string. It can be zero.

	\return The first character of the UTF16 "C" string

***************************************/

/*! ************************************

	\fn Burger::String16::length(void) const
	\brief Return the length of the "C" string

	\return The length in shorts of the UTF16 "C" string

***************************************/

/*! ************************************

	\fn Burger::String16::GetLength(void) const
	\brief Return the length of the UTF16 "C" string

	\return The length in shorts of the UTF16 "C" string

***************************************/

/*! ************************************

	\fn Burger::String16::GetPtr()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF16 string
	\return Pointer to the UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String16::GetPtr() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF16 string
	\return Pointer to the const UTF16 "C" string.

***************************************/

/*! ************************************

	\brief Force a buffer size

	Set the buffer to a specific size while retaining the existing string.
	If the preexisting string is too long for the new buffer, it will be
	truncated. The buffer size will be padded to reserve two bytes for the terminating
	zero.

	\note If a buffer of 100 characters is requested, 202 bytes will be allocated to hold
	a string up to 100 characters in length with the 101st short being the terminating
	zero. The output of Burger::StringLength(const Word16 *) is acceptable as input for a new string.

	\param uSize Number of characters to set the buffer to

***************************************/

void BURGER_API Burger::String16::SetBufferSize(WordPtr uSize)
{
	if (uSize!=m_uLength) {
		// If no space is requested, clear the buffer
		if (!uSize) {
			Clear();
		} else {
			// Hold the old buffer
			Word16 *pWork = m_pData;
			// Get the new buffer
			Word16 *pDest = m_Raw;
			// Allocate a new buffer if needed
			if (uSize>=BUFFERSIZE) {	// Buffer big enough?
				pDest = static_cast<Word16 *>(Alloc((uSize+1)*2));
				if (!pDest) {			// Oh oh...
					pDest = m_Raw;
					uSize = 0;			// Don't copy anything
				}
			}
			// Get the size of the string
			WordPtr uDestSize = m_uLength;
			if (uDestSize>=uSize) {
				// Truncate the string
				uDestSize = uSize;
			}
			MemoryCopy(pDest,pWork,uDestSize*2);
			pDest[uDestSize] = 0;		// Ensure the buffer is zero terminated
			if (pWork!=m_Raw) {			// Discard previous memory
				Free(pWork);
			}
			m_pData = pDest;			// Set the pointer
			m_uLength = uSize;			// Save the new length
			pDest[uSize] = 0;			// Ensure the terminating zero is present
		}
	}
}

/*! ************************************

	\fn Burger::String16::operator () (WordPtr uStart,WordPtr uEnd) const
	\brief Create a new Burger::String16 from a substring

	Given the start and end offsets of a string, create a new string with those
	offsets and return the resulting string

	\param uStart Offset to the starting character of the new string
	\param uEnd Offset to the character AFTER the last character of the new string
	\return A class instance of Burger::String16 containing the new string.

	\sa Burger::String16::String16(const Burger::String16 &,WordPtr,WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String16::operator () (WordPtr uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String16::operator [] (WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String16::operator () (WordPtr uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String16::operator [] (WordPtr) const

***************************************/

/*! ************************************

	\fn Burger::String16::operator [] (WordPtr uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String16::operator () (WordPtr)

***************************************/

/*! ************************************

	\fn Burger::String16::operator [] (WordPtr uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String16::operator () (WordPtr) const

***************************************/

/*! ************************************

	\fn Burger::String16::operator ! () const
	\brief Return \ref TRUE if the string is empty

	\return \ref TRUE on an empty string, \ref FALSE if there are characters in the string

***************************************/

/*! ************************************

	\fn Burger::String16::IsValid(void) const
	\brief Return \ref TRUE if the string has characters

	\return \ref FALSE on an empty string, \ref TRUE if there are characters in the string

***************************************/

/*! ************************************

	\brief Clear out the data

	Set the string to an empty string

***************************************/

void Burger::String16::Clear(void)
{
	Word16 *pWork = m_pData;	// Old data pointer
	m_pData = m_Raw;			// New pointer
	m_uLength = 0;				// No length
	m_Raw[0] = 0;				// Kill the string
	if (pWork!=m_Raw) {			// Dispose of the data
		Free(pWork);			// Kill the old memory
	}
}
