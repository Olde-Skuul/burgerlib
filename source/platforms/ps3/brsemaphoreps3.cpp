/***************************************

	Class for semaphores

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_PS3)
#include <cell/atomic.h>
#include <sys/synchronization.h>

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
	sys_semaphore_attribute_t attr;
	sys_semaphore_attribute_initialize(attr);
	sys_semaphore_create(&m_uSemaphore, &attr,
		static_cast<sys_semaphore_value_t>(uCount), 32768);
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
	sys_semaphore_destroy(m_uSemaphore);
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
	cellAtomicIncr32(const_cast<uint32_t*>(&m_uCount));
	if (sys_semaphore_post(m_uSemaphore, 1) != CELL_OK) {
		// Error!!! Undo the cellAtomicIncr32()
		cellAtomicDecr32(const_cast<uint32_t*>(&m_uCount));
	} else {
		// A-Okay!
		uResult = kErrorNone;
	}

	return uResult;
}

/*! ************************************

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
	eError uResult;

	// Do a trywait instead?
	if (!uMilliseconds) {

		// Try to acquire, but don't wait
		int iResult = sys_semaphore_trywait(m_uSemaphore);
		if (iResult == CELL_OK) {
			uResult = kErrorNone;
		} else if (iResult == EBUSY) {
			uResult = kErrorTimeout;
		} else {
			uResult = kErrorCantLock;
		}
	} else {

		// Perform a timeout
		usecond_t uMilliseconds64;

		// Convert milliseconds to microseconds
		if (uMilliseconds == UINT32_MAX) {

			// PS3 uses zero for infinity
			uMilliseconds64 = 0;
		} else {
			uMilliseconds64 = static_cast<usecond_t>(uMilliseconds) * 1000ULL;
		}

		// Acquire a resource
		int iResult = sys_semaphore_wait(m_uSemaphore, uMilliseconds64);

		if (iResult == CELL_OK) {

			// Got the lock. Decrement the count
			cellAtomicDecr32(const_cast<uint32_t*>(&m_uCount));
			uResult = kErrorNone;

		} else if (iResult == ETIMEDOUT) {

			// Timeout gets a special error
			uResult = kErrorTimeout;
		} else {
			uResult = kErrorCantLock;
		}
	}
	return uResult;
}

#endif
