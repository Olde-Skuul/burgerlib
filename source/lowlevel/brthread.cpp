/***************************************

	Class to manage threads

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"
#include "bratomic.h"
#include "brcriticalsection.h"
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"

#include <stdlib.h>

#if !defined(DOXYGEN)
// Used by tls_data_get_fallback() and tls_data_set_fallback()
// Root point to linked list of thread local storage records
static Burger::ThreadLocalStorageRecord_t* g_pThreadLocalStorageRecords;
static Burger::CriticalSection g_TLSRecordsCriticalSection;
#endif

/*! ************************************

	\enum Burger::eThreadPriority
	\brief Thread priority setting

	When calling Burger::set_thread_priority(Burger::ThreadID,
	Burger::eThreadPriority), this is the enumeration used to set the thread
	priority in a cross platform way.

	\note \ref kThreadPriorityInvalid is used as an error code for functions
		that fail to retrieve a thread priority.

	\sa \ref Burger::Thread, or Burger::set_thread_priority(Burger::ThreadID,
		Burger::eThreadPriority)

***************************************/

/*! ************************************

	\typedef Burger::ThreadID
	\brief Platform Thread ID

	Every thread has a thread identification number. The number itself is not
	important, and if 0 it means it's invalid. If non-zero, it's a value that is
	used by the platform to identify a thread. This ID is not a pointer, but an
	integer value used by the operating system.

	\sa \ref Burger::Thread

***************************************/

/*! ************************************

	\typedef Burger::TLSShutdownProc
	\brief Callback prototype for TLS shutdown

	typedef void(BURGER_API* TLSShutdownProc)(void *pThis);

	When ttls_set(uint32_t, const void*, TLSShutdownProc) is called, the passed
	pointer can be cleaned up using a callback function that matches this
	prototype. The data pointer is passed as the ``pThis`` parameter to the
	callback when \ref tls_release() is called.

	\sa tls_set(uint32_t, const void*, TLSShutdownProc) or, tls_release()

***************************************/

/*! ************************************

	\struct Burger::ThreadLocalStorageEntry_t
	\brief Thread local storage for shutdown callback

	Every thread has memory assigned to each thread that contains a "this"
	pointer and a callback function. If the function is set to \ref nullptr, no
	callback is issued. Otherwise, when the thread is shutdown, the function
	will be called to handle any sort of memory or resource cleanup needed for
	thread release.

	\note This structure is managed by the Thread manager and should not be used
	by applications directly

	\sa \ref Burger::Thread, or \ref Burger::ThreadLocalStorage_t

***************************************/

/*! ************************************

	\struct Burger::ThreadLocalStorage_t
	\brief Simple array of ThreadLocalStorageEntry_t records

	Every thread has memory assigned to each thread that contains a "this"
	pointer and a callback function. This array contains all of the records for
	all threads in the application.

	\note This structure is managed by the Thread manager and should not be used
	by applications directly

	\sa \ref Burger::Thread, or \ref Burger::ThreadLocalStorageEntry_t

***************************************/

/*! ************************************

	\struct Burger::ThreadLocalStorageRecord_t
	\brief Internal record to match ThreadLocalStorage_t to a thread.

	The functions tls_data_get_fallback() and
	tls_data_set_fallback(ThreadLocalStorage_t*) maintain a linked list of these
	records to match up threads to their ThreadLocalStorage_t entries.

	\note This structure is managed by the Thread manager and should not be used
	by applications directly

	\sa \ref Burger::Thread, or \ref Burger::tls_data_get()

***************************************/

/*! ************************************

	\brief Return the ID of the current thread

	Query the operating system for the ID number of the currently executing
	thread.

	If zero is returned, this feature is not supported

	\returns ID of the thread or 0 on unsupported systems, like MSDOS.

	\sa \ref Thread

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX) || defined(BURGER_MAC)) || \
	defined(DOXYGEN)
Burger::ThreadID BURGER_API Burger::get_ThreadID(void) BURGER_NOEXCEPT
{
	// Not supported, pretty much just MSDos
	return 0;
}
#endif

/*! ************************************

	\brief Get the execution priority of a thread

	Get the execution priority of any thread using a \ref ThreadID.

	If \ref kThreadPriorityInvalid is returned, this feature is not
	supported.

	\returns An \ref eThreadPriority enumeration.

	\sa \ref Thread, or set_thread_priority(ThreadID, eThreadPriority)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eThreadPriority BURGER_API Burger::get_thread_priority(
	ThreadID uThreadID) BURGER_NOEXCEPT
{
	// Not supported
	BURGER_UNUSED(uThreadID);
	return kThreadPriorityInvalid;
}

/*! ************************************

	\brief Set the execution priority of a thread

	Set the execution priority of any thread using a \ref ThreadID.

	If \ref kErrorNotSupportedOnThisPlatform is returned, this feature is not
	supported.

	\returns Zero if no error, non-zero on error.

	\sa \ref Thread, or get_thread_priority(ThreadID)

***************************************/

