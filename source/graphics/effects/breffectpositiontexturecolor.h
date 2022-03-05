/***************************************

    Position textured color shader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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

class EffectPositionColorTexture : public Effect {

    BURGER_DISABLE_COPY(EffectPositionColorTexture);
	BURGER_RTTI_IN_CLASS();

protected:
#if defined(BURGER_OPENGL) || defined(DOXYGEN)
	int_t m_iEffectMatrix;		///< (OpenGL only) Index for the Matrix
	int_t m_iEffectColor;			///< (OpenGL only) Index for the color adjustment
#endif

public:
	EffectPositionColorTexture(Display *pDisplay,const uint_t *pVertexMembers);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	virtual void SetProjection(const Matrix4D_t *pMatrix) = 0;
	virtual void SetColor(const Vector4D_t *pColor) = 0;
#else
	virtual uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void BURGER_API SetProjection(const Matrix4D_t *pMatrix);
	void BURGER_API SetColor(const Vector4D_t *pColor);
#endif
};

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class EffectPositionColorTextureDX9 : public EffectPositionColorTexture {
    BURGER_DISABLE_COPY(EffectPositionColorTextureDX9);
	BURGER_RTTI_IN_CLASS();
public:
	EffectPositionColorTextureDX9(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetProjection(const Matrix4D_t *pMatrix) BURGER_OVERRIDE;
	void SetColor(const Vector4D_t *pColor) BURGER_OVERRIDE;
};

class EffectPositionColorTextureOpenGL : public EffectPositionColorTexture {
    BURGER_DISABLE_COPY(EffectPositionColorTextureOpenGL);
	BURGER_RTTI_IN_CLASS();
public:
	EffectPositionColorTextureOpenGL(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetProjection(const Matrix4D_t *pMatrix) BURGER_OVERRIDE;
	void SetColor(const Vector4D_t *pColor) BURGER_OVERRIDE;
};
#endif

}
/* END */

#endif
