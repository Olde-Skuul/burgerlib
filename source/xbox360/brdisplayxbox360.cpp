/***************************************

	Display manager class for XBox 360

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
#define NONET
#include <xtl.h>

//
// Initialize the D3D variables
//

Burger::Display::Display(GameApp *pGameApp) :
	m_pD3DDevice(NULL),
	m_fClearDepth(1.0f)
{
	InitDefaults(pGameApp);
	InitGlobals();
}

//
// Shutdown
//

Burger::Display::~Display()
{
	Shutdown();
}

//
// Initialize a Direct3D9 context
//

Word Burger::Display::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	// Set the new size of the screen

	uFlags |= FULLPALETTEALLOWED;

	//
	// Create my directx9 instance and store it
	//

	m_uFlags = uFlags;
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_uDepth = uDepth;

	// Create my direct draw9 instance

	D3DPRESENT_PARAMETERS d3dpp;
	MemoryClear(&d3dpp,sizeof(d3dpp));
	d3dpp.BackBufferWidth = uWidth;
	d3dpp.BackBufferHeight = uHeight;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	//d3dpp.MultiSampleQuality = 0;
	//d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.hDeviceWindow = NULL;
	//d3dpp.Windowed = FALSE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	//d3dpp.Flags = 0;
	//d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.DisableAutoBackBuffer = FALSE;
	d3dpp.DisableAutoFrontBuffer = FALSE;
	d3dpp.FrontBufferFormat = D3DFMT_LE_X8R8G8B8;
	//d3dpp.FrontBufferColorSpace = D3DCOLORSPACE_RGB;

	HRESULT hResult = Direct3D::CreateDevice(0,D3DDEVTYPE_HAL,NULL,0,&d3dpp,&m_pD3DDevice);
	if (hResult != D3D_OK) {
		return 10;
	}
	InitState();
	return 0;
}

//
// Release the Direct3D context
//

void Burger::Display::Shutdown(void)
{
	if (m_pD3DDevice) {
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
}

//
// Start a D3D scene
//

void Burger::Display::BeginScene(void)
{
	D3DDevice *pD3DDevice = m_pD3DDevice;
	if (pD3DDevice) {
		pD3DDevice->BeginScene();
	}
}

//
// End a D3D scene and present it to the display
//

void Burger::Display::EndScene(void)
{
	// Display the final result!
	D3DDevice *pD3DDevice = m_pD3DDevice;
	if (pD3DDevice) {
		pD3DDevice->EndScene();
		pD3DDevice->Present(NULL,NULL,NULL,NULL);
	}
}

Burger::Texture *Burger::Display::CreateTextureObject(void)
{
	return new (Alloc(sizeof(Texture))) Texture;
}

Burger::VertexBuffer *Burger::Display::CreateVertexBufferObject(void)
{
	return new (Alloc(sizeof(VertexBuffer))) VertexBuffer;
}

void Burger::Display::SetViewport(Word uX,Word uY,Word uWidth,Word uHeight)
{
	D3DVIEWPORT9 Temp;
	m_pD3DDevice->GetViewport(&Temp);
	Temp.X = uX;
	Temp.Y = uY;
	Temp.Width = uWidth;
	Temp.Height = uHeight;
	m_pD3DDevice->SetViewport(&Temp);
}

void Burger::Display::SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	m_uClearColor = D3DCOLOR_COLORVALUE(fRed,fGreen,fBlue,fAlpha);
}

void Burger::Display::SetClearDepth(float fDepth)
{
	m_fClearDepth = fDepth;
}

void Burger::Display::Clear(Word uMask)
{
	Word uDXMask = 0;
	if (uMask&CLEAR_COLOR) {
		uDXMask = D3DCLEAR_TARGET;
	}
	if (uMask&CLEAR_DEPTH) {
		uDXMask |= D3DCLEAR_ZBUFFER;
	}
	if (uMask&CLEAR_STENCIL) {
		uDXMask |= D3DCLEAR_STENCIL;
	}
	m_pD3DDevice->Clear(0,NULL,uDXMask,m_uClearColor,m_fClearDepth,0);
}

void Burger::Display::Bind(Texture *pTexture,Word uIndex)
{
	static const DWORD s_Wrapping[] = {
		D3DTADDRESS_WRAP,	// WRAP_REPEAT
		D3DTADDRESS_CLAMP	// WRAP_CLAMP	
	};
	static const DWORD s_Filter[] = {
		D3DTEXF_POINT,	// FILTER_NEAREST,
		D3DTEXF_LINEAR	// FILTER_LINEAR
	};
	BURGER_ASSERT(uIndex<BURGER_ARRAYSIZE(m_pBoundTextures));
	m_pBoundTextures[uIndex] = pTexture;

	// Get the texture ID
	D3DDevice *pDevice = m_pD3DDevice;
	if (!pTexture) {
		pDevice->SetTexture(uIndex,NULL);
	} else {
		pTexture->Bind(this);
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,s_Wrapping[pTexture->GetWrappingS()]);
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,s_Wrapping[pTexture->GetWrappingT()]);
		pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,s_Filter[pTexture->GetMinFilter()]);
		pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,s_Filter[pTexture->GetMagFilter()]);
	}
}

void Burger::Display::SetBlend(Word bEnable)
{
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,bEnable!=0);
}

void Burger::Display::SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor)
{
	static const DWORD s_Source[] = {
		D3DBLEND_ZERO,			// SRCBLEND_ZERO
		D3DBLEND_ONE,			// SRCBLEND_ONE
		D3DBLEND_SRCCOLOR,		// SRCBLEND_COLOR
		D3DBLEND_INVSRCCOLOR,	// SRCBLEND_ONE_MINUS_COLOR
		D3DBLEND_SRCALPHA,		// SRCBLEND_SRC_ALPHA
		D3DBLEND_INVSRCALPHA,	// SRCBLEND_ONE_MINUS_SRC_ALPHA
		D3DBLEND_DESTALPHA,		// SRCBLEND_DST_ALPHA,
		D3DBLEND_INVDESTALPHA,	// SRCBLEND_ONE_MINUS_DST_ALPHA
		D3DBLEND_SRCALPHASAT	// SRCBLEND_SRC_ALPHA_SATURATE
	};
	static const DWORD s_Dest[] = {
		D3DBLEND_ZERO,			// DSTBLEND_ZERO
		D3DBLEND_ONE,			// DSTBLEND_ONE
		D3DBLEND_DESTCOLOR,		// DSTBLEND_COLOR
		D3DBLEND_INVDESTCOLOR,	// DSTBLEND_ONE_MINUS_COLOR
		D3DBLEND_DESTALPHA,		// DSTBLEND_DST_ALPHA
		D3DBLEND_INVDESTALPHA,	// DSTBLEND_ONE_MINUS_DST_ALPHA
		D3DBLEND_SRCALPHA,		// DSTBLEND_SRC_ALPHA
		D3DBLEND_INVSRCALPHA	// DSTBLEND_ONE_MINUS_SRC_ALPHA
	};
	BURGER_ASSERT(uSourceFactor<BURGER_ARRAYSIZE(s_Source));
	BURGER_ASSERT(uDestFactor<BURGER_ARRAYSIZE(s_Dest));
	D3DDevice *pDevice = m_pD3DDevice;
	pDevice->SetRenderState(D3DRS_SRCBLEND,s_Source[uSourceFactor]);
	pDevice->SetRenderState(D3DRS_DESTBLEND,s_Dest[uDestFactor]);
}

void Burger::Display::SetLighting(Word /* bEnable */)
{
//	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING,bEnable!=0);
}