Burger::eError BURGER_API Burger::set_thread_priority(
	ThreadID uThreadID, eThreadPriority uThreadPriority) BURGER_NOEXCEPT
{
	BURGER_UNUSED(uThreadID);
	BURGER_UNUSED(uThreadPriority);
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Create a unique index for Thread Local Storage

	Atomically increment a counter and return the value. This value is used as
	an index to obtain the thread local storage to the thread that possesses the
	index.

	\note Once assigned, a number cannot be released. As such, don't create and
		destroy threads, just create the thread and put it to sleep until it's
		needed again.

	\returns An index starting with 1, and then increments on each call.

	\sa tls_get(uint32_t)

***************************************/

uint32_t BURGER_API Burger::tls_new_index(void) BURGER_NOEXCEPT
{
	// Note: The value MUST be in the function, so if it's forked, it will
	// generate a Thread Local Storage version and "reset" the count

	// Starts with zero
	static volatile uint32_t s_TLSIndex;

	// Return 1,2,3, etc...
	return atomic_add(&s_TLSIndex, 1U) + 1U;
}

/*! ************************************

	\brief Scan for Thread Local Storage (Private)

	For really old operating systems, or if the Thread Local Storage on the
	native operating system is full, this function will perform the same task.

	Scan a private linked list for thread storage records and if found, return
	the pointer to the ThreadLocalStorageRecord_t that is assigned to the
	currently running thread.

	\note This is a private function, call \ref tls_data_get() instead to use
		the native platform's version.

	\returns The ThreadLocalStorageRecord_t pointer or \ref nullptr

	\sa tls_data_set_fallback(ThreadLocalStorage_t*)

***************************************/

Burger::ThreadLocalStorage_t* BURGER_API Burger::tls_data_get_fallback(
	void) BURGER_NOEXCEPT
{
	// Assume failure
	ThreadLocalStorage_t* pResult = nullptr;

	// Obtain the current thread ID
	const ThreadID uThreadID = get_ThreadID();

	// Lock the data
	g_TLSRecordsCriticalSection.lock();

	// Traverse the linked list
	ThreadLocalStorageRecord_t* pRecord = g_pThreadLocalStorageRecords;
	if (pRecord) {

		do {
			// Found it?
			if (pRecord->m_uThreadID == uThreadID) {

				// Get the result and exit
				pResult = pRecord->m_pThreadLocalStorage;
				break;
			}
			// Follow the list
			pRecord = pRecord->m_pNext;
		} while (pRecord);
	}

	// Release the list
	g_TLSRecordsCriticalSection.unlock();

	// Return the result, or not.
	return pResult;
}

/*! ************************************

	\brief Set a Thread Local Storage entry (Private)

	For really old operating systems, or if the Thread Local Storage on the
	native operating system is full, this function will perform the same task.

	Scan a private linked list for thread storage records and if found, set
	the pointer to the ThreadLocalStorageRecord_t for the currently running
	thread. If no record was found, allocate a new record and add the data to
	this new record.

	\note This is a private function, call \ref
		tls_data_set(ThreadLocalStorage_t*) instead to use the native platform's
		version.

	\param pInput Pointer to a ThreadLocalStorage_t or \ref nullptr to delete
		the record if found

	\returns \ref kErrorNone or \ref kErrorOutOfMemory

	\sa tls_data_get_fallback()

***************************************/

Burger::eError BURGER_API Burger::tls_data_set_fallback(
	ThreadLocalStorage_t* pInput) BURGER_NOEXCEPT
{
	// Assume success
	eError uResult = kErrorNone;

	// Obtain the current thread ID
	const ThreadID uThreadID = get_ThreadID();

	// Lock the data below
	g_TLSRecordsCriticalSection.lock();

	// Traverse the list for a match
	ThreadLocalStorageRecord_t* pRecord = g_pThreadLocalStorageRecords;
	if (pRecord) {

		// Needed for removing a record
		ThreadLocalStorageRecord_t* pPrev = nullptr;

		do {
			// Match?
			if (pRecord->m_uThreadID == uThreadID) {

				// Delete the record?
				if (!pInput) {

					// Was this the root record?
					if (pPrev) {
						// Remove from the list
						pPrev->m_pNext = pRecord->m_pNext;
					} else {
						// Set a new root record
						g_pThreadLocalStorageRecords = pRecord->m_pNext;
					}

					// In either case, discard this record
					Free(pRecord);
				} else {

					// Just update the record
					pRecord->m_pThreadLocalStorage = pInput;
				}
				break;
			}

			// Save as the previous record
			pPrev = pRecord;

			// Follow the list
			pRecord = pRecord->m_pNext;
		} while (pRecord);
	}

	// No match and it's not a deletion?
	if (!pRecord && pInput) {

		// Allocate a new record
		pRecord = static_cast<ThreadLocalStorageRecord_t*>(
			Alloc(sizeof(ThreadLocalStorageRecord_t)));

		// Allocation failure?
		if (!pRecord) {
			uResult = kErrorOutOfMemory;
		} else {
			// Set the new record
			pRecord->m_uThreadID = uThreadID;
			pRecord->m_pThreadLocalStorage = pInput;

			// Make this the new root entry
			pRecord->m_pNext = g_pThreadLocalStorageRecords;
			g_pThreadLocalStorageRecords = pRecord;
		}
	}

	// Release the list
	g_TLSRecordsCriticalSection.unlock();
	return uResult;
}

/*! ************************************

	\brief Get Thread Local Storage

	Scan a private linked list for thread storage records and if found, return
	the pointer to the ThreadLocalStorageRecord_t that is assigned to the
	currently running thread.

	\returns The ThreadLocalStorageRecord_t pointer or \ref nullptr

	\sa tls_data_set(ThreadLocalStorage_t*)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5) || \
	defined(BURGER_WIIU) || defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::ThreadLocalStorage_t* BURGER_API Burger::tls_data_get(
	void) BURGER_NOEXCEPT
{
	return tls_data_get_fallback();
}

/*! ************************************

	\brief Set a Thread Local Storage entry

	Scan a private linked list for thread storage records and if found, set
	the pointer to the ThreadLocalStorageRecord_t for the currently running
	thread. If no record was found, allocate a new record and add the data to
	this new record.

	\param pInput Pointer to a ThreadLocalStorage_t or \ref nullptr to delete
		the record if found

	\returns \ref kErrorNone or \ref kErrorOutOfMemory

	\sa tls_data_get(void)

***************************************/

Burger::eError BURGER_API Burger::tls_data_set(
	ThreadLocalStorage_t* pInput) BURGER_NOEXCEPT
{
	return tls_data_set_fallback(pInput);
}
#endif

/*! ************************************

	\brief Get thread local storage for current thread

	Every thread is assigned an array of entries for data that is specific to
	each thread. Given an index that starts with 1, retrieve data stored by a
	previous call to tls_set(uint32_t, const void*, TLSShutdownProc).

	If the index is out of bounds, or no data entry exists for this thread, a
	\ref nullptr is returned.

	\note A \ref nullptr doesn't always mean an error, it's possible that a
		previous call to tls_set(uint32_t, const void*, TLSShutdownProc) passed
	\ref nullptr as the data.

	\param uIndex Index that starts with 1 or higher.

	\returns Previously stored data pointer or \ref nullptr

	\sa TLSShutdownProc, tls_set(uint32_t, const void*, TLSShutdownProc)

***************************************/

void* BURGER_API Burger::tls_get(uint32_t uIndex) BURGER_NOEXCEPT
{
	// Assume failure by a zero index
	void* pResult = nullptr;
	if (uIndex) {

		// Change 1 index to 0
		--uIndex;

		// Get the data
		const ThreadLocalStorage_t* pTLS = tls_data_get();

		// Valid pointer and not out of bounds?
		if (pTLS && (uIndex < pTLS->m_uCount)) {

			// Get the data pointer
			pResult = pTLS->m_Entries[uIndex].m_pThis;
		}
	}
	return pResult;
}

/*! ************************************

	\brief Set thread local storage for current thread

	Every thread is assigned an array of entries for data that is specific to
	each thread. Given an index that starts with 1, set the data pointer and a
	possible function to be called once the thread has shut down.

	If the index is out of bounds for this thread, the internal data structure
	is grown in size to accomodate the index.

	\param uIndex Index that starts with 1 or higher
	\param pThis Pointer to data to store
	\param pShutdown Pointer to a shutdown function to call on thread shutdown,
		can be \ref nullptr

	\returns 0 if no error or, \ref kErrorInvalidParameter or \ref
		kErrorOutOfMemory

	\sa TLSShutdownProc, tls_get(uint32_t), or tls_release()

***************************************/

Burger::eError BURGER_API Burger::tls_set(uint32_t uIndex, const void* pThis,
	TLSShutdownProc pShutdown) BURGER_NOEXCEPT
{
	// Assume error
	eError uResult = kErrorInvalidParameter;
	if (uIndex) {

		// Convert from 1 to 0 indexing
		--uIndex;

		uResult = kErrorNone;

		// Get the thread storage infomation
		ThreadLocalStorage_t* pTLS = tls_data_get();

		// Record found?
		if (!pTLS || (uIndex >= pTLS->m_uCount)) {

			// Get the current count, but set to zero if there was no previous
			// data
			uint32_t uCurrentCount = pTLS ? pTLS->m_uCount : 0;

			// Resize to this count, with a little padding
			const uint32_t uNewCount = uIndex + 8U;

			// Reallocate the buffer for the new entries
			// The -1 is because ThreadLocalStorage_t has a single record
			// already
			pTLS = static_cast<ThreadLocalStorage_t*>(Realloc(pTLS,
				sizeof(ThreadLocalStorage_t) +
					((uNewCount - 1) * sizeof(ThreadLocalStorageEntry_t))));

			// Mark the error condition if pTLS is nullptr
			uResult = kErrorOutOfMemory;
			if (pTLS) {

				// Update the record
				pTLS->m_uCount = uNewCount;

				// Fill in the new records with nothing
				ThreadLocalStorageEntry_t* pWork =
					&pTLS->m_Entries[uCurrentCount];
				uCurrentCount = uNewCount - uCurrentCount;
				do {
					pWork->m_pThis = nullptr;
					pWork->m_pShutdown = nullptr;
					++pWork;
				} while (--uCurrentCount);

				// Update the Thread Local Storage to new record
				uResult = tls_data_set(pTLS);
			}
		}

		// Update the data in the record, if no error
		if (!uResult) {
			ThreadLocalStorageEntry_t* pWork2 = &pTLS->m_Entries[uIndex];
			pWork2->m_pThis = const_cast<void*>(pThis);
			pWork2->m_pShutdown = pShutdown;
		}
	}

	// Return the error code
	return uResult;
}

/*! ************************************

	\brief Release Thread Local Storage for current thread

	The function will check if there is Thread Local Storage for the currently
	executing thread and if found, the record will be deleted. If there is any
	data stored within, the shutdown function will be called to perform any
	additional cleanup.

	\sa TLSShutdownProc, tls_get(uint32_t), or tls_set(uint32_t,
		const void*, TLSShutdownProc)

***************************************/

void BURGER_API Burger::tls_release(void) BURGER_NOEXCEPT
{
	// Any data cached?
	ThreadLocalStorage_t* pTLS = tls_data_get();
	if (pTLS) {

		// Any records stored within?
		uint32_t uCount = pTLS->m_uCount;
		if (uCount) {
			// Iterate over the records
			ThreadLocalStorageEntry_t* pWork = pTLS->m_Entries;
			do {
				// Is there a shutdown function?
				if (pWork->m_pShutdown) {
					// Call it for cleanup
					pWork->m_pShutdown(pWork->m_pThis);
				}

				// Next record
				++pWork;
			} while (--uCount);
		}

		// Unlink the ThreadLocalStorage_t record from the linked list
		tls_data_set(nullptr);

		// Actually dispose of the memory
		Free(pTLS);
	}
}

/*! ************************************

	\class Burger::Thread
	\brief Class for creating a thread

	In a multiprocessor system, it's sometimes necessary to have
	another CPU run a concurrent thread. This class will handle
	the dispatching a thread.

	Further reading http://en.wikipedia.org/wiki/Thread_(computing)

	\note Most platforms support native preemptive multi-threading, but the
		Nintendo 64, Wii and WiiU use cooperative multithreading, so great care
		is required when using threads.

	\sa \ref CriticalSection, \ref Semaphore and \ref ConditionVariable

***************************************/

/*! ************************************

	\enum Burger::Thread::eState
	\brief Current state of the Thread class

	Once a Thread class is instanciated, it's set to the kStateInvalid state.
	Once a thread has begun, the state value will be updated to reflect the
	state of the thread as it executes and possibly terminates.

	\sa \ref Thread

***************************************/

/*! ************************************

	\brief Initialize a thread to power up defaults

	No thread is launched, the class is set up.

	\sa start(), or ~Thread()

***************************************/

#if !( \
	defined(BURGER_VITA) || defined(BURGER_PS3)) || \
	defined(DOXYGEN)
