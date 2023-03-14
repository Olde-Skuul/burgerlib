/***************************************

	Debug manager

	Android specific version

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
static uint_t g_uDebugger = 0;

void BURGER_API Burger::Debug::PrintString(const char* pString) BURGER_NOEXCEPT
{
	// Allow multiple threads to call me!

	if (pString) {
		uintptr_t i = StringLength(pString);
		if (i) {
			g_LockString.Lock();
			// Output the string to logcat
			__android_log_write(ANDROID_LOG_INFO, "burgerlib", pString);
			g_LockString.Unlock();
		}
	}
}

/***************************************

	\brief Detect if a debugger is attached

	Return \ref TRUE if a debugger is attached

	Docs for this version are found here
	https://man7.org/linux/man-pages/man5/proc.5.html

***************************************/

uint_t BURGER_API Burger::Debug::IsDebuggerPresent(void) BURGER_NOEXCEPT
{
	uint_t uResult = g_uDebugger;
	// Already tested?
	if (!(uResult & 0x80U)) {

		// Open the process status
		int ifpStatus = open("/proc/self/status", O_RDONLY);
		if (ifpStatus != -1) {

			// The status opened fine, read and close
			char TempBuffer[256];
			ssize_t uLength =
				read(ifpStatus, TempBuffer, sizeof(TempBuffer) - 1);
			close(ifpStatus);
			if (uLength != -1) {
				// Make sure it's a "C" string
				TempBuffer[uLength] = 0;

				// Sanity check for my magic number
				BURGER_STATIC_ASSERT(sizeof("TracerPid:\t") == (11 + 1));

				// Find the token.
				const char* pIndex = StringString(TempBuffer, "TracerPid:\t");
				if (pIndex) {
					// Found it? '0' means detached.
					uResult = pIndex[11] != '0';
				}
			}
		}
		// Save the debugger flag
		g_uDebugger = uResult | 0x80U;
	}
	// Return TRUE or FALSE
	return uResult & 1U;
}

#endif
