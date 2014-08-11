/***************************************

	OpenGLUT inclusion

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGLUT_H__
#define __BRGLUT_H__

#ifndef __BURGER__
#include <burger.h>
#endif

// MacOSX used an OpenGL Framework
#if __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#include <OpenGL/glu.h>		// glut.h for MacOS X

// Windows requires windows.h before glut.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>		// Needed before #include <glut.h> in windows
#include <glut.h>
#else

// Generic
#include <gl/glut.h>			// OpenGL for all other platforms
#endif

#endif

