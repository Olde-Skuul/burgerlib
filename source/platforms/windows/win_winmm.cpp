/***************************************

	Shims for winmm.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_winmm.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

#include <MMSystem.h>

// Function prototypes, lovingly stolen from mmsystem.h
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

uint32_t BURGER_API Burger::Win32::timeGetTime(void) BURGER_NOEXCEPT
{
	void* ptimeGetTime = load_function(kCall_timeGetTime);
	uint32_t uResult = 0;
	if (ptimeGetTime) {
		uResult = static_cast<timeGetTimePtr>(ptimeGetTime)();
	}
	return uResult;
}

#endif
