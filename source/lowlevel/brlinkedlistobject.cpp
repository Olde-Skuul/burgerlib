/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brlinkedlistobject.h"
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::LinkedListObjects
	\brief Linked list object class

	A LinkedListObjects is the root entry. It contains all the entries of the
	list and all calls use this root entry as a starting point.

	The LinkedListObjects::Object are where the magic happens. It is a member of
	a circular doubly linked list. The root LinkedListObjects structure is where
	you can find the beginning and the ending of the entire list quickly.

	Each LinkedListObjects::Object also has a pointer to the generic data that
	is contained and also a pointer to a function to discard the data once the
	entry itself is deleted. The function pointer could be \ref nullptr meaning
	that the data can be deleted with a simple call to Burger::free_memory(). The data
	could also be pointing to the end of the base LinkedListObjects::Object
	structure if the LinkedListObjects::Object structure is part of a larger
	structure.

	All Linked list manager calls are pointer based. There is no support of
	handles. However the Data field could be a handle and you can pass your own
	data destructor routine to discard the handle. There is a convenience
	routine to add data if the data is a handle. But you have to typecast the
	void * to a void ** manually when you access the data

***************************************/

/*! ************************************

	\typedef Burger::LinkedListObjects::ProcAction
	\brief Iterator callback pointer

	When iterate_forward(ProcAction) or iterate_reverse(ProcAction) is called,
	this function pointer is used to perform an action with every individual
	object data.

	The function returns \ref kContinue to continue iteration, \ref kAbort to
	stop iteration and \ref kDeleteObject to tell the iterator to call the
	disposal function at a safe time. \ref kAbort and \ref kDeleteObject can be
	or'd together to perform both actions.

	\param pData Pointer to the data in the object

	\sa Object::get_data(), iterate_forward(ProcAction),
		or iterate_reverse(ProcAction)

***************************************/

/*! ************************************

	\class Burger::LinkedListObjects::Object

	\brief Instance of a linked list object that contains a pointer to arbitrary
		data.

	Each LinkedListObjects::Object has a pointer to the generic data that is
	contained and also a pointer to a function to discard the data once the
	entry itself is deleted. The function pointer could be nullptr meaning that
	the data can be deleted with a simple call to Burger::free_memory(). The data could
	also be pointing to the end of the base LinkedListObjects::Object structure
	if the LinkedListObjects::Object structure is part of a larger structure.

	\note This class is used by LinkedListObjects

	\sa LinkedListObjects

***************************************/

/*! ************************************

	\typedef Burger::LinkedListObjects::Object::ProcDataDelete
	\brief Data deletion callback pointer

	Function pointer to delete data that's associated with this
	LinkedListObjects::Object

	\param pObject Pointer to the object to act upon.

	\sa Object, proc_null(Object*), proc_free_object_and_data(Object*),
		proc_free_data(Object*) or proc_free_object(Object*)

***************************************/

/*! ************************************

	\brief Memory release proc for doing nothing

	For data that is allocated and released by other means, use this function to
	perform no action when an item is deleted from the linked list.

	\param pObject Pointer to the Object to delete (Not used)

	\sa ProcDataDelete, proc_free_object_and_data(Object*),
		proc_free_data(Object*), or proc_free_object(Object*)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::proc_null(
	Object* /* pObject */) BURGER_NOEXCEPT
{
}

