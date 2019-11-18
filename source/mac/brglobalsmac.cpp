/***************************************

	MacOS version of Burger::Globals

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brglobals.h"
#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brstring.h"
#include "brstringfunctions.h"
#include <Folders.h>
#include <Gestalt.h>
#include <InternetConfig.h>

/*! ************************************

	\brief Return the version of MacOS.

	Ask MacOS what version it is and return that value.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

Word BURGER_API Burger::Globals::GetMacOSVersion(void)
{
	Globals* pGlobals = &g_Globals;
	Word uVersion;
	if (!pGlobals->m_bMacOSTested) {
		long MyAnswer;
		// Get the version with Gestalt
		if (Gestalt(gestaltSystemVersion, &MyAnswer)) {
			MyAnswer = 0; // Should NEVER execute, failsafe
		}
		// Get the version of the OS in 0x0102 (1.2) format
		uVersion = static_cast<Word>(MyAnswer & 0xFFFFU);
		pGlobals->m_uMacOSVersion = uVersion;
		pGlobals->m_bMacOSTested = TRUE;
	} else {
		// Use the cached version
		uVersion = pGlobals->m_uMacOSVersion;
	}
	return uVersion;
}

/***************************************

	\brief Convert an NSString to a Burger::String (MacOSX and Carbon Only)

	Given a valid const NSString, convert the string into
	UTF8 encoding and store the result into an output
	\ref String.

	\macosxonly
	\param pOutput Pointer to a String class instance to recieve the string
	\param pInput Pointer to a constant NSString or CFString to convert

***************************************/

#if TARGET_API_MAC_CARBON
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
#endif

/***************************************

	\brief Return the version of QuickTime.

	Detect if QuickTime is available, and if so, query
	it for the version present. If QuickTime is not available,
	the version returned is zero.

	This function is written so it only asks for the version
	once from QuickTime. It will cache the version and
	return the cached value on subsequent calls.

	\return Version in the format of 0x0102 -> 1.2

***************************************/

Word BURGER_API Burger::Globals::GetQuickTimeVersion(void)
{
	Globals* pGlobals = &g_Globals; // Get the pointer to the singleton
	if (!pGlobals->m_bQuickTimeVersionValid) {
		pGlobals->m_bQuickTimeVersionValid = TRUE; // I got the version
		long gestaltAnswer;
		Word uResult = 0;
		if (!Gestalt(gestaltQuickTimeVersion, &gestaltAnswer)) {
			uResult = (gestaltAnswer >> 16) & 0xFFFFU; // Major version
		}
		pGlobals->m_uQuickTimeVersion = uResult;
	}
	return pGlobals->m_uQuickTimeVersion; // Return the quicktime version
}

/***************************************

	Load and launch a web page from an address string

***************************************/

Word BURGER_API Burger::Globals::LaunchURL(const char* pURL)
{
	ICInstance inst;

	OSStatus err = -1;
	if (ICStart != NULL) {
		err = ICStart(&inst, '????'); // Use your creator code if you have one!
		if (err == noErr) {
#if !TARGET_API_MAC_CARBON
			err = ICFindConfigFile(inst, 0, nil);
			if (err == noErr)
#endif
			{
				long startSel = 0;
				long endSel = static_cast<long>(StringLength(pURL));
				err = ICLaunchURL(inst, "\p", const_cast<char*>(pURL), endSel,
					&startSel, &endSel);
			}
			ICStop(inst);
		}
	}
	return static_cast<Word>(err);
}

#endif
