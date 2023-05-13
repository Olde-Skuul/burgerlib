/***************************************

    Last in, first out, singly linked list queue

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brlastinfirstout.h"

/*! ************************************

	\class Burger::ForwardLink
	\brief Singly linked list pointer

	For classes that maintain singly linked lists,
	this class holds a single pointer to the next
	entry in the list or \ref NULL to denote the
	end of the list. For classes that work with
	circular lists, there will be no \ref NULL
	to the last entry

	\sa LastInFirstOut or MPLastInFirstOut

***************************************/

/*! ************************************

	\fn Burger::ForwardLink::ForwardLink()
	\brief Constructor with \ref NULL

	Default constructor that sets the next pointer
	to \ref NULL
	
	\sa ForwardLink(ForwardLink *)

***************************************/

/*! ************************************

	\fn Burger::ForwardLink::ForwardLink(ForwardLink *pNext)
	\brief Constructor with a supplied next pointer.

	Default constructor that sets the next pointer
	to pNext

	\param pNext Pointer to the ForwardLink that follows
		this class instance in the linked list
	
	\sa ForwardLink()

***************************************/

/*! ************************************

	\fn void Burger::ForwardLink::Clear(void)
	\brief Set the pointer to \ref NULL
	
	\sa SetNext(ForwardLink *)

***************************************/

/*! ************************************

	\fn void Burger::ForwardLink::GetNext(void) const
	\brief Get the pointer to the next entry in the linked list
	
	\return Pointer to the next entry in the linked list
	\sa SetNext(ForwardLink *)

***************************************/

/*! ************************************

	\fn void Burger::ForwardLink::SetNext(ForwardLink *pNext)
	\brief Set the pointer to pNext
	
	\param pNext Pointer to the ForwardLink that follows
		this class instance in the linked list

	\sa GetNext(void) const or Clear(void)

***************************************/


/*! ************************************

	\brief Reverse a singly linked list

	Traverse a singly linked list and reverse the linkage
	so that the order of the entries are in the
	opposite order.

	\param pRoot Pointer to the ForwardLink that is the head of the list.
	\return Pointer to the last entry of the linked list that is the new head of the list.

	\sa ForwardLink

***************************************/

Burger::ForwardLink * BURGER_API Burger::ReverseList(ForwardLink *pRoot)
{
	// No entries in the list?
	if (pRoot) {
		// There's at least one entry,
		// the next entry will be the new previous
		// entry

		ForwardLink *pPrev = pRoot->GetNext();
		// The next entry is the last one (NULL is terminator)
		ForwardLink *pNext = NULL;
		if (pPrev) {
			do {
				// pRoot will have the new next pointer
				pRoot->SetNext(pNext);

				// The current root will be the new next
				pNext = pRoot;

				// The root is previous next pointer
				pRoot = pPrev;

				// Step to the next pointer (Which is the new previous pointer)
				pPrev = pPrev->GetNext();

				// Still going?
			} while (pPrev);
		}

		// Set the last previous entry as the new 
		// next entry from the root
		pRoot->SetNext(pNext);
	}
	return pRoot;
}


/*! ************************************

	\class Burger::LastInFirstOut
	\brief Class that manages a last in first out singly linked list

	This class holds a linked list that is treated as a
	last in first out stack.

	This class is not reentrant, use \ref MPLastInFirstOut 
	instead.

	\sa MPLastInFirstOut

***************************************/

/*! ************************************

	\fn Burger::LastInFirstOut::LastInFirstOut()
	\brief Default constructor

	Construct the class with an empty list
	\sa LastInFirstOut(ForwardLink *)

***************************************/

/*! ************************************

	\fn Burger::LastInFirstOut::LastInFirstOut(ForwardLink *pRoot)
	\brief Default constructor

	Construct the class with an supplied list
	\param pRoot Pointer to the linked list to initialize this class with

	\sa LastInFirstOut()

***************************************/

