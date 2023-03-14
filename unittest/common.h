/***************************************

	Unit tests for burgerlib

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_LINUX) || \
	defined(BURGER_MACOSX) || defined(BURGER_MSDOS)
#define ALLOWCOMMANDLINE
#endif

// Flags to determine verbosity on output
#define VERBOSE_DISABLE 0x0000U  // Disable all tests and messages
#define VERBOSE_MSG 0x0001U      // Set for information messages
#define VERBOSE_TIME 0x0002U     // Set for time display
#define VERBOSE_MACROS 0x0004U   // Set for showing macros
#define VERBOSE_FILE 0x0008U     // Set for file testing
#define VERBOSE_NETWORK 0x0010U  // Set for network testing
#define VERBOSE_DIALOGS 0x0020U  // Set for testing dialogs
#define VERBOSE_DISPLAY 0x0040U  // Set for testing display modes
#define VERBOSE_COMPRESS 0x0080U // Set for testing compression algorithms
#define VERBOSE_DLL 0x100U       // Set for testing DLLS
#define VERBOSE_DOTESTS 0x8000U  // Set if tests are to be performed
#define VERBOSE_ALL 0xFFFFFFFFU

extern void BURGER_ANSIAPI ReportFailure(
	const char* pTemplate, uint_t uFailure, ...) BURGER_NOEXCEPT;
extern void BURGER_ANSIAPI Message(const char* pMessage, ...) BURGER_NOEXCEPT;
extern void BURGER_API BlastBuffer(
	void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT;
extern uint_t BURGER_API VerifyBuffer(const void* pBuffer, uintptr_t uSize,
	const void* pInput, uintptr_t uSkip) BURGER_NOEXCEPT;
extern int BURGER_ANSIAPI main(int argc, const char** argv);

#endif
