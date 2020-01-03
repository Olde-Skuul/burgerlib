/***************************************

    Unit tests for burgerlib

    Copyright (c) 1995-2018 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "common.h"
#include "brcommandparameterbooltrue.h"
#include "brconsolemanager.h"
#include "brglobals.h"
#include "createtables.h"
#include "testbralgorithm.h"
#include "testbrcompression.h"
#include "testbrdisplay.h"
#include "testbrendian.h"
#include "testbrfileloaders.h"
#include "testbrfilemanager.h"
#include "testbrfixedpoint.h"
#include "testbrfloatingpoint.h"
#include "testbrhashes.h"
#include "testbrmatrix3d.h"
#include "testbrmatrix4d.h"
#include "testbrnetwork.h"
#include "testbrprintf.h"
#include "testbrstaticrtti.h"
#include "testbrstrings.h"
#include "testbrtimedate.h"
#include "testbrtypes.h"
#include "testcharset.h"
#include "testmacros.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Burger;

#if defined(BURGER_WINDOWS)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif

#if (defined(_MSC_VER) && (_MSC_VER < 1400)) || \
    (defined(BURGER_METROWERKS) && !defined(__MSL__))
#else
#define USESECURE
#endif

//
// Boolean string to display PASSED on "false" and FAILED if "true".
//

static const char* cFailed[2] = {"PASSED", "FAILED"};

//
// Set to true if only failed tests are to be output. false will display all
// warnings.
//

static uint_t g_ErrorOnly = TRUE;

//
// Data pattern that's unlikely to be found in a unit test. Used to simulate
// uninitialized memory
//

static const uint8_t g_BlastPattern[16] = {0xD5, 0xAA, 0x96, 0xDE, 0xAA, 0xDE,
    0xAD, 0xBE, 0xEF, 0x91, 0x19, 0x0F, 0xF0, 0xCA, 0xFE, 0x44};

/***************************************

    Output the test failures to stderr and for Windows platforms,
    OutputDebugString()

    \param pTemplate Error message formatted for printf
    \param uFailure FALSE if no failure (And no debug spew), non-FALSE, print
        the error

***************************************/

void BURGER_ANSIAPI ReportFailure(const char* pTemplate, uint_t uFailure, ...) BURGER_NOEXCEPT
{
    if (!g_ErrorOnly || uFailure) {
        va_list Args;
        char TempString[2048];
        memcpy(TempString, cFailed[uFailure != 0], 6);
        uintptr_t uEndMark;
        if (pTemplate) {              // No message, no error!
            va_start(Args, uFailure); // Start parm passing
            TempString[6] = ' ';
#if defined(USESECURE)
            uEndMark = vsnprintf(TempString + 7, sizeof(TempString) - 9,
                           pTemplate, Args) +
                7U; // Create the message
#else
            uEndMark = vsprintf(TempString + 7, pTemplate, Args) +
                7U; // Create the message
#endif
            va_end(Args); // End parm passing
        } else {
            uEndMark = 6; // Kill the string
        }
        TempString[uEndMark] = '\n';
        fwrite(TempString, 1, uEndMark + 1, stdout);
#if defined(BURGER_WINDOWS)
        TempString[uEndMark + 1] = '\0';
        OutputDebugStringA(TempString);
#endif
    }
}

/***************************************

    Output a message to stdout and for Windows OutputDebugString

    \param pMessage String formatted for printf

***************************************/

void BURGER_ANSIAPI Message(const char* pMessage, ...) BURGER_NOEXCEPT
{
    if (pMessage && pMessage[0]) {
        va_list Args;
        char TempString[2048];
        uintptr_t uEndMark;
        va_start(Args, pMessage); // Start parm passing
#if defined(USESECURE)
        uEndMark =
            vsnprintf(TempString, sizeof(TempString) - 2, pMessage, Args) +
            0U; // Create the message
#else
        uEndMark =
            vsprintf(TempString, pMessage, Args) + 0U; // Create the message
#endif
        va_end(Args); // End parm passing
        TempString[uEndMark] = '\n';
        fwrite(TempString, 1, uEndMark + 1, stdout);
#if defined(BURGER_WINDOWS)
        TempString[uEndMark + 1] = '\0';
        OutputDebugStringA(TempString);
#endif
    }
}

