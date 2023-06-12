/***************************************

	OpenTptATalkPPC.o compatible library

	This replaces Apple's OpenTptATalkPPC.o file so profiling, debugging and
	other compiler features that are available with source level debugging
	become available to an application.

	This code is intended only for MacOS Classic PowerPC targeted applications.

	Copyright (c) 2002-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

// Only build on PowerPC MacOS Classic targets.

#if defined(BURGER_MACCLASSIC) && defined(BURGER_POWERPC) && !defined(DOXYGEN)

#include <Files.h>
#include <OpenTptAppleTalk.h>

//
// Below is some of Apple's Secret Sauce.
// These are the prototypes to the code in ROM on a PowerPC macintosh
//

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OTClientContext_t OTClientContext_t;

// Pointer to an application network state
extern OTClientContext_t __gOTClientRecord;

extern pascal OSStatus OTAsyncOpenAppleTalkServicesPriv(OTConfigurationRef cfig,
	OTOpenFlags flags, OTNotifyProcPtr, void* contextPtr,
	OTClientContext_t* pClientContext);

extern pascal ATSvcRef OTOpenAppleTalkServicesPriv(OTConfigurationRef cfig,
	OTOpenFlags flags, OSStatus* err, OTClientContext_t* pClientContext);

#ifdef __cplusplus
}
#endif

/***************************************

	Call OTAsyncOpenAppleTalkServices

***************************************/

pascal OSStatus OTAsyncOpenAppleTalkServices(OTConfigurationRef cfig,
	OTOpenFlags flags, OTNotifyProcPtr proc, void* contextPtr)
{
	return OTAsyncOpenAppleTalkServicesPriv(
		cfig, flags, proc, contextPtr, &__gOTClientRecord);
}

/***************************************

	Call OTOpenAppleTalkServices

***************************************/

pascal ATSvcRef OTOpenAppleTalkServices(
	OTConfigurationRef cfig, OTOpenFlags flags, OSStatus* err)
{
	return OTOpenAppleTalkServicesPriv(cfig, flags, err, &__gOTClientRecord);
}

#endif
