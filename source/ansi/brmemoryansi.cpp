/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\sa ConsoleApp, MemoryManagerANSI, AllocatorBase, or AllocatorHandle

***************************************/

/*! ************************************

	\fn Burger::AllocatorANSI::AllocatorANSI()
	\brief Default constructor.

***************************************/

/*! ************************************

	\brief Calls malloc().

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref nullptr is returned.

	\param uSize Number of byte requested to allocate.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa reallocate_memory(const void*, uintptr_t) const,
		or free_memory(const void*) const

***************************************/

void* Burger::AllocatorANSI::allocate_memory(
	uintptr_t uSize) const BURGER_NOEXCEPT
{
	// Return nullptr if zero bytes
	void* pResult = nullptr;
	if (uSize) {
		// Get the memory
		pResult = ::malloc(uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Calls free().

	If pInput is \ref nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool with a call to free().

	\param pInput \ref nullptr will perform no operation or a valid pointer to
		memory allocated by allocate_memory(uintptr_t) const, or
		reallocate_memory(const void*, uintptr_t) const.

	\sa allocate_memory(uintptr_t) const,
		or reallocate_memory(const void*, uintptr_t) const

***************************************/

void Burger::AllocatorANSI::free_memory(
	const void* pInput) const BURGER_NOEXCEPT
{
	if (pInput) {
		::free(const_cast<void*>(pInput));
	}
}

/*! ************************************

	\brief Calls realloc().

	Calls realloc() and returns the pointer allocated. If the requested memory
	size is zero or the call fails, a \ref nullptr is returned. The data in the
	buffer is retained. If the buffer is extended, the newly allocated space is
	uninitialized.

	If the function fails, the buffer in pInput is untouched, even though the
	function returned \ref nullptr. However, if the value in uSize is zero, the
	buffer is freed in all cases with a call to free().

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this is not guaranteed behavior. If
	larger, the buffer may be extended, and it too is not guaranteed to be the
	same pointer value. Assume the input pointer is invalided and the return
	value is a new pointer to the buffer.

	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to for the new buffer size.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa allocate_memory(uintptr_t) const,
		or free_memory(const void*) const

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

	\note This class cannot be copied or moved.

	\sa AllocatorANSI, or MemoryManagerGlobalANSI

***************************************/

/*! ************************************

	\brief Constructor for the ANSI memory allocator

	Initializes the jump table in the base class \ref MemoryManager

***************************************/

Burger::MemoryManagerANSI::MemoryManagerANSI() BURGER_NOEXCEPT
{
	// Copy pointers to the class' static function pointers
	m_pAllocate = allocate_memory;
	m_pFree = free_memory;
	m_pReallocate = reallocate_memory;
	m_pShutdown = shutdown;
}

/*! ************************************

	\fn Burger::MemoryManagerANSI::allocate_memory(uintptr_t uSize)
	\brief Allocates memory.

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref nullptr is returned.

	\param uSize Number of byte requested to allocate.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa free_memory(const void*), or allocate_memory(MemoryManager*, uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::MemoryManagerANSI::free_memory(const void *pInput)
	\brief Frees memory.

	If pInput is \ref nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pInput \ref nullptr to do no operation or it is a valid pointer to
		memory allocated by \ref allocate_memory(uintptr_t) or \ref
		reallocate_memory(const void*, uintptr_t).

	\sa  allocate_memory(MemoryManager*, uintptr_t),
		or free_memory(MemoryManager*, const void*)

***************************************/

/*! ************************************

	\fn Burger::MemoryManagerANSI::reallocate_memory(
		const void* pInput,uintptr_t uSize)

	\brief Reallocates memory.

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref nullptr is returned.

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this in not guaranteed behavior. If
	larger, the buffer may be extended, and it too is not guaranteed to be the
	same pointer value. Assume the input pointer is invalided from here on out
	and the return value is the new pointer to the buffer.

	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa reallocate_memory(MemoryManager*, const void*, uintptr_t), or
		allocate_memory(uintptr_t)

***************************************/

/*! ************************************

	\brief Calls malloc().

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref nullptr is returned.

	\param pThis Pointer to the current instance.
	\param uSize Number of byte requested to allocate.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
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

	If pInput is \ref nullptr, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pThis Pointer to the current instance.
	\param pInput \ref nullptr to do no operation or a valid pointer to memory
		allocated by \ref allocate_memory(MemoryManager*, uintptr_t) or \ref
		reallocate_memory(MemoryManager*, const void*, uintptr_t).

	\sa free_memory(const void*)

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
	memory size is zero or the call fails, a \ref nullptr is returned.

	\note If the requested size is smaller, it's likely that the buffer is
	truncated and not changed, however this in not guaranteed behavior. If
	larger, the buffer may be extended, and it too it not guaranteed to be the
	same pointer value. Assume the input pointer is invalided from here on out
	and the return value is the new pointer to the buffer.

	\param pThis Pointer to the current instance.
	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.

	\return \ref nullptr on failure or zero bytes allocated, or a valid memory
		pointer.

	\sa reallocate_memory(const void*, uintptr_t)

***************************************/

void* BURGER_API Burger::MemoryManagerANSI::reallocate_memory(
	MemoryManager* /* pThis */, const void* pInput,
	uintptr_t uSize) BURGER_NOEXCEPT
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

	This class is a helper that attaches a \ref MemoryManagerANSI class to the
	global memory manager. When this instance shuts down, it will remove itself
	from the global memory manager and set the global memory manager to the
	previous one.

	\note This class cannot be copied or moved.

	\sa GlobalMemoryManager, or MemoryManagerANSI

***************************************/

/*! ************************************

	\brief Attaches a \ref MemoryManagerANSI class to the global memory manager.

	When this class is created, it will automatically attach itself to the
	global memory manager and save the pointer to the previous instantiation so
	when this class goes out of scope, the previous memory manager is reinstated

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

	int main(int, char**)
	{
		// Use handle based memory
		MemoryManagerGlobalHandle HandleBasedMemory;

		// Function uses it's own memory manager
		DoSomething();

		// HandleBasedMemory is active again
		return 0;
	}

	\endcode

***************************************/

Burger::MemoryManagerGlobalANSI::MemoryManagerGlobalANSI() BURGER_NOEXCEPT
{
	// Set my pointers and save the previous one
	m_pPrevious = GlobalMemoryManager::init(this);
}

/*! ************************************

	\brief Releases a \ref MemoryManagerANSI class from the global memory
		manager.

	When this class is released, it will automatically remove itself to the
	global memory manager and restore the previous memory manager to the global
	memory manager.

	\sa MemoryManagerGlobalANSI(), or GlobalMemoryManager

***************************************/

Burger::MemoryManagerGlobalANSI::~MemoryManagerGlobalANSI()
{
	// Restore the previous state
	GlobalMemoryManager::shutdown(m_pPrevious);
}
