/***************************************

	Class for semaphores

	Unix version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_UNIX) && !defined(BURGER_DARWIN)
#include "bratomic.h"

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

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
	// Sanity check to verify the declaration matches the real thing
	BURGER_STATIC_ASSERT(sizeof(m_Semaphore) == sizeof(sem_t));

	// Initialize the semaphore
	sem_init(reinterpret_cast<sem_t*>(m_Semaphore), 0, uCount);
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

	sem_destroy(reinterpret_cast<sem_t*>(m_Semaphore));
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

	// Release the count immediately, because it's
	// possible that another thread, waiting for this semaphore,
	// can execute before the call to sem_post()
	// returns
	atomic_add(&m_uCount, 1);
	if (sem_post(reinterpret_cast<sem_t*>(m_Semaphore))) {
		// Error!!! Undo the atomic_add()
		atomic_add(&m_uCount, static_cast<uint32_t>(-1));
	} else {
		// A-Okay!
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	\brief Wait for a resource with a timeout.

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	posts to the semaphore or the time in milliseconds has elapsed. If the
	timeout is zero, do not block.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0
		means no wait, UINT32_MAX means never time out

	\return Zero on success, \ref kErrorTimeout on a timeout, or other error
		code in case of a semaphore failure

	\sa signal(void)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::wait_for_signal(
	uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	// Assume failure
	eError uResult = kErrorCantLock;

	// Trywait?
	if (!uMilliseconds) {

		// Use the fast function
		if (!sem_trywait(reinterpret_cast<sem_t*>(m_Semaphore))) {
			// Got it!
			atomic_add(&m_uCount, static_cast<uint32_t>(-1));
			uResult = kErrorNone;
		} else {
			uResult = kErrorTimeout;
		}

		// FOREVER?
	} else if (uMilliseconds == BURGER_MAXUINT) {

		// Use the special function for halt until acquired
		int iSemResult;
		do {
			iSemResult = sem_wait(reinterpret_cast<sem_t*>(m_Semaphore));
			// Got it?
			if (!iSemResult) {
				// Exit now
				atomic_add(&m_uCount, static_cast<uint32_t>(-1));
				uResult = kErrorNone;
				break;
			}
			// If the error was because of a system interrupt, try again
		} while (errno == EINTR);
	} else {
		// Posix doesn't use relative time. It uses
		// literal time.

		timeval CurrentTime;
		// Get the current time
		gettimeofday(&CurrentTime, nullptr);

		// Add the timeout to the current time to get the
		// timeout time

		// Split between seconds and MICROseconds
		uint32_t uSeconds = uMilliseconds / 1000U;
		uMilliseconds = (uMilliseconds - (uSeconds * 1000U)) * 1000U;

		uMilliseconds += CurrentTime.tv_usec;
		uSeconds += CurrentTime.tv_sec;
		// Handle wrap around
		if (uMilliseconds >= 1000000U) {
			uMilliseconds -= 1000000U;
			++uSeconds;
		}

		// Seconds and MICROseconds are ready.
		// Of course, timespec wants NANOseconds!

		timespec TimeSpecTimeOut;
		TimeSpecTimeOut.tv_sec = uSeconds;
		TimeSpecTimeOut.tv_nsec = uMilliseconds * 1000U;

		// Wait for the semaphore
		int iTest;
		do {
			iTest = sem_timedwait(
				reinterpret_cast<sem_t*>(m_Semaphore), &TimeSpecTimeOut);
			// Success?
			if (!iTest) {
				atomic_add(&m_uCount, static_cast<uint32_t>(-1));
				uResult = kErrorNone;
				break;
			}

			if (errno == ETIMEDOUT) {
				uResult = kErrorTimeout;
				break;
			}

			// Loop only on interrupts
		} while (errno == EINTR);
	}

	return uResult;
}

#endif
