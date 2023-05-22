/***************************************

	Class to handle critical sections, Unix version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_UNIX)
#include <errno.h>
#include <pthread.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	// Verify the the Burgerlib opaque version is the same size as the real one
	BURGER_STATIC_ASSERT(sizeof(pthread_mutex_t) == sizeof(m_Lock));
	pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(m_Lock), nullptr);
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::CriticalSection::~CriticalSection()
{
	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(m_Lock));
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa Burger::CriticalSection::unlock()

***************************************/

void Burger::CriticalSection::lock(void) BURGER_NOEXCEPT
{
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(m_Lock));
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock() and unlock()

***************************************/

uint_t Burger::CriticalSection::try_lock(void) BURGER_NOEXCEPT
{
	return pthread_mutex_trylock(reinterpret_cast<pthread_mutex_t*>(m_Lock)) !=
		EBUSY;
}

/***************************************

	\brief Unlocks the mutex

	Releases a lock on a mutex and if any other threads are waiting on this
	lock, they will obtain the lock and the other thread will continue
	execution. The caller will never block.

	\note This call MUST be preceded by a matching lock() call. Calling unlock()
	without a preceding lock() call will result in undefined behavior and in
	some cases can result in thread lock or a crash.

	\sa lock()

***************************************/

void Burger::CriticalSection::unlock(void) BURGER_NOEXCEPT
{
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(m_Lock));
}

#endif
