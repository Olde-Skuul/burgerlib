/***************************************

	Vertex buffer class for DirectX9

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbufferdirectx9.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "brdisplaydirectx9.h"
#include <windows.h>
#include <d3dx9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBufferDirectX9,Burger::VertexBuffer);

Burger::VertexBufferDirectX9::VertexBufferDirectX9() :
	m_pVertexBuffer(NULL),
	m_pDescription(NULL),
	m_uStride(0),
	m_uArrayEntryCount(0)
{
}

Burger::VertexBufferDirectX9::~VertexBufferDirectX9()
{
	VertexBufferDirectX9::ReleaseData();
}

Word Burger::VertexBufferDirectX9::LoadData(Display *pDisplay,const VertexAoS_t *pDescription)
{
	VertexBufferDirectX9::ReleaseData();
	Word uResult = 0;
	if (pDescription) {
		uResult = 10;
		IDirect3DDevice9 *pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();

		HRESULT hr = pDevice->CreateVertexBuffer(static_cast<UINT>(pDescription->m_uVertexArraySize),D3DUSAGE_WRITEONLY,
			0,D3DPOOL_MANAGED,&m_pVertexBuffer,NULL);
		if (hr==D3D_OK) {
			void *pVertices = NULL;
			hr = m_pVertexBuffer->Lock(0,static_cast<UINT>(pDescription->m_uVertexArraySize),&pVertices,0);
			if (hr==D3D_OK) {
				MemoryCopy(pVertices,pDescription->m_pVertexArray,pDescription->m_uVertexArraySize);
				m_pVertexBuffer->Unlock();

				//
				// Get the number of members in the array of structures
				//

				const Word *pMembers = pDescription->m_pMembers;

				// It's not an error to have no declaration
				uResult = 0;
				if (pMembers) {
					Word uMember = pMembers[0];
					if (uMember) {
					
						// Anything larger than 32 entries is totally INSANE!

						D3DVERTEXELEMENT9 TempElement[32];
						D3DVERTEXELEMENT9 *pDest = TempElement;
						Word uOffset = 0;
						do {
							pDest->Stream = 0;
							pDest->Offset = static_cast<WORD>(uOffset);
							Word uType = (uMember&0x30);
							Word uEntryCount = (uMember&USAGE_COUNTMASK);
							if (uType==USAGE_FLOAT) {
								BURGER_ASSERT(uEntryCount>=1 && uEntryCount<5);
								uOffset += uEntryCount*4;
								pDest->Type = static_cast<BYTE>((D3DDECLTYPE_FLOAT1-1)+uEntryCount);
							} else {
								BURGER_ASSERT(uType!=USAGE_FLOAT);
								uResult = 10;
								break;
							}
							pDest->Method = D3DDECLMETHOD_DEFAULT;
							// Convert to D3D usage type
							pDest->Usage = static_cast<BYTE>((uMember>>8)&0xF);
							pDest->UsageIndex = 0;
							++pMembers;

							// Buffer overflow?
							if (++pDest>=&TempElement[32]) {
								// Force error
								uResult = 10;
								break;
							}
							uMember = pMembers[0];
						} while (uMember);
						m_uStride = uOffset;
						m_uArrayEntryCount = static_cast<Word>(pDescription->m_uVertexArraySize/uOffset);
						if (!uResult) {
							// Mark the end of the array
							pDest->Stream = 0xFF;
							pDest->Offset = 0;
							pDest->Type = D3DDECLTYPE_UNUSED;
							pDest->Method = 0;
							pDest->Usage = 0;
							pDest->UsageIndex = 0;
	
							hr = pDevice->CreateVertexDeclaration(TempElement,&m_pDescription);
							if (hr!=D3D_OK) {
								uResult = static_cast<Word>(hr);
							}
						}
					}
				}
			} else {
				uResult = static_cast<Word>(hr);
			}
		} else {
			uResult = static_cast<Word>(hr);
		}
	}
	return uResult;
}

void Burger::VertexBufferDirectX9::ReleaseData(void)
{
	m_uStride = 0;
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if (m_pDescription) {
		m_pDescription->Release();
		m_pDescription = NULL;
	}
}

#endif
