/***************************************

	DIB display context class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYDIBSOFTWARE8_H__
#define __BRDISPLAYDIBSOFTWARE8_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRDISPLAYDIB_H__
#include "brdisplaydib.h"
#endif

#ifndef __BRRENDERERSOFTWARE8_H__
#include "brrenderersoftware8.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDIBSoftware8 : public DisplayDIB {
public:
	DisplayDIBSoftware8(GameApp *pGameApp);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
private:
	void *m_pBitMap;				///< Pointer the the HBITMAP's bitmap
	HPALETTE__ *m_pHPalette;		///< Pointer to the HPALETTE for this window
	HBITMAP__ *m_pHBitMap;			///< Pointer to the frame HBITMAP
	RendererSoftware8 m_Renderer;	///< Software renderer context
};
#endif
}
/* END */

#endif
