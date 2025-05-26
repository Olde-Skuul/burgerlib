/***************************************

	Class to handle threads, Linux version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_LINUX)
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

#if !defined(DOXYGEN)
#define MAX_RR_PRIORITY 99
#endif

/***************************************

	\brief Get the execution priority of a thread

	Get the execution priority of any thread using a \ref thread_ID_t.

	If \ref kThreadPriorityInvalid is returned, this feature is not
	supported.

	\returns An \ref eThreadPriority enumeration.

	\sa \ref Thread, or set_thread_priority(thread_ID_t, eThreadPriority)

***************************************/

Burger::eThreadPriority BURGER_API Burger::get_thread_priority(
	thread_ID_t uThreadID) BURGER_NOEXCEPT
{
	// Assume error
	eThreadPriority uResult = kThreadPriorityInvalid;
	sched_param Param;
	int iPolicy;

	// Get information about the thread
	if (!pthread_getschedparam(uThreadID, &iPolicy, &Param)) {

		// Linux used hard coded priorities based on policy
		int iPriority = Param.sched_priority;

		if ((iPolicy == SCHED_RR) || (iPolicy == SCHED_FIFO)) {
			if (iPriority > ((MAX_RR_PRIORITY * 3) / 4)) {
				uResult = kThreadPriorityRealTime;
			} else if (iPriority > (MAX_RR_PRIORITY / 2)) {
				uResult = kThreadPriorityHigh;
			} else if (iPriority > (MAX_RR_PRIORITY / 4)) {
				uResult = kThreadPriorityNormal;
			} else {
				uResult = kThreadPriorityLow;
			}
		} else {
			// Convert from enumeration to native priority values
			if (iPriority <= -20) {
				uResult = kThreadPriorityRealTime;
			} else if (iPriority <= -10) {
				uResult = kThreadPriorityHigh;
			} else if (iPriority <= 0) {
				uResult = kThreadPriorityNormal;
			} else {
				uResult = kThreadPriorityLow;
			}
		}
	}
	// Return error
	return uResult;
}

/***************************************

	\brief Set the execution priority of a thread

	Set the execution priority of any thread using a \ref thread_ID_t.

	If \ref kErrorNotSupportedOnThisPlatform is returned, this feature is not
	supported.

	\returns Zero if no error, non-zero on error.

	\sa \ref Thread, or get_thread_priority(thread_ID_t)

***************************************/

Burger::eError BURGER_API Burger::set_thread_priority(
	thread_ID_t uThreadID, eThreadPriority uThreadPriority) BURGER_NOEXCEPT
{
	// Assume uThreadID is bogus
	eError uResult = kErrorThreadNotFound;

	sched_param Param;
	int iPolicy;

	// Get information about the thread
	if (!pthread_getschedparam(uThreadID, &iPolicy, &Param)) {

		// Now assume the switch statement succeeds
		uResult = kErrorNone;

		// Convert from enumeration to native policy values
		switch (uThreadPriority) {
		case kThreadPriorityLow:
			iPolicy = SCHED_OTHER;
			break;

		case kThreadPriorityNormal:
			iPolicy = SCHED_OTHER;
			break;

		case kThreadPriorityHigh:
			iPolicy = SCHED_OTHER;
			break;

		case kThreadPriorityRealTime:
			iPolicy = SCHED_RR;
			break;

		// Bad enumeration?
		default:
			uResult = kErrorInvalidParameter;
			break;
		}

		// If not a bad enumeration, set it
		if (!uResult) {

			// Of course, priorities are a range, instead of values, like every
			// other platform
			int iMin = sched_get_priority_min(iPolicy);
			int iMax = sched_get_priority_max(iPolicy);

			// Convert from enumeration to native priority values
			int iPriority;
			switch (uThreadPriority) {
			case kThreadPriorityLow:
				iPriority = iMin;
				break;

			case kThreadPriorityNormal:
				iPriority = iMin + ((iMax - iMin) / 2);
				break;

			case kThreadPriorityHigh:
				iPriority = iMin + (((iMax - iMin) * 3) / 4);
				break;

			case kThreadPriorityRealTime:
			default:
				iPriority = iMax;
				break;
			}

			// Set the new thread priority
			Param.sched_priority = iPriority;
			// Was it successful?
			if (pthread_setschedparam(uThreadID, iPolicy, &Param)) {
				// Likely failure is lack of permissions
				uResult = kErrorThreadNotModified;
			} else {
				uResult = kErrorNone;
			}
		}
	}
	// Return error
	return uResult;
}

#endif
