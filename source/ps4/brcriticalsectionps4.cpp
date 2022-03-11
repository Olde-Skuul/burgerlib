/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_PS4)
#include "brstringfunctions.h"
#include <kernel.h>
#include <sceerror.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT: m_Lock(nullptr)
{
	// Create a First In, First Out, recursive lock
	// to mimic the PC CRITICAL_SECTION

	ScePthreadMutexattr LockAttributes;
	scePthreadMutexattrInit(&LockAttributes);
	scePthreadMutexattrSettype(&LockAttributes, SCE_PTHREAD_MUTEX_RECURSIVE);
	scePthreadMutexInit(&m_Lock, &LockAttributes, nullptr);
	scePthreadMutexattrDestroy(&LockAttributes);
}

Burger::CriticalSection::~CriticalSection()
{
	scePthreadMutexDestroy(&m_Lock);
}

/***************************************

	Lock the Mutex

***************************************/

void Burger::CriticalSection::Lock(void) BURGER_NOEXCEPT
{
	scePthreadMutexLock(&m_Lock);
}

/***************************************

	Try to lock the Mutex

***************************************/

uint_t Burger::CriticalSection::TryLock(void) BURGER_NOEXCEPT
{
	return scePthreadMutexTrylock(&m_Lock) == SCE_OK;
}

/***************************************

	Unlock the Mutex

***************************************/

void Burger::CriticalSection::Unlock(void) BURGER_NOEXCEPT
{
	scePthreadMutexUnlock(&m_Lock);
}

#endif
