/***************************************

	Run Queue execution handler

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRUNQUEUE_H__
#define __BRRUNQUEUE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDOUBLELINKEDLIST_H__
#include "brdoublelinkedlist.h"
#endif

/* BEGIN */
namespace Burger {
class RunQueue {
	BURGER_DISABLE_COPY(RunQueue);

public:
	enum eReturnCode {
		/** Executed normally */
		OKAY,
		/** Error occurred that requires an immediate abort */
		ABORT,
		/** Dispose of this callback */
		DISPOSE
	};

	enum {
		/** Highest priority for RunQueue tasks, executed first (Reserved for
		   Burgerlib, do not use or exceed this value) */
		PRIORITY_FIRST = 0x7FFFFFF,
		/** Priority for reading joypad (Can generate keystrokes and mouse
		   events) */
		PRIORITY_JOYPAD = 0x7000040,
		/** Priority for reading mouse (Can generate keystrokes) */
		PRIORITY_MOUSE = 0x70000030,
		/** Priority for reading keyboard */
		PRIORITY_KEYBOARD = 0x70000010,
		/** Priority for processing game input */
		PRIORITY_INPUTPROCESSING = 0x7000000,
		/** Priority for music processing */
		PRIORITY_SEQUENCING = 0x68001000,
		/** Priority for handling sound effects */
		PRIORITY_SOUNDPROCESSING = 0x6800000,
		/** Priority for asynchronous file I/O processing */
		PRIORITY_FILEPROCESSING = 0x6400000,
		/** High priority for RunQueue tasks */
		PRIORITY_HIGH = 0x6000000,
		/** Average priority for RunQueue tasks */
		PRIORITY_MEDIUM = 0x4000000,
		/** Low priority for RunQueue tasks */
		PRIORITY_LOW = 0x2000000,
		/** Lowest priority for RunQueue tasks, executed last, do not go lower
		   than this value */
		PRIORITY_LAST = 0

	};

	typedef eReturnCode(BURGER_API* CallbackProc)(void* pContext);

	class RunQueueEntry: protected DoubleLinkedList {
		BURGER_DISABLE_COPY(RunQueueEntry);

		friend class RunQueue;

		/** Function to call for this entry */
		CallbackProc m_pCallBack;
		/** Function to call on deletion */
		CallbackProc m_pShutdownCallback;
		/** User supplied data pointer to call the function with */
		void* m_pData;
		/** User supplied priority for inserting a new entry into the list */
		uint_t m_uPriority;

		RunQueueEntry(CallbackProc pCallBack, CallbackProc pShutdownCallback,
			void* pData, uint_t uPriority) BURGER_NOEXCEPT
			: m_pCallBack(pCallBack),
			  m_pShutdownCallback(pShutdownCallback),
			  m_pData(pData),
			  m_uPriority(uPriority)
		{
		}

	public:
		~RunQueueEntry();
		BURGER_INLINE uint_t GetPriority(void) const BURGER_NOEXCEPT
		{
			return m_uPriority;
		}
	};

protected:
	/** Head entry of the linked list */
	DoubleLinkedList m_Entries;
	/** \ref TRUE if this class is the process of executing. */
	uint_t m_bRecurse;

public:
	RunQueue() BURGER_NOEXCEPT: m_Entries(), m_bRecurse(FALSE) {}
	~RunQueue();
	void BURGER_API Call(void) BURGER_NOEXCEPT;
	RunQueueEntry* BURGER_API Add(CallbackProc pProc,
		CallbackProc pShutdown = nullptr, void* pData = nullptr,
		uint_t uPriority = PRIORITY_MEDIUM) BURGER_NOEXCEPT;
	RunQueueEntry* BURGER_API Find(CallbackProc pProc) const BURGER_NOEXCEPT;
	RunQueueEntry* BURGER_API Find(
		CallbackProc pProc, void* pData) const BURGER_NOEXCEPT;
	uint_t BURGER_API RemoveAll(CallbackProc pProc) BURGER_NOEXCEPT;
	uint_t BURGER_API Remove(
		CallbackProc pProc, void* pData = nullptr) BURGER_NOEXCEPT;
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
};

}
/* END */

#endif
