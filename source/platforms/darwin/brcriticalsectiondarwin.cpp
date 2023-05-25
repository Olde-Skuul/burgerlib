/***************************************

	Class to handle critical sections, Darwin version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

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

	\class Burger::Semaphore
	\brief Class for creating a semaphore

	In a multiprocessor system, it's necessary to have multiple
	resources available to multiple threads at the same time. To prevent
	resource starvation, this class is decremented until the number of
	available resources is exhausted and then it will halt future threads
	from executing until resources are made available.

	Further reading http://en.wikipedia.org/wiki/Semaphore_(programming)

	\note On operating systems that don't have native semaphore support, such as
		MSDOS, this class will always return error codes for all calls.

	\sa \ref CriticalSection and \ref Thread

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT
	: m_uCount(uCount),
	  m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the
	// real thing
	BURGER_STATIC_ASSERT(sizeof(m_uSemaphore) == sizeof(semaphore_t));

	// Initialize the semaphore
	task_t tOwner = mach_task_self();
	m_uOwner = tOwner;
	if (semaphore_create(tOwner, &m_uSemaphore, SYNC_POLICY_FIFO, uCount) ==
		KERN_SUCCESS) {
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
	if (m_bInitialized) {
		semaphore_destroy(m_uOwner, m_uSemaphore);
		m_bInitialized = FALSE;
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
	if (m_bInitialized) {
		// Infinite wait?
		if (uMilliseconds == UINT32_MAX) {

			// Use the special function for halt until acquired
			int iSemResult;
			do {
				iSemResult = semaphore_wait(m_uSemaphore);
				// Got it?
				if (iSemResult == KERN_SUCCESS) {
					// Exit now
					uResult = kErrorNone;
					break;
				}
				// If the error was because of a system interrupt, try again
			} while (iSemResult == KERN_ABORTED);

		} else {

			// Data for the timer thread
			mach_timespec_t TimeoutData;
			uint32_t uSeconds = uMilliseconds / 1000;
			uint32_t uNanoSeconds = (uMilliseconds - (uSeconds * 1000)) * 1000;
			TimeoutData.tv_sec = uSeconds;
			TimeoutData.tv_nsec = uNanoSeconds;

			kern_return_t kError;
			do {

				kError = semaphore_timedwait(m_uSemaphore, TimeoutData);
				if (kError == KERN_SUCCESS) {
					uResult = kErrorNone;
					break;
				}
				if (kError == KERN_OPERATION_TIMED_OUT) {
					uResult = kErrorTimeout;
					break;
				}
			} while (kError == KERN_ABORTED);
		}
		// If the lock was acquired, decrement the count
		if (!uResult) {
			atomic_add(&m_uCount, static_cast<uint32_t>(-1));
		}
	}
	return uResult;
}

/***************************************

	\brief Release a lock on a semaphore resource

	After a thread has acquired a resource via a semaphore, release it with
	this call once the resource is no longer needed.

	\return Zero on success, nonzero in the case of a semaphore failure

	\sa acquire(void) or try_acquire(uint_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::release(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorCantUnlock;
	if (m_bInitialized) {
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
	}
	return uResult;
}

#endif
