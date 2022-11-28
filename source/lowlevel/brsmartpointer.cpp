/***************************************

	Smart pointer template class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsmartpointer.h"

/*! ************************************

	\class Burger::ProxyReferenceCounter
	\brief Class to track an object that can be deleted at any time

	When using a WeakPointer, it needs to be able to keep track of
	an object's status in case the object is deleted. To handle
	this, a reference counted object will create an independently
	allocated object in which all weak pointers will reference
	count so if the parent object is deleted, this object
	will remain to notify all the other systems
	that the object is no more. When all the WeakPointer have
	been notified of the parent's deletion, the proxy
	will be disposed of.

	This class is not meant to be used as a stand alone
	class, it's meant to be created by ReferenceCounter objects
	when a WeakPointer object is requesting a connection.

	\sa WeakPointer, ReferenceCounter and SmartPointer

***************************************/

/*! ************************************

	\brief Allocate a ProxyReferenceCounter using the global memory manager

	Using the GlobalMemoryManager, allocate an instance of this class
	and return the pointer.

	\return A pointer to a new instance of the ProxyReferenceCounter class
		or \ref nullptr on failure

***************************************/

Burger::ProxyReferenceCounter* BURGER_API Burger::ProxyReferenceCounter::New(
	void) BURGER_NOEXCEPT
{
	// In place new
	return new (Alloc(sizeof(ProxyReferenceCounter))) ProxyReferenceCounter;
}

/*! ************************************

	\fn Burger::ProxyReferenceCounter::ProxyReferenceCounter()
	\brief Private default constructor.

	Sets the default values on construction. This can't be
	called by applications. Use Burger::ProxyReferenceCounter::New()
	instead.

	\sa New()

***************************************/

/*! ************************************

	\fn uint_t Burger::ProxyReferenceCounter::IsParentAlive(void) const
	\brief Return \ref TRUE if the parent still exists.

	When this class is created, this flag is set to \ref TRUE. Once
	the parent is deleted, it will clear this flag so all WeakPointer
	class instances will know that the parent object is no more.

	\return \ref TRUE if the parent is still valid or \ref FALSE
		if the parent object is gone

***************************************/

/*! ************************************

	\fn void Burger::ProxyReferenceCounter::ParentIsDead(void)
	\brief Send a signal that the parent was deleted.

	This function should only be called by the parent object
	when the parent object's destructor is called. The
	ProxyReferenceCounter will be signaled and all
	WeakPointer object that reference it will be
	able to know that the parent is gone.

***************************************/

/*! ************************************

	\fn void Burger::ProxyReferenceCounter::AddRef(void)
	\brief Increase the reference count by 1

	Take an ownership of this object by calling this function.
	Call Release() to give up ownership. Once the
	reference count reaches zero, the object will self destruct.

	\sa Release()

***************************************/

/*! ************************************

	\fn void Burger::ProxyReferenceCounter::Release(void)
	\brief Decrease the reference count by 1

	Give up ownership of this object by calling this function.
	Once the reference count reaches zero, the object will immediately self
	destruct. Call AddRef() to take ownership of this object.

	\sa AddRef()

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::ReferenceCounter, Burger::Base);
#endif

/*! ************************************

	\class Burger::ReferenceCounter
	\brief Class to track an object using SmartPointer

	When using a SmartPointer, it needs to be able to keep track of
	an object's reference count to know when the object should be
	deleted. Once all SmartPointer objects release ownership
	of this object, the reference count will reach zero
	and this object will immediately self-destruct.

	\sa WeakPointer, ProxyReferenceCounter and SmartPointer

***************************************/

/*! ************************************

	\fn Burger::ReferenceCounter::ReferenceCounter()
	\brief Sets the reference count to zero

***************************************/

/*! ************************************

	\brief Destructor.

	Does absolutely nothing

***************************************/

Burger::ReferenceCounter::~ReferenceCounter() {}

/*! ************************************

	\fn void Burger::ReferenceCounter::AddRef(void)
	\brief Increase the reference count by 1

	Take an ownership of this object by calling this function.
	Call Release() to give up ownership. Once the
	reference count reaches zero, the object will self destruct.

	\sa Release()

***************************************/

