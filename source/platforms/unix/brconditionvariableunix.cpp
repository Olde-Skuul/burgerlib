/***************************************

	Class to handle conditional variables

	Unix version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconditionvariable.h"

#if defined(BURGER_UNIX)
#include "brassert.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

/***************************************

	\brief Initialize a condition signaler

	\sa ~ConditionVariable()

***************************************/

Burger::ConditionVariable::ConditionVariable() BURGER_NOEXCEPT
	: m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the
	// real thing
	BURGER_STATIC_ASSERT(sizeof(m_ConditionVariable) == sizeof(pthread_cond_t));

	if (!pthread_cond_init(
			reinterpret_cast<pthread_cond_t*>(m_ConditionVariable), nullptr)) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Release the condition signaler's resources

	\sa ConditionVariable()

***************************************/

Burger::ConditionVariable::~ConditionVariable()
{
	if (m_bInitialized) {
		pthread_cond_destroy(
			reinterpret_cast<pthread_cond_t*>(m_ConditionVariable));
		m_bInitialized = FALSE;
	}
}

/***************************************

	\brief Signal a waiting thread

	If a thread is waiting for a signal, send a signal to the thread so it may
	continue execution. If no thread is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa broadcast() or wait(Mutex *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::signal(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {
		uResult = kErrorCantLock;
		if (!pthread_cond_signal(
				reinterpret_cast<pthread_cond_t*>(m_ConditionVariable))) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Signal all waiting threads

	If any threads are waiting for a signal, send a signal to all of them so
	they all may continue execution. If no thread is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa signal() or wait(Mutex *,uint32_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::broadcast(
	void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {
		uResult = kErrorCantLock;
		if (!pthread_cond_broadcast(
				reinterpret_cast<pthread_cond_t*>(m_ConditionVariable))) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Wait for a signal (With timeout)

	Halt the thread until a signal is received or if a timeout has elapsed. If
	any threads are waiting for a signal, send a signal to all of them so they
	all may continue execution. If no thread is waiting, do nothing.

	\param pMutex Pointer to a Mutex to use with this object
	\param uMilliseconds UINT32_MAX to wait forever, 0 for non-blocking

	\return Zero on success, non-zero on error.
	\sa signal() or broadcast()

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::wait(
	Mutex* pMutex, uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	if (m_bInitialized) {

		if (uMilliseconds == UINT32_MAX) {
			uResult = kErrorCantLock;
			if (!pthread_cond_wait(
					reinterpret_cast<pthread_cond_t*>(m_ConditionVariable),
					static_cast<pthread_mutex_t*>(
						pMutex->get_platform_mutex()))) {
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
			uSeconds += static_cast<uint_t>(CurrentTime.tv_sec);
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
					reinterpret_cast<pthread_cond_t*>(m_ConditionVariable),
					static_cast<pthread_mutex_t*>(pMutex->get_platform_mutex()),
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
