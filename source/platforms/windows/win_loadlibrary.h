/***************************************

	Library loader functions specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_LOADLIBRARY_H__
#define __WIN_LOADLIBRARY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern HINSTANCE__* BURGER_API LoadLibraryA(const char* pInput) BURGER_NOEXCEPT;
extern HINSTANCE__* BURGER_API LoadLibraryW(
	const uint16_t* pInput) BURGER_NOEXCEPT;
extern HINSTANCE__* BURGER_API LoadLibraryExA(
	const char* pInput, void* hFile, uint32_t uFlags) BURGER_NOEXCEPT;
extern HINSTANCE__* BURGER_API LoadLibraryExW(
	const uint16_t* pInput, void* hFile, uint32_t uFlags) BURGER_NOEXCEPT;
}}

/* END */

#endif