Burger::Thread::Thread() BURGER_NOEXCEPT: m_pFunction(nullptr),
										  m_pData(nullptr),
										  m_pName(nullptr),
										  m_uStackSize(0),
										  m_uResult(0),
										  m_uThreadID(0),
										  m_uState(kStateInvalid)

{
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE)
	m_pThreadHandle = nullptr;
#endif

#if defined(BURGER_WIIU)
	m_pStack = nullptr;
#endif
}
#endif

/*! ************************************

	\brief Destructor

	Wait until the thread completes and then release resources

	\sa wait(), start(FunctionPtr, void*, const char*, uintptr_t) or Thread()

***************************************/

Burger::Thread::~Thread()
{
	// If a thread is destructed, it will wait until the thread ends
	wait();

	// Make sure the name is released
	Free(m_pName);
	m_pName = nullptr;

#if defined(BURGER_WIIU)
	if (m_pStack) {
		delete[] m_pStack;
		m_pStack = nullptr;
	}
	if (m_pOSThread) {
		free(m_pOSThread);
		m_pOSThread = nullptr;
	}
#endif
}

/*! ************************************

	\brief Startup a thread

	If a thread isn't already running, launch this thread of execution. If a
	thread is already running, return \ref kErrorThreadAlreadyStarted.

	If a name is passed for the name of the thread, it is copied so it's not
	necessary to keep the name around once this function returns.

	\param pFunction Pointer to the entry point to the thread
	\param pData Pointer to data to pass to the thread
	\param pName Pointer to a "C" string for the name of the thread
	\param uStackSize Size of the stack for the thread

	\return Zero if no error, non-zero if there was an error

	\sa kill() or wait()

***************************************/

