/***************************************

	Class to handle mutex objects, Playstation 3 version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmutex.h"

#if defined(BURGER_PS3)
#include "brmemoryfunctions.h"

#include <sys/synchronization.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

Burger::Mutex::Mutex() BURGER_NOEXCEPT
{
	// Create a recursive lock to mimic the PC CRITICAL_SECTION
	BURGER_STATIC_ASSERT(sizeof(sys_lwmutex_t) == sizeof(m_PlatformMutex));

	sys_lwmutex_attribute_t lwattr;
	sys_lwmutex_attribute_initialize(lwattr);
	sys_lwmutex_create(
		reinterpret_cast<sys_lwmutex_t*>(m_PlatformMutex), &lwattr);
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::Mutex::~Mutex()
{
	sys_lwmutex_destroy(reinterpret_cast<sys_lwmutex_t*>(m_PlatformMutex));
}

/***************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void Burger::Mutex::lock() BURGER_NOEXCEPT
{
	sys_lwmutex_lock(reinterpret_cast<sys_lwmutex_t*>(m_PlatformMutex), 0);
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t Burger::Mutex::try_lock() BURGER_NOEXCEPT
{
	return sys_lwmutex_trylock(
			   reinterpret_cast<sys_lwmutex_t*>(m_PlatformMutex)) == CELL_OK;
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

void Burger::Mutex::unlock() BURGER_NOEXCEPT
{
	sys_lwmutex_unlock(reinterpret_cast<sys_lwmutex_t*>(m_PlatformMutex));
}

#endif
