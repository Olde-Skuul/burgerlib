/***************************************

	Class to handle critical sections, Playstation Vita version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_VITA)
#include "brstringfunctions.h"
#include "brassert.h"
#include "bratomic.h"
#include <kernel.h>
#include <errno.h>

/***************************************

	Init the mutex
	
***************************************/

Burger::CriticalSection::CriticalSection()
{
	m_iLock = sceKernelCreateMutex("BurgerCriticalSection",SCE_KERNEL_MUTEX_ATTR_TH_PRIO|SCE_KERNEL_MUTEX_ATTR_RECURSIVE,0,NULL);
}

Burger::CriticalSection::~CriticalSection()
{
	SceInt32 retValue = sceKernelDeleteMutex(m_iLock);
	if (retValue == SCE_KERNEL_ERROR_WAIT_DELETE) {
		sceKernelUnlockMutex(m_iLock,1);
		sceKernelDeleteMutex(m_iLock);
	}
}

/***************************************

	Lock the Mutex
	
***************************************/

void Burger::CriticalSection::Lock()
{
	sceKernelLockMutex(m_iLock,1,NULL);
}

/***************************************

	Try to lock the Mutex
	
***************************************/

Word Burger::CriticalSection::TryLock()
{
	return sceKernelTryLockMutex(m_iLock,1)==SCE_OK;
}


/***************************************

	Unlock the Mutex
	
***************************************/

void Burger::CriticalSection::Unlock()
{
	sceKernelUnlockMutex(m_iLock,1);
}


/***************************************

	Initialize the semaphore
	
***************************************/

Burger::Semaphore::Semaphore(Word32 uCount) :
	m_uCount(uCount)
{
	// Initialize the semaphore
	if (!uCount) {
		uCount = 1;
	}
	m_iSemaphore = sceKernelCreateSema("BurgerSemaphore",SCE_KERNEL_SEMA_ATTR_TH_FIFO,0,uCount,NULL);
}

/***************************************

	Release the semaphore
	
***************************************/

Burger::Semaphore::~Semaphore()
{
	if (m_iSemaphore>=SCE_OK) {
		sceKernelDeleteSema(m_iSemaphore);
		m_iSemaphore = -1;
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
	SceUInt32 uTimeout;
	if (m_iSemaphore>=SCE_OK) {
		// No wait?
		if (!uMilliseconds) {
			// Use the fast function
			if (sceKernelPollSema(m_iSemaphore,1)==SCE_OK) {
				// Got it!
				uResult = 0;
			}
		} else if (uMilliseconds==BURGER_MAXUINT) {

			// Use the special function for halt until acquired
			if (sceKernelWaitSema(m_iSemaphore,1,NULL)==SCE_OK) {
				// Exit now
				uResult = 0;
			}
		} else {

			// Wait for the semaphore
			uTimeout = uMilliseconds*1000;
			SceInt32 iTest = sceKernelWaitSema(m_iSemaphore,1,&uTimeout);
			// Success?
			if (iTest==SCE_OK) {
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
	Word uResult = 10;
	if (m_iSemaphore>=SCE_OK) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to ReleaseSemaphore()
		// returns
		AtomicPreIncrement(&m_uCount);
		if (sceKernelSignalSema(m_iSemaphore,1)<SCE_OK) {
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
	m_iConditionVariable(-1),
	m_iMutex(-1)
{
	SceUID iMutex = sceKernelCreateMutex("BurgerConditionMutex",SCE_KERNEL_ATTR_TH_PRIO,0,NULL);
	if (iMutex>=SCE_OK) {
		SceUID iCond = sceKernelCreateCond("BurgerConditionVariable",SCE_KERNEL_COND_ATTR_TH_FIFO,iMutex,NULL);
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
	if (m_iConditionVariable>=SCE_OK) {
		sceKernelCancelMutex(m_iMutex,0,NULL);
		sceKernelDeleteCond(m_iConditionVariable);
		sceKernelDeleteMutex(m_iMutex);
		m_iConditionVariable=-1;
		m_iMutex=-1;
	}
}

/***************************************

	Signal a waiting thread

***************************************/

Word BURGER_API Burger::ConditionVariable::Signal(void)
{
	Word uResult = 10;
	if (m_iConditionVariable>=SCE_OK) {
		if (sceKernelSignalCond(m_iConditionVariable)==SCE_OK) {
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
	if (m_iConditionVariable>=SCE_OK) {
		if (sceKernelSignalCondAll(m_iConditionVariable)==SCE_OK) {
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
	if (m_iConditionVariable>=SCE_OK) {
		if (uMilliseconds==BURGER_MAXUINT) {
			if (sceKernelWaitCond(m_iConditionVariable,NULL)==SCE_OK) {
				uResult = 0;
			}
		} else {

			// Use a timeout
			SceUInt32 uTimeout = uMilliseconds*1000;
			SceInt32 iResult = sceKernelWaitCond(m_iConditionVariable,&uTimeout);
			if (iResult==SCE_OK) {
				uResult = 0;
			} else if (iResult==SCE_KERNEL_ERROR_WAIT_TIMEOUT) {
				uResult = 1;
			}
		}
	}
	return uResult;
}

/***************************************
 
	This code fragment calls the Run function that has
	permission to access the members
 
***************************************/

static SceInt32 Dispatcher(SceSize argSize,void *pArgBlock)
{
	Burger::Thread *pThis = static_cast<Burger::Thread **>(pArgBlock)[0];
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
	m_iThreadID(-1),
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
	m_iThreadID(-1),
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
	if (m_iThreadID==-1) {
		m_pFunction = pFunction;
		m_pData = pData;
		// Use this temporary semaphore to force synchronization
		Semaphore Temp(0);
		m_pSemaphore = &Temp;
		m_iThreadID = sceKernelCreateThread("BurgerThread",Dispatcher,SCE_KERNEL_DEFAULT_PRIORITY_USER + 1,
			SCE_KERNEL_THREAD_STACK_SIZE_MIN,0,SCE_KERNEL_CPU_MASK_USER_ALL,SCE_NULL);

		if (m_iThreadID>=SCE_OK) {
			Thread *pData = this;
			SceInt32 iTest = sceKernelStartThread(m_iThreadID,sizeof(pData),&pData);
			if (iTest>=SCE_OK) {
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

Word BURGER_API Burger::Thread::Wait(void)
{
	Word uResult = 10;
	if (m_iThreadID!=-1) {
		// Wait until the thread completes execution
		sceKernelWaitThreadEnd(m_iThreadID,NULL,SCE_NULL);
		sceKernelDeleteThread(m_iThreadID);
		// Allow restarting
		m_iThreadID = -1;
		uResult = 0;
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
	if (m_iThreadID!=-1) {
		sceKernelDeleteThread(m_iThreadID);
		m_iThreadID = -1;
		uResult = 0;
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
	pThread->m_pSemaphore->Release();
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

#endif
