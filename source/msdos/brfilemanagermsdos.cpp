/***************************************

    File Manager Class
    MSDOS Target version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MSDOS)
#include "brfile.h"
#include "brfilename.h"
#include "brdosextender.h"

/***************************************

	Returns true if Win95 is present and long filenames
	are allowed.

***************************************/

uint_t BURGER_API Burger::FileManager::AreLongFilenamesAllowed(void)
{
	uint_t uResult = g_pFileManager->m_bAllowed;
	if (!(uResult&0x80U)) {		// Did I check already?
		uResult = 0x80U;		// Set the "I checked" flag
		Regs16 Regs;
		uint32_t uSeg = GetRealBufferPtr();		// Get real memory buffer
 		if (uSeg) {
			// Do a dos time to file time request
			// If the command executes, then
			// I have long filename support.
			Regs.ax = 0x71A7;					// Ask OS
			Regs.bx = 0x0001;
			Regs.cx = 0x3433;
			Regs.dx = 0x3433;
			Regs.es = static_cast<uint16_t>(uSeg>>16);		// Save the segment
			Regs.di = static_cast<uint16_t>(uSeg);
			Int86x(0x21,&Regs,&Regs);					// Call Win95 with an extended call
			if (!(Regs.flags&1)) {
				++uResult;								// Extended calls are present, long filenames are OK
			}
		}
		// Store the result in the global so I don't have to do this again
		g_pFileManager->m_bAllowed = static_cast<uint8_t>(uResult);
	}
	return uResult&1U;		// Return the flag, True or false
}


/***************************************

	Given a drive number, return in generic format
	the drive's name.

***************************************/

uint_t BURGER_API Burger::FileManager::GetVolumeName(Filename *pOutput,uint_t uVolumeNum)
{
	if (uVolumeNum>=26) {		// Bad drive number!!
		return File::OUTOFRANGE;
	}

	Regs16 Regs;				// Intel registers
	Regs.ax = 0x2F00;			// Get DTA address
	Int86x(0x21,&Regs,&Regs);	// Call DOS
	uint16_t OldOff = Regs.bx;	// Save the old DTA address for later restoration
	uint16_t OldSeg = Regs.es;

	uint32_t uRealBuffer = GetRealBufferPtr();	// Get some real memory
	char *pReal = static_cast<char *>(RealToProtectedPtr(uRealBuffer));	// Convert to true pointer

	Regs.ax = 0x1A00;			// Set the DTA address
	Regs.dx = static_cast<uint16_t>(uRealBuffer);
	Regs.ds = static_cast<uint16_t>(uRealBuffer>>16);
	Int86x(0x21,&Regs,&Regs);	// Call DOS

	// Copy the search string for labels
	StringCopy(pReal+256,"C:\*.*");	
	// Set the drive letter AFTER the fact
	pReal[256] = static_cast<char>('A' + uVolumeNum);	

	Regs.ax = 0x4e00;		// Find first
	Regs.cx = 0x0008;		// Only look for volume labels
	Regs.dx = static_cast<uint16_t>(uRealBuffer+256);	// Pointer to search string
	Regs.ds = static_cast<uint16_t>(uRealBuffer>>16);
	Int86x(0x21,&Regs,&Regs);	// Call DOS
	if (Regs.flags&1) {			// Error (No volume name)
		pReal[30] = 0;
	}

	// Note! The volume name is 30 bytes into the buffer

	pReal[38] = pReal[39];		// Remove the period for an 8.3 filename
	pReal[39] = pReal[40];
	pReal[40] = pReal[41];
	pReal[41] = 0;				// Make SURE it's terminated!
	uRealBuffer = StringLength(pReal+30);	// Size of the string
	if (!uRealBuffer) {
		StringCopy(pReal+30,"UntitledA");		/* Generic */
		pReal[38] = static_cast<char>('A' + uVolumeNum);
		uRealBuffer = 9;
	}
	pReal[29] = ':';
	pReal[uRealBuffer+30] = ':';
	pReal[uRealBuffer+31] = 0;
	if (pOutput) {
		pOutput->Set(pReal+29);
	}

	Regs.ax = 0x1A00;		// Restore the DTA address to the old value
	Regs.ds = OldSeg;
	Regs.dx = OldOff;
	Int86x(0x21,&Regs,&Regs);	// Call DOS
	return File::OKAY;		// Always pass
}

