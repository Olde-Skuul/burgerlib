/***************************************

	Number String Manager in hexadecimal

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberstringhex.h"

/*! ************************************

	\class Burger::NumberStringHex
	\brief Simple "C" string hexadecimal conversion class.
	
	To convert a hexadecimal number into a string quickly and without the
	worry of creating a text buffer to contain the ASCII
	string, use this class. Once the string is created, you can
	access via Burger::NumberStringHex::GetPtr() const or const char * accessors.
	
	The main benefit of this class is that the string buffer is
	part of the class instance, so no <i>behind the scenes</i> memory
	allocation is performed. It's best used with creating a
	local <i>on the stack</i> instance with the number to be converted
	as input to the constructor. You then access the string and
	use it as you wish.
	
	Most constructors can take an optional format parameter so you
	can have some control as to how the string is created.


	\code

	// Cross platform way of printing a 64 bit hex value
	Word64 uValue = 0x123456789ABCDULL;
	Burger::NumberStringHex Text(uValue);
	printf("uValue = 0x%s\n",Text.GetPtr());

	// Output is "uValue = 0x000123456789ABCD"

	\endcode

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::NumberStringHex()
	\brief Default constructor
	
	Initialize to an empty string

	\sa NumberStringHex(Word32) or NumberStringHex(Word64)
	
***************************************/

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer.
	
	Convert the 32 bit unsigned integer into an ASCII hex string
	and store that string inside the class. The resulting string is
	from 1 to 8 digits in length.
	
	\param uInput Unsigned 32 bit integer to convert to an ASCII hex string.
	\sa Burger::NumberStringHex::NumberStringHex(Word64) or Burger::NumberStringHex::NumberStringHex(Word32,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(Word32 uInput)
{
	NumberToAsciiHex(m_Data,uInput,Burger::LEADINGZEROS);
}

/*! ************************************

	\brief Default constructor for an unsigned 32 bit integer with formatting.
	
	Convert the 32 bit unsigned integer into an ASCII hex string
	and store that string inside the class. The resulting string is
	from 1 to 8 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAsciiHex(char *,Word32,Word),
	please see that function's documentation for what values you can pass.
	
	\param uInput Unsigned 32 bit integer to convert to an ASCII hex string.
	\param uFormat Formatting parameter.
	\sa Burger::NumberStringHex::NumberStringHex(Word32) or Burger::NumberStringHex::NumberStringHex(Word64,Word)
		Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(Word32 uInput,Word uFormat)
{
	NumberToAsciiHex(m_Data,uInput,uFormat);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer.
	
	Convert the 64 bit unsigned integer into an ASCII hex string
	and store that string inside the class. The resulting string is
	from 1 to 16 digits in length.
	
	\param uInput Unsigned 64 bit integer to convert to an ASCII hex string.
	\sa Burger::NumberStringHex::NumberStringHex(Word32) or Burger::NumberStringHex::NumberStringHex(Word64,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(Word64 uInput)
{
	NumberToAsciiHex(m_Data,uInput,Burger::LEADINGZEROS);
}

/*! ************************************

	\brief Default constructor for an unsigned 64 bit integer with formatting.
	
	Convert the 64 bit unsigned integer into an ASCII hex string
	and store that string inside the class. The resulting string is
	from 1 to 16 digits in length. Formatting will have some influence
	on the string's length.
	
	The format parameter is passed directly to Burger::NumberToAsciiHex(char *,Word64,Word),
	please see that function's documentation for what values you can pass.
	
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\param uFormat Formatting parameter.
	\sa Burger::NumberStringHex::NumberStringHex(Word64) or Burger::NumberStringHex::NumberStringHex(Word32,Word)
		Burger::NumberToAsciiHex(char *,Word64,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(Word64 uInput,Word uFormat)
{
	NumberToAsciiHex(m_Data,uInput,uFormat);
}

/*! ************************************

	\brief Default constructor for a 32 bit float.
	
	Convert the 32 bit float into an ASCII hex string
	and store that string inside the class. The resulting string is
	8 digits in length.

	\param fInput 32 bit float to convert to an ASCII hex string.
	\sa Burger::NumberStringHex::NumberStringHex(double) or
		Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(float fInput)
{
	NumberToAsciiHex(m_Data,reinterpret_cast<const Word32 *>(&fInput)[0],Burger::LEADINGZEROS);
}

/*! ************************************

	\brief Default constructor for a 64 bit float.
	
	Convert the 64 bit float into an ASCII hex string
	and store that string inside the class. The resulting string is
	16 digits in length.

	\param dInput 64 bit float to convert to ASCII.
	\sa Burger::NumberStringHex::NumberStringHex(float) or
		Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

Burger::NumberStringHex::NumberStringHex(double dInput)
{
	NumberToAsciiHex(m_Data,reinterpret_cast<const Word64 *>(&dInput)[0],Burger::LEADINGZEROS);
}

/*! ************************************

	\brief Copy operator for an unsigned 32 bit integer.
	
	Convert the 32 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	8 digits in length.
	
	\param uInput Unsigned 32 bit integer to convert to ASCII.
	\sa Burger::NumberStringHex::NumberStringHex(Word32) or Burger::NumberStringHex::NumberStringHex(Word32,Word)
	
***************************************/

Burger::NumberStringHex & Burger::NumberStringHex::operator = (Word32 uInput)
{
	NumberToAsciiHex(m_Data,uInput,Burger::LEADINGZEROS);
	return *this;
}

/*! ************************************

	\brief Copy operator for an unsigned 64 bit integer.
	
	Convert the 64 bit unsigned integer into an ASCII string
	and store that string inside the class. The resulting string is
	16 digits in length.
	
	\param uInput Unsigned 64 bit integer to convert to ASCII.
	\sa Burger::NumberStringHex::NumberStringHex(Word64) or Burger::NumberStringHex::NumberStringHex(Word64,Word)
	
***************************************/

Burger::NumberStringHex & Burger::NumberStringHex::operator = (Word64 uInput)
{
	NumberToAsciiHex(m_Data,uInput,Burger::LEADINGZEROS);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 32 bit float.
	
	Convert the 32 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	8 digits in length.
	
	\param fInput 32 bit float to convert to ASCII.
	\sa Burger::NumberStringHex::NumberStringHex(float) or
		Burger::NumberToAsciiHex(char *,Word32,Word)
	
***************************************/

Burger::NumberStringHex & Burger::NumberStringHex::operator = (float fInput)
{
	NumberToAsciiHex(m_Data,reinterpret_cast<const Word32 *>(&fInput)[0],Burger::LEADINGZEROS);
	return *this;
}

/*! ************************************

	\brief Copy operator for a 64 bit float.
	
	Convert the 64 bit float into an ASCII string
	and store that string inside the class. The resulting string is
	8 digits in length.
	
	\param dInput 64 bit float to convert to ASCII.
	\sa Burger::NumberStringHex::NumberStringHex(double) or
		Burger::NumberToAsciiHex(char *,Word64,Word)
	
***************************************/

Burger::NumberStringHex & Burger::NumberStringHex::operator = (double dInput)
{
	NumberToAsciiHex(m_Data,reinterpret_cast<const Word64 *>(&dInput)[0],Burger::LEADINGZEROS);
	return *this;
}

/*! ************************************

	\fn Burger::NumberStringHex::operator const char *() const
	\brief Get a pointer to the string.
	
	Retrieve a char * to the string contained in this class.
	The pointer is const because this class was not
	meant to have anything modify the string without
	the class knowing about the operation.
	
	\return const char * to the string within. The string is always valid.
	\sa Burger::NumberStringHex::GetPtr(void) const
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::GetPtr() const
	\brief Get a pointer to the string.
	
	Retrieve a char * to the string contained in this class.
	The pointer is const because this class was not
	meant to have anything modify the string without
	the class knowing about the operation.
	
	\return const char * to the string within. The string is always valid.
	\sa Burger::NumberStringHex::operator const char*() const
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::operator [](WordPtr uIndex) const
	\brief Get a character from the string.
	
	Retrieve a char from inside the string contained in this class.
	There is no bounds checking performed on the index passed
	just like if this were a literal char array.
	
	\return One character from the string.
	\sa Burger::NumberStringHex::operator [](WordPtr)
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::operator [](WordPtr uIndex)
	\brief Get a character reference from the string.
	
	Retrieve a const char refernce from inside the string contained in this class.
	There is no bounds checking performed on the index passed
	just like if this were a literal char array.
	
	\return A const character reference from the string.
	\sa Burger::NumberStringHex::operator [](WordPtr) const
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::Clear()
	\brief Erase the string.
	
	Set the string to a blank string, I.E. "".
	
	\sa Burger::NumberStringHex::IsEmpty() const
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::IsEmpty() const
	\brief Return \ref TRUE if the string is blank.
	
	If the string inside is blank, return \ref TRUE,
	otherwise return \ref FALSE.

	\return \ref TRUE if the string is blank, \ref FALSE if not.	
	\sa Burger::NumberStringHex::GetLength() const or
		Burger::NumberStringHex::Clear()
	
***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::GetLength() const
	\brief Get the length of the string in bytes.
	
	By calling Burger::StringLength(), return the length
	of the string contained within.
	
	\note This function performs no caching. If the length
	of the string is not expected to change in a
	performance critical loop, cache the returned value
	and use it in the loop instead of calling this function
	repeatedly.
	
	\return Length of the string in bytes. The terminating zero doesn't count.
	\sa Burger::NumberStringHex::IsEmpty() const
	
***************************************/
