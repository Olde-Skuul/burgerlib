/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_PS3)
#include "brstringfunctions.h"

#include <sys/ppu_thread.h>

/***************************************

	\brief Return the ID of the current thread

	Query the operating system for the ID number of the currently executing
	thread.

	If zero is returned, this feature is not supported

	\returns ID of the thread or 0 on unsupported systems, like MSDOS.

	\sa \ref Thread

***************************************/

Burger::ThreadID BURGER_API Burger::get_ThreadID(void) BURGER_NOEXCEPT
{
	sys_ppu_thread_t uThreadID;
	int iResult = sys_ppu_thread_get_id(&uThreadID);
	if (iResult != CELL_OK) {
		return 0;
	}
	return static_cast<ThreadID>(uThreadID);
}

/***************************************

	\brief Get the execution priority of a thread

	Get the execution priority of any thread using a \ref ThreadID.

	If \ref kThreadPriorityInvalid is returned, this feature is not
	supported.

	\returns An \ref eThreadPriority enumeration.

	\sa \ref Thread, or set_thread_priority(ThreadID, eThreadPriority)

***************************************/

Burger::eThreadPriority BURGER_API Burger::get_thread_priority(
	ThreadID uThreadID) BURGER_NOEXCEPT
{
	// Assume error
	eThreadPriority uResult = kThreadPriorityInvalid;

	// Get the priority value
	int iPriority;
	int iResult = sys_ppu_thread_get_priority(uThreadID, &iPriority);

	if (iResult == CELL_OK) {

		// PS3 docs say priority 1001 is "default"

		// Convert from enumeration to native priority values
		if (iPriority > 1001) {
			uResult = kThreadPriorityLow;
		} else if (iPriority > 500) {
			uResult = kThreadPriorityNormal;
		} else if (iPriority >= 250) {
			uResult = kThreadPriorityHigh;
		} else {
			uResult = kThreadPriorityRealTime;
		}
	}

	// Return error
	return uResult;
}

/***************************************

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
	// Set to zero to shut up compiler warning
	int iPriority = 0;
	eError uResult = kErrorNone;

	// Convert from enumeration to native priority values
	switch (uThreadPriority) {
	case kThreadPriorityLow:
		iPriority = 3071;
		break;

	case kThreadPriorityNormal:
		// PS3 docs say priority 1001 is "default"
		iPriority = 1001;
		break;
	case kThreadPriorityHigh:
		iPriority = 500;
		break;
	case kThreadPriorityRealTime:
		iPriority = 0;
		break;
	// Bad enumeration?
	default:
		uResult = kErrorInvalidParameter;
		break;
	}

	// If not a bad enumeration, set it
	if (!uResult) {

		// Get the thread handle from the ThreadID
		int iResult = sys_ppu_thread_set_priority(uThreadID, iPriority);
		if (iResult == ESRCH) {
			// Bad thread
			uResult = kErrorThreadNotFound;
		} else if (iResult != CELL_OK) {
			// Was it successful?
			uResult = kErrorThreadNotModified;
		} else {
			uResult = kErrorNone;
		}
	}

	// Return error
	return uResult;
}

/***************************************

	This code fragment calls the Run function that has
	permission to access the members

***************************************/

static void Dispatcher(uint64_t pThis) BURGER_NOEXCEPT
{
	Burger::Thread::run(reinterpret_cast<void*>(static_cast<uint32_t>(pThis)));
	sys_ppu_thread_exit(0);
}

/*! ************************************

	\brief Initialize a thread to power up defaults

	No thread is launched, the class is set up.

	\sa start(), or ~Thread()

***************************************/

Burger::Thread::Thread() BURGER_NOEXCEPT
	: m_pFunction(nullptr),
	  m_pData(nullptr),
	  m_pName(nullptr),
	  m_uStackSize(0),
	  m_uResult(0),
	  m_uThreadID(SYS_PPU_THREAD_ID_INVALID),
	  m_uState(kStateInvalid)
{
}

/***************************************

	\brief Internal function to start a thread

	This function executes platform specific code to start a thread.

	\note This function should not be called by applications.

	\sa platform_after_start()

***************************************/

Burger::eError BURGER_API Burger::Thread::platform_start(void) BURGER_NOEXCEPT
{
	uintptr_t uStackSize = m_uStackSize;
	if (!uStackSize) {
		// This is usually 4K, but let's double it to 8K
		uStackSize = 0x2000U;
		m_uStackSize = uStackSize;
	}

	// There is a hard coded maximum name length of 28 bytes
	char ThreadName[28];
	const char* pTName;
	if (m_pName) {
		StringCopy(ThreadName, 28, m_pName);
		pTName = ThreadName;
	} else {
		pTName = "Burgerlib Thread";
	}

	// Create the thread, but it's not started yet
	// PS3 docs say priority 1001 is "default"
	sys_ppu_thread_t pThread;
	int iResult = sys_ppu_thread_create(&pThread, Dispatcher,
		reinterpret_cast<uint64_t>(this), 1001, uStackSize,
		SYS_PPU_THREAD_CREATE_JOINABLE, pTName);

	// Thread created?
	if (iResult >= CELL_OK) {

		// All good!
		return kErrorNone;
	}
	return kErrorThreadNotStarted;
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
	// Get the thread ID and store it in the class
	sys_ppu_thread_t pThread;

	// Should never fail
	if (sys_ppu_thread_get_id(&pThread) == CELL_OK) {
		m_uThreadID = pThread;
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

	// Was there a thread running?
	if (m_uThreadID != SYS_PPU_THREAD_ID_INVALID) {

		// Wait until the thread completes execution
		uint64_t uExitStatus;
		int iResult = sys_ppu_thread_join(m_uThreadID, &uExitStatus);
		if (iResult == CELL_OK) {

			// Allow restarting
			m_uThreadID = SYS_PPU_THREAD_ID_INVALID;
			uResult = kErrorNone;
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
	if (m_uThreadID == SYS_PPU_THREAD_ID_INVALID) {
		return kErrorThreadNotStarted;
	}
	// Release control of the thread
	if (sys_ppu_thread_detach(m_uThreadID) != CELL_OK) {
		return kErrorThreadCantStop;
	}

	// Dispose of the internal reference
	m_uThreadID = SYS_PPU_THREAD_ID_INVALID;

	// Detached, and possibly still running
	m_uState = kStateDetached;
	return kErrorNone;
}

#endif
