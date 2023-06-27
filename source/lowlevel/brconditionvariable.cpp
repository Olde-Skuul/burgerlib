/***************************************

	Class to handle conditional variables

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brconditionvariable.h"

/*! ************************************

	\class Burger::ConditionVariable
	\brief Class for creating a condition variable

	In a multiprocessor system, it's sometimes necessary to have a thread wait
	until an external condition has been satisfied. Once this class is created,
	the consumer thread will call Wait() with a locked Mutex and a
	timeout and it will halt until time has expired or another thread sends a
	signal to this class to alert the waiting thread that the stars are finally
	right and great Cthulhu can rise again.

	Further reading
	http://en.wikipedia.org/wiki/Condition_variable#Condition_variables

	\sa Burger::Mutex, Burger::Semaphore and Burger::Thread

***************************************/

/*! ************************************

	\brief Initialize a condition signaler

	\sa ~ConditionVariable()

***************************************/

#if !(defined(BURGER_XBOXONE) || defined(BURGER_PS4) || defined(BURGER_PS5) || \
	defined(BURGER_SWITCH) || defined(BURGER_WINDOWS) || \
	defined(BURGER_UNIX)) || \
	defined(DOXYGEN)
Burger::ConditionVariable::ConditionVariable() BURGER_NOEXCEPT
	: m_Mutex(),
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

	If a thread is waiting for a signal, send a signal to the thread so it may
	continue execution. If no thread is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa broadcast() or wait(Mutex *,uint_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::signal(
	void) BURGER_NOEXCEPT
{
	m_Mutex.lock();
	// Is there anyone waiting for a signal?
	if (m_uWaiting > m_uSignals) {
		// Add to the signal count (Wait() will decrement)
		++m_uSignals;
		// Release a waiting thread
		m_WaitSemaphore.signal();
		// Unlock the data to allow Wait() to continue
		m_Mutex.unlock();
		// Increase the signals semaphore to match
		m_SignalsSemaphore.wait_for_signal(UINT32_MAX);
	} else {
		// No one was waiting, discard
		m_Mutex.unlock();
	}
	return kErrorNone;
}

/*! ************************************

	\brief Signal all waiting threads

	If any threads are waiting for a signal, send a signal to all of them so
	they all may continue execution. If no thread is waiting, do nothing.

	\return Zero on success, non-zero on error.
	\sa signal() or wait(Mutex *,uint32_t)

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::broadcast(
	void) BURGER_NOEXCEPT
{
	// Lock internal data
	m_Mutex.lock();
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
			m_WaitSemaphore.signal();
		} while (--uCount2);
		// The act of unlocking will fire all the released threads
		m_Mutex.unlock();

		// Reacquire the thread count to restore the wait count to
		// match m_uWaiting (Note that other threads could be modifying this
		// value, however, Wait() is updating this value, so
		// from our point of view, it must be restored)

		do {
			m_WaitSemaphore.wait_for_signal();
		} while (--uCount);
	} else {
		// Get out, nothing to see here
		m_Mutex.unlock();
	}
	return kErrorNone;
}

/*! ************************************

	\brief Wait for a signal (With timeout)

	Halt the thread until a signal is received or if a timeout has elapsed. If
	any threads are waiting for a signal, send a signal to all of them so they
	all may continue execution. If no thread is waiting, do nothing.

	\param pMutex Pointer to a Mutex to use with this object
	\param uMilliseconds UINT32_MAX to wait forever, 0 for non-blocking

	\return Zero on success, non-zero on error.
	\sa signal() or broadcast()

***************************************/

Burger::eError BURGER_API Burger::ConditionVariable::wait(
	Mutex* pMutex, uint32_t uMilliseconds) BURGER_NOEXCEPT
{
	// A thread is waiting
	m_Mutex.lock();
	++m_uWaiting;
	m_Mutex.unlock();

	// Unlock the thread's lock
	pMutex->unlock();
	// Wait for a signal (With timeout)
	const eError uResult = m_WaitSemaphore.wait_for_signal(uMilliseconds);

	// It returned!
	m_Mutex.lock();
	// Was there a signal from Signal() or Broadcast()?
	if (m_uSignals) {
		// Was there a timeout?
		if (uResult == kErrorTimeout) {
			// Force an obtaining of a resource
			m_WaitSemaphore.wait_for_signal();
		}
		// Mark this signal as processed
		m_SignalsSemaphore.signal();
		--m_uSignals;
	}
	// No longer waiting
	--m_uWaiting;
	m_Mutex.unlock();
	// Relock the thread's lock
	pMutex->lock();
	return uResult;
}

#endif
