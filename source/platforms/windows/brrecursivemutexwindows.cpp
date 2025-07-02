/***************************************

	Class to handle recursive mutex objects, Windows version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrecursivemutex.h"

#if defined(BURGER_WINDOWS)
#include "brassert.h"
#include "win_kernel32.h"
#include "win_version.h"
#include "win_windows.h"

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

	This function works both on WinXP-Vista and Windows 7 or higher

***************************************/

Burger::RecursiveMutex::RecursiveMutex() BURGER_NOEXCEPT
	: m_uOwnerThreadID(UINT32_MAX),
	  m_uCount(0),
	  m_bUseSRWLock(FALSE)
{
	// Sanity check to verify the declaration matches the real thing
	BURGER_STATIC_ASSERT(sizeof(CRITICAL_SECTION) == sizeof(m_PlatformMutex));

	// Only test if the SDK has this declared
#if defined(_WIN32_WINNT_VISTA)
	BURGER_STATIC_ASSERT(sizeof(_RTL_SRWLOCK) <= sizeof(m_PlatformMutex));
#endif

	// If running on Windows 7 or higher, use SRWLOCK
	if (Win32::is_7_or_higher()) {

		// Init the SRWLOCK
		m_bUseSRWLock = TRUE;
		Win32::InitializeSRWLock(
			reinterpret_cast<_RTL_SRWLOCK*>(m_PlatformMutex));

	} else {
		// Init the CriticalSection
		::InitializeCriticalSectionAndSpinCount(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex), 1000);
	}
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::RecursiveMutex::~RecursiveMutex()
{
	// Fun fact, a SRWLOCK doesn't need to be disposed of
	if (!m_bUseSRWLock) {
		::DeleteCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
	}
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void BURGER_API Burger::RecursiveMutex::lock(void) BURGER_NOEXCEPT
{
	if (!m_bUseSRWLock) {
		::EnterCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
	} else {
		// Is already owned by this thread?
		DWORD uThreadID = GetCurrentThreadId();
		if (m_uOwnerThreadID != uThreadID) {

			// No, try to take it
			Win32::AcquireSRWLockExclusive(
				reinterpret_cast<_RTL_SRWLOCK*>(m_PlatformMutex));

			// Since we won the lock race, take ownership
			m_uOwnerThreadID = uThreadID;
			m_uCount = 1;
		} else {

			// Just increase the reference count
			++m_uCount;
		}
	}
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t BURGER_API Burger::RecursiveMutex::try_lock(void) BURGER_NOEXCEPT
{
	// Assume failure
	uint_t bResult;

	// No, try to take it
	if (!m_bUseSRWLock) {
		bResult = static_cast<uint_t>(::TryEnterCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex)));
	} else {
		// Is already owned by this thread?
		DWORD uThreadID = GetCurrentThreadId();
		if (m_uOwnerThreadID != uThreadID) {

			bResult = Win32::TryAcquireSRWLockExclusive(
				reinterpret_cast<_RTL_SRWLOCK*>(m_PlatformMutex));

			if (bResult) {

				// Since we won the lock race, take ownership
				m_uOwnerThreadID = uThreadID;
				m_uCount = 1;
			}
		} else {
			// Just increase the reference count
			++m_uCount;
			bResult = TRUE;
		}
	}
	return bResult;
}

/***************************************

	\brief Unlocks the mutex

	Releases a lock on a mutex and if any other threads are waiting on this
	lock, they will obtain the lock and the other thread will continue
	execution. The caller will never block.

	\note This call MUST be preceded by a matching lock() call. Calling unlock()
	without a preceding lock() call will result in undefined behavior and in
	some cases can result in thread lock or a crash.

	\sa lock(), or try_lock()

***************************************/

void BURGER_API Burger::RecursiveMutex::unlock(void) BURGER_NOEXCEPT
{
	if (!m_bUseSRWLock) {
		::LeaveCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
	} else {

		// Are we screwed?
		DWORD uThreadID = GetCurrentThreadId();
		if (m_uOwnerThreadID != uThreadID) {
			do_assert(
				"Unlocking a RecursiveMutex that's not owned by this thread!",
				__FILE__, __LINE__);
		} else {

			// Release a reference
			if (!--m_uCount) {

				// Release the mutex, note, execute on another thread/process
				// will occur during the call, so release the owner now
				m_uOwnerThreadID = UINT32_MAX;

				Win32::ReleaseSRWLockExclusive(
					reinterpret_cast<_RTL_SRWLOCK*>(m_PlatformMutex));
			}
		}
	}
}

#endif
