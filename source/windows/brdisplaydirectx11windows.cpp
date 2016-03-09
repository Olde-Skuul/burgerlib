/***************************************

	DirectX 11 manager class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx11.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brgameapp.h"

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
#include <d3d11.h>
#include <ddraw.h>

#ifdef _DEBUG
#define PRINTHRESULT(hResult) /* m_pGameApp->Poll(); */ if (hResult!=D3D_OK) Debug::Message("Error at line " BURGER_MACRO_TO_STRING(__LINE__) " with 0x%08X\n",hResult)
#else
#define PRINTHRESULT(hResult)
#endif

/*! ************************************

	\brief Clear a DirectX 11 context

	Set all values of a DirectX 11 context to default values

	\param pDX11Context Pointer to a DirectX 11 context

***************************************/

void BURGER_API Burger::DisplayDirectX11::ClearContext(ID3D11DeviceContext *pDX11Context)
{
	// Detach everything from the DirectX 11 context
	if (pDX11Context) {
		// Detach the shaders
		pDX11Context->VSSetShader(NULL,NULL,0);
		pDX11Context->HSSetShader(NULL,NULL,0);
		pDX11Context->DSSetShader(NULL,NULL,0);
		pDX11Context->GSSetShader(NULL,NULL,0);
		pDX11Context->PSSetShader(NULL,NULL,0);

		// Clear this buffer to pass the array clear out states
		ID3D11Buffer *ZeroBuffers[16];
		MemoryClear(ZeroBuffers,sizeof(ZeroBuffers));

		// Clear out the index buffers
		pDX11Context->IASetVertexBuffers(0,16,ZeroBuffers,static_cast<const UINT *>(static_cast<const void *>(ZeroBuffers)),static_cast<const UINT *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->IASetIndexBuffer(NULL,DXGI_FORMAT_R16_UINT,0);
		pDX11Context->IASetInputLayout(NULL);

		// Clear out the constant buffers
		pDX11Context->VSSetConstantBuffers(0,14,ZeroBuffers);
		pDX11Context->HSSetConstantBuffers(0,14,ZeroBuffers);
		pDX11Context->DSSetConstantBuffers(0,14,ZeroBuffers);
		pDX11Context->GSSetConstantBuffers(0,14,ZeroBuffers);
		pDX11Context->PSSetConstantBuffers(0,14,ZeroBuffers);

		// Clear out the resources attached to the shaders
		pDX11Context->VSSetShaderResources(0,16,static_cast<ID3D11ShaderResourceView * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->HSSetShaderResources(0,16,static_cast<ID3D11ShaderResourceView * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->DSSetShaderResources(0,16,static_cast<ID3D11ShaderResourceView * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->GSSetShaderResources(0,16,static_cast<ID3D11ShaderResourceView * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->PSSetShaderResources(0,16,static_cast<ID3D11ShaderResourceView * const *>(static_cast<const void *>(ZeroBuffers)));

		// Clear out the samplers
		pDX11Context->VSSetSamplers(0,16,static_cast<ID3D11SamplerState * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->HSSetSamplers(0,16,static_cast<ID3D11SamplerState * const *>(static_cast<const void *>(ZeroBuffers)));	
		pDX11Context->DSSetSamplers(0,16,static_cast<ID3D11SamplerState * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->GSSetSamplers(0,16,static_cast<ID3D11SamplerState * const *>(static_cast<const void *>(ZeroBuffers)));
		pDX11Context->PSSetSamplers(0,16,static_cast<ID3D11SamplerState * const *>(static_cast<const void *>(ZeroBuffers)));

		// Clear out the render target
		pDX11Context->OMSetRenderTargets(8,static_cast<ID3D11RenderTargetView * const *>(static_cast<const void *>(ZeroBuffers)),NULL);

		// Clear out the rest of the states
		pDX11Context->OMSetBlendState(NULL,static_cast<const FLOAT *>(static_cast<const void *>(ZeroBuffers)),0xFFFFFFFF);
		pDX11Context->OMSetDepthStencilState(NULL,0);
		pDX11Context->RSSetState(NULL);
	}
}



BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayDirectX11,Burger::Display);

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayDirectX11::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/

#endif
