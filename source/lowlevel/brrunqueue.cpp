/***************************************

	Run Queue execution handler

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrunqueue.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::RunQueue
	\brief Simple manager of a list of function pointers.

	RunQueue will hold a list of simple function pointers and call them when the
	Call() member is invoked. It's useful for creating a list of functions to be
	called in the background on a demand basis, such as polling tasks and game
	logic objects.

	Each function is of a type of RunQueue::CallbackProc

	\note Due to the nature of memory use, the copying of this class is
	forbidden.

	\sa \ref RunQueueEntry

***************************************/

/*! ************************************

	\typedef Burger::RunQueue::CallbackProc
	\brief Function pointer type for callbacks.

	When the Call() function is invoked, all functions will be called using the
	type of RunQueue::eReturnCode (\ref BURGER_API *)(void *) with a single
	parameter that will have either a user supplied void * or a \ref nullptr if
	none was supplied.

***************************************/

/*! ************************************

	\class Burger::RunQueue::RunQueueEntry
	\brief Function pointer entry

	The RunQueue manages a list of these class entries that hold the callback
	pointers for every callback entry.

	They are created with a call to RunQueue::Add().

	To dispose this class, call delete_object()

	\sa \ref RunQueueEntry

***************************************/

/*! ************************************

	\fn Burger::RunQueue::RunQueueEntry::RunQueueEntry(
		CallbackProc pCallBack, CallbackProc pShutdownCallback,
		void *pData, uint_t uPriority)

	\brief RunQueueEntry constructor.

	\param pCallBack Pointer to a function that is called when the queue is
		polled
	\param pShutdownCallback Pointer to a function that is called when this
		entry is disposed of
	\param pData Pointer to function data.
	\param uPriority Priority value for location in the queue

	\sa RunQueue

***************************************/

/*! ************************************

	\brief RunQueueEntry destructor.

	When a RunQueueEntry is destroyed, if there is a shutdown procedure defined,
	call the function with the application supplied data pointer before deleting
	the entry.

	This function does not attempt to delete the application data, it's the
	shutdown function's responsibility to dispose of any data that it's
	managing.

	\sa RunQueue

***************************************/

Burger::RunQueue::RunQueueEntry::~RunQueueEntry()
{
	if (m_pShutdownCallback) {
		m_pShutdownCallback(m_pData);
	}
	m_pData = nullptr;
	m_pShutdownCallback = nullptr;
	m_pCallBack = nullptr;
}

/*! ************************************

	\fn uint_t Burger::RunQueue::RunQueueEntry::GetPriority(void) const
	\brief Get the priority value

	\return The priority of this entry
	\sa RunQueue

***************************************/

/*! ************************************

	\fn Burger::RunQueue::RunQueue()
	\brief RunQueue constructor.

	Initialize the class to contain no list. This is inline due to its only
	zeroing out members.

***************************************/

/*! ************************************

	\brief RunQueue destructor.

	This will call Clear() and dispose of all the memory allocated. No other
	functions will be called.

	\sa Clear().

***************************************/

Burger::RunQueue::~RunQueue()
{
	Clear();
}

/*! ************************************

	\brief Invoke every function stored within the list.

	Traverse the array of functions to call and invoke each and every one of
	them. The functions may add new entries to the list or remove themselves if
	they need to. If Add() is called, execution of the new entry will be
	deferred until the next time Call() is invoked.

	The function called is of type RunQueue::CallbackProc.

	\note This function can take a significant amount of CPU time if the
	functions invoked are very slow. This function runs with minimal overhead.

	\note There is a recursion checker, this function will do nothing if it is
	called by a RunQueue::CallbackProc proc.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t), Remove(CallbackProc,void*)
		or RemoveAll(CallbackProc);

***************************************/

