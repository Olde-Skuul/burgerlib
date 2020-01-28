/***************************************

    Operating system string functions

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROSSTRINGFUNCTIONS_H__
#define __BROSSTRINGFUNCTIONS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
namespace Burger {

extern eError BURGER_API GetLoggedInUserName(String* pOutput) BURGER_NOEXCEPT;
extern eError BURGER_API GetMachineName(String* pOutput) BURGER_NOEXCEPT;

}
/* END */

#endif
