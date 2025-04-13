/***************************************

	Simple String Manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsimplestring.h"
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::SimpleString
	\brief Simple "C" string container.

	Most times, an application or class needs to store a copy of
	a string of unknown length. This class will take a string, allocate
	memory to hold it and make a copy so the application doesn't
	have to worry about managing the buffer.
	
	The first and foremost goal of this class is to use the least
	amount of memory to store a string. This class only has a 
	single member of a const char * to the string so it only
	occupies 4 or 8 bytes when it's a member of a higher class.
	If no string is contained, it will point to a global Burger::g_EmptyString
	so no memory is allocated if no string is assigned. Only
	when a string is assigned, will a string be allocated.
	
	Because of the nature of the way the string is managed, it is
	not recommended to use this class for strings that are constantly
	modified or updated. You may wish to consider using a 
	Burger::String instead.

***************************************/

/*! ************************************

	\fn Burger::SimpleString::SimpleString()
	\brief Default constructor.
	
	Assign the global Burger::g_EmptyString string ("") and
	exit. No memory allocation is performed.
	
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

/*! ************************************

	\brief Default constructor for a single char.
	
	If the input is zero, then the global Burger::g_EmptyString string ("")
	is used, otherwise a 2 byte buffer is allocated and a string
	containing the single character and a terminating zero is created
	and assigned.
	
	\param cInput Single char to turn into a 1 character length string. Zero will create an empty string.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(char cInput)
{
	const char* pOutput = g_EmptyString; // Empty
	if (cInput) {                        // Valid?
		char* pTemp = static_cast<char*>(Alloc(2));
		if (pTemp) {
			pTemp[0] = cInput;
			pTemp[1] = 0;
			pOutput = pTemp;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default constructor for a single "C" string.
	
	If the input is \ref NULL or an empty string, the global
	Burger::g_EmptyString string ("") is used, otherwise a buffer is
	allocated and the string will be copied and assigned.
	
	\param pInput1 Pointer to input string. \ref NULL or an empty string is acceptable.
	\sa Burger::SimpleString::SimpleString(const char *,const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const char *pInput1)
{
	const char *pOutput = g_EmptyString;		// Empty
	if (pInput1 && pInput1[0]) {		// Non empty string?
		char *pTemp = StringDuplicate(pInput1);
		if (pTemp) {
			pOutput = pTemp;
		}
	}
	m_pData = pOutput;		// Empty
}

/*! ************************************

	\brief Default constructor for concatinated "C" strings.
	
	Take the passed "C" strings and create a "C" string by concatinating
	all of them together into a single unified string. If any
	of the inputs are \ref NULL or empty strings, they will be ignored.
	If the resulting string is empty, the global
	Burger::g_EmptyString string ("") is used internally, otherwise a buffer is
	allocated and the string(s) will be copied, concatinated and assigned
	to this class instance.
	
	\param pInput1 Pointer to the first input string. \ref NULL or an empty string is acceptable.
	\param pInput2 Pointer to the second input string. \ref NULL or an empty string is acceptable.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const char *pInput1,const char *pInput2)
{
	uintptr_t uLength1 = 0;
	if (pInput1) {
		uLength1 = StringLength(pInput1);
	}
	uintptr_t uLength2 = 0;
	if (pInput2) {
		uLength2 = StringLength(pInput2);
	}
	const char *pOutput = g_EmptyString;		// Empty
	uintptr_t uLength = uLength1+uLength2;
	if (uLength) {
		char *pTemp = static_cast<char *>(Alloc(uLength+1U));
		if (pTemp) {
			pOutput = pTemp;
			MemoryCopy(pTemp,pInput1,uLength1);
			pTemp += uLength1;
			MemoryCopy(pTemp,pInput2,uLength2);
			pTemp[uLength2] = 0;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default constructor for concatinated "C" strings.
	
	Take the passed "C" strings and create a "C" string by concatinating
	all of them together into a single unified string. If any
	of the inputs are \ref NULL or empty strings, they will be ignored.
	If the resulting string is empty, the global
	Burger::g_EmptyString string ("") is used internally, otherwise a buffer is
	allocated and the string(s) will be copied, concatinated and assigned
	to this class instance.
	
	\param pInput1 Pointer to the first input string. \ref NULL or an empty string is acceptable.
	\param pInput2 Pointer to the second input string. \ref NULL or an empty string is acceptable.
	\param pInput3 Pointer to the third input string. \ref NULL or an empty string is acceptable.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const char *pInput1,const char *pInput2,const char *pInput3)
{
	uintptr_t uLength1 = 0;
	if (pInput1) {
		uLength1 = StringLength(pInput1);
	}
	uintptr_t uLength2 = 0;
	if (pInput2) {
		uLength2 = StringLength(pInput2);
	}
	uintptr_t uLength3 = 0;
	if (pInput3) {
		uLength3 = StringLength(pInput3);
	}
	const char *pOutput = g_EmptyString;	// Empty
	uintptr_t uLength = uLength1+uLength2+uLength3;
	if (uLength) {
		char *pTemp = static_cast<char *>(Alloc(uLength+1U));
		if (pTemp) {
			pOutput = pTemp;
			MemoryCopy(pTemp,pInput1,uLength1);
			pTemp += uLength1;
			MemoryCopy(pTemp,pInput2,uLength2);
			pTemp += uLength2;
			MemoryCopy(pTemp,pInput3,uLength3);
			pTemp[uLength3] = 0;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default constructor for concatinated "C" strings.
	
	Take the passed "C" strings and create a "C" string by concatinating
	all of them together into a single unified string. If any
	of the inputs are \ref NULL or empty strings, they will be ignored.
	If the resulting string is empty, the global
	Burger::g_EmptyString string ("") is used internally, otherwise a buffer is
	allocated and the string(s) will be copied, concatinated and assigned
	to this class instance.
	
	\param pInput1 Pointer to the first input string. \ref NULL or an empty string is acceptable.
	\param pInput2 Pointer to the second input string. \ref NULL or an empty string is acceptable.
	\param pInput3 Pointer to the third input string. \ref NULL or an empty string is acceptable.
	\param pInput4 Pointer to the fourth input string. \ref NULL or an empty string is acceptable.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const char *pInput1,const char *pInput2,const char *pInput3,const char *pInput4)
{
	uintptr_t uLength1 = 0;
	if (pInput1) {
		uLength1 = StringLength(pInput1);
	}
	uintptr_t uLength2 = 0;
	if (pInput2) {
		uLength2 = StringLength(pInput2);
	}
	uintptr_t uLength3 = 0;
	if (pInput3) {
		uLength3 = StringLength(pInput3);
	}
	uintptr_t uLength4 = 0;
	if (pInput4) {
		uLength4 = StringLength(pInput4);
	}
	const char *pOutput = g_EmptyString;	// Empty
	uintptr_t uLength = uLength1+uLength2+uLength3+uLength4;
	if (uLength) {
		char *pTemp = static_cast<char *>(Alloc(uLength+1U));
		if (pTemp) {
			pOutput = pTemp;
			MemoryCopy(pTemp,pInput1,uLength1);
			pTemp += uLength1;
			MemoryCopy(pTemp,pInput2,uLength2);
			pTemp += uLength2;
			MemoryCopy(pTemp,pInput3,uLength3);
			pTemp += uLength3;
			MemoryCopy(pTemp,pInput4,uLength4);
			pTemp[uLength4] = 0;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default constructor for concatinated "C" strings.
	
	Take the passed "C" strings and create a "C" string by concatinating
	all of them together into a single unified string. If any
	of the inputs are \ref NULL or empty strings, they will be ignored.
	If the resulting string is empty, the global
	Burger::g_EmptyString string ("") is used internally, otherwise a buffer is
	allocated and the string(s) will be copied, concatinated and assigned
	to this class instance.
	
	\note If you need more than 5 strings, you will have to chain
		string creation or possibly use a Burger::String class
		instance instead.
		
	\param pInput1 Pointer to the first input string. \ref NULL or an empty string is acceptable.
	\param pInput2 Pointer to the second input string. \ref NULL or an empty string is acceptable.
	\param pInput3 Pointer to the third input string. \ref NULL or an empty string is acceptable.
	\param pInput4 Pointer to the fourth input string. \ref NULL or an empty string is acceptable.
	\param pInput5 Pointer to the fifth input string. \ref NULL or an empty string is acceptable.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const char *pInput1,const char *pInput2,const char *pInput3,const char *pInput4,const char *pInput5)
{
	uintptr_t uLength1 = 0;
	if (pInput1) {
		uLength1 = StringLength(pInput1);
	}
	uintptr_t uLength2 = 0;
	if (pInput2) {
		uLength2 = StringLength(pInput2);
	}
	uintptr_t uLength3 = 0;
	if (pInput3) {
		uLength3 = StringLength(pInput3);
	}
	uintptr_t uLength4 = 0;
	if (pInput4) {
		uLength4 = StringLength(pInput4);
	}
	uintptr_t uLength5 = 0;
	if (pInput5) {
		uLength5 = StringLength(pInput5);
	}
	const char *pOutput = g_EmptyString;	// Empty
	uintptr_t uLength = uLength1+uLength2+uLength3+uLength4+uLength5;
	if (uLength) {
		char *pTemp = static_cast<char *>(Alloc(uLength+1U));
		if (pTemp) {
			pOutput = pTemp;
			MemoryCopy(pTemp,pInput1,uLength1);
			pTemp += uLength1;
			MemoryCopy(pTemp,pInput2,uLength2);
			pTemp += uLength2;
			MemoryCopy(pTemp,pInput3,uLength3);
			pTemp += uLength3;
			MemoryCopy(pTemp,pInput4,uLength4);
			pTemp += uLength4;
			MemoryCopy(pTemp,pInput5,uLength5);
			pTemp[uLength5] = 0;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default copy constructor.
	
	Makes a copy of an existing SimpleString. If the
	passed string is empty, no memory is allocated, otherwise
	the string is copied so the original SimpleString can
	be discarded.
			
	\param rInput Reference to a SimpleString to copy.
	\sa Burger::SimpleString::SimpleString(const char *)
	
***************************************/

