/***************************************

	OpenGL inclusion

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#ifndef __gl_es20_h_
#include <OpenGLES/ES2/gl.h>
#endif

// Mac OS Carbon/Classic

#elif defined(BURGER_MAC)
#ifndef __gl_h__
#include <gl.h>
#endif

// MacOSX used an OpenGL Framework

#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#ifndef __gl_h_
#include <OpenGL/gl.h>		// gl.h for MacOS X
#endif

// Windows requires windows.h before gl.h

#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINDOWS_
#include <windows.h>		// Needed before #include <gl.h> in windows
#endif
#ifndef __gl_h__
#include <gl.h>				// OpenGL for all other platforms
#endif

// Android / Ouya / Shield

#elif defined(BURGER_SHIELD)
#ifndef __gl2_h_
#include <gles2/gl2.h>
#endif
#define BURGER_OPENGLES

#endif

//
// Cover values that may not be defined on some platform headers
//

#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 0x0503
#endif
#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 0x0504
#endif
#ifndef GL_OUT_OF_MEMORY
#define GL_OUT_OF_MEMORY 0x0505
#endif
#ifndef GL_TABLE_TOO_LARGE
#define GL_TABLE_TOO_LARGE 0x8031
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#endif
#ifndef GL_FRAMEBUFFER_UNSUPPORTED
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#endif

#if defined(BURGER_OPENGLES)
#define glClearDepth glClearDepthf
#endif

#endif
