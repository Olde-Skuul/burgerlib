/***************************************

	This resource template allows for the easy creation of Mac
	OS 9 compatible screen savers using the MacDim screen saver shell.
	
	MacDim is available at http://www.ibrium.se/
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MACDIM_R__
#define __MACDIM_R__

#define MDIMRESOURCEID 400	// Resource ID of the MDim resource

type 'MDim' {
	fill byte[3];			// Unused
flags:
	fill bit[3];			// Unused
	boolean allowDrawing,inhibitDrawing;		// Disable all other drawing calls?
	boolean powerPCOptional,powerPCOnly;		// Power PC only?
	boolean noRuntimePrefs,haveRuntimePrefs;	// Are there prefs during screen saver operation?
	boolean enableOthersavers,disableOthersavers;	// Can this screen saver work with other screen savers?
	boolean noPrefs,havePrefs;					// Are there configurable prefs at all?
};

#endif
