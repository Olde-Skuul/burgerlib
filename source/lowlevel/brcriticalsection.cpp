/***************************************

	Root base class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

/*! ************************************

	\class Burger::CriticalSection
	\brief Class for creating a Mutex for a critical section of data
	
	In a multiprocessor system, it's necessary to have multiple
	threads access the same data at the same time. To prevent
	data collisions, this class is locked just before the data is
	used/modified and unlocked after the data access/modification
	is complete so all CPUs/threads do not have to worry about
	data collisions.
	
	\sa Burger::CriticalSectionStatic and Burger::CriticalSectionLock

***************************************/

/*! ************************************

	\brief Initialize the data in the class.
	
	Sets up operating system defaults to the data
	
***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_SHIELD) || defined(BURGER_VITA) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::CriticalSection::CriticalSection()
{
}

/*! ************************************

	\brief Shutdown the data in the class.
	
	Releases the operating system resources allocated by the
	constructor.
	
***************************************/

Burger::CriticalSection::~CriticalSection()
{
}

/*! ************************************

	\brief Locks the mutex
	
	If the mutex is locked, a lock is obtained
	and execution continues. If the mutex was already
	locked, the thread halts until the alternate
	thread that has this mutex locked releases the
	lock. There is no timeout.

	\sa Burger::CriticalSection::Unlock()
		
***************************************/

void Burger::CriticalSection::Lock()
{
}

/*! ************************************

	\brief Attempt to lock the mutex
	
	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa Lock() and Unlock()
		
***************************************/

Word Burger::CriticalSection::TryLock(void)
{
	return FALSE;
}

/*! ************************************

	\brief Unlocks the mutex
	
	Releases a lock on a mutex and if any other threads are
	waiting on this lock, they will obtain the lock and the
	other thread will continue execution. The caller will never
	block.
	
	\note This call MUST be preceded by a matching Lock() call. Calling
	Unlock() without a preceding Lock() call will result in undefined
	behavior and in some cases can result in threadlock or a crash.

	\sa Burger::CriticalSection::Lock()
		
***************************************/

void Burger::CriticalSection::Unlock()
{
}

#endif

/*! ************************************

	\class Burger::CriticalSectionStatic
	\brief Class for creating a Mutex for a statically allocated critical section of data
	
	When an object is created in static memory, the order of startup and
	shutdown is undeterminate in a cross platform way. To avoid an
	accidental call to this class before it's constructed or after
	it's destructed, this class will test a flag to determine if
	it's out of sequence and if so, the Lock() and Unlock() functions
	will perform no action. Since C++'s startup and shutdown sequence
	is performed on a single thread, this is a safe method to ensure
	that unknown startup/shutdown sequences won't accidentally thread lock.

	\sa Burger::CriticalSection

***************************************/

/*! ************************************

	\brief Set the flag that a critical section is initialized

***************************************/

Burger::CriticalSectionStatic::CriticalSectionStatic() :
	m_bValid(TRUE)
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

	\fn Burger::CriticalSectionStatic::Lock(void)
	\brief Locks a mutex if initialized

	\sa Burger::CriticalSection::Lock() and Burger::CriticalSectionStatic::Unlock()

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionStatic::TryLock(void)
	\brief Tries to lock a mutex if initialized

	\sa Burger::CriticalSection::TryLock() and Burger::CriticalSectionStatic::Lock()
	
***************************************/

/*! ************************************

	\fn Burger::CriticalSectionStatic::Unlock(void)
	\brief Unlocks a mutex if initialized

	\sa Burger::CriticalSection::Unlock() and Burger::CriticalSectionStatic::Lock()
	
***************************************/