Burger::eError BURGER_API Burger::Thread::start(FunctionPtr pFunction,
	void* pData, const char* pName, uintptr_t uStackSize) BURGER_NOEXCEPT
{
	// Already started?
	if ((m_uState != kStateInvalid) && (m_uState != kStateEnded)) {
		return kErrorThreadAlreadyStarted;
	}
	// Release the thread name, if one was present
	Free(m_pName);

	// Set up all the thread's values
	m_pFunction = pFunction;
	m_pData = pData;
	m_pName = StringDuplicate(pName);
	m_uStackSize = uStackSize;

	// Assume result is zero
	m_uResult = 0;

	// Force invalid state until it's really started
	m_uState = kStateInvalid;

	// Actually start the thread
	eError uResult = platform_start();

	// Are are boned?
	if (uResult) {
		m_uState = kStateInvalid;
		Free(m_pName);
		m_pName = nullptr;
	}

	// Code has run.
	return uResult;
}

/*! ************************************

	\brief Wait for a thread to exit

	If a thread isn't already running, return immediately. If a thread is
	already running, sleep until the thread has completed execution.

	\return Zero if no error, non-zero if there was an error

	\sa detach() or start()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Thread::wait(void) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Detach a thread

	If a thread isn't already running, return immediately.
	If a thread is already running, alert the operating system
	that this thread should be aborted and shut down.

	\note This is a function of last resort. Some operating systems
	can leak resources if a thread is shut down in this manner. The
	proper way to shut down a thread is to sent the thread a signal
	to alert it to exit it's loop and call wait().

	\return Zero if no error, non-zero if there was an error
	\sa wait() or start()

***************************************/

