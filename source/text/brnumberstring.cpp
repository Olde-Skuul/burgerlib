/***************************************

	Number String Manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberstring.h"
#include "brnumberto.h"

/*! ************************************

	\class Burger::NumberString
	\brief Simple "C" string numeric conversion class.

	To convert a number into a string quickly and without the worry of creating
	a text buffer to contain the ASCII string, use this class. Once the string
	is created, you can access via c_str() const or const char * accessors.

	The main benefit of this class is that the string buffer is part of the
	class instance, so no *behind the scenes* memory allocation is
	performed. It's best used with creating a local *on the stack* instance
	with the number to be converted as input to the constructor. You then access
	the string and use it as you wish.

	Most constructors can take an optional format parameter so you can have some
	control as to how the string is created.

***************************************/

/*! ************************************

	\fn Burger::NumberString::NumberString()
	\brief Default constructor

	Initialize to an empty string

	\sa NumberString(int32_t) or NumberString(uint32_t)

***************************************/

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer.

	Convert the 32 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 10 digits in
	length.

	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\sa NumberString(int32_t) or NumberString(uint32_t,uint_t)

***************************************/

Burger::NumberString::NumberString(uint32_t uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput);
}

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer with formatting.

	Convert the 32 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 10 digits in
	length. Formatting will have some influence on the string's length.

	The format parameter is passed directly to
	NumberToAscii(char *, uint32_t, uint_t), please see that function's
	documentation for what values you can pass.

	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa NumberString(uint32_t) or NumberString(uint32_t, uint_t)
		NumberToAscii(char*, uint32_t, uint_t)

***************************************/

Burger::NumberString::NumberString(
	uint32_t uInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput, uFormat);
}

/*! ************************************

	\brief Default constructor for a signed 32 bit integer.

	Convert the 32 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 11 digits in length.

	\param iInput Signed 32 bit integer to convert to ASCII.
	\sa NumberString(uint32_t) or NumberString(int32_t, uint_t)

***************************************/

Burger::NumberString::NumberString(int32_t iInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput);
}

/*! ************************************

	\brief Default constructor for a signed 32 bit integer with formatting.

	Convert the 32 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 11 digits in length.
	Formatting will have some influence on the string's length.

	The format parameter is passed directly to
	NumberToAscii(char*, int32_t, uint_t), please see that function's
	documentation for what values you can pass.

	\param iInput Signed 32 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa NumberString(int32_t) or NumberString(uint32_t, uint_t)
		NumberToAscii(char*, uint32_t, uint_t)

***************************************/

Burger::NumberString::NumberString(
	int32_t iInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput, uFormat);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer.

	Convert the 64 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 20 digits in
	length.

	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa NumberString(int64_t) or NumberString(uint64_t, uint_t)

***************************************/

Burger::NumberString::NumberString(uint64_t uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer with formatting.

	Convert the 64 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 20 digits in
	length. Formatting will have some influence on the string's length.

	The format parameter is passed directly to
	NumberToAscii(char*,uint64_t,uint_t), please see that function's
	documentation for what values you can pass.

	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa NumberString(uint64_t) or NumberString(uint64_t, uint_t)
		NumberToAscii(char*, uint64_t, uint_t)

***************************************/

Burger::NumberString::NumberString(
	uint64_t uInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput, uFormat);
}

/*! ************************************

	\brief Default constructor for a signed 64 bit integer.

	Convert the 64 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 22 digits in length.

	\param iInput Signed 64 bit integer to convert to ASCII.
	\sa NumberString(uint64_t) or NumberString(int64_t, uint_t)

***************************************/

Burger::NumberString::NumberString(int64_t iInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput);
}

/*! ************************************

	\brief Default constructor for a signed 64 bit integer with formatting.

	Convert the 64 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 22 digits in length.
	Formatting will have some influence on the string's length.

	The format parameter is passed directly to
	NumberToAscii(char*,int64_t,uint_t), please see that function's
	documentation for what values you can pass.

	\param iInput Signed 64 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa NumberString(int64_t) or NumberString(uint64_t, uint_t)
		NumberToAscii(char*, uint64_t, uint_t)

***************************************/

Burger::NumberString::NumberString(
	int64_t iInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput, uFormat);
}

/*! ************************************

	\brief Default constructor for a 32 bit float.

	Convert the 32 bit float into an ASCII string and store that string inside
	the class. The resulting string is from 1 to 32 digits in length.

	\param fInput 32 bit float to convert to ASCII.
	\sa NumberString(double) or NumberToAscii(char *, float, uint_t)

***************************************/

Burger::NumberString::NumberString(float fInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, fInput);
}

/*! ************************************

	\brief Default constructor for a 64 bit float.

	Convert the 64 bit float into an ASCII string and store that string inside
	the class. The resulting string is from 1 to 32 digits in length.

	\param dInput 64 bit float to convert to ASCII.
	\sa NumberString(float) or NumberToAscii(char *, double, uint_t)

***************************************/

Burger::NumberString::NumberString(double dInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, dInput);
}

/*! ************************************

	\brief Default constructor for a wchar_t.

	Convert the 16/32 bit wchar_t into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 20 digits in
	length. wchar_t is considered unsigned for printing.

	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa NumberString(int64_t) or NumberString(uint64_t, uint_t)

***************************************/

#if defined(BURGER_HAS_WCHAR_T) || defined(DOXYGEN)
Burger::NumberString::NumberString(wchar_t uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<uint32_t>(uInput));
}

