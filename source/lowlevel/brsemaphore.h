/***************************************

	Class for semaphores

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSEMAPHORE_H__
#define __BRSEMAPHORE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRMUTEX_H__
#include "brmutex.h"
#endif

/* BEGIN */
namespace Burger {

class Semaphore {
	BURGER_DISABLE_COPY(Semaphore);

protected:
#if defined(BURGER_XBOXONE) || defined(DOXYGEN)

#elif defined(BURGER_WINDOWS)
	/** Semaphore handle */
	void* m_pSemaphore;
	/** TRUE if using Win 8 WakeAddress API */
	uint32_t m_bUsingWakeAddress;

#elif defined(BURGER_XBOX360)
	/** Platform specific semaphore data */
	void* m_pSemaphore;

#elif defined(BURGER_PS3)
	uint32_t m_uSemaphore;

#elif defined(BURGER_PS4) || defined(BURGER_PS5)
	uint32_t m_Semaphore[4];

#elif defined(BURGER_VITA)
	int m_iSemaphore;

#elif defined(BURGER_WIIU)
	Mutex m_Lock;
	uint32_t m_Semaphore[8];
	uint32_t m_Cond[7];

#elif defined(BURGER_SWITCH) && defined(BURGER_64BITCPU)
	void* m_Semaphore[40 / sizeof(void*)];

#elif defined(BURGER_SWITCH)
	void* m_Semaphore[28 / sizeof(void*)];

#elif defined(BURGER_DARWIN)
	/** Darwin semaphore_t */
	uint32_t m_uSemaphore;
	/** Darwin threadid */
	uint32_t m_uOwner;

#elif defined(BURGER_ANDROID) && defined(__LP64__)
	/** sem_t */
	uint32_t m_Semaphore[4];

#elif defined(BURGER_ANDROID)
	/** sem_t */
	uint32_t m_Semaphore[1];

#elif defined(BURGER_UNIX) && defined(__x86_64__) && !defined(__ILP32__)
	/** Semaphore instance */
	uint64_t m_Semaphore[32 / sizeof(uint64_t)];

#elif defined(BURGER_UNIX)
	/** Semaphore instance */
	uint64_t m_Semaphore[16 / sizeof(uint64_t)];
#endif

	/** Semaphore count value */
	volatile uint32_t m_uCount;

public:
	Semaphore(uint32_t uCount = 0) BURGER_NOEXCEPT;
	~Semaphore();

	eError BURGER_API signal(void) BURGER_NOEXCEPT;
	eError BURGER_API wait_for_signal(
		uint32_t uMilliseconds = 0) BURGER_NOEXCEPT;

	BURGER_INLINE uint32_t get_value(void) const BURGER_NOEXCEPT
	{
		return m_uCount;
	}
};
}
/* END */

#endif
