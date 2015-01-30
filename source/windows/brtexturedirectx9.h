/***************************************

	Texture for rendering class, DirectX9 version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTEXTUREDIRECTX9_H__
#define __BRTEXTUREDIRECTX9_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRIMAGE_H__
#include "brimage.h"
#endif

#ifndef __BRTEXTURE_H__
#include "brtexture.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class TextureDirectX9 : public Texture {
	BURGER_RTTI_IN_CLASS();
protected:
	IDirect3DTexture9 *m_pD3DTexture;
public:
	TextureDirectX9();
	TextureDirectX9(eWrapping uWrapping,eFilter uFilter);
	virtual ~TextureDirectX9();
	virtual Word Bind(Display *pDisplay);
	virtual void Release(Display *pDisplay);
	Word BURGER_API GetD3DFormat(void) const;
	BURGER_INLINE IDirect3DTexture9 * GetTexture(void) const { return m_pD3DTexture; }
};
#endif
}
/* END */
#endif
