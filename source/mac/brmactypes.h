/***************************************

    Typedefs specific to Mac OS Carbon and Classic

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without
    paying anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACTYPES_H__
#define __BRMACTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */

#if defined(BURGER_MAC) || defined(DOXYGEN)

#if !defined(DOXYGEN)

struct Point;
struct Rect;
struct MacRegion;
struct BitMap;
struct PixMap;
struct FSRef;
struct UTCDateTime;
struct CCrsr;
struct GrafPort;
struct CGrafPort;
struct OpaqueDialogPtr;
struct FSSpec;
struct OpaqueCFragConnectionID;
struct OpaqueFSIterator;
struct ProcessSerialNumber;
struct OTAddress;
struct OpaqueOTClientContextPtr;
struct AEDesc;

#if TARGET_API_MAC_CARBON
struct __CFBundle;
struct __CFString;
typedef struct OpaqueDialogPtr* DialogRef; ///< Alias for DialogRef for MacOS
#else
typedef struct GrafPort* DialogRef;
#endif

#endif

namespace Burger {
class Mac {
private:
    /** Discovered version of NavServices (Mac only) */
    uint32_t m_uNavServicesVersion;

    /** Discovered version of ControlStrip (Mac only) */
    uint32_t m_uControlStripVersion;

    /** Discovered version of AppleShare (Mac only) */
    uint_t m_uAppleShareVersion;

    /** Discovered version of InputSprocket (Mac only) */
    uint_t m_uInputSprocketVersion;

    /** Discovered version of DrawSprocket (Mac only) */
    uint_t m_uDrawSprocketVersion;

    /** Sound manager version in 0x0102 (1.2) format (Mac only) */
    uint_t m_uSoundManagerVersion;

    /** NavServices version was tested (Mac only) */
    uint8_t m_bNavServicesVersionTested;

    /** ControlStrip version was tested (Mac only) */
    uint8_t m_bControlStripVersionTested;

    /** AppleShare version was tested (Mac only) */
    uint8_t m_bAppleShareVersionTested;

    /** Non-zero if tested, low bit has \ref TRUE or \ref FALSE for QuickTime
     * Plugin present (Mac only) */
    uint8_t m_bIsQuickTimePlugInTested;

    /** InputSprocket version was tested (Mac only) */
    uint8_t m_bInputSprocketVersionTested;

    /** DrawSprocket version was tested (Mac only) */
    uint8_t m_bDrawSprocketVersionTested;

    /** \ref TRUE if sound manager version is valid (Mac only) */
    uint8_t m_bSoundManagerVersionValid;

    /** \ref TRUE if DrawSprocket was started */
    uint8_t m_bDrawSprocketActive;

    /** Singleton instance of the global variables */
    static Mac g_Globals;

public:
    static uint_t BURGER_API IsTrapAvailable(uint_t uTrapNum);
    static uint_t BURGER_API IsQuickTimePowerPlugAvailable(void);
    static uint_t BURGER_API GetAppleShareVersion(void);
    static uint_t BURGER_API GetInputSprocketVersion(void);
    static uint_t BURGER_API GetDrawSprocketVersion(void);
    static uint_t BURGER_API GetSoundManagerVersion(void);
    static void BURGER_API KillProcess(ProcessSerialNumber* pVictim);
    static void BURGER_API KillAllProcesses(void);
    static uint_t BURGER_API StartDrawSprocket(void);
    static void BURGER_API StopDrawSprocket(void);
    static uint_t BURGER_API IsRunningUnderMacOSX(void);
    static uint32_t BURGER_API GetNavServicesVersion(void);
    static uint32_t BURGER_API GetControlStripVersion(void);
};
}

#if defined(BURGER_MACCLASSIC) && !ACCESSOR_CALLS_ARE_FUNCTIONS
extern "C" {
extern void SetQDError(int16_t err);
extern PixMap **GetPortPixMap(CGrafPort* port);
extern const BitMap* GetPortBitMapForCopyBits(CGrafPort* port);
extern MacRegion** GetPortVisibleRegion(CGrafPort* port, MacRegion** visRgn);
}
#endif

#endif

/* END */

#endif
