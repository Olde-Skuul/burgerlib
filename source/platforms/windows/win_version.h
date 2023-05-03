/***************************************

	Function to test what version of Windows is present

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_VERSION_H__
#define __WIN_VERSION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {

enum eVersion {
	/** Set if running on XP or higher. */
	kVersionXPOrGreater = 0x01,
	/** Set if running on XP3 or higher. */
	kVersionXP3OrGreater = 0x02,
	/** Set if Vista or higher. */
	kVersionVistaOrGreater = 0x04,
	/** Set if Windows 7 or higher. */
	kVersion7OrGreater = 0x08,
	/** Set if Windows 8 or higher. */
	kVersion8OrGreater = 0x10,
	/** Set if Windows 10 or higher. */
	kVersion10OrGreater = 0x20,
	/** Set if the rest of the flags are valid. */
	kVersionValid = 0x8000
};

extern uint32_t BURGER_API get_version_flags(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_XP_or_higher(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_XP3_or_higher(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_vista_or_higher(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_7_or_higher(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_8_or_higher(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API is_10_or_higher(void) BURGER_NOEXCEPT;

extern int BURGER_API VerQueryValueA(const void* pBlock, const char* pSubBlock,
	void** ppBuffer, uint_t* pLen) BURGER_NOEXCEPT;
extern int BURGER_API VerQueryValueW(const void* pBlock,
	const uint16_t* pSubBlock, void** ppBuffer, uint_t* pLen) BURGER_NOEXCEPT;
extern int BURGER_API GetFileVersionInfoA(const char* ptstrFilename,
	uint32_t dwHandle, uint32_t dwLen, void* pData) BURGER_NOEXCEPT;
extern int BURGER_API GetFileVersionInfoW(const uint16_t* ptstrFilename,
	uint32_t dwHandle, uint32_t dwLen, void* pData) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetFileVersionInfoSizeA(
	const char* ptstrFilename, unsigned long* pdwHandle) BURGER_NOEXCEPT;
extern uint32_t BURGER_API GetFileVersionInfoSizeW(
	const uint16_t* ptstrFilename, unsigned long* pdwHandle) BURGER_NOEXCEPT;
extern uint64_t BURGER_API get_file_version64(
	const uint16_t* pWindowsFilename) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_DirectX_version_via_file_versions(
	void) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_DirectX_version(void) BURGER_NOEXCEPT;

}}
/* END */

#endif
