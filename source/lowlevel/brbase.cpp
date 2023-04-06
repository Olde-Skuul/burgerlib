/***************************************

	Root base class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brbase.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::Base
	\brief Base class for virtual destructor.

	Burgerlib has numerous pointers to unknown classes that are up cast from
	\ref Base if they are actually used. By only knowing a generic, empty
	base class, no code bloat is needed to manage destructor/constructor chains
	because of a class reference to something that uses OpenGL, DirectX or any
	other high overhead functionality. The only code linked in is the generic
	destructor.

***************************************/

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_BASE(Burger::Base);
#endif

/*! ************************************

	\fn Burger::Base::~Base()
	\brief Destructor.

	Does absolutely nothing

***************************************/

/*! ************************************

	\brief Delete a base class.

	If the pointer passed is not nullptr, call the destructor and then dispose
	of the pointer with a call to \ref Free(const void *)

	\param pInput Pointer to a base class

***************************************/

void BURGER_API Burger::Delete(const Base* pInput) BURGER_NOEXCEPT
{
	// Valid pointer?
	if (pInput) {

		// Destroy the class
		const_cast<Base*>(pInput)->~Base();

		// Dispose of the memory
		Free(pInput);
	}
}

/*! ************************************

	\fn const Burger::StaticRTTI *Burger::Base::get_StaticRTTI(void) const
	\brief Get the description to the class

	This virtual function will pull the pointer to the StaticRTTI instance that
	has the name of the class. Due to it being virtual, it will be the name of
	the most derived class.

	\return Pointer to a global, read only instance of StaticRTTI for the true
		class

***************************************/

/*! ************************************

	\fn const char *Burger::Base::get_class_name(void) const
	\brief Get the name of the class

	This inline function will pull the virtually declared pointer to the
	StaticRTTI instance that has the name of the class. Due to it being virtual,
	it will be the name of the most derived class.

	\return Pointer to a global, read only "C" string with the true name of the
		class

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Base::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a reference to the parent
	(If any)

***************************************/
