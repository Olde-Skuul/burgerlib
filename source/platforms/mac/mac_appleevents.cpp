/***************************************

	MacOS Apple Events code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_appleevents.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brmemoryfunctions.h"

#include <AEDataModel.h>
#include <Files.h>
#include <MacMemory.h>

/*! ************************************

	\brief Pull an FSSpec from an AppleEvent.

	Given an AppleEvent, index into it and extract an FSSpec. The index is
	passed to the MacOS function AEGetNthDesc().

	\param pList AEDescList pointer of a list of AppleEvents to process
	\param iIndex Entry index into the list.
	\param pFSSpec Pointer to the FSSpec record to receive the file reference.

	\return OSErr of 0 for no error on non-zero OSErr.

***************************************/

int16_t BURGER_API Burger::MacOS::get_FSSpec_nth_desc(
	AEDesc* pList, long iIndex, FSSpec* pFSSpec) BURGER_NOEXCEPT
{
	AEDesc firstDesc;
	AEKeyword ignoreKeyword;

	// Initialize firstDesc
	OSErr uError =
		AEGetNthDesc(pList, iIndex, typeFSS, &ignoreKeyword, &firstDesc);

	// Get data only if there was no error.
	if (!uError) {

#if defined(BURGER_MACCARBON)
		uError = AEGetDescData(&firstDesc, pFSSpec, sizeof(*pFSSpec));

		// MacOS Classic version
#else
		Size uSize = GetHandleSize(firstDesc.dataHandle);
		if (uSize < sizeof(*pFSSpec)) {
			uError = memSCErr;
		} else {
			memory_copy(pFSSpec, *(firstDesc.dataHandle), sizeof(*pFSSpec));
		}
#endif
		OSErr uDisposeError = AEDisposeDesc(&firstDesc);
		if (!uError) {
			uError = uDisposeError;
		}
	}
	return uError;
}

#endif