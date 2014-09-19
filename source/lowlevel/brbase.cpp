/***************************************

	Root base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brbase.h"

/*! ************************************

	\class Burger::Base
	\brief Base class for virtual destructor.
	
	Burgerlib has numerous pointers to unknown
	classes that are upcast from Burger::Base if they are actually
	used. By only knowing a generic, empty base
	class, no code bloat is needed to manage 
	destructor/constructor chains because of
	a class reference to something that uses
	OpenGL, DirectX or any other high overhead
	functionality. The only code linked in
	is the generic destructor.

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_BASE(Burger::Base);
BURGER_CREATE_STATICRTTI_PARENT(Burger::ReferenceCounter,Burger::Base);
#endif

/*! ************************************

	\brief Destructor.
	
	Does absolutely nothing
	
***************************************/

Burger::Base::~Base()
{
}

/*! ************************************

	\fn const Burger::StaticRTTI *Burger::Base::GetStaticRTTI(void) const
	\brief Get the description to the class

	This virtual function will pull the pointer to the
	StaticRTTI instance that has the name of the class. Due
	to it being virtual, it will be the name of the most derived class.
	
	\return Pointer to a global, read only instance of StaticRTTI for the true class

***************************************/

/*! ************************************

	\fn const char *Burger::Base::GetClassName(void) const
	\brief Get the name of the class

	This inline function will pull the virtually declared pointer to the
	StaticRTTI instance that has the name of the class. Due
	to it being virtual, it will be the name of the most derived class.
	
	\return Pointer to a global, read only "C" string with the true name of the class

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Base::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/




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
	class, it's mean to be created by ReferenceCounter objects
	when a WeakPointer object is requesting a connection.

	\sa WeakPointer, ReferenceCounter and SmartPointer

***************************************/

/*! ************************************

	\brief Allocate a ProxyReferenceCounter using the global memory manager

	Using the GlobalMemoryManager, allocate an instance of this class
	and return the pointer.

	\return A pointer to a new instance of the ProxyReferenceCounter class
		or \ref NULL on failure

***************************************/

Burger::ProxyReferenceCounter *Burger::ProxyReferenceCounter::New(void)
{
	// In place new
	return new(Alloc(sizeof(ProxyReferenceCounter))) ProxyReferenceCounter;
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

	\fn Word Burger::ProxyReferenceCounter::IsParentAlive(void) const
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
	Once the reference count reaches zero, the object will immediately self destruct.
	Call AddRef() to take ownership of this object.

	\sa AddRef()

***************************************/




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

Burger::ReferenceCounter::~ReferenceCounter()
{
}

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
	Once the reference count reaches zero, the object will immediately self destruct.
	Call AddRef() to take ownership of this object.

	\sa AddRef()

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::ReferenceCounter::g_StaticRTTI
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
	g_pShared = NULL;

	\endcode

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::Replace(T* pData)
	\brief Replace the current pointer

	This private function will test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref NULL) and call
	AddRef() to the new data (If not \ref NULL) and store
	the pointer to the new data in the class.

	\param pData Pointer to an instance of the class T or \ref NULL
	\sa SmartPointer::operator=(T*) or SmartPointer::operator= (const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer(T* pData)
	\brief Initialize a smart pointer

	Call AddRef() to the pointer if it's not \ref NULL 
	and store the pointer inside the class for tracking.

	\param pData Pointer to an instance of the class T or \ref NULL
	\sa SmartPointer::SmartPointer() or SmartPointer::SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer()
	\brief Initialize a smart pointer to \ref NULL

	Initialize to power up default.

	\sa SmartPointer::SmartPointer(T*) or SmartPointer::SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::SmartPointer(const SmartPointer &rData)
	\brief Copy a smart pointer

	Call AddRef() to the pointer if it's not \ref NULL 
	and store a copy of the pointer inside the class for tracking.

	\param rData Reference to an instance to another SmartPointer
	\sa SmartPointer::SmartPointer() or SmartPointer::SmartPointer(T*)

***************************************/

/*! ************************************

	\fn Burger::SmartPointer::~SmartPointer()
	\brief Release a reference to a smart pointer

	If the pointer contained is not \ref NULL, call Release() on it.
	The internal pointer is then set to\ref NULL to ensure 
	there are no dangling pointers

	\sa SmartPointer::SmartPointer(T*) or SmartPointer::SmartPointer(const SmartPointer &)

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::operator=(const SmartPointer &rData)
	\brief Replace the current pointer

	Test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref NULL) and call
	AddRef() to the new data (If not \ref NULL) and store
	the pointer to the new data in the class.

	\param rData Reference to an instance of another SmartPointer
	\sa SmartPointer::operator=(T*)

***************************************/

/*! ************************************

	\fn void Burger::SmartPointer::operator=(T*pData)
	\brief Replace the current pointer

	Test if the pointer is different
	to what is already contained in the class and if so, it will
	Release() the old data (If not \ref NULL) and call
	AddRef() to the new data (If not \ref NULL) and store
	the pointer to the new data in the class.

	\param pData Pointer to an instance of the class T or \ref NULL
	\sa SmartPointer::operator=(const SmartPointer &)

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::operator->() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref NULL if there is no pointer
	being tracked.

	\return \ref NULL or a pointer to an instance of a T class
	\sa SmartPointer::GetPtr()

***************************************/

/*! ************************************

	\fn T* Burger::SmartPointer::GetPtr() const
	\brief Get the current pointer

	Return an untracked copy of the pointer. The
	pointer can be \ref NULL if there is no pointer
	being tracked.

	\return \ref NULL or a pointer to an instance of a T class
	\sa SmartPointer::operator->()

***************************************/

/*! ************************************

	\fn Word Burger::SmartPointer::operator==(const SmartPointer &rData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if equal, \ref FALSE if not
	\sa SmartPointer::operator!=(const SmartPointer &) const or SmartPointer::operator==(const T*) const

***************************************/

/*! ************************************

	\fn Word Burger::SmartPointer::operator!=(const SmartPointer &rData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param rData Reference to an instance of another SmartPointer
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa SmartPointer::operator==(const SmartPointer &) const or SmartPointer::operator!=(const T*) const

***************************************/

/*! ************************************

	\fn Word Burger::SmartPointer::operator==(const T* pData) const
	\brief Test for equality

	Compare the pointers for equality and if equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref NULL
	\return \ref TRUE if equal, \ref FALSE if not
	\sa SmartPointer::operator!=(const T*) const or SmartPointer::operator==(const SmartPointer &) const

***************************************/

/*! ************************************

	\fn Word Burger::SmartPointer::operator!=(const T* pData) const
	\brief Test for inequality

	Compare the pointers for inequality and if not equal, return \ref TRUE

	\param pData Pointer to an instance of the class T or \ref NULL
	\return \ref TRUE if not equal, \ref FALSE if equal
	\sa SmartPointer::operator==(const T*) const or SmartPointer::operator!=(const SmartPointer&) const

***************************************/

