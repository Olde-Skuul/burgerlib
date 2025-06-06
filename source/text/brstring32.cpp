/***************************************

	C++ ANSI "C" string class for UTF32
	This is optimized for performance

	Copyright (c) 2023-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brstring32.h"
#include "brmemoryfunctions.h"
#include "brutf32.h"

/*! ************************************

	\class Burger::String32

	\brief UTF 32 bit string class

	This commonly used string class was designed for performance in mind. Each
	instance takes 128 bytes to contain data for the string and a pointer
	to allocated memory if the internal string buffer is too large. Since
	most strings rarely exceed 120 bytes in length, this class will be able to
	allocate and free strings with no calls to a memory manager. Only
	when the string exceeds the internal buffer will Burger::allocate_memory()
	and Burger::free_memory() be used.

	Functions exist to convert UTF8 and UTF16 data into UTF32 format,
	which this string class uses internally for data storage.

	\sa Burger::String

***************************************/

/*! ************************************

	\fn Burger::String32::String32(void)

	\brief Creates an empty string

***************************************/

/*! ************************************

	\brief Initialize a Burger::String32 with a Burger::String32
	\param rInput Reference to a Burger::String32

***************************************/

Burger::String32::String32(const Burger::String32& rInput)
{
	uintptr_t uInputLength = rInput.m_uLength; // Get the source length
	uint32_t* pWork = m_Raw;
	const uint32_t* pInput = rInput.m_pData;
	if (uInputLength >= BUFFERSIZE) { // Buffer big enough?
		pWork = static_cast<uint32_t*>(
			allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pWork) { // Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString32;
			uInputLength = 0;
		}
	}
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	memory_copy(pWork, pInput,
		(uInputLength + 1) *
			sizeof(uint32_t)); // Copy the string and the ending NULL
}

/*! ************************************

	\brief Initialize a Burger::String32 by using a subsection of a different
		Burger::String32

	Given a starting (inclusive) and ending (exclusive) offset, grab the sub
	string and use it to create a new Burger::String32

	\note If uEnd is less than or equal to uStart, the resulting string will be
		empty.

	\param rInput Burger::String32 to receive input from
	\param uStart Offset to the first character to read from
	\param uEnd Offset beyond the last character to read from

***************************************/

Burger::String32::String32(
	const Burger::String32& rInput, uintptr_t uStart, uintptr_t uEnd)
{
	uintptr_t uInputLength = rInput.m_uLength; // Get the source length
	if (uEnd > uInputLength) {                 // Clamp the end of the string
		uEnd = uInputLength;                   // Make sure it fits
	}
	const uint32_t* pInput = rInput.m_pData;
	if (uStart >= uEnd) { // Valid range?
		uInputLength = 0; // The result will be empty
	} else {
		uInputLength = uEnd - uStart; // Length of the new string
		pInput += uStart;
	}
	uint32_t* pWork = m_Raw;
	if (uInputLength >= BUFFERSIZE) { // Buffer big enough?
		pWork = static_cast<uint32_t*>(
			allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pWork) { // Oh oh...
			pWork = m_Raw;
			uInputLength = 0; // Don't copy anything
		}
	}
	// Since I can't assume the string being created ends with a zero, I'll
	// manually place one in the new string
	pWork[uInputLength] = 0;
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	memory_copy(
		pWork, pInput, uInputLength * sizeof(uint32_t)); // Copy the string
}

/*! ************************************

	\brief Initialize with a UTF8 "C" string

	Initialize the Burger::String32 with a copy of the passed string. The
	original string can be discarded after the call returns.

	\note This function will perform a conversion from UTF8 to UTF32

	\param pInput Pointer to a valid UTF8 "C" string or \ref NULL to create an
		empty string

***************************************/

Burger::String32::String32(const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	uintptr_t uInputLength = UTF32::translate_from_UTF8(nullptr, 0, pInput);
	uint32_t* pWork = m_Raw;
	if (uInputLength >= BUFFERSIZE) { // Buffer big enough?
		pWork = static_cast<uint32_t*>(
			Burger::allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pWork) { // Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0; // Don't copy anything
		}
	}
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	UTF32::translate_from_UTF8(
		pWork, uInputLength + 1, pInput); // Copy the string
}

