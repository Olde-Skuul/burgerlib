/***************************************

	Error code handler for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_ERROR_H__
#define __WIN_ERROR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern eError BURGER_API format_message(
	long lHResult, String* pOutput) BURGER_NOEXCEPT;
extern const char* BURGER_API get_DirectX_error_string(
	long lHResult) BURGER_NOEXCEPT;
extern eError BURGER_API get_error_string(
	long lHResult, String* pOutput) BURGER_NOEXCEPT;
}}
/* END */

#endif
