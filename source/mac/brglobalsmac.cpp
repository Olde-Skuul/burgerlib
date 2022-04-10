/***************************************

    MacOS version of Burger::Globals

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include <Folders.h>
#include <Gestalt.h>
#include <InternetConfig.h>

/*! ************************************

    \brief Return the version of MacOS.

    Ask MacOS what version it is and return that value.

    \return Version in the format of 0x0102 -> 1.2

***************************************/

uint_t BURGER_API Burger::Globals::GetMacOSVersion(void)
{
    Globals* pGlobals = &g_Globals;
    uint_t uVersion;
    if (!pGlobals->m_bMacOSTested) {
        long lAnswer;
        // Get the version with Gestalt
        if (Gestalt(gestaltSystemVersion, &lAnswer)) {
            lAnswer = 0; // Should NEVER execute, failsafe
        }
        // Get the version of the OS in 0x0102 (1.2) format
        uVersion = static_cast<uint_t>(lAnswer & 0xFFFFU);
        pGlobals->m_uMacOSVersion = uVersion;
        pGlobals->m_bMacOSTested = TRUE;
    } else {
        // Use the cached version
        uVersion = pGlobals->m_uMacOSVersion;
    }
    return uVersion;
}

/***************************************

    \brief Return the version of QuickTime.

    Detect if QuickTime is available, and if so, query
    it for the version present. If QuickTime is not available,
    the version returned is zero.

    This function is written so it only asks for the version
    once from QuickTime. It will cache the version and
    return the cached value on subsequent calls.

    \return Version in the format of 0x0102 -> 1.2

***************************************/

uint_t BURGER_API Burger::Globals::GetQuickTimeVersion(void) BURGER_NOEXCEPT
{
    // Get the pointer to the singleton
    Globals* pGlobals = &g_Globals;
    if (!pGlobals->m_bQuickTimeVersionValid) {
        pGlobals->m_bQuickTimeVersionValid = TRUE; // I got the version
        long gestaltAnswer;
        uint_t uResult = 0;
        if (!Gestalt(gestaltQuickTimeVersion, &gestaltAnswer)) {
            uResult = (gestaltAnswer >> 16) & 0xFFFFU; // Major version
        }
        pGlobals->m_uQuickTimeVersion = uResult;
    }
    return pGlobals->m_uQuickTimeVersion; // Return the quicktime version
}

/***************************************

    Load and launch a web page from an address string

***************************************/

uint_t BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
    ICInstance inst;

    OSStatus err = -1;
    if (ICStart != nullptr) {
        err = ICStart(&inst, '????'); // Use your creator code if you have one!
        if (err == noErr) {
#if !defined(BURGER_MACCARBON)
            err = ICFindConfigFile(inst, 0, nil);
            if (err == noErr)
#endif
            {
                long startSel = 0;
                long endSel = static_cast<long>(StringLength(pURL));
                err = ICLaunchURL(inst, "\p", const_cast<char*>(pURL), endSel,
                    &startSel, &endSel);
            }
            ICStop(inst);
        }
    }
    return static_cast<uint_t>(err);
}

/*! ************************************

    \brief Pull an FSSpec from an AppleEvent.

    Given an AppleEvent, index into in and extract an FSSpec.

    \param pList AEDescList pointer of a list of AppleEvents to process
    \param iIndex Entry index into the list.
    \param pFSSpec Pointer to the FSSpec record to receive the file reference.

    \return OSErr of 0 for no error on non-zero OSErr.

***************************************/

int16_t BURGER_API Burger::Globals::GetSpecFromNthDesc(
    AEDesc* pList, long iIndex, FSSpec* pFSSpec)
{
    AEDesc firstDesc;
    AEKeyword ignoreKeyword;

    // Initialize firstDesc
    OSErr uError =
        AEGetNthDesc(pList, iIndex, typeFSS, &ignoreKeyword, &firstDesc);
    if (!uError) {
// Get data only if there was no error.
#if defined(BURGER_MACCARBON)
        uError = AEGetDescData(&firstDesc, pFSSpec, sizeof(*pFSSpec));
#else
        Size uSize = GetHandleSize(firstDesc.dataHandle);
        if (uSize < sizeof(*pFSSpec)) {
            uError = memSCErr;
        } else {
            MemoryCopy(pFSSpec, *(firstDesc.dataHandle), sizeof(*pFSSpec));
        }
#endif
        OSErr uDisposeError = AEDisposeDesc(&firstDesc);
        if (!uError) {
            uError = uDisposeError;
        }
    }
    return uError;
}

#endif
