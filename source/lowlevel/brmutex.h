/***************************************

	Class to handle mutex objects

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMUTEX_H__
#define __BRMUTEX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

/* BEGIN */
namespace Burger {
class Mutex {
	BURGER_DISABLE_COPY(Mutex);

protected:
#if defined(BURGER_XBOX) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	/** Platform specific CRITICAL_SECTION or mutex object */
	uint32_t m_PlatformMutex[7];
	uint32_t m_uOwnerThreadID;

#elif defined(BURGER_XBOXONE)
	/** SRWLOCK for Mutex */
	void* m_PlatformMutex[1];
	/** thread_ID_t of the owner of this mutex */
	uint32_t m_uOwnerThreadID;

#elif defined(BURGER_WINDOWS)
	/** CRITICAL_SECTION or SRWLOCK object */
	void* m_PlatformMutex[4U + ((sizeof(uint32_t) * 2U) / sizeof(void*))];
	/** thread_ID_t of the owner of this mutex */
	uint32_t m_uOwnerThreadID;
	/** Use SRWLOCK or CRITICAL_SECTION */
	uint_t m_bUseSRWLock;

#elif defined(BURGER_PS3)
	uint64_t m_PlatformMutex[3];

#elif defined(BURGER_PS4) || defined(BURGER_PS5)
	void* m_PlatformMutex[1];

#elif defined(BURGER_VITA)
	int m_PlatformMutex[1];

#elif defined(BURGER_WIIU)
	uint32_t m_PlatformMutex[11];
	/** Pointer to OSThread record */
	void* m_pOSThread;

#elif defined(BURGER_SWITCH) && defined(BURGER_64BITCPU)
	void* m_PlatformMutex[8 / sizeof(void*)];

#elif defined(BURGER_SWITCH)
	void* m_PlatformMutex[24 / sizeof(void*)];

#elif defined(BURGER_MAC)
	void* m_PlatformMutex[1];
#if defined(BURGER_68K)
	/** Number of locks on this Mutex */
	uint32_t m_uLockCount;
	/** Number of pending threads */
	uint32_t m_uQueueCount;
	uint32_t m_Queue[32];
#endif

#elif defined(BURGER_DARWIN) && defined(BURGER_64BITCPU)
	uint64_t m_PlatformMutex[8];

#elif defined(BURGER_DARWIN)
	uint32_t m_PlatformMutex[11];

#elif defined(BURGER_ANDROID) && defined(__LP64__)
	uint32_t m_PlatformMutex[10];

#elif defined(BURGER_ANDROID)
	uint32_t m_PlatformMutex[1];

#elif defined(BURGER_UNIX) && defined(__x86_64__) && !defined(__ILP32__)
	uint64_t m_PlatformMutex[5];

#elif defined(BURGER_UNIX) && defined(__x86_64__)
	uint64_t m_PlatformMutex[4];

#elif defined(BURGER_UNIX)
	uint64_t m_PlatformMutex[3];

#else
	uint8_t m_PlatformMutex[1];
#endif

public:
	Mutex() BURGER_NOEXCEPT;
	~Mutex();
	void BURGER_API lock(void) BURGER_NOEXCEPT;
	uint_t BURGER_API try_lock(void) BURGER_NOEXCEPT;
	void BURGER_API unlock(void) BURGER_NOEXCEPT;

	BURGER_INLINE void* get_platform_mutex(void) BURGER_NOEXCEPT
	{
		return m_PlatformMutex;
	}

#if defined(BURGER_XBOXONE) || defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE uint32_t get_threadID(void) const BURGER_NOEXCEPT
	{
		return m_uOwnerThreadID;
	}
	BURGER_INLINE void set_state(uint32_t uThreadID) BURGER_NOEXCEPT
	{
		m_uOwnerThreadID = uThreadID;
	}
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE uint_t is_using_SRWlock(void) const BURGER_NOEXCEPT
	{
		return m_bUseSRWLock;
	}
#endif
};

class MutexStatic: public Mutex {
	BURGER_DISABLE_COPY(MutexStatic);

protected:
	/** Set to \ref TRUE when constructed */
	uint_t m_bValid;

public:
	MutexStatic() BURGER_NOEXCEPT;
	~MutexStatic();
	void BURGER_API lock(void) BURGER_NOEXCEPT;
	uint_t BURGER_API try_lock(void) BURGER_NOEXCEPT;
	void BURGER_API unlock(void) BURGER_NOEXCEPT;
};

class MutexLock {
	BURGER_DISABLE_COPY(MutexLock);

	/** Pointer to the lock held */
	Mutex* m_pMutex;

public:
	MutexLock(Mutex* pMutex) BURGER_NOEXCEPT: m_pMutex(pMutex)
	{
		m_pMutex->lock();
	}

	~MutexLock()
	{
		m_pMutex->unlock();
	}
};

}
/* END */

#endif
