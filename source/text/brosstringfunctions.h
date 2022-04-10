/***************************************

	Operating system string functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROSSTRINGFUNCTIONS_H__
#define __BROSSTRINGFUNCTIONS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#if defined(BURGER_DARWIN) && !defined(__BRDARWINTYPES_H__)
#include "brdarwintypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {

extern eError BURGER_API GetUserLoginName(String* pOutput) BURGER_NOEXCEPT;
extern eError BURGER_API GetUserRealName(String* pOutput) BURGER_NOEXCEPT;
extern eError BURGER_API GetMachineName(String* pOutput) BURGER_NOEXCEPT;

#if defined(BURGER_MAC) || defined(DOXYGEN)
extern eError BURGER_API GetOSString(
	String* pOutput, int16_t iStringID) BURGER_NOEXCEPT;
extern eError BURGER_API GetOSIndString(
	String* pOutput, int16_t iStringID, int16_t iIndex) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_MACOS) || defined(DOXYGEN)
extern eError BURGER_API GetMacModelIdentifier(String* pOutput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_MACCARBON) || defined(BURGER_DARWIN)) || defined(DOXYGEN)
extern void BURGER_API StringCopy(
	String* pOutput, const __CFString* pInput) BURGER_NOEXCEPT;
#endif
}
/* END */

#endif
