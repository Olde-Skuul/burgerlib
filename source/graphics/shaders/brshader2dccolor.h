/***************************************

	Simple 2D shader with color

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRSHADER2DCCOLOR_H__
#define __BRSHADER2DCCOLOR_H__

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
	}; \
	void BURGER_API SetPosition(float fX,float fY,float fWidth,float fHeight); \
	void BURGER_API SetPosition(const Vector4D_t *pPosition); \
	void BURGER_API SetColor(float fRed,float fGreen,float fBlue,float fAlpha); \
	void BURGER_API SetColor(const Vector4D_t *pColor)

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class Shader2DCColorDX9 : public EffectDX9 {
public:
	Shader2DCColorDX9(DisplayDirectX9 *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#if defined(BURGER_OPENGL_SUPPORTED)
class Shader2DCColorOpenGL : public EffectOpenGL {
protected:
	Int m_iEffect2DPosition;	///< Index for the position
	Int m_iEffect2DColor;		///< Index for the color
public:
	Shader2DCColorOpenGL(Display *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
class Shader2DCColor : public Effect {
public:
	Shader2DCColor(Display *pDisplay,const Word *pVertexMembers);
	BURGER_CLASSBODY();
};
#endif
#undef BURGER_CLASSBODY
extern const Word g_Shader2DCColorMemberDescription[];
extern const VertexBuffer::VertexAoS_t g_Shader2DCColorDefaultVertexBufferDescription;

#if defined(BURGER_WINDOWS)
typedef Shader2DCColorDX9 Shader2DCColor;
#elif defined(BURGER_OPENGL_SUPPORTED)
typedef Shader2DCColorOpenGL Shader2DCColor;
#endif
}
/* END */

#endif
