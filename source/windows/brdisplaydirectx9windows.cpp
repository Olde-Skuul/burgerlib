/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx9.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brgameapp.h"
#include "brglobals.h"
#include "brstring16.h"
#include "brtexturedirectx9.h"
#include "brvertexbufferdirectx9.h"
#include "brmatrix4d.h"

#if !defined(DIRECTDRAW_VERSION) && !defined(DOXYGEN)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(DIRECT3D_VERSION) && !defined(DOXYGEN)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(_DEBUG) && !defined(DOXYGEN)
#define D3D_DEBUG_INFO
#endif

#include <Windows.h>
#include <d3d9.h>
#include <ddraw.h>
#include <d3dx9math.h>

#ifdef _DEBUG
#define PRINTHRESULT(hResult) /* m_pGameApp->Poll(); */ if (hResult!=D3D_OK) Debug::Message("Error at line " BURGER_MACRO_TO_STRING(__LINE__) " with 0x%08X\n",hResult)
#else
#define PRINTHRESULT(hResult)
#endif
static D3DPRESENT_PARAMETERS g_DisplayPresentParms;

//
// Data primitive types
//

#if !defined(DOXYGEN)

static const D3DPRIMITIVETYPE g_Prims[] = {
	D3DPT_POINTLIST,		// PRIM_POINTS
	D3DPT_LINELIST,			// PRIM_LINES,
	D3DPT_LINESTRIP,		// PRIM_LINESTRIP,
	D3DPT_TRIANGLELIST,		// PRIM_TRIANGLES,
	D3DPT_TRIANGLESTRIP,	// PRIM_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN		// PRIM_TRIANGLEFAN
};

static const DWORD g_Wrapping[] = {
	D3DTADDRESS_WRAP,	// WRAP_REPEAT
	D3DTADDRESS_CLAMP	// WRAP_CLAMP	
};

static const DWORD g_Filter[] = {
	D3DTEXF_POINT,	// FILTER_NEAREST,
	D3DTEXF_LINEAR	// FILTER_LINEAR
};

