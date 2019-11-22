/***************************************

	Global variable manager, MacOSX version

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brfilename.h"
#include "broutputmemorystream.h"
#include "brstring.h"
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSHelpManager.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSMenuItem.h>
#import <AppKit/NSWindow.h>
#import <ApplicationServices/ApplicationServices.h>
#include <AvailabilityMacros.h>
#import <CoreFoundation/CFBundle.h>
#import <Foundation/NSInvocation.h>
#import <Foundation/NSString.h>
#import <IOKit/graphics/IOGraphicsLib.h>
#import <IOKit/hid/IOHIDBase.h>
#import <IOKit/hid/IOHIDManager.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <unistd.h>

//
// setAppleMenu disappeared from the headers in 10.4 Re-expose the hidden call
//

#if defined(MAC_OS_X_VERSION_10_4) && !defined(DOXYGEN)
@interface NSApplication (NSAppleMenu)
- (void)setAppleMenu:(NSMenu*)menu;
@end
#endif

/*! ************************************

	\brief Add a new NSMenu to the application menu bar (MacOSX Only)

	Given a created NSMenu, add the item as a submenu to the main menu bar

	\macosxonly
	\param pNSMenu A valid pointer to an NSMenu
	\sa CreateApplicationMenu(), CreateWindowMenu(), CreateViewMenu() or
		CreateHelpMenu()

***************************************/

