/***************************************

	MacOS version of Burger::Globals

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <Folders.h>
#include <Gestalt.h>
#include <InternetConfig.h>

/***************************************

	Load and launch a web page from an address string

***************************************/

uint_t BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
	ICInstance inst;

	OSStatus err = -1;
	if (ICStart != nullptr) {
		err = ICStart(&inst, '????'); // Use your creator code if you have one!
		if (err == noErr) {
#if !defined(BURGER_MACCARBON)
			err = ICFindConfigFile(inst, 0, nil);
			if (err == noErr)
#endif
			{
				long startSel = 0;
				long endSel = static_cast<long>(string_length(pURL));
				err = ICLaunchURL(inst, "\p", const_cast<char*>(pURL), endSel,
					&startSel, &endSel);
			}
			ICStop(inst);
		}
	}
	return static_cast<uint_t>(err);
}

#endif