/*! ************************************

	\brief Memory release proc for releasing the data and object

	Call Burger::free_memory() on both the Object and data pointers. This is the
	default behavior if no function was supplied

	\param pObject Pointer to the Object and data to delete

	\sa ProcDataDelete, proc_null(Object*), proc_free_data(Object*) or
		proc_free_object(Object*)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::proc_free_object_and_data(
	Object* pObject) BURGER_NOEXCEPT
{
	free_memory(pObject->m_pData);
	free_memory(pObject);
}

/*! ************************************

	\brief Memory release proc for releasing the data only

	Call Burger::free_memory() on the data pointer only. The Object is assumed to be
	either static or allocated elsewhere

	The data pointer is set to \ref nullptr

	\param pObject Pointer to the Object with data to delete (The object's
		pointer is not disposed of)

	\sa ProcDataDelete, proc_null(Object*),
		proc_free_object_and_data(Object*),
		or proc_free_object(Object*)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::proc_free_data(
	Object* pObject) BURGER_NOEXCEPT
{
	free_memory(pObject->m_pData);
	pObject->m_pData = nullptr;
}

/*! ************************************

	\brief Memory release proc for releasing the object only

	Call Burger::free_memory() on the Object pointer only. The data is assumed to
	be either static or allocated elsewhere

	\param pObject Pointer to the Object to delete

	\sa ProcDataDelete, proc_null(Object*),
		proc_free_object_and_data(Object*),
		or proc_free_data(Object*)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::proc_free_object(
	Object* pObject) BURGER_NOEXCEPT
{
	free_memory(pObject);
}

/*! ************************************

	\fn Burger::LinkedListObjects::Object::Object()
	\brief Default constructor

	Initialize the structure and set up a deallocation pointer that will
	only dispose of the data. This constructor is the one used for derived
	classes or static instances where the disposal of this class
	is handled by the compiler.

	\sa Object(void*, ProcDataDelete) or proc_free_data(Object*)

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::Object::Object(
		void *pData, ProcDataDelete pDataDelete)

	\brief Default constructor

	Initialize the structure and set up a deallocation pointer that will only
	dispose of the data. This constructor is the one used for derived classes or
	static instances where the disposal of this class requires finer control.

	\param pData Pointer to data to store in this object
	\param pDataDelete Pointer to function to dispose of this object

	\sa Object() or proc_free_data()

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::Object::~Object()
	\brief Destructor

	Unlink this object from the parent linked list and then dispose of the
	contents. All of this is performed in a call to shutdown()

	\note Under normal circumstances, the disposal function will not dispose of
	the object itself since calling a destructor implies that destruction is
	occuring by the caller. If an Object is assumed to be disposed of by the use
	of a destructor, only use the functions proc_free_data() or proc_null().

	\sa shutdown(), Object() or Object(void *,ProcDataDelete)

***************************************/

/*! ************************************

	\brief Dispose of a single object

	Unlink this object from the parent linked list and then dispose of the
	contents.

	\note The behavior of the memory deallocation function is dependent
		on the active memory disposal function.

	\sa ProcDataDelete, ~Object() or DoubleLinkedList::detach()

***************************************/

void Burger::LinkedListObjects::Object::shutdown(void) BURGER_NOEXCEPT
{
	detach();

	// Call the disposal proc
	if (m_ProcDataDelete) {
		m_ProcDataDelete(this);
	}
}

/*! ************************************

	\fn void Burger::LinkedListObjects::Object::insert_before(Object *pObject)
	\brief Attach the input object before this one in the list.

	Detach the input object from any linked list and then attach the input
	object before this one in the current list.

	\param pObject Pointer to a Object object to link before this one

	\sa insert_after(Object *)

***************************************/

/*! ************************************

	\fn void Burger::LinkedListObjects::Object::insert_after(Object *pObject)
	\brief Attach the input object after this one in the list.

	Detach the input object from any linked list and then attach the input
	object after this one in the current list.

	\param pObject Pointer to a Object object to link after this one

	\sa insert_before(Object *)

***************************************/

/*! ************************************

	\brief Create a new Object instance

	Allocate memory using Burger::allocate_memory() and initialize an Object with it.

	\note Since this object was allocated with the Burger::allocate_memory() function, use
		the disposal functions proc_free_object(Object*) or
		proc_free_object_and_data(Object*) to release it

	\param pData Pointer to the data this object will control, \ref nullptr is
		acceptable
	\param pProc Pointer to the disposal function

	\return \ref nullptr if out of memory

	\sa shutdown(), proc_free_object(Object*),
		or proc_free_object_and_data(Object*)

***************************************/

Burger::LinkedListObjects::Object* BURGER_API
Burger::LinkedListObjects::Object::new_object(
	void* pData, ProcDataDelete pProc) BURGER_NOEXCEPT
{
	// Manually allocate the memory
	return new (allocate_memory(sizeof(Object))) Object(pData, pProc);
}

