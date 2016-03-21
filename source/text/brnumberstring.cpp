/***************************************

	Number String Manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberstring.h"

/*! ************************************

	\class Burger::NumberString
	\brief Simple "C" string numeric conversion class.
	
	To convert a number into a string quickly and without the
	worry of creating a text buffer to contain the ASCII
	string, use this class. Once the string is created, you can
	access via Burger::NumberString::GetPtr() const or const char * accessors.
	
	The main benefit of this class is that the string buffer is
	part of the class instance, so no <i>behind the scenes</i> memory
	allocation is performed. It's best used with creating a
	local <i>on the stack</i> instance with the number to be converted
	as input to the constructor. You then access the string and
	use it as you wish.
	
	Most constructors can take an optional format parameter so you
	can have some control as to how the string is created.

***************************************/

/*! ************************************

	\fn Burger::NumberString::NumberString()
	\brief Default constructor
	
	Initialize to an empty string

	\sa NumberString(Int32) or NumberString(Word32)
	
***************************************/

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer.
	
	Convert the 32 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 10 digits in length.
	
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Int32) or Burger::NumberString::NumberString(Word32,Word)
	
***************************************/

Burger::NumberString::NumberString(Word32 uInput)
{
	NumberToAscii(m_Data,uInput);
}

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer with formatting.
	
	Convert the 32 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 10 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAscii(char *,Word32,Word),
	please see that function's documentation for what values you can pass.
	
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa Burger::NumberString::NumberString(Word32) or Burger::NumberString::NumberString(Word32,Word)
		Burger::NumberToAscii(char *,Word32,Word)
	
***************************************/

Burger::NumberString::NumberString(Word32 uInput,Word uFormat)
{
	NumberToAscii(m_Data,uInput,uFormat);
}

/*! ************************************

	\brief Default constructor for a signed 32 bit integer.
	
	Convert the 32 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 11 digits in length.
	
	\param iInput Signed 32 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Word32) or Burger::NumberString::NumberString(Int32,Word)
	
***************************************/

Burger::NumberString::NumberString(Int32 iInput)
{
	NumberToAscii(m_Data,iInput);
}

/*! ************************************

	\brief Default constructor for a signed 32 bit integer with formatting.
	
	Convert the 32 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 11 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAscii(char *,Int32,Word),
	please see that function's documentation for what values you can pass.
	
	\param iInput Signed 32 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa Burger::NumberString::NumberString(Int32) or Burger::NumberString::NumberString(Word32,Word)
		Burger::NumberToAscii(char *,Word32,Word)
	
***************************************/

Burger::NumberString::NumberString(Int32 iInput,Word uFormat)
{
	NumberToAscii(m_Data,iInput,uFormat);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer.
	
	Convert the 64 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 20 digits in length.
	
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Int64) or Burger::NumberString::NumberString(Word64,Word)
	
***************************************/