/*! ************************************

	\fn void Burger::LastInFirstOut::Clear(void)
	\brief Discard the linked list

	Set the root pointer to \ref NULL. No deallocations are
	performed.

	\sa GetRoot(void) const

***************************************/

/*! ************************************

	\fn Burger::LastInFirstOut::IsEmpty(void) const
	\brief Default constructor

	\return \ref TRUE if there are entries in the linked list.

	\sa Clear(void)

***************************************/

/*! ************************************

	\fn Burger::ForwardLink * Burger::LastInFirstOut::GetRoot(void) const
	\brief Get the head of the list.

	Return the head of the linked list for manual traversal.

	\return \ref NULL if there is nothing in the list, 
		or a valid pointer to the head of the list.

	\sa Clear(void)

***************************************/

/*! ************************************

	\brief Count the number of entries in this linked list

	Traverse the linked list until the end is reached
	and return the number of entries found.

	\return The number of elements in this linked list, can be zero.

	\sa IsEmpty(void) const

***************************************/

uintptr_t BURGER_API Burger::LastInFirstOut::Size(void) const
{

	// Get the first entry
	const ForwardLink *pWork = m_pRoot;

	uintptr_t uCount = 0;
	if (pWork) {
		// Follow the list to get the number of entries
		do {
			++uCount;
			pWork = pWork->GetNext();
		} while (pWork);
	}
	return uCount;
}

/*! ************************************

	\brief Insert an entry to the head of the list

	Insert a linked list into the head of the stored
	linked list.

	\param pNewRoot Pointer to the new head of the list 

	\sa AddLast(ForwardLink *) or RemoveFirst(void)

***************************************/

void BURGER_API Burger::LastInFirstOut::AddFirst(ForwardLink *pNewRoot)
{ 
	// Get the old root
	ForwardLink *pOldRoot = m_pRoot;

	// Set the new root
	m_pRoot = pNewRoot;

	// Traverse the application supplied list to the end
	ForwardLink *pNext = pNewRoot->GetNext();
	if (pNext) {
		do {
			pNewRoot = pNext;
			pNext = pNewRoot->GetNext();
		} while (pNext);
	}

	// Append the old list to the new one
	pNewRoot->SetNext(pOldRoot);	
}

/*! ************************************

	\brief Insert an entry to the end of the list

	Insert a linked list into the tail of the stored
	linked list.

	\param pNewTail Pointer to the list that will be appended
		to the current linked list

	\sa AddFirst(ForwardLink *)

***************************************/

void BURGER_API Burger::LastInFirstOut::AddLast(ForwardLink* pNewTail)
{
	// Get the old root
	ForwardLink *pOldRoot = m_pRoot;

	// Only in the case where there's no list,
	// do the easy thing and just set the supplied list
	// as the new root
	if (!pOldRoot) {
		m_pRoot = pNewTail;

	} else {
		// Traverse the application supplied list to the end
		ForwardLink *pNext = pOldRoot->GetNext();
		if (pNext) {
			do {
				pOldRoot = pNext;
				pNext = pOldRoot->GetNext();
			} while (pNext);
		}
		// Append the list to the end
		pOldRoot->SetNext(pNewTail);
	}
}

/*! ************************************

	\brief Remove an entry from the head of the list

	Remove the pointer from the root of the linked list. Sets
	the new root with the second entry of the linked list
	if there is a second entry.

	\return The unlinked head pointer or \ref NULL if the list was empty.

	\sa AddFirst(ForwardLink *)

***************************************/

Burger::ForwardLink* BURGER_API Burger::LastInFirstOut::RemoveFirst(void)
{
	// Was there an entry in the linked list?
	ForwardLink *pRoot = m_pRoot;
	if (pRoot) {
		// Set the new head of the list
		m_pRoot = pRoot->GetNext();
		// Unlink the pointer that's being returned
		pRoot->Clear();
	}
	return pRoot;
}