/*! ************************************

	\fn void Burger::ReferenceCounter::Release(void)
	\brief Decrease the reference count by 1

	Give up ownership of this object by calling this function.
	Once the reference count reaches zero, the object will immediately self
	destruct. Call AddRef() to take ownership of this object.

	\sa AddRef()

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::ReferenceCounter::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\class Burger::WeakPointerAnchor
	\brief Private sub class for use by WeakPointer

	When using a WeakPointer, it needs to be able to keep track of
	an object's status in case the object is deleted. To handle
	this, a reference counted object will create by this sub
	class if one is requested. Once created, ProxyReferenceCounter::AddRef() is
	called on it to ensure it's instantiation until the parent object is
	deleted, in which the object will have ProxyReferenceCounter::Release()
	called.

	If there are no WeakPointer objects pointing to the ProxyReferenceCounter
	object, it will also self destruct, otherwise, it will
	stay active with a message stating that the parent is gone
	until all WeakPointer objects have gotten the message and
	released their references.

	This class is not meant to be used as a stand alone
	class, it's mean to be instantiated by the
	macro BURGER_ALLOW_WEAK_POINTERS()

	\sa WeakPointer, ReferenceCounter and BURGER_ALLOW_WEAK_POINTERS()

***************************************/

/*! ************************************

	\fn Burger::WeakPointerAnchor::WeakPointerAnchor()
	\brief Sets the pointer to the proxy to \ref nullptr

***************************************/

/*! ************************************

	\brief Sends a destruction message

	If a ProxyReferenceCounter was created, send a message
	to it that the parent has self destructed and then
	call ProxyReferenceCounter::Release() to give up
	ownership of the ProxyReferenceCounter object.

***************************************/

Burger::WeakPointerAnchor::~WeakPointerAnchor()
{
	ProxyReferenceCounter* pReference = m_pReferenceCounter;
	if (pReference) {
		pReference->ParentIsDead();
		pReference->Release();
		m_pReferenceCounter = nullptr;
	}
}

/*! ************************************

	\brief Returns the owned ProxyReferenceCounter object.

	If a ProxyReferenceCounter object exists, return the pointer, otherwise,
	create the object and return the pointer while taking ownership of it.

	There will be only one shared ProxyReferenceCounter object created for each
	instantiation of this sub class.

	\return Pointer to a valid ProxyReferenceCounter object or \ref nullptr
		on failure

	\sa BURGER_ALLOW_WEAK_POINTERS()

***************************************/

Burger::ProxyReferenceCounter*
Burger::WeakPointerAnchor::GetProxyReferenceCounter(void) const BURGER_NOEXCEPT
{
	ProxyReferenceCounter* pResult = m_pReferenceCounter;
	if (pResult == nullptr) {
		pResult = ProxyReferenceCounter::New();
		m_pReferenceCounter = pResult;
		pResult->AddRef();
	}
	return pResult;
}

/*! ************************************

	\def BURGER_ALLOW_WEAK_POINTERS
	\brief Creates support for Burger::WeakPointer

	Burger::WeakPointer requires the function GetProxyReferenceCounter()
	that returns a valid Burger::ProxyReferenceCounter for tracking. This
	macro will insert the function and a Burger::WeakPointerAnchor
	member variable to support this functionality.

	\code
	class SharedObject {
		// Allow WeakPointer to work
		BURGER_ALLOW_WEAK_POINTERS();
	public:
		void DoStuff(void);
	};

	void foo()
	{
		Burger::WeakPointer<SharedObject> pShared;
		{
			SharedObject shared;
			pShared = &shared;
		}
		if (pShared.GetPtr()==nullptr) {
			printf("shared is out of scope!!");
		}
	}
	\endcode

	\note This macro will set the class setting to "private"
	\sa Burger::WeakPointerAnchor, Burger::ProxyReferenceCounter or
		Burger::WeakPointer

***************************************/

