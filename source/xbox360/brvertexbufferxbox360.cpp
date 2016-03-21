/***************************************

	Vertex buffer class for Xbox 360

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

static const DWORD g_D3DDECLTYPE[4] = {
	D3DDECLTYPE_FLOAT1,D3DDECLTYPE_FLOAT2,D3DDECLTYPE_FLOAT3,D3DDECLTYPE_FLOAT4
};

//
// Initialize the D3D Vertex buffer
//

Burger::VertexBuffer::VertexBuffer() :
	m_pD3DVertexBuffer(NULL),
	m_pD3DVertexDeclaration(NULL),
	m_uStride(0),
	m_uArrayEntryCount(0)
{
}

//
// Release the vertex buffer data and exit
//

Burger::VertexBuffer::~VertexBuffer()
{
	ReleaseData();
}

//
// Upload vertex data onto the video hardware
//

Word Burger::VertexBuffer::LoadData(Display *pDisplay,const VertexAoS_t *pDescription)
{
	// If there was any data, release it to
	// replace it with the new vertex data

	ReleaseData();

	// No error if the description is NULL
	HRESULT uResult = 0;
	if (pDescription) {
		D3DDevice *pDevice = pDisplay->GetD3DDevice();

		// Allocate memory for the vertex array
		uResult = pDevice->CreateVertexBuffer(static_cast<UINT>(pDescription->m_uVertexArraySize),D3DUSAGE_WRITEONLY,
			0,D3DPOOL_DEFAULT,&m_pD3DVertexBuffer,NULL);

		// Success?
		if (uResult==D3D_OK) {
			void *pVertices;
			// Access a buffer to upload vertex data
			uResult = m_pD3DVertexBuffer->Lock(0,static_cast<UINT>(pDescription->m_uVertexArraySize),&pVertices,0);
			if (uResult==D3D_OK) {
				// Copy it up
				MemoryCopy(pVertices,pDescription->m_pVertexArray,pDescription->m_uVertexArraySize);
				m_pD3DVertexBuffer->Unlock();

				//
				// Get the number of members in the array of structures
				//

				const Word *pMembers = pDescription->m_pMembers;

				// It's not an error to have no declaration
				if (pMembers) {

					// Create a description of the buffer for 
					Word uMember = pMembers[0];
					if (uMember) {
					
						// Anything larger than 32 entries is totally INSANE!

						D3DVERTEXELEMENT9 TempElement[32];
						D3DVERTEXELEMENT9 *pDest = TempElement;
						Word uOffset = 0;
						do {
							pDest->Stream = 0;
							pDest->Offset = static_cast<WORD>(uOffset);
							Word uChunk = (uMember&USAGE_CHUNKMASK);
							Word uEntryCount = (uMember&USAGE_COUNTMASK);
							if (uChunk==USAGE_FLOAT) {
								BURGER_ASSERT(uEntryCount>=1 && uEntryCount<5);
								uOffset += uEntryCount*4;
								pDest->Type = g_D3DDECLTYPE[uEntryCount-1];
							} else {
								// Short is not supported yet
								BURGER_ASSERT(uChunk!=USAGE_FLOAT);
								uResult = 10;
								break;
							}
							pDest->Method = D3DDECLMETHOD_DEFAULT;
							// Convert to D3D usage type
							// Note: The Burgerlib enumeration matches DirectX

							pDest->Usage = static_cast<BYTE>((uMember>>8)&0xF);
							pDest->UsageIndex = 0;
							++pMembers;

							// Buffer overflow?
							if (++pDest>=&TempElement[32]) {
								// Force error and abort immediately via truncation
								uResult = 10;
								break;
							}
							uMember = pMembers[0];
						} while (uMember);

						// Store the size in bytes of each element in the array
						m_uStride = uOffset;
						// Store the number of elements in the array
						m_uArrayEntryCount = static_cast<Word>(pDescription->m_uVertexArraySize/uOffset);

						if (!uResult) {
							// Mark the end of the array
							pDest->Stream = 0xFF;
							pDest->Offset = 0;
							pDest->Type = static_cast<DWORD>(D3DDECLTYPE_UNUSED);
							pDest->Method = 0;
							pDest->Usage = 0;
							pDest->UsageIndex = 0;
							
							// Create the declaration

							uResult = pDevice->CreateVertexDeclaration(TempElement,&m_pD3DVertexDeclaration);
						}
					}
				}
			}
		}
	}
	return static_cast<Word>(uResult);
}

//
// Release the buffer
//

void Burger::VertexBuffer::ReleaseData(void)
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
}

#endif