/*! ************************************

	\brief Assigns ownership of the linked list.

	Returns the head pointer of the linked list
	and sets the linked list in this class to being
	empty. The calling function is assigned ownership
	of the entire list.

	\return The pointer to the entire linked list or \ref NULL if the list was empty.

	\sa Clear(void)

***************************************/

Burger::ForwardLink * BURGER_API Burger::LastInFirstOut::TakeList(void)
{	
	// Get the list pointer
	ForwardLink *pRoot = m_pRoot;
	// Clear out the internal list
	m_pRoot = NULL;
	// Return the list
	return pRoot;
}

/*! ************************************

	\brief Remove an item from the linked list.

	Traverse the linked list until the pointer is found
	and then unlink it from the list. The pointer is
	not deleted, just unlinked.

	\param pEntry Item to remove from the linked list
	\return Zero on success, kErrorItemNotFound if the pointer wasn't in the list

***************************************/

Burger::eError BURGER_API Burger::LastInFirstOut::Remove(ForwardLink *pEntry)
{
	// Assume error
	eError uResult = kErrorItemNotFound;

	// Start at the beginning of the list
	ForwardLink *pPrev = m_pRoot;

	// Any list?
	if (pPrev && pEntry) {
		// Get the next link
		ForwardLink *pWork = pPrev->GetNext();
		// If the root is the match, set the new head
		if (pPrev==pEntry) {
			m_pRoot = pWork;
			// Kill the forward link
			pPrev->Clear();
			uResult = kErrorNone;

		} else if (pWork) {

			// Follow the list
			do {

				// Match?
				if (pWork==pEntry) {

					// Set the new next pointer to the next
					// entry to remove from the list
					pPrev->SetNext(pWork->GetNext());

					// Unlock this entry
					pWork->Clear();

					// Match!
					uResult = kErrorNone;
					break;
				}
				// New previous entry
				pPrev = pWork;
				// Next entry
				pWork = pWork->GetNext();
			} while (pWork);
		}
	}
	// Return the error
	return uResult;
}

/*! ************************************

	\fn void Burger::LastInFirstOut::ReverseList(void)
	\brief Reverse the singly linked list

	Traverse the singly linked list and reverse the linkage
	so that the order of the entries are in the
	opposite order.

	\sa Burger::ReverseList(ForwardLink *)

***************************************/






/*! ************************************

	\class Burger::MPLastInFirstOut
	\brief Thread safe class that manages a last in first out singly linked list

	This class holds a linked list that is treated as a
	last in first out stack with a \ref CriticalSection to ensure
	thread safety.

	\sa LastInFirstOut

***************************************/

/*! ************************************

	\brief Default constructor

	Construct the class with an empty list
	\sa MPLastInFirstOut(ForwardLink *)

***************************************/

Burger::MPLastInFirstOut::MPLastInFirstOut() :
	LastInFirstOut(),
	m_Lock()
{
}

/*! ************************************

	\brief Default constructor

	Construct the class with an supplied list
	\param pRoot Pointer to the linked list to initialize this class with

	\sa MPLastInFirstOut()

***************************************/

Burger::MPLastInFirstOut::MPLastInFirstOut(ForwardLink *pRoot) :
	LastInFirstOut(pRoot),
	m_Lock()
{
}

/*! ************************************

	\brief Discard the linked list

	Set the root pointer to \ref NULL. No deallocations are
	performed.

***************************************/

void BURGER_API Burger::MPLastInFirstOut::Clear(void)
{
	m_Lock.lock();
	m_pRoot = NULL;
	m_Lock.unlock();
}

/*! ************************************

	\fn Burger::MPLastInFirstOut::IsEmpty(void) const
	\brief Default constructor

	\return \ref TRUE if there are entries in the linked list.

	\sa Clear(void)

***************************************/

/*! ************************************

	\brief Count the number of entries in this linked list

	Traverse the linked list until the end is reached
	and return the number of entries found.

	\return The number of elements in this linked list, can be zero.

	\sa IsEmpty(void) const

***************************************/

