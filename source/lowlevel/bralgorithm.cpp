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

	\namespace Burger::type_traits
	\brief Semi-private template classes for type checking.

	For templates that requite checking of types, many helper structures are in
	this namespace to prevent pollution of the Burger namespace.

***************************************/

/*! ************************************

	\typedef Burger::type_traits::yes_type
	\brief Type used for templates to return 1.

	This type resolves to sizeof(char) to force sizeof( \ref
	type_traits::yes_type) != sizeof( \ref type_traits::no_type)

	\sa type_traits::no_type

***************************************/

/*! ************************************

	\struct Burger::type_traits::no_type
	\brief Type used for templates to return 0.

	This type resolves to (sizeof(char) * 8) to force sizeof( \ref
	type_traits::yes_type) != sizeof( \ref type_traits::no_type)

	\sa type_traits::yes_type

***************************************/

/*! ************************************

	\struct Burger::type_traits::size_type
	\brief Type used for templates the require a specific size.

	This type resolves to an empty struct that could be used to force a template
	to only instantiate with a specific data size. The struct itself is not
	meant to be used.

***************************************/

/*! ************************************

	\struct Burger::conditional
	\brief Select a type based if the conditional is true or false.

	The first parameter is a conditional boolean. This will be evaluated as true
	or false. The second parameter is the type used if the condition is true and
	the third parameter is the type used if the condition is false.

	\note This is functionally the same as std::conditional<>

	\tparam B Condition to test for ``true``.
	\tparam T Type of the result if condition is ``true``.
	\tparam F Type of the result if condition is ``false``.
	\sa enable_if

***************************************/

/*! ************************************

	\struct Burger::enable_if
	\brief Create typedef type if condition is true.

	The first parameter is a conditional boolean. This will be evaluated as true
	or false. The second parameter is the type of the resulting typedef.

	If the test is successful, typedef type exists. If not, the typedef does
	not exist, causing template instantiation failure.

	This is a duplicate of std::enable_if.

	\note: Not supported on Open WATCOM

	\tparam B Condition to test for ``true``.
	\tparam T Type of the result, void is the default.
	\sa disable_if

***************************************/

/*! ************************************

	\struct Burger::disable_if
	\brief Create typedef type if condition is false.

	The first parameter conditional boolean. This will be evaluated as true or
	false. The second parameter is the type of the resulting typedef.

	If the test failed, typedef type exists. If it succeeded, the typedef does
	not exist, causing template instantiation failure.

	\note: Not supported on Open WATCOM

	\tparam B Condition to test for ``false``.
	\tparam T Type of the result, void is the default.
	\sa enable_if

***************************************/

/*! ************************************

	\struct Burger::integral_constant
	\brief Wrap a static constant of specified type.

	A template to wrap a static constant of a specified type so templates can
	resolve the value at compile time. It's the base class for type traits.

	\tparam T Type of wrapped value
	\tparam _Value value to wrap.

	\sa bool_constant

***************************************/

/*! ************************************

	\fn Burger::integral_constant::operator T() const
	\brief Function to return the encapsulated value.

	\return The encapsulated value.

***************************************/

/*! ************************************

	\fn Burger::integral_constant::operator ()() const
	\brief Function to return the encapsulated value.

	\return The encapsulated value.

***************************************/

/*! ************************************

	\struct Burger::bool_constant
	\brief Wrap a static bool constant.

	A template to wrap a static bool constant so templates can resolve the value
	at compile time.

	This is an implementation from C++17.

	\tparam _Value value to wrap.

	\sa integral_constant

***************************************/

/*! ************************************

	\typedef Burger::true_type
	\brief Static bool constant of true.

	A bool_constant set to true.

	\sa bool_constant or false_type

***************************************/

/*! ************************************

	\typedef Burger::false_type
	\brief Static bool constant of false.

	A bool_constant set to false.

	\sa bool_constant or true_type

***************************************/

