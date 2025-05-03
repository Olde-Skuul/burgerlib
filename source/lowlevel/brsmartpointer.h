/***************************************

	Smart pointer template class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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
	BURGER_DISABLE_COPY(ProxyReferenceCounter);

private:
	/** Number of weak pointers that are using this object as an anchor */
	uint_t m_uRefCount;
	/** \ref TRUE if the parent object was deleted */
	uint_t m_bParentAlive;

	ProxyReferenceCounter() BURGER_NOEXCEPT: m_uRefCount(0),
											 m_bParentAlive(TRUE)
	{
	}

public:
	static ProxyReferenceCounter* BURGER_API new_object(void) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t IsParentAlive(void) const BURGER_NOEXCEPT
	{
		return m_bParentAlive;
	}
	BURGER_INLINE void ParentIsDead(void) BURGER_NOEXCEPT
	{
		m_bParentAlive = FALSE;
	}
	BURGER_INLINE void AddRef(void) BURGER_NOEXCEPT
	{
		++m_uRefCount;
	}
	BURGER_INLINE void Release(void) BURGER_NOEXCEPT
	{
		if (--m_uRefCount == 0) {
			free_memory(this);
		}
	}
};

class ReferenceCounter: public Base {
	BURGER_DISABLE_COPY(ReferenceCounter);
	BURGER_RTTI_IN_CLASS();

private:
	/** Number of smart pointers that are claiming ownership of this object */
	uint_t m_uRefCount;

public:
	ReferenceCounter() BURGER_NOEXCEPT: m_uRefCount(0) {}
	virtual ~ReferenceCounter();
	BURGER_INLINE void AddRef(void) BURGER_NOEXCEPT
	{
		++m_uRefCount;
	}
	BURGER_INLINE void Release(void) BURGER_NOEXCEPT
	{
		if (--m_uRefCount == 0) {
			delete_object(this);
		}
	}
};

class WeakPointerAnchor {
	/** Pointer to reference counter common object */
	mutable ProxyReferenceCounter* m_pReferenceCounter;

public:
	WeakPointerAnchor() BURGER_NOEXCEPT: m_pReferenceCounter(nullptr) {}
	~WeakPointerAnchor();
	ProxyReferenceCounter* GetProxyReferenceCounter(void) const BURGER_NOEXCEPT;
};

#define BURGER_ALLOW_WEAK_POINTERS() \
public: \
	BURGER_INLINE ProxyReferenceCounter* GetProxyReferenceCounter(void) \
		const BURGER_NOEXCEPT \
	{ \
		return m_WeakPointerAnchor.GetProxyReferenceCounter(); \
	} \
\
private: \
	WeakPointerAnchor m_WeakPointerAnchor

class WeakAndStrongBase: public ReferenceCounter { 
	BURGER_ALLOW_WEAK_POINTERS();
	BURGER_RTTI_IN_CLASS();
};

template<class T>
class SmartPointer {

	/** Private copy of the pointer this class is tracking */
	T* m_pData;

