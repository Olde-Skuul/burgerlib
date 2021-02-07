/***************************************

    Debug manager

    BeOS specific version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_BEOS)
#include <Alert.h>			// Include the Be headers

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
	BAlert *MyAlert = new BAlert(pTitle,pMessage,"Ok");
	MyAlert->Go();			// Execute the message
							// The alert self-destructed
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
	BAlert *MyAlert = new BAlert(pTitle,pMessage,"Cancel","Ok");
	return !MyAlert->Go();			// Execute the message
									// The alert self-destructed
}

#endif
