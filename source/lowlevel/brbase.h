/***************************************

	Root base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRBASE_H__
#define __BRBASE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTATICRTTI_H__
#include "brstaticrtti.h"
#endif

/* BEGIN */
namespace Burger {
class Base {
	BURGER_RTTI_IN_BASE_CLASS();
public:
	virtual ~Base();
};
}
/* END */

#endif
