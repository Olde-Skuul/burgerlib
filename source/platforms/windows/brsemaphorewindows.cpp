/***************************************

	Class for semaphores, Windows version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsemaphore.h"

#if defined(BURGER_WINDOWS)
#include "brintrinsics.h"
#include "brtick.h"
#include "brvisualstudio.h"
#include "brwatcom.h"
#include "win_synchapi.h"
#include "win_version.h"
#include "win_windows.h"

/***************************************

	\brief Initialize a semaphore

	Query the operating system for a semaphore and initialize it to the initial
	value.

	\param uCount Initial number of resources available (0 means a empty
		semaphore)

	\sa ~Semaphore()

***************************************/

Burger::Semaphore::Semaphore(uint32_t uCount) BURGER_NOEXCEPT
	: m_pSemaphore(nullptr),
	  m_bUsingWakeAddress(FALSE),
	  m_uCount(uCount)
{
	if (Win32::is_8_or_higher()) {
		m_bUsingWakeAddress = TRUE;
	} else {
		// Create an artificial limit of 32K semaphores
		m_pSemaphore = CreateSemaphoreW(
			nullptr, static_cast<long>(uCount), 32768, nullptr);
	}
}

/***************************************

	\brief Shut down a semaphore

	Release any operating system resources allocated in the creation of the
	semaphore.

	\note Care should be exercised in ensuring that all threads are are waiting
	on semaphores have been shutdown down already.

	\sa Semaphore(uint32_t)

***************************************/

Burger::Semaphore::~Semaphore()
{
	HANDLE hSemaphore = m_pSemaphore;
	if (hSemaphore) {
		CloseHandle(hSemaphore);
		m_pSemaphore = nullptr;
	}
	m_bUsingWakeAddress = FALSE;
	m_uCount = 0;
}

/***************************************

	\brief Signal a semaphore

	When a resource is made available, signal that one is available with this
	call. This function will increment the count, and may release a waiting
	thread.

	\return Zero on success, nonzero in the case of a semaphore failure
	\sa wait_for_signal(uint32_t)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::signal(void) BURGER_NOEXCEPT
{
	eError uResult;

	// Using XP-Windows 7 APIs?
	if (!m_bUsingWakeAddress) {

		uResult = kErrorCantUnlock;
		HANDLE hSemaphore = m_pSemaphore;
		if (hSemaphore) {

			// Release the count immediately, because it's
			// possible that another thread, waiting for this semaphore,
			// can execute before the call to ReleaseSemaphore()
			// returns

			_InterlockedIncrement(reinterpret_cast<volatile long*>(&m_uCount));
			if (ReleaseSemaphore(hSemaphore, 1, nullptr) == FALSE) {

				// Error!!! Undo the _InterlockedIncrement()
				_InterlockedDecrement(
					reinterpret_cast<volatile long*>(&m_uCount));
			} else {

				// A-Okay!
				uResult = kErrorNone;
			}
		}
	} else {

		// Release the count immediately, because it's
		// possible that another thread, waiting for this semaphore,
		// can execute before the call to WakeByAddressSingle()
		// returns

		_InterlockedIncrement(reinterpret_cast<volatile long*>(&m_uCount));
		Win32::WakeByAddressSingle(
			static_cast<void*>(const_cast<uint32_t*>(&m_uCount)));
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	\brief Acquire a lock on a semaphore resource with a timeout

	If the semaphore's resource count has not gone to zero or less, decrement
	the count and immediately return. Otherwise, block until another thread
	releases the semaphore or the time in milliseconds has elapsed. If the
	timeout is zero, return immediately with a non-zero error code.

	\param uMilliseconds Number of milliseconds to wait for the resource, 0
		means no wait, \ref UINT32_MAX means infinite

	\return Zero on success, One on a timeout, and non Zero or One in the case
		of a semaphore failure

	\sa acquire(void) or release(void)

***************************************/

