/***************************************

	Low devel doubly linked list manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRDOUBLYLINKEDLIST_H__
#define __BRDOUBLYLINKEDLIST_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

class DoublyLinkedList {
    BURGER_DISABLE_COPY(DoublyLinkedList);

private:
	DoublyLinkedList *m_pNext;		///< Pointer to the next entry
	DoublyLinkedList *m_pPrev;		///< Pointer to the previous entry

public:
	DoublyLinkedList() BURGER_NOEXCEPT { m_pNext = this; m_pPrev = this; }
	~DoublyLinkedList() { Detach(); }

	BURGER_INLINE void Detach(void) {
		DoublyLinkedList *pNext = m_pNext;
		DoublyLinkedList *pPrev = m_pPrev;
		pPrev->m_pNext = pNext;
		pNext->m_pPrev = pPrev;
		m_pNext = this;
		m_pPrev = this;
	}

	BURGER_INLINE void InsertAfter(DoublyLinkedList *pInput) {
		DoublyLinkedList *pNext = pInput->m_pNext;
		DoublyLinkedList *pPrev = pInput->m_pPrev;
		pPrev->m_pNext = pNext;		// Detach the object
		pNext->m_pPrev = pPrev;
		pNext = m_pNext;			// Link in
		pInput->m_pPrev = this;
		pInput->m_pNext = pNext;
		pNext->m_pPrev = pInput;
		m_pNext = pInput;
	}

	BURGER_INLINE void InsertBefore(DoublyLinkedList *pInput) {
		DoublyLinkedList *pNext = pInput->m_pNext;
		DoublyLinkedList *pPrev = pInput->m_pPrev;
		pPrev->m_pNext = pNext;		// Detach the object
		pNext->m_pPrev = pPrev;

		pPrev = m_pPrev;			// Link in
		pInput->m_pPrev = pPrev;
		pInput->m_pNext = this;
		pPrev->m_pNext = pInput;
		m_pPrev = pInput;
	}

	BURGER_INLINE DoublyLinkedList *GetNext(void) const { return m_pNext; }
	BURGER_INLINE DoublyLinkedList *GetPrevious(void) const { return m_pPrev; }

	static DoublyLinkedList * BURGER_API New(void);
};

}
/* END */

#endif

