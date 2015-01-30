/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

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
protected:
	IDirect3D9 *m_pDirect3D9;				///< Pointer to the Direct3D9 context
	IDirect3DDevice9 *m_pDirect3DDevice9;	///< Pointer to the Direct3D9 device
	IDirect3DSurface9 *m_pDefaultRenderTarget;
	IDirect3DSurface9 *m_pCurrentRenderTarget;
	Word m_bLostDevice;					///< TRUE if the device was lost (Minimized?)
	Word m_bPower2Textures;				///< TRUE if only power of two textures are allowed
	Word m_bVSynced;					///< TRUE if presentation is synced to VBlank
	Word m_bMultiRenderTargets;			///< TRUE if multiple render targets are supported
	Word m_bSeparateAlphaBlend;			///< TRUE if separated alpha blending is supported
	Word m_bSceneBegun;					///< TRUE if BeginScene() was called
	Word m_uBackBufferFormat;			///< Requested back buffer format
	Word m_uMaxTextureWidth;
	Word m_uMaxTextureHeight;
	Word m_uClearColor;
	float m_fClearDepth;
	void BURGER_API CreatePresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput);
	void BURGER_API InitState(void);
	Word BURGER_API Reset(void);
public:
	DisplayDirectX9(GameApp *pGameApp);
	~DisplayDirectX9();
	virtual Word Init(Word uWidth,Word uHeight,Word uDepth=32,Word uFlags=DEFAULTFLAGS);
	virtual void Shutdown(void);
	virtual void BeginScene(void);
	virtual void EndScene(void);
	virtual Texture *CreateTextureObject(void);
	virtual VertexBuffer *CreateVertexBufferObject(void);
	virtual void SetViewport(Word uX,Word uY,Word uWidth,Word uHeight);
	virtual void SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha);
	virtual void SetClearDepth(float fDepth);
	virtual void Clear(Word uMask);
	virtual void Bind(Texture *pTexture,Word uIndex=0);
	virtual void SetBlend(Word bEnable);
	virtual void SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor);
	virtual void SetLighting(Word bEnable);
	virtual void SetZWrite(Word bEnable);
	virtual void SetDepthTest(eDepthFunction uDepthFunction);
	virtual void SetCullMode(eCullMode uCullMode);
	virtual void DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	virtual void DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	IDirect3DVertexShader9 * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	IDirect3DPixelShader9 * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;

	BURGER_INLINE IDirect3D9 *GetDirect3D9(void) const { return m_pDirect3D9; }
	BURGER_INLINE IDirect3DDevice9 *GetDirect3DDevice9(void) const { return m_pDirect3DDevice9; }
};
#endif
}
/* END */

#endif