/*! ************************************

	\class Burger::CriticalSectionLock
	\brief Class for locking and releasing a mutex in a function
	
	To ease the obtaining and releasing a lock on a Burger::CriticalSection
	this class is passed a pointer to a mutex and obtains
	a lock immediately. When the function that has this class locally stored
	exits, the lock is released. 

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

	\fn Burger::CriticalSectionLock::CriticalSectionLock(CriticalSection *pCriticalSection)
	\brief Obtain a lock on a mutex
	
	Locks the critical section upon construction. Will release it when the class is destructed.

	\note \ref NULL pointers are NOT allowed!!

	\param pCriticalSection Pointer to a valid CriticalSectionLock
	\sa Burger::CriticalSectionLock and ~CriticalSectionLock()

***************************************/

/*! ************************************

	\fn Burger::CriticalSectionLock::~CriticalSectionLock()
	\brief Release a locked mutex
	
	Unlocks the critical section upon destruction.

	\sa Burger::CriticalSectionLock and CriticalSectionLock::CriticalSectionLock()

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
	
	\note On operating systems that don't have native semaphore support, such as MSDOS,
		this class will always return error codes for all calls.

	\sa Burger::CriticalSection and Burger::Thread

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_ANDROID) || defined(BURGER_VITA) || \
	defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)

/*! ************************************

	\brief Initialize a semaphore
	
	Query the operating system for a semaphore and initialize it to
	the initial value. 

	\param uCount Initial number of resources available (0 means a binary semaphore)
	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(Word32 uCount) :
	m_uCount(uCount)
{
}

/*! ************************************

	\brief Shut down a semaphore
	
	Release any operating system resources allocated in 
	the creation of the semaphore.

	\note Care should be exercised in ensuring that all threads are
	are waiting on semaphores have been shutdown down already.
	\sa Semaphore(Word32)

***************************************/

Burger::Semaphore::~Semaphore()
{
}

/*! ************************************

	\fn Word Burger::Semaphore::Acquire(void)
	\brief Acquire a lock on a semaphore resource

	If the semaphore's resource count has not gone to zero or less,
	decrement the count and immediately return. Otherwise, block
	until another thread releases the semaphore.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa TryAcquire(Word) or Release(void)

***************************************/

/*! ************************************

	\brief Acquire a lock on a semaphore resource with a timeout

	If the semaphore's resource count has not gone to zero or less,
	decrement the count and immediately return. Otherwise, block
	until another thread releases the semaphore or the time in
	milliseconds has elapsed. If the timeout is zero, return immediately
	with a non-zero error code.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0 means no wait, \ref BURGER_MAXUINT means infinite
	\return Zero on success, One on a timeout, and non Zero or One in the case of a semaphore failure
	\sa Acquire(void) or Release(void)

***************************************/

Word BURGER_API Burger::Semaphore::TryAcquire(Word /* uMilliseconds */)
{
	return 10;
}

/*! ************************************

	\brief Release a lock on a semaphore resource

	After a thread has acquired a resource via a semaphore, release
	it with this call once the resource is no longer needed.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa Acquire(void) or TryAcquire(Word)

***************************************/

Word BURGER_API Burger::Semaphore::Release(void)
{
	return 10;
}

/*! ************************************

	\fn Word32 Burger::Semaphore::GetValue(void) const
	\brief Get the current number of available resources.

	\return The number of available resources.
	\sa Semaphore(Word32)

***************************************/

#endif

/*! ************************************

	\class Burger::ConditionVariable
	\brief Class for creating a condition variable
	
	In a multiprocessor system, it's sometimes necessary to have
	a thread wait until an external condition has been
	satisfied. Once this class is created, the consumer thread
	will call Wait() with a locked CriticalSection and a timeout and
	it will halt until time has expired or another thread sends
	a signal to this class to alert the waiting thread
	that the stars are finally right and great Cthulhu can
	rise again.

	Further reading http://en.wikipedia.org/wiki/Condition_variable#Condition_variables

	\sa Burger::CriticalSection, Burger::Semaphore and Burger::Thread

***************************************/

/*! ************************************

	\brief Initialize a condition signaler

	\sa ~ConditionVariable()

***************************************/

