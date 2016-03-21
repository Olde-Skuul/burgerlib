/***************************************

	Linked list of data objects manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brlinkedlistobject.h"
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"

/*! ************************************

	\class Burger::LinkedListObjects
	\brief Linked list object class

	The linked list manager works this way.
	A Burger::LinkedListObjects is the root entry.
	It contains all the	entries of the list and all calls
	use this root entry as a starting point.

	The Burger::LinkedListObjects::Object's are where the magic happens.
	It is a member of a circular doubly linked list. The root Burger::LinkedListObjects
	structure is where you can find the beginning and the ending
	of the entire list quickly.

	Each Burger::LinkedListObjects::Object also has a pointer to
	the generic data that is contained and also a pointer to a function to
	discard the data once the entry itself is deleted. The
	function pointer could be \ref NULL 
	meaning that the data can be deleted with a simple call to
	Burger::Free(). The data could also be pointing to the end
	of the base Burger::LinkedListObjects::Object structure if the
	Burger::LinkedListObjects::Object
	structure is part of a larger structure.

	All Linked list manager calls are pointer based. There is
	no support of handles. However the Data field could be a
	handle and you can pass your own data destructor routine
	to discard the handle. There is a convenience routine to add
	data if the data is a handle. But you have to typecast
	the void * to a void ** manually when you access the data

***************************************/


/*! ************************************
	
	\typedef Burger::LinkedListObjects::ProcAction
	\brief Iterator callback pointer

	When IterateForward() or IterateReverse() is called, this function pointer
	is used to perform an action with every individual object data.

	The function returns \ref CONTINUE to continue iteration, \ref ABORT to stop iteration
	and \ref DELETEOBJECT to tell the iterator to call the disposal function at
	a safe time. \ref ABORT and \ref DELETEOBJECT can be or'd together to perform both actions.

	\sa IterateForward() or IterateReverse()

***************************************/


/*! ************************************
	
	\class Burger::LinkedListObjects::Object

	Instance of a linked list object that contains a pointer to
	arbitrary data.

	\sa Burger::LinkedListObjects

***************************************/

/*! ************************************
	
	\typedef Burger::LinkedListObjects::Object::ProcDataDelete
	\brief Data deletion callback pointer

	Function pointer to delete data that's associated with this
	Burger::LinkedListObjects::Object

	\sa Object, ProcNull(Object *), ProcFreeObjectAndData(Object *),
		ProcFreeData(Object *) or ProcFreeObject(Object *)

***************************************/


/*! ************************************
	
	\brief Memory release proc for doing nothing

	For data that is allocated and released by other means, use
	this function to perform no action when an item is deleted
	from the linked list.

	\param pObject Pointer to the Object to delete (Not used)
	\sa ProcDataDelete, ProcFreeObjectAndData(Object *),ProcFreeData(Object *) or ProcFreeObject(Object *)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::ProcNull(Object * /* pObject */)
{
}

/*! ************************************
	
	\brief Memory release proc for releasing the data and object

	Call Burger::Free() on both the Object and data pointers. This
	is the default behavior if no function was supplied

	\param pObject Pointer to the Object and data to delete
	\sa ProcDataDelete, ProcNull(Object *),ProcFreeData(Object *) or ProcFreeObject(Object *)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::ProcFreeObjectAndData(Object *pObject)
{
	Free(pObject->m_pData);
	Free(pObject);
}

/*! ************************************
	
	\brief Memory release proc for releasing the data only

	Call Burger::Free() on the data pointer only. The Object is assumed to 
	be either static or allocated elsewhere

	The data pointer is set to \ref NULL

	\param pObject Pointer to the Object with data to delete (The object's pointer is not disposed of)
	\sa ProcDataDelete, ProcNull(Object *),ProcFreeObjectAndData(Object *) or ProcFreeObject(Object *)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::ProcFreeData(Object *pObject)
{
	Free(pObject->m_pData);
	pObject->m_pData = NULL;
}

/*! ************************************
	
	\brief Memory release proc for releasing the object only

	Call Burger::Free() on the Object pointer only. The data is assumed to 
	be either static or allocated elsewhere

	\param pObject Pointer to the Object to delete
	\sa ProcDataDelete, ProcNull(Object *),ProcFreeObjectAndData(Object *) or ProcFreeData(Object *)

***************************************/

