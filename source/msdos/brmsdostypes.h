/***************************************

	Typedefs specific to MSDOS

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word16 ax;		///< 80x86 ax register
	Word16 bx;		///< 80x86 bx register
	Word16 cx;		///< 80x86 cx register
	Word16 dx;		///< 80x86 dx register
	Word16 si;		///< 80x86 si register
	Word16 di;		///< 80x86 di register
	Word16 bp;		///< 80x86 bp register
	Word16 ds;		///< 80x86 ds segment register
	Word16 es;		///< 80x86 es segment register
	Word16 flags;	///< 80x86 flags register
};
}
#endif
/* END */

#endif
