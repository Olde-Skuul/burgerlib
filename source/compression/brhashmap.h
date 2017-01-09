/***************************************

	HashMap template for mapping a key to data

	Inspired by an implementation found in gameswf by Thatcher Ulrich <tu@tulrich.com>

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
extern WordPtr BURGER_API SDBMHashFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API SDBMHashCaseFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2HashAddFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2HashAddCaseFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2HashXorFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2HashXorCaseFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2StringHashXorFunctor(const void *pData,WordPtr uDataSize);
extern WordPtr BURGER_API DJB2StringHashXorCaseFunctor(const void *pData,WordPtr uDataSize);

class HashMapShared {
protected:
	static const WordPtr INVALID_HASH = BURGER_MAXWORDPTR;		///< Invalid hash value for marking an Entry as uninitialized
	static const WordPtr INVALID_INDEX = BURGER_MAXWORDPTR;		///< Error value for invalid indexes
	static const WordPtr END_OF_CHAIN = BURGER_MAXWORDPTR;		///< Constant to mark the end of a hash chain
	static const WordPtr EMPTY_RECORD = BURGER_MAXWORDPTR-1;	///< Constant to mark an unused hash record
public:
	struct Entry {
		WordPtr m_uNextInChain;	///< Next item index in the linked list chain or \ref END_OF_CHAIN to mark the end of a linked list
		WordPtr m_uHashValue;	///< Computed hash value for this object (\ref INVALID_HASH indicates this entry is not initialized)
		BURGER_INLINE Word IsEmpty(void) const { return m_uNextInChain == EMPTY_RECORD; }
		BURGER_INLINE Word IsEndOfChain(void) const { return m_uNextInChain == END_OF_CHAIN; }
		BURGER_INLINE Word IsHashInvalid(void) const { return m_uHashValue == INVALID_HASH; }
	};
protected:
	typedef WordPtr (BURGER_API *HashProc)(const void *pData,WordPtr uDataSize);	///< Function prototype for user supplied hash generator
	typedef Word (BURGER_API *TestProc)(const void *pA,const void *pB);				///< Function prototype for testing keys
	typedef void (BURGER_API *EntryConstructProc)(Entry *pEntry);					///< Function prototype for destroying entries
	typedef void (BURGER_API *EntryCopyProc)(Entry *pEntry,const void *pT,const void *pU);	///< Function prototype for destroying entries
	typedef void (BURGER_API *EntryInvalidateProc)(Entry *pEntry);					///< Function prototype for destroying entries
	void *m_pEntries;			///< Pointer to the hash table Burger::Alloc(m_uEntrySize*(m_uSizeMask+1))
	WordPtr m_uEntrySize;		///< Size in bytes of each entry in the table
	WordPtr m_uFirstSize;		///< Size of the key in bytes
	WordPtr m_uSecondOffset;	///< Offset in bytes to the start of the data chunk
	WordPtr m_uEntryCount;		///< Number of valid entries in the hash
	WordPtr m_uSizeMask;		///< (Power of 2)-1 size mask used for masking indexes for instant table rounding
	HashProc m_pHashFunction;	///< Pointer to the hash function
	TestProc m_pTestFunction;	///< Pointer to the equality test function
	EntryConstructProc m_pEntryConstructFunction;		///< Pointer to function to construct Entry data
	EntryCopyProc m_pEntryCopyFunction;					///< Pointer to function to copy construct Entry data
	EntryInvalidateProc m_pEntryInvalidationFunction;	///< Pointer to function to destroy data in an Entry

	HashMapShared(WordPtr uEntrySize,WordPtr uFirstSize,WordPtr uSecondOffset,
		TestProc pTestFunction,EntryConstructProc pEntryConstructFunction,
		EntryCopyProc pEntryCopyFunction,EntryInvalidateProc pEntryInvalidationFunction,
		HashProc pHashFunction=SDBMHashFunctor) : 
		m_pEntries(NULL),
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
	WordPtr BURGER_API FindIndex(const void *pKey) const;
	void BURGER_API CreateBuffer(WordPtr uCount,WordPtr uEntrySize);
	void BURGER_API CreateHashBuffer(WordPtr uNewSize);
	void BURGER_API Erase(WordPtr uIndex);
	void BURGER_API Erase(const void *pKey);
	WordPtr BURGER_API FindFirst(void) const;
	WordPtr BURGER_API ComputeHash(const void *pKey) const;
	void BURGER_API Copy(const HashMapShared *pInput);
	void BURGER_API Add(const void *pT,const void *pU);
	const void* BURGER_API GetData(const void *pT) const;
	BURGER_INLINE Entry *GetEntry(WordPtr uIndex)
	{
		BURGER_ASSERT(m_pEntries && (uIndex <= m_uSizeMask));
		return reinterpret_cast<Entry *>(static_cast<Word8 *>(m_pEntries) + (uIndex*m_uEntrySize));
	}
	BURGER_INLINE const Entry *GetEntry(WordPtr uIndex) const
	{
		BURGER_ASSERT(m_pEntries && (uIndex <= m_uSizeMask));
		return reinterpret_cast<const Entry *>(static_cast<const Word8 *>(m_pEntries) + (uIndex*m_uEntrySize));
	}
public:
	class const_iterator {
	protected:
		const HashMapShared* m_pParent;	///< Pointer to the parent class instance
		WordPtr m_uIndex;				///< Last accessed index
		const_iterator(const HashMapShared *pParent,WordPtr uIndex) :
			m_pParent(pParent),m_uIndex(uIndex) 
		{
		}
	public:
		BURGER_INLINE Word IsEnd(void) const { return (m_uIndex == INVALID_INDEX); }
		BURGER_INLINE const Entry *GetPtr(void) const {
			const Entry *pEntry = m_pParent->GetEntry(m_uIndex);
			BURGER_ASSERT(!IsEnd() && !pEntry->IsEmpty() && !pEntry->IsHashInvalid());
			return pEntry;
		}
		BURGER_INLINE const Entry &operator*() const { return GetPtr()[0]; }
		BURGER_INLINE const Entry *operator->() const { return GetPtr(); }
		void BURGER_API operator++();
		BURGER_INLINE Word operator==(const const_iterator& it) const {
			return (m_pParent == it.m_pParent) && (m_uIndex == it.m_uIndex);
		}
		BURGER_INLINE Word operator!=(const const_iterator& it) const {
			return (m_pParent != it.m_pParent) || (m_uIndex != it.m_uIndex);
		}
	};
	void BURGER_API Clear(void);
	void BURGER_API Resize(WordPtr uNewSize);
	void BURGER_API SetCapacity(WordPtr uNewSize);
	BURGER_INLINE WordPtr GetEntryCount(void) const { return m_uEntryCount; }
	BURGER_INLINE WordPtr GetSizeMask(void) const { return m_uSizeMask; }
	BURGER_INLINE Word IsEmpty(void) const { return (m_uEntryCount == 0); }
	BURGER_INLINE WordPtr GetEntrySize(void) const { return m_uEntrySize; }
};

template<class T, class U >
class HashMap : public HashMapShared {
	struct Entry : public HashMapShared::Entry {
		T first;				///< Key value
		U second;				///< Data associated with the key
	};
	static void BURGER_API Construct(HashMapShared::Entry *pEntry)
	{
		new (&static_cast<Entry *>(pEntry)->first) T;
		new (&static_cast<Entry *>(pEntry)->second) U;
	}
	static void BURGER_API Copy(HashMapShared::Entry *pEntry,const void *pT,const void *pU)
	{
		new (&static_cast<Entry *>(pEntry)->first) T(static_cast<const T*>(pT)[0]);
		new (&static_cast<Entry *>(pEntry)->second) U(static_cast<const U*>(pU)[0]);
	}
	static void BURGER_API Invalidate(HashMapShared::Entry *pEntry)
	{
		static_cast<Entry *>(pEntry)->first.~T();
		static_cast<Entry *>(pEntry)->second.~U();
	}
	static Word BURGER_API EqualsTest(const void *pA,const void *pB) { return static_cast<const T *>(pA)[0] == static_cast<const T *>(pB)[0]; }
public:
	HashMap(HashProc pHashFunction = SDBMHashFunctor) : 
		HashMapShared(sizeof(Entry),sizeof(T),BURGER_OFFSETOF(Entry,second),
		EqualsTest,Construct,Copy,Invalidate,pHashFunction) { }
	HashMap(HashProc pHashFunction,TestProc pTestProc) : 
		HashMapShared(sizeof(Entry),sizeof(T),BURGER_OFFSETOF(Entry,second),
			pTestProc,Construct,Copy,Invalidate,pHashFunction) { }
	HashMap(HashProc pHashFunction,WordPtr uDefault) : 
		HashMapShared(sizeof(Entry),sizeof(T),BURGER_OFFSETOF(Entry,second),
		EqualsTest,Construct,Copy,Invalidate,pHashFunction) { SetCapacity(uDefault); }
	HashMap(const HashMap<T,U>& rHashMap) :
		HashMapShared(rHashMap.m_uEntrySize,rHashMap.m_uFirstSize,rHashMap.m_uSecondOffset,
		rHashMap.m_pTestFunction,rHashMap.m_pEntryConstructFunction,rHashMap.m_pEntryCopyFunction,
		rHashMap.m_pEntryInvalidationFunction,rHashMap.m_pHashFunction) {
		HashMapShared::Copy(&rHashMap);
	}
	~HashMap() { Clear(); }
	HashMap<T,U>& operator=(const HashMap<T,U>& rHashMap)
	{
		if (&rHashMap!=this) {
			Copy(&rHashMap);
		}
		return *this;
	}
	U &operator[](const T& rKey)
	{
		WordPtr uIndex = HashMapShared::FindIndex(&rKey);
		if (uIndex==INVALID_INDEX) {
			add(rKey,U());
			uIndex = HashMapShared::FindIndex(&rKey);
			if (uIndex==INVALID_INDEX) {
				// Couldn't add, die!
				BURGER_ASSERT(0);
			}
		}
		return static_cast<Entry *>(GetEntry(uIndex))->second;
	}

	// Set a new or existing value under the key, to the value.
	void Set(const T &rKey,const U &rValue)
	{
		WordPtr	uIndex = HashMapShared::FindIndex(&rKey);
		if (uIndex==INVALID_INDEX) {
			// Entry under key doesn't exist.
			Add(&rKey,&rValue);
		} else {
			static_cast<Entry *>(GetEntry(uIndex))->second = rValue;
		}
	}
	BURGER_INLINE void add(const T &rKey,const U &rValue) { Add(&rKey,&rValue); }
	BURGER_INLINE U *GetData(const T& rKey) { return const_cast<U*>(static_cast<const U*>(HashMapShared::GetData(&rKey))); }
	BURGER_INLINE const U *GetData(const T& rKey) const { return static_cast<const U*>(HashMapShared::GetData(&rKey)); }
	Word GetData(const T& rKey,U *pOutput) const
	{
		const U *pResult = static_cast<const U*>(HashMapShared::GetData(&rKey));
		Word uResult = FALSE;
		if (pResult) {
			pOutput[0] = pResult[0];
			uResult = TRUE;
		}
		return uResult;
	}

	class iterator;
	class const_iterator : public HashMapShared::const_iterator {
		const_iterator(const HashMapShared*pParent,WordPtr uIndex) : HashMapShared::const_iterator(pParent,uIndex) {}
		friend class HashMap<T,U>;
		friend class iterator;
	public:
		BURGER_INLINE const Entry &operator*() const { return static_cast<const Entry *>(GetPtr())[0]; }
		BURGER_INLINE const Entry *operator->() const { return static_cast<const Entry *>(GetPtr()); }
	};
	class iterator : public const_iterator {
		friend class HashMap<T,U>;
		iterator(HashMapShared*pParent,WordPtr uIndex) : const_iterator(pParent,uIndex) {}
	public:
		BURGER_INLINE Entry &operator*() const { return const_cast<Entry *>(static_cast<const Entry *>(HashMapShared::const_iterator::GetPtr()))[0]; }
		BURGER_INLINE Entry *operator->() const { return const_cast<Entry *>(static_cast<const Entry *>(HashMapShared::const_iterator::GetPtr())); }
	};
	BURGER_INLINE iterator begin(void) { return iterator(this,FindFirst()); }
	BURGER_INLINE const_iterator begin(void) const { return const_iterator(this,FindFirst()); }
	BURGER_INLINE iterator end(void) { return iterator(this,INVALID_INDEX); }
	BURGER_INLINE const_iterator end(void) const { return const_iterator(this,INVALID_INDEX); }
	BURGER_INLINE iterator find(const T& rKey) { return iterator(this,HashMapShared::FindIndex(&rKey)); }
	BURGER_INLINE const_iterator find(const T& rKey) const { return const_iterator(this,HashMapShared::FindIndex(&rKey)); }
	void erase(const iterator& it)
	{
		if (!it.IsEnd() && it.m_pParent == this) {
			Erase(it.m_uIndex);
		}
	}
	BURGER_INLINE void erase(const T& rKey) { HashMapShared::Erase(&rKey); }
};

template<class U>
class HashMapString : public HashMap<String,U > {
public: HashMapString() : HashMap<String,U >(DJB2StringHashXorFunctor) {}
};

extern Word BURGER_API HashMapStringCaseTest(const void *pA,const void *pB);
template<class U>
class HashMapStringCase : public HashMap<String,U > {
public: HashMapStringCase() : HashMap<String,U >(DJB2StringHashXorCaseFunctor,HashMapStringCaseTest) {}
};

}
/* END */

#endif
