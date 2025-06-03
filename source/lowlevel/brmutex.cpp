/***************************************

	Class to handle mutex objects

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmutex.h"

/*! ************************************

	\class Burger::Mutex
	\brief Class for creating a Mutex for a critical section of data

	This synchronization primitive is used to protect shared data from being
	simultaneously accessed by multiple threads.

	This class enforces these rules:
	- A calling thread owns the Mutex after a successfull call to lock() or
		try_lock() until a call to unlock() is performed.
	- When a thread owns a Mutex, all other threads will block or return \ref
		FALSE for try_lock()
	- Multiple calls by the owning thread to lock() are not allowed.

	This is a functional equivalent to [std::mutex](
	https://en.cppreference.com/w/cpp/thread/mutex/)

	\sa \ref MutexStatic and \ref MutexLock

***************************************/

/*! ************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX) || \
	defined(BURGER_XBOX360) || defined(BURGER_XBOXONE) || \
	defined(BURGER_PS3) || defined(BURGER_PS4) || defined(BURGER_PS5) || \
	defined(BURGER_VITA) || defined(BURGER_WIIU) || defined(BURGER_SWITCH) || \
	defined(BURGER_UNIX) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)
Burger::Mutex::Mutex() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::Mutex::~Mutex() {}

/*! ************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void BURGER_API Burger::Mutex::lock() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t BURGER_API Burger::Mutex::try_lock(void) BURGER_NOEXCEPT
{
	// Always fail
	return FALSE;
}

/*! ************************************

	\brief Unlocks the mutex

	Releases a lock on a mutex and if any other threads are waiting on this
	lock, they will obtain the lock and the other thread will continue
	execution. The caller will never block.

	\note This call MUST be preceded by a matching lock() call. Calling unlock()
	without a preceding lock() call will result in undefined behavior and in
	some cases can result in thread lock or a crash.

	\sa lock(), or try_lock()

***************************************/

void BURGER_API Burger::Mutex::unlock() BURGER_NOEXCEPT {}

/*! ************************************

	\fn Burger::Mutex::get_platform_mutex()
	\brief Access the platform mutex data

	On platforms that support mutexes, a structure is needed to contain the
	platform specific mutex. This function returns a pointer to that structure.
	Use this call with caution because the data returned is platform specific.

	\return Pointer to the internal pthread_mutex_t, CRITICAL_SECTION, etc
		structure

***************************************/

/*! ************************************

	\fn Burger::Mutex::get_threadID()
	\brief Access the owner thread ID for the SRWLock

	On Windows and Xbox ONE, if the Mutex is managed by an SRWLock, a
	thread_ID_t of the owner thread is maintained. If it's zero, there is no
	owner.

	\note Available on Xbox ONE and Windows only

	\return thread_ID_t of the owner of this Mutex

	\sa get_count(), or set_state(uint32_t, uint32_t)

***************************************/

/*! ************************************

	\fn Burger::Mutex::get_count()
	\brief Get the lock count for a SRWLock

	On Windows and Xbox ONE, if the Mutex is managed by an SRWLock, a count is
	maintained for the number of times a lock was obtained for recursion. Return
	that count.

	\note Available on Xbox ONE and Windows only

	\return Levels of recursion for this Mutex, usually zero.

	\sa get_threadID(), or set_state(uint32_t, uint32_t)

***************************************/

/*! ************************************

	\fn Burger::Mutex::set_state(uint32_t, uint32_t)
	\brief Set the SRWLock state

	On Windows and Xbox ONE, CondionalVariable may need to override the values
	of the SRWLock. This call will allow that.

	\note Available on Xbox ONE and Windows only

	\param uCount New recursion level
	\param uThreadID New owner ID, can be 0.

	\sa get_threadID(), or get_count()

***************************************/

/*! ************************************

	\fn Burger::Mutex::is_using_SRWlock()
	\brief Is the Mutex using Window 7 APIs

	On Windows 7 or higher, Mutex is implemented by using the lightweight
	SRWLock API. On Windows XP and Vista, Mutex will use a CRITICAL_SECTION.
	Test which API is being used.

	\windowsonly

	\return \ref TRUE if using Windows 7 APIs, \ref FALSE if not.

***************************************/

#endif

/*! ************************************

	\class Burger::MutexStatic
	\brief Class for creating a Mutex for a statically allocated critical
		section of data

	When an object is created in static memory, the order of startup and
	shutdown is indeterminate in a cross platform way. To avoid an accidental
	call to this class before it's constructed or after it's destructed, this
	class will test a flag to determine if it's out of sequence and if so, the
	lock() and unlock() functions will perform no action. Since C++'s startup
	and shutdown sequence is performed on a single thread, this is a safe method
	to ensure that unknown startup/shutdown sequences won't accidentally thread
	lock.

	\sa \ref Mutex

***************************************/

/*! ************************************

	\brief Set the flag that a critical section is initialized

***************************************/

Burger::MutexStatic::MutexStatic() BURGER_NOEXCEPT: m_bValid(TRUE) {}

/*! ************************************

	\brief Clears the flag that a critical section is initialized

***************************************/

Burger::MutexStatic::~MutexStatic()
{
	m_bValid = FALSE;
}

/*! ************************************

	\brief Locks a mutex if initialized

	\sa Mutex::lock() and \ref unlock()

***************************************/

void Burger::MutexStatic::lock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		Mutex::lock();
	}
}

/*! ************************************

	\brief Tries to lock a mutex if initialized

	\sa Mutex::try_lock() and \ref lock()

***************************************/

uint_t BURGER_API Burger::MutexStatic::try_lock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		return Mutex::try_lock();
	}
	return FALSE;
}

/*! ************************************

	\brief Unlocks a mutex if initialized

	\sa Mutex::unlock() and \ref lock()

***************************************/

void BURGER_API Burger::MutexStatic::unlock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		Mutex::unlock();
	}
}

/*! ************************************

	\class Burger::MutexLock
	\brief Class for locking and releasing a mutex in a function

	To ease the obtaining and releasing a lock on a Burger::Mutex, this
	class is passed a pointer to a mutex and obtains a lock immediately. When
	the function that has this class locally stored exits, the lock is released.

	\note This class is not to be used as a global variable or a member of
		another class. It will not release a lock until the class is destructed

	\code
	// Statically created mutex
	static Burger::MutexStatic g_Lock;
	void foo(void)
	{
		// Obtain a lock on mutex
		Burger::MutexLock(&g_Lock);
		printf("Do stuff");
		// Lock is released on function exit
	}
	\endcode

	\sa \ref MutexStatic and \ref Mutex

***************************************/

/*! ************************************

	\fn Burger::MutexLock::MutexLock(Mutex* pMutex)
	\brief Obtain a lock on a mutex

	Locks the critical section upon construction. Will release it when the class
	is destroyed.

	\note \ref nullptr pointers are NOT allowed!!

	\param pMutex Pointer to a valid Mutex

	\sa \ref Mutex, \ref MutexLock and ~MutexLock()

***************************************/

/*! ************************************

	\fn Burger::MutexLock::~MutexLock()
	\brief Release a locked mutex

	Unlocks the critical section upon destruction.

	\sa \ref MutexLock and MutexLock(Mutex*)

***************************************/
