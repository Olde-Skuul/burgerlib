/***************************************

	Last in, first out, singly linked list queue

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLASTINFIRSTOUT_H__
#define __BRLASTINFIRSTOUT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRMUTEX_H__
#include "brmutex.h"
#endif

/* BEGIN */
namespace Burger {

class ForwardLink {
	BURGER_DISABLE_COPY(ForwardLink);

private:
	/** Pointer to the next object in the singly linked list */
	ForwardLink* m_pNext;

public:
	ForwardLink() BURGER_NOEXCEPT: m_pNext(nullptr) {}
	ForwardLink(ForwardLink* pNext) BURGER_NOEXCEPT: m_pNext(pNext) {}

	BURGER_INLINE void clear(void) BURGER_NOEXCEPT
	{
		m_pNext = nullptr;
	}

	BURGER_INLINE ForwardLink* get_next(void) const BURGER_NOEXCEPT
	{
		return m_pNext;
	}

	BURGER_INLINE void set_next(ForwardLink* pNext) BURGER_NOEXCEPT
	{
		m_pNext = pNext;
	}
};

extern ForwardLink* BURGER_API reverse_list(ForwardLink* pRoot) BURGER_NOEXCEPT;

class LastInFirstOut {
	BURGER_DISABLE_COPY(LastInFirstOut);

protected:
	/** Root pointer to the singly linked list */
	ForwardLink* m_pRoot;

public:
	LastInFirstOut() BURGER_NOEXCEPT: m_pRoot(nullptr) {}
	LastInFirstOut(ForwardLink* pRoot) BURGER_NOEXCEPT: m_pRoot(pRoot) {}

	BURGER_INLINE void clear(void) BURGER_NOEXCEPT
	{
		m_pRoot = nullptr;
	}

	BURGER_INLINE uint_t is_empty(void) const BURGER_NOEXCEPT
	{
		return !m_pRoot;
	}

	BURGER_INLINE ForwardLink* get_root(void) const BURGER_NOEXCEPT
	{
		return m_pRoot;
	}

	uintptr_t BURGER_API size(void) const BURGER_NOEXCEPT;
	void BURGER_API add_first(ForwardLink* pNewRoot) BURGER_NOEXCEPT;
	void BURGER_API add_last(ForwardLink* pNewTail) BURGER_NOEXCEPT;
	ForwardLink* BURGER_API remove_first(void) BURGER_NOEXCEPT;
	ForwardLink* BURGER_API take_list(void) BURGER_NOEXCEPT;
	eError BURGER_API remove(ForwardLink* pEntry) BURGER_NOEXCEPT;

	BURGER_INLINE void reverse_list(void) BURGER_NOEXCEPT
	{
		m_pRoot = Burger::reverse_list(m_pRoot);
	}
};

class MPLastInFirstOut: protected LastInFirstOut {
	BURGER_DISABLE_COPY(MPLastInFirstOut);

protected:
	/** Critical section for thread safety */
	Mutex m_Lock;

public:
	MPLastInFirstOut() BURGER_NOEXCEPT;
	MPLastInFirstOut(ForwardLink* pRoot) BURGER_NOEXCEPT;
	void BURGER_API clear(void) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t is_empty(void) const BURGER_NOEXCEPT
	{
		return !m_pRoot;
	}
	uintptr_t BURGER_API size(void) BURGER_NOEXCEPT;
	void BURGER_API add_first(ForwardLink* pNewRoot) BURGER_NOEXCEPT;
	void BURGER_API add_last(ForwardLink* pNewTail) BURGER_NOEXCEPT;
	ForwardLink* BURGER_API remove_first(void) BURGER_NOEXCEPT;
	ForwardLink* BURGER_API take_list(void) BURGER_NOEXCEPT;
	eError BURGER_API remove(ForwardLink* pEntry) BURGER_NOEXCEPT;
	void BURGER_API reverse_list(void) BURGER_NOEXCEPT;

	BURGER_INLINE void lock(void) BURGER_NOEXCEPT
	{
		m_Lock.lock();
	}

	BURGER_INLINE void unlock(void) BURGER_NOEXCEPT
	{
		m_Lock.unlock();
	}
};

}

/* END */

#endif
