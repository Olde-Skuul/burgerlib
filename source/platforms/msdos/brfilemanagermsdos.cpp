/***************************************

	File Manager Class: MSDOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
#include "brendian.h"
#include "brfile.h"
#include "brfilename.h"
#include "brmemoryfunctions.h"
#include "brwin437.h"

#include "msdos_structs.h"
#include "msdos_memory.h"

#include <dos.h>

#if !defined(DOXYGEN)

// Addresses in real memory for the Dos Box signature
#define DOSBOX_SIG_REAL 0xF000E00EU
#define DOSBOX_SIG2_REAL 0xF000E061

static const char gDosBoxSig[] =
	"IBM COMPATIBLE 486 BIOS COPYRIGHT The DOSBox Team";
static const char gDosBoxSig2[] = "DOSBox FakeBIOS";

// Look up table to map known MSDos flavors to a name.
// It wasn't part of the API since Microsoft / IBM didn't think anyone would
// clone an operating system. Silly them.

// Entries were found from actual testing and internet searches. Heaven help me.

struct MSDosOEMLookup_t {
	const uint_t m_uOEMNumber;
	const char* m_pDOSName;
};

static const MSDosOEMLookup_t gOEMLookups[] = {{0, "IBM"}, {1, "Compaq"},
	{2, "MS/DOS"}, {4, "AT&T"}, {5, "Zenith"}, {6, "Hewlett Packard"},
	{7, "Groupe Bull"}, {8, "Tandon"}, {9, "AST"}, {0xA, "Asem"},
	{0xB, "Hantarex"}, {0xC, "SystemsLine"}, {0xD, "Packard Bell"},
	{0xE, "Intercomp"}, {0xF, "Unibit"}, {0x10, "Unidata"},
	{0x16, "Digital Equipment"}, {0x23, "Olivetti"},
	{0x28, "Texas Instruments"}, {0x29, "Toshiba"}, {0x33, "Novell 386"},
	{0x34, "MS Multimedia 386"}, {0x35, "MS Multimedia 386"},
	{0x4D, "Hewlett Packard"}, {0x5E, "RxDOS"}, {0x66, "PhysTechSoft"},
	{0x77, "DOSBox"}, {0x78, "Concurrent DOS"}, {0x99, "GenSoft DOS"},
	{0xEE, "DR-DOS"}, {0xEF, "Novell"}, {0xFD, "FreeDOS"}, {0xFF, "MS/DOS"}};

/*! ************************************

	\brief Replacement Abort, Retry, Ignore.

	This int 0x24 replacement handler will always return 3 (Fail) instead of
	asking the user to Abort, Retry, Ignore.

	\msdosonly

	\sa Burger::FileManager or BURGER_MSDOS

***************************************/

static int __far critical_error_handler(
	unsigned /* deverr */, unsigned /*errcode */, unsigned __far* /* devhdr */)
{
	return _HARDERR_FAIL;
}

#endif

