/***************************************

	XBox 360 thread helpers

	Copyright (c) 2010-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "x360_thread.h"

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
#define NOD3D
#define NONET
#include <xtl.h>

#if !defined(DOXYGEN)
// Each element is 32 bits wide on 32 bit systems, and 64 bits wide on 64 bit
// systems
typedef struct tagTHREADNAME_INFO {
	uintptr_t dwType;     // Must be 0x1000.
	LPCSTR szName;        // Pointer to name (in user addr space).
	uintptr_t dwThreadID; // Thread ID (-1=caller thread).
	uintptr_t dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#endif

/*! ************************************

	\brief Set a thread's name for debugging.

	Windows, and also the Xbox 360, use a MAGIC exception to notify the
	debugger that the currently executing thread has a name. This function will
	throw that exception so the debugger can capture the name.

	\sa Win32::throw_thread_naming_exception()

***************************************/

void Burger::Xbox360::set_thread_name(const char* pName, uint32_t uThreadID)
{

	// Invoke the DEEP magic by throwing an exception that's captured by
	// the Visual Studio debugger

	// Create the exception message
	THREADNAME_INFO NewThreadName;
	NewThreadName.dwType = 0x1000;
	NewThreadName.szName = pName;
	NewThreadName.dwThreadID = uThreadID;
	NewThreadName.dwFlags = 0;

	// Use Xbox 360 specific compiler commands
	// Ah, the beauty of console programming!
	__try {
		RaiseException(0x406D1388U, 0,
			sizeof(NewThreadName) / sizeof(ULONG_PTR),
			reinterpret_cast<const ULONG_PTR*>(&NewThreadName));
	} __except (GetExceptionCode() == 0x406D1388U ?
			EXCEPTION_CONTINUE_EXECUTION :
			EXCEPTION_EXECUTE_HANDLER) {
		__noop;
	}
}

#endif