/***************************************

	Set the initial default prefixs for a power up state
	*: = Boot volume
	$: = System folder
	@: = Prefs folder
	8: = Default directory
	9: = Application directory

***************************************/

#if defined(BURGER_WATCOM)
#ifdef __cplusplus
extern "C" {
#endif
extern char **_argv;		/* Used for Intel versions */
#ifdef __cplusplus
}
#endif
#endif

void BURGER_API Burger::FileManager::DefaultPrefixes(void)
{
	Filename MyFilename;
	MyFilename.SetFromNative("");			// Get the current directory
	SetPrefix(8,MyFilename.GetPtr());	// Set the standard work prefix

	MyFilename.SetFromNative(_argv[0]);
	MyFilename.DirName();
	SetPrefix(9,MyFilename.GetPtr());	// Set the application prefix

	SetPrefix(FileManager::PREFIXBOOT,".D2:");		// Assume C: is the boot volume
	SetPrefix(FileManager::PREFIXSYSTEM,"*:DOS");	// C:\DOS
	SetPrefix(FileManager::PREFIXPREFS,"9:");		// Place prefs in the data folder
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

uint32_t DoWorkDOSMod(const char *pReferance);

#pragma aux DoWorkDOSMod = \
	"XOR ECX,ECX"		/* Assume bogus time */ \
	"MOV EAX,03D00H"	/* Open file */ \
	"INT 021H"			/* Call messy dos */ \
	"JC Foo"			/* Oh oh... */ \
	"PUSH EAX"			/* Save the file handle */ \
	"MOV EBX,EAX"		/* Copy to EBX */ \
	"MOV EAX,05700H"	/* Get the file date */ \
	"INT 021H"			/* Call messy dos */ \
	"POP EBX"			/* Restore the file handle */ \
	"JNC Good"			/* Good read? */ \
	"XOR EDX,EDX"		/* Zark the time since it was bad */ \
	"XOR ECX,ECX" \
	"Good:"				/* Reenter */ \
	"SHL EDX,16"		/* Move the date to the upper 16 bits */ \
	"AND ECX,0FFFFH"	/* Mask off the time */ \
	"OR ECX,EDX"		/* Merge into ECX */ \
	"MOV EAX,03E00H"	/* Close the file and dispose of the handle */ \
	"INT 021H" \
	"Foo:"				/* Exit with result in ECX */ \
	parm [edx]			/* Filename in EDX */ \
	modify [eax ebx ecx edx]	/* I blast these */ \
	value [ecx]			/* Return in ECX */

uint_t BURGER_API Burger::FileManager::GetModificationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	uint32_t Temp;

	if (AreLongFilenamesAllowed()) {		/* Win95? */
		Regs16 MyRegs;

	/* This code does NOT work on CD's or Networks */
	/* oh crud... */

#if 0
		MyRegs.ax = 0x7143;	/* Get file attributes */
		MyRegs.bx = 4;		/* Get last modified time */
		MyRegs.cx = 0;		/* Normal access requested */
		Temp = GetRealBufferPtr();	/* Local buffer */
		MyRegs.dx = Temp;			/* Pass the filename buffer */
		MyRegs.ds = (Temp>>16);		/* Get the segment */
		StringCopy(GetRealBufferProtectedPtr(),pFileName->GetNative());
		Int86x(0x21,&MyRegs,&MyRegs);	/* Call Win95 */
		if (MyRegs.flags&1) {			/* Error? */
			goto FooBar;
		}
		Temp = (uint32_t)MyRegs.di;		/* Get the date and time */
		Temp = (Temp<<16)|MyRegs.cx;
#else
		/* This works on all devices */
		uint16_t Ref;
		MyRegs.ax = 0x716C;			/* Open with long filenames */
		MyRegs.bx = 0x0000;			/* Read */
		MyRegs.cx = 0x0000;
		MyRegs.dx = 0x0001;			/* Open the file */
		MyRegs.di = 0x0000;
		Temp = GetRealBufferPtr();	/* Local buffer */
		MyRegs.si = static_cast<uint16_t>(Temp);			/* Pass the filename buffer */
		MyRegs.ds = static_cast<uint16_t>(Temp>>16);		/* Get the segment */
		StringCopy(static_cast<char *>(GetRealBufferProtectedPtr()),pFileName->GetNative());
		Int86x(0x21,&MyRegs,&MyRegs);	/* Call Win95 */
		if (MyRegs.flags&1) {		/* Error? */
			goto FooBar;
		}
		Ref = MyRegs.ax;
		MyRegs.ax = 0x5700;		/* Read access time */
		MyRegs.bx = Ref;
		Int86x(0x21,&MyRegs,&MyRegs);
		Temp = MyRegs.flags;
		MyRegs.bx = Ref;
		MyRegs.ax = 0x3E00;		/* Close the file */
		Int86x(0x21,&MyRegs,&MyRegs);
		if (Temp&1) {			/* Error getting file time? */
			goto FooBar;
		}
		Temp = (uint32_t)MyRegs.dx;		/* Get the date and time */
		Temp = (Temp<<16)|MyRegs.cx;
#endif
	} else {
		Temp = DoWorkDOSMod(pFileName->GetNative());	/* Call DOS to perform the action */
		if (!Temp) {
			goto FooBar;		/* Error? */
		}
	}
	pOutput->LoadMSDOS(Temp);
	return FALSE;
FooBar:
	pOutput->Clear();	// Clear it on error
	return TRUE;		// Error
}

