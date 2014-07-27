/***************************************

	Code library (DLL) manager

	MacOS version
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcodelibrary.h"

#if defined(BURGER_MAC)
#include "brstringfunctions.h"
#include <CodeFragments.h>

/***************************************

	Attempt to load in a shared library or DLL using
	the standard paths. Return NULL if it fails

***************************************/

Word Burger::CodeLibrary::Init(const char *pFilename) 
{
	Str255 TempName;			// Copy of the "C" string as a PASCAL string
	Str255 ErrStr;				// Returned error code if any
	Ptr EntryPtr;				// Pointer to the fragment entry
	CFragConnectionID ConnID;

	/* This code only works for CFM functions */
	Burger::CStringToPString(TempName,pFilename);
	Word uResult = FALSE;
	if (!GetSharedLibrary(TempName,kCompiledCFragArch,kLoadCFrag, &ConnID,&EntryPtr,ErrStr)) {
		m_pLibInstance = ConnID;
		uResult = TRUE;
	}
	return uResult;
}

/***************************************

	Release a shared library

***************************************/

void Burger::CodeLibrary::Shutdown(void)
{
	if (m_pLibInstance) {
		CloseConnection(&m_pLibInstance);
		m_pLibInstance = NULL;
	}
}

/***************************************

	Return a function pointer to a procedure or data
	contained within a shared library

***************************************/

void * Burger::CodeLibrary::GetFunction(const char *pFunctionName)
{
	Str255 TempName;			// Copy of the "C" string as a PASCAL string
	Ptr ProcPtr;				// Pointer to the function
	void *pFunction = NULL;

	// This code only works for CFM functions
	if (pFunctionName && m_pLibInstance) {
		Burger::CStringToPString(TempName,pFunctionName);
		if (!FindSymbol(m_pLibInstance,TempName,&ProcPtr,0)) {
			pFunction = ProcPtr;
		}
	}
	return pFunction;
}

#endif
