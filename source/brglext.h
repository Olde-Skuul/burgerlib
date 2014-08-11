/***************************************

	OpenGLEXT inclusion

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLEXT_H__
#define __BRGLEXT_H__

#ifndef __BURGER__
#include <burger.h>
#endif

// iOS uses OpenGLES

#if defined(BURGER_IOS)
#include <OpenGLES/ES2/glext.h>
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#include <OpenGL/glext.h>		// glext.h for MacOS X
#if !CGL_VERSION_1_3
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glGenerateMipmap glGenerateMipmapEXT
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

// Windows requires windows.h before glext.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>		// Needed before #include <glext.h> in windows
#if !defined(GL_GLEXT_PROTOTYPES)
#define GL_GLEXT_PROTOTYPES
#endif
#include <glext.h>
#else

// Generic
#include <gl/glext.h>				// OpenGLEXT for all other platforms
#endif

#endif
