/***************************************

    Static position texturing shader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "breffectstaticpositiontexture.h"
#include "brshaders.h"

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)
#include <d3d9.h>
#include <windows.h>
BURGER_CREATE_STATICRTTI_PARENT(
    Burger::EffectPositionTextureDX9, Burger::EffectPositionTexture);
BURGER_CREATE_STATICRTTI_PARENT(
    Burger::EffectPositionTextureOpenGL, Burger::EffectPositionTexture);
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
BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectPositionTexture, Burger::Effect);

#if defined(BURGER_OPENGL)
static const Burger::Display::OpenGLVertexInputs_t g_Inputs[] = {
    {Burger::VertexBuffer::USAGE_POSITION, Burger::g_glPosition},
    {Burger::VertexBuffer::USAGE_TEXCOORD, Burger::g_glTexcoord},
    {Burger::VertexBuffer::USAGE_END}};
#endif
#endif

/*! ************************************

    \brief Default constructor for an EffectPositionTexture
    \param pDisplay Pointer to the current display
    \param pVertexMembers Pointer to a description to the vertex entries

***************************************/

Burger::EffectPositionTexture::EffectPositionTexture(
    Display* pDisplay, const uint_t* pVertexMembers)
#if defined(BURGER_OPENGL)
    :
    m_pVertexMembers(pVertexMembers),
    m_iEffectMatrix(0)
#endif
{
    BURGER_UNUSED(pDisplay);
#if !defined(BURGER_OPENGL)
    BURGER_UNUSED(pVertexMembers);
#endif
}

//
// Upload data
//

#if !defined(BURGER_WINDOWS) && !defined(DOXYGEN)

uint_t Burger::EffectPositionTexture::CheckLoad(Display* pDisplay)
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
        GLuint uProgram = static_cast<DisplayOpenGL*>(pDisplay)->CompileProgram(
            g_vsstaticpositiongl, 0, g_pstexturegl, 0, g_Inputs,
            m_pVertexMembers);
        if (uProgram) {
            m_ShaderData.m_GL.m_uProgramID = uProgram;
            glUseProgram(uProgram);
            GLint iSampler = glGetUniformLocation(uProgram, g_glDiffuseTexture);
            // Indicate that the diffuse texture will be bound to texture unit 0
            glUniform1i(iSampler, 0);
            m_iEffectMatrix =
                glGetUniformLocation(uProgram, g_glViewProjectionMatrix);
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

    \brief Set the projection matrix

    \param pMatrix Pointer to the matrix

***************************************/

void BURGER_API Burger::EffectPositionTexture::SetProjection(
    const Matrix4D_t* pMatrix)
{
#if defined(BURGER_XBOX360)
    Matrix4D_t Transposed;
    Transposed.Transpose(pMatrix);
    D3DDevice_SetVertexShaderConstantF(m_pDevice, 0, &Transposed.x.x, 4);

#elif defined(BURGER_OPENGL)
    glUniformMatrix4fv(m_iEffectMatrix, 1, GL_FALSE, &pMatrix->x.x);

#else
    BURGER_UNUSED(pMatrix);
#endif
}

#endif

//
// DirectX 9 version
//

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

/*! ************************************

    \brief Default constructor for an EffectPositionTextureDX9

***************************************/

Burger::EffectPositionTextureDX9::EffectPositionTextureDX9(
    DisplayDirectX9* pDisplay, const uint_t* pVertexMembers) :
    EffectPositionTexture(pDisplay, pVertexMembers)
{
    m_ShaderData.m_DX9.m_pDevice =
        static_cast<DisplayDirectX9*>(pDisplay)->GetDirect3DDevice9();
}

uint_t Burger::EffectPositionTextureDX9::CheckLoad(Display* pDisplay)
{
    m_ShaderData.m_DX9.m_pDevice =
        static_cast<DisplayDirectX9*>(pDisplay)->GetDirect3DDevice9();
    if (!m_ShaderData.m_DX9.m_pVertexShader) {
        m_ShaderData.m_DX9.m_pVertexShader =
            static_cast<DisplayDirectX9*>(pDisplay)->CreateVertexShader(
                g_vsstaticpositiondx9);
    }
    if (!m_ShaderData.m_DX9.m_pPixelShader) {
        m_ShaderData.m_DX9.m_pPixelShader =
            static_cast<DisplayDirectX9*>(pDisplay)->CreatePixelShader(
                g_pstexturedx9);
    }
    return 0;
}

void Burger::EffectPositionTextureDX9::Release(Display* /* pDisplay */)
{
    ReleaseDX9();
}

void Burger::EffectPositionTextureDX9::SetProjection(const Matrix4D_t* pMatrix)
{
    Matrix4D_t Transposed;
    Transposed.Transpose(pMatrix);
    m_ShaderData.m_DX9.m_pDevice->SetVertexShaderConstantF(
        0, &Transposed.x.x, 4);
}

//
// Windows OpenGL version
//

/*! ************************************

    \brief Default constructor for an EffectPositionTextureOpenGL

***************************************/

#if defined(BURGER_INTEL)

Burger::EffectPositionTextureOpenGL::EffectPositionTextureOpenGL(
    Display* pDisplay, const uint_t* pVertexMembers) :
    EffectPositionTexture(pDisplay, pVertexMembers)
{
}

uint_t Burger::EffectPositionTextureOpenGL::CheckLoad(Display* pDisplay)
{
    if (!m_ShaderData.m_GL.m_uProgramID) {
        GLuint uProgram = static_cast<DisplayOpenGL*>(pDisplay)->CompileProgram(
            g_vsstaticpositiongl, 0, g_pstexturegl, 0, g_Inputs,
            m_pVertexMembers);
        if (uProgram) {
            m_ShaderData.m_GL.m_uProgramID = uProgram;
            glUseProgram(uProgram);
            GLint iSampler = glGetUniformLocation(uProgram, g_glDiffuseTexture);
            // Indicate that the diffuse texture will be bound to texture unit 0
            glUniform1i(iSampler, 0);
            m_iEffectMatrix =
                glGetUniformLocation(uProgram, g_glViewProjectionMatrix);
        }
    }
    return 0;
}

void Burger::EffectPositionTextureOpenGL::Release(Display* /* pDisplay */)
{
    ReleaseGL();
}

void Burger::EffectPositionTextureOpenGL::SetProjection(
    const Matrix4D_t* pMatrix)
{
    glUniformMatrix4fv(m_iEffectMatrix, 1, GL_FALSE, &pMatrix->x.x);
}

#endif
#endif

/*! ************************************

    \var const Burger::StaticRTTI Burger::EffectPositionTexture::g_StaticRTTI
    \brief The global description of the class

    This record contains the name of this class and a
    reference to the parent

***************************************/

/*! ************************************

    \var const Burger::StaticRTTI Burger::EffectPositionTextureDX9::g_StaticRTTI
    \brief The global description of the class

    This record contains the name of this class and a
    reference to the parent

***************************************/

/*! ************************************

    \var const Burger::StaticRTTI Burger::EffectPositionTextureOpenGL::g_StaticRTTI

    \brief The global description of the class

    This record contains the name of this class and a reference to the parent

***************************************/
