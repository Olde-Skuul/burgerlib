/***************************************

    Helper functions for printf and the like

    Mac OSX specific code

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brstdouthelpers.h"

#if defined(BURGER_MACOSX) && !defined(DOXYGEN)
#include <term.h>
#include <unistd.h>

/***************************************

    \brief Clear the text console

    For platforms that support a text console, clear out text console by sending
    a line feed or system appropriate set of calls to perform the same action

***************************************/

void BURGER_API Burger::ClearConsole(void)
{
    if (!cur_term) {
        int result;
        setupterm(nullptr, STDOUT_FILENO, &result);

        // Error?
        if (result <= 0) {
            return;
        }
    }
    putp(tigetstr(const_cast<char*>("clear")));
}
#endif
