/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemoryansi.h"
#include "brglobalmemorymanager.h"
#include <stdlib.h>

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::AllocatorANSI, Burger::AllocatorBase);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::AllocatorANSI::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a reference to the parent

***************************************/

/*! ************************************

	\class Burger::AllocatorANSI
	\brief ANSI Memory Manager object

	This class allocates and releases memory using the ANSI malloc(), free() and
	realloc() calls. No support is present to track memory. If memory tracking
	is desired either use native memory tracking or use \ref AllocatorHandle
	instead.

	Use this object to instruct functions that allocate memory to use
	malloc/free as the memory allocation system.

***************************************/

/*! ************************************

	\fn Burger::AllocatorANSI::AllocatorANSI()
	\brief Default constructor.

***************************************/

/*! ************************************

	\brief Calls malloc().

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	\param uSize Number of byte requested to allocate.
	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.
	\sa reallocate_memory() or free_memory()

***************************************/

void* Burger::AllocatorANSI::allocate_memory(uintptr_t uSize) const BURGER_NOEXCEPT
{
	void* pResult = nullptr;
	if (uSize) {
		pResult = ::malloc(uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Calls free().

	If pInput is \ref nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool with a call to free().

	\param pInput nullptr to do no operation or a valid pointer to memory
		allocated by allocate_memory() or reallocate_memory().

	\sa allocate_memory() or reallocate_memory()

***************************************/

void Burger::AllocatorANSI::free_memory(const void* pInput) const BURGER_NOEXCEPT
{
	if (pInput) {
		::free(const_cast<void*>(pInput));
	}
}

/*! ************************************

	\brief Calls realloc().

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	If the function fails, the buffer in pInput is untouched, even though the
	function returned nullptr. However, if the value in uSize is zero, the
	buffer is freed in all cases with a call to free().

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this in not guaranteed behavior. If
	larger, the buffer may be extended, and it too is not guaranteed to be the
	same pointer value. Assume the input pointer is invalided from here on out
	and the return value is the new pointer to the buffer.

	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.

	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa allocate_memory() or free_memory()

***************************************/

void* Burger::AllocatorANSI::reallocate_memory(
	const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT
{
	// Assume an error
	void* pResult = nullptr;

	// Any bytes requested?
	if (uSize) {

		// Was there a valid pointer passed?
		if (pInput) {

			// Reallocate the memory
			pResult = ::realloc(const_cast<void*>(pInput), uSize);
		} else {

			// Allocate fresh, uninitialized memory
			pResult = malloc(uSize);
		}
	} else if (pInput) {
		// Since I have a pointer, and I don't want to allocated anything, just
		// delete the pointer
		::free(const_cast<void*>(pInput));
	}

	// Return the new memory or nullptr
	return pResult;
}

/*! ************************************

	\class Burger::MemoryManagerANSI
	\brief ANSI Memory Manager

	This class allocates and releases memory using the ANSI malloc(), free() and
	realloc() calls. No support is present to track memory is supplied by
	Burgerlib. If memory tracking is desired either use native memory tracking
	or use \ref MemoryManagerHandle instead.

***************************************/

/*! ************************************

	\brief Constructor for the ANSI memory allocator

	Initializes the jump table in the base class \ref MemoryManager

***************************************/

Burger::MemoryManagerANSI::MemoryManagerANSI() BURGER_NOEXCEPT
{
	m_pAllocate = allocate_memory;
	m_pFree = free_memory;
	m_pReallocate = reallocate_memory;
	m_pShutdown = shutdown;
}

/*! ************************************

	\fn Burger::MemoryManagerANSI::allocate_memory(uintptr_t uSize)
	\brief Allocates memory.

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	\param uSize Number of byte requested to allocate.

	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa allocate_memory(MemoryManager *,uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::MemoryManagerANSI::free_memory(const void *pInput)
	\brief Frees memory.

	If pInput is nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pInput nullptr to do no operation or a valid pointer to memory
		allocated by \ref allocate_memory(uintptr_t) or \ref reallocate_memory(const void*,
		uintptr_t).

	\sa free_memory(MemoryManager *,const void *)

***************************************/

/*! ************************************

	\fn Burger::MemoryManagerANSI::reallocate_memory(const void *pInput,uintptr_t uSize)

	\brief Reallocates memory.

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this in not guaranteed behavior. If
	larger, the buffer may be extended, and it too is not guaranteed to be the
	same pointer value. Assume the input pointer is invalided from here on out
	and the return value is the new pointer to the buffer.

	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.

	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa reallocate_memory(MemoryManager *,const void *,uintptr_t)

***************************************/

/*! ************************************

	\brief Calls malloc().

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	\param pThis Pointer to the current instance.
	\param uSize Number of byte requested to allocate.

	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa allocate_memory(uintptr_t)

***************************************/

void* BURGER_API Burger::MemoryManagerANSI::allocate_memory(
	MemoryManager* /* pThis */, uintptr_t uSize) BURGER_NOEXCEPT
{
	void* pResult = nullptr;
	if (uSize) {
		pResult = ::malloc(uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Calls free().

	If pInput is nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pThis Pointer to the current instance.
	\param pInput nullptr to do no operation or a valid pointer to memory
		allocated by \ref allocate_memory(MemoryManager*,uintptr_t) or \ref
		reallocate_memory(MemoryManager*,const void *,uintptr_t).

	\sa free_memory(const void *)

***************************************/

void BURGER_API Burger::MemoryManagerANSI::free_memory(
	MemoryManager* /* pThis */, const void* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		::free(const_cast<void*>(pInput));
	}
}

/*! ************************************

	\brief Calls realloc().

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a nullptr is returned.

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this in not guaranteed behavior. If
	larger, the buffer may be extended, and it too it not guaranteed to be the
	same pointer value. Assume the input pointer is invalided from here on out
	and the return value is the new pointer to the buffer.

	\param pThis Pointer to the current instance.
	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.

	\return nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa reallocate_memory(const void *,uintptr_t)

***************************************/

void* BURGER_API Burger::MemoryManagerANSI::reallocate_memory(MemoryManager* /* pThis */,
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	// Assume an error
	void* pResult = nullptr;

	// Any bytes requested?
	if (uSize) {

		// Was there a valid pointer passed?
		if (pInput) {

			// Reallocate the memory
			pResult = ::realloc(const_cast<void*>(pInput), uSize);
		} else {

			// Allocate fresh, uninitialized memory
			pResult = ::malloc(uSize);
		}
	} else if (pInput) {

		// Since I have a pointer, and I don't want to allocated anything, just
		// delete the pointer
		::free(const_cast<void*>(pInput));
	}

	// Return the new memory or nullptr
	return pResult;
}

/*! ************************************

	\class Burger::MemoryManagerGlobalANSI
	\brief Global ANSI Memory Manager helper class

	This class is a helper that attaches a \ref MemoryManagerANSI
	class to the global memory manager. When this instance shuts down,
	it will remove itself from the global memory manager and
	set the global memory manager to the previous one.

	\sa \ref GlobalMemoryManager

***************************************/

/*! ************************************

	\brief Attaches a \ref MemoryManagerANSI class to the global memory manager.

	When this class is created, it will automatically attach itself
	to the global memory manager and save the pointer
	to the previous instantiation so when this class goes
	out of scope, the previous memory manager is reinstated

	Example...

	\code

	void DoSomething()
	{
		// Tell Burgerlib to use ANSI memory chunks
		MemoryManagerGlobalANSI MallocMemory;
		CallFunctionThatUsesMemory();
		CallFunctionThatCleansUpMemory();
		// When going out of scope, HandleBasedMemory is restored
	}

	int main(int,char **)
	{
		// Use handle based memory
		Burger::MemoryManagerGlobalHandle HandleBasedMemory;
		DoSomething();
		return 0;
	}

	\endcode

***************************************/

Burger::MemoryManagerGlobalANSI::MemoryManagerGlobalANSI() BURGER_NOEXCEPT
{
	m_pPrevious = GlobalMemoryManager::init(this);
}

/*! ************************************

	\brief Releases a \ref MemoryManagerANSI class from the global memory
		manager.

	When this class is released, it will automatically remove itself
	to the global memory manager and restore the previous memory
	manager to the global memory manager.

	\sa MemoryManagerGlobalANSI(), or \ref GlobalMemoryManager

***************************************/

Burger::MemoryManagerGlobalANSI::~MemoryManagerGlobalANSI()
{
	GlobalMemoryManager::shutdown(m_pPrevious);
}
