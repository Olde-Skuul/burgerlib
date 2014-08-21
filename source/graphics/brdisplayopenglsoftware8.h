/***************************************

	8 Bit software renderer on top of OpenGL

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYOPENGLSOFTWARE8_H__
#define __BRDISPLAYOPENGLSOFTWARE8_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRDISPLAYOPENGL_H__
#include "brdisplayopengl.h"
#endif

#ifndef __BRRENDERERSOFTWARE8_H__
#include "brrenderersoftware8.h"
#endif

/* BEGIN */
namespace Burger {
class DisplayOpenGLSoftware8 : public DisplayOpenGL {
public:
	DisplayOpenGLSoftware8(GameApp *pGameApp);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
private:
	void *m_pBitMap;				///< Pointer the the HBITMAP's bitmap
	RendererSoftware8 m_Renderer;	///< Software renderer context
	Word m_uBitMapTexture;			///< OpenGL Texture for the bitmap
	Word m_uPaletteTexture;			///< OpenGL Texture for the palette
	Word m_uProgram;				///< OpenGL shader program
	Word m_uVertexShader;			///< OpenGL vertex shader subroutine
	Word m_uFragmentShader;			///< OpenGL fragment shader subroutine
	Word m_uvPositionHandle;		///< OpenGL shader handle for shader vertexes
	Word m_uUVHandle;				///< OpenGL shader handle for texture UV coordinates
};
}
/* END */

#endif
