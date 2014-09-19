/***************************************

	iOS application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIOSAPP_H__
#define __BRIOSAPP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

/* BEGIN */
#if defined(BURGER_IOS) || defined(DOXYGEN)
namespace Burger {
class iOSApp : public GameApp {
	BURGER_DISABLECOPYCONSTRUCTORS(iOSApp);
public:
	iOSApp(const char *pGameName,WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	~iOSApp();
};
}
#endif
/* END */

#endif
