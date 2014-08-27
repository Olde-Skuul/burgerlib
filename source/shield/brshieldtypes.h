/***************************************

	Typedefs specific to nVidia SHIELD (Android)

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSHIELDTYPES_H__
#define __BRSHIELDTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_ANDROID) && !defined(DOXYGEN)
namespace Burger {
struct pthread_mutex_t {
	int volatile value;
};
struct sem_t {
	volatile unsigned int count;
};
struct pthread_cond_t {
	int volatile value;
};
}
#endif
/* END */

#endif
