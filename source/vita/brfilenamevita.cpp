/***************************************

    Playstation Vita version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"
#include "brfilemanager.h"

/***************************************

	Expand a filename into Playstation Vita format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the MacOSX file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a volume name.
	If the path starts with ".D2:" then it is a full pathname starting with a volume name enumerated by volumes.
	If the path starts with a "$:","*:" or "@:" then use special prefix numbers 32-34
	If the path starts with 0: through 31: then use prefix 0-31.
	Otherwise prepend the pathname with the contents of prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory PrevDir

	All returned pathnames will NOT have a trailing "/", they will
	take the form of /foo/bar/file.txt or similar
	
	Examples:<br>
	If home drive is named "boot" then ":boot:foo:bar.txt" = "/foo/bar.txt"<br>
	If the home drive is not named "boot" then ":boot:foo:bar.txt" = "/Volumes/boot/foo/bar.txt"<br>
	"@:game:data.dat" = "/Users/<Current user>/Library/Preferences/game/data.dat"

***************************************/

const char *Burger::Filename::GetNative(void)
{
	Expand();		// Resolve prefixes

	// Determine the final length
	const Word8 *pFullPathName = reinterpret_cast<const Word8 *>(m_pFilename);
	WordPtr uOutputLength = StringLength(reinterpret_cast<const char *>(pFullPathName))+4;
	char *pOutput = m_NativeFilename;
	if (uOutputLength>=sizeof(m_NativeFilename)) {
		pOutput = static_cast<char *>(Alloc(uOutputLength));
		if (!pOutput) {
			// Out of memory failure case
			m_NativeFilename[0] = 0;
			return m_NativeFilename;
		}
	}
	m_pNativeFilename = pOutput;
	
	// Now, is this a fully qualified name?
	
	if (pFullPathName[0]==':') {				// First char is ':' for a qualified pathname
		// Discard the leading colon
		++pFullPathName;
		// Look for the mount name by scanning for the ending colon
		const Word8 *pFileParsed = reinterpret_cast<Word8*>(StringCharacter(reinterpret_cast<const char *>(pFullPathName),':'));
		if (pFileParsed) {
			// Length of the mount name
			WordPtr uLength = static_cast<WordPtr>(pFileParsed-pFullPathName)+1;
			// Copy :app0: to app0:/
			MemoryCopy(pOutput,pFullPathName,uLength);
			pOutput[uLength] = '/';
			pOutput+=uLength+1;
			// Accept the input
			pFullPathName = pFileParsed+1;
		}
	}

	// Convert the rest of the path
	// Colons to slashes
	
	Word uTemp = pFullPathName[0];
	if (uTemp) {
		do {
			++pFullPathName;
			if (uTemp==':') {
				uTemp = '/';		// Unix style
			}
			
			pOutput[0] = uTemp;
			++pOutput;
			uTemp = pFullPathName[0];
		} while (uTemp);
		
		// A trailing slash assumes more to follow, get rid of it
		--pOutput;
		if ((pOutput==m_pNativeFilename) ||		// Only a '/'? (Skip the check then)
			(reinterpret_cast<Word8*>(pOutput)[0]!='/')) {
			++pOutput;		// Remove trailing slash
		}
	}
	pOutput[0] = 0;			// Terminate the "C" string
	return m_pNativeFilename;
}

/***************************************

	Convert a Vita filename into BurgerLib format.

	Using the rules for a Burgerlib type pathname, expand a path
	from a Vita filename into BurgerLib.

	The pathname will have an ending colon.
	
	Examples:<br>
	app0:/foo.txt becomes :app0:foo.txt:<br>
	tempfile.txt becomes 8:tempfile.txt:<br>
		
***************************************/

void Burger::Filename::SetFromNative(const char *pInput)
{
	Clear();	// Clear out the previous string

	// Determine the length of the string buffer
	WordPtr uInputLength = StringLength(pInput);
	WordPtr uOutputLength = uInputLength+6;		// Could prepend :app0:, 6 characters
	char *pOutput = m_Filename;
	if (uOutputLength>=sizeof(m_Filename)) {
		pOutput = static_cast<char *>(Alloc(uOutputLength));
		if (!pOutput) {
			// We're boned
			return;
		}
	}
	m_pFilename = pOutput;

	// Convert mount names to drive names

	const char *pMountName = StringCharacter(pInput,':');
	if (!pMountName) {		// Must I prefix with the current directory?
		// Use the default directory
		if (pInput[0]=='/') {
			MemoryCopy(pOutput,":app0:",6);
			pOutput+=6;
		} else {
			pOutput[0] = '8';
			pOutput[1] = ':';
			pOutput+=2;
		}
	} else {
		// Convert app0:/ to :app0:
		WordPtr uMountNameSize = static_cast<WordPtr>((pMountName-pInput)+1);
		pOutput[0] = ':';
		MemoryCopy(pOutput+1,pInput,uMountNameSize);
		pOutput += uMountNameSize+1;
		pInput+=uMountNameSize;
	}

// Now, just copy the rest of the path	
	
	Word uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	if (uTemp=='/') {
		++pInput;
		uTemp = reinterpret_cast<const Word8*>(pInput)[0];
	}
	if (uTemp) {				// Any more?
		do {
			++pInput;			// Accept char
			if (uTemp=='/') {
				uTemp = ':';
			}
			pOutput[0] = uTemp;	// Save char
			++pOutput;
			uTemp = reinterpret_cast<const Word8*>(pInput)[0];	// Next char
		} while (uTemp);		// Still more?
	}

	// The wrap up...
	// Make sure it's appended with a colon

	if (reinterpret_cast<const Word8*>(pOutput)[-1]!=':') {
		pOutput[0] = ':';
		++pOutput;
	}
	pOutput[0] = 0;			// End the string with zero
}


#endif