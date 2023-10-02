/***************************************

    Simple 2D shader with color

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brshader2dccolor.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma disable_message(15)
#endif

#include <d3d9.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

#if defined(BURGER_WATCOM)
// Disable nested comment found in comment (Direct X headers trigger this)
#pragma enable_message(15)
#endif

BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColorDX9,Burger::Shader2DCColor);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColorOpenGL,Burger::Shader2DCColor);
#endif

#if defined(BURGER_XBOX360)
#define NONET
#include <xtl.h>
#endif

#if defined(BURGER_OPENGL) && !defined(DOXYGEN)
#define __BURGER__
#include "brgl.h"
#include "brglext.h"
#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#endif
#endif

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColor,Burger::Effect);

//
// Vertexes needed for Effect2D to render an entire texture
// as a 2D shape
//

static const Burger::Shader2DCColor::Vertex g_Vertices2DQuad[] = {
	{0.0f,-2.0f},
	{2.0f,-2.0f},
	{0.0f, 0.0f},
	{2.0f, 0.0f}
};

#if defined(BURGER_OPENGL)
static const Burger::Display::OpenGLVertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_END}
};
#endif
#endif

/*! ************************************

	\brief Vertex description for Shader2DCColor
	2 entries for position
	
***************************************/

const uint_t Burger::g_Shader2DCColorMemberDescription[] = {
	Burger::VertexBuffer::USAGE_FLOAT2|Burger::VertexBuffer::USAGE_POSITION,0};

/*! ************************************

	\brief Describe the location for Shader2DCColor

***************************************/

const Burger::VertexBuffer::VertexAoS_t Burger::g_Shader2DCColorDefaultVertexBufferDescription = {
	g_Vertices2DQuad,sizeof(g_Vertices2DQuad),
	NULL,0,
	Burger::g_Shader2DCColorMemberDescription,Burger::VertexBuffer::FLAGAOS_DONTCOPY_VERTICES|Burger::VertexBuffer::FLAGAOS_DONTCOPY_MEMBERS
};

/*! ************************************

	\brief Default constructor for an Shader2DCColor
	\param pDisplay Pointer to the current display
	\param pVertexMembers Pointer to a description to the vertex entries

***************************************/

Burger::Shader2DCColor::Shader2DCColor(Display *pDisplay,const uint_t * /* pVertexMembers */)
#if defined(BURGER_OPENGL)
	: m_iEffect2DPosition(0),
	m_iEffect2DColor(0)
#endif
{
	BURGER_UNUSED(pDisplay);
}

//
// Upload data
//

#if !defined(BURGER_WINDOWS) && !defined(DOXYGEN)

uint_t Burger::Shader2DCColor::CheckLoad(Display *pDisplay)
{
#if defined(BURGER_XBOX360)
	m_pDevice = pDisplay->GetD3DDevice();
	if (!m_pVertexShader) {
		m_pVertexShader = pDisplay->CreateVertexShader(g_vs20pos22d360);
	}
	if (!m_pPixelShader) {
		m_pPixelShader = pDisplay->CreatePixelShader(g_pscclr4360);
	}

#elif defined(BURGER_OPENGL)
	BURGER_UNUSED(pDisplay);
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20pos22dgl,0,g_pscclr4gl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
			m_iEffect2DColor = glGetUniformLocation(uProgram,g_glColor);
		}
	}
#else
	BURGER_UNUSED(pDisplay);
#endif
	return 0;
}
#endif


#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)

void BURGER_API Burger::Shader2DCColor::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
#if defined(BURGER_XBOX360)
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);

#elif defined(BURGER_OPENGL)
	glUniform4f(m_iEffect2DPosition,fX,fY,fWidth,fHeight);
#else
	BURGER_UNUSED(fX);
	BURGER_UNUSED(fY);
	BURGER_UNUSED(fWidth);
	BURGER_UNUSED(fHeight);
#endif
}

void BURGER_API Burger::Shader2DCColor::SetPosition(const Vector4D_t *pPosition)
{
#if defined(BURGER_XBOX360)
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
#elif defined(BURGER_OPENGL)
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
#else
	BURGER_UNUSED(pPosition);
#endif
}

