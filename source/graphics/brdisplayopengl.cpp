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

#if !defined(DOXYGEN)
#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 0x0503
#endif
#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 0x0504
#endif
#ifndef GL_OUT_OF_MEMORY
#define GL_OUT_OF_MEMORY 0x0505
#endif
#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#endif
#ifndef GL_TABLE_TOO_LARGE
#define GL_TABLE_TOO_LARGE 0x8031
#endif
#ifndef GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#endif
#ifndef GL_COMPRESSED_TEXTURE_FORMATS_ARB
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 0x8CDA
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#endif
#ifndef GL_FRAMEBUFFER_UNSUPPORTED
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#endif

#define CASE(x) { #x,x }
struct MessageLookup_t {
	const char *m_pName;
	Word m_uEnum;
};
#endif

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
	Display(pGameApp,OPENGL),
	m_pCompressedFormats(NULL),
	m_fOpenGLVersion(0.0f),
	m_fShadingLanguageVersion(0.0f),
	m_uCompressedFormatCount(0)
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

	\fn float Burger::DisplayOpenGL::GetOpenGLVersion(void) const
	\brief Return the version of the OpenGL implementation

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0.0f if OpenGL is not started or a valid version number, example: 4.4f.
	
***************************************/

/*! ************************************

	\fn float Burger::DisplayOpenGL::GetShadingLanguageVersion(void) const
	\brief Return the version of the OpenGL Shader compiler

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0.0f if OpenGL is not started or a valid version number for the compiler, example: 4.4f.
	
***************************************/

/*! ************************************

	\fn Word Burger::DisplayOpenGL::GetCompressedFormatCount(void) const
	\brief Return the number of supported compressed texture formats

	When OpenGL is started, it's queried for the number of compressed texture
	formats it natively supports. This will return the number of formats
	and GetCompressedFormats() will be an array of this size
	with the supported formats.

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0 if OpenGL is not started or no texture compression is supported.
	\sa GetCompressedFormats()

***************************************/

/*! ************************************

	\fn const Word *Burger::DisplayOpenGL::GetCompressedFormats(void) const
	\brief Return the pointer to an array of supported compressed texture formats

	When OpenGL is started, it's queried for the number of compressed texture
	formats it natively supports and the types are stored in this array.
	This array size can be retrieved with a call to GetCompressedFormatCount().

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return \ref NULL if OpenGL is not started or no texture compression is supported or an array of GetCompressedFormatCount() in size.
	\sa GetCompressedFormatCount()

***************************************/


/*! ************************************

	\brief Initialize the display for supporting OpenGL
	
	Once OpenGL is started, this function queries the driver for
	the supported feature list and sets up the rendering status for
	best performance in rendering scenes.

	\sa InitContext()
	
***************************************/

#if defined(GL_SUPPORTED) && defined(_DEBUG) && !defined(DOXYGEN)

// Data and a function to dump the OpenGL driver data
// for debugging

struct GLStringIndex_t {
	const char *m_pName;		// Printable name of the GL string
	GLenum m_eEnum;				// OpenGL enumeration to check
};

static const GLStringIndex_t g_StringIndexes[] = {
	{"OpenGL version",GL_VERSION},
	{"Vendor",GL_VENDOR},
	{"Renderer",GL_RENDERER},
	{"Extensions",GL_EXTENSIONS},
	{"Shader Language Version",GL_SHADING_LANGUAGE_VERSION}
};
#endif

void BURGER_API Burger::DisplayOpenGL::SetupOpenGL(void)
{
#if defined(GL_SUPPORTED)
	const char *pString;

#if defined(_DEBUG)

	// For debug version, dump out OpenGL strings to the console
	// or logfile.txt

	{
		WordPtr uCount = BURGER_ARRAYSIZE(g_StringIndexes);
		const GLStringIndex_t *pWork = g_StringIndexes;
		do {
			// Get the string for the enumeration
			pString = reinterpret_cast<const char *>(glGetString(pWork->m_eEnum));
			// If supported, print it
			if (pString) {
				Debug::Message("%s = ",pWork->m_pName);
				// Use String() because pResult can be long enough to overrun the buffer
				Debug::String(pString);
				Debug::String("\n");
			}
			++pWork;
		} while (--uCount);
	}
#endif

	//
	// Obtain the version of OpenGL found
	//

	float fVersion = 0.0f;
	pString = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	if (pString) {
		if (!MemoryCompare("OpenGL ES ",pString,10)) {
			pString += 10;
		}
		fVersion = AsciiToFloat(pString);
	}
	m_fOpenGLVersion = fVersion;

	//
	// Obtain the version of the OpenGL shader compiler
	//

	fVersion = 0.0f;
	pString = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	if (pString) {
		if (!MemoryCompare("OpenGL ES GLSL ES ",pString,18)) {
			pString += 18;
		}
		fVersion = AsciiToFloat(pString);
	}
	m_fShadingLanguageVersion = fVersion;

	//
	// Obtain the supported compressed texture types
	//

	Free(m_pCompressedFormats);
	m_pCompressedFormats = NULL;
	Word uTemp = 0;
	GLint iTemp = 0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB,&iTemp);
#if defined(_DEBUG)
	Debug::Message("GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB = %i\n",iTemp);
#endif
	if (iTemp) {
		GLint iTemp2 = iTemp;
		GLint *pBuffer = static_cast<GLint *>(Alloc(sizeof(GLint)*iTemp2));
		if (pBuffer) {
			glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS_ARB,pBuffer);
			uTemp = static_cast<Word>(iTemp);
			m_pCompressedFormats = reinterpret_cast<Word *>(pBuffer);
			// Dump the list on debug builds
#if defined(_DEBUG)
			do {
				Debug::Message("OpenGL supported compressed format 0x%04X\n",pBuffer[0]);
				++pBuffer;
			} while (--iTemp2);
#endif
		}
	}
	m_uCompressedFormatCount = uTemp;
#endif
}

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
				GLchar *pLog = static_cast<GLchar*>(Alloc(static_cast<WordPtr>(iLogLength)));
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

#if defined(GL_SUPPORTED) && !defined(DOXYGEN)
static const MessageLookup_t g_GetErrorString[] = {
	CASE(GL_NO_ERROR),
	CASE(GL_INVALID_VALUE),
	CASE(GL_INVALID_OPERATION),
	CASE(GL_STACK_OVERFLOW),
	CASE(GL_STACK_UNDERFLOW),
	CASE(GL_OUT_OF_MEMORY),
	CASE(GL_INVALID_FRAMEBUFFER_OPERATION),
	CASE(GL_TABLE_TOO_LARGE),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_FORMATS),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT),
	CASE(GL_FRAMEBUFFER_UNSUPPORTED),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
};
#endif

#if defined(GL_SUPPORTED) || defined(DOXYGEN)
const char * BURGER_API Burger::DisplayOpenGL::GetErrorString(Word uGLErrorEnum)
{
	const char *pResult = "GL_UNKNOWN_ERROR";
	WordPtr uCount = BURGER_ARRAYSIZE(g_GetErrorString);
	const MessageLookup_t *pLookup = g_GetErrorString;
	do {
		if (uGLErrorEnum==pLookup->m_uEnum) {
			pResult = pLookup->m_pName;
			break;
		}
		++pLookup;
	} while (--uCount);
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
