/***************************************

    Filename Class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"

#if defined(BURGER_WINDOWS)
#include "brfilemanager.h"
#include "brutf16.h"
#include "brutf8.h"
#include "brstring16.h"
#include "brglobals.h"
#include "brnumberto.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <ObjBase.h>
#include <shlobj.h>

#if defined(BURGER_WATCOM)
#pragma library ("shfolder.lib");
#endif

//
// GUIDs needed for locating folders in Vista or higher
//

#define KF_FLAG_DONT_UNEXPAND 0x00002000
#define KF_FLAG_DONT_VERIFY 0x00004000

EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_LocalAppData = {0xF1B32785,0x6FBA, 0x4FCF, 0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91};
EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_RoamingAppData = {0x3EB685DB,0x65F9, 0x4CF6, 0xA0, 0x3A, 0xE3, 0xEF, 0x65, 0x72, 0x9F, 0x3D};

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
	// First step, expand myself to a fully qualified pathname
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
	Clear();
	DWORD uLength = GetCurrentDirectoryW(0,NULL);
	if (uLength) {
		WCHAR *pWBuffer = static_cast<WCHAR *>(Alloc(uLength*2));
		if (pWBuffer) {
			uLength = GetCurrentDirectoryW(uLength*2,pWBuffer);
			if (uLength) {
				String UTF8(static_cast<const uint16_t *>(static_cast<void *>(pWBuffer)));
				SetFromNative(UTF8.GetPtr());
			}
			Free(pWBuffer);
		}
	}
}

/***************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set
	the filename to that directory. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetApplicationDirectory(void)
{
	Clear();

	// Ask windows what's the folder the app is running in

	WCHAR Buffer[MAX_PATH];

	// Try the easy way
	DWORD uLength = GetModuleFileNameW(NULL,Buffer,BURGER_ARRAYSIZE(Buffer));
	if (uLength) {
		// There's a filename
		WCHAR *pWBuffer;
		// See if the buffer was filled. If so, it's a deep path so deal with it.

		if (uLength>=BURGER_ARRAYSIZE(Buffer)) {

			//
			// DAMN YOU MICROSOFT!
			// In case the folder is longer than MAX_PATH, do
			// a binary doubling of a pathname buffer
			// until the function succeeds or 
			// the buffers just don't make sense anymore
			//

			// Start with this size
			DWORD uTestLength = BURGER_ARRAYSIZE(Buffer);
			pWBuffer = NULL;
			do {
				// Double the size for this pass
				uTestLength<<=1U;

				// Should NEVER happen
				if (!uTestLength) {
					Free(pWBuffer);
					pWBuffer = NULL;
					break;
				}

				// Memory failure?
				pWBuffer = static_cast<WCHAR *>(Realloc(pWBuffer,uTestLength*2));
				if (!pWBuffer) {
					break;
				}
				// Try to get the pathname
				uLength = GetModuleFileNameW(NULL,pWBuffer,uTestLength);
			} while (uLength>=uTestLength);

		} else {
			// Use the internal buffer
			pWBuffer = Buffer;
		}

		if (pWBuffer) {
			// Convert to UTF8
			String UTF8(static_cast<const uint16_t *>(static_cast<void *>(pWBuffer)));
			SetFromNative(UTF8.GetPtr());
			// Release the buffer
			if (pWBuffer!=Buffer) {
				Free(pWBuffer);
			}
			// Remove the :foo.exe extension
			DirName();
		}
	}
}

/***************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are
	local to the machine is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetMachinePrefsDirectory(void)
{
	Clear();

	// Try the code for Vista or higher
	uint16_t *pResult = NULL;
	uint_t uResult = Windows::SHGetKnownFolderPath(&FOLDERID_LocalAppData,KF_FLAG_DONT_UNEXPAND|KF_FLAG_DONT_VERIFY,NULL,&pResult);
	if (uResult==S_OK) {
		// All good! Use this!
		String UTF8(pResult);
		SetFromNative(UTF8.GetPtr());
		// Release the pointer
		CoTaskMemFree(pResult);
	} else {

		// Try it for Windows XP instead
		WCHAR NameBuffer[MAX_PATH];
		// Application system data folder (Local for Vista and Win7)
		uResult = static_cast<uint_t>(SHGetFolderPathW(NULL,CSIDL_LOCAL_APPDATA,NULL,0,NameBuffer));
		if ((uResult==S_OK) || (uResult==E_FAIL)) {
			// Convert to UTF8
			String MyName2(reinterpret_cast<const uint16_t*>(NameBuffer));
			SetFromNative(MyName2);
		}
	}
}


/***************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that
	could be shared among all machines the user has an account
	with is located. The path is converted
	into UTF8 character encoding and stored in Burgerlib
	filename format.

	On platforms where a current working directory doesn't make sense,
	like an ROM based system, the filename is cleared out.

***************************************/

void BURGER_API Burger::Filename::SetUserPrefsDirectory(void)
{
	Clear();
	uint16_t *pResult = NULL;
	uint_t uResult = Windows::SHGetKnownFolderPath(&FOLDERID_RoamingAppData,KF_FLAG_DONT_UNEXPAND|KF_FLAG_DONT_VERIFY,NULL,&pResult);
	if (uResult==S_OK) {
		// All good! Use this!
		String UTF8(pResult);
		SetFromNative(UTF8.GetPtr());
		// Release the pointer
		CoTaskMemFree(pResult);
	} else {

		// Try it for Windows XP instead
		WCHAR NameBuffer[MAX_PATH];
		// Application data folder (Roaming for Vista and Win7)
		uResult = static_cast<uint_t>(SHGetFolderPathW(NULL,CSIDL_APPDATA,NULL,0,NameBuffer));
		if ((uResult==S_OK) || (uResult==E_FAIL)) {
			// Convert to UTF8
			String MyName(reinterpret_cast<const uint16_t*>(NameBuffer));
			SetFromNative(MyName);
		}
	}
}



