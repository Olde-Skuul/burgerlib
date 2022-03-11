/***************************************

	Class to handle critical sections

	Xbox 360 version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_XBOX360)
#include "bratomic.h"
#define NOD3D
#define NONET
#include <xtl.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(
		sizeof(CRITICAL_SECTION) == sizeof(Burger_CRITICAL_SECTION));
	InitializeCriticalSectionAndSpinCount(
		reinterpret_cast<CRITICAL_SECTION*>(&m_Lock), 1000);
}

Burger::CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_Lock));
}

/***************************************

	Lock the CriticalSection

***************************************/

void Burger::CriticalSection::Lock() BURGER_NOEXCEPT
{
	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_Lock));
}

/***************************************

	Try to lock the CriticalSection

***************************************/

uint_t Burger::CriticalSection::TryLock() BURGER_NOEXCEPT
{
	return TryEnterCriticalSection(
		reinterpret_cast<CRITICAL_SECTION*>(&m_Lock));
}

/***************************************

	Unlock the CriticalSection

***************************************/

void Burger::CriticalSection::Unlock() BURGER_NOEXCEPT
{
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_Lock));
}

/***************************************

	Initialize the semaphore

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT: m_uCount(uCount)
{
	// Get the maximum semaphores
	uint32_t uMax = uCount + 32768U;
	// Did it wrap (Overflow?)
	if (uMax < uCount) {
		// Use max
		uMax = BURGER_MAXUINT;
	}
	m_pSemaphore = CreateSemaphoreA(nullptr, uCount, uMax, nullptr);
}

/***************************************

	Release the semaphore

***************************************/

Burger::Semaphore::~Semaphore()
{
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		CloseHandle(hSemaphore);
		m_pSemaphore = nullptr;
	}
	m_uCount = 0;
}

/***************************************

	Attempt to acquire the semaphore

***************************************/

Burger::eError BURGER_API Burger::Semaphore::TryAcquire(
	uint_t uMilliseconds) BURGER_NOEXCEPT
{
	// Assume failure
	eError uResult = kErrorCantLock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		DWORD dwMilliseconds;
#if INFINITE == BURGER_MAXUINT
		dwMilliseconds = static_cast<DWORD>(uMilliseconds);
#else
		if (uMilliseconds == BURGER_MAXUINT) {
			dwMilliseconds = INFINITE;
		} else {
			dwMilliseconds = static_cast<DWORD>(uMilliseconds);
		}
#endif
		// Acquire a lock
		DWORD uWait = WaitForSingleObject(hSemaphore, dwMilliseconds);
		if (uWait == WAIT_OBJECT_0) {
			// Got the lock. Decrement the count
			AtomicPreDecrement(&m_uCount);
			uResult = kErrorNone;
		} else if (uWait == WAIT_TIMEOUT) {
			// Timeout gets a special error
			uResult = kErrorTimeout;
		}
	}
	return uResult;
}

/***************************************

	Release the semaphore

***************************************/

Burger::eError BURGER_API Burger::Semaphore::Release(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorCantUnlock;
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (ReleaseSemaphore(hSemaphore, 1, nullptr) == FALSE) {
			// Error!!! Undo the AtomicPreIncrement()
			AtomicPreDecrement(&m_uCount);
		} else {
			// A-Okay!
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	This code fragment calls the Run function that has
	permission to access the members

***************************************/

static DWORD WINAPI Dispatcher(LPVOID pThis) BURGER_NOEXCEPT
{
	Burger::Thread::Run(pThis);
	return 0;
}

/***************************************

	Initialize a thread to a dormant state

***************************************/

Burger::Thread::Thread() BURGER_NOEXCEPT: m_pFunction(nullptr),
										  m_pData(nullptr),
										  m_pSemaphore(nullptr),
										  m_pThreadHandle(nullptr),
										  m_uThreadID(0),
										  m_uResult(BURGER_MAXUINT)
{
}

/***************************************

	Initialize a thread and begin execution

***************************************/

Burger::Thread::Thread(FunctionPtr pThread, void* pData) BURGER_NOEXCEPT
	: m_pFunction(nullptr),
	  m_pData(nullptr),
	  m_pSemaphore(nullptr),
	  m_pThreadHandle(nullptr),
	  m_uThreadID(0),
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

Burger::eError BURGER_API Burger::Thread::Start(
	FunctionPtr pFunction, void* pData) BURGER_NOEXCEPT
{
	eError uResult = kErrorThreadNotStarted;
	if (!m_pThreadHandle) {
		m_pFunction = pFunction;
		m_pData = pData;
		// Use this temporary semaphore to force synchronization
		Semaphore Temp(0);
		m_pSemaphore = &Temp;
		DWORD uID;
		// Start the thread
		HANDLE hThread = CreateThread(nullptr, 0, Dispatcher, this, 0, &uID);
		if (hThread) {
			// Ensure these are set up
			m_uThreadID = uID;
			m_pThreadHandle = hThread;
			// Wait until the thread has started
			Temp.Acquire();
			// Kill the dangling pointer
			m_pSemaphore = nullptr;

			// All good!
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Wait until the thread has completed execution

***************************************/

Burger::eError BURGER_API Burger::Thread::Wait(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorThreadNotStarted;
	if (m_pThreadHandle) {
		// Wait until the thread completes execution
		DWORD uError = WaitForSingleObject(m_pThreadHandle, INFINITE);
		// Close it down!
		CloseHandle(m_pThreadHandle);
		// Allow restarting
		m_uThreadID = 0;
		m_pThreadHandle = nullptr;
		// Shutdown fine?
		if (uError == WAIT_OBJECT_0) {
			uResult = kErrorNone;
		} else if (uError == WAIT_TIMEOUT) {
			uResult = kErrorTimeout; // Timeout!
		}
	}
	return uResult;
}

/***************************************

	Invoke the nuclear option to kill a thread
	NOT RECOMMENDED!

***************************************/

Burger::eError BURGER_API Burger::Thread::Kill(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (m_pThreadHandle) {
		if (!CloseHandle(m_pThreadHandle)) {
			uResult = kErrorThreadCantStop; // Error??
		}
		m_pThreadHandle = nullptr;
		m_uThreadID = 0;
	}
	return uResult;
}

/***************************************

	Synchronize and then execute the thread and save
	the result if any

***************************************/

void BURGER_API Burger::Thread::Run(void* pThis) BURGER_NOEXCEPT
{
	Thread* pThread = static_cast<Thread*>(pThis);
	pThread->m_uThreadID = GetCurrentThreadId();
	pThread->m_pSemaphore->Release();
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

#endif
