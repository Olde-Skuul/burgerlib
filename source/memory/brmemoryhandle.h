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

protected:
#if (UINTPTR_MAX == 0xFFFFFFFFU) || defined(DOXYGEN)
	/** Memory signature for allocated blocks */
	static const uintptr_t kSignatureUsed = 0xDEADBEEF;
	/** Memory signature for free blocks */
	static const uintptr_t kSignatureFree = 0xBADBADBA;
#else
	static const uintptr_t kSignatureUsed = 0xABCDDEADBEEFDCBAULL;
	static const uintptr_t kSignatureFree = 0xBADBADBADBADBADBULL;
#endif

	struct Handle_t {
		/** Pointer to true memory (Must be the first entry!) */
		void* m_pData;

		/** Length of allocated memory */
		uintptr_t m_uLength;

		/** Pointer to the next handle in the chain */
		Handle_t* m_pNextHandle;

		/** Pointer to the previous handle in the chain */
		Handle_t* m_pPrevHandle;

		/** Pointer to the next handle in purge list*/
		Handle_t* m_pNextPurge;

		/** Pointer to the previous handle in the purge list */
		Handle_t* m_pPrevPurge;

		/** Memory flags or parent used handle */
		uint32_t m_uFlags;

		/** Memory ID */
		uint32_t m_uID;
	};

	struct SystemBlock_t {
		/** Next block in the chain */
		SystemBlock_t* m_pNext;
	};

	struct PointerPrefix_t {

		/** Handle to the parent memory object */
		void** m_ppParentHandle;

		/** Signature for debugging */
		uintptr_t m_uSignature;

#if (UINTPTR_MAX == 0xFFFFFFFFU) && \
	!(defined(BURGER_MSDOS) || defined(BURGER_DS) || defined(BURGER_68K))

		/** Pad to alignment */
		uint32_t m_uPadding1;
		uint32_t m_uPadding2;
#endif
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
	Mutex m_Mutex;

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

	BURGER_INLINE void* alloc(uintptr_t uSize) BURGER_NOEXCEPT
	{
		return AllocProc(this, uSize);
	}

	BURGER_INLINE void free(const void* pInput) BURGER_NOEXCEPT
	{
		return FreeProc(this, pInput);
	}

	BURGER_INLINE void* realloc(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
	{
		return ReallocProc(this, pInput, uSize);
	}

	BURGER_INLINE void shutdown(void) BURGER_NOEXCEPT
	{
		ShutdownProc(this);
	}

	void** BURGER_API AllocHandle(
		uintptr_t uSize, uint_t uFlags = 0) BURGER_NOEXCEPT;
	void BURGER_API FreeHandle(void** ppInput) BURGER_NOEXCEPT;
	void** BURGER_API ReallocHandle(
		void** ppInput, uintptr_t uSize) BURGER_NOEXCEPT;
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

	void* alloc(uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->alloc(uSize);
	}

	void free(const void* pInput) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->free(pInput);
	}

	void* realloc(
		const void* pInput, uintptr_t uSize) const BURGER_NOEXCEPT BURGER_FINAL
	{
		return m_pMemoryManagerHandle->realloc(pInput, uSize);
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