/***************************************

	\brief Handle platform specific startup code

	Calls system functions to determine the version, state and several platform
	specific variables to allow the FileManager to run better by pre-caching
	relevant data.

	For MS-DOS, it will determine the version and flavor of MS/DOS this
	application is running. It will also detect DosBox.

	For Linux, it will scan all the mounted volumes for quick access to shared
	volumes.

	For Android and consoles, it will check if there are mounted SD Cards or
	other external data storage devices so the application can be aware of them.

	\sa platform_shutdown() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_setup(void) BURGER_NOEXCEPT
{
	// Disable Abort, Retry, Ignore
	_harderr(critical_error_handler);

	// Needed for INT 0x21 calls

	MSDos::Regs16_t Regs;

	// Obtain the DOS version number, set bx to prevent alternate resident
	// programs from intercepting
	Regs.ax = 0x3000U;
	Regs.bx = 0x0000U;
	MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);

	// Test is the major version is zero, this happens for DOS 1.0 which
	// doesn't implement this function. All other DOS versions return the
	// number in major in al and minor in ah.
	// http://www.ctyme.com/intr/rb-2711.htm
	uint16_t uVersion = Regs.ax;
	uint_t uOEMFlavor;
	if (!(uVersion & 0xFFU)) {
		// Geez, this version is old.
		uVersion = 0x100U;
		uOEMFlavor = 0;
	} else {
		uVersion = BigEndian::load(uVersion);
		uOEMFlavor = Regs.bx >> 8U;
	}

	// FreeDOS doesn't set the minor version (Bug)
	// Ensure the minor version is stable
	if (uOEMFlavor == 0xFDU) {
		uVersion &= 0xFF00U;
	}

	// Test for DosBox, since it reports as MSDos
	if (uOEMFlavor == 255U) {
		// Let's make SURE it's really Microsoft and not DosBox
		if (!MemoryCompare(MSDos::real_to_protected(DOSBOX_SIG_REAL), gDosBoxSig,
				sizeof(gDosBoxSig) - 1) ||
			!MemoryCompare(MSDos::real_to_protected(DOSBOX_SIG2_REAL), gDosBoxSig2,
				sizeof(gDosBoxSig2) - 1)) {
			// It's really DosBox
			uOEMFlavor = 0x77;
		}
	}

	// Save the stated version and the OEM flavor value.
	m_uMSDOSVersion = uVersion;

	// Determine the REAL version of MS/DOS, since the previous version
	// could be changed with the command SETVER
	uint16_t uTrueVersion = uVersion;
	uint8_t bIsNT = FALSE;
	if (uVersion >= 0x500U) {
		// Start with the "Real Version" call
		// http://www.ctyme.com/intr/rb-2730.htm
		// Note, ignore the carry flag, because it's broken on some versions
		// of DOS, the test ah for 0xFF works in all cases.
		Regs.ax = 0x3306U;
		Regs.bx = 0x0000U;
		MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);

		// Did the call work? Do several safety checks because no one can
		// agree on how to implement this function making my life a living
		// hell.
		if (((Regs.ax & 0xFFU) != 0xFFU) && (Regs.bx < (100U * 256U)) &&
			((Regs.bx & 0xFFU) >= 5U)) {
			uTrueVersion = BigEndian::load(&Regs.bx);

			// This detects a flavor of windows NT, XP, 2000
			if ((uTrueVersion & 0xFF) == 50U) {
				bIsNT = TRUE;
			}
		}
	}

	// Let's check for Concurrent DOS
	// http://www.ctyme.com/intr/rb-2919.htm
	Regs.ax = 0x4451;
	MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);
	if (!(Regs.flags & 1U)) {
		switch (static_cast<uint8_t>(Regs.ax)) {
		case 0x32:
			uTrueVersion = 0x0302;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		case 0x41:
			uTrueVersion = 0x0401;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		case 0x50:
			uTrueVersion = 0x0500;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		case 0x60:
			uTrueVersion = 0x0600;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		case 0x62:
			uTrueVersion = 0x0602;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		case 0x66:
			uTrueVersion = 0x0501;
			uOEMFlavor = 0x34; // Multimedia Windows NT
			break;
		case 0x67:
			uTrueVersion = 0x0501;
			uOEMFlavor = 0x78; // Concurrent DOS
			break;
		}
	}

	// Let's check for DR-DOS and OpenDOS
	// http://www.ctyme.com/intr/rb-2920.htm
	Regs.ax = 0x4452;
	MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);
	if (!(Regs.flags & 1U)) {
		switch (static_cast<uint8_t>(Regs.ax)) {
		case 0x41:
			uTrueVersion = 0x0102;
			uOEMFlavor = 0xEE; // DR-DOS
			break;
		case 0x60:
			uTrueVersion = 0x0200;
			uOEMFlavor = 0xEE; // DR-DOS
			break;
		case 0x63:
			uTrueVersion = 0x0329; // 3.41
			uOEMFlavor = 0xEE;     // DR-DOS
			break;
		case 0x64:
			uTrueVersion = 0x032A; // 3.42
			uOEMFlavor = 0xEE;     // DR-DOS
			break;
		case 0x65:
			uTrueVersion = 0x0500;
			uOEMFlavor = 0xEE; // DR-DOS
			break;
		case 0x67:
			uTrueVersion = 0x0600;
			uOEMFlavor = 0xEE; // DR-DOS
			break;
		case 0x71:
			uTrueVersion = 0x0601;
			uOEMFlavor = 0xEE; // DR-DOS
			break;
		case 0x72:
			uTrueVersion = 0x0700;
			uOEMFlavor = 0xEF; // Novell
			break;
		case 0x73:
			uTrueVersion = 0x0701;
			uOEMFlavor = 0xEE; // DR-DOS / OpenDOS
			break;
		default:
			break;
		}
	}

	m_uOEMFlavor = static_cast<uint8_t>(uOEMFlavor);
	m_uMSDOSTrueVersion = uTrueVersion;

	// Look up the OEM name using the flavor value.
	uintptr_t i = BURGER_ARRAYSIZE(gOEMLookups);
	const MSDosOEMLookup_t* pWork = gOEMLookups;
	const char* pDOSName = "Unknown DOS";
	do {
		if (pWork->m_uOEMNumber == uOEMFlavor) {
			pDOSName = pWork->m_pDOSName;
			break;
		}
		++pWork;
	} while (--i);
	m_pDOSName = pDOSName;

	// Test if long file name support is present by calling the DOS time to
	// FILETIME long name function and seeing if it succeeded. If it does,
	// then long filename support is present in this version of MS/DOS. Get
	// real memory buffer
	// http://www.ctyme.com/intr/rb-3218.htm
	uint32_t uSeg = MSDos::get_temp_real_buffer();
	uint8_t bLongNamesAllowed = FALSE;
	if (uSeg) {

		// Do a dos time to file time request, if the command executes, then
		// there is long filename support.

		Regs.ax = 0x71A7; // DOSTIME to FILETIME
		Regs.bx = 0x0001;
		Regs.cx = 0x3433; // Fake time
		Regs.dx = 0x3433;

		// Save the real pointer
		Regs.es = static_cast<uint16_t>(uSeg >> 16);
		Regs.di = static_cast<uint16_t>(uSeg);

		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			// Extended calls are present, long filenames are OK
			bLongNamesAllowed = TRUE;
		}
	}
	// Store the result in the global the test don't have to be done again
	m_bLongNamesAllowed = bLongNamesAllowed;
}

/*! ************************************

	\brief Returns \ref TRUE if long filenames are allowed.

	On most MSDOS file systems, only 8.3 filenames are permitted. Test if
	support for longer than 8.3 filenames is present and if so, return \ref
	TRUE, otherwise return \ref FALSE.

	\note This function returns \ref TRUE on all target platforms except \ref
	BURGER_MSDOS, the MS/DOS target performs a check to see if it's running
	under Windows 95 or higher or RxDOS and if so, Burgerlib will automatically
	support long filenames.

	\sa is_UTF8_file_system(), FileManager, or BURGER_MSDOS

***************************************/

