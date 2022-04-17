/***************************************

	MacOS specific convenience functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmactypes.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brglobals.h"
#include "brmacromanus.h"
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <Devices.h>
#include <DrawSprocket.h>
#include <Folders.h>
#include <Gestalt.h>
#include <HFSVolumes.h>
#include <InputSprocket.h>
#include <LowMem.h>
#include <Navigation.h>
#include <Sound.h>

#if !defined(BURGER_CFM) || !TARGET_API_MAC_CARBON
#include <Traps.h>
#endif

#if defined(BURGER_POWERPC)
#include <Movies.h>
#endif

#if !defined(DOXYGEN)

//
// Table to look up appleshare versions
//

static const uint16_t g_AppleShareVer[] = {0x000, 0x350, 0x360, 0x361, 0x362,
	0x363, 0x370, 0x372, 0x380, 0x381, 0x382, 0x383, 0x384};

#endif

Burger::Mac Burger::Mac::g_Globals;

/*! ************************************

	\brief Detect if a MacOS trap number exists.

	Detect if a specific 680x0 trap instruction is present.

	Due to the nature of this function, caching is impractical and it is not
	performed. This function is considered a performance bottleneck and should
	be called sparingly.

	\note This is a MacOS Classic function. It always returns \ref FALSE on
		Carbon targets.

	\param uTrapNum A valid MacOS 680x0 trap number from Traps.h.

	\return \ref TRUE if the requested trap index is present, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::Mac::IsTrapAvailable(uint_t uTrapNum) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	//
	// Only matters when running 680x0 code or PowerPC code
	// with a 680x0 emulator
	//
#if !TARGET_API_MAC_CARBON
	do {
		if (uTrapNum < 0x10000U) {        // Failsafe
			TrapType MyTrapType = OSTrap; // Assume it's an OS trap!
			if (uTrapNum & 0x0800) {      // What trap is it?
				// Is it in the extended trap table?
				// Check if the extended trap table is even present by checking
				// if _InitGraf and _InitGraf+0x200 map to the same address.

				if (((uTrapNum & 0x03FF) >= 0x0200) &&
					(GetToolboxTrapAddress(_InitGraf) ==
						GetToolboxTrapAddress(_InitGraf + 0x200))) {
					// This trap can't be in the short table. I don't
					// support extended traps.
					break;
				}
				MyTrapType = ToolTrap; // Toolbox trap
			}

			if (NGetTrapAddress(static_cast<UInt16>(uTrapNum), MyTrapType) !=
				GetToolboxTrapAddress(_Unimplemented)) {
				// The trap is present
				uResult = TRUE;
			}
		}
	} while (0);
#else
	BURGER_UNUSED(uTrapNum);
#endif
	// Return the flag if I have the trap or not.
	return uResult;
}

/*! ************************************

	\brief Detect if the PowerPC native version of Quicktime is present.

	Detect if the PowerPC native extension of Quicktime is loaded and available.
	This is only for Quicktime version 2.5 during the initial transition from
	680x0 to PowerPC. It's almost certainly going to return \ref TRUE on PowerPC
	systems. Only in very rare cases of Quicktime 2.5 where it's emulated in
	680x0 code (And therefore a performance bottleneck) in which this matters.

	This function is written so it only does the detection once and it caches
	the result. The cached value is returned on subsequent calls.

	\return \ref TRUE if Quicktime is running in PowerPC code, \ref FALSE if
		emulated 680x0.

	\note This code always returns \ref FALSE on non-PowerPC targets.

***************************************/

uint_t BURGER_API Burger::Mac::IsQuickTimePowerPlugAvailable(
	void) BURGER_NOEXCEPT
{
#if defined(BURGER_POWERPC)
	uint_t bResult = g_Globals.m_bIsQuickTimePlugInTested;
	// Was it already tested?
	if (!(bResult & 0x80)) {
		long gestaltAnswer;
		if (!Gestalt(gestaltQuickTimeFeatures, &gestaltAnswer)) {
			if ((gestaltAnswer & (1 << gestaltPPCQuickTimeLibPresent)) &&
				(EnterMovies != nullptr)) {
				// Quicktime is in PowerPC
				bResult = 0x80 | TRUE;
			} else {
				bResult = 0x80 | FALSE;
			}
		}
		g_Globals.m_bIsQuickTimePlugInTested = static_cast<uint8_t>(bResult);
	}
	// Return the value minus the other flags
	return bResult & 1U;
#else
	// Emulated 68k if false
	return FALSE;
#endif
}

