/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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
	BURGER_DISABLE_COPY(LinkedListObjects);

public:
	enum {
		/** Don't do anything */
		CONTINUE = 0,
		/** Abort iteration */
		ABORT = 1,
		/** Delete this object and continue iterating */
		DELETEOBJECT = 2
	};

	class Object: private DoublyLinkedList {
		friend class LinkedListObjects;
		BURGER_DISABLE_COPY(Object);

	public:
		typedef void(BURGER_API* ProcDataDelete)(Object*);
		static void BURGER_API ProcNull(Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API ProcFreeObjectAndData(
			Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API ProcFreeData(Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API ProcFreeObject(Object* pObject) BURGER_NOEXCEPT;

	private:
		/** Pointer to the data */
		void* m_pData;
		/** Data destructor */
		ProcDataDelete m_ProcDataDelete;

	public:
		Object() BURGER_NOEXCEPT: DoublyLinkedList(),
								  m_pData(nullptr),
								  m_ProcDataDelete(ProcFreeData)
		{
		}
		Object(void* pData,
			ProcDataDelete pDataDelete = ProcFreeData) BURGER_NOEXCEPT
			: DoublyLinkedList(),
			  m_pData(pData),
			  m_ProcDataDelete(pDataDelete)
		{
		}
		~Object()
		{
			Destroy();
		}

	private:
		void Destroy(void) BURGER_NOEXCEPT;
		BURGER_INLINE void InsertBefore(Object* pObject) BURGER_NOEXCEPT
		{
			static_cast<DoublyLinkedList*>(this)->InsertBefore(pObject);
		}
		BURGER_INLINE void InsertAfter(Object* pObject) BURGER_NOEXCEPT
		{
			static_cast<DoublyLinkedList*>(this)->InsertAfter(pObject);
		}

	public:
		static Object* BURGER_API New(void* pData,
			ProcDataDelete pProc = ProcFreeObjectAndData) BURGER_NOEXCEPT;
		BURGER_INLINE Object* GetNext(void) const BURGER_NOEXCEPT
		{
			return static_cast<Object*>(DoublyLinkedList::GetNext());
		}
		BURGER_INLINE Object* GetPrevious(void) const BURGER_NOEXCEPT
		{
			return static_cast<Object*>(DoublyLinkedList::GetPrevious());
		}
		BURGER_INLINE void* GetData(void) const BURGER_NOEXCEPT
		{
			return m_pData;
		}
		BURGER_INLINE void SetData(void* pInput) BURGER_NOEXCEPT
		{
			m_pData = pInput;
		}
		BURGER_INLINE ProcDataDelete GetDataDeleteProc(
			void) const BURGER_NOEXCEPT
		{
			return m_ProcDataDelete;
		}
		BURGER_INLINE void SetDataDeleteProc(
			ProcDataDelete pProc) BURGER_NOEXCEPT
		{
			m_ProcDataDelete = pProc;
		}
	};

private:
	/** Root linked list entry */
	Object* m_pRoot;
	/** Number of objects in the list */
	uint_t m_uCount;

public:
	typedef uint_t(BURGER_API* ProcAction)(void*);

	LinkedListObjects() BURGER_NOEXCEPT: m_pRoot(nullptr), m_uCount(0) {}
	~LinkedListObjects()
	{
		Destroy();
	}
	void Destroy(void) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t GetCount(void) const BURGER_NOEXCEPT
	{
		return m_uCount;
	}
	BURGER_INLINE Object* GetFirst(void) const BURGER_NOEXCEPT
	{
		return m_pRoot;
	}
	Object* GetLast(void) const BURGER_NOEXCEPT;
	void* GetFirstData(void) const BURGER_NOEXCEPT;
	void* GetLastData(void) const BURGER_NOEXCEPT;
	void* GetData(uint_t uIndex) const BURGER_NOEXCEPT;
	Object* GetObject(uint_t uIndex) const BURGER_NOEXCEPT;
	Object* GetObject(void* pData) const BURGER_NOEXCEPT;
	Object* GetStringObject(const char* pString) const BURGER_NOEXCEPT;
	uint_t GetStringIndex(const char* pString) const BURGER_NOEXCEPT;
	void DestroyObject(Object* pObject) BURGER_NOEXCEPT;
	void AppendObject(Object* pObject) BURGER_NOEXCEPT;
	void PrependObject(Object* pObject) BURGER_NOEXCEPT;
	void AppendObject(void* pData,
		Object::ProcDataDelete pDataDelete = Object::ProcFreeObjectAndData)
		BURGER_NOEXCEPT;
	void PrependObject(void* pData,
		Object::ProcDataDelete pDataDelete = Object::ProcFreeObjectAndData)
		BURGER_NOEXCEPT;
	void AppendObject(const char* pString) BURGER_NOEXCEPT;
	void PrependObject(const char* pString) BURGER_NOEXCEPT;
	Object* IterateForward(ProcAction pProc) BURGER_NOEXCEPT;
	Object* IterateReverse(ProcAction pProc) BURGER_NOEXCEPT;
};

}
/* END */

#endif
