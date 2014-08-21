/***************************************

	Global variable manager, MacOSX version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brstring.h"
#include "brfilename.h"
#include "broutputmemorystream.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>

/***************************************
 
	Load and launch a web page from an address string
 
***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char *pURL)
{
	// Convert the string to a URL
	CFURLRef URLReference = CFURLCreateWithBytes(NULL,reinterpret_cast<const UInt8 *>(pURL),
			StringLength(pURL),kCFStringEncodingUTF8,NULL);
	OSStatus uResult = LSOpenCFURLRef(URLReference,0);
	CFRelease(URLReference);
	return static_cast<Word>(uResult);
}

/***************************************

	Execute a tool and capture the text output

***************************************/

int BURGER_API Burger::Globals::ExecuteTool(const char *pFilename,const char *pParameters,OutputMemoryStream *pOutput)
{
	// Convert to a filename
	Filename ProgramName(pFilename);
	// Create a command line
	String CommandLine("\"",ProgramName.GetNative(),"\" ",pParameters);
	// Assume an error
	int iResult = 10;
	// Call the program
	FILE *pPipe = popen(CommandLine.GetPtr(),"r");
	if (pPipe) {
		// Get the output
		while (!feof(pPipe)) {
			char Buffer[1024];
			size_t uRead = fread(Buffer,1,1024,pPipe);
			if (pOutput) {
				// Keep it, or discard it
				pOutput->Append(Buffer,uRead);
			}
		}
		// Wait for the program to complete
		iResult = pclose(pPipe);
	}
	// Return the result
	return iResult;
}

/***************************************
 
	Read an environment variable as UTF8
 
***************************************/

const char *BURGER_API Burger::Globals::GetEnvironmentString(const char *pKey)
{
	const char *pValue = getenv(pKey);
	if (pValue) {
		pValue = StringDuplicate(pValue);
	}
	return pValue;
}

/***************************************
 
	Set an environment variable with a UTF8 string
 
***************************************/

Word BURGER_API Burger::Globals::SetEnvironmentString(const char *pKey,const char *pInput)
{
	// Pass to the operating system
	return static_cast<Word>(setenv(pKey,pInput,TRUE));
}

#endif
