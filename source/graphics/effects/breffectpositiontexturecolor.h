/***************************************

	Position textured color shader

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BREFFECTPOSITIONTEXTURECOLOR_H__
#define __BREFFECTPOSITIONTEXTURECOLOR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMATRIX4D_H__
#include "brmatrix4d.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BREFFECT_H__
#include "breffect.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRDISPLAYDIRECTX9_H__)
#include "brdisplaydirectx9.h"
#endif

#ifndef __BRDISPLAYOPENGL_H__
#include "brdisplayopengl.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class EffectPositionColorTextureDX9 : public EffectDX9 {
	BURGER_RTTI_IN_CLASS();
public:
	EffectPositionColorTextureDX9(DisplayDirectX9 *pDisplay,const Word *pVertexMembers);
	void BURGER_API SetProjection(const Matrix4D_t *pMatrix);
	void BURGER_API SetColor(const Vector4D_t *pColor);
};
#endif
#if defined(BURGER_OPENGL_SUPPORTED)
class EffectPositionColorTextureOpenGL : public EffectOpenGL {
	BURGER_RTTI_IN_CLASS();
protected:
	Int m_iEffectMatrix;		///< Index for the Matrix
	Int m_iEffectColor;			///< Index for the color adjustment
public:
	EffectPositionColorTextureOpenGL(Display *pDisplay,const Word *pVertexMembers);
	void BURGER_API SetProjection(const Matrix4D_t *pMatrix);
	void BURGER_API SetColor(const Vector4D_t *pColor);
};
#endif
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
class EffectPositionColorTexture : public Effect {
	BURGER_RTTI_IN_CLASS();
public:
	EffectPositionColorTexture(Display *pDisplay,const Word *pVertexMembers);
	void BURGER_API SetProjection(const Matrix4D_t *pMatrix);
	void BURGER_API SetColor(const Vector4D_t *pColor);
};
#endif

#if defined(BURGER_WINDOWS)
typedef EffectPositionColorTextureDX9 EffectPositionColorTexture;
#elif defined(BURGER_OPENGL_SUPPORTED)
typedef EffectPositionColorTextureOpenGL EffectPositionColorTexture;
#endif
}
/* END */

#endif