Burger::NumberString::NumberString(Word64 uInput)
{
	NumberToAscii(m_Data,uInput);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer with formatting.
	
	Convert the 64 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 20 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAscii(char *,Word64,Word),
	please see that function's documentation for what values you can pass.
	
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa Burger::NumberString::NumberString(Word64) or Burger::NumberString::NumberString(Word64,Word)
		Burger::NumberToAscii(char *,Word64,Word)
	
***************************************/

Burger::NumberString::NumberString(Word64 uInput,Word uFormat)
{
	NumberToAscii(m_Data,uInput,uFormat);
}

/*! ************************************

	\brief Default constructor for a signed 64 bit integer.
	
	Convert the 64 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 22 digits in length.
	
	\param iInput Signed 64 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Word64) or Burger::NumberString::NumberString(Int64,Word)
	
***************************************/

Burger::NumberString::NumberString(Int64 iInput)
{
	NumberToAscii(m_Data,iInput);
}

/*! ************************************

	\brief Default constructor for a signed 64 bit integer with formatting.
	
	Convert the 64 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 22 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAscii(char *,Int64,Word),
	please see that function's documentation for what values you can pass.
	
	\param iInput Signed 64 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa Burger::NumberString::NumberString(Int64) or Burger::NumberString::NumberString(Word64,Word)
		Burger::NumberToAscii(char *,Word64,Word)
	
***************************************/

Burger::NumberString::NumberString(Int64 iInput,Word uFormat)
{
	NumberToAscii(m_Data,iInput,uFormat);
}

/*! ************************************

	\brief Default constructor for a 32 bit float.
	
	Convert the 32 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 32 digits in length.
	
	\param fInput 32 bit float to convert to ASCII.
	\sa Burger::NumberString::NumberString(double) or
		Burger::NumberToAscii(char *,float)
	
***************************************/

Burger::NumberString::NumberString(float fInput)
{
	NumberToAscii(m_Data,fInput);
}

/*! ************************************

	\brief Default constructor for a 64 bit float.
	
	Convert the 64 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 32 digits in length.
	
	\param dInput 64 bit float to convert to ASCII.
	\sa Burger::NumberString::NumberString(float) or
		Burger::NumberToAscii(char *,double)
	
***************************************/

Burger::NumberString::NumberString(double dInput)
{
	NumberToAscii(m_Data,dInput);
}

/*! ************************************

	\brief Copy operator for an unsigned 32 bit integer.
	
	Convert the 32 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 10 digits in length.
	
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Word32) or Burger::NumberString::NumberString(Word32,Word)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (Word32 uInput)
{
	NumberToAscii(m_Data,uInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a signed 32 bit integer.
	
	Convert the 32 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 11 digits in length.
	
	\param iInput Signed 32 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Int32) or Burger::NumberString::NumberString(Int32,Word)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (Int32 iInput)
{
	NumberToAscii(m_Data,iInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for an unsigned 64 bit integer.
	
	Convert the 64 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 20 digits in length.
	
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Word64) or Burger::NumberString::NumberString(Word64,Word)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (Word64 uInput)
{
	NumberToAscii(m_Data,uInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a signed 64 bit integer.
	
	Convert the 64 bit signed integer into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 21 digits in length.
	
	\param iInput Signed 64 bit integer to convert to ASCII.
	\sa Burger::NumberString::NumberString(Int64) or Burger::NumberString::NumberString(Int64,Word)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (Int64 iInput)
{
	NumberToAscii(m_Data,iInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 32 bit float.
	
	Convert the 32 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 32 digits in length.
	
	\param fInput 32 bit float to convert to ASCII.
	\sa Burger::NumberString::NumberString(float) or
		Burger::NumberToAscii(char *,float)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (float fInput)
{
	NumberToAscii(m_Data,fInput);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 64 bit float.
	
	Convert the 64 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	from 1 to 32 digits in length.
	
	\param dInput 64 bit float to convert to ASCII.
	\sa Burger::NumberString::NumberString(double) or
		Burger::NumberToAscii(char *,float)
	
***************************************/

Burger::NumberString & Burger::NumberString::operator = (double dInput)
{
	NumberToAscii(m_Data,dInput);
	return *this;
}

/*! ************************************

	\fn Burger::NumberString::operator const char *() const
	\brief Get a pointer to the string.
	
	Retrieve a char * to the string contained in this class.
	The pointer is const because this class was not
	meant to have anything modify the string without
	the class knowing about the operation.
	
	\return const char * to the string within. The string is always valid.
	\sa Burger::NumberString::GetPtr(void) const
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::GetPtr() const
	\brief Get a pointer to the string.
	
	Retrieve a char * to the string contained in this class.
	The pointer is const because this class was not
	meant to have anything modify the string without
	the class knowing about the operation.
	
	\return const char * to the string within. The string is always valid.
	\sa Burger::NumberString::operator const char *() const
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::operator [](WordPtr uIndex) const
	\brief Get a character from the string.
	
	Retrieve a char from inside the string contained in this class.
	There is no bounds checking performed on the index passed
	just like if this were a literal char array.
	
	\return One character from the string.
	\sa Burger::NumberString::operator [](WordPtr)
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::operator [](WordPtr uIndex)
	\brief Get a character reference from the string.
	
	Retrieve a const char refernce from inside the string contained in this class.
	There is no bounds checking performed on the index passed
	just like if this were a literal char array.
	
	\return A const character reference from the string.
	\sa Burger::NumberString::operator [](WordPtr) const
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::Clear()
	\brief Erase the string.
	
	Set the string to a blank string, I.E. "".
	
	\sa Burger::NumberString::IsEmpty() const
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::IsEmpty() const
	\brief Return \ref TRUE if the string is blank.
	
	If the string inside is blank, return \ref TRUE,
	otherwise return \ref FALSE.

	\return \ref TRUE if the string is blank, \ref FALSE if not.	
	\sa Burger::NumberString::GetLength() const or
		Burger::NumberString::Clear()
	
***************************************/

/*! ************************************

	\fn Burger::NumberString::GetLength() const
	\brief Get the length of the string in bytes.
	
	By calling Burger::StringLength(), return the length
	of the string contained within.
	
	\note This function performs no caching. If the length
	of the string is not expected to change in a
	performance critical loop, cache the returned value
	and use it in the loop instead of calling this function
	repeatedly.
	
	\return Length of the string in bytes. The terminating zero doesn't count.
	\sa Burger::NumberString::IsEmpty() const
	
***************************************/
