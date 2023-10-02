/***************************************

    Position textured color shader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "breffectpositiontexturecolor.h"
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

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTextureDX9,Burger::EffectPositionColorTexture);
BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTextureOpenGL,Burger::EffectPositionColorTexture);
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
BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionColorTexture,Burger::Effect);

#if defined(BURGER_OPENGL)
static const Burger::Display::OpenGLVertexInputs_t g_Inputs[] = {
	{Burger::VertexBuffer::USAGE_POSITION,Burger::g_glPosition},
	{Burger::VertexBuffer::USAGE_TEXCOORD,Burger::g_glTexcoord},
	{Burger::VertexBuffer::USAGE_END}
};
#endif
#endif

/*! ************************************

	\brief Default constructor for an Effect2D
	\param pDisplay Pointer to the current display
	\param pVertexMembers Pointer to a description to the vertex entries

***************************************/

Burger::EffectPositionColorTexture::EffectPositionColorTexture(Display *pDisplay,const uint_t * /* pVertexMembers */)
#if defined(BURGER_OPENGL)
	: m_iEffectMatrix(0),
	m_iEffectColor(0)
#endif
{
	BURGER_UNUSED(pDisplay);
}

//
// Upload data
//

#if !defined(BURGER_WINDOWS) && !defined(DOXYGEN)

uint_t Burger::EffectPositionColorTexture::CheckLoad(Display *pDisplay)
{
#if defined(BURGER_XBOX360)
	m_pDevice = pDisplay->GetD3DDevice();
	if (!m_pVertexShader) {
		m_pVertexShader = pDisplay->CreateVertexShader(g_vsstaticposition360);
	}
	if (!m_pPixelShader) {
		m_pPixelShader = pDisplay->CreatePixelShader(g_pstexture360);
	}

#elif defined(BURGER_OPENGL)
	BURGER_UNUSED(pDisplay);
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vsstaticpositiongl,0,g_pstexturecolorgl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffectMatrix = glGetUniformLocation(uProgram,g_glViewProjectionMatrix);
			m_iEffectColor = glGetUniformLocation(uProgram,g_glColor);
		}
	}

#else
	BURGER_UNUSED(pDisplay);
#endif

	return 0;
}

#endif


#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)

/*! ************************************

	\brief Set the location of where to draw the 2D texture
	\param pMatrix Pointer to the position matrix

***************************************/

void BURGER_API Burger::EffectPositionColorTexture::SetProjection(const Matrix4D_t *pMatrix)
{

#if defined(BURGER_XBOX360)
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	D3DDevice_SetVertexShaderConstantF(m_pDevice,0,&Transposed.x.x,4);

#elif defined(BURGER_OPENGL)
	glUniformMatrix4fv(m_iEffectMatrix,1,GL_FALSE,&pMatrix->x.x);

#else
	BURGER_UNUSED(pMatrix);
#endif
}

void BURGER_API Burger::EffectPositionColorTexture::SetColor(const Vector4D_t *pColor)
{
#if defined(BURGER_XBOX360)
	D3DDevice_SetPixelShaderConstantF(m_pDevice,0,&pColor->x,1);

#elif defined(BURGER_OPENGL)
	glUniform4fv(m_iEffectColor,1,&pColor->x);

#else
	BURGER_UNUSED(pColor);
#endif
}

#endif


//
// DirectX 9 version
//

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

/*! ************************************

	\brief Default constructor for an EffectPositionColorTextureDX9

***************************************/

Burger::EffectPositionColorTextureDX9::EffectPositionColorTextureDX9(Display *pDisplay,const uint_t *pVertexMembers) :
	EffectPositionColorTexture(pDisplay,pVertexMembers)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
}

uint_t Burger::EffectPositionColorTextureDX9::CheckLoad(Display *pDisplay)
{
	m_ShaderData.m_DX9.m_pDevice = static_cast<DisplayDirectX9 *>(pDisplay)->GetDirect3DDevice9();
	if (!m_ShaderData.m_DX9.m_pVertexShader) {
		m_ShaderData.m_DX9.m_pVertexShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreateVertexShader(g_vsstaticpositiondx9);
	}
	if (!m_ShaderData.m_DX9.m_pPixelShader) {
		m_ShaderData.m_DX9.m_pPixelShader = static_cast<DisplayDirectX9 *>(pDisplay)->CreatePixelShader(g_pstexturecolordx9);
	}
	return 0;
}

void Burger::EffectPositionColorTextureDX9::Release(Display * /* pDisplay */)
{
	ReleaseDX9();
}

void Burger::EffectPositionColorTextureDX9::SetProjection(const Matrix4D_t *pMatrix)
{
	Matrix4D_t Transposed;
	Transposed.Transpose(pMatrix);
	m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(0,&Transposed.x.x,4);
}

void Burger::EffectPositionColorTextureDX9::SetColor(const Vector4D_t *pColor)
{
	m_ShaderData.m_DX9.m_pDevice->SetPixelShaderConstantF(0,&pColor->x,1);
}

//
// Windows OpenGL version
//

/*! ************************************

	\brief Default constructor for an EffectPositionColorTextureOpenGL

***************************************/

#if defined(BURGER_INTEL)
Burger::EffectPositionColorTextureOpenGL::EffectPositionColorTextureOpenGL(Display *pDisplay,const uint_t *pVertexMembers) :
	EffectPositionColorTexture(pDisplay,pVertexMembers)
{

}

uint_t Burger::EffectPositionColorTextureOpenGL::CheckLoad(Display *pDisplay)
{
	if (!m_ShaderData.m_GL.m_uProgramID) {
		GLuint uProgram = static_cast<DisplayOpenGL *>(pDisplay)->CompileProgram(g_vsstaticpositiongl,0,g_pstexturecolorgl,0,g_Inputs);
		if (uProgram) {
			m_ShaderData.m_GL.m_uProgramID = uProgram;
			glUseProgram(uProgram);
			GLint iSampler = glGetUniformLocation(uProgram,g_glDiffuseTexture);
			// Indicate that the diffuse texture will be bound to texture unit 0
			glUniform1i(iSampler,0);
			m_iEffectMatrix = glGetUniformLocation(uProgram,g_glViewProjectionMatrix);
			m_iEffectColor = glGetUniformLocation(uProgram,g_glColor);
		}
	}
	return 0;
}

void Burger::EffectPositionColorTextureOpenGL::Release(Display * /* pDisplay */)
{
	ReleaseGL();
}

void Burger::EffectPositionColorTextureOpenGL::SetProjection(const Matrix4D_t *pMatrix)
{
	glUniformMatrix4fv(m_iEffectMatrix,1,GL_FALSE,&pMatrix->x.x);
}

void Burger::EffectPositionColorTextureOpenGL::SetColor(const Vector4D_t *pColor)
{
	glUniform4fv(m_iEffectColor,1,&pColor->x);
}

#endif
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::EffectPositionColorTexture::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::EffectPositionColorTextureDX9::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::EffectPositionColorTextureOpenGL::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
