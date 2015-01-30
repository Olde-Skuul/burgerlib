/***************************************

	Class to handle critical sections

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if defined(BURGER_SHIELD) && !defined(__BRSHIELDTYPES_H__)
#include "brshieldtypes.h"
#endif

#if defined(BURGER_VITA) && !defined(__BRVITATYPES_H__)
#include "brvitatypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
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
#if (defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	friend class ConditionVariable;
	pthread_mutex_t m_Lock;		///< Critical section for Android/MacOSX/iOS (Android/MacOSX/iOS only)
#endif
#if defined(BURGER_VITA) || defined(DOXYGEN)
	int m_iLock;				///< Critical section ID for VITA
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

class Semaphore {
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void *m_pSemaphore;			///< Semaphore HANDLE (Windows only)
#endif
#if (defined(BURGER_SHIELD)) || defined(DOXYGEN)
	sem_t m_Semaphore;			///< Semaphore instance (Android)
	Word m_bInitialized;		///< \ref TRUE if the semaphore instance successfully initialized
#endif
#if defined(BURGER_VITA) || defined(DOXYGEN)
	int m_iSemaphore;				///< Semaphore ID for VITA
#endif
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	semaphore_t m_Semaphore;	///< Semaphore instance (MacOSX/iOS only)
	task_t m_Owner;				///< Task ID of the semaphore owner (MacOSX/iOS only)
	Word m_bInitialized;		///< \ref TRUE if the semaphore instance successfully initialized
#endif
	volatile Word32 m_uCount;	///< Semaphore count value
public:
	Semaphore(Word32 uCount=0);
	~Semaphore();
	BURGER_INLINE Word Acquire(void) { return TryAcquire(BURGER_MAXUINT); }
	Word BURGER_API TryAcquire(Word uMilliseconds=0);
	Word BURGER_API Release(void);
	BURGER_INLINE Word32 GetValue(void) const { return m_uCount; }
};

class ConditionVariable {
#if (defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	pthread_cond_t m_ConditionVariable;	///< Condition variable instance (Android/MacOSX/iOS only)
	Word m_bInitialized;			///< \ref TRUE if the Condition variable instance successfully initialized (Android/MacOSX/iOS only)
#endif
#if (defined(BURGER_VITA)) || defined(DOXYGEN)
	int m_iConditionVariable;	///< Condition variable instance (Vita only)
	int m_iMutex;				///< Mutex for the condition variable (Vita only)
#endif
#if !(defined(BURGER_SHIELD) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	CriticalSection m_CriticalSection;	///< CriticalSection for this class (Non-specialized platforms)
	Semaphore m_WaitSemaphore;		///< Binary semaphore for forcing thread to wait for a signal (Non-specialized platforms)
	Semaphore m_SignalsSemaphore;	///< Binary semaphore for the number of pending signals (Non-specialized platforms)
	Word32 m_uWaiting;				///< Count of waiting threads (Non-specialized platforms)
	Word32 m_uSignals;				///< Count of signals to be processed (Non-specialized platforms)
#endif
public:
	ConditionVariable();
	~ConditionVariable();
	Word BURGER_API Signal(void);
	Word BURGER_API Broadcast(void);
	Word BURGER_API Wait(CriticalSection *pCriticalSection,Word uMilliseconds=BURGER_MAXUINT);
};

class Thread {
public:
	typedef WordPtr (BURGER_API *FunctionPtr)(void *pThis);		///< Thread entry prototype
private:
	FunctionPtr m_pFunction;		///< Pointer to the thread
	void *m_pData;					///< Data pointer for the thread
	Semaphore *m_pSemaphore;		///< Temp Semaphore for synchronization
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void *m_pThreadHandle;			///< HANDLE to the thread (Windows/Xbox 360 only)
	Word32 m_uThreadID;				///< System ID of the thread (Windows/Xbox 360 only)
#endif
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	struct _opaque_pthread_t *m_pThreadHandle;	///< Pointer to the thread data (MacOSX/iOS only)
#endif
#if (defined(BURGER_VITA)) || defined(DOXYGEN)
	int m_iThreadID;				///< System ID of the thread (Vita only)
#endif
	WordPtr m_uResult;				///< Result code of the thread on exit
public:
	Thread();
	Thread(FunctionPtr pFunction,void *pData);
	~Thread();
	Word BURGER_API Start(FunctionPtr pFunction,void *pData);
	Word BURGER_API Wait(void);
	Word BURGER_API Kill(void);
	static void BURGER_API Run(void *pThis);
	BURGER_INLINE WordPtr GetResult(void) const { return m_uResult; }
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	BURGER_INLINE Word IsInitialized(void) const { return m_pThreadHandle!=NULL; }
#elif defined(BURGER_VITA)
	BURGER_INLINE Word IsInitialized(void) const { return m_iThreadID>=0; }
#else 
	BURGER_INLINE Word IsInitialized(void) const { return FALSE; }
#endif
};
}
/* END */

#endif
