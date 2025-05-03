/***************************************

    Display base class

    MacOSX version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"

#if defined(BURGER_MACOSX)
#include "brdebug.h"
#include "brgameapp.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include <AppKit/NSScreen.h>
#include <AppKit/NSWindow.h>
#include <ApplicationServices/ApplicationServices.h>
#include <AvailabilityMacros.h>
#include <CoreVideo/CVBase.h>
#include <CoreVideo/CVDisplayLink.h>
#include <IOKit/graphics/IOGraphicsLib.h>

/***************************************

	If a window is present, set the text to a specific string

***************************************/

void Burger::Display::SetWindowTitle(const char* pTitle)
{
	NSWindow* pWindow = m_pGameApp->GetWindow();
	// Is the window present?
	if (pWindow) {
		//
		// Create an auto-release pool for memory clean up
		//

		NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
		CFStringRef rString = CFStringCreateWithCStringNoCopy(
			NULL, pTitle, kCFStringEncodingUTF8, kCFAllocatorNull);
		[pWindow setTitle:(NSString*)rString];
		CFRelease(rString);
		[pPool release];
	}
}

//
// Get the default values
//

void BURGER_API Burger::Display::InitGlobals(void)
{
	if (!g_Globals.m_bInitialized) {
		CGDirectDisplayID pMainDisplay = CGMainDisplayID();
		g_Globals.m_uDefaultWidth =
			static_cast<uint_t>(CGDisplayPixelsWide(pMainDisplay));
		g_Globals.m_uDefaultHeight =
			static_cast<int>(CGDisplayPixelsHigh(pMainDisplay));

		// Get the pixel depth and refresh rate

#if defined(MAC_OS_X_VERSION_10_6)
		CGDisplayModeRef pCurrentMode = CGDisplayCopyDisplayMode(pMainDisplay);
		CFStringRef pPixelEncoding =
			CGDisplayModeCopyPixelEncoding(pCurrentMode);
		uint_t uDepth;
		if (CFStringCompare(pPixelEncoding, CFSTR(IO32BitDirectPixels),
				kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			uDepth = 32;
		} else if (CFStringCompare(pPixelEncoding, CFSTR(IO16BitDirectPixels),
					   kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			uDepth = 16;
		} else if (CFStringCompare(pPixelEncoding, CFSTR(IO8BitIndexedPixels),
					   kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			uDepth = 8;
		} else { // error
			uDepth = 0;
		}
		g_Globals.m_uDefaultDepth = uDepth;
		// We're responsible for this
		CFRelease(pPixelEncoding);

		// Frequency
		g_Globals.m_uDefaultHertz =
			static_cast<uint_t>(CGDisplayModeGetRefreshRate(pCurrentMode));
		CGDisplayModeRelease(pCurrentMode);
#else

		// Used for PowerPC version
		CFDictionaryRef pCurrentMode = CGDisplayCurrentMode(pMainDisplay);
		CFNumberRef pNumber = (CFNumberRef)CFDictionaryGetValue(
			pCurrentMode, kCGDisplayBitsPerPixel);
		int iValue;
		CFNumberGetValue(pNumber, kCFNumberIntType, &iValue);
		g_Globals.m_uDefaultDepth = static_cast<uint_t>(iValue);

		pNumber = (CFNumberRef)CFDictionaryGetValue(
			pCurrentMode, kCGDisplayRefreshRate);
		CFNumberGetValue(pNumber, kCFNumberIntType, &iValue);
		g_Globals.m_uDefaultHertz = static_cast<uint_t>(iValue);
#endif

		g_Globals.m_uDefaultTotalWidth = g_Globals.m_uDefaultWidth;
		g_Globals.m_uDefaultTotalHeight = g_Globals.m_uDefaultHeight;

		uint32_t uDisplayCount;
		// Get the number of displays attached to this mac.
		// It CAN be zero
		CGGetOnlineDisplayList(0, NULL, &uDisplayCount);

		g_Globals.m_uDefaultMonitorCount = uDisplayCount;
		g_Globals.m_bInitialized = TRUE;
	}
}

/***************************************

	Given a device, iterate over the modes available and add them to the pOutput

***************************************/

// MacOSX 6 or higher version for Intel and ARM

#if defined(MAC_OS_X_VERSION_10_6)
static void GetResolutions(Burger::Display::VideoCardDescription* pOutput)
{
	// Get the display link to get the refresh rate
	CVDisplayLinkRef pDisplayLink;
	CVDisplayLinkCreateWithCGDisplay(pOutput->m_uDevNumber, &pDisplayLink);
	// Get the list of display modes
	CFArrayRef pModeList =
		CGDisplayCopyAllDisplayModes(pOutput->m_uDevNumber, NULL);
	CFIndex uCount = CFArrayGetCount(pModeList);
	for (CFIndex i = 0; i < uCount; ++i) {
		// Get the mode and query it
		CGDisplayModeRef pDisplayMode = static_cast<CGDisplayModeRef>(
			const_cast<void*>(CFArrayGetValueAtIndex(pModeList, i)));
		Burger::Display::VideoMode_t Entry;
		uint_t bSkip = FALSE;
		// Width and height are trivial
		Entry.m_uWidth = static_cast<uint_t>(CGDisplayModeGetWidth(pDisplayMode));
		Entry.m_uHeight =
			static_cast<uint_t>(CGDisplayModeGetHeight(pDisplayMode));
		Entry.m_uFlags = Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;

		// Hertz is a value on monitors, however, some LCD screens
		// have no refresh rate
		Entry.m_uHertz =
			static_cast<uint_t>(CGDisplayModeGetRefreshRate(pDisplayMode));
		if (!Entry.m_uHertz) {
			CVTime NominalTime =
				CVDisplayLinkGetNominalOutputVideoRefreshPeriod(pDisplayLink);
			if (!(NominalTime.flags & kCVTimeIsIndefinite)) {
				Entry.m_uHertz = static_cast<uint_t>(
					static_cast<double>(NominalTime.timeScale) /
					static_cast<double>(NominalTime.timeValue));
			}
		}
		if (Entry.m_uHertz) {
			Entry.m_uFlags |=
				Burger::Display::VideoMode_t::VIDEOMODE_REFRESHVALID;
		}

		//
		// Apple uses a wacky way to map out pixels.
		//
		CFStringRef PixelTypeString =
			CGDisplayModeCopyPixelEncoding(pDisplayMode);
		if (!CFStringCompare(PixelTypeString, CFSTR(IO16BitDirectPixels), 0)) {
			Entry.m_uDepth = 16;
		} else if (!CFStringCompare(
					   PixelTypeString, CFSTR(IO32BitDirectPixels), 0)) {
			Entry.m_uDepth = 32;
		} else {
			bSkip = TRUE;
		}
		CFRelease(PixelTypeString);
		// Get rid of modes that are "faked" or unsafe
		if (!bSkip) {
			UInt32 IOFlags = CGDisplayModeGetIOFlags(pDisplayMode);
			if (((IOFlags & (kDisplayModeValidFlag | kDisplayModeSafeFlag)) !=
					(kDisplayModeValidFlag | kDisplayModeSafeFlag)) ||
				(IOFlags & kDisplayModeInterlacedFlag) ||
				(IOFlags & kDisplayModeStretchedFlag)) {
				bSkip = TRUE;
			}
		}
		if (!bSkip) {
			pOutput->m_Array.push_back(Entry);
		}
	}
	// Release the list and links
	CFRelease(pModeList);
	CVDisplayLinkRelease(pDisplayLink);
}
#else

//
// MacOSX 10.5 version for PowerPC
// Directly queries the dictionary (10.6 or later uses specific functions)
//

static void GetResolutions(Burger::Display::VideoCardDescription* pOutput)
{
	// Get the display link to get the refresh rate
	CVDisplayLinkRef pDisplayLink;
	CVDisplayLinkCreateWithCGDisplay(pOutput->m_uDevNumber, &pDisplayLink);

	// Get the list of display modes
	CFArrayRef pModeList = CGDisplayAvailableModes(pOutput->m_uDevNumber);
	CFIndex uCount = CFArrayGetCount(pModeList);
	for (CFIndex i = 0; i < uCount; ++i) {

		// Get the mode and query it
		CFDictionaryRef pDisplayMode = static_cast<CFDictionaryRef>(
			const_cast<void*>(CFArrayGetValueAtIndex(pModeList, i)));
		Burger::Display::VideoMode_t Entry;
		uint_t bSkip = FALSE;

		// Width and height are trivial
		Entry.m_uWidth = static_cast<uint_t>(
			Burger::Globals::NumberFromKey(pDisplayMode, "Width"));
		Entry.m_uHeight = static_cast<uint_t>(
			Burger::Globals::NumberFromKey(pDisplayMode, "Height"));
		Entry.m_uFlags = Burger::Display::VideoMode_t::VIDEOMODE_HARDWARE;

		// Hertz is a value on monitors, however, some LCD screens
		// have no refresh rate
		Entry.m_uHertz = static_cast<uint_t>(
			Burger::Globals::NumberFromKey(pDisplayMode, "RefreshRate"));
		if (!Entry.m_uHertz) {
			CVTime NominalTime =
				CVDisplayLinkGetNominalOutputVideoRefreshPeriod(pDisplayLink);
			if (!(NominalTime.flags & kCVTimeIsIndefinite)) {
				Entry.m_uHertz = static_cast<uint_t>(
					static_cast<double>(NominalTime.timeScale) /
					static_cast<double>(NominalTime.timeValue));
			}
		}
		if (Entry.m_uHertz) {
			Entry.m_uFlags |=
				Burger::Display::VideoMode_t::VIDEOMODE_REFRESHVALID;
		}

		Entry.m_uDepth =
			Burger::Globals::NumberFromKey(pDisplayMode, "BitsPerPixel");

		// Get rid of modes that are "faked" or unsafe
		if (!bSkip) {
			uint_t IOFlags = static_cast<uint_t>(
				Burger::Globals::NumberFromKey(pDisplayMode, "IOFlags"));
			if (((IOFlags & (kDisplayModeValidFlag | kDisplayModeSafeFlag)) !=
					(kDisplayModeValidFlag | kDisplayModeSafeFlag)) ||
				(IOFlags & kDisplayModeInterlacedFlag) ||
				(IOFlags & kDisplayModeStretchedFlag)) {
				bSkip = TRUE;
			}
		}
		if (!bSkip) {
			pOutput->m_Array.push_back(Entry);
		}
	}
	// Release the list and links
	CFRelease(pModeList);
	CVDisplayLinkRelease(pDisplayLink);
}
#endif

/***************************************

	Iterate over the displays and get the modes

***************************************/

uint_t Burger::Display::GetVideoModes(ClassArray<VideoCardDescription>* pOutput)
{
	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
	pOutput->clear();

	uint_t uResult = 0; // Assume success
	uint32_t uDisplayCount;

	// Get the number of displays attached to this mac.
	// It CAN be zero
	CGGetOnlineDisplayList(0, NULL, &uDisplayCount);
	if (uDisplayCount) {

		// Create a buffer to the display IDs
		CGDirectDisplayID* pDisplayIDs = static_cast<CGDirectDisplayID*>(
			allocate_memory_clear(uDisplayCount * sizeof(CGDirectDisplayID)));

		// Get the active display count (Some are mirrored or unplugged)
		uint32_t uActiveDisplayCount;
		CGGetOnlineDisplayList(
			uDisplayCount, pDisplayIDs, &uActiveDisplayCount);
		if (uActiveDisplayCount) {

			// Cache the screen count
			const CFArrayRef pNSScreens = (const CFArrayRef)[NSScreen screens];
			CFIndex uScreenCount = CFArrayGetCount(pNSScreens);

			// Iterate over the displays
			const CGDirectDisplayID* pDisplay = pDisplayIDs;
			do {

				// Get the ID of this display
				CGDirectDisplayID uDisplayID = pDisplay[0];

				// Skip sleeping monitors
				if (!CGDisplayIsAsleep(uDisplayID)) {

					// If the display is a mirror, obtain the true display
					// used by NSScreen
					CGDirectDisplayID uNSScreenDisplayID =
						CGDisplayMirrorsDisplay(uDisplayID);
					if (uNSScreenDisplayID == kCGNullDirectDisplay) {
						uNSScreenDisplayID = uDisplayID;
					}

					// Set up the video card description
					VideoCardDescription Entry;
					Globals::GetDisplayName(&Entry.m_MonitorName, uDisplayID);
					Entry.m_uDevNumber = uDisplayID;
					Entry.m_DeviceName = "OpenGL";

					// OpenGL is available?
					if (CGDisplayUsesOpenGLAcceleration(uDisplayID)) {
						Entry.m_uFlags |=
							VideoCardDescription::VIDEOCARD_HARDWARE;
					}

					// Is this the primary display?
					if (CGDisplayIsMain(uDisplayID)) {
						Entry.m_uFlags |=
							VideoCardDescription::VIDEOCARD_PRIMARY;
					}

					// Get the location of the monitor
					CGRect MonitorBounds = CGDisplayBounds(uDisplayID);
					Entry.m_SystemRect.Set(&MonitorBounds);
					Entry.m_CurrentResolution.SetRight(
						static_cast<int>(MonitorBounds.size.width));
					Entry.m_CurrentResolution.SetBottom(
						static_cast<int>(MonitorBounds.size.height));

					// See if there is an NSScreen that's attached to this
					// display

					for (CFIndex i = 0; i < uScreenCount; ++i) {
						NSScreen* pNSScreen =
							static_cast<NSScreen*>(const_cast<void*>(
								CFArrayGetValueAtIndex(pNSScreens, i)));

						NSDictionary* pScreenDictionary =
							[pNSScreen deviceDescription];
						NSNumber* pNumber =
							[pScreenDictionary objectForKey:@"NSScreenNumber"];
						// Matched?
						if ([pNumber unsignedIntegerValue] ==
							uNSScreenDisplayID) {

							// Save the screen pointer
							Entry.m_pNSScreen = pNSScreen;

							// Iterate the video modes
							GetResolutions(&Entry);
							// Save the monitor
							pOutput->push_back(Entry);
							break;
						}
					}
				}
				++pDisplay;
			} while (--uActiveDisplayCount);
		}
		// Release the buffer
		free_memory(pDisplayIDs);
	}
	[pPool release];
	return uResult;
}

#endif
