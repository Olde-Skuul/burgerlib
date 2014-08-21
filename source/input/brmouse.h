/***************************************

	Mouse Manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRMOUSE_H__
#define __BRMOUSE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
class Mouse {
	static RunQueue::eReturnCode BURGER_API Poll(void *pThis);
	Mouse(Mouse const &);						///< Prevent copying
	Mouse & operator = (Mouse const &);			///< Prevent copying
	GameApp *m_pAppInstance;			///< Application instances
	Word m_uX;
	Word m_uY;
	Word m_uBoundsX;
	Word m_uBoundsY;
	int m_iDeltaX;
	int m_iDeltaY;
	int m_iMouseWheel;
	Word m_uButtons;
	Word m_uPressedButtons;
#if defined(BURGER_WINDOWS)
	IDirectInputDevice8W *m_pMouseDevice;
#endif
public:
	Mouse(GameApp *pAppInstance);
	~Mouse();
	Word IsPresent();
	Word ReadButtons(void);
	Word ReadButtonDowns(void);
	void Read(Word *pX,Word *pY);
	void Read(int *pX,int *pY);
	int ReadWheel(void);
	void SetRange(Word x,Word y);
	void SetPosition(Word x,Word y);
#if defined(BURGER_WINDOWS)
	void Acquire(void);
	void Unacquire(void);
#endif
};
}
/* END */

#endif