Burger::eError BURGER_API Burger::Thread::detach(void) BURGER_NOEXCEPT
{
	// Already detached?
	if ((m_uState == kStateDetached) || (m_uState == kStateEnded)) {
		return kErrorNone;
	}

	// Bogus?
	if (m_uState == kStateInvalid) {
		return kErrorThreadNotStarted;
	}

	// Running? Let it go!
	return platform_detach();
}

/*! ************************************

	\brief Return the name of the thread

	Returns either an empty string or the assigned name of the thread. This
	function will not return \ref nullptr

	\return A "C" string pointer to either the name of the thread or ""

	\sa start() or is_initialized()

***************************************/

const char* BURGER_API Burger::Thread::get_name(void) const BURGER_NOEXCEPT
{
	const char* pResult = m_pName;
	return pResult ? pResult : g_EmptyString;
}

/*! ************************************

	\fn Burger::Thread::get_result(void) const
	\brief Return the exit code of the user supplied thread.

	When a thread launched by this class executes, it can return a result code
	which is captured by the dispatcher. The code can retrieved by this call.

	\note The code is only valid once the thread exits cleanly. Check the state
		for \ref kStateEnded to ensure the value is valid.

	\return Result code from the thread code

	\sa start() or is_initialized()

***************************************/

/*! ************************************

	\fn Burger::Thread::get_stack_size(void) const
	\brief Return the size of the stack allocated for this thread.

	When a thread launched by this class, a stack is assigned. If the default
	value of 0 is used, the platform will determine the default stack size. The
	value assigned can be obtained by this call after the thread began
	execution.

	\return Size of the thread's stack in bytes.

	\sa start() or is_initialized()

***************************************/

