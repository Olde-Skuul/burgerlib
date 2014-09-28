/***************************************

	intrinsic<T> compatible array template

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
template<class T>
class SimpleArray {
	T *m_pData;				///< Pointer to the array of class instances
	WordPtr m_uSize;		///< Number of active elements in the array
	WordPtr m_uBufferSize;	///< Maximum number of elements in the array
public:
	SimpleArray() : m_pData(NULL), m_uSize(0), m_uBufferSize(0) {}
	SimpleArray(WordPtr uDefault) : m_pData(NULL),m_uSize(uDefault),m_uBufferSize(uDefault) {
		// Anything?
		if (uDefault) {
			// Get the default buffer and die if failed in debug
			m_pData = static_cast<T*>(Alloc(sizeof(T) * uDefault));
			BURGER_ASSERT(m_pData);
		}
	}
	SimpleArray(const SimpleArray<T>& rData) : m_pData(NULL),m_uSize(rData.size()),m_uBufferSize(rData.size())
	{
		WordPtr uCount = m_uSize;
		m_pData = static_cast<T*>(AllocCopy(rData.m_pData,sizeof(T) * uCount));
	}
	~SimpleArray() {
		clear();
	}
	BURGER_INLINE T *GetPtr(void) { return m_pData; }
	BURGER_INLINE const T *GetPtr(void) const { return m_pData; }
	BURGER_INLINE T& operator[](WordPtr uIndex) { BURGER_ASSERT(uIndex < m_uSize); return m_pData[uIndex]; }
	BURGER_INLINE const T& operator[](WordPtr uIndex) const { BURGER_ASSERT(uIndex < m_uSize); return m_pData[uIndex]; }
	BURGER_INLINE WordPtr max_size(void) const { return m_uBufferSize; }
	BURGER_INLINE WordPtr size(void) const { return m_uSize; }
	BURGER_INLINE Word empty(void) const { return m_uSize==0; }
	BURGER_INLINE T& front(void) { return m_pData[0]; }
	BURGER_INLINE const T& front(void) const { return m_pData[0]; }
	BURGER_INLINE T& back(void) { return m_pData[m_uSize-1]; }
	BURGER_INLINE const T& back(void) const { return m_pData[m_uSize-1]; }
	void push_back(T rData) {
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
			m_pData = static_cast<T*>(Realloc(m_pData,sizeof(T) * uBufferSize));
			// Allocation failure?!
			BURGER_ASSERT(m_pData);
		}
		// Copy the new object with a copy operator
		m_pData[uSize] = rData;
		m_uSize = uSize+1U;
	}
	void pop_back(void) {
		BURGER_ASSERT(m_uSize);
		--m_uSize;
	}
	void clear(void) {
		Free(m_pData);
		m_pData = NULL;
		m_uBufferSize = 0;
		m_uSize = 0;
	}
	SimpleArray<T>& operator=(const SimpleArray<T>& rData) {
		if (&rData!=this) {
			clear();
			WordPtr uCount = rData.size();
			if (uCount) {
				m_uSize = uCount;
				m_uBufferSize = uCount;
				m_pData = static_cast<T*>(AllocCopy(rData.m_pData,sizeof(T) * uCount));
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
			--uSize;
			m_uSize = uSize;
			MemoryMove(m_pData+uIndex,m_pData+uIndex+1,sizeof(T) * (uSize - uIndex));
		}
	}
	void insert(WordPtr uIndex,T rData = T())
	{
		WordPtr uSize = m_uSize;
		BURGER_ASSERT(uIndex <= uSize);
		resize(uSize + 1);		// resize adjusts m_uSize to uSize+1
		if (uIndex < uSize) {
			MemoryMove(m_pData+uIndex+1,m_pData+uIndex,sizeof(T) * (uSize - uIndex));
		}
		// Copy the new entry
		m_pData[uIndex] = rData;
	}
	void append(const T *pSourceData,WordPtr uCount)
	{
		if (uCount) {
			WordPtr uSize = m_uSize;
			resize(uSize + uCount);
			MemoryCopy(m_pData+uSize,pSourceData,uCount*sizeof(T));
		}
	}
	BURGER_INLINE void append(const SimpleArray<T>& rData) { append(rData.m_pData,rData.size()); }
	void resize(WordPtr uNewSize) {
		if (!uNewSize) {
			clear();
		} else {
			if ((uNewSize <= m_uBufferSize) && (uNewSize > (m_uBufferSize >> 1U))) {
				// don't compact yet.
				BURGER_ASSERT(m_pData != 0);
			} else {
				// Allocate a buffer a bit bigger than needed to
				// reduce future memory allocation requests
				reserve(uNewSize + (uNewSize >> 1));
			}
			m_uSize = uNewSize;
		}
	}
	void reserve(WordPtr uNewBufferSize)
	{
		// Resize the buffer.
		if (!uNewBufferSize) {
			clear();
		} else {
			m_uSize = uNewBufferSize;
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
}
/* END */

#endif
