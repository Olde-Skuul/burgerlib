/***************************************

	intrinsic<T> compatible array template

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSIMPLEARRAY_H__
#define __BRSIMPLEARRAY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {

class SimpleArrayBase {

protected:
	void *m_pData;			///< Pointer to the array of class instances
	WordPtr m_uSize;		///< Number of active elements in the array
	WordPtr m_uBufferSize;	///< Maximum number of elements in the array
	WordPtr m_uChunkSize;	///< Size in bytes for each individual element in the array

public:
	SimpleArrayBase(WordPtr uChunkSize) :
		m_pData(nullptr),
		m_uSize(0),
		m_uBufferSize(0),
		m_uChunkSize(!uChunkSize ? 1U : uChunkSize)
	{}
	SimpleArrayBase(WordPtr uChunkSize,WordPtr uDefault);
	SimpleArrayBase(const SimpleArrayBase &rData);
	~SimpleArrayBase();
	SimpleArrayBase &operator=(const SimpleArrayBase &rData);
	void BURGER_API clear(void);
	eError BURGER_API remove_at(WordPtr uIndex);
	eError BURGER_API resize(WordPtr uNewSize);
	eError BURGER_API reserve(WordPtr uNewBufferSize);
protected:
	eError BURGER_API append(const void *pData,WordPtr uCount);
};

template<class T>
class SimpleArray : public SimpleArrayBase {
public:
	SimpleArray() : SimpleArrayBase(sizeof(T)) {}
	SimpleArray(WordPtr uDefault) : SimpleArrayBase(sizeof(T),uDefault) {}

	BURGER_INLINE T *GetPtr(void) { return static_cast<T *>(m_pData); }
	BURGER_INLINE const T *GetPtr(void) const { return static_cast<const T *>(m_pData); }

	BURGER_INLINE T& operator[](WordPtr uIndex) { BURGER_ASSERT(uIndex < m_uSize); return static_cast<T *>(m_pData)[uIndex]; }
	BURGER_INLINE const T& operator[](WordPtr uIndex) const { BURGER_ASSERT(uIndex < m_uSize); return static_cast<const T *>(m_pData)[uIndex]; }
	BURGER_INLINE T& GetIndexedItem(WordPtr uIndex) { BURGER_ASSERT(uIndex < m_uSize); return static_cast<T *>(m_pData)[uIndex]; }
	BURGER_INLINE const T& GetIndexedItem(WordPtr uIndex) const { BURGER_ASSERT(uIndex < m_uSize); return static_cast<T *>(m_pData)[uIndex]; }

	BURGER_INLINE WordPtr capacity(void) const { return m_uBufferSize; }
    static BURGER_INLINE WordPtr max_size(void) { return UINTPTR_MAX / sizeof(T); }
	BURGER_INLINE WordPtr size(void) const { return m_uSize; }
	BURGER_INLINE Word empty(void) const { return !m_uSize; }
	BURGER_INLINE T& front(void) { return static_cast<T *>(m_pData)[0]; }
	BURGER_INLINE const T& front(void) const { return static_cast<const T *>(m_pData)[0]; }
	BURGER_INLINE T& back(void) { return static_cast<T *>(m_pData)[m_uSize-1]; }
	BURGER_INLINE const T& back(void) const { return static_cast<const T *>(m_pData)[m_uSize-1]; }

	eError push_back(T rData) 
	{
		WordPtr uSize = m_uSize;
		WordPtr uBufferSize = m_uBufferSize;
		eError uResult;

		// Outgrew the buffer?
		if (uSize<uBufferSize) {
			uResult = kErrorNone;

			// Copy the new object with a copy operator
			static_cast<T *>(m_pData)[uSize] = rData;
			m_uSize = uSize+1U;

		} else {
			// Increase the buffer size with a bit of slop
			// to reduce the number of memory reallocations
			++uBufferSize;
			uResult = reserve(uBufferSize + (uBufferSize>>1U));
			if (uResult== kErrorNone) {
				// Copy the new object with a copy operator
				static_cast<T *>(m_pData)[uSize] = rData;
				m_uSize = uSize+1U;
			}
		}
		return uResult;
	}

	BURGER_INLINE eError pop_back(void) 
	{
		eError uResult = kErrorOutOfEntries;
		WordPtr uSize = m_uSize;
		if (uSize) {
			--uSize;
			m_uSize = uSize;
			uResult = kErrorNone;
		}
		return uResult;
	}

	BURGER_INLINE SimpleArray<T>& operator=(const SimpleArray<T>& rData) 
	{
		return static_cast<SimpleArray<T>&>(SimpleArrayBase::operator=(rData));
	}

	eError insert_at(WordPtr uIndex,T rData = T())
	{
		WordPtr uSize = m_uSize;
		eError uResult = kErrorInvalidParameter;
		if (uIndex <= uSize) {
			uResult = resize(uSize + 1);		// resize adjusts m_uSize to uSize+1
			if (uResult== kErrorNone) {
				if (uIndex < uSize) {
					MemoryMove(static_cast<T *>(m_pData)+uIndex+1,static_cast<const T *>(m_pData)+uIndex,sizeof(T) * (uSize - uIndex));
				}
				// Copy the new entry
				static_cast<T *>(m_pData)[uIndex] = rData;
			}
		}
		return uResult;
	}

	Word remove(T rData) 
	{
		WordPtr uSize = m_uSize;
		Word bResult = FALSE;
		if (uSize) {
			const T *pWork = static_cast<const T *>(m_pData);
			do {
				if (pWork[0] == rData) {
					remove_at(m_uSize-uSize);
					bResult = TRUE;
					break;
				}
				++pWork;
			} while (--uSize);
		}
		return bResult;
	}
	
	Word contains(T rData) const 
	{
		WordPtr uSize = m_uSize;
		Word bResult = FALSE;
		if (uSize) {
			const T *pWork = static_cast<const T *>(m_pData);
			do {
				if (pWork[0] == rData) {
					bResult = TRUE;
					break;
				}
				++pWork;
			} while (--uSize);
		}
		return bResult;
	}
	
	eError append(const T *pSourceData,WordPtr uCount) 
	{
		return SimpleArrayBase::append(pSourceData,uCount);
	}
	
	eError append(const SimpleArray<T> &rData)
	{
		return SimpleArrayBase::append(rData.m_pData,rData.m_uSize); 
	}
	
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef T value_type;
	BURGER_INLINE iterator begin(void) { return static_cast<T *>(m_pData); }
	BURGER_INLINE iterator end(void) { return static_cast<T *>(m_pData) + m_uSize; }
	BURGER_INLINE const_iterator begin(void) const { return static_cast<const T *>(m_pData); }
	BURGER_INLINE const_iterator end(void) const { return static_cast<const T *>(m_pData) + m_uSize; }
	BURGER_INLINE const_iterator cbegin(void) const { return static_cast<const T *>(m_pData); }
	BURGER_INLINE const_iterator cend(void) const { return static_cast<const T *>(m_pData) + m_uSize; }
	BURGER_INLINE eError erase(const_iterator it) {
		WordPtr uIndex = static_cast<WordPtr>(it-static_cast<const T *>(m_pData));
		return remove_at(uIndex);
	}
};

}
/* END */

#endif
