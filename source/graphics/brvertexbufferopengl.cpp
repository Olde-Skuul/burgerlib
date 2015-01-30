/***************************************

	Vertex buffer class for OpenGL

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbufferopengl.h"

#if defined(BURGER_OPENGL_SUPPORTED)
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

#if !defined(BURGER_WINDOWS)
#define VertexBufferOpenGL VertexBuffer
#define DisplayOpenGL Display
#else
BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBufferOpenGL,Burger::VertexBuffer);
#endif

//
// Initialize the vertex buffer code
//

Burger::VertexBufferOpenGL::VertexBufferOpenGL() :
	m_uVertexArrayObjectID(0),
	m_uElementID(0),
	m_uBufferID(0),
	m_uArrayEntryCount(0),
	m_uElementEntryCount(0)
{
}

//
// Dispose of the vertex buffer
//

Burger::VertexBufferOpenGL::~VertexBufferOpenGL()
{
	VertexBufferOpenGL::ReleaseData();
}

//
// Load the vertex data
//

Word Burger::VertexBufferOpenGL::LoadData(Display *pDisplay,const VertexAoS_t *pDescription)
{
	VertexBufferOpenGL::ReleaseData();
	Word uResult = 0;
	if (pDescription) {

		// Create a vertex array object
		GLuint uBufferID;
		glGenVertexArrays(1,&uBufferID);
		uResult = 10;
		if (uBufferID) {
			m_uVertexArrayObjectID = uBufferID;
			glBindVertexArray(uBufferID);

			uResult = 0;
			//
			// Are there vertex positions?
			//

			if (pDescription->m_uVertexArraySize) {

				// Create a vertex buffer object to store positions
				glGenBuffers(1,&uBufferID);
				if (!uBufferID) {
					uResult = 10;
				} else {

					m_uBufferID = uBufferID;
					glBindBuffer(GL_ARRAY_BUFFER,uBufferID);
					// Allocate and load position data into the Vertex Buffer Object
					glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uVertexArraySize),pDescription->m_pVertexArray,static_cast<GLenum>(pDescription->m_pVertexArray ? GL_STATIC_DRAW : GL_STREAM_DRAW));
					
					const Word *pMembers = pDescription->m_pMembers;
					if (pMembers) {
						Word uMember = pMembers[0];
						if (uMember) {

							Word uOffset = 0;
							Word uAttributeCount = 0;
							do {
								Word uType = (uMember&0x30);
								Word uEntryCount = (uMember&USAGE_COUNTMASK);
								if (uType==USAGE_FLOAT) {
									BURGER_ASSERT(uEntryCount>=1 && uEntryCount<5);
									uOffset += uEntryCount*4;
								} else {
									BURGER_ASSERT(uType!=USAGE_FLOAT);
									uResult = 10;
									break;
								}
								++pMembers;

								// Buffer overflow?
								if (++uAttributeCount>static_cast<DisplayOpenGL *>(pDisplay)->GetMaximumVertexAttributes()) {
									// Force error
									uResult = 10;
									break;
								}
								uMember = pMembers[0];
							} while (uMember);
							m_uArrayEntryCount = static_cast<Word>(pDescription->m_uVertexArraySize/uOffset);
							if (!uResult) {
								Word i = 0;
								Word8 *pDest = 0;
								pMembers = pDescription->m_pMembers;
								do {
									// Enable the position attribute for this Vertex Buffer Object
									glEnableVertexAttribArray(i);
									// Set up the description of the position array
									glVertexAttribPointer(i,static_cast<GLint>(pMembers[0]&0xF),GL_FLOAT,GL_FALSE,static_cast<GLsizei>(uOffset),pDest);
									pDest+=((pMembers[0]&0xF)*sizeof(float));
									++pMembers;
								} while (++i<uAttributeCount);
							}
						}
					}
				}
			}

			if (pDescription->m_uElementSize) {
				m_uElementEntryCount = static_cast<Word>(pDescription->m_uElementSize>>1);
				// Attach the array of elements to the Vertex Buffer Object
				glGenBuffers(1,&uBufferID);
				if (!uBufferID) {
					uResult = 10;
				} else {
					m_uElementID = uBufferID;
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,uBufferID);
					// Allocate and load vertex array element data into Vertex Buffer Object
					glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uElementSize),pDescription->m_pElements,static_cast<GLenum>(pDescription->m_pElements ? GL_STATIC_DRAW : GL_STREAM_DRAW));
				}
			}
		}
	}
	return uResult;
}

//
// Release the vertex buffer data
//

void Burger::VertexBufferOpenGL::ReleaseData(void)
{
	if (m_uVertexArrayObjectID) {
		// For every possible attribute set in the vertex array object
		GLuint uBufferID;

		if (m_uBufferID) {
			uBufferID = static_cast<GLuint>(m_uBufferID);
			glDeleteBuffers(1,&uBufferID);
			m_uBufferID = 0;
		}

		// If there was a element array vertex buffer object set in the vertex array object
		if (m_uElementID) {
			//...delete the VBO
			uBufferID = static_cast<GLuint>(m_uElementID);
			glDeleteBuffers(1,&uBufferID);
			m_uElementID = 0;
		}
		// Finally, delete the vertex array object
		uBufferID = static_cast<GLuint>(m_uVertexArrayObjectID);
		glDeleteVertexArrays(1,&uBufferID);
		m_uVertexArrayObjectID = 0;
	}
}

#endif
