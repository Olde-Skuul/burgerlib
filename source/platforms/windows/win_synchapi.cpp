/***************************************

	Shims for API-MS-Win-Core-Synch-l1-2-0.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_synchapi.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

// #include <synchapi.h>

// Function prototypes, lovingly stolen from synchapi.h
typedef BOOL(WINAPI* WaitOnAddressPtr)(volatile VOID* Address,
	PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds);
typedef VOID(WINAPI* WakeByAddressSinglePtr)(PVOID Address);
typedef VOID(WINAPI* WakeConditionVariablePtr)(
	_RTL_CONDITION_VARIABLE* ConditionVariable);
typedef VOID(WINAPI* WakeAllConditionVariablePtr)(
	_RTL_CONDITION_VARIABLE* ConditionVariable);
typedef BOOL(WINAPI* SleepConditionVariableSRWPtr)(
	_RTL_CONDITION_VARIABLE* ConditionVariable, _RTL_SRWLOCK* SRWLock,
	DWORD dwMilliseconds, ULONG Flags);
typedef BOOL(WINAPI* SleepConditionVariableCSPtr)(
	_RTL_CONDITION_VARIABLE* ConditionVariable,
	_RTL_CRITICAL_SECTION* CriticalSection, DWORD dwMilliseconds);

// Unit tests for pointers
// WaitOnAddressPtr gWaitOnAddress = ::WaitOnAddress;
// WakeByAddressSinglePtr gWakeByAddressSingle = ::WakeByAddressSingle;
// WakeConditionVariablePtr gWakeConditionVariable = ::WakeConditionVariable;
// WakeAllConditionVariablePtr gWakeAllConditionVariable =
//	::WakeAllConditionVariable;
// SleepConditionVariableSRWPtr gSleepConditionVariableSRW =
//	::SleepConditionVariableSRW;
// SleepConditionVariableCSPtr gSleepConditionVariableCS =
//	::SleepConditionVariableCS;

#endif

//
// API-MS-Win-Core-Synch-l1-2-0.dll
//

/*! ************************************

	\brief Load in Synch-l1-2-0.dll and call WaitOnAddress

	Manually load Synch-l1-2-0.dll if needed and call the Windows function
	WaitOnAddress()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitonaddress

	\note This function is present only in Windows 8 or later

	\param Address The address on which to wait.
	\param CompareAddress A pointer to the location of the previously observed
		value at Address

	\param AddressSize The size of the value, in bytes.

	\param dwMilliseconds The number of milliseconds to wait before the
		operation times out.

	\return TRUE if the wait succeeded.

***************************************/

uint_t BURGER_API Burger::Win32::WaitOnAddress(volatile void* Address,
	void* CompareAddress, uintptr_t AddressSize,
	uint32_t dwMilliseconds) BURGER_NOEXCEPT
{
	void* pWaitOnAddress = load_function(kCall_WaitOnAddress);

	// Failure
	BOOL bResult = FALSE;
	if (pWaitOnAddress) {
		bResult = static_cast<WaitOnAddressPtr>(pWaitOnAddress)(
			Address, CompareAddress, AddressSize, dwMilliseconds);
	}
	return static_cast<uint_t>(bResult);
}

/*! ************************************

	\brief Load in Synch-l1-2-0.dll and call WakeByAddressSingle

	Manually load Synch-l1-2-0.dll if needed and call the Windows function
	WakeByAddressSingle()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-wakebyaddresssingle

	\note This function is present only in Windows 8 or later

	\param Address The address to signal

***************************************/

void BURGER_API Burger::Win32::WakeByAddressSingle(
	void* Address) BURGER_NOEXCEPT
{
	void* pWakeByAddressSingle = load_function(kCall_WakeByAddressSingle);

	if (!pWakeByAddressSingle) {
		// Signal that this call failed
		SetLastError(ERROR_FUNCTION_NOT_CALLED);
	} else {
		static_cast<WakeByAddressSinglePtr>(pWakeByAddressSingle)(Address);
	}
}

