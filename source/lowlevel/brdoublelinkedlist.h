/***************************************

	Low devel double linked list manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDOUBLELINKEDLIST_H__
#define __BRDOUBLELINKEDLIST_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

struct DoubleLinkedList_t {
	/** Pointer to the next entry */
	DoubleLinkedList_t* m_pNext;
	/** Pointer to the previous entry */
	DoubleLinkedList_t* m_pPrev;

	BURGER_INLINE void init(void) BURGER_NOEXCEPT
	{
		m_pNext = this;
		m_pPrev = this;
	}
	void BURGER_API detach(void) BURGER_NOEXCEPT;
	void BURGER_API insert_after(DoubleLinkedList_t* pInput) BURGER_NOEXCEPT;
	void BURGER_API insert_before(DoubleLinkedList_t* pInput) BURGER_NOEXCEPT;
};

class DoubleLinkedList: protected DoubleLinkedList_t {
	BURGER_DISABLE_COPY(DoubleLinkedList);

public:
	DoubleLinkedList() BURGER_NOEXCEPT
	{
		init();
	}
	~DoubleLinkedList()
	{
		detach();
	}

	BURGER_INLINE void detach(void) BURGER_NOEXCEPT
	{
		DoubleLinkedList_t::detach();
	}

	BURGER_INLINE void insert_after(DoubleLinkedList* pInput) BURGER_NOEXCEPT
	{
		DoubleLinkedList_t::insert_after(pInput);
	}

	BURGER_INLINE void insert_before(DoubleLinkedList* pInput) BURGER_NOEXCEPT
	{
		DoubleLinkedList_t::insert_before(pInput);
	}

	BURGER_INLINE DoubleLinkedList* get_next(void) const BURGER_NOEXCEPT
	{
		return static_cast<DoubleLinkedList*>(m_pNext);
	}

	BURGER_INLINE DoubleLinkedList* get_previous(void) const BURGER_NOEXCEPT
	{
		return static_cast<DoubleLinkedList*>(m_pPrev);
	}

	static DoubleLinkedList* BURGER_API New(void) BURGER_NOEXCEPT;
};

}
/* END */

#endif
