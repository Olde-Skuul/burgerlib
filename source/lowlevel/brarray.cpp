/***************************************

	vector<T> compatible array template

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brarray.h"

/*! ************************************

	\class Burger::ClassArray
	\brief Template for creating arrays of class objects

	If a case where an array of a class type is only
	known at run time, this template class will
	dynamically allocate memory and instantiate
	class instances with proper shutdown and
	initialization.

	While it uses function names that match the
	class vector<T>, this is a lightweight version
	which doesn't match 100% due to implementation
	for performance.

	\sa SimpleArray

***************************************/

/*! ************************************

	\fn Burger::ClassArray::ClassArray()
	\brief Default constructor.

	Initializes the array to contain no data and have no members.

	\sa ClassArray(WordPtr) or ClassArray(const ClassArray& rData)

***************************************/

/*! ************************************

	\fn Burger::ClassArray::ClassArray()
	\brief Default constructor.

	Initializes the array to contain no data and have no members.

	\sa ClassArray(WordPtr) or ClassArray(const ClassArray&)

***************************************/

/*! ************************************

	\fn Burger::ClassArray::ClassArray(WordPtr uDefault)
	\brief Default constructor with a starting array

	Initializes the array to contain uDefault number of members
	that have been initialized with the default type constructor

	\param uDefault Number of members to initialize the class with. Zero
		will generate an empty array.

	\sa ClassArray() or ClassArray(const ClassArray&)

***************************************/

/*! ************************************

	\fn Burger::ClassArray::ClassArray(const ClassArray& rData)
	\brief Default constructor for making a copy of another ClassArray

	Initializes the array to contain a copy of
	another ClassArray. Copy constructors will be iterated on 
	each and every object.

	\param rData Reference to a matching ClassArray type

	\sa ClassArray() or ClassArray(WordPtr)

***************************************/

/*! ************************************

	\fn Burger::ClassArray::~ClassArray()
	\brief Standard destructor

	Call clear() to iterate over every valid object in the array
	with the type T destructor and then release the memory.

	\sa ClassArray(), ClassArray(WordPtr) or ClassArray(const ClassArray&)

***************************************/

/*! ************************************

	\fn T * Burger::ClassArray::GetPtr(void)
	\brief Obtain a pointer to the array.

	Allow direct access to the base pointer to the array. This
	can be \ref NULL if the array is empty.

	\return A pointer to the array.
	\sa GetPtr(void) const or front(void)

***************************************/

/*! ************************************

	\fn const T * Burger::ClassArray::GetPtr(void) const
	\brief Obtain a constant pointer to the array.

	Allow direct access to the base pointer to the array. This
	can be \ref NULL if the array is empty.

	\return A constant pointer to the array.
	\sa GetPtr(void) or front(void) const

***************************************/

/*! ************************************

	\fn T & Burger::ClassArray::operator[](WordPtr uIndex)
	\brief Obtain a reference to an item in the array.

	Index into the array and return a reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT()
	if the uIndex value exceeds the size of the number of valid
	entries in the array.

	\param uIndex Object number in the array to retrieve a reference to.
	\return A reference of the object indexed.
	\sa operator[](WordPtr) const

***************************************/

/*! ************************************

	\fn const T & Burger::ClassArray::operator[](WordPtr uIndex) const
	\brief Obtain a constant reference to an item in the array.

	Index into the array and return a constant reference to the object.

	\note In \ref _DEBUG builds, this will \ref BURGER_ASSERT()
	if the uIndex value exceeds the size of the number of valid
	entries in the array.

	\param uIndex Object number in the array to retrieve a constant reference to.
	\return A constant reference of the object indexed.
	\sa operator[](WordPtr)

***************************************/

/*! ************************************

	\fn WordPtr Burger::ClassArray::max_size(void) const
	\brief Return the number of objects the current buffer could hold.
	 
	 The buffer size may exceed the number of valid objects, so that if
	 the array grew, memory allocations won't be needed. This
	 function will return the size of the true buffer.

	\return Number of objects the currently allocated buffer can hold.
	\sa size(void) const

***************************************/

/*! ************************************

	\fn WordPtr Burger::ClassArray::size(void) const
	\brief Return the valid number of objects in the array.
	
	This value is less than or equal to the buffer size.

	\return Number of valid objects in the array.
	\sa max_size(void) const or empty(void) const

***************************************/

/*! ************************************

	\fn Word Burger::ClassArray::empty(void) const
	\brief Return \ref TRUE if the array is empty.

	If there is no valid data in the array, return \ref TRUE.

	\return \ref TRUE if the valid object count is zero, \ref FALSE if there are valid objects
	\sa size(void) const

***************************************/

/*! ************************************

	\fn T & Burger::ClassArray::front(void)
	\brief Obtain a reference to first item in the array.

	Return a reference to the first object.

	\return A reference to the first object in the array
	\sa back(void) or front(void) const

***************************************/

/*! ************************************

	\fn const T & Burger::ClassArray::front(void) const
	\brief Obtain a constant reference to first item in the array.

	Return a constant reference to the first object.

	\return A constant reference to the first object in the array
	\sa back(void) const or front(void)

***************************************/

/*! ************************************

	\fn T & Burger::ClassArray::back(void)
	\brief Obtain a reference to last item in the array.

	Return a reference to the last object.

	\return A reference to the last object in the array
	\sa front(void) or back(void) const

***************************************/

