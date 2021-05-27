/***************************************

    Filename Class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_MSDOS)
#include "brfilemanager.h"
#include "brdosextender.h"
#include "brnumberto.h"
#include <dos.h>
#include <direct.h>

/*! ************************************

	\brief Expand a filename into Windows format.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Windows file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a volume name.
	If the path starts with ".D2:" then it is a full pathname starting with a drive number.
	If the path starts with a "$:","*:" or "@:" then use special prefix numbers 32-34
	If the path starts with 0: through 31: then use prefix 0-31.
	Otherwise prepend the pathname with the contents of prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory PrevDir

	All returned pathnames will NOT have a trailing "\", they will
	take the form of c:\\foo\\bar\\file.txt or similar
	
	Examples:<br>
	If drive C: is named "boot" then ":boot:foo:bar.txt" = "c:\foo\bar.txt"<br>
	If there is no drive named "boot" then ":boot:foo:bar.txt" = "\\boot\foo\bar.txt"<br>
	".D2:foo:bar.txt" = "c:\foo\bar.txt"<br>
	".D4:foo:bar.txt" = "e:\foo\bar.txt"<br>
	"@:game:data.dat" = "c:\users\<Current user>\appdata\roaming\game\data.dat"
	
	\param pInput Pointer to a pathname string
	\sa Burger::Filename::Set(const char *)

***************************************/

const char *Burger::Filename::GetNative(void)
{
	// First step, expand to a full pathname without
	// prefixes

	Expand();

/***************************************

	DOS version and Win 95 version
	I prefer for all paths intended for DOS use
	a generic drive specifier before the working directory.
	The problem is that Volume LABEL are very difficult to parse
	and slow to access.

***************************************/

	// First parse either the volume name of a .DXX device number
	// I hopefully will get a volume number since DOS prefers it

	const uint8_t *pPath = reinterpret_cast<uint8_t *>(m_pFilename);		// Copy to running pointer
	uint_t uDeviceNum = static_cast<uint_t>(-1);	// Init the default drive number
	if (pPath[0] == ':') {			// Fully qualified pathname?
		uintptr_t uLength = 0;		// Init index to the volume name
		uint8_t uTemp;
		do {
			++uLength;				// Parse to the next colon
			uTemp = pPath[uLength];
		} while (uTemp!=':' && uTemp);
		uint8_t uTemp2 = pPath[uLength+1];	// Save the next char in cache
		// Ensure the name ends with ':' in the case of ":foobar"
		const_cast<uint8_t *>(pPath)[uLength] = ':';
		const_cast<uint8_t *>(pPath)[uLength+1] = 0;				// Zap the entry
		// Find a volume
		uDeviceNum = FileManager::GetVolumeNumber(reinterpret_cast<const char *>(pPath));
		const_cast<uint8_t *>(pPath)[uLength] = uTemp;				// Restore char in string
		const_cast<uint8_t *>(pPath)[uLength+1] = uTemp2;
		if (uDeviceNum == static_cast<uint_t>(-1)) {		// Can't find the volume?!?
			uDeviceNum = static_cast<uint_t>(-2);
			++pPath;					// Ignore the leading colon
		} else {
			pPath = pPath+uLength;		// Accept the name
			if (uTemp) {				// Remove the colon if it had one at the end
				++pPath;
			}
		}
		
	// Is this a "drive letter"? Look for ".d2:"
	} else if (pPath[0] == '.') {
		uint_t uTemp = pPath[1];			// Get the second char
		if ((uTemp & 0xDF) =='D') {		// Is it a 'D'?
			uintptr_t uLength = 2;		// Init numeric index
			uDeviceNum = 0;				// Init drive number
			do {
				uTemp = pPath[uLength];	// Get an ASCII char
				++uLength;
				if (uTemp==':') {		// Proper end of string?
					// If nothing was parsed, abort
					if (uLength==3) {
						uLength = 0;
						uDeviceNum = static_cast<uint_t>(-1);
					}
					break;
				}
				uTemp-='0';
				if (uTemp>=10) {		// Numeric value?
					uLength = 0;		// Abort
					uDeviceNum = static_cast<uint_t>(-1);	// Force using the CWD
					break;			// Go to phase 2
				}
				uDeviceNum = uDeviceNum*10;		// Adjust previous value */
				uDeviceNum = uDeviceNum+uTemp;	// Make full decimal result */
			} while (uDeviceNum<26);			// Loop until done */
			pPath = pPath+uLength;		// Discard accepted input
		}
	}

	// Now that I have the drive number, determine the length
	// of the output buffer and start the conversion

	uintptr_t uPathLength = StringLength(reinterpret_cast<const char *>(pPath));
	// Reserve 6 extra bytes for the prefix and/or the trailing / and null
	char *pOutput = m_NativeFilename;

	if (uPathLength>=(sizeof(m_NativeFilename)-6)) {
		pOutput = static_cast<char *>(Alloc(uPathLength+6));
		if (!pOutput) {
			pOutput=m_NativeFilename;
			uPathLength = 0;		// I'm so boned
		} else {
			m_pNativeFilename = pOutput;	// This is my buffer
		}
	}

	// Insert the prefix, if any, to the output string

	if (uDeviceNum==static_cast<uint_t>(-2)) {
		// Since I didn't find the volume name, I'll assume it's
		// a network volume
		pOutput[0] = '\\';
		pOutput[1] = '\\';
		pOutput+=2;
	} else if (uDeviceNum!=static_cast<uint_t>(-1)) {
		pOutput[0] = static_cast<char>(uDeviceNum+'A');
		pOutput[1] = ':';
		pOutput[2] = '\\';
		pOutput+=3;
	}

	// Convert the colons to slashes
	if (uPathLength) {
		uint_t uTemp;
		do {
			uTemp = pPath[0];
			++pPath;
			if (uTemp==':') {
				uTemp = '\\';
			}
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
		} while (--uPathLength);
		// Remove trailing slash
		if (uTemp=='\\') {
			--pOutput;
		}
	}
	pOutput[0] = 0;			// Terminate the "C" string
	return m_pNativeFilename;
}

