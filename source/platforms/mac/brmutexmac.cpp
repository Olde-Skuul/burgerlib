/***************************************

	Class to handle mutex objects, MacOS version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmutex.h"

#if defined(BURGER_MAC)

#include "brmemoryfunctions.h"

#include <Multiprocessing.h>
#include <OpenTransport.h>
#include <Threads.h>

/***************************************

	\brief Initialize the data in the class.

	Sets up operating system defaults to the data

	The MacOS version uses the Multiprocessing API for PowerPC macs since they
	can have multiple CPUs. The 680x0 mac uses the Thread Manager for
	cooperative single CPU multitasking.

***************************************/

Burger::Mutex::Mutex() BURGER_NOEXCEPT
{
	// Set up a proper critical region for PowerPC on the MACH Kernel
#if defined(BURGER_PPC)

	MPCriticalRegionID pRegionID = nullptr;
	OSStatus uError = MPCreateCriticalRegion(&pRegionID);
	if (uError) {
		pRegionID = nullptr;
		// TODO: Log error
	}
	m_PlatformMutex[0] = pRegionID;

#else

	// 68K version is more manual

	// No thread yet
	m_PlatformMutex[0] = reinterpret_cast<void*>(kNoThreadID);

	// No waiting threads
	m_uQueueCount = 0;

	// No locks yet
	m_uLockCount = 0;
#endif
}

/***************************************

	\brief Shutdown the data in the class.

	Releases the operating system resources allocated by the
	constructor.

***************************************/

Burger::Mutex::~Mutex()
{
#if defined(BURGER_PPC)
	MPCriticalRegionID pRegionID =
		static_cast<MPCriticalRegionID>(m_PlatformMutex[0]);
	if (pRegionID) {
		MPDeleteCriticalRegion(pRegionID);
		m_PlatformMutex[0] = nullptr;
	}
#endif
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
#if defined(BURGER_PPC)

	// Just obtain the critical section and wait until it's released
	MPEnterCriticalRegion(
		static_cast<MPCriticalRegionID>(m_PlatformMutex[0]), kDurationForever);

#else

	// For 68K, disable task switching
	ThreadBeginCritical();

	// Get my thread ID
	ThreadID MyID = kNoThreadID;
	MacGetCurrentThread(&MyID);

	// Is the Mutex free or already owned by this thread?
	ThreadID OldID = reinterpret_cast<ThreadID>(m_PlatformMutex[0]);
	while ((OldID != kNoThreadID) && (OldID != MyID)) {

		// Append this ID to the run queue
		// Don't overflow the buffer
		if (m_uQueueCount < BURGER_ARRAYSIZE(m_Queue)) {
			m_Queue[m_uQueueCount] = MyID;
			++m_uQueueCount;
		}

		// Stop this thread and run the owner's thread to clear the mutex
		SetThreadStateEndCritical(kCurrentThreadID, kStoppedThreadState, OldID);

		// Let's lock the globals and see if this thread can take possession
		ThreadBeginCritical();

		// Reload since it can change
		OldID = reinterpret_cast<ThreadID>(m_PlatformMutex[0]);
	}

	// Claim the Mutex!
	m_PlatformMutex[0] = reinterpret_cast<void*>(MyID);
	++m_uLockCount;

	// Restore multitasking
	ThreadEndCritical();
#endif
}

/***************************************

	\brief Attempt to lock the mutex

	If the mutex is locked, the function fails and returns \ref FALSE.
	Otherwise, the mutex is locked and the function returns \ref TRUE.

	\sa lock(), or unlock()

***************************************/

uint_t Burger::Mutex::try_lock() BURGER_NOEXCEPT
{
#if defined(BURGER_PPC)
	// Try to get the lock, but fail immediately if the lock is not obtained
	return MPEnterCriticalRegion(
			   static_cast<MPCriticalRegionID>(m_PlatformMutex[0]),
			   kDurationImmediate) == noErr;

#else

	// For 68K, disable task switching
	ThreadBeginCritical();

	// Get my thread ID
	ThreadID MyID = kNoThreadID;
	MacGetCurrentThread(&MyID);

	// Is the Mutex free or already owned by this thread?
	ThreadID OldID = reinterpret_cast<ThreadID>(m_PlatformMutex[0]);

	// Assume failure
	uint_t bResult = FALSE;
	if ((OldID == kNoThreadID) || (OldID == MyID)) {

		// Claim the Mutex!
		m_PlatformMutex[0] = reinterpret_cast<void*>(MyID);
		++m_uLockCount;
		bResult = TRUE;
	}

	// Restore multitasking
	ThreadEndCritical();
	return bResult;
#endif
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
#if defined(BURGER_PPC)

	// Release the critical section
	MPExitCriticalRegion(static_cast<MPCriticalRegionID>(m_PlatformMutex[0]));

#else

	// For 68K, disable task switching
	ThreadBeginCritical();

	// Perform a release
	if (m_uLockCount) {
		--m_uLockCount;
	}

	// Release the ownership
	m_PlatformMutex[0] = reinterpret_cast<void*>(kNoThreadID);

	// Now, find a thread to jump to
	ThreadID pNextThread = kNoThreadID;
	while (m_uQueueCount) {

		// Pull an entry from the list
		pNextThread = m_Queue[0];
		--m_uQueueCount;
		memory_move(
			&m_Queue[0], &m_Queue[1], sizeof(m_Queue[0]) * m_uQueueCount);

		// Is this thread still valid?
		OSErr iError =
			SetThreadState(pNextThread, kReadyThreadState, kNoThreadID);
		if (iError != threadNotFoundErr) {
			// It's good, use this thread
			break;
		}

		// Set to invalid in case the look exits
		pNextThread = kNoThreadID;
	}

	// Restore multitasking	and transfer control to the next thread
	// This function also releases the ThreadBeginCritical() call
	SetThreadStateEndCritical(kCurrentThreadID, kReadyThreadState, pNextThread);

#endif
}

#endif
