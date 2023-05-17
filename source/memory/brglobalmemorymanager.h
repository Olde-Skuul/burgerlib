/***************************************

	Global Master Memory Manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLOBALMEMORYMANAGER_H__
#define __BRGLOBALMEMORYMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRMEMORYMANAGER_H__
#include "brmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {

class GlobalMemoryManager {
private:
	/** Pointer to the currently active memory manager */
	static MemoryManager* g_pInstance;

public:
	static MemoryManager* BURGER_API init(
		MemoryManager* pInstance) BURGER_NOEXCEPT;
	static void BURGER_API shutdown(
		MemoryManager* pPrevious = nullptr) BURGER_NOEXCEPT;

	static BURGER_INLINE MemoryManager* get_instance(void) BURGER_NOEXCEPT
	{
		return g_pInstance;
	}
};

extern void* BURGER_API Alloc(uintptr_t uSize) BURGER_NOEXCEPT;
extern void BURGER_API Free(const void* pInput) BURGER_NOEXCEPT;
extern void* BURGER_API Realloc(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;

extern void* BURGER_API alloc_clear(uintptr_t uSize) BURGER_NOEXCEPT;
extern void* BURGER_API alloc_copy(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;

template<class T>
T* BURGER_API New(void) BURGER_NOEXCEPT
{
	void* pResult = Burger::Alloc(sizeof(T));
	if (pResult) {
		new (pResult) T();
	}
	return static_cast<T*>(pResult);
}

template<class T>
void BURGER_API Delete(const T* pInput) BURGER_NOEXCEPT
{
	if (pInput) {
		const_cast<T*>(pInput)->~T();
		Free(pInput);
	}
}
}

/* END */

#endif
