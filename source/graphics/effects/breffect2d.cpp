/***************************************

	Simple 2D texturing shader

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "breffect2d.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_OPENGL_SUPPORTED)

//
// Vertexes needed for Effect2D to render an entire texture
// as a 2D shape
//

static const Burger::Effect2D::Vertex g_Vertices2DQuad[] = {
	{0.0f,-2.0f,0.0f,1.0f},
	{2.0f,-2.0f,1.0f,1.0f},
	{0.0f, 0.0f,0.0f,0.0f},
	{2.0f, 0.0f,1.0f,0.0f}
};

// 2 entries for position
// 2 entries for UV
const Word Burger::g_Effect2DMemberDescription[] = {2|Burger::VertexBuffer::USAGE_POSITION,2|Burger::VertexBuffer::USAGE_TEXCOORD,0};

// Describe the location
const Burger::VertexBuffer::VertexAoS_t Burger::g_Effect2DDefaultVertexBufferDescription = {
	g_Vertices2DQuad,sizeof(g_Vertices2DQuad),NULL,0,Burger::g_Effect2DMemberDescription
};
#endif

#if defined(BURGER_WINDOWS)
#include <windows.h>
#include <d3d9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::Effect2DDX9,Burger::EffectDX9);

Burger::Effect2DDX9::Effect2DDX9(DisplayDirectX9 *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetDirect3DDevice9();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vs20sprite2ddx9);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexturedx9);
}

void BURGER_API Burger::Effect2DDX9::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Effect2DDX9::SetPosition(const Vector4D_t *pPosition)
{
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

#endif


//
// OpenGL version
//

#if defined(BURGER_OPENGL_SUPPORTED) || defined(DOXYGEN)
#define __BURGER__
#include "brgl.h"
#include "brglext.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::Effect2DOpenGL,Burger::EffectOpenGL);

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#endif

static const Burger::DisplayOpenGL::VertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_TEXCOORD,Burger::g_glTexcoord},
	{Burger::DisplayOpenGL::VERTEX_END}
};

Burger::Effect2DOpenGL::Effect2DOpenGL(Display *pDisplay,const Word * /* pVertexMembers */)
{
	GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20sprite2dgl,0,g_pstexturegl,0,g_Inputs);
	if (uProgram) {
		m_uEffect = uProgram;
		glUseProgram(uProgram);
		GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
		// Indicate that the diffuse texture will be bound to texture unit 0
		glUniform1i(iSampler,0);
		m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
	}
}

void BURGER_API Burger::Effect2DOpenGL::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	glUniform4f(m_iEffect2DPosition,fX,fY,fWidth,fHeight);
}

void BURGER_API Burger::Effect2DOpenGL::SetPosition(const Vector4D_t *pPosition)
{
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
}

#endif

//
// XBox 360 version
//

#if defined(BURGER_XBOX360)
#define NONET
#include <xtl.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::Effect2D,Burger::Effect);

Burger::Effect2D::Effect2D(Display *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetD3DDevice();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vs20sprite2d360);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexture360);
}

void BURGER_API Burger::Effect2D::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Effect2D::SetPosition(const Vector4D_t *pPosition)
{
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

#endif

