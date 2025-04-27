/***************************************

	Run Time Type Information class

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brstaticrtti.h"

/*! ************************************

	\struct Burger::StaticRTTI
	\brief Structure to handle simple Run Time Type Identification

	This implements a method for handling run time type identification without
	any runtime overhead and a tiny amount of memory space in the form of a
	single extra pointer in the VTable and a single instance of piece of code
	returning a pointer.

	The class itself does not contain any additional data to support this
	feature!!!

	By using macros for calls, data tables and class hooks, a simple way to
	check if a base class is really a specific class is created.

	\code

	// **** In the header ****

	// Class definitions using a virtual table (A VTable must exist in the base
	// for this system to work

	// Base class
	class foo {
	public:
		foo() {}
		virtual ~foo() {}
		BURGER_RTTI_IN_BASE_CLASS();	// Insert magic code
	};

	// Derived classes

	class foo2 : public foo {
	public:
		foo2() {}
		virtual ~foo2() {}
		BURGER_RTTI_IN_CLASS();		// Insert magic code
	};
	class foo3 : public foo2 {
	public:
		foo3() {}
		virtual ~foo3() {}
		BURGER_RTTI_IN_CLASS();		// Insert magic code
	};
	class foo4 : public foo2 {
	public:
		foo4() {}
		virtual ~foo4() {}
		BURGER_RTTI_IN_CLASS();		// Insert magic code
	};
	class foo5 : public foo {
	public:
		foo5() {}
		virtual ~foo5() {}
		BURGER_RTTI_IN_CLASS();		// Insert magic code
	};

	// ***** Declared in a single CPP file

	// Declare the base class
	BURGER_CREATE_STATICRTTI_BASE(foo);
	// Declare the derived classes and their immediate parents
	BURGER_CREATE_STATICRTTI_PARENT(foo2,foo);
	BURGER_CREATE_STATICRTTI_PARENT(foo3,foo2);
	BURGER_CREATE_STATICRTTI_PARENT(foo4,foo2);
	BURGER_CREATE_STATICRTTI_PARENT(foo5,foo);

	// Example code for use

	foo bar;
	foo2 bar2;
	foo3 bar3;
	foo4 bar4;
	foo5 bar5;
	foo *pBar = &bar;
	foo *pBar2 = &bar2;
	foo *pBar3 = &bar3;
	foo *pBar4 = &bar4;
	foo *pBar5 = &bar5;

	// Return a string with the true class name

	Debug::Message(pBar->get_class_name());		// "foo"
	Debug::Message(pBar2->get_class_name());	// "foo2"
	Debug::Message(pBar3->get_class_name());	// "foo3"
	Debug::Message(pBar4->get_class_name());	// "foo4"
	Debug::Message(pBar5->get_class_name());	// "foo5"

	// Demonstrate a truth table for class interactions

	// TRUE
	Debug::Message("foo pBar %u\n",BURGER_STATICRTTI_ISTYPE(foo,pBar));
	// FALSE
	Debug::Message("foo2 pBar %u\n",BURGER_STATICRTTI_ISTYPE(foo2,pBar));
	// FALSE
	Debug::Message("foo3 pBar %u\n",BURGER_STATICRTTI_ISTYPE(foo3,pBar));
	// FALSE
	Debug::Message("foo4 pBar %u\n",BURGER_STATICRTTI_ISTYPE(foo4,pBar));
	// FALSE
	Debug::Message("foo5 pBar %u\n\n",BURGER_STATICRTTI_ISTYPE(foo5,pBar));

	// TRUE
	Debug::Message("foo pBar2 %u\n",BURGER_STATICRTTI_ISTYPE(foo,pBar2));
	// TRUE
	Debug::Message("foo2 pBar2 %u\n",BURGER_STATICRTTI_ISTYPE(foo2,pBar2));
	// FALSE
	Debug::Message("foo3 pBar2 %u\n",BURGER_STATICRTTI_ISTYPE(foo3,pBar2));
	// FALSE
	Debug::Message("foo4 pBar2 %u\n",BURGER_STATICRTTI_ISTYPE(foo4,pBar2));
	// FALSE
	Debug::Message("foo5 pBar2 %u\n\n",BURGER_STATICRTTI_ISTYPE(foo5,pBar2));

	// TRUE
	Debug::Message("foo pBar3 %u\n",BURGER_STATICRTTI_ISTYPE(foo,pBar3));
	// TRUE
	Debug::Message("foo2 pBar3 %u\n",BURGER_STATICRTTI_ISTYPE(foo2,pBar3));
	// TRUE
	Debug::Message("foo3 pBar3 %u\n",BURGER_STATICRTTI_ISTYPE(foo3,pBar3));
	// FALSE
	Debug::Message("foo4 pBar3 %u\n",BURGER_STATICRTTI_ISTYPE(foo4,pBar3));
	// FALSE
	Debug::Message("foo5 pBar3 %u\n\n",BURGER_STATICRTTI_ISTYPE(foo5,pBar3));

	// TRUE
	Debug::Message("foo pBar4 %u\n",BURGER_STATICRTTI_ISTYPE(foo,pBar4));
	// TRUE
	Debug::Message("foo2 pBar4 %u\n",BURGER_STATICRTTI_ISTYPE(foo2,pBar4));
	// FALSE
	Debug::Message("foo3 pBar4 %u\n",BURGER_STATICRTTI_ISTYPE(foo3,pBar4));
	// TRUE
	Debug::Message("foo4 pBar4 %u\n",BURGER_STATICRTTI_ISTYPE(foo4,pBar4));
	// FALSE
	Debug::Message("foo5 pBar4 %u\n\n",BURGER_STATICRTTI_ISTYPE(foo5,pBar4));

	// TRUE
	Debug::Message("foo pBar5 %u\n",BURGER_STATICRTTI_ISTYPE(foo,pBar5));
	// FALSE
	Debug::Message("foo2 pBar5 %u\n",BURGER_STATICRTTI_ISTYPE(foo2,pBar5));
	// FALSE
	Debug::Message("foo3 pBar5 %u\n",BURGER_STATICRTTI_ISTYPE(foo3,pBar5));
	// FALSE
	Debug::Message("foo4 pBar5 %u\n",BURGER_STATICRTTI_ISTYPE(foo4,pBar5));
	// TRUE
	Debug::Message("foo5 pBar5 %u\n\n",BURGER_STATICRTTI_ISTYPE(foo5,pBar5));

	// Will properly cast a foo to a foo4
	foo5 *pTemp = BURGER_RTTICAST(foo5,pBar5);
	const foo5 *pCTemp = BURGER_RTTICONSTCAST(foo5,pBar5);

	// Will return nullptr since these classes are not a foo5
	pTemp = BURGER_RTTICAST(foo5,pBar4);
	pCTemp = BURGER_RTTICONSTCAST(foo5,pBar3);

	\endcode

	\note This system was made for classes that use single inheritance. It will
		only backtrack one class tree and cannot track multiple inheritances.

	\sa BURGER_RTTICAST, BURGER_RTTICONSTCAST, BURGER_STATICRTTI_ISTYPE,
		BURGER_RTTI_IN_CLASS, BURGER_CREATE_STATICRTTI_BASE or
		BURGER_CREATE_STATICRTTI_PARENT

***************************************/