void Burger::Display::SetZWrite(Word bEnable)
{
	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,bEnable!=0);
}

void Burger::Display::SetDepthTest(eDepthFunction uDepthFunction)
{
	static const DWORD s_WriteFunction[] = {
		D3DCMP_NEVER,			// DEPTHCMP_NEVER
		D3DCMP_LESS,			// DEPTHCMP_LESS
		D3DCMP_EQUAL,			// DEPTHCMP_EQUAL
		D3DCMP_LESSEQUAL,		// DEPTHCMP_LESSEQUAL
		D3DCMP_GREATER,			// DEPTHCMP_GREATER
		D3DCMP_NOTEQUAL,		// DEPTHCMP_NOTEQUAL
		D3DCMP_GREATEREQUAL,	// DEPTHCMP_GREATEREQUAL
		D3DCMP_ALWAYS			// DEPTHCMP_ALWAYS
	};
	BURGER_ASSERT(uDepthFunction<BURGER_ARRAYSIZE(s_WriteFunction));
	m_pD3DDevice->SetRenderState(D3DRS_ZFUNC,s_WriteFunction[uDepthFunction]);
}

void Burger::Display::SetCullMode(eCullMode uCullMode)
{
	static const DWORD s_Cull[] = {
		D3DCULL_NONE,		// CULL_NONE
		D3DCULL_CW,			// CULL_CLOCKWISE
		D3DCULL_CCW			// CULL_COUNTERCLOCKWISE
	};
	BURGER_ASSERT(uCullMode<BURGER_ARRAYSIZE(s_Cull));
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,s_Cull[uCullMode]);
}

