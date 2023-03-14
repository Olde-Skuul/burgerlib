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
	\brief Delete an object using Burger::Delete()

	A template to pass to \ref unique_ptr to delete the object with
	Burger::Delete.

	Most classes in Burgerlib use the New allocator to use the Burgerlib memory
	manager, as such they need to be released using the Burger::Delete
	function.

	\tparam T Type of object to delete with Burger::Delete()
	\sa unique_ptr or Burger::Delete()

***************************************/

/*! ************************************

	\struct Burger::Free_delete
	\brief Delete an object using Burger::Free()

	A template to pass to \ref unique_ptr to delete the object with
	Burger::Free().

	When memory is allocated with Burger::Alloc(), it should be released with
	Burger::Free(), this deleter handles the case.

	\tparam T Type of object to delete with Burger::Free()
	\sa unique_ptr or Burger::Free()

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