static const DWORD g_SourceBlend[] = {
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

static const DWORD g_DestBlend[] = {
	D3DBLEND_ZERO,			// DSTBLEND_ZERO
	D3DBLEND_ONE,			// DSTBLEND_ONE
	D3DBLEND_DESTCOLOR,		// DSTBLEND_COLOR
	D3DBLEND_INVDESTCOLOR,	// DSTBLEND_ONE_MINUS_COLOR
	D3DBLEND_DESTALPHA,		// DSTBLEND_DST_ALPHA
	D3DBLEND_INVDESTALPHA,	// DSTBLEND_ONE_MINUS_DST_ALPHA
	D3DBLEND_SRCALPHA,		// DSTBLEND_SRC_ALPHA
	D3DBLEND_INVSRCALPHA	// DSTBLEND_ONE_MINUS_SRC_ALPHA
};

static const DWORD g_WriteFunction[] = {
	D3DCMP_NEVER,			// DEPTHCMP_NEVER
	D3DCMP_LESS,			// DEPTHCMP_LESS
	D3DCMP_EQUAL,			// DEPTHCMP_EQUAL
	D3DCMP_LESSEQUAL,		// DEPTHCMP_LESSEQUAL
	D3DCMP_GREATER,			// DEPTHCMP_GREATER
	D3DCMP_NOTEQUAL,		// DEPTHCMP_NOTEQUAL
	D3DCMP_GREATEREQUAL,	// DEPTHCMP_GREATEREQUAL
	D3DCMP_ALWAYS			// DEPTHCMP_ALWAYS
};

static const DWORD g_CullOperation[] = {
	D3DCULL_NONE,		// CULL_NONE
	D3DCULL_CW,			// CULL_CLOCKWISE
	D3DCULL_CCW			// CULL_COUNTERCLOCKWISE
};

#endif

//
// Initialize the D3D variables
//

Burger::DisplayDirectX9::DisplayDirectX9(GameApp *pGameApp) :
	Display(pGameApp),
	m_pDirect3D9(NULL),
	m_pDirect3DDevice9(NULL),
	m_pD3DXMatrixStack(NULL),
	m_pDefaultRenderTarget(NULL),
	m_pCurrentRenderTarget(NULL),
	m_bLostDevice(FALSE),
	m_bPower2Textures(FALSE),
	m_bVSynced(FALSE),
	m_bMultiRenderTargets(FALSE),
	m_bSeparateAlphaBlend(FALSE),
	m_bSceneBegun(FALSE),
	m_bFullScreenGamma(FALSE),
	m_bCanCalibrateGamma(FALSE),
	m_bRasterSlopeScaleDepthBias(FALSE),
	m_bRasterDepthBias(FALSE),
	m_bIsnVidia(FALSE),
	m_bIsATI(FALSE),
	m_bIsIntel(FALSE),
	m_uBackBufferFormat(0),
	m_uMaxTextureWidth(0),
	m_uMaxTextureHeight(0),
	m_uMaxTextureStages(0),
	m_uMaxTextureSamplers(0),
	m_uMaxPossibleAnisotropy(0),
	m_uClearColor(0),
	m_uMatrixStackDepth(0),
	m_fClearDepth(1.0f)
{
}

//
// Shutdown
//

Burger::DisplayDirectX9::~DisplayDirectX9()
{
	Shutdown();
}

//
// Initialize a Direct3D9 context
//

Word Burger::DisplayDirectX9::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	// Set the new size of the screen

	uFlags |= FULLPALETTEALLOWED;

	Shutdown();

	//
	// Create my directx9 instance and store it
	//

	IDirect3D9 *pDirect3D9 = m_pDirect3D9;

	//
	// Was on already in existence?
	//

	if (!pDirect3D9) {
		pDirect3D9 = Globals::Direct3DCreate9(D3D_SDK_VERSION);
		if (!pDirect3D9) {
			return 10;		// Boned?
		}
		// Set the Direct X context
		m_pDirect3D9 = pDirect3D9;
	}

	//
	// Choose which adapter to use
	//

	Word uAdapterIndex = D3DADAPTER_DEFAULT;

	//
	// Get adapter manufacturer, so specific code could be easily added
	//
	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	HRESULT hResult = pDirect3D9->GetAdapterIdentifier(uAdapterIndex,0,&AdapterIdentifier);
	PRINTHRESULT(hResult);
	if (hResult!=D3D_OK) {
		MemoryClear(&AdapterIdentifier,sizeof(AdapterIdentifier));
	}
	// Capture the vendor ID of the card that's being requested
	m_bIsnVidia = (AdapterIdentifier.VendorId == 0x10DE);
	m_bIsATI = (AdapterIdentifier.VendorId == 0x1002);
	m_bIsIntel = (AdapterIdentifier.VendorId == 0x8086);
	
	//
	// Get the device caps
	//

	D3DCAPS9 CurrentDeviceCaps;
	hResult = pDirect3D9->GetDeviceCaps(uAdapterIndex,D3DDEVTYPE_HAL,&CurrentDeviceCaps);
	PRINTHRESULT(hResult);

	// Save the gamma caps
	m_bFullScreenGamma = (CurrentDeviceCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA)!=0;
	m_bCanCalibrateGamma = (CurrentDeviceCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA)!=0;

	// Save the depth bias supported flags
	m_bRasterSlopeScaleDepthBias = (CurrentDeviceCaps.RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS)!=0;
	m_bRasterDepthBias = (CurrentDeviceCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS)!=0;

	// Grab some constants
	m_uMaxTextureStages = CurrentDeviceCaps.MaxTextureBlendStages;
	m_uMaxTextureSamplers = CurrentDeviceCaps.MaxSimultaneousTextures;
	m_uMaxPossibleAnisotropy = CurrentDeviceCaps.MaxAnisotropy;
	m_uMaxTextureWidth = CurrentDeviceCaps.MaxTextureWidth;
	m_uMaxTextureHeight = CurrentDeviceCaps.MaxTextureHeight; 
	m_bMultiRenderTargets = (CurrentDeviceCaps.NumSimultaneousRTs >= 2);
	m_bSeparateAlphaBlend = (CurrentDeviceCaps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)!=0;

	//
	// Determine if power of 2 textures are required
	//

	m_bPower2Textures = (CurrentDeviceCaps.TextureCaps & (D3DPTEXTURECAPS_NONPOW2CONDITIONAL|D3DPTEXTURECAPS_POW2))==D3DPTEXTURECAPS_POW2;
	
	//
	// Get the D3D matrix stack
	//

	if (!m_pD3DXMatrixStack) {
		if (Globals::D3DXCreateMatrixStack(0,&m_pD3DXMatrixStack)!=D3D_OK) {
			return 10;		// Boned?
		}
	}



	// Save the states
	m_uFlags = uFlags;
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_uDepth = uDepth;

	//
	// Set up the window
	//

	HWND pWindow = m_pGameApp->GetWindow();
	if (uFlags & FULLSCREEN) {
		m_pGameApp->SetWindowFullScreen(uWidth,uHeight);
	} else {
		m_pGameApp->SetWindowSize(uWidth,uHeight);
	}

	//
	// Create my D3D device
	//

	D3DPRESENT_PARAMETERS DisplayPresentParms;
	MemoryClear(&DisplayPresentParms,sizeof(DisplayPresentParms));

	// Shared values
	DisplayPresentParms.BackBufferWidth = m_uWidth;
	DisplayPresentParms.BackBufferHeight = m_uHeight;
	DisplayPresentParms.BackBufferCount = 1;
	DisplayPresentParms.MultiSampleType = D3DMULTISAMPLE_NONE;
	DisplayPresentParms.MultiSampleQuality = 0;
	DisplayPresentParms.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DisplayPresentParms.hDeviceWindow = m_pGameApp->GetWindow();
	DisplayPresentParms.EnableAutoDepthStencil = TRUE;
	DisplayPresentParms.AutoDepthStencilFormat = D3DFMT_D24S8;		// 24 bit depth, 8 bit stencil
	DisplayPresentParms.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	DisplayPresentParms.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (m_uFlags & FULLSCREEN) {
		DisplayPresentParms.Windowed = FALSE;
		DisplayPresentParms.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// Insert requested refresh rate
		DisplayPresentParms.BackBufferFormat = D3DFMT_A8R8G8B8;
	} else {
		DisplayPresentParms.Windowed = TRUE;
		DisplayPresentParms.FullScreen_RefreshRateInHz = 0;
		DisplayPresentParms.BackBufferFormat = D3DFMT_UNKNOWN;	// Always set to unknown for windowed mode!!!
	}

	DWORD uDisplayFlags = D3DCREATE_FPU_PRESERVE;
	// Does it support hardware transformation?
	if (CurrentDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		// Check against for version 1.1 or higher
		if (LOWORD(CurrentDeviceCaps.VertexShaderVersion) < ((1<<8)+1)) {
			// No vertex shader support? Use software and hardware
			uDisplayFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
		} else {
			// Use hardware only
			uDisplayFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
	} else {
		// Use SSE2 enhanced vertex processing instead of hardware
		uDisplayFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	if (m_uFlags&MULTITHREADED) {
		uDisplayFlags |= D3DCREATE_MULTITHREADED;
	}
	
	//
	// Try obtaining a device
	//

	hResult = pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pWindow,
		uDisplayFlags,&DisplayPresentParms,&m_pDirect3DDevice9);
	PRINTHRESULT(hResult);
	if (hResult != D3D_OK) {
		uDisplayFlags &= (~(D3DCREATE_MIXED_VERTEXPROCESSING|D3DCREATE_HARDWARE_VERTEXPROCESSING));
		uDisplayFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		hResult = pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pWindow,
			uDisplayFlags,&DisplayPresentParms,&m_pDirect3DDevice9);
		PRINTHRESULT(hResult);
		if (hResult != D3D_OK) {
			return 10;
		}
	}

	m_bLostDevice = FALSE;

	/* Get presentation parameters to fill info */
	IDirect3DSwapChain9 *pChain;
	hResult = m_pDirect3DDevice9->GetSwapChain(0,&pChain);
	PRINTHRESULT(hResult);
	if (hResult != D3D_OK) {
		return 10;
	}
	hResult = pChain->GetPresentParameters(&DisplayPresentParms);
	pChain->Release();
	PRINTHRESULT(hResult);
	if (hResult != D3D_OK) {
		return 10;
	}
	MemoryCopy(&g_DisplayPresentParms,&DisplayPresentParms,sizeof(DisplayPresentParms));
	m_bVSynced = FALSE;

	if (DisplayPresentParms.PresentationInterval == D3DPRESENT_INTERVAL_ONE) {
		m_bVSynced = TRUE;
	}

	// Store the default render target
	m_pDirect3DDevice9->GetRenderTarget(0,&m_pDefaultRenderTarget);
	m_pCurrentRenderTarget = NULL;

	InitState();
	m_bSceneBegun = FALSE;

	return 0;
}

