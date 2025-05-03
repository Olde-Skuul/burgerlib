/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRDOUBLELINKEDLIST_H__
#include "brdoublelinkedlist.h"
#endif

/* BEGIN */
namespace Burger {

class LinkedListObjects {
	BURGER_DISABLE_COPY(LinkedListObjects);

public:
	/** Don't do anything */
	static const uint32_t kContinue = 0;
	/** Abort iteration */
	static const uint32_t kAbort = 0x01;
	/** Delete this object and continue iterating */
	static const uint32_t kDeleteObject = 0x02;

	typedef uint32_t(BURGER_API* ProcAction)(void* pData);

	class Object: private DoubleLinkedList {
		friend class LinkedListObjects;
		BURGER_DISABLE_COPY(Object);

	public:
		typedef void(BURGER_API* ProcDataDelete)(Object* pObject);

		static void BURGER_API proc_null(Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API proc_free_object_and_data(
			Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API proc_free_data(Object* pObject) BURGER_NOEXCEPT;
		static void BURGER_API proc_free_object(
			Object* pObject) BURGER_NOEXCEPT;

	private:
		/** Pointer to the data */
		void* m_pData;
		/** Data destructor callback */
		ProcDataDelete m_ProcDataDelete;

	public:
		Object() BURGER_NOEXCEPT: m_pData(nullptr),
								  m_ProcDataDelete(proc_free_data)
		{
		}

		Object(void* pData,
			ProcDataDelete pDataDelete = proc_free_data) BURGER_NOEXCEPT
			: m_pData(pData),
			  m_ProcDataDelete(pDataDelete)
		{
		}

		~Object()
		{
			shutdown();
		}

	private:
		void shutdown(void) BURGER_NOEXCEPT;
		BURGER_INLINE void insert_before(Object* pObject) BURGER_NOEXCEPT
		{
			DoubleLinkedList::insert_before(pObject);
		}

		BURGER_INLINE void insert_after(Object* pObject) BURGER_NOEXCEPT
		{
			DoubleLinkedList::insert_after(pObject);
		}

	public:
		static Object* BURGER_API new_object(void* pData,
			ProcDataDelete pProc = proc_free_object_and_data) BURGER_NOEXCEPT;

		BURGER_INLINE Object* get_next(void) const BURGER_NOEXCEPT
		{
			return static_cast<Object*>(DoubleLinkedList::get_next());
		}

		BURGER_INLINE Object* get_previous(void) const BURGER_NOEXCEPT
		{
			return static_cast<Object*>(DoubleLinkedList::get_previous());
		}

		BURGER_INLINE void* get_data(void) const BURGER_NOEXCEPT
		{
			return m_pData;
		}

		BURGER_INLINE void set_data(void* pInput) BURGER_NOEXCEPT
		{
			m_pData = pInput;
		}

		BURGER_INLINE ProcDataDelete get_data_delete_proc(
			void) const BURGER_NOEXCEPT
		{
			return m_ProcDataDelete;
		}

		BURGER_INLINE void set_data_delete_proc(
			ProcDataDelete pProc) BURGER_NOEXCEPT
		{
			m_ProcDataDelete = pProc;
		}
	};

private:
	/** Root linked list entry */
	Object* m_pRoot;
	/** Number of objects in the list */
	uint32_t m_uCount;

public:
	LinkedListObjects() BURGER_NOEXCEPT: m_pRoot(nullptr), m_uCount(0) {}
	~LinkedListObjects()
	{
		shutdown();
	}
	void shutdown(void) BURGER_NOEXCEPT;

	BURGER_INLINE uint32_t get_count(void) const BURGER_NOEXCEPT
	{
		return m_uCount;
	}

	BURGER_INLINE Object* get_first(void) const BURGER_NOEXCEPT
	{
		return m_pRoot;
	}

	Object* get_last(void) const BURGER_NOEXCEPT;
	void* get_first_data(void) const BURGER_NOEXCEPT;
	void* get_last_data(void) const BURGER_NOEXCEPT;
	void* get_data(uint32_t uIndex) const BURGER_NOEXCEPT;
	Object* get_object(uint32_t uIndex) const BURGER_NOEXCEPT;
	Object* get_object(void* pData) const BURGER_NOEXCEPT;
	Object* get_string_object(const char* pString) const BURGER_NOEXCEPT;
	uint32_t get_string_index(const char* pString) const BURGER_NOEXCEPT;
	void destroy(Object* pObject) BURGER_NOEXCEPT;
	void append(Object* pObject) BURGER_NOEXCEPT;
	void prepend(Object* pObject) BURGER_NOEXCEPT;
	void append(void* pData,
		Object::ProcDataDelete pDataDelete = Object::proc_free_object_and_data)
		BURGER_NOEXCEPT;
	void prepend(void* pData,
		Object::ProcDataDelete pDataDelete = Object::proc_free_object_and_data)
		BURGER_NOEXCEPT;
	void append(const char* pString) BURGER_NOEXCEPT;
	void prepend(const char* pString) BURGER_NOEXCEPT;
	Object* iterate_forward(ProcAction pProc) BURGER_NOEXCEPT;
	Object* iterate_reverse(ProcAction pProc) BURGER_NOEXCEPT;
};

}
/* END */

#endif
