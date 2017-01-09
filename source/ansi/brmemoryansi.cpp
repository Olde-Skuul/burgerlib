/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemoryansi.h"
#include "brglobalmemorymanager.h"
#include <stdlib.h>

/*! ************************************

	\class Burger::MemoryManagerANSI
	\brief ANSI Memory Manager
	
	This class allocates and releases memory using the ANSI
	malloc(), free() and realloc() calls. No support is
	present to track memory is supplied by burgerlib. If memory
	tracking is desired either use native memory tracking or
	use \ref MemoryManagerHandle instead.

***************************************/

/*! ************************************

	\brief Constructor for the ANSI memory allocator

	Initializes the jump table in the base class \ref MemoryManager

***************************************/

Burger::MemoryManagerANSI::MemoryManagerANSI()
{
	m_pAlloc = Alloc;
	m_pFree = Free;
	m_pRealloc = Realloc;
	m_pShutdown = Shutdown;
}

/*! ************************************

	\fn void *Burger::MemoryManagerANSI::Alloc(WordPtr uSize)
	\brief Allocates memory.

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref NULL is returned.

	\param uSize Number of byte requested to allocate.
	\return \ref NULL on failure or zero bytes allocated, or a valid memory pointer.
	\sa Alloc(MemoryManager *,WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::MemoryManagerANSI::Free(const void *pInput)
	\brief Frees memory.

	If pInput is \ref NULL, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pInput \ref NULL to do no operation or a valid pointer to memory allocated by \ref Alloc(WordPtr)
	or \ref Realloc(const void *,WordPtr).
	\sa Free(MemoryManager *,const void *)

***************************************/

/*! ************************************

	\fn void *Burger::MemoryManagerANSI::Realloc(const void *pInput,WordPtr uSize)
	\brief Reallocates memory.

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref NULL is returned.

	\note If the requested size is smaller, it's likely that the buffer is truncated 
	and not changed, however this in not guaranteed behavior. If larger, the buffer may
	be extended, and it too it not guaranteed to be the same pointer value. Assume the input
	pointer is invalided from here on out and the return value is the new pointer to the buffer.

	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.
	\return \ref NULL on failure or zero bytes allocated, or a valid memory pointer.
	\sa Realloc(MemoryManager *,const void *,WordPtr)

***************************************/

/*! ************************************

	\brief Calls malloc().

	Calls malloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref NULL is returned.

	\param pThis Pointer to the current instance.
	\param uSize Number of byte requested to allocate.
	\return \ref NULL on failure or zero bytes allocated, or a valid memory pointer.
	\sa Alloc(WordPtr)

***************************************/

void * BURGER_API Burger::MemoryManagerANSI::Alloc(MemoryManager * /* pThis */,WordPtr uSize)
{
	void *pResult = NULL;
	if (uSize) {
		pResult = malloc(uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Calls free().

	If pInput is \ref NULL, do nothing. If non-zero, then release the memory
	back into the free memory pool.

	\param pThis Pointer to the current instance.
	\param pInput \ref NULL to do no operation or a valid pointer to memory allocated by \ref Alloc(MemoryManager*,WordPtr)
	or \ref Realloc(MemoryManager *,const void *,WordPtr).
	\sa Free(const void *)

***************************************/

void BURGER_API Burger::MemoryManagerANSI::Free(MemoryManager * /* pThis */,const void *pInput)
{
	if (pInput) {
		free(const_cast<void *>(pInput));
	}
}

/*! ************************************

	\brief Calls realloc().

	Calls realloc() and returns the pointer allocated. If the requested
	memory size is zero or the call fails, a \ref NULL is returned.

	\note If the requested size is smaller, it's likely that the buffer is truncated 
	and not changed, however this in not guaranteed behavior. If larger, the buffer may
	be extended, and it too it not guaranteed to be the same pointer value. Assume the input
	pointer is invalided from here on out and the return value is the new pointer to the buffer.

	\param pThis Pointer to the current instance.
	\param pInput Pointer to a valid buffer to resize.
	\param uSize Number of byte requested to allocate.
	\return \ref NULL on failure or zero bytes allocated, or a valid memory pointer.
	\sa Realloc(const void *,WordPtr)

***************************************/

void * BURGER_API Burger::MemoryManagerANSI::Realloc(MemoryManager * /* pThis */,const void *pInput,WordPtr uSize)
{
	// Assume an error
	void *pResult = NULL;
	// Any bytes requested?
	if (uSize) {
		// Was there a valid pointer passed?
		if (pInput) {
			// Reallocate the memory
			pResult = realloc(const_cast<void *>(pInput),uSize);
		} else {
			// Allocate fresh, uninitialized memory
			pResult = malloc(uSize);
		}
	} else if (pInput) {
		// Since I have a pointer, and I don't want to allocated anything, just delete the pointer
		free(const_cast<void *>(pInput));
	}
	// Return the new memory or NULL
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

	Example..

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

Burger::MemoryManagerGlobalANSI::MemoryManagerGlobalANSI()
{
	m_pPrevious = GlobalMemoryManager::Init(this);
}

/*! ************************************

	\brief Releases a \ref MemoryManagerANSI class from the global memory manager.

	When this class is released, it will automatically remove itself
	to the global memory manager and restore the previous memory
	manager to the global memory manager.

	\sa MemoryManagerGlobalANSI()

***************************************/

Burger::MemoryManagerGlobalANSI::~MemoryManagerGlobalANSI()
{
	GlobalMemoryManager::Shutdown(m_pPrevious);
}