uintptr_t BURGER_API Burger::MPLastInFirstOut::Size(void)
{
	m_Lock.lock();
	uintptr_t uSize = LastInFirstOut::Size();
	m_Lock.unlock();
	return uSize;
}

/*! ************************************

	\brief Insert an entry to the head of the list

	Insert a linked list into the head of the stored
	linked list.

	\param pNewRoot Pointer to the new head of the list 

	\sa AddLast(ForwardLink *) or RemoveFirst(void)

***************************************/

void BURGER_API Burger::MPLastInFirstOut::AddFirst(ForwardLink *pNewRoot)
{ 
	m_Lock.lock();
	LastInFirstOut::AddFirst(pNewRoot);
	m_Lock.unlock();	
}

/*! ************************************

	\brief Insert an entry to the end of the list

	Insert a linked list into the tail of the stored
	linked list.

	\param pNewTail Pointer to the list that will be appended
		to the current linked list

	\sa AddFirst(ForwardLink *)

***************************************/

void BURGER_API Burger::MPLastInFirstOut::AddLast(ForwardLink* pNewTail)
{
	m_Lock.lock();
	LastInFirstOut::AddLast(pNewTail);
	m_Lock.unlock();
}

/*! ************************************

	\brief Remove an entry from the head of the list

	Remove the pointer from the root of the linked list. Sets
	the new root with the second entry of the linked list
	if there is a second entry.

	\return The unlinked head pointer or \ref NULL if the list was empty.

	\sa AddFirst(ForwardLink *)

***************************************/

Burger::ForwardLink* BURGER_API Burger::MPLastInFirstOut::RemoveFirst(void)
{
	m_Lock.lock();
	ForwardLink *pRoot = LastInFirstOut::RemoveFirst();
	m_Lock.unlock();
	return pRoot;
}

/*! ************************************

	\brief Assigns ownership of the linked list.

	Returns the head pointer of the linked list
	and sets the linked list in this class to being
	empty. The calling function is assigned ownership
	of the entire list.

	\return The pointer to the entire linked list or \ref NULL if the list was empty.

	\sa Clear(void)

***************************************/

Burger::ForwardLink * BURGER_API Burger::MPLastInFirstOut::TakeList(void)
{	
	m_Lock.lock();
	// Get the list pointer
	ForwardLink *pRoot = LastInFirstOut::TakeList();
	m_Lock.unlock();
	// Return the list
	return pRoot;
}

/*! ************************************

	\brief Remove an item from the linked list.

	Traverse the linked list until the pointer is found
	and then unlink it from the list. The pointer is
	not deleted, just unlinked.

	\param pEntry Item to remove from the linked list
	\return Zero on success, kErrorItemNotFound if the pointer wasn't in the list

***************************************/

Burger::eError BURGER_API Burger::MPLastInFirstOut::Remove(ForwardLink *pEntry)
{
	m_Lock.lock();
	eError uResult = LastInFirstOut::Remove(pEntry);
	m_Lock.unlock();
	// Return the error
	return uResult;
}

/*! ************************************

	\brief Reverse the singly linked list

	Traverse the singly linked list and reverse the linkage
	so that the order of the entries are in the
	opposite order.

	\sa Burger::ReverseList(ForwardLink *)

***************************************/

void BURGER_API Burger::MPLastInFirstOut::ReverseList(void)
{
	m_Lock.lock();
	LastInFirstOut::ReverseList();
	m_Lock.unlock();
}

/*! ************************************

	\fn Burger::MPLastInFirstOut::Lock(void)
	\brief Lock the singly linked list

	Obtain the lock on the \ref CriticalSection
	so that the application can traverse
	or otherwise manipulate the linked list.

	\sa Burger::MPLastInFirstOut::Unlock(void)

***************************************/

/*! ************************************

	\fn Burger::MPLastInFirstOut::Unlock(void)
	\brief Unlock the singly linked list

	Release the lock on the \ref CriticalSection
	so that the linked list can be used by
	other threads.

	\sa Burger::MPLastInFirstOut::Lock(void)

***************************************/