void BURGER_API Burger::Shader2DCColor::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
#if defined(BURGER_XBOX360)
	Vector4D_t Upload;
	Upload.x = fRed;
	Upload.y = fGreen;
	Upload.z = fBlue;
	Upload.w = fAlpha;
	m_pDevice->SetPixelShaderConstantF(0,&Upload.x,1);

#elif defined(BURGER_OPENGL)
	glUniform4f(m_iEffect2DColor,fRed,fGreen,fBlue,fAlpha);

#else
	BURGER_UNUSED(fRed);
	BURGER_UNUSED(fGreen);
	BURGER_UNUSED(fBlue);
	BURGER_UNUSED(fAlpha);
#endif
}

void BURGER_API Burger::Shader2DCColor::SetColor(const Vector4D_t *pColor)
{
#if defined(BURGER_XBOX360)
	m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);

#elif defined(BURGER_OPENGL)
	glUniform4fv(m_iEffect2DColor,1,&pColor->x);

#else
	BURGER_UNUSED(pColor);
#endif
}

#endif

//
// DirectX 9 version
//

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
Burger::Shader2DCColorDX9::Shader2DCColorDX9(Display *pDisplay,const uint_t *pVertexMembers) :
	Shader2DCColor(pDisplay,pVertexMembers)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
}

uint_t Burger::Shader2DCColorDX9::CheckLoad(Display *pDisplay)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
	if (!m_ShaderData.m_DX9.m_pVertexShader) {
		m_ShaderData.m_DX9.m_pVertexShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreateVertexShader(g_vs20pos22ddx9);
	}
	if (!m_ShaderData.m_DX9.m_pPixelShader) {
		m_ShaderData.m_DX9.m_pPixelShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreatePixelShader(g_pscclr4dx9);
	}
	return 0;
}

void Burger::Shader2DCColorDX9::Release(Display * /* pDisplay */)
{
	ReleaseDX9();
}

void Burger::Shader2DCColorDX9::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void Burger::Shader2DCColorDX9::SetPosition(const Vector4D_t *pPosition)
{
	m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

void Burger::Shader2DCColorDX9::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	Vector4D_t Upload;
	Upload.x = fRed;
	Upload.y = fGreen;
	Upload.z = fBlue;
	Upload.w = fAlpha;
	m_ShaderData.m_DX9.m_pDevice->SetPixelShaderConstantF(0,&Upload.x,1);
}

void Burger::Shader2DCColorDX9::SetColor(const Vector4D_t *pColor)
{
	m_ShaderData.m_DX9.m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);
}

//
// OpenGL version
//

#if defined(BURGER_INTEL)
Burger::Shader2DCColorOpenGL::Shader2DCColorOpenGL(Display *pDisplay,const uint_t *pVertexMembers) :
	Shader2DCColor(pDisplay,pVertexMembers)
{
}

uint_t Burger::Shader2DCColorOpenGL::CheckLoad(Display *pDisplay)
{
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20pos22dgl,0,g_pscclr4gl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
			m_iEffect2DColor = glGetUniformLocation(uProgram,g_glColor);
		}
	}
	return 0;
}

void Burger::Shader2DCColorOpenGL::Release(Display * /* pDisplay */)
{
	ReleaseGL();
}


void Burger::Shader2DCColorOpenGL::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	glUniform4f(m_iEffect2DPosition,fX,fY,fWidth,fHeight);
}

void Burger::Shader2DCColorOpenGL::SetPosition(const Vector4D_t *pPosition)
{
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
}

void Burger::Shader2DCColorOpenGL::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	glUniform4f(m_iEffect2DColor,fRed,fGreen,fBlue,fAlpha);
}

void Burger::Shader2DCColorOpenGL::SetColor(const Vector4D_t *pColor)
{
	glUniform4fv(m_iEffect2DColor,1,&pColor->x);
}

#endif
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::Shader2DCColor::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Shader2DCColorDX9::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Shader2DCColorOpenGL::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
