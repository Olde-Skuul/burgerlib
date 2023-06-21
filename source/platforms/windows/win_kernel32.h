/***************************************

	Memory manager functions specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_KERNEL32_H__
#define __WIN_KERNEL32_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern uint_t BURGER_API GetSystemWow64DirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern uint_t BURGER_API GetSystemWow64DirectoryW(
	uint16_t* pBuffer, uint32_t uSize) BURGER_NOEXCEPT;
extern int BURGER_API IsDebuggerPresent(void) BURGER_NOEXCEPT;
extern long BURGER_API SetThreadDescription(
	void* hThread, const uint16_t* lpThreadDescription) BURGER_NOEXCEPT;
extern void BURGER_API InitializeSRWLock(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT;
extern void BURGER_API AcquireSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT;
extern uint_t BURGER_API TryAcquireSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT;
extern void BURGER_API ReleaseSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT;
extern void BURGER_API throw_thread_naming_exception(
	uint32_t uThreadID, const char* pThreadName) BURGER_NOEXCEPT;
extern void BURGER_API set_thread_name(const char* pName,
	void* hThread = nullptr, uint32_t uThreadId = 0) BURGER_NOEXCEPT;
}}
/* END */

#endif
