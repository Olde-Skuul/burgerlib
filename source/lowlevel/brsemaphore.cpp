/***************************************

	Class for semaphores

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

/*! ************************************

	\class Burger::Semaphore
	\brief Class for creating a semaphore

	In a multiprocessor system, it's necessary to have multiple
	resources available to multiple threads at the same time. To prevent
	resource starvation, this class is decremented until the number of
	available resources is exhausted and then it will halt future threads
	from executing until resources are made available via calls to signal().

	Further reading http://en.wikipedia.org/wiki/Semaphore_(programming)

	\note On operating systems that don't have native semaphore support, such as
		MSDOS, this class will always return error codes for all calls.

	\sa \ref Mutex and \ref Thread

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX) || \
	defined(BURGER_XBOX360) || defined(BURGER_XBOXONE) || \
	defined(BURGER_PS3) || defined(BURGER_PS4) || defined(BURGER_PS5) || \
	defined(BURGER_VITA) || defined(BURGER_WIIU) || defined(BURGER_SWITCH) || \
	defined(BURGER_UNIX) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)

/*! ************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a empty
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT: m_uCount(uCount)
{
}

/*! ************************************

	\brief Shut down a semaphore

	Release any operating system resources allocated in the creation of the
	semaphore.

	\note Care should be exercised in ensuring that all threads are are waiting
	on semaphores have been shutdown down already.

	\sa Semaphore(uint32_t)

***************************************/

Burger::Semaphore::~Semaphore() {}

/*! ************************************

	\brief Signal a semaphore

	When a resource is made available, signal that one is available with this
	call. This function will increment the count, and may release a waiting
	thread.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa wait_for_signal(uint32_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::signal(void) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	uint32_t /* uMilliseconds */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

	\fn uint32_t Burger::Semaphore::get_value(void) const
	\brief Get the current number of available resources.

	\return The number of available resources.
	\sa Semaphore(uint32_t)

***************************************/

#endif
