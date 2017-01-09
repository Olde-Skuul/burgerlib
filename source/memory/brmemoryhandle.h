/***************************************

	Handle based memory manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRMEMORYHANDLE_H__
#define __BRMEMORYHANDLE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMEMORYMANAGER_H__
#include "brmemorymanager.h"
#endif

#ifndef __BRCRITICALSECTION_H__
#include "brcriticalsection.h"
#endif

/* BEGIN */
namespace Burger {
class MemoryManagerHandle : public MemoryManager {
	BURGER_DISABLECOPYCONSTRUCTORS(MemoryManagerHandle);
public:
	enum {
		LOCKED=0x80,		///< Lock flag
		FIXED=0x40,			///< Set if memory is fixed (High memory)
		MALLOC=0x20,		///< Memory was Malloc'd
		INUSE=0x10,			///< Set if handle is used
		PURGABLE=0x01,		///< Set if handle is purgeable
		DEFAULTHANDLECOUNT=500,	///< Starting number of handles
		DEFAULTMEMORYCHUNK=0x1000000,	///< Default memory to allocate
		DEFAULTMINIMUMRESERVE=0x40000,	///< Default minimum free system memory
		MEMORYIDUNUSED=0xFFFDU,			///< Free handle ID
		MEMORYIDFREE=0xFFFEU,			///< Internal free memory ID
		MEMORYIDRESERVED=0xFFFFU,		///< Immutable handle ID
		// ALIGNMENT cannot be smaller than sizeof(void *)
#if defined(BURGER_MSDOS) || defined(BURGER_DS) || defined(BURGER_68K)
		ALIGNMENT=4			///< Default memory alignment
#else
		ALIGNMENT=16		///< Default memory alignment
#endif
	};
	enum eMemoryStage {
		StageCompact,		///< Garbage collection stage to compact memory
		StagePurge,			///< Garbage collection stage to purge purgeable memory
		StageHailMary,		///< Garbage collection stage to purge and then compact memory
		StageGiveup			///< Critical memory stage, release all possibly freeable memory
	};
	typedef void (BURGER_API *MemPurgeProc)(void *pThis,eMemoryStage eStage);	///< Function prototype for user supplied garbage collection subroutine
private:
	struct Handle_t {
		void *m_pData;				///< Pointer to true memory (Must be the first entry!)
		WordPtr m_uLength;			///< Length of allocated memory
		Handle_t *m_pNextHandle;	///< Next handle in the chain
		Handle_t *m_pPrevHandle;	///< Previous handle in the chain
		Handle_t *m_pNextPurge;		///< Next handle in purge list
		Handle_t *m_pPrevPurge;		///< Previous handle in the purge list
		Word m_uFlags;				///< Memory flags or parent used handle
		Word m_uID;					///< Memory ID
	};
	struct SystemBlock_t {
		SystemBlock_t *m_pNext;		///< Next block in the chain
	};
	SystemBlock_t *m_pSystemMemoryBlocks;	///< Linked list of memory blocks taken from the system
	MemPurgeProc m_MemPurgeCallBack;	///< Callback before memory purging
	void *m_pMemPurge;					///< User pointer for memory purge
	WordPtr m_uTotalAllocatedMemory;	///< All of the memory currently allocated
	WordPtr m_uTotalSystemMemory;		///< Total allocated system memory
	Handle_t *m_pFreeHandle;			///< Pointer to the free handle list
	Word m_uTotalHandleCount;			///< Number of handles allocated

	Handle_t m_LowestUsedMemory;	///< First used memory handle (Start of linked list)
	Handle_t m_HighestUsedMemory;	///< Last used memory handle (End of linked list)
	Handle_t m_FreeMemoryChunks;	///< Free handle list
	Handle_t m_PurgeHands;			///< Purged handle list
	Handle_t m_PurgeHandleFiFo;		///< Purged handle linked list
	CriticalSection m_Lock;			///< Lock for multithreading support
	static void *BURGER_API AllocProc(MemoryManager *pThis,WordPtr uSize);
	static void BURGER_API FreeProc(MemoryManager *pThis,const void *pInput);
	static void *BURGER_API ReallocProc(MemoryManager *pThis,const void *pInput,WordPtr uSize);
	static void BURGER_API ShutdownProc(MemoryManager *pThis);
	Handle_t *BURGER_API AllocNewHandle(void);
	void BURGER_API GrabMemoryRange(void *pData,WordPtr uLength,Handle_t *pParent,Handle_t *pHandle);
	void BURGER_API ReleaseMemoryRange(void *pData,WordPtr uLength,Handle_t *pParent);
	void BURGER_API PrintHandles(const Handle_t *pFirst,const Handle_t *pLast,Word bNoCheck);
public:
	MemoryManagerHandle(WordPtr uDefaultMemorySize=DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=DEFAULTMINIMUMRESERVE);
	~MemoryManagerHandle();
	BURGER_INLINE WordPtr GetTotalAllocatedMemory(void) const { return m_uTotalAllocatedMemory; }
	BURGER_INLINE void *Alloc(WordPtr uSize) { return AllocProc(this,uSize); }
	BURGER_INLINE void Free(const void *pInput) { return FreeProc(this,pInput); }
	BURGER_INLINE void *Realloc(const void *pInput,WordPtr uSize) { return ReallocProc(this,pInput,uSize); }
	BURGER_INLINE void Shutdown(void) { ShutdownProc(this); }
	void **BURGER_API AllocHandle(WordPtr uSize,Word uFlags=0);
	void BURGER_API FreeHandle(void **ppInput);
	void **BURGER_API ReallocHandle(void **ppInput,WordPtr uSize);
	void **BURGER_API RefreshHandle(void **ppInput);
	void **BURGER_API FindHandle(const void *pInput);
	static WordPtr BURGER_API GetSize(void **ppInput);
	static WordPtr BURGER_API GetSize(const void *pInput);
	WordPtr BURGER_API GetTotalFreeMemory(void);
	static void * BURGER_API Lock(void **ppInput);
	static void BURGER_API Unlock(void **ppInput);
	static void BURGER_API SetID(void **ppInput,Word uID);
	void BURGER_API SetPurgeFlag(void **ppInput,Word uFlag);
	static Word BURGER_API GetLockedState(void **ppInput);
	void BURGER_API SetLockedState(void **ppInput,Word uFlag);
	void BURGER_API Purge(void **ppInput);
	Word BURGER_API PurgeHandles(WordPtr uSize);
	void BURGER_API CompactHandles(void);
	void BURGER_API DumpHandles(void);
};
class MemoryManagerGlobalHandle : public MemoryManagerHandle {
	BURGER_DISABLECOPYCONSTRUCTORS(MemoryManagerGlobalHandle);
	MemoryManager *m_pPrevious;			///< Pointer to the previous memory manager
public:
	MemoryManagerGlobalHandle(WordPtr uDefaultMemorySize=DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=DEFAULTMINIMUMRESERVE);
	~MemoryManagerGlobalHandle();
};
}
/* END */

#endif

