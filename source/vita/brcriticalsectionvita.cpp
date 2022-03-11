/***************************************

	Class to handle critical sections, Playstation Vita version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_VITA)
#include "brassert.h"
#include "bratomic.h"
#include "brstringfunctions.h"
#include <errno.h>
#include <kernel.h>

/***************************************

	Init the mutex

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	m_iLock = sceKernelCreateMutex("BurgerCriticalSection",
		SCE_KERNEL_MUTEX_ATTR_TH_PRIO | SCE_KERNEL_MUTEX_ATTR_RECURSIVE, 0,
		nullptr);
}

Burger::CriticalSection::~CriticalSection()
{
	SceInt32 retValue = sceKernelDeleteMutex(m_iLock);
	if (retValue == SCE_KERNEL_ERROR_WAIT_DELETE) {
		sceKernelUnlockMutex(m_iLock, 1);
		sceKernelDeleteMutex(m_iLock);
	}
}

/***************************************

	Lock the Mutex

***************************************/

void Burger::CriticalSection::Lock() BURGER_NOEXCEPT
{
	sceKernelLockMutex(m_iLock, 1, nullptr);
}

/***************************************

	Try to lock the Mutex

***************************************/

uint_t Burger::CriticalSection::TryLock() BURGER_NOEXCEPT
{
	return sceKernelTryLockMutex(m_iLock, 1) == SCE_OK;
}

/***************************************

	Unlock the Mutex

***************************************/

void Burger::CriticalSection::Unlock() BURGER_NOEXCEPT
{
	sceKernelUnlockMutex(m_iLock, 1);
}

/***************************************

	Initialize the semaphore

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount): m_uCount(uCount)
{
	// Initialize the semaphore
	if (!uCount) {
		uCount = 1;
	}
	m_iSemaphore = sceKernelCreateSema(
		"BurgerSemaphore", SCE_KERNEL_SEMA_ATTR_TH_FIFO, 0, uCount, nullptr);
}

/***************************************

	Release the semaphore

***************************************/

Burger::Semaphore::~Semaphore()
{
	if (m_iSemaphore >= SCE_OK) {
		sceKernelDeleteSema(m_iSemaphore);
		m_iSemaphore = -1;
	}
	m_uCount = 0;
}

/***************************************

	Attempt to acquire the semaphore

***************************************/