uint_t BURGER_API Burger::FileManager::MSDOS_has_long_filenames(
	void) BURGER_NOEXCEPT
{
	// Return the flag, True or false
	return g_pFileManager->m_bLongNamesAllowed;
}

/*! ************************************

	\brief Returns SETVER version of MS/DOS

	Returns the version of MS/DOS. This can be overridden by the command SETVER
	for compatibility. Use the function MSDOS_get_os_true_version() to get the true
	version.

	\msdosonly

	\sa MSDOS_get_os_true_version() or MSDOS_get_name()

***************************************/

uint_t BURGER_API Burger::FileManager::MSDOS_get_os_version(void) BURGER_NOEXCEPT
{
	// Return the version
	return g_pFileManager->m_uMSDOSVersion;
}

/*! ************************************

	\brief Returns SETVER version of MS/DOS

	Returns the real version of MS/DOS. This cannot be overridden by the command
	SETVER.

	\msdosonly

	\sa MSDOS_get_os_version() or MSDOS_get_name()

***************************************/

uint_t BURGER_API Burger::FileManager::MSDOS_get_os_true_version(
	void) BURGER_NOEXCEPT
{
	// Return the true version
	return g_pFileManager->m_uMSDOSTrueVersion;
}

/*! ************************************

	\brief Returns the name of the version of DOS running.

	Returns a string with the name of the type of MS/DOS running. DOSBox is
	detected and reported as ``DOSBox``.

	Example strings:
	- IBM
	- Compaq
	- MS/DOS
	- AT&T
	- Zenith
	- Hewlett Packard
	- Groupe Bull
	- Tandon
	- AST
	- Asem
	- Hantarex
	- SystemsLine
	- Packard Bell
	- Intercomp
	- Unibit
	- Unidata
	- Digital Equipment
	- Olivetti
	- Texas Instruments
	- Toshiba
	- Novell 386
	- MS Multimedia 386
	- RxDOS
	- PhysTechSoft
	- DOSBox
	- Concurrent DOS
	- GenSoft DOS
	- DR-DOS
	- Novell

	\msdosonly

	\sa MSDOS_get_flavor()

***************************************/

const char* BURGER_API Burger::FileManager::MSDOS_get_name(void) BURGER_NOEXCEPT
{
	// Return the flag, True or false
	return g_pFileManager->m_pDOSName;
}

/*! ************************************

	\brief Returns the OEM flavor of MS/DOS

	Returns the OEM flavor value for MS/DOS to determine the vendor of the
	version of the OS running.

	\msdosonly

	\sa MSDOS_get_name()

***************************************/

uint_t BURGER_API Burger::FileManager::MSDOS_get_flavor(void) BURGER_NOEXCEPT
{
	return g_pFileManager->m_uOEMFlavor;
}

/*! ************************************

	\brief Convert 8.3 MSDos filename to long version

	On versions of MS/DOS that supports long filenames, convert an input 8.3
	filename into its long filename counterpart if possible. If the operating
	system does not support long filenames, nothing is done to the input string.

	\msdosonly

	\param pInput Pointer to String with the 8.3 filename in MS/DOS format

	\return Error code, kErrorNone if no error.

	\sa MSDOS_get_name() or MSDOS_convert_to_8_3_filename()

***************************************/

