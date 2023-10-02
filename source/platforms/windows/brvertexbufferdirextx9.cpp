/***************************************

    Vertex buffer class for DirectX9

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbufferdirectx9.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"
#include "brdisplaydirectx9.h"

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma disable_message(15)
// Disable '//' style comment continues on next line
#pragma disable_message(735)
#endif

#include <d3dx9.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma enable_message(15)
// Disable '//' style comment continues on next line
#pragma enable_message(735)
#endif

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBufferDirectX9,Burger::VertexBuffer);
#endif

Burger::VertexBufferDirectX9::VertexBufferDirectX9()
{
}

Burger::VertexBufferDirectX9::~VertexBufferDirectX9()
{
	Release(NULL);
}

#if !defined(DOXYGEN)
static const D3DDECLTYPE g_D3DType[5] = {
	D3DDECLTYPE_FLOAT1,
	D3DDECLTYPE_FLOAT2,
	D3DDECLTYPE_FLOAT3,
	D3DDECLTYPE_FLOAT4,
	D3DDECLTYPE_D3DCOLOR
};

#endif

//
// Update the vertex data to VRAM
//

uint_t Burger::VertexBufferDirectX9::CheckLoad(Display *pDisplay)
{
	HRESULT hResult = D3D_OK;
	if (m_uFlags & FLAG_VERTEXDIRTY) {
		VertexBufferDirectX9::Release(NULL);

		IDirect3DDevice9 *pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();

		if (m_uVertexArraySize) {

			// Create a vertex buffer object 

			DWORD uUsage = D3DUSAGE_WRITEONLY;
			if (m_uFlags&FLAGAOS_VERTICES_DYNAMIC) {
				uUsage = D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC;
			}
			hResult = pDevice->CreateVertexBuffer(static_cast<UINT>(m_uVertexArraySize),uUsage,
				0,D3DPOOL_MANAGED,&m_Platform.m_DX9.m_pVertexBuffer,NULL);
			
			// If there's data to initialize with, upload it.

			if ((hResult>=0)&&
				m_pVertexArray) {
				void *pVertices = NULL;
				hResult = m_Platform.m_DX9.m_pVertexBuffer->Lock(0,static_cast<UINT>(m_uVertexArraySize),&pVertices,0);
				if (hResult>=0) {
					MemoryCopy(pVertices,m_pVertexArray,m_uVertexArraySize);
					m_Platform.m_DX9.m_pVertexBuffer->Unlock();
				}
			}
		}

		// Create the vertex data description if there is one

		if ((hResult>=0) &&
			m_uMemberCount) {

			// Anything larger than 32 entries is totally INSANE!
			D3DVERTEXELEMENT9 TempElement[32];
			D3DVERTEXELEMENT9 *pDest = TempElement;

			// Size of the description
			const uint_t *pMembers = m_pMembers;
			uint_t uCount = m_uMemberCount;
			uint_t uOffset = 0;
			do {
				uint_t uMember = pMembers[0];
				++pMembers;

				pDest->Stream = 0;
				pDest->Offset = static_cast<WORD>(uOffset);
				uint_t uType = (uMember&USAGE_CHUNKMASK)>>USAGE_CHUNKMASKSHIFT;
				uOffset += g_ChunkElementSizes[uType]*g_ChunkElementCounts[uType];
				pDest->Type = static_cast<BYTE>(g_D3DType[uType]);

				pDest->Method = D3DDECLMETHOD_DEFAULT;
				// Convert to D3D usage type
				// Note: The Burgerlib enumeration matches DirectX
				pDest->Usage = static_cast<BYTE>((uMember&USAGE_TYPEMASK)>>USAGE_TYPEMASKSHIFT);
				pDest->UsageIndex = 0;

				// Buffer overflow?
				if (++pDest>=&TempElement[32]) {
					// Force error
					hResult = -10;
					break;
				}
			} while (--uCount);

			// No error in generating the table?
			if (hResult>=0) {
				// Mark the end of the array
				pDest->Stream = 0xFF;
				pDest->Offset = 0;
				pDest->Type = D3DDECLTYPE_UNUSED;
				pDest->Method = 0;
				pDest->Usage = 0;
				pDest->UsageIndex = 0;

				// Create the declaration
				hResult = pDevice->CreateVertexDeclaration(TempElement,&m_Platform.m_DX9.m_pDescription);
			}
		}

		// Upload any index buffers
		if ((hResult>=0)&&
			m_uElementSize) {

			// Handle both 16 and 32 bit index buffers
			D3DFORMAT uFormat = D3DFMT_INDEX16;
			if (m_uFlags&FLAGAOS_ELEMENTS_32BIT) {
				uFormat = D3DFMT_INDEX32;
			}

			// Create the buffer
			hResult = pDevice->CreateIndexBuffer(static_cast<UINT>(m_uElementSize),
				D3DUSAGE_WRITEONLY,uFormat,D3DPOOL_DEFAULT,&m_Platform.m_DX9.m_pIndexBuffer,NULL);
			if (hResult>=0) {
				void *pIndices = NULL;
				hResult = m_Platform.m_DX9.m_pIndexBuffer->Lock(0,static_cast<UINT>(m_uElementSize),&pIndices,0);
				if (hResult>=0) {
					MemoryCopy(pIndices,m_pElements,m_uElementSize);
					m_Platform.m_DX9.m_pIndexBuffer->Unlock();
				}
			}
		}

		// All good! Clear the flag
		if (hResult>=0) {
			hResult = 0;
			m_uFlags &= (~FLAG_VERTEXDIRTY);
		}
	}
	// Set the error code to return
	return static_cast<uint_t>(hResult);
}

void Burger::VertexBufferDirectX9::Release(Display *)
{
	// Release the vertex buffer
	if (m_Platform.m_DX9.m_pVertexBuffer) {
		m_Platform.m_DX9.m_pVertexBuffer->Release();
		m_Platform.m_DX9.m_pVertexBuffer = NULL;
	}
	// Release the vertex entry description
	if (m_Platform.m_DX9.m_pDescription) {
		m_Platform.m_DX9.m_pDescription->Release();
		m_Platform.m_DX9.m_pDescription = NULL;
	}
	// Release the index buffer
	if (m_Platform.m_DX9.m_pIndexBuffer) {
		m_Platform.m_DX9.m_pIndexBuffer->Release();
		m_Platform.m_DX9.m_pIndexBuffer = NULL;
	}
	m_uFlags |= FLAG_VERTEXDIRTY;
}

#endif
