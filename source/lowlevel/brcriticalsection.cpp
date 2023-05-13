/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

/*! ************************************

	\class Burger::CriticalSection
	\brief Class for creating a Mutex for a critical section of data

	In a multiprocessor system, it's necessary to have multiple threads access
	the same data at the same time. To prevent data collisions, this class is
	locked just before the data is used/modified and unlocked after the data
	access/modification is complete so all CPUs/threads do not have to worry
	about data collisions.

	\sa Burger::CriticalSectionStatic and Burger::CriticalSectionLock

***************************************/

/*! ************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS3) || defined(BURGER_PS4) || \
	defined(BURGER_PS5) || defined(BURGER_VITA) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)
Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::CriticalSection::~CriticalSection() {}

/*! ************************************

	\brief Locks the mutex

	If the mutex is locked, a lock is obtained and execution continues. If the
	mutex was already locked, the thread halts until the alternate thread that
	has this mutex locked releases the lock. There is no timeout.

	\sa Burger::CriticalSection::unlock()

***************************************/

void Burger::CriticalSection::lock() BURGER_NOEXCEPT {}

/*! ************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock() and unlock()

***************************************/

uint_t Burger::CriticalSection::try_lock(void) BURGER_NOEXCEPT
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

	\sa lock()

***************************************/

void Burger::CriticalSection::unlock() BURGER_NOEXCEPT {}

#endif

/*! ************************************

	\class Burger::CriticalSectionStatic
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

	\sa \ref CriticalSection

***************************************/

/*! ************************************

	\brief Set the flag that a critical section is initialized

***************************************/

Burger::CriticalSectionStatic::CriticalSectionStatic() BURGER_NOEXCEPT
	: m_bValid(TRUE)
{
}

/*! ************************************

	\brief Clears the flag that a critical section is initialized

***************************************/

Burger::CriticalSectionStatic::~CriticalSectionStatic()
{
	m_bValid = FALSE;
}

/*! ************************************

	\fn Burger::CriticalSectionStatic::lock(void)
	\brief Locks a mutex if initialized

	\sa CriticalSection::lock() and
		CriticalSectionStatic::unlock()

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionStatic::try_lock(void)
	\brief Tries to lock a mutex if initialized

	\sa CriticalSection::try_lock() and
		CriticalSectionStatic::lock()

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionStatic::unlock(void)
	\brief Unlocks a mutex if initialized

	\sa CriticalSection::unlock() and
		CriticalSectionStatic::lock()

***************************************/

/*! ************************************

	\class Burger::CriticalSectionLock
	\brief Class for locking and releasing a mutex in a function

	To ease the obtaining and releasing a lock on a Burger::CriticalSection this
	class is passed a pointer to a mutex and obtains a lock immediately. When
	the function that has this class locally stored exits, the lock is released.

	\note This class is not to be used as a global variable or a member of
		another class. It will not release a lock until the class is destructed

	\code
	// Statically created mutex
	static Burger::CriticalSectionStatic g_Lock;
	void foo(void)
	{
		// Obtain a lock on mutex
		Burger::CriticalSectionLock(&g_Lock);
		printf("Do stuff");
		// Lock is released on function exit
	}
	\endcode

	\sa Burger::CriticalSectionStatic and Burger::CriticalSection

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionLock::CriticalSectionLock(
		CriticalSection *pCriticalSection)
	\brief Obtain a lock on a mutex

	Locks the critical section upon construction. Will release it when the class
	is destructed.

	\note \ref nullptr pointers are NOT allowed!!

	\param pCriticalSection Pointer to a valid CriticalSectionLock

	\sa \ref CriticalSectionLock and ~CriticalSectionLock()

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionLock::~CriticalSectionLock()
	\brief Release a locked mutex

	Unlocks the critical section upon destruction.

	\sa \ref CriticalSectionLock and
		CriticalSectionLock::CriticalSectionLock()

***************************************/

/*! ************************************

	\class Burger::Semaphore
	\brief Class for creating a semaphore

	In a multiprocessor system, it's necessary to have multiple
	resources available to multiple threads at the same time. To prevent
	resource starvation, this class is decremented until the number of
	available resources is exhausted and then it will halt future threads
	from executing until resources are made available.

	Further reading http://en.wikipedia.org/wiki/Semaphore_(programming)

	\note On operating systems that don't have native semaphore support, such as
		MSDOS, this class will always return error codes for all calls.

	\sa \ref CriticalSection and \ref Thread

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS3) || defined(BURGER_PS4) || \
	defined(BURGER_PS5) || defined(BURGER_VITA) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_DARWIN)) || \
	defined(DOXYGEN)

/*! ************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a binary
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT
	: m_uCount(uCount),
	  m_bInitialized(FALSE)
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

	\fn Burger::Semaphore::acquire(void)
	\brief Acquire a lock on a semaphore resource

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	releases the semaphore.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa try_acquire(uint_t) or release(void)

***************************************/

