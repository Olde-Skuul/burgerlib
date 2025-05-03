/***************************************

	intrinsic<T> compatible array template

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsimplearray.h"
#include "brdebug.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::SimpleArrayBase
	\brief Base class for SimpleArray

	This class will perform most work for the SimpleArray template class

	\sa SimpleArray or ClassArray

***************************************/

/*! ************************************

	\fn Burger::SimpleArrayBase::SimpleArrayBase(uintptr_t uChunkSize)
	\brief Default constructor.

	Initializes the array to contain no data and have no members and set to a
	specific chunk size.

	\param uChunkSize Size in bytes of each element in the array
	\sa SimpleArrayBase(uintptr_t,uintptr_t) or
		SimpleArrayBase(const SimpleArrayBase&)

***************************************/

/*! ************************************

	\brief Default constructor with a starting array and chunk size

	Initializes the array to contain uDefault number of uninitialized members.

	\note If the initial buffer allocation fails, the array size will be set to
		zero.

	\param uChunkSize Size of each data chunk
	\param uDefault Number of members to create the array with. Zero will
		generate an empty array.

	\sa SimpleArrayBase(uintptr_t) or SimpleArrayBase(const SimpleArrayBase&)

***************************************/

Burger::SimpleArrayBase::SimpleArrayBase(uintptr_t uChunkSize,
	uintptr_t uDefault) BURGER_NOEXCEPT: m_uSize(uDefault),
										 m_uBufferSize(uDefault)
{
	// Minimum chunk size of 1 byte per entry
	if (!uChunkSize) {
		uChunkSize = 1;
	}
	m_uChunkSize = uChunkSize;

	// Anything?
	void* pData = nullptr;
	if (uDefault) {
		// Get the default buffer and die if failed in debug
		pData = allocate_memory(uChunkSize * uDefault);
		if (!pData) {
			// Set the size to empty
			uDefault = 0;
		}
	}
	m_uBufferSize = uDefault;
	m_uSize = uDefault;
	m_pData = pData;
}

/*! ************************************

	\brief Default constructor for making a copy of another SimpleArrayBase

	Initializes the array to contain a copy of another SimpleArrayBase.

	\param rData Reference to a matching SimpleArrayBase type

	\sa SimpleArrayBase(uintptr_t) or SimpleArrayBase(uintptr_t,uintptr_t)

***************************************/

Burger::SimpleArrayBase::SimpleArrayBase(
	const SimpleArrayBase& rData) BURGER_NOEXCEPT: m_pData(nullptr)
{
	const uintptr_t uCount = rData.m_uSize;
	m_uSize = uCount;
	m_uBufferSize = uCount;
	const uintptr_t uChunkSize = rData.m_uChunkSize;
	m_uChunkSize = uChunkSize;
	if (uCount) {
		m_pData = allocate_memory_copy(rData.m_pData, uChunkSize * uCount);
		if (!m_pData) {
			m_uBufferSize = 0;
			m_uSize = 0;
			Debug::Fatal(
				"SimpleArrayBase(const SimpleArrayBase &) allocation failure");
		}
	}
}

/*! ************************************

	\fn Burger::SimpleArrayBase::~SimpleArrayBase()
	\brief Standard destructor

	Releases the memory buffer with a call to free_memory(const void *)

	\sa SimpleArrayBase(uintptr_t), SimpleArrayBase(uintptr_t,uintptr_t) or
		SimpleArrayBase(const SimpleArrayBase&)

***************************************/

Burger::SimpleArrayBase::~SimpleArrayBase(void)
{
	clear();
}

/*! ************************************

	\brief Copy an array into this one

	If the copy is not itself, call clear() to erase the contents of this class
	and make a duplicate of every entry in the rData class into this one.

	\param rData Reference to a matching SimpleArray type
	\return *this
	\sa resize(uintptr_t) or reserve(uintptr_t)

***************************************/

