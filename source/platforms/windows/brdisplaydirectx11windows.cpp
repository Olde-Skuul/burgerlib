/***************************************

    DirectX 11 manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx11.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brgameapp.h"
#include "brwindowstypes.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)
#if !defined(DIRECTDRAW_VERSION)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(DIRECT3D_VERSION)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <d3d11.h>
#include <ddraw.h>

// Available on the Windows SDK 8.0 or higher
// Mimic this for 7.0 or lower
#if !defined(WINAPI_FAMILY_PARTITION)
#define DXGI_FORMAT_B4G4R4A4_UNORM 115
#endif

#ifdef _DEBUG
#define PRINTHRESULT(hResult) /* m_pGameApp->Poll(); */ \
	if (hResult != D3D_OK) \
	Debug::Message( \
		"Error at line " BURGER_STRINGIZE(__LINE__) " with 0x%08X\n", \
		hResult)
#else
#define PRINTHRESULT(hResult)
#endif

#endif

/*! ************************************

	\brief Clear a DirectX 11 context

	Set all values of a DirectX 11 context to default values

	\param pDX11Context Pointer to a DirectX 11 context

***************************************/

void BURGER_API Burger::DisplayDirectX11::ClearContext(
	ID3D11DeviceContext* pDX11Context)
{
	// Detach everything from the DirectX 11 context
	if (pDX11Context) {
		// Detach the shaders
		pDX11Context->VSSetShader(NULL, NULL, 0);
		pDX11Context->HSSetShader(NULL, NULL, 0);
		pDX11Context->DSSetShader(NULL, NULL, 0);
		pDX11Context->GSSetShader(NULL, NULL, 0);
		pDX11Context->PSSetShader(NULL, NULL, 0);

		// Clear this buffer to pass the array clear out states
		ID3D11Buffer* ZeroBuffers[16];
		MemoryClear(ZeroBuffers, sizeof(ZeroBuffers));

		// Clear out the index buffers
		pDX11Context->IASetVertexBuffers(0, 16, ZeroBuffers,
			static_cast<const UINT*>(static_cast<const void*>(ZeroBuffers)),
			static_cast<const UINT*>(static_cast<const void*>(ZeroBuffers)));
		pDX11Context->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
		pDX11Context->IASetInputLayout(NULL);

		// Clear out the constant buffers
		pDX11Context->VSSetConstantBuffers(0, 14, ZeroBuffers);
		pDX11Context->HSSetConstantBuffers(0, 14, ZeroBuffers);
		pDX11Context->DSSetConstantBuffers(0, 14, ZeroBuffers);
		pDX11Context->GSSetConstantBuffers(0, 14, ZeroBuffers);
		pDX11Context->PSSetConstantBuffers(0, 14, ZeroBuffers);

		// Clear out the resources attached to the shaders
		pDX11Context->VSSetShaderResources(0, 16,
			static_cast<ID3D11ShaderResourceView* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->HSSetShaderResources(0, 16,
			static_cast<ID3D11ShaderResourceView* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->DSSetShaderResources(0, 16,
			static_cast<ID3D11ShaderResourceView* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->GSSetShaderResources(0, 16,
			static_cast<ID3D11ShaderResourceView* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->PSSetShaderResources(0, 16,
			static_cast<ID3D11ShaderResourceView* const*>(
				static_cast<const void*>(ZeroBuffers)));

		// Clear out the samplers
		pDX11Context->VSSetSamplers(0, 16,
			static_cast<ID3D11SamplerState* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->HSSetSamplers(0, 16,
			static_cast<ID3D11SamplerState* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->DSSetSamplers(0, 16,
			static_cast<ID3D11SamplerState* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->GSSetSamplers(0, 16,
			static_cast<ID3D11SamplerState* const*>(
				static_cast<const void*>(ZeroBuffers)));
		pDX11Context->PSSetSamplers(0, 16,
			static_cast<ID3D11SamplerState* const*>(
				static_cast<const void*>(ZeroBuffers)));

		// Clear out the render target
		pDX11Context->OMSetRenderTargets(8,
			static_cast<ID3D11RenderTargetView* const*>(
				static_cast<const void*>(ZeroBuffers)),
			NULL);

		// Clear out the rest of the states
		pDX11Context->OMSetBlendState(NULL,
			static_cast<const FLOAT*>(static_cast<const void*>(ZeroBuffers)),
			0xFFFFFFFF);
		pDX11Context->OMSetDepthStencilState(NULL, 0);
		pDX11Context->RSSetState(NULL);
	}
}

/*! ************************************

	\brief Update the cursor clip rect

	If the cursor is clipped to the full screen, let DirectX 11 know.

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX11::SetupCursor(void) const
{
	uint_t bIsFullScreen = m_uFlags & FULLSCREEN;

	// Clip cursor if requested
	if (bIsFullScreen && m_bClipCursorWhenFullScreen) {
		RECT TheRect;
		// Confine cursor to full screen window
		GetWindowRect(m_pGameApp->GetWindow(), &TheRect);
		ClipCursor(&TheRect);
	} else {
		ClipCursor(NULL);
	}
}

/*! ************************************

	\brief Get the number of bits for a single color channel

	Using a DirectX 11 DXGI_FORMAT value, return the number of bits needed to
	hold a smallest color value in a single pixel.

	Return 0 if the value is invalid

	\note For formats like DXGI_FORMAT_B5G6R5_UNORM where the color channels
	have different widths, the smallest width is returned, which in this example
	is five.

	\windowsonly
	\param uDXGI_FORMAT DXGI_FORMAT value
	\return Number of bits per pixel, 0 on error
	\sa GetD3DFORMATColorChannelBits(uint_t)

***************************************/

uint_t BURGER_API Burger::GetDXGI_FORMATColorChannelBits(uint_t uDXGI_FORMAT)
{
	switch (uDXGI_FORMAT) {
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 32;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return 16;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return 10;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 8;

	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 5;

	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 4;
	default:
		return 0;
	}
}

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayDirectX11, Burger::Display);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayDirectX11::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a reference to the parent
	(If any)

***************************************/

#endif
