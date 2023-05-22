/***************************************

	Debug manager

	Android specific version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_ANDROID)
#include "brcriticalsection.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

#include <android/log.h>
#include <fcntl.h>
#include <unistd.h>

// Make it thread safe

static Burger::CriticalSectionStatic g_LockString;

void BURGER_API Burger::Debug::PrintString(const char* pString) BURGER_NOEXCEPT
{
	// Allow multiple threads to call me!

	if (pString) {
		uintptr_t i = StringLength(pString);
		if (i) {
			g_LockString.lock();
			// Output the string to logcat
			__android_log_write(ANDROID_LOG_INFO, "burgerlib", pString);
			g_LockString.unlock();
		}
	}
}

#endif