Burger::SimpleArrayBase& Burger::SimpleArrayBase::operator=(
	const SimpleArrayBase& rData) BURGER_NOEXCEPT
{
	// Copying over itself?
	if (&rData != this) {

		// Dispose of the contents
		clear();

		// Get the size to copy
		const uintptr_t uCount = rData.m_uSize;
		const uintptr_t uChunkSize = rData.m_uChunkSize;

		// Chunk size COULD change, bad idea, however, support
		// it to prevent subtle bugs
		m_uChunkSize = uChunkSize;
		if (uCount) {
			// Set the new size
			m_uSize = uCount;
			m_uBufferSize = uCount;
			m_pData = allocate_memory_copy(rData.m_pData, uChunkSize * uCount);
			if (!m_pData) {
				m_uSize = 0;
				m_uBufferSize = 0;
				Debug::Fatal(
					"SimpleArrayBase::operator=(const SimpleArrayBase &) allocation failure");
			}
		}
	}
	return *this;
}

/*! ************************************

	\brief Remove all objects from the array.

	Dispose of the array and set the size to zero.

	\sa resize(uintptr_t) or reserve(uintptr_t)

***************************************/

void BURGER_API Burger::SimpleArrayBase::clear(void) BURGER_NOEXCEPT
{
	free_memory(m_pData);
	m_pData = nullptr;
	m_uBufferSize = 0;
	m_uSize = 0;
}

/*! ************************************

	\brief Remove an object from the array.

	Call the destructor on the specific object in the array and then compact the
	array if needed.

	\param uIndex Index into the array of the object to remove.
	\return Zero on success, or non zero on failure

	\sa resize(uintptr_t) or reserve(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SimpleArrayBase::remove_at(
	uintptr_t uIndex) BURGER_NOEXCEPT
{
	eError uResult = kErrorInvalidParameter;
	uintptr_t uSize = m_uSize;
	if (uIndex < uSize) {
		if (uSize == 1) {
			// Nuke it
			clear();

		} else {
			--uSize;
			m_uSize = uSize;
			const uintptr_t uChunkSize = m_uChunkSize;

			// Calculate the base pointer to the array
			uint8_t* pMark =
				static_cast<uint8_t*>(m_pData) + (uIndex * uChunkSize);

			// Copy over the single entry
			memory_move(
				pMark, pMark + uChunkSize, uChunkSize * (uSize - uIndex));
		}
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Resize the valid entry count of the array.

	If uNewSize is zero, erase all data. If uNewSize increases
	the size of the array, increase the buffer size if necessary. If
	the size is smaller than the existing array, truncate the array.

	In some cases, the buffer size will be reduced if the new size
	is substantially smaller.

	\param uNewSize Number of valid objects the new array will contain.
	\return Zero on success, or non zero on failure

	\sa clear(void) or reserve(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SimpleArrayBase::resize(
	uintptr_t uNewSize) BURGER_NOEXCEPT
{
	eError uResult;
	if (!uNewSize) {
		clear();
		uResult = kErrorNone;

	} else {
		uResult = reserve(uNewSize);
		if (uResult == kErrorNone) {
			// If no error, set the new size
			m_uSize = uNewSize;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Resize the memory used by the array.

	This function sets the size of the master buffer which can exceed
	the number of valid entries in the array. This is a performance
	function in that if it's known at runtime what is the maximum
	memory requirements for this array, it can be pre-allocated
	and all functions can use this buffer until the class
	is disposed of without any intermediate memory allocation
	calls.

	If the reservation size is zero, the array is released.

	The array size will be adjusted to the match the buffer size.

	\param uNewBufferSize Size in elements of the memory buffer.
	\return Zero on success, or non zero on failure

	\sa clear(void) or resize(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SimpleArrayBase::reserve(
	uintptr_t uNewBufferSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Resize the buffer.
	if (!uNewBufferSize) {
		clear();

	} else {

		// If the reservation size truncates the buffer, update the size
		if (m_uSize > uNewBufferSize) {
			m_uSize = uNewBufferSize;
		}
		void* pData = reallocate_memory(m_pData, m_uChunkSize * uNewBufferSize);

		// If a bad pointer, return the error
		if (!pData) {
			uResult = kErrorOutOfMemory;
			uNewBufferSize = 0;
			m_uSize = 0;
		}
		m_uBufferSize = uNewBufferSize;
		m_pData = pData;
	}
	return uResult;
}

/*! ************************************

	\brief Append an array of object to this array.

	Given a base pointer and an object count, iterate over the objects and copy
	them to the end of this array. This function will increase the size of the
	buffer if needed.

	\param pData Pointer to the first element in an array of objects
	\param uCount Number of elements in the array
	\return Zero on success, or non zero on failure

	\sa resize(uintptr_t) or reserve(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SimpleArrayBase::append(
	const void* pData, uintptr_t uCount) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	// No new entries?
	if (uCount) {

		// Expand the buffer by the entry count
		const uintptr_t uSize = m_uSize;
		uResult = resize(uSize + uCount);

		// Success?
		if (uResult == kErrorNone) {
			// Copy in the new entries
			const uintptr_t uChunkSize = m_uChunkSize;
			memory_copy(static_cast<uint8_t*>(m_pData) + (uSize * uChunkSize),
				pData, uCount * uChunkSize);
		}
	}
	return uResult;
}

/*! ************************************

	\class Burger::SimpleArray
	\brief Template for creating arrays of intrinsic objects

	This template class will dynamically allocate memory as needed. Since
	it's assuming the data is of intrinsic values, no initialization or shutdown
	is performed on the elements. If this behavior is desired, use the
	ClassArray template instead.

	While it uses function names that match the class vector<T>, this is a
	lightweight version which doesn't match 100% due to implementation for
	performance.

	\sa ClassArray

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::SimpleArray()
	\brief Default constructor.

	Initializes the array to contain no data and have no members.

	\sa SimpleArray(uintptr_t) or SimpleArray(const SimpleArray& rData)

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::SimpleArray()
	\brief Default constructor.

	Initializes the array to contain no data and have no members.

	\sa SimpleArray(uintptr_t) or SimpleArray(const SimpleArray&)

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::SimpleArray(uintptr_t uDefault)
	\brief Default constructor with a starting array

	Initializes the array to contain uDefault number of uninitialized members.

	\param uDefault Number of members to create the array with. Zero
		will generate an empty array.

	\sa SimpleArray() or SimpleArray(const SimpleArray&)

***************************************/

