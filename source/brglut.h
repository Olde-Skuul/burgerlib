/***************************************

	OpenGLUT inclusion

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRGL_H__
#include <brgl.h>
#endif

// Mac OS Carbon/Classic

#if defined(BURGER_MAC)
#ifndef __glut_h__
#include <glut.h>
#endif

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#ifndef __glu_h_
#include <OpenGL/glu.h>		// glut.h for MacOS X
#endif

// Windows requires windows.h before glut.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINDOWS_
#include <windows.h>		// Needed before #include <glut.h> in windows
#endif
#ifndef __glut_h__
#include <glut.h>
#endif
#endif

#endif

