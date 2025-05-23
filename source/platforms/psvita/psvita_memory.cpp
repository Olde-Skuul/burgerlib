/***************************************

	Memory manager for Playstation Vita

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#if !defined(DOXYGEN)
#define BURGER_NONEW
#endif

#include "brmemoryfunctions.h"

#if defined(BURGER_VITA)
#include "brmemorymanager.h"
#include "psvita_memory.h"

#include <kernel.h>
#include <new>
#include <stdlib.h>

/***************************************

	Initialize the Playstation Vita memory
	system

***************************************/

void user_malloc_init(void) {}

void user_malloc_finalize(void) {}

void user_malloc_for_tls_init(void) {}

void user_malloc_for_tls_finalize(void) {}

void* user_malloc(size_t size)
{
	return Burger::allocate_platform_memory(size);
}

void user_free(void* ptr)
{
	Burger::free_platform_memory(ptr);
}

void* user_calloc(size_t nelem, size_t size)
{
	uintptr_t uTotal = nelem * size;
	void* pResult = Burger::allocate_platform_memory(uTotal);
	if (pResult) {
		Burger::memory_clear(pResult, uTotal);
	}
	return pResult;
}

void* user_realloc(void* ptr, size_t size)
{
	void* pResult = Burger::allocate_platform_memory(size);
	if (pResult) {
		if (ptr) {
			uintptr_t uChunk = size;
			SceKernelMemBlockInfo Info;
			Info.size = sizeof(Info);
			if (sceKernelGetMemBlockInfoByAddr(ptr, &Info) >= SCE_OK) {
				if (uChunk >= Info.mappedSize) {
					uChunk = Info.mappedSize;
				}
				Burger::memory_copy(pResult, ptr, uChunk);
			}
		}
	}
	if (ptr) {
		user_free(ptr);
	}
	return pResult;
}

void* user_memalign(size_t /* boundary */, size_t size)
{
	return user_malloc(size);
}

void* user_reallocalign(void* ptr, size_t size, size_t boundary)
{
	return user_realloc(ptr, size);
}

int user_malloc_stats(struct malloc_managed_size* mmsize)
{
	Burger::memory_clear(mmsize, sizeof(malloc_managed_size));
	return 0;
}

int user_malloc_stats_fast(struct malloc_managed_size* mmsize)
{
	Burger::memory_clear(mmsize, sizeof(malloc_managed_size));
	return 0;
}

size_t user_malloc_usable_size(void* ptr)
{
	return 0;
}

void* user_malloc_for_tls(size_t size)
{
	return Burger::allocate_platform_memory(size);
}

void user_free_for_tls(void* ptr)
{
	Burger::free_platform_memory(ptr);
}

void* user_new(std::size_t size) throw(std::bad_alloc)
{
	// Can't allocate zero bytes
	if (!size) {
		size = 1;
	}

	void* ptr;
	while ((ptr = Burger::allocate_platform_memory(size)) == nullptr) {
		// If an error occurred, attempt to recover
		std::new_handler handler = std::_get_new_handler();
		if (!handler) {
			break;
		}
		(*handler)();
	}
	return ptr;
}

void* user_new(std::size_t size, const std::nothrow_t& /* x */) throw()
{
	// Can't allocate zero bytes
	if (!size) {
		size = 1;
	}

	void* ptr;
	while ((ptr = Burger::allocate_platform_memory(size)) == nullptr) {
		// If an error occurred, attempt to recover
		std::new_handler handler = std::_get_new_handler();
		if (!handler) {
			break;
		}
		(*handler)();
	}
	return ptr;
}

void* user_new_array(std::size_t size) throw(std::bad_alloc)
{
	return user_new(size);
}

void* user_new_array(std::size_t size, const std::nothrow_t& x) throw()
{
	return user_new(size, x);
}

void user_delete(void* ptr) throw()
{
	Burger::free_platform_memory(ptr);
}

void user_delete(void* ptr, const std::nothrow_t& /* x */) throw()
{
	Burger::free_platform_memory(ptr);
}

void user_delete_array(void* ptr) throw()
{
	Burger::free_platform_memory(ptr);
}

void user_delete_array(void* ptr, const std::nothrow_t& x) throw()
{
	Burger::free_platform_memory(ptr);
}

/***************************************

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

void* BURGER_API Burger::allocate_platform_memory(
	uintptr_t uSize) BURGER_NOEXCEPT
{
	void* pData = nullptr;
	if (uSize) {
		// Blocks must be in 4K chunks
		// Why? Because!
		uSize = (uSize + 0xFFFU) & (~0xFFFU);
		SceUID ID = sceKernelAllocMemBlock(
			"BurgerAlloc", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, uSize, nullptr);
		if (ID >= SCE_OK) {
			if (sceKernelGetMemBlockBase(ID, &pData) < SCE_OK) {
				// Error!?!
				pData = nullptr;
				sceKernelFreeMemBlock(ID);
			}
		}
	}
	return pData;
}

/***************************************

	\brief Release memory back to the underlying operating system

	The handle based memory manager obtained the memory it controls from the
	operating system. This function releases the memory back. Generic systems
	call free(), other systems call the low level functions directly

	On MacOS, this calls DisposePtr(), Windows calls HeapFree(), etc...

	\param pInput Pointer to memory previously allocated by
		allocate_platform_memory(uintptr_t)

	\sa allocate_platform_memory(uintptr_t)

***************************************/

void BURGER_API Burger::free_platform_memory(const void* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		// Find the memory chunk
		SceUID ID = sceKernelFindMemBlockByAddr(const_cast<void*>(pInput), 0);
		if (ID >= SCE_OK) {
			// Release the chunk
			sceKernelFreeMemBlock(ID);
		}
	}
}

#endif
