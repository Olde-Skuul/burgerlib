/***************************************

	OpenGL manager class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopengl.h"

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_WINDOWS)) && !defined(DOXYGEN)
#define GL_SUPPORTED
#endif

// Don't include burger.h
#define __BURGER__

#if defined(GL_SUPPORTED)
#include "brgl.h"
#include "brglext.h"
#endif
#include "brdebug.h"

/*! ************************************

	\class Burger::DisplayOpenGL

	\brief OpenGL screen setup

	Base class for instantiating a video display using OpenGL

	\sa Burger::RendererBase, Burger::DisplayBase, Burger::Display, Burger::DisplayDirectX9, Burger::DisplayDirectX11

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

/*! ************************************

	\brief Initialize OpenGL

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::~DisplayOpenGL()

***************************************/

Burger::DisplayOpenGL::DisplayOpenGL(Burger::GameApp *pGameApp) :
	Display(pGameApp,OPENGL)
{
	SetRenderer(NULL);
}

/*! ************************************

	\brief Start up the OpenGL context

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

Word Burger::DisplayOpenGL::InitContext(void)
{
	return 10;
}

/*! ************************************

	\brief Start up the OpenGL context

	Shut down OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

void Burger::DisplayOpenGL::PostShutdown(void)
{
}
#endif

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

/*! ************************************

	\brief Update the video display

	Calls SwapBuffers() in OpenGL to draw the rendered scene

	\sa Burger::DisplayOpenGL::PreBeginScene()

***************************************/

void Burger::DisplayOpenGL::PostEndScene(void)
{
}
#endif

/*! ************************************

	\brief Compile an OpenGL shader
	
	Given a string that has the source to a shader, compile it
	with OpenGL's GLSL compiler.

	\param GLEnum OpenGL shader enum
	\param pShaderCode "C" string of the source code of the shader to compile
	\return Zero if the code can't be compiled, non-zero is a valid OpenGL shader reference
	
***************************************/

#if defined(GL_SUPPORTED) || defined(DOXYGEN)
Word BURGER_API Burger::DisplayOpenGL::LoadShader(Word GLEnum,const char *pShaderCode)
{
	// Create a blank shader
	GLuint uShader = glCreateShader(GLEnum);
	if (uShader) {
		// Upload the source
		glShaderSource(uShader,1,&pShaderCode,NULL);
		glCompileShader(uShader);
		GLint bCompiled = GL_FALSE;
		// Did it compile okay?
		glGetShaderiv(uShader,GL_COMPILE_STATUS,&bCompiled);
		if (bCompiled==GL_FALSE) {
			// Dump out what happened so a programmer
			// can debug the faulty shader
			GLint iLogLength;
			glGetShaderiv(uShader,GL_INFO_LOG_LENGTH,&iLogLength);
			if (iLogLength > 1) {
				// iLogLength includes space for the terminating null at the end of the string
				GLchar *pLog = static_cast<GLchar*>(Alloc(iLogLength));
				glGetShaderInfoLog(uShader,iLogLength,&iLogLength,pLog);
				Debug::Message("Shader compile log:\n%s\n",pLog);
				Free(pLog);
			}
			glDeleteShader(uShader);
			uShader = 0;
		}
	}
	return uShader;
}
#else
Word BURGER_API Burger::DisplayOpenGL::LoadShader(Word /* GLEnum */,const char * /* pShaderCode */)
{
	return 0;
}
#endif

/*! ************************************

	\brief Convert an OpenGL error enumeration into a string
	
	Given an enum from a call to glGetError(), call this function 
	convert the number into a string describing the error.

	\param uGLErrorEnum OpenGL error enum
	\return Pointer to a "C" string with the error message (Don't dispose)
	
***************************************/

