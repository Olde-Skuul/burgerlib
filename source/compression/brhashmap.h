/***************************************

	HashMap template for mapping a key to data

	Inspired by an implementation found in gameswf by Thatcher Ulrich
	<tu@tulrich.com>

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRHASHMAP_H__
#define __BRHASHMAP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
namespace Burger {
extern uintptr_t BURGER_API SDBMHashFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API SDBMHashCaseFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2HashAddFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2HashAddCaseFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2HashXorFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2HashXorCaseFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2StringHashXorFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;
extern uintptr_t BURGER_API DJB2StringHashXorCaseFunctor(
	const void* pData, uintptr_t uDataSize) BURGER_NOEXCEPT;

class HashMapShared {
protected:
	/** Invalid hash value for marking an Entry as uninitialized */
	static const uintptr_t INVALID_HASH = UINTPTR_MAX;
	/** Error value for invalid indexes */
	static const uintptr_t INVALID_INDEX = UINTPTR_MAX;
	/** Constant to mark the end of a hash chain */
	static const uintptr_t END_OF_CHAIN = UINTPTR_MAX;
	/** Constant to mark an unused hash record */
	static const uintptr_t EMPTY_RECORD = UINTPTR_MAX - 1;

public:
	struct Entry {
		/** Next item index in the linked list chain or \ref END_OF_CHAIN to
		 * mark the end of a linked list */
		uintptr_t m_uNextInChain;

		/** Computed hash value for this object (\ref INVALID_HASH indicates
		 * this entry is not initialized) */
		uintptr_t m_uHashValue;

		BURGER_INLINE uint_t IsEmpty(void) const BURGER_NOEXCEPT
		{
			return m_uNextInChain == EMPTY_RECORD;
		}
		BURGER_INLINE uint_t IsEndOfChain(void) const BURGER_NOEXCEPT
		{
			return m_uNextInChain == END_OF_CHAIN;
		}
		BURGER_INLINE uint_t IsHashInvalid(void) const BURGER_NOEXCEPT
		{
			return m_uHashValue == INVALID_HASH;
		}
	};

protected:
	/** Function prototype for user supplied hash generator */
	typedef uintptr_t(BURGER_API* HashProc)(
		const void* pData, uintptr_t uDataSize);
	/** Function prototype for testing keys */
	typedef uint_t(BURGER_API* TestProc)(const void* pA, const void* pB);
	/** Function prototype for destroying entries */
	typedef void(BURGER_API* EntryConstructProc)(Entry* pEntry);
	/** Function prototype for destroying entries */
	typedef void(BURGER_API* EntryCopyProc)(
		Entry* pEntry, const void* pT, const void* pU);
	/** Function prototype for destroying entries */
	typedef void(BURGER_API* EntryInvalidateProc)(Entry* pEntry);
	/** Pointer to the hash table Burger::Alloc(m_uEntrySize*(m_uSizeMask+1)) */
	void* m_pEntries;

	/** Size in bytes of each entry in the table */
	uintptr_t m_uEntrySize;
	/** Size of the key in bytes */
	uintptr_t m_uFirstSize;
	/** Offset in bytes to the start of the data chunk */
	uintptr_t m_uSecondOffset;
	/** Number of valid entries in the hash */
	uintptr_t m_uEntryCount;
	/** (Power of 2)-1 size mask used for masking indexes for instant table
	 * rounding */
	uintptr_t m_uSizeMask;
	/** Pointer to the hash function */
	HashProc m_pHashFunction;
	/** Pointer to the equality test function */
	TestProc m_pTestFunction;
	/** Pointer to function to construct Entry data */
	EntryConstructProc m_pEntryConstructFunction;
	/** Pointer to function to copy construct Entry data */
	EntryCopyProc m_pEntryCopyFunction;
	/** Pointer to function to destroy data in an Entry */
	EntryInvalidateProc m_pEntryInvalidationFunction;

	HashMapShared(uintptr_t uEntrySize, uintptr_t uFirstSize,
		uintptr_t uSecondOffset, TestProc pTestFunction,
		EntryConstructProc pEntryConstructFunction,
		EntryCopyProc pEntryCopyFunction,
		EntryInvalidateProc pEntryInvalidationFunction,
		HashProc pHashFunction = SDBMHashFunctor) BURGER_NOEXCEPT
		: m_pEntries(nullptr),
		  m_uEntrySize(uEntrySize),
		  m_uFirstSize(uFirstSize),
		  m_uSecondOffset(uSecondOffset),
		  m_uEntryCount(0),
		  m_uSizeMask(0),
		  m_pHashFunction(pHashFunction),
		  m_pTestFunction(pTestFunction),
		  m_pEntryConstructFunction(pEntryConstructFunction),
		  m_pEntryCopyFunction(pEntryCopyFunction),
		  m_pEntryInvalidationFunction(pEntryInvalidationFunction)
	{
	}
	uintptr_t BURGER_API FindIndex(const void* pKey) const BURGER_NOEXCEPT;
	void BURGER_API CreateBuffer(
		uintptr_t uCount, uintptr_t uEntrySize) BURGER_NOEXCEPT;
	void BURGER_API CreateHashBuffer(uintptr_t uNewSize) BURGER_NOEXCEPT;
	void BURGER_API Erase(uintptr_t uIndex) BURGER_NOEXCEPT;
	void BURGER_API Erase(const void* pKey) BURGER_NOEXCEPT;
	uintptr_t BURGER_API FindFirst(void) const BURGER_NOEXCEPT;
	uintptr_t BURGER_API ComputeHash(const void* pKey) const BURGER_NOEXCEPT;
	void BURGER_API Copy(const HashMapShared* pInput) BURGER_NOEXCEPT;
	void BURGER_API Add(const void* pT, const void* pU) BURGER_NOEXCEPT;
	const void* BURGER_API GetData(const void* pT) const BURGER_NOEXCEPT;

