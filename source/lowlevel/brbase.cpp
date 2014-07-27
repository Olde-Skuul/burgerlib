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

/*! ************************************

	\brief Destructor.
	
	Does absolutely nothing
	
***************************************/

Burger::Base::~Base()
{
}