void BURGER_API Burger::Globals::AddToMenubar(NSMenu* pNSMenu)
{
	NSMenuItem* pTempMenuItem =
		[[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[pTempMenuItem setSubmenu:pNSMenu];
	NSApplication* pApp = NSApp;
	[[pApp mainMenu] addItem:pTempMenuItem];
	[pTempMenuItem release];
}

/*! ************************************

	\brief Query the application's plist for the Bundle name

	To determine the name of the application for updating menus or other systems
	that need the application's for display, call this function which checks
	"CFBundleDisplayName" first, "CFBundleName" second and the processName last.

	\note Don't release the returned NSString!

	\macosxonly
	\return Pointer to a NSString * with the name if found, \ref NULL if not.

***************************************/

NSString* BURGER_API Burger::Globals::GetApplicationName(void)
{
	// Get access to the info.plist
	NSBundle* pBundle = [NSBundle mainBundle];
	// Try the requested name
	NSString* pResult =
		[pBundle objectForInfoDictionaryKey:@"CFBundleDisplayName"];
	if (!pResult) {
		pResult = [pBundle objectForInfoDictionaryKey:@"CFBundleName"];
	}
	// Since there's no info.plist, just use the executable's name
	if (!pResult || ![pResult length]) {
		pResult = [[NSProcessInfo processInfo] processName];
	}
	return pResult;
}

/*! ************************************

	\brief Add a new application menu for to the application menu bar (MacOSX
	Only)

	Creates an NSMenu with the name of the application and add a several
	entries.

	"About %s" with no hot key and calls orderFrontStandardAboutPanel
	"Preferences…" with a hot key of Option-',' and has no action
	"Services" with no hot key and calls no action and is attached to
		setServicesMenu
	"Hide %s" with no hot key and calls hide
	"Hide others" with no hot key and calls hideOtherApplications
	"Show All" with no hot key and calls unhideAllApplications
	"Quit %s" with a hot key of Option-Q and calls terminate

	\macosxonly
	\sa AddToMenubar(), CreateWindowMenu(), CreateViewMenu() or CreateHelpMenu()

***************************************/

void BURGER_API Burger::Globals::CreateApplicationMenu(void)
{
	NSString* pNSApplicationName = GetApplicationName();
	// Create the menu list for the application
	NSMenu* pAppMenu = [[NSMenu alloc] initWithTitle:@"Apple Menu"];
	NSString* pMenuText =
		[[NSString alloc] initWithFormat:@"About %@", pNSApplicationName];
	[pAppMenu addItemWithTitle:pMenuText
						action:@selector(orderFrontStandardAboutPanel:)
				 keyEquivalent:@""];
	[pAppMenu addItem:[NSMenuItem separatorItem]];
	[pMenuText release];

	[pAppMenu addItemWithTitle:@"Preferences…" action:nil keyEquivalent:@","];

	[pAppMenu addItem:[NSMenuItem separatorItem]];

	NSMenu* pServiceMenu = [[NSMenu alloc] initWithTitle:@""];
	NSMenuItem* pMenuItem =
		[pAppMenu addItemWithTitle:@"Services" action:nil keyEquivalent:@""];
	[pMenuItem setSubmenu:pServiceMenu];
	[NSApp setServicesMenu:pServiceMenu];
	[pServiceMenu release];

	[pAppMenu addItem:[NSMenuItem separatorItem]];

	pMenuText =
		[[NSString alloc] initWithFormat:@"Hide %@", pNSApplicationName];
	[pAppMenu addItemWithTitle:pMenuText
						action:@selector(hide:)
				 keyEquivalent:@"h"];
	[pMenuText release];

	pMenuItem = [pAppMenu addItemWithTitle:@"Hide Others"
									action:@selector(hideOtherApplications:)
							 keyEquivalent:@"h"];
	[pMenuItem
		setKeyEquivalentModifierMask:(NSAlternateKeyMask | NSCommandKeyMask)];

	[pAppMenu addItemWithTitle:@"Show All"
						action:@selector(unhideAllApplications:)
				 keyEquivalent:@""];

	[pAppMenu addItem:[NSMenuItem separatorItem]];

	pMenuText =
		[[NSString alloc] initWithFormat:@"Quit %@", pNSApplicationName];
	[pAppMenu addItemWithTitle:pMenuText
						action:@selector(terminate:)
				 keyEquivalent:@"q"];
	[pMenuText release];

	// Set as the application menu
	[NSApp setAppleMenu:pAppMenu];
	AddToMenubar(pAppMenu);
	[pAppMenu release];
}

/*! ************************************

	\brief Add a new view menu for window selection to the application menu bar
	(MacOSX Only)

	Creates an NSMenu called "Window" and add a several entries.

	"Minimize" with a hot key of Option-M and calls performMiniaturize
	"Zoom" with no hot key and calls performZoom
	"Bring All to Front" with no hot key and calls arrangeInFront

	\macosxonly
	\sa AddToMenubar(), CreateApplicationMenu(), CreateViewMenu() or
		CreateHelpMenu()

***************************************/

void BURGER_API Burger::Globals::CreateWindowMenu(void)
{
	// Create the window menu (Copied from the layout in Finder and XCode)
	NSMenu* pWindowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
	// Add items
	[pWindowMenu addItemWithTitle:@"Minimize"
						   action:@selector(performMiniaturize:)
					keyEquivalent:@"m"];
	[pWindowMenu addItemWithTitle:@"Zoom"
						   action:@selector(performZoom:)
					keyEquivalent:@""];
	[pWindowMenu addItem:[NSMenuItem separatorItem]];
	[pWindowMenu addItemWithTitle:@"Bring All to Front"
						   action:@selector(arrangeInFront:)
					keyEquivalent:@""];

	// Tell the application manager that this is the Windows menu
	[NSApp setWindowsMenu:pWindowMenu];
	AddToMenubar(pWindowMenu);
	[pWindowMenu release];
}

/*! ************************************

	\brief Add a new view menu for full screen toggling to the application menu
		bar (MacOSX Only)

	Creates an NSMenu called "View" and add a single entry: "Toggle Full
	Screen". The menu item triggers the selector toggleFullScreen when invoked.
	The hot key is Option-Enter (ALT-Enter)

	This function tests if it's running on 10.6 or higher, if this test fails,
	the function does nothing.

	\macosxonly
	\sa AddToMenubar(), CreateApplicationMenu(), CreateWindowMenu() or
		CreateHelpMenu()

***************************************/

void BURGER_API Burger::Globals::CreateViewMenu(void)
{
	// Detect if this is 10.6 or higher
	if ([NSApp respondsToSelector:@selector(setPresentationOptions:)]) {
		// Create the view menu
		NSMenu* pViewMenu = [[NSMenu alloc] initWithTitle:@"View"];
		// Add the hot key of Open Apple \r to toggle full screen
		NSMenuItem* pViewMenuItem =
			[pViewMenu addItemWithTitle:@"Toggle Full Screen"
								 action:@selector(toggleFullScreen:)
						  keyEquivalent:@"\r"];
		[pViewMenuItem setKeyEquivalentModifierMask:NSAlternateKeyMask];
		AddToMenubar(pViewMenu);
		[pViewMenu release];
	}
}

/*! ************************************

	\brief Add a new help menu with search to the application menu bar (MacOSX
	Only)

	Creates an NSMenu called "Help" and adds two entries, the first is the
	"Search" text entry field, followed by a menu item of "%s Help" where
	%s is the pApplicationName and it called the showHelp selector when
	invoked.

	\macosxonly
	\sa AddToMenubar(), CreateApplicationMenu(), CreateWindowMenu() or
		CreateViewMenu()

***************************************/

void BURGER_API Burger::Globals::CreateHelpMenu(void)
{
	// Create the window menu (Copied from the layout in Finder and XCode)
	NSString* pNSApplicationName = GetApplicationName();
	NSMenu* pHelpMenu = [[NSMenu alloc] initWithTitle:@"Help"];
	NSString* pMenuText =
		[NSString stringWithFormat:@"%@ Help", pNSApplicationName];
	[pHelpMenu addItemWithTitle:pMenuText
						 action:@selector(showHelp:)
				  keyEquivalent:@"?"];
	// Set this as the help menu
#if defined(MAC_OS_X_VERSION_10_6)
	[NSApp setHelpMenu:pHelpMenu];
#endif
	AddToMenubar(pHelpMenu);
	[pHelpMenu release];
}

/*! ************************************

	\brief Create a new application menu bar (MacOSX Only)

	If there's no [NSApp mainMenu], create one.

	\macosxonly
	\sa CreateApplicationMenu(), CreateWindowMenu(), CreateViewMenu() or
		CreateHelpMenu()

***************************************/

void BURGER_API Burger::Globals::CreateDefaultMenus(void)
{
	NSApplication* pApp = NSApp;
	if (pApp != nil) {
		// Create the application menu
		NSMenu* pMenu = [pApp mainMenu];
		if (pMenu == nil) {
			// Ensure no memory leaks!
			NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
			pMenu = [[NSMenu alloc] initWithTitle:@""];
			[pApp setMainMenu:pMenu];
			[pMenu release];
			// Create the application menu
			CreateApplicationMenu();
			// Create the window menu
			CreateWindowMenu();
			// Add the full screen toggle menu if possible
			CreateViewMenu();
			// Add in the help menu
			CreateHelpMenu();
			// Resolve the menu bar
			[pApp finishLaunching];
			[pPool release];
		}
	}
}

/*! ************************************

	\brief Convert an NSString to a Burger::String (MacOSX and Carbon Only)

	Given a valid const NSString, convert the string into UTF8 encoding and
	store the result into an output \ref String.

	\macosxonly
	\param pOutput Pointer to a String class instance to recieve the string
	\param pInput Pointer to a constant NSString or CFString to convert

***************************************/

void BURGER_API Burger::Globals::StringCopy(String* pOutput, CFStringRef pInput)
{
	// Try the easy way the just yank a "C" string pointer out directly
	const char* pResult = CFStringGetCStringPtr(pInput, kCFStringEncodingUTF8);
	if (pResult) {
		// Piece of cake!
		pOutput->Set(pResult);
	} else {
		// Crap, it's not encoded in UTF8 (Likely UTF-16)

		// Get the length of the string in UTF16 characters
		CFIndex uLength = CFStringGetLength(pInput);
		if (!uLength) {
			// If it's empty, leave now
			pOutput->Clear();
		} else {
			// Determine the maximum buffer that would be needed for conversion
			// to UTF-8
			CFIndex uMaxLength = CFStringGetMaximumSizeForEncoding(
				uLength, kCFStringEncodingUTF8);
			// Create the buffer
			pOutput->SetBufferSize(static_cast<WordPtr>(uMaxLength));
			// Convert the string and store into the buffer
			if (!CFStringGetCString(pInput, pOutput->GetPtr(), uMaxLength + 1,
					kCFStringEncodingUTF8)) {
				// Lovely, failure
				pOutput->Clear();
			} else {
				// Truncate the string to fit the final string
				// Note: Due to the manual copy, don't assume
				// pOutput->GetLength() returns a valid value. Once
				// SetBufferSize() completes, the length is correct
				pOutput->SetBufferSize(StringLength(pOutput->GetPtr()));
			}
		}
	}
}

/*! ************************************

	\brief Obtain the name of an HID device (MacOSX Only)

	Given an IOHIDDeviceRef, query for a device name, and if none is found, a
	manufacturer name and store it in the output.

	\macosxonly
	\param pOutput Pointer to a String class instance to recieve the string
	\param pDevice Pointer to a valid IOHIDDeviceRef

***************************************/

void BURGER_API Burger::Globals::GetHIDDeviceName(
	String* pOutput, IOHIDDeviceRef pDevice)
{
	CFStringRef pCFString = static_cast<CFStringRef>(
		IOHIDDeviceGetProperty(pDevice, CFSTR(kIOHIDProductKey)));
	if (!pCFString) {
		// Rare case when the device doesn't have a name, try the manufacturer
		pCFString = static_cast<CFStringRef>(
			IOHIDDeviceGetProperty(pDevice, CFSTR(kIOHIDManufacturerKey)));
	}
	if (!pCFString) {
		// What???? Surrender
		pOutput->Set("Unknown device");
	} else {
		StringCopy(pOutput, pCFString);
		// Note: We "got" the string, so releasing isn't needed
		// CFRelease(pCFString);
	}
}

/*! ************************************

	\brief Create a MutableDictionary for use to the HID Manager (MacOSX Only)

	Given a Page and Usage key values, create a dictionary for passing into the
	IOHID Kit manager.

	\macosxonly
	\param uPage Value to associate with kIOHIDDeviceUsagePageKey
	\param uUsage Value to associate with kIOHIDDeviceUsageKey
	\return \ref NULL if failed, or a valid CFMutableDictionaryRef value

***************************************/

CFMutableDictionaryRef BURGER_API Burger::Globals::CreateHIDDictionary(
	Word uPage, Word uUsage)
{
	CFMutableDictionaryRef pDictionary =
		CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
			&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFMutableDictionaryRef pResult = NULL;
	// Did it allocate?
	if (pDictionary) {
		// Store in a known int (to ensure code portability
		int iValue = static_cast<int>(uPage);
		CFNumberRef pNumber =
			CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &iValue);
		// Uh oh...
		if (pNumber) {
			// Set the device usage value
			CFDictionarySetValue(
				pDictionary, CFSTR(kIOHIDDeviceUsagePageKey), pNumber);
			// Yield control to the dictionary
			CFRelease(pNumber);
			// Store in a known int
			iValue = static_cast<int>(uUsage);
			pNumber =
				CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &iValue);
			if (pNumber) {
				CFDictionarySetValue(
					pDictionary, CFSTR(kIOHIDDeviceUsageKey), pNumber);
				// Yield control
				CFRelease(pNumber);
				// The dictionary is okay. Return it
				pResult = pDictionary;
			}
		}
		// It didn't complete?
		if (!pResult) {
			// Dispose of the dictionary
			CFRelease(pDictionary);
		}
	}
	// Return NULL or the dictionary
	return pResult;
}

