/***************************************

    Class to handle critical sections, MacOSX version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_MACOSX)
#include "brassert.h"
#include "bratomic.h"
#include "brstringfunctions.h"
#include <errno.h>
#include <mach/mach_init.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

/***************************************

	Initialize the CriticalSection

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	// Verify the the Burgerlib opaque version is the same size as the real one
    BURGER_STATIC_ASSERT(
		sizeof(Burgerpthread_mutex_t) == sizeof(pthread_mutex_t));

	pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(&m_Lock), NULL);
}

Burger::CriticalSection::~CriticalSection()
{
	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(&m_Lock));
}

/***************************************

	Lock the CriticalSection

***************************************/

void Burger::CriticalSection::Lock() BURGER_NOEXCEPT
{
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(&m_Lock));
}

/***************************************

	Try to lock the CriticalSection

***************************************/

uint_t Burger::CriticalSection::TryLock() BURGER_NOEXCEPT
{
	return pthread_mutex_trylock(reinterpret_cast<pthread_mutex_t*>(&m_Lock)) !=
		EBUSY;
}

/***************************************

	Unlock the CriticalSection

***************************************/

void Burger::CriticalSection::Unlock() BURGER_NOEXCEPT
{
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(&m_Lock));
}

/***************************************

	Initialize the semaphore

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) :
	m_uCount(uCount), m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the
	// real thing
    BURGER_STATIC_ASSERT(
		sizeof(Burgersemaphore_t) == sizeof(semaphore_t));

	// Initialize the semaphore
	task_t tOwner = mach_task_self();
	m_Owner = tOwner;
	if (semaphore_create(tOwner, &m_Semaphore, SYNC_POLICY_FIFO, uCount) ==
		KERN_SUCCESS) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	Release the semaphore

***************************************/

Burger::Semaphore::~Semaphore()
{
	if (m_bInitialized) {
		semaphore_destroy(m_Owner, m_Semaphore);
		m_bInitialized = FALSE;
	}
	m_uCount = 0;
}

/***************************************

	Try to acquire the semaphore, with lots of Apple invoked drama

***************************************/

uint_t BURGER_API Burger::Semaphore::TryAcquire(uint_t uMilliseconds)
{
	// Assume failure
	uint_t uResult = 10;
	if (m_bInitialized) {
		// Infinite wait?
		if (uMilliseconds == BURGER_MAXUINT) {

			// Use the special function for halt until acquired
			int iSemResult;
			do {
				iSemResult = semaphore_wait(m_Semaphore);
				// Got it?
				if (iSemResult == KERN_SUCCESS) {
					// Exit now
					uResult = 0;
					break;
				}
				// If the error was because of a system interrupt, try again
			} while (iSemResult == KERN_ABORTED);

		} else {

			// Data for the timer thread
			mach_timespec_t TimeoutData;
			uint_t uSeconds = uMilliseconds / 1000;
			uint_t uNanoSeconds = (uMilliseconds - (uSeconds * 1000)) * 1000;
			TimeoutData.tv_sec = uSeconds;
			TimeoutData.tv_nsec = uNanoSeconds;

			kern_return_t kError;
			do {

				kError = semaphore_timedwait(m_Semaphore, TimeoutData);
				if (kError == KERN_SUCCESS) {
					uResult = 0;
					break;
				}
				if (kError == KERN_OPERATION_TIMED_OUT) {
					uResult = 1;
					break;
				}
			} while (kError == KERN_ABORTED);
		}
		// If the lock was acquired, decrement the count
		if (!uResult) {
			AtomicPreDecrement(&m_uCount);
		}
	}
	return uResult;
}

/***************************************

	Release the semaphore

***************************************/

uint_t BURGER_API Burger::Semaphore::Release(void)
{
	uint_t uResult = 10;
	if (m_bInitialized) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (semaphore_signal(m_Semaphore) != KERN_SUCCESS) {
			// Error!!! Undo the AtomicPreIncrement()
			AtomicPreDecrement(&m_uCount);
		} else {
			// A-Okay!
			uResult = 0;
		}
	}
	return uResult;
}

/***************************************

	Initialize the condition variable

***************************************/

Burger::ConditionVariable::ConditionVariable() : m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the
	// real thing
    BURGER_STATIC_ASSERT(
		sizeof(Burgerpthread_cond_t) == sizeof(pthread_cond_t));

	if (!pthread_cond_init(
			reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable), NULL)) {
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

uint_t BURGER_API Burger::ConditionVariable::Signal(void)
{
	uint_t uResult = 10;
	if (m_bInitialized) {
		if (!pthread_cond_signal(
				reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable))) {
			uResult = 0;
		}
	}
	return uResult;
}