/***************************************

	Convert a Windows path to a Burgerlib path
	
	Paths without a leading '\' are prefixed with
	the current working directory
	
	Paths with a drive letter but no leading \ will use
	the drive's current working directory
	
	If it's a network path "\\" then use that as the volume name.
	
	The Windows version converts these types of paths:

	"C:\foo\bar2" = ".D2:foo:bar2:"<br>
	"foo" = "(working directory from 8):foo:"<br>
	"foo\bar2" = "(working directory from 8):foo:bar2:"<br>
	"\foo" = ".D(Mounted drive number):foo:"<br>
	"\\foo\bar\file.txt" = ":foo:bar:file.txt:"
	
***************************************/

void BURGER_API Burger::Filename::SetFromNative(const char *pInput)
{
	Clear();

	if (!pInput || !pInput[0]) {	// No directory at all?
		pInput = ".";				// Just get the current directory
	}
	
	// First thing, convert it to UTF16
	
	WCHAR InputPath[512];
	WCHAR *pInputPath;
	uintptr_t uInputLength = UTF16::FromUTF8(reinterpret_cast<uint16_t *>(InputPath),sizeof(InputPath),pInput);	
	if (uInputLength>=sizeof(InputPath)) {
		pInputPath = static_cast<WCHAR *>(Alloc(uInputLength+2));
		if (!pInputPath) {
			return;
		}
		uInputLength = UTF16::FromUTF8(reinterpret_cast<uint16_t *>(pInputPath),uInputLength+2,pInput);
	} else {
		pInputPath = InputPath;
	}
	
	// Now that it's UTF16, let's have Windows expand it

	WCHAR ExpandedPath[512];
	WCHAR *pExpanded;
	// Have windows expand it out
	uintptr_t uExpandedLength = GetFullPathNameW(pInputPath,sizeof(ExpandedPath)/2,ExpandedPath,NULL)*2;
	if (uExpandedLength>=sizeof(ExpandedPath)) {
		pExpanded = static_cast<WCHAR *>(Alloc(uExpandedLength+2));
		if (pExpanded) {
			uExpandedLength = GetFullPathNameW(pInputPath,static_cast<DWORD>((uExpandedLength+2)/2),pExpanded,NULL);
		}
	} else {
		pExpanded = ExpandedPath;
	}

	// I don't need the original anymore
	if (pInputPath!=InputPath) {
		Free(pInputPath);
	}
	// Was there a memory error above?
	if (!pExpanded) {
		return;		// Fudge
	}
	
	// How long would the string be if it was UTF8?
	
	uintptr_t uOutputLength = UTF8::FromUTF16(NULL,0,reinterpret_cast<uint16_t*>(pExpanded))+6;
	char *pWork = m_Filename;
	if (uOutputLength>=sizeof(m_Filename)) {
		pWork = static_cast<char *>(Alloc(uOutputLength));
		if (!pWork) {
			if (pExpanded!=ExpandedPath) {
				Free(pExpanded);
			}
			return;
		}
	}
	m_pFilename = pWork;

	WCHAR *pSrc = pExpanded;
	char *pOutput = pWork;

	// Network name?
	if ((uExpandedLength>=(2*2)) && pSrc[0]=='\\' && pSrc[1]=='\\') {
		pOutput[0] = ':';	// Leading colon 
		++pOutput;			// Accept it
		pSrc+=2;			// Only return 1 colon
	} else {
		uint_t uTemp = static_cast<uint_t>(pSrc[0]);	// Get the drive letter
		if ((uTemp>='a') && (uTemp<('z'+1))) {		// Upper case 
			uTemp &= 0xDF;
		}
		uTemp = uTemp-'A';
		pSrc += 3;			// Accept the "C:\"

	// At this point I have the drive number, create the drive number prefix

		pOutput[0] = '.';	// .D2 for C:
		pOutput[1] = 'D';
		pOutput = NumberToAscii(&pOutput[2],static_cast<uint32_t>(uTemp),NOENDINGNULL);
		pOutput[0] = ':';	// Append a colon
		++pOutput;
	}
	
	// Append the filename to output and convert from UTF16 to UTF8
	UTF8::FromUTF16(pOutput,uOutputLength-(pOutput-pWork),reinterpret_cast<uint16_t*>(pSrc));
	if (pExpanded!=ExpandedPath) {
		Free(pExpanded);
	}

	uint_t uTemp2 = reinterpret_cast<uint8_t *>(pOutput)[0];
	if (uTemp2) {
		do {
			if (uTemp2=='\\') {		// Convert directory holders
				uTemp2 = ':';		// To generic paths
			}
			pOutput[0] = static_cast<char>(uTemp2);	// Save char
			++pOutput;
			uTemp2 = pOutput[0];	// Next char
		} while (uTemp2);			// Still more?
	}
	
	// The wrap up...
	// Make sure it's appended with a colon

	if (pOutput[-1]!=':') {	// Last char a colon?
		pOutput[0] = ':';	// End with a colon!
		pOutput[1] = 0;
	}
}

#endif
