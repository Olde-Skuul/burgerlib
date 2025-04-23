/***************************************

	Error codes.

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if !defined(DOXYGEN)
struct ErrorLookup_t {
	Burger::eError m_uError;
	const char* m_pMessage;
};

static const ErrorLookup_t g_ErrorLookup[] = {{Burger::kErrorNone, "No error"},
	{Burger::kErrorGeneric, "Generic error"},
	{Burger::kErrorInvalidParameter, "Input parameter is invalid"},
	{Burger::kErrorInvalidArgument, "Invalid argument in function call"},
	{Burger::kErrorNotInitialized, "Class or subsystem not initialized"},
	{Burger::kErrorAlreadyInitialized,
		"Class or subsystem already initialized"},
	{Burger::kErrorNotSupported, "Request is not supported"},
	{Burger::kErrorNotSupportedOnThisPlatform,
		"Function not supported on this platform"},
	{Burger::kErrorVersionMismatch, "Version mismatch"},
	{Burger::kErrorItemNotFound, "Item was not found"},
	{Burger::kErrorNotReady, "Not ready"},
	{Burger::kErrorOutOfEntries, "Out of available entries"},
	{Burger::kErrorOutOfBounds, "Index is beyond the allowed range"},
	{Burger::kErrorFailedTest, "Unit test failure"},
	{Burger::kErrorDuplicateEntry, "Data was found more than once"},
	{Burger::kErrorRestartRequired, "Restart is required"},
	{Burger::kErrorFile, "File system failure"},
	{Burger::kErrorFileNotFound, "File not found"},
	{Burger::kErrorPathNotFound, "Directory to file not found"},
	{Burger::kErrorVolumeNotFound, "Volume not found"},
	{Burger::kErrorWriteProtected, "No write access"},
	{Burger::kErrorNoMoreFileDescriptors,
		"No more file descriptors are available"},
	{Burger::kErrorFileTableOverflow, "System file table overflow"},
	{Burger::kErrorDirectoryNotEmpty, "Files are present in the directory"},
	{Burger::kErrorNotAFile,
		"Operation attempted on an object that wasn't a file"},
	{Burger::kErrorNotADirectory,
		"Operation attempted on an object that wasn't a directory"},
	{Burger::kErrorFileExists, "File already exists"},
	{Burger::kErrorOpenFailure, "Failed to open an object"},
	{Burger::kErrorReadFailure, "Failed on reading data"},
	{Burger::kErrorWriteFailure, "Failed on writing data"},
	{Burger::kErrorSeekFailure, "File or stream seek operation failed"},
	{Burger::kErrorAccessDenied, "Permission not granted to perform operation"},
	{Burger::kErrorResourceNotFound, "Data resource not found"},
	{Burger::kErrorIO, "IO Error"},
	{Burger::kErrorEndOfFile, "Reached the end of file"},
	{Burger::kErrorIsLocked, "File object is currently locked"},
	{Burger::kErrorMemory, "Memory system failure"},
	{Burger::kErrorOutOfMemory, "Not enough memory to perform operation"},
	{Burger::kErrorDataCorruption,
		"Data isn't valid or memory overrun detected"},
	{Burger::kErrorBadAlignment,
		"Data wasn't aligned on a proper byte boundary"},
	{Burger::kErrorNoData, "No data remaining"},
	{Burger::kErrorBufferTooSmall, "Buffer not large enough for operation"},
	{Burger::kErrorBufferTooLarge, "Buffer is too large for this operation"},
	{Burger::kErrorAudio, "Sound system error"},
	{Burger::kErrorAudioDriverFailure, "Audio driver failed"},
	{Burger::kErrorAudioFormatNotSupported,
		"Audio data format is not supported by the driver"},
	{Burger::kErrorNetwork, "Network error"},
	{Burger::kErrorCancelled, "User cancelled"},
	{Burger::kErrorRefused, "Network operation was refused"},
	{Burger::kErrorNetworkFailure, "Network hardware or software failure"},
	{Burger::kErrorDataStarvation,
		"Data required for stream was not available in time"},
	{Burger::kErrorSocketFailure, "Network socket failure"},
	{Burger::kErrorAcceptFailure, "Failed on accepting a connection"},
	{Burger::kErrorAddressNotFound, "Network address not found"},
	{Burger::kErrorFlowInterrupted, "Network flow interrupted (Or blocked)"},
	{Burger::kErrorVideo, "Video system error"},
	{Burger::kErrorColorDepthNotSupported,
		"Color depth requested is not available on current hardware"},
	{Burger::kErrorResolutionNotSupported,
		"Requested display resolution is not available on current hardware"},
	{Burger::kErrorGPUFailure, "GPU hardware fault"},
	{Burger::kErrorShaderCompile, "Shader didn't compile"},
	{Burger::kErrorShaderIncludeMissing, "Shader has a bad include"},
	{Burger::kErrorOutOfVideoMemory, "Out of video memory"},
	{Burger::kErrorPalette, "Palette upload failure"},
	{Burger::kErrorThread, "Thread error"},
	{Burger::kErrorTimeout, "Operation timed out"},
	{Burger::kErrorEnumerationInProgress,
		"Enumeration operation is in progress"},
	{Burger::kErrorNotEnumerating, "Enumeration operation isn't running"},
	{Burger::kErrorCantLock, "Thread lock failure"},
	{Burger::kErrorCantUnlock, "Thread unlock failure"},
	{Burger::kErrorThreadNotStarted, "Thread couldn't start"},
	{Burger::kErrorThreadAlreadyStarted, "Thread already in progress"},
	{Burger::kErrorThreadCantStop, "Thread won't stop"},
	{Burger::kErrorThreadNotFound, "Invalid thread ID"},
	{Burger::kErrorThreadNotModified, "Thread information not changed"}};
#endif

/*! ************************************

	\enum Burger::eError
	\brief Global error codes

	When a Burgerlib function fails, it will return one of these error codes. If
	the code returned by the operating system can't be mapped to these codes, it
	will be returned unmodified instead.

***************************************/