/***************************************

	Signal all waiting threads

***************************************/

uint_t BURGER_API Burger::ConditionVariable::Broadcast(void)
{
	uint_t uResult = 10;
	if (m_bInitialized) {
		if (!pthread_cond_broadcast(
				reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable))) {
			uResult = 0;
		}
	}
	return uResult;
}

/***************************************

	Wait for a signal (With timeout)

***************************************/

uint_t BURGER_API Burger::ConditionVariable::Wait(
	CriticalSection* pCriticalSection, uint_t uMilliseconds)
{
	uint_t uResult = 10;
	if (m_bInitialized) {
		if (uMilliseconds == BURGER_MAXUINT) {
			if (!pthread_cond_wait(
					reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable),
					reinterpret_cast< ::pthread_mutex_t*>(
						&pCriticalSection->m_Lock))) {
				uResult = 0;
			}
		} else {

			// Use a timeout

			// Get the current time
			timeval CurrentTime;
			gettimeofday(&CurrentTime, NULL);

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
					reinterpret_cast<pthread_cond_t*>(&m_ConditionVariable),
					reinterpret_cast< ::pthread_mutex_t*>(
						&pCriticalSection->m_Lock),
					&StopTimeHere);
				// Interrupted?
			} while (iResult == EINTR);

			// W00t! We're good!
			if (!iResult) {
				uResult = 0;

				// Time out?
			} else if (iResult == ETIMEDOUT) {
				uResult = 1;
			}
			// Otherwise, leave uResult as an error
		}
	}
	return uResult;
}

/***************************************

	This code fragment calls the Run function that has permission to access the
	members

***************************************/

static void* Dispatcher(void* pThis)
{
	Burger::Thread::Run(pThis);
	return NULL;
}

/***************************************

	Initialize a thread to a dormant state

***************************************/

Burger::Thread::Thread() :
	m_pFunction(NULL), m_pData(NULL), m_pSemaphore(NULL), m_pThreadHandle(NULL),
	m_uResult(BURGER_MAXUINT)
{
    BURGER_STATIC_ASSERT(sizeof(pthread_t) == sizeof(m_pThreadHandle));
}

/***************************************

	Initialize a thread and begin execution

***************************************/

Burger::Thread::Thread(FunctionPtr pThread, void* pData) :
	m_pFunction(NULL), m_pData(NULL), m_pSemaphore(NULL), m_pThreadHandle(NULL),
	m_uResult(BURGER_MAXUINT)
{
	Start(pThread, pData);
}

/***************************************

	Release resources

***************************************/

Burger::Thread::~Thread()
{
	Kill();
}

/***************************************

	Launch a new thread if one isn't already started

***************************************/

uint_t BURGER_API Burger::Thread::Start(FunctionPtr pFunction, void* pData)
{
	uint_t uResult = 10;
	if (!m_pThreadHandle) {
		m_pFunction = pFunction;
		m_pData = pData;
		// Use this temporary semaphore to force synchronization
		Semaphore Temp(0);
		m_pSemaphore = &Temp;
		pthread_attr_t Attributes;
		if (!pthread_attr_init(&Attributes)) {
			pthread_attr_setdetachstate(&Attributes, PTHREAD_CREATE_JOINABLE);
			if (!pthread_create(
					&m_pThreadHandle, &Attributes, Dispatcher, this)) {
				// Wait until the thread has started
				Temp.Acquire();
				// Kill the dangling pointer
				m_pSemaphore = NULL;
				// All good!
				uResult = 0;
			}
		}
	}
	return uResult;
}

/***************************************

	Wait until the thread has completed execution

***************************************/

uint_t BURGER_API Burger::Thread::Wait(void)
{
	uint_t uResult = 10;
	if (m_pThreadHandle) {
		// Wait until the thread completes execution
		pthread_join(m_pThreadHandle, 0);
		// Allow restarting
		m_pThreadHandle = NULL;
		uResult = 0;
	}
	return uResult;
}

/***************************************

	Invoke the nuclear option to kill a thread
	NOT RECOMMENDED!

***************************************/

uint_t BURGER_API Burger::Thread::Kill(void)
{
	uint_t uResult = 0;
	if (m_pThreadHandle) {
		pthread_kill(m_pThreadHandle, SIGKILL);
		uResult = 0;
	}
	return uResult;
}

/***************************************

	Synchronize and then execute the thread and save the result if any

***************************************/

void BURGER_API Burger::Thread::Run(void* pThis)
{
	Thread* pThread = static_cast<Thread*>(pThis);
	pThread->m_pSemaphore->Release();
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

#endif
