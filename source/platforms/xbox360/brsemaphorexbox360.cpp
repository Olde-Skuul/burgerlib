/***************************************

	Class for semaphores

	Xbox 360 version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_XBOX360)
#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a empty
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT: m_uCount(uCount)
{
	// Create an artificial limit of 32K semaphores
	m_pSemaphore = CreateSemaphoreA(nullptr, uCount, 32768, nullptr);
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

	\brief Signal a semaphore

	When a resource is made available, signal that one is available with this
	call. This function will increment the count, and may release a waiting
	thread.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa wait_for_signal(uint32_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::signal(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorCantUnlock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {

		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		_InterlockedIncrement(reinterpret_cast<volatile long*>(&m_uCount));
		if (ReleaseSemaphore(hSemaphore, 1, nullptr) == FALSE) {

			// Error!!! Undo the _InterlockedIncrement()
			_InterlockedDecrement(reinterpret_cast<volatile long*>(&m_uCount));
		} else {

			// A-Okay!
			uResult = kErrorNone;
		}
	}
	return uResult;
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

Burger::eError BURGER_API Burger::Semaphore::wait_for_signal(
	uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	// Assume failure
	eError uResult = kErrorCantLock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {

		// Wait for a signal
		// If dwMilliseconds is zero, then it will immediately return
		// If dwMilliseconds is UINT32_MAX, it will wait FOREVER
		DWORD uWait = WaitForSingleObject(hSemaphore, uMilliseconds);
		if (uWait == WAIT_OBJECT_0) {

			// Got the signal. Decrement the count
			_InterlockedDecrement(reinterpret_cast<volatile long*>(&m_uCount));
			uResult = kErrorNone;
		} else if (uWait == WAIT_TIMEOUT) {

			// Timeout gets a special error
			uResult = kErrorTimeout;
		}
	}
	return uResult;
}

#endif