/*! ************************************

	\fn const T & Burger::ClassArray::back(void) const
	\brief Obtain a constant reference to last item in the array.

	Return a constant reference to the last object.

	\return A constant reference to the last object in the array
	\sa front(void) const or back(void)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::push_back(const T &rData)
	\brief Append an object to the end of the array.

	Make a copy of the object at the end of the array. If there
	is no room for the new object, increase the size of
	the buffer to make room. Buffer size increases are made
	in groups to reduce memory allocation calls to
	improve performance.

	\param rData A constant reference to an object to copy at the end of the array
	\sa pop_back(void), insert(WordPtr,const T&), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::pop_back(void)
	\brief Remove an object from the end of the array.

	Call the destructor on the last object in the array and
	reduce the array size by one.

	\sa push_back(const T&), remove(WordPtr), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::clear(void)
	\brief Remove all objects from the array.

	Call the destructor on every object in the array and
	then release the array memory.

	\sa resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn ClassArray & Burger::ClassArray::operator=(const ClassArray &rData)
	\brief Copy an array into this one

	If the copy is not itself, call clear() to erase the
	contents of this class and make a duplicate
	of every entry in the rData class into this one.

	\param rData Reference to a matching ClassArray type
	\return *this
	\sa resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::remove(WordPtr uIndex)
	\brief Remove an object from the array.

	Call the destructor on the specific object in the array and
	then compact the array if needed.

	\param uIndex Index into the array of the object to remove.
	\sa insert(WordPtr,const T&), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::insert(WordPtr uIndex,const T& rData)
	\brief Insert an object into the array.

	Expand the buffer if needed and make a copy of the rData
	object into the array at the index uIndex.

	\param uIndex Index into the array for the location of the object to insert.
	\param rData Reference to the object to copy into the array
	\sa remove(WordPtr), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::append(const T *pSourceData,WordPtr uCount)
	\brief Append an array of object to this array.

	Given a base pointer and an object count, iterate over the objects
	and copy them to the end of this array. This function
	will increase the size of the buffer if needed.

	\param pSourceData Pointer to the first element in an array of objects
	\param uCount Number of elements in the array
	\sa append(const ClassArray&), insert(WordPtr,const T&), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::append(const ClassArray& rData)
	\brief Append an array to this array.

	Given another array, iterate over the objects contained within
	and copy them to the end of this array. This function
	will increase the size of the buffer if needed.

	\param rData Reference to a like typed ClassArray to copy from.
	\sa append(const T *,WordPtr), resize(WordPtr) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::resize(WordPtr uNewSize)
	\brief Resize the valid entry count of the array.

	If uNewSize is zero, erase all data. If uNewSize increases
	the size of the array, increase the buffer size if necessary
	and invoke the default constructor on the new objects. If
	the size is smaller than the existing array, call the destructors
	on the removed objects.

	In some cases, the buffer size will be reduced if the new size
	is substantially smaller.

	\param uNewSize Number of valid objects the new array will contain.
	\sa clear(void) or reserve(WordPtr)

***************************************/

/*! ************************************

	\fn void Burger::ClassArray::reserve(WordPtr uNewBufferSize)
	\brief Resize the memory used by the array.

	This function sets the size of the master buffer which can exceed
	the number of valid entries in the array. This is a performance
	function in that if it's known at runtime what is the maximum
	memory requirements for this array, it can be pre-allocated
	and all functions can use this buffer until the class
	is disposed of without any intermediate memory allocation
	calls.

	If the reservation size is zero, the array is released.

	If the reservation forces the array to shrink, all of the
	truncated objects will be properly destructed. The array size
	will be adjusted to the match the buffer size.

	\param uNewBufferSize Size in elements of the memory buffer.
	\sa clear(void) or resize(WordPtr)

***************************************/

/*! ************************************

	\typedef Burger::ClassArray::iterator
	\brief STL compatible iterator

	Iterator object for using functions in &lt;algorithm&gt;

	\sa const_iterator, value_type, begin(void) or end(void)

***************************************/

/*! ************************************

	\typedef Burger::ClassArray::const_iterator
	\brief STL compatible iterator

	Constant iterator object for using functions in &lt;algorithm&gt;

	\sa iterator, value_type, begin(void) const or end(void) const

***************************************/

/*! ************************************

	\typedef Burger::ClassArray::value_type
	\brief STL compatible type declaration

	Type declarator to use in functions in &lt;algorithm&gt;. It's
	used to create variables of T by using this typedef
	in the class.

	\sa iterator or const_iterator

***************************************/

/*! ************************************

	\fn Burger::ClassArray::iterator Burger::ClassArray::begin(void)
	\brief Iterator for the start of the array.

	STL compatible iterator for the start of the array.

	\return Iterator for starting a traversal of the array.
	\sa end(void) or begin(void) const

***************************************/

/*! ************************************

	\fn Burger::ClassArray::iterator Burger::ClassArray::end(void)
	\brief Iterator for the end of the array.

	STL compatible iterator for the end of the array.

	\return Iterator for ending a traversal of the array.
	\sa begin(void) or end(void) const

***************************************/


/*! ************************************

	\fn Burger::ClassArray::const_iterator Burger::ClassArray::begin(void) const
	\brief Constant iterator for the start of the array.

	STL compatible constant iterator for the start of the array.

	\return Constant iterator for starting a traversal of the array.
	\sa end(void) const or begin(void)

***************************************/

/*! ************************************

	\fn Burger::ClassArray::const_iterator Burger::ClassArray::end(void) const
	\brief Constant iterator for the end of the array.

	STL compatible constant iterator for the end of the array.

	\return Constant iterator for ending a traversal of the array.
	\sa begin(void) const or end(void)

***************************************/