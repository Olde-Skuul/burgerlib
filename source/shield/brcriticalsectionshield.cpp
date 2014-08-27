/***************************************

	Class to handle critical sections, Android version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_ANDROID)
#include "brstringfunctions.h"
#include "brassert.h"
#include "bratomic.h"
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks
	
***************************************/

Burger::CriticalSection::CriticalSection()
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the real thing
	BURGER_COMPILE_TIME_ASSERT(sizeof(Burger::pthread_mutex_t)==sizeof(::pthread_mutex_t));

	pthread_mutex_init(reinterpret_cast< ::pthread_mutex_t *>(&m_Lock),NULL);
}

Burger::CriticalSection::~CriticalSection()
{
	pthread_mutex_destroy(reinterpret_cast< ::pthread_mutex_t *>(&m_Lock));
}

/***************************************

	Lock the Mutex
	
***************************************/

void Burger::CriticalSection::Lock()
{
	pthread_mutex_lock(reinterpret_cast< ::pthread_mutex_t *>(&m_Lock));
}

/***************************************

	Try to lock the Mutex
	
***************************************/

Word Burger::CriticalSection::TryLock()
{
	return pthread_mutex_trylock(reinterpret_cast< ::pthread_mutex_t *>(&m_Lock))!=EBUSY;
}


/***************************************

	Unlock the Mutex
	
***************************************/

void Burger::CriticalSection::Unlock()
{
	pthread_mutex_unlock(reinterpret_cast< ::pthread_mutex_t *>(&m_Lock));
}


/***************************************

	Initialize the semaphore
	
***************************************/

Burger::Semaphore::Semaphore(Word32 uCount) :
	m_uCount(uCount),
	m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the real thing
	BURGER_COMPILE_TIME_ASSERT(sizeof(Burger::sem_t)==sizeof(::sem_t));

	// Initialize the semaphore
	if (!sem_init(reinterpret_cast< ::sem_t *>(&m_Semaphore),0,uCount)) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	Release the semaphore
	
***************************************/

Burger::Semaphore::~Semaphore()
{
	if (m_bInitialized) {
		sem_destroy(reinterpret_cast< ::sem_t *>(&m_Semaphore));
		m_bInitialized = FALSE;
	}
	m_uCount = 0;
}

/***************************************

	Attempt to acquire the semaphore
	
***************************************/

Word BURGER_API Burger::Semaphore::TryAcquire(Word uMilliseconds)
{
	// Assume failure
	Word uResult = 10;
	if (m_bInitialized) {
		// No wait?
		if (!uMilliseconds) {
			// Use the fast function
			if (!sem_trywait(reinterpret_cast< ::sem_t *>(&m_Semaphore))) {
				// Got it!
				uResult = 0;
			}
		} else if (uMilliseconds==BURGER_MAXUINT) {

			// Use the special function for halt until acquired
			int iSemResult;
			do {
				iSemResult = sem_wait(reinterpret_cast< ::sem_t *>(&m_Semaphore));
				// Got it?
				if (!iSemResult) {
					// Exit now
					uResult = 0;
					break;
				}
				// If the error was because of a system interrupt, try again
			} while ((iSemResult == -1) && (errno == EINTR));
		} else {
			// Posix doesn't use relative time. It uses
			// literal time.

			timeval CurrentTime;
			// Get the current time
			gettimeofday(&CurrentTime,NULL);

			// Add the timeout to the current time to get the
			// timeout time

			// Split between seconds and MICROseconds
			Word uSeconds = uMilliseconds/1000U;
			uMilliseconds = (uMilliseconds - (uSeconds * 1000U)) * 1000U;

			uMilliseconds += CurrentTime.tv_usec;
			uSeconds += CurrentTime.tv_sec;
			// Handle wrap around
			if (uMilliseconds>=1000000U) {
				uMilliseconds-=1000000U;
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
				iTest = sem_timedwait(reinterpret_cast< ::sem_t *>(&m_Semaphore),&TimeSpecTimeOut);
				// Loop only on interrupts
			} while ((iTest == -1) && (errno == EINTR));
			// Success?
			if (!iTest) {
				uResult = 0;
			}
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

Word BURGER_API Burger::Semaphore::Release(void)
{
	pthread_cond_t foo;
	Word uResult = 10;
	if (m_bInitialized) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (sem_post(reinterpret_cast< ::sem_t *>(&m_Semaphore))) {
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

Burger::ConditionVariable::ConditionVariable() :
	m_bInitialized(FALSE)
{
	// Safety switch to verify the declaration in brshieldtypes.h matches the real thing
	BURGER_COMPILE_TIME_ASSERT(sizeof(Burger::pthread_cond_t)==sizeof(::pthread_cond_t));

	if (!pthread_cond_init(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable),NULL)) {
		m_bInitialized = TRUE;
	}
}

/***************************************

	Release the resources

***************************************/

Burger::ConditionVariable::~ConditionVariable()
{
	if (m_bInitialized) {
		pthread_cond_destroy(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable));
		m_bInitialized = FALSE;
	}
}

/***************************************

	Signal a waiting thread

***************************************/

Word BURGER_API Burger::ConditionVariable::Signal(void)
{
	Word uResult = 10;
	if (m_bInitialized) {
		if (!pthread_cond_signal(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable))) {
			uResult = 0;
		}
	}
	return uResult;
}

/***************************************

	Signal all waiting threads

***************************************/

Word BURGER_API Burger::ConditionVariable::Broadcast(void)
{
	Word uResult = 10;
	if (m_bInitialized) {
		if (!pthread_cond_broadcast(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable))) {
			uResult = 0;
		}
	}
	return uResult;
}

/***************************************

	Wait for a signal (With timeout)

***************************************/

Word BURGER_API Burger::ConditionVariable::Wait(CriticalSection *pCriticalSection,Word uMilliseconds)
{
	Word uResult = 10;
	if (m_bInitialized) {
		if (uMilliseconds==BURGER_MAXUINT) {
			if (!pthread_cond_wait(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable),reinterpret_cast< ::pthread_mutex_t *>(&pCriticalSection->m_Lock))) {
				uResult = 0;
			}
		} else {

			// Use a timeout

			// Get the current time
			timeval CurrentTime;
			gettimeofday(&CurrentTime,NULL);

			// Determine the time in the future to timeout at
			struct timespec StopTimeHere;
			Word uSeconds = uMilliseconds/1000;
			// Get the remainder in NANOSECONDS
			uMilliseconds = (uMilliseconds-(uSeconds*1000))*1000000;

			// Add to the current time
			uMilliseconds += CurrentTime.tv_usec;
			uSeconds += CurrentTime.tv_sec;
			// Handle wrap around
			if (uMilliseconds>=1000000000) {
				uMilliseconds-=1000000000;
				++uSeconds;
			}
			StopTimeHere.tv_sec = uSeconds;
			StopTimeHere.tv_nsec = uMilliseconds;
			int iResult;
			do {
				// Send the signal and possibly time out
				iResult = pthread_cond_timedwait(reinterpret_cast< ::pthread_cond_t *>(&m_ConditionVariable),reinterpret_cast< ::pthread_mutex_t *>(&pCriticalSection->m_Lock), &StopTimeHere);
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

#endif