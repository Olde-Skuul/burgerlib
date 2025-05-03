/***************************************

	Templates to support "algorithm"

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRALGORITHM_H__
#define __BRALGORITHM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRTEMPLATES_H__
#include "brtemplates.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

/* BEGIN */
namespace Burger {

// Default deleter for Burger::delete_object
template<class T>
struct default_delete_Base {
	/** Perform the delete with Burger::delete_object() */
	BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(0 < sizeof(T));
		delete_object(pInput);
	}
};

// Default deleter for Burger::allocate_memory and free_memory
template<class T>
struct default_delete_Free {
	/** Perform the delete with Burger::free_memory() */
	BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
	{
		BURGER_STATIC_ASSERT(0 < sizeof(T));
		free_memory(pInput);
	}
};

// Implementation of C++20 std::unique_ptr
template<class T, class Deleter = Burger::default_delete<T> >
class unique_ptr {
public:
	/** Object type for this unique_ptr */
	typedef T element_type;
	/** Deleter type for this unique_ptr */
	typedef Deleter deleter_type;
	/** Pointer type of object in this unique_ptr */
	typedef T* pointer;

protected:
	/** Pointer to the object to track */
	pointer m_pObject;

	/** Instance of the deleter object */
	deleter_type m_Deleter;

public:
	unique_ptr() BURGER_NOEXCEPT: m_pObject(nullptr) {}
	unique_ptr(pointer pObject) BURGER_NOEXCEPT: m_pObject(pObject) {}

private:
	unique_ptr(const unique_ptr&) BURGER_EQUALS_DELETE;
	unique_ptr& operator=(const unique_ptr&) BURGER_EQUALS_DELETE;

public:
#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
	unique_ptr(unique_ptr&& rInput) BURGER_NOEXCEPT
		: m_pObject(rInput.m_pObject),
		  m_Deleter(rInput.m_Deleter)
	{
		rInput.m_pObject = nullptr;
	}
	unique_ptr& operator=(unique_ptr&& rInput) BURGER_NOEXCEPT
	{
		if (this != &rInput) {
			m_Deleter(m_pObject);
			m_pObject = rInput.m_pObject;
			rInput.m_pObject = nullptr;
			m_Deleter = static_cast<deleter_type&&>(rInput.m_Deleter);
		}
		return *this;
	}
#endif

	/** Dispose of the object */
	~unique_ptr()
	{
		m_Deleter(m_pObject);
	}

	BURGER_INLINE unique_ptr& operator=(pointer pObject) BURGER_NOEXCEPT
	{
		pointer pTemp = m_pObject;
		if (pTemp != pObject) {
			m_Deleter(pTemp);
			m_pObject = pObject;
		}
		return *this;
	}

	BURGER_INLINE pointer release() BURGER_NOEXCEPT
	{
		pointer pTemp = m_pObject;
		m_pObject = nullptr;
		return pTemp;
	}

	BURGER_INLINE void reset(pointer pObject) BURGER_NOEXCEPT
	{
		pointer pTemp = m_pObject;
		if (pTemp != pObject) {
			m_Deleter(pTemp);
			m_pObject = pObject;
		}
	}

	BURGER_INLINE void reset(void) BURGER_NOEXCEPT
	{
		m_Deleter(m_pObject);
		m_pObject = nullptr;
	}

	BURGER_NODISCARD BURGER_INLINE pointer get() const BURGER_NOEXCEPT
	{
		return m_pObject;
	}

	BURGER_NODISCARD BURGER_INLINE deleter_type& get_deleter() BURGER_NOEXCEPT
	{
		return m_Deleter;
	}

	BURGER_NODISCARD BURGER_INLINE const deleter_type&
	get_deleter() const BURGER_NOEXCEPT
	{
		return m_Deleter;
	}

	BURGER_NODISCARD BURGER_INLINE operator bool() const BURGER_NOEXCEPT
	{
		return m_pObject != nullptr;
	}

	BURGER_NODISCARD BURGER_INLINE element_type&
	operator*() const BURGER_NOEXCEPT
	{
		return *m_pObject;
	}

	BURGER_NODISCARD BURGER_INLINE pointer operator->() const BURGER_NOEXCEPT
	{
		return m_pObject;
	}
};

template<class T>
struct unique_ptr_array
	: Burger::unique_ptr<T, Burger::default_delete_array<T> > {};

template<class T>
struct unique_ptr_Base: Burger::unique_ptr<T, Burger::default_delete_Base<T> > {
};

template<class T>
struct unique_ptr_Free: Burger::unique_ptr<T, Burger::default_delete_Free<T> > {
};

}
/* END */

#endif