//
// Release the Direct3D context
//

void Burger::DisplayDirectX9::Shutdown(void)
{
	Texture::ReleaseAll(this);

	// 
	// Release the allocated data
	//

	if (m_pD3DXMatrixStack) {
		m_pD3DXMatrixStack->Release();
		m_pD3DXMatrixStack = NULL;
		m_uMatrixStackDepth = 0;
	}

	if (m_pCurrentRenderTarget) {
		m_pCurrentRenderTarget->Release();
		m_pCurrentRenderTarget = NULL;
	}
	if (m_pDefaultRenderTarget) {
		m_pDefaultRenderTarget->Release();
		m_pDefaultRenderTarget = NULL;
	}

	if (m_pDirect3DDevice9) {
		m_pDirect3DDevice9->Release();
		m_pDirect3DDevice9 = NULL;
	}
	if (m_pDirect3D9) {
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
}

//
// Start a D3D scene
//

void Burger::DisplayDirectX9::BeginScene(void)
{
	if (!m_bSceneBegun) {
		IDirect3DDevice9 *pDirect3DDevice9 = m_pDirect3DDevice9;
		if (pDirect3DDevice9) {
			HRESULT hResult = pDirect3DDevice9->BeginScene();
			PRINTHRESULT(hResult);
			if (hResult == D3DERR_DEVICELOST) {
				if (Reset()) {
					return;
				}
				hResult = pDirect3DDevice9->BeginScene();
				PRINTHRESULT(hResult);
			}
			if (hResult==D3D_OK) {
				m_bSceneBegun = TRUE;
			}
		}
	}
}

//
// End a D3D scene and present it to the display
//

void Burger::DisplayDirectX9::EndScene(void)
{
	// Display the final result!
	IDirect3DDevice9 *pDirect3DDevice9 = m_pDirect3DDevice9;
	if (pDirect3DDevice9) {
		if (m_bSceneBegun) {
			pDirect3DDevice9->EndScene();
			m_bSceneBegun = FALSE;
		}

		HRESULT hResult = pDirect3DDevice9->TestCooperativeLevel();
		PRINTHRESULT(hResult);
		if (hResult == D3DERR_DEVICELOST) {
			/* We'll reset later */
			return;
		}
		if (hResult == D3DERR_DEVICENOTRESET) {
			Reset();
		}
		HWND pWindow = m_pGameApp->GetWindow();
		hResult = pDirect3DDevice9->Present(NULL,NULL,NULL,NULL);
		PRINTHRESULT(hResult);
		if (hResult==D3D_OK) {
			if (!(m_uFlags&FULLSCREEN)) {
				ValidateRect(pWindow,NULL);
			}
		}
	}
}

Burger::Texture *Burger::DisplayDirectX9::CreateTextureObject(void)
{
	return new (Alloc(sizeof(TextureDirectX9))) TextureDirectX9;
}

Burger::VertexBuffer *Burger::DisplayDirectX9::CreateVertexBufferObject(void)
{
	return new (Alloc(sizeof(VertexBufferDirectX9))) VertexBufferDirectX9;
}

void Burger::DisplayDirectX9::Resize(Word /* uWidth */,Word /* uHeight */)
{
//	if (m_pDirect3DDevice9) {
//		m_uWidth = uWidth;
//		m_uHeight = uHeight;
//		SetViewport(0,0,uWidth,uHeight);
//	}
}

void Burger::DisplayDirectX9::SetViewport(Word uX,Word uY,Word uWidth,Word uHeight)
{
	IDirect3DDevice9 *pDevice = m_pDirect3DDevice9;
	if (pDevice) {
		D3DVIEWPORT9 Temp;
		pDevice->GetViewport(&Temp);
		Temp.X = uX;
		Temp.Y = uY;
		Temp.Width = uWidth;
		Temp.Height = uHeight;
		pDevice->SetViewport(&Temp);
	}
}

void Burger::DisplayDirectX9::SetScissorRect(Word uX,Word uY,Word uWidth,Word uHeight)
{
	RECT Temp;
	Temp.left = static_cast<LONG>(uX);
	Temp.top = static_cast<LONG>(uY);
	Temp.right = static_cast<LONG>(uX+uWidth);
	Temp.bottom = static_cast<LONG>(uY+uHeight);
	m_pDirect3DDevice9->SetScissorRect(&Temp);
}

void Burger::DisplayDirectX9::SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	m_uClearColor = D3DCOLOR_COLORVALUE(fRed,fGreen,fBlue,fAlpha);
}

