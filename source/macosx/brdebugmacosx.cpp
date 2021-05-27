/***************************************

    Debug manager

    MacOSX specific version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_MACOSX)
#include "brcriticalsection.h"
#include "brfile.h"
#include "broscursor.h"
#include "brmemoryfunctions.h"
#import <AppKit/NSAlert.h>
#import <AppKit/NSApplication.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSString.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <unistd.h>

// Make it thread safe

static Burger::CriticalSectionStatic g_LockString;
static uint_t g_uDebugger = 0;

/***************************************

	Print to debugger or file

***************************************/

void BURGER_API Burger::Debug::PrintString(const char* pString) BURGER_NOEXCEPT
{
	// Allow multiple threads to call me!

	if (pString) {
		uintptr_t i = StringLength(pString);
		if (i) {
			if (!IsDebuggerPresent()) {
				g_LockString.Lock();
				File MyFile;
				if (MyFile.Open("9:logfile.txt", File::APPEND) == File::OKAY) {
					MyFile.Write(pString, i); // Send the string to the log file
					MyFile.Close();
				}
			} else {
				g_LockString.Lock();
				fwrite(pString, 1, i, stdout); // Output to the debugger window
			}
			g_LockString.Unlock();
		}
	}
}

/***************************************

	\brief Detect if a debugger is attached

	Return \ref TRUE if a debugger is attached

***************************************/

uint_t BURGER_API Burger::Debug::IsDebuggerPresent(void) BURGER_NOEXCEPT
{
	uint_t uResult = g_uDebugger;
	// Already tested?
	if (uResult & (~0x80U)) {

		// Set up for querying the kernel about this process

		int ManagementInfobase[4];
		ManagementInfobase[0] = CTL_KERN;  // Query the kernel
		ManagementInfobase[1] = KERN_PROC; // Asking for a kinfo_proc structure
		ManagementInfobase[2] = KERN_PROC_PID; // This process ID
		ManagementInfobase[3] = getpid();	  // Here's the application's ID

		// Prepare the output structure

		struct kinfo_proc Output;
		MemoryClear(&Output, sizeof(Output));
		size_t uOutputSize = sizeof(Output);

		// Call BSD for the state of the process
		int iResult = sysctl(ManagementInfobase,
			static_cast<u_int>(BURGER_ARRAYSIZE(ManagementInfobase)), &Output,
			&uOutputSize, NULL, 0);
		uResult = 0x80U;
		if (!iResult) {
			// Test for tracing (Debugging)
			uResult |= (Output.kp_proc.p_flag & P_TRACED) != 0;
		}
		// Save the debugger flag
		g_uDebugger = uResult;
	}
	// Return TRUE or FALSE
	return uResult & 1U;
}

/***************************************

	\brief Display a dialog box

	On platforms that support pop up dialogs, display a dialog that has an
	"Okay" button

	On platforms that do not support pop up dialogs, the messages are logged

	\param pMessage Message to print in the center of the dialog box
	\param pTitle Pointer to "C" string or \ref NULL for a message in the title
		bar
	\sa OkCancelAlertMessage() or Debug::String(const char *)

***************************************/

void BURGER_API Burger::OkAlertMessage(const char* pMessage, const char* pTitle)
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

	\sa OkAlertMessage() or Debug::String(const char *)

***************************************/

uint_t BURGER_API Burger::OkCancelAlertMessage(
	const char* pMessage, const char* pTitle)
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
