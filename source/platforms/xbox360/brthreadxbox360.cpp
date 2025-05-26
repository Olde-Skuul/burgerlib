/***************************************

	Class to handle threads, Xbox 360 version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_XBOX360)
#include "bratomic.h"
#include "x360_thread.h"

#define NOD3D
#define NONET
#include <xtl.h>

#include <process.h>

#if !defined(DOXYGEN)
// Global Thread Local Storage index
static DWORD gStorage = TlsAlloc();
#endif

/***************************************

	\brief Return the ID of the current thread

	Query the operating system for the ID number of the currently executing
	thread.

	If zero is returned, this feature is not supported

	\returns ID of the thread or 0 on unsupported systems, like MSDOS.

	\sa \ref Thread

***************************************/

Burger::thread_ID_t BURGER_API Burger::get_ThreadID(void) BURGER_NOEXCEPT
{
	return GetCurrentThreadId();
}

/***************************************

	\brief Get the execution priority of a thread

	Get the execution priority of any thread using a \ref thread_ID_t.

	If \ref kThreadPriorityInvalid is returned, this feature is not
	supported.

	\returns An \ref eThreadPriority enumeration.

	\sa \ref Thread, or set_thread_priority(thread_ID_t, eThreadPriority)

***************************************/

Burger::eThreadPriority BURGER_API Burger::get_thread_priority(
	thread_ID_t uThreadID) BURGER_NOEXCEPT
{
	// Assume error
	eThreadPriority uResult = kThreadPriorityInvalid;

	// Get the thread handle from the thread_ID_t
	HANDLE hThread = OpenThread(
		THREAD_QUERY_INFORMATION, FALSE, static_cast<DWORD>(uThreadID));

	if (hThread) {
		// Get the priority value
		const int iPriority = GetThreadPriority(hThread);

		if (iPriority != THREAD_PRIORITY_ERROR_RETURN) {

			// Convert from enumeration to native priority values
			if (iPriority <= THREAD_PRIORITY_LOWEST) {
				uResult = kThreadPriorityLow;
			} else if (iPriority <= THREAD_PRIORITY_NORMAL) {
				uResult = kThreadPriorityNormal;
			} else if (iPriority <= THREAD_PRIORITY_HIGHEST) {
				uResult = kThreadPriorityHigh;
			} else {
				uResult = kThreadPriorityRealTime;
			}
		}
		// Release the handle
		CloseHandle(hThread);
	}

	// Return error
	return uResult;
}

/***************************************

	\brief Set the execution priority of a thread

	Set the execution priority of any thread using a \ref thread_ID_t.

	If \ref kErrorNotSupportedOnThisPlatform is returned, this feature is not
	supported.

	\returns Zero if no error, non-zero on error.

	\sa \ref Thread, or get_thread_priority(thread_ID_t)

***************************************/

Burger::eError BURGER_API Burger::set_thread_priority(
	thread_ID_t uThreadID, eThreadPriority uThreadPriority) BURGER_NOEXCEPT
{
	// Set to zero to shut up compiler warning
	int iPriority = 0;
	eError uResult = kErrorNone;

	// Convert from enumeration to native priority values
	switch (uThreadPriority) {
	case kThreadPriorityLow:
		iPriority = THREAD_PRIORITY_LOWEST;
		break;
	case kThreadPriorityNormal:
		iPriority = THREAD_PRIORITY_NORMAL;
		break;
	case kThreadPriorityHigh:
		iPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case kThreadPriorityRealTime:
		iPriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	// Bad enumeration?
	default:
		uResult = kErrorInvalidParameter;
		break;
	}

	// If not a bad enumeration, set it
	if (!uResult) {

		// Assume error
		uResult = kErrorThreadNotFound;

		// Get the thread handle from the thread_ID_t
		HANDLE hThread = OpenThread(
			THREAD_SET_INFORMATION, FALSE, static_cast<DWORD>(uThreadID));

		if (hThread) {
			// Apply the new priority
			const BOOL bResult = SetThreadPriority(hThread, iPriority);

			// Was it successful?
			if (!bResult) {
				uResult = kErrorThreadNotModified;
			} else {
				uResult = kErrorNone;
			}

			// Release the handle
			CloseHandle(hThread);
		}
	}

	// Return error
	return uResult;
}

/***************************************

	\brief Get Thread Local Storage

	Scan a private linked list for thread storage records and if found, return
	the pointer to the thread_local_storage_record_t that is assigned to the
	currently running thread.

	\returns The thread_local_storage_record_t pointer or \ref nullptr

	\sa tls_data_set(thread_local_storage_t*)

***************************************/