Burger::SimpleString::SimpleString(const Burger::SimpleString &rInput)
{
	const char *pInput = rInput.m_pData;
	const char *pOutput = g_EmptyString;	// Empty
	if (pInput[0]) {
		char *pTemp = StringDuplicate(pInput);
		if (pTemp) {
			pOutput = pTemp;
		}
	}
	m_pData = pOutput;
}

/*! ************************************

	\brief Default destructor.
	
	Releases any allocated memory.
			
	\sa Burger::SimpleString::Clear(void)
	
***************************************/

Burger::SimpleString::~SimpleString()
{
	const char *pData = m_pData;
	if (pData!=g_EmptyString) {		// If not the global string
		Free(pData);	// Kill it.
	}
}

/*! ************************************

	\brief Assign a new string to an existing Burger::SimpleString.
	
	Dispose of the previous string and assign a new one that's
	one character long. If cInput is zero, the resulting string
	will be empty.
	
	\param cInput Single character to make a string out of. If zero,
		the string will be empty.
	\return A reference to the "this" record.
	\sa Burger::SimpleString::operator=(const char *)
	
***************************************/

Burger::SimpleString & Burger::SimpleString::operator = (char cInput)
{
	// Delete the previous string
	const char *pData = m_pData;
	if (pData!=g_EmptyString) {
		Free(pData);
	}
	// Create the new string
	if (!cInput) {				// Empty?
		m_pData = g_EmptyString;		// Set it empty
	} else {
		char *pData2 = static_cast<char *>(Alloc(2));
		if (!pData2) {
			m_pData = g_EmptyString;
		} else {
			m_pData = pData2;
			pData2[0] = cInput;
			pData2[1] = 0;
		}
	}
	return *this;
}

