/***************************************

    Unit tests for burgerlib

    Copyright (c) 1995-2018 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if defined(BURGER_WINDOWS) || defined(BURGER_LINUX) || defined(BURGER_MACOSX)
#define ALLOWCOMMANDLINE
#endif

// Flags to determine verbosity on output
#define VERBOSE_DISABLE 0x0000  // Disable all tests and messages
#define VERBOSE_MSG 0x0001      // Set for information messages
#define VERBOSE_TIME 0x0002     // Set for time display
#define VERBOSE_MACROS 0x0004   // Set for showing macros
#define VERBOSE_FILE 0x0008     // Set for file testing
#define VERBOSE_NETWORK 0x0010  // Set for network testing
#define VERBOSE_DIALOGS 0x0020  // Set for testing dialogs
#define VERBOSE_DISPLAY 0x0040  // Set for testing display modes
#define VERBOSE_COMPRESS 0x0080 // Set for testing compression algorithms
#define VERBOSE_DOTESTS 0x8000  // Set if tests are to be performed
#define VERBOSE_ALL 0xFFFF

extern void BURGER_ANSIAPI ReportFailure(
    const char* pTemplate, uint_t uFailure, ...) BURGER_NOEXCEPT;
extern void BURGER_ANSIAPI Message(const char* pMessage, ...) BURGER_NOEXCEPT;
extern void BURGER_API BlastBuffer(void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT;
extern uint_t BURGER_API VerifyBuffer(
    const void* pBuffer, uintptr_t uSize, const void* pInput, uintptr_t uSkip) BURGER_NOEXCEPT;
extern int BURGER_ANSIAPI main(int argc, const char** argv);

#endif
