/***************************************

    HashMap template for mapping a key to data

    Inspired by an implementation found in gameswf by Thatcher Ulrich
<tu@tulrich.com>

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brhashmap.h"
#include "brsdbmhash.h"
#include "brdjb2hash.h"
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "brstring.h"

/*! ************************************

	\brief SDBM (Service Data Base Management) hash callback for HashMapShared

	Invoke the SDBM (Service Data Base Management) hashing algorithm on the contents of a class
	using the default seed value.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa SDBMHashCaseFunctor(), HashMapShared or SDBMHash()

***************************************/

uintptr_t BURGER_API Burger::SDBMHashFunctor(const void *pData,uintptr_t uDataSize)
{
	return SDBMHash(pData,uDataSize);
}

/*! ************************************

	\brief Case insensitive SDBM (Service Data Base Management) hash callback for HashMapShared

	Invoke the SDBM (Service Data Base Management) hashing algorithm on the contents of a class
	using the default seed value and convert all upper case characters into lower case.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa SDBMHashFunctor(), HashMapShared or SDBMHashCase()

***************************************/

uintptr_t BURGER_API Burger::SDBMHashCaseFunctor(const void *pData,uintptr_t uDataSize)
{
	return SDBMHashCase(pData,uDataSize);
}

/*! ************************************

	\brief DJB2 Additive hash callback for HashMapShared

	Invoke the DJB2 Additive hashing algorithm on the contents of a class
	using the default seed value.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa DJB2HashAddCaseFunctor(), HashMapShared or DJB2HashAdd()

***************************************/

uintptr_t BURGER_API Burger::DJB2HashAddFunctor(const void *pData,uintptr_t uDataSize)
{
	return DJB2HashAdd(pData,uDataSize);
}

/*! ************************************

	\brief Case insensitive DJB2 Additive hash callback for HashMapShared
	
	Invoke the DJB2 Additive hashing algorithm on the contents of a class
	using the default seed value and convert all upper case characters into lower case.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa DJB2HashAddFunctor(), HashMapShared or DJB2HashAddCase()

***************************************/

uintptr_t BURGER_API Burger::DJB2HashAddCaseFunctor(const void *pData,uintptr_t uDataSize)
{
	return DJB2HashAddCase(pData,uDataSize);
}

/*! ************************************

	\brief DJB2 Exclusive Or hash callback for HashMapShared

	Invoke the DJB2 Exclusive Or hashing algorithm on the contents of a class
	using the default seed value.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa DJB2HashXorCaseFunctor(), HashMapShared or DJB2HashXor()

***************************************/

uintptr_t BURGER_API Burger::DJB2HashXorFunctor(const void *pData,uintptr_t uDataSize)
{
	return DJB2HashXor(pData,uDataSize);
}

/*! ************************************

	\brief DJB2 Exclusive Or hash callback for HashMapShared

	Invoke the DJB2 Exclusive Or hashing algorithm on the contents of a class
	using the default seed value.

	\param pData Pointer to a data chunk to hash
	\param uDataSize Size of the data chunk in bytes
	\return 32 bit wide hash of the class
	\sa DJB2HashXorFunctor(), HashMapShared or DJB2HashXorCase()

***************************************/

uintptr_t BURGER_API Burger::DJB2HashXorCaseFunctor(const void *pData,uintptr_t uDataSize)
{
	return DJB2HashXorCase(pData,uDataSize);
}

/*! ************************************

	\brief DJB2 Exclusive Or hash callback for HashMapString

	Invoke the DJB2 Exclusive Or hashing algorithm on the contents of 
	a \ref String class using the default seed value.

	\param pData Pointer to a \ref String to hash
	\param uDataSize Size of the data chunk in bytes (Not used)
	\return 32 bit wide hash of the class
	\sa DJB2StringHashXorCaseFunctor(), HashMapString or DJB2HashXor()

***************************************/

uintptr_t BURGER_API Burger::DJB2StringHashXorFunctor(const void *pData,uintptr_t /* uDataSize */)
{
	return DJB2HashXor(static_cast<const String *>(pData)->GetPtr(),static_cast<const String *>(pData)->GetLength());
}

/*! ************************************

	\brief Case insensitive DJB2 Exclusive Or hash callback for HashMapStringCase

	Invoke the case insensitive DJB2 Exclusive Or hashing algorithm on the contents of 
	a \ref String class using the default seed value.

	\param pData Pointer to a \ref String to hash with case insensitivity
	\param uDataSize Size of the data chunk in bytes (Not used)
	\return 32 bit wide hash of the class
	\sa DJB2StringHashXorFunctor(), HashMapStringCase or DJB2HashXorCase()

***************************************/

uintptr_t BURGER_API Burger::DJB2StringHashXorCaseFunctor(const void *pData,uintptr_t /*uDataSize */)
{
	return DJB2HashXorCase(static_cast<const String *>(pData)->GetPtr(),static_cast<const String *>(pData)->GetLength());
}

/*! ************************************

	\class Burger::HashMapShared::Entry
	\brief Base data entry for HashMap

	This is the base class for each data entry
	used by the HashMap / HashMapShared hash
	manager. These entries are used for managing
	the linked list for multiple hash items

	The derived class will append the key / value
	pairs to this class.

	An entry is considered initialized if the hash
	value is not \ref INVALID_HASH

	An entry is considered "dormant", that is, part
	of a linked list, yet must be skipped when
	the entry is not \ref EMPTY_RECORD and the hash is
	\ref INVALID_HASH.

	This is to support erasing entries during the 
	use of an iterator without complicating the iterator
	code.

	\sa HashMap, HashMap::Entry or HashMapShared

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::Entry::IsEmpty(void) const
	\brief Is the Entry an empty record.

	If the Entry is not used at all, it will return \ref TRUE.
	Otherwise, it returns \ref FALSE. It's possible the entry
	is invalid, however it can be still part of a linked list.

	\return \ref TRUE if this Entry is not used at all.

	\sa IsEndOfChain(void) const or IsHashInvalid(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::Entry::IsEndOfChain(void) const
	\brief Is the Entry the last Entry of a linked list.

	If the Entry is the last link of the linked list, it will return \ref TRUE.
	Otherwise, it returns \ref FALSE. 

	\return \ref TRUE if this Entry is the last entry of a valid linked list.

	\sa IsEmpty(void) const or IsHashInvalid(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::Entry::IsHashInvalid(void) const
	\brief Is the Entry uninitialized?

	If the Entry doesn't contain valid data, it will return \ref TRUE.
	Otherwise, it returns \ref FALSE.

	\return \ref TRUE if this Entry not initialized.

	\sa IsEndOfChain(void) const or IsEmpty(void) const

***************************************/