public:
	BURGER_INLINE Entry* GetEntry(uintptr_t uIndex) BURGER_NOEXCEPT
	{
		BURGER_ASSERT(m_pEntries && (uIndex <= m_uSizeMask));
		return reinterpret_cast<Entry*>(
			static_cast<uint8_t*>(m_pEntries) + (uIndex * m_uEntrySize));
	}
	BURGER_INLINE const Entry* GetEntry(uintptr_t uIndex) const BURGER_NOEXCEPT
	{
		BURGER_ASSERT(m_pEntries && (uIndex <= m_uSizeMask));
		return reinterpret_cast<const Entry*>(
			static_cast<const uint8_t*>(m_pEntries) + (uIndex * m_uEntrySize));
	}

public:
	class const_iterator {
	protected:
		/** Pointer to the parent class instance */
		const HashMapShared* m_pParent;
		/** Last accessed index */
		uintptr_t m_uIndex;
		const_iterator(const HashMapShared* pParent,
			uintptr_t uIndex) BURGER_NOEXCEPT: m_pParent(pParent),
											   m_uIndex(uIndex)
		{
		}

	public:
		BURGER_INLINE uint_t IsEnd(void) const BURGER_NOEXCEPT
		{
			return (m_uIndex == INVALID_INDEX);
		}
		BURGER_INLINE const Entry* GetPtr(void) const BURGER_NOEXCEPT
		{
			const Entry* pEntry = m_pParent->GetEntry(m_uIndex);
			BURGER_ASSERT(
				!IsEnd() && !pEntry->IsEmpty() && !pEntry->IsHashInvalid());
			return pEntry;
		}
		BURGER_INLINE const Entry& operator*() const BURGER_NOEXCEPT
		{
			return GetPtr()[0];
		}
		BURGER_INLINE const Entry* operator->() const BURGER_NOEXCEPT
		{
			return GetPtr();
		}
		void BURGER_API operator++() BURGER_NOEXCEPT;
		BURGER_INLINE uint_t operator==(
			const const_iterator& it) const BURGER_NOEXCEPT
		{
			return (m_pParent == it.m_pParent) && (m_uIndex == it.m_uIndex);
		}
		BURGER_INLINE uint_t operator!=(
			const const_iterator& it) const BURGER_NOEXCEPT
		{
			return (m_pParent != it.m_pParent) || (m_uIndex != it.m_uIndex);
		}
	};
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
	void BURGER_API Resize(uintptr_t uNewSize) BURGER_NOEXCEPT;
	void BURGER_API SetCapacity(uintptr_t uNewSize) BURGER_NOEXCEPT;
	BURGER_INLINE uintptr_t GetEntryCount(void) const BURGER_NOEXCEPT
	{
		return m_uEntryCount;
	}
	BURGER_INLINE uintptr_t GetSizeMask(void) const BURGER_NOEXCEPT
	{
		return m_uSizeMask;
	}
	BURGER_INLINE uint_t IsEmpty(void) const BURGER_NOEXCEPT
	{
		return (m_uEntryCount == 0);
	}
	BURGER_INLINE uintptr_t GetEntrySize(void) const BURGER_NOEXCEPT
	{
		return m_uEntrySize;
	}
};

