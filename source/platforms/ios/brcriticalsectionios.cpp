/***************************************

	Class to handle critical sections, iOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_IOS)
#include "brassert.h"
#include "bratomic.h"
#include "brstringfunctions.h"
#include "brthread.h"

#include <errno.h>
#include <mach/mach_init.h>
#include <mach/mach_traps.h>
#include <mach/semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

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

	Initialize the condition variable

***************************************/

Burger::ConditionVariable::ConditionVariable() BURGER_NOEXCEPT
	: m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the
	// real thing
	BURGER_STATIC_ASSERT(
		sizeof(Burgerpthread_cond_t) == sizeof(pthread_cond_t));

	if (!pthread_cond_init(
			reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable), nullptr)) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	Release the resources

***************************************/

Burger::ConditionVariable::~ConditionVariable()
{
	if (m_bInitialized) {
		pthread_cond_destroy(
			reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable));
		m_bInitialized = FALSE;
	}
}

/***************************************

	Signal a waiting thread

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Signal(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {
		if (!pthread_cond_signal(
				reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable))) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Signal all waiting threads

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Broadcast(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {
		if (!pthread_cond_broadcast(
				reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable))) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Wait for a signal (With timeout)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Wait(
	CriticalSection* pCriticalSection, uint_t uMilliseconds) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {
		if (uMilliseconds == BURGER_MAXUINT) {
			if (!pthread_cond_wait(
					reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable),
					reinterpret_cast<pthread_mutex_t*>(
						&pCriticalSection->m_Lock))) {
				uResult = kErrorNone;
			}
		} else {

			// Use a timeout

			// Get the current time
			timeval CurrentTime;
			gettimeofday(&CurrentTime, nullptr);

			// Determine the time in the future to timeout at
			struct timespec StopTimeHere;
			uint_t uSeconds = uMilliseconds / 1000;
			// Get the remainder in NANOSECONDS
			uMilliseconds = (uMilliseconds - (uSeconds * 1000)) * 1000000;

			// Add to the current time
			uMilliseconds += CurrentTime.tv_usec;
			uSeconds += CurrentTime.tv_sec;
			// Handle wrap around
			if (uMilliseconds >= 1000000000) {
				uMilliseconds -= 1000000000;
				++uSeconds;
			}
			StopTimeHere.tv_sec = uSeconds;
			StopTimeHere.tv_nsec = uMilliseconds;
			int iResult;
			do {
				// Send the signal and possibly time out
				iResult = pthread_cond_timedwait(
					reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable),
					reinterpret_cast<pthread_mutex_t*>(
						&pCriticalSection->m_Lock),
					&StopTimeHere);
				// Interrupted?
			} while (iResult == EINTR);

			// W00t! We're good!
			if (!iResult) {
				uResult = kErrorNone;

				// Time out?
			} else if (iResult == ETIMEDOUT) {
				uResult = kErrorTimeout;
			}
			// Otherwise, leave uResult as an error
		}
	}
	return uResult;
}

#endif
