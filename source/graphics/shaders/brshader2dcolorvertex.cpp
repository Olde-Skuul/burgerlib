/***************************************

    Simple 2D texturing shader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brshader2dcolorvertex.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)

#include "win_windows.h"

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl off
#endif

#include <d3d9.h>

#if defined(BURGER_METROWERKS)
#pragma warn_emptydecl reset
#endif

BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DColorVertexDX9,Burger::Shader2DColorVertex);
BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DColorVertexOpenGL,Burger::Shader2DColorVertex);
#endif

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
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
BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DColorVertex,Burger::Effect);

//
// Vertexes needed for Effect2D to render an entire texture
// as a 2D shape
//

static const Burger::Shader2DColorVertex::Vertex g_Vertices2DQuad[] = {
	{0.0f,-2.0f,0.0f,1.0f,{255,255,255,255}},
	{2.0f,-2.0f,1.0f,1.0f,{255,255,255,255}},
	{0.0f, 0.0f,0.0f,0.0f,{255,255,255,255}},
	{2.0f, 0.0f,1.0f,0.0f,{255,255,255,255}}
};

#if defined(BURGER_OPENGL)
static const Burger::Display::OpenGLVertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_TEXCOORD,Burger::g_glTexcoord},
	{Burger::VertexBuffer::USAGE_COLOR,Burger::g_glColor},
	{Burger::VertexBuffer::USAGE_END}
};
#endif
#endif

/*! ************************************

	\brief Vertex description for Shader2DColorVertex
	2 entries for position
	2 entries for UV
	4 entries for color

***************************************/

const uint_t Burger::g_Shader2DColorVertexMemberDescription[] = {
	Burger::VertexBuffer::USAGE_FLOAT2|Burger::VertexBuffer::USAGE_POSITION,
	Burger::VertexBuffer::USAGE_FLOAT2|Burger::VertexBuffer::USAGE_TEXCOORD,
	Burger::VertexBuffer::USAGE_COLOR4|Burger::VertexBuffer::USAGE_COLOR,
	Burger::VertexBuffer::USAGE_END
};

/*! ************************************

	\brief Describe the location for Shader2DColorVertex

***************************************/

const Burger::VertexBuffer::VertexAoS_t Burger::g_Shader2DColorVertexDefaultVertexBufferDescription = {
	g_Vertices2DQuad,sizeof(g_Vertices2DQuad),
	NULL,0,
	Burger::g_Shader2DColorVertexMemberDescription,Burger::VertexBuffer::FLAGAOS_DONTCOPY_VERTICES|Burger::VertexBuffer::FLAGAOS_DONTCOPY_MEMBERS
};

/*! ************************************

	\brief Default constructor for an Effect2D
	\param pDisplay Pointer to the current display
	\param pVertexMembers Pointer to a description to the vertex entries

***************************************/


Burger::Shader2DColorVertex::Shader2DColorVertex(Display *pDisplay,const uint_t * /* pVertexMembers */)
#if defined(BURGER_OPENGL)
	: m_iEffect2DPosition(0)
#endif
{
	BURGER_UNUSED(pDisplay);
}

//
// Upload data
//

#if !defined(BURGER_WINDOWS) && !defined(DOXYGEN)

uint_t Burger::Shader2DColorVertex::CheckLoad(Display *pDisplay)
{
#if defined(BURGER_XBOX360)
	m_pDevice = pDisplay->GetD3DDevice();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vs20pos2tex2clr42d360);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstex2clr4360);

#elif defined(BURGER_OPENGL)
	BURGER_UNUSED(pDisplay);
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20pos2tex2clr42dgl,0,g_pstex2clr4gl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
		}
	}
#else
	BURGER_UNUSED(pDisplay);
#endif

	return 0;
}
#endif


#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
void BURGER_API Burger::Shader2DColorVertex::SetPosition(float fX,float fY,float fWidth,float fHeight)
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

void BURGER_API Burger::Shader2DColorVertex::SetPosition(const Vector4D_t *pPosition)
{
#if defined(BURGER_XBOX360)
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
#elif defined(BURGER_OPENGL)
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
#else
	BURGER_UNUSED(pPosition);
#endif
}

#endif

//
// DirectX 9 version
//

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

/*! ************************************

	\brief Default constructor for an Shader2DColorVertexDX9

***************************************/

Burger::Shader2DColorVertexDX9::Shader2DColorVertexDX9(Display *pDisplay,const uint_t *pVertexMembers) :
	Shader2DColorVertex(pDisplay,pVertexMembers)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
}

uint_t Burger::Shader2DColorVertexDX9::CheckLoad(Display *pDisplay)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
	if (!m_ShaderData.m_DX9.m_pVertexShader) {
		m_ShaderData.m_DX9.m_pVertexShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreateVertexShader(g_vs20pos2tex2clr42ddx9);
	}
	if (!m_ShaderData.m_DX9.m_pPixelShader) {
		m_ShaderData.m_DX9.m_pPixelShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreatePixelShader(g_pstex2clr4dx9);
	}
	return 0;
}

void Burger::Shader2DColorVertexDX9::Release(Display * /* pDisplay */)
{
	ReleaseDX9();
}

void Burger::Shader2DColorVertexDX9::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void Burger::Shader2DColorVertexDX9::SetPosition(const Vector4D_t *pPosition)
{
	m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

//
// OpenGL version
//

#if defined(BURGER_INTEL)
Burger::Shader2DColorVertexOpenGL::Shader2DColorVertexOpenGL(Display *pDisplay,const uint_t *pVertexMembers) :
	Shader2DColorVertex(pDisplay,pVertexMembers)
{
}

uint_t Burger::Shader2DColorVertexOpenGL::CheckLoad(Display *pDisplay)
{
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20pos2tex2clr42dgl,0,g_pstex2clr4gl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
		}
	}
	return 0;

}

void Burger::Shader2DColorVertexOpenGL::Release(Display * /* pDisplay */)
{
	ReleaseGL();
}


void Burger::Shader2DColorVertexOpenGL::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	glUniform4f(m_iEffect2DPosition,fX,fY,fWidth,fHeight);
}

void Burger::Shader2DColorVertexOpenGL::SetPosition(const Vector4D_t *pPosition)
{
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
}

#endif
#endif


