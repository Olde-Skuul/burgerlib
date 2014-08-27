/***************************************

	Typedefs specific to iOS

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIOSTYPES_H__
#define __BRIOSTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_IOS) && !defined(DOXYGEN)
struct Point;
struct Rect;
struct FSRef;
struct UTCDateTime;
struct CGPoint;
struct CGSize;
struct CGRect;
struct _opaque_pthread_t;
namespace Burger {
	typedef int sem_t;
	typedef unsigned int semaphore_t;
	typedef unsigned int task_t;
#if defined(BURGER_64BITCPU)
	struct pthread_mutex_t { Word64 m_Opaque[8]; };
	struct pthread_cond_t { Word64 m_Opaque[6]; };
#else
	struct pthread_mutex_t { Word32 m_Opaque[11]; };
	struct pthread_cond_t { Word32 m_Opaque[7]; };
#endif
}
#endif
/* END */

#endif
