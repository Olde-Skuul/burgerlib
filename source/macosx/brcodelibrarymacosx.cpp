/***************************************

    Code library (DLL) manager

    MacOSX version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	Attempt to load in a shared library or DLL using the standard paths. Return
	NULL if it fails

	Please note, in posix, passing in just the DLL name without a full path will
	allow the SYSTEM DLL's to be loaded, so, to see if you want to load a system
	DLL, I check the pathname if it has a ':' in it.

***************************************/

uint_t Burger::CodeLibrary::Init(const char* pFilename)
{
	// If there was a previous library, release it
	Shutdown();

	Filename Pathname;
	// Is this a burgerlib pathname?
	// Test by checking for a colon
	if (StringCharacter(pFilename, ':')) {
		// Convert to a native windows pathname
		Pathname.Set(pFilename);
		pFilename = Pathname.GetNative();
	}
	// Load the library from Linux
	void* pModule = dlopen(pFilename, RTLD_LAZY);
	// On success!
	uint_t uResult = FALSE;
	if (pModule != NULL) {
		m_pLibInstance = pModule;
		uResult = TRUE; // Error!
	}
	return uResult;
}

/***************************************

	Release a shared library

***************************************/

void Burger::CodeLibrary::Shutdown(void)
{
	// Release the Android lib
	if (m_pLibInstance) {
		dlclose(m_pLibInstance);
		m_pLibInstance = NULL;
	}
}

/***************************************

	Return a function pointer to a procedure or data contained within a shared
	library

***************************************/

void* Burger::CodeLibrary::GetFunction(const char* pFunctionName)
{
	void* pFunction = NULL;
	if (pFunctionName && m_pLibInstance) {
		pFunction = dlsym(m_pLibInstance, pFunctionName);
	}
	return pFunction;
}

/***************************************

	\brief Default constructor

***************************************/

Burger::CodeFramework::CodeFramework() : m_pBundle(NULL) {}

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

Burger::eError BURGER_API Burger::CodeFramework::Init(const char* pName)
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

void BURGER_API Burger::CodeFramework::Shutdown(void)
{
	if (m_pBundle) {
		CFRelease(m_pBundle);
		m_pBundle = NULL;
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

void* BURGER_API Burger::CodeFramework::GetFunction(const char* pFunctionName)
{
	// Assume failure
	void* pResult = NULL;

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
