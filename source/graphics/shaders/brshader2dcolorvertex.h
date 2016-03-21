/***************************************

	Simple 2D texturing shader with color per vertex

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
#define BURGER_CLASSBODY() \
	BURGER_RTTI_IN_CLASS(); \
public: \
	struct Vertex { \
		float m_fX,m_fY; \
		float m_fU,m_fV; \
		Vector4D_t m_fColor; \
	}; \
	void BURGER_API SetPosition(float fX,float fY,float fWidth,float fHeight); \
	void BURGER_API SetPosition(const Vector4D_t *pPosition)

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class Shader2DColorVertexDX9 : public EffectDX9 {
public:
	Shader2DColorVertexDX9(DisplayDirectX9 *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#if defined(BURGER_OPENGL_SUPPORTED)
class Shader2DColorVertexOpenGL : public EffectOpenGL {
protected:
	Int m_iEffect2DPosition;	///< Index for the position
public:
	Shader2DColorVertexOpenGL(Display *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
class Shader2DColorVertex : public Effect {
public:
	Shader2DColorVertex(Display *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#undef BURGER_CLASSBODY
extern const Word g_Shader2DColorVertexMemberDescription[];
extern const VertexBuffer::VertexAoS_t g_Shader2DColorVertexDefaultVertexBufferDescription;

#if defined(BURGER_WINDOWS)
typedef Shader2DColorVertexDX9 Shader2DColorVertex;
#elif defined(BURGER_OPENGL_SUPPORTED)
typedef Shader2DColorVertexOpenGL Shader2DColorVertex;
#endif
}
/* END */

#endif
