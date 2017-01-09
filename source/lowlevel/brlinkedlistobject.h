/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLINKEDLISTOBJECT_H__
#define __BRLINKEDLISTOBJECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDOUBLYLINKEDLIST_H__
#include "brdoublylinkedlist.h"
#endif

/* BEGIN */
namespace Burger {
class LinkedListObjects {
public:
	enum {
		CONTINUE = 0,		///< Don't do anything
		ABORT = 1,			///< Abort iteration
		DELETEOBJECT = 2	///< Delete this object and continue iterating
	};
	typedef Word (BURGER_API *ProcAction)(void *);
	class Object : private DoublyLinkedList {
	friend class LinkedListObjects;
	public:
		typedef void (BURGER_API *ProcDataDelete)(Object *);
		static void BURGER_API ProcNull(Object *pObject);
		static void BURGER_API ProcFreeObjectAndData(Object *pObject);
		static void BURGER_API ProcFreeData(Object *pObject);
		static void BURGER_API ProcFreeObject(Object *pObject);
	private:
		void *m_pData;						///< Pointer to the data
		ProcDataDelete m_ProcDataDelete;	///< Data destructor
	public:
		Object() : DoublyLinkedList(), m_pData(NULL), m_ProcDataDelete(ProcFreeData) {}
		Object(void *pData,ProcDataDelete pDataDelete=ProcFreeData) : DoublyLinkedList(), m_pData(pData), m_ProcDataDelete(pDataDelete) {}
		~Object() { Destroy(); }
	private:
		void Destroy(void);
		BURGER_INLINE void InsertBefore(Object *pObject) { static_cast<DoublyLinkedList *>(this)->InsertBefore(pObject); }
		BURGER_INLINE void InsertAfter(Object *pObject) { static_cast<DoublyLinkedList *>(this)->InsertAfter(pObject); }
	public:
		static Object * BURGER_API New(void *pData,ProcDataDelete pProc=ProcFreeObjectAndData);
		BURGER_INLINE Object *GetNext(void) const { return static_cast<Object *>(static_cast<const DoublyLinkedList *>(this)->GetNext()); }
		BURGER_INLINE Object *GetPrevious(void) const { return static_cast<Object *>(static_cast<const DoublyLinkedList *>(this)->GetPrevious()); }
		BURGER_INLINE void *GetData(void) const { return m_pData; }
		BURGER_INLINE void SetData(void *pInput) { m_pData = pInput; }
		BURGER_INLINE ProcDataDelete GetDataDeleteProc(void) const { return m_ProcDataDelete; }
		BURGER_INLINE void SetDataDeleteProc(ProcDataDelete pProc) { m_ProcDataDelete = pProc; }
	};
private:
	Object *m_pRoot;		///< Root linked list entry
	Word m_uCount;			///< Number of objects in the list
public:
	LinkedListObjects() : m_pRoot(NULL),m_uCount(0) {}
	~LinkedListObjects() { Destroy(); }
	void Destroy(void);
	BURGER_INLINE Word GetCount(void) const { return m_uCount; }
	BURGER_INLINE Object *GetFirst(void) const { return m_pRoot; }
	Object *GetLast(void) const;
	void *GetFirstData(void) const;
	void *GetLastData(void) const;
	void *GetData(Word uIndex) const;
	Object *GetObject(Word uIndex) const;
	Object *GetObject(void *pData) const;
	Object *GetStringObject(const char *pString) const;
	Word GetStringIndex(const char *pString) const;
	void DestroyObject(Object *pObject);
	void AppendObject(Object *pObject);
	void PrependObject(Object *pObject);
	void AppendObject(void *pData,Object::ProcDataDelete pDataDelete=Object::ProcFreeObjectAndData);
	void PrependObject(void *pData,Object::ProcDataDelete pDataDelete=Object::ProcFreeObjectAndData);
	void AppendObject(const char *pString);
	void PrependObject(const char *pString);
	Object *IterateForward(ProcAction pProc);
	Object *IterateReverse(ProcAction pProc);
};
}
/* END */

#endif

