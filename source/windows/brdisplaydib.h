/***************************************

    DIB display context class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYDIB_H__
#define __BRDISPLAYDIB_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDIB : public Display {
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
	HDC__ *m_pWindowDC;				///< Pointer to the window's device context
	HDC__ *m_pBitMapDC;				///< Pointer to the BitMap's device context
public:
	DisplayDIB(GameApp *pGameApp);
};
#endif
}
/* END */

#endif
