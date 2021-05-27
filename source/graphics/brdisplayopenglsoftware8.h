/***************************************

    8 Bit software renderer on top of OpenGL

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
#if defined(BURGER_OPENGL)
#if defined(BURGER_WINDOWS)
class DisplayOpenGLSoftware8 : public DisplayOpenGL {
#else
class DisplayOpenGLSoftware8 : public Display {
#endif
	void *m_pBitMap;				///< Pointer the the HBITMAP's bitmap
	RendererSoftware8 m_Renderer;	///< Software renderer context
	uint_t m_uBitMapTexture;			///< OpenGL Texture for the bitmap
	uint_t m_uPaletteTexture;			///< OpenGL Texture for the palette
	uint_t m_uProgram;				///< OpenGL shader program
	uint_t m_uVertexShader;			///< OpenGL vertex shader subroutine
	uint_t m_uFragmentShader;			///< OpenGL fragment shader subroutine
	uint_t m_uvPositionHandle;		///< OpenGL shader handle for shader vertexes
	uint_t m_uUVHandle;				///< OpenGL shader handle for texture UV coordinates
	BURGER_RTTI_IN_CLASS();
public:
	DisplayOpenGLSoftware8(GameApp *pGameApp);
#if defined(BURGER_WINDOWS)
	uint_t Init(uint_t uWidth,uint_t uHeight,uint_t uDepth=8,uint_t uFlags=DEFAULTFLAGS) BURGER_OVERRIDE;
	void Shutdown(void) BURGER_OVERRIDE;
	void BeginScene(void) BURGER_OVERRIDE;
	void EndScene(void) BURGER_OVERRIDE;
#else
	uint_t Init(uint_t uWidth, uint_t uHeight, uint_t uDepth = 8, uint_t uFlags = DEFAULTFLAGS);
	void Shutdown(void);
	void BeginScene(void);
	void EndScene(void);
#endif
	BURGER_INLINE RendererSoftware8 *GetRenderer(void) { return &m_Renderer; }
};
#endif
}
/* END */

#endif