void Burger::Display::DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer)
{
	static const D3DPRIMITIVETYPE s_Prims[] = {
		D3DPT_POINTLIST,		// PRIM_POINTS
		D3DPT_LINELIST,			// PRIM_LINES,
		D3DPT_LINESTRIP,		// PRIM_LINESTRIP,
		D3DPT_TRIANGLELIST,		// PRIM_TRIANGLES,
		D3DPT_TRIANGLESTRIP,	// PRIM_TRIANGLESTRIP,
		D3DPT_TRIANGLEFAN		// PRIM_TRIANGLEFAN
	};
	D3DDevice *pDevice = m_pD3DDevice;
	pDevice->SetVertexDeclaration(pVertexBuffer->GetD3DVertexDescription());
	pDevice->SetStreamSource(0,pVertexBuffer->GetD3DVertexBuffer(),0,pVertexBuffer->GetStride());
	Word uCount = pVertexBuffer->GetArrayEntryCount();
	switch (uPrimitiveType) {
	case PRIM_POINTS:
	default:
		break;
	case PRIM_LINES:
		uCount>>=1;
		break;
	case PRIM_LINESTRIP:
	case PRIM_TRIANGLEFAN:
		--uCount;
		break;
	case PRIM_TRIANGLES:
		uCount/=3;
		break;
	case PRIM_TRIANGLESTRIP:
		uCount-=2;
		break;
	}
	pDevice->DrawPrimitive(s_Prims[uPrimitiveType],0,uCount);
}


D3DVertexShader * BURGER_API Burger::Display::CreateVertexShader(const void *pVertexShaderBinary) const
{
	D3DVertexShader *pResult = 0;
	HRESULT hr = m_pD3DDevice->CreateVertexShader(static_cast<const DWORD *>(pVertexShaderBinary),&pResult);
	if (hr!=D3D_OK) {
		pResult = 0;
	}
	return pResult;
}


D3DPixelShader * BURGER_API Burger::Display::CreatePixelShader(const void *pPixelShaderBinary) const
{
	D3DPixelShader *pResult = 0;
	HRESULT hr = m_pD3DDevice->CreatePixelShader(static_cast<const DWORD *>(pPixelShaderBinary),&pResult);
	if (hr!=D3D_OK) {
		pResult = 0;
	}
	return pResult;
}


void BURGER_API Burger::Display::InitState(void)
{
	D3DDevice *pDirect3DDevice9 = m_pD3DDevice;

	pDirect3DDevice9->SetVertexShader(NULL);
	pDirect3DDevice9->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDirect3DDevice9->SetRenderState_Inline(D3DRS_ZENABLE,D3DZB_FALSE);
	pDirect3DDevice9->SetRenderState_Inline(D3DRS_CULLMODE,D3DCULL_NONE);

	/* Enable separate alpha blend function, if possible */

	pDirect3DDevice9->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,TRUE);
}


#endif
