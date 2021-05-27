/***************************************

	Handle based memory manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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
class MemoryManagerHandle: public MemoryManager {
	BURGER_DISABLE_COPY(MemoryManagerHandle);

public:
	enum {
		LOCKED = 0x80,            ///< Lock flag
		FIXED = 0x40,             ///< Set if memory is fixed (High memory)
		MALLOC = 0x20,            ///< Memory was Malloc'd
		INUSE = 0x10,             ///< Set if handle is used
		PURGABLE = 0x01,          ///< Set if handle is purgeable
		DEFAULTHANDLECOUNT = 500, ///< Starting number of handles
		DEFAULTMEMORYCHUNK = 0x1000000,  ///< Default memory to allocate
		DEFAULTMINIMUMRESERVE = 0x40000, ///< Default minimum free system memory
		MEMORYIDUNUSED = 0xFFFDU,        ///< Free handle ID
		MEMORYIDFREE = 0xFFFEU,          ///< Internal free memory ID
		MEMORYIDRESERVED = 0xFFFFU,      ///< Immutable handle ID
	// ALIGNMENT cannot be smaller than sizeof(void *)
#if defined(BURGER_MSDOS) || defined(BURGER_DS) || defined(BURGER_68K)
		ALIGNMENT = 4 ///< Default memory alignment
#else
		ALIGNMENT = 16 ///< Default memory alignment
#endif
	};
	enum eMemoryStage {
		/** Garbage collection stage to compact memory */
		StageCompact,
		/** Garbage collection stage to purge purgeable memory */
		StagePurge,
		/** Garbage collection stage to purge and then compact memory */
		StageHailMary,
		/** Critical memory stage, release all possibly releasable memory */
		StageGiveup

	};
	/** Function prototype for user supplied garbage collection subroutine */
	typedef void(BURGER_API* MemPurgeProc)(void* pThis, eMemoryStage eStage);

