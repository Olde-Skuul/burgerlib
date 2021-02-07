/***************************************

    Vertex buffer class for OpenGL on Windows

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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

#if defined(BURGER_WINDOWS) && defined(BURGER_INTEL)

class VertexBufferOpenGL : public VertexBuffer {
    BURGER_DISABLE_COPY(VertexBufferOpenGL);
	BURGER_RTTI_IN_CLASS();
public:
	VertexBufferOpenGL();
	virtual ~VertexBufferOpenGL();
	Word CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
};
#endif
}
/* END */
#endif
