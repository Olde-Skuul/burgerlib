/***************************************

	Root base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360) && !defined(BURGER_PS3)) || defined(DOXYGEN)
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