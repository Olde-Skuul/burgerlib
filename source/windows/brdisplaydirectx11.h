/***************************************

	DirectX 11 manager class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYDIRECTX11_H__
#define __BRDISPLAYDIRECTX11_H__

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDirectX11 : public Display {
	BURGER_DISABLECOPYCONSTRUCTORS(DisplayDirectX11);
	BURGER_RTTI_IN_CLASS();
public:
	static void BURGER_API ClearContext(ID3D11DeviceContext *pDX11Context);
};
#endif
}
/* END */

#endif
