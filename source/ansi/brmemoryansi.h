/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	void* alloc(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
	void free(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL;
	void* realloc(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
};

class MemoryManagerANSI: public MemoryManager {
	BURGER_DISABLE_COPY(MemoryManagerANSI);

public:
	MemoryManagerANSI() BURGER_NOEXCEPT;

	BURGER_INLINE void* alloc(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return alloc(this, uSize);
	}

	BURGER_INLINE void free(const void* pInput) BURGER_NOEXCEPT
	{
		return free(this, pInput);
	}

	BURGER_INLINE void* realloc(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return realloc(this, pInput, uSize);
	}

protected:
	static void* BURGER_API alloc(
		MemoryManager* pThis, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API free(
		MemoryManager* pThis, const void* pInput) BURGER_NOEXCEPT;
	static void* BURGER_API realloc(MemoryManager* pThis, const void* pInput,
		uintptr_t uSize) BURGER_NOEXCEPT;
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
