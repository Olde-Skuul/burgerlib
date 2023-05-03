/***************************************

	Registry functions for Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_REGISTRY_H__
#define __WIN_REGISTRY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern long BURGER_API create_user_registry_key(
	const char* pKey, const char* pSubKey, const char* pData) BURGER_NOEXCEPT;
extern void BURGER_API associate_file_extension_to_exe(
	const char* pFileExtension, const char* pDescription,
	const char* pProgramID) BURGER_NOEXCEPT;

// Quicktime functions for Windows
extern eError BURGER_API GetQTFolderFromRegistry(const char* pSubKey,
	const char* pValueName, char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern uint_t BURGER_API GetPathToQuickTimeFolder(
	char* pBuffer, uint32_t uSize, uint32_t* pReserved) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetQTSystemDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetQTApplicationDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetQTExtensionDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetQTComponentDirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern HINSTANCE__* BURGER_API QTLoadLibrary(
	const char* pDLLName) BURGER_NOEXCEPT;
}}

/* END */

#endif