Burger::thread_local_storage_t* BURGER_API Burger::tls_data_get(
	void) BURGER_NOEXCEPT
{
	if (gStorage == TLS_OUT_OF_INDEXES) {
		return tls_data_get_fallback();
	}
	return static_cast<thread_local_storage_t*>(TlsGetValue(gStorage));
}

/***************************************

	\brief Set a Thread Local Storage entry

	Scan a private linked list for thread storage records and if found, set
	the pointer to the thread_local_storage_record_t for the currently running
	thread. If no record was found, allocate a new record and add the data to
	this new record.

	\param pInput Pointer to a thread_local_storage_t or \ref nullptr to delete
		the record if found

	\returns \ref kErrorNone or \ref kErrorOutOfMemory

	\sa tls_data_get(void)

***************************************/

Burger::eError BURGER_API Burger::tls_data_set(
	thread_local_storage_t* pInput) BURGER_NOEXCEPT
{
	if (gStorage == TLS_OUT_OF_INDEXES) {
		return tls_data_set_fallback(pInput);
	}
	if (!TlsSetValue(gStorage, pInput)) {
		return kErrorOutOfEntries;
	}
	return kErrorNone;
}

/***************************************

	This code fragment calls the Run function that has
	permission to access the members

***************************************/

static uint_t __stdcall Dispatcher(void* pThis) BURGER_NOEXCEPT
{
	Burger::Thread::run(pThis);
	_endthreadex(0);
	return 0;
}

/***************************************

	\brief Internal function to start a thread

	This function executes platform specific code to start a thread.

	\note This function should not be called by applications.

	\sa platform_after_start()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_start(void) BURGER_NOEXCEPT
{
	// Set the flag to set the stack
	// Note, create the thread, but DON'T START IT!

	// Create the thread, and don't start it, yet.
	uint_t uThreadID;
	HANDLE hHandle = reinterpret_cast<HANDLE>(
		_beginthreadex(nullptr, static_cast<uint_t>(m_uStackSize), Dispatcher,
			this, CREATE_SUSPENDED, &uThreadID));

	// On the Xbox 360, the default is 64K
	if (!m_uStackSize) {
		m_uStackSize = 0x10000U;
	}

	// Assume failure
	eError uResult = kErrorThreadNotStarted;
	if (hHandle) {

		// Store the found thread_ID_t and handle
		m_uThreadID = uThreadID;
		m_pThreadHandle = hHandle;

		// Mark as running
		m_uState = kStateRunning;

		// Fire it up!
		ResumeThread(hHandle);

		// All good!
		uResult = kErrorNone;
	}

	return uResult;
}

/***************************************

	\brief Internal function to set up data specific to a thread

	This function executes platform specific code to execute before actually
	dispatching to the thread. This function is called in the same process as
	the thread, not the parent process.

	\note This function should not be called by applications.

	\sa platform_start()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_after_start(
	void) BURGER_NOEXCEPT
{
	// Was a name assigned?
	if (m_pName) {
		// Perform dark magic to assign the thread's name
		Xbox360::set_thread_name(m_pName, m_uThreadID);
	}
	return kErrorNone;
}

/*! ************************************

	\brief Wait for a thread to exit

	If a thread isn't already running, return immediately. If a thread is
	already running, sleep until the thread has completed execution.

	\return Zero if no error, non-zero if there was an error

	\sa detach() or start()

***************************************/

Burger::eError BURGER_API Burger::Thread::wait(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorThreadNotStarted;
	if (m_pThreadHandle) {

		// Wait until the thread completes execution
		// Note: This also updates the thread's state
		const DWORD uError =
			WaitForSingleObjectEx(m_pThreadHandle, INFINITE, FALSE);

		// Close it down!
		CloseHandle(m_pThreadHandle);

		// Allow restarting
		m_uThreadID = 0;
		m_pThreadHandle = nullptr;

		// Shutdown fine?
		if (uError == WAIT_OBJECT_0) {
			uResult = kErrorNone;

			// Timeout!
		} else if (uError == WAIT_TIMEOUT) {
			uResult = kErrorTimeout;
		}
	}
	return uResult;
}

/***************************************

	\brief Internal function to detach a thread

	This function executes platform specific code detach a thread.

	\note This function should not be called by applications.

	\sa detach()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_detach(void) BURGER_NOEXCEPT
{
	// Release control of the thread
	CloseHandle(m_pThreadHandle);

	// Dispose of the internal reference
	m_pThreadHandle = nullptr;
	m_uThreadID = 0;

	// Detached, and possibly still running
	m_uState = kStateDetached;
	return kErrorNone;
}

#endif
