/***************************************

	Debug manager

	MacOSX specific version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_MACOSX)
#include "brmutex.h"
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "broscursor.h"

#import <AppKit/NSAlert.h>
#import <AppKit/NSApplication.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSString.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <unistd.h>

/***************************************

	\brief Print a string to a file or debugger.

	Given a "C" string, stream the data to a text file, or if a debugger is
	attached, to the debugger console.

	No parsing is done on the string, it's written as is.

	\param pString Pointer to a "C" string to print.

***************************************/

// Make it thread safe

static Burger::MutexStatic g_LockString;

void BURGER_API Burger::Debug::PrintString(const char* pString) BURGER_NOEXCEPT
{
	// Allow multiple threads to call me!

	if (pString) {
		uintptr_t i = string_length(pString);
		if (i) {
			if (!is_debugger_present()) {
				// Send the string to the log file
				g_LockString.lock();
				File MyFile;
				if (MyFile.open("9:logfile.txt", File::kAppend) == kErrorNone) {
					MyFile.write(pString, i);
					MyFile.close();
				}
			} else {
				// Output to the debugger window
				g_LockString.lock();
				fwrite(pString, 1, i, stdout);
			}
			g_LockString.unlock();
		}
	}
}

/***************************************

	\brief Display a dialog box

	On platforms that support pop up dialogs, display a dialog that has an
	"Okay" button

	On platforms that do not support pop up dialogs, the messages are logged

	\param pMessage Message to print in the center of the dialog box
	\param pTitle Pointer to "C" string or \ref nullptr for a message in the
		title bar

	\sa OkCancelAlertMessage() or Debug::PrintString(const char *)

***************************************/

void BURGER_API Burger::OkAlertMessage(
	const char* pMessage, const char* pTitle) BURGER_NOEXCEPT
{
	// Make sure that the OS cursor is visible otherwise the user will
	// wonder what's up when the user can't see the cursor to click the button
	uint_t bVisible = OSCursor::Show();

	// Handle all memory allocations
	NSAutoreleasePool* pMemoryPool = [[NSAutoreleasePool alloc] init];

	// Make sure there's an NSApplication active
	if (!NSApp) {
		[NSApplication sharedApplication];
	}

	// Create the alert dialog
	NSAlert* pAlert = [[[NSAlert alloc] init] autorelease];

	// Information alert
	[pAlert setAlertStyle:NSInformationalAlertStyle];

	// Set the text to the dialog (Already UTF-8)
	[pAlert setMessageText:[NSString stringWithUTF8String:pTitle]];
	[pAlert setInformativeText:[NSString stringWithUTF8String:pMessage]];

	// Add the OK button
	[pAlert addButtonWithTitle:[NSString stringWithUTF8String:"OK"]];

	// Handle the dialog and wait for a press of "OK"
	[pAlert runModal];

	// Release all of the memory
	[pMemoryPool release];
	OSCursor::Show(bVisible);
}

/***************************************

	\brief Display a dialog to alert the user of a possible error condition or
		message.

	On platforms that support pop up dialogs, display a dialog that has two
	buttons, one for "Okay" and another for "Cancel"

	On platforms that do not support pop up dialogs, the messages are logged and
	\ref FALSE (Cancel) is always returned

	\param pMessage Pointer to "C" string with the message that asks a question
		that can be answered with Okay or Cancel
	\param pTitle Pointer to "C" string or \ref NULL for a message in the title
		bar

	\return \ref TRUE if the user pressed "Okay" or \ref FALSE if pressed
		"Cancel"

	\sa OkAlertMessage() or Debug::PrintString(const char *)

***************************************/

uint_t BURGER_API Burger::OkCancelAlertMessage(
	const char* pMessage, const char* pTitle) BURGER_NOEXCEPT
{
	// Make sure that the OS cursor is visible otherwise the user will
	// wonder what's up when he can't see the cursor to click the button
	uint_t bVisible = OSCursor::Show();
	// Handle all memory allocations
	NSAutoreleasePool* pMemoryPool = [[NSAutoreleasePool alloc] init];

	// Make sure there's an NSApplication active
	if (!NSApp) {
		[NSApplication sharedApplication];
	}

	// Create the alert dialog
	NSAlert* pAlert = [[[NSAlert alloc] init] autorelease];

	// Information alert
	[pAlert setAlertStyle:NSCriticalAlertStyle];

	// Set the text to the dialog (Already UTF-8)
	[pAlert setMessageText:[NSString stringWithUTF8String:pTitle]];
	[pAlert setInformativeText:[NSString stringWithUTF8String:pMessage]];

	// Add the OK button
	[pAlert addButtonWithTitle:[NSString stringWithUTF8String:"OK"]];
	[pAlert addButtonWithTitle:[NSString stringWithUTF8String:"Cancel"]];

	// Handle the dialog and wait for a press of "OK" or "Cancel"
	NSInteger iResult = [pAlert runModal];

	// Release all of the memory
	[pMemoryPool release];
	// Return TRUE if pressed okay
	uint_t result = (iResult == NSAlertFirstButtonReturn);
	OSCursor::Show(bVisible);
	return result;
}

#endif
