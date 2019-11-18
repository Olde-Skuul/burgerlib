/***************************************

	Display object manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYOBJECT_H__
#define __BRDISPLAYOBJECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

/* BEGIN */
namespace Burger {
class Display;

class DisplayObject : public ReferenceCounter {
    BURGER_DISABLE_COPY(DisplayObject);
	BURGER_RTTI_IN_CLASS();
protected:
	DisplayObject *m_pNextDisplayObject;		///< Pointer to the next DisplayObject
	DisplayObject *m_pPrevDisplayObject;		///< Pointer to the previous DisplayObject
	static DisplayObject *g_pHead;				///< Global pointer to the linked list of display objects
	DisplayObject(void);
	virtual ~DisplayObject();
public:
    static BURGER_INLINE DisplayObject *GetFirstObject(void) { return g_pHead; }
	BURGER_INLINE DisplayObject *GetNext(void) const { return m_pNextDisplayObject; }
	BURGER_INLINE DisplayObject *GetPrevious(void) const { return m_pPrevDisplayObject; }
	virtual Word CheckLoad(Display *pDisplay) = 0;
	virtual void Release(Display *pDisplay) = 0;
	static void BURGER_API ReleaseAll(Display *pDisplay);
};
}
/* END */
#endif