void Burger::DisplayDirectX9::SetClearDepth(float fDepth)
{
	m_fClearDepth = fDepth;
}

void Burger::DisplayDirectX9::Clear(Word uMask)
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
	m_pDirect3DDevice9->Clear(0,NULL,uDXMask,m_uClearColor,m_fClearDepth,0);
}

void Burger::DisplayDirectX9::Bind(Texture *pTexture,Word uIndex)
{
	BURGER_ASSERT(uIndex<BURGER_ARRAYSIZE(m_pBoundTextures));
	m_pBoundTextures[uIndex] = pTexture;

	// Get the texture ID
	IDirect3DDevice9 *pDevice = m_pDirect3DDevice9;
	if (!pTexture) {
		pDevice->SetTexture(uIndex,NULL);
	} else {
		pTexture->Bind(this);
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,g_Wrapping[pTexture->GetWrappingS()]);
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,g_Wrapping[pTexture->GetWrappingT()]);
		pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,g_Filter[pTexture->GetMinFilter()]);
		pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,g_Filter[pTexture->GetMagFilter()]);
	}
}

void Burger::DisplayDirectX9::SetBlend(Word bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE,bEnable!=0);
}

void Burger::DisplayDirectX9::SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor)
{
	BURGER_ASSERT(uSourceFactor<BURGER_ARRAYSIZE(g_SourceBlend));
	BURGER_ASSERT(uDestFactor<BURGER_ARRAYSIZE(g_DestBlend));
	IDirect3DDevice9 *pDevice = m_pDirect3DDevice9;
	pDevice->SetRenderState(D3DRS_SRCBLEND,g_SourceBlend[uSourceFactor]);
	pDevice->SetRenderState(D3DRS_DESTBLEND,g_DestBlend[uDestFactor]);
}