Burger::eError BURGER_API Burger::FileManager::MSDOS_expand_8_3_filename(
	String* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	// If not supported, do nothing.
	if (MSDOS_has_long_filenames()) {

		// Get the shared real buffer pointer and convert to protected
		uint32_t uRealBuffer = MSDos::get_temp_real_buffer();
		char* pRealBuffer = static_cast<char*>(MSDos::real_to_protected(uRealBuffer));

		// Offset into the Disk Transfer shared buffer for the input filename
		const uint32_t kNameOffset = 512;

		// Copy the string to the real buffer
		StringCopy(pRealBuffer, kNameOffset, pInput->c_str());

		// Convert 8.3 filename to long filename
		// http://www.ctyme.com/intr/rb-3208.htm
		MSDos::Regs16_t Regs;
		Regs.ax = 0x7160;
		Regs.cx = 0x8002;
		Regs.si = static_cast<uint16_t>(uRealBuffer);
		Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
		Regs.di = static_cast<uint16_t>(uRealBuffer + kNameOffset);
		Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

		// Was the conversion successful?
		if (!(Regs.flags & 0x1U)) {
			// Update the string to the long version.
			uResult = pInput->assign(pRealBuffer + kNameOffset);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Convert long filename to an MSDos 8.3 filename

	On versions of MS/DOS that supports long filenames, convert an input long
	filename into its 8.3 filename counterpart if possible. If the operating
	system does not support long filenames, nothing is done to the input string.

	\msdosonly

	\param pInput Pointer to String with the long filename in MS/DOS format

	\return Error code, kErrorNone if no error.

	\sa MSDOS_get_name() or MSDOS_expand_8_3_filename()

***************************************/

Burger::eError BURGER_API Burger::FileManager::MSDOS_convert_to_8_3_filename(
	String* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	// If not supported, do nothing.
	if (MSDOS_has_long_filenames()) {

		// Get the shared real buffer pointer and convert to protected
		uint32_t uRealBuffer = MSDos::get_temp_real_buffer();
		char* pRealBuffer = static_cast<char*>(MSDos::real_to_protected(uRealBuffer));

		// Offset into the Disk Transfer shared buffer for the input filename
		const uint32_t kNameOffset = 512;

		// Copy the string to the real buffer
		StringCopy(pRealBuffer, kNameOffset, pInput->c_str());

		// Convert long filename to 8.3 filename
		// http://www.ctyme.com/intr/rb-3207.htm
		MSDos::Regs16_t Regs;
		Regs.ax = 0x7160;
		Regs.cx = 0x8001;
		Regs.si = static_cast<uint16_t>(uRealBuffer);
		Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
		Regs.di = static_cast<uint16_t>(uRealBuffer + kNameOffset);
		Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

		// Was the conversion successful?
		if (!(Regs.flags & 0x1U)) {
			// Update the string to the 8.3 version.
			uResult = pInput->assign(pRealBuffer + kNameOffset);
		}
	}
	return uResult;
}

/***************************************

	\brief Return the name of a drive

	Given a drive number (0-?), return the name of the volume in the format of
	":Volume name:". The function will guarantee the existence of the colons.

	\note This function should be used with caution. Only mounted drives would
	return immediately and if the drive has ejectable media may take a while for
	it to respond to a volume name query.

	\param pOutput A \ref Filename structure to contain the filename (Can be
		\ref nullptr )

	\param uVolumeNum A valid drive number from 0-?? with ?? being the
		maximum number of drives in the system

	\return Zero if no error, non-zero if an error occurred

	\sa get_volume_number(const char *)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_volume_name(
	Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT
{
	// Programming note: Int 0x21 0x714E does NOT return volume names.

	// Bad drive number!!
	if (uVolumeNum >= 26) {
		if (pOutput) {
			pOutput->clear();
		}
		return kErrorInvalidParameter;
	}

	// Offset into the Disk Transfer shared buffer for the input filename
	const uint32_t kNameOffset = 256;

	// Get the shared real buffer pointer and convert to protected
	uint32_t uRealBuffer = MSDos::get_temp_real_buffer();
	char* pRealBuffer = static_cast<char*>(MSDos::real_to_protected(uRealBuffer));

	MSDos::Regs16_t Regs; // Intel registers

	// Check if the drive is enabled before attempting to obtain the label
	pRealBuffer[kNameOffset] = static_cast<char>('A' + uVolumeNum);
	pRealBuffer[kNameOffset + 1] = ':';
	pRealBuffer[kNameOffset + 2] = '\\';
	pRealBuffer[kNameOffset + 3] = '*';
	pRealBuffer[kNameOffset + 4] = 0;

	// Use Parse Filename into FCB
	// http://www.ctyme.com/intr/rb-2685.htm#Table1380
	Regs.ax = 0x2900;
	Regs.si = static_cast<uint16_t>(uRealBuffer + kNameOffset);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
	Regs.di = static_cast<uint16_t>(uRealBuffer);
	Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	uint_t bAbort = FALSE;

	// The drive letter is invalid. Return bogus name and error out.
	if ((Regs.ax & 0xFFU) == 0xFFU) {
		bAbort = TRUE;
	}

	// Special case, if a non-existant floppy drive is accessed, then it's
	// possible MSDos 6.22 and others will crash when queried for a volume name.
	// To prevent the crash, check for if a floppy device is present and abort
	// if one is not found.

	// DOS Box doesn't have this issue, so skip the check
	else if ((MSDOS_get_flavor() != 0x77) && (uVolumeNum < 2)) {

		// Query the BIOS if there are floppy drives present.
		// http://www.ctyme.com/intr/rb-0575.htm
		Regs.ax = 0x0000;
		MSDos::real_mode_interrupt(0x11, &Regs, &Regs);

		// If no floppies, don't allow drive 0 or 1
		if (!(Regs.ax & 1)) {
			bAbort = TRUE;
		} else {
			// Get the drive count 0-3 and check against volume number
			if (((Regs.ax >> 6U) & 0x3U) < uVolumeNum) {
				bAbort = TRUE;

			} else if (uVolumeNum == 1) {
				// Just because it reported 2 drives, is it really?
				// Check if drive B: is a phantom drive.

				// http://www.ctyme.com/intr/rb-2907.htm
				Regs.ax = 0x440E;
				Regs.bx = static_cast<uint16_t>(uVolumeNum + 1);
				MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
				if ((Regs.flags & 1) || !(Regs.ax & 0xFFU)) {
					bAbort = TRUE;
				}
			}
		}
	}

	// If the drive was found missing, abort
	if (bAbort) {
		if (pOutput) {
			pOutput->clear();
		}
		return kErrorVolumeNotFound;
	}

	// Get the Disk Transfer address and make a copy
	// http://www.ctyme.com/intr/rb-2710.htm
	Regs.ax = 0x2F00;
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
	uint16_t uOldOffset = Regs.bx;
	uint16_t uOldSegment = Regs.es;

	// Set the disk transfer address to my buffer
	// http://www.ctyme.com/intr/rb-2589.htm
	Regs.ax = 0x1A00;
	Regs.dx = static_cast<uint16_t>(uRealBuffer);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	// Copy the search string for labels
	StringCopy(pRealBuffer + kNameOffset, "C:\\*");
	// Set the drive letter AFTER the fact
	pRealBuffer[kNameOffset] = static_cast<char>('A' + uVolumeNum);

	// Find first matching file
	// http://www.ctyme.com/intr/rb-2977.htm
	Regs.ax = 0x4E00;

	// Only look for volume labels
	// http://www.ctyme.com/intr/rb-2803.htm
	Regs.cx = 0x0008;

	// Pointer to search string
	Regs.dx = static_cast<uint16_t>(uRealBuffer + kNameOffset);
	Regs.ds = static_cast<uint16_t>((uRealBuffer + kNameOffset) >> 16);
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	if (Regs.flags & 1) {
		pRealBuffer[30] = 0;
	} else {
		// Note! The volume name is 30 bytes into the buffer
		// Remove the period for an 8.3 filename
		pRealBuffer[38] = pRealBuffer[39];
		pRealBuffer[39] = pRealBuffer[40];
		pRealBuffer[40] = pRealBuffer[41];
		// Make SURE it's terminated!
		pRealBuffer[41] = 0;
	}

	// Size of the string
	uintptr_t uLength = StringLength(pRealBuffer + 30);
	if (!uLength) {
		// Generic disk name
		StringCopy(pRealBuffer + 30, "C_DRIVE");
		pRealBuffer[30] = static_cast<char>('A' + uVolumeNum);
		uLength = 7;
	}
	pRealBuffer[29] = ':';
	pRealBuffer[uLength + 30] = ':';
	pRealBuffer[uLength + 31] = 0;
	if (pOutput) {
		String Temp;
		Temp.assign_win437(pRealBuffer + 29);
		pOutput->assign(Temp.c_str());
	}

	// Restore the disk transfer address address to the old value
	// http://www.ctyme.com/intr/rb-2589.htm
	Regs.ax = 0x1A00;
	Regs.dx = uOldOffset;
	Regs.ds = uOldSegment;
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	// We are done.
	return kErrorNone;
}

/***************************************

	\brief Get a file's last modification time.

	Given a BurgerLib path, access the file and return the time/date that it was
	last modified.

	\param pFileName Pointer to a "C" string to a BurgerLib pathname
	\param pOutput Pointer to an uninitialized TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_modification_time(Filename*, TimeDate_t*)

***************************************/

uint32_t DoWorkDOSMod(const char* pReferance);

// clang-format off

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

// clang-format on

Burger::eError BURGER_API Burger::FileManager::get_modification_time(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	uint32_t Temp;

	if (MSDOS_has_long_filenames()) { /* Win95? */
		MSDos::Regs16_t MyRegs;

		/* This code does NOT work on CD's or Networks */
		/* oh crud... */

#if 0
		MyRegs.ax = 0x7143;	/* Get file attributes */
		MyRegs.bx = 4;		/* Get last modified time */
		MyRegs.cx = 0;		/* Normal access requested */
		Temp = MSDos::get_temp_real_buffer();	/* Local buffer */
		MyRegs.dx = Temp;			/* Pass the filename buffer */
		MyRegs.ds = (Temp>>16);		/* Get the segment */
		StringCopy(MSDos::get_temp_protected_buffer(),pFileName->get_native());
		MSDos::real_mode_interrupt(0x21,&MyRegs,&MyRegs);	/* Call Win95 */
		if (MyRegs.flags & 1) {			/* Error? */
			goto FooBar;
		}
		Temp = (uint32_t)MyRegs.di;		/* Get the date and time */
		Temp = (Temp << 16) | MyRegs.cx;
#else
		/* This works on all devices */
		uint16_t Ref;
		MyRegs.ax = 0x716C; /* Open with long filenames */
		MyRegs.bx = 0x0000; /* Read */
		MyRegs.cx = 0x0000;
		MyRegs.dx = 0x0001; /* Open the file */
		MyRegs.di = 0x0000;

		/* Local buffer */
		Temp = MSDos::get_temp_real_buffer();

		/* Pass the filename buffer */
		MyRegs.si = static_cast<uint16_t>(Temp);
		/* Get the segment */
		MyRegs.ds = static_cast<uint16_t>(Temp >> 16);

		Win437::translate_from_UTF8(
			static_cast<char*>(MSDos::get_temp_protected_buffer()), 512,
			pFileName->get_native());

		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs); /* Call Win95 */
		if (MyRegs.flags & 1) {         /* Error? */
			goto FooBar;
		}
		Ref = MyRegs.ax;
		MyRegs.ax = 0x5700; /* Read access time */
		MyRegs.bx = Ref;
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		Temp = MyRegs.flags;
		MyRegs.bx = Ref;
		MyRegs.ax = 0x3E00; /* Close the file */
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (Temp & 1) { /* Error getting file time? */
			goto FooBar;
		}
		Temp = (uint32_t)MyRegs.dx; /* Get the date and time */
		Temp = (Temp << 16) | MyRegs.cx;
#endif
	} else {
		/* Call DOS to perform the action */
		Win437::translate_from_UTF8(
			static_cast<char*>(MSDos::get_temp_protected_buffer()), 512,
			pFileName->get_native());

		Temp = DoWorkDOSMod(static_cast<char*>(MSDos::get_temp_protected_buffer()));
		if (!Temp) {
			goto FooBar; /* Error? */
		}
	}
	pOutput->LoadMSDOS(Temp);
	return kErrorNone;
FooBar:
	pOutput->Clear();          // Clear it on error
	return kErrorWriteFailure; // Error
}

/***************************************

	\brief Get a file's creation time.

	Given a native operating system path, access the file and return the
	time/date that it was created.

	\param pFileName Pointer to a \ref Filename class that is properly
		initialized to the native file system.
	\param pOutput Pointer to an uninitialized \ref TimeDate_t structure that
		will receive the time/date.

	\return Zero on success, non-zero in the event of an error (Usually file not
		found)

	\sa get_modification_time(Filename*, TimeDate_t*) or get_creation_time(
		const char*, TimeDate_t*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_creation_time(
	Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	// If no dos support then don't return an error
	uint32_t Temp;
	eError uResult = kErrorNone;

	// Win95?
	if (MSDOS_has_long_filenames()) {
		MSDos::Regs16_t MyRegs;
		Win437::translate_from_UTF8(
			static_cast<char*>(MSDos::get_temp_protected_buffer()), 512,
			pFileName->get_native());
		MyRegs.ax = 0x7143;                      /* Get file attributes */
		MyRegs.bx = 8;                           /* Get creation date/time */
		Temp = MSDos::get_temp_real_buffer();               /* Local buffer */
		MyRegs.dx = static_cast<uint16_t>(Temp); /* Pass the filename buffer */
		MyRegs.ds = static_cast<uint16_t>(Temp >> 16); /* Get the segment */
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (!(MyRegs.flags & 1)) {
			Temp = (uint32_t)MyRegs.di; /* Get the date and time */
			Temp = (Temp << 16) | MyRegs.cx;
			pOutput->LoadMSDOS(Temp);
			pOutput->m_usMilliseconds =
				(uint16_t)MyRegs.si; /* Get milliseconds */
			return kErrorNone;
		}
		uResult = kErrorReadFailure; // Error condition
	} else {
		if (!does_file_exist(pFileName)) {
			uResult = kErrorFileNotFound;
		}
	}
	pOutput->Clear(); // No DOS support
	return uResult;   // Error!
}

