/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRGLOBALS_H__
#include "brglobals.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDirectX9 : public Display {
	BURGER_DISABLECOPYCONSTRUCTORS(DisplayDirectX9);
	BURGER_RTTI_IN_CLASS();
public:
	enum {
		DIRECTXRESETATTEMPTS=30			///< Number of times the app will retry restarting the display before giving up
	};
protected:
	IDirect3D9 *m_pDirect3D9;				///< Pointer to the Direct3D9 context
	IDirect3DDevice9 *m_pDirect3DDevice9;	///< Pointer to the Direct3D9 device
	ID3DXMatrixStack *m_pD3DXMatrixStack;	///< Pointer to the current D3DMatrix stack
	IDirect3DSurface9 *m_pDefaultRenderTarget;
	IDirect3DSurface9 *m_pCurrentRenderTarget;
	Word m_bLostDevice;					///< TRUE if the device was lost (Minimized?)
	Word m_bPower2Textures;				///< TRUE if only power of two textures are allowed
	Word m_bVSynced;					///< TRUE if presentation is synced to VBlank
	Word m_bMultiRenderTargets;			///< TRUE if multiple render targets are supported
	Word m_bSeparateAlphaBlend;			///< TRUE if separated alpha blending is supported
	Word m_bSceneBegun;					///< TRUE if BeginScene() was called
	Word m_bFullScreenGamma;			///< TRUE if full screen gamma was supported
	Word m_bCanCalibrateGamma;			///< TRUE if gamma can be calibrated
	Word m_bRasterSlopeScaleDepthBias;	///< TRUE if sloped scaling depth bias is supported in rasterizer
	Word m_bRasterDepthBias;			///< TRUE if depth bias is supported in rasterizer
	Word m_bIsnVidia;					///< TRUE if the video card was made by nVidia
	Word m_bIsATI;						///< TRUE if the video card was made by ATI
	Word m_bIsIntel;					///< TRUE if the video card was made by Intel
	Word m_uBackBufferFormat;			///< Requested back buffer format
	Word m_uMaxTextureWidth;			///< Maximum pixels wide a texture can be
	Word m_uMaxTextureHeight;			///< Maximum pixels tall a texture can be
	Word m_uMaxTextureStages;			///< Maximum number of texture blend stages
	Word m_uMaxTextureSamplers;			///< Maximum number of texture samplers
	Word m_uMaxPossibleAnisotropy;		///< Maximum number of anisotropy filters
	Word m_uClearColor;					///< RGBA of the color to clear the screen with
	Word m_uMatrixStackDepth;			///< Number of matrices in the matrix stack
	float m_fClearDepth;				///< Z Value to clear the Z buffer with

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
	virtual void Resize(Word uWidth,Word uHeight);
	virtual void SetViewport(Word uX,Word uY,Word uWidth,Word uHeight);
	virtual void SetScissorRect(Word uX,Word uY,Word uWidth,Word uHeight);
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
	virtual void SetScissor(Word bEnable);
	virtual void DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	virtual void DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	IDirect3DVertexShader9 * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	IDirect3DPixelShader9 * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;

	BURGER_INLINE IDirect3D9 *GetDirect3D9(void) const { return m_pDirect3D9; }
	BURGER_INLINE IDirect3DDevice9 *GetDirect3DDevice9(void) const { return m_pDirect3DDevice9; }
	BURGER_INLINE ID3DXMatrixStack *GetD3DXMatrixStack(void) const { return m_pD3DXMatrixStack; }
	BURGER_INLINE Word OnlyPowerOf2(void) const { return m_bPower2Textures; }
	BURGER_INLINE Word IsVSynced(void) const { return m_bVSynced; }
	BURGER_INLINE Word FullScreenGammaSupported(void) const { return m_bFullScreenGamma; }
	BURGER_INLINE Word CanCalibrateGamma(void) const { return m_bCanCalibrateGamma; }
	BURGER_INLINE Word IsRasterSlopeScaleDepthBias(void) const { return m_bRasterSlopeScaleDepthBias; }
	BURGER_INLINE Word IsRasterDepthBias(void) const { return m_bRasterDepthBias; }
	BURGER_INLINE Word IsnVidia(void) const { return m_bIsnVidia; }
	BURGER_INLINE Word IsATI(void) const { return m_bIsATI; }
	BURGER_INLINE Word IsIntel(void) const { return m_bIsIntel; }
	BURGER_INLINE Word GetMaxTextureWidth(void) const { return m_uMaxTextureWidth; }
	BURGER_INLINE Word GetMaxTextureHeight(void) const { return m_uMaxTextureHeight; }
	BURGER_INLINE Word GetMaxTextureStages(void) const { return m_uMaxTextureStages; }
	BURGER_INLINE Word GetMaxPossibleAnisotropy(void) const { return m_uMaxPossibleAnisotropy; }
	BURGER_INLINE Word GetClearColor(void) const { return m_uClearColor; }
	BURGER_INLINE float GetClearDepth(void) const { return m_fClearDepth; }
};

class DirectXPerfMarker {
	BURGER_DISABLECOPYCONSTRUCTORS(DirectXPerfMarker);
public:
	BURGER_INLINE DirectXPerfMarker(Word32 uColor,const Word16 *pMessage) 
	{
		Globals::D3DPERF_BeginEvent(uColor,pMessage);
	}
	BURGER_INLINE ~DirectXPerfMarker() 
	{
		Globals::D3DPERF_EndEvent();
	}
};

#endif
}
/* END */

#endif
