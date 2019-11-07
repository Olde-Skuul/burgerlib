/***************************************

    Error codes.

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRERROR_H__
#define __BRERROR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

// Global error types

enum eError {

    kErrorNone,                       /*!< No error (Always zero). */
    kErrorGeneric = 0x7000,           /*!< General error. */
    kErrorInvalidParameter,           /*!< Input parameter is invalid. */
    kErrorNotInitialized,             /*!< Class or subsystem not initialized */
    kErrorNotSupportedOnThisPlatform, /*!< Function not supported on this
                                         platform */
    kErrorVersionMismatch,            /*!< Version not supported */
    kErrorItemNotFound,               /*!< Item searched for was not found */
    kErrorNotReady,                   /*!< Not ready yet */
    kErrorOutOfEntries,               /*!< Fixed array is out of entries */
    kErrorOutOfBounds,                /*!< Index is beyond the allowed range */

    kErrorFile = 0x7100,         /*!< File system errors */
    kErrorFileNotFound,          /*!< File not found */
    kErrorPathNotFound,          /*!< Directory to file not found */
    kErrorWriteProtected,        /*!< Write access was not granted */
    kErrorNoMoreFileDescriptors, /*!< No more file descriptors are available */
    kErrorFileTableOverflow,     /*!< System file table overflow */
    kErrorDirectoryNotEmpty,     /*!< Files are present in the directory */
    kErrorNotAFile, /*!< Operation attempted on an object that wasn't a file */
    kErrorNotADirectory,    /*!< Operation attempted on an object that wasn't a
                               directory */
    kErrorFileExists,       /*!< File already exists */
    kErrorOpenFailure,      /*!< Failed on opening */
    kErrorReadFailure,      /*!< Failed on reading data */
    kErrorWriteFailure,     /*!< Failed on writing data */
    kErrorSeekFailure,      /*!< File or stream seek operation failed */
    kErrorAccessDenied,     /*!< Permission not granted to perform operation */
    kErrorResourceNotFound, /*!< Data resource not found */

    kErrorMemory = 0x7200, /*!< Memory system errors */
    kErrorOutOfMemory,     /*!< Not enough memory to perform operation */
    kErrorDataCorruption,  /*!< Data isn't valid or memory overrun detected */
    kErrorBadAlignment,    /*!< Data wasn't aligned on a proper byte boundary */
    kErrorNoData,          /*!< No data remaining */
    kErrorBufferTooSmall,  /*!< Buffer not large enough for operation */
    kErrorBufferTooLarge,  /*!< Buffer is too large for this operation */

    kErrorAudio = 0x7300,     /*!< Sound system errors */
    kErrorAudioDriverFailure, /*!< Audio driver failed */

    kErrorNetwork = 0x7400, /*!< Network errors */
    kErrorDataStarvation,   /*!< Data required for stream was not available in
                               time */
    kErrorSocketFailure,    /*!< Network socket failure */
    kErrorAcceptFailure,    /*!< Failed on accepting a connection */
    kErrorAddressNotFound,  /*!< Network address not found */
    kErrorFlowInterrupted,  /*!< Network flow interrupted (Or blocked) */

    kErrorVideo = 0x7500,         /*!< Video and shader errors */
    kErrorColorDepthNotSupported, /*!< Color depth requested is not available on
                                     current hardware */
    kErrorResolutionNotSupported, /*!< Requested display resolution is not
                                     available on current hardware */

    kErrorThread = 0x7600,       /*!< Thread and process errors */
    kErrorTimeout,               /*!< Operation timed out */
    kErrorEnumerationInProgress, /*!< Enumeration operation is in progress */
    kErrorNotEnumerating         /*!< Enumeration operation isn't running */
};

}
/* END */

#endif