/*! ************************************

	\brief Return the version of Quickdraw.

	Use Gestalt to test for what version of Quickdraw is available.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

uint_t BURGER_API Burger::Mac::GetQuickdrawVersion(void) BURGER_NOEXCEPT
{
	Mac* pGlobals = &g_Globals;
	uint_t uVersion;
	if (!pGlobals->m_bQuickdrawVersionTested) {
		long MyAnswer;

		// Assume failure (Ancient Mac, like Mac 128K)
		uVersion = gestaltOriginalQD;

		// Get the version (Quickdraw always exists)
		if (!Gestalt(gestaltQuickdrawVersion, &MyAnswer)) {
			uVersion = static_cast<uint_t>(MyAnswer);
		}

		pGlobals->m_uQuickdrawVersion = uVersion;
		pGlobals->m_bQuickdrawVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uQuickdrawVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Return the version of AppleShare library.

	Ask the AppleShare library what version it is and return that value. The
	function caches the value so subsequent calls do not ask AppleShare for the
	version again.

	\return Version in the format of 0x0102 -> 1.2

	\note If the AppleShare library is not installed, it will return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetAppleShareVersion(void) BURGER_NOEXCEPT
{
	Mac* pGlobals = &g_Globals;
	uint_t uVersion;
	if (!pGlobals->m_bAppleShareVersionTested) {
		long MyAnswer;
		// Assume failure
		uVersion = 0;
		// Detect Appleshare
		if (!Gestalt(gestaltAFPClient, &MyAnswer)) {
			uint_t uIndex =
				static_cast<uint_t>(MyAnswer & gestaltAFPClientVersionMask);
			if (uIndex > 12) {
				uIndex = 12;
			}
			// Look up the version via a table
			uVersion = g_AppleShareVer[uIndex];
		}
		pGlobals->m_uAppleShareVersion = uVersion;
		pGlobals->m_bAppleShareVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uAppleShareVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Return the version of InputSprocket.

	Detect if InputSprocket is available, and if so, query it for the version
	present. If InputSprocket is not available, the version returned is zero.

	This function is written so it only asks for the version once from
	InputSprocket. It will cache the version and return the cached value on
	subsequent calls.

	\return Version in the format of 0x0102 -> 1.2

	\note This code only works on CFM versions of Mac Classic applications.
		Non-CFM 68k targets will always return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetInputSprocketVersion(void) BURGER_NOEXCEPT
{
// 680x0 CFM and PowerPC have InputSprocket
#if defined(BURGER_CFM)
	Mac* pGlobals = &g_Globals;
	uint_t uVersion;
	if (!pGlobals->m_bInputSprocketVersionTested) {
		// Code even present?
		uVersion = 0;
		if (ISpGetVersion != nullptr) {

			// Call inputsprocket and get the version

			// Don't coalesce, the compiler isn't smart
			// enough for NumVersion = foo();
			NumVersion MyVersion;
			MyVersion = ISpGetVersion();
			uVersion = (MyVersion.majorRev << 8) & 0xFF00U;
			uVersion |= (MyVersion.minorAndBugRev) & 0xFFU;
		}
		pGlobals->m_uInputSprocketVersion = uVersion;
		pGlobals->m_bInputSprocketVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uInputSprocketVersion;
	}
	return uVersion;
#else
	return 0;
#endif
}

/*! ************************************

	\brief Return the version of DrawSprocket.

	Detect if DrawSprocket is available, and if so, query it for the version
	present. If DrawSprocket is not available, the version returned is zero.

	This function is written so it only asks for the version once from
	DrawSprocket. It will cache the version and return the cached value on
	subsequent calls.

	DSpGetVersion() only appeared in version 1.7 or later and as a result, to
	get the version from earlier versions the version resource found in the
	library itself must be opened and read. Because of this, the first call to
	this function may take some time.

	\return Version in the format of 0x0102 -> 1.2

	\note This code only works on CFM versions of Mac Classic applications.
		Non-CFM 68k targets will always return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetDrawSprocketVersion(void) BURGER_NOEXCEPT
{
// 680x0 CFM and PowerPC have DrawSprocket
#if defined(BURGER_CFM)

	static const uint8_t DebugDrawSpocketName[] = "\pDrawSprocketDebugLib";
	static const uint8_t DrawSprocketName[] = "\pDrawSprocketLib";

	Mac* pGlobals = &g_Globals;
	uint_t uVersion;
	if (!pGlobals->m_bDrawSprocketVersionTested) {

		// Can I do it the easy way?

#if !defined(BURGER_68K)
		if (DSpGetVersion == nullptr) {
#endif
			uVersion = 0;
			// I have to do it the hard way. :(

			// If it's version 1.1.4 or earlier, find the extension
			// and manually grab the version resource

			// Get the previous resource load setting
			UInt8 OldResLoad = LMGetResLoad();
			// Get the current resource ref
			short SavedResourceRef = CurResFile();

			// Don't load in all the preloaded resources
			SetResLoad(FALSE);

			// Volume for extensions folder
			short VolumeRef;

			// Directory for extensions folder
			long DirIDRef;

			// Get my current folder
			HGetVol(0, &VolumeRef, &DirIDRef);

			// Is it in my directory? Check for debug version first
			short DrawSprocketResourceRef = HOpenResFile(
				VolumeRef, DirIDRef, DebugDrawSpocketName, fsRdPerm);

			// Error?
			OSErr ErrorCode = ResError();
			// Let's try again with release
			if (ErrorCode == fnfErr) {
				DrawSprocketResourceRef = HOpenResFile(
					VolumeRef, DirIDRef, DrawSprocketName, fsRdPerm);

				// Error?
				ErrorCode = ResError();

				// Try the system folder now
				if (ErrorCode == fnfErr) {

					// At this time, search the extensions folder

					FindFolder(kOnSystemDisk, kExtensionFolderType,
						kDontCreateFolder, &VolumeRef, &DirIDRef);

					// Release build is checked first

					FSSpec DrawSprocketFSSpec;
					FSMakeFSSpec(VolumeRef, DirIDRef, DrawSprocketName,
						&DrawSprocketFSSpec);

					DrawSprocketResourceRef =
						FSpOpenResFile(&DrawSprocketFSSpec, fsRdPerm);
					ErrorCode = ResError();
					if (ErrorCode == fnfErr) {

						// Try the debug build next
						FSMakeFSSpec(VolumeRef, DirIDRef, DebugDrawSpocketName,
							&DrawSprocketFSSpec);
						DrawSprocketResourceRef =
							FSpOpenResFile(&DrawSprocketFSSpec, fsRdPerm);

						// Here is where I give up and call it a day
						ErrorCode = ResError();
					}
				}
			}

			// Do we have a resource open?
			if (!ErrorCode && (DrawSprocketResourceRef != -1)) {

				// Ok, let's actually load something
				SetResLoad(TRUE);

				// Get the version resource
				Handle hVersion = GetResource('vers', 1);

				// Valid?
				if (hVersion && !ResError()) {
					// Deref the handle
					const uint8_t* pWork =
						reinterpret_cast<const uint8_t**>(hVersion)[0];
					// Major version
					uVersion = static_cast<uint_t>(pWork[0]) << 8;
					// Minor version
					uVersion |= pWork[1];
					// Release the version resource
					ReleaseResource(hVersion);
				}
				// Restore the resource file first
				UseResFile(SavedResourceRef);

				// Close the lib's version resource file
				CloseResFile(DrawSprocketResourceRef);
			}
			// Set the resource file to original setting
			UseResFile(SavedResourceRef);
			// Set the ResLoad flag to original setting
			SetResLoad(OldResLoad);

#if !defined(BURGER_68K)
		} else {

			// Let's do it the easy way!

			// Get the version
			NumVersion MyVersion = DSpGetVersion();

			// Merge the version number
			uVersion = (MyVersion.majorRev << 8) & 0xFF00U;
			uVersion |= (MyVersion.minorAndBugRev) & 0xFF;
		}
#endif
		pGlobals->m_uDrawSprocketVersion = uVersion;
		pGlobals->m_bDrawSprocketVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uDrawSprocketVersion;
	}
	return uVersion;

#else
	// Mac 68K Classic doesn't support DrawSprocket
	return 0;
#endif
}

/*! ************************************

	\brief Return the version of the Sound Manager.

	Ask the MacOS Sound Manager what version it is and return that value. The
	function caches the value so subsequent calls do not ask the Sound Manager
	for the version again.

	\return Version in the format of 0x0102 -> 1.2

	\note If the sound manager is not installed (Very old mac models), it will
		return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetSoundManagerVersion(void) BURGER_NOEXCEPT
{
	Mac* pGlobals = &g_Globals; // Get the pointer to the singleton
	if (!pGlobals->m_bSoundManagerVersionValid) {
		pGlobals->m_bSoundManagerVersionValid = TRUE; // I got the version
#if !defined(BURGER_CFM)
		// 68K trap only
		if (GetToolTrapAddress(_SoundDispatch) !=
			GetToolTrapAddress(_Unimplemented))
#else
		if (SndSoundManagerVersion !=
			nullptr) // Check the pointer for CFM 68k or PPC
#endif
		{
			NumVersion MyVersion;
			MyVersion = SndSoundManagerVersion(); // Get the version

			// 8.8 version number
			uint_t uResult = (MyVersion.majorRev << 8) & 0xFF00U;
			uResult |= static_cast<uint_t>(MyVersion.minorAndBugRev & 0xFFU);
			// Store the version in cache
			pGlobals->m_uSoundManagerVersion = uResult;
		}
	}
	return pGlobals->m_uSoundManagerVersion; // Return the version
}

/*! ************************************

	\brief Send a "Quit" event to the requested process

	Send an Apple Event to the process to tell the process to properly quit.

	\param pVictim Pointer to a process serial number of the process to kill

***************************************/

void BURGER_API Burger::Mac::KillProcess(
	ProcessSerialNumber* pVictim) BURGER_NOEXCEPT
{

	// Create a "Quit" event

	AEAddressDesc target;
	if (!AECreateDesc(typeProcessSerialNumber,
			static_cast<Ptr>(static_cast<void*>(pVictim)),
			sizeof(ProcessSerialNumber), &target)) {
		AppleEvent theEvent;
		OSErr err = AECreateAppleEvent('aevt', 'quit', &target,
			kAutoGenerateReturnID, kAnyTransactionID, &theEvent);
		AEDisposeDesc(&target);
		if (!err) {
			// Send the "Quit" event
			AESend(&theEvent, 0, kAENoReply + kAENeverInteract,
				kAENormalPriority, kAEDefaultTimeout, 0, 0);
			AEDisposeDesc(&theEvent);
		}
	}

	// Give some CPU time for the event to trigger

	uint_t uWaits = 7;
	do {
		EventRecord event;
		if (!WaitNextEvent(everyEvent, &event, 180, 0)) {
			break;
		}
	} while (--uWaits);
}