/***************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and
	set the filename to that directory. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetSystemWorkingDirectory(void)
{
	SetFromNative(getcwd(NULL,0));
}

/***************************************

	Convert an MSDOS path to a Burgerlib path
	
	There is no volume name support in DOS
	
	Paths without a leading '\' are prefixed with
	the current working directory
	
	Paths with a drive letter but no leading \ will use
	the drive's current working directory
	
	If it's a network path "\\" then dispose of the drive
	letter and use the pathname.
	
	The DOS version converts these types of paths:

	"C:\TEMP\TEMP2" = ".D2:TEMP:TEMP2:"<br>
	"TEMP" = "(working directory from 8):TEMP:"<br>
	"TEMP\TEMP2" = "(working directory from 8):TEMP:TEMP2:"<br>
	"\TEMP" = ".D(Mounted drive number):TEMP:"

	Only available on MSDOS
	
***************************************/

void Burger::Filename::SetFromNative(const char *pInput)
{
	Clear();

	Regs16 Regs;		// Used for DOS calls

	// Parse out the C: (Drive number)
	// Assume no drive is found
	uint_t uDriveNum = static_cast<uint_t>(-1);

	// Get the possible drive letter
	uintptr_t uInputLength = Burger::StringLength(pInput);
	if (uInputLength>=2) {
		uint_t uTemp = reinterpret_cast<const uint8_t*>(pInput)[0];
		uTemp = (uTemp&0xDF)-'A';		// Upper case
		// Could this be a drive letter?
		if ((uTemp<26) && (reinterpret_cast<const uint8_t*>(pInput)[1]==':')) {
			uDriveNum = uTemp;
			pInput += 2;				// Accept the drive letter
			uInputLength -= 2;			// Shrink the input string
		}
	}
	
	// If not a network name, (//), query MSDOS for the current drive
	
	if (uDriveNum==static_cast<uint_t>(-1)) {
		if ((uInputLength<2) ||
			(reinterpret_cast<const uint8_t*>(pInput)[0]!='\\') ||
			(reinterpret_cast<const uint8_t*>(pInput)[1]!='\\')) {
			// Get the default drive number
			_dos_getdrive(&uDriveNum);
			--uDriveNum;
		} else {
			pInput+=2;
			uInputLength-=2;	// Parse past the volume slashes
		}
	}

	uintptr_t uWorkingDirectoryLength = 0;
	const uint8_t *pWorkingDirectory = NULL;
	
	// Obtain the current working directory for the current drive
	
	if (uInputLength && (uDriveNum!=static_cast<uint_t>(-1))) {
		if (reinterpret_cast<const uint8_t*>(pInput)[0] != '\\') {
			Regs.dx = static_cast<uint16_t>(uDriveNum+1);	// Requested drive
			uint32_t DosBuffer = GetRealBufferPtr();		// Get real memory buffer
			Regs.ds = static_cast<uint16_t>(DosBuffer>>16U);	// Pass to Dos call
			Regs.si = static_cast<uint16_t>(DosBuffer&0xFFFFU);
			Regs.ax = 0x7147;				// First try long version
			Int86x(0x21,&Regs,&Regs);		// Call DOS
			if (Regs.flags&1) {				// Carry set??
				Regs.ax = 0x4700;			// Try DOS 2.0 version
				Int86x(0x21,&Regs,&Regs);	// Get the working directory
			}
			// If carry is clear, then one of the preceding calls succeeded
			if (!(Regs.flags&1)) {
				// Convert to my pointer
				pWorkingDirectory = static_cast<uint8_t *>(RealToProtectedPtr(DosBuffer));
				uWorkingDirectoryLength = Burger::StringLength(reinterpret_cast<const char *>(pWorkingDirectory));
			}
		} else {
			--uInputLength;
			++pInput;
		}
	}
	
	// At this point, uWorkingDirectoryLength and uInputLength
	// contain the approximate length of the final string.
	// Let's make a buffer to store it!
	// 7 for .D26: and a trailing colon and ending null
	
	uintptr_t uOutputLength = uWorkingDirectoryLength+uInputLength+7;
	char *pWork = m_Filename;
	if (uOutputLength>=sizeof(m_Filename)) {
		pWork = static_cast<char *>(Alloc(uOutputLength));
		if (!pWork) {
			return;
		}
	}
	m_pFilename = pWork;
		
	// Now if the path starts with a '\' then I assume it's fully qualified
	// otherwise, I must insert the working directory for the drive
	// I assume that DriveNum has the current requested drive number

	if (uDriveNum!=static_cast<uint_t>(-1)) {
		pWork[0] = '.';		// .D2 for C:
		pWork[1] = 'D';
		pWork = Burger::NumberToAscii(&pWork[2],static_cast<uint32_t>(uDriveNum));
	}
	pWork[0] = ':';		// Append a colon
	++pWork;
	
	if (uWorkingDirectoryLength) {
		do {
			uint_t uTemp = pWorkingDirectory[0];
			++pWorkingDirectory;
			if (uTemp=='\\') {		// Convert directory holders
				uTemp = ':';		// To generic paths
			}
			pWork[0] = static_cast<char>(uTemp);	// Save char
			++pWork;
		} while (--uWorkingDirectoryLength);
	}

	if (uInputLength) {
		do {
			uint_t uTemp = reinterpret_cast<const char *>(pInput)[0];
			++pInput;
			if (uTemp=='\\') {		// Convert directory holders
				uTemp = ':';		// To generic paths
			}
			pWork[0] = static_cast<char>(uTemp);	// Save char
			++pWork;
		} while (--uInputLength);
	}

	// The wrap up...
	// Make sure it's appended with a colon

	if (pWork[-1] != ':') {
		pWork[0] = ':';
		++pWork;
	}
	pWork[0] = 0;		// Final null for "C" string
}
#endif
