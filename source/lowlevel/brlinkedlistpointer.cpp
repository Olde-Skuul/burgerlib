/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brlinkedlistpointer.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::LinkedListPointer
	\brief Double linked list with pointer class

	This derived class from DoubleLinkedList adds an application supplied
	pointer so the linked list objects point to parent data. It's intended to be
	a simple extension to a linked list class.

	If more robustness is needed, consider using \ref LinkedListObjects instead

	\sa LinkedListObjects or DoubleLinkedList

***************************************/

/*! ************************************

	\fn Burger::LinkedListPointer::LinkedListPointer()
	\brief Initializer for a LinkedListPointer

	Set the links to point to itself. The object is not attached to another
	list. The data is set to \ref nullptr.

	\sa ~DoubleLinkedList() or LinkedListPointer(void *)

***************************************/

/*! ************************************

	\fn Burger::LinkedListPointer::LinkedListPointer(void *)
	\brief Initializer for a LinkedListPointer with default data.

	Set the links to point to itself. The object is not attached to another
	list. The data is set to to the input value.

	\param pData Pointer to initialize the pointer with.

	\sa ~DoubleLinkedList() or LinkedListPointer()

***************************************/

/*! ************************************

	\fn const LinkedListPointer * Burger::LinkedListPointer::get_next(
		void) const

	\brief Get the next pointer in the list

	Return the pointer to the next object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the start and end
	are detected properly

	\return Pointer to the next LinkedListPointer object in the chain

	\sa get_previous(void) const

***************************************/

/*! ************************************

	\fn const LinkedListPointer * Burger::LinkedListPointer::get_previous(
		void) const

	\brief Get the previous pointer in the list

	Return the pointer to the previous object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the
	start and end are detected properly

	\return Pointer to the previous LinkedListPointer object in the chain

	\sa get_next(void) const

***************************************/

/*! ************************************

	\fn void * Burger::LinkedListPointer::get_data(void) const
	\brief Get the pointer stored in this node.

	Return the pointer stored in this node.

	\return Pointer stored in this node.

	\sa set_data(void *)

***************************************/

/*! ************************************

	\fn void Burger::LinkedListPointer::set_data(void *)
	\brief Set the pointer to store in this node.

	Change the pointer stored in this node with the new pointer. The previous
	pointer is discarded.

	\param pInput Pointer to store in this node.

	\sa get_data(void) const

***************************************/

/*! ************************************

	\brief Create a new LinkedListPointer instance

	Allocate memory using Burger::allocate_memory() and initialize a LinkedListPointer
	with it.

	\return \ref nullptr if out of memory
	\sa delete_object(const T *)

***************************************/

Burger::LinkedListPointer* BURGER_API Burger::LinkedListPointer::new_object(
	void* pInput) BURGER_NOEXCEPT
{
	// Manually allocate the memory
	return new (allocate_memory(sizeof(LinkedListPointer))) LinkedListPointer(pInput);
}