/*! ************************************

	\brief Initialize with a UTF8 "C" string with padding

	Initialize the Burger::String32 with a copy of the passed string. The
	original string can be discarded after the call returns. Allocate a buffer
	that can hold the initialization string + the uPadding number of bytes so
	the programmer can manually append data to the end of the string with
	Burger::string_copy() or equivalent

	\param pInput Pointer to a valid UTF8 "C" string or \ref NULL to create an
		empty string

	\param uPadding Number of bytes to extend the string buffer. The extra bytes
		are NOT initialized

***************************************/

Burger::String32::String32(const char* pInput, uintptr_t uPadding)
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	uintptr_t uInputLength = UTF32::translate_from_UTF8(NULL, 0, pInput);
	uint32_t* pWork = m_Raw;
	if ((uInputLength + uPadding) >= BUFFERSIZE) { // Buffer big enough?
		pWork = static_cast<uint32_t*>(
			allocate_memory((uInputLength + uPadding + 1) * sizeof(uint32_t)));
		if (!pWork) { // Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString;
			uInputLength = 0; // Don't copy anything
		}
	}
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	UTF32::translate_from_UTF8(
		pWork, uInputLength + 1, pInput); // Copy the string
}

/*! ************************************

	\brief Initialize with a UTF32 string

	Copy the UTF32 encoded input string
	and initialize this class with it. The input
	string can be discarded after this call.

	\param pInput A pointer to a valid UTF32 "C" string

***************************************/

Burger::String32::String32(const uint32_t* pInput)
{
	if (!pInput) {
		pInput = g_EmptyString32;
	}
	uintptr_t uInputLength = string_length(pInput);
	uint32_t* pWork = m_Raw;
	if (uInputLength >= BUFFERSIZE) { // Buffer big enough?
		pWork = static_cast<uint32_t*>(
			allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pWork) { // Oh oh...
			pWork = m_Raw;
			pInput = g_EmptyString32;
			uInputLength = 0; // Don't copy anything
		}
	}
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	memory_copy(pWork, pInput,
		(uInputLength + 1) * sizeof(uint32_t)); // Copy the string
}

/*! ************************************

	\fn Burger::String32::~String32()
	\brief Release memory, if any

	Releases any allocated memory for the string. Under most cases, this
	performs no operation.

***************************************/

/*! ************************************

	\fn Burger::String32::operator uint32_t *()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF32 string
	\return Pointer to the UTF32 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String32::operator const uint32_t *() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF32 string
	\return Pointer to the const UTF32 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String32::c_str()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF32 string
	\return Pointer to the UTF32 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String32::c_str() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF32 string
	\return Pointer to the const UTF16 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String32::operator uint32_t () const
	\brief Return the first character of the UTF32 "C" string

	Helper to access the first character of the UTF32 "C" string. It can be
	zero.

	\return The first character of the UTF32 "C" string

***************************************/

/*! ************************************

	\fn Burger::String32::length(void) const
	\brief Return the length of the "C" string

	\return The length in longs of the UTF32 "C" string

***************************************/

/*! ************************************

	\fn Burger::String32::GetLength(void) const
	\brief Return the length of the UTF32 "C" string

	\return The length in shorts of the UTF32 "C" string

***************************************/

/*! ************************************

	\fn Burger::String32::GetPtr()
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF32 string
	\return Pointer to the UTF32 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String32::GetPtr() const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF32 string
	\return Pointer to the const UTF32 "C" string.

***************************************/

