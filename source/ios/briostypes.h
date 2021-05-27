/***************************************

    Typedefs specific to iOS

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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

@class EAGLContext;
@class NSObject;
@class UIView;
@class UIWindow;
@class UIImage;
@class UIImageView;
@class UIViewController;

typedef int Burgersem_t;
typedef unsigned int Burgersemaphore_t;
typedef unsigned int Burgertask_t;

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
