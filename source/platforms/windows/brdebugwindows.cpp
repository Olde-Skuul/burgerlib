/***************************************

	Debug manager

	Windows specific version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_WINDOWS)
#include "brcriticalsection.h"
#include "brfile.h"
#include "brnumberstringhex.h"
#include "broscursor.h"
#include "brstring.h"
#include "brstring16.h"

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

/***************************************

	\brief Print a string to a file or debugger.

	Given a "C" string, stream the data to a text file, or if a debugger is
	attached, to the debugger console.

	No parsing is done on the string, it's written as is.

	\param pString Pointer to a "C" string to print.

***************************************/

// Make it thread safe

static Burger::CriticalSectionStatic g_LockString;

void BURGER_API Burger::Debug::PrintString(const char* pString) BURGER_NOEXCEPT
{
	// Allow multiple threads to call me!

	if (pString) {
		const uintptr_t i = StringLength(pString);
		if (i) {
			if (!IsDebuggerPresent()) {
				// Send the string to the log file
				g_LockString.Lock();
				File MyFile;
				if (MyFile.open("9:logfile.txt", File::kAppend) == kErrorNone) {
					MyFile.write(pString, i);
					MyFile.close();
				}
			} else {
				g_LockString.Lock();
				// Note: Windows only supports ASCII to the Visual Studio debug
				// console. It does NOT support unicode
				OutputDebugStringA(pString);
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
	// This function in Windows is just an accessor, so optimizing
	// it is not necessary
	return static_cast<uint_t>(::IsDebuggerPresent());
}

/***************************************

	\brief Print the error message for an OS error code

	Given an error code from the native operating system and print
	it out the \ref Debug messaging system.

	\param uErrorCode Error code from Windows/MacOS/etc...

***************************************/

void BURGER_API Burger::Debug::PrintErrorMessage(
	uint_t uErrorCode) BURGER_NOEXCEPT
{
	// Print the error string
	PrintString("Windows error: 0x");

	// Show the error in hex
	const NumberStringHex TempBuffer(uErrorCode);
	PrintString(TempBuffer.c_str());

	// Convert to a windows string in the native language
	char* pBuffer = NULL;
	if (FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr, uErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			static_cast<LPSTR>(static_cast<void*>(&pBuffer)), 0, nullptr)) {
		PrintString(", ");
		PrintString(pBuffer);
		LocalFree(pBuffer);
	} else {
		PrintString("\n");
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

	const uint_t bVisible = OSCursor::Show();
	HWND hFrontWindow = GetForegroundWindow();
	SetForegroundWindow(GetDesktopWindow());

	// Convert UTF-8 to UTF-16
	String16 Message(pMessage);
	String16 Title(pTitle);
	MessageBoxW(GetDesktopWindow(), reinterpret_cast<LPCWSTR>(Message.c_str()),
		reinterpret_cast<LPCWSTR>(Title.c_str()), MB_OK);

	// Restore state
	SetForegroundWindow(hFrontWindow);
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
	// wonder what's up when they can't see the cursor to click the button

	const uint_t bVisible = OSCursor::Show();
	HWND hFrontWindow = GetForegroundWindow();
	SetForegroundWindow(GetDesktopWindow());

	// Convert UTF-8 to UTF-16
	String16 Message(pMessage);
	String16 Title(pTitle);
	const uint_t bResult = MessageBoxW(GetDesktopWindow(),
							   reinterpret_cast<LPCWSTR>(Message.c_str()),
							   reinterpret_cast<LPCWSTR>(Title.c_str()),
							   MB_ICONWARNING | MB_OKCANCEL) == IDOK;

	// Restore state
	SetForegroundWindow(hFrontWindow);
	OSCursor::Show(bVisible);
	return bResult;
}

#endif
