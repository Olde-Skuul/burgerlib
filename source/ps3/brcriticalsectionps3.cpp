/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcriticalsection.h"

#if defined(BURGER_PS3)
#include "brstringfunctions.h"
#include <sys/synchronization.h>

/***************************************

	Initialize the spin count to 1000 since this
	class is usually used for quick data locks
	
***************************************/

Burger::CriticalSection::CriticalSection()
{
	// Create a First In, First Out, recursive lock
	// to mimic the PC CRITICAL_SECTION

    sys_lwmutex_attribute_t LockAttributes;
	MemoryClear(&LockAttributes,sizeof(LockAttributes));
    LockAttributes.attr_protocol = SYS_SYNC_FIFO;
    LockAttributes.attr_recursive = SYS_SYNC_RECURSIVE;
    sys_lwmutex_create(reinterpret_cast< ::sys_lwmutex_t *>(&m_Lock),&LockAttributes);
}

Burger::CriticalSection::~CriticalSection()
{
	sys_lwmutex_destroy(reinterpret_cast< ::sys_lwmutex_t *>(&m_Lock));
}

/***************************************

	Lock the Mutex
	
***************************************/

void Burger::CriticalSection::Lock()
{
	sys_lwmutex_lock(reinterpret_cast< ::sys_lwmutex_t *>(&m_Lock),0);
}

/***************************************

	Try to lock the Mutex
	
***************************************/

Word Burger::CriticalSection::TryLock()
{
	return sys_lwmutex_trylock(reinterpret_cast< ::sys_lwmutex_t *>(&m_Lock))==CELL_OK;
}


/***************************************

	Unlock the Mutex
	
***************************************/

void Burger::CriticalSection::Unlock()
{
	sys_lwmutex_unlock(reinterpret_cast< ::sys_lwmutex_t *>(&m_Lock));
}
	
#endif