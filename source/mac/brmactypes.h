/***************************************

	Typedefs specific to Mac OS Carbon and Classic

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACTYPES_H__
#define __BRMACTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MAC) && !defined(DOXYGEN)
struct Point;
struct Rect;
struct FSRef;
struct UTCDateTime;
struct CCrsr;
struct GrafPort;
struct OpaqueDialogPtr;
struct FSSpec;
struct OpaqueCFragConnectionID;
struct OpaqueFSIterator;
struct ProcessSerialNumber;
#if TARGET_API_MAC_CARBON
typedef struct OpaqueDialogPtr* DialogRef;	///< Alias for DialogRef for MacOS
#else
typedef struct GrafPort *DialogRef;
#endif
#endif
/* END */

#endif
