/***************************************

	OpenGLXEXT inclusion

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRGL_H__
#include <brgl.h>
#endif

// iOS uses OpenGLES

#if defined(BURGER_IOS)
#ifndef __gl_es20ext_h_
#include <OpenGLES/ES2/glext.h>
#endif

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#ifndef __glext_h_
#include <OpenGL/glext.h>		// glext.h for MacOS X
#endif

// Windows requires windows.h before glxext.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINDOWS_
#include <windows.h>		// Needed before #include <glxext.h> in windows
#endif
#ifndef XA_H
#include <xa.h>
#endif
#ifndef __glxext_h_
#include <glxext.h>			// OpenGLXEX for all other platforms
#endif
#endif

#endif