/***************************************

    \brief Fill a buffer with a known byte pattern

    Take a 16 byte pattern and fill the buffer with it. This is designed to
    check for buffer overruns or underruns by looking for an unlikely data
    pattern in memory

    \param pOutput Buffer to fill with the pattern
    \param uSize Size of the buffer to fill with the pattern

***************************************/

void BURGER_API BlastBuffer(void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT
{
    uintptr_t i = 0;
    do {
        static_cast<uint8_t*>(pOutput)[0] = g_BlastPattern[i];
        i = (i + 1) & 15;
        pOutput = static_cast<uint8_t*>(pOutput) + 1;
    } while (--uSize);
}

/***************************************

    \brief Test a buffer with a known byte pattern

    Check to see if the "uninitialized" memory is still untouched. There will be
    a pie and check the "out of bounds memory" if it matches the fill buffer
    from the function BlastBuffer().

    This is used to verify memory write functions to ensure that there is no
    buffer over or under run.

    \param pBuffer Buffer to fill with the pattern
    \param uSize Size of the buffer to fill with the pattern

***************************************/

uint_t BURGER_API VerifyBuffer(
    const void* pBuffer, uintptr_t uSize, const void* pInput, uintptr_t uSkip) BURGER_NOEXCEPT
{
    uintptr_t i = 0; // Index to the BlastPattern
                   // buffer
    uint_t uResult = FALSE;
    // Get the offset mark. Note that "negative" numbers become huge positive
    // numbers so the uMark>=uSkip test works for both before and after the skip
    // buffer
    uintptr_t uMark = static_cast<uintptr_t>(
        static_cast<const uint8_t*>(pBuffer) - static_cast<const uint8_t*>(pInput));
    do {
        // Check if it's in the "skip" area.
        if (uMark >= uSkip) {
            uResult |=
                (static_cast<const uint8_t*>(pBuffer)[0] != g_BlastPattern[i]);
        }
        i = (i + 1) & 15; // Wrap around the BlastPattern Buffer
        pBuffer = static_cast<const uint8_t*>(pBuffer) + 1;
        ++uMark;
    } while (--uSize);
    return uResult;
}

//
// Test everything
//

int BURGER_ANSIAPI main(int argc, const char** argv)
{
    uint_t uVerbose = VERBOSE_ALL;
    int iResult = 0;
    uint_t bVersion;

    // On systems that support a command line, allow the command line to be
    // parsed and handle the tests based on the input

    ConsoleApp MyApp(argc, argv);
#if defined(ALLOWCOMMANDLINE)
    CommandParameterBooleanTrue Version("Show version and exit", "version");
    CommandParameterBooleanTrue WriteTables(
        "Generate data tables and exit", "table");
    CommandParameterBooleanTrue AllTests("Perform all tests", "all");

    CommandParameterBooleanTrue Verbose("Verbose output", "v");
    CommandParameterBooleanTrue ShowMacros("Show Macros", "macros");
    CommandParameterBooleanTrue DialogTests("Dialog tests", "dialog");
    CommandParameterBooleanTrue NetworkTests("Network tests", "network");
    CommandParameterBooleanTrue TimeTests("Time tests", "time");
    CommandParameterBooleanTrue DisplayTests("Display tests", "display");
    CommandParameterBooleanTrue FileTests("File tests", "file");
    CommandParameterBooleanTrue CompressTests("Compression tests", "compress");

    const CommandParameter* MyParms[] = {&Version, &WriteTables, &AllTests,
        &Verbose, &ShowMacros, &DialogTests, &NetworkTests, &TimeTests,
        &DisplayTests, &FileTests, &CompressTests};

    iResult = CommandParameter::Process(MyApp.GetArgc(), MyApp.GetArgv(),
        MyParms, sizeof(MyParms) / sizeof(MyParms[0]),
        "Usage: unittests\n\n"
        "This program tests Burgerlib\n");

    bVersion = Version.GetValue();

    // Determine the tests to perform
    if (iResult < 0) {
        uVerbose = VERBOSE_DISABLE;
    } else if (AllTests.GetValue()) {
        uVerbose = VERBOSE_ALL;
        bVersion = TRUE;
    } else {
        uVerbose = VERBOSE_DOTESTS;
        if (Verbose.GetValue()) {
            uVerbose |= VERBOSE_MSG;
        }
        if (ShowMacros.GetValue()) {
            uVerbose |= VERBOSE_MACROS;
        }
        if (DialogTests.GetValue()) {
            uVerbose |= VERBOSE_DIALOGS;
        }
        if (NetworkTests.GetValue()) {
            uVerbose |= VERBOSE_NETWORK;
        }
        if (TimeTests.GetValue()) {
            uVerbose |= VERBOSE_TIME;
        }
        if (DisplayTests.GetValue()) {
            uVerbose |= VERBOSE_DISPLAY;
        }
        if (FileTests.GetValue()) {
            uVerbose |= VERBOSE_FILE;
        }
        if (CompressTests.GetValue()) {
            uVerbose |= VERBOSE_COMPRESS;
        }
    }

    // Special cases

    // Write out the data tables.
    if (WriteTables.GetValue()) {
        WriteDataTables();
        uVerbose = VERBOSE_DISABLE;
        iResult = 0;
    }

#else
    BURGER_UNUSED(argc);
    BURGER_UNUSED(argv);
    // Create an instance of the memory manager so tests that allocate memory
    // won't fail
    bVersion = TRUE;
#endif

    // Display the compiler used to build this tool
    Message("Built with " BURGER_COMPILER_NAME " / " BURGER_STDCPP_NAME
        " / Version %d / " BURGER_CPU_NAME " / " BURGER_PLATFORM_NAME,
        BURGER_COMPILER_VERSION);

    if (uVerbose & VERBOSE_DOTESTS) {
        if (bVersion) {
            const uint32_t uVersion = Globals::Version();
            const uint32_t uBuild = Globals::VersionBuild();
            Message("Burgerlib version %d.%d build #%u", uVersion >> 24U,
                (uVersion >> 16U) & 0xFF, uBuild);
        }

        // Perform the main tests
        iResult = TestMacros(uVerbose);
        iResult |= TestBrtypes(uVerbose);
        iResult |= TestBrendian(uVerbose);
        iResult |= TestBralgorithm(uVerbose);
        iResult |= TestBrfixedpoint(uVerbose);
        iResult |= TestBrfloatingpoint(uVerbose);
        iResult |= TestBrmatrix3d(uVerbose);
        iResult |= TestBrmatrix4d(uVerbose);
        iResult |= TestBrstaticrtti(uVerbose);
        iResult |= TestBrhashes(uVerbose);
        iResult |= TestCharset(uVerbose);
        iResult |= TestBrstrings(uVerbose);
        iResult |= TestStdoutHelpers(uVerbose);
        iResult |= TestDateTime(uVerbose);
        iResult |= TestNetwork(uVerbose);
        iResult |= TestBrcompression(uVerbose);
        iResult |= TestBrFileManager(uVerbose);

        if (uVerbose & VERBOSE_DIALOGS) {
            TestBrDialogs();
        }

        if (uVerbose & VERBOSE_DISPLAY) {
            iResult |= TestBrDisplay();
        }

#if 0
		CreateTables();

		iResult |= TestBrprintf();

		iResult |= FileLoaderTest(bVerbose);
#endif
    }
    return iResult;
}
