/***************************************

	Class for a string command parameter

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcommandparameterstring.h"

/*! ************************************

	\class Burger::CommandParameterString
	\brief Class for a string command line parameter

	Sets a string if one is present

	\sa Burger::CommandParameter or Burger::CommandParameterWordPtr

***************************************/

/*! ************************************

	\fn Burger::CommandParameterString::CommandParameterString(const char *pHelp,
		const char *const *ppParameterName,WordPtr uParameterCount,const char *pDefault)
	\brief Constructor for multiple valid commands

	In cases there multiple commands can trigger this command, pass a pointer
	to an array of "C" strings and the array size in entries to construct this
	parameter parser

	\note Do not set the count to 0, it will cause this class to treat the array
		pointer as a direct pointer which may not be desired behavior

	\param pHelp Pointer to "C" string for usage
	\param ppParameterName Pointer to an array of "C" strings for valid inputs
	\param uParameterCount Number of entries in the array (1-XX).
	\param pDefault Pointer to "C" string for default value or \ref NULL for empty string

***************************************/


/*! ************************************

	\fn Burger::CommandParameterString::CommandParameterString(const char *pHelp,const char *pParameterName,const char *pDefault)
	\brief Constructor for a single valid command

	The default value is set to \ref FALSE.

	\param pHelp Pointer to "C" string for usage.
	\param pParameterName Pointer to a "C" string to match to trigger this parameter.
	\param pDefault Pointer to "C" string for default value or \ref NULL for empty string

***************************************/

/*! ************************************

	\brief Sets the value to a passed string

	Parse a parameter if present and copy it
	as is into the string buffer

	\param argc Number of parameters in argv
	\param argv Array of "C" strings
	\return Returns 2 or -1 if no string was present (Or empty)

***************************************/

int Burger::CommandParameterString::Action(int argc,const char **argv)
{
	if (!argc) {
		return -1;
	}
	m_Value.Set(argv[0]);
	return 2;
}

/*! ************************************

	\fn const char *Burger::CommandParameterString::GetValue(void) const 
	\brief Returns the string value

	Returns the contained string. Will not be \ref NULL

	\return Returns pointer to the string (Can be an empty string)

***************************************/