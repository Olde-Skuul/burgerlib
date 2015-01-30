/***************************************

	Run Queue execution handler

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrunqueue.h"
#include "brglobalmemorymanager.h"

/*! ************************************

	\class Burger::RunQueue
	\brief Simple manager of a list of function pointers.

	RunQueue will hold a list of simple function pointers
	and call them when the Call() member is invoked. It's useful
	for creating a list of functions to be called in the background
	on a demand basis, such as polling tasks and game logic objects.
	
	Each function is of a type of RunQueue::CallbackProc
	
	\note Due to the nature of memory use, the copying of this
	class is forbidden.
	
***************************************/

/*! ************************************

	\typedef Burger::RunQueue::CallbackProc
	\brief Function pointer type for callbacks.

	When the Call() function is invoked, all functions will be called
	using the type of RunQueue::eReturnCode (\ref BURGER_API *)(void *)
	with a single parameter that will have either
	a user supplied void * or a \ref NULL if none was supplied.
	
***************************************/


/*! ************************************

	\fn Burger::RunQueue::RunQueue()
	\brief RunQueue constructor.

	Initialize the class to contain no list. This is
	inlined due to its only zeroing out members.
	
***************************************/


/*! ************************************

	\brief RunQueue destructor.

	This will call Clear() and dispose of all the memory
	allocated. No other functions will be called.
	
	\sa Clear().
	
***************************************/

Burger::RunQueue::~RunQueue()
{
	Clear();
}

/*! ************************************

	\brief Invoke every function stored within the list.

	Traverse the array of functions to call and invoke each
	and every one of them. The functions may add new entries
	to the list or remove themselves if they need to. If Add()
	is called, execution of the new entry will be deferred until the next time
	Call() is invoked.
	
	The function called is of type RunQueue::CallbackProc.
	
	\note This function can take a significant amount
	of CPU time if the functions invoked are very slow.
	This function runs with minimal overhead.
	
	\note There is a recursion checker, this function will
	do nothing if it is called by a RunQueue::CallbackProc proc.

	\sa Add(CallbackProc,void*,Word), Remove(CallbackProc,void*) or RemoveAll(CallbackProc);

***************************************/

void Burger::RunQueue::Call(void)
{
	if (!m_Recurse) {
		RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
		if (pWork) { 							// Any in the list?
			m_Recurse = TRUE;					// Prevent recursion
			RunQueueEntry_t **pPrev = &m_pFirst;		// Previous entry
			do {
				// Call the funtion
				eReturnCode eCode = pWork->m_pProc(pWork->m_pData);

				// Next in the chain
				RunQueueEntry_t *pNext = pWork->m_pNext;

				// Abort execution
				if (eCode==ABORT) {
					break;

				// Dispose of this entry
				} else if (eCode==DISPOSE) {
					// Remove from the linked list
					pPrev[0] = pNext;
					// Remove this entry
					Free(pWork);
				} else {
					// Since this is retained, this will be the previous entry for 
					// unlinking (If needed)
					pPrev = &pWork->m_pNext;
				}
				pWork = pNext;				// Next one
			} while (pWork); 				// Any more?
			m_Recurse = FALSE;				// I'm done, so release the lock
		}
	}
}

/*! ************************************

	\brief Add a function to the list.
	
	Given a function pointer and a pointer to data to pass to the
	function pointer, add this entry to the list of functions that are to be called
	with each call to Call(). The pointer pData is not used by
	this class and it's solely used as a parameter when the function
	pointer is called. Priority values are used to sort the function pointers to call
	them in a desired order. The default is RunQueue::MEDIUMPRIORITY. There is no sorting with
	like numbered priorities.
	
	\param pProc Pointer to a function of type RunQueue::CallbackProc.
	\param pData Pointer to be passed to the function when called.
	\param uPriority Priority value to determine order of calling. Higher values get called first.

	\return Returns \ref TRUE if successful, \ref FALSE if out of memory or if the
	function pointer was \ref NULL.
	\sa Remove(CallbackProc,void*).
	
***************************************/