/*! ************************************

	\struct Burger::alignment_of
	\brief Determine the alignment of an object.

	A template to obtain the alignment value of an object.

	\tparam T Type of object to test

	Example of use:
	\code

	printf("Alignment of int is %u", Burger::alignment_of<int>::value);

	\endcode
	\sa integral_constant

***************************************/

/*! ************************************

	\struct Burger::ice_and
	\brief Test for all values being true.

	A helper template that sets its output to true or false based on all inputs
	being true. It's implemented as a compile time constant for use in other
	templates.

	Example of use:

	\code

	// iResult is false
	int iResult = Burger::ice_and<false, false>::value;

	// iResult is true
	iResult = Burger::ice_and<true, true>::value;

	// iResult is true
	iResult = Burger::ice_and<true, true, true>::value;

	// iResult is false
	iResult = Burger::ice_and<true, false, true>::value;
	\endcode

***************************************/

/*! ************************************

	\struct Burger::ice_or
	\brief Test for any value being true.

	A helper template that sets its output to true or false based on any input
	being true. It's implemented as a compile time constant for use in other
	templates.

	Example of use:

	\code

	// iResult is false
	int iResult = Burger::ice_or<false, false>::value;

	// iResult is true
	iResult = Burger::ice_or<true, true>::value;

	// iResult is true
	iResult = Burger::ice_or<true, true, true>::value;

	// iResult is true
	iResult = Burger::ice_or<true, false, true>::value;
	\endcode

***************************************/

/*! ************************************

	\struct Burger::ice_eq
	\brief Test for equality

	A helper template that sets its output to true if both inputs are the same
	integer value. It's implemented as a compile time constant for use in other
	templates.

	Example of use:

	\code

	// iResult is true
	int iResult = Burger::ice_eq<false, false>::value;

	// iResult is false
	iResult = Burger::ice_eq<true, false>::value;

	\endcode

***************************************/

/*! ************************************

	\struct Burger::ice_ne
	\brief Test for inequality

	A helper template that sets its output to true if both inputs are different
	integer values. It's implemented as a compile time constant for use in other
	templates.

	Example of use:

	\code

	// iResult is false
	int iResult = Burger::ice_ne<false, false>::value;

	// iResult is true
	iResult = Burger::ice_ne<true, false>::value;

	\endcode

***************************************/

/*! ************************************

	\struct Burger::ice_not
	\brief Reverse boolean input

	A helper template that sets its output to true if the input is false and
	vice versa. It's implemented as a compile time constant for use in other
	templates.

	Example of use:

	\code

	// iResult is true
	int iResult = Burger::ice_not<false>::value;

	// iResult is false
	iResult = Burger::ice_not<true>::value;

	\endcode

***************************************/

/*! ************************************

	\struct Burger::is_same
	\brief Determine if two objects are the same type.

	A template that sets its value to true if both classes are the same type.

	Based on
	[std::is_same](https://en.cppreference.com/w/cpp/types/is_same)

	\tparam T Type of first object to test
	\tparam U Type of second object to test

	Example of use:
	\code

	printf("int and float are not the same %u", Burger::is_same<int,
		float>::value);
	printf("int and int are the same %u", Burger::is_same<int, int>::value);

	\endcode
	\sa is_integral

***************************************/

/*! ************************************

	\struct Burger::remove_const
	\brief Remove the const qualifier from a type.

	A template that sets its type to be the declared type without the const
	keyword.

	Based on
	[std::remove_const](https://en.cppreference.com/w/cpp/types/remove_cv)

	\tparam T Type to remove the const keyword.

	\sa remove_const<const T>

***************************************/

/*! ************************************

	\struct Burger::remove_volatile
	\brief Remove the volatile qualifier from a type.

	A template that sets its type to be the declared type without the volatile
	keyword.

	Based on
	[std::remove_volatile](https://en.cppreference.com/w/cpp/types/remove_cv)

	\tparam T Type to remove the volatile keyword.

	\sa remove_volatile<const T>

***************************************/