void BURGER_API Burger::RunQueue::Call(void) BURGER_NOEXCEPT
{
	if (!m_bRecurse) {

		// Get the master handle
		RunQueueEntry* pWork = static_cast<RunQueueEntry*>(m_Entries.get_next());

		// Any in the list?
		if (pWork != &m_Entries) {
			m_bRecurse = TRUE; // Prevent recursion
			do {
				// Call the function
				const eReturnCode uCode = pWork->m_pCallBack(pWork->m_pData);
				// Abort execution
				if (uCode == ABORT) {
					break;
				}

				// Next in the chain
				RunQueueEntry* pNext =
					static_cast<RunQueueEntry*>(pWork->get_next());

				// Dispose of this entry
				if (uCode == DISPOSE) {
					// Remove this entry
					delete_object(pWork);
				}
				pWork = pNext;             // Next one
			} while (pWork != &m_Entries); // Any more?
			m_bRecurse = FALSE;            // I'm done, so release the lock
		}
	}
}

/*! ************************************

	\brief Add a function to the list.

	Given a function pointer and a pointer to data to pass to the function
	pointer, add this entry to the list of functions that are to be called with
	each call to Call(). The pointer pData is not used by this class and it's
	solely used as a parameter when the function pointer is called. Priority
	values are used to sort the function pointers to call them in a desired
	order. The default is RunQueue::MEDIUMPRIORITY. There is no sorting with
	like numbered priorities.

	\param pProc Pointer to a function of type RunQueue::CallbackProc.
	\param pShutdown Pointer to a function of type RunQueue::CallbackProc that
		is called when this entry is disposed of
	\param pData Pointer to be passed to the function when called.
	\param uPriority Priority value to determine order of calling. Higher values
		get called first.

	\return Returns a pointer to the created \ref RunQueueEntry if successful,
		\ref nullptr if out of memory or if the function pointer was
		\ref nullptr.

	\sa Remove(CallbackProc,void*).

***************************************/

