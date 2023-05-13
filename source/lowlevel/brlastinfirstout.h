/***************************************

    Last in, first out, singly linked list queue

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRCRITICALSECTION_H__
#include "brcriticalsection.h"
#endif

/* BEGIN */
namespace Burger {

class ForwardLink {
    BURGER_DISABLE_COPY(ForwardLink);

private:
	ForwardLink *m_pNext;			///< Pointer to the next object in the singly linked list

public:
	ForwardLink() : m_pNext(NULL) {}
	ForwardLink(ForwardLink *pNext) : m_pNext(pNext) {}
	BURGER_INLINE void Clear(void) { m_pNext = NULL; }
	BURGER_INLINE ForwardLink *GetNext(void) const { return m_pNext; }
	BURGER_INLINE void SetNext(ForwardLink *pNext) { m_pNext = pNext; }
};

extern ForwardLink * BURGER_API ReverseList(ForwardLink *pRoot);

class LastInFirstOut {
    BURGER_DISABLE_COPY(LastInFirstOut);

protected:
	ForwardLink *m_pRoot;		///< Root pointer to the singly linked list

public:
	LastInFirstOut() : m_pRoot(NULL) {}		
	LastInFirstOut(ForwardLink *pRoot) : m_pRoot(pRoot) {}
	BURGER_INLINE void Clear(void) { m_pRoot = NULL; }
	BURGER_INLINE uint_t IsEmpty(void) const { return !m_pRoot; }
	BURGER_INLINE ForwardLink *GetRoot(void) const { return m_pRoot; }
	uintptr_t BURGER_API Size(void) const;
	void BURGER_API AddFirst(ForwardLink *pNewRoot);
	void BURGER_API AddLast(ForwardLink* pNewTail);
	ForwardLink * BURGER_API RemoveFirst(void);
	ForwardLink * BURGER_API TakeList(void);
	eError BURGER_API Remove(ForwardLink *pEntry);
	BURGER_INLINE void ReverseList(void) { m_pRoot = Burger::ReverseList(m_pRoot); }
};

class MPLastInFirstOut : protected LastInFirstOut {
    BURGER_DISABLE_COPY(MPLastInFirstOut);

protected:
	CriticalSection m_Lock;		///< Critical section for thread safety

public:
	MPLastInFirstOut();	
	MPLastInFirstOut(ForwardLink *pRoot);	
	void BURGER_API Clear(void);
	BURGER_INLINE uint_t IsEmpty(void) const { return !m_pRoot; }
	uintptr_t BURGER_API Size(void);
	void BURGER_API AddFirst(ForwardLink *pNewRoot);
	void BURGER_API AddLast(ForwardLink* pNewTail);
	ForwardLink * BURGER_API RemoveFirst(void);
	ForwardLink * BURGER_API TakeList(void);
	eError BURGER_API Remove(ForwardLink *pEntry);
	void BURGER_API ReverseList(void);
	BURGER_INLINE void Lock(void) { m_Lock.lock(); }
	BURGER_INLINE void Unlock(void) { m_Lock.unlock(); }
};

}

/* END */

#endif
