/***************************************

	Directory search Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::DirectorySearch
	\brief Directory contents iteration class.

	To open and traverse a directory is this classes' main purpose.
	Given a BurgerLib formatted pathname, open the directory and iterate over it
	until all the names of the files contained are presented to the application
	for use.

	Using it is very simple...

	\code
	Burger::DirectorySearch MyDir;
	// Open the directory for traversal
	uint_t uError = MyDir.Open("9:Directory");
	if (!uError) {		// No error?
		while (!MyDir.GetNextEntry()) {
			printf("Found \"%s\"!\n",MyDir.m_Name);
		}
		MyDir.Close();
	}
	\endcode

	\note All filenames are encoded using UTF8! Do not assume native encoding

	\sa class FileManager

***************************************/

/*! ************************************

	\brief Initialize a DirectorySearch class

	Simple initialization function.

	\sa Open(const char *)

***************************************/

Burger::DirectorySearch::DirectorySearch() BURGER_NOEXCEPT: m_uFileSize(0),
															m_bDir(FALSE),
															m_bSystem(FALSE),
															m_bHidden(FALSE),
															m_bLocked(FALSE)
{
	m_CreationDate.Clear();
	m_ModificatonDate.Clear();
	MemoryClear(m_Name, sizeof(m_Name));
#if defined(BURGER_MAC)
	// I do not own an iterator object, yet
	m_pIterator = nullptr;
	m_pLocal = nullptr;
#elif defined(BURGER_MACOSX)
	m_fp = -1;
	m_uEntryCount = 0;
	m_bDone = FALSE;
	m_pEntry = nullptr;
#elif defined(BURGER_IOS) || defined(BURGER_VITA) || defined(BURGER_LINUX)
	m_fp = -1;
#elif defined(BURGER_MSDOS)
	// No file open yet
	m_bHandleOk = 0;
#elif defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)
	// INVALID_HANDLE_VALUE;
	m_hDirHandle = reinterpret_cast<void*>(-1LL);
	MemoryClear(m_MyFindW, sizeof(m_MyFindW));
#endif
}

/*! ************************************

	\brief Closes any open directory.

	\sa Close(void)

***************************************/

Burger::DirectorySearch::~DirectorySearch()
{
	Close();
}

/*! ************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa Close(void) or GetNextEntry(void)

***************************************/

Burger::eError Burger::DirectorySearch::Open(
	const char* pDirName) BURGER_NOEXCEPT
{
	Filename TempDir(pDirName);
	return Open(&TempDir);
}

/*! ************************************

	\brief Open a directory for scanning
	\param pDirName Pointer to the "C" string filename in BurgerLib format.
	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa Close(void) or GetNextEntry(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError Burger::DirectorySearch::Open(
	Filename* /* pDirName */) BURGER_NOEXCEPT
{
	// Error!
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Iterate on a directory and return the next found filename.

	Reads the current directory opened previously by a call to \ref Open(
	const char *) and initializes all of the internal public variables with
	information of the file that was found.

	\return Zero on success, non-zero on an error or if the directory wasn't
		opened or if the end of the directory was reached

	\sa Close(void) or GetNextEntryExtension(const char *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	// Error!
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Iterate on a directory and return the next found filename that
		matches a specific file extension

	Reads the current directory opened previously by a call to
	\ref Open(const char *) and initializes all of the internal public variables
	with information of the file that was found.

	This differs from \ref GetNextEntry(void) in that only files that end with a
	specific file extension are returned. This is a case insensitive search.

	\code
	while (!MyDir.GetNextEntryExtension("txt")) {
		printf("Found text file \"%s\"!\n",MyDir.m_Name);
	}
	\endcode

	\param pExt Pointer to a "C" string of the file extension being scanned for.
	\return Zero on success, non-zero on an error or if the directory wasn't
		opened or if the end of the directory was reached

	\sa Close(void) or GetNextEntry(void)

	\note Only filenames are returned. All directory filenames are skipped, even
		if their names match the extension.

***************************************/

Burger::eError Burger::DirectorySearch::GetNextEntryExtension(
	const char* pExt) BURGER_NOEXCEPT
{
	// Anything left to scan?
	eError uResult = GetNextEntry();
	if (!uResult) {
		do {
			// Non-directory files
			if (!m_bDir && m_uFileSize) {
				// Get the file extension
				const char* pTest = GetFileExtension(m_Name);
				if (pTest) {
					// Match the extension?
					if (!StringCaseCompare(pTest, pExt)) {
						// I got a match!
						return kErrorNone;
					}
				}
			}
			uResult = GetNextEntry();
		} while (!uResult);
	}
	return uResult;
}

/*! ************************************

	\brief Closes any open directory.

	\sa ~DirectorySearch()

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT {}
#endif
