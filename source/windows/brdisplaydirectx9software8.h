/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYDIRECTX9SOFTWARE8_H__
#define __BRDISPLAYDIRECTX9SOFTWARE8_H__

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRDISPLAYDIRECTX9_H__
#include "brdisplaydirectx9.h"
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
class DisplayDirectX9Software8 : public DisplayDirectX9 {
protected:
	IDirect3DPixelShader9 *m_pPixelShader8Bit;	///< Pointer to the 8 bit pixel shader
	IDirect3DTexture9 *m_pBitMapTextureSysMem;	///< Texture for the 8 bit offscreen buffer
	IDirect3DTexture9 *m_pBitMapTexture;		///< Texture for the 8 bit video memory buffer
	IDirect3DTexture9 *m_pPaletteTexture;		///< Texture for the palette
	IDirect3DVertexBuffer9 *m_pVertexBuffer;	///< Vertex buffer to blit the software buffer to hardware
	RendererSoftware8 m_Renderer;				///< Software renderer context
	Word m_bFrontBufferTrueColor;				///< \ref TRUE if the front buffer is true color (Not 8 bit paletted)
	Word m_uResetAttempts;						///< Number of tries to restore the video display
	void FillVertexBuffer(void);
	long AllocateResources(void);
	void ReleaseResources(void);
	long ResetLostDevice(void);
public:
	DisplayDirectX9Software8(GameApp *pGameApp);
	virtual Word Init(Word uWidth,Word uHeight,Word uDepth=32,Word uFlags=DEFAULTFLAGS);
	virtual void Shutdown(void);
	virtual void BeginScene(void);
	virtual void EndScene(void);
	BURGER_INLINE Renderer *GetRenderer(void) { return &m_Renderer; }
};
#endif
}
/* END */

#endif
