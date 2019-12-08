/***************************************

    OpenTptInetPPC.o compatible library

    This replaces Apple's OpenTptInetPPC.o file so profiling, debugging and
    other compiler features that are available with source level debugging
    become available to an application.

    This code is intended only for MacOS Classic PowerPC targeted applications.

    Copyright (c) 2002-2008 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

// Only build on PowerPC MacOS Classic targets.

#if defined(BURGER_MACCLASSIC) && defined(BURGER_POWERPC) && !defined(DOXYGEN)

#include <Files.h>
#include <OpenTptInternet.h>

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

extern pascal OSStatus OTAsyncOpenInternetServicesPriv(OTConfigurationRef cfig,
    OTOpenFlags oflag, OTNotifyProcPtr proc, void* contextPtr,
    OTClientContext_t* pClientContext);

extern pascal InetSvcRef OTOpenInternetServicesPriv(OTConfigurationRef cfig,
    OTOpenFlags oflag, OSStatus* err, OTClientContext_t* pClientContext);

#ifdef __cplusplus
}
#endif

/***************************************

    Call OTAsyncOpenInternetServices

***************************************/

pascal OSStatus OTAsyncOpenInternetServices(OTConfigurationRef cfig,
    OTOpenFlags oflag, OTNotifyProcPtr proc, void* contextPtr)
{
    return OTAsyncOpenInternetServicesPriv(
        cfig, oflag, proc, contextPtr, &__gOTClientRecord);
}

/***************************************

    Call OTOpenInternetServices

***************************************/

pascal InetSvcRef OTOpenInternetServices(
    OTConfigurationRef cfig, OTOpenFlags oflag, OSStatus* err)
{
    return OTOpenInternetServicesPriv(cfig, oflag, err, &__gOTClientRecord);
}

#endif
