/***************************************

	Localization manager
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brlocalization.h"

/*! ************************************

	\class Burger::ISOLatin1
	\brief Conversion routines to support the ISOLatin1 text encoding scheme.
	
	ISOLatin1 is a high ASCII encoding that's used by the ISO 9660 file system for
	CD-ROM and DVD-ROMs. This class will allow the conversion of
	other character mappings to ISOLatin1 for use obtaining the proper filenames on
	read only media.

	The character map for ISOLatin1
	<a href="isolatin1.htm">looks like this. Click here.</a>

***************************************/
