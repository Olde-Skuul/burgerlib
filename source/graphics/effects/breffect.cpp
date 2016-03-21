/***************************************

	Base shader effect class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "breffect.h"

/*! ************************************

	\class Burger::EffectDX9

	\brief Base class for DirectX 9 shader effects.

	This base class has default members and functions to 
	connect a vertex and pixel shader using a cross
	platform API with this class performing
	the functions to implement the class
	using DirectX 9

	\note This class is only present for Windows
	\sa Effect or EffectOpenGL

***************************************/

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include <windows.h>
#include <d3d9.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectDX9,Burger::Base);

/*! ************************************

	\brief Standard constructor

	Only initializes member variables to \ref NULL.

***************************************/

Burger::EffectDX9::EffectDX9() :
	m_pDevice(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL)
{
}

/*! ************************************

	\brief Standard destructor

	If any base member variables are set, it will
	call the Release() member function(s) in
	DirectX 9 and set the members to \ref NULL

	One exception is the DirectX9 device, it will
	be ignored.

***************************************/

Burger::EffectDX9::~EffectDX9()
{
	// Release the pixel and vertex shaders
	IDirect3DPixelShader9 *pPixelShader = m_pPixelShader;
	if (pPixelShader != NULL) {
		pPixelShader->Release(); 
		m_pPixelShader = NULL;
	}
	IDirect3DVertexShader9 *pVertexShader = m_pVertexShader;
	if (pVertexShader != NULL) {
		pVertexShader->Release(); 
		m_pVertexShader = NULL;
	}
}

/*! ************************************

	\brief Connect the shader effect

	Connect the DirectX 9 effect to the device they
	were created against. Will set both
	the Pixel and Vertex shaders.

***************************************/

void BURGER_API Burger::EffectDX9::SetEffect(void)
{
	IDirect3DDevice9 *pDevice = m_pDevice;
	pDevice->SetPixelShader(m_pPixelShader);
	pDevice->SetVertexShader(m_pVertexShader);
}

#endif

/*! ************************************

	\class Burger::EffectOpenGL

	\brief Base class for OpenGL 9 shader effects.

	This base class has default members and functions to 
	connect a vertex and pixel shader using a cross
	platform API with this class performing
	the functions to implement the class
	using OpenGL

	\sa Effect or EffectDX9

***************************************/

#if defined(BURGER_OPENGL_SUPPORTED) || defined(DOXYGEN)
#define __BURGER__
#include "brgl.h"
#include "brglext.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::EffectOpenGL,Burger::Base);


/*! ************************************

	\brief Standard constructor

	Only initializes member variables to zero.

***************************************/

Burger::EffectOpenGL::EffectOpenGL() :
	m_uEffect(0)
{
}

/*! ************************************

	\brief Standard destructor

	If an OpenGL program was created, this destructor
	will call glDeleteProgram() to dispose of it.

***************************************/

Burger::EffectOpenGL::~EffectOpenGL()
{
	// Release shader
	glDeleteProgram(m_uEffect);
	m_uEffect = 0;
}

/*! ************************************

	\brief Connect the shader effect

	Connect the OpenGL program.

***************************************/

void BURGER_API Burger::EffectOpenGL::SetEffect(void)
{
	glUseProgram(m_uEffect);
}

#endif

/*! ************************************

	\class Burger::Effect

	\brief Base class for generic shader effects.

	This base class has default members and functions to 
	connect a vertex and pixel shader using a cross
	platform API with this class performing
	the functions to implement the class
	using the preferred graphics API for the platform.

	\sa EffectDX9 or EffectOpenGL

***************************************/

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
#define NONET
#include <xtl.h>

BURGER_CREATE_STATICRTTI_PARENT(Burger::Effect,Burger::Base);

/*! ************************************

	\brief Standard constructor

	Only initializes member variables to zero.

***************************************/

Burger::Effect::Effect() :
	m_pDevice(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL)
{
}

/*! ************************************

	\brief Standard destructor

	Releases all allocated shaders

***************************************/

Burger::Effect::~Effect()
{
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
}

/*! ************************************

	\brief Connect the shader effect

	Connect the vertex and pixel shader to the
	current renderer

***************************************/

void BURGER_API Burger::Effect::SetEffect(void)
{
	D3DDevice *pDevice = m_pDevice;
	pDevice->SetPixelShader(m_pPixelShader);
	pDevice->SetVertexShader(m_pVertexShader);
}

#endif