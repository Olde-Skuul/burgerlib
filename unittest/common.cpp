/***************************************

	Unit tests for burgerlib

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "common.h"
#include "testbrtypes.h"
#include "testbrendian.h"
#include "testbrfloatingpoint.h"
#include "testbrfixedpoint.h"
#include "testbrhashes.h"
#include "testbrstrings.h"
#include "testbrcompression.h"
#include "testbrtimedate.h"
#include "testbrmatrix3d.h"
#include "testbrmatrix4d.h"
#include "testbrstaticrtti.h"
#include "testbrdisplay.h"
#include "testbrfilemanager.h"
#include "testbrfileloaders.h"
#include "testbrprintf.h"
#include "createtables.h"
#include "brconsolemanager.h"
#include "brcommandparameterbooltrue.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(BURGER_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#if defined(BURGER_XBOX) || defined(BURGER_XBOX360) || (defined(BURGER_WINDOWS) && !defined(BURGER_WATCOM) && !defined(BURGER_METROWERKS))
#define USESECURE
#endif

//
// Boolean string to display PASSED on "false" and
// FAILED if "true".
//

static const char *cFailed[2] = {
	"PASSED",
	"FAILED"
};

//
// Set to true if only failed tests are to be output.
// false will display all warnings.
//

static Word g_ErrorOnly = TRUE;

//
// Data pattern that's unlikely to be found in a unit test.
// Used to simulate uninitialized memory
//

static const Word8 g_BlastPattern[16] = {
	0xD5,0xAA,0x96,0xDE,0xAA,0xDE,0xAD,0xBE,
	0xEF,0x91,0x19,0x0F,0xF0,0xCA,0xFE,0x44
};

/***************************************

	Output the test failures to stderr and
	for Windows platforms, OutputDebugString()

	\param pTemplate Error message formatted for printf
	\param uFailure FALSE if no failure (And no debug spew), non-FALSE, print the error

***************************************/

void BURGER_ANSIAPI ReportFailure(const char *pTemplate,Word uFailure,...)
{
	if (!g_ErrorOnly || uFailure) {
		va_list Args;
		char TempString[2048];
		memcpy(TempString,cFailed[uFailure!=0],6);
		WordPtr uEndMark;
		if (pTemplate) {						// No message, no error!
			va_start(Args,uFailure);		// Start parm passing
			TempString[6]=' ';
#if defined(USESECURE)
			uEndMark = vsprintf_s(TempString+7,sizeof(TempString)-9,pTemplate,Args)+7U;		// Create the message
#else
			uEndMark = vsprintf(TempString+7,pTemplate,Args)+7U;		// Create the message
#endif
			va_end(Args);					// End parm passing
		} else {
			uEndMark = 6;		// Kill the string
		}
		TempString[uEndMark] = '\n';
		fwrite(TempString,1,uEndMark+1,stdout);
#if defined(BURGER_WINDOWS)
		TempString[uEndMark+1] = '\0';
		OutputDebugStringA(TempString);
#endif
	}
}

/***************************************

	Output a message to stdout and for Windows
	OutputDebugString

	\param pMessage String formatted for printf

***************************************/

void BURGER_ANSIAPI Message(const char *pMessage,...)
{
	if (pMessage && pMessage[0]) {
		va_list Args;
		char TempString[2048];
		WordPtr uEndMark;
		va_start(Args,pMessage);		// Start parm passing
#if defined(USESECURE)
		uEndMark = vsprintf_s(TempString,sizeof(TempString)-2,pMessage,Args)+0U;		// Create the message
#else
		uEndMark = vsprintf(TempString,pMessage,Args)+0U;		// Create the message
#endif
		va_end(Args);					// End parm passing
		TempString[uEndMark] = '\n';
		fwrite(TempString,1,uEndMark+1,stdout);
#if defined(BURGER_WINDOWS)
		TempString[uEndMark+1] = '\0';
		OutputDebugStringA(TempString);
#endif
	}
}

