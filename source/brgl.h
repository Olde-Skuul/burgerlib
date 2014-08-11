/***************************************

	OpenGL inclusion

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGL_H__
#define __BRGL_H__

#ifndef __BURGER__
#include <burger.h>
#endif

// iOS uses OpenGLES

#if defined(BURGER_IOS)
#include <OpenGLES/ES2/gl.h>

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#include <OpenGL/gl.h>		// gl.h for MacOS X

// Windows requires windows.h before gl.h

#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>		// Needed before #include <gl.h> in windows
#include <gl.h>				// OpenGL for all other platforms
#else

// Generic
#include <gl/gl.h>			// OpenGL for all other platforms
#endif

#endif
