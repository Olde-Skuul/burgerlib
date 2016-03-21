/***************************************

	Vertex buffer class for OpenGL on Windows

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVERTEXBUFFEROPENGL_H__
#define __BRVERTEXBUFFEROPENGL_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRVERTEXBUFFER_H__
#include "brvertexbuffer.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS)
class VertexBufferOpenGL : public VertexBuffer {
	BURGER_RTTI_IN_CLASS();
protected:
	Word m_uVertexArrayObjectID;	///< Main Vertex Object Array
	Word m_uElementID;				///< ID to the index array
	Word m_uBufferID;				///< ID for the buffer
	Word m_uArrayEntryCount;		///< Number of entries in the array
	Word m_uElementEntryCount;		///< Number of entries in the element array
public:
	VertexBufferOpenGL();
	virtual ~VertexBufferOpenGL();
	Word LoadData(Display *pDisplay,const VertexAoS_t *pDescription);
	void ReleaseData(void);
	BURGER_INLINE Word GetVertexArrayObject(void) const { return m_uVertexArrayObjectID; }
	BURGER_INLINE Word GetElementID(void) const { return m_uElementID; }
	BURGER_INLINE Word GetBufferID(void) const { return m_uBufferID; }
	BURGER_INLINE Word GetArrayEntryCount(void) const { return m_uArrayEntryCount; }
	BURGER_INLINE Word GetElementEntryCount(void) const { return m_uElementEntryCount; }
};
#endif
}
/* END */
#endif