/*! ************************************

	\brief Copy a "C" string to a Burger::String

	\note This function allows using the tail end of
	the current string as input

	\param pInput Pointer to a UTF8 "C" string. \ref NULL generates an empty
		string.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String32::assign(
	const uint32_t* pInput) BURGER_NOEXCEPT
{
	// Assume no error
	eError uResult = kErrorNone;

	if (!pInput) {
		pInput = g_EmptyString32;
	}
	uint32_t* pDest = m_Raw;
	uintptr_t uInputLength = string_length(pInput); // Length of the new string
	if (uInputLength >= BUFFERSIZE) {               // Buffer big enough?
		pDest = static_cast<uint32_t*>(
			allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pDest) { // Oh oh...
			pDest = m_Raw;
			uInputLength = 0;            // Don't copy anything
			pInput = g_EmptyString32;    // Will copy the null character
			uResult = kErrorOutOfMemory; // Error!
		}
	}
	const uint32_t* pOld = m_pData;
	m_uLength = uInputLength; // Save the new length
	m_pData = pDest;          // Set the pointer
	memory_move(pDest, pInput,
		(uInputLength + 1) * sizeof(uint32_t)); // Copy the string
	if (pOld != m_Raw) {                        // Discard previous memory
		free_memory(pOld);
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy an 8 bit "C" string to a Burger::String32

	\param pInput Pointer to a UTF16 "C" string. \ref NULL generates an empty
	string.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String32::assign(
	const char* pInput) BURGER_NOEXCEPT
{
	// Assume no error
	eError uResult = kErrorNone;

	if (!pInput) {
		pInput = g_EmptyString;
	}
	uint32_t* pDest = m_Raw;
	// Length of the new string
	uintptr_t uInputLength = UTF32::translate_from_UTF8(nullptr, 0, pInput);
	// Buffer big enough?
	if (uInputLength >= BUFFERSIZE) {
		pDest = static_cast<uint32_t*>(
			allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
		if (!pDest) { // Oh oh...
			pDest = m_Raw;
			uInputLength = 0;            // Don't copy anything
			pInput = g_EmptyString;      // Will copy the null character
			uResult = kErrorOutOfMemory; // Error!
		}
	}
	const uint32_t* pOld = m_pData;
	m_uLength = uInputLength; // Save the new length
	m_pData = pDest;          // Set the pointer
	// Copy the string
	UTF32::translate_from_UTF8(pDest, uInputLength + 1, pInput);
	// Discard previous memory
	if (pOld != m_Raw) {
		free_memory(pOld);
	}
	// Return error
	return uResult;
}

/*! ************************************

	\brief Force a buffer size

	Set the buffer to a specific size while retaining the existing string.
	If the preexisting string is too long for the new buffer, it will be
	truncated. The buffer size will be padded to reserve two bytes for the
	terminating zero.

	\note If a buffer of 100 characters is requested, 202 bytes will be
	allocated to hold a string up to 100 characters in length with the 101st
	short being the terminating zero. The output of Burger::string_length(
	const uint32_t *) is acceptable as input for a new string.

	\param uSize Number of characters to set the buffer to

***************************************/

Burger::eError BURGER_API Burger::String32::SetBufferSize(
	uintptr_t uSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (uSize != m_uLength) {
		// If no space is requested, clear the buffer
		if (!uSize) {
			clear();
		} else {
			// Hold the old buffer
			uint32_t* pWork = m_pData;
			// Get the new buffer
			uint32_t* pDest = m_Raw;
			// Allocate a new buffer if needed
			if (uSize >= BUFFERSIZE) { // Buffer big enough?
				pDest = static_cast<uint32_t*>(allocate_memory((uSize + 1) * 4));
				if (!pDest) { // Oh oh...
					pDest = m_Raw;
					uSize = 0; // Don't copy anything
					uResult = kErrorOutOfMemory;
				}
			}
			// Get the size of the string
			uintptr_t uDestSize = m_uLength;
			if (uDestSize >= uSize) {
				// Truncate the string
				uDestSize = uSize;
			}
			memory_copy(pDest, pWork, uDestSize * 4);
			pDest[uDestSize] = 0; // Ensure the buffer is zero terminated
			if (pWork != m_Raw) { // Discard previous memory
				free_memory(pWork);
			}
			m_pData = pDest;   // Set the pointer
			m_uLength = uSize; // Save the new length
			pDest[uSize] = 0;  // Ensure the terminating zero is present
		}
	}
	return uResult;
}

/*! ************************************

	\brief Assign a Burger::String32

	Copy the contents of a Burger::String32 into this Burger::String32

	\param rInput Reference to a Burger::String32 to copy from
	\return A reference to the this pointer

***************************************/

