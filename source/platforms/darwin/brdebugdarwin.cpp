/***************************************

	Darwin version of the Debug class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brmemoryfunctions.h"

#include <sys/sysctl.h>
#include <unistd.h>

#if !defined(DOXYGEN)
// Cached value of the debugger status
static uint8_t g_bDebugger = 0;
#endif

/***************************************

	\brief Detect if a debugger is attached

	Return \ref TRUE if a debugger is attached

***************************************/

uint_t BURGER_API Burger::is_debugger_present(void) BURGER_NOEXCEPT
{
	uint8_t bResult = g_bDebugger;

	// Already tested?
	if (!bResult) {

		// Set up for querying the kernel about this process

		int ManagementInfobase[4];

		// Query the kernel
		ManagementInfobase[0] = CTL_KERN;

		// Asking for a kinfo_proc structure
		ManagementInfobase[1] = KERN_PROC;

		// This process ID
		ManagementInfobase[2] = KERN_PROC_PID;

		// Here's the application's ID
		ManagementInfobase[3] = getpid();

		// Prepare the output structure

		struct kinfo_proc Output;
		MemoryClear(&Output, sizeof(Output));
		size_t uOutputSize = sizeof(Output);

		// Call BSD for the state of the process
		int iResult = sysctl(ManagementInfobase,
			static_cast<u_int>(BURGER_ARRAYSIZE(ManagementInfobase)), &Output,
			&uOutputSize, nullptr, 0);

		// Test for tracing (Debugging)
		bResult = 0x80U;
		if (!iResult && (Output.kp_proc.p_flag & P_TRACED)) {
			bResult = 0x81U;
		}

		// Save the debugger flag
		g_bDebugger = bResult;
	}

	// Return TRUE or FALSE
	return bResult & 1U;
}

#endif
