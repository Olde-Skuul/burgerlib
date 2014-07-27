/***************************************

	Debug manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"
#include "brglobals.h"
#include "brstringfunctions.h"
#include "brnumberstring.h"
#include "brfile.h"
#include <stdarg.h>
#include <stdio.h>

/*! ************************************

	\class Burger::Debug
	\brief Class of functions to assist in debugging

	These thread safe functions will print messages to the
	debug console of the attached debugger and
	if a debugger is not available, the output
	will be logged into a text file

	\sa Burger::Globals
	
***************************************/

/*! ************************************

	\brief A fatal error has occured, print message, then exit.
	
	If the input message pointer is not \ref NULL, then print
	the message string into the global error string buffer
	using printf() rules.
	
	This message, if any, will be output through \ref Burger::Debug::Message()
	which either outputs it to a log file or a debugger's console (If
	a debugger is present and running).
		
	\code
	if (TestError) {
		Burger::Debug::Fatal("Fatal error, file %s was not found.",pFileName);
		return;
	}
	\endcode
	
	\note This will call exit() with a default error code of 1. You
	must call \ref Burger::Globals::SetErrorCode() to change the code to something else.
	
	Secondly, this function won't return unless it was called while
	Burger::Globals::Shutdown(), then this function CAN return, so you must
	add a return statement after calling this function in the case
	that there is a recursion error in progress.
	
	\param pMessage Pointer to a string suitable for printf or \ref NULL.
	
	\sa Burger::Debug::Warning
	
***************************************/

Word BURGER_ANSIAPI Burger::Debug::Fatal(const char *pMessage,...)
{
	if (!Globals::GetExitFlag()) {
		va_list Args;
		if (pMessage) {		// Message to print?
			va_start(Args,pMessage);
			vsprintf(Globals::g_Globals.m_ErrorMsg,pMessage,Args);	// Create the message
			va_end(Args);
			String(Globals::GetErrorMsg());		// Print it
		}
		Globals::Shutdown();	// Exit to OS
	}
	return FALSE;				// Return FALSE for \ref BURGER_ASSERTTEST()
}

/*! ************************************

	\brief Store a non-fatal error message, then return or exit.
	
	If the input message pointer is not \ref NULL, then store
	the message string into the global error string buffer
	using printf() rules. If it's \ref NULL, do nothing to the error message buffer.
	
	If \ref Globals::SetErrorCode() was called with \ref TRUE, this function
	will then treat this warning as an error and call \ref Burger::Debug::Fatal(const char *pFatalMsg,...) and exit.
		
	\code
	if (TestError) {
		Burger::Debug::Warning("Warning, file %s was not found.",pFileName);
		return;
	}
	\endcode
	
	\note The standard behavior is to store the text string and immediately
	return. Only if an explicit call to \ref Globals::SetErrorCode() setting
	it \ref TRUE will change the operation of this procedure.
		
	\param pMessage Pointer to a string suitable for printf or \ref NULL.
	
	\sa Burger::Debug::Fatal(const char *pFatalMsg,...)
	
***************************************/

void BURGER_ANSIAPI Burger::Debug::Warning(const char *pMessage,...)
{
	va_list Args;
	if (pMessage) {						// No message, no error!
		va_start(Args,pMessage);		// Start parm passing
		vsprintf(Globals::g_Globals.m_ErrorMsg,pMessage,Args);		// Create the message
		va_end(Args);					// End parm passing
		// Should the warning be printed?
		if ((Globals::GetTraceFlag()&Globals::TRACE_WARNINGS) ||
			Globals::GetErrorBombFlag()) {
			String(Globals::GetErrorMsg());
		}
	}
	if (Globals::GetErrorBombFlag()) {	// Bomb on ANY Error?
		Fatal(NULL);					// Force a fatal error with the stored message
	}
}


/*! ************************************

	\brief Print a message to the debugger 
	output stream

	Given a "printf()" message string and parameters, print
	the message to the output stream, which is a console or an
	attached debugger

	\sa Debug::String(const char *), Debug::Message(Word32) and Debug::Message(Word64)

***************************************/

void BURGER_ANSIAPI Burger::Debug::Message(const char *pMessage,...)
{
	va_list Args;
	// Local string buffer
	char TempBuffer[2048];
	// No message, no output!
	if (pMessage) {
		// Start parameter passing
		va_start(Args,pMessage);
		// Create the message in a single buffer
		vsprintf(TempBuffer,pMessage,Args);
		// End the parameter passing
		va_end(Args);
		// Actually print the message to the console
		String(TempBuffer);
	}
}

/*! ************************************

	\brief Print a string to a file

***************************************/

#if !defined(BURGER_WINDOWS)
void BURGER_API Burger::Debug::String(const char *pString)
{
	if (pString && Globals::GetExitFlag()) {
		WordPtr i = StringLength(pString);
		if (i) {
			File MyFile;
			if (MyFile.Open("9:LogFile.Txt",File::APPEND)==File::OKAY) {
				MyFile.Write(pString,i);		/* Send the string to the log file */
				MyFile.Close();
			}
		}
	}
}
#endif

/*! ************************************

	\brief Print a Word32 to the debug port
	\sa Debug::String(Word64)

***************************************/

void BURGER_API Burger::Debug::String(Word32 uInput)
{
	NumberString TempBuffer(uInput);
	String(TempBuffer);
}

/*! ************************************

	\brief Print a Word32 to the debug port
	\sa Debug::String(Word32)

***************************************/

void BURGER_API Burger::Debug::String(Word64 uInput)
{
	NumberString TempBuffer(uInput);
	String(TempBuffer);
}

/*! ************************************

	\brief Display a dialog box
	
	On platforms that support pop up dialogs, display a dialog
	that has an "Okay" button

	On platforms that do not support pop up dialogs, the messages are logged

	\param pMessage Message to print in the center of the dialog box
	\param pTitle Pointer to "C" string or \ref NULL for a message in the title bar
	\sa OkCancelAlertMessage() or Debug::String(const char *)

***************************************/

#if (!defined(BURGER_MAC) && !defined(BURGER_WINDOWS) && !defined(BURGER_BEOS)) || defined(DOXYGEN)

void BURGER_API Burger::OkAlertMessage(const char *pMessage,const char *pTitle)
{
	Debug::String("Alert message : ");
	if (pTitle) {
		Debug::String(pTitle);
		Debug::String(", ");
	}
	Debug::String(pMessage);
	Debug::String("\n");
}

#endif

/*! ************************************

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

#if (!defined(BURGER_MAC) && !defined(BURGER_WINDOWS) && !defined(BURGER_BEOS)) || defined(DOXYGEN)

Word BURGER_API Burger::OkCancelAlertMessage(const char *pMessage,const char *pTitle)
{
	Debug::String("Cancel alert message : ");
	if (pTitle) {
		Debug::String(pTitle);
		Debug::String(", ");
	}
	Debug::String(pMessage);
	Debug::String("\n");
	return FALSE;		// Always cancel!
}

#endif
