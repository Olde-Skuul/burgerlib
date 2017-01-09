/***************************************

	Low devel doubly linked list manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brdoublylinkedlist.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::DoublyLinkedList
	\brief Simple double linked list manager

	Using inlined code, this class handles all the tasks needed
	to manage a circular doubly linked list. All objects when
	created point to themselves and can be added and removed
	to other linked list objects at will.

***************************************/

/*! ************************************

	\fn Burger::DoublyLinkedList::DoublyLinkedList()
	\brief Initializer for a DoublyLinkedList

	Set the links to point to itself. The object is not attached to another list

	\sa ~DoublyLinkedList() or Detach()

***************************************/

/*! ************************************

	\fn Burger::DoublyLinkedList::~DoublyLinkedList()
	\brief Destructor for a DoublyLinkedList

	By calling Detach(), this object will remove itself from any other
	list and then be ready for memory disposal

	\sa Delete(const DoublyLinkedList*) or Detach()

***************************************/

/*! ************************************

	\fn void Burger::DoublyLinkedList::Detach(void)
	\brief Detach this object from any attached linked list

	This object will remove itself from any other
	list and both pointers will point to itself

	\sa InsertBefore(DoublyLinkedList *), InsertAfter(DoublyLinkedList *),
		Delete(const DoublyLinkedList*) or ~DoublyLinkedList()

***************************************/

/*! ************************************

	\fn void Burger::DoublyLinkedList::InsertAfter(DoublyLinkedList *pInput)
	\brief Attach the input object after this one in the list

	Detach the input object from any linked list and then attach the
	input object after this one in the current list.

	\param pInput Pointer to a DoublyLinkedList object to link after this one
	\sa InsertBefore(DoublyLinkedList *) or Detach(void)

***************************************/

/*! ************************************

	\fn void Burger::DoublyLinkedList::InsertBefore(DoublyLinkedList *pInput)
	\brief Attach the input object before this one in the list

	Detach the input object from any linked list and then attach the
	input object before this one in the current list.

	\param pInput Pointer to a DoublyLinkedList object to link before this one
	\sa InsertAfter(DoublyLinkedList *) or Detach(void)

***************************************/

/*! ************************************

	\fn const DoublyLinkedList * Burger::DoublyLinkedList::GetNext(void) const
	\brief Get the next pointer in the list

	Return the pointer to the next object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the
	start and end are detected properly

	\return Pointer to the next DoublyLinkedList object in the chain
	\sa GetPrevious(void) const

***************************************/


/*! ************************************

	\fn const DoublyLinkedList * Burger::DoublyLinkedList::GetPrevious(void) const
	\brief Get the previous pointer in the list

	Return the pointer to the previous object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the
	start and end are detected properly

	\return Pointer to the previous DoublyLinkedList object in the chain
	\sa GetNext(void) const

***************************************/



/*! ************************************

	\brief Create a new DoublyLinkedList instance

	Allocate memory using Burger::Alloc() and 
	initialize a DoublyLinkedList with it.

	\return \ref NULL if out of memory
	\sa Delete(const DoublyLinkedList *)

***************************************/

Burger::DoublyLinkedList * BURGER_API Burger::DoublyLinkedList::New(void)
{
	// Manually allocate the memory
	DoublyLinkedList *pThis = new (Alloc(sizeof(DoublyLinkedList))) DoublyLinkedList();
	// Sorry Charlie!
	return pThis;
}
