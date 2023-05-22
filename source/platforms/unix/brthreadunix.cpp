/***************************************

	Class to handle critical sections, Unix version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_UNIX)
#include "brintrinsics.h"
#include "unix_shims.h"

#include <pthread.h>
#include <signal.h>

#if !defined(DOXYGEN)
// For spawned threads, mask these signals
static const int g_SignalList[] = {SIGQUIT, SIGHUP, SIGINT, SIGPIPE, SIGALRM,
	SIGTERM, SIGCHLD, SIGWINCH, SIGPROF, SIGVTALRM};

// Function to return a pthread key
static pthread_key_t BURGER_API pthread_key_create(void) BURGER_NOEXCEPT
{
	pthread_key_t iResult;
	if (pthread_key_create(&iResult, nullptr)) {
		// It failed, use this as a marker for failure
		iResult = INT32_MAX;
	}
	return iResult;
}
// Global Thread Local Storage index
static pthread_key_t gStorage = pthread_key_create();
#endif

/***************************************

	\brief Return the ID of the current thread

	Query the operating system for the ID number of the currently executing
	thread.

	If zero is returned, this feature is not supported

	\returns ID of the thread or 0 on unsupported systems, like MSDOS.

	\sa \ref Thread

***************************************/

Burger::ThreadID BURGER_API Burger::get_ThreadID(void) BURGER_NOEXCEPT
{
	// Can't use reinterpret_cast<> because some platforms use a uintptr_t and
	// others are a pthread_t *
	return (ThreadID)pthread_self();
}

#if !defined(BURGER_LINUX)
/***************************************

	\brief Get the execution priority of a thread

	Get the execution priority of any thread using a \ref ThreadID.

	If \ref kThreadPriorityInvalid is returned, this feature is not
	supported.

	\returns An \ref eThreadPriority enumeration.

	\sa \ref Thread, or set_thread_priority(ThreadID, eThreadPriority)

***************************************/

