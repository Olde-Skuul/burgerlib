/***************************************

	Shims for shlwapi.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_SHLWAPI_H__
#define __WIN_SHLWAPI_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern int BURGER_API PathSearchAndQualifyA(
	const char* pszPath, char* pszBuf, uint32_t cchBuf) BURGER_NOEXCEPT;
extern int BURGER_API PathSearchAndQualifyW(
	const uint16_t* pszPath, uint16_t* pszBuf, uint32_t cchBuf) BURGER_NOEXCEPT;
}}
/* END */

#endif
