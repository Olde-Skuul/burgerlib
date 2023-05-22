/***************************************

	Debug manager

	Posix/Linux specific version

	Copyright (c) 2021-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include "brstringfunctions.h"

#include <fcntl.h>
#include <unistd.h>

#if !defined(DOXYGEN) && !defined(BURGER_DARWIN)
// Cached result from is_debugger_present()
static uint8_t g_bDebugger = 0;
#endif

#if !defined(BURGER_DARWIN)
/***************************************

	\brief Detect if a debugger is attached

	Return \ref TRUE if a debugger is attached

	Docs for this version are found here
	https://man7.org/linux/man-pages/man5/proc.5.html

***************************************/

uint_t BURGER_API Burger::is_debugger_present(void) BURGER_NOEXCEPT
{
	uint8_t bResult = g_bDebugger;

	// Already tested?
	if (!bResult) {

		// Set it to FALSE
		bResult = 0x80U;

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

				// Found it? '0' means detached.
				if (pIndex && (pIndex[11] != '0')) {
					bResult = 0x81U;
				}
			}
		}

		// Save the debugger flag
		g_bDebugger = bResult;
	}
	// Return TRUE or FALSE
	return bResult & 1U;
}
#endif

#endif
