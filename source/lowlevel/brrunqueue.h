/***************************************

	Run Queue execution handler

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRUNQUEUE_H__
#define __BRRUNQUEUE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class RunQueue {
	BURGER_DISABLECOPYCONSTRUCTORS(RunQueue);
public:
	enum eReturnCode {
		OKAY,		///< Executed normally
		ABORT,		///< Error occurred that requires an immediate abort
		DISPOSE		///< Dispose of this callback
	};
	enum {
		PRIORITY_FIRST=0x7FFFFFF,			///< Highest priority for RunQueue tasks, executed first (Reserved for Burgerlib, do not use or exceed this value)
		PRIORITY_JOYPAD=0x7000040,			///< Priority for reading joypad (Can generate keystrokes and mouse events)
		PRIORITY_MOUSE=0x70000030,			///< Priority for reading mouse (Can generate keystrokes)
		PRIORITY_KEYBOARD=0x70000010,		///< Priority for reading keyboard
		PRIORITY_INPUTPROCESSING=0x7000000,	///< Priority for processing game input
		PRIORITY_SEQUENCING=0x68001000,		///< Priority for music processing
		PRIORITY_SOUNDPROCESSING=0x6800000,	///< Priority for handling sound effects
		PRIORITY_FILEPROCESSING=0x6400000,	///< Priority for async file I/O processing
		PRIORITY_HIGH=0x6000000,			///< High priority for RunQueue tasks
		PRIORITY_MEDIUM=0x4000000,			///< Average priority for RunQueue tasks
		PRIORITY_LOW=0x2000000,				///< Low priority for RunQueue tasks
		PRIORITY_LAST=0						///< Lowest priority for RunQueue tasks, executed last, do not go lower than this value
	};
	typedef eReturnCode (BURGER_API *CallbackProc)(void *);
private:
	struct RunQueueEntry_t {
		RunQueueEntry_t *m_pNext;	///< Handle to the next FunctionEntry_t in chain
		CallbackProc m_pProc;		///< Function to call for this entry
		void *m_pData;				///< User supplied data pointer to call the function with
		Word m_uPriority;			///< User supplied priority for inserting a new entry into the list
	};
	RunQueueEntry_t *m_pFirst;	///< Pointer to the first entry
	Word m_Recurse;				///< \ref TRUE if this class is the process of executing.
public:
	RunQueue() : m_pFirst(NULL), m_Recurse(FALSE) {}
	~RunQueue();
	void BURGER_API Call(void);
	Word BURGER_API Add(CallbackProc pProc,void *pData=NULL,Word uPriority=PRIORITY_MEDIUM);
	Word BURGER_API Find(CallbackProc pProc) const;
	Word BURGER_API Find(CallbackProc pProc,void *pData) const;
	Word BURGER_API RemoveAll(CallbackProc pProc);
	Word BURGER_API Remove(CallbackProc pProc,void *pData=NULL);
	void BURGER_API Clear(void);
};
}
/* END */

#endif