/*! ************************************

	\class Burger::HashMapShared
	\brief Base class for HashMap

	HashMap is a template class to quickly look up data chunks using
	a key value. To cut down on compile time and 
	reduce code bloat, a majority of the runtime
	it contained in this class and the template
	creates a front end with only the minimum amount
	of code to support the class.

	From a users point of view, HashMap is a 100% template
	based class, from a code point of view, HashMap
	is a dispatcher to HashMapShared.

	\note This class is not intended to be directly used,
		it's intended to be derived to allow its functionality
		to be shared with different HashMap template types

	\sa HashMap

***************************************/

/*! ************************************

	\fn Burger::HashMapShared::HashMapShared(uintptr_t uEntrySize,uintptr_t uFirstSize,uintptr_t uSecondOffset,
		TestProc pTestFunction,EntryConstructProc pEntryConstructFunction,
		EntryCopyProc pEntryCopyFunction,EntryInvalidateProc pEntryInvalidationFunction,HashProc pHashFunction)

	\brief Default constructor

	A HashMapShared needs values to properly support the derived class, notably
	the byte size of each data entry and a function to perform
	the hash calculation. This constructor is called by derived classes,
	it's not meant to be used by an application.

	\param uEntrySize Size of each Entry in bytes
	\param uFirstSize Size in bytes for the key class, usually sizeof(T)
	\param uSecondOffset Offset in bytes for the data value in Entry
	\param pTestFunction Function that returns \ref TRUE if the keys match
	\param pEntryConstructFunction Function that invokes the default constructors for the Entry
	\param pEntryCopyFunction Function that invokes the copy constructors for the Entry
	\param pEntryInvalidationFunction Function that invokes the destructors for the Entry
	\param pHashFunction Function that hashes the key value
	\sa HashMap

***************************************/

/*! ************************************

	\brief Locate an entry in the hash

	Hash the key and use the hash to look up the data in 
	the entry table. If found, an entry index is returned. If not,
	\ref INVALID_INDEX is returned.

	\param pKey Pointer to the key value
	\return A valid index or \ref INVALID_INDEX if not found.
	\sa FindFirst(void) const or ComputeHash(const void*) const

***************************************/

