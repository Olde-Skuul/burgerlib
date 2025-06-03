/***************************************

	Class to handle recursive mutex objects

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrecursivemutex.h"

/*! ************************************

	\class Burger::RecursiveMutex
	\brief Class for creating a RecursiveMutex for a critical section of data

	This synchronization primitive is used to protect shared data from being
	simultaneously accessed by multiple threads.

	This class enforces these rules:
	- A calling thread owns the RecursiveMutex after a successfull call to
		lock() or try_lock() until a call to unlock() is performed.
	- When a thread owns a RecursiveMutex, all other threads other than the
		owning thread will block or return \ref FALSE for try_lock()
	- Multiple calls by the owning thread to lock() are allowed. They must be
		matched by an equal number of calls to unlock().

	This is a functional equivalent to [std::recursive_mutex](
	https://en.cppreference.com/w/cpp/thread/recursive_mutex.html)

	\sa \ref RecursiveMutexStatic and \ref RecursiveMutexLock

***************************************/

/*! ************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX) || \
	defined(BURGER_XBOX360) || defined(BURGER_XBOXONE) || \
	defined(BURGER_PS3) || defined(BURGER_PS4) || \
	defined(BURGER_PS5) || /* defined(BURGER_VITA) || */ \
	defined(BURGER_WIIU) || defined(BURGER_SWITCH) || \
	defined(BURGER_UNIX) /* || defined(BURGER_MAC) */) || \
	defined(DOXYGEN)
Burger::RecursiveMutex::RecursiveMutex() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::RecursiveMutex::~RecursiveMutex() {}

/*! ************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa try_lock(), or unlock()

***************************************/

void BURGER_API Burger::RecursiveMutex::lock() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t BURGER_API Burger::RecursiveMutex::try_lock(void) BURGER_NOEXCEPT
{
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

void BURGER_API Burger::RecursiveMutex::unlock() BURGER_NOEXCEPT {}

/*! ************************************

	\fn Burger::RecursiveMutex::get_platform_mutex()
	\brief Access the platform mutex data

	On platforms that support mutexes, a structure is needed to contain the
	platform specific mutex. This function returns a pointer to that structure.
	Use this call with caution because the data returned is platform specific.

	\return Pointer to the internal pthread_mutex_t, CRITICAL_SECTION, etc
		structure

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutex::get_threadID()
	\brief Access the owner thread ID for the SRWLock

	On Windows and Xbox ONE, if the Mutex is managed by an SRWLock, a
	thread_ID_t of the owner thread is maintained. If it's zero, there is no
	owner.

	\note Available on Xbox ONE and Windows only

	\return thread_ID_t of the owner of this RecursiveMutex

	\sa get_count(), or set_state(uint32_t, uint32_t)

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutex::get_count()
	\brief Get the lock count for a SRWLock

	On Windows and Xbox ONE, if the RecursiveMutex is managed by an SRWLock, a
	count is maintained for the number of times a lock was obtained for
	recursion. Return that count.

	\note Available on Xbox ONE and Windows only

	\return Levels of recursion for this RecursiveMutex, usually zero.

	\sa get_threadID(), or set_state(uint32_t, uint32_t)

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutex::set_state(uint32_t, uint32_t)
	\brief Set the SRWLock state

	On Windows and Xbox ONE, CondionalVariable may need to override the values
	of the SRWLock. This call will allow that.

	\note Available on Xbox ONE and Windows only

	\param uCount New recursion level
	\param uThreadID New owner ID, can be 0.

	\sa get_threadID(), or get_count()

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutex::is_using_SRWlock()
	\brief Is the RecursiveMutex using Window 7 APIs

	On Windows 7 or higher, RecursiveMutex is implemented by using the
	lightweight SRWLock API. On Windows XP and Vista, RecursiveMutex will use a
	CRITICAL_SECTION. Test which API is being used.

	\windowsonly

	\return \ref TRUE if using Windows 7 APIs, \ref FALSE if not.

***************************************/

#endif

/*! ************************************

	\class Burger::RecursiveMutexStatic
	\brief Class for creating a RecursiveMutex for a statically allocated
		critical section of data

	When an object is created in static memory, the order of startup and
	shutdown is indeterminate in a cross platform way. To avoid an accidental
	call to this class before it's constructed or after it's destructed, this
	class will test a flag to determine if it's out of sequence and if so, the
	lock() and unlock() functions will perform no action. Since C++'s startup
	and shutdown sequence is performed on a single thread, this is a safe method
	to ensure that unknown startup/shutdown sequences won't accidentally thread
	lock.

	\sa \ref RecursiveMutex

***************************************/

/*! ************************************

	\brief Set the flag that a critical section is initialized

***************************************/

Burger::RecursiveMutexStatic::RecursiveMutexStatic() BURGER_NOEXCEPT
	: m_bValid(TRUE)
{
}

/*! ************************************

	\brief Clears the flag that a critical section is initialized

***************************************/

Burger::RecursiveMutexStatic::~RecursiveMutexStatic()
{
	m_bValid = FALSE;
}

/*! ************************************

	\brief Locks a mutex if initialized

	\sa RecursiveMutexStatic::lock() and \ref unlock()

***************************************/

void BURGER_API Burger::RecursiveMutexStatic::lock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		RecursiveMutex::lock();
	}
}

/*! ************************************

	\brief Tries to lock a mutex if initialized

	\sa RecursiveMutexStatic::try_lock() and \ref lock()

***************************************/

uint_t BURGER_API Burger::RecursiveMutexStatic::try_lock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		return RecursiveMutex::try_lock();
	}
	return FALSE;
}

/*! ************************************

	\fn Burger::RecursiveMutexStatic::unlock(void)
	\brief Unlocks a mutex if initialized

	\sa RecursiveMutexStatic::unlock() and \ref lock()

***************************************/

void BURGER_API Burger::RecursiveMutexStatic::unlock(void) BURGER_NOEXCEPT
{
	if (m_bValid) {
		RecursiveMutex::unlock();
	}
}

/*! ************************************

	\class Burger::RecursiveMutexLock
	\brief Class for locking and releasing a mutex in a function

	To ease the obtaining and releasing a lock on a Burger::RecursiveMutex, this
	class is passed a pointer to a mutex and obtains a lock immediately. When
	the function that has this class locally stored exits, the lock is released.

	\note This class is not to be used as a global variable or a member of
		another class. It will not release a lock until the class is destructed

	\code
	// Statically created mutex
	static Burger::RecursiveMutexStatic g_Lock;
	void foo(void)
	{
		// Obtain a lock on mutex
		Burger::RecursiveMutexLock(&g_Lock);
		printf("Do stuff");
		// Lock is released on function exit
	}
	\endcode

	\sa \ref RecursiveMutexStatic and \ref RecursiveMutex

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutexLock::RecursiveMutexLock(RecursiveMutex* pMutex)
	\brief Obtain a lock on a mutex

	Locks the critical section upon construction. Will release it when the class
	is destroyed.

	\note \ref nullptr pointers are NOT allowed!!

	\param pMutex Pointer to a valid Mutex

	\sa \ref RecursiveMutex, \ref RecursiveMutexLock and ~RecursiveMutexLock()

***************************************/

/*! ************************************

	\fn Burger::RecursiveMutexLock::~RecursiveMutexLock()
	\brief Release a locked mutex

	Unlocks the critical section upon destruction.

	\sa \ref RecursiveMutexLock and RecursiveMutexLock(Mutex*)

***************************************/