void BURGER_API Burger::LinkedListObjects::Object::ProcFreeObject(Object *pObject)
{
	Free(pObject);
}



/*! ************************************
	
	\fn Burger::LinkedListObjects::Object::Object()
	\brief Default constructor

	Initialize the structure and set up a deallocation pointer that will
	only dispose of the data. This constructor is the one used for derived
	classes or static instances where the disposal of this class
	is handled by the compiler.

	\sa Object(void *,ProcDataDelete) or ProcFreeData()

***************************************/

/*! ************************************
	
	\fn Burger::LinkedListObjects::Object::Object(void *pData,ProcDataDelete pDataDelete)
	\brief Default constructor

	Initialize the structure and set up a deallocation pointer that will
	only dispose of the data. This constructor is the one used for derived
	classes or static instances where the disposal of this class
	requires finer control.

	\param pData Pointer to data to store in this object
	\param pDataDelete Pointer to function to dispose of this object
	\sa Object() or ProcFreeData()

***************************************/

/*! ************************************
	
	\fn Burger::LinkedListObjects::Object::~Object()
	\brief Destructor

	Unlink this object from the parent linked list and then dispose of the
	contents. All of this is performed in a call to Destroy()

	\note Under normal circumstances, the disposal function will not
	dispose of the object itself since calling a destructor implies
	that destruction is occuring by the caller. If an Object
	is assumed to be disposed of by the use of a destructor, only use
	the functions ProcFreeData() or ProcNull().

	\sa Destroy(), Object() or Object(void *,ProcDataDelete)

***************************************/


/*! ************************************
	
	\brief Dispose of a single object

	Unlink this object from the parent linked list and then dispose of the
	contents.

	\note The behavior of the memory deallocation function is dependent
		on the active memory disposal function.

	\sa ProcDataDelete, ~Object() or Detach()

***************************************/

void Burger::LinkedListObjects::Object::Destroy(void)
{
	Detach();
	m_ProcDataDelete(this);		// Call the disposal proc
}

/*! ************************************
	
	\fn void Burger::LinkedListObjects::Object::InsertBefore(Object *pObject)
	\brief Attach the input object before this one in the list.
	
	Detach the input object from any linked list and then
	attach the input object before this one in the current list.

	\param pObject Pointer to a Object object to link before this one
	\sa InsertAfter(Object *)

***************************************/

/*! ************************************
	
	\fn void Burger::LinkedListObjects::Object::InsertAfter(Object *pObject)
	\brief Attach the input object after this one in the list.
	
	Detach the input object from any linked list and then
	attach the input object after this one in the current list.

	\param pObject Pointer to a Object object to link after this one
	\sa InsertBefore(Object *)

***************************************/

/*! ************************************

	\brief Create a new Object instance

	Allocate memory using Burger::Alloc() and 
	initialize an Object with it.

	\note Since this object was allocated with the Burger::Alloc() function, use the
		disposal functions ProcFreeObject() or ProcFreeObjectAndData() to release it

	\param pData Pointer to the data this object will control, \ref NULL is acceptable
	\param pProc Pointer to the disposal function
	\return \ref NULL if out of memory
	\sa Destroy(), ProcFreeObject() or ProcFreeObjectAndData()

***************************************/

Burger::LinkedListObjects::Object * BURGER_API Burger::LinkedListObjects::Object::New(void *pData,ProcDataDelete pProc)
{
	// Manually allocate the memory
	Object *pThis = new (Alloc(sizeof(Object))) Object(pData,pProc);
	// Sorry Charlie!
	return pThis;
}

