/***************************************

	Class to handle critical sections, Playstation Vita version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brthread.h"

#if defined(BURGER_VITA)
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

#include <errno.h>
#include <kernel.h>

// Macros for the fake normal and high priority thread settings
#if !defined(DOXYGEN)
#define NORMAL \
	(SCE_KERNEL_HIGHEST_PRIORITY_USER + \
		((SCE_KERNEL_LOWEST_PRIORITY_USER - \
			 SCE_KERNEL_HIGHEST_PRIORITY_USER) / \
			3))

#define HIGH \
	(SCE_KERNEL_HIGHEST_PRIORITY_USER + \
		(((SCE_KERNEL_LOWEST_PRIORITY_USER - \
			  SCE_KERNEL_HIGHEST_PRIORITY_USER) / \
			 3) * \
			2))
#endif

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
	return static_cast<ThreadID>(sceKernelGetThreadId());
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
	// Get the information about the thread of interest
	SceKernelThreadInfo Info;
	memory_clear(&Info, sizeof(Info));
	Info.size = sizeof(Info);
	SceInt32 iResult = sceKernelGetThreadInfo(uThreadID, &Info);

	// Assume error
	eThreadPriority uResult = kThreadPriorityInvalid;
	if (iResult >= 0) {
		SceInt32 iPriority = Info.currentPriority;

		// Convert from enumeration to native priority values
		if (iPriority <= SCE_KERNEL_HIGHEST_PRIORITY_USER) {
			uResult = kThreadPriorityRealTime;
		} else if (iPriority <= HIGH) {
			uResult = kThreadPriorityHigh;
		} else if (iPriority <= NORMAL) {
			uResult = kThreadPriorityNormal;
		} else {
			uResult = kThreadPriorityLow;
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
	// Make sure LOWER is higher in value than highest
	BURGER_STATIC_ASSERT(
		SCE_KERNEL_LOWEST_PRIORITY_USER > SCE_KERNEL_HIGHEST_PRIORITY_USER);

	// Set to zero to shut up compiler warning
	SceInt32 iPriority = 0;
	eError uResult = kErrorNone;

	// Convert from enumeration to native priority values
	switch (uThreadPriority) {
	case kThreadPriorityLow:
		iPriority = SCE_KERNEL_LOWEST_PRIORITY_USER;
		break;
	case kThreadPriorityNormal:
		iPriority = NORMAL;
		break;
	case kThreadPriorityHigh:
		iPriority = HIGH;
		break;
	case kThreadPriorityRealTime:
		iPriority = SCE_KERNEL_HIGHEST_PRIORITY_USER;
		break;
	// Bad enumeration?
	default:
		uResult = kErrorInvalidParameter;
		break;
	}

	// If not a bad enumeration, set it
	if (!uResult) {
		// Apply the new priority
		int iResult = sceKernelChangeThreadPriority(uThreadID, iPriority);

		// Was it successful?
		if (iResult < SCE_OK) {
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

static SceInt32 Dispatcher(SceSize argSize, void* pArgBlock)
{
	Burger::Thread* pThis = static_cast<Burger::Thread**>(pArgBlock)[0];
	Burger::Thread::run(pThis);
	return 0;
}

/***************************************

	\brief Initialize a thread to power up defaults

	No thread is launched, the class is set up.

	\sa start(), or ~Thread()

***************************************/

Burger::Thread::Thread() BURGER_NOEXCEPT: m_pFunction(nullptr),
										  m_pData(nullptr),
										  m_pName(nullptr),
										  m_uStackSize(0),
										  m_uResult(UINT32_MAX),
										  m_uThreadID(0),
										  m_uState(kStateInvalid)
{
}

/***************************************

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
	if (m_uThreadID != UINT32_MAX) {
		// Wait until the thread completes execution
		sceKernelWaitThreadEnd(m_uThreadID, nullptr, nullptr);
		sceKernelDeleteThread(m_uThreadID);

		// Allow restarting
		m_uThreadID = UINT32_MAX;
		uResult = kErrorNone;
	}
	return uResult;
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
	if (uStackSize) {
		// Keep the stack size in bounds or thread creation will fail
		if (uStackSize < SCE_KERNEL_THREAD_STACK_SIZE_MIN) {
			uStackSize = SCE_KERNEL_THREAD_STACK_SIZE_MIN;
		}
		if (uStackSize > SCE_KERNEL_THREAD_STACK_SIZE_MAX) {
			uStackSize = SCE_KERNEL_THREAD_STACK_SIZE_MAX;
		}
	} else {
		// This is usually 4K, but let's double it to 8K
		uStackSize = SCE_KERNEL_THREAD_STACK_SIZE_DEFAULT * 2;
	}
	m_uStackSize = uStackSize;

	// There is a hard coded maximum name length of 32 bytes
	char ThreadName[32];
	const char* pTName;
	if (m_pName) {
		string_copy(ThreadName, 32, m_pName);
		pTName = ThreadName;
	} else {
		pTName = "Burgerlib Thread";
	}

	// Create the thread, but it's not started yet
	SceUID iThreadID = sceKernelCreateThread(pTName, Dispatcher, 0, uStackSize,
		0, SCE_KERNEL_THREAD_CPU_AFFINITY_MASK_DEFAULT, nullptr);

	// Thread created?
	if (iThreadID >= SCE_OK) {

		// Update the thread ID
		m_uThreadID = static_cast<ThreadID>(iThreadID);

		// Create record to send to the thread
		Thread* pData = this;
		SceInt32 iResult =
			sceKernelStartThread(iThreadID, sizeof(pData), &pData);

		// Started fine?
		if (iResult >= SCE_OK) {
			// All good!
			return kErrorNone;
		}
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
	// Not needed
	return kErrorNone;
}

#endif