/*! ************************************

	\fn Burger::LinkedListObjects::Object * \
		Burger::LinkedListObjects::Object::get_next(void) const
	\brief Get the next pointer in the list.

	Return the pointer to the next object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the start and end
	are detected properly

	\return Pointer to the next Object in the chain

	\sa get_previous(void) const

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::Object * \
		Burger::LinkedListObjects::Object::get_previous(void) const
	\brief Get the previous pointer in the list.

	Return the pointer to the previous object in the list. Since the list is
	circular, it's the caller's responsibility to ensure that the start and end
	are detected properly

	\return Pointer to the previous Object in the chain

	\sa get_next(void) const

***************************************/

/*! ************************************

	\fn void * Burger::LinkedListObjects::Object::get_data(void) const
	\brief Get the data pointer contained in the Object

	Return the application supplied data pointer. It can be \ref nullptr

	\return Data pointer contained within

	\sa set_data(void*)

***************************************/

/*! ************************************

	\fn void Burger::LinkedListObjects::Object::set_data(void *pInput)
	\brief Set the data pointer that will be contained in the Object

	Sets the a new data pointer. It can be \ref nullptr

	\param pInput The new data pointer for the Object

	\sa get_data(void) const

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::Object::ProcDataDelete \
		Burger::LinkedListObjects::Object::get_data_delete_proc(void) const
	\brief Get the disposal function pointer associated with the Object

	Return the application supplied data disposal pointer.

	\return Disposal function pointer contained within
	\sa set_data_delete_proc(ProcDataDelete)

***************************************/

/*! ************************************

	\fn void Burger::LinkedListObjects::Object::set_data_delete_proc(
		Burger::LinkedListObjects::Object::ProcDataDelete pProc)
	\brief Set the disposal function pointer that will be contained in the
		Object

	Sets the a new disposal function pointer.

	\param pProc The new disposal function pointer for the Object

	\sa get_data_delete_proc(void) const

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::LinkedListObjects()
	\brief Initialize to default

	Creates an empty class.

	\sa ~LinkedListObjects() or shutdown()

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::~LinkedListObjects()
	\brief Dispose of all data

	Calls shutdown() to dispose of all the linked list members

	\sa LinkedListObjects() or shutdown()

***************************************/

/*! ************************************

	\brief Dispose of all data

	Iterate though all of the linked list items and either use a default
	destructor or a custom destructor to dispose of the contents

	\sa ~LinkedListObjects() or destroy(Object*)

***************************************/

void Burger::LinkedListObjects::shutdown(void) BURGER_NOEXCEPT
{
	// Get the root pointer
	Object* pStart = m_pRoot;
	if (pStart) {
		do {
			// Destroying the object removes it from the list
			destroy(pStart);
		} while ((pStart = m_pRoot) != nullptr);
	}
}

/*! ************************************

	\fn Burger::LinkedListObjects::get_count(void) const
	\brief Return the number of Objects in the list

	Return 0 if the list is empty, the count of objects otherwise.

	\sa get_last() or get_first()

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::Object *Burger::LinkedListObjects::get_first(
		void) const
	\brief Return the pointer to the first object in the list

	If there are no entries in the list, return \ref nullptr. Otherwise, return
	the pointer of the first item in the linked list.

	\note This function should not be used to determine if the list is empty.
	Use get_count() instead.

	\sa get_last(), get_first_data() or get_count()

***************************************/

/*! ************************************

	\brief Return the pointer to the last object in the list

	If there are no entries in the list, return \ref nullptr. Otherwise, return
	the pointer of the last item in the linked list.

	\note This function should not be used to determine if the list is empty.
		Use get_count() instead.

	\sa get_last_data(), get_first() or get_count()

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::get_last(
	void) const BURGER_NOEXCEPT
{
	// Get the root pointer, if there is anything
	Object* pResult = m_pRoot;
	if (pResult) {
		// Step back one (Could be the same one!)
		pResult = pResult->get_previous();
	}
	// Return the object pointer
	return pResult;
}

/*! ************************************

	\brief Return the data pointer from the first object in the list

	If there are no entries in the list, return \ref nullptr. Otherwise, return
	the data pointer of the first item in the linked list.

	\note This function should not be used to determine if the list is empty.
		Use get_count() instead.

	\sa get_first(), get_last_data() or get_count()

***************************************/

