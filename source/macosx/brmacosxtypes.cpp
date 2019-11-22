/***************************************

	Typedefs specific to Mac OSX

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmacosxtypes.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include <dlfcn.h>
#include <errno.h>

typedef int (*getattrlistbulkPtr)(int dirfd, attrlist* attrList, void* attrBuf,
	WordPtr attrBufSize, Word64 options);

/// Singleton instance of the macOS global variables
Burger::MacOSX Burger::MacOSX::g_Globals;

/***************************************

	DLLs that can be dynamically loaded at runtime (So the application can
	launch if they are missing or missing functions)

***************************************/

//
// These filenames MUST match eDLLIndex
//

static const char* s_LibaryNames[Burger::MacOSX::DLL_COUNT] = {"libdl.dylib"};

//
// Internal structure for DLL function lookup
//

struct CallNames_t {
	Burger::MacOSX::eDLLIndex eDLL; // Enumeration for the DLL
	const char* m_pName;			// Function name
};

static const CallNames_t g_CallNames[Burger::MacOSX::CALL_COUNT] = {
	{Burger::MacOSX::LIBDL_DLL, "getattrlistbulk"}};

/*! ************************************

	\struct Burger::MacOSX
	\brief Functions exclusive to Apple macOS

	\macosxonly

	\sa Burger::Globals

***************************************/

/*! ************************************

	\enum Burger::MacOSX::eDLLIndex
	\brief DLL enumeration for loading in system DLLs

	\macosxonly

	\sa LoadLibraryIndex()

***************************************/

/*! ************************************

	\enum Burger::MacOSX::eCallIndex
	\brief Function enumeration for loading in system calls

	\macosxonly

	\sa LoadFunctionIndex()

***************************************/

/*! ************************************

	\brief Clean up globals

	If any global resources were allocated at runtime, this function will
	release all resources before application shutdown

	\note This should not be called by an application directly.

***************************************/

Burger::MacOSX::~MacOSX()
{
	// Dispose of all resolved calls to Windows
	MemoryClear(m_pMacOSXCalls, sizeof(m_pMacOSXCalls));
	MemoryClear(m_bFunctionsTested, sizeof(m_bFunctionsTested));

	// Finally, release all of the allocated DLLs
	WordPtr i = 0;
	do {
		if (m_pInstances[i]) {
			dlclose(m_pInstances[i]);
			m_pInstances[i] = nullptr;
		}
		m_bInstancesTested[i] = FALSE;
	} while (++i < DLL_COUNT);
}

/*! ************************************

	\brief Load a library if needed

	Given a DLL index, detect if the library has already been loaded and if so,
	return the existing void *, otherwise, load the DLL and if successful,
	return the void *. If the load failed, return \ref NULL.

	\macosxonly

	\param eIndex \ref eDLLIndex index to a DLL Burgerlib is tracking.

	\return \ref NULL if the DLL was not loaded, a valid pointer on success

	\sa LoadFunctionIndex()

***************************************/

void* BURGER_API Burger::MacOSX::LoadLibraryIndex(eDLLIndex eIndex)
{
	void* pResult = nullptr;
	// Valid index?
	if (eIndex < DLL_COUNT) {

		// Has it been loaded?
		pResult = g_Globals.m_pInstances[eIndex];

		// If not already tested and not loaded?
		if (!pResult && !g_Globals.m_bInstancesTested[eIndex]) {

			// Mark as tested
			g_Globals.m_bInstancesTested[eIndex] = TRUE;

			// Load the most recent version of the DLL
			pResult = dlopen(s_LibaryNames[eIndex], RTLD_LAZY);

			// If it loaded fine, save the result
			if (pResult) {
				g_Globals.m_pInstances[eIndex] = pResult;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Load a function from a DLL library if needed

	Given a function index, detect if the library has already been loaded and if
	not, load it. If loaded, look up the function and return the pointer to the
	function or \ref NULL if not found.

	\macosxonly
	\param eIndex \ref eCallIndex index to a macOS function Burgerlib is
		tracking.

	\return \ref NULL if the DLL was not loaded or the function didn't exist, a
		valid function pointer on success

	\sa LoadLibraryIndex()

***************************************/

void* BURGER_API Burger::MacOSX::LoadFunctionIndex(eCallIndex eIndex)
{
	void* pResult = nullptr;
	// Valid index?
	if (eIndex < CALL_COUNT) {

		// Has the function been loaded?
		pResult = g_Globals.m_pMacOSXCalls[eIndex];

		// Speed up in the form of disabling multiple tests
		if (!pResult && !g_Globals.m_bFunctionsTested[eIndex]) {

			// Mark as tested
			g_Globals.m_bFunctionsTested[eIndex] = TRUE;
			void* pModule = LoadLibraryIndex(g_CallNames[eIndex].eDLL);
			if (pModule) {

				// Get the function from the DLL
				pResult = dlsym(pModule, g_CallNames[eIndex].m_pName);
				g_Globals.m_pMacOSXCalls[eIndex] = pResult;
			}
		}
	}
	return pResult;
}

// macOS 10.10 functions

/*! ************************************

	\brief Call getattrlistbulk() if present on macOS

	This function exists on macOS 10.10 or later.

	\macosxonly
	\param dirfd A file descriptor that references a directory opened for
		reading.
	\param attrList Pointer to a attribute request structure.
	\param attrBuf Pointer to a buffer to accept output.
	\param attrBufSize The size of the buffer in bytes of attrBuf.
	\param options A bit set that controls the behavior.

	\return Negative on error, positive for the number of entries read.

	\sa LoadLibraryIndex()

***************************************/

int BURGER_API Burger::MacOSX::getattrlistbulk(int dirfd, attrlist* attrList,
	void* attrBuf, WordPtr attrBufSize, Word64 options)
{
	void* pgetattrlistbulk = LoadFunctionIndex(CALL_getattrlistbulk);
	int iResult = -1; // Failure
	if (!pgetattrlistbulk) {
		errno = 45;
	} else {
		iResult = reinterpret_cast<getattrlistbulkPtr>(pgetattrlistbulk)(
			dirfd, attrList, attrBuf, attrBufSize, options);
	}
	return iResult;
}

#endif
