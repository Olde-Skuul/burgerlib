/***************************************

    Debug manager

    Windows specific version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_WINDOWS)
#include "brstring.h"
#include "brstring16.h"
#include "broscursor.h"
#include "brcriticalsection.h"
#include "brfile.h"
#include "brnumberstringhex.h"

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501				// Windows XP
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// Make it thread safe

static Burger::CriticalSectionStatic g_LockString;

void BURGER_API Burger::Debug::PrintString(const char *pString)
{
	// Allow multiple threads to call me!

	if (pString) {
		WordPtr i = StringLength(pString);
		if (i) {
			if (!IsDebuggerPresent()) {
				g_LockString.Lock();
				File MyFile;
				if (MyFile.Open("9:logfile.txt",File::APPEND)==File::OKAY) {
					MyFile.Write(pString,i);		// Send the string to the log file
					MyFile.Close();
				}
			} else {
				g_LockString.Lock();
				// Note: Windows only supports ASCII to the Visual Studio debug console.
				// It does NOT support unicode
				OutputDebugStringA(pString);		// Send to the developer studio console window
			}
			g_LockString.Unlock();
		}
	}
}

/***************************************

	\brief Detect if a debugger is attached

	Return \ref TRUE if a debugger is attached

***************************************/

Word BURGER_API Burger::Debug::IsDebuggerPresent(void)
{
	// This function in Windows is just an accessor, so optimizing
	// it is not necessary
	return static_cast<Word>(::IsDebuggerPresent());
}

/***************************************

	\brief Print the error message for an OS error code

	Given an error code from the native operating system and print
	it out the \ref Debug messaging system.

	\param uErrorCode Error code from Windows/MacOS/etc...

***************************************/

void BURGER_API Burger::Debug::PrintErrorMessage(Word uErrorCode)
{
	// Print the error string
	PrintString("Windows error: 0x");

	// Show the error in hex
	NumberStringHex TempBuffer(static_cast<Word32>(uErrorCode));
	PrintString(TempBuffer);

	// Convert to a windows string in the native language
	char *pBuffer = NULL;
	if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,uErrorCode,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),static_cast<LPSTR>(static_cast<void *>(&pBuffer)),0,NULL)) {
		PrintString(", ");
		PrintString(pBuffer);
		LocalFree(pBuffer);
	} else {
		PrintString("\n");
	}
}

/***************************************

	\brief Display a dialog box
	
	On platforms that support pop up dialogs, display a dialog
	that has an "Okay" button

	On platforms that do not support pop up dialogs, the messages are logged

	\param pMessage Message to print in the center of the dialog box
	\param pTitle Pointer to "C" string or \ref NULL for a message in the title bar
	\sa OkCancelAlertMessage() or Debug::String(const char *)

***************************************/

void BURGER_API Burger::OkAlertMessage(const char *pMessage,const char *pTitle)
{
	// Make sure that the OS cursor is visible otherwise the user will
	// wonder what's up when the user can't see the cursor to click the button

	Word bVisible = OSCursor::Show();
	HWND hFrontWindow = GetForegroundWindow();
	SetForegroundWindow(GetDesktopWindow());
	
	// Convert UTF-8 to UTF-16
	String16 Message(pMessage);
	String16 Title(pTitle);
	MessageBoxW(GetDesktopWindow(),
		reinterpret_cast<LPCWSTR>(Message.GetPtr()),
		reinterpret_cast<LPCWSTR>(Title.GetPtr()),MB_OK);
	
	// Restore state
	SetForegroundWindow(hFrontWindow);
	OSCursor::Show(bVisible);
}

/***************************************

	\brief Display a dialog to alert the user of a possible error condition or message.

	On platforms that support pop up dialogs, display a dialog
	that has two buttons, one for "Okay" and another for "Cancel"

	On platforms that do not support pop up dialogs, the messages are logged
	and \ref FALSE (Cancel) is always returned

	\param pMessage Pointer to "C" string with the message that asks a question that
	can be answered with Okay or Cancel
	\param pTitle Pointer to "C" string or \ref NULL for a message in the title bar
	\return \ref TRUE if the user pressed "Okay" or \ref FALSE if pressed "Cancel"
	\sa OkAlertMessage() or Debug::String(const char *)

***************************************/

Word BURGER_API Burger::OkCancelAlertMessage(const char *pMessage,const char *pTitle)
{
	// Make sure that the OS cursor is visible otherwise the user will
	// wonder what's up when they can't see the cursor to click the button

	Word bVisible = OSCursor::Show();
	HWND hFrontWindow = GetForegroundWindow();
	SetForegroundWindow(GetDesktopWindow());

	// Convert UTF-8 to UTF-16
	String16 Message(pMessage);
	String16 Title(pTitle);
	Word bResult = MessageBoxW(GetDesktopWindow(),
		reinterpret_cast<LPCWSTR>(Message.GetPtr()),
		reinterpret_cast<LPCWSTR>(Title.GetPtr()),MB_ICONWARNING|MB_OKCANCEL) == IDOK;

	// Restore state
	SetForegroundWindow(hFrontWindow);
	OSCursor::Show(bVisible);
	return bResult;
}

#endif