Burger::eError BURGER_API Burger::Semaphore::wait_for_signal(
	uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	eError uResult;

	// Using XP-Windows 7 APIs?
	if (!m_bUsingWakeAddress) {

		// Assume failure
		uResult = kErrorCantLock;
		HANDLE hSemaphore = m_pSemaphore;
		if (hSemaphore) {

			// Wait for a signal
			// If dwMilliseconds is zero, then it will immediately return
			// If dwMilliseconds is UINT32_MAX, it will wait FOREVER
			const DWORD uWait = WaitForSingleObject(hSemaphore, uMilliseconds);
			if (uWait == WAIT_OBJECT_0) {

				// Got the signal. Decrement the count
				_InterlockedDecrement(
					reinterpret_cast<volatile long*>(&m_uCount));
				uResult = kErrorNone;
			} else if (uWait == WAIT_TIMEOUT) {

				// Timeout gets a special error
				uResult = kErrorTimeout;
			}
		}
	} else {
		// Assume failure
		uResult = kErrorTimeout;

		uint32_t uCount = m_uCount;

		// Perform try_wait?
		if (!uMilliseconds) {

			// Are there instances available?
			if (uCount) {

				// Try to remove a signal
				if (_InterlockedCompareExchange(
						reinterpret_cast<volatile long*>(&m_uCount),
						static_cast<long>(uCount - 1),
						static_cast<long>(uCount)) ==
					static_cast<long>(uCount)) {

					// It was decremented
					uResult = kErrorNone;
				}
			}

			// Stay forever?
		} else if (uMilliseconds == UINT32_MAX) {

			// Assume no error
			uResult = kErrorNone;
			for (;;) {

				// No signals? Wait for one.
				if (!uCount) {
					// Wait until a resource is signaled
					do {

						// Sleep until signal() is called
						if (!Win32::WaitOnAddress(
								&m_uCount, &uCount, sizeof(uCount), INFINITE)) {

							// Error?
							uResult = kErrorCantLock;
							break;
						}

						// Get the new count
						uCount = m_uCount;

						// Still no signals?
					} while (!uCount);
				}

				// Was there an error? If so, exit
				if (uResult) {
					break;
				}

				// Try to remove a signal
				if (_InterlockedCompareExchange(
						reinterpret_cast<volatile long*>(&m_uCount),
						static_cast<long>(uCount - 1),
						static_cast<long>(uCount)) ==
					static_cast<long>(uCount)) {

					// Exit with no error
					break;
				}

				// Stay a while, stay forever!
				uCount = m_uCount;
			}
		} else {

			// Use a timeout, get a time mark
			uint32_t uMark = Tick::read_ms();

			// Assume no error
			uResult = kErrorNone;
			for (;;) {

				// No signals? Wait for one.
				if (!uCount) {
					// Wait until a resource is signaled
					do {

						// Run out of time?
						if (!uMilliseconds) {
							uResult = kErrorTimeout;
							break;
						}

						// Sleep until signal() is called
						if (!Win32::WaitOnAddress(&m_uCount, &uCount,
								sizeof(uCount), uMilliseconds)) {

							// Did it abort due to a failure or a timeout?
							if (GetLastError() == ERROR_TIMEOUT) {
								// It timed out
								uResult = kErrorTimeout;
							} else {
								// Error?
								uResult = kErrorCantLock;
							}
							break;
						}

						// Removed the amount of time that has elapsed
						const uint32_t uNewMark = Tick::read_ms();
						uint32_t uElapsed = uNewMark - uMark;
						uMark = uNewMark;

						// Handle timing overflow
						if (uElapsed >= uMilliseconds) {
							uElapsed = uMilliseconds;
						}
						uMilliseconds -= uElapsed;

						// Get the new count
						uCount = m_uCount;

						// Still no signals?
					} while (!uCount);
				}

				// Was there an error? If so, exit
				if (uResult) {
					break;
				}

				// Try to remove a signal
				if (_InterlockedCompareExchange(
						reinterpret_cast<volatile long*>(&m_uCount),
						static_cast<long>(uCount - 1),
						static_cast<long>(uCount)) ==
					static_cast<long>(uCount)) {

					// Exit with no error
					break;
				}

				// Stay a while, stay forever!
				uCount = m_uCount;
			}
		}
	}
	return uResult;
}

#endif
