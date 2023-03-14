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

#if defined(BURGER_DARWIN) && !defined(__BRDARWINTYPES_H__)
#include "brdarwintypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MACOSX) || defined(DOXYGEN)

#if !defined(DOXYGEN)

struct _NSPoint;
struct _NSSize;
struct _NSRect;
struct _CGLContextObject;
struct _CGLPixelFormatObject;
struct _CGLRendererInfoObject;
struct _CGLPBufferObject;
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
	uint8_t m_bInstancesTested[DLL_COUNT];
	/** Flags to determine if a function was tested for loading \macosxonly */
	uint8_t m_bFunctionsTested[CALL_COUNT];

	static MacOSX g_Globals;

public:
	~MacOSX();

	static void* BURGER_API LoadLibraryIndex(eDLLIndex eIndex);
	static void* BURGER_API LoadFunctionIndex(eCallIndex eIndex);

	// macOS 10.10 functions
	static int BURGER_API getattrlistbulk(int dirfd, attrlist* attrList,
		void* attrBuf, uintptr_t attrBufSize, uint64_t options);
};
}

#endif
/* END */

#endif