Burger::eThreadPriority BURGER_API Burger::get_thread_priority(
	ThreadID uThreadID) BURGER_NOEXCEPT
{
	// Assume error
	eThreadPriority uResult = kThreadPriorityInvalid;
	sched_param Param;
	int iPolicy;

	// Get information about the thread
	if (!pthread_getschedparam((pthread_t)uThreadID, &iPolicy, &Param)) {

		// Since the policy COULD affect the range, ask the operating system for
		// these values
		int iPriority = Param.sched_priority;

		// Switch usually is 5 and 36
		int iMaxPriority = sched_get_priority_max(iPolicy);

		// Convert from enumeration to native priority values
		if (iPriority >= iMaxPriority) {
			uResult = kThreadPriorityRealTime;
		} else {

			// Lowest priority
			int iMinPriority = sched_get_priority_min(iPolicy);

			// Value range
			int iRange = iMaxPriority - iMinPriority;

			// Normal is halfway
			int iNormal = iMinPriority + (iRange / 2);

			// High is halfway between normal and highest
			int iHigh = iMinPriority + ((iRange * 3) / 4);

			// Because Apple... Use these defaults instead
#if defined(BURGER_DARWIN)
			if ((iMinPriority == 15) && (iMaxPriority == 47)) {
				iNormal = 37;
				iHigh = 45;
			}
#endif

			if (iPriority >= iHigh) {
				uResult = kThreadPriorityHigh;
			} else if (iPriority >= iNormal) {
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

	Set the execution priority of any thread using a \ref ThreadID.

	If \ref kErrorNotSupportedOnThisPlatform is returned, this feature is not
	supported.

	\returns Zero if no error, non-zero on error.

	\sa \ref Thread, or get_thread_priority(ThreadID)

***************************************/

Burger::eError BURGER_API Burger::set_thread_priority(
	ThreadID uThreadID, eThreadPriority uThreadPriority) BURGER_NOEXCEPT
{
	eError uResult = kErrorThreadNotFound;
	sched_param Param;
	int iPolicy;

	// Get information about the thread
	if (!pthread_getschedparam((pthread_t)uThreadID, &iPolicy, &Param)) {

		uResult = kErrorNone;
		// Convert from enumeration to native policy values
		switch (uThreadPriority) {
		case kThreadPriorityLow:
		case kThreadPriorityNormal:
			iPolicy = SCHED_OTHER;
			break;

		case kThreadPriorityHigh:
			// Of course Apple has to be different
#if defined(BURGER_DARWIN)
			iPolicy = SCHED_RR;
#else
			iPolicy = SCHED_OTHER;
#endif
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

			int iPriority;
			int iMin = sched_get_priority_min(iPolicy);
			int iMax = sched_get_priority_max(iPolicy);

			// Convert from enumeration to native priority values
			switch (uThreadPriority) {
			case kThreadPriorityLow:
				iPriority = iMin;
				break;

			case kThreadPriorityNormal:
#if defined(BURGER_DARWIN)
				if ((iMin == 15) && (iMax == 47)) {
					// Apple says this is "Normal"
					iPriority = 37;
					break;
				}
#endif
				iPriority = iMin + ((iMax - iMin) / 2);

				break;

			case kThreadPriorityHigh:
#if defined(BURGER_DARWIN)
				if ((iMin == 15) && (iMax == 47)) {
					// Apple says this is "High"
					iPriority = 45;
					break;
				}
#endif
				iPriority = iMin + (((iMax - iMin) * 3) / 4);
				break;

			case kThreadPriorityRealTime:
			default:
				iPriority = iMax;
				break;
			}

			Param.sched_priority = iPriority;
			// Was it successful?
			if (pthread_setschedparam((pthread_t)uThreadID, iPolicy, &Param)) {
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

/***************************************

	\brief Get Thread Local Storage

	Scan a private linked list for thread storage records and if found, return
	the pointer to the ThreadLocalStorageRecord_t that is assigned to the
	currently running thread.

	\returns The ThreadLocalStorageRecord_t pointer or \ref nullptr

	\sa tls_data_set(ThreadLocalStorage_t*)

***************************************/

Burger::ThreadLocalStorage_t* BURGER_API Burger::tls_data_get(
	void) BURGER_NOEXCEPT
{
	if (gStorage == INT32_MAX) {
		return tls_data_get_fallback();
	}
	BURGER_MEMORYBARRIER();
	return static_cast<ThreadLocalStorage_t*>(pthread_getspecific(gStorage));
}

/***************************************

	\brief Set a Thread Local Storage entry

	Scan a private linked list for thread storage records and if found, set
	the pointer to the ThreadLocalStorageRecord_t for the currently running
	thread. If no record was found, allocate a new record and add the data to
	this new record.

	\param pInput Pointer to a ThreadLocalStorage_t or \ref nullptr to delete
		the record if found

	\returns \ref kErrorNone or \ref kErrorOutOfMemory

	\sa tls_data_get(void)

***************************************/

Burger::eError BURGER_API Burger::tls_data_set(
	ThreadLocalStorage_t* pInput) BURGER_NOEXCEPT
{
	if (gStorage == INT32_MAX) {
		return tls_data_set_fallback(pInput);
	}
	if (pthread_setspecific(gStorage, pInput)) {
		return kErrorOutOfEntries;
	}
	return kErrorNone;
}

/***************************************

	This code fragment calls the Run function that has
	permission to access the members

***************************************/

static void* Dispatcher(void* pThis) BURGER_NOEXCEPT
{
	Burger::Thread::run(pThis);
	return nullptr;
}

/***************************************

	\brief Wait for a thread to exit

	If a thread isn't already running, return immediately. If a thread is
	already running, sleep until the thread has completed execution.

	\return Zero if no error, non-zero if there was an error

	\sa detach() or start()

***************************************/

Burger::eError BURGER_API Burger::Thread::wait(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorThreadNotStarted;

	// Was there a thread running?
	if (m_uThreadID) {
		// Wait until the thread completes execution
		pthread_join((pthread_t)m_uThreadID, nullptr);

		// Allow restarting
		m_uThreadID = 0;
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	\brief Internal function to start a thread

	This function executes platform specific code to start a thread.

	\note This function should not be called by applications.

	\sa platform_after_start()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_start(void) BURGER_NOEXCEPT
{
	// Data to initialize a thread with
	pthread_attr_t Attributes;

	eError uResult = kErrorThreadNotStarted;

	// If this fails, we are truly boned
	int iResult = pthread_attr_init(&Attributes);
	if (!iResult) {

		// The thread needs to be joinable
		iResult =
			pthread_attr_setdetachstate(&Attributes, PTHREAD_CREATE_JOINABLE);
		if (!iResult) {

			// Set the stack size needed
			if (m_uStackSize) {
				pthread_attr_setstacksize(&Attributes, m_uStackSize);
			} else {
				size_t uTempStack;
				if (!pthread_attr_getstacksize(&Attributes, &uTempStack)) {
					m_uStackSize = uTempStack;
				}
			}

			// Create and start the thread
			iResult = pthread_create(reinterpret_cast<pthread_t*>(&m_uThreadID),
				&Attributes, Dispatcher, this);

			// Thread created?
			if (!iResult) {
				uResult = kErrorNone;
			}
		}

		// Release the resources, if any
		pthread_attr_destroy(&Attributes);
	}
	return uResult;
}

/***************************************

	\brief Internal function to set up data specific to a thread

	This function executes platform specific code to execute before actually
	dispatching to the thread. This function is called in the same process as
	the thread, not the parent process.

	\note This function should not be called by applications.

	\sa platform_start()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_after_start(
	void) BURGER_NOEXCEPT
{
	// Make sure this is set before anything else
	m_uThreadID = (ThreadID)pthread_self();

	// Name the thread
	const char* pName = m_pName;
	if (!pName) {
		pName = "Burger Thread";
	}
#if defined(BURGER_DARWIN)
	Unix::pthread_setname_np(pName);
#else
	Unix::pthread_setname_np(m_uThreadID, pName);
#endif

	// Set the signal masks
	sigset_t SignalMask;
	sigemptyset(&SignalMask);
	uintptr_t uCount = BURGER_ARRAYSIZE(g_SignalList);
	const int* pWork = g_SignalList;
	do {
		sigaddset(&SignalMask, *pWork);
		++pWork;
	} while (--uCount);
	pthread_sigmask(SIG_BLOCK, &SignalMask, nullptr);

// Allow cancellation if supported
// Check first if supported
#if defined(PTHREAD_CANCEL_ASYNCHRONOUS)
	int iTemp;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &iTemp);
#endif

	return kErrorNone;
}

/***************************************

	\brief Internal function to detach a thread

	This function executes platform specific code detach a thread.

	\note This function should not be called by applications.

	\sa detach()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_detach(void) BURGER_NOEXCEPT
{
	if (!m_uThreadID) {
		return kErrorThreadNotStarted;
	}
	// Release control of the thread
	pthread_detach((pthread_t)m_uThreadID);

	// Dispose of the internal reference
	m_uThreadID = 0;

	// Detached, and possibly still running
	m_uState = kStateDetached;
	return kErrorNone;
}

#endif
