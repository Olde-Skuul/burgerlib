/***************************************

    Operating system string functions
    MacOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brosstringfunctions.h"

#if defined(BURGER_MAC)
#include "brcodelibrary.h"
#include "brglobals.h"

#include <Folders.h>
#include <Gestalt.h>
#include <NameRegistry.h>
#include <Resources.h>
#include <TextUtils.h>

// MacOSX types
#define MACH_PORT_NULL 0
#define KERN_SUCCESS 0
#define kIOReturnSuccess KERN_SUCCESS
#define KERN_NO_ACCESS 8
typedef const struct __SCDynamicStore* SCDynamicStoreRef;
typedef UInt32 IOOptionBits;
typedef int kern_return_t;
typedef unsigned int __darwin_natural_t;
typedef __darwin_natural_t natural_t;
typedef natural_t mach_port_name_t;
typedef mach_port_name_t mach_port_t;
typedef mach_port_t io_object_t;
typedef io_object_t io_iterator_t;
typedef io_object_t io_service_t;
typedef io_object_t io_registry_entry_t;
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)

/*! ************************************

    Call GetString() using the system resource file.

    Call the function GetString() with UseResFile(0) so the string is forced to
    be obtained from the system resource.

    \maconly

    \param pOutput Pointer to a \ref String to receive the name in UTF-8
        encoding.
    \param iStringID MacOS resource ID number of the string.

    \return Zero on no error, or non zero on failure.

    \sa GetOSIndString(String *, int16_t, int16_t)

 ***************************************/

Burger::eError BURGER_API Burger::GetOSString(
    String* pOutput, int16_t iStringID) BURGER_NOEXCEPT
{
    // Get my resource reference
    short sOldResFile = CurResFile();

    // Use the Mac system reference
    UseResFile(0);

    // Get the string from the system resource
    StringHandle ppString = GetString(iStringID);

    // Restore the resource reference
    UseResFile(sOldResFile);

    // Assume not found
    eError uResult = kErrorItemNotFound;

    // Did I get a string?
    if (ppString && ppString[0][0]) {
        uResult = pOutput->Set(
            static_cast<const char*>(static_cast<const void*>(&ppString[0][1])),
            ppString[0][0]);
    }

    // Return the error
    return uResult;
}

/*! ************************************

    Call GetIndString() using the system resource file.

    Call the function GetIndString() with UseResFile(0) so the string is forced
    to be obtained from the system resource.

    \maconly

    \param pOutput Pointer to a \ref String to receive the name in UTF-8
        encoding.
    \param iStringID MacOS resource ID number of the string array.
    \param iIndex Index into the STR# list

    \return Zero on no error, or non zero on failure.

    \sa GetOSString(String *,int16_t)

 ***************************************/

Burger::eError BURGER_API Burger::GetOSIndString(
    String* pOutput, int16_t iStringID, int16_t iIndex) BURGER_NOEXCEPT
{
    // Get my resource reference
    short sOldResFile = CurResFile();

    // Use the Mac system reference
    UseResFile(0);

    // Get the string from the system resource string array
    Str255 resStr;
    resStr[0] = 0;
    GetIndString(resStr, iStringID, iIndex);

    // Restore the resource reference
    UseResFile(sOldResFile);

    // Assume not found
    eError uResult = kErrorItemNotFound;

    // Did I get a name?
    if (resStr[0]) {
        uResult = pOutput->Set(
            static_cast<const char*>(static_cast<const void*>(&resStr[1])),
            resStr[0]);
    }

    // Return the error
    return uResult;
}