/*! ************************************

	\brief Send a "Quit" event to every other process

	Send a "Quit" event to every other app however, don't kill myself and kill
	the finder last

***************************************/

void BURGER_API Burger::Mac::KillAllProcesses(void) BURGER_NOEXCEPT
{
	// My apps own process number
	ProcessSerialNumber MyAppNumber;

	// Get my current app process number
	GetCurrentProcess(&MyAppNumber);

	// Next process in chain
	ProcessSerialNumber next;
	// Start following the process list
	next.highLongOfPSN = 0;
	next.lowLongOfPSN = kNoProcess;
	GetNextProcess(&next); // Get my number

	ProcessSerialNumber finder;  // Finder process ID
	uint_t bFoundFinder = FALSE; // Found the finder?

	// Found another process?
	if (next.highLongOfPSN || next.lowLongOfPSN != kNoProcess) {
		do {
			ProcessSerialNumber current = next;
			// Preload the NEXT process since I may kill it now
			GetNextProcess(&next);

			Bool bIsFlag = FALSE;
			// Don't kill myself
			SameProcess(&current, &MyAppNumber, &bIsFlag);
			if (!bIsFlag) {

				// If I have found the finder, the rest are easy.
				// Otherwise...

				if (!bFoundFinder) { // Find it already?
					Str31 processName;
					FSSpec procSpec;
					ProcessInfoRec infoRec;

					infoRec.processInfoLength = sizeof(ProcessInfoRec);
					infoRec.processName = (StringPtr)&processName;
					infoRec.processAppSpec = &procSpec;

					// What process is this?
					GetProcessInformation(&current, &infoRec);
					if (infoRec.processSignature == 'MACS' &&
						infoRec.processType == 'FNDR') {
						// This is the finder!
						finder = current;
						bFoundFinder = TRUE;
						bIsFlag = TRUE;
					} else {
						bIsFlag = FALSE;
					}
				} else {

					// You may ask yourself, why check for finder when the
					// finder is already found? The finder has multiple
					// processes!!!

					// Just do a compare
					SameProcess(&current, &finder, &bIsFlag);
				}

				// Is this app not the finder?

				if (!bIsFlag) {
					KillProcess(&current); // Kill it
				}
			}
		} while (next.highLongOfPSN || (next.lowLongOfPSN != kNoProcess));
	}

	// Now, did I locate the finder?

	if (bFoundFinder) {
		KillProcess(&finder); // Bye bye
	}
}

/*! ************************************

	\brief Start DrawSprocket

	If DrawSprocket was not started, test for its existance and initialize the
	library. If DrawSprocket was started successfully or was already started,
	return \ref TRUE, otherwise return \ref FALSE

	\note This call does nothing on non-CFM targets

	\return \ref TRUE on success

***************************************/

uint_t BURGER_API Burger::Mac::StartDrawSprocket(void) BURGER_NOEXCEPT
{
#if !defined(BURGER_CFM)
	uint_t bResult = FALSE;
#else
	Mac* pGlobals = &g_Globals;

	// Did I test for DrawSprocket?
	uint_t bResult = pGlobals->m_bDrawSprocketActive;
	if (!bResult) {

		// Is draw sprocket present?
		if (DSpStartup != nullptr) {
			// Init draw sprocket
			if (!DSpStartup()) {

				// It's ok!
				pGlobals->m_bDrawSprocketActive = TRUE;
				// Initialize the blanking color to black
				// (Some Performa computers don't set this
				// on startup)

				RGBColor Black;
				Black.red = 0;
				Black.green = 0;
				Black.blue = 0;
				DSpSetBlankingColor(&Black);
				bResult = TRUE;
			}
		}
	}
#endif
	// Return TRUE if active
	return bResult;
}

/*! ************************************

	\brief Shut down DrawSprocket

	If DrawSprocket was started, shut it down.

	\note This call does nothing on non-CFM targets

***************************************/

void BURGER_API Burger::Mac::StopDrawSprocket(void) BURGER_NOEXCEPT
{
#if defined(BURGER_CFM)
	Mac* pGlobals = &g_Globals;

	// Already started?
	if (pGlobals->m_bDrawSprocketActive) {
		pGlobals->m_bDrawSprocketActive = FALSE;

		// Bye bye
		DSpShutdown();
	}
#endif
}

/*! ************************************

	\brief Test if the application is running under MacOSX

	Ask MacOS what version it is and if it's 10.0 or higher, return \ref TRUE

	\return \ref TRUE if running under MacOSX, \ref FALSE if MacOS 9 or earlier

***************************************/

uint_t BURGER_API Burger::Mac::IsRunningUnderMacOSX(void) BURGER_NOEXCEPT
{
	return (Globals::GetMacOSVersion() >= 0x1000U);
}

/*! ************************************

	\brief Get the version of NavServices

	Ask NavServices if it exists and return the version.

	\return 32 bit version number or 0 if NavServices is not available.

***************************************/

uint32_t BURGER_API Burger::Mac::GetNavServicesVersion(void) BURGER_NOEXCEPT
{
	Mac* pGlobals = &g_Globals;
	uint32_t uVersion;
	if (!pGlobals->m_bNavServicesVersionTested) {
		// Code even present?
		uVersion = 0;
		if (NavServicesAvailable()) {
			uVersion = NavLibraryVersion();
		}
		pGlobals->m_uNavServicesVersion = uVersion;
		pGlobals->m_bNavServicesVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uNavServicesVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Get the version of ControlStrip

	Ask ControlStrip if it exists and return the version.

	\return 32 bit version number or 0 if NavServices is not available.

***************************************/

uint32_t BURGER_API Burger::Mac::GetControlStripVersion(void) BURGER_NOEXCEPT
{
	Mac* pGlobals = &g_Globals;
	uint32_t uVersion;
	if (!pGlobals->m_bControlStripVersionTested) {
		// Code even present?
		uVersion = 0;
		long gestaltAnswer;
		if (!Gestalt(gestaltControlStripVersion, &gestaltAnswer)) {
			uVersion = static_cast<uint32_t>(gestaltAnswer);
		}
		pGlobals->m_uControlStripVersion = uVersion;
		pGlobals->m_bControlStripVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uControlStripVersion;
	}
	return uVersion;
}

/*! ************************************

	\brief Test for color Quickdraw

	Check the version of quickdraw and if color is supported, return TRUE.

	\return TRUE if color is supported.

***************************************/

uint_t BURGER_API Burger::Mac::HaveColorQuickDraw(void) BURGER_NOEXCEPT
{
	return GetQuickdrawVersion() > gestaltOriginalQD;
}

/*! ************************************

	\brief Test for color GrafPort

	Check the version of the GrafPort and if color is supported, return TRUE.

	\return TRUE if the GrafPort is really a CGrafPort.

***************************************/

uint_t BURGER_API Burger::Mac::IsColorGrafPort(
	const GrafPort* pInput) BURGER_NOEXCEPT
{
#if defined(BURGER_MACCARBON)
	BURGER_UNUSED(pInput);
	return TRUE;
#else
	// Do a version check to detect for color
	return reinterpret_cast<const CGrafPort*>(pInput)->portVersion < 0;
#endif
}

/*! ************************************

	\brief Return the version of a GetVolParmsInfoBuffer

	Depending on the version of MacOS the application is running, and the device
	driver found for the mounted drive, it will support levels of information.
	This accessor will return the version of the GetVolParmsInfoBuffer buffer so
	the application will know what levels of data are present.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Version of the GetVolParmsInfoBuffer (0 through 3)

***************************************/

short BURGER_API Burger::GetInfoVersion(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return pVolParamsInfoBuffer->vMVersion;
}

/*! ************************************

	\brief Return the volume attributes of a GetVolParmsInfoBuffer

	Obtain the volume attribute bits.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume attribute bits.

***************************************/

long BURGER_API Burger::GetInfoAttrib(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return pVolParamsInfoBuffer->vMAttrib;
}

/*! ************************************

	\brief Return the volume Handle

	Obtain the Handle of the volume driver data. USE AT YOUR OWN RISK!

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume driver local data Handle.

***************************************/

Handle BURGER_API Burger::GetLocalHand(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return pVolParamsInfoBuffer->vMLocalHand;
}

/*! ************************************

	\brief Return the server IPv4 address

	If the volume is a mounted network server, the IPv4 address can be obtained
	from this function. If the value is zero, then this is a locally mounted
	block device. Firewire, Floppy, USB and IDE drives will return zero.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return IPv4 address of network shared volume, or zero.

***************************************/

uint32_t BURGER_API Burger::GetServerAdr(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return static_cast<uint32_t>(pVolParamsInfoBuffer->vMServerAdr);
}

/*! ************************************

	\brief Return the volume speed grade.

	If the speed class of a volume is known, return that value. Higher is
	better. If the speed is unknown or if the version is less than 2, return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Speed grade of volume, or zero if unknown.

***************************************/

long BURGER_API Burger::GetVolumeGrade(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 2) ?
        0 :
        pVolParamsInfoBuffer->vMVolumeGrade;
}

/*! ************************************

	\brief Return the foreign privilege model

	Volumes may have additional privileges models, this value is 0 for HFS
	volumes which may or may not support AFP, fsUnixPriv if it supports A/UX. If
	the version is less than 2, return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume privilege ID, or zero if unknown.

***************************************/

long BURGER_API Burger::GetForeignPrivID(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 2) ?
        0 :
        pVolParamsInfoBuffer->vMForeignPrivID;
}

