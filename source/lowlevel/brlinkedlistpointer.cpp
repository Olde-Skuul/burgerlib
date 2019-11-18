/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brlinkedlistpointer.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::LinkedListPointer
	\brief Double linked list with pointer class

	This derived class from DoublyLinkedList adds an application
	supplied pointer so the linked list objects point to
	parent data. It's intended to be a simple extension to
	a linked list class.

	If more robustness is needed, consider using \ref LinkedListObjects instead

	\sa LinkedListObjects

***************************************/

/*! ************************************

	\brief Create a new LinkedListPointer instance

	Allocate memory using Burger::Alloc() and 
	initialize a LinkedListPointer with it.

	\return \ref NULL if out of memory
	\sa Delete(const LinkedListPointer *)

***************************************/

Burger::LinkedListPointer * BURGER_API Burger::LinkedListPointer::New(void *pInput)
{
	// Manually allocate the memory
	LinkedListPointer *pThis = new (Alloc(sizeof(LinkedListPointer))) LinkedListPointer(pInput);
	// Sorry Charlie!
	return pThis;
}

