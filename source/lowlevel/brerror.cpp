/***************************************

	Error codes.

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brerror.h"
#include "brstringfunctions.h"

#if defined(BURGER_MSVC) && (BURGER_MSVC >= 140000000)
// Prefer 'enum class' over 'enum' (Enum.3)
#pragma warning(disable : 26812)
#endif

/*! ************************************

	\enum Burger::eError
	\brief Global error codes

	When a Burgerlib function fails, it will return one of these error codes. If
	the code returned by the operating system can't be mapped to these codes, it
	will be returned unmodified instead.

***************************************/

/*! ************************************

	\brief Convert error code to a meaningful message.

	Sometimes it's better to get an string explaining the error code. It's
	implemented in a way that it will never allocate memory, therefore it can
	safely be called from an error handler.

	It the output buffer size is set to zero, it will not attempt to return any
	string data, just the length of the output in case the caller needs to know
	the size of the data in advance to allocate a buffer. In a majority of
	cases, a 256 byte hard coded buffer will suffice.

	\param pOutput Pointer to the buffer to receive the "C" string
	\param uOutputSize Length of the output buffer.
	\param uError Error code to convert to message.

***************************************/

uintptr_t BURGER_API Burger::GetErrorString(
	char* pOutput, uintptr_t uOutputSize, eError uError) BURGER_NOEXCEPT
{
	const char* pErrorMessage;
	switch (uError) {
	case kErrorNone:
		pErrorMessage = "No error";
		break;
	case kErrorGeneric:
		pErrorMessage = "Generic error";
		break;
	case kErrorInvalidParameter:
		pErrorMessage = "Input parameter is invalid";
		break;
	case kErrorInvalidArgument:
		pErrorMessage = "Invalid argument in function call";
		break;
	case kErrorNotInitialized:
		pErrorMessage = "Class or subsystem not initialized";
		break;
	case kErrorNotSupported:
		pErrorMessage = "Request is not supported";
		break;
	case kErrorNotSupportedOnThisPlatform:
		pErrorMessage = "Function not supported on this platform";
		break;
	case kErrorVersionMismatch:
		pErrorMessage = "Version mismatch";
		break;
	case kErrorItemNotFound:
		pErrorMessage = "Item was not found";
		break;
	case kErrorNotReady:
		pErrorMessage = "Not ready";
		break;
	case kErrorOutOfEntries:
		pErrorMessage = "Out of available entries";
		break;
	case kErrorOutOfBounds:
		pErrorMessage = "Index is beyond the allowed range";
		break;
	case kErrorFailedTest:
		pErrorMessage = "Unit test failure";
		break;
	case kErrorDuplicateEntries:
		pErrorMessage = "Data was found more than once";
		break;

	case kErrorFile:
		pErrorMessage = "File system failure";
		break;
		/**  */
	case kErrorFileNotFound:
		pErrorMessage = "File not found";
		break;
	case kErrorPathNotFound:
		pErrorMessage = "Directory to file not found";
		break;
	case kErrorVolumeNotFound:
		pErrorMessage = "Volume not found";
		break;
	case kErrorWriteProtected:
		pErrorMessage = "No write access";
		break;
	case kErrorNoMoreFileDescriptors:
		pErrorMessage = "No more file descriptors are available";
		break;
	case kErrorFileTableOverflow:
		pErrorMessage = "System file table overflow";
		break;
	case kErrorDirectoryNotEmpty:
		pErrorMessage = "Files are present in the directory";
		break;
	case kErrorNotAFile:
		pErrorMessage = "Operation attempted on an object that wasn't a file";
		break;
	case kErrorNotADirectory:
		pErrorMessage =
			"Operation attempted on an object that wasn't a directory";
		break;
	case kErrorFileExists:
		pErrorMessage = "File already exists";
		break;
	case kErrorOpenFailure:
		pErrorMessage = "Failed to open an object";
		break;
	case kErrorReadFailure:
		pErrorMessage = "Failed on reading data";
		break;
	case kErrorWriteFailure:
		pErrorMessage = "Failed on writing data";
		break;
	case kErrorSeekFailure:
		pErrorMessage = "File or stream seek operation failed";
		break;
	case kErrorAccessDenied:
		pErrorMessage = "Permission not granted to perform operation";
		break;
	case kErrorResourceNotFound:
		pErrorMessage = "Data resource not found";
		break;
	case kErrorIO:
		pErrorMessage = "IO Error";
		break;

	case kErrorMemory:
		pErrorMessage = "Memory system failure";
		break;
	case kErrorOutOfMemory:
		pErrorMessage = "Not enough memory to perform operation";
		break;
	case kErrorDataCorruption:
		pErrorMessage = "Data isn't valid or memory overrun detected";
		break;
	case kErrorBadAlignment:
		pErrorMessage = "Data wasn't aligned on a proper byte boundary";
		break;
	case kErrorNoData:
		pErrorMessage = "No data remaining";
		break;
	case kErrorBufferTooSmall:
		pErrorMessage = "Buffer not large enough for operation";
		break;
	case kErrorBufferTooLarge:
		pErrorMessage = "Buffer is too large for this operation";
		break;

	case kErrorAudio:
		pErrorMessage = "Sound system error";
		break;
	case kErrorAudioDriverFailure:
		pErrorMessage = "Audio driver failed";
		break;
	case kErrorAudioFormatNotSupported:
		pErrorMessage = "Audio data format is not supported by the driver";
		break;

	case kErrorNetwork:
		pErrorMessage = "Network error";
		break;
	case kErrorCancelled:
		pErrorMessage = "User cancelled";
		break;
	case kErrorRefused:
		pErrorMessage = "Network operation was refused";
		break;
	case kErrorNetworkFailure:
		pErrorMessage = "Network hardware or software failure";
		break;
	case kErrorDataStarvation:
		pErrorMessage = "Data required for stream was not available in time";
		break;
	case kErrorSocketFailure:
		pErrorMessage = "Network socket failure";
		break;
	case kErrorAcceptFailure:
		pErrorMessage = "Failed on accepting a connection";
		break;
	case kErrorAddressNotFound:
		pErrorMessage = "Network address not found";
		break;
	case kErrorFlowInterrupted:
		pErrorMessage = "Network flow interrupted (Or blocked)";
		break;

	case kErrorVideo:
		pErrorMessage = "Video system error";
		break;
	case kErrorColorDepthNotSupported:
		pErrorMessage =
			"Color depth requested is not available on current hardware";
		break;
	case kErrorResolutionNotSupported:
		pErrorMessage =
			"Requested display resolution is not available on current hardware";
		break;
	case kErrorGPUFailure:
		pErrorMessage = "GPU hardware fault";
		break;
	case kErrorShaderCompile:
		pErrorMessage = "Shader didn't compile";
		break;
	case kErrorShaderIncludeMissing:
		pErrorMessage = "Shader has a bad include";
		break;
	case kErrorOutOfVideoMemory:
		pErrorMessage = "Out of video memory";
		break;
	case kErrorPalette:
		pErrorMessage = "Palette upload failure";
		break;

	case kErrorThread:
		pErrorMessage = "Thread error";
		break;
	case kErrorTimeout:
		pErrorMessage = "Operation timed out";
		break;
	case kErrorEnumerationInProgress:
		pErrorMessage = "Enumeration operation is in progress";
		break;
	case kErrorNotEnumerating:
		pErrorMessage = "Enumeration operation isn't running";
		break;
	case kErrorCantLock:
		pErrorMessage = "Thread lock failure";
		break;
	case kErrorCantUnlock:
		pErrorMessage = "Thread unlock failure";
		break;
	case kErrorThreadNotStarted:
		pErrorMessage = "Thread couldn't start";
		break;
	case kErrorThreadCantStop:
		pErrorMessage = "Thread won't stop";
		break;

	default:
		pErrorMessage = "Unknown error";
		break;
	}

	// Return the string
	const uintptr_t uStringLength = StringLength(pErrorMessage);
	// Return the string
	if (uOutputSize) {
		StringCopy(pOutput, uOutputSize, pErrorMessage);
	}
	return uStringLength;
}
