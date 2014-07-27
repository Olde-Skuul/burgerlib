/***************************************

	Run Queue execution handler

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
		ABORT,		///< Error occured that requires an immediate abort
		DISPOSE		///< Dispose of this callback
	};
	enum {
		LOWESTPRIORITY=0,		///< Lowest priority for RunQueue tasks, do not go lower than this value
		LOWPRIORITY=25,			///< Low priority for RunQueue tasks
		MEDIUMPRIORITY=50,		///< Average priority for RunQueue tasks
		HIGHPRIORITY=75,		///< High priority for RunQueue tasks
		HIGHESTPRIORITY=100		///< Highest priority for RunQueue tasks, (Reserved for Burgerlib, do not use or exceed this value)
	};
	typedef eReturnCode (BURGER_API *CallbackProc)(void *);
	RunQueue() : m_pFirst(NULL), m_Recurse(FALSE) {}
	~RunQueue();
	void Call(void);
	Word Add(CallbackProc pProc,void *pData=NULL,Word uPriority=MEDIUMPRIORITY);
	Word Find(CallbackProc pProc) const;
	Word Find(CallbackProc pProc,void *pData) const;
	Word RemoveAll(CallbackProc pProc);
	Word Remove(CallbackProc pProc,void *pData=NULL);
	void Clear(void);
private:
	struct RunQueueEntry_t {
		RunQueueEntry_t *m_pNext;	///< Handle to the next FunctionEntry_t in chain
		CallbackProc m_pProc;		///< Function to call for this entry
		void *m_pData;				///< User supplied data pointer to call the function with
		Word m_uPriority;			///< User supplied priority for inserting a new entry into the list
	};
	RunQueueEntry_t *m_pFirst;	///< Pointer to the first entry
	Word m_Recurse;				///< \ref TRUE if this class is the process of executing.
};
}
/* END */

#endif

