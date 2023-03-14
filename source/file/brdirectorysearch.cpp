/***************************************

	Directory search Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::DirectoryEntry_t
	\brief A directory entry returned by \ref DirectorySearch

	When the \ref DirectorySearch class loads a directory, it will store a list
	of filenames and then return data on a file using this structure.

	The data is valid until the next call to
	\ref DirectorySearch::get_next_entry(void) or if the \ref DirectorySearch
	class is closed or goes out of scope.

	\sa \ref DirectorySearch

***************************************/

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
	uint_t uError = MyDir.open("9:Directory");
	if (!uError) {		// No error?
		DirectoryEntry_t Entry;
		while (!MyDir.get_next_entry(&Entry)) {
			printf("Found \"%s\"!\n", Entry.m_pName);
		}
		MyDir.close();
	}
	\endcode

	\note All filenames are encoded using UTF8! Do not assume native encoding

	\sa \ref FileManager or \ref DirectoryEntry_t

***************************************/

/*! ************************************

	\brief Initialize a DirectorySearch class

	Simple initialization function.

	\sa open(const char *)

***************************************/

Burger::DirectorySearch::DirectorySearch() BURGER_NOEXCEPT: m_Entries(),
															m_uIndex(0)
{
}

/*! ************************************

	\brief Closes any open directory.

	\sa close(void)

***************************************/

Burger::DirectorySearch::~DirectorySearch()
{
	close();
}

/*! ************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(
	const char* pDirName) BURGER_NOEXCEPT
{
	Filename TempDir(pDirName);
	return open(&TempDir);
}

/*! ************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the \ref Filename object

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError Burger::DirectorySearch::open(
	Filename* /* pDirName */) BURGER_NOEXCEPT
{
	// Error!
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Iterate on a directory and return the next found filename.

	If there is a directory entry in the queue, return the UTF8 encoded filename
	in the list.

	\note If more information than the filename is needed, use
		get_next_entry(DirectoryEntry_t*) instead.

	\return Zero on success, non-zero on an error or if the directory wasn't
		opened or if the end of the directory was reached

	\sa get_next_entry(DirectoryEntry_t*) or get_next_entry(DirectoryEntry_t*,
		const char*)

***************************************/

const char* Burger::DirectorySearch::get_next_entry(void) BURGER_NOEXCEPT
{
	const char* pResult;

	// End of the list?
	uintptr_t uIndex = m_uIndex;
	if (uIndex >= m_Entries.size()) {
		pResult = nullptr;
	} else {

		// Get the filename and increment the index
		pResult = m_Entries[uIndex].m_pName;
		++uIndex;
		m_uIndex = uIndex;
	}

	return pResult;
}

/*! ************************************

	\brief Iterate on a directory and return the next found filename.

	Reads the current directory opened previously by a call to \ref open(
	const char *) and stores the information into the supplied DirectoryEntry_t
	buffer.

	\return Zero on success, non-zero on an error or if the directory wasn't
		opened or if the end of the directory was reached

	\sa close(void) or get_next_entry(DirectoryEntry_t*, const char *)

***************************************/

Burger::eError Burger::DirectorySearch::get_next_entry(
	DirectoryEntry_t* pEntry) BURGER_NOEXCEPT
{
	eError uResult;

	// End of the list?
	uintptr_t uIndex = m_uIndex;
	if (uIndex >= m_Entries.size()) {

		// Clear out the entry and exit with an error
		MemoryClear(pEntry, sizeof(*pEntry));
		uResult = kErrorNotEnumerating;

	} else {

		// Copy the entry
		const DirectoryEntry_t* pTemp = &m_Entries[uIndex];
		++uIndex;
		m_uIndex = uIndex;
		uResult = direntry_copy(pEntry, pTemp);
	}
	return uResult;
}

/*! ************************************

	\brief Iterate on a directory and return the next found filename that
		matches a specific file extension

	Reads the current directory opened previously by a call to
	\ref open(const char *) and initializes all of the internal public variables
	with information of the file that was found.

	This differs from \ref get_next_entry(void) in that only files that end with
	a specific file extension are returned. This is a case insensitive search.

	\code
	DirectoryEntry_t Entry;
	while (!MyDir.get_next_entry(&Entry, "txt")) {
		printf("Found text file \"%s\"!\n", Entry.m_pName);
	}
	\endcode

	\note Only filenames are returned. All directory filenames are skipped, even
		if their names match the extension.

	\param pEntry Pointer to an uninitialized DirectoryEntry_t
	\param pExt Pointer to a "C" string of the file extension being scanned for.

	\return Zero on success, non-zero on an error or if the directory wasn't
		opened or if the end of the directory was reached

	\sa close(void), get_next_entry(DirectoryEntry_t*)

***************************************/

Burger::eError Burger::DirectorySearch::get_next_entry(
	DirectoryEntry_t* pEntry, const char* pExt) BURGER_NOEXCEPT
{
	// Anything left to scan?
	uintptr_t uIndex = m_uIndex;
	eError uResult;
	for (;;) {
		if (uIndex >= m_Entries.size()) {
			MemoryClear(pEntry, sizeof(*pEntry));
			uResult = kErrorNotEnumerating;
			break;
		}

		// Accept an entry
		const DirectoryEntry_t* pTemp = &m_Entries[uIndex];
		++uIndex;

		// Get the file extension
		const char* pTest = GetFileExtension(pTemp->m_pName);
		if (pTest) {

			// Match the extension?
			if (!StringCaseCompare(pTest, pExt)) {
				// Check if it's a file or directory, only accept files.
				if (!pTemp->m_bDir) {

					// I got a match!
					uResult = direntry_copy(pEntry, pTemp);
					break;
				}
			}
		}
	}
	m_uIndex = uIndex;

	return uResult;
}

/*! ************************************

	\brief Release all memory and close the directory.

	Release all resources allocated by the open(Filename*) call.

	\sa open(Filename*)

***************************************/

void Burger::DirectorySearch::close(void) BURGER_NOEXCEPT
{
	// Any entries?
	uintptr_t uCount = m_Entries.size();
	if (uCount) {

		// Release the memory allocated for the filenames
		DirectoryEntry_t* pWork = m_Entries.data();
		do {
			Free(pWork->m_pName);
			++pWork;
		} while (--uCount);
	}
	m_Entries.clear();
}

/*! ************************************

	\brief Internal function for copying \ref DirectoryEntry_t

	On some platforms, reading a directory only returns a filename list. For
	performance, only DirectoryEntry_t records are fully parsed on records when
	they are being used and records skipped will be ignored.

	All other platforms, the entry is simply copied.

	\return Zero on success, non-zero on an error.

	\sa get_next_entry(DirectoryEntry_t*)

***************************************/

#if !(defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
Burger::eError Burger::DirectorySearch::direntry_copy(
	DirectoryEntry_t* pOutput, const DirectoryEntry_t* pInput) BURGER_NOEXCEPT
{
	MemoryCopy(pOutput, pInput, sizeof(DirectoryEntry_t));
	return kErrorNone;
}
#endif