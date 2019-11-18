/***************************************

	Memory Manager Base Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMEMORYMANAGER_H__
#define __BRMEMORYMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
struct MemoryManager {
	typedef void *(BURGER_API *ProcAlloc)(MemoryManager *pThis,uintptr_t uSize);		///< Function prototype for allocating memory
	typedef void (BURGER_API *ProcFree)(MemoryManager *pThis,const void *pInput);	///< Function prototype for releasing memory
	typedef void *(BURGER_API *ProcRealloc)(MemoryManager *pThis,const void *pInput, uintptr_t uSize);	///< Function prototype for reallocating memory.
	typedef void (BURGER_API *ProcShutdown)(MemoryManager *pThis);					///< Function prototype for destructor
	ProcAlloc m_pAlloc;			///< Pointer to allocation function
	ProcFree m_pFree;			///< Pointer to memory release function
	ProcRealloc m_pRealloc;		///< Pointer to the memory reallocation function
	ProcShutdown m_pShutdown;	///< Pointer to the shutdown function
	BURGER_INLINE void *Alloc(uintptr_t uSize) { return m_pAlloc(this,uSize); }
	BURGER_INLINE void Free(const void *pInput) { return m_pFree(this,pInput); }
	BURGER_INLINE void *Realloc(const void *pInput, uintptr_t uSize) { return m_pRealloc(this,pInput,uSize); }
	BURGER_INLINE void Shutdown(void) { m_pShutdown(this); }
	void * BURGER_API AllocClear(uintptr_t uSize);
	static void BURGER_API Shutdown(MemoryManager *pThis);
	static void * BURGER_API AllocSystemMemory(uintptr_t uSize);
	static void BURGER_API FreeSystemMemory(const void *pInput);
};
}
/* END */

#endif

