/***************************************

	Typedefs specific to Linux

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLINUXTYPES_H__
#define __BRLINUXTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_LINUX) && !defined(DOXYGEN)

struct Burgerpthread_mutex_t {
	int volatile value;
#if defined(BURGER_64BITCPU)
	char __reserved[36];
#endif
};

struct Burgersem_t {
	volatile unsigned int count;
#if defined(BURGER_64BITCPU)
	int __reserved[3];
#endif
};

struct Burgerpthread_cond_t {
	int volatile value;
#if defined(BURGER_64BITCPU)
	char __reserved[44];
#endif
};

#endif
/* END */

#endif