/*! ************************************

	\class Burger::WeakAndStrongBase
	\brief Base class to support WeakPointer and SmartPointer

	Sometimes it's desired to support both weak and smart pointers
	at the same time. This base class will offer this support.
	Derive from this class to obtain the functions to easily
	allow SmartPointer and WeakPointer support.

	\sa WeakPointer, SmartPointer and ReferenceCounter

***************************************/

/*! ************************************

	\fn Burger::ProxyReferenceCounter* \
		Burger::WeakAndStrongBase::GetProxyReferenceCounter(void) const

	\brief Function used by WeakPointer

	This function is inserted by \ref BURGER_ALLOW_WEAK_POINTERS()
	to give support to WeakPointer

	\sa WeakPointer or ProxyReferenceCounter

***************************************/

/*! ************************************

	\var WeakPointerAnchor Burger::WeakAndStrongBase::m_WeakPointerAnchor
	\brief Data to track WeakPointer data

	This data is used by the WeakPointer system to notify
	other objects that this object was deleted.

	\sa WeakPointer, WeakPointerAnchor or GetProxyReferenceCounter(void) const

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(
	Burger::WeakAndStrongBase, Burger::ReferenceCounter);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::WeakAndStrongBase::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\class Burger::SmartPointer
	\brief Template for auto reference counting a pointer

	For shared pointer classes that implement the AddRef() and
	Release() functions, this template will create a structure
	that contains a single pointer that is reference counted
	so shared data can be tracked.

	\code

	// Declare an empty pointer
	Burger::SmartPointer<Burger::SmartObject> g_pShared;

	// Allocate a new object and +1 to the reference
	g_pShared = new Burger::SmartObject;

	// Allocate a new object and +1 to the reference
	// and delete the old one by calling Release() on it
	g_pShared = new Burger::SmartObject;

	// Deletes the object by calling Release()
	// which auto destructs the object
	g_pShared = nullptr;

	\endcode

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::Replace(T* pData)
	\brief Replace the current pointer

	This private function will test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref nullptr) and call
	AddRef() to the new data (If not \ref nullptr) and store
	the pointer to the new data in the class.

	\param pData Pointer to an instance of the class T or \ref nullptr
	\sa operator=(T*) or operator= (const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer(T* pData)
	\brief Initialize a smart pointer

	Call AddRef() to the pointer if it's not \ref nullptr
	and store the pointer inside the class for tracking.

	\param pData Pointer to an instance of the class T or \ref nullptr
	\sa SmartPointer() or SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer()
	\brief Initialize a smart pointer to \ref nullptr

	Initialize to power up default.

	\sa SmartPointer(T*) or SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer(const SmartPointer &rData)
	\brief Copy a smart pointer

	Call AddRef() to the pointer if it's not \ref nullptr
	and store a copy of the pointer inside the class for tracking.

	\param rData Reference to an instance to another SmartPointer
	\sa SmartPointer() or SmartPointer(T*)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::~SmartPointer()
	\brief Release a reference to a smart pointer

	If the pointer contained is not \ref nullptr, call Release() on it.
	The internal pointer is then set to\ref nullptr to ensure
	there are no dangling pointers

	\sa SmartPointer(T*) or SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::operator=(const SmartPointer &rData)
	\brief Replace the current pointer

	Test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref nullptr) and call
	AddRef() to the new data (If not \ref nullptr) and store
	the pointer to the new data in the class.

	\param rData Reference to an instance of another SmartPointer
	\sa operator=(T*)

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::operator=(T*pData)
	\brief Replace the current pointer

	Test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref nullptr) and call
	AddRef() to the new data (If not \ref nullptr) and store
	the pointer to the new data in the class.

	\param pData Pointer to an instance of the class T or \ref nullptr
	\sa operator=(const SmartPointer &)

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::operator->() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref nullptr if there is no pointer
	being tracked.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa GetPtr()

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::operator T*() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref nullptr if there is no pointer
	being tracked.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa GetPtr() const or operator->()

***************************************/

