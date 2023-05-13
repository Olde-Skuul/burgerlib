/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#if defined(BURGER_ANDROID) && !defined(__BRANDROIDTYPES_H__)
#include "brandroidtypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

/* BEGIN */
namespace Burger {
class CriticalSection {
	friend class ConditionVariable;
	BURGER_DISABLE_COPY(CriticalSection);

protected:
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOXONE) || defined(DOXYGEN)
	/** Platform specific CRITICAL_SECTION or mutex object */
	void* m_Lock[4U + ((sizeof(uint32_t) * 2U) / sizeof(void*))];

#elif defined(BURGER_XBOX360)
	uint32_t m_Lock[7];

#elif defined(BURGER_PS3)
	uint32_t m_Lock[6];

#elif defined(BURGER_PS4) || defined(BURGER_PS5)
	void* m_Lock;

#elif defined(BURGER_VITA)
	int m_iLock;

#elif defined(BURGER_WIIU)
	uint32_t m_Lock[11];

#elif defined(BURGER_SWITCH)
#if defined(BURGER_64BITCPU)
	void* m_Lock[40 / sizeof(void*)];
#else
	void* m_Lock[24 / sizeof(void*)];
#endif

#elif defined(BURGER_DARWIN)
#if defined(BURGER_64BITCPU)
	uint64_t m_Lock[8];
#else
	uint32_t m_Lock[11];
#endif

#elif defined(BURGER_ANDROID)
#if defined(__LP64__)
	uint32_t m_Lock[10];
#else
	uint32_t m_Lock[1];
#endif

#elif defined(BURGER_UNIX)
#if defined(__x86_64__)
#if !defined(__ILP32__)
	uint64_t m_Lock[5];
#else
	uint64_t m_Lock[4];
#endif
#else
	uint64_t m_Lock[3];
#endif

#elif defined(BURGER_MAC)
	uint8_t m_Lock;
#endif

public:
	CriticalSection() BURGER_NOEXCEPT;
	~CriticalSection();
	void lock(void) BURGER_NOEXCEPT;
	uint_t try_lock(void) BURGER_NOEXCEPT;
	void unlock(void) BURGER_NOEXCEPT;
};

class CriticalSectionStatic: public CriticalSection {
	BURGER_DISABLE_COPY(CriticalSectionStatic);

protected:
	/** Set to \ref TRUE when constructed */
	uint_t m_bValid;

public:
	CriticalSectionStatic() BURGER_NOEXCEPT;
	~CriticalSectionStatic();

	BURGER_INLINE void lock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			CriticalSection::lock();
		}
	}

	BURGER_INLINE uint_t try_lock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			return CriticalSection::try_lock();
		}
		return FALSE;
	}

	BURGER_INLINE void unlock(void) BURGER_NOEXCEPT
	{
		if (m_bValid) {
			CriticalSection::unlock();
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
		pCriticalSection->lock();
	}

	~CriticalSectionLock()
	{
		m_pCriticalSection->unlock();
	}
};

class Semaphore {
	BURGER_DISABLE_COPY(Semaphore);

protected:
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5)) || \
	defined(DOXYGEN)
	/** Platform specific semaphore data */
	void* m_pSemaphore;

#elif defined(BURGER_PS3)
	uint32_t m_uSemaphore;

#elif defined(BURGER_VITA)
	int m_iSemaphore;

#elif defined(BURGER_WIIU)
	CriticalSection m_Lock;
	uint32_t m_Semaphore[8];
	uint32_t m_Cond[7];

#elif defined(BURGER_SWITCH)
#if defined(BURGER_64BITCPU)
	void* m_Semaphore[40 / sizeof(void*)];
#else
	void* m_Semaphore[28 / sizeof(void*)];
#endif

#elif defined(BURGER_DARWIN)
	uint32_t m_uSemaphore;
	uint32_t m_uOwner;
#endif

#if defined(BURGER_ANDROID) || defined(DOXYGEN)
	/** Semaphore instance (Android) */
	Burgersem_t m_Semaphore;
#endif

	/** Semaphore count value */
	volatile uint32_t m_uCount;

	/** \ref TRUE if the semaphore instance successfully initialized */
	uint_t m_bInitialized;

public:
	Semaphore(uint32_t uCount = 0) BURGER_NOEXCEPT;
	~Semaphore();

	BURGER_INLINE eError acquire(void) BURGER_NOEXCEPT
	{
		return try_acquire(UINT32_MAX);
	}
	eError BURGER_API try_acquire(uint32_t uMilliseconds = 0) BURGER_NOEXCEPT;
	eError BURGER_API release(void) BURGER_NOEXCEPT;

	BURGER_INLINE uint32_t get_value(void) const BURGER_NOEXCEPT
	{
		return m_uCount;
	}

	BURGER_INLINE uint_t is_initalized(void) const BURGER_NOEXCEPT
	{
		return m_bInitialized;
	}
};

class ConditionVariable {

	BURGER_DISABLE_COPY(ConditionVariable);

protected:
#if (defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS)) || \
	defined(DOXYGEN)
	/** Condition variable instance (Android/MacOSX/iOS only) */
	Burgerpthread_cond_t m_ConditionVariable;
	/** \ref TRUE if the Condition variable instance successfully initialized
	 * (Android/MacOSX/iOS only) */
	uint_t m_bInitialized;
#endif

#if defined(BURGER_VITA) || defined(DOXYGEN)
	/** Condition variable instance (Vita only) */
	int m_iConditionVariable;
	/** Mutex for the condition variable (Vita only) */
	int m_iMutex;
#endif

#if !(defined(BURGER_ANDROID) || defined(BURGER_MACOSX) || \
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
	ConditionVariable() BURGER_NOEXCEPT;
	~ConditionVariable();
	eError BURGER_API Signal(void) BURGER_NOEXCEPT;
	eError BURGER_API Broadcast(void) BURGER_NOEXCEPT;
	eError BURGER_API Wait(CriticalSection* pCriticalSection,
		uint_t uMilliseconds = BURGER_MAXUINT) BURGER_NOEXCEPT;
};

}
/* END */

#endif
