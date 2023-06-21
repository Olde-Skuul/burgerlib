/***************************************

	Class for semaphores

	Darwin version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_DARWIN)
#include "bratomic.h"

#include <mach/mach_init.h>
#include <mach/semaphore.h>

// Workaround. The header for task.h for 64 bit Intel CPUs is MISSING! Manually
// inserted the prototypes
#if defined(__x86_64__)
extern kern_return_t semaphore_create(
	task_t task, semaphore_t* semaphore, int policy, int value);
extern kern_return_t semaphore_destroy(task_t task, semaphore_t semaphore);
#else
#include <mach/task.h>
#endif

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
	// Sanity check to verify the declaration in matches the real thing
	BURGER_STATIC_ASSERT(sizeof(m_uSemaphore) == sizeof(semaphore_t));

	// Initialize the semaphore and keep the thread ID
	task_t tOwner = mach_task_self();
	m_uOwner = tOwner;
	semaphore_create(tOwner, &m_uSemaphore, SYNC_POLICY_FIFO, uCount);
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
	semaphore_destroy(m_uOwner, m_uSemaphore);
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
	// can execute before the call to ReleaseSemaphore()
	// returns
	atomic_add(&m_uCount, 1);
	if (semaphore_signal(m_uSemaphore) != KERN_SUCCESS) {
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

	// Infinite wait?
	if (uMilliseconds == UINT32_MAX) {

		// Use the special function for halt until acquired
		int iSemResult;
		do {
			iSemResult = semaphore_wait(m_uSemaphore);

			// Got it?
			if (iSemResult == KERN_SUCCESS) {
				// Exit now
				atomic_add(&m_uCount, static_cast<uint32_t>(-1));
				uResult = kErrorNone;
				break;
			}
			// If the error was because of a system interrupt, try again
		} while (iSemResult == KERN_ABORTED);

	} else {

		// Data for the timer thread
		mach_timespec_t TimeoutData;

		// Note, if uMilliseconds is zero, it will set TimeoutData
		// to zero. Which then makes semaphore_timedwait() into a
		// trywait() call

		// Verified by looking at the source code of Darwin
		// semaphore_timedwait()

		uint32_t uSeconds = uMilliseconds / 1000;
		uint32_t uNanoSeconds = (uMilliseconds - (uSeconds * 1000)) * 1000;
		TimeoutData.tv_sec = uSeconds;
		TimeoutData.tv_nsec = uNanoSeconds;

		kern_return_t iError;
		do {

			iError = semaphore_timedwait(m_uSemaphore, TimeoutData);
			if (iError == KERN_SUCCESS) {
				atomic_add(&m_uCount, static_cast<uint32_t>(-1));
				uResult = kErrorNone;
				break;
			}

			// Timed out, including zero
			if (iError == KERN_OPERATION_TIMED_OUT) {
				uResult = kErrorTimeout;
				break;
			}
		} while (iError == KERN_ABORTED);
	}

	return uResult;
}

#endif
