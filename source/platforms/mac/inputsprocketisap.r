/***************************************

	This resource is required to alert InputSprocket
	to enable its services
	
	All mac classic compatible apps using InputSprocket must include this file
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include <InputSprocket.r>

resource 'isap' (128) {
	callsISpInit,
	usesInputSprocket
};