/***************************************

	This routine will get the time and date
	from a file.
	Note, this routine is Operating system specific!!!

***************************************/

uint_t BURGER_API Burger::FileManager::GetCreationTime(Filename *pFileName,TimeDate_t *pOutput)
{
	uint32_t Temp;
	uint_t Result = FALSE;		/* If no dos support then don't return an error */
	if (AreLongFilenamesAllowed()) {		/* Win95? */
		Regs16 MyRegs;
		StringCopy(static_cast<char *>(GetRealBufferProtectedPtr()),pFileName->GetNative());
		MyRegs.ax = 0x7143;	/* Get file attributes */
		MyRegs.bx = 8;		/* Get creation date/time */
		Temp = GetRealBufferPtr();	/* Local buffer */
		MyRegs.dx = static_cast<uint16_t>(Temp);		/* Pass the filename buffer */
		MyRegs.ds = static_cast<uint16_t>(Temp>>16);	/* Get the segment */
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1)) {
			Temp = (uint32_t)MyRegs.di;		/* Get the date and time */
			Temp = (Temp<<16)|MyRegs.cx;
			pOutput->LoadMSDOS(Temp);
			pOutput->m_usMilliseconds = (uint16_t)MyRegs.si;	/* Get milliseconds */
			return FALSE;
		}
		Result = TRUE;		// Error condition
	}
	pOutput->Clear();	// No DOS support
	return Result;		// Error!
}

/***************************************

	Determine if a file exists.
	I will return TRUE if the specified path
	is a path to a file that exists, if it doesn't exist
	or it's a directory, I return FALSE.
	Note : I do not check if the file has any data in it.
	Just the existence of the file.

***************************************/

uint32_t DoWorkDOSExist(const char *Referance);