void Burger::DisplayDirectX9::SetLighting(Word bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_LIGHTING,bEnable!=0);
}

void Burger::DisplayDirectX9::SetZWrite(Word bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE,bEnable!=0);
}

void Burger::DisplayDirectX9::SetDepthTest(eDepthFunction uDepthFunction)
{
	BURGER_ASSERT(uDepthFunction<BURGER_ARRAYSIZE(g_WriteFunction));
	m_pDirect3DDevice9->SetRenderState(D3DRS_ZFUNC,g_WriteFunction[uDepthFunction]);
}

void Burger::DisplayDirectX9::SetCullMode(eCullMode uCullMode)
{
	BURGER_ASSERT(uCullMode<BURGER_ARRAYSIZE(g_CullOperation));
	m_pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE,g_CullOperation[uCullMode]);
}

void Burger::DisplayDirectX9::SetScissor(Word bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE,bEnable!=0);
}

void Burger::DisplayDirectX9::DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer)
{
	IDirect3DDevice9 *pDevice = m_pDirect3DDevice9;
	pDevice->SetVertexDeclaration(static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetDX9VertexDescription());
	pDevice->SetStreamSource(0,static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetDX9VertexBuffer(),0,static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetStride());
	Word uCount = static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetArrayEntryCount();
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
	pDevice->DrawPrimitive(g_Prims[uPrimitiveType],0,uCount);
}

void Burger::DisplayDirectX9::DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer)
{
	IDirect3DDevice9 *pDevice = m_pDirect3DDevice9;
	pDevice->SetVertexDeclaration(static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetDX9VertexDescription());
	pDevice->SetStreamSource(0,static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetDX9VertexBuffer(),0,static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetStride());
	Word uCount = static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetArrayEntryCount();
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
	pDevice->DrawIndexedPrimitive(g_Prims[uPrimitiveType],0,0,static_cast<VertexBufferDirectX9 *>(pVertexBuffer)->GetArrayEntryCount(),0,uCount);
}


IDirect3DVertexShader9 * BURGER_API Burger::DisplayDirectX9::CreateVertexShader(const void *pVertexShaderBinary) const
{
	IDirect3DVertexShader9 *pResult = 0;
	HRESULT hr = m_pDirect3DDevice9->CreateVertexShader(static_cast<const DWORD *>(pVertexShaderBinary),&pResult);
	if (hr!=D3D_OK) {
		pResult = 0;
	}
	return pResult;
}


