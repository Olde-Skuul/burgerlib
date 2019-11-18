/***************************************

	Typedefs specific to Mac OS Carbon and Classic

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
struct FSRef;
struct UTCDateTime;
struct CCrsr;
struct GrafPort;
struct OpaqueDialogPtr;
struct FSSpec;
struct OpaqueCFragConnectionID;
struct OpaqueFSIterator;
struct ProcessSerialNumber;
struct OTAddress;
struct OpaqueOTClientContextPtr;

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
	Word m_uAppleShareVersion;				///< Discovered version of AppleShare (Mac only)
	Word m_uInputSprocketVersion;			///< Discovered version of InputSprocket (Mac only)
	Word m_uDrawSprocketVersion;			///< Discovered version of DrawSprocket (Mac only)
	Word m_uSoundManagerVersion;			///< Sound manager version in 0x0102 (1.2) format (Mac only)
	Word8 m_bAppleShareVersionTested;		///< AppleShare version was tested (Mac only)
	Word8 m_bIsQuickTimePlugInTested;		///< Non-zero if tested, low bit has \ref TRUE or \ref FALSE for QuickTime Plugin present (Mac only)
	Word8 m_bInputSprocketVersionTested;	///< InputSprocket version was tested (Mac only)
	Word8 m_bDrawSprocketVersionTested;		///< DrawSprocket version was tested (Mac only)
	Word8 m_bSoundManagerVersionValid;		///< \ref TRUE if sound manager version is valid (Mac only)
	Word8 m_bDrawSprocketActive;			///< \ref TRUE if DrawSprocket was started

	static Mac g_Globals;		///< Singleton instance of the global variables

public:
	static Word BURGER_API IsTrapAvailable(Word uTrapNum);
	static Word BURGER_API IsQuickTimePowerPlugAvailable(void);
	static Word BURGER_API GetAppleShareVersion(void);
	static Word BURGER_API GetInputSprocketVersion(void);
	static Word BURGER_API GetDrawSprocketVersion(void);
	static Word BURGER_API GetSoundManagerVersion(void);
	static void BURGER_API KillProcess(ProcessSerialNumber *pVictim);
	static void BURGER_API KillAllProcesses(void);
	static Word BURGER_API StartDrawSprocket(void);
	static void BURGER_API StopDrawSprocket(void);
	static Word BURGER_API IsRunningUnderMacOSX(void);
};
}

#endif

/* END */

#endif
