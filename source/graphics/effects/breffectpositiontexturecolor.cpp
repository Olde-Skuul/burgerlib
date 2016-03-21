/***************************************

	Position textured color shader

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "breffectpositiontexturecolor.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS)
#include <windows.h>
#include <d3d9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTextureDX9,Burger::EffectDX9);

Burger::EffectPositionColorTextureDX9::EffectPositionColorTextureDX9(DisplayDirectX9 *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetDirect3DDevice9();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vsstaticpositiondx9);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexturecolordx9);
}

void BURGER_API Burger::EffectPositionColorTextureDX9::SetProjection(const Matrix4D_t *pMatrix)
{
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	m_pDevice->SetVertexShaderConstantF(0,&Transposed.x.x,4);
}

void BURGER_API Burger::EffectPositionColorTextureDX9::SetColor(const Vector4D_t *pColor)
{
	m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);
}
#endif


//
// OpenGL version
//

#define __BURGER__
#include "brgl.h"

#if defined(BURGER_OPENGL_SUPPORTED) || defined(DOXYGEN)
#include "brglext.h"
#include "brdebug.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTextureOpenGL,Burger::EffectOpenGL);

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#endif

static const Burger::DisplayOpenGL::VertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_TEXCOORD,Burger::g_glTexcoord},
	{Burger::DisplayOpenGL::VERTEX_END}
};

Burger::EffectPositionColorTextureOpenGL::EffectPositionColorTextureOpenGL(Display *pDisplay,const Word * /* pVertexMembers */)
{
	GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vsstaticpositiongl,0,g_pstexturecolorgl,0,g_Inputs);
	if (uProgram) {
		m_uEffect = uProgram;
		glUseProgram(uProgram);
		GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
		// Indicate that the diffuse texture will be bound to texture unit 0
		glUniform1i(iSampler,0);
		m_iEffectMatrix = glGetUniformLocation(uProgram,g_glViewProjectionMatrix);
		m_iEffectColor = glGetUniformLocation(uProgram,g_glColor);
	}
}

void BURGER_API Burger::EffectPositionColorTextureOpenGL::SetProjection(const Matrix4D_t *pMatrix)
{
	glUniformMatrix4fv(m_iEffectMatrix,1,GL_FALSE,&pMatrix->x.x);
}

void BURGER_API Burger::EffectPositionColorTextureOpenGL::SetColor(const Vector4D_t *pColor)
{
	glUniform4fv(m_iEffectColor,1,&pColor->x);
}
#endif

//
// XBox 360 version
//

#if defined(BURGER_XBOX360)
#define NONET
#include <xtl.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTexture,Burger::Effect);

Burger::EffectPositionColorTexture::EffectPositionColorTexture(Display *pDisplay,const Word *pVertexMembers)
{
	m_pDevice = pDisplay->GetD3DDevice();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vsstaticposition360);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexture360);
}

void BURGER_API Burger::EffectPositionColorTexture::SetProjection(const Matrix4D_t *pMatrix)
{
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	D3DDevice_SetVertexShaderConstantF(m_pDevice,0,&Transposed.x.x,4);
}

void BURGER_API Burger::EffectPositionColorTexture::SetColor(const Vector4D_t *pColor)
{
	D3DDevice_SetPixelShaderConstantF(m_pDevice,0,&pColor->x,1);
}

#endif