/*! ************************************

	\fn T& Burger::SmartPointer::operator *() const
	\brief Get the current reference

	Return an untracked reference of the object. The
	reference can be \ref nullptr if there is no pointer
	being tracked.

	\return \ref nullptr or a reference to an instance of a T class
	\sa SmartPointer::operator T*() const, GetPtr() const or operator->()

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::operator T*() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref nullptr if there is no pointer
	being tracked.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa GetPtr() const or operator->()

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::GetPtr() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref nullptr if there is no pointer
	being tracked.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa operator->() or SmartPointer::operator T*() const

***************************************/

/*! ************************************

	\fn uint_t Burger::SmartPointer::operator==(const SmartPointer &rData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if equal, \ref FALSE if not
	\sa operator!=(const SmartPointer &) const or operator==(const T*) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SmartPointer::operator!=(const SmartPointer &rData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa operator==(const SmartPointer &) const or operator!=(const T*) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SmartPointer::operator==(const T* pData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if equal, \ref FALSE if not
	\sa operator!=(const T*) const or operator==(const SmartPointer &) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SmartPointer::operator!=(const T* pData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa operator==(const T*) const or operator!=(const SmartPointer&) const

***************************************/

/*! ************************************

	\class Burger::WeakPointer
	\brief Template for tracking an object that could self destruct

	For objects that can be destroyed by other systems or can self
	destruct, this pointer template class will handle monitoring
	the object and will detect if the object isn't present
	in the system anymore.

	\code
	class SharedObject {
		// Allow WeakPointer to work
		BURGER_ALLOW_WEAK_POINTERS();
	public:
		void DoStuff(void);
	};

	void foo()
	{
		Burger::WeakPointer<SharedObject> pShared;
		{
			SharedObject shared;
			pShared = &shared;
			if (pShared.GetPtr()!=nullptr) {
				printf("shared is in scope!!");
			}
		}
		if (pShared.GetPtr()==nullptr) {
			printf("shared is out of scope!!");
		}
	}

	\endcode

	\sa BURGER_ALLOW_WEAK_POINTERS()

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::WeakPointer()
	\brief Initialize a weak pointer to \ref nullptr

	Initialize to power up default.

	\sa WeakPointer::WeakPointer(T*) or
		WeakPointer::WeakPointer(const WeakPointer &)

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::WeakPointer(T* pData)
	\brief Initialize a weak pointer

	Obtain the pointer to the object's ProxyReferenceCounter
	and call ProxyReferenceCounter::AddRef() on it. Store the pointer inside the
	class for tracking.

	\param pData Pointer to an instance of the class T or \ref nullptr
	\sa WeakPointer::WeakPointer() or
		WeakPointer::WeakPointer(const WeakPointer &)

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::WeakPointer(const WeakPointer &rData)
	\brief Copy a weak pointer

	Obtain the pointer to the object's ProxyReferenceCounter
	and call ProxyReferenceCounter::AddRef() on it. Store the pointer inside the
	class for tracking.

	\param rData Reference to an instance of another WeakPointer
	\sa WeakPointer::WeakPointer() or WeakPointer::WeakPointer(T*)

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::WeakPointer(const SmartPointer &rData)
	\brief Copy a weak pointer from a SmartPointer object

	Obtain the pointer to the object's ProxyReferenceCounter
	and call ProxyReferenceCounter::AddRef() on it. Store the pointer inside the
	class for tracking.

	\param rData Reference to an instance of a SmartPointer
	\sa WeakPointer::WeakPointer() or WeakPointer::WeakPointer(T*)

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::~WeakPointer()
	\brief Release a reference to a weak pointer

	If the ProxyReferenceCounter pointer contained
	is not \ref nullptr, call ProxyReferenceCounter::Release() on it.
	The internal pointer is then set to \ref nullptr to ensure
	there are no dangling pointers

	\sa WeakPointer::WeakPointer(T*) or
		WeakPointer::WeakPointer(const WeakPointer &)

***************************************/

/*! ************************************

	\fn void Burger::WeakPointer::operator=(T*pData)
	\brief Track a new object

	Obtain the pointer to the object's ProxyReferenceCounter
	and call ProxyReferenceCounter::AddRef() on it. Store the pointer inside the
	class for tracking. If there was an object previously
	being tracked, call ProxyReferenceCounter::Release() on
	it.

	\param pData Pointer to an instance of the class T or \ref nullptr
	\sa WeakPointer::operator=(const SmartPointer &)

***************************************/