	BURGER_INLINE void Replace(T* pData) BURGER_NOEXCEPT
	{
		T* pOld = m_pData;
		if (pData != pOld) {
			// Increment the new one first (To prevent accidental circular
			// deletion)
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
	SmartPointer(T* pData) BURGER_NOEXCEPT: m_pData(pData)
	{
		// Add a reference to own
		if (pData) {
			pData->AddRef();
		}
	}
	SmartPointer() BURGER_NOEXCEPT: m_pData(nullptr) {}
	SmartPointer(const SmartPointer<T>& rData) BURGER_NOEXCEPT
		: m_pData(rData.m_pData)
	{
		// Add a reference to own
		if (m_pData) {
			m_pData->AddRef();
		}
	}
	~SmartPointer()
	{
		// Release if one is owned
		if (m_pData) {
			m_pData->Release();
			m_pData = nullptr;
		}
	}

	BURGER_INLINE void operator=(const SmartPointer<T>& rData) BURGER_NOEXCEPT
	{
		Replace(rData.m_pData);
	}
	BURGER_INLINE void operator=(T* pData) BURGER_NOEXCEPT
	{
		Replace(pData);
	}
	BURGER_INLINE T* operator->() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE T& operator*() const BURGER_NOEXCEPT
	{
		return *m_pData;
	}
	BURGER_INLINE operator T*() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE T* GetPtr() const BURGER_NOEXCEPT
	{
		return m_pData;
	}
	BURGER_INLINE uint_t operator==(
		const SmartPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return m_pData == rData.m_pData;
	}
	BURGER_INLINE uint_t operator!=(
		const SmartPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return m_pData != rData.m_pData;
	}
	BURGER_INLINE uint_t operator==(const T* pData) const BURGER_NOEXCEPT
	{
		return m_pData == pData;
	}
	BURGER_INLINE uint_t operator!=(const T* pData) const BURGER_NOEXCEPT
	{
		return m_pData != pData;
	}
};

template<class T>
class WeakPointer {

	/** Pointer to the object being tracked */
	mutable T* m_pData;

	/** Pointer to the object the tracked object will notify if it's deleted */
	mutable SmartPointer<ProxyReferenceCounter> m_pProxy;

	BURGER_INLINE T* Dereference(void) const BURGER_NOEXCEPT
	{
		T* pData = m_pData;
		if (pData) {
			// If the parent was deleted, remove the shared proxy reference
			if (!m_pProxy->IsParentAlive()) {
				pData = nullptr;
				m_pProxy = nullptr;
				m_pData = nullptr;
			}
		}
		return pData;
	}

public:
	WeakPointer() BURGER_NOEXCEPT: m_pData(nullptr), m_pProxy() {}
	WeakPointer(T* pData) BURGER_NOEXCEPT: m_pData(pData), m_pProxy()
	{
		if (pData) {
			m_pProxy = pData->GetProxyReferenceCounter();
		}
	}
	WeakPointer(const WeakPointer<T>& rData) BURGER_NOEXCEPT
		: m_pData(rData.GetPtr()),
		  m_pProxy()
	{
		if (m_pData) {
			m_pProxy = m_pData->GetProxyReferenceCounter();
		}
	}
	WeakPointer(const SmartPointer<T>& rData) BURGER_NOEXCEPT
		: m_pData(rData.GetPtr()),
		  m_pProxy()
	{
		if (m_pData) {
			m_pProxy = m_pData->GetProxyReferenceCounter();
		}
	}
	~WeakPointer() {}
	BURGER_INLINE void operator=(T* pData) BURGER_NOEXCEPT
	{
		m_pData = pData;
		ProxyReferenceCounter* pProxy;
		if (!pData) {
			pProxy = nullptr;
		} else {
			pProxy = pData->GetProxyReferenceCounter();
		}
		m_pProxy = pProxy;
	}
	BURGER_INLINE void operator=(const SmartPointer<T>& rData) BURGER_NOEXCEPT
	{
		operator=(rData.GetPtr());
	}
	BURGER_INLINE T* operator->() const BURGER_NOEXCEPT
	{
		return Dereference();
	}
	BURGER_INLINE T* GetPtr() const BURGER_NOEXCEPT
	{
		return Dereference();
	}
	BURGER_INLINE operator SmartPointer<T>() BURGER_NOEXCEPT
	{
		return SmartPointer<T>(Dereference());
	}
	BURGER_INLINE uint_t operator==(const T* pData) const BURGER_NOEXCEPT
	{
		return Dereference() == pData;
	}
	BURGER_INLINE uint_t operator!=(const T* pData) const BURGER_NOEXCEPT
	{
		return Dereference() != pData;
	}
	BURGER_INLINE uint_t operator==(const T* pData) BURGER_NOEXCEPT
	{
		return Dereference() == pData;
	}
	BURGER_INLINE uint_t operator!=(const T* pData) BURGER_NOEXCEPT
	{
		return Dereference() != pData;
	}
	BURGER_INLINE uint_t operator==(
		const SmartPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return Dereference() == rData.GetPtr();
	}
	BURGER_INLINE uint_t operator!=(
		const SmartPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return Dereference() != rData.GetPtr();
	}
	BURGER_INLINE uint_t operator==(
		const SmartPointer<T>& rData) BURGER_NOEXCEPT
	{
		return Dereference() == rData.GetPtr();
	}
	BURGER_INLINE uint_t operator!=(
		const SmartPointer<T>& rData) BURGER_NOEXCEPT
	{
		return Dereference() != rData.GetPtr();
	}
	BURGER_INLINE uint_t operator==(
		const WeakPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return Dereference() == rData.Dereference();
	}
	BURGER_INLINE uint_t operator!=(
		const WeakPointer<T>& rData) const BURGER_NOEXCEPT
	{
		return Dereference() != rData.Dereference();
	}
};
}
/* END */

#endif
