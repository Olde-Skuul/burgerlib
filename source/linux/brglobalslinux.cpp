/***************************************

    Global variable manager

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_LINUX) || defined(DOXYGEN)
#include "brlinuxtypes.h"
#include "brstringfunctions.h"
#include <unistd.h>

/***************************************

    Read an environment variable as UTF8

***************************************/

const char* BURGER_API Burger::GetEnvironmentString(
    const char* pKey) BURGER_NOEXCEPT
{
    const char* pValue = getenv(pKey);
    if (pValue) {
        pValue = StringDuplicate(pValue);
    }
    return pValue;
}

/***************************************

    Set an environment variable with a UTF8 string

***************************************/

Burger::eError BURGER_API Burger::SetEnvironmentString(
    const char* pKey, const char* pInput) BURGER_NOEXCEPT
{
    // Pass to the operating system
    return static_cast<eError>(setenv(pKey, pInput, TRUE));
}

/***************************************

    Test if the application has elevated privileges

***************************************/

uint_t BURGER_API Burger::IsElevated(void) BURGER_NOEXCEPT
{
    // Assume not elevated
    uint_t bResult = FALSE;

    if (!getuid()) {
        bResult = TRUE;
    }
    return bResult;
}

#endif