/***************************************

	\brief Detect for a file's existence using a native pathname.

	Given a OS native pathname, return \ref TRUE if the file exists. \ref FALSE
	if there is an disk error or the file does not exist.

	\note This does not guarantee that the file can be opened due to security
		permissions or the file being on a locked folder. It only guarantees its
		existence.

	\param pFileName Pointer to a \ref Filename which contains a native
		version of the path to the file.

	\return \ref FALSE if the file does not exist, \ref TRUE if the file exists.

	\sa does_file_exist(const char*)

***************************************/

uint32_t DoWorkDOSExist(const char* Referance);

// clang-format off
#pragma aux DoWorkDOSExist = \
	"MOV EAX,04300H"	/* Get file attributes */ \
	"INT 021H"			/* Call messy dos */ \
	"JNC Foo"			/* No error */ \
	"MOV ECX,0x18"		/* Force error */ \
	"Foo:" \
	parm [edx]			/* Filename in EDX */ \
	modify [eax ecx edx]	/* I blast these */ \
	value [ecx]			/* Return in ECX */
// clang-format on

uint_t BURGER_API Burger::FileManager::does_file_exist(
	Filename* pFileName) BURGER_NOEXCEPT
{
	if (MSDOS_has_long_filenames()) { /* Win95? */
		MSDos::Regs16_t MyRegs;
		MyRegs.ax = 0x7143;                      /* Get file attributes */
		MyRegs.bx = 0;                           /* Get file attributes only */
		uint32_t Temp = MSDos::get_temp_real_buffer();      /* Local buffer */
		MyRegs.dx = static_cast<uint16_t>(Temp); /* Pass the filename buffer */
		MyRegs.ds = static_cast<uint16_t>(Temp >> 16); /* Get the segment */
		Win437::translate_from_UTF8(
			static_cast<char*>(MSDos::get_temp_protected_buffer()), 512,
			pFileName->get_native());
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);             /* Call Win95 */
		if (MyRegs.flags & 1 || MyRegs.cx & 0x18) { /* Error? Or directory? */
			return FALSE;
		}
	} else {
		Win437::translate_from_UTF8(
			static_cast<char*>(MSDos::get_temp_protected_buffer()), 512,
			pFileName->get_native());
		/* Call DOS to perform the action */
		if (DoWorkDOSExist(static_cast<char*>(MSDos::get_temp_protected_buffer())) &
			0x18) {
			/* Error? */
			return FALSE;
		}
	}
	return TRUE; /* File was found */
}