/***************************************

    \brief Retrieves the login name of the user associated with the current
        thread.

    On systems that use user logins, return the login name of the account
    associated with the current thread. If the platform doesn't support multiple
    user accounts, it will return "User" and the error code \ref
    kErrorNotSupportedOnThisPlatform.

    \param pOutput Pointer to a \ref String to receive the name in UTF-8
        encoding
     \return Zero on no error, or non zero on failure.

     \note On platforms where networking or user level access isn't available,
        it will return \ref kErrorNotSupportedOnThisPlatform as an error code.

    \sa GetUserRealName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserLoginName(
    String* pOutput) BURGER_NOEXCEPT
{
#if defined(BURGER_MACCARBON)
    eError uResult = kErrorNotSupportedOnThisPlatform;
    // Carbon
    if (Globals::GetMacOSVersion() >= 0x1000U) {
        uResult = kErrorItemNotFound;
        CodeFramework LibRef;
        if (!LibRef.Init("SystemConfiguration.framework")) {

            CFStringRef (*pNSUserName)(void);
            pNSUserName =
                (CFStringRef(*)(void))LibRef.GetFunction("NSUserName");
            if (pNSUserName) {

                // Return the user name
                CFStringRef pStringRef = pNSUserName();
                if (pStringRef) {

                    Globals::StringCopy(pOutput, pStringRef);

                    // Dispose of the string ref
                    CFRelease(pStringRef);
                    uResult = kErrorNone;
                }
            }
        }
    }
    if (uResult) {
        // Get the user folder name
        if (uResult) {
            pOutput->Set("User");
        }
    }
    return uResult;
    
    #else
    pOutput->Set("User");
    return kErrorNotSupportedOnThisPlatform;
    #endif
}

/***************************************

    \brief Get the real name of the current user.

    When someone has logged onto a computer, that person can associate a real
    name to the login user account. This routine will retrieve real name of the
    user. If for some reason a user name can't be found or the operating system
    doesn't support user logins, the name "User" will be returned.

    \param pOutput Pointer to a \ref String to receive the real name in UTF-8
        encoding
    \return Zero on no error, or non zero on failure.

     \note On platforms where networking or user level access isn't available,
        it will always return \ref kErrorNotSupportedOnThisPlatform as an error
        code.

    \sa GetUserLoginName(String *) or GetMachineName(String *)

***************************************/

Burger::eError BURGER_API Burger::GetUserRealName(
    String* pOutput) BURGER_NOEXCEPT
{
    eError uResult = kErrorItemNotFound;

    // Carbon
#if defined(BURGER_MACCARBON)
    if (Globals::GetMacOSVersion() < 0x1000U)
#endif

    {
        // Get the user name string from the system resource
        uResult = GetOSString(pOutput, -16096);
    }

#if defined(BURGER_MACCARBON)
    else {
        CodeFramework LibRef;
        if (!LibRef.Init("SystemConfiguration.framework")) {

            CFStringRef (*pNSFullUserName)(void);
            pNSFullUserName =
                (CFStringRef(*)(void))LibRef.GetFunction("NSFullUserName");
            if (pNSFullUserName) {

                // Return the user name
                CFStringRef pStringRef = pNSFullUserName();
                if (pStringRef) {

                    Globals::StringCopy(pOutput, pStringRef);

                    // Dispose of the string ref
                    CFRelease(pStringRef);
                    uResult = kErrorNone;
                }
            }
        }
    }
#endif

    if (uResult) {
        // The name wasn't present, use the default
        pOutput->Set("User");
    }

    return uResult;
}

/***************************************

    \brief Get the name the user has called the computer.

    Some computer owners have the option to give their computer a whimsical
    name. This routine will retrieve that name. If for some reason a name can't
    be found or the operating system doesn't support naming, the name of
    "Computer" will be returned.

    \param pOutput Pointer to a \ref String to receive the name in UTF-8
        encoding
    \return Zero on no error, or non zero on failure.

    \note On platforms where networking or user level access isn't available, it
        will return \ref kErrorNotSupportedOnThisPlatform as an error code.

    \note On MacOS 9, the machine name is found in the OS string number -16413
        from the system resource file.

    \sa GetUserLoginName(String *) or NetworkManager::GetHostName()

***************************************/

Burger::eError BURGER_API Burger::GetMachineName(
    String* pOutput) BURGER_NOEXCEPT
{
    eError uResult = kErrorItemNotFound;

    // Carbon
#if defined(BURGER_MACCARBON)
    if (Globals::GetMacOSVersion() < 0x1000U)
#endif
    {
        // Get the machine name string from the system resource
        uResult = GetOSString(pOutput, -16413);
    }

#if defined(BURGER_MACCARBON)
    else {
        CodeFramework LibRef;
        if (!LibRef.Init("SystemConfiguration.framework")) {

            CFStringRef (*pSCDynamicStoreCopyComputerName)(
                SCDynamicStoreRef store, CFStringEncoding * nameEncoding);

            pSCDynamicStoreCopyComputerName =
                (CFStringRef(*)(SCDynamicStoreRef, CFStringEncoding*))
                    LibRef.GetFunction("SCDynamicStoreCopyComputerName");
            if (pSCDynamicStoreCopyComputerName) {

                // Return the computer name
                CFStringRef pStringRef = pSCDynamicStoreCopyComputerName(0, 0);
                if (pStringRef) {
                    Globals::StringCopy(pOutput, pStringRef);

                    // Dispose of the string ref
                    CFRelease(pStringRef);
                    uResult = kErrorNone;
                }
            }
        }
    }
#endif

    if (uResult) {
        // The name wasn't present, use the default
        pOutput->Set("Computer");
    }

    return uResult;
}

