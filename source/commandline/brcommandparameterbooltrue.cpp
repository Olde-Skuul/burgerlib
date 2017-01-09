/***************************************

	Class for a boolean command parameter

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcommandparameterbooltrue.h"

/*! ************************************

	\class Burger::CommandParameterBooleanTrue
	\brief Class for a boolean command line parameters

	Sets a flag to true if this command was present

	\sa Burger::CommandParameter or Burger::CommandParameterWordPtr

***************************************/

/*! ************************************

	\fn Burger::CommandParameterBooleanTrue::CommandParameterBooleanTrue(const char *pHelp,
		const char *const *ppParameterName,WordPtr uParameterCount)
	\brief Constructor for multiple valid commands

	In cases there multiple commands can trigger this command, pass a pointer
	to an array of "C" strings and the array size in entries to construct this
	parameter parser

	\note Do not set the count to 0, it will cause this class to treat the array
		pointer as a direct pointer which may not be desired behavior

	\param pHelp Pointer to "C" string for usage
	\param ppParameterName Pointer to an array of "C" strings for valid inputs
	\param uParameterCount Number of entries in the array (1-XX).

***************************************/

/*! ************************************

	\fn Burger::CommandParameterBooleanTrue::CommandParameterBooleanTrue(const char *pHelp,
		const char *const *ppParameterName,WordPtr uParameterCount)
	\brief Constructor for multiple valid commands

	The default value is set to \ref FALSE.

	In cases there multiple commands can trigger this command, pass a pointer
	to an array of "C" strings and the array size in entries to construct this
	parameter parser.

	\note Do not set the count to 0, it will cause this class to treat the array
		pointer as a direct pointer which may not be desired behavior

	\param pHelp Pointer to "C" string for usage
	\param ppParameterName Pointer to an array of "C" strings for valid inputs
	\param uParameterCount Number of entries in the array (1-XX).

***************************************/

/*! ************************************

	\fn Burger::CommandParameterBooleanTrue::CommandParameterBooleanTrue(const char *pHelp,const char *pParameterName)
	\brief Constructor for a single valid command

	The default value is set to \ref FALSE.

	\param pHelp Pointer to "C" string for usage.
	\param pParameterName Pointer to a "C" string to match to trigger this parameter.

***************************************/

/*! ************************************

	\brief Sets the value to \ref TRUE

	No parameters are parsed. 

	\param argc Number of parameters in argv (Not used)
	\param argv Array of "C" strings (Not used)
	\return Returns 1

***************************************/

int Burger::CommandParameterBooleanTrue::Action(int /* argc */,const char ** /* argv */)
{
	m_bValue = TRUE;
	return 1;
}

/*! ************************************

	\fn Word Burger::CommandParameterBooleanTrue::GetValue(void) const 
	\brief Returns the boolean value

	Returns the contained boolean of \ref TRUE or \ref FALSE

	\return Returns \ref TRUE or \ref FALSE

***************************************/