/*! ************************************

	\fn Burger::Thread::is_initialized(void) const
	\brief Return \ref TRUE if a thread is running.

	Once a thread is started, this function will return \ref TRUE. Once either
	wait() is called or the thread terminates, this function will return \ref
	FALSE.

	\return \ref TRUE if a thread is in progress, or \ref FALSE if not.

	\sa start() or get_result()

***************************************/

/*! ************************************

	\brief Internal function to dispatch to the thread

	This internal routine is used to synchronize with the main thread to ensure
	that the class variables are stable before execution begins. It will also
	capture the result code before exiting back to the operating system.

	\note This function should not be called by applications.

	\param pThis Pointer to an instance of Thread

	\sa start(), or get_result()

***************************************/

void BURGER_API Burger::Thread::run(void* pThis) BURGER_NOEXCEPT
{
	// Create the "this" pointer
	Thread* pThread = static_cast<Thread*>(pThis);

	// Have the platform name the thread and get other information
	pThread->platform_after_start();

	// Call the thread, and capture the return value.
	pThread->m_uResult = pThread->m_pFunction(pThread->m_pData);

	// If during execution, Thread Local Storage was created, release it
	tls_release();

	// The thread has terminated execution, m_uResult is valid
	pThread->m_uState = kStateEnded;
}

/*! ************************************

	\brief Internal function to start a thread

	This function executes platform specific code to start a thread.

	\note This function should not be called by applications.

	\sa platform_after_start()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Thread::platform_start(void) BURGER_NOEXCEPT
{
	// Generic code says, uh... No.
	m_uState = kStateInvalid;
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Internal function to set up data specific to a thread

	This function executes platform specific code to execute before actually
	dispatching to the thread. This function is called in the same process as
	the thread, not the parent process.

	\note This function should not be called by applications.

	\sa platform_start()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA) || defined(BURGER_PS3) || \
	defined(BURGER_PS4) || defined(BURGER_PS5) || defined(BURGER_WIIU) || \
	defined(BURGER_SWITCH) || defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Thread::platform_after_start(
	void) BURGER_NOEXCEPT
{
	// Fake ThreadID
	m_uThreadID = reinterpret_cast<ThreadID>(this);

	// Seriously, not supported
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Internal function to detach a thread

	This function executes platform specific code detach a thread.

	\note This function should not be called by applications.

	\sa detach()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_PS3) || defined(BURGER_PS4) || \
	defined(BURGER_PS5) || defined(BURGER_WIIU) || defined(BURGER_SWITCH) || \
	defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Thread::platform_detach(void) BURGER_NOEXCEPT
{
	// Seriously, not supported
	return kErrorNotSupportedOnThisPlatform;
}
#endif