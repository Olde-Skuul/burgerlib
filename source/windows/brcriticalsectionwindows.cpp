/***************************************

	Class to handle critical sections

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_WINDOWS)
#include "brassert.h"
#include "bratomic.h"
// InitializeCriticalSectionAndSpinCount() is minimum XP
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501				// Windows XP
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks
	
***************************************/

Burger::CriticalSection::CriticalSection()
{
	// Safety switch to verify the declaration in brwindowstypes.h matches the real thing
	BURGER_COMPILE_TIME_ASSERT(sizeof(::CRITICAL_SECTION)==sizeof(Burger::CRITICAL_SECTION));

	InitializeCriticalSectionAndSpinCount(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock),1000);
}

Burger::CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}

/***************************************

	Lock the CriticalSection
	
***************************************/

void Burger::CriticalSection::Lock()
{
	EnterCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}

/***************************************

	Try to lock the CriticalSection
	
***************************************/

Word Burger::CriticalSection::TryLock()
{
	return static_cast<Word>(TryEnterCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock)));
}

/***************************************

	Unlock the CriticalSection
	
***************************************/

void Burger::CriticalSection::Unlock()
{
	LeaveCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}


/***************************************

	Initialize the semaphore
	
***************************************/

Burger::Semaphore::Semaphore(Word32 uCount) :
	m_uCount(uCount)
{
	// Get the maximum semaphores
	Word32 uMax = uCount+32768U;
	// Did it wrap (Overflow?)
	if (uMax<uCount) {
		// Use max
		uMax = BURGER_MAXUINT;
	}
	m_pSemaphore = CreateSemaphoreW(NULL,static_cast<LONG>(uCount),static_cast<LONG>(uMax),NULL);
}

/***************************************

	Release the semaphore
	
***************************************/

Burger::Semaphore::~Semaphore()
{
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		CloseHandle(hSemaphore);
		m_pSemaphore = NULL;
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
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		DWORD dwMilliseconds;
#if INFINITE==BURGER_MAXUINT
		dwMilliseconds = static_cast<DWORD>(uMilliseconds);
#else
		if (uMilliseconds==BURGER_MAXUINT) {
			dwMilliseconds = INFINITE;
		} else {
			dwMilliseconds = static_cast<DWORD>(uMilliseconds);
		}
#endif
		// Acquire a lock
		DWORD uWait = WaitForSingleObject(hSemaphore,dwMilliseconds);
		if (uWait==WAIT_OBJECT_0) {
			// Got the lock. Decrement the count
			AtomicPreDecrement(&m_uCount);
			uResult = 0;
		} else if (uWait==WAIT_TIMEOUT) {
			// Timeout gets a special error
			uResult = 1;
		}
	}
	return uResult;
}

/***************************************

	Release the semaphore
	
***************************************/

Word BURGER_API Burger::Semaphore::Release(void)
{
	Word uResult = 10;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (ReleaseSemaphore(hSemaphore,1,NULL)==FALSE) {
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

	This code fragment calls the Run function that has
	permission to access the members
	
***************************************/

static DWORD WINAPI Dispatcher(LPVOID pThis)
{
	Burger::Thread::Run(pThis);
	return 0;
}

/***************************************

	Initialize a thread to a dormant state
	
***************************************/

Burger::Thread::Thread() :
	m_pFunction(NULL),
	m_pData(NULL),
	m_pSemaphore(NULL),
	m_pThreadHandle(NULL),
	m_uThreadID(0),
	m_uResult(BURGER_MAXUINT)
{
}

/***************************************

	Initialize a thread and begin execution
	
***************************************/

Burger::Thread::Thread(FunctionPtr pThread,void *pData) :
	m_pFunction(NULL),
	m_pData(NULL),
	m_pSemaphore(NULL),
	m_pThreadHandle(NULL),
	m_uThreadID(0),
	m_uResult(BURGER_MAXUINT)
{
	Start(pThread,pData);
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

Word BURGER_API Burger::Thread::Start(FunctionPtr pFunction,void *pData)
{
	Word uResult = 10;
	if (!m_pThreadHandle) {
		m_pFunction = pFunction;
		m_pData = pData;
		// Use this temporary semaphore to force synchronization
		Semaphore Temp(0);
		m_pSemaphore = &Temp;
		DWORD uID;
		// Start the thread
		HANDLE hThread = CreateThread(NULL,0,Dispatcher,this,0,&uID);
		if (hThread) {
			// Ensure these are set up
			m_uThreadID = uID;
			m_pThreadHandle = hThread;
			// Wait until the thread has started
			Temp.Acquire();
			// Kill the dangling pointer
			m_pSemaphore = NULL;

			// All good!
			uResult = 0;
		}
	}
	return uResult;
}


/***************************************

	Wait until the thread has completed execution
	
***************************************/

Word BURGER_API Burger::Thread::Wait(void)
{
	Word uResult = 10;
	if (m_pThreadHandle) {
		// Wait until the thread completes execution
		DWORD uError = WaitForSingleObject(m_pThreadHandle,INFINITE);
		// Close it down!
		CloseHandle(m_pThreadHandle);
		// Allow restarting
		m_uThreadID = 0;
		m_pThreadHandle = NULL;
		// Shutdown fine?
		if (uError==WAIT_OBJECT_0) {
			uResult = 0;
		} else if (uError==WAIT_TIMEOUT) {
			uResult = 1;		// Timeout!
		}
	}
	return uResult;
}

/***************************************

	Invoke the nuclear option to kill a thread
	NOT RECOMMENDED!
	
***************************************/

Word BURGER_API Burger::Thread::Kill(void)
{
	Word uResult = 0;
	if (m_pThreadHandle) {
		if (!TerminateThread(m_pThreadHandle,BURGER_MAXUINT)) {
			uResult = 10;		// Error??
		} else {
			// Release everything
			uResult = Wait();
		}
	}
	return uResult;
}

/***************************************

	Synchronize and then execute the thread and save
	the result if any
	
***************************************/

void BURGER_API Burger::Thread::Run(void *pThis)
{
	Thread *pThread = static_cast<Thread *>(pThis);
	pThread->m_uThreadID = GetCurrentThreadId();
	pThread->m_pSemaphore->Release();
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

#endif

