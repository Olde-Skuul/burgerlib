/***************************************

    Class for an integer number command parameter

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.
    
    Please? It's not like I'm asking you for money!

***************************************/

#include "brcommandparameterwordptr.h"
#include "brasciito.h"

/*! ************************************

    \class Burger::CommandParameterWordPtr
    \brief Class for a numeric string command line parameter

    Parse an ASCII string and converts it into an integer number.
    Bounds checking is also performed.

    \sa Burger::CommandParameter or Burger::CommandParameterBooleanTrue

***************************************/

/*! ************************************

    \fn Burger::CommandParameterWordPtr::CommandParameterWordPtr(
        const char *pHelp,const char **ppParameterName,
        uintptr_t uParameterCount,uintptr_t uDefault,uintptr_t uMin,
        uintptr_t uMax)

    \brief Constructor for multiple valid commands

    In cases there multiple commands can trigger this command, pass a pointer
    to an array of "C" strings and the array size in entries to construct this
    parameter parser

    \note Do not set the count to 0, it will cause this class to treat the array
        pointer as a direct pointer which may not be desired behavior

    \param pHelp Pointer to "C" string for usage
    \param ppParameterName Pointer to an array of "C" strings for valid inputs
    \param uParameterCount Number of entries in the array (1-XX).
    \param uDefault Default value to return if no parameter was parsed
    \param uMin Minimum allowable value
    \param uMax Maximum allowable value

***************************************/

/*! ************************************

    \fn Burger::CommandParameterWordPtr::CommandParameterWordPtr(
        const char *pHelp, const char *pParameterName,
        uintptr_t uDefault,uintptr_t uMin,uintptr_t uMax)

    \brief Constructor for a single valid command

    The default value is set to uDefault.

    \param pHelp Pointer to "C" string for usage.
    \param pParameterName Pointer to a "C" string to match to trigger this
        parameter.
    \param uDefault Default value to return if no parameter was parsed
    \param uMin Minimum allowable value
    \param uMax Maximum allowable value

***************************************/

/*! ************************************

    \brief Parses a numeric string

    If there is a parameter in the list, parse it as a numeric string. Test
    against the valid range and return either -1 if an error occurred or 2 to
    remove the 2 command entries.

    \param argc Number of parameters in argv.
    \param argv Array of "C" strings.
    \return Returns -1 on error or 2 for success

***************************************/

int Burger::CommandParameterWordPtr::Action(
    int argc, const char** argv)
{
    // A parameter is needed
    if (argc) {
        m_uValue = AsciiToInteger(argv[0]);
        if ((m_uValue >= m_uMin) && (m_uValue <= m_uMax)) {
            return 2;
        }
    }
    return -1;
}

/*! ************************************

    \fn Burger::CommandParameterWordPtr::GetValue(void) const
    \brief Returns the value

    Returns the contained value

    \return Returns the parsed value

***************************************/
