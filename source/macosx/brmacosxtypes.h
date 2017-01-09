/***************************************

	Typedefs specific to Mac OSX

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACOSXTYPES_H__
#define __BRMACOSXTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MACOSX) && !defined(DOXYGEN)
struct Point;
struct Rect;
struct FSRef;
struct UTCDateTime;
struct _NSPoint;
struct _NSSize;
struct _NSRect;
struct CGPoint;
struct CGSize;
struct CGRect;
struct _opaque_pthread_t;
struct _CGLContextObject;
struct _CGLPixelFormatObject;
struct _CGLRendererInfoObject;
struct _CGLPBufferObject;
struct __CFString;
struct __CFDictionary;
struct __IOHIDDevice;
struct __IOHIDElement;
struct __IOHIDValue;
struct __IOHIDManager;
struct ComponentInstanceRecord;
struct AudioTimeStamp;
struct AudioBufferList;
struct OpaqueAUGraph;
@class NSApplication;
@class NSApplicationDelegate;
@class NSAutoreleasePool;
@class NSConnection;
@class NSEvent;
@class NSMenu;
@class NSMenuItem;
@class NSOpenGLView;
@class NSResponder;
@class NSScreen;
@class NSString;
@class NSView;
@class NSWindow;
@class NSWindowController;
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