/*! ************************************

	\brief Assign a new string to an existing SimpleString.
	
	Dispose of the previous string and assign a new one.
	If pInput is NULL or an empty string, the
	resulting string will be empty.
	
	\param pInput Pointer to a "C" string to make a copy of. NULL is acceptable.
	\return A reference to the "this" record.
	\sa Burger::SimpleString::operator=(const Burger::SimpleString &)
	
***************************************/

Burger::SimpleString & Burger::SimpleString::operator = (const char *pInput)
{
	// Delete the previous string
	const char *pData = m_pData;
	if (pData!=g_EmptyString) {
		Free(pData);
	}
	// Create the new string
	if (!pInput || !pInput[0]) {			// Empty?
		m_pData = g_EmptyString;		// Set it empty
	} else {
		char *pData2 = StringDuplicate(pInput);
		if (!pData2) {
			m_pData = g_EmptyString;
		} else {
			m_pData = pData2;
		}
	}
	return *this;
}

/*! ************************************

	\brief Assign a new string to an existing Burger::SimpleString.
	
	Dispose of the previous string and assign a new one.
	If rInput is an empty string, the resulting string will be empty.
	
	\param rInput Reference to a Burger::SimpleString to make a copy of.
	\return A reference to the "this" record.
	\sa Burger::SimpleString::operator=(const char *)
	
***************************************/