Word Burger::RunQueue::Add(CallbackProc pProc,void *pData,Word uPriority)
{
	// Assume failure
	Word uResult = FALSE;
	if (pProc) {
		RunQueueEntry_t *pNew = static_cast<RunQueueEntry_t *>(Burger::Alloc(sizeof(RunQueueEntry_t)));
		if (pNew) {
			pNew->m_pProc = pProc;		// Save the proc
			pNew->m_pData = pData;		// Save the data
			pNew->m_uPriority = uPriority;	// Priority
			RunQueueEntry_t *pNext = m_pFirst;
			// No entries? No need to test priorities
			if (!pNext) {
				pNew->m_pNext = NULL;	// Link it in
				m_pFirst = pNew;		// New master handle
			} else {
				// Traverse the linked list until the end is 
				// reached, or an entry with lower priority shows up
				RunQueueEntry_t *pWork;
				do {
					pWork = pNext;
					pNext = pNext->m_pNext;
				} while (pNext && (pWork->m_uPriority>uPriority));
				pNew->m_pNext = pNext;
				pWork->m_pNext = pNew;
				uResult = TRUE;				// Success
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.
	
	\param pProc Pointer to the function.
	\return \ref TRUE if the function pointer was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word Burger::RunQueue::Find(CallbackProc pProc) const
{
	Word uResult = FALSE;
	const RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		do {
			if (pWork->m_pProc==pProc) {	// Match?
				uResult = TRUE;				// I found it
				break;
			}
			pWork = pWork->m_pNext;			// Follow the list
		} while (pWork);					// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Return \ref TRUE if a function is in the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, return \ref TRUE.
		
	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Find(CallbackProc) const or Call().

***************************************/

Word Burger::RunQueue::Find(CallbackProc pProc,void *pData) const
{
	Word uResult = FALSE;
	const RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		do {
			if (pWork->m_pProc==pProc && pWork->m_pData==pData) {	// Match?
				uResult = TRUE;			// I found it
				break;
			}
			pWork = pWork->m_pNext;				// Follow the list
		} while (pWork);				// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Remove all entries to a function from the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, remove the entry and return \ref TRUE, saying I found it.
	
	\note This function should not be invoked while Call() is executing.
	It may cause a list traversal error.
	
	\param pProc Pointer to the function.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word Burger::RunQueue::RemoveAll(CallbackProc pProc)
{
	Word uResult = FALSE;
	RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		RunQueueEntry_t **pPrev = &m_pFirst;	// This is where I'll store the forward link
		do {
			RunQueueEntry_t *pNext = pWork->m_pNext;		// Get the forward link (For unlinking)
			if (pWork->m_pProc==pProc) {	// Match?
				pPrev[0] = pNext;			// I'm gone!
				Free(pWork);				// Dispose of the current record
				uResult = TRUE;				// I deleted it
			} else {
				pPrev = &pWork->m_pNext;	// This is the link that will be broken
			}
			pWork = pNext;					// Follow the list
		} while (pWork);					// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Remove a function from the list.

	Given a function pointer and a pointer to data to pass to the
	function pointer, search the list to see if there is a match.
	If a match is found, remove the entry and return \ref TRUE, saying
	it's been found.
	
	\note Functions can remove themselves from the list at exection time, but
	you cannot remove a different function during your time quantum since
	it might break the linked list travesal. If you wish to remove other entries,
	you must defer this action.
	
	\param pProc Pointer to the function.
	\param pData Void pointer to pass to the function if called.
	\return \ref TRUE if the Function/Data pair was found, \ref FALSE if not.
	\sa Add(CallbackProc,void*,Word), Clear() or Call().

***************************************/

Word Burger::RunQueue::Remove(CallbackProc pProc,void *pData)
{
	Word uResult = FALSE;
	RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		RunQueueEntry_t **pPrev = &m_pFirst;	// This is where I'll store the forward link
		do {
			RunQueueEntry_t *pNext = pWork->m_pNext;		// Get the forward link (For unlinking)
			if (pWork->m_pProc==pProc && pWork->m_pData==pData) {	// Match?
				pPrev[0] = pNext;	// I'm gone!
				Free(pWork);		// Dispose of the current record
				uResult = TRUE;			// I deleted it
				break;
			}
			pPrev = &pWork->m_pNext;	// This is the link that will be broken
			pWork = pNext;				// Follow the list
		} while (pWork);				// Still more?
	}
	return uResult;
}

/*! ************************************

	\brief Release all function entries.
	
	Release all of the allocated memory and discard
	all function and data pointers.
	
	\note No functions will be called. Only the list will be destroyed.	

	\sa Add(CallbackProc,void*,Word) or Remove(CallbackProc,void*).
	
***************************************/

void Burger::RunQueue::Clear(void)
{
	RunQueueEntry_t *pWork = m_pFirst;		// Get the master handle
	if (pWork) {							// Is it valid?
		do {
			RunQueueEntry_t *pNext = pWork->m_pNext;		// Get the forward link (For unlinking)
			Free(pWork);		// Dispose of the current record
			pWork = pNext;				// Follow the list
		} while (pWork);				// Still more?
		m_pFirst = pWork;				// Erase with NULL without loading a constant
	}
}

#if 0
/***************************************

	Allocate and initialize a Master RunQueue_t
	structure, return the structure or a NULL if error

***************************************/

RunQueue_t ** BURGER_API MasterRunQueueNew(void)
{
	RunQueue_t **MasterQueueHandle;

	MasterQueueHandle = (RunQueue_t **)AllocAHandle(sizeof(RunQueue_t));
	if (MasterQueueHandle) {
		MasterRunQueueInit(MasterQueueHandle);		/* Init the queue */
	}
	return MasterQueueHandle;		/* Return the handle */
}

/***************************************

	Using a preexisting RunQueue_t struct, initialize the data

***************************************/

void BURGER_API MasterRunQueueInit(RunQueue_t **MasterQueueHandle)
{
	RunQueue_t *RunQueuePtr;
	if (MasterQueueHandle) {		/* Valid handle? */
		RunQueuePtr = *MasterQueueHandle;
		RunQueuePtr->NextQueue = MasterQueueHandle;	/* Init the handles */
		RunQueuePtr->PrevQueue = MasterQueueHandle;
		RunQueuePtr->Proc = 0;			/* No code proc */
		RunQueuePtr->TimeQuantum = 0;	/* How many times to call myself */
		RunQueuePtr->ElapsedTime = 0;	/* Time elapsed */
		RunQueuePtr->Priority = 0;		/* Low priority */
		RunQueuePtr->IDNum = (Word)-1;	/* Master RunQueue_t */
	}
}


/***************************************

	Dispose of the entire contents of a Master
	RunQueue_t handle and then dispose of the handle

***************************************/

void BURGER_API MasterRunQueueDelete(RunQueue_t **MasterQueueHandle)
{
	if (MasterQueueHandle) {
		MasterRunQueueDestroy(MasterQueueHandle);		/* Discard the contents */
		DeallocAHandle((void **)MasterQueueHandle);	/* Dispose of the memory */
	}
}


/***************************************

	Dispose of the entire contents of a Master
	RunQueue_t handle.

***************************************/

void BURGER_API MasterRunQueueDestroy(RunQueue_t **MasterQueueHandle)
{
	RunQueue_t **NextHandle;
	if (MasterQueueHandle) {		/* Valid? */
		goto Middle;			/* Begin the code */

		do {
			RunQueueDelete(NextHandle);		/* Dispose of the record */
Middle:
			NextHandle = (*MasterQueueHandle)->NextQueue;	/* Next one */
		} while (NextHandle!=MasterQueueHandle);		/* All out? */
	}
}


/***************************************

	Traversing the linked list, execute the proc for each
	RunQueue_t struct. If the TimeQuantum input is zero, then only
	execute code that allows time to pass.

	Note : Code in a Proc can NEVER call DeallocARunQueue(), if
		must call DeallocARunQueueDefer() to yield the same results.
	Note : If the time quantum is -1 then I can only execute the code
		once, and the code COULD be a deferred delete, so make sure
		I cache the NextQueue handle before I call the proc to
		prevent the memory from disappearing under me.
	Note : A TimeQuantum in the RunQueue_t means that I execute forever.
		This allows subroutines called as DSP code.

***************************************/

void BURGER_API MasterRunQueueExecute(RunQueue_t **MasterQueueHandle,Word TimeQuantum)
{
	RunQueue_t **NextHandle;
	RunQueue_t *WorkPtr;
	RunQueue_t **WorkHandle;

	if (MasterQueueHandle) {		/* Is the master handle ok? */
		NextHandle = (*MasterQueueHandle)->NextQueue;	/* Get the next one */
		if (NextHandle!=MasterQueueHandle) {
			do {
				WorkHandle = NextHandle;	/* I use this handle */
				WorkPtr = *WorkHandle;		/* Deref the handle */
				NextHandle = WorkPtr->NextQueue;		/* Cache the NEXT handle */
				if (WorkPtr->TimeQuantum==(Word)-1) {	/* Special case? */
					WorkPtr->ElapsedTime = TimeQuantum;		/* Return the elapsed time */
					WorkPtr->Proc((void **)WorkHandle,RQTIME);	/* Call code */
				} else if (TimeQuantum) {		/* Any time elapsed? */
					WorkPtr->ElapsedTime += TimeQuantum;	/* Adjust timer */
					while (WorkPtr->ElapsedTime>=WorkPtr->TimeQuantum) {	/* Time up? */
						WorkPtr->ElapsedTime-=WorkPtr->TimeQuantum;	/* Remove the time */
						WorkPtr->Proc((void **)WorkHandle,RQTIME);	/* Call the proc */
						WorkPtr = *WorkHandle;		/* Deref again */
						if (WorkPtr->TimeQuantum==(Word)-1) {		/* No more? */
							break;		/* End now! */
						}
					}
				}
			} while (NextHandle!=MasterQueueHandle);	/* No more? */
		}
	}
}


/***************************************

	Used for debugging only, it will dump a chain of
	RunQueue_t structs

***************************************/

static Word BURGER_API DumpQueuesProc(RunQueue_t **Input)
{
	char OneLine[60];
	RunQueue_t *WorkPtr;

	WorkPtr = *Input;
	OneLine[0] = '$';
	OneLine[10] = '$';
	OneLine[20] = '$';
	OneLine[53] = '\n';
	OneLine[9] = ' ';
	OneLine[19] = ' ';
	OneLine[29] = ' ';
	OneLine[35] = ' ';
	OneLine[41] = ' ';
	OneLine[47] = ' ';
	OneLine[54] = 0;
	Burger::NumberToAsciiHex(&OneLine[1],reinterpret_cast<WordPtr>(Input),Burger::NOENDINGNULL|Burger::LEADINGZEROS|8);
	Burger::NumberToAsciiHex(&OneLine[11],reinterpret_cast<WordPtr>(WorkPtr->PrevQueue),Burger::NOENDINGNULL|Burger::LEADINGZEROS|8);
	Burger::NumberToAsciiHex(&OneLine[21],reinterpret_cast<WordPtr>(WorkPtr->NextQueue),Burger::NOENDINGNULL|Burger::LEADINGZEROS|8);
	Burger::NumberToAscii(&OneLine[30],(Word32)WorkPtr->IDNum,Burger::NOENDINGNULL|Burger::LEADINGZEROS|5);
	Burger::NumberToAscii(&OneLine[36],(Word32)WorkPtr->TimeQuantum,Burger::NOENDINGNULL|Burger::LEADINGZEROS|5);
	Burger::NumberToAscii(&OneLine[42],(Word32)WorkPtr->ElapsedTime,Burger::NOENDINGNULL|Burger::LEADINGZEROS|5);
	Burger::NumberToAscii(&OneLine[48],(Word32)WorkPtr->Priority,Burger::NOENDINGNULL|Burger::LEADINGZEROS|5);
	Burger::Debug::String(OneLine);
	return FALSE;		/* Do not abort */
}

void BURGER_API MasterRunQueueDump(RunQueue_t **MasterQueueHandle)
{
	Burger::Debug::String("List of active RunQueue_t's\n"
	"  Handle     Prev      Next    ID   Time  Elaps Prior\n");
/*  "$00000000 $00000000 $00000000 00000 00000 00000 00000 */
	RunQueuePoll(MasterQueueHandle,(RunQueuePollCallback_t) DumpQueuesProc,0);
}


/***************************************

	Allocate a RunQueue_t handle and insert it into
	a linked list.

***************************************/

Word BURGER_API RunQueueCallProc(RunQueue_t **RunQueueHandle,Word Event)
{
	if (RunQueueHandle) {
		return (*RunQueueHandle)->Proc((void **)RunQueueHandle,Event);
	}
	return FALSE;		/* I didn't do anything special */
}


/***************************************

	Default RunQueue_t code procedure

***************************************/

Word BURGER_API RunQueueDefaultProc(void ** /* RunQueueHandle */,Word /* Event */)
{
	return FALSE;		/* Pass through everything */
}



/***************************************

	Allocate a new RunQueue_t struct

***************************************/

RunQueue_t ** BURGER_API RunQueueNew(RunQueue_t **MasterQueueHandle,Word MemSize,
	Word TimeQuantum,Word Priority,Word IDNum,RunQueueProc_t Proc)
{
	RunQueue_t **WorkHandle;
	RunQueue_t *WorkPtr;

	if (!Proc) {		/* Use default? */
		Proc = RunQueueDefaultProc;		/* Assign a default proc */
	}
	if (MasterQueueHandle && MemSize>=sizeof(RunQueue_t)) {
		WorkHandle = (RunQueue_t **)AllocAHandleClear(MemSize);	/* Get memory */
		if (WorkHandle) {			/* Got it? */
			WorkPtr = *WorkHandle;		/* Deref the handle */
			WorkPtr->TimeQuantum = TimeQuantum;	/* Set the defaults */
			WorkPtr->IDNum = IDNum;
			WorkPtr->Proc = Proc;
			RunQueueLink(MasterQueueHandle,WorkHandle,Priority);
			Proc((void **)WorkHandle,RQINIT);	/* Call the init code (If any) */
			return WorkHandle;		/* Return the new record */
		}
	}
	return 0;			/* No good! */
}


/***************************************

	Dispose of a RunQueue_t struct immediately,
	Warning : Never use this routine to dispose of a MasterRunQueue.
		That would be bad. Use MasterRunQueueDelete() instead.

***************************************/

void BURGER_API RunQueueDelete(RunQueue_t **RunQueueHandle)
{
	if (RunQueueHandle) {		/* Valid handle? */
		RunQueueUnlink(RunQueueHandle);		/* Unlink it */
		(*RunQueueHandle)->Proc((void **)RunQueueHandle,RQDESTROY);	/* Call proc */
		DeallocAHandle((void **)RunQueueHandle);	/* Dispose of the memory */
	}
}

/***************************************

	This is a RunQueue_t proc that will dispose of my own handle.
	It is called ONLY by MasterQueueExecute() since the handle input
	will be disposed of.

***************************************/

Word BURGER_API RunQueueDeleteProc(void **RunQueueHandle,Word /* Event */)
{
	RunQueueDelete((RunQueue_t **)RunQueueHandle);	/* Dispose of myself */
	return TRUE;			/* I handled it! */
}


/***************************************

	Dispose of a RunQueue_t at a later time, this allows
	RunQueue_t routines to dispose of themselves safely so that
	programs that traverse linked lists won't be surprised.

***************************************/

void BURGER_API RunQueueDeleteDefer(RunQueue_t **RunQueueHandle)
{
	RunQueue_t *WorkPtr;
	if (RunQueueHandle) {
		WorkPtr = *RunQueueHandle;
		WorkPtr->TimeQuantum = (Word)-1;	/* Force a call on main loop */
		WorkPtr->IDNum = 0;			/* Kill the ID number */
		WorkPtr->Proc = RunQueueDeleteProc;		/* Delete me next time! */
	}
}



/***************************************

	Insert a RunQueue_t struct into a linked list using
	a priority value.
	If it's 255, link to the beginning of the list.
	If it's 0, link to the end of the list.
	All others, must traverse the list to find its place.

***************************************/

void BURGER_API RunQueueLink(RunQueue_t **MasterQueueHandle,RunQueue_t **RunQueueHandle,Word Priority)
{
	RunQueue_t **NextHandle;
	if (MasterQueueHandle && RunQueueHandle) {		/* Valid?? */
		if (Priority) {		/* Link to the end of the list? */
			/* Should I link AFTER MasterQueueHandle? */
			if (Priority>=255) {	/* Link to the beginning of the list? */
				Priority = 255;		/* Failsafe */
				MasterQueueHandle = (*MasterQueueHandle)->NextQueue;
			} else {

	/* Follow the list and link BEFORE the first handle that is LOWER */
	/* than the priority requested */

				NextHandle = (*MasterQueueHandle)->NextQueue;	/* Get the next one */
				if (NextHandle!=MasterQueueHandle) {
					do {
						if ((*NextHandle)->Priority<=Priority) {		/* Is the priority lower? */
							MasterQueueHandle = NextHandle;		/* Link BEFORE this? */
							break;
						}
						NextHandle = (*NextHandle)->NextQueue;	/* Next */
					} while (NextHandle!=MasterQueueHandle);	/* No more? */
				}
			}
		}

	/* Use the Handle "MasterQueueHandle" and link BEFORE it */

		(*RunQueueHandle)->NextQueue = MasterQueueHandle;
		(*RunQueueHandle)->PrevQueue = (*MasterQueueHandle)->PrevQueue;
		(*(*MasterQueueHandle)->PrevQueue)->NextQueue = RunQueueHandle;
		(*MasterQueueHandle)->PrevQueue = RunQueueHandle;
		(*RunQueueHandle)->Priority = Priority;
	}
}


/***************************************

	Remove a RunQueue struct from a linked list.
	When this routine is done, the RunQueue is no longer linked
	to its master list.

***************************************/

void BURGER_API RunQueueUnlink(RunQueue_t **RunQueueHandle)
{
	RunQueue_t *WorkPtr;
	if (RunQueueHandle) {
		WorkPtr = *RunQueueHandle;		/* Deref my handle */
		(*WorkPtr->PrevQueue)->NextQueue = WorkPtr->NextQueue;	/* Unlink */
		(*WorkPtr->NextQueue)->PrevQueue = WorkPtr->PrevQueue;
	}
}

/***************************************

	Scan a list of RunQueues and return either
	a NULL if I hit the end of the list or a handle to
	a RunQueue that matches the IDNum
	If IDNum input is zero, then just traverse the list to the end.

***************************************/

RunQueue_t ** BURGER_API RunQueueFindIDNum(RunQueue_t **RunQueueHandle,Word IDNum)
{
	Word TheID;
	if (RunQueueHandle) {		/* Valid handle? */
		goto Middle;
		do {
			if (!IDNum || 		/* Looking for all? */
				TheID==IDNum) {	/* Specific match? */
				return RunQueueHandle;		/* Return the current handle */
			}
Middle:
			RunQueueHandle = (*RunQueueHandle)->NextQueue;	/* Next one in chain */
			TheID = (*RunQueueHandle)->IDNum;	/* Get the ID number */
		} while (TheID!=(Word)-1);		/* Last member? */
	}
	return 0;		/* No more scanning */
}

/***************************************

	Traverse the linked list of RunQueue_t records and
	call a callback routine for each one that matches a specific
	ID. If the ID requested is 0, then scan all of them.

***************************************/

Word BURGER_API RunQueuePoll(RunQueue_t **MasterQueueHandle,RunQueuePollCallback_t CallBack,Word IDNum)
{
	Word Result;
	Result = FALSE;
	do {
		MasterQueueHandle = RunQueueFindIDNum(MasterQueueHandle,IDNum);
		if (!MasterQueueHandle) {		/* Found one? */
			break;
		}
		Result = CallBack((void **)MasterQueueHandle);	/* Call the proc */
	} while (!Result);		/* Stay here? */
	return Result;		/* Did I abort? */
}
#endif
