/***************************************

	Shims for winmm.dll

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <MMSystem.h>

typedef DWORD(WINAPI* timeGetTimePtr)(void);

// Unit tests for pointers
// timeGetTimePtr gtimeGetTime = ::timeGetTime;

#endif

//
// winmm.dll
//

/*! ************************************

	\brief Load in winmm.dll and call timeGetTime

	Manually load winmm.dll if needed and call the Windows function
	timeGetTime()

	http://msdn.microsoft.com/en-us/library/dd757629(v=vs.85).aspx

	\return Time in milliseconds

***************************************/

Word32 BURGER_API Burger::Windows::timeGetTime(void)
{
	void* ptimeGetTime = LoadFunctionIndex(CALL_timeGetTime);
	Word uResult = 0;
	if (ptimeGetTime) {
		uResult = static_cast<timeGetTimePtr>(ptimeGetTime)();
	}
	return uResult;
}


#endif
