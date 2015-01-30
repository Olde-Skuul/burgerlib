/***************************************

	Texture for rendering class, OpenGL version

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTEXTUREOPENGL_H__
#define __BRTEXTUREOPENGL_H__

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

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS)
class TextureOpenGL : public Texture {
	BURGER_RTTI_IN_CLASS();
protected:
	Word m_uTextureID;			///< OpenGL Texture ID
public:
	TextureOpenGL();
	TextureOpenGL(eWrapping uWrapping,eFilter uFilter);
	virtual ~TextureOpenGL();
	virtual Word Bind(Display *pDisplay);
	virtual void Release(Display *pDisplay);	
	static int GetWrapping(eWrapping uWrapping);
	static int GetFilter(eFilter uFilter);
	BURGER_INLINE Word GetTextureID(void) const { return m_uTextureID; }
};
#endif
}
/* END */
#endif
