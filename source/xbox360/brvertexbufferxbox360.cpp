/***************************************

	Vertex buffer class for Xbox 360

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbuffer.h"
#if defined(BURGER_XBOX360) || defined(DOXYGEN)

#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "brdisplay.h"
#define NONET
#include <xtl.h>

//
// Upload vertex data onto the video hardware
//

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

Word Burger::VertexBuffer::CheckLoad(Display *pDisplay)
{
	HRESULT hResult = D3D_OK;
	if (m_uFlags & FLAG_VERTEXDIRTY) {
		VertexBuffer::Release(NULL);

		D3DDevice *pDevice = pDisplay->GetD3DDevice();

		if (m_uVertexArraySize) {

			// Create a vertex buffer object 

			hResult = pDevice->CreateVertexBuffer(static_cast<UINT>(m_uVertexArraySize),0,
				0,D3DPOOL_DEFAULT,&m_pD3DVertexBuffer,NULL);

			// If there's data to initialize with, upload it.

			if ((hResult>=0)&&
				m_pVertexArray) {
				void *pVertices = NULL;
				hResult = m_pD3DVertexBuffer->Lock(0,static_cast<UINT>(m_uVertexArraySize),&pVertices,0);
				if (hResult>=0) {
					MemoryCopy(pVertices,m_pVertexArray,m_uVertexArraySize);
					m_pD3DVertexBuffer->Unlock();
				}
			}

			// Create the vertex data description if there is one

			if ((hResult>=0) &&
				m_uMemberCount) {

				// Anything larger than 32 entries is totally INSANE!
				D3DVERTEXELEMENT9 TempElement[32];
				D3DVERTEXELEMENT9 *pDest = TempElement;

				// Size of the description
				const Word *pMembers = m_pMembers;
				Word uCount = m_uMemberCount;
				Word uOffset = 0;
				do {
					Word uMember = pMembers[0];
					++pMembers;

					pDest->Stream = 0;
					pDest->Offset = static_cast<WORD>(uOffset);
					Word uType = (uMember&USAGE_CHUNKMASK)>>USAGE_CHUNKMASKSHIFT;
					uOffset += g_ChunkElementSizes[uType]*g_ChunkElementCounts[uType];
					pDest->Type = g_D3DType[uType];

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
					pDest->Type = static_cast<DWORD>(D3DDECLTYPE_UNUSED);
					pDest->Method = 0;
					pDest->Usage = 0;
					pDest->UsageIndex = 0;

					// Create the declaration
					hResult = pDevice->CreateVertexDeclaration(TempElement,&m_pD3DVertexDeclaration);
				}
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
				0,uFormat,D3DPOOL_DEFAULT,&m_pD3DIndexBuffer,NULL);
			if (hResult>=0) {
				void *pIndices = NULL;
				hResult = m_pD3DIndexBuffer->Lock(0,static_cast<UINT>(m_uElementSize),&pIndices,0);
				if (hResult>=0) {
					MemoryCopy(pIndices,m_pElements,m_uElementSize);
					m_pD3DIndexBuffer->Unlock();
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
	return static_cast<Word>(hResult);
}

//
// Release the buffer
//

void Burger::VertexBuffer::Release(Display *)
{
	D3DVertexBuffer *pVertexBuffer = m_pD3DVertexBuffer;
	if (pVertexBuffer) {
		// Make sure there is no connection to a D3D device
		IDirect3DDevice9 *pDevice;
		pVertexBuffer->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetStreamSource(0,0,0,0);
		}
		pVertexBuffer->Release();
		m_pD3DVertexBuffer = NULL;
	}
	D3DVertexDeclaration *pDeclaration = m_pD3DVertexDeclaration; 
	if (pDeclaration) {
		// Make sure there is no connection to a D3D device
		IDirect3DDevice9 *pDevice;
		pDeclaration->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetVertexDeclaration(0);
		}
		pDeclaration->Release();
		m_pD3DVertexDeclaration = NULL;
	}

	D3DIndexBuffer *pIndexBuffer = m_pD3DIndexBuffer; 
	if (pIndexBuffer) {
		// Make sure there is no connection to a D3D device
		IDirect3DDevice9 *pDevice;
		pIndexBuffer->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetIndices(NULL);
		}
		pIndexBuffer->Release();
		m_pD3DIndexBuffer = NULL;
	}	
}

#endif
