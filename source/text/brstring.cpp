/***************************************

	This is the Burgerlib C++ string class. I will not trigger exceptions on
	memory errors. However, I do take great care in making sure that the class
	structure is in a valid state at all times

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brstring.h"
#include "brmemoryfunctions.h"
#include "brnumberto.h"
#include "brutf32.h"
#include "brutf8.h"

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

	\brief Create buffer for constructors

	This is a private function for constructors only. It will set up an initial
	buffer for storing a string requested by a constructor. If the allocation
	fails, the function will return \ref nullptr.

	\note This function assumes the String object is uninitialized.

	\sa String(const char *) or String(const String&)

***************************************/

char* BURGER_API Burger::String::constructor_reserve(
	uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	char* pResult;

	if (uBufferSize >= kBufferSize) {

		// Buffer is allocated.
		pResult = static_cast<char*>(Alloc(uBufferSize + 1));
		// Oh oh...
		if (!pResult) {
			// Failure case, neuter the class and return nullptr
			pResult = m_Raw;
			uBufferSize = kBufferSize - 1;
			m_uBufferSize = kBufferSize - 1;
		} else {
			// Save the new string buffer
			m_uBufferSize = uBufferSize;
		}

	} else {
		// Use the internal buffer
		pResult = m_Raw;
		m_uBufferSize = kBufferSize - 1;
	}
	// Always ensure zero terminated
	m_pData = pResult;
	m_uLength = uBufferSize;
	pResult[uBufferSize] = 0;
	return pResult;
}

/*! ************************************

	\brief Copy "C" string without safeguards.

	If the string is under \ref kBufferSize -1 bytes in length AND it's not
	using the a buffer where a call to MemoryCopy() would fail, then call this
	function to copy the string since it performs some optimizations that cannot
	be done if either case of the input data were true.

	\note Due to optimizations, some bounds checking is not performed. Do
	not call this function if the input string could be larger than
	kBufferSize-1 bytes in length!!!

	\param pInput Pointer to a UTF8 "C" string that's less than kBufferSize
		in length. \ref nullptr will page fault.

***************************************/

void BURGER_API Burger::String::assign_small(const char* pInput) BURGER_NOEXCEPT
{
	// Get the length of the string
	uintptr_t uLength = string_length(pInput);
	m_uLength = uLength;
	// The string already has a terminating zero
	MemoryCopy(m_pData, pInput, uLength + 1);
}

/*! ************************************

	\brief Copy an array of characters to a String with shortcuts

	If the string is under \ref kBufferSize -1 bytes in length AND it's not
	using the a buffer where a call to MemoryCopy() would fail, then call this
	function to update the string since it performs some optimizations that
	cannot be done if either case of the input data were true. A zero will be
	appended to the final string. The input string should not contain a zero.

	\note Due to optimizations, some bounds checking is not performed. Do
	not call this function if the input string could be larger than
	kBufferSize-1 bytes in length!!!

	\param pInput Pointer to a UTF8 "C" string that's less than kBufferSize
		in length. \ref nullptr will page fault.

	\param uLength Number of bytes to copy.

***************************************/

void BURGER_API Burger::String::assign_small(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Copy the new string
	m_uLength = uLength;
	// Ensure it's terminated
	m_pData[uLength] = 0;
	MemoryCopy(m_pData, pInput, uLength);
}

/*! ************************************

	\fn Burger::String::String(void)

	\brief Creates an empty string

***************************************/

/*! ************************************

	\brief Initialize a Burger::String with a Burger::String

	\param rInput Reference to a Burger::String

***************************************/

Burger::String::String(const Burger::String& rInput) BURGER_NOEXCEPT
{
	// Reserve a buffer
	uintptr_t uInputLength = rInput.m_uLength;
	char* pWork = constructor_reserve(uInputLength);

	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy the data
	MemoryCopy(pWork, rInput.m_pData, uInputLength);
}

/*! ************************************

	\brief Copy constructor

	Only available on C++11 or higher.

	\param rInput Reference to a Burger::String

***************************************/

#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
Burger::String::String(String&& rInput) BURGER_NOEXCEPT
{
	m_uLength = rInput.m_uLength;
	m_uBufferSize = rInput.m_uBufferSize;
	if (rInput.m_pData != rInput.m_Raw) {
		m_pData = rInput.m_pData;

		// Zap the origin
		rInput.m_pData = rInput.m_Raw;
		rInput.m_uLength = 0;
		rInput.m_uBufferSize = kBufferSize - 1;
		rInput.m_Raw[0] = 0;
	} else {
		// No need to neuter the original class.
		m_pData = m_Raw;
		MemoryCopy(m_Raw, rInput.m_Raw, rInput.m_uLength + 1);
	}
}

/*! ************************************

	\brief Copy assignment operator

	Only available on C++11 or higher.

	\param rInput Reference to a Burger::String

***************************************/

Burger::String& Burger::String::operator=(String&& rInput) BURGER_NOEXCEPT
{
	if (this != &rInput) {

		// If the internal buffer is too small, it must be a pointer
		if (m_uBufferSize < rInput.m_uLength) {

			// Capturing the other pointer. Release our pointer
			if (m_pData != m_Raw) {
				Free(m_pData);
			}

			// Copy the contents
			m_pData = rInput.m_pData;
			m_uLength = rInput.m_uLength;
			m_uBufferSize = rInput.m_uBufferSize;

			// Neuter the other class
			rInput.m_pData = rInput.m_Raw;
			rInput.m_uLength = 0;
			rInput.m_uBufferSize = kBufferSize - 1;
			rInput.m_Raw[0] = 0;

		} else {
			// If the buffer is big enough, just copy and be done with it.
			MemoryCopy(m_pData, rInput.m_pData, rInput.m_uLength + 1);
			m_uLength = rInput.m_uLength;
		}
	}
	return *this;
}
#endif

/*! ************************************

	\brief Initialize a Burger::String by using a subsection of a different
		Burger::String

	Given a starting (inclusive) and ending (exclusive) offset, grab the sub
	string and use it to create a new Burger::String

	\note If uEnd is less than or equal to uStart, the resulting string will be
		empty.

	\param rInput Burger::String to receive input from
	\param uStart Offset to the first character to read from
	\param uLength Number of characters to copy.

***************************************/

Burger::String::String(const Burger::String& rInput, uintptr_t uStart,
	uintptr_t uLength) BURGER_NOEXCEPT
{
	// Get the source pointer
	const char* pInput = rInput.m_pData;

	// Get the source length
	uintptr_t uInputLength = rInput.m_uLength;

	// If the start is beyond the end of the string, no text will be transferred
	if (uStart >= uInputLength) {
		uInputLength = 0;
	} else {
		// Skip the start entries
		pInput += uStart;
		uInputLength -= uStart;
	}

	// Clamp the end of the string
	if (uLength < uInputLength) {
		// This will be the new string size
		uInputLength = uLength;
	}

	// Reserve a buffer
	char* pWork = constructor_reserve(uInputLength);

	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy the data
	MemoryCopy(pWork, pInput, uInputLength);
}

/*! ************************************

	\brief Initialize with a "C" string

	Initialize the Burger::String with a copy of the passed string. The original
	string can be discarded after the call returns.

	\param pInput Pointer to a valid "C" string or \ref NULL to create an empty
		string

***************************************/

Burger::String::String(const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	uintptr_t uInputLength = string_length(pInput);

	// Reserve a buffer
	char* pWork = constructor_reserve(uInputLength);

	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy the data
	MemoryCopy(pWork, pInput, uInputLength);
}

/*! ************************************

	\brief Initialize with a "C" string with padding

	Initialize the Burger::String with a copy of the passed string. The original
	string can be discarded after the call returns. Allocate a buffer that can
	hold the initialization string + the uPadding number of bytes so the
	programmer can manually append data to the end of the string with
	Burger::string_copy() or equivalent

	\param pInput Pointer to a valid "C" string or \ref NULL to create an empty
		string
	\param uPadding Number of bytes to extend the string buffer. The extra
		bytes are NOT initialized

***************************************/

Burger::String::String(const char* pInput, uintptr_t uPadding) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	uintptr_t uInputLength = string_length(pInput);

	char* pWork = constructor_reserve(uInputLength + uPadding);
	// Oh oh... Was it clamped?
	if ((uInputLength + uPadding) != m_uLength) {

		// Try again without padding
		pWork = constructor_reserve(uInputLength);
		// If truncated, so be it.
		uInputLength = m_uLength;
	}

	// constructor_reserve() sets the length to buffersize, correct it.
	m_uLength = uInputLength;
	// Copy the data
	MemoryCopy(pWork, pInput, uInputLength);
}

/*! ************************************

	\brief Initialize with a UTF16 string

	Convert the UTF16 encoded input string into a UFT8 encoded string and
	initialize this class with the UTF8 version. The input string can be
	discarded after this call.

	\param pInput A pointer to a valid UTF16 "C" string

***************************************/