IDirect3DPixelShader9 * BURGER_API Burger::DisplayDirectX9::CreatePixelShader(const void *pPixelShaderBinary) const
{
	IDirect3DPixelShader9 *pResult = 0;
	HRESULT hr = m_pDirect3DDevice9->CreatePixelShader(static_cast<const DWORD *>(pPixelShaderBinary),&pResult);
	if (hr!=D3D_OK) {
		pResult = 0;
	}
	return pResult;
}

/*! ************************************

	\brief Initialize a directx D3DPRESENT_PARAMETERS structure.

	This is called on device creation or device reset
	
***************************************/

void BURGER_API Burger::DisplayDirectX9::CreatePresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput)
{
	MemoryClear(pOutput,sizeof(*pOutput));

	// Shared values
	pOutput->BackBufferWidth = m_uWidth;
	pOutput->BackBufferHeight = m_uHeight;
	pOutput->BackBufferFormat = static_cast<D3DFORMAT>(m_uBackBufferFormat);
	pOutput->MultiSampleType = D3DMULTISAMPLE_NONE;
	pOutput->MultiSampleQuality = 0;
	pOutput->SwapEffect = D3DSWAPEFFECT_DISCARD;
	pOutput->hDeviceWindow = m_pGameApp->GetWindow();
	pOutput->EnableAutoDepthStencil = TRUE;
	pOutput->AutoDepthStencilFormat = D3DFMT_D24X8;
	pOutput->Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	pOutput->FullScreen_RefreshRateInHz = 0;
	pOutput->PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (m_uFlags & FULLSCREEN) {
		pOutput->BackBufferCount = 1;
		pOutput->Windowed = FALSE;
	} else {
		pOutput->Windowed = TRUE;
	}
}


void BURGER_API Burger::DisplayDirectX9::InitState(void)
{
	IDirect3DDevice9 *pDirect3DDevice9 = m_pDirect3DDevice9;

	pDirect3DDevice9->SetVertexShader(NULL);
	pDirect3DDevice9->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDirect3DDevice9->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice9->SetRenderState(D3DRS_LIGHTING,FALSE);

	/* Enable color modulation by diffuse color */
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	/* Enable alpha modulation by diffuse alpha */
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pDirect3DDevice9->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

	/* Enable separate alpha blend function, if possible */
	if (m_bSeparateAlphaBlend) {
		pDirect3DDevice9->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,TRUE);
	}

	/* Disable second texture stage, since we're done */
	pDirect3DDevice9->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pDirect3DDevice9->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	/* Set an identity world and view matrix */
	
	pDirect3DDevice9->SetTransform(D3DTS_WORLD,reinterpret_cast<const D3DMATRIX *>(&s_Matrix4DIdentity));
	pDirect3DDevice9->SetTransform(D3DTS_VIEW,reinterpret_cast<const D3DMATRIX *>(&s_Matrix4DIdentity));
}


Word BURGER_API Burger::DisplayDirectX9::Reset(void)
{
	/* Release the default render target before reset */
	if (m_pDefaultRenderTarget) {
		m_pDefaultRenderTarget->Release();
		m_pDefaultRenderTarget = NULL;
	}
	if (m_pCurrentRenderTarget) {
		m_pCurrentRenderTarget->Release();
		m_pCurrentRenderTarget = NULL;
	}

	/* Release application render targets */
	//Texture::ReleaseAll();

	HRESULT hResult = m_pDirect3DDevice9->Reset(&g_DisplayPresentParms);
	PRINTHRESULT(hResult);
	if (hResult!=D3D_OK) {
		if (hResult == D3DERR_DEVICELOST) {
			/* Don't worry about it, we'll reset later... */
			return 0;
		} else {
			return 10;
		}
	}

	/* Allocate application render targets */
	//Texture::RebuildAll();

	m_pDirect3DDevice9->GetRenderTarget(0,&m_pDefaultRenderTarget);
	InitState();
	//D3D_SetRenderTargetInternal(renderer, renderer->target);
	//D3D_UpdateViewport(renderer);
	return 0;
}


BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayDirectX9,Burger::Display);

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayDirectX9::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/

#endif
