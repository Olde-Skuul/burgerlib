/***************************************

    MacOS version of Burger::Globals

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brstring.h"
#include "brstringfunctions.h"
#include <DrawSprocket.h>
#include <Folders.h>
#include <Gestalt.h>
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

uint_t BURGER_API Burger::Mac::IsTrapAvailable(uint_t uTrapNum)
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

uint_t BURGER_API Burger::Mac::IsQuickTimePowerPlugAvailable(void)
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

    \brief Return the version of AppleShare library.

    Ask the AppleShare library what version it is and return that value. The
    function caches the value so subsequent calls do not ask AppleShare for the
    version again.

    \return Version in the format of 0x0102 -> 1.2

    \note If the AppleShare library is not installed, it will return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetAppleShareVersion(void)
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

uint_t BURGER_API Burger::Mac::GetInputSprocketVersion(void)
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
            uVersion |= (MyVersion.minorAndBugRev) & 0xFF;
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

uint_t BURGER_API Burger::Mac::GetDrawSprocketVersion(void)
{
// 680x0 CFM and PowerPC have DrawSprocket
#if defined(BURGER_CFM)

    static const uint8_t DebugDrawSpocketName[] = "\pDrawSprocketDebugLib";
    static const uint8_t DrawSprocketName[] = "\pDrawSprocketLib";

    Mac* pGlobals = &g_Globals;
    uint_t uVersion;
    if (!pGlobals->m_bDrawSprocketVersionTested) {

        // Can I do it the easy way?

        if (DSpGetVersion == nullptr) {

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
                ErrorCode = ResError(); // Error?
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
        } else {

            // Let's do it the easy way!

            NumVersion MyVersion;

            // Get the version
            MyVersion = DSpGetVersion();

            // Merge the version number
            uVersion = (MyVersion.majorRev << 8) & 0xFF00U;
            uVersion |= (MyVersion.minorAndBugRev) & 0xFF;
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

    \brief Return the version of the Sound Manager.

    Ask the MacOS Sound Manager what version it is and return that value. The
    function caches the value so subsequent calls do not ask the Sound Manager
    for the version again.

    \return Version in the format of 0x0102 -> 1.2

    \note If the sound manager is not installed (Very old mac models), it will
        return 0.

***************************************/

uint_t BURGER_API Burger::Mac::GetSoundManagerVersion(void)
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

void BURGER_API Burger::Mac::KillProcess(ProcessSerialNumber* pVictim)
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

void BURGER_API Burger::Mac::KillAllProcesses(void)
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

uint_t BURGER_API Burger::Mac::StartDrawSprocket(void)
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

void BURGER_API Burger::Mac::StopDrawSprocket(void)
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

uint_t BURGER_API Burger::Mac::IsRunningUnderMacOSX(void)
{
    return (Globals::GetMacOSVersion() >= 0x1000U);
}

/*! ************************************

    \brief Get the version of NavServices

    Ask NavServices if it exists and return the version.

    \return 32 bit version number or 0 if NavServices is not available.

***************************************/

uint32_t BURGER_API Burger::Mac::GetNavServicesVersion(void)
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

uint32_t BURGER_API Burger::Mac::GetControlStripVersion(void)
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

#endif