/*! ************************************

	\struct Burger::error_message_t
	\brief Error message information

	When a Burgerlib function fails, it will call set_last_error(eError,
	uint32_t, const char*) to set the thread local global error message. This
	message will remain until either cleared or overwritten with a new error
	message. Contained is the Burgerlib error code, a native operating system
	error code (if applicable), and a detailed message as to what the error was.

	\sa set_last_error(eError, uint32_t, const char*), or
		get_last_error(void)

***************************************/

#if !defined(DOXYGEN)
static BURGER_THREAD_LOCAL Burger::error_message_t g_ErrorMessage;
#endif

/*! ************************************

	\brief Return the last recorded error message.

	When a Burgerlib function fails, it will call set_last_error(eError,
	uint32_t, const char*) to set the thread local global error message. This
	message will remain until either cleared or overwritten with a new error
	message. This function will return a pointer to the thread local storage
	global record. Make a copy of the data if persistence is desired.

	\note The message is stored in Thread Local Storage so it can't be used
	to get the error from another process or thread.

	\return Pointer to thread local error_message_t structure

	\sa set_last_error(eError, uint32_t, const char*)

***************************************/

const Burger::error_message_t* BURGER_API Burger::get_last_error(
	void) BURGER_NOEXCEPT
{
	return &g_ErrorMessage;
}

/*! ************************************

	\brief Set the global error message.

	If an error occurs, the system can be globally notified by setting the error
	message with this call. The data is stored in thread local storage so the
	message only is for the currently running thread.

	If `uError` is \ref kErrorNone but `uSystemError` is not zero, the
	equivalent Burgerlib error code will be set as the `uError` value. Only if
	`uError` and `uSystemError` are zero will an error condition be cleared.

	\param uError Burgerlib error code
	\param uSystemError Platform specific error code
	\param pMessage Pointer to "C" string with error message

	\sa get_last_error(void)

***************************************/

void BURGER_API Burger::set_last_error(
	eError uError, uint32_t uSystemError, const char* pMessage) BURGER_NOEXCEPT
{
	// Got a system error but not a Burgerlib error, remap
	if (!uError && uSystemError) {
		uError = platform_convert_to_error(uSystemError);
	}

	// If there wasn't a message, determine what generic message it will be
	if (!pMessage) {
		pMessage = error_lookup_string(uError);
	}

	// Set the error entries
	g_ErrorMessage.m_uError = uError;
	g_ErrorMessage.m_uSystemError = uSystemError;

	// Only set the string if there's an error
	if (uError || uSystemError) {
		string_copy(g_ErrorMessage.m_Message, sizeof(g_ErrorMessage.m_Message),
			pMessage);
	} else {
		g_ErrorMessage.m_Message[0] = 0;
	}
}

/*! ************************************

	\brief Convert a Burgerlib error code into a string.

	Look up a Burgerlib error code into an array of error messages and return a
	pointer to a "C" string with a message in English giving a meaningful
	message.

	\note A string is always returned, so no need to test for \ref nullptr.
		The string pointer does not need to be released or freed.

	\param uError Error code to convert to message.

	\return Pointer to a "C" string with an error.

	\sa error_get_string(char *, uintptr_t, eError)

***************************************/

const char* BURGER_API Burger::error_lookup_string(
	eError uError) BURGER_NOEXCEPT
{
	// Assume not found
	const char* pErrorMessage = "Unknown error";

	// Scan the look up table for a match
	uintptr_t uCount = BURGER_ARRAYSIZE(g_ErrorLookup);
	const ErrorLookup_t* pWork = g_ErrorLookup;
	do {

		// Match?
		if (pWork->m_uError == uError) {

			// Got the error message
			pErrorMessage = pWork->m_pMessage;
			break;
		}
		++pWork;
	} while (--uCount);
	return pErrorMessage;
}

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

	\return Length in bytes of the string being returned.

	\sa error_lookup_string(eError)

***************************************/

uintptr_t BURGER_API Burger::error_get_string(
	char* pOutput, uintptr_t uOutputSize, eError uError) BURGER_NOEXCEPT
{
	// Assume not found
	const char* pErrorMessage = error_lookup_string(uError);

	// Return the string
	const uintptr_t uStringLength = string_length(pErrorMessage);
	if (uOutputSize) {
		string_copy(pOutput, uOutputSize, pErrorMessage);
	}
	return uStringLength;
}

/*! ************************************

	\brief Convert platform error code to a Burgerlib error code.

	Take a native platform error code and convert it to a Burgerlib equivalent
	error code.

	\param iNativeError MacOS OSErr or OSStatus, Windows HRESULT error code

	\return Burgerlib error code, or kErrorGeneric if unknown

***************************************/

#if !(defined(BURGER_MAC)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::platform_convert_to_error(
	int iNativeError) BURGER_NOEXCEPT
{
	BURGER_UNUSED(iNativeError);
	return kErrorGeneric;
}
#endif