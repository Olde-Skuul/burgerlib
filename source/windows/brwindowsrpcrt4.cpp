/***************************************

    Shims for rpcrt4.dll

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <Rpc.h>

typedef RPC_STATUS(RPC_ENTRY* UuidCreateSequentialPtr)(UUID __RPC_FAR* Uuid);

// Unit tests for pointers
// UuidCreateSequentialPtr gUuidCreateSequential = ::UuidCreateSequential;

#endif

//
// rpcrt4.dll
//

/*! ************************************

	\brief Load in rpcrt4.dll and call UuidCreateSequential

	Manually load rpcrt4.dll if needed and call the Windows function
	UuidCreateSequential()

	http://msdn.microsoft.com/en-us/library/windows/desktop/aa379322(v=vs.85).aspx
	http://msdn.microsoft.com/en-us/library/windows/desktop/aa378645(v=vs.85).aspx

	\windowsonly

	\param pOutput A pointer to an uninitialized GUID.

	\return Zero for success or List of error codes

***************************************/

uint_t BURGER_API Burger::Windows::UuidCreateSequential(GUID* pOutput)
{
	void* pUuidCreateSequential = LoadFunctionIndex(CALL_UuidCreateSequential);
	RPC_STATUS uResult = RPC_S_CALL_FAILED;
	if (pUuidCreateSequential) {
		uResult = static_cast<UuidCreateSequentialPtr>(pUuidCreateSequential)(
			pOutput);
	}
	return static_cast<uint_t>(uResult);
}

#endif
