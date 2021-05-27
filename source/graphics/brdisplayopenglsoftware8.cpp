/***************************************

    8 Bit software renderer on top of OpenGL

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopenglsoftware8.h"

#if defined(BURGER_OPENGL)
#if !defined(DOXYGEN)
#define __BURGER__
#endif

#include "brgl.h"
#include "brglext.h"

//
// OpenGL is a derived class for Windows
// to allow multiple API support. All other
// OpenGL based platforms, this is a base class
//

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#define TextureOpenGL Texture
#define VertexBufferOpenGL VertexBuffer
#endif

//
// Quad for the image rendered, x,y,u,v
//

static const GLfloat gTriangleVertices[] = {
	-1.0f, 1.0f,0.0f,0.0f,
	-1.0f,-1.0f,0.0f,1.0f,
	 1.0f,-1.0f,1.0f,1.0f,
	 1.0f, 1.0f,1.0f,0.0f};

// Simple 2D vertex shader for position and texture UV coordinates

static const char g8BitgVertexShader[] = 
	"#ifdef GL_ES\n"
	"precision highp float;\n"
	"#endif\n"
	"PIPED vec2 vPipedTexcoord;"
	"VERTEX_INPUT vec4 vPosition;"		// Vector for the vertex location
	"VERTEX_INPUT vec4 vTexCoord;"		// Vector for the UV coordinates
	"void main(){"
	"vPipedTexcoord=vTexCoord.xy;"		// Store the value as is to first texture coordinate
	"gl_Position=vPosition;"		// Store the vertex location as is
	"}";

// Simple 2D vertex shader for position and texture UV coordinates

static const char g8BitFragmentShader[] =
	"uniform sampler1D ColorTable;\n"
	"uniform sampler2D IndexTexture;\n"
	"PIPED vec2 vPipedTexcoord;\n"
	"FRAGCOLOR_USED\n"
	"void main(){\n"
	"float myindex=texture2D(IndexTexture,vPipedTexcoord.xy).x;\n"
	"gl_FragColor=texture1D(ColorTable,myindex);\n"
	"}";


Burger::DisplayOpenGLSoftware8::DisplayOpenGLSoftware8(GameApp *pGameApp) :
	DisplayOpenGL(pGameApp),
	m_pBitMap(NULL),
	m_Renderer(),
	m_uBitMapTexture(0),
	m_uPaletteTexture(0),	
	m_uProgram(0),
	m_uVertexShader(0),
	m_uFragmentShader(0),
	m_uvPositionHandle(0),
	m_uUVHandle(0)
{
}


uint_t Burger::DisplayOpenGLSoftware8::Init(uint_t uWidth,uint_t uHeight,uint_t /* uDepth */,uint_t uFlags)
{
	uint_t uResult = DisplayOpenGL::Init(uWidth,uHeight,32,uFlags);
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)
	if (!uResult) {
		uResult = 10;
		// Calculate the bytes per scan line
		m_uDepth = 8;
		// Use DWORD alignment
		uintptr_t uStride = ((m_uWidth*(8>>3))+3)&(~3);
		m_Renderer.SetClip(0,0,static_cast<int>(uWidth),static_cast<int>(uHeight));
		m_Renderer.SetStride(uStride);

		// Create the bitmap buffer
		m_pBitMap = AllocClear(uStride*m_uHeight);
		m_Renderer.SetFrameBuffer(m_pBitMap);

		// Create the palette texture
		glGenTextures(1,&m_uPaletteTexture);
		glBindTexture(GL_TEXTURE_1D,m_uPaletteTexture);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		// Default palette
		glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGB,GL_UNSIGNED_BYTE,m_Palette);
	
		// Create the bitmap texture
		glGenTextures(1,&m_uBitMapTexture);
		glBindTexture(GL_TEXTURE_2D,m_uBitMapTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,static_cast<GLsizei>(m_uWidth),static_cast<GLsizei>(m_uHeight),0,GL_LUMINANCE,GL_UNSIGNED_BYTE,m_pBitMap);
		glViewport(0,0,static_cast<GLsizei>(m_uWidth),static_cast<GLsizei>(m_uHeight));

		// Create the shaders

		GLuint uVertexShader = CompileShader(GL_VERTEX_SHADER,g8BitgVertexShader);
		if (uVertexShader) {
			m_uVertexShader = uVertexShader;

			GLuint uFragmentShader = CompileShader(GL_FRAGMENT_SHADER,g8BitFragmentShader);
			if (uFragmentShader) {
				m_uFragmentShader = uFragmentShader;

				// Link the shaders together
				GLuint uProgram = glCreateProgram();
				if (uProgram) {
					glAttachShader(uProgram,uVertexShader);
					glAttachShader(uProgram,uFragmentShader);
					glLinkProgram(uProgram);
					GLint iLinkStatus = GL_FALSE;
					glGetProgramiv(uProgram,GL_LINK_STATUS,&iLinkStatus);
					if (iLinkStatus != GL_TRUE) {
						glDeleteProgram(uProgram);
					} else {
						// The program linked!
						m_uProgram = uProgram;

						// Use it! It has to be used now or glUniform1() will fail
						glUseProgram(uProgram);

                        m_uvPositionHandle = static_cast<uint_t>(glGetAttribLocation(uProgram, "vPosition"));
						m_uUVHandle = static_cast<uint_t>(glGetAttribLocation(uProgram,"vTexCoord"));

						GLint uIndexTexture = glGetUniformLocation(uProgram,"IndexTexture");
						glUniform1i(uIndexTexture,0);

						GLint uColorTable = glGetUniformLocation(uProgram,"ColorTable");
						glUniform1i(uColorTable,1);
						// All good!
						uResult = 0;
					}
				}
			}
		}
	}
