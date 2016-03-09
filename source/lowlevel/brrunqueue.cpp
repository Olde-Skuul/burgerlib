/***************************************

	Run Queue execution handler

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrunqueue.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::RunQueue
	\brief Simple manager of a list of function pointers.

	RunQueue will hold a list of simple function pointers
	and call them when the Call() member is invoked. It's useful
	for creating a list of functions to be called in the background
	on a demand basis, such as polling tasks and game logic objects.
	
	Each function is of a type of RunQueue::CallbackProc
	
	\note Due to the nature of memory use, the copying of this
	class is forbidden.
	
***************************************/

/*! ************************************

	\typedef Burger::RunQueue::CallbackProc
	\brief Function pointer type for callbacks.

	When the Call() function is invoked, all functions will be called
	using the type of RunQueue::eReturnCode (\ref BURGER_API *)(void *)
	with a single parameter that will have either
	a user supplied void * or a \ref NULL if none was supplied.
	
***************************************/


/*! ************************************

	\fn Burger::RunQueue::RunQueue()
	\brief RunQueue constructor.

	Initialize the class to contain no list. This is
	inlined due to its only zeroing out members.
	
***************************************/


/*! ************************************

	\brief RunQueue destructor.

	This will call Clear() and dispose of all the memory
	allocated. No other functions will be called.
	
	\sa Clear().
	
***************************************/

Burger::RunQueue::~RunQueue()
{
	Clear();
}

/*! ************************************

	\brief Invoke every function stored within the list.

	Traverse the array of functions to call and invoke each
	and every one of them. The functions may add new entries
	to the list or remove themselves if they need to. If Add()
	is called, execution of the new entry will be deferred until the next time
	Call() is invoked.
	
	The function called is of type RunQueue::CallbackProc.
	
	\note This function can take a significant amount
	of CPU time if the functions invoked are very slow.
	This function runs with minimal overhead.
	
	\note There is a recursion checker, this function will
	do nothing if it is called by a RunQueue::CallbackProc proc.

	\sa Add(CallbackProc,void*,Word), Remove(CallbackProc,void*) or RemoveAll(CallbackProc);

***************************************/

void BURGER_API Burger::RunQueue::Call(void)
{
	if (!m_Recurse) {
		RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
		if (pWork) { 							// Any in the list?
			m_Recurse = TRUE;					// Prevent recursion
			RunQueueEntry_t **pPrev = &m_pFirst;		// Previous entry
			do {
				// Call the function
				eReturnCode eCode = pWork->m_pProc(pWork->m_pData);

				// Next in the chain
				RunQueueEntry_t *pNext = pWork->m_pNext;

				// Abort execution
				if (eCode==ABORT) {
					break;

				// Dispose of this entry
				} else if (eCode==DISPOSE) {
					// Remove from the linked list
					pPrev[0] = pNext;
					// Remove this entry
					Free(pWork);
				} else {
					// Since this is retained, this will be the previous entry for 
					// unlinking (If needed)
					pPrev = &pWork->m_pNext;
				}
				pWork = pNext;				// Next one
			} while (pWork); 				// Any more?
			m_Recurse = FALSE;				// I'm done, so release the lock
		}
	}
}

/*! ************************************

	\brief Add a function to the list.
	
	Given a function pointer and a pointer to data to pass to the
	function pointer, add this entry to the list of functions that are to be called
	with each call to Call(). The pointer pData is not used by
	this class and it's solely used as a parameter when the function
	pointer is called. Priority values are used to sort the function pointers to call
	them in a desired order. The default is RunQueue::MEDIUMPRIORITY. There is no sorting with
	like numbered priorities.
	
	\param pProc Pointer to a function of type RunQueue::CallbackProc.
	\param pData Pointer to be passed to the function when called.
	\param uPriority Priority value to determine order of calling. Higher values get called first.

	\return Returns \ref TRUE if successful, \ref FALSE if out of memory or if the
	function pointer was \ref NULL.
	\sa Remove(CallbackProc,void*).
	
***************************************/