Burger::RunQueue::RunQueueEntry* BURGER_API Burger::RunQueue::Add(
	CallbackProc pProc, CallbackProc pShutdown, void* pData,
	uint_t uPriority) BURGER_NOEXCEPT
{
	// Assume failure
	RunQueueEntry* pResult = nullptr;
	if (pProc) {
		pResult = new (allocate_memory(sizeof(RunQueueEntry)))
			RunQueueEntry(pProc, pShutdown, pData, uPriority);
		if (pResult) {
			RunQueueEntry* pNext =
				static_cast<RunQueueEntry*>(m_Entries.get_next());
			// No entries? No need to test priorities
			if (pNext == &m_Entries) {
				m_Entries.insert_after(pResult); // Link it in
			} else {
				// Traverse the linked list until the end is
				// reached, or an entry with lower priority shows up
				RunQueueEntry* pWork;
				do {
					pWork = pNext;
					pNext = static_cast<RunQueueEntry*>(pNext->get_next());
				} while (
					(pNext != &m_Entries) && (pWork->m_uPriority > uPriority));
				pNext->insert_before(pResult);
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.

	\param pProc Pointer to the function.
	\return \ref RunQueueEntry pointer if the function pointer was found,
		\ref nullptr if not.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t), Clear() or Call().

***************************************/

Burger::RunQueue::RunQueueEntry* BURGER_API Burger::RunQueue::Find(
	CallbackProc pProc) const BURGER_NOEXCEPT
{
	RunQueueEntry* pResult = nullptr;
	// Get the master handle
	RunQueueEntry* pWork = static_cast<RunQueueEntry*>(m_Entries.get_next());
	if (pWork != &m_Entries) { // Is it valid?
		do {
			if (pWork->m_pCallBack == pProc) { // Match?
				pResult = pWork;               // I found it
				break;
			}
			pWork = static_cast<RunQueueEntry*>(
				pWork->get_next());     // Follow the list
		} while (pWork != &m_Entries); // Still more?
	}
	return pResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.

	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.
	\return \ref RunQueueEntry pointer if the Function/Data pair was found, \ref
		nullptr if not.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t), Find(CallbackProc) const or
		Call().

***************************************/

Burger::RunQueue::RunQueueEntry* BURGER_API Burger::RunQueue::Find(
	CallbackProc pProc, void* pData) const BURGER_NOEXCEPT
{
	RunQueueEntry* pResult = nullptr;
	// Get the master handle
	RunQueueEntry* pWork = static_cast<RunQueueEntry*>(m_Entries.get_next());
	if (pWork != &m_Entries) { // Is it valid?
		do {
			// Match?
			if ((pWork->m_pCallBack == pProc) && (pWork->m_pData == pData)) {
				pResult = pWork; // I found it
				break;
			}
			pWork = static_cast<RunQueueEntry*>(
				pWork->get_next());     // Follow the list
		} while (pWork != &m_Entries); // Still more?
	}
	return pResult;
}

/*! ************************************

	\brief Remove all entries to a function from the list.

	Given a function pointer and a pointer to data to pass to the function
	pointer, search the list to see if there is a match. If a match is found,
	remove the entry and return \ref TRUE, saying I found it.

	\note This function should not be invoked while Call() is executing.
	It may cause a list traversal error.

	\param pProc Pointer to the function.

	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t), Clear() or Call().

***************************************/

uint_t BURGER_API Burger::RunQueue::RemoveAll(
	CallbackProc pProc) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	RunQueueEntry* pWork = static_cast<RunQueueEntry*>(
		m_Entries.get_next());  // Get the master handle
	if (pWork != &m_Entries) { // Is it valid?
		do {
			// Get the forward link (For unlinking)
			RunQueueEntry* pNext =
				static_cast<RunQueueEntry*>(pWork->get_next());

			// Match?
			if (pWork->m_pCallBack == pProc) {
				delete_object(pWork);  // Dispose of the current record
				uResult = TRUE; // I deleted it
			}
			pWork = pNext;             // Follow the list
		} while (pWork != &m_Entries); // Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Remove a function from the list.

	Given a function pointer and a pointer to data to pass to the function
	pointer, search the list to see if there is a match. If a match is found,
	remove the entry and return \ref TRUE, saying it's been found.

	\note Functions can remove themselves from the list at execution time, but
	you cannot remove a different function during your time quantum since
	it might break the linked list traversal. If you wish to remove other
	entries, you must defer this action.

	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.

	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t), Clear() or Call().

***************************************/

uint_t BURGER_API Burger::RunQueue::Remove(
	CallbackProc pProc, void* pData) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	// Get the master handle
	RunQueueEntry* pWork = static_cast<RunQueueEntry*>(m_Entries.get_next());

	// Is it valid?
	if (pWork != &m_Entries) {
		do {
			// Match?
			if ((pWork->m_pCallBack == pProc) && (pWork->m_pData == pData)) {
				delete_object(pWork);  // Dispose of the current record
				uResult = TRUE; // I deleted it
				break;
			}
			// Follow the list
			pWork = static_cast<RunQueueEntry*>(pWork->get_next());
			// Still more?
		} while (pWork != &m_Entries);
	}
	return uResult;
}

/*! ************************************

	\brief Release all function entries.

	Release all of the allocated memory and discard all function and data
	pointers.

	\note No functions will be called. Only the list will be destroyed.

	\sa Add(CallbackProc,CallbackProc,void*,uint_t) or
		Remove(CallbackProc,void*).

***************************************/

void BURGER_API Burger::RunQueue::Clear(void) BURGER_NOEXCEPT
{
	// Get the master handle
	RunQueueEntry* pWork = static_cast<RunQueueEntry*>(m_Entries.get_next());

	// Is it valid?
	if (pWork != &m_Entries) {
		do {
			// Get the forward link (For unlinking)
			RunQueueEntry* pNext =
				static_cast<RunQueueEntry*>(pWork->get_next());

			// Dispose of the current record
			delete_object(pWork);

			// Follow the list
			pWork = pNext;

			// Still more?
		} while (pWork != &m_Entries);
	}
}