/*! ************************************

	\brief Acquire a lock on a semaphore resource with a timeout

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	releases the semaphore or the time in milliseconds has elapsed. If the
	timeout is zero, return immediately with a non-zero error code.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0
		means no wait, UINT32_MAX means infinite

	\return Zero on success, One on a timeout, and non Zero or One in the case
		of a semaphore failure

	\sa acquire(void) or release(void)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::try_acquire(
	uint32_t /* uMilliseconds */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

	\brief Release a lock on a semaphore resource

	After a thread has acquired a resource via a semaphore, release it with this
	call once the resource is no longer needed.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa acquire(void) or try_acquire(uint_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::release(void) BURGER_NOEXCEPT
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

/*! ************************************

	\class Burger::ConditionVariable
	\brief Class for creating a condition variable

	In a multiprocessor system, it's sometimes necessary to have a thread wait
	until an external condition has been satisfied. Once this class is created,
	the consumer thread will call Wait() with a locked CriticalSection and a
	timeout and it will halt until time has expired or another thread sends a
	signal to this class to alert the waiting thread that the stars are finally
	right and great Cthulhu can rise again.

	Further reading
	http://en.wikipedia.org/wiki/Condition_variable#Condition_variables

	\sa Burger::CriticalSection, Burger::Semaphore and Burger::Thread

***************************************/

/*! ************************************

	\brief Initialize a condition signaler

	\sa ~ConditionVariable()

***************************************/

#if !(defined(BURGER_SHIELD) || defined(BURGER_VITA) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || \
	defined(DOXYGEN)
Burger::ConditionVariable::ConditionVariable() BURGER_NOEXCEPT
	: m_CriticalSection(),
	  m_WaitSemaphore(0),
	  m_SignalsSemaphore(0),
	  m_uWaiting(0),
	  m_uSignals(0)
{
}

/*! ************************************

	\brief Release the condition signaler's resources

	\sa ConditionVariable()

***************************************/

Burger::ConditionVariable::~ConditionVariable()
{
	// Invoke the auto destructors
}

/*! ************************************

	\brief Signal a waiting thread

	If a thread is waiting for a signal, send a signal to the
	thread so it may continue execution. If no thread
	is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa Broadcast() or Wait(CriticalSection *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Signal(
	void) BURGER_NOEXCEPT
{
	m_CriticalSection.lock();
	// Is there anyone waiting for a signal?
	if (m_uWaiting > m_uSignals) {
		// Add to the signal count (Wait() will decrement)
		++m_uSignals;
		// Release a waiting thread
		m_WaitSemaphore.release();
		// Unlock the data to allow Wait() to continue
		m_CriticalSection.unlock();
		// Increase the signals semaphore to match
		m_SignalsSemaphore.acquire();
	} else {
		// No one was waiting, discard
		m_CriticalSection.unlock();
	}
	return kErrorNone;
}

/*! ************************************

	\brief Signal all waiting threads

	If any threads are waiting for a signal, send a signal to
	all of them so they all may continue execution. If no thread
	is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa Signal() or Wait(CriticalSection *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Broadcast(
	void) BURGER_NOEXCEPT
{
	// Lock internal data
	m_CriticalSection.lock();
	// Anyone waiting?
	if (m_uWaiting > m_uSignals) {
		// For all the ones that haven't already gotten a signal
		// let them execute
		uint32_t uCount = m_uWaiting - m_uSignals; // Always positive
		// Set the new count
		m_uSignals = m_uWaiting;
		// Reduce the number of waiting threads
		uint32_t uCount2 = uCount;
		do {
			m_WaitSemaphore.release();
		} while (--uCount2);
		// The act of unlocking will fire all the released threads
		m_CriticalSection.unlock();

		// Reacquire the thread count to restore the wait count to
		// match m_uWaiting (Note that other threads could be modifying this
		// value, however, Wait() is updating this value, so
		// from our point of view, it must be restored)

		do {
			m_WaitSemaphore.acquire();
		} while (--uCount);
	} else {
		// Get out, nothing to see here
		m_CriticalSection.unlock();
	}
	return kErrorNone;
}

/*! ************************************

	\brief Wait for a signal (With timeout)

	Halt the thread until a signal is received or if a timeout has elapsed. If
	any threads are waiting for a signal, send a signal to all of them so they
	all may continue execution. If no thread is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa Signal() or Wait(CriticalSection *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::Wait(
	CriticalSection* pCriticalSection, uint_t uMilliseconds) BURGER_NOEXCEPT
{
	// A thread is waiting
	m_CriticalSection.lock();
	++m_uWaiting;
	m_CriticalSection.unlock();

	// Unlock the thread's lock
	pCriticalSection->unlock();
	// Wait for a signal (With timeout)
	const eError uResult = m_WaitSemaphore.try_acquire(uMilliseconds);

	// It returned!
	m_CriticalSection.lock();
	// Was there a signal from Signal() or Broadcast()?
	if (m_uSignals) {
		// Was there a timeout?
		if (uResult == kErrorTimeout) {
			// Force an obtaining of a resource
			m_WaitSemaphore.acquire();
		}
		// Mark this signal as processed
		m_SignalsSemaphore.release();
		--m_uSignals;
	}
	// No longer waiting
	--m_uWaiting;
	m_CriticalSection.unlock();
	// Relock the thread's lock
	pCriticalSection->lock();
	return uResult;
}

#endif