/*! ************************************
	
	\fn Burger::LinkedListObjects::Object * Burger::LinkedListObjects::Object::GetNext(void) const
	\brief Get the next pointer in the list. 
	
	Return the pointer to the next object in the list.
	Since the list is circular, it's the caller's responsibility to ensure
	that the start and end are detected properly

	\return Pointer to the next Object in the chain 
	\sa GetPrevious(void) const

***************************************/

/*! ************************************
	
	\fn Burger::LinkedListObjects::Object * Burger::LinkedListObjects::Object::GetPrevious(void) const
	\brief Get the previous pointer in the list. 
	
	Return the pointer to the previous object in the list.
	Since the list is circular, it's the caller's responsibility to ensure
	that the start and end are detected properly

	\return Pointer to the previous Object in the chain 
	\sa GetNext(void) const

***************************************/

/*! ************************************
	
	\fn void * Burger::LinkedListObjects::Object::GetData(void) const
	\brief Get the data pointer contained in the Object
	
	Return the application supplied data pointer. It can be \ref NULL

	\return Data pointer contained within
	\sa SetData(void *)

***************************************/

/*! ************************************
	
	\fn void Burger::LinkedListObjects::Object::SetData(void *pInput)
	\brief Set the data pointer that will be contained in the Object
	
	Sets the a new data pointer. It can be \ref NULL

	\param pInput The new data pointer for the Object
	\sa GetData(void) const

***************************************/


/*! ************************************
	
	\fn Burger::LinkedListObjects::Object::ProcDataDelete Burger::LinkedListObjects::Object::GetDataDeleteProc(void) const
	\brief Get the disposal function pointer associated with the Object
	
	Return the application supplied data disposal pointer.

	\return Disposal function pointer contained within
	\sa SetDataDeleteProc(ProcDataDelete)

***************************************/

/*! ************************************
	
	\fn void Burger::LinkedListObjects::Object::SetDataDeleteProc(Burger::LinkedListObjects::Object::ProcDataDelete pProc)
	\brief Set the disposal function pointer that will be contained in the Object
	
	Sets the a new disposal function pointer.

	\param pProc The new disposal function pointer for the Object
	\sa GetDataDeleteProc(void) const

***************************************/





/*! ************************************

	\fn Burger::LinkedListObjects::LinkedListObjects()
	\brief Initialize to default

	Creates an empty class.

	\sa ~LinkedListObjects() or Destroy()

***************************************/

/*! ************************************

	\fn Burger::LinkedListObjects::~LinkedListObjects()
	\brief Dispose of all data

	Calls Destroy() to dispose of all the linked list members

	\sa LinkedListObjects() or Destroy()

***************************************/

/*! ************************************

	\brief Dispose of all data

	Iterate though all of the linked list items and either
	use a default destructor or a custom destructor to dispose of the contents

	\sa ~LinkedListObjects() or DestroyObject(Object *)

***************************************/

void Burger::LinkedListObjects::Destroy(void)
{
	// Get the root pointer
	Object *pStart = m_pRoot;
	if (pStart) {
		do {
			DestroyObject(pStart);
		} while ((pStart = m_pRoot)!=NULL);
	}
}

/*! ************************************
	
	\fn Word Burger::LinkedListObjects::GetCount(void) const
	\brief Return the number of Objects in the list

	Return 0 if the list is empty, the count of objects otherwise.

	\sa GetLast() or GetFirst() 

***************************************/


/*! ************************************
	
	\fn Burger::LinkedListObjects::Object *Burger::LinkedListObjects::GetFirst(void) const
	\brief Return the pointer to the first object in the list

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the pointer of the first item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa GetLast(), GetFirstData() or GetCount()

***************************************/

/*! ************************************

	\brief Return the pointer to the last object in the list

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the pointer of the last item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa GetLastData(), GetFirst() or GetCount()

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::GetLast(void) const
{
	// Get the root pointer
	Object *pResult = m_pRoot;
	if (pResult) {
		// Step back one (Could be the same one!)
		pResult = pResult->GetPrevious();
	}
	// Return the object pointer
	return pResult;
}

/*! ************************************
	
	\brief Return the data pointer from the first object in the list

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the data pointer of the first item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa GetFirst(), GetLastData() or GetCount()

***************************************/

