/***************************************

	Shims for kernel32.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_kernel32.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brstring16.h"
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

// Function prototypes, lovingly stolen from kernel32.h
typedef UINT(WINAPI* GetSystemWow64DirectoryAPtr)(LPSTR lpBuffer, UINT uSize);
typedef UINT(WINAPI* GetSystemWow64DirectoryWPtr)(LPWSTR lpBuffer, UINT uSize);
typedef BOOL(WINAPI* IsDebuggerPresentPtr)(VOID);
typedef HRESULT(WINAPI* SetThreadDescriptionPtr)(HANDLE, PCWSTR);
typedef VOID(WINAPI* InitializeSRWLockPtr)(_RTL_SRWLOCK*);
typedef VOID(WINAPI* ReleaseSRWLockExclusivePtr)(_RTL_SRWLOCK*);
typedef VOID(WINAPI* AcquireSRWLockExclusivePtr)(_RTL_SRWLOCK*);
typedef BOOLEAN(WINAPI* TryAcquireSRWLockExclusivePtr)(_RTL_SRWLOCK*);

// Unit tests for pointers

// GetSystemWow64DirectoryAPtr gGetSystemWow64DirectoryA =
//	::GetSystemWow64DirectoryA;
// GetSystemWow64DirectoryWPtr gGetSystemWow64DirectoryW =
//	::GetSystemWow64DirectoryW;
// IsDebuggerPresentPtr gIsDebuggerPresent = ::IsDebuggerPresent;
// SetThreadDescriptionPtr gSetThreadDescription = ::SetThreadDescription;
// InitializeSRWLockPtr gInitializeSRWLock = InitializeSRWLock;
// ReleaseSRWLockExclusivePtr gReleaseSRWLockExclusive =
// ReleaseSRWLockExclusive; AcquireSRWLockExclusivePtr gAcquireSRWLockExclusive
// = ::AcquireSRWLockExclusive; TryAcquireSRWLockExclusivePtr
// gTryAcquireSRWLockExclusive =
//	::TryAcquireSRWLockExclusive;
#endif

//
// kernel32.dll
//

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryA

	Manually load kernel32.dll if needed and call the Windows function
	GetSystemWow64DirectoryA()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx

	\windowsonly

	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in bytes

	\return If the function succeeds, the return value is the length, in bytes,
		of the string copied to the buffer

***************************************/

