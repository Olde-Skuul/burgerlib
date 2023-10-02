/***************************************

	Typedefs specific to Darwin (iOS/macOS)

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDARWINTYPES_H__
#define __BRDARWINTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_DARWIN) && !defined(DOXYGEN)

struct Point;
struct Rect;
struct FSRef;
struct UTCDateTime;
struct CGPoint;
struct CGSize;
struct CGRect;

struct _opaque_pthread_t;
struct __CFString;
struct __CFDictionary;
struct __CFBundle;

#if defined(__OBJC__)
@class NSAutoreleasePool;
@class NSObject;
#else
class NSAutoreleasePool;
class NSObject;
#endif

#if defined(BURGER_64BITCPU)
struct Burgerpthread_mutex_t {
	uint64_t m_Opaque[8];
};

struct Burgerpthread_cond_t {
	uint64_t m_Opaque[6];
};
#else
struct Burgerpthread_mutex_t {
	uint32_t m_Opaque[11];
};

struct Burgerpthread_cond_t {
	uint32_t m_Opaque[7];
};
#endif
#endif

/* END */

#endif
