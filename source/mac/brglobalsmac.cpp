/***************************************

	MacOS version of Burger::Globals

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brglobals.h"
#if defined(BURGER_MAC)
#include "brstringfunctions.h"
#include <InternetConfig.h>

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char *pURL)
{
	ICInstance inst;

	OSStatus err = -1;
	if (ICStart!=NULL) {
		err = ICStart(&inst,'????');		// Use your creator code if you have one!
		if (err == noErr) {
#if !TARGET_API_MAC_CARBON
			err = ICFindConfigFile(inst, 0, nil);
			if (err == noErr)
#endif
			{
				long startSel = 0;
				long endSel = StringLength(pURL);
				err = ICLaunchURL(inst, "\p",const_cast<char *>(pURL),endSel,&startSel,&endSel);
			}
			ICStop(inst);
		}
	}
	return static_cast<Word>(err);
 }

#endif