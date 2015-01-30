/***************************************

	ANSI Based Memory Manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
class MemoryManagerANSI : public MemoryManager {
	BURGER_DISABLECOPYCONSTRUCTORS(MemoryManagerANSI);
public:
	MemoryManagerANSI();
	BURGER_INLINE void *Alloc(WordPtr uSize) { return Alloc(this,uSize); }
	BURGER_INLINE void Free(const void *pInput) { return Free(this,pInput); }
	BURGER_INLINE void *Realloc(const void *pInput,WordPtr uSize)  { return Realloc(this,pInput,uSize); }
private:
	static void *BURGER_API Alloc(MemoryManager *pThis,WordPtr uSize);
	static void BURGER_API Free(MemoryManager *pThis,const void *pInput);
	static void *BURGER_API Realloc(MemoryManager *pThis,const void *pInput,WordPtr uSize);
};
class MemoryManagerGlobalANSI : public MemoryManagerANSI {
	BURGER_DISABLECOPYCONSTRUCTORS(MemoryManagerGlobalANSI);
	MemoryManager *m_pPrevious;			///< Pointer to the previous memory manager
public:
	MemoryManagerGlobalANSI();
	~MemoryManagerGlobalANSI();
};
}

/* END */

#endif

