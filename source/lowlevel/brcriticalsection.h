/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCRITICALSECTION_H__
#define __BRCRITICALSECTION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

#if defined(BURGER_PS3) && !defined(__BRPS3TYPES_H__)
#include "brps3types.h"
#endif

#if defined(BURGER_PS4) && !defined(__BRPS4TYPES_H__)
#include "brps4types.h"
#endif

#if defined(BURGER_SHIELD) && !defined(__BRSHIELDTYPES_H__)
#include "brshieldtypes.h"
#endif

#if defined(BURGER_VITA) && !defined(__BRVITATYPES_H__)
#include "brvitatypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {
class CriticalSection {
	BURGER_DISABLE_COPY(CriticalSection);
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	/** Critical section for OS calls (Windows and Xbox 360 only) */
	Burger_CRITICAL_SECTION m_Lock;
#endif

#if defined(BURGER_PS3) || defined(DOXYGEN)
	/** Critical section for PS3 (PS3 only) */
	Burgersys_lwmutex_t m_Lock;
#endif

#if defined(BURGER_PS4) || defined(DOXYGEN)
	/** Critical section for PS4 (PS4 only) */
	pthread_mutex* m_Lock;
#endif

#if (defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	friend class ConditionVariable;

	/** Critical section for Android/MacOSX/iOS (Android/MacOSX/iOS only) */
	Burgerpthread_mutex_t m_Lock;
#endif

#if defined(BURGER_VITA) || defined(DOXYGEN)
	/** Critical section ID for VITA */
	int m_iLock;
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	/** Critical section ID for OpenTransport */
	uint8_t m_bLock;
#endif

public:
	CriticalSection() BURGER_NOEXCEPT;
	~CriticalSection();
	void Lock(void) BURGER_NOEXCEPT;
	uint_t TryLock(void) BURGER_NOEXCEPT;
	void Unlock(void) BURGER_NOEXCEPT;
};

class CriticalSectionStatic: public CriticalSection {
	/** Set to \ref TRUE when constructed */
	uint_t m_bValid;

public:
	CriticalSectionStatic() BURGER_NOEXCEPT;
	~CriticalSectionStatic();
	BURGER_INLINE void Lock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			CriticalSection::Lock();
		}
	}
	BURGER_INLINE uint_t TryLock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			return CriticalSection::TryLock();
		}
		return FALSE;
	}
	BURGER_INLINE void Unlock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			CriticalSection::Unlock();
		}
	}
};

class CriticalSectionLock {
	BURGER_DISABLE_COPY(CriticalSectionLock);

	/** Pointer to the lock held */
	CriticalSection* m_pCriticalSection;

public:
	CriticalSectionLock(CriticalSection* pCriticalSection) BURGER_NOEXCEPT
		: m_pCriticalSection(pCriticalSection)
	{
		m_pCriticalSection->Lock();
	}
	~CriticalSectionLock()
	{
		m_pCriticalSection->Unlock();
	}
};

class Semaphore {

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	/** Semaphore HANDLE (Windows only) */
	void* m_pSemaphore;
#endif

#if (defined(BURGER_SHIELD)) || defined(DOXYGEN)
	/** Semaphore instance (Android) */
	Burgersem_t m_Semaphore;
	/** \ref TRUE if the semaphore instance successfully initialized */
	uint_t m_bInitialized;
#endif

#if defined(BURGER_VITA) || defined(DOXYGEN)
	/** Semaphore ID for VITA */
	int m_iSemaphore;
#endif

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	/** Semaphore instance (MacOSX/iOS only) */
	Burgersemaphore_t m_Semaphore;
	/** Task ID of the semaphore owner (MacOSX/iOS only) */
	Burgertask_t m_Owner;
	/** \ref TRUE if the semaphore instance successfully initialized */
	uint_t m_bInitialized;
#endif