/***************************************

    Return the name of the mac model.

    Query the Registry entries that are found in a Macintosh and
    return the model string that denotes the specific model of Macintosh
    that is currently running this code.

    \param pOutput Pointer to a \ref String to receive the name in UTF-8
        encoding

    \return Zero on no error, or non zero on failure.

 ***************************************/

Burger::eError BURGER_API Burger::GetMacModelIdentifier(
    String* pOutput) BURGER_NOEXCEPT
{
    eError uResult = kErrorItemNotFound;
#if defined(BURGER_MACCLASSIC)
    long lResponse = 0;
    if (Gestalt(gestaltNameRegistryVersion, &lResponse)) {
        if (!Gestalt(gestaltUserVisibleMachineName, &lResponse)) {
            // Did I get a name?
            if (reinterpret_cast<uint8_t*>(lResponse)[0]) {
                uResult = pOutput->Set(reinterpret_cast<char*>(lResponse + 1),
                reinterpret_cast<uint8_t*>(lResponse)[0]);
            }
        } else if (!Gestalt(gestaltMachineType, &lResponse)) {
            // Get the machine name string from the system resource
            GetOSIndString(
                pOutput, kMachineNameStrID, static_cast<short>(lResponse));
        }
    } else {
        // Registry dictionary object
        RegEntryID uRegEntryID;
        OSStatus err = RegistryEntryIDInit(&uRegEntryID);
        if (err == noErr) {
            // Find the namespace name
            err = RegistryCStrEntryLookup(
                nullptr, "Devices:device-tree", &uRegEntryID);
            if (err == noErr) {

                // Get the size of the data, if it exists
                RegPropertyValueSize uLength;
                err = RegistryPropertyGetSize(
                    &uRegEntryID, "compatible", &uLength);
                if (err == noErr) {
                    uResult = pOutput->SetBufferSize(uLength);
                    if (!uResult) {
                        err = RegistryPropertyGet(&uRegEntryID, "compatible",
                            pOutput->c_str(), &uLength);
                        if (err != noErr) {
                            uResult = kErrorItemNotFound;
                        }
                    }
                }
            }
            // Dispose of the RegEntryID
            RegistryEntryIDDispose(&uRegEntryID);
        }
    }
#else
    // Carbon
    if (Globals::GetMacOSVersion() < 0x1000U) {
        // Manually handle the Classic code
        CodeLibrary* pNameRegistryLib = GetNameRegistryLib();
        if (pNameRegistryLib) {
            OSStatus (*RegistryEntryIDInit)(RegEntryID*) =
                static_cast<OSStatus (*)(RegEntryID*)>(
                    pNameRegistryLib->GetFunction("RegistryEntryIDInit"));
            OSStatus (*RegistryCStrEntryLookup)(
                const RegEntryID*, const RegCStrPathName*, RegEntryID*) =
                static_cast<OSStatus (*)(
                    const RegEntryID*, const RegCStrPathName*, RegEntryID*)>(
                    pNameRegistryLib->GetFunction("RegistryCStrEntryLookup"));
            OSStatus (*RegistryPropertyGet)(const RegEntryID*,
                const RegPropertyName*, void*, RegPropertyValueSize*) =
                static_cast<OSStatus (*)(const RegEntryID*,
                    const RegPropertyName*, void*, RegPropertyValueSize*)>(
                    pNameRegistryLib->GetFunction("RegistryPropertyGet"));
            OSStatus (*RegistryPropertyGetSize)(const RegEntryID*,
                const RegPropertyName*, RegPropertyValueSize*) =
                static_cast<OSStatus (*)(const RegEntryID*,
                    const RegPropertyName*, RegPropertyValueSize*)>(
                    pNameRegistryLib->GetFunction("RegistryPropertyGetSize"));
            OSStatus (*RegistryEntryIDDispose)(RegEntryID*) =
                static_cast<OSStatus (*)(RegEntryID*)>(
                    pNameRegistryLib->GetFunction("RegistryEntryIDDispose"));

            // Registry dictionary object
            RegEntryID uRegEntryID;
            OSStatus err = RegistryEntryIDInit(&uRegEntryID);
            if (err == noErr) {
                // Find the namespace name
                err = RegistryCStrEntryLookup(
                    nullptr, "Devices:device-tree", &uRegEntryID);
                if (err == noErr) {

                    // Get the size of the data, if it exists
                    RegPropertyValueSize uLength;
                    err = RegistryPropertyGetSize(
                        &uRegEntryID, "compatible", &uLength);
                    if (err == noErr) {
                        uResult = pOutput->SetBufferSize(uLength);
                        if (!uResult) {
                            err = RegistryPropertyGet(&uRegEntryID,
                                "compatible", pOutput->c_str(), &uLength);
                            if (err != noErr) {
                                uResult = kErrorItemNotFound;
                            }
                        }
                    }
                }
                // Dispose of the RegEntryID
                RegistryEntryIDDispose(&uRegEntryID);
            }
        }
    } else {
        CodeFramework LibRef;
        if (!LibRef.Init("IOKit.framework")) {

            kern_return_t (*IOMasterPort)(mach_port_t, mach_port_t*) =
                static_cast<kern_return_t (*)(mach_port_t, mach_port_t*)>(
                    LibRef.GetFunction("IOMasterPort"));
            kern_return_t (*IOObjectRelease)(io_object_t) =
                static_cast<kern_return_t (*)(io_object_t)>(
                    LibRef.GetFunction("IOObjectRelease"));
            CFMutableDictionaryRef (*IOServiceMatching)(const char*) =
                static_cast<CFMutableDictionaryRef (*)(const char*)>(
                    LibRef.GetFunction("IOServiceMatching"));
            kern_return_t (*IOServiceGetMatchingServices)(
                mach_port_t, CFDictionaryRef, io_iterator_t*) =
                static_cast<kern_return_t (*)(
                    mach_port_t, CFDictionaryRef, io_iterator_t*)>(
                    LibRef.GetFunction("IOServiceGetMatchingServices"));
            CFTypeRef (*IORegistryEntryCreateCFProperty)(io_registry_entry_t,
                CFStringRef, CFAllocatorRef, IOOptionBits) =
                static_cast<CFTypeRef (*)(io_registry_entry_t, CFStringRef,
                    CFAllocatorRef, IOOptionBits)>(
                    LibRef.GetFunction("IORegistryEntryCreateCFProperty"));
            io_object_t (*IOIteratorNext)(io_iterator_t) =
                static_cast<io_object_t (*)(io_iterator_t)>(
                    LibRef.GetFunction("IOIteratorNext"));

            // Get the master port
            mach_port_t pMachPort;
            kern_return_t uKernelReturn =
                IOMasterPort(MACH_PORT_NULL, &pMachPort);
            if (uKernelReturn != kIOReturnSuccess) {
                // If the failure was because of lack of privilege, note
                // that.
                if (uKernelReturn == KERN_NO_ACCESS) {
                    uResult = kErrorAccessDenied;
                }
            } else {

                // Got the master port, let's get the registry dictionary
                CFDictionaryRef pServiceMatch =
                    IOServiceMatching("IOPlatformExpertDevice");
                if (pServiceMatch) {
                    // IOServiceGetMatchingServices consumes the
                    // pServiceMatch
                    io_iterator_t pIOIterator;
                    uKernelReturn = IOServiceGetMatchingServices(
                        pMachPort, pServiceMatch, &pIOIterator);
                    if (uKernelReturn == kIOReturnSuccess) {

                        // Loop until a device has the name of the Mac
                        io_service_t pIOService;
                        while ((pIOService = IOIteratorNext(pIOIterator)) !=
                            nullptr) {

                            // Look up the entry
                            CFDataRef pDataRef = static_cast<CFDataRef>(
                                IORegistryEntryCreateCFProperty(pIOService,
                                    CFSTR("model"), kCFAllocatorDefault,
                                    kNilOptions));
                            if (pDataRef) {
                                // Found it! However, it's raw data, not a
                                // string
                                uintptr_t uLength = static_cast<uintptr_t>(
                                    CFDataGetLength(pDataRef));
                                const char* pData =
                                    reinterpret_cast<const char*>(
                                        CFDataGetBytePtr(pDataRef));
                                uResult = pOutput->Set(pData, uLength);

                                // Release the data and exit
                                CFRelease(pDataRef);
                                break;
                            }
                        }
                        // Release the objects
                        IOObjectRelease(pIOService);
                        IOObjectRelease(pIOIterator);
                    }
                }
            }
        }
    }
#endif
    if (uResult) {
        // The name wasn't present, use the default
        pOutput->Set("Macintosh");
    }
    return uResult;
}

#endif