template<class T, class U>
class HashMap: public HashMapShared {
	struct Entry: public HashMapShared::Entry {
		T first;  ///< Key value
		U second; ///< Data associated with the key
	};
	static void BURGER_API Construct(
		HashMapShared::Entry* pEntry) BURGER_NOEXCEPT
	{
		new (&static_cast<Entry*>(pEntry)->first) T;
		new (&static_cast<Entry*>(pEntry)->second) U;
	}
	static void BURGER_API Copy(HashMapShared::Entry* pEntry, const void* pT,
		const void* pU) BURGER_NOEXCEPT
	{
		new (&static_cast<Entry*>(pEntry)->first)
			T(static_cast<const T*>(pT)[0]);
		new (&static_cast<Entry*>(pEntry)->second)
			U(static_cast<const U*>(pU)[0]);
	}
	static void BURGER_API Invalidate(
		HashMapShared::Entry* pEntry) BURGER_NOEXCEPT
	{
		static_cast<Entry*>(pEntry)->first.~T();
		static_cast<Entry*>(pEntry)->second.~U();
	}
	static uint_t BURGER_API EqualsTest(
		const void* pA, const void* pB) BURGER_NOEXCEPT
	{
		return static_cast<const T*>(pA)[0] == static_cast<const T*>(pB)[0];
	}

public:
	HashMap(HashProc pHashFunction = SDBMHashFunctor) BURGER_NOEXCEPT
		: HashMapShared(sizeof(Entry), sizeof(T),
			  BURGER_OFFSETOF(Entry, second), EqualsTest, Construct, Copy,
			  Invalidate, pHashFunction)
	{
	}
	HashMap(HashProc pHashFunction, TestProc pTestProc) BURGER_NOEXCEPT
		: HashMapShared(sizeof(Entry), sizeof(T),
			  BURGER_OFFSETOF(Entry, second), pTestProc, Construct, Copy,
			  Invalidate, pHashFunction)
	{
	}
	HashMap(HashProc pHashFunction, uintptr_t uDefault) BURGER_NOEXCEPT
		: HashMapShared(sizeof(Entry), sizeof(T),
			  BURGER_OFFSETOF(Entry, second), EqualsTest, Construct, Copy,
			  Invalidate, pHashFunction)
	{
		SetCapacity(uDefault);
	}
	HashMap(const HashMap<T, U>& rHashMap) BURGER_NOEXCEPT
		: HashMapShared(rHashMap.m_uEntrySize, rHashMap.m_uFirstSize,
			  rHashMap.m_uSecondOffset, rHashMap.m_pTestFunction,
			  rHashMap.m_pEntryConstructFunction, rHashMap.m_pEntryCopyFunction,
			  rHashMap.m_pEntryInvalidationFunction, rHashMap.m_pHashFunction)
	{
		HashMapShared::Copy(&rHashMap);
	}
	~HashMap()
	{
		Clear();
	}
	HashMap<T, U>& operator=(const HashMap<T, U>& rHashMap) BURGER_NOEXCEPT
	{
		if (&rHashMap != this) {
			Copy(&rHashMap);
		}
		return *this;
	}
	U& operator[](const T& rKey) BURGER_NOEXCEPT
	{
		uintptr_t uIndex = HashMapShared::FindIndex(&rKey);
		if (uIndex == INVALID_INDEX) {
			add(rKey, U());
			uIndex = HashMapShared::FindIndex(&rKey);
			if (uIndex == INVALID_INDEX) {
				// Couldn't add, die!
				BURGER_ASSERT(0);
			}
		}
		return static_cast<Entry*>(GetEntry(uIndex))->second;
	}

