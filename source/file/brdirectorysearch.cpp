/***************************************

	Directory search Class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

/*! ************************************

	\class Burger::DirectorySearch
	\brief Directory contents iteration class.
	
	To open and traverse a directory is this classes' main purpose.
	Given a BurgerLib formatted pathname, open the directory
	and iterate over it until all the names of the files contained
	are presented to the application for use.
	
	Using it is very simple...
	
	\code
	Burger::DirectorySearch MyDir;
	// Open the directory for traversal
	Word uError = MyDir.Open("9:Directory");
	if (!uError) {		// No error?
		while (!MyDir.GetNextEntry()) {
			printf("Found \"%s\"!\n",MyDir.m_Name);
		}
		MyDir.Close();
	}
	\endcode
	
	\sa struct Burger::FileManager
	\note All filenames are encoded using UTF8! Do not assume native encoding
		
***************************************/

/*! ************************************

	\brief Initialize a Burger::DirectorySearch class
	
	Simple initialization function.
	
	\sa Burger::DirectorySearch::Open(const char *)
			
***************************************/

Burger::DirectorySearch::DirectorySearch()
{
#if defined(BURGER_MAC)
	m_pIterator = NULL;		// I do not own an iterator object, yet
#elif defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_VITA)
	m_fp = -1;
#elif defined(BURGER_MSDOS)
	m_bHandleOk = 0;		// No file open yet
#elif defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)
	m_hDirHandle = reinterpret_cast<void *>(-1LL);	//INVALID_HANDLE_VALUE;
#endif
}

/*! ************************************

	\brief Closes any open directory.
	
	\sa Burger::DirectorySearch::Close(void)
			
***************************************/

Burger::DirectorySearch::~DirectorySearch()
{
	Close();
}

/*! ************************************

	\fn Word Burger::DirectorySearch::Open(const char *pDirName)
	\brief Open a directory for scanning
	\param pDirName Pointer to the "C" string filename in BurgerLib format.	
	\return Zero on success, non-zero on an error or if the directory doesn't exist
	\sa Burger::DirectorySearch::Close(void) or Burger::DirectorySearch::GetNextEntry(void)
			
***************************************/

Word Burger::DirectorySearch::Open(const char *pDirName)
{
	Filename TempDir(pDirName);
	return Open(&TempDir);		// Error!
}

/*! ************************************

	\fn Word Burger::DirectorySearch::Open(Filename *pDirName)
	\brief Open a directory for scanning
	\param pDirName Pointer to the "C" string filename in BurgerLib format.	
	\return Zero on success, non-zero on an error or if the directory doesn't exist
	\sa Burger::DirectorySearch::Close(void) or Burger::DirectorySearch::GetNextEntry(void)
			
***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || defined(BURGER_VITA)) || defined(DOXYGEN)
Word Burger::DirectorySearch::Open(Filename * /* pDirName */)
{
	return TRUE;		// Error!
}
#endif


/*! ************************************

	\brief Iterate on a directory and return the next found filename.
	
	Reads the current directory opened previously by a call to 
	\ref Burger::DirectorySearch::Open(const char *) and initializes all
	of the internal public variables with information of the file that was found.
	
	\return Zero on success, non-zero on an error or if the directory wasn't opened or if the end of the directory was reached
	\sa Burger::DirectorySearch::Close(void) or Burger::DirectorySearch::GetNextEntryExtension(const char *)
			
***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || defined(BURGER_VITA)) || defined(DOXYGEN)
Word Burger::DirectorySearch::GetNextEntry(void)
{
	return TRUE;		// Error!
}
#endif

/*! ************************************

	\brief Iterate on a directory and return the next found filename.that matches a specific file extension
	
	Reads the current directory opened previously by a call to 
	\ref Burger::DirectorySearch::Open(const char *) and initializes all
	of the internal public variables with information of the file that was found.
	
	This differs from \ref Burger::DirectorySearch::GetNextEntry(void) in that
	only files that end with a specific file extension are returned. This is 
	a case insensitive search.
	
	\code
	while (!MyDir.GetNextEntryExtension("txt")) {
		printf("Found text file \"%s\"!\n",MyDir.m_Name);
	}
	\endcode
	
	\param pExt Pointer to a "C" string of the file extension being scanned for.
	\return Zero on success, non-zero on an error or if the directory wasn't opened or if the end of the directory was reached
	\sa Burger::DirectorySearch::Close(void) or Burger::DirectorySearch::GetNextEntry(void)
	\note Only filenames are returned. All directory filenames are skipped, even if their names match the extension.
			
***************************************/

Word Burger::DirectorySearch::GetNextEntryExtension(const char *pExt)
{
	// Anything left to scan?
	if (!GetNextEntry()) {			
		do {
			// Non-directory files
			if (!m_bDir && m_uFileSize) {		
				// Get the file extension
				const char *pTest = GetFileExtension(m_Name);
				if (pTest) {
					// Match the extension?
					if (!StringCaseCompare(pTest,pExt)) {
						return FALSE;			// I got a match!
					}
				}
			}
		} while (!GetNextEntry());
	}
	return TRUE;
}

/*! ************************************

	\brief Closes any open directory.
	
	\sa Burger::DirectorySearch::~DirectorySearch(void)
			
***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || defined(BURGER_VITA)) || defined(DOXYGEN)
void Burger::DirectorySearch::Close(void)
{
}
#endif
