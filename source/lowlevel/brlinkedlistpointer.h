/***************************************

	Doubly linked list with a parent pointer

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLINKEDLISTPOINTER_H__
#define __BRLINKEDLISTPOINTER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDOUBLYLINKEDLIST_H__
#include "brdoublylinkedlist.h"
#endif

/* BEGIN */
namespace Burger {
class LinkedListPointer : public DoublyLinkedList {
	void *m_pData;			///< Pointer to the data
public:
	LinkedListPointer() : DoublyLinkedList(), m_pData(NULL) {}
	LinkedListPointer(void *pData) : DoublyLinkedList(), m_pData(pData) {}
	BURGER_INLINE LinkedListPointer *GetNext(void) const { return static_cast<LinkedListPointer *>(DoublyLinkedList::GetNext()); }
	BURGER_INLINE LinkedListPointer *GetPrevious(void) const { return static_cast<LinkedListPointer *>(DoublyLinkedList::GetPrevious()); }
	BURGER_INLINE void *GetData(void) const { return m_pData; }
	BURGER_INLINE void SetData(void *pInput) { m_pData = pInput; }
	static LinkedListPointer * BURGER_API New(void *pInput = NULL);
};
extern void BURGER_API Delete(const LinkedListPointer *pInput);
}
/* END */

#endif