/*! ************************************

	\brief Get the name of a monitor (MacOSX Only)

	Given a CGDirectDisplayID, return the name of a monitor. If the monitor has
	no name, in cases such as if it's a Mac without a monitor, it will be set to
	"Inactive"

	\macosxonly
	\param pOutput Pointer to the String to receive the name of the monitor
	\param uDisplayID CGDirectDisplayID of the monitor to obtain the name from

***************************************/

void Burger::Globals::GetDisplayName(String* pOutput, Word uDisplayID)
{
	CFDictionaryRef rInfoDictionary = IODisplayCreateInfoDictionary(
		CGDisplayIOServicePort(uDisplayID), kIODisplayOnlyPreferredName);

	CFDictionaryRef rNameList = static_cast<CFDictionaryRef>(
		CFDictionaryGetValue(rInfoDictionary, CFSTR(kDisplayProductName)));

	CFStringRef rMonitorName;
	if (!rNameList ||
		!CFDictionaryGetValueIfPresent(rNameList, CFSTR("en_US"),
			reinterpret_cast<const void**>(&rMonitorName))) {
		// Name not present? Possible it's a mac without a monitor or display
		pOutput->Set("Inactive");
	} else {
		StringCopy(pOutput, rMonitorName);
	}
	// Release the dictionary
	CFRelease(rInfoDictionary);
}

