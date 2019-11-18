/***************************************

	Memory manager for Playstation Vita

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#if !defined(DOXYGEN)
#define BURGER_NONEW
#endif
#include "brvitamemory.h"

#if defined(BURGER_VITA)
#include "brmemorymanager.h"
#include "brmemoryfunctions.h"
#include <stdlib.h>
#include <kernel.h>
#include <new>

/***************************************

	Initialize the Playstation Vita memory
	system

***************************************/

void user_malloc_init(void)
{
}

void user_malloc_finalize(void)
{
}

void user_malloc_for_tls_init(void)
{
}

void user_malloc_for_tls_finalize(void)
{
}

void *user_malloc(size_t size)
{
	return Burger::MemoryManager::AllocSystemMemory(size);
}

void user_free(void *ptr)
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

void *user_calloc(size_t nelem,size_t size)
{
	WordPtr uTotal = nelem*size;
	void *pResult = Burger::MemoryManager::AllocSystemMemory(uTotal);
	if (pResult) {
		Burger::MemoryClear(pResult,uTotal);
	}
	return pResult;
}

void *user_realloc(void *ptr,size_t size)
{
	void *pResult = Burger::MemoryManager::AllocSystemMemory(size);
	if (pResult) {
		if (ptr) {
			WordPtr uChunk = size;
			SceKernelMemBlockInfo Info;
			Info.size = sizeof(Info);
			if (sceKernelGetMemBlockInfoByAddr(ptr,&Info)>=SCE_OK) {
				if (uChunk >= Info.mappedSize) {
					uChunk = Info.mappedSize;
				}
				Burger::MemoryCopy(pResult,ptr,uChunk);
			}
		}
	}
	if (ptr) {
		user_free(ptr);
	}
	return pResult;
}

void *user_memalign(size_t /* boundary */, size_t size)
{
	return user_malloc(size);
}

void *user_reallocalign(void *ptr, size_t size, size_t boundary)
{
	return user_realloc(ptr,size);
}

int user_malloc_stats(struct malloc_managed_size *mmsize)
{
	Burger::MemoryClear(mmsize,sizeof(malloc_managed_size));
	return 0;
}

int user_malloc_stats_fast(struct malloc_managed_size *mmsize)
{
	Burger::MemoryClear(mmsize,sizeof(malloc_managed_size));
	return 0;
}

size_t user_malloc_usable_size(void *ptr)
{
	return 0;
}

void *user_malloc_for_tls(size_t size)
{
	return Burger::MemoryManager::AllocSystemMemory(size);
}

void user_free_for_tls(void *ptr)
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

void *user_new(std::size_t size) throw(std::bad_alloc)
{
	// Can't allocate zero bytes
	if (size == 0) {
		size = 1;
	}

	void *ptr;
	while ((ptr = Burger::MemoryManager::AllocSystemMemory(size)) == NULL) {
		// If an error occurred, attempt to recover
		std::new_handler handler = std::_get_new_handler();
		if (!handler) {
			break;
		}
		(*handler)();
	}
	return ptr;
}

void *user_new(std::size_t size, const std::nothrow_t& /* x */) throw()
{
	// Can't allocate zero bytes
	if (size == 0) {
		size = 1;
	}

	void *ptr;
	while ((ptr = Burger::MemoryManager::AllocSystemMemory(size)) == NULL) {
		// If an error occurred, attempt to recover
		std::new_handler handler = std::_get_new_handler();
		if (!handler) {
			break;
		}
		(*handler)();
	}
	return ptr;
}

void *user_new_array(std::size_t size) throw(std::bad_alloc)
{
	return user_new(size);
}

void *user_new_array(std::size_t size, const std::nothrow_t& x) throw()
{
	return user_new(size,x);
}

void user_delete(void *ptr) throw()
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

void user_delete(void *ptr, const std::nothrow_t& /* x */) throw()
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

void user_delete_array(void *ptr) throw()
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

void user_delete_array(void *ptr, const std::nothrow_t& x) throw()
{
	Burger::MemoryManager::FreeSystemMemory(ptr);
}

/***************************************
	
	Allocate Vita system memory

***************************************/

void * BURGER_API Burger::MemoryManager::AllocSystemMemory(WordPtr uSize)
{
	void *pData = NULL;
	if (uSize) {
		// Blocks must be in 4K chunks
		uSize = (uSize+0xFFF)&(~0xFFF);
		SceUID ID = sceKernelAllocMemBlock("BurgerAlloc",SCE_KERNEL_MEMBLOCK_TYPE_USER_RW,uSize,NULL);
		if (ID>=SCE_OK) {
			if (sceKernelGetMemBlockBase(ID,&pData)<SCE_OK) {
				// Error!?!
				pData = NULL;
				sceKernelFreeMemBlock(ID);
			}
		}
	}
	return pData;
}

/***************************************
	
	Handle freeing Vita system memory

***************************************/

void BURGER_API Burger::MemoryManager::FreeSystemMemory(const void *pInput)
{
	if (pInput) {
		// Find the memory chunk
		SceUID ID = sceKernelFindMemBlockByAddr(const_cast<void *>(pInput),0);
		if (ID>=SCE_OK) {
			// Release the chunk
			sceKernelFreeMemBlock(ID);
		}
	}
}

#endif