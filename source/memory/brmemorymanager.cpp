/***************************************

	Master Memory Manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemorymanager.h"
#include "brstringfunctions.h"
#include <stdlib.h>

/*! ************************************

	\struct Burger::MemoryManager
	\brief Base class for memory managers.
	
	To avoid the use of virtual pointers and
	to gain ANSI C compatibility, the
	"base class" has all the virtual functions
	defined explicitly, and calls to the base
	class are passed through the function pointers
	while calls to the derived classed (When known)
	are performed by direct calls.
	
	Since this is defined as a base class, it's not meant
	to be used directly. You should derive from this class 
	and either implement your own memory handler, or
	use the predefined \ref Burger::MemoryManagerANSI or 
	\ref Burger::MemoryManagerHandle classes.

***************************************/

/*! ************************************

	\fn void *Burger::MemoryManager::Alloc(WordPtr uSize)
	\brief Allocate memory.

	Call the "virtual" function in \ref m_pAlloc to allocate memory

	\param uSize Number of bytes to allocate
	\return Pointer to valid memory or \ref NULL on error or no memory requested.

	\sa Burger::MemoryManager::Free() or Burger::MemoryManager::AllocClear()
	
***************************************/

/*! ************************************

	\fn void Burger::MemoryManager::Free(const void *pInput)
	\brief Release memory.

	Call the "virtual" function in \ref m_pFree to allocate memory

	\param pInput Pointer to memory to release
	
	\sa Burger::MemoryManager::Alloc() or Burger::MemoryManager::AllocClear()

***************************************/

/*! ************************************

	\fn void *Burger::MemoryManager::Realloc(const void *pInput,WordPtr uSize)
	\brief Reallocate memory.

	Call the "virtual" function in \ref m_pRealloc to reallocate memory

	\param pInput Pointer to memory to read from and release
	\param uSize Number of bytes to allocate
	\return Pointer to valid memory or \ref NULL on error or no memory requested.

	\sa Burger::MemoryManager::Alloc() or Burger::MemoryManager::Free()
	
***************************************/

/*! ************************************

	\fn void Burger::MemoryManager::Shutdown(void)
	\brief Shut down the memory manager.

	Call the "virtual" function in \ref m_pShutdown to shut down the memory system

***************************************/

/*! ************************************

	\brief Allocate a block of pre-zeroed memory.

	Allocate a block of memory and return either \ref NULL
	in the case of an out of memory condition or if the amount
	requested was zero, otherwise return a valid pointer of memory
	that has been preset to zero.

	\param uSize Number of bytes to allocate.
	\return \ref NULL on error or a valid pointer to allocated memory.

	\sa Burger::MemoryManager::Alloc() or Burger::MemoryManager::Realloc()
	
***************************************/

void * BURGER_API Burger::MemoryManager::AllocClear(WordPtr uSize)
{
	// Get the function pointer into a register
	void *pResult = m_pAlloc(this,uSize);
	// Valid memory?
	if (pResult) {
		MemoryClear(pResult,uSize);
	}
	// Return NULL or a valid pointer.
	return pResult;
}

/*! ************************************

	\brief Default memory manager destructor.

	This function does nothing. It's intended to be
	a placeholder for derived memory managers that do not
	have shutdown functions (Such as those that have the OS
	or ANSI malloc/free to perform these operations)

	\param pThis The "this" pointer

***************************************/

void BURGER_API Burger::MemoryManager::Shutdown(MemoryManager * /*pThis */)
{
}

/*! ************************************

	\brief Allocate memory from the underlying operating system

	The handle based memory manager obtains the memory it
	controls from the operating system. Generic systems call malloc(),
	other systems call the low level functions directly

	\param uSize Number of bytes requested from the operating system
	\return Pointer to memory allocated from the operating system
	\sa Burger::MemoryManager::FreeSystemMemory(const void *)

***************************************/

#if !defined(BURGER_VITA) || defined(DOXYGEN)
void * BURGER_API Burger::MemoryManager::AllocSystemMemory(WordPtr uSize)
{
	return malloc(uSize);
}
#endif

/*! ************************************

	\brief Release memory back to the underlying operating system

	The handle based memory manager obtained the memory it
	controls from the operating system. This function releases
	the memory back. Generic systems call free(),
	other systems call the low level functions directly

	\param pInput Pointer to memory previously allocated by AllocSystemMemory(WordPtr)
	\sa Burger::MemoryManager::AllocSystemMemory(WordPtr uSize)

***************************************/

#if !defined(BURGER_VITA) || defined(DOXYGEN)
void BURGER_API Burger::MemoryManager::FreeSystemMemory(const void *pInput)
{
	if (pInput) {
		free(const_cast<void *>(pInput));
	}
}
#endif