private:
	struct Handle_t {
		void* m_pData; ///< Pointer to true memory (Must be the first entry!)
		uintptr_t m_uLength;     ///< Length of allocated memory
		Handle_t* m_pNextHandle; ///< Next handle in the chain
		Handle_t* m_pPrevHandle; ///< Previous handle in the chain
		Handle_t* m_pNextPurge;  ///< Next handle in purge list
		Handle_t* m_pPrevPurge;  ///< Previous handle in the purge list
		uint_t m_uFlags;         ///< Memory flags or parent used handle
		uint_t m_uID;            ///< Memory ID
	};
	struct SystemBlock_t {
		SystemBlock_t* m_pNext; ///< Next block in the chain
	};
	/** Linked list of memory blocks taken from the system */
	SystemBlock_t* m_pSystemMemoryBlocks;
	/** Callback before memory purging */
	MemPurgeProc m_MemPurgeCallBack;
	/** User pointer for memory purge */
	void* m_pMemPurge;
	/** All of the memory currently allocated */
	uintptr_t m_uTotalAllocatedMemory;
	/** Total allocated system memory */
	uintptr_t m_uTotalSystemMemory;
	/** Pointer to the free handle list */
	Handle_t* m_pFreeHandle;
	/** Number of handles allocated */
	uint_t m_uTotalHandleCount;
	/** First used memory handle (Start of linked list) */
	Handle_t m_LowestUsedMemory;
	/** Last used memory handle (End of linked list) */
	Handle_t m_HighestUsedMemory;
	/** Free handle list */
	Handle_t m_FreeMemoryChunks;
	/** Purged handle list */
	Handle_t m_PurgeHands;
	/** Purged handle linked list */
	Handle_t m_PurgeHandleFiFo;
	/** Lock for multi-threading support */
	CriticalSection m_Lock;

	static void* BURGER_API AllocProc(
		MemoryManager* pThis, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API FreeProc(
		MemoryManager* pThis, const void* pInput) BURGER_NOEXCEPT;
	static void* BURGER_API ReallocProc(MemoryManager* pThis,
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API ShutdownProc(MemoryManager* pThis) BURGER_NOEXCEPT;
	Handle_t* BURGER_API AllocNewHandle(void) BURGER_NOEXCEPT;
	void BURGER_API GrabMemoryRange(void* pData, uintptr_t uLength,
		Handle_t* pParent, Handle_t* pHandle) BURGER_NOEXCEPT;
	void BURGER_API ReleaseMemoryRange(
		void* pData, uintptr_t uLength, Handle_t* pParent) BURGER_NOEXCEPT;
	void BURGER_API PrintHandles(
		const Handle_t* pFirst, const Handle_t* pLast, uint_t bNoCheck) BURGER_NOEXCEPT;

public:
	MemoryManagerHandle(uintptr_t uDefaultMemorySize = DEFAULTMEMORYCHUNK,
		uint_t uDefaultHandleCount = DEFAULTHANDLECOUNT,
		uintptr_t uMinReserveSize = DEFAULTMINIMUMRESERVE) BURGER_NOEXCEPT;
	~MemoryManagerHandle();
	BURGER_INLINE uintptr_t GetTotalAllocatedMemory(void) const BURGER_NOEXCEPT
	{
		return m_uTotalAllocatedMemory;
	}
	BURGER_INLINE void* Alloc(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return AllocProc(this, uSize);
	}
	BURGER_INLINE void Free(const void* pInput) BURGER_NOEXCEPT
	{
		return FreeProc(this, pInput);
	}
	BURGER_INLINE void* Realloc(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return ReallocProc(this, pInput, uSize);
	}
	BURGER_INLINE void Shutdown(void) BURGER_NOEXCEPT
	{
		ShutdownProc(this);
	}
	void** BURGER_API AllocHandle(uintptr_t uSize, uint_t uFlags = 0) BURGER_NOEXCEPT;
	void BURGER_API FreeHandle(void** ppInput) BURGER_NOEXCEPT;
	void** BURGER_API ReallocHandle(void** ppInput, uintptr_t uSize) BURGER_NOEXCEPT;
	void** BURGER_API RefreshHandle(void** ppInput) BURGER_NOEXCEPT;
	void** BURGER_API FindHandle(const void* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetSize(void** ppInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetSize(const void* pInput) BURGER_NOEXCEPT;
	uintptr_t BURGER_API GetTotalFreeMemory(void) BURGER_NOEXCEPT;
	static void* BURGER_API Lock(void** ppInput) BURGER_NOEXCEPT;
	static void BURGER_API Unlock(void** ppInput) BURGER_NOEXCEPT;
	static void BURGER_API SetID(void** ppInput, uint_t uID) BURGER_NOEXCEPT;
	void BURGER_API SetPurgeFlag(void** ppInput, uint_t uFlag) BURGER_NOEXCEPT;
	static uint_t BURGER_API GetLockedState(void** ppInput) BURGER_NOEXCEPT;
	void BURGER_API SetLockedState(
		void** ppInput, uint_t uFlag) BURGER_NOEXCEPT;
	void BURGER_API Purge(void** ppInput) BURGER_NOEXCEPT;
	uint_t BURGER_API PurgeHandles(uintptr_t uSize) BURGER_NOEXCEPT;
	void BURGER_API CompactHandles(void) BURGER_NOEXCEPT;
	void BURGER_API DumpHandles(void) BURGER_NOEXCEPT;
};

class AllocatorHandle: public AllocatorBase {
	/** Pointer to MemoryManagerHandle instance for memory allocation */
	MemoryManagerHandle* m_pMemoryManagerHandle;
	BURGER_RTTI_IN_CLASS();

public:
	/** Default Constructor.
	\param pMemoryManagerHandle Pointer to memory manager
	*/
	AllocatorHandle(MemoryManagerHandle* pMemoryManagerHandle) BURGER_NOEXCEPT
		: m_pMemoryManagerHandle(pMemoryManagerHandle)
	{
	}
	void* Alloc(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->Alloc(uSize);
	}
	void Free(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->Free(pInput);
	}
	void* Realloc(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->Realloc(pInput, uSize);
	}
};

class MemoryManagerGlobalHandle: public MemoryManagerHandle {
	BURGER_DISABLE_COPY(MemoryManagerGlobalHandle);

	/** Pointer to the previous memory manager */
	MemoryManager* m_pPrevious;

public:
	MemoryManagerGlobalHandle(uintptr_t uDefaultMemorySize = DEFAULTMEMORYCHUNK,
		uint_t uDefaultHandleCount = DEFAULTHANDLECOUNT,
		uintptr_t uMinReserveSize = DEFAULTMINIMUMRESERVE) BURGER_NOEXCEPT;
	~MemoryManagerGlobalHandle();
};
}
/* END */

#endif