/*! ************************************

	\fn const char* Burger::StaticRTTI::get_class_name(void) const
	\brief Return the class name

	Return the name of the class, not the base class.

	\return Pointer to a "C" string with the name of the true class

***************************************/

/*! ************************************

	\brief Determine if a class is of a specific type

	Walk the StaticRTTI linked list from the most derived type and up to the
	base class, all the while checking for a match.

	\param pInput Pointer to a StaticRTTI record to compare to this one

	\return \ref TRUE if this record is in the linked list chain

***************************************/

uint_t BURGER_API Burger::StaticRTTI::is_in_list(
	const StaticRTTI* pInput) const BURGER_NOEXCEPT
{
	// Since there should be only one static instance of the type ID, it should
	// be safe to compare the ID string pointers
	uint_t bRetVal = (pInput->m_pClassName == m_pClassName);
	if (!bRetVal && m_pParent) {
		bRetVal = m_pParent->is_in_list(pInput);
	}
	return bRetVal;
}

/*! ************************************

	\def BURGER_RTTI_IN_BASE_CLASS()
	\brief Macro for Burger::StaticRTTI support in a base class

	If a class needs to support Burger::StaticRTTI, insert this macro in the
	class declaration. Unlike the macro BURGER_RTTI_IN_CLASS(), this one adds
	the inline function Burger::StaticRTTI::get_class_name() to retrieve the
	name of the class easily.

	\note This macro will set the class setting to "public"

	\sa Burger::StaticRTTI, BURGER_RTTI_IN_CLASS, BURGER_CREATE_STATICRTTI_BASE
		or BURGER_CREATE_STATICRTTI_PARENT

***************************************/