void* Burger::LinkedListObjects::get_first_data(void) const BURGER_NOEXCEPT
{
	// Get the root pointer
	Object* pRoot = m_pRoot;

	// Assume no data
	void* pResult = nullptr;

	// Valid pointer?
	if (pRoot) {

		// Obtain the data pointer (Which could be nullptr)
		pResult = pRoot->get_data();
	}
	// Return with the data or nullptr
	return pResult;
}

/*! ************************************

	\brief Return the data pointer from the last object in the list

	If there are no entries in the list, return \ref nullptr. Otherwise,
	return the data pointer of the last item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use get_count() instead.

	\sa get_last(), get_first_data() or get_count()

***************************************/

void* Burger::LinkedListObjects::get_last_data(void) const BURGER_NOEXCEPT
{
	// Get the root pointer
	Object* pRoot = m_pRoot;

	// Assume no data
	void* pResult = nullptr;

	// Valid pointer?
	if (pRoot) {
		// Go to the last entry (Could be itself if only one entry)
		pRoot = pRoot->get_previous();

		// Obtain the data pointer (Could be nullptr)
		pResult = pRoot->get_data();
	}

	// Return with the data or nullptr
	return pResult;
}

/*! ************************************

	\brief Get the data from an indexed entry in the list

	Iterate over the linked list "uIndex" times and return the data from that
	object.

	If the index is out of bounds, return \ref nullptr

	\param uIndex 0 for the first entry, 1 for the second and so on.

	\return Data in the Object::get_data() field.

	\sa Object::get_data()

***************************************/

void* Burger::LinkedListObjects::get_data(uint32_t uIndex) const BURGER_NOEXCEPT
{
	void* pResult = nullptr;

	// Invalid count?
	if (uIndex < m_uCount) {

		// Get the first entry
		Object* pObject = m_pRoot;

		// Should I traverse?
		if (uIndex) {
			do {
				// Follow the list
				pObject = pObject->get_next();
				// All done?
			} while (--uIndex);
		}

		// Found it!
		pResult = pObject->get_data();
	}

	return pResult;
}

/*! ************************************

	\brief Get the data from an indexed entry in the list

	Iterate over the linked list "uIndex" times and return the data from that
	object.

	If the index is out of bounds, return \ref nullptr

	\param uIndex 0 for the first entry, 1 for the second and so on.

	\return Data in the Object::get_data() field.

	\sa Object::get_data()

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::get_object(
	uint32_t uIndex) const BURGER_NOEXCEPT
{
	Object* pObject = nullptr;

	// Invalid count?
	if (uIndex < m_uCount) {

		// Get the first entry
		pObject = m_pRoot;

		// Should I traverse?
		if (uIndex) {
			do {
				// Follow the list
				pObject = pObject->get_next();
				// All done?
			} while (--uIndex);
		}
	}
	return pObject;
}

/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied data

	Returns \ref nullptr if the data is not in the list

	\param pData Data pointer to scan for

	\return Pointer to the located Object or \ref nullptr if a match was not
		found

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::get_object(
	void* pData) const BURGER_NOEXCEPT
{
	// Get the first entry
	Object* pResult = m_pRoot;

	// Should I traverse?
	if (pResult) {
		uint32_t uCount = m_uCount;
		Object* pObject = pResult;
		do {
			// Match to the data?
			if (pObject->get_data() == pData) {
				pResult = pObject;
				break;
			}

			// Follow the list
			pObject = pObject->get_next();

			// All done?
		} while (--uCount);
	}
	return pResult;
}

/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied "C"
		string

	Returns \ref nullptr if the string is not in the list. The comparison is
	case insensitive

	\param pString A pointer to a "C" string to scan for

	\return Pointer to the located Object or \ref nullptr if a match was not
		found

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::get_string_object(
	const char* pString) const BURGER_NOEXCEPT
{
	// Get the first entry
	Object* pResult = m_pRoot;

	// Should I traverse?
	if (pResult) {
		uint32_t uCount = m_uCount;
		Object* pObject = pResult;
		do {
			if (!StringCaseCompare(
					static_cast<const char*>(pObject->get_data()), pString)) {
				pResult = pObject;
				break;
			}
			// Follow the list
			pObject = pObject->get_next();

			// All done?
		} while (--uCount);
	}
	return pResult;
}

/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied "C"
		string

	Returns UINT32_MAX if the string is not in the list. The comparison is case
	insensitive

	0 is the first object, 1 is the second object and so on...

	\param pString A pointer to a "C" string to scan for

	\return Numeric index in the list of the location of the string or -1 if not
		found

***************************************/