void *Burger::LinkedListObjects::GetFirstData(void) const
{
	Object *pRoot = m_pRoot;	// Get the root pointer
	void *pResult = NULL;		// Assume no data
	if (pRoot) {				// Valid pointer?
		pResult = pRoot->GetData();	// Obtain the data pointer
	}
	// Return with the data or NULL
	return pResult;
}

/*! ************************************
	
	\brief Return the data pointer from the last object in the list

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the data pointer of the last item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa GetLast(), GetFirstData() or GetCount()

***************************************/

void *Burger::LinkedListObjects::GetLastData(void) const
{
	Object *pRoot = m_pRoot;		// Get the root pointer
	void *pResult = NULL;			// Assume no data
	if (pRoot) {					// Valid pointer?
		pRoot = pRoot->GetPrevious();	// Go to the last entry (Could be itself if only one entry)
		pResult = pRoot->GetData();		// Obtain the data pointer
	}
	// Return with the data or NULL
	return pResult;
}

/*! ************************************

	\brief Get the data from an indexed entry in the list

	Iterate over the linked list "uIndex" times and return the data
	from that object.

	If the index is out of bounds, return \ref NULL

	\param uIndex 0 for the first entry, 1 for the second and so on.
	\return Data in the Object::GetData() field.

	\sa Object::GetData()

***************************************/

void *Burger::LinkedListObjects::GetData(Word uIndex) const
{
	void *pResult = NULL;
	if (uIndex<m_uCount) {			// Invalid count?
		Object *pObject = m_pRoot;	// Get the first entry
		if (uIndex) {				// Should I traverse?
			do {
				pObject = pObject->GetNext();	// Follow the list
			} while (--uIndex);			// All done?
		}
		pResult = pObject->GetData();	// Found it!
	}
	return pResult;
}

/*! ************************************

	\brief Get the data from an indexed entry in the list

	Iterate over the linked list "uIndex" times and return the data
	from that object.

	If the index is out of bounds, return \ref NULL

	\param uIndex 0 for the first entry, 1 for the second and so on.
	\return Data in the Object::GetData() field.

	\sa Object::GetData()

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::GetObject(Word uIndex) const
{
	Object *pObject = NULL;
	if (uIndex<m_uCount) {			// Invalid count?
		pObject = m_pRoot;	// Get the first entry
		if (uIndex) {				// Should I traverse?
			do {
				pObject = pObject->GetNext();	// Follow the list
			} while (--uIndex);			// All done?
		}
	}
	return pObject;
}


/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied data

	Returns \ref NULL if the data is not in the list

	\param pData Data pointer to scan for
	\return Pointer to the located Object or \ref NULL if a match was not found

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::GetObject(void *pData) const
{
	Object *pResult = m_pRoot;	// Get the first entry
	if (pResult) {				// Should I traverse?
		Word uCount = m_uCount;
		Object *pObject = pResult;
		do {
			if (pObject->GetData()==pData) {
				pResult = pObject;
				break;
			}
			pObject = pObject->GetNext();	// Follow the list
		} while (--uCount);		// All done?
	}
	return pResult;
}

/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied "C" string

	Returns \ref NULL if the string is not in the list. The comparison is
	case insensitive

	\param pString A pointer to a "C" string to scan for
	\return Pointer to the located Object or \ref NULL if a match was not found

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::GetStringObject(const char *pString) const
{
	Object *pResult = m_pRoot;	// Get the first entry
	if (pResult) {				// Should I traverse?
		Word uCount = m_uCount;
		Object *pObject = pResult;
		do {
			if (!StringCaseCompare(static_cast<const char *>(pObject->GetData()),pString)) {
				pResult = pObject;
				break;
			}
			pObject = pObject->GetNext();	// Follow the list
		} while (--uCount);		// All done?
	}
	return pResult;
}

/*! ************************************

	\brief Traverse the linked list until an entry matches the supplied "C" string

	Returns -1 if the string is not in the list. The comparison is
	case insensitive

	0 is the first object, 1 is the second object and so on...

	\param pString A pointer to a "C" string to scan for
	\return Numeric index in the list of the location of the string or -1 if not found

***************************************/

