/***************************************

	Forward declarations of MacOS types

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_TYPES_H__
#define __MAC_TYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if !defined(DOXYGEN)
struct GetVolParmsInfoBuffer;
struct AEDesc;
struct ControlRecord;
struct MenuInfo;
struct Point;
struct Rect;
struct MacRegion;
struct BitMap;
struct PixMap;
struct PixPat;
struct Pattern;
struct Cursor;
struct RGBColor;
struct GrafPort;
struct CGrafPort;
struct QDGlobals;
struct CQDProcs;
struct OpaqueDialogPtr;
struct OpaqueWindowPtr;
struct OpaqueTSMDocumentID;
struct TSMTERec;
struct ListRec;
struct OpaqueListClickLoopProcPtr;
struct FSSpec;
struct RoutineDescriptor;
struct ProcessSerialNumber;
struct GDevice;
struct GammaTbl;

#if TARGET_API_MAC_CARBON
struct __CFBundle;
struct __CFString;
typedef OpaqueWindowPtr* WindowRef; ///< Alias for WindowRef for macOS
typedef OpaqueDialogPtr* DialogRef; ///< Alias for DialogRef for macOS
#else
typedef GrafPort* WindowRef;
typedef GrafPort* DialogRef;
#endif

#if defined(BURGER_MACCARBON)
typedef OpaqueListClickLoopProcPtr* ListClickLoopUPP;
#elif defined(BURGER_CFM)
typedef RoutineDescriptor* ListClickLoopUPP;
#else
typedef pascal void (*ListClickLoopUPP)(void);
#endif

#endif
/* END */

#endif