Burger::String32& Burger::String32::operator=(const Burger::String32& rInput)
{
	if (this != &rInput) { // Am I copying myself?
		uint32_t* pWork = m_pData;
		if (pWork != m_Raw) { // Discard previous memory
			free_memory(pWork);
			pWork = m_Raw;
		}

		uintptr_t uInputLength = rInput.m_uLength; // Length of the new string
		const uint32_t* pInput = rInput.m_pData;   // Copy the new length
		if (uInputLength >= BUFFERSIZE) {          // Buffer big enough?
			pWork = static_cast<uint32_t*>(
				allocate_memory((uInputLength + 1) * sizeof(uint32_t)));
			if (!pWork) { // Oh oh...
				pWork = m_Raw;
				uInputLength = 0;         // Don't copy anything
				pInput = g_EmptyString32; // Will copy the null character
			}
		}
		m_uLength = uInputLength; // Save the new length
		m_pData = pWork;          // Set the pointer
		memory_copy(pWork, pInput,
			(uInputLength + 1) * sizeof(uint32_t)); // Copy the string
	}
	return *this;
}

/*! ************************************

	\brief Assign a "C" string to a Burger::String32

	Copy the contents of a UTF8 "C" string into this Burger::String32

	\param pInput Pointer to a UTF8 "C" string. \ref NULL generates an empty
		string. 

	\return A reference to the this pointer

***************************************/

Burger::String32& Burger::String32::operator=(const uint32_t* pInput)
{
	assign(pInput);
	return *this;
}

/*! ************************************

	\brief Assign a "C" string to a Burger::String32

	Copy the contents of a UTF8 "C" string into this Burger::String32

	\param pInput Pointer to a UTF8 "C" string. \ref NULL generates an empty
		string. 

	\return A reference to the this pointer

***************************************/

Burger::String32& Burger::String32::operator=(const char* pInput)
{
	assign(pInput);
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

Burger::String32& Burger::String32::operator=(char cInput)
{
	uint32_t* pWork = m_pData;
	if (pWork != m_Raw) { // Discard previous memory
		free_memory(pWork);
		pWork = m_Raw;
	}

	pWork[0] = cInput; // Store the char in the string
	pWork[1] = 0;
	uintptr_t uInputLength = 1;
	if (!cInput) {        // Valid string?
		uInputLength = 0; // 1 char long
	}
	m_uLength = uInputLength; // Save the new length
	m_pData = pWork;          // Set the pointer
	return *this;
}

/*! ************************************

	\fn Burger::String32::operator () (uintptr_t uStart,uintptr_t uEnd) const
	\brief Create a new Burger::String32 from a substring

	Given the start and end offsets of a string, create a new string with those
	offsets and return the resulting string

	\param uStart Offset to the starting character of the new string
	\param uEnd Offset to the character AFTER the last character of the new
		string

	\return A class instance of Burger::String32 containing the new string.

	\sa Burger::String32::String32(const Burger::String32 &,uintptr_t,uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String32::operator () (uintptr_t uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String32::operator [] (uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String32::operator () (uintptr_t uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String32::operator [] (uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::String32::operator [] (uintptr_t uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String32::operator () (uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String32::operator [] (uintptr_t uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String32::operator () (uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::String32::operator ! () const
	\brief Return \ref TRUE if the string is empty

	\return \ref TRUE on an empty string, \ref FALSE if there are characters in
		the string

***************************************/

/*! ************************************

	\fn Burger::String32::is_valid(void) const
	\brief Return \ref TRUE if the string has characters

	\return \ref FALSE on an empty string, \ref TRUE if there are characters in
		the string

***************************************/

/*! ************************************

	\brief Clear out the data

	Set the string to an empty string

***************************************/

void Burger::String32::clear(void) BURGER_NOEXCEPT
{
	uint32_t* pWork = m_pData; // Old data pointer
	m_pData = m_Raw;           // New pointer
	m_uLength = 0;             // No length
	m_Raw[0] = 0;              // Kill the string
	if (pWork != m_Raw) {      // Dispose of the data
		free_memory(pWork);           // Kill the old memory
	}
}