	// Set a new or existing value under the key, to the value.
	void Set(const T& rKey, const U& rValue) BURGER_NOEXCEPT
	{
		uintptr_t uIndex = HashMapShared::FindIndex(&rKey);
		if (uIndex == INVALID_INDEX) {
			// Entry under key doesn't exist.
			Add(&rKey, &rValue);
		} else {
			static_cast<Entry*>(GetEntry(uIndex))->second = rValue;
		}
	}
	BURGER_INLINE void add(const T& rKey, const U& rValue) BURGER_NOEXCEPT
	{
		Add(&rKey, &rValue);
	}
	BURGER_INLINE U* GetData(const T& rKey) BURGER_NOEXCEPT
	{
		return const_cast<U*>(
			static_cast<const U*>(HashMapShared::GetData(&rKey)));
	}
	BURGER_INLINE const U* GetData(const T& rKey) const BURGER_NOEXCEPT
	{
		return static_cast<const U*>(HashMapShared::GetData(&rKey));
	}
	uint_t GetData(const T& rKey, U* pOutput) const BURGER_NOEXCEPT
	{
		const U* pResult = static_cast<const U*>(HashMapShared::GetData(&rKey));
		uint_t uResult = FALSE;
		if (pResult) {
			pOutput[0] = pResult[0];
			uResult = TRUE;
		}
		return uResult;
	}

	class iterator;
	class const_iterator: public HashMapShared::const_iterator {
		const_iterator(
			const HashMapShared* pParent, uintptr_t uIndex) BURGER_NOEXCEPT
			: HashMapShared::const_iterator(pParent, uIndex)
		{
		}
		friend class HashMap<T, U>;
		friend class iterator;

	public:
		BURGER_INLINE const Entry& operator*() const BURGER_NOEXCEPT
		{
			return static_cast<const Entry*>(GetPtr())[0];
		}
		BURGER_INLINE const Entry* operator->() const BURGER_NOEXCEPT
		{
			return static_cast<const Entry*>(GetPtr());
		}
	};
	class iterator: public const_iterator {
		friend class HashMap<T, U>;
		iterator(HashMapShared* pParent, uintptr_t uIndex) BURGER_NOEXCEPT
			: const_iterator(pParent, uIndex)
		{
		}

	public:
		BURGER_INLINE Entry& operator*() const BURGER_NOEXCEPT
		{
			return const_cast<Entry*>(static_cast<const Entry*>(
				HashMapShared::const_iterator::GetPtr()))[0];
		}
		BURGER_INLINE Entry* operator->() const BURGER_NOEXCEPT
		{
			return const_cast<Entry*>(static_cast<const Entry*>(
				HashMapShared::const_iterator::GetPtr()));
		}
	};
	BURGER_INLINE iterator begin(void) BURGER_NOEXCEPT
	{
		return iterator(this, FindFirst());
	}
	BURGER_INLINE const_iterator begin(void) const BURGER_NOEXCEPT
	{
		return const_iterator(this, FindFirst());
	}
	BURGER_INLINE iterator end(void) BURGER_NOEXCEPT
	{
		return iterator(this, INVALID_INDEX);
	}
	BURGER_INLINE const_iterator end(void) const BURGER_NOEXCEPT
	{
		return const_iterator(this, INVALID_INDEX);
	}
	BURGER_INLINE iterator find(const T& rKey) BURGER_NOEXCEPT
	{
		return iterator(this, HashMapShared::FindIndex(&rKey));
	}
	BURGER_INLINE const_iterator find(const T& rKey) const BURGER_NOEXCEPT
	{
		return const_iterator(this, HashMapShared::FindIndex(&rKey));
	}
	void erase(const iterator& it) BURGER_NOEXCEPT
	{
		if (!it.IsEnd() && it.m_pParent == this) {
			Erase(it.m_uIndex);
		}
	}
	BURGER_INLINE void erase(const T& rKey) BURGER_NOEXCEPT
	{
		HashMapShared::Erase(&rKey);
	}
};

template<class U>
class HashMapString: public HashMap<String, U> {
public:
	HashMapString() BURGER_NOEXCEPT
		: HashMap<String, U>(DJB2StringHashXorFunctor)
	{
	}
};

extern uint_t BURGER_API HashMapStringCaseTest(
	const void* pA, const void* pB) BURGER_NOEXCEPT;
template<class U>
class HashMapStringCase: public HashMap<String, U> {
public:
	HashMapStringCase() BURGER_NOEXCEPT
		: HashMap<String, U>(
			  DJB2StringHashXorCaseFunctor, HashMapStringCaseTest)
	{
	}
};

}
/* END */

#endif