/***************************************

	\brief Delete a file or empty directory using a \ref Filename.

	Given an OS native pathname, delete the file at the end of the pathname.

	Example:

	If a call with "c:\Foo\Bar\Fooey\Temp.txt" is issued, only the file
	"Temp.txt" is deleted. The rest of the path is left intact. This call will
	not delete non-empty directories, only files. If the file is already
	deleted, an error is returned.

	\param pFileName Pointer to a \ref Filename of a Burgerlib path.

	\return Zero if successful, non-zero on error.

	\sa delete_file(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::delete_file(
	Filename* pFileName) BURGER_NOEXCEPT
{
	// Used by DOS
	MSDos::Regs16_t Regs;

	// Get real memory
	uint32_t RealBuffer = MSDos::get_temp_real_buffer();

	// Copy path
	Win437::translate_from_UTF8(
		static_cast<char*>(MSDos::real_to_protected(RealBuffer)), 512,
		pFileName->get_native());

	if (MSDOS_has_long_filenames()) {
		// Try it via windows
		// http://www.ctyme.com/intr/rb-3200.htm
		Regs.ax = 0x7141;
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
		Regs.cx = 0;                // Normal file
		Regs.si = 0;                // No wildcards are present
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs); // Delete the file
		if (!(Regs.flags & 1)) {    // Error?
			return kErrorNone;
		}

		// http://www.ctyme.com/intr/rb-3198.htm
		Regs.ax = 0x713A;
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs); // Delete the directory
		if (!(Regs.flags & 1)) {    // Error?
			return kErrorNone;
		}
		return kErrorFileNotFound;
	}

	// http://www.ctyme.com/intr/rb-2797.htm
	Regs.ax = 0x4100; // Try it the DOS 5.0 way
	Regs.dx = static_cast<uint16_t>(RealBuffer);
	Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	// Error?
	if (Regs.flags & 1) {

		// Try deleting as a directory
		// http://www.ctyme.com/intr/rb-2776.htm
		Regs.ax = 0x3A00;
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (Regs.flags & 1) {
			// Oh forget it!!!
			return kErrorFileNotFound;
		}
	}
	// Success!!
	return kErrorNone; 
}

/***************************************

	\brief Change the OS working directory using an OS native pathname..

	Given an OS native pathname, set the OS's current working directory to this
	path.

	This function is useful in cases where an OS native file dialog to select a
	file or perform an OS native function where the current working directory
	needs to be preset. This function under most circumstances is not necessary
	to use, however, it exists since there are some OS native functions in MacOS
	and Windows that require a little "help".

	\note On most systems, this is the same as calling cwd().

	\param pDirName Pointer to a \ref Filename of a native OS path.

	\return Zero if successful, non-zero on error.

	\sa change_OS_directory(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::change_OS_directory(
	Filename* pDirName)
{
	MSDos::Regs16_t Regs; // Used by DOS

	// Flag for long filenames
	uint_t LongOk = MSDOS_has_long_filenames();
	uint32_t RealBuffer = MSDos::get_temp_real_buffer(); // Get real memory
	// Copy path
	Win437::translate_from_UTF8(
		static_cast<char*>(MSDos::real_to_protected(RealBuffer)), 512,
		pDirName->get_native());

	if (LongOk) {         // Win95 is present?
		Regs.ax = 0x713B; // Try it via windows
		Regs.dx = static_cast<uint16_t>(RealBuffer);
		Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs); // Change the directory
		if (!(Regs.flags & 1)) {    // Error?
			return kErrorNone;
		}
	}
	Regs.ax = 0x3B00; // Try it the DOS 5.0 way
	Regs.dx = static_cast<uint16_t>(RealBuffer);
	Regs.ds = static_cast<uint16_t>(RealBuffer >> 16);
	MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
	if (Regs.flags & 1) { // Error?
		return kErrorIO;  // Oh forget it!!!
	}
	return kErrorNone; // Success!!
}

/***************************************

	\brief Create a directory path.

	Given an Burgerlib directory, create each and every part of the directory.
	If the directory already exists, or is successfully created, the call is
	considered successful.

	Example:

	If the path c:\\Foo\\Bar exists, a call with "c:\Foo\Bar\Fooey\Temp\" will
	create both "Fooey" and "Temp" in this single call.

	\param pFileName Pointer to a \ref Filename that contains a Burgerlib
		pathname.

	\return Zero if successful, non-zero on error.

	\sa create_directory_path(const char*) or
		create_directory_path_dirname(const char*)

***************************************/

