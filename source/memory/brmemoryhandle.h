/***************************************

	Handle based memory manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRMUTEX_H__
#include "brmutex.h"
#endif

/* BEGIN */
namespace Burger {
class MemoryManagerHandle: public MemoryManager {
	BURGER_DISABLE_COPY(MemoryManagerHandle);

public:
	/** Set if the memory handle is temporarily locked */
	static const uint32_t kFlagLocked = 0x80;
	/** Set if the memory cannot be moved (High memory) */
	static const uint32_t kFlagFixed = 0x40;
	/** Set if the memory was allocated with malloc() */
	static const uint32_t kFlagMalloc = 0x20;
	/** Set if the handle is purgable */
	static const uint32_t kFlagPurgable = 0x01;
	/** Memory alignment, power of 2, larger or equal to sizeof(void*) */
#if defined(BURGER_MSDOS) || defined(BURGER_DS) || defined(BURGER_68K)
	static const uintptr_t kAlignment = 4;
#else
	static const uintptr_t kAlignment = 16;
#endif
	/** Default starting number of memory handles */
	static const uint32_t kDefaultHandleCount = 512;
	/** Default memory chunk allocation size from system */
	static const uintptr_t kSystemMemoryChuckSize = 0x1000000;
	/** Default reserved system memory size */
	static const uintptr_t kSystemMemoryReservedSize = 0x40000;
	/** Unused handle memory ID */
	static const uint32_t kMemoryIDUnused = UINT32_MAX - 2;
	/** Free handle memory ID */
	static const uint32_t kMemoryIDFree = UINT32_MAX - 1;
	/** Reserved handle memory ID */
	static const uint32_t kMemoryIDReserved = UINT32_MAX;

	enum eMemoryStage {
		/** Garbage collection stage to compact memory */
		kStageCompact,
		/** Garbage collection stage to purge purgeable memory */
		kStagePurge,
		/** Garbage collection stage to purge and then compact memory */
		kStageHailMary,
		/** Critical memory stage, release all possibly releasable memory */
		kStageGiveup
	};

	/** Function prototype for user supplied garbage collection subroutine */
	typedef void(BURGER_API* MemPurgeProc)(void* pThis, eMemoryStage uStage);

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
	Mutex m_Lock;

	static void* BURGER_API alloc_proc(
		MemoryManager* pThis, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API free_proc(
		MemoryManager* pThis, const void* pInput) BURGER_NOEXCEPT;
	static void* BURGER_API realloc_proc(MemoryManager* pThis,
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;
	static void BURGER_API shutdown_proc(MemoryManager* pThis) BURGER_NOEXCEPT;
	Handle_t* BURGER_API AllocNewHandle(void) BURGER_NOEXCEPT;
	void BURGER_API GrabMemoryRange(void* pData, uintptr_t uLength,
		Handle_t* pParent, Handle_t* pHandle) BURGER_NOEXCEPT;
	void BURGER_API ReleaseMemoryRange(
		void* pData, uintptr_t uLength, Handle_t* pParent) BURGER_NOEXCEPT;
	void BURGER_API PrintHandles(const Handle_t* pFirst, const Handle_t* pLast,
		uint_t bNoCheck) BURGER_NOEXCEPT;

public:
	MemoryManagerHandle(uintptr_t uDefaultMemorySize = kSystemMemoryChuckSize,
		uint_t uDefaultHandleCount = kDefaultHandleCount,
		uintptr_t uMinReserveSize = kSystemMemoryReservedSize) BURGER_NOEXCEPT;
	~MemoryManagerHandle();

	BURGER_INLINE uintptr_t GetTotalAllocatedMemory(void) const BURGER_NOEXCEPT
	{
		return m_uTotalAllocatedMemory;
	}

	BURGER_INLINE void* allocate_memory(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return alloc_proc(this, uSize);
	}

	BURGER_INLINE void free_memory(const void* pInput) BURGER_NOEXCEPT
	{
		return free_proc(this, pInput);
	}

	BURGER_INLINE void* reallocate_memory(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return realloc_proc(this, pInput, uSize);
	}

	BURGER_INLINE void shutdown(void) BURGER_NOEXCEPT
	{
		shutdown_proc(this);
	}

	void** BURGER_API alloc_handle(
		uintptr_t uSize, uint_t uFlags = 0) BURGER_NOEXCEPT;
	void BURGER_API free_handle(void** ppInput) BURGER_NOEXCEPT;
	void** BURGER_API ReallocHandle(
		void** ppInput, uintptr_t uSize) BURGER_NOEXCEPT;
	void** BURGER_API RefreshHandle(void** ppInput) BURGER_NOEXCEPT;
	void** BURGER_API FindHandle(const void* pInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetSize(void** ppInput) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API GetSize(const void* pInput) BURGER_NOEXCEPT;
	uintptr_t BURGER_API GetTotalFreeMemory(void) BURGER_NOEXCEPT;
	static void* BURGER_API lock(void** ppInput) BURGER_NOEXCEPT;
	static void BURGER_API unlock(void** ppInput) BURGER_NOEXCEPT;
	static void BURGER_API set_ID(void** ppInput, uint_t uID) BURGER_NOEXCEPT;
	void BURGER_API clear_purge_flag(void** ppInput) BURGER_NOEXCEPT;
	void BURGER_API set_purge_flag(void** ppInput) BURGER_NOEXCEPT;
	static uint_t BURGER_API GetLockedState(void** ppInput) BURGER_NOEXCEPT;
	void BURGER_API SetLockedState(
		void** ppInput, uint_t uFlag) BURGER_NOEXCEPT;
	void BURGER_API Purge(void** ppInput) BURGER_NOEXCEPT;
	uint_t BURGER_API PurgeHandles(uintptr_t uSize) BURGER_NOEXCEPT;
	void BURGER_API CompactHandles(void) BURGER_NOEXCEPT;
	void BURGER_API dump_handles(void) BURGER_NOEXCEPT;
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

	void* allocate_memory(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->allocate_memory(uSize);
	}

	void free_memory(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->free_memory(pInput);
	}

	void* reallocate_memory(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->reallocate_memory(pInput, uSize);
	}
};

class MemoryManagerGlobalHandle: public MemoryManagerHandle {
	BURGER_DISABLE_COPY(MemoryManagerGlobalHandle);

	/** Pointer to the previous memory manager */
	MemoryManager* m_pPrevious;

public:
	MemoryManagerGlobalHandle(
		uintptr_t uDefaultMemorySize = kSystemMemoryChuckSize,
		uint_t uDefaultHandleCount = kDefaultHandleCount,
		uintptr_t uMinReserveSize = kSystemMemoryReservedSize) BURGER_NOEXCEPT;
	~MemoryManagerGlobalHandle();
};
}
/* END */

#endif
