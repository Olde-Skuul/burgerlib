/***************************************

    Flash player ActionScript disassembler

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFLASHDISASMACTIONSCRIPT_H__
#define __BRFLASHDISASMACTIONSCRIPT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
extern uintptr_t BURGER_API DisassembleActionScript(const uint8_t* pInput);
}}
/* END */

#endif
