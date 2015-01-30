/***************************************

	Global Master Memory Manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLOBALMEMORYMANAGER_H__
#define __BRGLOBALMEMORYMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMEMORYMANAGER_H__
#include "brmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {

extern void *BURGER_API Alloc(WordPtr uSize);
extern void *BURGER_API AllocClear(WordPtr uSize);
extern void BURGER_API Free(const void *pInput);
extern void *BURGER_API Realloc(const void *pInput,WordPtr uSize);
extern void *BURGER_API AllocCopy(const void *pInput,WordPtr uSize);

class GlobalMemoryManager {
public:
	static MemoryManager * BURGER_API Init(MemoryManager *pInstance);
	static void BURGER_API Shutdown(MemoryManager *pPrevious=NULL);
	static BURGER_INLINE MemoryManager *GetInstance(void) { return g_pInstance; }
private:
	static MemoryManager *g_pInstance;	///< Pointer to the currently active memory manager
};
template <class T>
T * BURGER_API New(void) {
	return new (Alloc(sizeof(T))) T();
}

template <class T>
void BURGER_API Delete(const T*pInput) {
	if (pInput) {
		const_cast<T *>(pInput)->~T();
		Free(pInput);
	}
}
}

/* END */

#endif

