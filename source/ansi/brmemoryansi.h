/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMEMORYANSI_H__
#define __BRMEMORYANSI_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMEMORYMANAGER_H__
#include "brmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {

class AllocatorANSI: public AllocatorBase {
	BURGER_RTTI_IN_CLASS();

public:
	AllocatorANSI() BURGER_DEFAULT_CONSTRUCTOR;
	void* allocate_memory(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
	void free_memory(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL;
	void* reallocate_memory(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
};

class MemoryManagerANSI: public MemoryManager {
	BURGER_DISABLE_COPY(MemoryManagerANSI);

public:
	MemoryManagerANSI() BURGER_NOEXCEPT;

	BURGER_INLINE void* allocate_memory(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return allocate_memory(this, uSize);
	}

	BURGER_INLINE void free_memory(const void* pInput) BURGER_NOEXCEPT
	{
		return free_memory(this, pInput);
	}

	BURGER_INLINE void* reallocate_memory(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return reallocate_memory(this, pInput, uSize);
	}

protected:
	static void* BURGER_API allocate_memory(
		MemoryManager* pThis, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API free_memory(
		MemoryManager* pThis, const void* pInput) BURGER_NOEXCEPT;
	static void* BURGER_API reallocate_memory(MemoryManager* pThis,
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;
};

class MemoryManagerGlobalANSI: public MemoryManagerANSI {
	BURGER_DISABLE_COPY(MemoryManagerGlobalANSI);

protected:
	/** Pointer to the previous memory manager */
	MemoryManager* m_pPrevious;

public:
	MemoryManagerGlobalANSI() BURGER_NOEXCEPT;
	~MemoryManagerGlobalANSI();
};
}

/* END */

#endif
