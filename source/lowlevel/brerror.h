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

	/** No error (Always zero). */
	kErrorNone,

	/** General error. */
	kErrorGeneric = 0x7000,
	/** Input parameter is invalid. */
	kErrorInvalidParameter,
	/** Class or subsystem not initialized */
	kErrorNotInitialized,
	/** Command not supported */
	kErrorNotSupported,
	/** Function not supported on this platform */
	kErrorNotSupportedOnThisPlatform,
	/** Version not supported */
	kErrorVersionMismatch,
	/** Item searched for was not found */
	kErrorItemNotFound,
	/** Not ready yet */
	kErrorNotReady,
	/** Fixed array is out of entries */
	kErrorOutOfEntries,
	/** Index is beyond the allowed range */
	kErrorOutOfBounds,

	/** File system errors */
	kErrorFile = 0x7100,
	/** File not found */
	kErrorFileNotFound,
	/** Directory to file not found */
	kErrorPathNotFound,
	/** Write access was not granted */
	kErrorWriteProtected,
	/** No more file descriptors are available */
	kErrorNoMoreFileDescriptors,
	/** System file table overflow */
	kErrorFileTableOverflow,
	/** Files are present in the directory */
	kErrorDirectoryNotEmpty,
	/** Operation attempted on an object that wasn't a file */
	kErrorNotAFile,
	/** Operation attempted on an object that wasn't a directory */
	kErrorNotADirectory,
	/** File already exists */
	kErrorFileExists,
	/** Failed on opening */
	kErrorOpenFailure,
	/** Failed on reading data */
	kErrorReadFailure,
	/** Failed on writing data */
	kErrorWriteFailure,
	/** File or stream seek operation failed */
	kErrorSeekFailure,
	/** Permission not granted to perform operation */
	kErrorAccessDenied,
	/** Data resource not found */
	kErrorResourceNotFound,
	/** IO Error */
	kErrorIO,

	/** Memory system errors */
	kErrorMemory = 0x7200,
	/** Not enough memory to perform operation */
	kErrorOutOfMemory,
	/** Data isn't valid or memory overrun detected */
	kErrorDataCorruption,
	/** Data wasn't aligned on a proper byte boundary */
	kErrorBadAlignment,
	/** No data remaining */
	kErrorNoData,
	/** Buffer not large enough for operation */
	kErrorBufferTooSmall,
	/** Buffer is too large for this operation */
	kErrorBufferTooLarge,

	/** Sound system errors */
	kErrorAudio = 0x7300,
	/** Audio driver failed */
	kErrorAudioDriverFailure,

	/** Network errors */
	kErrorNetwork = 0x7400,
	/** Data required for stream was not available in time */
	kErrorDataStarvation,
	/** Network socket failure */
	kErrorSocketFailure,
	/** Failed on accepting a connection */
	kErrorAcceptFailure,
	/** Network address not found */
	kErrorAddressNotFound,
	/** Network flow interrupted (Or blocked) */
	kErrorFlowInterrupted,

	/** Video and shader errors */
	kErrorVideo = 0x7500,
	/** Color depth requested is not available on current hardware */
	kErrorColorDepthNotSupported,
	/** Requested display resolution is not available on current hardware */
	kErrorResolutionNotSupported,

	/** Thread and process errors */
	kErrorThread = 0x7600,
	/** Operation timed out */
	kErrorTimeout,
	/** Enumeration operation is in progress */
	kErrorEnumerationInProgress,
	/** Enumeration operation isn't running */
	kErrorNotEnumerating
};

}
/* END */

#endif
