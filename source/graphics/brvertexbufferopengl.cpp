/***************************************

    Vertex buffer class for OpenGL

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbufferopengl.h"

#if defined(BURGER_OPENGL)
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "brdisplayopengl.h"

#define __BURGER__
#include "brgl.h"
#include "brglext.h"

//
// OpenGL is a derived class for Windows
// to allow multiple API support. All other
// OpenGL based platforms, this is a base class
//

#if !defined(BURGER_WINDOWS) && !defined(DOXYGEN)
#define VertexBufferOpenGL VertexBuffer
#define DisplayOpenGL Display
#else

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBufferOpenGL,Burger::VertexBuffer);
#endif

#endif

//
// Initialize the vertex buffer code
//

#if defined(BURGER_WINDOWS)
Burger::VertexBufferOpenGL::VertexBufferOpenGL()
{
}

Burger::VertexBufferOpenGL::~VertexBufferOpenGL()
{
	Release(NULL);
}

#endif

//
// Load the vertex data
//

#if !defined(DOXYGEN)
static const GLenum g_GLTypes[5] = {
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_UNSIGNED_BYTE
};

static const GLboolean g_ChunkNormals[5] = {GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE};

#endif

uint_t Burger::VertexBufferOpenGL::CheckLoad(Display *pDisplay)
{
	uint_t uResult = 0;
	if (m_uFlags & FLAG_VERTEXDIRTY) {
		VertexBufferOpenGL::Release(pDisplay);

		// Create a vertex array object
		GLuint uBufferID;
		glGenVertexArrays(1,&uBufferID);
		uResult = 10;
		if (uBufferID) {
			m_Platform.m_GL.m_uVertexArrayObjectID = uBufferID;
			glBindVertexArray(uBufferID);

			uResult = 0;
			//
			// Are there vertex positions?
			//

			if (m_uVertexArraySize) {

				// Create a vertex buffer object to store positions
				glGenBuffers(1,&uBufferID);
				if (!uBufferID) {
					uResult = 10;
				} else {

					m_Platform.m_GL.m_uBufferID = uBufferID;
					glBindBuffer(GL_ARRAY_BUFFER,uBufferID);
					// Allocate and load position data into the Vertex Buffer Object
					GLenum uUsage = GL_STATIC_DRAW;
					if (m_uFlags&FLAGAOS_VERTICES_DYNAMIC) {
						uUsage = GL_STREAM_DRAW;
					}
					// Upload vertex data (Or reserve data if m_pVertexArray is NULL)
					glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(m_uVertexArraySize),m_pVertexArray,uUsage);

					uint_t uCount = m_uMemberCount;
					if (uCount) {

						// glVertexAttribPointer() requires the stride precalculated,
						// so do a pass to get the full stride of the vertex data
						uint_t i = 0;
						uint8_t *pDest = 0;
						const uint_t *pMembers = m_pMembers;
						do {
							// Enable the position attribute for this Vertex Buffer Object
							glEnableVertexAttribArray(i);
							uint_t uType = (pMembers[0]&USAGE_CHUNKMASK)>>USAGE_CHUNKMASKSHIFT;

							// Set up the description of the position array
							glVertexAttribPointer(i,static_cast<GLint>(g_ChunkElementCounts[uType]),g_GLTypes[uType],g_ChunkNormals[uType],static_cast<GLsizei>(m_uStride),pDest);
							pDest += g_ChunkElementSizes[uType]*g_ChunkElementCounts[uType];
							++pMembers;

							// Check if going out of bounds for OpenGL
							if (++i>static_cast<DisplayOpenGL *>(pDisplay)->GetMaximumVertexAttributes()) {
								uResult = 10;
								break;
							}
						} while (i<uCount);
					}
				}
			}

			if (m_uElementSize) {
				// Attach the array of elements to the Vertex Buffer Object
				glGenBuffers(1,&uBufferID);
				if (!uBufferID) {
					uResult = 10;
				} else {
					m_Platform.m_GL.m_uElementID = uBufferID;
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,uBufferID);
					// Allocate and load vertex array element data into Vertex Buffer Object
					glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(m_uElementSize),m_pElements,static_cast<GLenum>(GL_STATIC_DRAW));
				}
			}
		}
		m_uFlags &= (~FLAG_VERTEXDIRTY);
	}
	return uResult;
}

//
// Release the vertex buffer data
//

void Burger::VertexBufferOpenGL::Release(Display *)
{
	if (m_Platform.m_GL.m_uVertexArrayObjectID) {
		// For every possible attribute set in the vertex array object
		GLuint uBufferID;

		if (m_Platform.m_GL.m_uBufferID) {
			uBufferID = static_cast<GLuint>(m_Platform.m_GL.m_uBufferID);
			glDeleteBuffers(1,&uBufferID);
			m_Platform.m_GL.m_uBufferID = 0;
		}

		// If there was a element array vertex buffer object set in the vertex array object
		if (m_Platform.m_GL.m_uElementID) {
			//...delete the VBO
			uBufferID = static_cast<GLuint>(m_Platform.m_GL.m_uElementID);
			glDeleteBuffers(1,&uBufferID);
			m_Platform.m_GL.m_uElementID = 0;
		}
		// Finally, delete the vertex array object
		uBufferID = static_cast<GLuint>(m_Platform.m_GL.m_uVertexArrayObjectID);
		glDeleteVertexArrays(1,&uBufferID);
		m_Platform.m_GL.m_uVertexArrayObjectID = 0;
	}
	m_uFlags |= FLAG_VERTEXDIRTY;
}

#endif