/*! ************************************

	\fn T * Burger::SimpleArray::GetPtr(void)
	\brief Obtain a pointer to the array.

	Allow direct access to the base pointer to the array. This can be \ref
	nullptr if the array is empty.

	\return A pointer to the array.
	\sa GetPtr(void) const or front(void)

***************************************/

/*! ************************************

	\fn const T * Burger::SimpleArray::GetPtr(void) const
	\brief Obtain a constant pointer to the array.

	Allow direct access to the base pointer to the array. This can be \ref
	nullptr if the array is empty.

	\return A constant pointer to the array.
	\sa GetPtr(void) or front(void) const

***************************************/

/*! ************************************

	\fn T * Burger::SimpleArray::data(void)
	\brief Obtain a pointer to the array.

	Allow direct access to the base pointer to the array. This can be \ref
	nullptr if the array is empty.

	\return A pointer to the array.
	\sa data(void) const or front(void)

***************************************/

/*! ************************************

	\fn const T * Burger::SimpleArray::data(void) const
	\brief Obtain a constant pointer to the array.

	Allow direct access to the base pointer to the array. This can be \ref
	nullptr if the array is empty.

	\return A constant pointer to the array.
	\sa data(void) or front(void) const

***************************************/

/*! ************************************

	\fn T & Burger::SimpleArray::operator[](uintptr_t uIndex)
	\brief Obtain a reference to an item in the array.

	Index into the array and return a reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT() if the uIndex
		value exceeds the size of the number of valid entries in the array.

	\param uIndex Object number in the array to retrieve a reference to.
	\return A reference of the object indexed.
	\sa operator[](uintptr_t) const or GetIndexedItem(uintptr_t)

***************************************/

/*! ************************************

	\fn const T & Burger::SimpleArray::operator[](uintptr_t uIndex) const
	\brief Obtain a constant reference to an item in the array.

	Index into the array and return a constant reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT()
	if the uIndex value exceeds the size of the number of valid
	entries in the array.

	\param uIndex Object number in the array to retrieve a constant reference
		to.

	\return A constant reference of the object indexed.

	\sa operator[](uintptr_t) or GetIndexedItem(uintptr_t) const

***************************************/

/*! ************************************

	\fn T & Burger::SimpleArray::GetIndexedItem(uintptr_t uIndex)
	\brief Obtain a reference to an item in the array.

	Index into the array and return a reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT()
	if the uIndex value exceeds the size of the number of valid
	entries in the array.

	\param uIndex Object number in the array to retrieve a reference to.

	\return A reference of the object indexed.

	\sa operator[](uintptr_t) or GetIndexedItem(uintptr_t) const

***************************************/

