/***************************************

	Global variable manager, iOS version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brstringfunctions.h"
#include <Foundation/NSString.h>
#include <UIKit/UIApplication.h>

/***************************************
 
	Load and launch a web page from an address string
 
***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char *pURL)
{
	// Convert to an NSString
	NSString *pString = [[NSString alloc] initWithCString:pURL encoding:NSUTF8StringEncoding];
	// Convert to a NSURL
	NSURL *pNSURL = [[NSURL alloc] initWithString:pString];
	Word uResult = 10;
	// Open the website
	if ([[UIApplication sharedApplication] openURL:pNSURL]==YES) {
		// Success!
		uResult = 0;
	}
	// Release the strings
	[pNSURL release];
	[pString release];
	return uResult;
}


#endif
