/***************************************

	Double linked list with a parent pointer

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLINKEDLISTPOINTER_H__
#define __BRLINKEDLISTPOINTER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDOUBLELINKEDLIST_H__
#include "brdoublelinkedlist.h"
#endif

/* BEGIN */
namespace Burger {

class LinkedListPointer: public DoubleLinkedList {
	BURGER_DISABLE_COPY(LinkedListPointer);

private:
	/** Pointer to the data */
	void* m_pData;

public:
	LinkedListPointer() BURGER_NOEXCEPT: DoubleLinkedList(), m_pData(nullptr) {}
	LinkedListPointer(void* pData) BURGER_NOEXCEPT: DoubleLinkedList(),
													m_pData(pData)
	{
	}

	BURGER_INLINE LinkedListPointer* get_next(void) const BURGER_NOEXCEPT
	{
		return static_cast<LinkedListPointer*>(DoubleLinkedList::get_next());
	}

	BURGER_INLINE LinkedListPointer* get_previous(void) const BURGER_NOEXCEPT
	{
		return static_cast<LinkedListPointer*>(
			DoubleLinkedList::get_previous());
	}

	BURGER_INLINE void* get_data(void) const BURGER_NOEXCEPT
	{
		return m_pData;
	}

	BURGER_INLINE void set_data(void* pInput) BURGER_NOEXCEPT
	{
		m_pData = pInput;
	}

	static LinkedListPointer* BURGER_API New(
		void* pInput = nullptr) BURGER_NOEXCEPT;
};

}
/* END */

#endif