Word Burger::LinkedListObjects::GetStringIndex(const char *pString) const
{
	Word uResult = static_cast<Word>(-1);
	Object *pObject = m_pRoot;	// Get the first entry
	if (pObject) {				// Should I traverse?
		Word uCount = m_uCount;
		do {
			if (!StringCaseCompare(static_cast<const char *>(pObject->GetData()),pString)) {
				uResult = m_uCount-uCount;		// Determine the index number
				break;
			}
			pObject = pObject->GetNext();	// Follow the list
		} while (--uCount);		// All done?
	}
	return uResult;
}

/*! ************************************

	\brief Remove an object from the list and dispose of it

	If the input is not \ref NULL, detach the object from the list
	and then dispose of it. If the object was the head of the list,
	the next item will be made the new head. If this was the only
	item in the list, the list will be have the head set to \ref NULL

	\param pObject Pointer to the Object to delete
	\sa AppendObject(Object *)

***************************************/

void Burger::LinkedListObjects::DestroyObject(Object *pObject)
{
	if (pObject) {
		// If this is the root object, update to the new root
		if (pObject==m_pRoot) {
			m_pRoot = pObject->GetNext();
		}
		// Dispose of the item
		pObject->Destroy();
		// Any left?
		if (!--m_uCount) {
			m_pRoot = NULL;
		}
	}
}


/*! ************************************
	
	\brief Add a data object to the end of the list

	Accept an Object entry and append it to the end of the linked list..

	\sa AppendObject(void *,Object::ProcDataDelete), PrependObject(Object *)

***************************************/

void Burger::LinkedListObjects::AppendObject(Object *pObject)
{
	Object *pRoot = m_pRoot;
	if (!pRoot) {
		// It's the only entry
		m_pRoot = pObject;
		m_uCount = 1;
	} else {
		pRoot->InsertBefore(pObject);
		++m_uCount;				// Entry added
	}
}

/*! ************************************
	
	\brief Add a data object to the beginning of the list

	Accept an Object entry and append it to the beginning of the linked list..

	\sa PrependObject(void *,Object::ProcDataDelete), AppendObject(Object *)

***************************************/

void Burger::LinkedListObjects::PrependObject(Object *pObject)
{
	Object *pRoot = m_pRoot;
	if (!pRoot) {
		// It's the only entry
		m_pRoot = pObject;
		m_uCount = 1;
	} else {
		m_pRoot = pObject;
		pObject->InsertAfter(pRoot);
		++m_uCount;				// Entry added
	}
}

/*! ************************************
	
	\brief Add a pointer of data to the end of the list

	Allocate a new Object entry and attach this data pointer to it.
	The default data deletion procedure 

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the data pointer of the last item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa AppendObject(Object *), PrependObject(void *,Object::ProcDataDelete)

***************************************/

void Burger::LinkedListObjects::AppendObject(void *pData,Object::ProcDataDelete pDataDelete)
{
	// Create the new object
	Object *pObject = Object::New(pData,pDataDelete);
	// If successful, append it
	if (pObject) {
		AppendObject(pObject);
	}
}

/*! ************************************
	
	\brief Add a pointer of data to the beginning of the list

	Allocate a new Object entry and attach this data pointer to it.
	The default data deletion procedure 

	If there are no entries in the list, return \ref NULL. Otherwise,
	return the data pointer of the last item in the linked list.

	\note This function should not be used to determine if the
	list is empty. Use GetCount() instead.

	\sa PrependObject(Object *), AppendObject(void *,Object::ProcDataDelete)

***************************************/

