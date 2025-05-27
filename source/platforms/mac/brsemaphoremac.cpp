/***************************************

	Class for semaphores, MacOS version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_MAC)
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"

#include <MacErrors.h>
#include <Multiprocessing.h>
#include <Threads.h>

/***************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	For MacOS 68K, there's a limit of 32 threads for use against a semaphore.
	This should not be an issue since it's rare for having more than 4 threads.
	Also take into account that there is no 680x0 MacOS system that has more
	than one CPU.

	\param uCount Initial number of resources available (0 means a empty
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT: m_uCount(uCount)
{
#if defined(BURGER_PPC)
	// Initialize the semaphore
	MPCreateSemaphore(
		INT32_MAX, uCount, reinterpret_cast<MPSemaphoreID*>(&m_pSemaphore));

#else
	// No threads are waiting on this semaphore
	m_uWaitingCount = 0;

	// Is there an initial value?
	uint32_t uOwnersCount;
	if (uCount) {

		// Save this thread ID and the initial count
		ThreadID MyID = kNoThreadID;
		MacGetCurrentThread(&MyID);
		m_Owners[0] = MyID;
		m_OwnersCount[0] = uCount;
		uOwnersCount = 1U;
	} else {
		uOwnersCount = 0;
	}
	// Set the number of owners of this Semaphore
	// 1 or 0
	m_uOwnersCount = uOwnersCount;
#endif
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
#if defined(BURGER_PPC)
	MPSemaphoreID pSemaphore = static_cast<MPSemaphoreID>(m_pSemaphore);
	if (pSemaphore) {
		// Dispose of the semaphore
		MPDeleteSemaphore(pSemaphore);
		m_pSemaphore = nullptr;
		MPYield();
	}
#else
	// Clear the caches
	m_uWaitingCount = 0;
	m_uOwnersCount = 0;
#endif
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

#if defined(BURGER_PPC)
	MPSemaphoreID pSemaphore = static_cast<MPSemaphoreID>(m_pSemaphore);
	if (pSemaphore) {
		// Signal the semaphore
		OSStatus iErr = MPSignalSemaphore(pSemaphore);

		// Allow the another thread to act on the semaphore
		MPYield();

		// No error? Got it!
		if (!iErr) {
			uResult = kErrorNone;
		}
	}
#else

	// Start by getting this thread's id
	ThreadID MyID = kNoThreadID;
	MacGetCurrentThread(&MyID);

	// Check if this thread is trying to signal to something it doesn't own
	uint32_t uOwnersCount = m_uOwnersCount;
	uint32_t i = 0;
	if (uOwnersCount) {
		do {
			// Found it?
			if (m_Owners[i] == MyID) {
				break;
			}
		} while (++i < uOwnersCount);
	}

	// Valid call?
	if (i != uOwnersCount) {

		// Likely this is good release
		uResult = kErrorNone;

		// Release a reference
		if (!--m_OwnersCount[i]) {

			// If it got down to zero, remove this thread from the owner list
			--uOwnersCount;
			m_uOwnersCount = uOwnersCount;
			uint32_t uUnits = uOwnersCount - i;
			memory_move(
				&m_Owners[i], &m_Owners[i + 1], sizeof(m_Owners[0]) * uUnits);
			memory_move(&m_OwnersCount[i], &m_OwnersCount[i + 1],
				sizeof(m_OwnersCount[0]) * uUnits);

			// Is there a waiting thread?
			uint32_t uWaitingCount = m_uWaitingCount;
			if (uWaitingCount) {

				// Let's get ready to switch
				ThreadBeginCritical();

				// Get the next pending ThreadID
				MyID = m_Waiting[0];

				// Remove the entry from the list
				--uWaitingCount;
				m_uWaitingCount = uWaitingCount;
				memory_move(&m_Waiting[0], &m_Waiting[1],
					sizeof(m_Waiting[0]) * uWaitingCount);

				// Hard code semaphore count to the awaiting thread to 1
				// This completes the wait_for_signal() call by setting the
				// count to 1
				i = 0;
				if (uOwnersCount) {
					do {
						// Found it?
						if (m_Owners[i] == MyID) {
							m_OwnersCount[i] = 1U;
							break;
						}
					} while (++i < uOwnersCount);
				}

				// Not found? Weird!
				if (uOwnersCount == i) {
					// Add it to the list of owners
					if (uOwnersCount < BURGER_ARRAYSIZE(m_Owners)) {
						m_Owners[uOwnersCount] = MyID;
						m_OwnersCount[uOwnersCount] = 1U;
						m_uOwnersCount = uOwnersCount + 1U;
					}
				}

				// Task switch to the waiting thread
				SetThreadStateEndCritical(MyID, kReadyThreadState, kNoThreadID);
			}
		}
	}

#endif
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

#if defined(BURGER_PPC)
	MPSemaphoreID pSemaphore = static_cast<MPSemaphoreID>(m_pSemaphore);
	if (pSemaphore) {
		// Signal the semaphore
		OSStatus iErr =
			MPWaitOnSemaphore(pSemaphore, static_cast<Duration>(uMilliseconds));

		// Did it timeout?
		if (iErr == kMPTimeoutErr) {
			uResult = kErrorTimeout;

			// No error? Got it!
		} else if (!iErr) {
			uResult = kErrorNone;
		}
	}

#else

	// Start by getting the this thread's id
	ThreadID MyID = kNoThreadID;
	MacGetCurrentThread(&MyID);

	// Simple, see if it's already owned by this thread?
	uint32_t uOwnersCount = m_uOwnersCount;
	if (uOwnersCount) {
		uint32_t i = 0;
		do {
			if (m_Owners[i] == MyID) {

				// Increment the signal
				++m_OwnersCount[i];

				// Profit!
				uResult = kErrorNone;
				break;
			}
		} while (++i < uOwnersCount);
	}

	// Was it not found in the owner's list?
	if (uResult) {

		// Add to the owners list (Waking up a thread)
		if (uOwnersCount < BURGER_ARRAYSIZE(m_Owners)) {
			m_Owners[uOwnersCount] = MyID;
			m_OwnersCount[uOwnersCount] = 1U;
			uResult = kErrorNone;
			m_uOwnersCount = uOwnersCount + 1U;
		} else {

			// Wait until it's signaled?
			if (uMilliseconds) {
				ThreadBeginCritical();

				// Can it be put to sleep?
				uint32_t uWaitingCount = m_uWaitingCount;
				if (uWaitingCount < BURGER_ARRAYSIZE(m_Waiting)) {
					m_Waiting[uWaitingCount] = MyID;
					m_uWaitingCount = uWaitingCount + 1U;

					// Apply the night-night stick on this thread until it's
					// restarted with a call to signal()
					SetThreadStateEndCritical(
						MyID, kStoppedThreadState, kNoThreadID);

				} else {
					ThreadEndCritical();
					uResult = kErrorTimeout;
				}
			} else {

				// Is this tryacquire? Just return with a failure
				uResult = kErrorTimeout;
			}
		}
	}
#endif
	return uResult;
}

#endif