Burger::SimpleString & Burger::SimpleString::operator = (const Burger::SimpleString &rInput)
{
	if (this != &rInput) {
		// Delete the previous string
		const char *pData = m_pData;
		if (pData!=g_EmptyString) {
			Free(pData);
		}
		// Create the new string
		pData = rInput.m_pData;
		if (!pData[0]) {			// Empty?
			m_pData = g_EmptyString;		// Set it empty
		} else {
			char *pData2 = StringDuplicate(pData);
			if (!pData2) {
				m_pData = g_EmptyString;
			} else {
				m_pData = pData2;
			}
		}
	}
	return *this;
}

/*! ************************************

	\fn Burger::SimpleString::operator const char *() const
	\brief Convert a Burger::SimpleString into a const char *.

	Most functions passed a Burger::SimpleString will want
	a const char * instead. This inline accessor will allow access
	to the contained string.

	\note It is not advised to cast away the const and modify the string.
	You can do this on a non-empty string as long as you do not attempt
	to write past the terminating zero.

	\return A pointer to the contained "C" string. Modify with caution.

	\sa Burger::SimpleString::GetPtr(void) const.

***************************************/

/*! ************************************

	\fn const char *Burger::SimpleString::GetPtr(void) const
	\brief Convert a Burger::SimpleString into a const char *.

	Most functions passed a Burger::SimpleString will want
	a const char * instead. This inline accessor will allow access
	to the contained string.

	\note It is not advised to cast away the const and modify the string.
	You can do this on a non-empty string as long as you do not attempt
	to write past the terminating zero.

	\return A pointer to the contained "C" string. Modify with caution.

	\sa Burger::SimpleString::operator char*() const.

***************************************/

/*! ************************************

	\fn char Burger::SimpleString::operator[](uintptr_t uIndex) const
	\brief Access a character inside of the string.

	Returns a single character inside the string by using
	a supplied index.

	\note No bounds checking is done. It's possible for you to access
	invalid data. Make sure that your index is valid before accessing
	the string in this manner.

	\param uIndex Index into the string, 0 to less than
Burger::SimpleString::GetLength(void) const. \return Character at the supplied
index. \sa Burger::SimpleString::operator[](uintptr_t)

***************************************/

/*! ************************************

	\fn const char &Burger::SimpleString::operator[](uintptr_t uIndex)
	\brief Access a character reference inside of the string.

	Returns a reference to a single character inside the string by using
	a supplied index.

	\note No bounds checking is done. It's possible for you to access
	invalid data. Make sure that your index is valid before accessing
	the string in this manner.

	\param uIndex Index into the string, 0 to less than
Burger::SimpleString::GetLength(void) const. \return Character reference at the
supplied index. \sa Burger::SimpleString::operator[](uintptr_t) const

***************************************/

/*! ************************************

	\brief Force the assignment of an empty string.

	Dispose of the previous string and assign
	the global Burger::g_EmptyString ("") as the string. This
	has the effect of releasing all used memory and
	this instance having an empty string.

	\sa Burger::SimpleString::IsEmpty(void) const

***************************************/

void Burger::SimpleString::Clear(void)
{
	const char* pData = m_pData;
	if (pData != g_EmptyString) {
		m_pData = g_EmptyString; // Set to empty
		Free(pData);             // Dispose of the memory
	}
}

/*! ************************************

	\fn uint_t Burger::SimpleString::IsEmpty(void) const
	\brief Determine if a string is empty.

	Return \ref TRUE if the string is only a terminating zero
	and nothing else. \ref FALSE is returned if the string has
	any data inside.

	\return \ref TRUE if the string is an empty one, \ref FALSE if it isn't.
	\sa Burger::SimpleString::GetLength(void) const or
Burger::SimpleString::Clear(void)

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SimpleString::GetLength(void) const
	\brief Return the length of the string in bytes.

	Return the length (in bytes) of the "C" string contained
	inside of this class. The terminating zero will not count
	towards the total. The length is the same as if you called
	Burger::StringLength(const char *) on the "C" string.

	\note It isn't recommended to use this member function in
	a bottleneck routine because the length is calculated and
	not cached.

	\return The length of the "C" string in bytes (Without counting the
terminating zero).

	\sa Burger::SimpleString::IsEmpty(void) const

***************************************/
