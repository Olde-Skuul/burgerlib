/***************************************

	Debug manager

	MacOS specific version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_MAC)
#include "brcriticalsection.h"
#include "brfile.h"
#include "brmemoryfunctions.h"
#include "broscursor.h"
#include "brstring16.h"

#include <Dialogs.h>
#include <Quickdraw.h>

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

static const uint8_t g_OkTemplate[] = {0x00, 0x01, // 2 items in the list
	0x00, 0x00, 0x00, 0x00,                        // Nothing
	0x00, 160, 0x00, 141, 0x00, 180, 0x00,
	209, // Rect for the OK button Width 68
	0x04, 0x02, 'O', 'K', 0x00, 0x00, 0x00, 0x00, 0x00, 20, 0x00, 20, 0x00, 140,
	0x01, 330 - 256, // Width 310
	0x88};

void BURGER_API Burger::OkAlertMessage(
	const char* pMessage, const char* pTitle) BURGER_NOEXCEPT
{
	// Save the current grafport
	GrafPtr MyPort;
	GetPort(&MyPort);

	Rect WorkRect;

	// Center my dialog to the screen
#if ACCESSOR_CALLS_ARE_FUNCTIONS
	GetPortBounds(MyPort, &WorkRect);
#else
	WorkRect = MyPort->portRect;
#endif

	// Rect of the dialog window
	Rect DialogRect;
	DialogRect.top = static_cast<short>(
		(((WorkRect.bottom - WorkRect.top) - 190) / 2) + WorkRect.top);
	DialogRect.left = static_cast<short>(
		(((WorkRect.right - WorkRect.left) - 350) / 2) + WorkRect.left);
	DialogRect.bottom = static_cast<short>(DialogRect.top + 190);
	DialogRect.right = static_cast<short>(DialogRect.left + 350);

	// Assume no length
	uintptr_t uTitleLen = 0;
	if (pTitle) {
		// Get the length of the title string
		uTitleLen = StringLength(pTitle);
	}

	// Convert the title to a pascal string
	Str255 PascalTitle;
	CStringToPString(PascalTitle, pTitle);

	// Size of the message
	uint_t uMessageLength = StringLength(pMessage);
	Handle hItemList =
		NewHandle(static_cast<Size>(sizeof(g_OkTemplate) + 1 + uMessageLength));

	// Ok?
	if (hItemList) {
		// Copy the template
		MemoryCopy(*hItemList, g_OkTemplate, sizeof(g_OkTemplate));

		// Copy the message
		CStringToPString(
			reinterpret_cast<uint8_t*>(*hItemList) + sizeof(g_OkTemplate),
			pMessage);

		short ItemHit; // Junk

		DialogPtr pDialog = NewDialog(0, &DialogRect, PascalTitle, TRUE,
			movableDBoxProc, (WindowPtr)-1, FALSE, 0, hItemList);
		if (pDialog) {
			SetDialogDefaultItem(pDialog, 1); // Default for OK button
			ModalDialog(0, &ItemHit);         // Handle the event
			DisposeDialog(pDialog);           // Kill the dialog
		} else {
			DisposeHandle(hItemList); // I must kill this myself!
		}
	}

	// Restore my grafport
	SetPort(MyPort);
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

static const uint8_t g_OkCancelTemplate[] = {0x00, 0x02, // 3 items in the list
	0x00, 0x00, 0x00, 0x00,                              // Nothing
	0x00, 160, 0x01, 260 - 256, 0x00, 180, 0x01,
	328 - 256, // Rect for the OK button Width 68
	0x04, 0x02, 'O', 'K', 0x00, 0x00, 0x00, 0x00, // Nothing
	0x00, 160, 0x00, 180, 0x00, 180, 0x00,
	248, // Rect for the OK button Width 68
	0x04, 0x06, 'C', 'a', 'n', 'c', 'e', 'l', 0x00, 0x00, 0x00, 0x00, 0x00, 20,
	0x00, 20, 0x00, 140, 0x01, 330 - 256, // Width 310
	0x88};

uint_t BURGER_API Burger::OkCancelAlertMessage(
	const char* pMessage, const char* pTitle) BURGER_NOEXCEPT
{

	// Save the current grafport
	GrafPtr MyPort;
	GetPort(&MyPort);

	Rect WorkRect;

	// Center my dialog to the screen
#if ACCESSOR_CALLS_ARE_FUNCTIONS
	GetPortBounds(MyPort, &WorkRect);
#else
	WorkRect = MyPort->portRect;
#endif

	// Place the dialog in the center of the screen
	Rect DialogRect;
	DialogRect.top = static_cast<short>(
		(((WorkRect.bottom - WorkRect.top) - 190) / 2) + WorkRect.top);
	DialogRect.left = static_cast<short>(
		(((WorkRect.right - WorkRect.left) - 350) / 2) + WorkRect.left);
	DialogRect.bottom = static_cast<short>(DialogRect.top + 190);
	DialogRect.right = static_cast<short>(DialogRect.left + 350);

	// Assume no length
	uintptr_t uTitleLen = 0;
	if (pTitle) {
		// Get the length of the title string
		uTitleLen = StringLength(pTitle);
	}

	// Convert the title to a pascal string
	Str255 PascalTitle;
	CStringToPString(PascalTitle, pTitle);

	// Assume cancel
	uint_t bResult = FALSE;

	// Size of the message
	uintptr_t uMessageLen = StringLength(pMessage);
	Handle hItemList = NewHandle(
		static_cast<Size>(sizeof(g_OkCancelTemplate) + 1 + uMessageLen));
	// Ok?
	if (hItemList) {
		MemoryCopy(*hItemList, g_OkCancelTemplate,
			sizeof(g_OkCancelTemplate)); // Copy the template
		CStringToPString(
			reinterpret_cast<uint8_t*>(*hItemList) + sizeof(g_OkCancelTemplate),
			pMessage); // Copy the message
		short ItemHit; // Junk
		DialogPtr pDialog = NewDialog(0, &DialogRect, PascalTitle, TRUE,
			movableDBoxProc, (WindowPtr)-1, FALSE, 0, hItemList);
		if (pDialog) {
			SetDialogDefaultItem(pDialog, 1); // Default for OK button
			SetDialogCancelItem(pDialog, 2);  // Default for cancel button
			ModalDialog(0, &ItemHit);         // Handle the event
			// Pressed ok?
			if (ItemHit == 1) {
				bResult = TRUE;
			}
			// Kill the dialog
			DisposeDialog(pDialog);
		} else {
			// I must kill this myself!
			DisposeHandle(hItemList);
		}
	}
	// Restore my grafport
	SetPort(MyPort);
	return bResult;
}

#endif