#pragma aux DoWorkDOSExist = \
	"MOV EAX,04300H"	/* Get file attributes */ \
	"INT 021H"			/* Call messy dos */ \
	"JNC Foo"			/* No error */ \
	"MOV ECX,0x18"		/* Force error */ \
	"Foo:" \
	parm [edx]			/* Filename in EDX */ \
	modify [eax ecx edx]	/* I blast these */ \
	value [ecx]			/* Return in ECX */

uint_t BURGER_API Burger::FileManager::DoesFileExist(Filename *pFileName)
{
	if (AreLongFilenamesAllowed()) {	/* Win95? */
		Regs16 MyRegs;
		MyRegs.ax = 0x7143;		/* Get file attributes */
		MyRegs.bx = 0;			/* Get file attributes only */
		uint32_t Temp = GetRealBufferPtr();	/* Local buffer */
		MyRegs.dx = static_cast<uint16_t>(Temp);			/* Pass the filename buffer */
		MyRegs.ds = static_cast<uint16_t>(Temp>>16);		/* Get the segment */
		StringCopy(static_cast<char *>(GetRealBufferProtectedPtr()),pFileName->GetNative());
		Int86x(0x21,&MyRegs,&MyRegs);	/* Call Win95 */
		if (MyRegs.flags&1 || MyRegs.cx&0x18) {		/* Error? Or directory? */
			return FALSE;
		}
	} else {
		if (DoWorkDOSExist(pFileName->GetNative())&0x18) {		/* Call DOS to perform the action */
			return FALSE;		/* Error? */
		}
	}
	return TRUE;		/* File was found */
}

/***************************************

	Delete a file using native file system

***************************************/

uint_t BURGER_API Burger::FileManager::DeleteFile(Filename *pFileName)
{
	Regs16 Regs;		// Used by DOS

	uint_t LongOk = AreLongFilenamesAllowed();
	uint32_t RealBuffer = GetRealBufferPtr();	/* Get real memory */
	StringCopy(static_cast<char *>(RealToProtectedPtr(RealBuffer)),pFileName->GetNative());	// Copy path

	if (LongOk) {
		Regs.ax = 0x7141;		// Try it via windows
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer>>16);
		Regs.cx = 0;			// Normal file
		Regs.si = 0;			// No wildcards are present
		Int86x(0x21,&Regs,&Regs);	// Delete the file
		if (!(Regs.flags&1)) {		// Error?
			return FALSE;
		}
	}
	Regs.ax = 0x4100;			// Try it the DOS 5.0 way
	Regs.dx = static_cast<uint16_t>(RealBuffer);
	Regs.ds = static_cast<uint16_t>(RealBuffer>>16);
	Int86x(0x21,&Regs,&Regs);
	if (Regs.flags&1) {		// Error?
		return TRUE;		// Oh forget it!!!
	}
	return FALSE;		// Success!!
}

/***************************************

	Change a directory using long filenames
	This only accepts Native OS filenames

***************************************/

uint_t BURGER_API Burger::FileManager::ChangeOSDirectory(Filename *pDirName)
{
	Regs16 Regs;		// Used by DOS

	// Flag for long filenames
	uint_t LongOk = AreLongFilenamesAllowed();
	uint32_t RealBuffer = GetRealBufferPtr();	// Get real memory
	// Copy path
	StringCopy(static_cast<char *>(RealToProtectedPtr(RealBuffer)),pDirName->GetNative());

	if (LongOk) {					// Win95 is present?
		Regs.ax = 0x713B;			// Try it via windows
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer>>16);
		Int86x(0x21,&Regs,&Regs);	// Change the directory
		if (!(Regs.flags&1)) {		// Error?
			return 0;
		}
	}
	Regs.ax = 0x3B00;		// Try it the DOS 5.0 way
	Regs.dx = static_cast<uint16_t>(RealBuffer);
	Regs.ds = static_cast<uint16_t>(RealBuffer>>16);
	Int86x(0x21,&Regs,&Regs);
	if (Regs.flags&1) {		// Error?
		return (uint_t)-1;	// Oh forget it!!!
	}
	return 0;				// Success!!
}