/*! ************************************

	\struct Burger::remove_cv
	\brief Remove the volatile and const qualifier from a type.

	A template that sets its type to be the declared type without the volatile
	or const keywords.

	Based on
	[std::remove_cv](https://en.cppreference.com/w/cpp/types/remove_cv)

	\tparam T Type to remove the volatile or const keywords.

	\sa remove_volatile or remove_const

***************************************/

/*! ************************************

	\struct Burger::add_const
	\brief Add the const qualifier to a type.

	A template that sets its type to be the declared type with the const
	keyword.

	Based on
	[std::add_const](https://en.cppreference.com/w/cpp/types/add_cv)

	\tparam T Type to add the const keyword.

	\sa add_volatile or add_cv

***************************************/

/*! ************************************

	\struct Burger::add_volatile
	\brief Add the volatile qualifier to a type.

	A template that sets its type to be the declared type with the volatile
	keyword.

	Based on
	[std::add_volatile](https://en.cppreference.com/w/cpp/types/add_cv)

	\tparam T Type to add the volatile keyword.

	\sa add_const or add_cv

***************************************/

/*! ************************************

	\struct Burger::add_cv
	\brief Add the const and volatile qualifier to a type.

	A template that sets its type to be the declared type with the const and
	volatile keywords.

	Based on
	[std::add_cv](https://en.cppreference.com/w/cpp/types/add_cv)

	\tparam T Type to add the const and volatile keywords.

	\sa add_const or add_volatile

***************************************/

/*! ************************************

	\struct Burger::remove_reference
	\brief Remove the reference qualifier to a type.

	A template that sets its type to be the declared type without references.

	Based on
	[std::remove_reference](https://en.cppreference.com/w/cpp/types/remove_reference)

	\tparam T Type to remove reference.

	\sa remove_reference<T&> or remove_reference<T&&>

***************************************/

/*! ************************************

	\struct Burger::remove_reference<T&>
	\brief Remove the reference qualifier to a type.

	A template that sets its type to be the declared type without references.

	Based on
	[std::remove_reference](https://en.cppreference.com/w/cpp/types/remove_reference)

	\tparam T Type to remove reference.

	\sa remove_reference or remove_reference<T&&>

***************************************/

/*! ************************************

	\struct Burger::remove_reference<T&&>
	\brief Remove the reference qualifier to a type.

	A template that sets its type to be the declared type without references.

	Based on
	[std::remove_reference](https://en.cppreference.com/w/cpp/types/remove_reference)

	\tparam T Type to remove reference.

	\sa remove_reference or remove_reference<T&>

***************************************/

/*! ************************************

	\struct Burger::is_const
	\brief Test if a type is const.

	A template that checks a type if it has the keyword const.

	This instantiation derives from \ref false_type

	Based on
	[std::is_const](https://en.cppreference.com/w/cpp/types/is_const)

	\tparam T Type to check.

	\sa is_volatile or false_type

***************************************/

/*! ************************************

	\struct Burger::is_volatile
	\brief Test if a type is volatile.

	A template that checks a type if it has the keyword volatile.

	This instantiation derives from \ref false_type

	Based on
	[std::is_volatile](https://en.cppreference.com/w/cpp/types/is_volatile)

	\tparam T Type to check.

	\sa is_volatile<volatile T> or false_type

***************************************/

/*! ************************************

	\struct Burger::is_void
	\brief Test if a type is void.

	A template that checks a type if it is void.

	This instantiation derives from \ref true_type

	Based on
	[std::is_void](https://en.cppreference.com/w/cpp/types/is_void)

	\tparam T Type to check.

	\sa is_integral or true_type

***************************************/

/*! ************************************

	\struct Burger::is_floating_point
	\brief Test if a type is a float.

	A template that checks a type if it is a floating point integral.

	\tparam T Type to check.

	Based on
	[std::is_floating_point](https://en.cppreference.com/w/cpp/types/is_floating_point)

	\sa is_integral or is_arithmetic

***************************************/

/*! ************************************

	\struct Burger::is_integral
	\brief Test if a type is an integer.

	A template that checks a type if it is an integer integral.

	\tparam T Type to check.

	Based on
	[std::is_integral](https://en.cppreference.com/w/cpp/types/is_integral)

	\sa is_arithmetic or is_floating_point

***************************************/