/*! ************************************

	\brief Return the extended volume attributes.

	Most volumes support extended attributes that denote if the volume can be
	ejected, max size, and other details. If the GetVolParmsInfoBuffer record is
	less than version 3, this function will return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume extended attributes or 0 if not supported.

***************************************/

long BURGER_API Burger::GetExtendedAttributes(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 3) ?
        0 :
        pVolParamsInfoBuffer->vMExtendedAttributes;
}

/*! ************************************

	\brief Is the volume a network drive?

	Test if the volume is a shared drive accessed by a network.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if it's a remote drive, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::IsNetworkVolume(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	// Test the IPv4 address
	return pVolParamsInfoBuffer->vMServerAdr != 0;
}

/*! ************************************

	\brief Is the File Control Blocks limited to 8?

	Test if the volume uses only 8 control blocks instead of 16.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if it limits FCBs, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::DoesItHaveLimitFCBs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bLimitFCBs)) != 0;
}

/*! ************************************

	\brief Does the Finder uses the shared volume handle.

	Test if the Finder uses the shared volume handle for its local window list.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the Finder uses the handle, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::DoesItHaveLocalWList(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bLocalWList)) != 0;
}

/*! ************************************

	\brief Is the mini finder disabled?

	Burgerlib requires a minimum of MacOS 7.5 or higher, so this will always
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the mini Finder is disabled, \ref FALSE if available.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoMiniFndr(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoMiniFndr)) != 0;
}

/*! ************************************

	\brief Is the volume name locked?

	Test if the volume name can be changed. Locked and network volumes cannot be
	altered, so they will return \ref TRUE. Local drives almost always return
	\ref FALSE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume name is immutable, \ref FALSE if is can be
		changed.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoVNEdit(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoVNEdit)) != 0;
}

/*! ************************************

	\brief Is the modification time controlled by the volume?

	Test if the volume modification times is managed by the volume. Network
	drives tend to manage the file creation/modification times, so if this
	returns \ref TRUE, all requests to alter the time codes will fail.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume manages time codes, \ref FALSE if the codes
		can be locally changed.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoLclSync(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoLclSync)) != 0;
}

/*! ************************************

	\brief Does the volume zoom to the trash?

	If this volume is unmounted, should the Finder show a "zoom" to the trash?

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume should make a visual effect, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveTrshOffLine(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bTrshOffLine)) != 0;
}

/*! ************************************

	\brief Don't switch to launching applications on this volume?

	If the volume forbids launching applications from it, this accessor will
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if applications shouldn't launch from this volume,
		\ref FALSE if permitted.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoSwitchTo(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoSwitchTo)) != 0;
}

/*! ************************************

	\brief Forbid desktop items on this volume?

	If the volume forbids placing items on the desktop, this accessor will
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the desktop folder is not supported on this volume,
		\ref FALSE if permitted.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoDeskItems(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoDeskItems)) != 0;
}

/*! ************************************

	\brief Is booting disabled on this volume?

	If the volume does not have boot blocks, return \ref TRUE. Boot blocks are
	required to boot into classic MacOS. The Control Panel will dim the icon for
	this drive for startup if this bit is set.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume cannot be booted, \ref FALSE if bootable.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoBootBlks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoBootBlks)) != 0;
}

/*! ************************************

	\brief Is AppleTalk File Protocol access interfaces supported?

	If the volume supports AppleTalk File Protocol, this accessor will return
	\ref TRUE. Calls to ``PBHGetLoginInfo``, ``PBHGetDirAccess``,
	``PBHSetDirAccess``,``PBHMapID``, and ``PBHMapName`` will succeed.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supported AFP, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveAccessCntl(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bAccessCntl)) != 0;
}

/*! ************************************

	\brief Is the volume missing a System directory?

	This volume doesn't support a System directory. Do not switch launch to this
	volume if \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume don't support a System directory, \ref FALSE
		if it does.

***************************************/

uint_t BURGER_API Burger::DoesItHaveNoSysDir(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoSysDir)) != 0;
}

/*! ************************************

	\brief Is the volume using an external file system?

	This volume is managed remotely or within a device and not from the local
	file manager if this returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume file system is controlled by an external
		device, \ref FALSE if it's managed locally.

***************************************/

uint_t BURGER_API Burger::DoesItHaveExtFSVol(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasExtFSVol)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHOpenDeny?

	Does the volume support the calls ``PBHOpenDeny`` and ``PBHOpenRFDeny``? If
	so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHOpenDeny, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveOpenDeny(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasOpenDeny)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHCopyFile?

	Does the volume support the call ``PBHCopyFile``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHCopyFile, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveCopyFile(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasCopyFile)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHMoveRename?

	Does the volume support the call ``PBHMoveRename``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHMoveRename, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveMoveRename(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasMoveRename)) != 0;
}