UINT BURGER_API Burger::Win32::GetSystemWow64DirectoryA(
	char* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	void* pGetSystemWow64DirectoryA =
		load_function(kCall_GetSystemWow64DirectoryA);

	// Failure
	UINT uResult = 0;
	if (!pGetSystemWow64DirectoryA) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult = static_cast<GetSystemWow64DirectoryAPtr>(
			pGetSystemWow64DirectoryA)(pBuffer, uSize);
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call GetSystemWow64DirectoryW

	Manually load kernel32.dll if needed and call the Windows function
	GetSystemWow64DirectoryW()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms724405(v=vs.85).aspx

	\windowsonly

	\param pBuffer A pointer to the buffer to receive the path
	\param uSize The maximum size of the buffer, in uint16_t entries

	\return If the function succeeds, the return value is the length, in
		uint16_t entries, of the string copied to the buffer

***************************************/

UINT BURGER_API Burger::Win32::GetSystemWow64DirectoryW(
	uint16_t* pBuffer, uint32_t uSize) BURGER_NOEXCEPT
{
	void* pGetSystemWow64DirectoryW =
		load_function(kCall_GetSystemWow64DirectoryW);

	// Failure
	UINT uResult = 0;
	if (!pGetSystemWow64DirectoryW) {
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	} else {
		uResult =
			static_cast<GetSystemWow64DirectoryWPtr>(pGetSystemWow64DirectoryW)(
				reinterpret_cast<LPWSTR>(pBuffer), uSize);
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call IsDebuggerPresent

	Manually load kernel32.dll if needed and call the Windows function
	IsDebuggerPresent()

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms680345(v=vs.85).aspx

	\windowsonly

	\return Nonzero if a debugger is present, Zero if none is found

***************************************/

BOOL BURGER_API Burger::Win32::IsDebuggerPresent(void) BURGER_NOEXCEPT
{
	void* pIsDebuggerPresent = load_function(kCall_IsDebuggerPresent);

	// None
	BOOL uResult = 0;
	if (pIsDebuggerPresent) {
		uResult = static_cast<IsDebuggerPresentPtr>(pIsDebuggerPresent)();
	}
	return uResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call SetThreadDescription

	Manually load kernel32.dll if needed and call the Windows function
	SetThreadDescription()

	https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription

	\windowsonly

	\param hThread Handle to the thread to work with
	\param lpThreadDescription Name of the thread in a UTF16 "C" string

	\return Zero if no error, E_FAIL if function not found.

***************************************/

HRESULT BURGER_API Burger::Win32::SetThreadDescription(
	void* hThread, const uint16_t* lpThreadDescription) BURGER_NOEXCEPT
{
	void* pSetThreadDescription = load_function(kCall_SetThreadDescription);

	// None
	HRESULT lResult = E_FAIL;
	if (pSetThreadDescription) {
		lResult = static_cast<SetThreadDescriptionPtr>(pSetThreadDescription)(
			hThread, reinterpret_cast<PCWSTR>(lpThreadDescription));
	}
	return lResult;
}

/*! ************************************

	\brief Load in kernel32.dll and call InitializeSRWLock

	Manually load kernel32.dll if needed and call the Windows function
	InitializeSRWLock()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializesrwlock

	\note This function is present only in Windows Vista or later

	\param pSRWLock A pointer to the SRW lock

***************************************/

void BURGER_API Burger::Win32::InitializeSRWLock(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT
{
	void* pInitializeSRWLock = load_function(kCall_InitializeSRWLock);
	if (pInitializeSRWLock) {
		static_cast<InitializeSRWLockPtr>(pInitializeSRWLock)(pSRWLock);
	}
}

/*! ************************************

	\brief Load in kernel32.dll and call AcquireSRWLockExclusive

	Manually load kernel32.dll if needed and call the Windows function
	AcquireSRWLockExclusive()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-acquiresrwlockexclusive

	\note This function is present only in Windows Vista or later

	\param pSRWLock A pointer to the SRW lock

***************************************/

void BURGER_API Burger::Win32::AcquireSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT
{
	void* pAcquireSRWLockExclusive =
		load_function(kCall_AcquireSRWLockExclusive);
	if (pAcquireSRWLockExclusive) {
		static_cast<AcquireSRWLockExclusivePtr>(pAcquireSRWLockExclusive)(
			pSRWLock);
	}
}

/*! ************************************

	\brief Load in kernel32.dll and call TryAcquireSRWLockExclusive

	Manually load kernel32.dll if needed and call the Windows function
	TryAcquireSRWLockExclusive()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-tryacquiresrwlockexclusive

	\note This function is present only in Windows 7 or later

	\param pSRWLock A pointer to the SRW lock

	\return 0 on failure, non-zero on acquiring the lock

***************************************/

uint_t BURGER_API Burger::Win32::TryAcquireSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT
{
	void* pTryAcquireSRWLockExclusive =
		load_function(kCall_TryAcquireSRWLockExclusive);

	// Assume failure
	BOOL uResult = FALSE;
	if (pTryAcquireSRWLockExclusive) {
		uResult = static_cast<TryAcquireSRWLockExclusivePtr>(
			pTryAcquireSRWLockExclusive)(pSRWLock);
	}
	return static_cast<uint_t>(uResult);
}

/*! ************************************

	\brief Load in kernel32.dll and call ReleaseSRWLockExclusive

	Manually load kernel32.dll if needed and call the Windows function
	ReleaseSRWLockExclusive()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-releasesrwlockexclusive

	\note This function is present only in Windows Vista or later

	\param pSRWLock A pointer to the SRW lock

***************************************/

void BURGER_API Burger::Win32::ReleaseSRWLockExclusive(
	_RTL_SRWLOCK* pSRWLock) BURGER_NOEXCEPT
{
	void* pReleaseSRWLockExclusive =
		load_function(kCall_ReleaseSRWLockExclusive);
	if (pReleaseSRWLockExclusive) {
		static_cast<ReleaseSRWLockExclusivePtr>(pReleaseSRWLockExclusive)(
			pSRWLock);
	}
}

/*! ************************************

	\brief Send an exception to the debugger to name this thread

	On Windows, to change the name of a thread, an exception is thrown that is
	captured by the debugger. A test will be performed if the debugger is
	attached and if so, the exception is thrown.

	\note If this exception is interfering with C# or other high level code, set
		the \ref eHintFlags \ref kHintWin32DisableThreadNamingException to
		disable this function.

	More reading:
	https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2022

	\sa get_hintflags(), or kHintWin32DisableThreadNamingException

***************************************/

#if !defined(DOXYGEN)

// Each element is 32 bits wide on 32 bit systems, and 64 bits wide on 64 bit
// systems
typedef struct tagTHREADNAME_INFO {
	uintptr_t dwType;     // Must be 0x1000.
	LPCSTR szName;        // Pointer to name (in user addr space).
	uintptr_t dwThreadID; // Thread ID (-1=caller thread).
	uintptr_t dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;

#endif

void BURGER_API Burger::Win32::throw_thread_naming_exception(
	uint32_t uThreadID, const char* pThreadName) BURGER_NOEXCEPT
{
	// Is the feature disabled?
	if (!(get_hintflags() & kHintWin32DisableThreadNamingException)) {

		// Only the debugger gets this exception.
		// Do NOT throw the exception if no debugger is present
		if (Win32::IsDebuggerPresent()) {

			// Force the ThreadID to this process ID
			if (!uThreadID) {
				uThreadID = static_cast<uint32_t>(-1);
			}

			// Invoke the DEEP magic by throwing an exception that's captured by
			// the Visual Studio debugger

			// Create the exception message
			THREADNAME_INFO NewThreadName;
			NewThreadName.dwType = 0x1000U;
			NewThreadName.szName = pThreadName;
			NewThreadName.dwThreadID = uThreadID;
			NewThreadName.dwFlags = 0;

			// Send a message to the debugger and return
			RaiseException(0x406D1388U, 0,
				sizeof(NewThreadName) / sizeof(ULONG_PTR),
				reinterpret_cast<const ULONG_PTR*>(&NewThreadName));
		}
	}
}

/*! ************************************

	\brief Set a thread's name for debugging.

	On Windows, either the function SetThreadDescription() is called, or an
	exception is thrown that's captured by the debugger so a thread can be
	named.

	This function will call the functions to do both actions.

	\note If this function is interfering with C# or other high level code, set
		the \ref eHintFlags \ref kHintWin32DisableThreadNamingException to
		disable this function.

	\sa SetThreadDescription(), throw_thread_naming_exception(), or
		kHintWin32DisableThreadNamingException

***************************************/

void BURGER_API Burger::Win32::set_thread_name(
	const char* pName, void* hThread, uint32_t uThreadId) BURGER_NOEXCEPT
{
	// Convert the name from UTF8 to UTF16
	String16 TempName(pName);

	// Use the new Windows API
	Win32::SetThreadDescription(hThread, TempName.c_str());

	// Throw the exception
	throw_thread_naming_exception(uThreadId, pName);
}

#endif