#if !(defined(BURGER_SHIELD) || defined(BURGER_VITA) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::ConditionVariable::ConditionVariable() :
	m_CriticalSection(),
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
	\sa Broadcast() or Wait(CriticalSection *,Word)

***************************************/

Word BURGER_API Burger::ConditionVariable::Signal(void)
{
	m_CriticalSection.Lock();
	// Is there anyone waiting for a signal?
	if (m_uWaiting > m_uSignals) {
		// Add to the signal count (Wait() will decrement)
		++m_uSignals;
		// Release a waiting thread
		m_WaitSemaphore.Release();
		// Unlock the data to allow Wait() to continue
		m_CriticalSection.Unlock();
		// Increase the signals semaphore to match
		m_SignalsSemaphore.Acquire();
	} else {
		// No one was waiting, discard
		m_CriticalSection.Unlock();
	}
	return 0;
}

/*! ************************************

	\brief Signal all waiting threads

	If any threads are waiting for a signal, send a signal to 
	all of them so they all may continue execution. If no thread
	is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa Signal() or Wait(CriticalSection *,Word)

***************************************/

Word BURGER_API Burger::ConditionVariable::Broadcast(void)
{
	// Lock internal data
	m_CriticalSection.Lock();
	// Anyone waiting?
	if (m_uWaiting > m_uSignals) {
		// For all the ones that haven't already gotten a signal
		// let them execute
		Word32 uCount = m_uWaiting-m_uSignals;	// Always positive
		// Set the new count
		m_uSignals = m_uWaiting;
		// Reduce the number of waiting threads
		Word32 uCount2 = uCount;
		do {
			m_WaitSemaphore.Release();
		} while (--uCount2);
		// The act of unlocking will fire all the released threads
		m_CriticalSection.Unlock();

		// Reacquire the thread count to restore the wait count to
		// match m_uWaiting (Note that other threads could be modifying this
		// value, however, Wait() is updating this value, so 
		// from our point of view, it must be restored)

		do {
			m_WaitSemaphore.Acquire();
		} while (--uCount);
	} else {
		// Get out, nothing to see here
		m_CriticalSection.Unlock();
	}
	return 0;
}

/*! ************************************

	\brief Wait for a signal (With timeout)

	Halt the thread until a signal is received or if a timeout
	has elapsed.
	If any threads are waiting for a signal, send a signal to 
	all of them so they all may continue execution. If no thread
	is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa Signal() or Wait(CriticalSection *,Word)

***************************************/

Word BURGER_API Burger::ConditionVariable::Wait(CriticalSection *pCriticalSection,Word uMilliseconds)
{
	// A thread is waiting
	m_CriticalSection.Lock();
	++m_uWaiting;
	m_CriticalSection.Unlock();

	// Unlock the thread's lock
	pCriticalSection->Unlock();
	// Wait for a signal (With timeout)
	Word uResult = m_WaitSemaphore.TryAcquire(uMilliseconds);

	// It returned!
	m_CriticalSection.Lock();
	// Was there a signal from Signal() or Broadcast()?
	if (m_uSignals) {
		// Was there a timeout?
		if (uResult==1) {
			// Force an obtaining of a resource
			m_WaitSemaphore.Acquire();
		}
		// Mark this signal as processed
		m_SignalsSemaphore.Release();
		--m_uSignals;
	}
	// No longer waiting
	--m_uWaiting;
	m_CriticalSection.Unlock();
	// Relock the thread's lock
	pCriticalSection->Lock();
	return uResult;
}

#endif

/*! ************************************

	\class Burger::Thread
	\brief Class for creating a thread
	
	In a multiprocessor system, it's sometimes necessary to have
	another CPU run a concurrent thread. This class will handle
	the dispatching a thread.

	Further reading http://en.wikipedia.org/wiki/Thread_(computing)

	\sa Burger::CriticalSection, Burger::Semaphore and Burger::ConditionVariable

***************************************/

