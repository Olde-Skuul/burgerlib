/***************************************

	Global Master Memory Manager

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::GlobalMemoryManager
	\brief Singleton class for global memory allocation.

	To implement a workable copy of malloc(), free() etc., a method was needed
	to contain the instance pointer of the Burgerlib memory manager and retain
	the ability to change the implementation at will. This was needed since
	Burgerlib Memory Managers are all class based so a "this" pointer is needed
	in all calls whereas the ANSI calling convention assumes a global
	implementation.

	By using this class, which is initialized either with
	\ref Burger::MemoryManagerANSI or \ref Burger::MemoryManagerHandle
	all functions that allocate memory from the global pool can and will be
	redirected.

	Mostly used to make search and replace of malloc and new trivial. Use of
	this class is not encouraged. It's only for quick porting of legacy code
	that isn't multi-thread friendly.

	\note This class is assumed to have functions that are thread safe.	Any
	custom implementations on target platforms that support multi-threading must
	all be multi-core thread safe.

	\note Since this a reference to the controlled class, NEVER manually up cast
	the global instance of this class. Use the get_instance(void)
	function to get the pointer to the real class if up casting is desired for
	system specific functionality is desired.

***************************************/

Burger::MemoryManager* Burger::GlobalMemoryManager::g_pInstance;

/*! ************************************

	\brief Initialize the global memory allocator

	This function copies the pointer to the MemoryManager class and will use
	this instance for all memory operations.

	\note The MemoryManager is not copied, so the class should not go out of
	scope without a call to either shutdown() or reset to another implementation

	\param pInstance Pointer to an initialized MemoryManager.

	\return Pointer to the previous version of the MemoryManager

	\sa shutdown(MemoryManager *)

***************************************/

Burger::MemoryManager* BURGER_API Burger::GlobalMemoryManager::init(
	MemoryManager* pInstance) BURGER_NOEXCEPT
{
	MemoryManager* pOldInstance = g_pInstance;
	g_pInstance = pInstance;
	return pOldInstance;
}

/*! ************************************

	\brief Shut down the global memory allocator

	Shut down the memory allocator through the
	\ref Burger::MemoryManager::m_pShutdown pointer.

	\note This call will zero out all entries in this class, so all future use
	of this class except for a call to \ref init(MemoryManager *)
	will result in an assert or page fault.

	\param pPrevious Pointer to an previous MemoryManager.

	\sa init(MemoryManager *)

***************************************/

void BURGER_API Burger::GlobalMemoryManager::shutdown(
	MemoryManager* pPrevious) BURGER_NOEXCEPT
{
	MemoryManager* pThis = g_pInstance;
	if (pThis) {
		pThis->m_pShutdown(pThis);
	}
	g_pInstance = pPrevious;
}

/*! ************************************

	\fn Burger::GlobalMemoryManager::get_instance(void)
	\brief Return the instance of the active global memory manager.

	\return Pointer to the current global memory manager, \ref nullptr if none
		is active.

***************************************/

/*! ************************************

	\brief Allocate memory

	Allocate memory using the \ref GlobalMemoryManager

	\param uSize Number of bytes requested to allocate

	\return \ref nullptr if no bytes are requested or an out of memory condition
		exists.

	\sa Burger::allocate_memory_clear() or Burger::free_memory()

***************************************/

void* BURGER_API Burger::allocate_memory(uintptr_t uSize) BURGER_NOEXCEPT
{
	return GlobalMemoryManager::get_instance()->allocate_memory(uSize);
}

/*! ************************************

	\brief Release memory

	Free memory using the Burger::GlobalMemoryManager. \ref nullptr performs no
	operation.

	\param pInput Pointer to memory to release.

	\sa Burger::allocate_memory()

***************************************/

void BURGER_API Burger::free_memory(const void* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		GlobalMemoryManager::get_instance()->free_memory(pInput);
	}
}

/*! ************************************

	\brief Reallocate previously allocated memory

	Reallocate memory using the Burger::GlobalMemoryManager. If the new buffer
	is smaller, the data is truncated. If the new buffer is larger, the extra
	memory is not initialized.

	\param pInput Pointer to memory to reallocate. \ref nullptr assumes no
		memory to copy from.
	\param uSize New size of the buffer.

	\return Pointer to the memory allocated with the data copied from the
		previous pointer. \ref nullptr if out of memory or no memory requested.

	\sa Burger::allocate_memory() or Burger::allocate_memory_copy()

***************************************/

void* BURGER_API Burger::reallocate_memory(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	return GlobalMemoryManager::get_instance()->reallocate_memory(pInput, uSize);
}

/*! ************************************

	\brief Allocate memory that is preinitialized to zero

	Allocate memory using the Burger::GlobalMemoryManager. If the allocation was
	successful, initialize all the memory to zero.

	\param uSize Number of bytes requested to allocate

	\return \ref nullptr if no bytes are requested or of an out of memory
		condition exists.

	\sa Burger::allocate_memory() or Burger::free_memory()

***************************************/

void* BURGER_API Burger::allocate_memory_clear(uintptr_t uSize) BURGER_NOEXCEPT
{
	void* pResult = GlobalMemoryManager::get_instance()->allocate_memory(uSize);
	if (pResult) {
		memory_clear(pResult, uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Allocate a buffer and copy data into it

	Allocate memory and copy the contents of the pointer to the new memory

	\param pInput Pointer to the data to copy. \ref nullptr will leave the data
		uninitialized
	\param uSize Size of the buffer to allocate.

	\return \ref nullptr on failure, a pointer with the data on success

	\sa Burger::allocate_memory() or Burger::reallocate_memory()

***************************************/

void* BURGER_API Burger::allocate_memory_copy(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	void* pOutput = nullptr;

	// Sanity check
	if (uSize) {
		pOutput = GlobalMemoryManager::get_instance()->allocate_memory(uSize);

		// Valid?
		if (pOutput) {
			// Anything to copy?
			if (pInput) {
				// Copy it!
				memory_copy(pOutput, pInput, uSize);
			}
		}
	}

	// Return the new memory
	return pOutput;
}

/*! ************************************

	\fn Burger::new_object(void)
	\brief Allocate a class instance

	Allocate memory with Burger::allocate_memory(uintptr_t) and invoke the default
	constructor on it.

	\return \ref nullptr on memory error or a valid pointer to a new class
		instance

	\sa Burger::delete_object(const T*)

***************************************/

/*! ************************************

	\fn Burger::delete_object(const T*)
	\brief Dispose of a generic class instance

	When Burger::new_object<T> is called, release the memory with this call

	\param pInput \ref nullptr or a valid pointer to a generic class to dispose
		of.

	\sa Burger::new_object<T>()

***************************************/
