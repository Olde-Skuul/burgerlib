/***************************************

	Class to manage threads

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTHREAD_H__
#define __BRTHREAD_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

/* BEGIN */
namespace Burger {

enum eThreadPriority {
	/** Invalid thread priority */
	kThreadPriorityInvalid,
	/** Set thread to only execute when main thread is sleepin */
	kThreadPriorityLow,
	/** Set thread to equal priority to main game thread */
	kThreadPriorityNormal,
	/** Set thread priority to higher than main game thread */
	kThreadPriorityHigh,
	/** Set thread priority over everything */
	kThreadPriorityRealTime
};

typedef uintptr_t thread_ID_t;
typedef void(BURGER_API* TLS_shutdown_proc_t)(void* pThis);

struct thread_local_storage_entry_t {
	/** Callback to issue when this entry is released */
	TLS_shutdown_proc_t m_pShutdown;
	/** `this` pointer to pass as parameter for m_pShutdown */
	void* m_pThis;
};

struct thread_local_storage_t {
	/** Number of m_Entries in the array */
	uint32_t m_uCount;
	/** Array of thread_local_storage_entry_t */
	thread_local_storage_entry_t m_Entries[1];
};

struct thread_local_storage_record_t {
	/** thread_ID_t of the owner thread */
	thread_ID_t m_uThreadID;
	/** Pointer to array of shutdown procs */
	thread_local_storage_t* m_pThreadLocalStorage;
	/** Pointer to the next entry in the list */
	thread_local_storage_record_t* m_pNext;
};

extern thread_ID_t BURGER_API get_ThreadID(void) BURGER_NOEXCEPT;
extern eThreadPriority BURGER_API get_thread_priority(
	thread_ID_t uThreadID) BURGER_NOEXCEPT;
extern eError BURGER_API set_thread_priority(
	thread_ID_t uThreadID, eThreadPriority uThreadPriority) BURGER_NOEXCEPT;

extern uint32_t BURGER_API tls_new_index(void) BURGER_NOEXCEPT;
extern thread_local_storage_t* BURGER_API tls_data_get_fallback(
	void) BURGER_NOEXCEPT;
extern eError BURGER_API tls_data_set_fallback(
	thread_local_storage_t* pInput) BURGER_NOEXCEPT;
extern thread_local_storage_t* BURGER_API tls_data_get(void) BURGER_NOEXCEPT;
extern eError BURGER_API tls_data_set(
	thread_local_storage_t* pInput) BURGER_NOEXCEPT;
extern void* BURGER_API tls_get(uint32_t uIndex) BURGER_NOEXCEPT;
extern eError BURGER_API tls_set(uint32_t uIndex, const void* pThis,
	TLS_shutdown_proc_t pShutdown = nullptr) BURGER_NOEXCEPT;
extern void BURGER_API tls_release(void) BURGER_NOEXCEPT;

class Thread {
	BURGER_DISABLE_COPY(Thread);

public:
	enum eState {
		/** Thread has not been set up */
		kStateInvalid,
		/** Thread is in progress */
		kStateRunning,
		/** Thread has concluded */
		kStateEnded,
		/** Thread was detached */
		kStateDetached
	};

	/** Thread entry prototype */
	typedef uintptr_t(BURGER_API* function_proc_t)(void* pThis);

protected:
	/** Pointer to the thread */
	function_proc_t m_pFunction;

	/** Data pointer for the thread */
	void* m_pData;

	/** Name of the thread */
	const char* m_pName;

	/** Size of the stack allocated for the thread */
	uintptr_t m_uStackSize;

	/** Result code of the thread on exit */
	uintptr_t m_uResult;

	/** thread_ID_t assigned to this thread */
	thread_ID_t m_uThreadID;

	/** State of the thread, see /ref eState */
	uint32_t m_uState;

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE)) || \
	defined(DOXYGEN)
	/** HANDLE to the thread (Windows/Xbox only) */
	void* m_pThreadHandle;

#endif

#if defined(BURGER_WIIU) || defined(DOXYGEN)
	/** Memory for the thread's stack */
	uint8_t* m_pStack;
	/** Pointer to OSThread record */
	void* m_pOSThread;
#endif

public:
	Thread() BURGER_NOEXCEPT;
	~Thread();

	eError BURGER_API start(function_proc_t pFunction, void* pData,
		const char* pName = nullptr, uintptr_t uStackSize = 0) BURGER_NOEXCEPT;
	eError BURGER_API wait(void) BURGER_NOEXCEPT;
	eError BURGER_API detach(void) BURGER_NOEXCEPT;
	const char* BURGER_API get_name(void) const BURGER_NOEXCEPT;

	BURGER_INLINE uintptr_t get_result(void) const BURGER_NOEXCEPT
	{
		return m_uResult;
	}

	BURGER_INLINE uintptr_t get_stack_size(void) const BURGER_NOEXCEPT
	{
		return m_uStackSize;
	}

	BURGER_INLINE uint_t is_initialized(void) const BURGER_NOEXCEPT
	{
		return m_uState != kStateInvalid;
	}

	static void BURGER_API run(void* pThis) BURGER_NOEXCEPT;

protected:
	eError BURGER_API platform_start(void) BURGER_NOEXCEPT;
	eError BURGER_API platform_after_start(void) BURGER_NOEXCEPT;
	eError BURGER_API platform_detach(void) BURGER_NOEXCEPT;
};
}
/* END */

#endif