/*! ************************************

	\brief Load in kernel32.dll and call WakeConditionVariable

	Manually load kernel32.dll if needed and call the Windows function
	WakeConditionVariable()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-wakeconditionvariable

	\note This function is present only in Windows Vista or later

	\param ConditionVariable A pointer to the condition variable.

***************************************/

void BURGER_API Burger::Win32::WakeConditionVariable(
	_RTL_CONDITION_VARIABLE* ConditionVariable) BURGER_NOEXCEPT
{
	void* pWakeConditionVariable = load_function(kCall_WakeConditionVariable);

	if (!pWakeConditionVariable) {
		// Signal that this call failed
		SetLastError(ERROR_FUNCTION_NOT_CALLED);
	} else {
		static_cast<WakeConditionVariablePtr>(pWakeConditionVariable)(
			ConditionVariable);
	}
}

/*! ************************************

	\brief Load in kernel32.dll and call WakeAllConditionVariable

	Manually load kernel32.dll if needed and call the Windows function
	WakeAllConditionVariable()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-wakeallconditionvariable

	\note This function is present only in Windows Vista or later

	\param ConditionVariable A pointer to the condition variable.

***************************************/

void BURGER_API Burger::Win32::WakeAllConditionVariable(
	_RTL_CONDITION_VARIABLE* ConditionVariable) BURGER_NOEXCEPT
{
	void* pWakeAllConditionVariable =
		load_function(kCall_WakeAllConditionVariable);

	if (!pWakeAllConditionVariable) {
		// Signal that this call failed
		SetLastError(ERROR_FUNCTION_NOT_CALLED);
	} else {
		static_cast<WakeAllConditionVariablePtr>(pWakeAllConditionVariable)(
			ConditionVariable);
	}
}

/*! ************************************

	\brief Load in kernel32.dll and call SleepConditionVariableSRW

	Manually load kernel32.dll if needed and call the Windows function
	SleepConditionVariableSRW()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleepconditionvariablesrw

	\note This function is present only in Windows Vista or later

	\param ConditionVariable A pointer to the condition variable.
	\param SRWLock A pointer to the lock

	\param dwMilliseconds The number of milliseconds to wait before the
		operation times out.

	\param Flags Flags to share the lock

	\return TRUE if the wait succeeded.

***************************************/

uint_t BURGER_API Burger::Win32::SleepConditionVariableSRW(
	_RTL_CONDITION_VARIABLE* ConditionVariable, _RTL_SRWLOCK* SRWLock,
	uint32_t dwMilliseconds, uint32_t Flags) BURGER_NOEXCEPT
{
	void* pSleepConditionVariableSRW =
		load_function(kCall_SleepConditionVariableSRW);

	// Failure
	BOOL bResult = FALSE;
	if (pSleepConditionVariableSRW) {
		bResult = static_cast<SleepConditionVariableSRWPtr>(
			pSleepConditionVariableSRW)(
			ConditionVariable, SRWLock, dwMilliseconds, Flags);
	}
	return static_cast<uint_t>(bResult);
}

/*! ************************************

	\brief Load in kernel32.dll and call SleepConditionVariableCS

	Manually load kernel32.dll if needed and call the Windows function
	SleepConditionVariableCS()

	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleepconditionvariablecs

	\note This function is present only in Windows Vista or later

	\param ConditionVariable A pointer to the condition variable.
	\param CriticalSection A pointer to the critical section object.
	\param dwMilliseconds The number of milliseconds to wait before the
		operation times out.

	\return TRUE if the wait succeeded.

***************************************/

uint_t BURGER_API Burger::Win32::SleepConditionVariableCS(
	_RTL_CONDITION_VARIABLE* ConditionVariable,
	_RTL_CRITICAL_SECTION* CriticalSection,
	uint32_t dwMilliseconds) BURGER_NOEXCEPT
{
	void* pSleepConditionVariableCS =
		load_function(kCall_SleepConditionVariableCS);

	// Failure
	BOOL bResult = FALSE;
	if (pSleepConditionVariableCS) {
		bResult =
			static_cast<SleepConditionVariableCSPtr>(pSleepConditionVariableCS)(
				ConditionVariable, CriticalSection, dwMilliseconds);
	}
	return static_cast<uint_t>(bResult);
}

#endif