Word BURGER_API Burger::RunQueue::Add(CallbackProc pProc,void *pData,Word uPriority)
{
	// Assume failure
	Word uResult = FALSE;
	if (pProc) {
		RunQueueEntry_t *pNew = static_cast<RunQueueEntry_t *>(Alloc(sizeof(RunQueueEntry_t)));
		if (pNew) {
			pNew->m_pProc = pProc;		// Save the proc
			pNew->m_pData = pData;		// Save the data
			pNew->m_uPriority = uPriority;	// Priority
			RunQueueEntry_t *pNext = m_pFirst;
			// No entries? No need to test priorities
			if (!pNext) {
				pNew->m_pNext = NULL;	// Link it in
				m_pFirst = pNew;		// New master handle
			} else {
				// Traverse the linked list until the end is 
				// reached, or an entry with lower priority shows up
				RunQueueEntry_t *pWork;
				do {
					pWork = pNext;
					pNext = pNext->m_pNext;
				} while (pNext && (pWork->m_uPriority>uPriority));
				pNew->m_pNext = pNext;
				pWork->m_pNext = pNew;
				uResult = TRUE;				// Success
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.
	
	\param pProc Pointer to the function.
	\return \ref TRUE if the function pointer was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word BURGER_API Burger::RunQueue::Find(CallbackProc pProc) const
{
	Word uResult = FALSE;
	// Get the master handle
	const RunQueueEntry_t *pWork = m_pFirst;
	if (pWork) {							// Is it valid?
		do {
			if (pWork->m_pProc==pProc) {	// Match?
				uResult = TRUE;				// I found it
				break;
			}
			pWork = pWork->m_pNext;			// Follow the list
		} while (pWork);					// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.
		
	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Find(CallbackProc) const or Call().

***************************************/

Word BURGER_API Burger::RunQueue::Find(CallbackProc pProc,void *pData) const
{
	Word uResult = FALSE;
	// Get the master handle
	const RunQueueEntry_t *pWork = m_pFirst;	
	if (pWork) {							// Is it valid?
		do {
			// Match?
			if ((pWork->m_pProc==pProc) && (pWork->m_pData==pData)) {
				uResult = TRUE;			// I found it
				break;
			}
			pWork = pWork->m_pNext;				// Follow the list
		} while (pWork);				// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Remove all entries to a function from the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, remove the entry and return \ref TRUE, saying I found it.
	
	\note This function should not be invoked while Call() is executing.
	It may cause a list traversal error.
	
	\param pProc Pointer to the function.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word BURGER_API Burger::RunQueue::RemoveAll(CallbackProc pProc)
{
	Word uResult = FALSE;
	RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		RunQueueEntry_t **pPrev = &m_pFirst;	// This is where I'll store the forward link
		do {
			RunQueueEntry_t *pNext = pWork->m_pNext;		// Get the forward link (For unlinking)
			if (pWork->m_pProc==pProc) {	// Match?
				pPrev[0] = pNext;			// I'm gone!
				Free(pWork);				// Dispose of the current record
				uResult = TRUE;				// I deleted it
			} else {
				pPrev = &pWork->m_pNext;	// This is the link that will be broken
			}
			pWork = pNext;					// Follow the list
		} while (pWork);					// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Remove a function from the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, remove the entry and return \ref TRUE, saying
	it's been found.
	
	\note Functions can remove themselves from the list at execution time, but
	you cannot remove a different function during your time quantum since
	it might break the linked list traversal. If you wish to remove other entries,
	you must defer this action.
	
	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word BURGER_API Burger::RunQueue::Remove(CallbackProc pProc,void *pData)
{
	Word uResult = FALSE;
	RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		RunQueueEntry_t **pPrev = &m_pFirst;	// This is where I'll store the forward link
		do {
			RunQueueEntry_t *pNext = pWork->m_pNext;		// Get the forward link (For unlinking)
			if (pWork->m_pProc==pProc && pWork->m_pData==pData) {	// Match?
				pPrev[0] = pNext;	// I'm gone!
				Free(pWork);		// Dispose of the current record
				uResult = TRUE;			// I deleted it
				break;
			}
			pPrev = &pWork->m_pNext;	// This is the link that will be broken
			pWork = pNext;				// Follow the list
		} while (pWork);				// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Release all function entries.
	
	Release all of the allocated memory and discard
	all function and data pointers.
	
	\note No functions will be called. Only the list will be destroyed.	

	\sa Add(CallbackProc,void*,Word) or Remove(CallbackProc,void*).
	
***************************************/

void BURGER_API Burger::RunQueue::Clear(void)
{
	RunQueueEntry_t *pWork = m_pFirst;	// Get the master handle
	if (pWork) {						// Is it valid?
		do {
			// Get the forward link (For unlinking)
			RunQueueEntry_t *pNext = pWork->m_pNext;	
			Free(pWork);				// Dispose of the current record
			pWork = pNext;				// Follow the list
		} while (pWork);				// Still more?
		m_pFirst = pWork;				// Erase with NULL without loading a constant
	}
}
