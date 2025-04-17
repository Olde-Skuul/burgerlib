/***************************************

	Operating system string functions
	Darwin version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)

#include <CoreFoundation/CFString.h>

/*! ************************************

	\brief Convert an NSString to a Burger::String (MacOSX and Carbon Only)

	Given a valid const NSString, convert the string into UTF8 encoding and
	store the result into an output \ref String.

	\macosxonly
	\param pOutput Pointer to a String class instance to recieve the string
	\param pInput Pointer to a constant NSString or CFString to convert

***************************************/

void BURGER_API Burger::StringCopy(String* pOutput, const __CFString* pInput) BURGER_NOEXCEPT
{
	// Try the easy way the just yank a "C" string pointer out directly
	const char* pResult = CFStringGetCStringPtr(pInput, kCFStringEncodingUTF8);
	if (pResult) {
		// Piece of cake!
		pOutput->assign(pResult);
	} else {
		// Crap, it's not encoded in UTF8 (Likely UTF-16)

		// Get the length of the string in UTF16 characters
		CFIndex uLength = CFStringGetLength(pInput);
		if (!uLength) {
			// If it's empty, leave now
			pOutput->clear();
		} else {
			// Determine the maximum buffer that would be needed for conversion
			// to UTF-8
			CFIndex uMaxLength = CFStringGetMaximumSizeForEncoding(
				uLength, kCFStringEncodingUTF8);
			// Create the buffer
			pOutput->resize(static_cast<uintptr_t>(uMaxLength));
			// Convert the string and store into the buffer
			if (!CFStringGetCString(pInput, pOutput->c_str(), uMaxLength + 1,
					kCFStringEncodingUTF8)) {
				// Lovely, failure
				pOutput->clear();
			} else {
				// Truncate the string to fit the final string
				// Note: Due to the manual copy, don't assume
				// pOutput->length() returns a valid value. Once
				// SetBufferSize() completes, the length is correct
				pOutput->resize(string_length(pOutput->c_str()));
			}
		}
	}
}

#endif
