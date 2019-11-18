/***************************************

	Flash player action buffer

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brflashactionvalue.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Flash::ActionScriptProperty,Burger::WeakAndStrongBase);
#endif

/*! ************************************

	\class Burger::Flash::ActionScriptProperty
	\brief Shared data buffer

	A shared buffer that's reference counted so multiple
	action script items can manager a single copy
	of this buffer

	\sa ReferenceCounter

***************************************/

#if defined(BURGER_VITA) || defined(BURGER_LINUX)

namespace Burger { namespace Flash {
class ActionScriptFunction : public ReferenceCounter {
};
}}
Burger::Flash::ActionScriptProperty::~ActionScriptProperty()
{
}
#endif