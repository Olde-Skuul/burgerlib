/***************************************

	Class to handle critical sections

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_WINDOWS)

// InitializeCriticalSectionAndSpinCount() is minimum XP
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0502
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks
	
***************************************/

Burger::CriticalSection::CriticalSection()
{
	InitializeCriticalSectionAndSpinCount(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock),1000);
}

Burger::CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}

/***************************************

	Lock the Mutex
	
***************************************/

void Burger::CriticalSection::Lock()
{
	EnterCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}

/***************************************

	Try to lock the Mutex
	
***************************************/

Word Burger::CriticalSection::TryLock()
{
	return TryEnterCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}


/***************************************

	Unlock the Mutex
	
***************************************/

void Burger::CriticalSection::Unlock()
{
	LeaveCriticalSection(reinterpret_cast< ::CRITICAL_SECTION *>(&m_Lock));
}
	
#endif