uintptr_t BURGER_API Burger::HashMapShared::FindIndex(const void *pKey) const
{
	// Assume failure
	uintptr_t uResult = INVALID_INDEX;
	const Entry *pEntry = static_cast<const Entry *>(m_pEntries);
	// No data in the hash?
	if (pEntry) {

		// Get the hash to look up
		uintptr_t uHash = ComputeHash(pKey);
		
		// Mask to the size of the array
		uintptr_t uIndex = uHash & m_uSizeMask;
		
		// Get the initial index
		pEntry = reinterpret_cast<const Entry *>(reinterpret_cast<const uint8_t *>(pEntry) + (uIndex*m_uEntrySize));
		if (!pEntry->IsEmpty()) {
			// Is this entry occupied by a ROOT linked list entry?
			if (pEntry->IsHashInvalid() || ((pEntry->m_uHashValue & m_uSizeMask) == uIndex)) {
				// Since this is a valid root entry, begin the scan!

				for (;;) {
					// Test the hash first, then the key to ensure there isn't a hash collision

					// Note, pEntry+1 is used because the key is the data that
					// follows the base Entry class. 
					if ((pEntry->m_uHashValue == uHash) && m_pTestFunction(pEntry+1,pKey)) {
						// Found it.
						uResult = uIndex;
						break;
					}
					// Keys are equal, but hash differs!
					// Can occur if the == operator allows equality for incomplete data
					BURGER_ASSERT(pEntry->IsHashInvalid() || !m_pTestFunction(pEntry+1,pKey));	

					// Keep looking through the chain.
					uIndex = pEntry->m_uNextInChain;
					if (uIndex == END_OF_CHAIN) {
						break;			// No more data! Exit with failure
					}
					BURGER_ASSERT(uIndex <= m_uSizeMask);
					pEntry = GetEntry(uIndex);
					BURGER_ASSERT((!pEntry->IsEmpty() || pEntry->IsHashInvalid()) && 
						((pEntry->m_uHashValue & m_uSizeMask) == (uHash & m_uSizeMask)));
				}
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Create a buffer to store all of the data entries

	This helper function assumes that there is no data already
	allocated by the HashMapShared class. It will allocate a
	buffer and then mark each entry as empty and initialize
	all of the internal variables.

	\param uCount Number of entries to allocate (Must be a power of 2)
	\param uEntrySize Size in bytes of each Entry record
	\sa Clear(void)

***************************************/

void BURGER_API Burger::HashMapShared::CreateBuffer(uintptr_t uCount,uintptr_t uEntrySize)
{
	Entry *pEntry = static_cast<Entry*>(Alloc(uEntrySize * uCount));
	BURGER_ASSERT(pEntry);
	// Update the pointer
	m_pEntries = pEntry;
	m_uEntrySize = uEntrySize;
	m_uSizeMask = uCount-1;
	m_uEntryCount = 0;

	// Initialize the base Entry records
	// However, do NOT initialize the derived data
	do {
		pEntry->m_uNextInChain = EMPTY_RECORD;
		pEntry->m_uHashValue = INVALID_HASH;
		pEntry = reinterpret_cast<Entry*>(reinterpret_cast<uint8_t *>(pEntry)+uEntrySize);
	} while (--uCount);
}

/*! ************************************

	\brief Function to change the size of the buffer

	Dynamically resize the buffer and retain all data within
	by reentering every entry into the newly resized hash table

	If uNewSize is zero, delete all data in the hash

	\note If the data buffer is already a comparable size, no action will be performed.

	\param uNewSize Number of entries to allocate (Will be rounded up to a power of 2)
	\sa CreateBuffer(uintptr_t,uintptr_t) or Clear(void)

***************************************/

void BURGER_API Burger::HashMapShared::CreateHashBuffer(uintptr_t uNewSize)
{
	// Handle data purging
	if (!uNewSize) {
		Clear();
	} else {

		// Force new_size to be a power of two.
		uintptr_t uRoundedUp = PowerOf2(uNewSize);
		BURGER_ASSERT(uRoundedUp >= uNewSize);

		uNewSize = uRoundedUp;

		// Assume a minimum size to give the hash a good chance
		// to avoid collisions
		if (uNewSize < 16) {
			uNewSize = 16;
		}

		// Already the same size?

		if (!m_uSizeMask || (uNewSize != (m_uSizeMask + 1))) {

			// Detach the current array of data

			Entry *pOldEntries = static_cast<Entry *>(m_pEntries);
			uintptr_t uCount = m_uSizeMask+1;
			uintptr_t uEntrySize = m_uEntrySize;

			// Create the new buffer
			CreateBuffer(uNewSize,uEntrySize);

			// Copy the previous data to the new hash
			if (pOldEntries) {
				Entry *pTempEntry = pOldEntries;
				do {
					// Every entry with a valid hash will be added to the new hash
					if (!pTempEntry->IsHashInvalid()) {
						Add(pTempEntry+1,reinterpret_cast<const uint8_t *>(pTempEntry)+m_uSecondOffset);
						// Call a disposal function?
						if (m_pEntryInvalidationFunction) {
							m_pEntryInvalidationFunction(pTempEntry);
						}
					}
					pTempEntry = reinterpret_cast<Entry*>(reinterpret_cast<uint8_t *>(pTempEntry)+uEntrySize);
				} while (--uCount);

				// Delete our old data buffer.
				Free(pOldEntries);
			}
		}
	}
}

/*! ************************************

	\brief Erase a specific hash entry

	Assuming a data entry is initialized, this function
	will remove it from the linked list and call the
	invalidation function.

	If the entry being erased is part of a linked list chain,
	it will be destroyed, but the linked list will be retained.
	This is to allow iterators to continue to function without
	error.

	\param uIndex Index into the hash for the entry to erase.
	\sa Erase(const void *)

***************************************/

void BURGER_API Burger::HashMapShared::Erase(uintptr_t uIndex)
{
	BURGER_ASSERT(m_pEntries && (uIndex <= m_uSizeMask));
	Entry *pEntryToErase = GetEntry(uIndex);

	// Get the root index entry
	uintptr_t uTestIndex = pEntryToErase->m_uHashValue & m_uSizeMask;

	// Not a root?
	if (uIndex != uTestIndex) {

		// Iterate from the root until the desired entry
		// is found so it can be spliced in

		Entry* pEntry = GetEntry(uTestIndex);
		while (pEntry->m_uNextInChain != uIndex) {
			BURGER_ASSERT(!pEntry->IsEndOfChain());
			pEntry = GetEntry(pEntry->m_uNextInChain);
		}
		// pEntry has the parent entry, unlink from the chain
		pEntry->m_uNextInChain = pEntryToErase->m_uNextInChain;
		// This entry is totally free!
		pEntryToErase->m_uNextInChain = EMPTY_RECORD;
	} else if (pEntryToErase->IsEndOfChain()) {
		// We are the head of a single entry chain
		pEntryToErase->m_uNextInChain = EMPTY_RECORD;
	}
	// In the final case, it's a root object with a link.
	// Do an in place disposal and retain the link, since
	// moving entries is not supported in the hash
	//	else {}
	
	// Dispose of the derived data
	if (m_pEntryInvalidationFunction) {
		m_pEntryInvalidationFunction(pEntryToErase);
	}
	// Mark as uninitialized
	pEntryToErase->m_uHashValue = INVALID_HASH;
	// Reduce the valid count
	--m_uEntryCount;
}

/*! ************************************

	\brief Erase a hash entry by searching for it

	Search the hash for a key and dispose of the Entry if
	one is found.

	\param pKey Pointer to the key value
	\sa Erase(uintptr_t)

***************************************/

void BURGER_API Burger::HashMapShared::Erase(const void *pKey)
{
	// Find the entry
	uintptr_t uIndex = FindIndex(pKey);
	// Valid?
	if (uIndex!=INVALID_INDEX) {
		// Dispose of it
		Erase(uIndex);
	}
}

/*! ************************************

	\brief Find the index for the first valid entry

	Iterate over the hash data and find the index to the first
	entry that contains valid data. This is used by HashMap::begin() and
	HashMap::begin() const

	\return Index to the first valid entry or \ref INVALID_INDEX if no entries are valid.
	\sa FindIndex(const void *) const

***************************************/

uintptr_t BURGER_API Burger::HashMapShared::FindFirst(void) const
{
	// Assume failure
	uintptr_t uIndex = INVALID_INDEX;
	Entry *pEntry = static_cast<Entry *>(m_pEntries);
	if (pEntry) {
		// Scan until we hit the first valid entry.
		uintptr_t uCount = m_uSizeMask+1;
		uintptr_t uEntrySize = m_uEntrySize;
		do {
			if (!pEntry->IsHashInvalid()) {
				uIndex = (m_uSizeMask+1)-uCount;
				break;
			}
			pEntry = reinterpret_cast<Entry*>(reinterpret_cast<uint8_t *>(pEntry)+uEntrySize);
		} while (--uCount);
	}
	return uIndex;
}

/*! ************************************

	\brief Calculate the hash for a key

	Given a pointer to a key and the length in bytes of the key,
	call the hash algorithm using the stored function pointer. If
	it the very rare case that a hash matches \ref INVALID_HASH,
	it will be changed to \ref INVALID_HASH -0x8000 to "validate"
	the hash.

	\param pKey Pointer to the key value
	\return A valid hash value for the key
	\sa HashProc

***************************************/

uintptr_t BURGER_API Burger::HashMapShared::ComputeHash(const void*pKey) const
{
	// Create the hash
	uintptr_t uHash = m_pHashFunction(pKey,m_uFirstSize);
	// Collision with special hash?
	if (uHash == INVALID_HASH) {
		// Likely, this value will work in the rare case
		uHash = (INVALID_HASH-0x8000);
	}
	return uHash;
}

/*! ************************************

	\brief Replace the contents of this hash with a copy of another

	Clear out all the data in this hash and copy the entries
	from another hash into this one.

	\param pInput Pointer to the HashMap to copy data from
	\sa Clear() and EntryInvalidateProc

***************************************/

void BURGER_API Burger::HashMapShared::Copy(const HashMapShared *pInput)
{
	Clear();

	m_uFirstSize = pInput->m_uFirstSize;
	m_uSecondOffset = pInput->m_uSecondOffset;
	m_pHashFunction = pInput->m_pHashFunction;
	m_pTestFunction = pInput->m_pTestFunction;
	m_pEntryConstructFunction = pInput->m_pEntryConstructFunction;
	m_pEntryCopyFunction = pInput->m_pEntryCopyFunction;
	m_pEntryInvalidationFunction = pInput->m_pEntryInvalidationFunction;

	uintptr_t uEntrySize = pInput->m_uEntrySize;
	m_uEntrySize = uEntrySize;
	uintptr_t uCount = pInput->m_uEntryCount;
	if (uCount) {
		HashMapShared::CreateHashBuffer((uCount * 3) / 2);
		const Entry *pEntry = static_cast<const Entry *>(pInput->m_pEntries);
		uCount = pInput->m_uSizeMask+1;
		do {
			// Every entry with a valid hash will be added to the new hash
			if (!pEntry->IsHashInvalid()) {
				Add(pEntry+1,reinterpret_cast<const uint8_t *>(pEntry)+m_uSecondOffset);
			}
			pEntry = reinterpret_cast<const Entry*>(reinterpret_cast<const uint8_t *>(pEntry)+uEntrySize);
		} while (--uCount);
	}
}


/*! ************************************

	\brief Add a new key data pair into the hash

	Expand the size of the hash if needed, and then insert
	a new key data pair into the hash. This function should now
	be called if a key should be replaced if present. Use
	the HashMap::Set() function for that.

	\param pT Pointer to the key to add
	\param pU Pointer to the data to add
	\sa HashMap::Set()

***************************************/

void BURGER_API Burger::HashMapShared::Add(const void *pT,const void *pU)
{
	BURGER_ASSERT(FindIndex(pT) == INVALID_INDEX);
	if (!m_pEntries) {
		// Initial creation of table. Make a minimum-sized table.
		CreateHashBuffer(16);
	} else if ((m_uEntryCount * 3) > ((m_uSizeMask + 1) * 2)) {
		// Table is more than 2/3rds full. Expand.
		CreateHashBuffer((m_uSizeMask + 1) * 2);
	}
	BURGER_ASSERT(m_pEntries);
	++m_uEntryCount;

	uintptr_t uHash = ComputeHash(pT);
	uintptr_t uIndex = uHash & m_uSizeMask;
	Entry *pEntry = GetEntry(uIndex);

	// If the key is empty, this is simplicity itself
	if (pEntry->IsEmpty()) {
		// Put the new entry in.
		pEntry->m_uNextInChain = END_OF_CHAIN;
		pEntry->m_uHashValue = uHash;
		m_pEntryCopyFunction(pEntry,pT,pU);

	} else if (pEntry->IsHashInvalid()) {
		// This is a "Marker" entry. Invalid data, but a valid link.
		// Make the data valid and retain the link
		//pEntry->m_uNextInChain = pEntry->m_uNextInChain;
		pEntry->m_uHashValue = uHash;
		m_pEntryCopyFunction(pEntry,pT,pU);
	} else {
		// Find a blank spot and link it in
		// Only entries with the EMPTY_RECORD index can be used for insertions
		uintptr_t uBlankIndex = uIndex;
		Entry *pNewEntry;
		do {
			uBlankIndex = (uBlankIndex + 1) & m_uSizeMask;
			pNewEntry = GetEntry(uBlankIndex);
		} while (!pNewEntry->IsEmpty());

		if ((pEntry->m_uHashValue & m_uSizeMask) == uIndex) {
			// Copy the parent entry into the newly found entry
			// to "insert" the new hash entry to the top
			// of the linked list.

			pNewEntry->m_uHashValue = pEntry->m_uHashValue;
			pNewEntry->m_uNextInChain = pEntry->m_uNextInChain;
			m_pEntryCopyFunction(pNewEntry,pEntry+1,reinterpret_cast<const uint8_t *>(pEntry)+m_uSecondOffset);

			// Make the new entry the new parent
			pEntry->m_uNextInChain = uBlankIndex;
		} else {
			// Heavy sigh, the parent doesn't event belong here.
			// Move it into this empty slot and find out
			// which entry is pointing to this one (Very rare case)

			// Start with the parent of the collided entry
			uintptr_t uCollisionIndex = pEntry->m_uHashValue & m_uSizeMask;
			for (;;) {
				Entry *pCollisionEntry = GetEntry(uCollisionIndex);
				// Found the parent?
				if (pCollisionEntry->m_uNextInChain == uIndex) {
					// Copy the collided entry into the new entry
					pNewEntry->m_uHashValue = pEntry->m_uHashValue;
					pNewEntry->m_uNextInChain = pEntry->m_uNextInChain;
					m_pEntryCopyFunction(pNewEntry,pEntry+1,reinterpret_cast<const uint8_t *>(pEntry)+m_uSecondOffset);
					// Fix up the linked list
					pCollisionEntry->m_uNextInChain = uBlankIndex;
					break;
				}
				uCollisionIndex = pCollisionEntry->m_uNextInChain;
				BURGER_ASSERT(uCollisionIndex <= m_uSizeMask);
			}
			pEntry->m_uNextInChain = END_OF_CHAIN;
		}
		// Set up the new starting entry
		pEntry->m_uHashValue = uHash;
		if (m_pEntryInvalidationFunction) {
			m_pEntryInvalidationFunction(pEntry);
		}
		m_pEntryCopyFunction(pEntry,pT,pU);
	}
}

/*! ************************************

	\brief Get the pointer to the data index by a key

	Given a pointer to a key, scan for it using FindIndex(const void *)
	and if found, return the pointer to the data pointed by 
	the Entry pointer added with m_uSecondOffset.

	\param pT Pointer to the key look for
	\return \ref NULL if the record wasn't found or a pointer to
		the data attached to the key.
	\sa HashMap::GetData(const T&) const

***************************************/

const void * Burger::HashMapShared::GetData(const void *pT) const
{
	uintptr_t uIndex = FindIndex(pT);
	const void *pResult = NULL;
	if (uIndex!=INVALID_INDEX) {
		pResult = reinterpret_cast<const uint8_t *>(GetEntry(uIndex))+m_uSecondOffset;
	}
	return pResult;
}

/*! ************************************

	\fn Burger::HashMapShared::Entry *Burger::HashMapShared::GetEntry(uintptr_t uIndex)
	\brief Return the pointer to an Entry

	Since the Entry class is of a runtime decided size, this function
	manually performs the indexing into the array.

	\param uIndex Valid index into the array.
	\return Pointer to a specific Entry into the array.
	\sa GetEntry(uintptr_t) const

***************************************/

/*! ************************************

	\fn const Burger::HashMapShared::Entry *Burger::HashMapShared::GetEntry(uintptr_t uIndex) const
	\brief Return a constant pointer to an Entry

	Since the Entry class is of a runtime decided size, this function
	manually performs the indexing into the array.

	\param uIndex Valid index into the array.
	\return Constant pointer to a specific Entry into the array.
	\sa GetEntry(uintptr_t)

***************************************/



/*! ************************************

	\class Burger::HashMapShared::const_iterator
	\brief STL compatible iterator base class

	This class is the base class for iteration over a hash
	table using standard STL algorithms. It's not meant to be
	used by applications. A HashMap will derive from this
	class to inherit the common functionality and then provide
	access to the specialized key and data pairs.

	\sa HashMap::const_iterator or HashMap::iterator

***************************************/

/*! ************************************

	\fn Burger::HashMapShared::const_iterator::const_iterator(const HashMapShared *pParent,uintptr_t uIndex)
	\brief Base class constructor

	This class is initialized with a pointer to the parent class and
	an index to start iteration. If uIndex is set to \ref INVALID_INDEX, it's
	assumed to be an HashMap::end() marker.

	\param pParent Pointer to the parent class
	\param uIndex Index to start iteration from (Must an index to a valid entry or \ref INVALID_INDEX)
	\sa HashMap::const_iterator or HashMap::iterator

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::const_iterator::IsEnd(void) const
	\brief Is the iterator at the end of the array?

	\return \ref TRUE if the iterator is not pointing to a valid entry.
	\sa GetPtr(void) const

***************************************/

/*! ************************************

	\fn const Burger::HashMapShared::Entry *Burger::HashMapShared::const_iterator::GetPtr(void) const
	\brief Get the Entry pointer

	Given an iterator that's pointing to a valid object, return a pointer
	to the Entry. This function returns the base class which the derived HashMap
	will up cast to the true data type.

	\return Pointer to the specific entry the iterator is pointing to.
	\sa IsEnd()

***************************************/

/*! ************************************

	\fn const Burger::HashMapShared::Entry &Burger::HashMapShared::const_iterator::operator*() const
	\brief Get the Entry reference

	Given an iterator that's pointing to a valid object, return a reference
	to the Entry. This function returns the base class which the derived HashMap
	will up cast to the true data type.

	\return Reference to the specific entry the iterator is pointing to.
	\sa GetPtr() const

***************************************/

/*! ************************************

	\fn const Burger::HashMapShared::Entry *Burger::HashMapShared::const_iterator::operator->() const
	\brief Get the Entry pointer

	Given an iterator that's pointing to a valid object, return a pointer
	to the Entry. This function returns the base class which the derived HashMap
	will up cast to the true data type.

	\return Pointer to the specific entry the iterator is pointing to.
	\sa GetPtr() const

***************************************/

/*! ************************************

	\brief Increment the iterator to the next valid entry

	Step the iterator to the next valid entry in the hash
	table. If the end of the table is reached, the index will be set
	to \ref INVALID_INDEX

	\sa HashMap::begin() const or HashMap::end() const

***************************************/

void BURGER_API Burger::HashMapShared::const_iterator::operator++()
{
	uintptr_t uIndex = m_uIndex;
	if (uIndex!=INVALID_INDEX) {
		const HashMapShared *pParent = m_pParent;
		BURGER_ASSERT(pParent);
		uintptr_t uEndMask = pParent->GetSizeMask();
		// Get the starting pointer
		const Entry *pEntry = pParent->GetEntry(uIndex);
		uintptr_t uEntrySize = pParent->GetEntrySize();
		do {
			// End of the array?
			if (++uIndex > uEndMask) {
				// Force to invalid
				uIndex = INVALID_INDEX;
				break;
			}
			// Increment the pointer
			pEntry = reinterpret_cast<const Entry*>(reinterpret_cast<const uint8_t *>(pEntry)+uEntrySize);
			// Exit if valid
		} while (pEntry->IsHashInvalid());
		// Update the iterator
		m_uIndex = uIndex;
	}
}

/*! ************************************

	\fn uint_t Burger::HashMapShared::const_iterator::operator==(const const_iterator& it) const
	\brief Test for equality between iterators

	If two iterators point to the same index, return \ref TRUE

	\param it Reference to iterator to test against this one
	\return \ref TRUE if the iterators match, \ref FALSE if not. 
	\sa operator!=(const const_iterator&) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::const_iterator::operator!=(const const_iterator& it) const
	\brief Test for inequality between iterators

	If two iterators point different indexes, return \ref TRUE

	\param it Reference to iterator to test against this one
	\return \ref TRUE if the iterators do not match, \ref FALSE if they do. 
	\sa operator==(const const_iterator&) const

***************************************/



/*! ************************************

	\brief Purge all allocated data

	Iterate over all of the initialized entries and destroy
	any entry that has valid data using a function that
	will handle the derived class disposal.

	If the invalidation function is \ref NULL, the main buffer
	is discarded immediately.

	\sa CreateBuffer(uintptr_t,uintptr_t)

***************************************/

void BURGER_API Burger::HashMapShared::Clear(void)
{
	if (m_pEntryInvalidationFunction) {
		Entry *pEntry = static_cast<Entry *>(m_pEntries);
		if (pEntry) {
			uintptr_t uCount = m_uSizeMask+1;
			uintptr_t uEntrySize = m_uEntrySize;
			do {
				if (!pEntry->IsHashInvalid()) {
					m_pEntryInvalidationFunction(pEntry);
				}
				pEntry = reinterpret_cast<Entry*>(reinterpret_cast<uint8_t *>(pEntry)+uEntrySize);
			} while (--uCount);
		}
	}
	Free(m_pEntries);
	m_pEntries = NULL;
	m_uSizeMask = 0;
	m_uEntryCount = 0;
}

/*! ************************************

	\brief Sets a specific capacity to the hash

	A non-destructive function to resize the hash to
	a specific size.

	\param uNewSize Number of entries for the new hash buffer
	\sa SetCapacity(uintptr_t)

***************************************/

void BURGER_API Burger::HashMapShared::Resize(uintptr_t uNewSize) 
{
	if (uNewSize < m_uEntryCount) {
		uNewSize = m_uEntryCount;
	}
	CreateHashBuffer(uNewSize);
}

/*! ************************************

	\brief Sets a comfortable capacity of the hash

	A non-destructive function to resize the hash to a size
	that has padding for new entries to be added with minimal
	memory allocations.

	\param uNewSize Minimum number of entries to size the cache
	\sa Resize(uintptr_t)

***************************************/

void BURGER_API Burger::HashMapShared::SetCapacity(uintptr_t uNewSize)
{
	// Don't delete entries!
	if (uNewSize < m_uEntryCount) {
		uNewSize = m_uEntryCount;
	}
	// Mul by 1.5
	CreateHashBuffer((uNewSize * 3U) >> 1U);
}

/*! ************************************

	\fn uintptr_t Burger::HashMapShared::GetEntryCount(void) const
	\brief Returns the number of valid entries in the hash

	\return The number of valid entries in the hash.
	\sa IsEmpty(void) const or GetSizeMask(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::HashMapShared::GetSizeMask(void) const
	\brief Returns the mask used by the hash for rounding.

	When the hash buffer is created, it's set to a size that's
	a power of two and that value is stored as (size-1) to 
	use as a wrap around mask. To get the hash size, take this
	value and add one to it.

	\code
		HashMap<int,int> foo;
		DoStuffToAddEntriesToTheHash(&foo);
		printf("Maximum entries in the hash %u\n",foo.GetSizeMask()+1);
	\endcode

	\return Zero if hash hasn't been used, or the entry count - 1
	\sa GetEntryCount(void) const or IsEmpty(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::IsEmpty(void) const
	\brief Returns \ref TRUE if the hash is empty

	\return \ref TRUE if the hash is empty, \ref FALSE if not
	\sa GetEntryCount(void) const or GetSizeMask(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMapShared::GetEntrySize(void) const
	\brief Returns the size of each entry in bytes

	\return The number of bytes each Entry occupies in the hash array.
	\sa GetEntryCount(void) const or GetSizeMask(void) const

***************************************/


/*! ************************************

	\class Burger::HashMap
	\brief Key / data pair hash for quick lookup and retrieval

	HashMap is a template class to quickly look up data chunks using
	a key value. To cut down on compile time and 
	reduce code bloat, a majority of the runtime
	it contained in a parent class HashMapShared and the template
	creates a front end with only the minimum amount
	of code to support the class.

	From a users point of view, HashMap is a 100% template
	based class, from a code point of view, HashMap
	is a dispatcher to HashMapShared.

	\sa HashMapShared

***************************************/

/*! ************************************

	\class Burger::HashMap::Entry
	\brief Key / data pair for HashMap

	Entry records have some extra data for connecting
	to the hash and also data for a copy of the key
	and data. This is declared in the template
	so it adjusts depending on the data needed

	\sa HashMapShared

***************************************/

/*! ************************************

	\class Burger::HashMap::const_iterator
	\brief STL compatible constant iterator for HashMap

	To allow STL functionality, this class allow iteration
	over the entire hash.

	\note The const_iterator can only be created by a call to HashMap::begin() or
		HashMap::end()

	\sa iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn Burger::HashMap::const_iterator::const_iterator(const HashMapShared *pParent,uintptr_t uIndex)
	\brief Standard constructor

	This private function is used to create the const_iterator for returning to
	applications.

	\note The const_iterator can only be created by a call to HashMap::begin() or
		HashMap::end()

	\sa iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn const Burger::HashMap::Entry & Burger::HashMap::const_iterator::operator*() const
	\brief Get a reference to the Entry indexed by the const_iterator

	\note Function will fail if the iterator is at the end of the array.

	\return Reference to the Entry record pointed by the const_iterator
	\sa iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn const Burger::HashMap::Entry *Burger::HashMap::const_iterator::operator->() const
	\brief Get a pointer to the Entry indexed by the const_iterator

	\note Function will fail if the iterator is at the end of the array.

	\return Pointer to the Entry record pointed by the const_iterator
	\sa iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\class Burger::HashMap::iterator
	\brief STL compatible iterator for HashMap

	To allow STL functionality, this class allow iteration
	over the entire hash.

	\note The iterator can only be created by a call to HashMap::begin() or
	HashMap::end()

	\sa const_iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn Burger::HashMap::Entry & Burger::HashMap::iterator::operator*() const
	\brief Get a reference to the Entry indexed by the iterator

	\note Function will fail if the iterator is at the end of the array.

	\return Reference to the Entry record pointed by the iterator
	\sa const_iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn Burger::HashMap::Entry *Burger::HashMap::iterator::operator->() const
	\brief Get a pointer to the Entry indexed by the const_iterator

	\note Function will fail if the iterator is at the end of the array.

	\return Pointer to the Entry record pointed by the iterator
	\sa const_iterator and HashMapShared::const_iterator

***************************************/

/*! ************************************

	\fn void Burger::HashMap::Construct(HashMapShared::Entry *pEntry)
	\brief Default constructor for an Entry

	Callback function to default initialize the template Entry structure

	\param pEntry Pointer to an uninitialized Entry record
	\sa EntryConstructProc. Copy(HashMapShared::Entry *,const void *,const void *) or Invalidate(HashMapShared::Entry *)

***************************************/

/*! ************************************

	\fn void Burger::HashMap::Copy(HashMapShared::Entry *pEntry,const void *pT,const void *pU)
	\brief Default copy constructor for an Entry

	Callback function to copy construct an uninitialized template Entry structure

	\param pEntry Pointer to an uninitialized Entry record
	\param pT Pointer to a key record to copy
	\param pU Pointer to a data record to copy
	\sa EntryCopyProc, Construct(HashMapShared::Entry *) or Invalidate(HashMapShared::Entry *)

***************************************/

/*! ************************************

	\fn void Burger::HashMap::Invalidate(HashMapShared::Entry *pEntry)
	\brief Destructor for an Entry

	Callback function to destroy an initialized template Entry structure

	\param pEntry Pointer to an initialized Entry record
	\sa EntryInvalidateProc, Copy(HashMapShared::Entry *,const void *,const void *) or Construct(HashMapShared::Entry *)

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMap::EqualsTest(const void *pA,const void *pB)
	\brief Key comparison function.
	
	Callback function to test two keys and return \ref TRUE if equal.

	\param pA Pointer to the first key to test
	\param pB Pointer to the second key to test
	\sa TestProc

***************************************/

/*! ************************************

	\fn Burger::HashMap::HashMap(HashProc pHashFunction)
	\brief Default constructor

	Create an empty hash and select a hash algorithm.
	
	\param pHashFunction Pointer to a hash function
	\sa HashMap(HashProc,uintptr_t) or HashMap(const HashMap &)

***************************************/

/*! ************************************

	\fn Burger::HashMap::HashMap(HashProc pHashFunction,TestProc pTestProc)
	\brief Default constructor with hash and equality function declarations

	Create an empty hash and select a hash algorithm and
	an Entry test function.
	
	\param pHashFunction Pointer to a hash function
	\param pTestProc Pointer to an Entry equality test
	\sa HashMap(HashProc,uintptr_t) or HashMap(const HashMap &)

***************************************/

/*! ************************************

	\fn Burger::HashMap::HashMap(HashProc pHashFunction,uintptr_t uDefault)
	\brief Default constructor with a set number of preallocated entries

	Construct the hash with a minimum number of entries so they don't
	need to be allocated as data is inserted into the hash during runtime.

	\param pHashFunction Pointer to a hash function
	\param uDefault Number of entries to allocate (Will be rounded up to a power of 2)
	\sa HashMap(HashProc) or HashMap(const HashMap &)

***************************************/

/*! ************************************

	\fn Burger::HashMap::HashMap(const HashMap &rHashMap)
	\brief Copy constructor

	Make a copy of a hash.

	\param rHashMap Reference to a hash to copy
	\sa HashMap(HashProc) or HashMap(HashProc,uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::HashMap::~HashMap()
	\brief Destructor

	Dispose of all data in the hash

	\sa Clear(), HashMap(HashProc), HashMap(HashProc,uintptr_t) or HashMap(const HashMap &)

***************************************/

/*! ************************************

	\fn Burger::HashMap &Burger::HashMap::operator=(const HashMap &rHashMap)
	\brief Copy operator

	Delete all of the data and make a copy of another hash and
	place it in this class.

	\param rHashMap Reference to a hash to copy
	\return *this
	\sa HashMap(const HashMap &rHashMap)

***************************************/

/*! ************************************

	\fn U & Burger::HashMap::operator[](const T &rKey)
	\brief Index operator

	Using a key, look up the item in the hash and if present,
	return a reference to the data. If the entry didn't exist,
	create it with a default constructor for the data.

	\param rKey Reference to the key to look up
	\return Reference to the data found or created.
	\sa GetData(const T&)const or GetData(const T&,U*) const

***************************************/

/*! ************************************

	\fn void Burger::HashMap::Set(const T &rKey,const U &rValue)
	\brief Set a key/data pair in the hash

	Using a key, look up the item in the hash and if present,
	replace the data with the a copy of the data passed. If the entry didn't exist,
	create it with a copy of the passed data.

	\param rKey Reference to the key to look up
	\param rValue Reference to the data to copy
	\return Reference to the data found or created.
	\sa GetData(const T&)const or GetData(const T&,U*) const

***************************************/

/*! ************************************

	\fn void Burger::HashMap::add(const T &rKey,const U &rValue)
	\brief Add a key/data pair to the hash

	Using a key / data pair, add the entry into the hash. This function
	is a helper, it will fail if the key was already present in the hash.

	\param rKey Reference to the key to look up
	\param rValue Reference to the data to copy
	\return Reference to the data found or created.
	\sa Set(const T&,const U&)

***************************************/

/*! ************************************

	\fn U* Burger::HashMap::GetData(const T &rKey)
	\brief Get data by looking it up by a hash key

	Scan the hash for data indexed by the key. If found, return a
	pointer to the data or \ref NULL if the data wasn't found.

	\param rKey Reference to the key to look up
	\return \ref NULL if the key wasn't in the hash or a valid pointer to the data if so.
	\sa GetData(const T &,U *) const

***************************************/

/*! ************************************

	\fn const U* Burger::HashMap::GetData(const T &rKey) const
	\brief Get data by looking it up by a hash key

	Scan the hash for data indexed by the key. If found, return a
	pointer to the data or \ref NULL if the data wasn't found.

	\param rKey Reference to the key to look up
	\return \ref NULL if the key wasn't in the hash or a valid pointer to the data if so.
	\sa GetData(const T &,U *) const

***************************************/

/*! ************************************

	\fn uint_t Burger::HashMap::GetData(const T &rKey,U *pOutput) const
	\brief Get a copy of data by looking it up by a hash key

	Scan the hash for data indexed by the key. If found, copy the
	data and return \ref TRUE. Return \ref FALSE on failure.

	\param rKey Reference to the key to look up
	\param pOutput Pointer to a buffer to receive a copy of the data
	\return \ref TRUE if the data was found, \ref FALSE if not.
	\sa GetData(const T &) const

***************************************/

/*! ************************************

	\fn Burger::HashMap::iterator Burger::HashMap::begin(void)
	\brief Set an iterator to the start of the hash

	Return an STL compatible iterator pointing to the first entry
	in the hash table. The iterator can be set to end() if there
	is no data in the hash.

	\return iterator preset to the beginning of the hash
	\sa end(void)

***************************************/

/*! ************************************

	\fn Burger::HashMap::const_iterator Burger::HashMap::begin(void) const
	\brief Set an const_iterator to the start of the hash

	Return an STL compatible const_iterator pointing to the first entry
	in the hash table. The const_iterator can be set to end() const if there
	is no data in the hash.

	\return const_iterator preset to the beginning of the hash
	\sa end(void) const

***************************************/

/*! ************************************

	\fn Burger::HashMap::iterator Burger::HashMap::end(void)
	\brief Set an iterator to the end of the hash

	Return an STL compatible iterator pointing to the terminating entry
	in the hash table. This is the value to test to determine
	if the end of the iterations has been reached.

	\return iterator preset to the terminator of the hash
	\sa begin(void)

***************************************/

/*! ************************************

	\fn Burger::HashMap::const_iterator Burger::HashMap::end(void) const
	\brief Set an const_iterator to the end of the hash

	Return an STL compatible const_iterator pointing to the first entry
	in the hash table. This is the value to test to determine
	if the end of the iterations has been reached.

	\return const_iterator preset to the terminator of the hash
	\sa begin(void) const

***************************************/

/*! ************************************

	\fn Burger::HashMap::iterator Burger::HashMap::find(const T &rKey)
	\brief Set an iterator to a specific entry in the hash

	Return an STL compatible iterator pointing to the requested entry
	in the hash table or end() in case the entry was not found.

	\param rKey Reference to the key to look up
	\return iterator preset to the terminator of the hash or the requested entry
	\sa find(const T&rKey) const

***************************************/

/*! ************************************

	\fn Burger::HashMap::const_iterator Burger::HashMap::find(const T &rKey) const
	\brief Set an iterator to a specific entry in the hash

	Return an STL compatible const_iterator pointing to the requested entry
	in the hash table or end() const in case the entry was not found.

	\param rKey Reference to the key to look up
	\return const_iterator preset to the terminator of the hash or the requested entry
	\sa find(const T&rKey)

***************************************/

/*! ************************************

	\fn void Burger::HashMap::erase(const iterator &it)
	\brief Erase a specific entry in the hash indexed by an iterator

	Using an iterator to look up an entry in the hash, delete the entry.

	\param it Reference to the iterator pointing to the entry to delete
	\sa erase(const T&)

***************************************/

/*! ************************************

	\fn void Burger::HashMap::erase(const T&rKey)
	\brief Erase a specific entry in the hash

	Using a key to look up an entry in the hash, if it's found, delete it.

	\param rKey Reference to the key to look up
	\sa erase(const iterator &it)

***************************************/

/*! ************************************

	\class Burger::HashMapString
	\brief String key / data pair hash for quick lookup and retrieval

	HashMapString is a template class to quickly look up
	data chunks using a String as a key value.
	
	Unlike the standard HashMap which applies a hash to the class
	contents, this HashMap will use the string contained
	in the String class as the key data.

	From a users point of view, HashMapString is a 100% template
	based class, from a code point of view, HashMapString
	is a dispatcher to HashMapShared.

	\note String hashing is case sensitive. For case insensitive
	hashing, use HashMapStringCase

	\sa DJB2StringHashXorFunctor, HashMapShared, HashMap or HashMapStringCase

***************************************/

/*! ************************************

	\fn Burger::HashMapString::HashMapString()
	\brief Default constructor.

	Create an empty hash and select to a case sensitive String hash algorithm.

	\note String hashing is case sensitive. For case insensitive
	hashing, use HashMapStringCase

	\sa DJB2StringHashXorFunctor or HashMapStringCase

***************************************/


/*! ************************************

	\brief Case insensitive string test for HashMapStringCase

	This internal function performs a case insensitive string comparison
	between two String classes.

	\param pA Pointer to the first String to test
	\param pB Pointer to the second String to test
	\return \ref TRUE if the strings match, \ref FALSE if not.
	\sa HashMapStringCase

***************************************/

uint_t BURGER_API Burger::HashMapStringCaseTest(const void *pA,const void *pB) 
{
	return StringCaseCompare(static_cast<const String *>(pA)->GetPtr(),static_cast<const String *>(pB)->GetPtr())==0;
}


/*! ************************************

	\class Burger::HashMapStringCase
	\brief String key / data pair hash for quick lookup and retrieval

	HashMapStringCase is a template class to quickly look up
	data chunks using a String as a key value.
	
	Unlike the standard HashMap which applies a hash to the class
	contents, this HashMap will use the string contained
	in the String class as the key data.

	From a users point of view, HashMapStringCase is a 100% template
	based class, from a code point of view, HashMapStringCase
	is a dispatcher to HashMapShared.

	\note String hashing is case insensitive. For case sensitive
	hashing, use HashMapString

	\sa DJB2StringHashXorCaseFunctor, HashMapStringCaseTest(const void *,const void *),
		HashMapShared, HashMap or HashMapString

***************************************/

/*! ************************************

	\fn Burger::HashMapStringCase::HashMapStringCase()
	\brief Default constructor.

	Create an empty hash and select to a case insensitive String hash algorithm.

	\note String hashing is case insensitive. For case sensitive
	hashing, use HashMapString

	\sa DJB2StringHashXorCaseFunctor or HashMapString

***************************************/
