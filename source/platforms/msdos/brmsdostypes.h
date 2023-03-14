/***************************************

    Typedefs specific to MSDOS

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMSDOSTYPES_H__
#define __BRMSDOSTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

/* BEGIN */
#if defined(BURGER_MSDOS) || defined(DOXYGEN)

namespace Burger {

struct Regs16 {
	uint16_t ax;		///< 80x86 ax register
	uint16_t bx;		///< 80x86 bx register
	uint16_t cx;		///< 80x86 cx register
	uint16_t dx;		///< 80x86 dx register
	uint16_t si;		///< 80x86 si register
	uint16_t di;		///< 80x86 di register
	uint16_t bp;		///< 80x86 bp register
	uint16_t ds;		///< 80x86 ds segment register
	uint16_t es;		///< 80x86 es segment register
	uint16_t flags;	///< 80x86 flags register
};

}
#endif
/* END */

#endif
