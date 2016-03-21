/***************************************

	Smart pointer template class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSMARTPOINTER_H__
#define __BRSMARTPOINTER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRSTATICRTTI_H__
#include "brstaticrtti.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

/* BEGIN */
namespace Burger {
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

class WeakPointerAnchor {
	mutable ProxyReferenceCounter* m_pReferenceCounter;
public:
	WeakPointerAnchor() : m_pReferenceCounter(NULL) {}
	~WeakPointerAnchor();
	ProxyReferenceCounter* GetProxyReferenceCounter(void) const;
};

#define BURGER_ALLOW_WEAK_POINTERS() \
public: BURGER_INLINE ProxyReferenceCounter* GetProxyReferenceCounter(void) const { return m_WeakPointerAnchor.GetProxyReferenceCounter(); } \
private: WeakPointerAnchor m_WeakPointerAnchor

class WeakAndStrongBase : public ReferenceCounter {
	BURGER_ALLOW_WEAK_POINTERS();
	BURGER_RTTI_IN_CLASS();
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
		// Add a reference to own
		if (pData) {
			pData->AddRef();
		}
	}
	SmartPointer() : m_pData(NULL) {}
	SmartPointer(const SmartPointer<T>& rData) : m_pData(rData.m_pData)
	{
		// Add a reference to own
		if (m_pData) {
			m_pData->AddRef();
		}
	}
	~SmartPointer() {
		// Release if one is owned
		if (m_pData) {
			m_pData->Release();
			m_pData = NULL;
		}
	}

	BURGER_INLINE void operator=(const SmartPointer<T>& rData) { Replace(rData.m_pData); }
	BURGER_INLINE void operator=(T* pData) { Replace(pData); }
	BURGER_INLINE T* operator->() const { return m_pData; }
	BURGER_INLINE T& operator*() const { return *m_pData; }
	BURGER_INLINE operator T*() const { return m_pData; }
	BURGER_INLINE T* GetPtr() const { return m_pData; }
	BURGER_INLINE Word operator==(const SmartPointer<T>& rData) const { return m_pData == rData.m_pData; }
	BURGER_INLINE Word operator!=(const SmartPointer<T>& rData) const { return m_pData != rData.m_pData; }
	BURGER_INLINE Word operator==(const T* pData) const { return m_pData == pData; }
	BURGER_INLINE Word operator!=(const T* pData) const { return m_pData != pData; }
};

template<class T>
class WeakPointer {
	mutable T* m_pData;			///< Pointer to the object being tracked
	mutable SmartPointer<ProxyReferenceCounter> m_pProxy;	///< Pointer to the object the tracked object will notify if it's deleted
	BURGER_INLINE T* Dereference(void) const
	{
		T* pData = m_pData;
		if (pData) {
			// If the parent was deleted, remove the shared proxy reference
			if (!m_pProxy->IsParentAlive()) {
				pData = NULL;
				m_pProxy = NULL;
				m_pData = NULL;
			}
		}
		return pData;
	}
public:
	WeakPointer() : m_pData(NULL),m_pProxy() { }
	WeakPointer(T* pData) : m_pData(pData),m_pProxy() {
		if (pData) {
			m_pProxy = pData->GetProxyReferenceCounter();
		}
	}
	WeakPointer(const WeakPointer<T>& rData) : m_pData(rData.GetPtr()),m_pProxy() {
		if (m_pData) {
			m_pProxy = m_pData->GetProxyReferenceCounter();
		}
	}
	WeakPointer(const SmartPointer<T>& rData) : m_pData(rData.GetPtr()),m_pProxy() {
		if (m_pData) {
			m_pProxy = m_pData->GetProxyReferenceCounter();
		}
	}
	~WeakPointer() {}
	BURGER_INLINE void operator=(T* pData)
	{
		m_pData = pData;
		ProxyReferenceCounter *pProxy;
		if (!pData) {
			pProxy = NULL;
		} else {
			pProxy = pData->GetProxyReferenceCounter();
		}
		m_pProxy = pProxy;
	}
	BURGER_INLINE void operator=(const SmartPointer<T>& rData) { operator=(rData.GetPtr()); }
	BURGER_INLINE T* operator->() const { return Dereference(); }
	BURGER_INLINE T* GetPtr() const {	return Dereference(); }
	BURGER_INLINE operator SmartPointer<T>() { return SmartPointer<T>(Dereference()); }
	BURGER_INLINE Word operator==(const T* pData) const { return Dereference() == pData; }
	BURGER_INLINE Word operator!=(const T* pData) const { return Dereference() != pData; }
	BURGER_INLINE Word operator==(const T* pData) { return Dereference() == pData; }
	BURGER_INLINE Word operator!=(const T* pData) { return Dereference() != pData; }
	BURGER_INLINE Word operator==(const SmartPointer<T>& rData) const { return Dereference() == rData.GetPtr(); }
	BURGER_INLINE Word operator!=(const SmartPointer<T>& rData) const { return Dereference() != rData.GetPtr(); }
	BURGER_INLINE Word operator==(const SmartPointer<T>& rData) { return Dereference() == rData.GetPtr(); }
	BURGER_INLINE Word operator!=(const SmartPointer<T>& rData) { return Dereference() != rData.GetPtr(); }
	BURGER_INLINE Word operator==(const WeakPointer<T>& rData) const { return Dereference() == rData.Dereference(); }
	BURGER_INLINE Word operator!=(const WeakPointer<T>& rData) const { return Dereference() != rData.Dereference(); }
};
}
/* END */

#endif
