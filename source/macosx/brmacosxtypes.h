/***************************************

	Typedefs specific to Mac OSX

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACOSXTYPES_H__
#define __BRMACOSXTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

#if defined(BURGER_MACOSX) || defined(DOXYGEN)

#if !defined(DOXYGEN)

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
struct __CFBundle;
struct __IOHIDDevice;
struct __IOHIDElement;
struct __IOHIDValue;
struct __IOHIDManager;
struct ComponentInstanceRecord;
struct AudioTimeStamp;
struct AudioBufferList;
struct OpaqueAUGraph;
struct attrlist;

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

typedef int Burgersem_t;
typedef unsigned int Burgersemaphore_t;
typedef unsigned int Burgertask_t;

#if defined(BURGER_64BITCPU)
struct Burgerpthread_mutex_t {
	Word64 m_Opaque[8];
};

struct Burgerpthread_cond_t {
	Word64 m_Opaque[6];
};

#else

struct Burgerpthread_mutex_t {
	Word32 m_Opaque[11];
};

struct Burgerpthread_cond_t {
	Word32 m_Opaque[7];
};

#endif

#endif

namespace Burger {
class MacOSX {
public:
	enum eDLLIndex {
		/** Index for libdl.dylib */
		LIBDL_DLL,
		/** Total number of dynamic libraries to be managed */
		DLL_COUNT
	};

	enum eCallIndex {
		/** Index for getattrlistbulk */
		CALL_getattrlistbulk,
		/** Total number of function calls to be managed */
		CALL_COUNT
	};

private:
	/** Instances of dynamically loaded system DLLs \macosxonly */
	void* m_pInstances[DLL_COUNT];
	/** Pointers to resolved windows function calls \macosxonly */
	void* m_pMacOSXCalls[CALL_COUNT];
	/** Flags to determine if a DLL was tested for loading \macosxonly */
	Word8 m_bInstancesTested[DLL_COUNT];
	/** Flags to determine if a function was tested for loading \macosxonly */
	Word8 m_bFunctionsTested[CALL_COUNT];

	static MacOSX g_Globals;

public:
	~MacOSX();

	static void* BURGER_API LoadLibraryIndex(eDLLIndex eIndex);
	static void* BURGER_API LoadFunctionIndex(eCallIndex eIndex);

	// macOS 10.10 functions
	static int BURGER_API getattrlistbulk(int dirfd, attrlist* attrList,
		void* attrBuf, WordPtr attrBufSize, Word64 options);
};
}

#endif
/* END */

#endif
