/***************************************

	Number String Manager in hexadecimal

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brnumberstringhex.h"
#include "brnumberto.h"

/*! ************************************

	\class Burger::NumberStringHex
	\brief Simple "C" string hexadecimal conversion class.

	To convert a hexadecimal number into a string quickly and without the worry
	of creating a text buffer to contain the ASCII string, use this class. Once
	the string is created, you can access via NumberStringHex::c_str() const or
	const char * accessors.

	The main benefit of this class is that the string buffer is part of the
	class instance, so no *behind the scenes* memory allocation is performed.
	It's best used with creating a local *on the stack* instance with the number
	to be converted as input to the constructor. After which, the program can
	access the string and use it as needed.

	Most constructors can take an optional format parameter so the code can have
	some control as to how the string is created.


	\code

	// Cross platform way of printing a 64 bit hex value
	uint64_t uValue = 0x123456789ABCDULL;
	Burger::NumberStringHex Text(uValue);
	printf("uValue = 0x%s\n",Text.c_str());

	// Output is "uValue = 0x000123456789ABCD"

	\endcode

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::NumberStringHex()
	\brief Default constructor

	Initialize to an empty string

	\sa NumberStringHex(T) or NumberStringHex(T, uint_t)

***************************************/

/*! ************************************

	\brief Default constructor for an integral.
	\fn Burger::NumberStringHex::NumberStringHex(T input)

	Convert the integral into an ASCII hex string and store that string inside
	the class. The resulting string is from 1 to 8 digits in length depending on
	the number of bytes the integral occupies.

	\tparam T input data type, tested with is_integral.

	\param input Integral value to print in HEX to a string.
	\sa NumberToAsciiHex(char *, T) or
		NumberStringHex::NumberStringHex(T, uint_t)

***************************************/

/*! ************************************

	\brief Default constructor for an integral with formatting.
	\fn Burger::NumberStringHex::NumberStringHex(T input, uint_t uFormat)

	Convert the integral into an ASCII hex string and store that string inside
	the class. The resulting string is from 1 to 8 digits in length in length
	depending on the number of bytes the integral occupies. Formatting will have
	some influence on the string's length.

	The format parameter is passed directly to NumberToAsciiHex(char*,T,uint_t),
	please see that function's documentation for what values you can pass.

	\tparam T input data type, tested with is_integral.

	\param input Integral value to print in HEX to a string.
	\param uFormat Formatting parameter.
	\sa NumberToAsciiHex(char *, T, uint_t) or NumberStringHex(T, uint_t)

***************************************/

/*! ************************************

	\brief Copy operator for an integral.
	\fn Burger::NumberStringHex::operator=(T input)

	Convert the 32 bit unsigned integer into an ASCII string and store that
	string inside the class. The resulting string is 8 digits in length.

	\tparam T input data type, tested with is_integral.

	\param input Integral value to print in HEX to a string.
	\sa NumberStringHex::NumberStringHex(T)

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::operator const char *() const
	\brief Get a pointer to the string.

	Retrieve a char * to the string contained in this class. The pointer is
	const because this class was not meant to have anything modify the string
	without the class knowing about the operation.

	\return const char * to the string within. The string is always valid.
	\sa c_str(void) const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::data() const
	\brief Get a pointer to the string.

	Retrieve a char * to the string contained in this class. The pointer is
	const because this class was not meant to have anything modify the string
	without the class knowing about the operation.

	\return const char * to the string within. The string is always valid.
	\sa \ref operator const char*() const or c_str() const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::c_str() const
	\brief Get a pointer to the string.

	Retrieve a char * to the string contained in this class. The pointer is
	const because this class was not meant to have anything modify the string
	without the class knowing about the operation.

	\return const char * to the string within. The string is always valid.
	\sa \ref operator const char*() const or data() const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::operator [](uintptr_t uIndex) const
	\brief Get a character from the string.

	Retrieve a char from inside the string contained in this class. There is no
	bounds checking performed on the index passed just like if this were a
	literal char array.

	\return One character from the string.
	\sa Burger::NumberStringHex::operator [](uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::operator [](uintptr_t uIndex)
	\brief Get a character reference from the string.

	Retrieve a const char reference from inside the string contained in this
	class. There is no bounds checking performed on the index passed just like
	if this were a literal char array.

	\return A const character reference from the string.
	\sa Burger::NumberStringHex::operator [](uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::clear()
	\brief Erase the string.

	Set the string to a blank string, I.E. "".

	\sa empty() const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::empty() const
	\brief Return \ref TRUE if the string is blank.

	If the string inside is blank, return \ref TRUE, otherwise return
	\ref FALSE.

	\return \ref TRUE if the string is blank, \ref FALSE if not.
	\sa length() const or clear()

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::length() const
	\brief Get the length of the string in bytes.

	By calling Burger::string_length(), return the length of the string contained
	within.

	\note This function performs no caching. If the length of the string is not
	expected to change in a performance critical loop, cache the returned value
	and use it in the loop instead of calling this function repeatedly.

	\return Length of the string in bytes. The terminating zero doesn't count.
	\sa empty() const

***************************************/

/*! ************************************

	\fn Burger::NumberStringHex::capacity() const
	\brief Get the size of the internal buffer in bytes.

	Return the immutable buffer length.

	\return Length of the total buffer in bytes.
	\sa length() const

***************************************/