void Burger::LinkedListObjects::PrependObject(void *pData,Object::ProcDataDelete pDataDelete)
{
	// Create the new object
	Object *pObject = Object::New(pData,pDataDelete);
	// If successful, prepend it
	if (pObject) {
		PrependObject(pObject);
	}
}


/*! ************************************
	
	\brief Add a copy of a string into the linked list

	Make a copy of the input string and append the string to the
	end of the linked list

	\sa PrependObject(const char *)

***************************************/

void Burger::LinkedListObjects::AppendObject(const char *pString)
{
	char *pStringCopy = StringDuplicate(pString);
	if (pStringCopy) {
		// Create the new object
		Object *pObject = Object::New(pStringCopy);
		// If successful, append it
		if (pObject) {
			AppendObject(pObject);
		}
	}
}

/*! ************************************
	
	\brief Add a copy of a string into the linked list

	Make a copy of the input string and append the string to the
	beginning of the linked list

	\sa AppendObject(const char *)

***************************************/

void Burger::LinkedListObjects::PrependObject(const char *pString)
{
	char *pStringCopy = StringDuplicate(pString);
	if (pStringCopy) {
		// Create the new object
		Object *pObject = Object::New(pStringCopy);
		// If successful, append it
		if (pObject) {
			PrependObject(pObject);
		}
	}
}

/*! ************************************
	
	\brief Forward iterate over the linked list

	Call a callback function on each and every entry in the list.

	The function returns \ref CONTINUE to continue iteration, \ref ABORT to stop iteration
	and \ref DELETEOBJECT to tell the iterator to call the disposal function at
	a safe time. \ref ABORT and \ref DELETEOBJECT can be or'd together to perform both actions.

	\param pProc Function callback
	\return \ref NULL if the list iterated to the end, or an Object pointer when \ref ABORT was returned
	\sa ProcAction or IterateReverse()

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::IterateForward(ProcAction pProc)
{
	Object *pFirst = m_pRoot;		// Is there a list?
	if (pFirst) {
		Object *pObject = pFirst;
		do {
			Word uResult = pProc(pObject->GetData());	// Call the function
			Object *pNext = pObject->GetNext();			// Get the next entry
			if (uResult&DELETEOBJECT) {					// Dispose of the entry?
				DestroyObject(pObject);
			}
			if (uResult&ABORT) {		// Abort the traversal?
				pFirst = pObject;
				break;
			}
			pObject = pNext;			// Next entry please...
		} while (pObject!=pFirst);		// More left?
	}
	return pFirst;						// Entry I stopped on
}

/*! ************************************
	
	\brief Reverse iterate over the linked list

	Call a callback function on each and every entry in the list.

	The function returns \ref CONTINUE to continue iteration, \ref ABORT to stop iteration
	and \ref DELETEOBJECT to tell the iterator to call the disposal function at
	a safe time. \ref ABORT and \ref DELETEOBJECT can be or'd together to perform both actions.

	\param pProc Function callback
	\return \ref NULL if the list iterated to the end, or an Object pointer when \ref ABORT was returned
	\sa ProcAction or IterateForward()

***************************************/

Burger::LinkedListObjects::Object *Burger::LinkedListObjects::IterateReverse(ProcAction pProc)
{
	Object *pFirst = m_pRoot;		// Is there a list?
	if (pFirst) {
		// Start at the last entry
		pFirst = pFirst->GetPrevious();
		Object *pObject = pFirst;
		do {
			Word uResult = pProc(pObject->GetData());	// Call the function
			Object *pPrevious = pObject->GetPrevious();			// Get the next entry
			if (uResult&DELETEOBJECT) {					// Dispose of the entry?
				DestroyObject(pObject);
			}
			if (uResult&ABORT) {		// Abort the traversal?
				pFirst = pObject;
				break;
			}
			pObject = pPrevious;		// Next entry please...
		} while (pObject!=pFirst);		// More left?
	}
	return pFirst;						// Entry I stopped on
}

