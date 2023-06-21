/***************************************

	Shims for API-MS-Win-Core-Synch-l1-2-0.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_SYNCHAPI_H__
#define __WIN_SYNCHAPI_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern uint_t BURGER_API WaitOnAddress(volatile void* Address,
	void* CompareAddress, uintptr_t AddressSize,
	uint32_t dwMilliseconds) BURGER_NOEXCEPT;
extern void BURGER_API WakeByAddressSingle(void* Address) BURGER_NOEXCEPT;
extern void BURGER_API WakeConditionVariable(
	_RTL_CONDITION_VARIABLE* ConditionVariable) BURGER_NOEXCEPT;
extern void BURGER_API WakeAllConditionVariable(
	_RTL_CONDITION_VARIABLE* ConditionVariable) BURGER_NOEXCEPT;
extern uint_t BURGER_API SleepConditionVariableSRW(
	_RTL_CONDITION_VARIABLE* ConditionVariable, _RTL_SRWLOCK* SRWLock,
	uint32_t dwMilliseconds, uint32_t Flags) BURGER_NOEXCEPT;
extern uint_t BURGER_API SleepConditionVariableCS(
	_RTL_CONDITION_VARIABLE* ConditionVariable,
	_RTL_CRITICAL_SECTION* CriticalSection,
	uint32_t dwMilliseconds) BURGER_NOEXCEPT;
}}
/* END */

#endif
