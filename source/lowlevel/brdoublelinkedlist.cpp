/***************************************

	Low devel double linked list manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdoublelinkedlist.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\struct Burger::DoubleLinkedList_t
	\brief Double linked list

	Using inlined code, this structure handles all the tasks needed	to manage a
	circular double linked list. All objects when created point to themselves
	and can be added and removed to other linked list objects at will.

	\note This is a structure, as such, it does not have a destructor or
		constructor

	\sa \ref DoubleLinkedList

***************************************/

/*! ************************************

	\fn Burger::DoubleLinkedList_t::init(void)
	\brief Double linked list initialization

	Using inlined code, initialize the linked list entries to point to
	themselves so it is a circular linked list of only one entry.

	\sa detach(void)

***************************************/

/*! ************************************

	\brief Detach entry from connected list

	Unlink this list object from any connected list.

	Once this function ends, the entries in this object will point to itself.

	\sa insert_after(DoubleLinkedList_t*), or insert_before(DoubleLinkedList_t*)

***************************************/

void BURGER_API Burger::DoubleLinkedList_t::detach(void) BURGER_NOEXCEPT
{
	// Get the entries to the other list

	// Note: Get both entries so there's no dependency to the object
	DoubleLinkedList_t* pNext = m_pNext;
	DoubleLinkedList_t* pPrev = m_pPrev;

	// Unlink from the list
	pPrev->m_pNext = pNext;
	pNext->m_pPrev = pPrev;

	// Set the entries to itself
	m_pNext = this;
	m_pPrev = this;
}

/*! ************************************

	\brief Insert the supplied object after this object

	Unlink the input object from any linked list it's attached to and then
	insert the object after this one.

	\sa detach(void), or insert_before(DoubleLinkedList_t*)

***************************************/

void BURGER_API Burger::DoubleLinkedList_t::insert_after(
	DoubleLinkedList_t* pInput) BURGER_NOEXCEPT
{
	// Get the entries to the other list

	// Note: Get both entries so there's no dependency to the object
	DoubleLinkedList_t* pNext = pInput->m_pNext;
	DoubleLinkedList_t* pPrev = pInput->m_pPrev;

	// Detach the object
	pPrev->m_pNext = pNext;
	pNext->m_pPrev = pPrev;

	// Get the next object
	pNext = m_pNext;

	// Mark the new object to point to this as the previous and the new after
	pInput->m_pPrev = this;
	pInput->m_pNext = pNext;

	// Set the next object to point to the new object as its previous
	pNext->m_pPrev = pInput;

	// Link AFTER this object
	m_pNext = pInput;
}

/*! ************************************

	\brief Insert the supplied object before this object

	Unlink the input object from any linked list it's attached to and then
	insert the object before this one.

	\sa detach(void), or insert_after(DoubleLinkedList_t*)

***************************************/

void BURGER_API Burger::DoubleLinkedList_t::insert_before(
	DoubleLinkedList_t* pInput) BURGER_NOEXCEPT
{
	// Get the entries to the other list

	// Note: Get both entries so there's no dependency to the object
	DoubleLinkedList_t* pNext = pInput->m_pNext;
	DoubleLinkedList_t* pPrev = pInput->m_pPrev;

	// Detach the object
	pPrev->m_pNext = pNext;
	pNext->m_pPrev = pPrev;

	// Get the previous object
	pPrev = m_pPrev;

	// Mark the new object to point to this as the next and the new before
	pInput->m_pPrev = pPrev;
	pInput->m_pNext = this;

	// Set the previous object to point to the new object as its next
	pPrev->m_pNext = pInput;

	// Link BEFORE this object
	m_pPrev = pInput;
}

/*! ************************************

	\class Burger::DoubleLinkedList
	\brief Simple double linked list manager

	Using inlined code, this class handles all the tasks needed	to manage a
	circular doubly linked list. All objects when created point to themselves
	and can be added and removed to other linked list objects at will.

***************************************/

/*! ************************************

	\fn Burger::DoubleLinkedList::DoubleLinkedList()
	\brief Initializer for a DoubleLinkedList

	Set the links to point to itself. The object is not attached to another list

	\sa ~DoubleLinkedList() or detach()

***************************************/

/*! ************************************

	\fn Burger::DoubleLinkedList::~DoubleLinkedList()
	\brief Destructor for a DoubleLinkedList

	By calling detach(), this object will remove itself from any other list and
	then be ready for memory disposal

	\sa detach()

***************************************/

/*! ************************************

	\fn void Burger::DoubleLinkedList::detach(void)
	\brief Detach this object from any attached linked list

	This object will remove itself from any other list and both pointers will
	point to itself

	\sa DoubleLinkedList_t::detach(void), insert_before(DoubleLinkedList *),
		insert_after(DoubleLinkedList *), delete_object(const T*), or
		~DoubleLinkedList()

***************************************/

/*! ************************************

	\fn void Burger::DoubleLinkedList::insert_after(DoubleLinkedList *pInput)
	\brief Attach the input object after this one in the list

	Detach the input object from any linked list and then attach the input
	object after this one in the current list.

	\param pInput Pointer to a DoubleLinkedList object to link after this one

	\sa DoubleLinkedList_t::insert_after(DoubleLinkedList_t*),
		insert_before(DoubleLinkedList *) or detach(void)

***************************************/

/*! ************************************

	\fn void Burger::DoubleLinkedList::insert_before(DoubleLinkedList *pInput)
	\brief Attach the input object before this one in the list

	Detach the input object from any linked list and then attach the input
	object before this one in the current list.

	\param pInput Pointer to a DoubleLinkedList object to link before this one

	\sa DoubleLinkedList_t::insert_before(DoubleLinkedList_t*),
		insert_after(DoubleLinkedList *) or detach(void)

***************************************/

/*! ************************************

	\fn const DoubleLinkedList * Burger::DoubleLinkedList::get_next(void) const
	\brief Get the next pointer in the list

	Return the pointer to the next object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the start and end
	are detected properly

	\return Pointer to the next DoubleLinkedList object in the chain

	\sa get_previous(void) const

***************************************/

/*! ************************************

	\fn const DoubleLinkedList * Burger::DoubleLinkedList::get_previous(
		void) const

	\brief Get the previous pointer in the list

	Return the pointer to the previous object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the
	start and end are detected properly

	\return Pointer to the previous DoubleLinkedList object in the chain
	\sa get_next(void) const

***************************************/

/*! ************************************

	\brief Create a new DoubleLinkedList instance

	Allocate memory using Burger::allocate_memory() and initialize a DoubleLinkedList with
	it.

	\return \ref nullptr if out of memory
	\sa delete_object(const T*)

***************************************/

Burger::DoubleLinkedList* BURGER_API Burger::DoubleLinkedList::new_object(
	void) BURGER_NOEXCEPT
{
	// Manually allocate the memory
	return new (allocate_memory(sizeof(DoubleLinkedList))) DoubleLinkedList;
}
