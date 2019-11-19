/***************************************

	Vertex buffer class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbuffer.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBuffer,Burger::DisplayObject);
#endif

const Word Burger::VertexBuffer::g_ChunkElementSizes[5] = {4,4,4,4,1};
const Word Burger::VertexBuffer::g_ChunkElementCounts[5] = {1,2,3,4,4};

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty vertex buffer

***************************************/

Burger::VertexBuffer::VertexBuffer() :
	m_pVertexArray(NULL),
	m_uVertexArraySize(0),
	m_pElements(NULL),
	m_uElementSize(0),
	m_pMembers(NULL),

#if defined(BURGER_XBOX360)
	m_pD3DVertexBuffer(NULL),
	m_pD3DVertexDeclaration(NULL),
	m_pD3DIndexBuffer(NULL),
#endif

	m_uStride(0),
	m_uArrayEntryCount(0),
	m_uElementEntryCount(0),
	m_uMemberCount(0),
	m_uFlags(FLAG_VERTEXDIRTY)
{
#if defined(BURGER_WINDOWS) || defined(BURGER_OPENGL)
	MemoryClear(&m_Platform,sizeof(m_Platform));
#endif
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::VertexBuffer::~VertexBuffer()
{
#if !defined(BURGER_WINDOWS)
	Release(NULL);
#endif
	// Make sure the buffers are cleared
	PurgeData();
}

/*! ************************************

	\brief Load vertex data to VRAM

	Given a pointer to the display to render to and a structure
	describing the vertex data, upload the data to VRAM
	and retain information to allow rendering with
	graphics primitives

	\param pDisplay Pointer to the current display
	\param pDescription Pointer to an initialized structure that describes the data to upload to VRAM.
	\sa Release(Display *)

***************************************/

Word BURGER_API Burger::VertexBuffer::LoadData(Display *pDisplay,const VertexAoS_t *pDescription)
{
	// Make sure the buffers are cleared
	PurgeData();

	// Get the flags
	Word uFlags = pDescription->m_uFlags;

	// Copy the vertex buffer if needed

	const void *pBuffer;
	WordPtr uSize = pDescription->m_uVertexArraySize;
	if (!uSize) {
		// No buffer due to lack of size
		pBuffer = NULL;
	} else {
		// Get the default buffer
		pBuffer = pDescription->m_pVertexArray;
		if (!(uFlags&FLAGAOS_DONTCOPY_VERTICES)) {

			// If pBuffer is NULL, it will act as Alloc()
			pBuffer = AllocCopy(pBuffer,uSize);
			BURGER_ASSERT(pBuffer);
			if (!pBuffer) {
				uSize = 0;
			}
		}
	}
	// Set the final buffer and size
	m_pVertexArray = pBuffer;
	m_uVertexArraySize = uSize;

	// Copy the index buffer if needed

	uSize = pDescription->m_uElementSize;
	if (!uSize) {
		// No buffer due to lack of size
		pBuffer = NULL;
	} else {

		// Get the default buffer
		pBuffer = pDescription->m_pElements;
		if (!(uFlags&FLAGAOS_DONTCOPY_ELEMENTS)) {

			// If pBuffer is NULL, it will act as Alloc()
			pBuffer = AllocCopy(pBuffer,uSize);
			BURGER_ASSERT(pBuffer);
			if (!pBuffer) {
				uSize = 0;
			}
		}
	}
	// Set the final element buffer and size
	m_pElements = pBuffer;
	m_uElementSize = uSize;

	// Convert the indicies from bytes to shorts
	uSize >>= 1U;
	if (uFlags&FLAGAOS_ELEMENTS_32BIT) {
		// Convert the index count to 32 bit ints
		uSize >>= 1U;
	}
	m_uElementEntryCount = static_cast<Word>(uSize);

	// Determine the buffer description

	Word uStride = 0;			// No stride yet
	Word uAttributeCount = 0;	// No attributes found
	const Word *pMembers = pDescription->m_pMembers;
	if (pMembers) {
		Word uMember = pMembers[0];
		if (uMember!=VertexBuffer::USAGE_END) {
			do {
				Word uType = (uMember&USAGE_CHUNKMASK)>>USAGE_CHUNKMASKSHIFT;
				if (uType>=BURGER_ARRAYSIZE(g_ChunkElementSizes)) {
					BURGER_ASSERT(uType>=BURGER_ARRAYSIZE(g_ChunkElementSizes));
					// Failsafe!!
					uAttributeCount = 0;
					uStride = 0;
					break;
				}
				// Add in the byte count for the chunk
				uStride += g_ChunkElementSizes[uType]*g_ChunkElementCounts[uType];
				++pMembers;
				++uAttributeCount;
				uMember = pMembers[0];
			} while (uMember!=VertexBuffer::USAGE_END);
		}
	}

	// Save the bytes per vertex
	m_uStride = uStride;
	// Number of chunks
	if (uStride) {
		uStride = static_cast<Word>(m_uVertexArraySize/uStride);
	}
	// Number of vertices in the array
	m_uArrayEntryCount = uStride;

	// Copy the member buffer if needed
	if (!uAttributeCount) {
		// No buffer due to lack of size
		pBuffer = NULL;
	} else {
		// Get the default buffer
		pBuffer = pDescription->m_pMembers;
		if (!(uFlags&FLAGAOS_DONTCOPY_MEMBERS)) {

			// If pBuffer is NULL, it will act as Alloc()
			pBuffer = AllocCopy(pBuffer,uAttributeCount*sizeof(Word));
			BURGER_ASSERT(pBuffer);
			if (!pBuffer) {
				uAttributeCount = 0;
			}
		}
	}
	// Set the final element buffer and size
	m_pMembers = static_cast<const Word *>(pBuffer);
	m_uMemberCount = uAttributeCount;


	// Update the flags data
	m_uFlags = uFlags|FLAG_VERTEXDIRTY;

	// If there was a Display record, upload the vertex information 
	// to VRAM now.

	Word uResult = 0;
	if (pDisplay) {
		uResult = CheckLoad(pDisplay);
	}
	return uResult;
}

/*! ************************************

	\brief Release allocated memory buffers

	Clear out an allocated memory arrays from system
	memory. This function does not clear out platform
	specific data.

	\sa LoadData(const VertexAoS_t *)

***************************************/

void BURGER_API Burger::VertexBuffer::PurgeData(void)
{
	// Only erase buffers that have been allocated
	if (!(m_uFlags&FLAGAOS_DONTCOPY_VERTICES)) {
		Free(m_pVertexArray);
	}
	m_pVertexArray = NULL;
	m_uVertexArraySize = 0;

	// Elements were copied?
	if (!(m_uFlags&FLAGAOS_DONTCOPY_ELEMENTS)) {
		Free(m_pElements);
	}
	m_pElements = NULL;
	m_uElementSize = 0;

	// Members were copies?
	if (!(m_uFlags&FLAGAOS_DONTCOPY_MEMBERS)) {
		Free(m_pMembers);
	}
	m_pMembers = NULL;
	m_uMemberCount = 0;
	m_uFlags |= FLAG_VERTEXDIRTY;
}

#if !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_OPENGL)) || defined(DOXYGEN)

/*! ************************************

	\brief Upload vertex data from VRAM

	If any data needed to be uploaded to VRAM, upload it.

	\param pDisplay Pointer to the parent display device
	\return Zero on success, non-zero on failure
	\sa Release(Display *)

***************************************/

Word Burger::VertexBuffer::CheckLoad(Display *)
{
	// Always return an error
	return 10;
}

/*! ************************************

	\brief Release vertex data from VRAM

	If any data was uploaded to VRAM, release it.

	\param pDisplay Pointer to the parent display device
	\sa CheckLoad(Display *)

***************************************/

void Burger::VertexBuffer::Release(Display *)
{
}

#endif