/*! ************************************

	\fn const T & Burger::SimpleArray::GetIndexedItem(uintptr_t uIndex) const
	\brief Obtain a constant reference to an item in the array.

	Index into the array and return a constant reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT()
	if the uIndex value exceeds the size of the number of valid
	entries in the array.

	\param uIndex Object number in the array to retrieve a constant reference
		to.

	\return A constant reference of the object indexed.

	\sa operator[](uintptr_t) const or GetIndexedItem(uintptr_t)

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SimpleArray::capacity(void) const
	\brief Return the number of objects the current buffer could hold.

	The buffer size may exceed the number of valid objects, so that if the array
	grew, memory allocations won't be needed. This function will return the size
	of the true buffer.

	\return Number of objects the currently allocated buffer can hold.
	\sa size(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SimpleArray::max_size(void)
	\brief Return the maximum number of objects the buffer could ever hold.

	Given the maximum possible size of memory in the machine, return the
	theoretical maximum number of objects the buffer could hold.

	\return Number of objects the buffer could possibly hold.
	\sa capacity(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SimpleArray::size(void) const
	\brief Return the valid number of objects in the array.

	This value is less than or equal to the buffer size.

	\return Number of valid objects in the array.
	\sa max_size(void) const or empty(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SimpleArray::empty(void) const
	\brief Return \ref TRUE if the array is empty.

	If there is no valid data in the array, return \ref TRUE.

	\return \ref TRUE if the valid object count is zero, \ref FALSE if there are
		valid objects

	\sa size(void) const

***************************************/

/*! ************************************

	\fn T & Burger::SimpleArray::front(void)
	\brief Obtain a reference to first item in the array.

	Return a reference to the first object.

	\return A reference to the first object in the array
	\sa back(void) or front(void) const

***************************************/

/*! ************************************

	\fn const T & Burger::SimpleArray::front(void) const
	\brief Obtain a constant reference to first item in the array.

	Return a constant reference to the first object.

	\return A constant reference to the first object in the array
	\sa back(void) const or front(void)

***************************************/

/*! ************************************

	\fn T & Burger::SimpleArray::back(void)
	\brief Obtain a reference to last item in the array.

	Return a reference to the last object.

	\return A reference to the last object in the array
	\sa front(void) or back(void) const

***************************************/

/*! ************************************

	\fn const T & Burger::SimpleArray::back(void) const
	\brief Obtain a constant reference to last item in the array.

	Return a constant reference to the last object.

	\return A constant reference to the last object in the array
	\sa front(void) const or back(void)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::SimpleArray::push_back(T rData)
	\brief Append an object to the end of the array.

	Make a copy of the object at the end of the array. If there is no room for
	the new object, increase the size of the buffer to make room. Buffer size
	increases are made in groups to reduce memory allocation calls to improve
	performance.

	\param rData An instance of the object to copy at the end of the array
	\return Zero on success, or non zero on failure

	\sa pop_back(void), insert_at(uintptr_t,T), resize(uintptr_t) or
		reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::SimpleArray::pop_back(void)
	\brief Remove an object from the end of the array.

	Call the destructor on the last object in the array and reduce the array
	size by one.

	\return Zero on success, or non zero on failure

	\sa push_back(T), remove_at(uintptr_t), resize(uintptr_t) or
		reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn SimpleArray & Burger::SimpleArray::operator=(const SimpleArray &rData)
	\brief Copy an array into this one

	If the copy is not itself, call clear() to erase the contents of this class
	and make a duplicate of every entry in the rData class into this one.

	\param rData Reference to a matching SimpleArray type

	\return *this

	\sa resize(uintptr_t) or reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::SimpleArray::insert_at(uintptr_t uIndex,T rData)
	\brief Insert an object into the array.

	Expand the buffer if needed and make a copy of the rData object into the
	array at the index uIndex.

	\param uIndex Index into the array for the location of the object to insert.
	\param rData Reference to the object to copy into the array
	\return Zero on success, or non zero on failure

	\sa remove_at(uintptr_t), resize(uintptr_t) or reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::SimpleArray::remove(T rData)
	\brief Find an item and remove it from the array.

	Given an item, scan the array for the first element found and then remove
	the item.

	\note If there are multiple copies of the item in the array, this function
		only removes the first occurrence, not all copies.

	\param rData Copy of the item to remove

	\return \ref TRUE if an item was removed, \ref FALSE if not

	\sa append(const SimpleArray&), insert_at(uintptr_t,T), resize(uintptr_t) or
		reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::SimpleArray::contains(T rData) const
	\brief Find an item in the array.

	Given an item, scan the array for an element that matches the value.

	\param rData Copy of the item to search for
	\return \ref TRUE if an item was found, \ref FALSE if not
	\sa append(const SimpleArray&), insert_at(uintptr_t,T), resize(uintptr_t) or
		reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn void Burger::SimpleArray::append(const T *pSourceData,uintptr_t uCount)
	\brief Append an array of object to this array.

	Given a base pointer and an object count, iterate over the objects and copy
	them to the end of this array. This function will increase the size of the
	buffer if needed.

	\param pSourceData Pointer to the first element in an array of objects

	\param uCount Number of elements in the array

	\return Zero on success, or non zero on failure

	\sa append(const SimpleArray&), insert_at(uintptr_t,T), resize(uintptr_t) or
		reserve(uintptr_t)

***************************************/

