/***************************************

    String handlers for MacRomanUS support

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACROMANUS_H__
#define __BRMACROMANUS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class MacRomanUS {
public:
    static const uint_t BAD = BURGER_MAXUINT;
    BURGER_ALIGN(static const uint8_t, UpperCaseTable[256], 16);
    BURGER_ALIGN(static const uint8_t, LowerCaseTable[256], 16);
    BURGER_ALIGN(static const uint8_t, ToUTF8Table[128][4], 16);
    BURGER_ALIGN(static const uint16_t, ToUTF16Table[128], 16);
    static uint_t BURGER_API FromUTF8(const char* pInput) BURGER_NOEXCEPT;
    static uint_t BURGER_API FromUTF8(char* pOutput, uintptr_t uOutputSize,
        const char* pInput) BURGER_NOEXCEPT;
    static uint_t BURGER_API FromUTF8(char* pOutput, uintptr_t uOutputSize,
        const char* pInput, uintptr_t uInputSize) BURGER_NOEXCEPT;
};
}
/* END */

#endif