	/** Semaphore count value */
	volatile uint32_t m_uCount;

public:
	Semaphore(uint32_t uCount = 0);
	~Semaphore();
	BURGER_INLINE uint_t Acquire(void)
	{
		return TryAcquire(BURGER_MAXUINT);
	}
	uint_t BURGER_API TryAcquire(uint_t uMilliseconds = 0);
	uint_t BURGER_API Release(void);
	BURGER_INLINE uint32_t GetValue(void) const BURGER_NOEXCEPT
	{
		return m_uCount;
	}
};

class ConditionVariable {

#if (defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	/** Condition variable instance (Android/MacOSX/iOS only) */
	Burgerpthread_cond_t m_ConditionVariable;
	/** \ref TRUE if the Condition variable instance successfully initialized
	 * (Android/MacOSX/iOS only) */
	uint_t m_bInitialized;
#endif

#if (defined(BURGER_VITA)) || defined(DOXYGEN)
	/** Condition variable instance (Vita only) */
	int m_iConditionVariable;
	/** Mutex for the condition variable (Vita only) */
	int m_iMutex;
#endif

#if !(defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	/** CriticalSection for this class (Non-specialized platforms) */
	CriticalSection m_CriticalSection;
	/** Binary semaphore for forcing thread to wait for a signal
	 * (Non-specialized platforms) */
	Semaphore m_WaitSemaphore;
	/** Binary semaphore for the number of pending signals (Non-specialized
	 * platforms) */
	Semaphore m_SignalsSemaphore;
	/** Count of waiting threads (Non-specialized platforms) */
	uint32_t m_uWaiting;
	/** Count of signals to be processed (Non-specialized platforms) */
	uint32_t m_uSignals;
#endif

public:
	ConditionVariable();
	~ConditionVariable();
	uint_t BURGER_API Signal(void);
	uint_t BURGER_API Broadcast(void);
	uint_t BURGER_API Wait(CriticalSection* pCriticalSection,
		uint_t uMilliseconds = BURGER_MAXUINT);
};

class Thread {
public:
	/** Thread entry prototype */
	typedef uintptr_t(BURGER_API* FunctionPtr)(void* pThis);

private:
	/** Pointer to the thread */
	FunctionPtr m_pFunction;
	/** Data pointer for the thread */
	void* m_pData;
	/** Temp Semaphore for synchronization */
	Semaphore* m_pSemaphore;

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	/** HANDLE to the thread (Windows/Xbox 360 only) */
	void* m_pThreadHandle;
	/** System ID of the thread (Windows/Xbox 360 only) */
	uint32_t m_uThreadID;
#endif

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	/** Pointer to the thread data (MacOSX/iOS only) */
	_opaque_pthread_t* m_pThreadHandle;
#endif

#if (defined(BURGER_VITA)) || defined(DOXYGEN)
	/** System ID of the thread (Vita only) */
	int m_iThreadID;
#endif

	/** Result code of the thread on exit */
	uintptr_t m_uResult;

public:
	Thread();
	Thread(FunctionPtr pFunction, void* pData);
	~Thread();
	uint_t BURGER_API Start(FunctionPtr pFunction, void* pData);
	uint_t BURGER_API Wait(void) BURGER_NOEXCEPT;
	uint_t BURGER_API Kill(void);
	static void BURGER_API Run(void* pThis);
	BURGER_INLINE uintptr_t GetResult(void) const BURGER_NOEXCEPT
	{
		return m_uResult;
	}

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	BURGER_INLINE uint_t IsInitialized(void) const BURGER_NOEXCEPT
	{
		return m_pThreadHandle != nullptr;
	}

#elif defined(BURGER_VITA)
	BURGER_INLINE uint_t IsInitialized(void) const BURGER_NOEXCEPT
	{
		return m_iThreadID >= 0;
	}

#else
	BURGER_INLINE uint_t IsInitialized(void) const BURGER_NOEXCEPT
	{
		return FALSE;
	}
#endif
};
}
/* END */

#endif
