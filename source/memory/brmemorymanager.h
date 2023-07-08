/***************************************

	Memory Manager Base Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMEMORYMANAGER_H__
#define __BRMEMORYMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRTEMPLATES_H__
#include "brtemplates.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

/* BEGIN */
namespace Burger {

template<class T>
class allocator {
public:
	/** Type of object this allocator is creating memory */
	typedef T value_type;

	/** Quantities of elements */
	typedef uintptr_t size_type;

	/** Difference between pointers*/
	typedef intptr_t difference_type;

	/** Pointer to element */
	typedef T* pointer;

	/** Pointer to constant element */
	typedef const T* const_pointer;

	/** Reference to element */
	typedef T& reference;

	/** Reference to constant element */
	typedef const T& const_reference;

	/** Propagate on move assignment */
	typedef true_type propagate_on_container_move_assignment;

	/** Copy the allocator when copy assigned */
	typedef true_type propagate_on_container_copy_assignment;

	/** Swap allocators on swap assignment */
	typedef true_type propagate_on_container_swap;

	/** This allocator matches any other allocator type */
	typedef true_type is_always_equal;

	/** Its member type ``other`` is the equivalent allocator type to allocate
	 * elements of type ``T`` */
	template<class U>
	struct rebind {
		/** ``other`` matches ``T`` */
		typedef allocator<U> other;
	};

	/** Default constructor. */
	BURGER_CONSTEXPR allocator() BURGER_DEFAULT_CONSTRUCTOR;

	/** Default destructor. */
	~allocator() BURGER_DEFAULT_DESTRUCTOR;

	/** Copy constructor. */
	BURGER_CONSTEXPR allocator(const allocator&) BURGER_DEFAULT_CONSTRUCTOR;

	/* Assignment copy constructor. */
	//allocator& operator=(const allocator&) BURGER_DEFAULT_CONSTRUCTOR;

#if defined(BURGER_RVALUE_REFERENCES)
	/** Move constructor. */
	BURGER_CONSTEXPR allocator(allocator&&) BURGER_DEFAULT_CONSTRUCTOR;

	/** Assignment move constructor. */
	allocator& operator=(allocator&&) BURGER_DEFAULT_CONSTRUCTOR;
#endif

	/** Allocator that crosses types.
	Since this allocator can share pools, this exists and does nothing. */
	template<class U>
	allocator(const allocator<U>&)
	{
	}

	/** Returns the address of x. */
	BURGER_INLINE pointer address(reference x) BURGER_NOEXCEPT
	{
		return &x;
	}

	/** Returns the const address of x. */
	BURGER_INLINE const_pointer address(const_reference x) const BURGER_NOEXCEPT
	{
		return &x;
	}

	BURGER_INLINE T* allocate(uintptr_t n) BURGER_NOEXCEPT
	{
		return static_cast<pointer>(::operator new(n * sizeof(T)));
	}

	BURGER_INLINE void deallocate(pointer p, size_type) BURGER_NOEXCEPT
	{
		::operator delete((void*)p);
	}

	BURGER_INLINE uintptr_t max_size() const BURGER_NOEXCEPT
	{
		return UINTPTR_MAX;
	}

	/** Default constructs an object */
	BURGER_INLINE void construct(pointer p)
	{
		::new ((void*)p) value_type();
	}

	/** Destroys the contents of an object */
	BURGER_INLINE void destroy(pointer p)
	{
		p->~T();
	}
};

template<>
class allocator<void> {
public:
	/** Type is hard coded to void */
	typedef void value_type;

	/** Pointer to nothing */
	typedef void* pointer;

	/** Const pointer to nothing */
	typedef const void* const_pointer;

	/** Its member type ``other`` is the equivalent allocator type to allocate
	 * elements of type ``T`` */
	template<class U>
	struct rebind {
		/** ``other`` matches ``T`` */
		typedef allocator<U> other;
	};
};

template<>
class allocator<const void> {
public:
	/** Type is hard coded to const void */
	typedef const void value_type;

	/** Pointer to nothing */
	typedef const void* pointer;

	/** Const pointer to nothing */
	typedef const void* const_pointer;

	/** Its member type ``other`` is the equivalent allocator type to allocate
	 * elements of type ``T`` */
	template<class U>
	struct rebind {

		/** ``other`` matches ``T`` */
		typedef allocator<U> other;
	};
};

/** Allocators are always equal. */
template<class T, class U>
BURGER_INLINE bool operator==(
	const allocator<T>&, const allocator<U>&) BURGER_NOEXCEPT
{
	return true;
}

/** Allocators are always equal. */
template<class T, class U>
BURGER_INLINE bool operator!=(
	const allocator<T>&, const allocator<U>&) BURGER_NOEXCEPT
{
	return false;
}

class AllocatorBase: public Base {
	BURGER_RTTI_IN_CLASS();

public:
	AllocatorBase() BURGER_DEFAULT_CONSTRUCTOR;
	virtual void* alloc(uintptr_t uSize) const BURGER_NOEXCEPT = 0;
	virtual void free(const void* pInput) const BURGER_NOEXCEPT = 0;
	virtual void* realloc(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT = 0;

	void* BURGER_API alloc_clear(uintptr_t uSize) const BURGER_NOEXCEPT;
	void* BURGER_API alloc_copy(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT;
};

struct MemoryManager {

	/** Function prototype for allocating memory */
	typedef void*(BURGER_API* ProcAlloc)(MemoryManager* pThis, uintptr_t uSize);

	/** Function prototype for releasing memory */
	typedef void(BURGER_API* ProcFree)(
		MemoryManager* pThis, const void* pInput);

	/** Function prototype for reallocating memory. */
	typedef void*(BURGER_API* ProcRealloc)(
		MemoryManager* pThis, const void* pInput, uintptr_t uSize);

	/** Function prototype for destructor */
	typedef void(BURGER_API* ProcShutdown)(MemoryManager* pThis);

	/** Pointer to allocation function */
	ProcAlloc m_pAlloc;

	/** Pointer to memory release function */
	ProcFree m_pFree;

	/** Pointer to the memory reallocation function */
	ProcRealloc m_pRealloc;

	/** Pointer to the shutdown function */
	ProcShutdown m_pShutdown;

	BURGER_INLINE void* alloc(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return m_pAlloc(this, uSize);
	}

	BURGER_INLINE void free(const void* pInput) BURGER_NOEXCEPT
	{
		return m_pFree(this, pInput);
	}

	BURGER_INLINE void* realloc(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return m_pRealloc(this, pInput, uSize);
	}

	BURGER_INLINE void shutdown(void) BURGER_NOEXCEPT
	{
		m_pShutdown(this);
	}

	void* BURGER_API alloc_clear(uintptr_t uSize) BURGER_NOEXCEPT;

	static void BURGER_API shutdown(MemoryManager* pThis) BURGER_NOEXCEPT;
};

extern void* BURGER_API alloc_platform_memory(uintptr_t uSize) BURGER_NOEXCEPT;
extern void BURGER_API free_platform_memory(const void* pInput) BURGER_NOEXCEPT;
}
/* END */

#endif
