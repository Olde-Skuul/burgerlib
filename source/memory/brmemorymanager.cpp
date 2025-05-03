/***************************************

	Master Memory Manager

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemorymanager.h"
#include "brmemoryfunctions.h"

#include <stdlib.h>

/*! ************************************

	\class Burger::allocator
	\brief Implementation of std::allocator<>

	Implements a standard allocator using a combination of functions from C++98
	through C++20. Allocates memory through ::operator new() and
	operator delete()

	\sa allocator<void>

***************************************/

/*! ************************************

	\fn Burger::allocator::allocate(uintptr_t n)
	\brief Allocate memory for n chunks of data.

	\param n Number of elements to allocate

	\returns Pointer to allocated memory or nullptr on failure.

	\sa deallocate(pointer, size_type)

***************************************/

/*! ************************************

	\fn Burger::allocator::deallocate(pointer p, size_type)
	\brief Free memory allocated.

	\note The second parameter is ignored in this allocator

	\param p Pointer to memory to release

	\sa allocate(uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::allocator::max_size() const
	\brief Maximum size possible to allocate.

	Returns the maximum number of elements, each of member type value_type (an
	alias of allocator's template parameter) that could potentially be allocated
	by a call to member allocate.

	\return UINTMAX_MAX

***************************************/

/*! ************************************

	\class Burger::allocator<void>
	\brief Implementation of std::allocator<void>

	Special type of allocator that has no ability to allocate or free any
	memory.

	\sa allocator<const void>, or allocator

***************************************/

/*! ************************************

	\class Burger::allocator<const void>
	\brief Implementation of std::allocator<void>

	Special type of allocator that has no ability to allocate or free any
	memory.

	\sa allocator<void>, or allocator

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::AllocatorBase, Burger::Base);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::AllocatorBase::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a reference to the parent

***************************************/

/*! ************************************

	\struct Burger::AllocatorBase
	\brief Base class for memory manager objects.

	To allow functions that allocate memory to use arbitrary memory allocation
	systems, this class instance is passed so functionality could be redirected
	to any memory allocated needed.

	Since this is defined as a base class, it's not meant to be created
	directly. Derive from this class to implement a custom memory handler, or
	use the predefined \ref AllocatorANSI or \ref AllocatorHandle classes.

	\sa AllocatorANSI or AllocatorHandle

***************************************/

/*! ************************************

	\fn Burger::AllocatorBase::AllocatorBase()
	\brief Default constructor.

***************************************/

/*! ************************************

	\fn void *Burger::AllocatorBase::allocate_memory(uintptr_t uSize) const
	\brief Allocate memory.

	Allocate memory. If the function fails, return \ref nullptr

	\param uSize Number of bytes to allocate

	\return Pointer to valid memory or \ref nullptr on error or no memory
		requested.

	\sa free_memory(const void*) const or allocate_memory_clear(uintptr_t) const

***************************************/

/*! ************************************

	\fn void Burger::AllocatorBase::free_memory(const void *pInput) const
	\brief Release memory.

	If \ref nullptr is passed, do nothing. Otherwise, release the memory.

	\note Only release memory allocated with this allocator object. Do not call
		this function with another classes' allocated memory.

	\param pInput Pointer to memory to release, or \ref nullptr.

	\sa allocate_memory(uintptr_t) const,
		or reallocate_memory(const void*, uintptr_t) const

***************************************/

/*! ************************************

	\fn Burger::AllocatorBase::reallocate_memory(
		const void *pInput,uintptr_t uSize) const

	\brief Reallocate memory.

	Call the virtual function to reallocate memory. If the function fails, the
	memory in pInput is left intact and \ref nullptr is returned. If uSize is
	zero, pInput is freed and \ref nullptr is returned.

	\note If nullptr is returned, assume the memory point is still valid, and
		dispose of it if it is not needed. Otherwise memory can leak.

	\param pInput Pointer to memory to read from and release
	\param uSize Number of bytes to allocate

	\return Pointer to valid memory or \ref nullptr on error or no memory
		requested.

	\sa allocate_memory(uintptr_t) const, or free_memory(const void*) const

***************************************/

/*! ************************************

	\brief Allocate memory that is preinitialized to zero

	Allocate memory using the call allocate_memory(uintptr_t) const. If the
	allocation was successful, initialize all the memory to zero.

	\param uSize Number of bytes requested to allocate

	\return \ref nullptr if no bytes are requested or if an out of memory
		condition exists.

	\sa allocate_memory(uintptr_t) const, allocate_memory_copy(const void*,
		uintptr_t) const, or free_memory(const void*) const

***************************************/

void* BURGER_API Burger::AllocatorBase::allocate_memory_clear(
	uintptr_t uSize) const BURGER_NOEXCEPT
{
	// Get the new memory
	void* pResult = allocate_memory(uSize);
	if (pResult) {
		// Clear it out if successful
		memory_clear(pResult, uSize);
	}
	return pResult;
}

/*! ************************************

	\brief Allocate a buffer and copy data into it

	Allocate memory and copy the contents of the pointer to the new memory. If
	uSize is zero, \ref nullptr is returned.

	\note If pInput is \ref nullptr and uSize is non-zero, the memory allocated
	will be uninitialized.

	\param pInput Pointer to the data to copy. \ref nullptr will leave the data
		uninitialized
	\param uSize Size of the buffer to allocate.

	\return \ref nullptr on failure, a pointer with the data on success

	\sa allocate_memory(uintptr_t) const,
		or reallocate_memory(const void*, uintptr_t) const

***************************************/