/*! ************************************

	\def BURGER_RTTI_IN_CLASS()
	\brief Macro for Burger::StaticRTTI support in a class

	If a class needs to support Burger::StaticRTTI, insert this macro in the
	class declaration.

	\note This macro will set the class setting to "public"

	\sa Burger::StaticRTTI, BURGER_RTTI_IN_BASE_CLASS,
		BURGER_CREATE_STATICRTTI_BASE or BURGER_CREATE_STATICRTTI_PARENT

***************************************/

/*! ************************************

	\def BURGER_CREATE_STATICRTTI_BASE(__ClassName)
	\brief Define to create a Burger::StaticRTTI for a base class.

	For a class that's not derived from anything, use this macro to generate a
	terminating linked list entry

	\param __ClassName Class name (No quotes)

	\sa Burger::StaticRTTI, BURGER_RTTI_IN_CLASS or
		BURGER_CREATE_STATICRTTI_PARENT

***************************************/

/*! ************************************

	\def BURGER_CREATE_STATICRTTI_PARENT(__ClassName,__ParentClass)
	\brief Define to create a Burger::StaticRTTI for a derived class.

	For a class that's derived, use this macro to generate  a linked list entry
	that connects to its parent

	\param __ClassName Class name (No quotes)
	\param __ParentClass Parent class name (No quotes)
	\sa Burger::StaticRTTI, BURGER_RTTI_IN_CLASS or
		BURGER_CREATE_STATICRTTI_BASE

***************************************/

/*! ************************************

	\def BURGER_STATICRTTI_ISTYPE(__ClassName,__Pointer)
	\brief Return \ref TRUE if the base class is also the requested class

	This macro will use the magic data in the class to find if its really the
	requested class.

	\param __ClassName Class name (No quotes)
	\param __Pointer Pointer to the base class to test
	\sa Burger::StaticRTTI, BURGER_RTTICAST or BURGER_RTTICONSTCAST

***************************************/

/*! ************************************

	\def BURGER_RTTICAST(__ClassName,__Pointer)
	\brief Cast up a base class with verification

	Given a derived class, check if the pointer is part of the derived class and
	return a properly cast pointer. If the test fails, return nullptr.

	\param __ClassName Class name (No quotes)
	\param __Pointer Pointer to the base class to test
	\sa Burger::StaticRTTI, BURGER_STATICRTTI_ISTYPE or BURGER_RTTICONSTCAST

***************************************/

/*! ************************************

	\def BURGER_RTTICONSTCAST(__ClassName,__Pointer)
	\brief Cast up a const base class with verification

	Given a derived class, check if the pointer is part of the derived class and
	return a properly cast const pointer. If the test fails, return nullptr.

	\param __ClassName Class name (No quotes)
	\param __Pointer Pointer to the base class to test
	\sa Burger::StaticRTTI, BURGER_STATICRTTI_ISTYPE or BURGER_RTTICAST

***************************************/
