/***************************************

	Burgerlib resource definitions
	
	Define common resource file types
	Used only for MacOS and MacOSX targets
	with Apple's Rez compiler
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BURGER_R__
#define __BURGER_R__

// Common non-standard ASCII characters

#define COPYRIGHT "\0XA9"
#define TRADEMARK "\0XAA"

// Define the carb resource for Carbon applications

type 'carb' {
};

// Raw text

type 'TEXT' {
	hex string;
};

#endif