/*! ************************************

	\fn void Burger::WeakPointer::operator=(const SmartPointer &rData)
	\brief Track a new object

	Obtain the pointer to the object's ProxyReferenceCounter
	and call ProxyReferenceCounter::AddRef() on it. Store the pointer inside the
	class for tracking. If there was an object previously being tracked, call
	ProxyReferenceCounter::Release() on it.

	\param rData Reference to an instance of another SmartPointer
	\sa WeakPointer::operator=(T*)

***************************************/

/*! ************************************

	\fn T* Burger::WeakPointer::operator->() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The pointer can be \ref nullptr if
	the object being track was destroyed.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa WeakPointer::GetPtr()

***************************************/

/*! ************************************

	\fn T* Burger::WeakPointer::GetPtr() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The pointer can be \ref nullptr if
	the object being track was destroyed.

	\return \ref nullptr or a pointer to an instance of a T class
	\sa WeakPointer::operator->()

***************************************/

/*! ************************************

	\fn Burger::WeakPointer::operator SmartPointer<T>()
	\brief Award shared ownership of an object

	Obtain the pointer to the object or \ref nullptr
	if the object was destroyed and create a SmartPointer
	with a reference to the object. If the object exists,
	the SmartPointer will call AddRef() on it or
	the SmartPointer will contain a \ref nullptr

	\return A SmartPointer for the shared instance of a T class (Which may be
		\ref nullptr )
	\sa WeakPointer::WeakPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator==(const T* pData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if equal, \ref FALSE if not
	\sa WeakPointer::operator!=(const T*) const or
		WeakPointer::operator==(const WeakPointer &) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator!=(const T* pData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa WeakPointer::operator==(const T*) const or
		WeakPointer::operator!=(const WeakPointer&) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator==(const T* pData)
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if equal, \ref FALSE if not
	\sa WeakPointer::operator!=(const T*) or
		WeakPointer::operator==(const WeakPointer &) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator!=(const T* pData)
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref nullptr
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa WeakPointer::operator==(const T*) or
		WeakPointer::operator!=(const WeakPointer&) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator==(const SmartPointer &rData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if equal, \ref FALSE if not
	\sa WeakPointer::operator!=(const SmartPointer &) const or
		WeakPointer::operator==(const T*) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator!=(const SmartPointer &rData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa WeakPointer::operator==(const SmartPointer &) const or
		WeakPointer::operator!=(const T*) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator==(const SmartPointer &rData)
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if equal, \ref FALSE if not
	\sa WeakPointer::operator!=(const SmartPointer &) or
		WeakPointer::operator==(const T*)

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator!=(const SmartPointer &rData)
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa WeakPointer::operator==(const SmartPointer &) or
		WeakPointer::operator!=(const T*)

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator==(const WeakPointer &rData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param rData Reference to an instance of another WeakPointer
	\return \ref TRUE if equal, \ref FALSE if not
	\sa WeakPointer::operator!=(const WeakPointer &) const or
		WeakPointer::operator==(const T*) const

***************************************/

/*! ************************************

	\fn uint_t Burger::WeakPointer::operator!=(const WeakPointer &rData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param rData Reference to an instance of another WeakPointer
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa WeakPointer::operator==(const WeakPointer &) const or
		WeakPointer::operator!=(const T*) const

***************************************/

/*! ************************************

	\fn T* Burger::WeakPointer::Dereference(void) const
	\brief Return the pointer to an object

	Test if the object exists, and if so, or if no object is
	being tracked, return \ref nullptr

	\note If it was detected that an object has been destroyed,
		this function will call ProxyReferenceCounter::Release()
		on the tracking object to release interest in further tracking.

	\return \ref nullptr if no object is being tracked or exists, or a valid
		pointer
	\sa WeakPointer::operator->() const or WeakPointer::GetPtr() const

***************************************/
