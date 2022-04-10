/***************************************

    iOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "brmemoryfunctions.h"
#include "brfilemanager.h"

/***************************************

	Expand a filename into iOS format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the iOS file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a volume name.
	If the path starts with ".D2:" then it is a full pathname starting with a volume name enumated by volumes.
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

const char *Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	Expand();		// Resolve prefixes

	const uint8_t *pFullPathName = reinterpret_cast<const uint8_t *>(m_Filename.c_str());
	uintptr_t uOutputLength = StringLength(reinterpret_cast<const char *>(pFullPathName))+10;
	m_NativeFilename.resize(uOutputLength);

	char *pOutput = m_NativeFilename.data();
	
	// Now, is this a fully qualified name?
	
	if (pFullPathName[0]==':') {				// First char is ':' for a qualified pathname
	
		// Look for the volume name by scanning for the ending colon
		const uint8_t *pFileParsed = reinterpret_cast<uint8_t*>(StringCharacter(reinterpret_cast<const char *>(pFullPathName)+1,':'));
		if (pFileParsed) {
			// Is this on the boot volume?
			// Also test for the special case of :Foo vs :FooBar

            {
                uintptr_t uIndex = FileManager::GetBootNameSize();

				// Test for boot name match
				if (MemoryCaseCompare(FileManager::GetBootName(),pFullPathName,uIndex)) {
					StringCopy(pOutput,"/Volumes");	// Look in the mounted volumes folder
					pOutput+=8;								// At the end of /Volumes
				} else {
					// If the volume requested is the boot volume, remove the name
					// and use the leading "/" for the root prefix.
					pFullPathName+=(uIndex-1);
				}
			}
		}
	}

	// Convert the rest of the path
	// Colons to slashes
	
	uint_t uTemp = pFullPathName[0];
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
		if ((pOutput==m_NativeFilename.c_str()) ||		// Only a '/'? (Skip the check then)
			(reinterpret_cast<uint8_t*>(pOutput)[0]!='/')) {
			++pOutput;		// Remove trailing slash
		}
	}
	pOutput[0] = 0;			// Terminate the "C" string
	return m_NativeFilename.c_str();
}

/***************************************

	Convert a iOS filename into BurgerLib format.

	Using the rules for a Burgerlib type pathname, expand a path
	from a iOS filename into BurgerLib.

	The pathname will have an ending colon.
	
	Examples:<br>
	If home drive is named "boot" then "/foo/bar.txt" = ":boot:foo:bar.txt"<br>
	If the first name is "/Volumes" then "/Volumes/boot/foo/bar.txt" = ":boot:foo:bar.txt"<br>
		
***************************************/

Burger::eError BURGER_API Burger::Filename::SetFromNative(const char *pInput) BURGER_NOEXCEPT
{
	clear();	// Clear out the previous string

	// Determine the length of the prefix
	uintptr_t uInputLength = StringLength(pInput);
	const char *pBaseName;
	uintptr_t uBaseNameLength;
	if (reinterpret_cast<const uint8_t *>(pInput)[0]!='/') {		// Must I prefix with the current directory?
		if ((uInputLength>=2) && !MemoryCompare("./",pInput,2)) {		// Dispose of "current directory"
			pInput+=2;
			uInputLength-=2;
		}
		pBaseName = "8:";
		uBaseNameLength = 2;
	} else {
		if ((uInputLength>9) && !MemoryCompare(pInput,"/Volumes/",9)) {
			pBaseName = ":";		// Place a leading colon in the output
			uBaseNameLength = 1;
			pInput+=9;
			uInputLength-=9;
		} else {
			pBaseName = FileManager::GetBootName();
			uBaseNameLength = FileManager::GetBootNameSize();
			++pInput;
			--uInputLength;
		}
	}

	uintptr_t uOutputLength = uBaseNameLength+uInputLength+10;
	char *pOutput = m_Filename;
	if (uOutputLength>=sizeof(m_Filename)) {
		pOutput = static_cast<char *>(Alloc(uOutputLength));
		if (!pOutput) {
			return kErrorOutOfMemory;
		}
	}
	m_Filename.assign(pOutput);
	
	MemoryCopy(pOutput,pBaseName,uBaseNameLength);
	pOutput+=uBaseNameLength;
	
// Now, just copy the rest of the path	
	
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
	if (uTemp) {				// Any more?
		do {
			++pInput;			// Accept char
			if (uTemp=='/') {
				uTemp = ':';
			}
			pOutput[0] = uTemp;	// Save char
			++pOutput;
			uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];	// Next char
		} while (uTemp);		// Still more?
	}

	// The wrap up...
	// Make sure it's appended with a colon

	if (reinterpret_cast<const uint8_t*>(pOutput)[-1]!=':') {
		pOutput[0] = ':';
		++pOutput;
	}
	pOutput[0] = 0;			// End the string with zero
	return kErrorNone;
}


#endif