Burger::String::String(const uint16_t* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString16;
	}
	uintptr_t uInputLength = UTF8::GetUTF16Size(pInput);
	char* pWork = constructor_reserve(uInputLength);
	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy and zero terminate
	UTF8::from_UTF16(pWork, uInputLength + 1, pInput);
}

/*! ************************************

	\brief Initialize with a UTF32 string

	Convert the UTF32 encoded input string into a UFT8 encoded string
	and initialize this class with the UTF8 version. The input
	string can be discarded after this call.

	\param pInput A pointer to a valid UTF32 "C" string

***************************************/

Burger::String::String(const uint32_t* pInput) BURGER_NOEXCEPT
{
	uint32_t uTemp;
	if (!pInput) {
		uTemp = 0;
		pInput = &uTemp;
	}

	uintptr_t uInputLength = UTF8::from_UTF32(nullptr, 0, pInput);
	char* pWork = constructor_reserve(uInputLength);
	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy and zero terminate
	UTF8::from_UTF32(pWork, uInputLength + 1, pInput);
}

/*! ************************************

	\brief Initialize a Burger::String by using a subsection of a different "C"
		string

	Copy a range of bytes from a "C" string. This will terminate early if a zero
	byte is found in the input stream.

	\param pInput A pointer to a valid "C" string
	\param uStart Offset to the first character to read from
	\param uLength Number of bytes to copy

***************************************/

Burger::String::String(
	const char* pInput, uintptr_t uStart, uintptr_t uLength) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}

	// Get the source length
	uintptr_t uInputLength = string_length(pInput);

	// If the start is beyond the end of the string, no text will be transferred
	if (uStart >= uInputLength) {
		uInputLength = 0;
	} else {
		// Skip the start entries
		pInput += uStart;
		uInputLength -= uStart;
	}

	// Clamp the end of the string
	if (uLength < uInputLength) {
		// This will be the new string size
		uInputLength = uLength;
	}

	// Reserve a buffer
	char* pWork = constructor_reserve(uInputLength);

	// Get the adjusted length
	uInputLength = m_uLength;
	// Copy the data
	MemoryCopy(pWork, pInput, uInputLength);
}

/*! ************************************

	\brief Initialize a Burger::String with a single character

	If the input character is zero, the resulting string will be empty

	\note It is NOT recommended to use "high ASCII" as input such as
	values 128-255. They are considered prefix codes as per the UTF8 standard
	and will yield undefined results for code that's expecting a
	valid UTF8 string

	\param iInput Character to convert to a single byte string

***************************************/

Burger::String::String(char iInput) BURGER_NOEXCEPT
{
	m_Raw[0] = iInput; // Store the char in the string
	m_Raw[1] = 0;
	uintptr_t uInputLength = 1;
	if (!iInput) {        // Valid string?
		uInputLength = 0; // 1 char long
	}

	// Set the pointer
	m_pData = m_Raw;
	// Save the new length
	m_uLength = uInputLength;
	m_uBufferSize = kBufferSize - 1;
}

/*! ************************************

	\brief Initialize a Burger::String filled with a single character

	If the input character is zero, the resulting string will be empty

	\param iInput Character to fill the buffer with
	\param uFillSize Size, in bytes, of the resulting string

***************************************/

Burger::String::String(char iInput, uintptr_t uFillSize) BURGER_NOEXCEPT
{
	// Fix a logic error. If the character is null, then the string is empty
	// by default. Kill the fill length

	if (!iInput) {
		uFillSize = 0;
	}

	// Reserve a buffer
	char* pWork = constructor_reserve(uFillSize);

	// Get the adjusted length
	uFillSize = m_uLength;
	// Fill in the data
	MemoryFill(pWork, static_cast<uint8_t>(iInput), uFillSize);
}

/*! ************************************

	\brief Initialize a Burger::String with two concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty
	string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first

***************************************/

Burger::String::String(const char* pInput1, const char* pInput2) BURGER_NOEXCEPT
{
	if (!pInput1) { // Remove all NULLs
		pInput1 = g_EmptyString;
	}
	if (!pInput2) {
		pInput2 = g_EmptyString;
	}
	uintptr_t uLength1 = string_length(pInput1);
	uintptr_t uLength2 = string_length(pInput2);

	// Size of the finished string
	uintptr_t uInputLength = uLength1 + uLength2;

	char* pWork = constructor_reserve(uInputLength);

	// Truncation is possible...
	uInputLength = m_uLength;

	if (uLength1 > uInputLength) {
		uLength1 = uInputLength;
	}
	MemoryCopy(pWork, pInput1, uLength1);

	pWork += uLength1;
	uInputLength -= uLength1;
	if (uLength2 > uInputLength) {
		uLength2 = uInputLength;
	}
	MemoryCopy(pWork, pInput2, uLength2);
}

/*! ************************************

	\brief Initialize a Burger::String with three concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty
	string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first
	\param pInput3 String to append after the second

***************************************/

Burger::String::String(const char* pInput1, const char* pInput2,
	const char* pInput3) BURGER_NOEXCEPT
{
	if (!pInput1) { // Remove all NULLs
		pInput1 = g_EmptyString;
	}
	if (!pInput2) {
		pInput2 = g_EmptyString;
	}
	if (!pInput3) {
		pInput3 = g_EmptyString;
	}
	uintptr_t uLength1 = string_length(pInput1);
	uintptr_t uLength2 = string_length(pInput2);
	uintptr_t uLength3 = string_length(pInput3);

	// Size of the finished string
	uintptr_t uInputLength = uLength1 + uLength2 + uLength3;

	char* pWork = constructor_reserve(uInputLength);

	// Truncation is possible...
	uInputLength = m_uLength;

	if (uLength1 > uInputLength) {
		uLength1 = uInputLength;
	}
	MemoryCopy(pWork, pInput1, uLength1);

	pWork += uLength1;
	uInputLength -= uLength1;
	if (uLength2 > uInputLength) {
		uLength2 = uInputLength;
	}
	MemoryCopy(pWork, pInput2, uLength2);

	pWork += uLength2;
	uInputLength -= uLength2;
	if (uLength3 > uInputLength) {
		uLength3 = uInputLength;
	}
	MemoryCopy(pWork, pInput3, uLength3);
}

/*! ************************************

	\brief Initialize a Burger::String with four concatenated "C" strings

	If any input string pointer is \ref NULL, it will be treated as an empty
	string.

	\param pInput1 First string to initialize with
	\param pInput2 String to append after the first
	\param pInput3 String to append after the second
	\param pInput4 String to append after the third

***************************************/

