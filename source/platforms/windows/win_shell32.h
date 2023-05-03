/***************************************

	Shims for shell32.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_SHELL32_H__
#define __WIN_SHELL32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern long BURGER_API SHGetKnownFolderPath(const GUID* pGuid, uint32_t uFlags,
	void* hHandle, uint16_t** ppResult) BURGER_NOEXCEPT;
extern eError BURGER_API add_group_to_program_menu(
	const char* pGroupName) BURGER_NOEXCEPT;
extern eError BURGER_API get_known_folder_path(
	Filename* pFilename, const GUID* pVista, int iXP) BURGER_NOEXCEPT;
}}
/* END */

#endif