void* BURGER_API Burger::AllocatorBase::allocate_memory_copy(
	const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT
{
	void* pOutput = nullptr;

	// Sanity check
	if (uSize) {

		// Valid?
		pOutput = allocate_memory(uSize);

		// Anything to copy?
		if (pOutput && pInput) {

			// Copy it!
			memory_copy(pOutput, pInput, uSize);
		}
	}

	// Return the new memory
	return pOutput;
}

/*! ************************************

	\struct Burger::MemoryManager
	\brief Base class for memory managers.

	To avoid the use of virtual pointers and to gain ANSI C compatibility, the
	"base class" has all the virtual functions defined explicitly, and calls to
	the base class are passed through the function pointers while calls to the
	derived classed (When known) are performed by direct calls.

	Avoiding the use of C++ virtual pointers avoids a redirection when looking
	up the function pointer to the derived class.

	Since this is defined as a base class, it's not meant to be used directly.
	Derive from this class and either implement a custom memory handler, or use
	the predefined \ref MemoryManagerANSI or \ref MemoryManagerHandle classes.

***************************************/

/*! ************************************

	\fn Burger::MemoryManager::allocate_memory(uintptr_t uSize)
	\brief Allocate memory.

	Call the "virtual" function in \ref m_pAllocate to allocate memory

	\param uSize Number of bytes to allocate

	\return Pointer to valid memory or \ref nullptr on error or no memory
		requested.

	\sa free_memory(const void*) or allocate_memory_clear(uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::MemoryManager::free_memory(const void *pInput)
	\brief Release memory.

	Call the "virtual" function in \ref m_pFree to allocate memory

	\param pInput Pointer to memory to release

	\sa allocate_memory(uintptr_t) or allocate_memory_clear(uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::MemoryManager::reallocate_memory(
		const void *pInput,uintptr_t uSize)
	\brief Reallocate memory.

	Call the "virtual" function in \ref m_pReallocate to reallocate memory

	\param pInput Pointer to memory to read from and release
	\param uSize Number of bytes to allocate

	\return Pointer to valid memory or \ref nullptr on error or no memory
		requested.

	\sa allocate_memory(uintptr_t) or free_memory(const void*)

***************************************/

/*! ************************************

	\fn void Burger::MemoryManager::shutdown(void)
	\brief Shut down the memory manager.

	Call the "virtual" function in \ref m_pShutdown to shut down the memory
	system

	\sa shutdown(MemoryManager*)

***************************************/

/*! ************************************

	\brief Allocate a block of pre-zeroed memory.

	Allocate a block of memory and return either \ref nullptr in the case of an
	out of memory condition or if the amount requested was zero, otherwise
	return a valid pointer of memory that has been preset to zero.

	\param uSize Number of bytes to allocate.
	\return \ref nullptr on error or a valid pointer to allocated memory.

	\sa allocate_memory(uintptr_t) or reallocate_memory(const void*, uintptr_t)

***************************************/

void* BURGER_API Burger::MemoryManager::allocate_memory_clear(
	uintptr_t uSize) BURGER_NOEXCEPT
{
	// Get the function pointer into a register
	void* pResult = m_pAllocate(this, uSize);

	// Valid memory?
	if (pResult) {
		memory_clear(pResult, uSize);
	}

	// Return nullptr or a valid pointer.
	return pResult;
}

/*! ************************************

	\brief Default memory manager destructor.

	This function does nothing. It's intended to be	a placeholder for derived
	memory managers that do not have shutdown functions (Such as those that have
	the OS or ANSI malloc/free to perform these operations)

	\param pThis The "this" pointer

	\sa shutdown(void)

***************************************/

void BURGER_API Burger::MemoryManager::shutdown(
	MemoryManager* /*pThis */) BURGER_NOEXCEPT
{
}

/*! ************************************

	\brief Allocate memory from the underlying operating system

	The handle based memory manager obtains the memory it controls from the
	operating system. Generic systems call malloc(), other systems call the low
	level functions directly

	On MacOS, this calls NewPtr(), Windows calls HeapAlloc(), etc...

	On platforms that don't have operating system level calls for memory
	allocation, this function uses malloc() with glue code to handle cases of
	zero byte allocations always returning \ref nullptr.

	\param uSize Number of bytes requested from the operating system

	\return Pointer to memory allocated from the operating system

	\sa free_platform_memory(const void *)

***************************************/

#if !(defined(BURGER_MAC) || defined(BURGER_WINDOWS) || \
	defined(BURGER_XBOX) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
void* BURGER_API Burger::allocate_platform_memory(
	uintptr_t uSize) BURGER_NOEXCEPT
{
	if (uSize) {
		return ::malloc(uSize);
	}
	return nullptr;
}
#endif

/*! ************************************

	\brief Release memory back to the underlying operating system

	The handle based memory manager obtained the memory it controls from the
	operating system. This function releases the memory back. Generic systems
	call free(), other systems call the low level functions directly

	On MacOS, this calls DisposePtr(), Windows calls HeapFree(), etc...

	\param pInput Pointer to memory previously allocated by
		allocate_platform_memory(uintptr_t)

	\sa allocate_platform_memory(uintptr_t)

***************************************/

#if !(defined(BURGER_MAC) || defined(BURGER_WINDOWS) || \
	defined(BURGER_XBOX) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
void BURGER_API Burger::free_platform_memory(const void* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		::free(const_cast<void*>(pInput));
	}
}
#endif