Burger::String::String(const char* pInput1, const char* pInput2,
	const char* pInput3, const char* pInput4) BURGER_NOEXCEPT
{
	if (!pInput1) { // Remove all NULLs
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
	uintptr_t uLength1 = string_length(pInput1);
	uintptr_t uLength2 = string_length(pInput2);
	uintptr_t uLength3 = string_length(pInput3);
	uintptr_t uLength4 = string_length(pInput4);

	// Size of the finished string
	uintptr_t uInputLength = uLength1 + uLength2 + uLength3 + uLength4;

	char* pWork = constructor_reserve(uInputLength);

	// Truncation is possible...
	uInputLength = m_uLength;

	if (uLength1 > uInputLength) {
		uLength1 = uInputLength;
	}
	MemoryCopy(pWork, pInput1, uLength1);

	pWork += uLength1;
	uInputLength -= uLength1;
	if (uLength2 > uInputLength) {
		uLength2 = uInputLength;
	}
	MemoryCopy(pWork, pInput2, uLength2);

	pWork += uLength2;
	uInputLength -= uLength2;
	if (uLength3 > uInputLength) {
		uLength3 = uInputLength;
	}
	MemoryCopy(pWork, pInput3, uLength3);

	pWork += uLength3;
	uInputLength -= uLength3;
	if (uLength4 > uInputLength) {
		uLength4 = uInputLength;
	}
	MemoryCopy(pWork, pInput4, uLength4);
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

	\fn Burger::String::c_str(void)
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF8 ASCII string
	\return Pointer to the UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::c_str(void) const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF8 ASCII string
	\return Pointer to the const UTF8 "C" string.

***************************************/

/*! ************************************

	\fn Burger::String::data(void)
	\brief Return the pointer to the string

	Helper to access the pointer to the UTF8 ASCII string
	\return Pointer to the UTF8 "C" string.

	\sa c_str(void)

***************************************/

/*! ************************************

	\fn Burger::String::data(void) const
	\brief Return the const pointer to the string

	Helper to access the const pointer to the UTF8 ASCII string
	\return Pointer to the const UTF8 "C" string.

	\sa c_str(void) const

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

	\fn Burger::String::size(void) const
	\brief Return the length of the "C" string

	\return The length in bytes of the UTF8 "C" string

***************************************/

/*! ************************************

	\fn Burger::String::capacity(void) const
	\brief Return the size of the buffer.

	\return The size of the buffer in bytes.

***************************************/

/*! ************************************

	\fn Burger::String::empty(void) const
	\brief Return \ref TRUE if the string is empty.

	\return \ref TRUE or \ref FALSE.

***************************************/

/*! ************************************

	\fn Burger::String::~String()
	\brief Release memory, if any

	Releases any allocated memory for the string. Under most cases, this
	performs no operation.

***************************************/

/*! ************************************

	\brief Fill the string with a character.

	Set the string size to uLength and fill the contents with iInput. The
	resulting string will be zero terminated.

	\param uLength Size in bytes of the resulting string.
	\param iInput Character to fill the buffer with.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	uintptr_t uLength, char iInput) BURGER_NOEXCEPT
{
	const eError uResult = reserve(uLength);

	// Only if the buffer could not be resized will it truncate.
	if (uLength > m_uBufferSize) {
		uLength = m_uBufferSize;
	}
	m_uLength = uLength;
	char* pWork = m_pData;
	MemoryFill(pWork, static_cast<uint8_t>(iInput), uLength);
	pWork[uLength] = 0;
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a String into this string.

	Copy the contents of the string into this string.

	\param rInput Reference to the String to copy.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const String& rInput) BURGER_NOEXCEPT
{
	return assign(rInput.m_pData, rInput.m_uLength);
}

/*! ************************************

	\brief Move a String into this string.

	Move the contents of the string into this string. The rvalue will be empty
	after this call completes.

	\param rInput Reference to the String to copy.

***************************************/

#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
void BURGER_API Burger::String::assign(String&& rInput) BURGER_NOEXCEPT
{
	// If this string has a buffer, dispose of it.
	if (m_pData != m_Raw) {
		Free(m_pData);
	}

	// Does the other one have a buffer?
	if (rInput.m_pData != rInput.m_Raw) {

		// It does, take the pointer and clear out the original
		m_pData = rInput.m_pData;
		m_uLength = rInput.m_uLength;
		m_uBufferSize = rInput.m_uBufferSize;

		// Zap the donor String
		rInput.m_pData = rInput.m_Raw;
		rInput.m_uLength = 0;
		rInput.m_uBufferSize = kBufferSize - 1;
		rInput.m_Raw[0] = 0;
	} else {
		// Copy as POD
		MemoryCopy(m_Raw, rInput.m_Raw, rInput.m_uLength + 1);
		m_pData = m_Raw;
		m_uLength = rInput.m_uLength;
		m_uBufferSize = kBufferSize - 1;
	}
}
#endif

/*! ************************************

	\brief Copy a "C" string to a Burger::String

	\note This function allows using the tail end of the current string as input

	\param pInput Pointer to a UTF8 "C" string. \ref nullptr generates an empty
		string.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}

	// Length of the new string
	return assign(pInput, string_length(pInput));
}

/*! ************************************

	\brief Copy an array of characters into a string

	The input data will be converted into a string.

	\param pInput Pointer to an array of UTF8 characters. \ref nullptr generates
		an empty string.

	\param uInputLength Length of the input data

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const char* pInput, uintptr_t uInputLength) BURGER_NOEXCEPT
{
	// Failsafe
	if (!pInput) {
		uInputLength = 0;
	}

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);
	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = uInputLength;
	char* pWork = m_pData;
	MemoryCopy(pWork, pInput, uInputLength);
	pWork[uInputLength] = 0;
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 16 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF16 "C" string. \ref nullptr generates an empty
		string.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString16;
	}

	// Length of the new string
	uintptr_t uInputLength = UTF8::GetUTF16Size(pInput);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);
	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::from_UTF16(m_pData, uInputLength + 1, pInput);

	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 16 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF16 "C" string. \ref nullptr generates an empty
		string.
	\param uLength Length of the UTF16 string in characters (sizeof(buffer)/2)

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const uint16_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString16;
	}

	// Length of the new string
	uintptr_t uInputLength = UTF8::GetUTF16Size(pInput, uLength);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);
	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::from_UTF16(m_pData, uInputLength + 1, pInput, uLength);

	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 32 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF32 "C" string. \ref nullptr generates an empty
		string.

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const uint32_t* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString32;
	}

	// Length of the new string
	uintptr_t uInputLength = UTF8::GetUTF32Size(pInput);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);
	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::from_UTF32(m_pData, uInputLength + 1, pInput);

	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a 32 bit "C" string to a Burger::String

	\param pInput Pointer to a UTF32 "C" string. \ref nullptr generates an empty
		string.
	\param uLength Length of the UTF32 string in characters (sizeof(buffer)/2)

	\return Zero if no error, non zero if memory allocation failed

***************************************/

Burger::eError BURGER_API Burger::String::assign(
	const uint32_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString32;
	}

	// Length of the new string
	uintptr_t uInputLength = UTF8::GetUTF32Size(pInput, uLength);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);
	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::from_UTF32(m_pData, uInputLength + 1, pInput, uLength);

	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a "C" string encoded with Win437 into the String

	Given a "C" string using Win437 encoding, convert it to UTF8 and store it in
	the string.

	\param pInput Pointer to a "C" string encoded with Win437. \ref nullptr
		generates an empty string.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_win437(
	const char* pInput) BURGER_NOEXCEPT
{
	// Test for nullptr
	if (pInput) {
		return assign_win437(pInput, string_length(pInput));
	}
	// Create empty string
	m_uLength = 0;
	m_pData[0] = 0;
	return kErrorNone;
}

/*! ************************************

	\brief Copy a buffer encoded with Win437 into the String

	Given a buffer using Win437 encoding, convert it to UTF8 and store it in
	the string.

	\param pInput Pointer to a buffer encoded with Win437.
	\param uLength Length of the buffer.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_win437(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	uintptr_t uInputLength = UTF8::GetWin437Size(pInput, uLength);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);

	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::FromWin437(m_pData, uInputLength + 1, pInput, uLength);
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a "C" string encoded with Win1252 into the String

	Given a "C" string using Win1252 encoding, convert it to UTF8 and store it
	in the string.

	\param pInput Pointer to a "C" string encoded with Win1252. \ref nullptr
		generates an empty string.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_win1252(
	const char* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		return assign_win1252(pInput, string_length(pInput));
	}
	// Create empty string
	m_uLength = 0;
	m_pData[0] = 0;
	return kErrorNone;
}

/*! ************************************

	\brief Copy a buffer encoded with Win1252 into the String

	Given a buffer using Win1252 encoding, convert it to UTF8 and store it in
	the string.

	\param pInput Pointer to a buffer encoded with Win1252.
	\param uLength Length of the buffer.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_win1252(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	uintptr_t uInputLength = UTF8::GetWin1252Size(pInput, uLength);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);

	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength = UTF8::FromWin1252(m_pData, uInputLength + 1, pInput, uLength);
	// Return error
	return uResult;
}

/*! ************************************

	\brief Copy a "C" string encoded with MacRomanUS into the String

	Given a "C" string using MacRomanUS encoding, convert it to UTF8 and store
	it in the string.

	\param pInput Pointer to a "C" string encoded with MacRomanUS. \ref nullptr
		generates an empty string.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_mac_roman_us(
	const char* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		return assign_mac_roman_us(pInput, string_length(pInput));
	}
	// Create empty string
	m_uLength = 0;
	m_pData[0] = 0;
	return kErrorNone;
}

/*! ************************************

	\brief Copy a buffer encoded with MacRomanUS into the String

	Given a buffer using MacRomanUS encoding, convert it to UTF8 and store it in
	the string.

	\param pInput Pointer to a buffer encoded with MacRomanUS.
	\param uLength Length of the buffer.

	\return Zero if no error, non zero if memory allocation failed

	\sa assign(const char *, uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::String::assign_mac_roman_us(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	uintptr_t uInputLength = UTF8::GetMacRomanUSSize(pInput, uLength);

	// Ensure the buffer is ready
	const eError uResult = reserve(uInputLength);

	// Only if the buffer could not be resized will it truncate.
	if (uInputLength > m_uBufferSize) {
		uInputLength = m_uBufferSize;
	}
	m_uLength =
		UTF8::FromMacRomanUS(m_pData, uInputLength + 1, pInput, uLength);
	// Return error
	return uResult;
}

/*! ************************************

	\brief Force a buffer size

	Set the buffer to a specific size while retaining the existing string.
	If the preexisting string is too long for the new buffer, it will be
	truncated. The buffer size will be padded to reserve one byte for the
	terminating zero.

	\note If a buffer of 100 bytes is requested, 101 bytes will be allocated to
	hold a string up to 100 characters in length with the 101st byte being the
	terminating zero. The output of Burger::string_length() is acceptable as
	input for a new string.

	\param uSize Number of bytes to set the buffer to
	\return Zero if no error, error code if the buffer couldn't be resized

***************************************/

Burger::eError BURGER_API Burger::String::resize(
	uintptr_t uSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// If the same, early out.
	if (uSize != m_uLength) {

		// If shortening, just truncate and exit.
		if (uSize < m_uLength) {
			m_pData[uSize] = 0;
			m_uLength = uSize;
		} else {

			// Grow while retaining the buffer.
			uResult = reserve(uSize);

			// Only if the buffer could not be resized will it truncate.
			if (uSize > m_uBufferSize) {
				uSize = m_uBufferSize;
			}
			const uintptr_t uFillCount = uSize - m_uLength;

			// Fill in the extra data with zeros plus the terminating zero
			MemoryClear(m_pData + m_uLength, uFillCount + 1);

			m_uLength = uSize;
		}
	}

	// Return error
	return uResult;
}

/*! ************************************

	\brief Ensure a minimum buffer size

	Set the buffer to at least a specific size while retaining the existing
	string. If the preexisting buffer is not large enough, it will be grown to
	accomodate the new buffer. The buffer size will be padded to reserve one
	byte for the terminating zero.

	If the buffer is already large enough to satisfy this request, no operation
	will be performed.

	\note If a buffer of 100 bytes is requested, 101 bytes will be allocated to
	hold a string up to 100 characters in length with the 101st byte being the
	terminating zero. The output of Burger::string_length() is acceptable as
	input for a new string.

	\param uNewBufferSize Number of bytes to set the buffer to

	\return Zero if no error, error code if the buffer couldn't be resized

***************************************/

Burger::eError BURGER_API Burger::String::reserve(
	uintptr_t uNewBufferSize) BURGER_NOEXCEPT
{
	// Assume no error
	eError uResult = kErrorNone;

	// Should it grow?
	if (uNewBufferSize > m_uBufferSize) {

		// Create a new buffer
		char* pDest = static_cast<char*>(Alloc(uNewBufferSize + 1));

		// Oh oh...
		if (!pDest) {
			// Out of memory error!
			// Do nothing.
			uResult = kErrorOutOfMemory;

		} else {

			// Copy data to the new buffer and keep it
			m_uBufferSize = uNewBufferSize;
			const char* pWork = m_pData;
			m_pData = pDest;
			MemoryCopy(pDest, pWork, m_uLength + 1);

			// Discard previous memory if it was allocated
			if (pWork != m_Raw) {
				Free(pWork);
			}
		}
	}

	// Return error
	return uResult;
}

/*! ************************************

	\brief Locate a String in a String.

	Starting from uPosition (Default = 0), scan this string for a match to a
	supplied String and return the offset into this String where the first match
	was found. Return \ref npos if there was no match.

	\note Empty rInput strings always match.

	\param rInput Reference to a String to match.
	\param uPosition Offset in elements of the string that began the match.

	\return 0 to length()-1 or \ref npos if no match.

***************************************/

uintptr_t BURGER_API Burger::String::find(
	const String& rInput, uintptr_t uPosition) const BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// Out of bounds?
	if (uPosition >= m_uLength) {
		// No match
		uResult = npos;
	} else {
		const char* pFound = StringString(m_pData + uPosition, rInput.m_pData);
		if (!pFound) {
			uResult = npos;
		} else {
			uResult = static_cast<uintptr_t>(pFound - m_pData);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Locate a "C" string in a String.

	Starting from uPosition (Default = 0), scan the string for a match to a "C"
	string and return the offset into this string where the first match was
	found. Return \ref npos if there was no match.

	\note If the size of the "C" string to match is zero, it always matches.

	\param pInput Pointer to "C" string to match.
	\param uPosition Offset in elements of this string to begin the match.

	\return 0 to length()-1 or \ref npos if no match.

***************************************/

uintptr_t BURGER_API Burger::String::find(
	const char* pInput, uintptr_t uPosition) const BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// Out of bounds?
	if ((uPosition >= m_uLength) || !pInput) {
		// No match
		uResult = npos;
	} else {
		const char* pFound = StringString(m_pData + uPosition, pInput);
		if (!pFound) {
			uResult = npos;
		} else {
			uResult = static_cast<uintptr_t>(pFound - m_pData);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Locate a buffer in a String.

	Starting from uPosition (Default = 0), scan the string for a match to a
	buffer and return the offset into this string where the first match was
	found. Return \ref npos if there was no match.

	\note If the size of the buffer to match is zero, it always matches.

	\param pInput Pointer to the buffer to match.
	\param uPosition Offset in elements of this string to begin the match.
	\param uInputSize Length of the buffer in elements.

	\return 0 to length()-1 or \ref npos if no match.

***************************************/
uintptr_t BURGER_API Burger::String::find(const char* pInput,
	uintptr_t uPosition, uintptr_t uInputSize) const BURGER_NOEXCEPT
{
	uintptr_t uResult;
	// Out of bounds?
	if (uPosition >= m_uLength) {
		// No match
		uResult = npos;
	} else {
		const char* pFound =
			StringString(m_pData + uPosition, pInput, uInputSize);
		if (!pFound) {
			uResult = npos;
		} else {
			uResult = static_cast<uintptr_t>(pFound - m_pData);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Locate a character in a String.

	Starting from uPosition (Default = 0), scan the string for a character and
	return the offset into the string where the first match was found. Return
	\ref npos if there was no match.

	\param iInput Single ASCII character to match in the string.
	\param uPosition Offset in elements of the string to begin the match.

	\return 0 to length()-1 or \ref npos if no match.

***************************************/

uintptr_t BURGER_API Burger::String::find(
	char iInput, uintptr_t uPosition) const BURGER_NOEXCEPT
{
	uintptr_t uResult;

	// Out of bounds?
	if (uPosition >= m_uLength) {
		// No match
		uResult = npos;
	} else {
		// Perform the search
		const char* pFound = StringCharacter(m_pData + uPosition, iInput);

		// Not found?
		if (!pFound) {
			uResult = npos;
		} else {
			// Return the offset of where in the string the match was found.
			uResult = static_cast<uintptr_t>(pFound - m_pData);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Assign a single character length string to this Burger::String

	If the input is zero, the string is empty.

	\note Since the default encoding is UFT8, input that's greater than
	127 will yield a possibly invalid string due to UTF8 decoding.

	\param iInput Single low ASCII character to create a string.

	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator=(char iInput) BURGER_NOEXCEPT
{
	m_pData[0] = iInput;
	m_uLength = 1;
	m_pData[1] = 0;
	return *this;
}

/*! ************************************

	\brief Assign a "C" string to a Burger::String

	Copy the contents of a UTF8 "C" string into this Burger::String

	\param pInput Pointer to a UTF8 "C" string. \ref nullptr generates an empty
		string.

	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator=(const char* pInput) BURGER_NOEXCEPT
{
	assign(pInput);
	return *this;
}

/*! ************************************

	\brief Assign a 16 bit "C" string to a Burger::String

	Copy the contents of a UTF16 "C" string into this Burger::String

	\param pInput Pointer to a UTF16 "C" string. \ref nullptr generates an empty
		string.

	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator=(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	assign(pInput);
	return *this;
}

/*! ************************************

	\brief Assign a Burger::String

	Copy the contents of a Burger::String into this Burger::String

	\param rInput Reference to a Burger::String to copy from
	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator=(
	const Burger::String& rInput) BURGER_NOEXCEPT
{
	// Am I copying myself?
	if (this != &rInput) {
		assign(rInput);
	}
	return *this;
}

/*! ************************************

	\brief Append a single character to the current string.

	Increase the buffer if needed and append the input character to the existing
	string. If the input character is zero, no operation is performed.

	\param iInput Character to append, zero does nothing.
	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator+=(char iInput) BURGER_NOEXCEPT
{
	push_back(iInput);
	return *this;
}

/*! ************************************

	\brief Append a UTF8 "C" string to the current string.

	Increase the buffer if needed and append the input string to the existing
	string. If the input string is empty, no operation is performed.

	\param pInput Pointer to a UTF8 "C" string to append. \ref nullptr performs
		no operation

	\return A reference to the this object

***************************************/

Burger::String& Burger::String::operator+=(const char* pInput) BURGER_NOEXCEPT
{
	append(pInput);
	return *this;
}

/*! ************************************

	\brief Append a UTF16 "C" string to the current string.

	Increase the buffer if needed and append the input string to the existing
	string. If the input string is empty, no operation is performed.

	\param pInput Pointer to a UTF8 "C" string to append. \ref nullptr performs
		no operation

	\return A reference to the this object

***************************************/

Burger::String& Burger::String::operator+=(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	append(pInput);
	return *this;
}

/*! ************************************

	\brief Append a Burger::String to the current string.

	Increase the buffer if needed and append the input string to the existing
	string. If the input string is empty, no operation is performed.

	\param rInput Reference to string to append
	\return A reference to the this pointer

***************************************/

Burger::String& Burger::String::operator+=(
	const Burger::String& rInput) BURGER_NOEXCEPT
{
	append(rInput);
	return *this;
}

/*! ************************************

	\fn Burger::String::operator () (uintptr_t uStart,uintptr_t uLength) const
	\brief Create a new Burger::String from a substring

	Given the start and end offsets of a string, create a new string with those
	offsets and return the resulting string

	\param uStart Offset to the starting character of the new string
	\param uLength Number of characters to copy

	\return A class instance of Burger::String containing the new string.

	\sa Burger::String::String(const Burger::String &,uintptr_t,uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String::operator () (uintptr_t uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String::operator [] (uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String::operator () (uintptr_t uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String::operator [] (uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::String::operator [] (uintptr_t uInput)
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A reference to the character in the string.

	\sa Burger::String::operator () (uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::String::operator [] (uintptr_t uInput) const
	\brief Return the reference to a location in the string

	Given an offset into the string, return the reference to the character.
	If the offset exceeds the length of the string, it will be clamped
	to the terminating zero.

	\param uInput Offset to the starting character of the new string
	\return A constant reference to the character in the string.

	\sa Burger::String::operator () (uintptr_t) const

***************************************/

/*! ************************************

	\brief Return a single character from the string

	If the index is beyond the end if the string, the
	function will return a zero.

	\param uWhere Index to fetch a character from
	\return Character found in the string or zero if the index is beyond the end
		of the string.

***************************************/

char BURGER_API Burger::String::Get(uintptr_t uWhere) const BURGER_NOEXCEPT
{
	if (uWhere < m_uLength) {   // In bounds?
		return m_pData[uWhere]; // Fetch it
	}
	return 0; // Return zilch
}

/*! ************************************

	\brief Modify a single character in the string.

	\note This function will NOT increase the size of the
	string.
	\param uWhere Offset into the string to write to.
	\param cInput Character to update the string with.

***************************************/

void BURGER_API Burger::String::Put(
	uintptr_t uWhere, char cInput) BURGER_NOEXCEPT
{
	if (uWhere < m_uLength) {     // In bounds?
		m_pData[uWhere] = cInput; // Store it
	}
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

	Make a copy of the string with all of the ASCII uppercase characters
	converted to lowercase.

	\return New instance of the string class in lower case

***************************************/

/*! ************************************

	\fn Burger::String::Uppercase(void) const
	\brief Return an uppercase copy of the string

	Make a copy of the string with all of the ASCII lowercase characters
	converted to uppercase.

	\return New instance of the string class in upper case

***************************************/

/*! ************************************

	\fn Burger::String::operator ! () const
	\brief Return \ref TRUE if the string is empty

	\return \ref TRUE on an empty string, \ref FALSE if there are characters in
		the string

***************************************/

/*! ************************************

	\fn Burger::String::is_valid(void) const
	\brief Return \ref TRUE if the string has characters

	\return \ref FALSE on an empty string, \ref TRUE if there are characters in
		the string

***************************************/

/*! ************************************

	\brief Clear out the data

	Set the string to an empty string

***************************************/

void BURGER_API Burger::String::clear(void) BURGER_NOEXCEPT
{
	// Old data pointer
	const char* pWork = m_pData;

	// Dispose of the data?
	if (pWork != m_Raw) {
		Free(pWork);
	}

	// Reset to power up defaults
	m_pData = m_Raw;
	m_uLength = 0;
	m_uBufferSize = kBufferSize - 1;
	m_Raw[0] = 0;
}

/*! ************************************

	\brief Extract the string into a buffer of a specific size

	This will truncate the string if the buffer is too small

	\param pOutput Pointer to a buffer to receive the "C" string
	\param uOutputSize Size in bytes of the output buffer
	\return Length of the string in bytes without the terminating zero

	\sa Burger::String::PCopy(uint8_t *,uintptr_t) const

***************************************/

uintptr_t BURGER_API Burger::String::copy(
	char* pOutput, uintptr_t uOutputSize) const BURGER_NOEXCEPT
{
	// Get the size of the current string
	const uintptr_t uLength = m_uLength;

	// Set the maximum
	if (uOutputSize > uLength) {
		// Truncate
		uOutputSize = uLength;
	}
	// Copy the string
	MemoryCopy(pOutput, m_pData, uOutputSize);

	return uOutputSize;
}

/*! ************************************

	\brief Extract the string into a Pascal buffer of a specific size

	This will truncate the string if the buffer is too small

	\note Pascal strings have a hard limit of 256 byte (255 characters). If the
	buffer exceeds this length, it will be truncated

	\note The resulting string will NOT have a terminating zero, so it must
	manually be converted into a "C". It is not a dual "C" string and Pascal
	string.

	\param pOutput Buffer of up a maximum of 256 bytes in length
	\param uOutputSize Size of the buffer in bytes. If the value is greater than
		256 it will be truncated to 256.

	\sa Burger::String::Copy(char *,uintptr_t) const

***************************************/

uintptr_t BURGER_API Burger::String::PCopy(
	uint8_t* pOutput, uintptr_t uOutputSize) const BURGER_NOEXCEPT
{
	// There must be space for the length byte
	if (uOutputSize) {

		// Pascal strings maximum length
		if (uOutputSize > 256) {
			uOutputSize = 256U;
		}

		// Space for the "Length"
		--uOutputSize;

		// Get the size of the current string
		const uintptr_t uLength = m_uLength;
		// Set the maximum
		if (uOutputSize > uLength) {
			// Truncate
			uOutputSize = uLength;
		}
		// Pascal length
		pOutput[0] = static_cast<uint8_t>(uOutputSize);
		// Copy the string
		MemoryCopy(pOutput + 1, m_pData, uOutputSize);
	}
	return uOutputSize;
}

/*! ************************************

	\brief Remove a part of the string.

	This routine will not resize the buffer

	\param uStart Starting index into the string for the section to remove
	\param uLength Number of bytes to remove from the string

	\return Number of bytes of remaining string but not including the
		terminating zero

***************************************/

uintptr_t BURGER_API Burger::String::Remove(
	uintptr_t uStart, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Get the string's length
	uintptr_t uMyLength = m_uLength;

	// Start off the end?
	if (uStart < uMyLength) {

		// Start pointer
		char* pWork = &m_pData[uStart];
		// Real maximum number of bytes I can remove
		uintptr_t uRemainingLength = uMyLength - uStart;

		// Truncation?
		if (uLength >= uRemainingLength) {
			// End the string here
			pWork[0] = 0;
			// Return this as the new length
			uMyLength = uStart;
			// Save the new length
			m_uLength = uStart;
		} else {

			// Am I removing anything?
			if (uLength) {

				// Adjust the length
				uMyLength -= uLength;
				m_uLength = uMyLength;
				// +1 for the zero terminator
				uRemainingLength = (uRemainingLength - uLength) + 1;
				// I have the loop here so I'm not calling a subroutine
				do {
					// Copy the string
					pWork[0] = pWork[uLength];
					// Next byte
					++pWork;
					// All done?
				} while (--uRemainingLength);
			}
		}
	}

	// Return the new length
	return uMyLength;
}

/*! ************************************

	\brief Insert a string into a current String

	If uStart is beyond the end of the string, it's placed at the end of the
	string.

	If Text points to a "C" string that is smaller than MaxLen, then only the
	length of the "C" string is used

	\param uStart Index to insert the new characters
	\param pInput Pointer to the string to insert
	\param uLength Number of bytes to insert

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError BURGER_API Burger::String::insert(
	uintptr_t uStart, const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// No insertion?
	if (!uLength) {
		return kErrorNone;
	}

	// Check if this an append
	const uintptr_t uOriginalLength = m_uLength;
	if (uStart >= uOriginalLength) {
		// Perform append operation
		return append(pInput, uLength);
	}

	// Adjust the buffer size
	const uintptr_t uTotalLength = uOriginalLength + uLength;
	const eError uResult = reserve(uTotalLength);
	if (!uResult) {

		m_uLength = uTotalLength;
		// Move tail end of the string to make room new data
		uintptr_t uInsertOffset = uStart + uLength;
		MemoryMove(m_pData + uInsertOffset, m_pData + uStart,
			(uOriginalLength - uStart) + 1);

		// Insert the new data
		MemoryCopy(m_pData + uStart, pInput, uLength);
	}
	return uResult;
}

/*! ************************************

	\brief Append a single character to the current string.

	Increase the buffer if needed and append the input character to the existing
	string. If the input character is zero, no operation is performed.

	\param iInput Character to append, zero does nothing.

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError Burger::String::push_back(char iInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Check if there is already room.
	uintptr_t uInputLen1 = m_uLength;
	if (uInputLen1 >= m_uBufferSize) {

		// Increase the buffer a bit
		uResult = reserve(uInputLen1 + (uInputLen1 >> 1));
		if (uResult) {
			// No padding?
			uResult = reserve(uInputLen1 + 1);
		}

		// No error? Append it
		if (!uResult) {
			// Append it
			char* pDest2 = m_pData;
			pDest2[uInputLen1] = iInput;
			pDest2[uInputLen1 + 1] = 0;
			m_uLength = uInputLen1 + 1;
		}

	} else {
		// Append it
		char* pDest1 = m_pData;
		pDest1[uInputLen1] = iInput;
		pDest1[uInputLen1 + 1] = 0;
		m_uLength = uInputLen1 + 1;
	}
	return uResult;
}

/*! ************************************

	\brief Remove the last character from a string.

	If there is data, remove the last entry, update the length and then return
	the removed character. If the string is empty, zero is returned and no other
	action is performed.

	\return Last character in the string, or zero if the string is empty.

***************************************/

char BURGER_API Burger::String::pop_back(void) BURGER_NOEXCEPT
{
	uintptr_t uLength = m_uLength;
	char iResult;

	// Any data?
	if (!uLength) {
		iResult = 0;
	} else {
		// Remove the last entry
		--uLength;
		// Capture it
		iResult = m_pData[uLength];
		// Set the new terminating zero
		m_pData[uLength] = 0;
		m_uLength = uLength;
	}
	return iResult;
}

/*! ************************************

	\brief Append a UTF8 string buffer to the current string.

	Increase the buffer if needed and append the input string to the
	existing string. If the input string is empty, no operation is performed.

	\param pInput Pointer to a UTF8 string to append. \ref NULL performs no
		operation
	\param uInputSize Number of bytes to append

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError BURGER_API Burger::String::append(
	const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Check if there is already room.
	uintptr_t uInputLength1 = m_uLength;
	uintptr_t uTotalLength = uInputLength1 + uInputSize;
	if (m_uBufferSize < uTotalLength) {

		// Increase the buffer a bit
		uResult = reserve(uTotalLength + (uTotalLength >> 1));
		if (uResult) {
			// No padding?
			uResult = reserve(uTotalLength + 1);
		}

		// No error? Append it
		if (!uResult) {
			// Append it
			MemoryCopy(m_pData + uInputLength1, pInput, uInputSize);
			m_pData[uTotalLength] = 0;
			m_uLength = uTotalLength;
		}

	} else {
		// Append it
		MemoryCopy(m_pData + uInputLength1, pInput, uInputSize);
		m_pData[uTotalLength] = 0;
		m_uLength = uTotalLength;
	}
	return uResult;
}

/*! ************************************

	\brief Append a UTF8 string buffer to the current string.

	Increase the buffer if needed and append the input string to the
	existing string. If the input string is empty or \ref nullptr , no operation
	is performed.

	\param pInput Pointer to a "C" string to append

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError BURGER_API Burger::String::append(
	const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput) {
		return kErrorNone;
	}
	return append(pInput, string_length(pInput));
}

/*! ************************************

	\brief Append a UTF16 string buffer to the current string.

	Increase the buffer if needed and append the input string to the
	existing string. If the input string is empty or \ref nullptr , no operation
	is performed.

	\param pInput Pointer to a UTF16 "C" string to append

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError BURGER_API Burger::String::append(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (pInput) {

		// Perform a quick conversion
		String TempString;

		// Capture any errors.
		uResult = TempString.assign(pInput);
		if (!uResult) {
			// Append it.
			uResult = append(TempString);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Append a UTF8 string buffer to the current string.

	Increase the buffer if needed and append the input string to the
	existing string. If the input string is empty, no operation is performed.

	\param rInput Reference to the String to append with

	\return Zero if no error, non-zero on memory error.

***************************************/

Burger::eError BURGER_API Burger::String::append(
	const String& rInput) BURGER_NOEXCEPT
{
	return append(rInput.m_pData, rInput.m_uLength);
}

/*! ************************************

	\brief Resize a string and either truncate or append with a fill
		character

	Force the string to be a specific size. If the string grows, fill in the
	extra space with the pad character. Retain the preexisting string from index
	zero to index uNewLength

	\param uNewLength Number of bytes the string will occupy
	\param iInput character to fill the buffer with in case of growth
	\return Reference to the Burger::String

***************************************/

Burger::String& BURGER_API Burger::String::Left(
	uintptr_t uNewLength, char iInput) BURGER_NOEXCEPT
{
	uintptr_t uOldLen = m_uLength; // Get the current length
	m_uLength = uNewLength;        // Set the new length
	char* pWork = m_pData;         // Buffer pointer
	if (uNewLength > uOldLen) {    // Did it grow?
		char* pDest;
		if (uNewLength < kBufferSize) {
			pDest = m_Raw; // Used the internal buffer
		} else {
			// Get a new string
			pDest = static_cast<char*>(Alloc(uNewLength + 1));
			if (!pDest) {
				m_uLength = 0; // No memory!!!
				m_Raw[0] = 0;
				m_pData = m_Raw;
				m_uBufferSize = kBufferSize - 1;
				return *this;
			}
			m_uBufferSize = uNewLength;
			MemoryCopy(pDest, pWork, uOldLen);
		}
		m_pData = pDest;       // Save the new buffer pointer
		pDest[uNewLength] = 0; // End it
		// Fill in the extra
		MemoryFill(pDest + uOldLen, static_cast<uint8_t>(iInput),
			uNewLength - uOldLen);
		// Get rid of the old string?
		if (pWork != m_Raw) {
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

	\brief Resize a string and either truncate or prepend with a fill
		character

	Force the string to be a specific size. If the string grows, fill in the
	extra space with the pad character. Note: I am padding the string from the
	"left" side. I.E. the string is moved to the right.

	\param uNewLength Number of bytes the string will occupy
	\param iInput character to fill the buffer with in case of growth
	\return Reference to the Burger::String

***************************************/

Burger::String& BURGER_API Burger::String::Right(
	uintptr_t uNewLength, char iInput) BURGER_NOEXCEPT
{
	uintptr_t uOldLen = m_uLength; // Get the current length
	m_uLength = uNewLength;        // Set the new length
	char* pWork = m_pData;         // Buffer pointer
	if (uNewLength > uOldLen) {    // Did it grow?
		char* pDest;
		if (uNewLength < kBufferSize) {
			pDest = m_Raw; // Used the internal buffer
		} else {
			pDest = static_cast<char*>(Alloc(uNewLength + 1));
			if (!pDest) {
				m_uLength = 0; // No memory!!!
				m_Raw[0] = 0;
				m_pData = m_Raw;
				m_uBufferSize = kBufferSize - 1;
				return *this;
			}
			m_uBufferSize = uNewLength;
			// Copy the old string
			MemoryCopy(pDest, pWork, uOldLen);
		}
		m_pData = pDest; // Save the new buffer pointer
		// Copy over the characters from the right
		MemoryMove(pDest + uNewLength - uOldLen, pDest, uOldLen + 1);
		// Fill in the extra
		MemoryFill(pDest, static_cast<uint8_t>(iInput), uNewLength - uOldLen);
		// Get rid of the old string?
		if (pWork != m_Raw) {
			// Bye bye
			Free(pWork);
		}
	} else {
		// Copy over the characters from the right
		MemoryMove(pWork, pWork + uOldLen - uNewLength, uNewLength + 1);
	}
	return *this;
}

/*! ************************************

	\brief Test if the string starts with a char

	If the string is not empty, test the first character for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty.

	\note Test is case sensitive.

	\param iInput Character to match

	\return \ref TRUE if a match, \ref FALSE if not.

	\sa ends_with(char)

***************************************/

uint_t BURGER_API Burger::String::starts_with(char iInput) BURGER_NOEXCEPT
{
	uint_t uResult;
	const uintptr_t uLength = m_uLength;
	if (!uLength) {
		uResult = FALSE;
	} else {
		uResult = m_pData[0] == iInput;
	}
	return uResult;
}

/*! ************************************

	\brief Test if the string starts with a "C" string

	If the string is not empty, test the input "C" string for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty. If the
	input string is empty, always return \ref FALSE.

	\note Test is case sensitive.

	\param pInput "C" string to match. \ref nullptr will return \ref FALSE

	\return \ref TRUE if a match, \ref FALSE if not.

	\sa ends_with(const char *)

***************************************/

uint_t BURGER_API Burger::String::starts_with(
	const char* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		return starts_with(pInput, string_length(pInput));
	}
	return FALSE;
}

/*! ************************************

	\brief Test if the string starts with a buffer of data.

	If the string is not empty, test the input buffer for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty. If the
	input buffer is empty, always return \ref FALSE.

	\note Test is case sensitive.

	\param pInput Text buffer to match.
	\param uLength Length of the text buffer. Zero will always return \ref FALSE
	\return \ref TRUE if a match, \ref FALSE if not.

	\sa ends_with(const char *, uintptr_t)

***************************************/

uint_t BURGER_API Burger::String::starts_with(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	uint_t uResult;
	const uintptr_t uStringLength = m_uLength;

	// Check if an invalid compare.
	if (!uLength || (uLength > uStringLength)) {
		// Always fail.
		uResult = FALSE;
	} else {
		// Compare the memory
		uResult = MemoryCompare(m_pData, pInput, uLength) == 0;
	}
	return uResult;
}

/*! ************************************

	\brief Test if the string ends with a char

	If the string is not empty, test the last character for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty.

	\note Test is case sensitive.

	\param iInput Character to match

	\return \ref TRUE if a match, \ref FALSE if not.

	\sa starts_with(char)

***************************************/

uint_t BURGER_API Burger::String::ends_with(char iInput) BURGER_NOEXCEPT
{
	uint_t uResult;
	const uintptr_t uLength = m_uLength;
	if (!uLength) {
		uResult = FALSE;
	} else {
		uResult = m_pData[uLength - 1] == iInput;
	}
	return uResult;
}

/*! ************************************

	\brief Test if the string ends with a "C" string

	If the string is not empty, test the input "C" string for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty. If the
	input string is empty, always return \ref FALSE.

	\note Test is case sensitive.

	\param pInput "C" string to match. \ref nullptr will return \ref FALSE

	\return \ref TRUE if a match, \ref FALSE if not.

	\sa starts_with(const char *)

***************************************/

uint_t BURGER_API Burger::String::ends_with(const char* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		return ends_with(pInput, string_length(pInput));
	}
	return FALSE;
}

/*! ************************************

	\brief Test if the string ends with a buffer of data.

	If the string is not empty, test the input buffer for a match and return
	\ref TRUE if so. Always return \ref FALSE if the string is empty. If the
	input buffer is empty, always return \ref FALSE.

	\note Test is case sensitive.

	\param pInput Text buffer to match.
	\param uLength Length of the text buffer. Zero will always return \ref FALSE
	\return \ref TRUE if a match, \ref FALSE if not.

	\sa starts_with(const char *, uintptr_t)

***************************************/

uint_t BURGER_API Burger::String::ends_with(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	uint_t uResult;
	const uintptr_t uStringLength = m_uLength;

	// Check if an invalid compare.
	if (!uLength || (uLength > uStringLength)) {
		// Always fail.
		uResult = FALSE;
	} else {
		// Compare the memory at the end
		uResult = MemoryCompare(m_pData + (uStringLength - uLength), pInput,
					  uLength) == 0;
	}
	return uResult;
}

/*! ************************************

	\fn int Burger::String::Compare(const Burger::String &rInput) const

	\brief Compare the string

	Compare two strings for equality using the rules found in the
	ANSI function strcmp(). This is a functional equivalent

	\param rInput Reference to a Burger::String to compare against.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

/*! ************************************

	\brief Compare the string

	Compare two strings for equality using the rules found in the ANSI function
	strcmp(). This is a functional equivalent

	\param pInput Pointer to a "C" string to compare against. \ref nullptr will
		perform as if an empty string was passed in.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

int BURGER_API Burger::String::Compare(const char* pInput) const BURGER_NOEXCEPT
{
	if (!pInput) {
		pInput = g_EmptyString;
	}
	return StringCompare(m_pData, pInput);
}

/*! ************************************

	\brief Compare the string to a single character

	Compare two strings for equality using the rules found in the ANSI function
	strcmp(). This is a functional equivalent

	\param iInput Character to convert to a string of a single character in
		length. Zero will perform as if an empty string was passed in.

	\return 0 if equal, positive if greater and negative if lessor

***************************************/

int BURGER_API Burger::String::Compare(char iInput) const BURGER_NOEXCEPT
{
	const char Temp[2] = {iInput, 0};
	return StringCompare(m_pData, Temp);
}

/*! ************************************

	\brief Remove each and every char in the list

	Given a string containing characters, remove the characters found from
	the current string. Duplicating characters in the input string will reduce
	performance slightly. The order of the characters is not important.

	\param pInput Pointer to a "C" string containing the characters to
	remove. \ref nullptr performs no action

	\return Number of bytes of remaining string but not including the
		terminating zero

	\sa RemoveChar(char)

***************************************/

uintptr_t BURGER_API Burger::String::RemoveChars(
	const char* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		if (uTemp) {
			do {
				// Accept the char
				++pInput;

				// Remove this char
				RemoveChar(static_cast<char>(uTemp));

				// Next char in the list
				uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];

				// Not the end of the list?
			} while (uTemp);
		}
	}

	// Return the new length
	return m_uLength;
}

/*! ************************************

	\brief Remove a specific character from the string

	Find all occurrences of the character and remove them. The string will
	be reduced in size if necessary.

	\param iInput Character to remove from the string. Zero will perform no
		action

	\return Number of bytes of remaining string but not including the
		terminating zero

	\sa RemoveChars(const char *)

***************************************/

uintptr_t BURGER_API Burger::String::RemoveChar(char iInput) BURGER_NOEXCEPT
{
	uintptr_t uLength = m_uLength;
	// Should I bother?
	if (iInput && uLength) {
		// Get the pointer
		uint8_t* pWork = reinterpret_cast<uint8_t*>(m_pData);
		uint8_t* pDest = pWork;
		do {
			// Get the char
			const uint_t uTemp = pWork[0];

			// Accept it
			++pWork;
			if (uTemp != static_cast<uint8_t>(iInput)) {
				pDest[0] = static_cast<uint8_t>(uTemp); // Keep this
				++pDest;
			}
		} while (--uLength);
		// Zero terminate
		pDest[0] = 0;

		// Set the new length
		uLength =
			static_cast<uintptr_t>(pDest - reinterpret_cast<uint8_t*>(m_pData));

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

	\return Number of bytes of remaining string but not including the
		terminating zero

	\sa RemoveChar(char)

***************************************/

uintptr_t BURGER_API Burger::String::NormalizeLineFeeds(void) BURGER_NOEXCEPT
{
	uintptr_t uLength = m_uLength;
	// Should I bother?
	if (uLength) {
		// Get the pointer
		uint8_t* pWork = reinterpret_cast<uint8_t*>(m_pData);
		uint8_t* pDest = pWork;
		do {
			uint_t uTemp = pWork[0]; // Get the char
			++pWork;                 // Accept it
			if (uTemp == '\r') {     // CR?
				// Is this a CR/LF case?
				if ((uLength >= 2) && (pWork[0] == '\n')) {
					++pWork; // Remove the 2nd characters
					--uLength;
				}
				uTemp = '\n'; // Force a LF into the output
			}
			pDest[0] = static_cast<uint8_t>(uTemp); // Keep this
			++pDest;
		} while (--uLength);

		// Zero terminate
		pDest[0] = 0;

		// Set the new length
		uLength =
			static_cast<uintptr_t>(pDest - reinterpret_cast<uint8_t*>(m_pData));

		// Save the new length if needed
		m_uLength = uLength;
	}
	// Return the new length
	return uLength;
}

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::String::GetBoolean(
		uint_t bDefault) const

	\brief Convert an ASCII string into a boolean.

	Convert the string to a \ref TRUE or a \ref FALSE. If the input is
	\ref nullptr or invalid, return the default value.

	"true" and "yes" are considered \ref TRUE while "false" and "no" are
	considered \ref FALSE. The comparison is case insensitive.

	\param bDefault Default boolean to return in the event of a parsing
		error.

	\return \ref TRUE or \ref FALSE

	\sa AsciiToBoolean(const char *,const char **) or AsciiToBoolean(
		const char *,uint_t)

***************************************/

/*! ************************************

	\brief Set the string to "true" or "false"

	If the input value is zero, set the string to "false", otherwise set the
	string to "true".

	\param bInput Value to store as a string
	\sa GetBoolean(uint_t) const or SetYesNo(uint_t)

***************************************/

void BURGER_API Burger::String::SetTrueFalse(uint_t bInput) BURGER_NOEXCEPT
{
	const char* pSource;
	uintptr_t uLength;
	// Choose the string
	if (bInput) {
		pSource = g_TrueString;
		uLength = sizeof(g_TrueString) - 1;
	} else {
		pSource = g_FalseString;
		uLength = sizeof(g_FalseString) - 1;
	}
	assign_small(pSource, uLength);
}

/*! ************************************

	\brief Set the string to "true" or "false"

	If the input value is zero, set the string to "false", otherwise set the
	string to "true".

	\param bInput Value to store as a string
	\sa GetBoolean(uint_t) const or SetTrueFalse(uint_t)

***************************************/

void BURGER_API Burger::String::SetYesNo(uint_t bInput) BURGER_NOEXCEPT
{
	const char* pSource;
	uintptr_t uLength;
	// Choose the string
	if (bInput) {
		pSource = g_YesString;
		uLength = sizeof(g_YesString) - 1;
	} else {
		pSource = g_NoString;
		uLength = sizeof(g_NoString) - 1;
	}
	assign_small(pSource, uLength);
}

/*! ************************************

	\fn BURGER_INLINE uint_t Burger::String::GetWord(uint_t uDefault,
		uint_t uMin,uint_t uMax) const

	\brief Return an unsigned integer value

	Scan the value string as a 32 bit unsigned integer or hex value and if
	successful, test it against the valid range and return the value clamped
	to that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form of $1234 and 0x1234

	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa GetInt(int_t,int_t,int_t) const, SetWord(uint_t),
		SetWordHex(uint_t), or AsciiToWord(const char *,uint_t,uint_t,uint_t)

***************************************/

/*! ************************************

	\brief Set an unsigned integer value

	Convert the input into an unsigned integer UTF-8 string and set the
	value to this string

	\param uInput Value to store as an unsigned integer string
	\sa GetWord(uint_t,uint_t,uint_t) const or SetWordHex(uint_t)

***************************************/

void BURGER_API Burger::String::SetWord(uint_t uInput) BURGER_NOEXCEPT
{
	m_uLength = static_cast<uintptr_t>(
		NumberToAscii(m_pData, static_cast<uint32_t>(uInput)) - m_pData);
}

/*! ************************************

	\brief Set an unsigned integer value as hex

	Convert the input into a "C" style hex string in the format of
	0x12345678 and store this string as the value

	\param uValue Value to store as an unsigned integer hex string
	\sa GetWord(uint_t,uint_t,uint_t) const or SetWord(uint_t)

***************************************/

void BURGER_API Burger::String::SetWordHex(uint_t uValue) BURGER_NOEXCEPT
{
	m_pData[0] = '0';
	m_pData[1] = 'x';
	m_uLength = static_cast<uintptr_t>(
		to_hex_ascii(m_pData + 2, static_cast<uint32_t>(uValue), 0) -
		m_pData);
}

/*! ************************************

	\fn int_t Burger::String::GetInt(int_t iDefault,int_t iMin,
		int_t iMax) const

	\brief Return a signed integer value

	Scan the value string as a 32 bit signed integer or hex value and if
	successful, test it against the valid range and return the value clamped
	to that range. If it's not a number, return the default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa GetWord(uint_t,uint_t,uint_t) const, SetInt(int_t) or
		AsciiToInteger(const char *,int_t,int_t,int_t)

***************************************/

/*! ************************************

	\brief Set a signed integer value

	Convert the input into an signed integer UTF-8 string and set the value to
	this string

	\param iInput Value to store as a signed integer string
	\sa GetInt(int_t,int_t,int_t) const or SetWord(uint_t)

***************************************/

void BURGER_API Burger::String::SetInt(int_t iInput) BURGER_NOEXCEPT
{
	m_uLength = static_cast<uintptr_t>(
		NumberToAscii(m_pData, static_cast<int32_t>(iInput)) - m_pData);
}

/*! ************************************

	\fn BURGER_INLINE float Burger::String::GetFloat(float fDefault) const
	\brief Return a floating point value

	Scan the value string as a 32 bit floating point numeric value and if
	successful, return it. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param fDefault Value to return on error

	\return Value or fDefault

	\sa GetDouble(double) const, SetFloat(float) or AsciiToFloat(
		const char *,float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::String::GetFloat(float fDefault,float
		fMin,float fMax) const

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point numeric value and if
	successful, test it against the valid range and return the value clamped
	to that range. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault

	\sa GetDouble(double,double,double) const, SetFloat(float) or
		AsciiToFloat(const char *,float,float,float)

***************************************/

/*! ************************************

	\brief Set a 32 bit floating point value

	Convert the input into an floating point representation of a UTF-8
	string and set the value to this string

	\param fValue Value to store as a floating point string
	\sa GetFloat(float,float,float) const or SetDouble(double)

***************************************/

void BURGER_API Burger::String::SetFloat(float fValue) BURGER_NOEXCEPT
{
	ArgumentType Arg(fValue);
	const ArgumentType* Args[1] = {&Arg};
	InitFormattedString("%f", 1, Args);
}

/*! ************************************

	\fn double Burger::String::GetDouble(double dDefault) const

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point numeric value and if
	successful, return it. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param dDefault Value to return on error
	\return Value or dDefault

	\sa GetFloat(float) const, SetDouble(double) or AsciiToDouble(
		const char *,double)

***************************************/

/*! ************************************

	\fn double Burger::String::GetDouble(double dDefault,double dMin,
		double dMax) const

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point numeric value and if
	successful, test it against the valid range and return the value clamped
	to that range. If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero to prevent
	floating point issues.

	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault

	\sa GetFloat(float,float,float) const, SetDouble(double) or
		AsciiToDouble(const char *,double,double,double)

***************************************/

/*! ************************************

	\brief Set a 64 bit floating point value

	Convert the input into an floating point representation of a UTF-8
	string and set the value to this string

	\param dValue Value to store as a 64 bit floating point string
	\sa GetDouble(double,double,double) const or SetFloat(float)

***************************************/

void BURGER_API Burger::String::SetDouble(double dValue) BURGER_NOEXCEPT
{
	ArgumentType Arg(dValue);
	const ArgumentType* Args[1] = {&Arg};
	InitFormattedString("%f", 1, Args);
}

void BURGER_API Burger::String::InitFormattedString(const char* pFormat,
	uintptr_t uArgCount, const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Remove any previously allocated buffer
	clear();
	if (!pFormat || !pFormat[0]) {
		return;
	}

	if (uArgCount && ppArgs) {
		if (!SprintfUserAlloc(FormattedAllocCallback, this, TRUE, pFormat,
				uArgCount, ppArgs)) {
			clear();
		}
	} else {
		assign(pFormat);
	}
}

uint_t BURGER_API Burger::String::FormattedAllocCallback(uint_t bNoErrors,
	uintptr_t uRequestedSize, void** ppOutputBuffer,
	void* pContext) BURGER_NOEXCEPT
{
	if (!bNoErrors || !pContext || !uRequestedSize) {
		return FALSE; // Abort
	}
	String* theString = static_cast<String*>(pContext);
	theString->resize(uRequestedSize);
	ppOutputBuffer[0] = theString->m_pData;
	return TRUE; // Proceed
}

/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the strings are equal (Case sensitive)

	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the strings are equal (Case sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (const char *pInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the strings are equal (Case sensitive)

	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (Burger::String const &rInput1,
		char cInput2)
	\brief Return \ref TRUE if the strings are equal (Case sensitive)

	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against

	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator == (char cInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the strings are equal (Case sensitive)

	\param cInput1 Single character as a string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the strings are equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the strings are not equal (Case sensitive)
	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the strings are not equal (Case sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the strings are not equal

***************************************/

/*! ************************************

	\fn Burger::String::operator != (const char *pInput1,
		Burger::String const &rInput2)

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

	\fn Burger::String::operator < (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is less than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the first is less than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (const char *pInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is less than the second (Case
		sensitive)

	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (Burger::String const &rInput1,char cInput2)

	\brief Return \ref TRUE if the first is less than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against

	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator < (char cInput1,Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is less than the second (Case
		sensitive)

	\param cInput1 Single character as a string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is less than or equal to the second
		(Case sensitive)

	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the first is less than or equal to the second
		(Case sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (const char *pInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is less than or equal to the second
		(Case sensitive)

	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (Burger::String const &rInput1,char cInput2)

	\brief Return \ref TRUE if the first is less than or equal to the second
		(Case sensitive)

	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against

	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator <= (char cInput1,Burger::String const &rInput2)

	Return \ref TRUE if the first is less than or equal to the
		second (Case sensitive)

	\param cInput1 Single character as a string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the first is greater than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (const char *pInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than the second (Case
		sensitive)

	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (Burger::String const &rInput1, char cInput2)

	\brief Return \ref TRUE if the first is greater than the second (Case
		sensitive)

	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against

	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator > (char cInput1, Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than the second (Case
		sensitive)

	\param cInput1 Single character as a string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the
		second (Case sensitive)

	\param rInput1 Burger::String to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,
		const char *pInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the
		second (Case sensitive)

	\param rInput1 Burger::String to test
	\param pInput2 Pointer to "C" string to test against

	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (const char *pInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the
		second (Case sensitive)

	\param pInput1 Pointer to "C" string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (Burger::String const &rInput1,
		char cInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the
		second (Case sensitive)

	\param rInput1 Burger::String to test
	\param cInput2 Single character as a string to test against

	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::String::operator >= (char cInput1,
		Burger::String const &rInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the
		second (Case sensitive)

	\param cInput1 Single character as a string to test
	\param rInput2 Burger::String to test against

	\return \ref TRUE if the first is greater than or equal to the second

***************************************/
