/***************************************

	Class to handle critical sections, MacOS version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_MAC)
#include <OpenTransport.h>

/***************************************

	Initialize the CriticalSection

***************************************/

Burger::CriticalSection::CriticalSection() BURGER_NOEXCEPT
{
	OTClearLock(&m_bLock);
}

Burger::CriticalSection::~CriticalSection() {}

/***************************************

	Lock the CriticalSection

***************************************/

void Burger::CriticalSection::Lock() BURGER_NOEXCEPT
{
	while (OTAcquireLock(&m_bLock)) {
	}
}

/***************************************

	Try to lock the CriticalSection

***************************************/

uint_t Burger::CriticalSection::TryLock() BURGER_NOEXCEPT
{
	return OTAcquireLock(&m_bLock);
}

/***************************************

	Unlock the CriticalSection

***************************************/

void Burger::CriticalSection::Unlock() BURGER_NOEXCEPT
{
	OTClearLock(&m_bLock);
}

#endif
