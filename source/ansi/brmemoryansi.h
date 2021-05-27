/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	void* Alloc(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
	void Free(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL;
	void* Realloc(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL;
};

class MemoryManagerANSI: public MemoryManager {
	BURGER_DISABLE_COPY(MemoryManagerANSI);

public:
	MemoryManagerANSI() BURGER_NOEXCEPT;
	BURGER_INLINE void* Alloc(uintptr_t uSize)
	{
		return Alloc(this, uSize);
	}
	BURGER_INLINE void Free(const void* pInput)
	{
		return Free(this, pInput);
	}
	BURGER_INLINE void* Realloc(const void* pInput, uintptr_t uSize)
	{
		return Realloc(this, pInput, uSize);
	}

protected:
	static void* BURGER_API Alloc(MemoryManager* pThis, uintptr_t uSize);
	static void BURGER_API Free(MemoryManager* pThis, const void* pInput);
	static void* BURGER_API Realloc(
		MemoryManager* pThis, const void* pInput, uintptr_t uSize);
};

class MemoryManagerGlobalANSI: public MemoryManagerANSI {
	BURGER_DISABLE_COPY(MemoryManagerGlobalANSI);

	MemoryManager* m_pPrevious; ///< Pointer to the previous memory manager
public:
	MemoryManagerGlobalANSI() BURGER_NOEXCEPT;
	~MemoryManagerGlobalANSI();
};
}

/* END */

#endif
