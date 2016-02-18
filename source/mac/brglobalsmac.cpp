/***************************************

	MacOS version of Burger::Globals

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brglobals.h"
#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brstringfunctions.h"
#include <InternetConfig.h>
#include <Gestalt.h>
#include <InputSprocket.h>
#include <DrawSprocket.h>
#include <LowMem.h>
#include <Folders.h>

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

static const Word16 g_AppleShareVer[] = {
	0x000,0x350,0x360,0x361,0x362,
	0x363,0x370,0x372,0x380,0x381,
	0x382,0x383,0x384};
	
#endif

/*! ************************************

	\brief Detect if a MacOS trap number exists.
	
	Detect if a specific 680x0 trap instruction is present.
	
	Due to the nature of this function, caching in impractical
	and it is not performed. This function is considered a performance
	bottleneck and should be called sparingly.
	
	\note This is a MacOS Classic function. It always returns \ref FALSE on Carbon targets.

	\param uTrapNum A valid MacOS 680x0 trap number from Traps.h.
	
	\return \ref TRUE if the requested trap index is present, \ref FALSE if not.
		
***************************************/

Word BURGER_API Burger::Globals::IsTrapAvailable(Word uTrapNum) 
{
	Word uResult = FALSE;
	//
	// Only matters when running 680x0 code or PowerPC code
	// with a 680x0 emulator
	//
#if !TARGET_API_MAC_CARBON
	do {
		if (uTrapNum<0x10000U) {			// Failsafe
			TrapType MyTrapType = OSTrap;	// Assume it's an OS trap!
			if (uTrapNum & 0x0800) {		// What trap is it?
				// Is it in the extended trap table?
				// Check if the extended trap table is even present by checking if
				// _InitGraf and _InitGraf+0x200 map to the same address.
		
				if (((uTrapNum & 0x03FF) >= 0x0200) &&
					(GetToolboxTrapAddress(_InitGraf) == GetToolboxTrapAddress(_InitGraf+0x200))) {
					break;		// This trap can't be in the short table. I don't support extended traps.
				}
				MyTrapType = ToolTrap;		// Toolbox trap
			}
		
			if (NGetTrapAddress(static_cast<UInt16>(uTrapNum),MyTrapType) != GetToolboxTrapAddress(_Unimplemented)) {
				uResult = TRUE;		// The trap is present
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
	
	Detect if the PowerPC native extension of Quicktime is
	loaded and available. This is only for Quicktime version 2.5
	during the initial transition from 680x0 to PowerPC. It's
	almost certainly going to return \ref TRUE on PowerPC systems.
	Only in very rare cases of Quicktime 2.5 where it's emulated
	in 680x0 code (And therefore a performance bottleneck) in
	which this matters.
	
	This function is written so it only does the detection
	once and it caches the result. The cached value is
	returned on subsequent calls.
	
	\return \ref TRUE if Quicktime is running in PowerPC code, \ref FALSE if emulated 680x0.
	
	\note This code always returns \ref FALSE on non-PowerPC targets.
	
***************************************/

Word BURGER_API Burger::Globals::IsQuickTimePowerPlugAvailable(void)
{
#if defined(BURGER_POWERPC)
	Word bResult = g_Globals.m_bIsQuickTimePlugInTested;
	// Was it already tested?
	if (!(bResult&0x80)) {
		long gestaltAnswer;
		if (!Gestalt(gestaltQuickTimeFeatures,&gestaltAnswer)) {
			if ((gestaltAnswer & (1<<gestaltPPCQuickTimeLibPresent)) && (EnterMovies!=NULL)) {
				bResult = 0x80|TRUE;		// Quicktime is in PowerPC
			} else {
				bResult = 0x80|FALSE;
			}
		}
		g_Globals.m_bIsQuickTimePlugInTested = static_cast<Word8>(bResult);
	}
	// Return the value minus the other flags
	return bResult&1U;
#else
	// Emulated 68k if false
	return FALSE;
#endif
}

/*! ************************************

	\brief Return the version of AppleShare library.
	
	Ask the AppleShare library what version it is and return that value.
	The function caches the value so subsequent calls
	do not ask AppleShare for the version again.
	
	\return Version in the format of 0x0102 -> 1.2
	
	\note If the AppleShare library is not installed, it will return 0.
	
***************************************/

Word BURGER_API Burger::Globals::GetAppleShareVersion(void)
{
	Globals *pGlobals = &g_Globals;
	Word uVersion;
	if (!pGlobals->m_bAppleShareVersionTested) {
		long MyAnswer;
		uVersion = 0;		// Assume failure
		if (!Gestalt(gestaltAFPClient,&MyAnswer)) {		// Detect Appleshare
			Word uIndex = static_cast<Word>(MyAnswer&gestaltAFPClientVersionMask);
			if (uIndex>12) {
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
	
	Detect if InputSprocket is available, and if so, query
	it for the version present. If InputSprocket is not available,
	the version returned is zero.
	
	This function is written so it only asks for the version
	once from InputSprocket. It will cache the version and
	return the cached value on subsequent calls.
	
	\return Version in the format of 0x0102 -> 1.2
	
	\note This code only works on CFM versions of Mac Classic
	applications. Non-CFM 68k targets will always return 0.
	
***************************************/

Word BURGER_API Burger::Globals::GetInputSprocketVersion(void)
{	
	// 680x0 CFM and PowerPC have InputSprocket
	#if defined(BURGER_CFM)
	Globals *pGlobals = &g_Globals;
	Word uVersion;
	if (!pGlobals->m_bInputSprocketVersionTested) {
		// Code even present?
		uVersion = 0;
		if (ISpGetVersion!=NULL) {

			// Call inputsprocket and get the version

			NumVersion MyVersion;		// Don't coalesce, the compiler isn't smart
										// enough for NumVersion = foo();
			MyVersion = ISpGetVersion();
			uVersion = (MyVersion.majorRev<<8)&0xFF00U;
			uVersion |= (MyVersion.minorAndBugRev)&0xFF;		
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
	
	Detect if DrawSprocket is available, and if so, query
	it for the version present. If DrawSprocket is not available,
	the version returned is zero.
	
	This function is written so it only asks for the version
	once from DrawSprocket. It will cache the version and
	return the cached value on subsequent calls.
	
	DSpGetVersion() only appeared in version 1.7 or later and
	as a result, to get the version from earlier versions
	the version resource found in the 
	library itself must be opened and read. Because of this,
	the first call to this function may take some time.
	
	\return Version in the format of 0x0102 -> 1.2
	
	\note This code only works on CFM versions of Mac Classic
	applications. Non-CFM 68k targets will always return 0.
	
***************************************/

Word BURGER_API Burger::Globals::GetDrawSprocketVersion(void)
{
	// 680x0 CFM and PowerPC have DrawSprocket
	#if defined(BURGER_CFM)

	static const Word8 DebugDrawSpocketName[] = "\pDrawSprocketDebugLib";
	static const Word8 DrawSprocketName[] = "\pDrawSprocketLib";

	Globals *pGlobals = &g_Globals;
	Word uVersion;
	if (!pGlobals->m_bDrawSprocketVersionTested) {

		// Can I do it the easy way?
		
		if (DSpGetVersion==NULL) {

			uVersion = 0;
			// I have to do it the hard way. :(
				
			// If it's version 1.1.4 or earlier, find the extension 
			// and manually grab the version resource

			UInt8 OldResLoad = LMGetResLoad();			// Get the previous resource load setting
			short SavedResourceRef = CurResFile();		// Get the current resource ref

			SetResLoad(FALSE);				// Don't load in all the preloaded resources
			short VolumeRef;				// Volume for extensions folder
			long DirIDRef;					// Directory for extensions folder
			HGetVol(0,&VolumeRef,&DirIDRef);	// Get my current folder
				
			// Is it in my directory? Check for debug version first
			short DrawSprocketResourceRef = HOpenResFile(VolumeRef,DirIDRef,DebugDrawSpocketName,fsRdPerm);
			OSErr ErrorCode = ResError();							// Error?
			if (ErrorCode == fnfErr) {								// Let's try again with release
				DrawSprocketResourceRef = HOpenResFile(VolumeRef,DirIDRef,DrawSprocketName,fsRdPerm);
				ErrorCode = ResError();								// Error?
				if (ErrorCode == fnfErr) {							// Try the system folder now
					
					// At this time, search the extensions folder
						
					FindFolder(kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &VolumeRef, &DirIDRef);

					// Release build is checked first
						
					FSSpec DrawSprocketFSSpec;
					FSMakeFSSpec(VolumeRef,DirIDRef,DrawSprocketName,&DrawSprocketFSSpec);

					DrawSprocketResourceRef = FSpOpenResFile(&DrawSprocketFSSpec,fsRdPerm);
					ErrorCode = ResError();
					if (ErrorCode == fnfErr) {
							
						// Try the debug build next
						FSMakeFSSpec(VolumeRef, DirIDRef, DebugDrawSpocketName, &DrawSprocketFSSpec);
						DrawSprocketResourceRef = FSpOpenResFile (&DrawSprocketFSSpec, fsRdPerm);
							
						// Here is where I give up and call it a day
						ErrorCode = ResError();
					}
				}
			}
			
			// Do we have a resource open?
			if (!ErrorCode && (DrawSprocketResourceRef!=-1)) {
				SetResLoad(TRUE);							// Ok, let's actually load something
				Handle hVersion = GetResource('vers',1); 	// Get the version resource
				if (hVersion && !ResError()) {				// Valid?
					const Word8 *pWork = reinterpret_cast<const Word8 **>(hVersion)[0];	// Deref the handle
					uVersion = static_cast<Word>(pWork[0])<<8;		// Major version
					uVersion |= pWork[1];					// Minor version
					ReleaseResource(hVersion);				// Release the version resource
				}
				UseResFile(SavedResourceRef);				// Restore the resource file first
				CloseResFile(DrawSprocketResourceRef);		// Close the lib's version resource file
			}
			
			UseResFile(SavedResourceRef);	// Set the resource file to original setting
			SetResLoad(OldResLoad);			// Set the ResLoad flag to original setting
		} else {
		
			// Let's do it the easy way!
		
			NumVersion MyVersion;
					
			// Get the version 
			MyVersion = DSpGetVersion();
		
			// Merge the version number
			uVersion = (MyVersion.majorRev<<8)&0xFF00U;
			uVersion |= (MyVersion.minorAndBugRev)&0xFF;
		}
		pGlobals->m_uDrawSprocketVersion = uVersion;
		pGlobals->m_bDrawSprocketVersionTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uDrawSprocketVersion;
	}
	return uVersion;

#else
	return 0;
#endif
}

/*! ************************************

	\brief Return the version of MacOS.
	
	Ask MacOS what version it is and return that value.
	
	\return Version in the format of 0x0102 -> 1.2
	
***************************************/

Word BURGER_API Burger::Globals::GetMacOSVersion(void)
{
	Globals *pGlobals = &g_Globals;
	Word uVersion;
	if (!pGlobals->m_bMacOSTested) {
		long MyAnswer;
		// Get the version with Gestalt
		if (Gestalt(gestaltSystemVersion,&MyAnswer)) {
			MyAnswer = 0;	// Should NEVER execute, failsafe
		}
		// Get the version of the OS in 0x0102 (1.2) format
		uVersion = static_cast<Word>(MyAnswer&0xFFFFU);
		pGlobals->m_uMacOSVersion = uVersion;
		pGlobals->m_bMacOSTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uMacOSVersion;
	}
	return uVersion;
}

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char *pURL)
{
	ICInstance inst;

	OSStatus err = -1;
	if (ICStart!=NULL) {
		err = ICStart(&inst,'????');		// Use your creator code if you have one!
		if (err == noErr) {
#if !TARGET_API_MAC_CARBON
			err = ICFindConfigFile(inst, 0, nil);
			if (err == noErr)
#endif
			{
				long startSel = 0;
				long endSel = static_cast<long>(StringLength(pURL));
				err = ICLaunchURL(inst, "\p",const_cast<char *>(pURL),endSel,&startSel,&endSel);
			}
			ICStop(inst);
		}
	}
	return static_cast<Word>(err);
 }

#endif