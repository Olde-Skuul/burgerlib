/***************************************

	Templates to support "algorithm"

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "bralgorithm.h"

/*! ************************************

	\struct Burger::Base_delete
	\brief Delete an object using Burger::delete_object()

	A template to pass to \ref Burger::unique_ptr to delete the object with
	Burger::delete_object.

	Most classes in Burgerlib use the New allocator to use the Burgerlib memory
	manager, as such they need to be released using the Burger::delete_object
	function.

	\tparam T Type of object to delete with Burger::delete_object()
	\sa Burger::unique_ptr or Burger::delete_object()

***************************************/

/*! ************************************

	\struct Burger::Free_delete
	\brief Delete an object using Burger::free_memory()

	A template to pass to \ref Burger::unique_ptr to delete the object with
	Burger::free_memory().

	When memory is allocated with Burger::allocate_memory(), it should be released with
	Burger::free_memory(), this deleter handles the case.

	\tparam T Type of object to delete with Burger::free_memory()
	\sa unique_ptr or Burger::free_memory()

***************************************/

/*! ************************************

	\struct Burger::unique_ptr
	\brief Simplified implementation of std::unique_ptr

	Burger::unique_ptr is a smart pointer that owns and manages a pointer to an
	object and disposes of the object when this class goes out of scope.

	This class is based on
	[std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)

	\note This class can be moved, but not copied.

	\tparam T Type of object to maintain a pointer to.
	\tparam Deleter Type of object to delete the object.
	\sa default_delete, Free_delete or Base_delete

***************************************/