/*! ************************************

	\brief Get an integer from a Dictionary (MacOSX Only)

	Given a CFDictionaryRef and a query key, look up the item in the dictionary
	and if present, return the value as an integer. Returns zero on failure.

	\macosxonly
	\param pDictionary CFDictionaryRef of the dictionary to query
	\param pKey "C" string of the data to query
	\return Zero on failure or the integer value found in the dictionary

***************************************/

int Burger::Globals::NumberFromKey(
	CFDictionaryRef pDictionary, const char* pKey)
{
	// Convert the key to a string
	CFStringRef rString = CFStringCreateWithCStringNoCopy(
		NULL, pKey, kCFStringEncodingUTF8, kCFAllocatorNull);

	// Perform the query
	CFNumberRef pValue =
		static_cast<CFNumberRef>(CFDictionaryGetValue(pDictionary, rString));

	// Release the string
	CFRelease(rString);

	// Assume failure
	int iResult = 0;
	if (pValue) {
		// Convert the number
		CFNumberGetValue(pValue, kCFNumberIntType, &iResult);
	}
	return iResult;
}

/***************************************

	\brief Return the version of MacOS.

	Ask MacOS what version it is and return that value.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

Word BURGER_API Burger::Globals::GetMacOSVersion(void)
{
	int selector[2];

	// Create a machine information block

	selector[0] = CTL_KERN;
	selector[1] = KERN_OSRELEASE;
	size_t uLength = 0;

	// Get the size of the string
	int iError = sysctl(selector, 2, NULL, &uLength, NULL, 0);
	Word uResult = 0;
	if (!iError) {
		//
		// Space for the string
		//
		String Buffer;
		Buffer.SetBufferSize(uLength);
		// Make sure the buffer was allocated
		if (Buffer.GetLength() == uLength) {
			// Get the string
			iError = sysctl(selector, 2, Buffer.GetPtr(), &uLength, NULL, 0);

			//
			// String is in the format of 9.8.0 where
			// 10 is assumed, 9 is the minor version (+4) and
			// 8 is the patch version. 9.8.0 translates
			// to 1058 (10.5.8)

			const char* pBuffer = Buffer.GetPtr();
			const char* pEnd;
			Word32 uMajor = AsciiToInteger(pBuffer, &pEnd);
			if (pEnd != pBuffer) {
				pBuffer = pEnd + 1;
				Word32 uMinor = AsciiToInteger(pBuffer, &pEnd);
				if (pEnd != pBuffer) {
					uResult = 0x1000 + ((uMajor - 4) << 4) + uMinor;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
	// Convert the string to a URL
	CFURLRef URLReference =
		CFURLCreateWithBytes(NULL, reinterpret_cast<const UInt8*>(pURL),
			StringLength(pURL), kCFStringEncodingUTF8, NULL);
	OSStatus uResult = LSOpenCFURLRef(URLReference, 0);
	CFRelease(URLReference);
	return static_cast<Word>(uResult);
}

/***************************************

	Execute a tool and capture the text output

***************************************/

