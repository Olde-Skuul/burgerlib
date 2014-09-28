/***************************************

	vector<T> compatible array template

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRARRAY_H__
#define __BRARRAY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4345)	// Plain Old Data (POD) initializer are okay
#endif
template<class T>
class ClassArray {
	T *m_pData;				///< Pointer to the array of class instances
	WordPtr m_uSize;		///< Number of active elements in the array
	WordPtr m_uBufferSize;	///< Maximum number of elements in the array
public:
	ClassArray() : m_pData(NULL), m_uSize(0), m_uBufferSize(0) {}
	ClassArray(WordPtr uDefault) : m_pData(NULL),m_uSize(uDefault),m_uBufferSize(uDefault) {
		// Anything?
		if (uDefault) {
			// Get the default buffer and die if failed in debug
			T *pData = static_cast<T*>(Alloc(sizeof(T) * uDefault));
			BURGER_ASSERT(pData);
			m_pData = pData;
			do {
				// Create default elements
				new (pData) T();
				++pData;
			} while (--uDefault);
		}
	}
	ClassArray(const ClassArray<T>& rData) : m_pData(NULL),m_uSize(rData.size()),m_uBufferSize(rData.size())
	{
		WordPtr uCount = m_uSize;
		// Empty?
		if (uCount) {
			T *pData = static_cast<T*>(Alloc(sizeof(T) * uCount));
			m_pData = pData;
			const T* pSource = &rData.front();
			do {
				new (pData) T(pSource[0]);
				++pSource;
				++pData;
			} while (--uCount);
		}
	}
	~ClassArray() {
		clear();
	}
	BURGER_INLINE T *GetPtr(void) { return m_pData; }
	BURGER_INLINE const T *GetPtr(void) const { return m_pData; }
	BURGER_INLINE T &operator[](WordPtr uIndex) { BURGER_ASSERT(uIndex < m_uSize); return m_pData[uIndex]; }
	BURGER_INLINE const T &operator[](WordPtr uIndex) const { BURGER_ASSERT(uIndex < m_uSize); return m_pData[uIndex]; }
	BURGER_INLINE WordPtr max_size(void) const { return m_uBufferSize; }
	BURGER_INLINE WordPtr size(void) const { return m_uSize; }
	BURGER_INLINE Word empty(void) const { return m_uSize==0; }
	BURGER_INLINE T& front(void) { return m_pData[0]; }
	BURGER_INLINE const T& front(void) const { return m_pData[0]; }
	BURGER_INLINE T& back(void) { return m_pData[m_uSize-1]; }
	BURGER_INLINE const T & back(void) const { return m_pData[m_uSize-1]; }
	void push_back(const T& rData) {
		WordPtr uSize = m_uSize;
		WordPtr uBufferSize = m_uBufferSize;
		// Outgrew the buffer?
		if (uSize>=uBufferSize) {
			// Increase the buffer size with a bit of slop
			// to reduce the number of memory reallocations
			++uBufferSize;
			uBufferSize = uBufferSize + (uBufferSize>>1U);
			// Save the new buffer size in objects
			m_uBufferSize = uBufferSize;
			T* pData = static_cast<T*>(Realloc(m_pData,sizeof(T) * uBufferSize));
			// Allocation failure?!
			BURGER_ASSERT(pData);
			// Save the base array pointer
			m_pData = pData;
			// For the new data, initialize it to the default class
			// instances so copy operators don't explode.
			WordPtr uCount = uBufferSize-uSize;
			pData = pData+uSize;
			do {
				new (pData) T();	// placement new
				++pData;
			} while (--uCount);
		}
		// Copy the new object with a copy operator
		m_pData[uSize] = rData;
		m_uSize = uSize+1U;
	}
	void pop_back(void) {
		BURGER_ASSERT(m_uSize);
		m_pData[--m_uSize].~T();
	}
	void clear(void) {
		WordPtr uCount = m_uSize;
		if (uCount) {
			T *pWork = m_pData;
			do {
				pWork->~T();
				++pWork;
			} while (--uCount);
		}
		Free(m_pData);
		m_pData = NULL;
		m_uBufferSize = 0;
		m_uSize = 0;
	}
	ClassArray<T>& operator=(const ClassArray<T>& rData) {
		if (&rData!=this) {
			clear();
			WordPtr uCount = rData.size();
			if (uCount) {
				m_uSize = uCount;
				m_uBufferSize = uCount;
				T *pData = static_cast<T*>(Alloc(sizeof(T) * uCount));
				m_pData = pData;
				const T* pSource = &rData.front();
				do {
					new (pData) T(pSource[0]);
					++pSource;
				++pData;
				} while (--uCount);
			}
		}
		return *this;
	}
	void remove(WordPtr uIndex)
	{
		WordPtr uSize = m_uSize;
		BURGER_ASSERT(uIndex < uSize);
		if (uSize == 1) {
			// Nuke it
			clear();
		} else {
			m_pData[uIndex].~T();	// destructor
			--uSize;
			MemoryMove(m_pData+uIndex,m_pData+uIndex+1,sizeof(T) * (uSize - uIndex));
			m_uSize = uSize;
		}
	}
	void insert(WordPtr uIndex,const T & rData = T())
	{
		WordPtr uSize = m_uSize;
		BURGER_ASSERT(uIndex <= uSize);
		resize(uSize + 1);		// resize adjusts m_uSize to uSize+1
		if (uIndex < uSize) {
			MemoryMove(m_pData+uIndex+1,m_pData+uIndex,sizeof(T) * (uSize - uIndex));
		}
		// Copy the new entry
		new (m_pData + uIndex) T(rData);
	}
	void append(const T *pSourceData,WordPtr uCount)
	{
		if (uCount) {
			WordPtr uSize = m_uSize;
			resize(uSize + uCount);
			T* pData = m_pData+uSize;
			do {
				pData[0] = pSourceData[0];
				++pSourceData;
				++pData;
			} while (--uCount);
		}
	}
	BURGER_INLINE void append(const ClassArray<T>& rData) { append(rData.m_pData,rData.size()); }
	void resize(WordPtr uNewSize) {
		if (!uNewSize) {
			clear();
		} else {
			WordPtr	uOldSize = m_uSize;

			// Destruct old elements (if we're shrinking).
			if (uNewSize<uOldSize) {
				T *pData = m_pData + uNewSize;
				WordPtr uCount = uOldSize-uNewSize;
				do {
					pData->~T();
					++pData;
				} while (--uCount);
				// Mark the proper size so
				// reserve() doesn't do a double destruction
				m_uSize = uNewSize;
			}
			if ((uNewSize <= m_uBufferSize) && (uNewSize > (m_uBufferSize >> 1U))) {
				// don't compact yet.
				BURGER_ASSERT(m_pData != 0);
			} else {
				// Allocate a buffer a bit bigger than needed to
				// reduce future memory allocation requests
				reserve(uNewSize + (uNewSize >> 1));
			}

			// Copy default T into new elements.
			if (uOldSize<uNewSize) {
				T *pData = m_pData + uOldSize;
				WordPtr uCount2 = uNewSize-uOldSize;
				do {
					new (pData) T();
					++pData;
				} while (--uCount2);
				// Save the increased size
				m_uSize = uNewSize;
			}
		}
	}
	void reserve(WordPtr uNewBufferSize)
	{
		// Resize the buffer.
		if (!uNewBufferSize) {
			clear();
		} else {
			// Will the reserve function shrink the valid buffer?
			WordPtr uSize = m_uSize;
			if (uSize>uNewBufferSize) {
				// Destroy the excess objects
				m_uSize = uNewBufferSize;
				T *pData = m_pData + uNewBufferSize;
				WordPtr uCount = uSize-uNewBufferSize;
				do {
					pData->~T();
					++pData;
				} while (--uCount);
			}
			m_uBufferSize = uNewBufferSize;
			m_pData = static_cast<T*>(Realloc(m_pData,sizeof(T) * uNewBufferSize));
			BURGER_ASSERT(m_pData);	// need to throw (or something) on malloc failure!
		}
	}
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef T value_type;
	BURGER_INLINE iterator begin(void) { return m_pData; }
	BURGER_INLINE iterator end(void) { return m_pData + m_uSize; }
	BURGER_INLINE const_iterator begin(void) const { return m_pData; }
	BURGER_INLINE const_iterator end(void) const { return m_pData + m_uSize; }
};
#if defined(BURGER_MSVC)
#pragma warning(pop)
#endif
}
/* END */

#endif
