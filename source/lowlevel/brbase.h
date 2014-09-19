/***************************************

	Root base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRBASE_H__
#define __BRBASE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTATICRTTI_H__
#include "brstaticrtti.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
class Base {
	BURGER_RTTI_IN_BASE_CLASS();
public:
	virtual ~Base();
};

class ProxyReferenceCounter {
	BURGER_DISABLECOPYCONSTRUCTORS(ProxyReferenceCounter);
	Word m_uRefCount;				///< Number of weak pointers that are using this object as an anchor
	Word m_bParentAlive;			///< \ref TRUE if the parent object was deleted
	ProxyReferenceCounter() : m_uRefCount(0),m_bParentAlive(TRUE) {}
public:
	static ProxyReferenceCounter *New(void);
	BURGER_INLINE Word IsParentAlive(void) const { return m_bParentAlive; }
	BURGER_INLINE void ParentIsDead(void) { m_bParentAlive = FALSE; }
	BURGER_INLINE void AddRef(void) { ++m_uRefCount; }
	BURGER_INLINE void Release(void) { if (--m_uRefCount == 0) { Free(this); } }
};

class ReferenceCounter : public Base {
	BURGER_DISABLECOPYCONSTRUCTORS(ReferenceCounter);
	Word m_uRefCount;					///< Number of smart pointers that are claiming ownership of this object
	BURGER_RTTI_IN_CLASS();
public:
	ReferenceCounter() : m_uRefCount(0) { }
	virtual ~ReferenceCounter();
	BURGER_INLINE void AddRef(void) { ++m_uRefCount; }
	BURGER_INLINE void Release(void) { if (--m_uRefCount == 0) { Delete(this); } }
};

template<class T>
class SmartPointer {
	T* m_pData;			///< Private copy of the pointer this class is tracking
	BURGER_INLINE void Replace(T* pData) {
		T* pOld = m_pData;
		if (pData != pOld) {
			// Increment the new one first (To prevent accidental circular deletion)
			if (pData) {
				pData->AddRef();
			}
			// Release the old data (Which COULD zero m_pData)
			if (pOld) {
				pOld->Release();
			}
			// Capture the new data
			m_pData = pData;
		}
	}
public:
	SmartPointer(T* pData) : m_pData(pData)
	{
		if (pData) {
			pData->AddRef();
		}
	}
	SmartPointer() : m_pData(NULL) {}
	SmartPointer(const SmartPointer<T>& rData) : m_pData(rData.m_pData)
	{
		if (m_pData) {
			m_pData->AddRef();
		}
	}
	~SmartPointer() {
		if (m_pData) {
			m_pData->Release();
			m_pData = NULL;
		}
	}

	BURGER_INLINE void operator=(const SmartPointer<T>& rData) { Replace(rData.m_pData); }
	BURGER_INLINE void operator=(T* pData) { Replace(pData); }
	BURGER_INLINE T* operator->() const { return m_pData; }
	BURGER_INLINE T* GetPtr() const { return m_pData; }
	BURGER_INLINE Word operator==(const SmartPointer<T>& rData) const { return m_pData == rData.m_pData; }
	BURGER_INLINE Word operator!=(const SmartPointer<T>& rData) const { return m_pData != rData.m_pData; }
	BURGER_INLINE Word operator==(const T* pData) const { return m_pData == pData; }
	BURGER_INLINE Word operator!=(const T* pData) const { return m_pData != pData; }
};
}
/* END */

#endif