/*! ************************************

	\brief Does the volume support desktop functions?

	Does the volume support all of the desktop functions for the Desktop
	Manager? If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Desktop Manager, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveDesktopMgr(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasDesktopMgr)) != 0;
}

/*! ************************************

	\brief Does the volume support AFP short names?

	Does the volume support AppleTalk File Protocol short names? If so, it
	returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports AFP short names, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveShortName(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasShortName)) != 0;
}

/*! ************************************

	\brief Does the volume support locked folders?

	Does the volume support attribute bits on folders so they can be locked? If
	so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports folder locking, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveFolderLock(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasFolderLock)) != 0;
}

/*! ************************************

	\brief Is local file sharing enabled on this volume?

	If this volume is being shared, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if file sharing is enabled on this volume, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItHavePersonalAccessPrivileges(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bHasPersonalAccessPrivileges)) != 0;
}

/*! ************************************

	\brief Does this volume support "Users and Groups"?

	If this volume is either on MacOS X, or on a network volume that supports
	user level privilege access, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume support user level access \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveUserGroupList(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasUserGroupList)) != 0;
}

/*! ************************************

	\brief Does the volume support PBCatSearch?

	Does the volume support the call ``PBCatSearch``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBCatSearch, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveCatSearch(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasCatSearch)) != 0;
}

/*! ************************************

	\brief Does the volume support PBExchangeFiles?

	Does the volume support the call ``PBExchangeFiles`` and file ID functions?
	If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBExchangeFiles, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveFileIDs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasFileIDs)) != 0;
}

/*! ************************************

	\brief Does the volume use BTrees?

	Does the volume use BTrees? This is \ref TRUE on local volume formatted with
	HFS. If the volume is formatted with a different file system, this will
	return \ref FALSE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume uses BTrees, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveBTreeMgr(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasBTreeMgr)) != 0;
}

/*! ************************************

	\brief Can folders inherit privileges?

	Does the volume support privilege inheritance? If so, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume allows access privilege inheritance,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItHaveBlankAccessPrivileges(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bHasBlankAccessPrivileges)) != 0;
}

/*! ************************************

	\brief Can async file operations be performed on this volume?

	Does the volume support asynchronous file operations properly? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports asynchronous file operations,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportAsyncRequests(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bSupportsAsyncRequests)) !=
		0;
}

/*! ************************************

	\brief Does the volume use a Trash cache?

	Mac OS X doesn't support Trash attributes and simulates the behavior by
	using a hidden folder. If this is being used, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume uses a trash cache, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportTrashVolumeCache(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bSupportsTrashVolumeCache)) != 0;
}

/*! ************************************

	\brief Can the volume be ejected?

	If the volume can be unmounted, return \ref TRUE. This is especially true
	for USB and CD/DVD/BluRay drives.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume can be ejected, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::IsEjectable(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bIsEjectable)) != 0;
}

/*! ************************************

	\brief Does the volume driver directly support HFS+?

	If the volume driver supports the HFS+ API, return \ref TRUE. Otherwise the
	Mac File Manager will emulate them. If a volume supports the HFS+ APIs
	doesn't mean it's formatted with HFS+.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume support HFS+ APIs, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportHFSPlusAPIs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsHFSPlusAPIs)) != 0;
}

/*! ************************************

	\brief Does the volume support FSCatalogSearch?

	Does the volume support the call ``FSCatalogSearch`` and file ID functions?
	If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports FSCatalogSearch, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportFSCatalogSearch(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsFSCatalogSearch)) != 0;
}

/*! ************************************

	\brief Does the volume support FSExchangeObjects?

	Does the volume support the call ``FSExchangeObjects`` and file ID
	functions? If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports FSExchangeObjects, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportFSExchangeObjects(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsFSExchangeObjects)) != 0;
}

/*! ************************************

	\brief Does the volume support 2 terabyte files?

	Does the volume support files as large as 2 terabytes? Return \ref TRUE if
	true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports 2 TB files, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupport2TBFiles(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupports2TBFiles)) != 0;
}

/*! ************************************

	\brief Does the volume support filenames longer than 31 characters?

	Does the volume support filenames longer than 31 characters? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports long filenames, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportLongNames(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsLongNames)) != 0;
}

/*! ************************************

	\brief Does the volume support Unicode?

	Does the volume support Unicode so it can handle multiple languages? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Unicode, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportMultiScriptNames(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsMultiScriptNames)) != 0;
}

/*! ************************************

	\brief Does the volume support named forks?

	The mac uses a file system with two forks, a data fork and a resource fork.
	If the volume supports forks beyond that, they have names. The default is
	none for the data fork, and "RESOURCE_FORK" for the resource fork. If
	alternate forks are supported, this accessor will return \ref TRUE.

	Does the volume support Unicode so it can handle multiple languages? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports more than two forks, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportNamedForks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsNamedForks)) != 0;
}

/*! ************************************

	\brief Does the volume support recursive iterators?

	The volume supports recursive iterators, not at the volume root if this
	returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports recursive iterators, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::DoesItSupportSubtreeIterators(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsSubtreeIterators)) != 0;
}

/*! ************************************

	\brief Does the volume support Lg2Phys SPI correctly.

	The volume supports Lg2Phys correctly if this returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Lg2Phys, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::L2PCanMapFileBlocks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (GetExtendedAttributes(pVolParamsInfoBuffer) &
			   (1L << bL2PCanMapFileBlocks)) != 0;
}

/*! ************************************

	\brief Does the logged in use have ownership of this directory?

	The directory is owned by the currently logged in user, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the directory is owned by the user, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::IsUserOwner(int8_t ioACUser) BURGER_NOEXCEPT
{
	return !(ioACUser & kioACUserNotOwnerMask);
}

/*! ************************************

	\brief Does the logged in use have full access to this directory?

	If the user has full access to this directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if user has full directory access, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesUserHaveFullAccess(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return !(ioACUser &
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
			kioACUserNoMakeChangesMask));
}

/*! ************************************

	\brief Is the directory a drop box?

	Drop box folders allow operations like copying into the directory. But it
	doesn't allow viewing of the files and folders within. Check the directory
	attributes if the directory is a drop box. Return \ref TRUE if so.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the directory is a drop box, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesUserHaveDropBoxAccess(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) ==
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask);
}

/*! ************************************

	\brief Can the contents of the directory be changed?

	If the user has access to make changes in the directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the user has modification access to a directory,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesUserHaveChangeAccess(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) == kioACUserNoMakeChangesMask;
}

/*! ************************************

	\brief Is the user denied access to this directory?

	If the user has no access to this directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the user has no access to a directory,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::DoesUserHaveNoAccess(int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) ==
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
			kioACUserNoMakeChangesMask);
}

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record. pName is converted into a pascal string
	before being passed to PBGetCatInfoSync(). The internal buffer is removed
	from ioNamePtr before this function exits to prevent dangling pointers.

	\param pInfoPBRec Pointer to empty CInfoPBRec to be filled in.
	\param pNameBuffer Pointer to a 257 byte buffer for the filename.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::DoGetCatInfo(CInfoPBRec* pInfoPBRec,
	uint8_t pNameBuffer[257], short sVRefNum, long lDirID,
	const char* pName) BURGER_NOEXCEPT
{
	// Valid name?
	if (!pName || !pName[0]) {
		pNameBuffer[0] = 0;
		pInfoPBRec->dirInfo.ioNamePtr = pNameBuffer;

		// Use ioDirID only
		pInfoPBRec->dirInfo.ioFDirIndex = -1;
	} else {
		// Convert from UTF8 to MacRomanUS
		uintptr_t uLength = MacRomanUS::TranslateFromUTF8(
			reinterpret_cast<char*>(pNameBuffer + 1), 256, pName);
		pNameBuffer[0] = static_cast<uint8_t>(uLength);
		pInfoPBRec->dirInfo.ioNamePtr = pNameBuffer;

		// Use ioNamePtr and ioDirID
		pInfoPBRec->dirInfo.ioFDirIndex = 0;
	}

	// Pass in the directory and volume
	pInfoPBRec->dirInfo.ioVRefNum = sVRefNum;
	pInfoPBRec->dirInfo.ioDrDirID = lDirID;

	// Get the directory information and return the error
	return PBGetCatInfoSync(pInfoPBRec);
}

/*! ************************************

	\brief Call PBGetCatInfoSync() quickly.

	Given information about a file/folder, quickly obtain information about this
	file object and return the record. pName is converted into a pascal string
	before being passed to PBGetCatInfoSync(). The internal buffer is removed
	from ioNamePtr before this function exits to prevent dangling pointers.

	\param pInfoPBRec Pointer to empty CInfoPBRec to be filled in.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::DoGetCatInfo(CInfoPBRec* pInfoPBRec, short sVRefNum,
	long lDirID, const char* pName) BURGER_NOEXCEPT
{
	uint8_t TempString[256 + 1];

	// Perform the command with a temp buffer.
	int iError = DoGetCatInfo(pInfoPBRec, TempString, sVRefNum, lDirID, pName);

	// Get rid of the pointer, since it's local
	pInfoPBRec->dirInfo.ioNamePtr = nullptr;

	// Return the error, if any.
	return iError;
}

/*! ************************************

	\brief Get Finder DInfo for a directory.

	Given information about a folder, obtain the finder information about this
	folder object and return the DInfo record. pName is converted into a pascal
	string before being passed to PBGetCatInfoSync().

	\param pFinderInfo Pointer to empty DInfo to be filled in.
	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryInfo(DInfo* pFinderInfo, short sVRefNum,
	long lDirID, const char* pName) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	// Call PBGetCatInfoSync()
	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);
	if (iError == noErr) {

		// Check
		if (InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask) {

			// Only directories have finder information
			*pFinderInfo = InfoPBRec.dirInfo.ioDrUsrWds;

		} else {
			// Whoops! Abort
			iError = dirNFErr;
		}
	}

	// Return the error, if any
	return iError;
}

/*! ************************************

	\brief Get Finder DInfo for a directory using a FSSpec

	Given an FSSpec for a folder, obtain the finder information about this
	folder object and return the DInfo record.

	\param pFinderInfo Pointer to empty DInfo to be filled in.
	\param pFSSpec Pointer to a FSSpec that points to the folder of interest.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryInfo(
	DInfo* pFinderInfo, const FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	// Convert to a "C" string
	char TempBuffer[256];
	PStringToCString(TempBuffer, pFSSpec->name);

	// Call the function that does all the work.
	return GetDirectoryInfo(
		pFinderInfo, pFSSpec->vRefNum, pFSSpec->parID, TempBuffer);
}

/*! ************************************

	\brief Get common information from a directory.

	Given information about a file, get the ID of the named file object, if it's
	a directory and if the access is that of a "Drop box" folder.

	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.
	\param pDirID Pointer to receive the DirID of the named folder
	\param pbDirectory Pointer to receive a boolean that is \ref TRUE if it's a
		directory
	\param pbDropBox Pointer to receive a boolean if the folder is a drop box.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDestinationDirectoryInfo(short sVRefNum, long lDirID,
	const char* pName, long* pDirID, uint_t* pbDirectory,
	uint_t* pbDropBox) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	// This has to be cleared since some file systems don't mark it properly
	InfoPBRec.dirInfo.ioACUser = 0;

	// Pull in the information
	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);

	// Next directory ID in the chain.
	if (pDirID) {
		*pDirID = InfoPBRec.dirInfo.ioDrDirID;
	}

	// Directory?
	if (pbDirectory) {
		*pbDirectory = (InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask) != 0;
	}

	// Check if this is a mac "Drop box" folder.
	if (pbDropBox) {
		*pbDropBox = ((InfoPBRec.dirInfo.ioACUser &
						  (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
							  kioACUserNoMakeChangesMask)) ==
			(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask));
	}

	return iError;
}

/*! ************************************

	\brief Get finder comment ID

	Given information about a file, get the comment ID of the named file object.

	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.
	\param pCommentID Pointer to receive the finder comment ID.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetCommentID(short sVRefNum, long lDirID,
	const char* pName, short* pCommentID) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;
	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);

	// Return the comment resource ID
	*pCommentID = InfoPBRec.hFileInfo.ioFlXFndrInfo.fdComment;
	return iError;
}

/*! ************************************

	\brief Get directory ID of a named directory.

	Given information about a directory, get its directory ID.

	\param sVRefNum Volume reference number
	\param lDirID Current directory ID
	\param pName Name of the file of interest, can be nullptr.
	\param pDirID Pointer to receive the directory's ID.
	\param pbDirectory Pointer to receive \ref TRUE if it's a directory.

	\return Zero if no error, otherwise return MacOS error code.

***************************************/