#if defined(GL_SUPPORTED) || defined(DOXYGEN)
const char * BURGER_API Burger::DisplayOpenGL::GetErrorString(Word uGLErrorEnum)
{
	const char *pResult;
	switch (uGLErrorEnum) {
	case GL_NO_ERROR:
		pResult = "GL_NO_ERROR";
		break;
	case GL_INVALID_ENUM:
		pResult = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		pResult = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		pResult = "GL_INVALID_OPERATION";
		break;
// Some machines don't support these errors
#if defined(GL_OUT_OF_MEMORY)
	case GL_OUT_OF_MEMORY:
		pResult = "GL_OUT_OF_MEMORY";
		break;
#endif
#if defined(GL_INVALID_FRAMEBUFFER_OPERATION)
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		pResult = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
#endif
#if defined(GL_STACK_OVERFLOW)
	case GL_STACK_OVERFLOW:
		pResult = "GL_STACK_OVERFLOW";
		break;
#endif
#if defined(GL_STACK_UNDERFLOW)
	case GL_STACK_UNDERFLOW:
		pResult = "GL_STACK_UNDERFLOW";
		break;
#endif
#if defined(GL_TABLE_TOO_LARGE)
	case GL_TABLE_TOO_LARGE:
		pResult = "GL_TABLE_TOO_LARGE";
		break;
#endif
	default:
		pResult = "GL_UNKNOWN_ERROR";
		break;
	}
	return pResult;
}
#else
const char * BURGER_API Burger::DisplayOpenGL::GetErrorString(Word /* uGLErrorEnum */)
{
	return "GL_UNKNOWN_ERROR";
}
#endif

/*! ************************************

	\brief Determine an OpenGL type enumeration byte length

	Given an OpenGL data type, such as GL_BYTE, GL_SHORT, or GL_FLOAT,
	return the number of bytes such a data chunk would occupy

	\param uGLTypeEnum OpenGL data type enum
	\return Number of bytes for the type or 0 if unknown
	
***************************************/

#if defined(GL_SUPPORTED) || defined(DOXYGEN)
WordPtr BURGER_API Burger::DisplayOpenGL::GetGLTypeSize(Word uGLTypeEnum)
{
	WordPtr uResult;
	switch (uGLTypeEnum) {
	case GL_BYTE:
		uResult = sizeof(GLbyte);
		break;
	case GL_UNSIGNED_BYTE:
		uResult = sizeof(GLubyte);
		break;
	case GL_SHORT:
		uResult = sizeof(GLshort);
		break;
	case GL_UNSIGNED_SHORT:
		uResult = sizeof(GLushort);
		break;
	case GL_INT:
		uResult = sizeof(GLint);
		break;
	case GL_UNSIGNED_INT:
		uResult = sizeof(GLuint);
		break;
	case GL_FLOAT:
		uResult = sizeof(GLfloat);
		break;
#if defined(GL_2_BYTES)
	case GL_2_BYTES:
		uResult = 2;
		break;
#endif
#if defined(GL_3_BYTES)
	case GL_3_BYTES:
		uResult = 3;
		break;
#endif
#if defined(GL_4_BYTES)
	case GL_4_BYTES:
		uResult = 4;
		break;
#endif
#if defined(GL_DOUBLE)
	case GL_DOUBLE:
		uResult = sizeof(GLdouble);
		break;
#endif
	default:
		uResult = 0;
	}
	return uResult;
}
#else
WordPtr BURGER_API Burger::DisplayOpenGL::GetGLTypeSize(Word /* uGLTypeEnum */)
{
	return 0;
}
#endif

/*! ************************************
 
	\brief Poll OpenGL for errors and print them
 
	Call glGetError(), and if any errors were found, 
	print them using Debug::Warning().
 
	Used for debugging OpenGL
 
	\param pErrorLocation Pointer to a string that describes where this
		error condition could be occurring.
 
	\return \ref TRUE if an error was found, \ref FALSE if not
 
***************************************/

#if defined(GL_SUPPORTED) || defined(DOXYGEN)

Word BURGER_API Burger::DisplayOpenGL::PrintGLError(const char *pErrorLocation)
{
	Word uResult = FALSE;
	GLenum eError = glGetError();
	if (eError != GL_NO_ERROR) {
		do {
			Debug::Message("GLError %s set in location %s\n",GetErrorString(static_cast<Word>(eError)),pErrorLocation);
			eError = glGetError();
		} while (eError != GL_NO_ERROR);
		uResult = TRUE;
	}
	return uResult;
}
#else
Word BURGER_API Burger::DisplayOpenGL::PrintGLError(const char *pErrorLocation)
{
	Debug::Warning("OpenGL is not supported in location %s\n",pErrorLocation);
	return TRUE;
}
#endif
