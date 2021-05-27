/***************************************

    Base shader effect class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "breffect.h"
#include "brmemoryfunctions.h"

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
#define NONET
#include <xtl.h>
#endif

#if defined(BURGER_WINDOWS) && !defined(DOXYGEN)
#include <windows.h>
#include <d3d9.h>
#endif

#if defined(BURGER_OPENGL) && !defined(DOXYGEN)
#define __BURGER__
#include "brgl.h"
#include "brglext.h"
#endif

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Effect,Burger::DisplayObject);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::Effect::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\class Burger::Effect

	\brief Base class for shader effects.

	This base class has default members and functions to 
	connect a vertex and pixel shader using a cross
	platform API with this class performing
	the functions to implement the class
	using whatever is the native API. 

	\note On Windows platforms, this is a union with data for
	DirectX 9, DirectX 11 and OpenGL

	\sa Display

***************************************/

/*! ************************************

	\brief Standard constructor

	Only initializes member variables to \ref NULL.

***************************************/

Burger::Effect::Effect()

#if defined(BURGER_XBOX360)
	: m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pDevice(NULL)
#endif

{

	// OpenGL and Windows
#if defined(BURGER_WINDOWS) || defined(BURGER_OPENGL)
	MemoryClear(&m_ShaderData,sizeof(m_ShaderData));
#endif

}

/*! ************************************

	\brief Standard destructor

	Calls Release(Display *) to release all allocated resources

***************************************/

Burger::Effect::~Effect()
{
	// Release the pixel and vertex shaders
#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
	Release(NULL);
#endif
}

/*! ************************************

	\brief Release the shaders

	On shutdown and Direct X reset, this function is
	called to release the shader resources.

	\param pDisplay Pointer to the Display instance

***************************************/

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
void Burger::Effect::Release(Display * /* pDisplay */)
{

#if defined(BURGER_XBOX360)
	D3DPixelShader *pPixelShader = m_pPixelShader;
	if (pPixelShader != NULL) {
		IDirect3DDevice9 *pDevice;
		pPixelShader->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetPixelShader(0);
		}

		pPixelShader->Release(); 
		m_pPixelShader = NULL;
	}

	D3DVertexShader *pVertexShader = m_pVertexShader;
	if (pVertexShader != NULL) {
		IDirect3DDevice9 *pDevice;
		pVertexShader->GetDevice(&pDevice);
		if (pDevice) {
			pDevice->SetVertexShader(0);
		}
		pVertexShader->Release(); 
		m_pVertexShader = NULL;
	}
	m_pDevice = NULL;
#endif

	// Release shader on non-windows OpenGL platforms
#if defined(BURGER_OPENGL)
	if (m_ShaderData.m_GL.m_uProgramID) {
		glDeleteProgram(m_ShaderData.m_GL.m_uProgramID);
		m_ShaderData.m_GL.m_uProgramID = 0;
	}
#endif

}
#endif

/*! ************************************

	\fn D3DPixelShader *Burger::Effect::GetPixelShader360(void) const
	\brief Get the pointer to the Xbox 360 Pixel shader instance

	\xbox360only
	\return \ref NULL if no pixel shader is used, a valid D3DPixelShader if CheckLoad() was called

***************************************/

/*! ************************************

	\fn D3DVertexShader *Burger::Effect::GetVertexShader360(void) const
	\brief Get the pointer to the Xbox 360 Vertex shader instance

	\xbox360only
	\return \ref NULL if no vertex shader is used, a valid D3DVertexShader if CheckLoad() was called

***************************************/

/*! ************************************

	\fn D3DDevice *Burger::Effect::GetDevice360(void) const
	\brief Get the pointer to the Xbox 360 device instance

	\xbox360only
	\return \ref NULL if no device is used, a valid D3DDevice if CheckLoad() was called

***************************************/

/*! ************************************

	\fn uint_t Burger::Effect::GetProgramID(void) const
	\brief Get the OpenGL program ID for the compiled effect

	\return Zero if no effect was compiled, non-zero if CheckLoad() was called

***************************************/

/*! ************************************

	\fn IDirect3DPixelShader9 *Burger::Effect::GetPixelShaderDX9(void) const
	\brief Get the pointer to the DirectX 9 Pixel shader instance
	
	\windowsonly
	\return \ref NULL if no pixel shader is used, a valid IDirect3DPixelShader9 if CheckLoad() was called

***************************************/

/*! ************************************

	\fn IDirect3DVertexShader9 *Burger::Effect::GetVertexShaderDX9(void) const
	\brief Get the pointer to the DirectX 9 Vertex shader instance

	\windowsonly
	\return \ref NULL if no vertex shader is used, a valid IDirect3DVertexShader9 if CheckLoad() was called

***************************************/

/*! ************************************

	\fn IDirect3DDevice9 *Burger::Effect::GetDeviceDX9(void) const
	\brief Get the pointer to the DirectX 9 device instance

	\windowsonly
	\return \ref NULL if no device is used, a valid IDirect3DDevice9 if CheckLoad() was called

***************************************/


#if (defined(BURGER_WINDOWS) && defined(BURGER_INTEL)) || defined(DOXYGEN)

/*! ************************************

	\brief Release the OpenGL data

	For OpenGL shaders, Release(Display *) calls this function to perform the actual work

	\windowsonly

***************************************/

void BURGER_API Burger::Effect::ReleaseGL(void)
{
	GLuint uID = m_ShaderData.m_GL.m_uProgramID;
	if (uID) {
		glDeleteProgram(uID);
		m_ShaderData.m_GL.m_uProgramID = 0;
	}
}
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

/*! ************************************

	\brief Release the DirectX 9 data

	For DirectX 9 shaders, Release(Display *) calls this function to perform the actual work

	\windowsonly

***************************************/

void BURGER_API Burger::Effect::ReleaseDX9(void)
{
	// Release the pixel and vertex shaders
	IDirect3DPixelShader9 *pPixelShader = m_ShaderData.m_DX9.m_pPixelShader;
	if (pPixelShader!=NULL) {
		pPixelShader->Release();
		m_ShaderData.m_DX9.m_pPixelShader = NULL;
	}

	IDirect3DVertexShader9 *pVertexShader = m_ShaderData.m_DX9.m_pVertexShader;
	if (pVertexShader!=NULL) {
		pVertexShader->Release();
		m_ShaderData.m_DX9.m_pVertexShader = NULL;
	}
	m_ShaderData.m_DX9.m_pDevice = NULL;
}

#endif