uint32_t Burger::LinkedListObjects::get_string_index(
	const char* pString) const BURGER_NOEXCEPT
{
	uint32_t uResult = UINT32_MAX;

	// Get the first entry
	Object* pObject = m_pRoot;

	// Should I traverse?
	if (pObject) {
		uint32_t uCount = m_uCount;
		do {
			if (!StringCaseCompare(
					static_cast<const char*>(pObject->get_data()), pString)) {

				// Determine the index number
				uResult = m_uCount - uCount;
				break;
			}

			// Follow the list
			pObject = pObject->get_next();

			// All done?
		} while (--uCount);
	}
	return uResult;
}

/*! ************************************

	\brief Remove an object from the list and dispose of it

	If the input is not \ref nullptr, detach the object from the list and then
	dispose of it. If the object was the head of the list, the next item will be
	made the new head. If this was the only item in the list, the list will be
	have the head set to \ref nullptr

	\param pObject Pointer to the Object to delete

	\sa append(Object*)

***************************************/

void Burger::LinkedListObjects::destroy(Object* pObject) BURGER_NOEXCEPT
{
	if (pObject) {

		// If this is the root object, update to the new root
		if (pObject == m_pRoot) {
			m_pRoot = pObject->get_next();
		}

		// Dispose of the item
		pObject->shutdown();

		// Any left?
		if (!--m_uCount) {

			// There is no root anymore
			m_pRoot = nullptr;
		}
	}
}

/*! ************************************

	\brief Add a data object to the end of the list

	Accept an Object entry and append it to the end of the linked list.

	\param pObject Pointer to the Object to append

	\sa append(void*, Object::ProcDataDelete), prepend(Object*)

***************************************/

void Burger::LinkedListObjects::append(Object* pObject) BURGER_NOEXCEPT
{
	Object* pRoot = m_pRoot;
	if (!pRoot) {

		// It's the only entry
		m_pRoot = pObject;
		m_uCount = 1;

	} else {

		// Append to the end of the list
		pRoot->insert_before(pObject);

		// Entry added
		++m_uCount;
	}
}

/*! ************************************

	\brief Add a data object to the beginning of the list

	Accept an Object entry and insert it into the beginning of the linked list.

	\param pObject Pointer to the Object to prepend

	\sa prepend(void *,Object::ProcDataDelete), append(Object *)

***************************************/

void Burger::LinkedListObjects::prepend(Object* pObject) BURGER_NOEXCEPT
{
	Object* pRoot = m_pRoot;
	if (!pRoot) {

		// It's the only entry
		m_pRoot = pObject;
		m_uCount = 1;
	} else {

		// Since it's the first one now, set the new root
		m_pRoot = pObject;

		pObject->insert_after(pRoot);

		// Entry added
		++m_uCount;
	}
}

/*! ************************************

	\brief Add a pointer of data to the end of the list

	Allocate a new Object entry and attach this data pointer to it.

	\param pData Pointer to the data pointer
	\param pDataDelete The default data deletion procedure

	\sa append(Object*), prepend(void*, Object::ProcDataDelete)

***************************************/

void Burger::LinkedListObjects::append(
	void* pData, Object::ProcDataDelete pDataDelete) BURGER_NOEXCEPT
{
	// Create the new object
	Object* pObject = Object::new_object(pData, pDataDelete);

	// If successful, append it
	if (pObject) {
		append(pObject);
	}
}

/*! ************************************

	\brief Add a pointer of data to the beginning of the list

	Allocate a new Object entry and attach this data pointer to it. Insert the
	new Object at the beginning of the list.

	\param pData Pointer to the data pointer
	\param pDataDelete The default data deletion procedure

	\sa prepend(Object *), append(void *,Object::ProcDataDelete)

***************************************/

