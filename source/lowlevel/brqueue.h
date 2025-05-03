/***************************************

    Simple First In First Out queue

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRQUEUE_H__
#define __BRQUEUE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
template<class T>
class Queue {
	struct Element {
		T m_Data;				///< Copy of the data attached to this entry
		Element* m_pNext;		///< Pointer to the next element in the singly linked list
		Element(const T& rData) : m_Data(rData), m_pNext(NULL) { }
	};
	Element* m_pFirstElement;	///< Pointer to the first element in the linked list
	Element* m_pLastElement;	///< Pointer to the last element in the linked list
	uintptr_t m_uCount;			///< Number of entries in the linked list
public:
	Queue() : m_pFirstElement(NULL),m_pLastElement(NULL),m_uCount(0) {}
	~Queue()
	{
		clear();
	}

	void clear(void)
	{
		Element *pElement = m_pFirstElement;
		if (pElement) {
			do {
				Element *pNext = pElement->m_pNext;
				delete_object(pElement);
				pElement = pNext;
			} while (pElement);
		}
		m_pFirstElement = NULL;
		m_pLastElement = NULL;
		m_uCount = 0;
	}

	BURGER_INLINE const T& back(void) const { return m_pLastElement->m_Data; }
	BURGER_INLINE T& back(void) { return m_pLastElement->m_Data; }
	BURGER_INLINE const T& front(void) const { return m_pFirstElement->m_Data; }
	BURGER_INLINE T& front(void) { return m_pFirstElement->m_Data; }
	void pop(void) {
		Element *pElement = m_pFirstElement;
		m_pFirstElement = pElement->m_pNext;
		delete_object(pElement);
		--m_uCount;
		if (m_pLastElement == pElement) {
			m_pLastElement = NULL;
			BURGER_ASSERT((!m_pFirstElement) && (!m_uCount));
		}
	}
	void push(const T& rData) {
		Element *pElement = new (allocate_memory(sizeof(Element))) Element(rData);
		if (!m_pLastElement) {
			BURGER_ASSERT((!m_pFirstElement) && (!m_uCount));
			m_pLastElement = pElement;
			m_pFirstElement = pElement;
		} else {
			BURGER_ASSERT(!m_pLastElement->m_pNext);
			m_pLastElement->m_pNext = pElement;
			m_pLastElement = pElement;
		}
		++m_uCount;
	}
	BURGER_INLINE uintptr_t size(void) const { return m_uCount; }
	BURGER_INLINE uint_t empty(void) const { return (m_uCount==0); }
};
}
/* END */

#endif
