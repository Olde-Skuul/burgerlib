/***************************************

	DirectX 9 manager class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!n

***************************************/

#ifndef __BRDISPLAYDIRECTX9_H__
#define __BRDISPLAYDIRECTX9_H__

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDirectX9 : public Display {
public:
	enum {
		DIRECTXRESETATTEMPTS=30			///< Number of times the app will retry restarting the display before giving up
	};
	DisplayDirectX9(GameApp *pGameApp);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
	IDirect3D9 *m_pDirect3D9;				///< Pointer to the Direct3D9 context
	IDirect3DDevice9 *m_pDirect3DDevice9;	///< Pointer to the Direct3D9 device
	Word m_bLostDevice;					///< TRUE if the device was lost (Minimized?)
	Word m_bPower2Textures;				///< TRUE if only power of two textures are allowed
	Word m_uBackBufferFormat;			///< Requested back buffer format
	void CreatePresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput);
};
#endif
}
/* END */

#endif
