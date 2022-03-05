/***************************************

    Simple 2D texturing shader with color per vertex

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSHADER2DCOLORVERTEX_H__
#define __BRSHADER2DCOLORVERTEX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
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

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {

class Shader2DColorVertex : public Effect {

    BURGER_DISABLE_COPY(Shader2DColorVertex);
	BURGER_RTTI_IN_CLASS();

protected:
#if defined(BURGER_OPENGL) || defined(DOXYGEN)
	int_t m_iEffect2DPosition;	///< (OpenGL only) Index for the position
#endif

public:
	struct Vertex {
		float m_fX,m_fY;
		float m_fU,m_fV;
		RGBAWord8_t m_Color;
	};

	Shader2DColorVertex(Display *pDisplay,const uint_t *pVertexMembers);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	virtual void SetPosition(float fX,float fY,float fWidth,float fHeight) = 0;
	virtual void SetPosition(const Vector4D_t *pPosition) = 0;
#else
	virtual uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void BURGER_API SetPosition(float fX,float fY,float fWidth,float fHeight);
	void BURGER_API SetPosition(const Vector4D_t *pPosition);
#endif
};

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class Shader2DColorVertexDX9 : public Shader2DColorVertex {
    BURGER_DISABLE_COPY(Shader2DColorVertexDX9);
	BURGER_RTTI_IN_CLASS();
public:
	Shader2DColorVertexDX9(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetPosition(float fX,float fY,float fWidth,float fHeight) BURGER_OVERRIDE;
	void SetPosition(const Vector4D_t *pPosition) BURGER_OVERRIDE;
};

class Shader2DColorVertexOpenGL : public Shader2DColorVertex {
    BURGER_DISABLE_COPY(Shader2DColorVertexOpenGL);
	BURGER_RTTI_IN_CLASS();
protected:
	int_t m_iEffect2DPosition;	///< Index for the position
public:
	Shader2DColorVertexOpenGL(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetPosition(float fX,float fY,float fWidth,float fHeight) BURGER_OVERRIDE;
	void SetPosition(const Vector4D_t *pPosition) BURGER_OVERRIDE;
};
#endif

extern const uint_t g_Shader2DColorVertexMemberDescription[];
extern const VertexBuffer::VertexAoS_t g_Shader2DColorVertexDefaultVertexBufferDescription;

}
/* END */

#endif
