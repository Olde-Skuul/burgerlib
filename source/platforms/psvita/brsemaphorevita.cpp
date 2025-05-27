/***************************************

	Class for semaphores, Playstation Vita version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_VITA)
#include <errno.h>
#include <kernel.h>
#include <sce_atomic.h>

/***************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a empty
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount): m_uCount(uCount)
{
	// Initialize the semaphore
	m_iSemaphore = sceKernelCreateSema(
		"BurgerSemaphore", 0, static_cast<SceInt32>(uCount), 32768, nullptr);
}

/***************************************

	\brief Shut down a semaphore

	Release any operating system resources allocated in the creation of the
	semaphore.

	\note Care should be exercised in ensuring that all threads are are waiting
	on semaphores have been shutdown down already.

	\sa Semaphore(uint32_t)

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

	\brief Signal a semaphore

	When a resource is made available, signal that one is available with this
	call. This function will increment the count, and may release a waiting
	thread.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa wait_for_signal(uint32_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::signal(void)
{
	eError uResult = kErrorCantUnlock;
	if (m_iSemaphore >= SCE_OK) {
		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to sceKernelSignalSema()
		// returns
		sceAtomicIncrement32(reinterpret_cast<volatile int32_t*>(&m_uCount));
		if (sceKernelSignalSema(m_iSemaphore, 1) < SCE_OK) {
			// Error!!! Undo the sceAtomicIncrement32()
			sceAtomicDecrement32(
				reinterpret_cast<volatile int32_t*>(&m_uCount));
		} else {
			// A-Okay!
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Wait for a resource with a timeout.

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	posts to the semaphore or the time in milliseconds has elapsed. If the
	timeout is zero, do not block.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0
		means no wait, UINT32_MAX means never time out

	\return Zero on success, \ref kErrorTimeout on a timeout, or other error
		code in case of a semaphore failure

	\sa signal(void)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::wait_for_signal(
	uint_t uMilliseconds)
{
	// Assume failure
	eError uResult = kErrorCantLock;
	if (m_iSemaphore >= SCE_OK) {

		// try_wait?
		if (!uMilliseconds) {

			// Use the fast function
			SceInt32 iResult = sceKernelPollSema(m_iSemaphore, 1);
			if (iResult == SCE_OK) {
				// Got it!
				sceAtomicDecrement32(
					reinterpret_cast<volatile int32_t*>(&m_uCount));
				uResult = kErrorNone;

			} else if (iResult == SCE_KERNEL_ERROR_SEMA_ZERO) {
				uResult = kErrorTimeout;
			}

		} else if (uMilliseconds == UINT32_MAX) {

			// Use the special function for halt until acquired
			SceInt32 iResult2 = sceKernelWaitSema(m_iSemaphore, 1, nullptr);
			if (iResult2 == SCE_OK) {
				// Got it!
				sceAtomicDecrement32(
					reinterpret_cast<volatile int32_t*>(&m_uCount));
				uResult = kErrorNone;
			}

		} else {

			// Wait for the semaphore
			SceUInt32 uTimeout = uMilliseconds * 1000;
			SceInt32 iResult3 = sceKernelWaitSema(m_iSemaphore, 1, &uTimeout);
			// Success?
			if (iResult3 == SCE_OK) {
				// Got it!
				sceAtomicDecrement32(
					reinterpret_cast<volatile int32_t*>(&m_uCount));
				uResult = kErrorNone;
			} else if (iResult3 == SCE_KERNEL_ERROR_WAIT_TIMEOUT) {
				uResult = kErrorTimeout;
			}
		}
	}
	return uResult;
}

#endif
