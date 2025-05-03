/***************************************

	Master Memory Manager, Windows version

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmemorymanager.h"

#if defined(BURGER_WINDOWS)
#include "win_windows.h"

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

void* BURGER_API Burger::allocate_platform_memory(uintptr_t uSize) BURGER_NOEXCEPT
{
	if (uSize) {
		// Note: GetProcessHeap() is an accessor
		return HeapAlloc(GetProcessHeap(), 0, uSize);
	}
	return nullptr;
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
		// Note: GetProcessHeap() is an accessor
		HeapFree(GetProcessHeap(), 0, const_cast<void*>(pInput));
	}
}

#endif