int BURGER_API Burger::GetDirectoryID(short sVRefNum, long lDirID,
	const char* pName, long* pDirID, uint_t* pbDirectory) BURGER_NOEXCEPT
{
	CInfoPBRec InfoPBRec;

	int iError = DoGetCatInfo(&InfoPBRec, sVRefNum, lDirID, pName);
	if (iError == noErr) {

		uint_t bDirectory =
			(InfoPBRec.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0;

		// Return values if wanted.
		if (pbDirectory) {
			*pbDirectory = bDirectory;
		}

		if (pDirID) {
			// Which one?
			long lDirectory;
			if (bDirectory) {
				lDirectory = InfoPBRec.dirInfo.ioDrDirID;
			} else {
				lDirectory = InfoPBRec.hFileInfo.ioFlParID;
			}
			*pDirID = lDirectory;
		}
	}
	return iError;
}

/*! ************************************

	\brief Copy a fork of a file.

	Given an opened source and destination file fork, and a buffer, perform a
	complete copy of the data from the source file to the destination file,
	replacing the destination file fork with a copy of the source. Buffer is
	assumed to a size that's a minimum of 512 bytes or in a chunk size of 512
	bytes.

	\maconly

	\param iSourceRefNum Opened file reference for the source file
	\param iDestRefNum Opened file reference for the destination file
	\param pBuffer Pointer to the buffer to use for data transfer
	\param uBufferSize Size in bytes of the data transfer buffer

	\return Zero or Mac OSErr error code (Not Burgerlib)

***************************************/

int BURGER_API Burger::CopyFork(short iSourceRefNum, short iDestRefNum,
	uint8_t* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	// Param blocks for reading and writing
	ParamBlockRec SourcePB;
	ParamBlockRec DestPB;

	// Init the Param block records for the source and dest files
	SourcePB.ioParam.ioRefNum = iSourceRefNum;
	DestPB.ioParam.ioRefNum = iDestRefNum;

	// Ensure the destination file is the correct size (To ensure there is space
	// for the operation)
	OSErr iResult;
	for (;;) {

		// Get the source file size
		iResult = PBGetEOFSync(&SourcePB);
		if (iResult) {
			break;
		}

		// Set the new file to this size
		DestPB.ioParam.ioMisc = SourcePB.ioParam.ioMisc;
		iResult = PBSetEOFSync(&DestPB);
		if (iResult) {
			break;
		}

		// Set the marks for both source and destination
		SourcePB.ioParam.ioPosMode = fsFromStart;
		SourcePB.ioParam.ioPosOffset = 0;
		DestPB.ioParam.ioPosMode = fsFromStart;
		DestPB.ioParam.ioPosOffset = 0;

		iResult = PBSetFPosSync(&SourcePB);
		if (iResult) {
			break;
		}
		iResult = PBSetFPosSync(&DestPB);
		if (iResult) {
			break;
		}

		// Prepare the Param blocks for the transfer loop

		// Buffer to work with
		SourcePB.ioParam.ioBuffer = reinterpret_cast<Ptr>(pBuffer);
		DestPB.ioParam.ioBuffer = reinterpret_cast<Ptr>(pBuffer);

		// Don't bother caching
		SourcePB.ioParam.ioPosMode = fsAtMark + noCacheMask;
		DestPB.ioParam.ioPosMode = fsAtMark + noCacheMask;

		// Read in this chunk size
		SourcePB.ioParam.ioReqCount = static_cast<long>(uBufferSize);

		// Copy the data
		do {
			// Read it
			OSErr iSourceErr = PBReadSync(&SourcePB);
			// Only EOF and no error are acceptable
			if (iSourceErr && (iSourceErr != eofErr)) {
				iResult = iSourceErr;
				break;
			}
			// Write it
			DestPB.ioParam.ioReqCount = SourcePB.ioParam.ioActCount;
			iResult = PBWriteSync(&DestPB);

			// All done?
			if (iSourceErr == eofErr) {
				break;
			}
		} while (!iResult);

		break;
	}

	return iResult;
}

/*! ************************************

	\brief Initialize a Mac Finder FileInfo

	When creating a file on MacOS, a file type and creator code needs to be
	assigned. The default for generic files is file type "BINA" for binary and
	the creator code is "????" for unknown. It's up to the app developer to call
	SetFileAndAuxType() to assign the proper values after the file is created or
	modified.

	\maconly

	\param pFileInfo Pointer to an uninitialized FileInfo to be filled in.

	\sa FileManager::SetFileAndAuxType(const char *, uint32_t, uint32_t)

***************************************/

void BURGER_API Burger::InitFileInfo(FileInfo* pFileInfo) BURGER_NOEXCEPT
{
	pFileInfo->fileType = 'BINA';
	pFileInfo->fileCreator = 0x3F3F3F3F; // ???? kills doxygen
	pFileInfo->finderFlags = 0;
	pFileInfo->location.h = 0;
	pFileInfo->location.v = 0;
	pFileInfo->reservedField = 0;
}

/*! ************************************

	\brief Initialize a Mac FSRefParam

	When calling the Mac function PBGetCatalogInfoSync(), a FSRefParam needs to
	be prepared so only the information queried is returned. This structure is
	large, so this subroutine clears it out quickly. All entries except for
	"ref" and "whichInfo" are cleared to zero. These two values are set to
	the passed values.

	\maconly

	\param pFSRefParam Pointer to an uninitialized FSRefParam to be filled in.
	\param pFSRef Pointer to a valid FSRef of the file of interest.
	\param uCatInfoBitmap Value to put in ``whichInfo``.

***************************************/

void BURGER_API Burger::InitFSRefParam(FSRefParam* pFSRefParam, FSRef* pFSRef,
	uint32_t uCatInfoBitmap) BURGER_NOEXCEPT
{
	// Initialize the record
	MemoryClear(pFSRefParam, sizeof(FSRefParam));
	pFSRefParam->ref = pFSRef;
	pFSRefParam->whichInfo = uCatInfoBitmap;
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected.

	\maconly

	\param svRefNum Volume the file resides on
	\param lDirID Directory ID the file resides on
	\param pName Pointer to a PASCAL string of the filename
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

***************************************/

int BURGER_API Burger::CheckForForks(short svRefNum, long lDirID,
	const uint8_t* pName, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
	// Use PBHGetFInfoSync() for old school API
	HParamBlockRec hpb;

	hpb.fileParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<unsigned char*>(pName));
	hpb.fileParam.ioVRefNum = svRefNum;
	hpb.fileParam.ioFVersNum = 0;
	hpb.fileParam.ioDirID = lDirID;
	hpb.fileParam.ioFDirIndex = 0;

	// Read in the file sizes
	int iMacError = PBHGetFInfoSync(&hpb);

	// Gotcha!
	*pbDataFork = (hpb.fileParam.ioFlLgLen != 0);
	*pbResourceFork = (hpb.fileParam.ioFlRLgLen != 0);
	return iMacError;
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected.

	\maconly

	\param pFSSpec Pointer to the FSSpec of the file of interest.
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

	\sa CheckForForks(short, long, const uint8_t *, uint_t *, uint_t *)

***************************************/

int BURGER_API Burger::CheckForForks(const FSSpec* pFSSpec, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
	return CheckForForks(pFSSpec->vRefNum, pFSSpec->parID, pFSSpec->name,
		pbDataFork, pbResourceFork);
}

/*! ************************************

	\brief Test if a file has either a resource or data fork.

	Before copying a file, check if the source has a resource or data fork, so
	only the detected forks will be copied. Return two booleans that are either
	\ref TRUE or \ref FALSE if either fork is detected.

	\maconly

	\param pFSRef Pointer to the FSRef of the file of interest.
	\param pbDataFork Pointer to a boolean to receive the presence of a data
		fork.
	\param pbResourceFork Pointer to a boolean to receive the presence of a
		resource fork.

***************************************/

int BURGER_API Burger::CheckForForks(const FSRef* pFSRef, uint_t* pbDataFork,
	uint_t* pbResourceFork) BURGER_NOEXCEPT
{
#if !defined(BURGER_CFM) || !defined(BURGER_68K)
	FSCatalogInfo MyInfo;

	OSErr iMacError =
		FSGetCatalogInfo(pFSRef, kFSCatInfoDataSizes | kFSCatInfoRsrcSizes,
			&MyInfo, nullptr, nullptr, nullptr);
	*pbDataFork = (MyInfo.dataLogicalSize != 0);
	*pbResourceFork = (MyInfo.rsrcLogicalSize != 0);

	return iMacError;
#else
	// Not supported
	BURGER_UNUSED(pFSRef);
	BURGER_UNUSED(pbDataFork);
	BURGER_UNUSED(pbResourceFork);
	return paramErr;
#endif
}

/*! ************************************

	\brief Obtain information from a drive volume.

	Call PBHGetVolParmsSync and fill in a supplied GetVolParmsInfoBuffer record.
	Pass in a pointer to a uint32_t that has the size, in bytes, of the
	GetVolParmsInfoBuffer structure and the function will replace the value with
	the actual transferred buffer size.

	\maconly

	\param pVolumeName Pointer to a pascal string of the volume name.
	\param sVRefNum Volume reference number
	\param pGetVolParmsInfoBuffer Pointer to an uninitialized
		GetVolParmsInfoBuffer buffer
	\param pParmsLength Pointer to a value that contains the size, in bytes, of
		the GetVolParmsInfoBuffer. Will return the actual size filled in.

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::HGetVolParms(const uint8_t* pVolumeName, short sVRefNum,
	GetVolParmsInfoBuffer* pGetVolParmsInfoBuffer, uint32_t* pParmsLength)
{
	// Set up the parameter block
	HParamBlockRec hpbr;

	hpbr.ioParam.ioNamePtr =
		reinterpret_cast<StringPtr>(const_cast<uint8_t*>(pVolumeName));
	hpbr.ioParam.ioVRefNum = sVRefNum;
	hpbr.ioParam.ioBuffer = reinterpret_cast<Ptr>(pGetVolParmsInfoBuffer);
	hpbr.ioParam.ioReqCount = static_cast<long>(*pParmsLength);

	// Get the volume information
	int iMacError = PBHGetVolParmsSync(&hpbr);
	if (!iMacError) {
		// Return the actual number of bytes transferred
		*pParmsLength = static_cast<uint32_t>(hpbr.ioParam.ioActCount);
	}
	return iMacError;
}

/*! ************************************

	\brief Obtain information about a volume.

	Given a volume reference number, fill in a HParamBlockRec record with
	information about the volume. The ioNamePtr entry is zeroed out so
	retrieving a volume name is not possible with this call.

	\maconly

	\param sVRefNum Volume reference number
	\param pHParamBlockRec Pointer to an uninitialized HParamBlockRec buffer

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetVolumeInfo(
	short sVRefNum, HParamBlockRec* pHParamBlockRec) BURGER_NOEXCEPT
{
	// Set the volume number
	pHParamBlockRec->volumeParam.ioVRefNum = sVRefNum;

	// Don't worry about the name
	pHParamBlockRec->volumeParam.ioNamePtr = nullptr;

	// Use only the volume reference number only
	pHParamBlockRec->volumeParam.ioVolIndex = 0;

	// Get the information
	return PBHGetVInfoSync(pHParamBlockRec);
}

/*! ************************************

	\brief Find the real volume reference number.

	Given a volume reference number, get its information and determine if it is
	an alias or a real number. This function will call PBHGetVInfoSync and may
	return a different number that was passed in. This number is the true device
	number.

	\maconly

	\param svRefNum Volume reference number
	\param pRealVRefNum Pointer to receive the returned volume reference number

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::DetermineVRefNum(
	short svRefNum, short* pRealVRefNum) BURGER_NOEXCEPT
{
	HParamBlockRec hpbr;

	// Get the information of the volume
	int iMacError = GetVolumeInfo(svRefNum, &hpbr);
	if (!iMacError) {
		// Get the real volume reference number
		*pRealVRefNum = hpbr.volumeParam.ioVRefNum;
	}
	return iMacError;
}

/*! ************************************

	\brief Check if a volume is locked.

	Given a volume reference number, get its information and determine if it is
	locked. If no error is returned, the volume is not locked. A wPrErr is
	returned if the lock is by hardware (Write protect tab) or vLckdErr if the
	volume was locked by a software flag.

	\maconly

	\param svRefNum Volume reference number

	\return Zero if no error, or mac error code which shows how it was locked.

***************************************/

int BURGER_API Burger::CheckVolLock(short svRefNum)
{
	HParamBlockRec hpbr;

	int iMacError = GetVolumeInfo(svRefNum, &hpbr);
	if (!iMacError) {
		if (hpbr.volumeParam.ioVAtrb & kHFSVolumeHardwareLockMask) {
			// Hardware volume lock
			iMacError = wPrErr;
		} else if (hpbr.volumeParam.ioVAtrb & kHFSVolumeSoftwareLockMask) {
			// Software volume lock
			iMacError = vLckdErr;
		}
	}

	return iMacError;
}

/*! ************************************

	\brief Find a file location from a file reference number.

	When a file is opened, a file reference number is generated. This function
	will return the volume, directory ID and filename of the opened file.

	\maconly

	\param iFileRefNum Open file reference ID
	\param pVRefNum Pointer to variable to receive the volume reference number
	\param pDirID Pointer to variable to receive the file's parent ID.
	\param pOutputFilename Pointer to a 256 byte buffer to receive the Pascal
		filename

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetFileLocation(short iFileRefNum, short* pVRefNum,
	long* pDirID, uint8_t* pOutputFilename) BURGER_NOEXCEPT
{
	// File control block record
	FCBPBRec fbcpbrec;

	// Save the open file reference number
	fbcpbrec.ioRefNum = iFileRefNum;

	// Set the buffer to capture the filename
	fbcpbrec.ioNamePtr = pOutputFilename;
	fbcpbrec.ioVRefNum = 0;
	fbcpbrec.ioFCBIndx = 0;

	// Grab it
	int iMacError = PBGetFCBInfoSync(&fbcpbrec);
	if (!iMacError) {
		// Return the volume reference and the directory ID
		// The filename is already stored in pOutputFilename
		*pVRefNum = fbcpbrec.ioFCBVRefNum;
		*pDirID = fbcpbrec.ioFCBParID;
	}
	return iMacError;
}

/*! ************************************

	\brief Determine the name of the Finder Desktop Database

	On network shares, the finder stores metadata like file comments in a file
	called "Desktop" on the root folder. Due to localization, this file name
	might be in another language so loading by name is impossible. Iterate
	through the root directory of the volume for the first file with the creator
	code of 'ERIK' and the file type of 'FNDR' and return the name of that file.

	\maconly

	\param svRefNum Volume reference number of the volume to scan for the file.
	\param pDesktopName Pointer to a 256 byte buffer to receive the Pascal
		filename

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetDesktopFileName(
	short svRefNum, uint8_t* pDesktopName) BURGER_NOEXCEPT
{
	CInfoPBRec cinfo;

	// Set up the volume to iterate
	cinfo.hFileInfo.ioNamePtr = pDesktopName;
	cinfo.hFileInfo.ioVRefNum = svRefNum;

	// Start with file entry #1
	short iIndex = 1;
	int iMacError;
	for (;;) {

		// Scan the root volume
		cinfo.dirInfo.ioDrDirID = fsRtDirID;
		cinfo.dirInfo.ioFDirIndex = iIndex;
		iMacError = PBGetCatInfoSync(&cinfo);

		// Possible it ran out of entries
		if (iMacError) {
			break;
		}

		// Is this the file of interest?
		if ((cinfo.hFileInfo.ioFlFndrInfo.fdType == 'FNDR') &&
			(cinfo.hFileInfo.ioFlFndrInfo.fdCreator == 'ERIK')) {
			// Bingo!
			break;
		}
		++iIndex;
	}

	// Only returns zero if the file was found
	return iMacError;
}

/*! ************************************

	\brief Load in a comment from a Desktop resource file.

	On network shares, the finder stores metadata like file comments in a file
	called "Desktop" on the root folder. Given the location of a file of
	interest, look up its comment ID in the database looking for a match. If
	found, return a copy of the data within.

	\maconly

	\param svRefNum Volume reference number of the file to check
	\param lDirID Directory ID of the file to check
	\param pFileName Pointer to a "C" string of the filename to check
	\param pComment Pointer to a 256 byte buffer to receive the Pascal
		comment

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::GetCommentFromDesktopFile(short svRefNum, long lDirID,
	const char* pFileName, uint8_t* pComment) BURGER_NOEXCEPT
{
	// Obtain the file's comment resource ID
	short sCommentID;
	int iMacError = GetCommentID(svRefNum, lDirID, pFileName, &sCommentID);
	if (!iMacError) {

		// The ID must not be zero
		if (!sCommentID) {
			// If there is no comment, just return an error
			iMacError = afpItemNotFound;

		} else {

			// Find the actual volume reference number
			short sRealVRefNum;
			iMacError = DetermineVRefNum(svRefNum, &sRealVRefNum);
			if (!iMacError) {

				// Using the real volume number, locate the "Desktop" file
				Str255 DesktopFileName;
				iMacError = GetDesktopFileName(sRealVRefNum, DesktopFileName);

				// If not found, give up.
				if (iMacError) {
					iMacError = afpItemNotFound;

				} else {

					// Open the "Desktop" resource file, however, make sure it
					// doesn't preload everything else when opened
					short sSavedResFile = CurResFile();

					// Open the resource file wrapped with the auto load
					// disabled.
					SetResLoad(false);
					short iRezRefNum = HOpenResFile(
						sRealVRefNum, fsRtDirID, DesktopFileName, fsRdPerm);
					SetResLoad(true);

					// Opened?
					if (iRezRefNum == -1) {

						// Nope!
						iMacError = afpItemNotFound;
					} else {

						// Using the comment ID, get the resource
						StringHandle hComment = reinterpret_cast<StringHandle>(
							Get1Resource('FCMT', sCommentID));
						if (!hComment) {
							// No handle means no data available
							iMacError = afpItemNotFound;

							// Verify the handle is not damaged
						} else if (GetHandleSize(reinterpret_cast<Handle>(
									   hComment)) <= 0) {
							iMacError = afpItemNotFound;
						} else {
							// Copy the data (Pascal string)
							MemoryCopy(pComment, *hComment,
								static_cast<uintptr_t>(*hComment[0] + 1));
						}

						// Restore the resource file chain and close the Desktop
						// file
						UseResFile(sSavedResFile);
						CloseResFile(iRezRefNum);
					}
				}
			}
		}
	}
	return iMacError;
}

#endif
