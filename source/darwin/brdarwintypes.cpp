/***************************************

	Typedefs specific to Darwin (iOS/macOS)

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdarwintypes.h"
#include <Foundation/NSAutoreleasePool.h>

/*! ************************************

	\def BURGER_SCOPED_AUTORELEASE

	\brief Create a scoped autorelease memory pool.

	\note Only available on Darwin platforms when compiling using Objective-C or
		Objective-C++. If not, this macro will reduce to no operation.

	\sa Burger::AutoreleasePool

***************************************/

/*! ************************************

	\class Burger::AutoreleasePool

	\brief Create an instance of a NSAutoreleasePool

	For code written for Darwin based platforms, it may be needed to create an
	autoreleasepool. While modern compilers have a keyword of \@autoreleasepool,
	older compilers do not. This class handles the case for both older and
	modern compilers for creating and releasing an autoreleasepool.

	\code

	void foo() {
		const AutoreleasePool Temp;

		// Do code that creates pool memory
		DoCocoaCode();
		// On exit of the function, release the memory
	}
	\endcode

	\note Only available on Darwin platforms when compiling using Objective-C or
		Objective-C++. If not, this class does not exist. Use the macro below
		for all cases.

	\sa BURGER_SCOPED_AUTORELEASE

***************************************/

#if defined(__OBJC__) || defined(DOXYGEN)

/*! ************************************

	\brief Create an autoreleasepool

	Calls [[NSAutoreleasePool alloc] init] and stores the pointer.

	\sa BURGER_SCOPED_AUTORELEASE

***************************************/

Burger::AutoreleasePool::AutoreleasePool()
{
	m_pAutoPool = [[NSAutoreleasePool alloc] init];
}

/*! ************************************

	\brief Release an autoreleasepool

	Calls [m_pAutoPool release] to release the pool

	\sa BURGER_SCOPED_AUTORELEASE

***************************************/

Burger::AutoreleasePool::~AutoreleasePool()
{
	[m_pAutoPool release];
}

#endif
