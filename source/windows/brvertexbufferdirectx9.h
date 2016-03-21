/***************************************

	Vertex buffer class for DirectX9

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVERTEXBUFFERDIRECTX9_H__
#define __BRVERTEXBUFFERDIRECTX9_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRVERTEXBUFFER_H__
#include "brvertexbuffer.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class VertexBufferDirectX9 : public VertexBuffer {
	BURGER_RTTI_IN_CLASS();
protected:
	IDirect3DVertexBuffer9 * m_pVertexBuffer;		///< DirectX vertex buffer
	IDirect3DVertexDeclaration9 *m_pDescription;	///< DirectX vertex array description
	Word m_uStride;									///< Size in bytes of each entry in the array
	Word m_uArrayEntryCount;						///< Number of entries in the array
public:
	VertexBufferDirectX9();
	virtual ~VertexBufferDirectX9();
	virtual Word LoadData(Display *pDisplay,const VertexAoS_t *pDescription);
	virtual void ReleaseData(void);
	BURGER_INLINE IDirect3DVertexBuffer9 *GetDX9VertexBuffer(void) const { return m_pVertexBuffer; }
	BURGER_INLINE IDirect3DVertexDeclaration9 *GetDX9VertexDescription(void) const { return m_pDescription; }
	BURGER_INLINE Word GetStride(void) const { return m_uStride; }
	BURGER_INLINE Word GetArrayEntryCount(void) const { return m_uArrayEntryCount; }
};
#endif
}
/* END */
#endif