int BURGER_API Burger::Globals::ExecuteTool(
	const char* pFilename, const char* pParameters, OutputMemoryStream* pOutput)
{
	// Convert to a filename
	Filename ProgramName(pFilename);
	// Create a command line
	String CommandLine("\"", ProgramName.GetNative(), "\" ", pParameters);
	// Assume an error
	int iResult = 10;
	// Call the program
	FILE* pPipe = popen(CommandLine.GetPtr(), "r");
	if (pPipe) {
		// Get the output
		while (!feof(pPipe)) {
			char Buffer[1024];
			size_t uRead = fread(Buffer, 1, 1024, pPipe);
			if (pOutput) {
				// Keep it, or discard it
				pOutput->Append(Buffer, uRead);
			}
		}
		// Wait for the program to complete
		iResult = pclose(pPipe);
	}
	// Return the result
	return iResult;
}

/***************************************

	\brief Return the version of QuickTime.

	Detect if QuickTime is available, and if so, query it for the version
	present. If QuickTime is not available, the version returned is zero.

	This function is written so it only asks for the version once from
	QuickTime. It will cache the version and return the cached value on
	subsequent calls.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

Word BURGER_API Burger::Globals::GetQuickTimeVersion(void)
{
	Globals* pGlobals = &g_Globals; // Get the pointer to the singleton
	if (!pGlobals->m_bQuickTimeVersionValid) {
		pGlobals->m_bQuickTimeVersionValid = TRUE; // I got the version
		SInt32 gestaltAnswer;
		Word uResult = 0;
		if (!Gestalt(gestaltQuickTimeVersion, &gestaltAnswer)) {
			uResult = (gestaltAnswer >> 16) & 0xFFFFU; // Major version
		}
		pGlobals->m_uQuickTimeVersion = uResult;
	}
	return pGlobals->m_uQuickTimeVersion; // Return the quicktime version
}

/***************************************

	Read an environment variable as UTF8

***************************************/

const char* BURGER_API Burger::Globals::GetEnvironmentString(const char* pKey)
{
	const char* pValue = getenv(pKey);
	if (pValue) {
		pValue = StringDuplicate(pValue);
	}
	return pValue;
}

/***************************************

	Set an environment variable with a UTF8 string

***************************************/

Word BURGER_API Burger::Globals::SetEnvironmentString(
	const char* pKey, const char* pInput)
{
	// Pass to the operating system
	return static_cast<Word>(setenv(pKey, pInput, TRUE));
}

#endif