uint_t DoWorkDOSCrDir(const char* Referance);

// clang-format off
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
// clang-format on

static uint_t BURGER_API DirCreate(const char* pFileName) BURGER_NOEXCEPT
{
	if (Burger::FileManager::MSDOS_has_long_filenames()) {
		Burger::MSDos::Regs16_t MyRegs;
		MyRegs.ax = 0x7139; /* Create long filename version */
		uint32_t Temp = Burger::MSDos::get_temp_real_buffer();
		MyRegs.dx =
			static_cast<uint16_t>(Temp); /* Save the real memory pointer */
		MyRegs.ds = static_cast<uint16_t>(Temp >> 16);
		Burger::Win437::translate_from_UTF8(
			static_cast<char*>(Burger::MSDos::get_temp_protected_buffer()), 512, pFileName);
		Burger::MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs); /* Make the directory */
		if (!(MyRegs.flags & 1)) {
			return FALSE;
		}
		MyRegs.ax = 0x7143;
		MyRegs.bx = 0; /* Get attributes */
		MyRegs.dx = static_cast<uint16_t>(Temp);
		MyRegs.ds = static_cast<uint16_t>(Temp >> 16);
		Burger::MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (!(MyRegs.flags & 1) && MyRegs.cx & 0x10) { /* Directory here? */
			return FALSE; /* Directory already present */
		}
		return TRUE; /* Error! */
	}
	Burger::Win437::translate_from_UTF8(
		static_cast<char*>(Burger::MSDos::get_temp_protected_buffer()), 512, pFileName);
	// Dos 5.0 or previous
	return DoWorkDOSCrDir(static_cast<char*>(Burger::MSDos::get_temp_protected_buffer()));
}

