
/***************************************

    Simple 2D shader with color

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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

class Shader2DCColor : public Effect {

    BURGER_DISABLE_COPY(Shader2DCColor);
	BURGER_RTTI_IN_CLASS();

protected:
#if defined(BURGER_OPENGL) || defined(DOXYGEN)
	int_t m_iEffect2DPosition;	///< (OpenGL only) Index for the position
	int_t m_iEffect2DColor;		///< (OpenGL only) Index for the color
#endif

public:
	struct Vertex {
		float m_fX,m_fY;
	};
		
	Shader2DCColor(Display *pDisplay,const uint_t *pVertexMembers);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	virtual void SetPosition(float fX,float fY,float fWidth,float fHeight) = 0;
	virtual void SetPosition(const Vector4D_t *pPosition) = 0;
	virtual void SetColor(float fRed,float fGreen,float fBlue,float fAlpha) = 0;
	virtual void SetColor(const Vector4D_t *pColor) = 0;
#else
	virtual uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void BURGER_API SetPosition(float fX,float fY,float fWidth,float fHeight);
	void BURGER_API SetPosition(const Vector4D_t *pPosition);
	void BURGER_API SetColor(float fRed,float fGreen,float fBlue,float fAlpha);
	void BURGER_API SetColor(const Vector4D_t *pColor);
#endif
};

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class Shader2DCColorDX9 : public Shader2DCColor {
    BURGER_DISABLE_COPY(Shader2DCColorDX9);
	BURGER_RTTI_IN_CLASS();
public:
	Shader2DCColorDX9(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetPosition(float fX,float fY,float fWidth,float fHeight) BURGER_OVERRIDE;
	void SetPosition(const Vector4D_t *pPosition) BURGER_OVERRIDE;
	void SetColor(float fRed,float fGreen,float fBlue,float fAlpha) BURGER_OVERRIDE;
	void SetColor(const Vector4D_t *pColor) BURGER_OVERRIDE;
};

class Shader2DCColorOpenGL : public Shader2DCColor {
    BURGER_DISABLE_COPY(Shader2DCColorOpenGL);
	BURGER_RTTI_IN_CLASS();
public:
	Shader2DCColorOpenGL(Display *pDisplay,const uint_t *pVertexMembers);
	uint_t CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
	void SetPosition(float fX,float fY,float fWidth,float fHeight) BURGER_OVERRIDE;
	void SetPosition(const Vector4D_t *pPosition) BURGER_OVERRIDE;
	void SetColor(float fRed,float fGreen,float fBlue,float fAlpha) BURGER_OVERRIDE;
	void SetColor(const Vector4D_t *pColor) BURGER_OVERRIDE;
};
#endif

extern const uint_t g_Shader2DCColorMemberDescription[];
extern const VertexBuffer::VertexAoS_t g_Shader2DCColorDefaultVertexBufferDescription;

}
/* END */

#endif
