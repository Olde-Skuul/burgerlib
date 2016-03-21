/***************************************

	Simple 2D shader with color

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brshader2dccolor.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(BURGER_OPENGL_SUPPORTED)

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

// 2 entries for position
const Word Burger::g_Shader2DCColorMemberDescription[] = {
	2|Burger::VertexBuffer::USAGE_POSITION,
	0};

// Describe the location
const Burger::VertexBuffer::VertexAoS_t Burger::g_Shader2DCColorDefaultVertexBufferDescription = {
	g_Vertices2DQuad,sizeof(g_Vertices2DQuad),NULL,0,Burger::g_Shader2DCColorMemberDescription
};
#endif

#if defined(BURGER_WINDOWS)
#include <windows.h>
#include <d3d9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColorDX9,Burger::EffectDX9);

Burger::Shader2DCColorDX9::Shader2DCColorDX9(DisplayDirectX9 *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetDirect3DDevice9();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vs20pos22ddx9);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pscclr4dx9);
}

void BURGER_API Burger::Shader2DCColorDX9::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Shader2DCColorDX9::SetPosition(const Vector4D_t *pPosition)
{
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

void BURGER_API Burger::Shader2DCColorDX9::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	Vector4D_t Upload;
	Upload.x = fRed;
	Upload.y = fGreen;
	Upload.z = fBlue;
	Upload.w = fAlpha;
	m_pDevice->SetPixelShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Shader2DCColorDX9::SetColor(const Vector4D_t *pColor)
{
	m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);
}

#endif


//
// OpenGL version
//

#if defined(BURGER_OPENGL_SUPPORTED) || defined(DOXYGEN)
#define __BURGER__
#include "brgl.h"
#include "brglext.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColorOpenGL,Burger::EffectOpenGL);

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#endif

static const Burger::DisplayOpenGL::VertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::DisplayOpenGL::VERTEX_END}
};

Burger::Shader2DCColorOpenGL::Shader2DCColorOpenGL(Display *pDisplay,const Word * /* pVertexMembers */)
{
	GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vs20pos22dgl,0,g_pscclr4gl,0,g_Inputs);
	if (uProgram) {
		m_uEffect = uProgram;
		glUseProgram(uProgram);
		GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
		// Indicate that the diffuse texture will be bound to texture unit 0
		glUniform1i(iSampler,0);
		m_iEffect2DPosition = glGetUniformLocation(uProgram,g_glXYWidthHeight);
		m_iEffect2DColor = glGetUniformLocation(uProgram,g_glColor);
	}
}

void BURGER_API Burger::Shader2DCColorOpenGL::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	glUniform4f(m_iEffect2DPosition,fX,fY,fWidth,fHeight);
}

void BURGER_API Burger::Shader2DCColorOpenGL::SetPosition(const Vector4D_t *pPosition)
{
	glUniform4fv(m_iEffect2DPosition,1,&pPosition->x);
}

void BURGER_API Burger::Shader2DCColorOpenGL::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	glUniform4f(m_iEffect2DColor,fRed,fGreen,fBlue,fAlpha);
}

void BURGER_API Burger::Shader2DCColorOpenGL::SetColor(const Vector4D_t *pColor)
{
	glUniform4fv(m_iEffect2DColor,1,&pColor->x);
}

#endif

//
// XBox 360 version
//

#if defined(BURGER_XBOX360)
#define NONET
#include <xtl.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::Shader2DCColor,Burger::Effect);

Burger::Shader2DCColor::Shader2DCColor(Display *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetD3DDevice();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vs20pos22d360);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pscclr4360);
}

void BURGER_API Burger::Shader2DCColor::SetPosition(float fX,float fY,float fWidth,float fHeight)
{
	Vector4D_t Upload;
	Upload.x = fX;
	Upload.y = fY;
	Upload.z = fWidth;
	Upload.w = fHeight;
	m_pDevice->SetVertexShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Shader2DCColor::SetPosition(const Vector4D_t *pPosition)
{
	m_pDevice->SetVertexShaderConstantF(0,&pPosition->x,1);
}

void BURGER_API Burger::Shader2DCColor::SetColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	Vector4D_t Upload;
	Upload.x = fRed;
	Upload.y = fGreen;
	Upload.z = fBlue;
	Upload.w = fAlpha;
	m_pDevice->SetPixelShaderConstantF(0,&Upload.x,1);
}

void BURGER_API Burger::Shader2DCColor::SetColor(const Vector4D_t *pColor)
{
	m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);
}

#endif