#endif
	return uResult;
}

void Burger::DisplayOpenGLSoftware8::Shutdown(void)
{
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)
		// Dispose of the shader program
	if (m_uProgram) {
		// The shaders have to be orphaned first
		glDetachShader(m_uProgram,m_uVertexShader);
		glDetachShader(m_uProgram,m_uFragmentShader);
		glDeleteProgram(m_uProgram);
		m_uProgram = 0;
	}

	// Dispose of the shaders
	if (m_uVertexShader) {
		glDeleteShader(m_uVertexShader);
		m_uVertexShader = 0;
	}

	if (m_uFragmentShader) {
		glDeleteShader(m_uFragmentShader);
		m_uFragmentShader = 0;
	}

	// Free up the textures
	if (m_uBitMapTexture) {
		glDeleteTextures(1,&m_uBitMapTexture);
		m_uBitMapTexture = 0;
	}
	if (m_uPaletteTexture) {
		glDeleteTextures(1,&m_uPaletteTexture);
		m_uPaletteTexture = 0;
	}

	// If there was a bitmap, kill it.
	if (m_pBitMap) {
		Free(m_pBitMap);
		m_pBitMap = NULL;
	}
#endif
	DisplayOpenGL::Shutdown();
}

void Burger::DisplayOpenGLSoftware8::BeginScene(void)
{
}

void Burger::DisplayOpenGLSoftware8::EndScene(void)
{
#if defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)
	if (m_bPaletteDirty && m_uPaletteTexture) {
		// Upload the new color map
		glBindTexture(GL_TEXTURE_1D,m_uPaletteTexture);
		glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGB,GL_UNSIGNED_BYTE,m_Palette);
		m_bPaletteDirty = FALSE;
	}
	glUseProgram(m_uProgram);
	// Bind the 8 bit texture map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,m_uBitMapTexture);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,static_cast<GLsizei>(m_uWidth),static_cast<GLsizei>(m_uHeight),GL_LUMINANCE,GL_UNSIGNED_BYTE,m_pBitMap);
	// Bind the 256 color palette
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,m_uPaletteTexture);
	// Enable the hard coded vertex/uv coordinates
	glEnableVertexAttribArray(m_uvPositionHandle);
	glEnableVertexAttribArray(m_uUVHandle);
	glVertexAttribPointer(m_uvPositionHandle,2,GL_FLOAT,GL_FALSE,16,gTriangleVertices);
	glVertexAttribPointer(m_uUVHandle,2,GL_FLOAT,GL_FALSE,16,gTriangleVertices+2);
	// Draw the image
	glDrawArrays(GL_QUADS,0,4);
#endif
	DisplayOpenGL::EndScene();
}

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayOpenGLSoftware8,Burger::DisplayOpenGL);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayOpenGLSoftware8::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/

#endif
