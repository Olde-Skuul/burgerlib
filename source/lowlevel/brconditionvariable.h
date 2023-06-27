/***************************************

	Class to handle conditional variables

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCONDITIONVARIABLE_H__
#define __BRCONDITIONVARIABLE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRMUTEX_H__
#include "brmutex.h"
#endif

#ifndef __BRSEMAPHORE_H__
#include "brsemaphore.h"
#endif

/* BEGIN */
namespace Burger {
class ConditionVariable {

	BURGER_DISABLE_COPY(ConditionVariable);

protected:
#if defined(BURGER_UNIX) || defined(BURGER_SWITCH)
	/** Condition variable instance */
#if defined(BURGER_SWITCH)
	uint32_t m_ConditionVariable[12 / sizeof(uint32_t)];
#elif defined(BURGER_DARWIN) && defined(BURGER_64BITCPU)
	uint64_t m_ConditionVariable[48 / sizeof(uint64_t)];
#elif defined(BURGER_DARWIN)
	uint32_t m_ConditionVariable[28 / sizeof(uint32_t)];
#elif defined(BURGER_ANDROID) && defined(__LP64__)
	uint32_t m_ConditionVariable[12];
#elif defined(BURGER_ANDROID)
	uint32_t m_ConditionVariable[1];
#else
	uint64_t m_ConditionVariable[48 / sizeof(uint64_t)];
#endif

	/** \ref TRUE if the Condition variable instance successfully initialized */
	uint_t m_bInitialized;

#elif defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5)
	/** CONDITION_VARIABLE for Condition Variable */
	void* m_ConditionVariable[1];

#else

#if defined(BURGER_WINDOWS)
	/** CONDITION_VARIABLE for Condition Variable */
	void* m_ConditionVariable[1];
	/** True if using Windows Vista's APIs*/
	uint32_t m_bUsingNewAPI;
#endif

	/** Mutex for this class (Non-specialized platforms) */
	Mutex m_Mutex;
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
	eError BURGER_API signal(void) BURGER_NOEXCEPT;
	eError BURGER_API broadcast(void) BURGER_NOEXCEPT;
	eError BURGER_API wait(
		Mutex* pMutex, uint32_t uMilliseconds = UINT32_MAX) BURGER_NOEXCEPT;
};

}
/* END */

#endif
