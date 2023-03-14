/***************************************

	Code library (DLL) manager

	MacOSX version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcodelibrary.h"

#if defined(BURGER_MACOSX)
#include "brfilename.h"
#include "brstringfunctions.h"
#include <dlfcn.h>

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFDate.h>

#if defined(__MWERKS__)
#include <CarbonCore/Folders.h>
#else
#include <Carbon/Carbon.h>
#endif

/***************************************

	\brief Default constructor

***************************************/

Burger::CodeFramework::CodeFramework() BURGER_NOEXCEPT: m_pBundle(nullptr) {}

/***************************************

	\brief Default destructor

	If a Framework was loaded, it will be released

	\sa Shutdown(void)

***************************************/

Burger::CodeFramework::~CodeFramework()
{
	Shutdown();
}

/***************************************

	\brief Load a Framework

	Attempt to load in a Framework using the standard paths.

	\param pName Name of the Framework to load
	\return Zero on success, non-zero on failure

***************************************/

Burger::eError BURGER_API Burger::CodeFramework::Init(
	const char* pName) BURGER_NOEXCEPT
{
	// Release any previous instance
	Shutdown();

	// Folder for the framework
	FSRef frameworksFolderRef;

	// Does the framework folder exist? (Only on a MacOS X install)

	eError Result = static_cast<eError>(FSFindFolder(
		kOnAppropriateDisk, kFrameworksFolderType, TRUE, &frameworksFolderRef));

	if (Result == kErrorNone) {

		// Convert the FSRef into a URL

		CFURLRef baseURL = CFURLCreateFromFSRef(0, &frameworksFolderRef);

		Result = kErrorOutOfMemory;
		if (baseURL) {

			// Convert to a string ref

			CFStringRef StringRef =
				CFStringCreateWithCString(0, pName, kCFStringEncodingUTF8);
			CFURLRef bundleURL = CFURLCreateCopyAppendingPathComponent(
				0, baseURL, StringRef, FALSE);

			// Dispose of the string ref
			CFRelease(StringRef);

			if (bundleURL) {
				CFBundleRef bundleRef = CFBundleCreate(0, bundleURL);
				if (bundleRef) {
					if (!CFBundleLoadExecutable(bundleRef)) {
						CFRelease(bundleRef);
						Result = kErrorReadFailure;
					} else {
						m_pBundle = bundleRef;
						Result = kErrorNone;
					}
				}
				CFRelease(bundleURL);
			}
			CFRelease(baseURL);
		}
	}
	return Result;
}

/***************************************

	\brief Release the Framework

	If a Framework was loaded, it will be released

***************************************/

void BURGER_API Burger::CodeFramework::Shutdown(void) BURGER_NOEXCEPT
{
	if (m_pBundle) {
		CFRelease(m_pBundle);
		m_pBundle = nullptr;
	}
}

/***************************************

	\brief Locate a function pointer in a Framework

	Return a function pointer to a procedure or data contained within a shared
	library

	\param pFunctionName Pointer to a "C" string in UTF-8
		encoding of the function to locate.
	\return \ref NULL on failure, or valid pointer on success

***************************************/

void* BURGER_API Burger::CodeFramework::GetFunction(
	const char* pFunctionName) BURGER_NOEXCEPT
{
	// Assume failure
	void* pResult = nullptr;

	// Is the lib valid?
	if (m_pBundle) {

		// Convert to a string ref
		CFStringRef StringRef =
			CFStringCreateWithCString(0, pFunctionName, kCFStringEncodingUTF8);

		// Get the function
		pResult = CFBundleGetFunctionPointerForName(m_pBundle, StringRef);

		// Dispose of the string ref
		CFRelease(StringRef);
	}
	// Return 0 or the pointer
	return pResult;
}

#endif
