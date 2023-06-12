/***************************************

	MacOS Apple Events code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_APPLEEVENTS_H__
#define __MAC_APPLEEVENTS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __MAC_TYPES_H__
#include "mac_types.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {
extern int16_t BURGER_API get_FSSpec_nth_desc(
	AEDesc* pList, long iIndex, FSSpec* pFSSpec) BURGER_NOEXCEPT;
}}

/* END */

#endif