Burger::eError BURGER_API Burger::Semaphore::TryAcquire(uint_t uMilliseconds)
{
	// Assume failure
	eError uResult = kErrorCantLock;
	SceUInt32 uTimeout;
	if (m_iSemaphore >= SCE_OK) {
		// No wait?
		if (!uMilliseconds) {
			// Use the fast function
			if (sceKernelPollSema(m_iSemaphore, 1) == SCE_OK) {
				// Got it!
				uResult = kErrorNone;
			}
		} else if (uMilliseconds == BURGER_MAXUINT) {

			// Use the special function for halt until acquired
			if (sceKernelWaitSema(m_iSemaphore, 1, nullptr) == SCE_OK) {
				// Exit now
				uResult = kErrorNone;
			}
		} else {

			// Wait for the semaphore
			uTimeout = uMilliseconds * 1000;
			SceInt32 iTest = sceKernelWaitSema(m_iSemaphore, 1, &uTimeout);
			// Success?
			if (iTest == SCE_OK) {
				uResult = kErrorNone;
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

Burger::eError BURGER_API Burger::Semaphore::Release(void)
{
	eError uResult = kErrorCantUnlock;
	if (m_iSemaphore >= SCE_OK) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (sceKernelSignalSema(m_iSemaphore, 1) < SCE_OK) {
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

	Initialize the condition variable

***************************************/

Burger::ConditionVariable::ConditionVariable():
	m_iConditionVariable(-1), m_iMutex(-1)
{
	SceUID iMutex = sceKernelCreateMutex(
		"BurgerConditionMutex", SCE_KERNEL_ATTR_TH_PRIO, 0, nullptr);
	if (iMutex >= SCE_OK) {
		SceUID iCond = sceKernelCreateCond("BurgerConditionVariable",
			SCE_KERNEL_COND_ATTR_TH_FIFO, iMutex, nullptr);
		if (iCond < SCE_OK) {
			sceKernelDeleteMutex(iMutex);
		} else {
			m_iMutex = iMutex;
			m_iConditionVariable = iCond;
		}
	}
}

/***************************************

	Release the resources

***************************************/

Burger::ConditionVariable::~ConditionVariable()
{
	if (m_iConditionVariable >= SCE_OK) {
		sceKernelCancelMutex(m_iMutex, 0, nullptr);
		sceKernelDeleteCond(m_iConditionVariable);
		sceKernelDeleteMutex(m_iMutex);
		m_iConditionVariable = -1;
		m_iMutex = -1;
	}
}

/***************************************

	Signal a waiting thread

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Signal(void)
{
	eError uResult = kErrorNotInitialized;
	if (m_iConditionVariable >= SCE_OK) {
		if (sceKernelSignalCond(m_iConditionVariable) == SCE_OK) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Signal all waiting threads

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Broadcast(void)
{
	eError uResult = kErrorNotInitialized;
	if (m_iConditionVariable >= SCE_OK) {
		if (sceKernelSignalCondAll(m_iConditionVariable) == SCE_OK) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	Wait for a signal (With timeout)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Wait(
	CriticalSection* pCriticalSection, uint_t uMilliseconds)
{
	eError uResult = kErrorNotInitialized;
	if (m_iConditionVariable >= SCE_OK) {
		if (uMilliseconds == BURGER_MAXUINT) {
			if (sceKernelWaitCond(m_iConditionVariable, nullptr) == SCE_OK) {
				uResult = kErrorNone;
			}
		} else {

			// Use a timeout
			SceUInt32 uTimeout = uMilliseconds * 1000;
			SceInt32 iResult =
				sceKernelWaitCond(m_iConditionVariable, &uTimeout);
			if (iResult == SCE_OK) {
				uResult = kErrorNone;
			} else if (iResult == SCE_KERNEL_ERROR_WAIT_TIMEOUT) {
				uResult = kErrorTimeout;
			}
		}
	}
	return uResult;
}

/***************************************

	This code fragment calls the Run function that has
	permission to access the members

***************************************/

static SceInt32 Dispatcher(SceSize argSize, void* pArgBlock)
{
	Burger::Thread* pThis = static_cast<Burger::Thread**>(pArgBlock)[0];
	Burger::Thread::Run(pThis);
	return 0;
}

/***************************************

	Initialize a thread to a dormant state

***************************************/

Burger::Thread::Thread():
	m_pFunction(nullptr),
	m_pData(nullptr),
	m_pSemaphore(nullptr),
	m_iThreadID(-1),
	m_uResult(BURGER_MAXUINT)
{
}

/***************************************

	Initialize a thread and begin execution

***************************************/

Burger::Thread::Thread(FunctionPtr pThread, void* pData):
	m_pFunction(nullptr),
	m_pData(nullptr),
	m_pSemaphore(nullptr),
	m_iThreadID(-1),
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
	FunctionPtr pFunction, void* pData)
{
	eError uResult = kErrorThreadNotStarted;
	if (m_iThreadID == -1) {
		m_pFunction = pFunction;
		m_pData = pData;
		// Use this temporary semaphore to force synchronization
		Semaphore Temp(0);
		m_pSemaphore = &Temp;
		m_iThreadID = sceKernelCreateThread("BurgerThread", Dispatcher,
			SCE_KERNEL_DEFAULT_PRIORITY_USER + 1,
			SCE_KERNEL_THREAD_STACK_SIZE_MIN, 0, SCE_KERNEL_CPU_MASK_USER_ALL,
			SCE_NULL);

		if (m_iThreadID >= SCE_OK) {
			Thread* pData = this;
			SceInt32 iTest =
				sceKernelStartThread(m_iThreadID, sizeof(pData), &pData);
			if (iTest >= SCE_OK) {
				// Wait until the thread has started
				Temp.Acquire();
				// Kill the dangling pointer
				m_pSemaphore = nullptr;
				// All good!
				uResult = kErrorNone;
			}
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
	if (m_iThreadID != -1) {
		// Wait until the thread completes execution
		sceKernelWaitThreadEnd(m_iThreadID, nullptr, SCE_NULL);
		sceKernelDeleteThread(m_iThreadID);
		// Allow restarting
		m_iThreadID = -1;
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Invoke the nuclear option to kill a thread
	NOT RECOMMENDED!

***************************************/

Burger::eError BURGER_API Burger::Thread::Kill(void)
{
	eError uResult = kErrorThreadNotStarted;
	if (m_iThreadID != -1) {
		sceKernelDeleteThread(m_iThreadID);
		m_iThreadID = -1;
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	Synchronize and then execute the thread and save
	the result if any

***************************************/

void BURGER_API Burger::Thread::Run(void* pThis)
{
	Thread* pThread = static_cast<Thread*>(pThis);
	pThread->m_pSemaphore->Release();
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

#endif
