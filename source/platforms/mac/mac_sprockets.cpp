/***************************************

	MacOS Game Spockets code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_sprockets.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <DrawSprocket.h>
#include <Folders.h>
#include <InputSprocket.h>
#include <LowMem.h>
#include <Resources.h>
#include <Sound.h>
#include <Traps.h>

#if !defined(DOXYGEN)
// \ref TRUE if DrawSprocket was started
static uint8_t g_bDrawSprocketActive;

#endif

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

uint32_t BURGER_API Burger::MacOS::get_input_sprocket_version(
	void) BURGER_NOEXCEPT
{
// 680x0 CFM and PowerPC have InputSprocket
#if defined(BURGER_CFM)

	// Discovered version of InputSprocket (Cached)
	static uint32_t s_uInputSprocketVersion = UINT32_MAX;

	uint32_t uVersion = s_uInputSprocketVersion;
	if (uVersion == UINT32_MAX) {

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
		s_uInputSprocketVersion = uVersion;
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

uint32_t BURGER_API Burger::MacOS::get_draw_sprocket_version(
	void) BURGER_NOEXCEPT
{
// 680x0 CFM and PowerPC have DrawSprocket
#if defined(BURGER_CFM)

	static const uint8_t g_DebugDrawSpocketName[] = "\pDrawSprocketDebugLib";
	static const uint8_t g_DrawSprocketName[] = "\pDrawSprocketLib";

	// Discovered version of DrawSprocket (Cached)
	static uint32_t s_uDrawSprocketVersion = UINT32_MAX;

	uint32_t uVersion = s_uDrawSprocketVersion;
	if (uVersion == UINT32_MAX) {

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
				VolumeRef, DirIDRef, g_DebugDrawSpocketName, fsRdPerm);

			// Error?
			OSErr ErrorCode = ResError();
			// Let's try again with release
			if (ErrorCode == fnfErr) {
				DrawSprocketResourceRef = HOpenResFile(
					VolumeRef, DirIDRef, g_DrawSprocketName, fsRdPerm);

				// Error?
				ErrorCode = ResError();

				// Try the system folder now
				if (ErrorCode == fnfErr) {

					// At this time, search the extensions folder

					FindFolder(kOnSystemDisk, kExtensionFolderType,
						kDontCreateFolder, &VolumeRef, &DirIDRef);

					// Release build is checked first

					FSSpec DrawSprocketFSSpec;
					FSMakeFSSpec(VolumeRef, DirIDRef, g_DrawSprocketName,
						&DrawSprocketFSSpec);

					DrawSprocketResourceRef =
						FSpOpenResFile(&DrawSprocketFSSpec, fsRdPerm);
					ErrorCode = ResError();
					if (ErrorCode == fnfErr) {

						// Try the debug build next
						FSMakeFSSpec(VolumeRef, DirIDRef,
							g_DebugDrawSpocketName, &DrawSprocketFSSpec);
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
		s_uDrawSprocketVersion = uVersion;
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

uint32_t BURGER_API Burger::MacOS::get_sound_manager_version(
	void) BURGER_NOEXCEPT
{
	/** Sound manager version in 0x0102 (1.2) format (Mac only) */
	static uint32_t s_uSoundManagerVersion = UINT32_MAX;

	uint32_t uVersion = s_uSoundManagerVersion;
	if (uVersion == UINT32_MAX) {

		uVersion = 0;

#if !defined(BURGER_CFM)
		// 68K trap only
		if (GetToolTrapAddress(_SoundDispatch) !=
			GetToolTrapAddress(_Unimplemented))
#else
		// Check the pointer for CFM 68k or PPC
		if (SndSoundManagerVersion != nullptr)
#endif
		{
			// Get the version
			NumVersion MyVersion;
			MyVersion = SndSoundManagerVersion();

			// 8.8 version number
			uVersion = (MyVersion.majorRev << 8) & 0xFF00U;
			uVersion |= static_cast<uint32_t>(MyVersion.minorAndBugRev & 0xFFU);
			// Store the version in cache
			s_uSoundManagerVersion = uVersion;
		}
	}

	// Return the version
	return uVersion;
}

/*! ************************************

	\brief Start DrawSprocket

	If DrawSprocket was not started, test for its existance and initialize the
	library. If DrawSprocket was started successfully or was already started,
	return \ref TRUE, otherwise return \ref FALSE

	\note This call does nothing on non-CFM targets

	\return \ref TRUE on success

***************************************/

uint_t BURGER_API Burger::MacOS::start_draw_sprocket(void) BURGER_NOEXCEPT
{
#if !defined(BURGER_CFM)
	uint_t bResult = FALSE;
#else
	// Did I test for DrawSprocket?
	uint_t bResult = g_bDrawSprocketActive;
	if (!bResult) {

		// Is draw sprocket present?
		if (DSpStartup != nullptr) {
			// Init draw sprocket
			if (!DSpStartup()) {

				// Initialize the blanking color to black
				// (Some Performa computers don't set this
				// on startup)

				RGBColor Black;
				Black.red = 0;
				Black.green = 0;
				Black.blue = 0;
				DSpSetBlankingColor(&Black);

				bResult = TRUE;
				g_bDrawSprocketActive = TRUE;
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

void BURGER_API Burger::MacOS::stop_draw_sprocket(void) BURGER_NOEXCEPT
{
#if defined(BURGER_CFM)
	// Already started?
	if (g_bDrawSprocketActive) {
		g_bDrawSprocketActive = FALSE;

		// Bye bye
		DSpShutdown();
	}
#endif
}

#endif