/***************************************

	Class to handle mutex objects, Xbox 360 version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmutex.h"

#if defined(BURGER_XBOX360)
#include "brassert.h"

#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

	Initialize the spin count to 1000 since this class is usually used for quick
	data locks

***************************************/

Burger::Mutex::Mutex() BURGER_NOEXCEPT: m_uOwnerThreadID(UINT32_MAX)
{
	// Safety switch to verify the declaration matches the real thing
	BURGER_STATIC_ASSERT(sizeof(CRITICAL_SECTION) == sizeof(m_PlatformMutex));

	RtlInitializeCriticalSectionAndSpinCount(
		reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex), 1000);
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::Mutex::~Mutex()
{
	RtlDeleteCriticalSection(
		reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void BURGER_API Burger::Mutex::lock() BURGER_NOEXCEPT
{
	// Is already owned by this thread?
	DWORD uThreadID = GetCurrentThreadId();
	if (m_uOwnerThreadID == uThreadID) {
		do_assert("Double locking a Mutex will freeze this thread!", __FILE__,
			__LINE__);
	} else {
		RtlEnterCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));

		// Since we won the lock race, take ownership
		m_uOwnerThreadID = uThreadID;
	}
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t BURGER_API Burger::Mutex::try_lock() BURGER_NOEXCEPT
{
	// Assume failure
	uint_t bResult = FALSE;

	// Is already owned by this thread?
	DWORD uThreadID = GetCurrentThreadId();
	if (m_uOwnerThreadID != uThreadID) {

		// No, try to take it
		bResult = RtlTryEnterCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
		if (bResult) {

			// Since we won the lock race, take ownership
			m_uOwnerThreadID = uThreadID;
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

void BURGER_API Burger::Mutex::unlock() BURGER_NOEXCEPT
{
	// Are we screwed?
	DWORD uThreadID = GetCurrentThreadId();
	if (m_uOwnerThreadID != uThreadID) {
		do_assert("Unlocking a Mutex that's not owned by this thread!",
			__FILE__, __LINE__);
	} else {
		m_uOwnerThreadID = UINT32_MAX;
		RtlLeaveCriticalSection(
			reinterpret_cast<CRITICAL_SECTION*>(m_PlatformMutex));
	}
}

#endif