/*! ************************************

	\fn void Burger::SimpleArray::append(const SimpleArray& rData)
	\brief Append an array to this array.

	Given another array, iterate over the objects contained within and copy them
	to the end of this array. This function will increase the size of the buffer
	if needed.

	\param rData Reference to a like typed SimpleArray to copy from.
	\return Zero on success, or non zero on failure

	\sa append(const T *,uintptr_t), resize(uintptr_t) or reserve(uintptr_t)

***************************************/

/*! ************************************

	\typedef Burger::SimpleArray::iterator
	\brief STL compatible iterator

	Iterator object for using functions in &lt;algorithm&gt;

	\sa const_iterator, value_type, begin(void) or end(void)

***************************************/

/*! ************************************

	\typedef Burger::SimpleArray::const_iterator
	\brief STL compatible iterator

	Constant iterator object for using functions in &lt;algorithm&gt;

	\sa iterator, value_type, begin(void) const or end(void) const

***************************************/

/*! ************************************

	\typedef Burger::SimpleArray::value_type
	\brief STL compatible type declaration

	Type declarator to use in functions in &lt;algorithm&gt;. It's used to
	create variables of T by using this typedef in the class.

	\sa iterator or const_iterator

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::iterator Burger::SimpleArray::begin(void)
	\brief Iterator for the start of the array.

	STL compatible iterator for the start of the array.

	\return Iterator for starting a traversal of the array.
	\sa end(void) or begin(void) const

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::iterator Burger::SimpleArray::end(void)
	\brief Iterator for the end of the array.

	STL compatible iterator for the end of the array.

	\return Iterator for ending a traversal of the array.
	\sa begin(void) or end(void) const

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::const_iterator Burger::SimpleArray::begin(
		void) const
	\brief Constant iterator for the start of the array.

	STL compatible constant iterator for the start of the array.

	\return Constant iterator for starting a traversal of the array.
	\sa end(void) const or begin(void)

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::const_iterator Burger::SimpleArray::end(void) const
	\brief Constant iterator for the end of the array.

	STL compatible constant iterator for the end of the array.

	\return Constant iterator for ending a traversal of the array.
	\sa begin(void) const or end(void)

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::const_iterator Burger::SimpleArray::cbegin(
		void) const
	\brief Constant iterator for the start of the array.

	STL compatible constant iterator for the start of the array.

	\return Constant iterator for starting a traversal of the array.
	\sa end(void) const or begin(void)

***************************************/

/*! ************************************

	\fn Burger::SimpleArray::const_iterator Burger::SimpleArray::cend(
		void) const
	\brief Constant iterator for the end of the array.

	STL compatible constant iterator for the end of the array.

	\return Constant iterator for ending a traversal of the array.
	\sa begin(void) const or end(void)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::SimpleArray::erase(const_iterator it)
	\brief Remove an entry from the array using an iterator as the index

	Using an iterator index, delete an entry in the array. Note, this will
	change the end() value in an index

	\param it Iterator index into an array
	\return Zero on success, or non zero on failure

	\sa remove_at(uintptr_t)

***************************************/