/*! ************************************

	\brief Default constructor for a wchar_t.

	Convert the 16 or 32 bit wchar_t into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 32 digits in length.
	wchar_t is considered unsigned for printing.

	\param uInput 16/32 bit wchar_t to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa NumberString(uint32_t) or NumberToAscii(char *, uint32_t, uint_t)

***************************************/

Burger::NumberString::NumberString(
	wchar_t uInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<uint32_t>(uInput), uFormat);
}
#endif

#if defined(BURGER_INT_NOT_IN_STDINT) || defined(DOXYGEN)
Burger::NumberString::NumberString(unsigned int uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<uint2uint_t>(uInput));
}

Burger::NumberString::NumberString(
	unsigned int uInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<uint2uint_t>(uInput), uFormat);
}
#endif

#if defined(BURGER_LONG_NOT_IN_STDINT) || defined(DOXYGEN)
Burger::NumberString::NumberString(unsigned long uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<ulong2uint_t>(uInput));
}
Burger::NumberString::NumberString(
	unsigned long uInput, uint_t uFormat) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, static_cast<ulong2uint_t>(uInput), uFormat);
}
#endif

/*! ************************************

	\brief Copy operator for an unsigned 32 bit integer.

	Convert the 32 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 10 digits in
	length.

	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\sa NumberString(uint32_t) or NumberString(uint32_t, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	uint32_t uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a signed 32 bit integer.

	Convert the 32 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 11 digits in length.

	\param iInput Signed 32 bit integer to convert to ASCII.
	\sa NumberString(int32_t) or NumberString(int32_t, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	int32_t iInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for an unsigned 64 bit integer.

	Convert the 64 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is from 1 to 20 digits in
	length.

	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa NumberString(uint64_t) or NumberString(uint64_t, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	uint64_t uInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, uInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a signed 64 bit integer.

	Convert the 64 bit signed integer into an ASCII string and store that string
	inside the class. The resulting string is from 1 to 21 digits in length.

	\param iInput Signed 64 bit integer to convert to ASCII.
	\sa NumberString(int64_t) or NumberString(int64_t, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	int64_t iInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, iInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 32 bit float.

	Convert the 32 bit float into an ASCII string and store that string inside
	the class. The resulting string is from 1 to 32 digits in length.

	\param fInput 32 bit float to convert to ASCII.
	\sa NumberString(float) or NumberToAscii(char *, float, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	float fInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, fInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 64 bit float.

	Convert the 64 bit float into an ASCII string and store that string inside
	the class. The resulting string is from 1 to 32 digits in length.

	\param dInput 64 bit float to convert to ASCII.
	\sa NumberString(double) or NumberToAscii(char *, double, uint_t)

***************************************/

Burger::NumberString& Burger::NumberString::operator=(
	double dInput) BURGER_NOEXCEPT
{
	NumberToAscii(m_Data, dInput);
	return *this;
}

/*! ************************************

	\fn Burger::NumberString::operator const char *() const
	\brief Get a pointer to the string.

	Retrieve a char * to the string contained in this class. The pointer is
	const because this class was not meant to have anything modify the string
	without the class knowing about the operation.

	\return const char * to the string within. The string is always valid.
	\sa c_str(void) const

***************************************/

/*! ************************************

	\fn Burger::NumberString::c_str() const
	\brief Get a pointer to the string.

	Retrieve a char * to the string contained in this class. The pointer is
	const because this class was not meant to have anything modify the string
	without the class knowing about the operation.

	\return const char * to the string within. The string is always valid.
	\sa NumberString::operator const char *() const

***************************************/

/*! ************************************

	\fn Burger::NumberString::operator [](uintptr_t uIndex) const
	\brief Get a character from the string.

	Retrieve a char from inside the string contained in this class. There is no
	bounds checking performed on the index passed just like if this were a
	literal char array.

	\return One character from the string.
	\sa NumberString::operator [](uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::NumberString::operator [](uintptr_t uIndex)
	\brief Get a character reference from the string.

	Retrieve a const char refernce from inside the string contained in this
	class. There is no bounds checking performed on the index passed just like
	if this were a literal char array.

	\return A const character reference from the string.
	\sa NumberString::operator [](uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::NumberString::clear()
	\brief Erase the string.

	Set the string to a blank string, I.E. "".

	\sa empty() const

***************************************/

/*! ************************************

	\fn Burger::NumberString::empty() const
	\brief Return \ref TRUE if the string is blank.

	If the string inside is blank, return \ref TRUE, otherwise return \ref
	FALSE.

	\return \ref TRUE if the string is blank, \ref FALSE if not.
	\sa length() const or clear()

***************************************/

/*! ************************************

	\fn Burger::NumberString::size() const
	\brief Get the length of the string in bytes.

	By calling StringLength(), return the length of the string contained
	within.

	\note This function performs no caching. If the length of the string is not
	expected to change in a performance critical loop, cache the returned value
	and use it in the loop instead of calling this function repeatedly.

	\return Length of the string in bytes. The terminating zero doesn't count.
	\sa empty() const or length() const

***************************************/

/*! ************************************

	\fn Burger::NumberString::length() const
	\brief Get the length of the string in bytes.

	By calling StringLength(), return the length of the string contained
	within.

	\note This function performs no caching. If the length of the string is not
	expected to change in a performance critical loop, cache the returned value
	and use it in the loop instead of calling this function repeatedly.

	\return Length of the string in bytes. The terminating zero doesn't count.
	\sa empty() const or size() const

***************************************/