void Burger::LinkedListObjects::prepend(
	void* pData, Object::ProcDataDelete pDataDelete) BURGER_NOEXCEPT
{
	// Create the new object
	Object* pObject = Object::new_object(pData, pDataDelete);

	// If successful, prepend it
	if (pObject) {
		prepend(pObject);
	}
}

/*! ************************************

	\brief Add a copy of a string into the linked list

	Make a copy of the input string and append the string to the end of the
	linked list

	\param pString Pointer to the "C" to copy.

	\sa prepend(const char*)

***************************************/

void Burger::LinkedListObjects::append(const char* pString) BURGER_NOEXCEPT
{
	char* pStringCopy = StringDuplicate(pString);
	if (pStringCopy) {

		// Create the new object
		Object* pObject = Object::new_object(pStringCopy);

		// If successful, append it
		if (pObject) {
			append(pObject);
		}
	}
}

/*! ************************************

	\brief Add a copy of a string into the linked list

	Make a copy of the input string and append the string to the beginning of
	the linked list

	\param pString Pointer to the "C" to copy.

	\sa append(const char*)

***************************************/

void Burger::LinkedListObjects::prepend(const char* pString) BURGER_NOEXCEPT
{
	char* pStringCopy = StringDuplicate(pString);
	if (pStringCopy) {
		// Create the new object
		Object* pObject = Object::new_object(pStringCopy);
		// If successful, append it
		if (pObject) {
			prepend(pObject);
		}
	}
}

/*! ************************************

	\brief Forward iterate over the linked list

	Call a callback function on each and every entry in the list.

	The function returns \ref kContinue to continue iteration, \ref kAbort to
	stop iteration and \ref kDeleteObject to tell the iterator to call the
	disposal function at a safe time. \ref kAbort and \ref kDeleteObject can be
	or'd together to perform both actions.

	\param pProc Function callback

	\return \ref nullptr if the list iterated to the end, or an Object pointer
		when \ref kAbort was returned

	\sa ProcAction or iterate_reverse(ProcAction)

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::iterate_forward(
	ProcAction pProc) BURGER_NOEXCEPT
{
	// Is there a list?
	Object* pFirst = m_pRoot;
	if (pFirst) {

		Object* pObject = pFirst;
		do {
			// Call the function
			const uint32_t uResult = pProc(pObject->get_data());

			// Get the next entry
			Object* pNext = pObject->get_next();

			// Dispose of the entry?
			if (uResult & kDeleteObject) {
				destroy(pObject);
			}

			// Abort the traversal?
			if (uResult & kAbort) {
				pFirst = pObject;
				break;
			}

			// Next entry please
			pObject = pNext;
		} while (pObject != pFirst);
	}

	// Entry stopped on, can be nullptr
	return pFirst;
}

/*! ************************************

	\brief Reverse iterate over the linked list

	Call a callback function on each and every entry in the list.

	The function returns \ref kContinue to continue iteration, \ref kAbort to
	stop iteration and \ref kDeleteObject to tell the iterator to call the
	disposal function at a safe time. \ref kAbort and \ref kDeleteObject can be
	or'd together to perform both actions.

	\param pProc Function callback

	\return \ref nullptr if the list iterated to the end, or an Object pointer
		when \ref kAbort was returned

	\sa ProcAction or iterate_forward(ProcAction)

***************************************/

Burger::LinkedListObjects::Object* Burger::LinkedListObjects::iterate_reverse(
	ProcAction pProc) BURGER_NOEXCEPT
{
	// Is there a list?
	Object* pFirst = m_pRoot;
	if (pFirst) {

		// Start at the last entry
		pFirst = pFirst->get_previous();
		Object* pObject = pFirst;
		do {
			// Call the function
			const uint32_t uResult = pProc(pObject->get_data());

			// Get the next entry
			Object* pPrevious = pObject->get_previous();

			// Dispose of the entry?
			if (uResult & kDeleteObject) {
				destroy(pObject);
			}

			// Abort the traversal?
			if (uResult & kAbort) {
				pFirst = pObject;
				break;
			}

			// Next entry please
			pObject = pPrevious;
		} while (pObject != pFirst);
	}

	// Entry stopped on
	return pFirst;
}
