/***************************************

    Vertex buffer class for DirectX9

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
    BURGER_DISABLE_COPY(VertexBufferDirectX9);
	BURGER_RTTI_IN_CLASS();
public:
	VertexBufferDirectX9();
	virtual ~VertexBufferDirectX9();
	Word CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
};
#endif
}
/* END */
#endif
