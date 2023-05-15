/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_WINDOWS)
#include "brassert.h"
#include "bratomic.h"
#include "brthread.h"

// InitializeCriticalSectionAndSpinCount() is minimum XP

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	// Safety switch to verify the declaration matches the real thing
	BURGER_STATIC_ASSERT(sizeof(CRITICAL_SECTION) == sizeof(m_Lock));

	::InitializeCriticalSectionAndSpinCount(
		reinterpret_cast<CRITICAL_SECTION*>(m_Lock), 1000);
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_Lock));
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa Burger::CriticalSection::unlock()

***************************************/

void Burger::CriticalSection::lock(void) BURGER_NOEXCEPT
{
	::EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_Lock));
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock() and unlock()

***************************************/

uint_t Burger::CriticalSection::try_lock(void) BURGER_NOEXCEPT
{
	return static_cast<uint_t>(
		::TryEnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_Lock)));
}

/***************************************

	\brief Unlocks the mutex

	Releases a lock on a mutex and if any other threads are waiting on this
	lock, they will obtain the lock and the other thread will continue
	execution. The caller will never block.

	\note This call MUST be preceded by a matching lock() call. Calling unlock()
	without a preceding lock() call will result in undefined behavior and in
	some cases can result in thread lock or a crash.

	\sa lock()

***************************************/

void Burger::CriticalSection::unlock(void) BURGER_NOEXCEPT
{
	::LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m_Lock));
}

/***************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a binary
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT
	: m_uCount(uCount),
	  m_bInitialized(FALSE)
{
	// Get the maximum semaphores
	uint32_t uMax = uCount + 32768U;

	// Did it wrap (Overflow?)
	if (uMax < uCount) {
		// Use max
		uMax = UINT32_MAX;
	}
	m_pSemaphore = CreateSemaphoreW(
		nullptr, static_cast<LONG>(uCount), static_cast<LONG>(uMax), nullptr);
	if (m_pSemaphore) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Shut down a semaphore

	Release any operating system resources allocated in the creation of the
	semaphore.

	\note Care should be exercised in ensuring that all threads are are waiting
	on semaphores have been shutdown down already.

	\sa Semaphore(uint32_t)

***************************************/

Burger::Semaphore::~Semaphore()
{
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		CloseHandle(hSemaphore);
		m_pSemaphore = nullptr;
	}
	m_uCount = 0;
}

/***************************************

	\brief Acquire a lock on a semaphore resource with a timeout

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	releases the semaphore or the time in milliseconds has elapsed. If the
	timeout is zero, return immediately with a non-zero error code.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0
		means no wait, \ref UINT32_MAX means infinite

	\return Zero on success, One on a timeout, and non Zero or One in the case
		of a semaphore failure

	\sa acquire(void) or release(void)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::try_acquire(
	uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	// Assume failure
	eError uResult = kErrorCantLock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		DWORD dwMilliseconds;
#if INFINITE == UINT32_MAX
		dwMilliseconds = static_cast<DWORD>(uMilliseconds);
#else
		if (uMilliseconds == UINT32_MAX) {
			dwMilliseconds = INFINITE;
		} else {
			dwMilliseconds = static_cast<DWORD>(uMilliseconds);
		}
#endif
		// Acquire a lock
		DWORD uWait = WaitForSingleObject(hSemaphore, dwMilliseconds);
		if (uWait == WAIT_OBJECT_0) {

			// Got the lock. Decrement the count
			atomic_add(&m_uCount, static_cast<uint32_t>(-1));
			uResult = kErrorNone;

		} else if (uWait == WAIT_TIMEOUT) {

			// Timeout gets a special error
			uResult = kErrorTimeout;
		}
	}
	return uResult;
}

/***************************************

	\brief Release a lock on a semaphore resource

	After a thread has acquired a resource via a semaphore, release it with this
	call once the resource is no longer needed.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa acquire(void) or try_acquire(uint_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::release(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorCantUnlock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		atomic_add(&m_uCount, 1);
		if (ReleaseSemaphore(hSemaphore, 1, nullptr) == FALSE) {
			// Error!!! Undo the atomic_add()
			atomic_add(&m_uCount, static_cast<uint32_t>(-1));
		} else {
			// A-Okay!
			uResult = kErrorNone;
		}
	}
	return uResult;
}

#endif
