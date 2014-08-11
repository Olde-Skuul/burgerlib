/***************************************

	OpenGLXEXT inclusion

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLXEXT_H__
#define __BRGLXEXT_H__

#ifndef __BURGER__
#include <burger.h>
#endif

// iOS uses OpenGLES

#if defined(BURGER_IOS)
#include <OpenGLES/ES2/glxext.h>

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#include <OpenGL/glxext.h>		// glext.h for MacOS X

// Windows requires windows.h before glxext.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>		// Needed before #include <glxext.h> in windows
#include <xa.h>
#include <glxext.h>			// OpenGLXEX for all other platforms
#else

// Generic
#include <gl/glxext.h>				// OpenGLXEXT for all other platforms
#endif

#endif
