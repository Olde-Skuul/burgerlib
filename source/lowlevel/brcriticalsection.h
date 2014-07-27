/***************************************

	Class to handle critical sections

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRCRITICALSECTION_H__
#define __BRCRITICALSECTION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

#if defined(BURGER_PS3) && !defined(__BRPS3TYPES_H__)
#include "brps3types.h"
#endif

#if defined(BURGER_PS4) && !defined(__BRPS4TYPES_H__)
#include "brps4types.h"
#endif

/* BEGIN */
namespace Burger {
class CriticalSection {
	BURGER_DISABLECOPYCONSTRUCTORS(CriticalSection);
#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)
	CRITICAL_SECTION m_Lock;	///< Critical section for OS calls (Windows and Xbox 360 only)
#endif
#if defined(BURGER_PS3) || defined(DOXYGEN)
	sys_lwmutex_t m_Lock;		///< Critical section for PS3 (PS3 only)
#endif
#if defined(BURGER_PS4) || defined(DOXYGEN)
	pthread_mutex *m_Lock;		///< Critical section for PS4 (PS4 only)
#endif
public:
	CriticalSection();
	~CriticalSection();
	void Lock(void);
	Word TryLock(void);
	void Unlock(void);
};
class CriticalSectionStatic : public CriticalSection {
	Word m_bValid;			///< Set to \ref TRUE when constructed
public:
	CriticalSectionStatic();
	~CriticalSectionStatic();
	BURGER_INLINE void Lock(void) {	if (m_bValid) { CriticalSection::Lock(); }}
	BURGER_INLINE Word TryLock(void) { if (m_bValid) { return CriticalSection::TryLock(); } return FALSE; }
	BURGER_INLINE void Unlock(void) { if (m_bValid) { CriticalSection::Unlock(); }}
};
class CriticalSectionLock {
	BURGER_DISABLECOPYCONSTRUCTORS(CriticalSectionLock);
	CriticalSection *m_pCriticalSection;			///< Pointer to the lock held
public:
	CriticalSectionLock(CriticalSection *pCriticalSection) : 
		m_pCriticalSection(pCriticalSection) { m_pCriticalSection->Lock(); }
	~CriticalSectionLock() { m_pCriticalSection->Unlock(); }
};
}
/* END */

#endif