/***************************************

	\brief Fill a buffer with a known byte pattern
	Take a 16 byte pattern and fill the buffer with it.
	This is designed to check for buffer overruns or
	underruns by looking for an unlikely data pattern in
	memory

	\param pOutput Buffer to fill with the pattern
	\param uSize Size of the buffer to fill with the pattern

***************************************/

void BURGER_API BlastBuffer(void *pOutput,WordPtr uSize)
{
	WordPtr i = 0;
	do {
		static_cast<Word8 *>(pOutput)[0] = g_BlastPattern[i];
		i=(i+1)&15;
		pOutput = static_cast<Word8 *>(pOutput)+1;
	} while (--uSize);
}

/***************************************

	\brief Test a buffer with a known byte pattern

	Check to see if the "uninitialized" memory is still
	untouched. There will be a pie
	and check the "out of bounds memory" if it matches
	the fill buffer from the function BlastBuffer().

	This is used to verify memory write functions to ensure
	that there is no buffer over or under run.

	\param pBuffer Buffer to fill with the pattern
	\param uSize Size of the buffer to fill with the pattern

***************************************/

Word BURGER_API VerifyBuffer(const void *pBuffer,WordPtr uSize,const void *pInput,WordPtr uSkip)
{
	WordPtr i=0;	// Index to the BlastPattern buffer
	Word uResult=FALSE;
	// Get the offset mark. Note that "negative" numbers become huge positive
	// numbers so the uMark>=uSkip test works for both before and after the skip buffer
	WordPtr uMark=static_cast<WordPtr>(static_cast<const Word8*>(pBuffer)-static_cast<const Word8 *>(pInput));
	do {
		// Check if it's in the "skip" area.
		if (uMark>=uSkip) {
			uResult |= (static_cast<const Word8 *>(pBuffer)[0] != g_BlastPattern[i]);
		}
		i=(i+1)&15;		// Wrap around the BlastPattern Buffer
		pBuffer = static_cast<const Word8 *>(pBuffer)+1;
		++uMark;
	} while (--uSize);
	return uResult;
}

//
// Test everything
//

int BURGER_ANSIAPI main(int argc,const char **argv)
{
	Word bVerbose = TRUE;
	int iResult = 0;
	Word bDoTests = TRUE;

	// On systems that support a command line, allow the command line
	// to be parsed and handle the tests based on the input

#if defined(ALLOWCOMMANDLINE)
	Burger::ConsoleApp MyApp(argc,argv);
	Burger::CommandParameterBooleanTrue Verbose("Verbose output","v");
	Burger::CommandParameterBooleanTrue WriteTables("Generate data tables","t");

	const Burger::CommandParameter *MyParms[] = {
		&Verbose,&WriteTables
	};

	iResult = Burger::CommandParameter::Process(MyApp.GetArgc(),MyApp.GetArgv(),MyParms,sizeof(MyParms)/sizeof(MyParms[0]),
		"Usage: unittests\n\n"
		"This program tests Burgerlib\n");

	if (iResult<0) {
		bDoTests = FALSE;
	}
	bVerbose = Verbose.GetValue();

	if (WriteTables.GetValue()) {
		WriteDataTables();
		bDoTests = FALSE;
		iResult = 0;
	}


#else
	BURGER_UNUSED(argc);
	BURGER_UNUSED(argv);
#endif

	if (bDoTests) {
		iResult = TestBrtypes(bVerbose);
		iResult |= TestBrendian(bVerbose);
		iResult |= TestBrfixedpoint(bVerbose);
		iResult |= TestBrfloatingpoint(bVerbose);
		iResult |= TestBrhashes(bVerbose);

#if 0
		CreateTables();
		iResult |= TestBrmatrix3d();
		iResult |= TestBrmatrix4d();
		iResult |= TestBrstrings();
		iResult |= TestBrstaticrtti();
		iResult |= TestBrcompression();
		iResult |= TestBrDisplay();
		iResult |= TestDateTime();
		iResult |= TestStdoutHelpers(bVerbose);
		iResult |= TestBrprintf();
		iResult |= FileManagerTest(bVerbose);
		iResult |= FileLoaderTest(bVerbose);
#endif
	}
	return iResult;
}