/***************************************

	Create a directory path using an operating system native name
	Return FALSE if successful, or TRUE if an error

***************************************/

uint_t DoWorkDOSCrDir(const char *Referance);

#pragma aux DoWorkDOSCrDir = \
	"MOV EAX,03900H"	/* Create directory */ \
	"PUSH EDX"			/* Save the filename pointer */ \
	"INT 021H"			/* Call DOS */ \
	"POP EDX"			/* Restore the filename */ \
	"JNC Good"			/* Excellent!! */ \
	"MOV EAX,04300H"	/* Get the file attributes */ \
	"INT 021H"			/* Call DOS */ \
	"JC Bad"			/* File not found!! */ \
	"TEST ECX,010H"		/* Is this a preexisting directory? */ \
	"JNZ Good"			/* Excellent! */ \
	"Bad:" \
	"MOV EAX,EAX"		/* Error */ \
	"JMP Foo"			/* Exit */ \
	"Good:" \
	"XOR EAX,EAX"		/* No error */ \
	"Foo:" \
	parm [edx]			/* Filename in EDX */ \
	modify [eax ecx edx]	/* I blast these */ \
	value [eax]			/* Return in EAX */

static uint_t BURGER_API DirCreate(const char *pFileName)
{
	if (Burger::FileManager::AreLongFilenamesAllowed()) {
		Burger::Regs16 MyRegs;
		MyRegs.ax = 0x7139;		/* Create long filename version */
		uint32_t Temp = GetRealBufferPtr();
		MyRegs.dx = static_cast<uint16_t>(Temp);		/* Save the real memory pointer */
		MyRegs.ds = static_cast<uint16_t>(Temp>>16);
		Burger::StringCopy(static_cast<char *>(GetRealBufferProtectedPtr()),pFileName);
		Int86x(0x21,&MyRegs,&MyRegs);	/* Make the directory */
		if (!(MyRegs.flags&1)) {
			return FALSE;
		}
		MyRegs.ax = 0x7143;
		MyRegs.bx = 0;			/* Get attributes */
		MyRegs.dx = static_cast<uint16_t>(Temp);
		MyRegs.ds = static_cast<uint16_t>(Temp>>16);
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1) && MyRegs.cx & 0x10) {	/* Directory here? */
			return FALSE;		/* Directory already present */
		}
		return TRUE;		/* Error! */
	}
	return DoWorkDOSCrDir(pFileName);		/* Dos 5.0 or previous */
}

uint_t BURGER_API Burger::FileManager::CreateDirectoryPath(Filename *pFileName)
{
	char *pPath = const_cast<char *>(pFileName->GetNative());
	if (!DirCreate(pPath)) {		/* Easy way! */
		return FALSE;				/* No error */
	}
	/* Ok see if I can create the directory tree */
	if (pPath[0]) {			/* Is there a filename? */
		const char *WorkPtr = pPath;
		if (WorkPtr[0] && WorkPtr[1]==':') {	/* Drive name? */
			WorkPtr+=2;			/* Skip the drive name */
		}
		if (WorkPtr[0] == '\\') {		/* Accept the first slash */
			++WorkPtr;
		}
		char Old;		/* Marker for a filename */
		uint_t Err;		/* Error code */
		do {
			WorkPtr = StringCharacter(WorkPtr,'\\');		/* Skip to the next colon */
			if (!WorkPtr) {			/* No colon found? */
				WorkPtr = StringCharacter(pPath,0);
			}
			Old = WorkPtr[0];		/* Get the previous char */
			((char *)WorkPtr)[0] = 0;		/* End the string */
			Err = DirCreate(pPath);		/* Create the directory */
			((char *)WorkPtr)[0] = Old;		/* Restore the string */
			++WorkPtr;		/* Index past the char */
		} while (Old);		/* Still more string? */
		if (!Err) {			/* Cool!! */
			return FALSE;	/* No error */
		}
	}
	return TRUE;		/* Didn't do it! */
}

#endif
