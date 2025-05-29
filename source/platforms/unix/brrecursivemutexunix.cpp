/***************************************

	Class to handle recursive mutex objects, Unix version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrecursivemutex.h"

#if defined(BURGER_UNIX)
#include <errno.h>
#include <pthread.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

Burger::RecursiveMutex::RecursiveMutex() BURGER_NOEXCEPT
{
	// Verify the the Burgerlib opaque version is the same size as the real one
	BURGER_STATIC_ASSERT(sizeof(pthread_mutex_t) == sizeof(m_PlatformMutex));

	// Set the flavor to recursive for Unix
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(
		reinterpret_cast<pthread_mutex_t*>(m_PlatformMutex), &attr);
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::RecursiveMutex::~RecursiveMutex()
{
	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(m_PlatformMutex));
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void BURGER_API Burger::RecursiveMutex::lock(void) BURGER_NOEXCEPT
{
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(m_PlatformMutex));
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t BURGER_API Burger::RecursiveMutex::try_lock(void) BURGER_NOEXCEPT
{
	return pthread_mutex_trylock(
			   reinterpret_cast<pthread_mutex_t*>(m_PlatformMutex)) != EBUSY;
}

/***************************************

	\brief Unlocks the mutex

	Releases a lock on a mutex and if any other threads are waiting on this
	lock, they will obtain the lock and the other thread will continue
	execution. The caller will never block.

	\note This call MUST be preceded by a matching lock() call. Calling unlock()
	without a preceding lock() call will result in undefined behavior and in
	some cases can result in thread lock or a crash.

	\sa lock(), or try_lock()

***************************************/

void BURGER_API Burger::RecursiveMutex::unlock(void) BURGER_NOEXCEPT
{
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(m_PlatformMutex));
}

#endif
