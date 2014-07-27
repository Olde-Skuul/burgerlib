/***************************************

	Adler32 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/Adler-32
	and http://tools.ietf.org/html/rfc1950

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRADLER32_H__
#define __BRADLER32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern Word32 BURGER_API CalcAdler32(const void *pInput,WordPtr uInputLength,Word32 uAdler32=1);
}
/* END */

#endif