/*! ************************************

	\brief Initialize a thread to power up defaults

	\sa Start(), Thread(FunctionPtr,void *) or ~Thread()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_VITA) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
Burger::Thread::Thread() :
	m_pFunction(NULL),
	m_pData(NULL),
	m_pSemaphore(NULL),
	m_uResult(0)
{
}

/*! ************************************

	\brief Initialize a thread and begin execution

	This constructor will immediately start the thread.

	If deferred startup is desired, use the default constructor
	and then call Start() when it's time to launch the thread.

	\param pFunction Pointer to the entry point to the thread
	\param pData Pointer to data to pass to the thread
	\sa Thread() or ~Thread()

***************************************/

Burger::Thread::Thread(FunctionPtr pFunction,void *pData) :
	m_pFunction(pFunction),
	m_pData(pData),
	m_pSemaphore(NULL),
	m_uResult(0)
{
	// Runt the code
	Run(this);
}

/*! ************************************

	\brief Kill any running threads

	\sa Kill(), Thread(FunctionPtr,void *) or Thread()

***************************************/

Burger::Thread::~Thread()
{
	Kill();
}

/*! ************************************

	\brief Startup a thread

	If a thread isn't already running, launch this thread 
	of execution. If a thread is already running, return an error

	\param pFunction Pointer to the entry point to the thread
	\param pData Pointer to data to pass to the thread
	\return Zero if no error, non-zero if there was an error
	\sa Kill() or Wait()

***************************************/

Word BURGER_API Burger::Thread::Start(FunctionPtr pFunction,void *pData)
{
	m_pFunction = pFunction;
	m_pData = pData;
	Run(this);
	return 10;
}

/*! ************************************

	\brief Wait for a thread to exit

	If a thread isn't already running, return immediately.
	If a thread is already running, sleep until the thread
	has completed execution.

	\return Zero if no error, non-zero if there was an error
	\sa Kill() or Start()

***************************************/

Word BURGER_API Burger::Thread::Wait(void)
{
	m_pFunction = NULL;
	m_pData = NULL;
	return 10;
}

/*! ************************************

	\brief Force shutdown of a thread

	If a thread isn't already running, return immediately.
	If a thread is already running, alert the operating system
	that this thread should be aborted and shut down.

	\note This is a function of last resort. Some operating systems
	can leak resources if a thread is shut down in this manner. The
	proper way to shut down a thread is to sent the thread a signal
	to alert it to exit it's loop and call Wait().

	\return Zero if no error, non-zero if there was an error
	\sa Wait() or Start()

***************************************/

Word BURGER_API Burger::Thread::Kill(void)
{
	m_pFunction = NULL;
	m_pData = NULL;
	return 10;
}

/*! ************************************

	\brief Synchronize and then execute the thread and save
	the result if any

	This internal routine is used to synchronize with the main thread
	to ensure that the class variables are stable before execution
	begins. It will also capture the result code before
	exiting back to the operating system.

	\note This function should not be called by applications.

	\param pThis Pointer to an instance of Thread
	\sa Start()

***************************************/

void BURGER_API Burger::Thread::Run(void *pThis)
{
	Thread *pThread = static_cast<Thread *>(pThis);
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);
}

/*! ************************************

	\fn WordPtr Burger::Thread::GetResult(void) const
	\brief Return the exit code of the user supplied thread.

	When a thread launched by this class executes, it can return
	a result code which is captured by the dispatcher.
	The code can retrieved by this call. 

	\note The code is only valid once the thread exits cleanly.
	Ending a thread with a call to Kill() will set the error code
	to a non-zero number.

	\return Result code from the user code
	\sa Start() or IsInitialized()

***************************************/

/*! ************************************

	\fn Word Burger::Thread::IsInitialized(void) const
	\brief Return \ref TRUE if a thread is running.

	Once a thread is started, this function will return \ref
	TRUE. Once either Wait() or Kill() is called, this function
	will return \ref FALSE.

	\return \ref TRUE if a thread was started, or \ref FALSE if not.
	\sa Start() or GetResult()

***************************************/

#endif