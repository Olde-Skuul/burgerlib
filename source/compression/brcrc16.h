/***************************************

    CRC16 hash manager

    Implemented following the documentation found in
    http://en.wikipedia.org/wiki/Cyclic_redundancy_check

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCRC16_H__
#define __BRCRC16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern uint16_t BURGER_API CalcCRC16IBM(const void *pInput,uintptr_t uInputLength,uint16_t uCRC=0);
extern void BURGER_API GenerateCRCTable(uint16_t *pOutput,uint16_t uPolynomial=0x8005,uint_t bBitReverse=TRUE);
}
/* END */

#endif