/*! ************************************

	\struct Burger::is_arithmetic
	\brief Test if a type is an integer or a floating point number.

	A template that checks a type if it is an integer or floating point
	integral.

	\tparam T Type to check.

	Based on
	[std::is_arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic)

	\sa is_floating_point or is_integral

***************************************/

/*! ************************************

	\struct Burger::is_pointer
	\brief Test if a type is a pointer.

	A template that checks a type if it is a pointer.

	\tparam T Type to check.

	Based on
	[std::is_pointer](https://en.cppreference.com/w/cpp/types/is_pointer)

	\sa is_function

***************************************/

/*! ************************************

	\fn Burger::move(T&& t)
	\brief Upconvert to && for move semantics.

	A template that converts the type to a && type. Used to force move
	semantics. It's used to allow for C++11 or higher compilers to generate
	better code by moving the contents of a class or structure instead of making
	a copy.

	\note On compilers that don't support move semantics, it will return a
		reference and use the older copy semantics.

	\tparam T Type to convert to &&.
	\param t Object reference to convert to &&

	\return Object reference converted to &&, or & on old compilers.

	Based on
	[std::move](https://en.cppreference.com/w/cpp/utility/move)

	\sa is_function

***************************************/

/*! ************************************

	\struct Burger::is_function
	\brief Simplified implementation of std::is_function

	A template that checks a type if it is a function pointer.

	This class is based on
	[std::is_function](https://en.cppreference.com/w/cpp/types/is_function)

	\tparam T Type to check.

	\sa false_type or true_type

***************************************/

/*! ************************************

	\fn Burger::round_up_pointer(T*,uintptr_t uSize)
	\brief Align a pointer.

	A template to force the alignment value of a pointer. The template will use
	a specified alignment or auto detect the alignment of the data referenced by
	the pointer.

	\tparam T Pointer of object to align.
	\param pInput Pointer to force alignment
	\param uSize Power of 2 alignment to use.

	\return Newly aligned pointer.

	Example of use:
	\code

	// Pointer is already aligned
	char *pWork;
	pWork = reinterpret_cast<char *>(0);
	pWork = Burger::round_up_pointer(pWork,8);
	pWork == reinterpret_cast<char *>(0);

	// Force 8 byte alignment
	pWork = reinterpret_cast<char *>(1);
	pWork = Burger::round_up_pointer(pWork,8);
	pWork == reinterpret_cast<char *>(8);

	// Force alignment of data automatically by using the alignment of the data
	//of the pointer.
	pWork = reinterpret_cast<uint32_t *>(1);
	pWork = Burger::round_up_pointer(pWork);
	pWork == reinterpret_cast<char *>(4);

	\endcode
	\sa alignment_of

***************************************/

/*! ************************************

	\struct Burger::default_delete
	\brief Delete an object using delete

	A template to pass to \ref unique_ptr to delete the object with std::delete.

	\tparam T Type of object to delete
	\sa unique_ptr

***************************************/

/*! ************************************

	\struct Burger::default_delete_array
	\brief Delete an object using delete[]

	A template to pass to \ref unique_ptr to delete the object with
	std::delete[].

	\note This is explicitly used to maintain compatibility with compilers that
	don't support SFINAE.

	\tparam T Type of object to delete with std::delete[]
	\sa unique_ptr

***************************************/

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

/*! ************************************

	\fn T Burger::Min(T A,T B)
	\brief Return the lesser of two objects

	A template to compare the two input values and return the lesser of the two.

	\tparam T Type of the two values
	\param A First value to test
	\param B Second value to test
	\return The lesser of the two inputs

	\sa Max(T,T)

***************************************/

/*! ************************************

	\fn T Burger::Max(T A,T B)
	\brief Return the greater of two objects

	A template to compare the two input values and return the greater of the
	two.

	\tparam T Type of the two values
	\param A First value to test
	\param B Second value to test
	\return The greater of the two inputs

	\sa Min(T,T)

***************************************/