Burger::eError BURGER_API Burger::FileManager::create_directory_path(
	Filename* pFileName) BURGER_NOEXCEPT
{

	char* pPath = const_cast<char*>(pFileName->get_native());
	if (!DirCreate(pPath)) { /* Easy way! */
		return kErrorNone;   /* No error */
	}
	/* Ok see if I can create the directory tree */
	if (pPath[0]) { /* Is there a filename? */
		const char* WorkPtr = pPath;
		if (WorkPtr[0] && WorkPtr[1] == ':') { /* Drive name? */
			WorkPtr += 2;                      /* Skip the drive name */
		}
		if (WorkPtr[0] == '\\') { /* Accept the first slash */
			++WorkPtr;
		}
		char Old;   /* Marker for a filename */
		uint_t Err; /* Error code */
		do {
			WorkPtr =
				StringCharacter(WorkPtr, '\\'); /* Skip to the next colon */
			if (!WorkPtr) {                     /* No colon found? */
				WorkPtr = StringCharacter(pPath, 0);
			}
			Old = WorkPtr[0];          /* Get the previous char */
			((char*)WorkPtr)[0] = 0;   /* End the string */
			Err = DirCreate(pPath);    /* Create the directory */
			((char*)WorkPtr)[0] = Old; /* Restore the string */
			++WorkPtr;                 /* Index past the char */
		} while (Old);                 /* Still more string? */
		if (!Err) {                    /* Cool!! */
			return kErrorNone;         /* No error */
		}
	}
	return kErrorIO; /* Didn't do it! */
}

#endif
