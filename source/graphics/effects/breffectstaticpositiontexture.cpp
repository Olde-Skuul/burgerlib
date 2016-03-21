/***************************************

	Static position texturing shader

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "breffectstaticpositiontexture.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS)
#include <windows.h>
#include <d3d9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionTextureDX9,Burger::EffectDX9);

Burger::EffectPositionTextureDX9::EffectPositionTextureDX9(DisplayDirectX9 *pDisplay,const Word * /* pVertexMembers */)
{
	m_pDevice = pDisplay->GetDirect3DDevice9();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vsstaticpositiondx9);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexturedx9);
}

void BURGER_API Burger::EffectPositionTextureDX9::SetProjection(const Matrix4D_t *pMatrix)
{
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	m_pDevice->SetVertexShaderConstantF(0,&Transposed.x.x,4);
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
#include "brvertexbuffer.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionTextureOpenGL,Burger::EffectOpenGL);

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#endif

static const Burger::DisplayOpenGL::VertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_TEXCOORD,Burger::g_glTexcoord},
	{Burger::DisplayOpenGL::VERTEX_END}
};

Burger::EffectPositionTextureOpenGL::EffectPositionTextureOpenGL(Display *pDisplay,const Word *pVertexMembers)
{
	GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vsstaticpositiongl,0,g_pstexturegl,0,g_Inputs,pVertexMembers);
	if (uProgram) {
		m_uEffect = uProgram;
		glUseProgram(uProgram);
		GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
		// Indicate that the diffuse texture will be bound to texture unit 0
		glUniform1i(iSampler,0);
		m_iEffectMatrix = glGetUniformLocation(uProgram,g_glViewProjectionMatrix);
	}
}

void BURGER_API Burger::EffectPositionTextureOpenGL::SetProjection(const Matrix4D_t *pMatrix)
{
	glUniformMatrix4fv(m_iEffectMatrix,1,GL_FALSE,&pMatrix->x.x);
}

#endif

//
// XBox 360 version
//

#if defined(BURGER_XBOX360)
#define NONET
#include <xtl.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionTexture,Burger::Effect);

Burger::EffectPositionTexture::EffectPositionTexture(Display *pDisplay,const Word * /*pVertexMembers */)
{
	m_pDevice = pDisplay->GetD3DDevice();
	m_pVertexShader = pDisplay->CreateVertexShader(g_vsstaticposition360);
	m_pPixelShader = pDisplay->CreatePixelShader(g_pstexture360);
}

void BURGER_API Burger::EffectPositionTexture::SetProjection(const Matrix4D_t *pMatrix)
{
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	D3DDevice_SetVertexShaderConstantF(m_pDevice,0,&Transposed.x.x,4);
}
#endif
