/***************************************

	OpenGLEXT inclusion

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRGL_H__
#include <brgl.h>
#endif

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

// iOS uses OpenGLES

#if defined(BURGER_IOS)
#ifndef __gl_es20ext_h_
#include <OpenGLES/ES2/glext.h>
#endif
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glDiscardFramebuffer glDiscardFramebufferEXT

// Mac OS Carbon/Classic

#elif defined(BURGER_MAC)
#ifndef __glext_h_
#include <glext.h>
#endif
#define glActiveTexture glActiveTextureARB

// MacOSX used an OpenGL Framework
#elif __CF_USE_FRAMEWORK_INCLUDES__ || defined(BURGER_MACOSX)
#ifndef __glext_h_
#include <OpenGL/glext.h>		// glext.h for MacOS X
#endif
#if !CGL_VERSION_1_3
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glGenerateMipmap glGenerateMipmapEXT
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif
#if !GL_ARB_framebuffer_object
#define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivEXT
#define glDeleteRenderbuffers glDeleteRenderbuffersEXT
#define glGenRenderbuffers glGenRenderbuffersEXT
#define glBindRenderbuffer glBindRenderbufferEXT
#define glRenderbufferStorage glRenderbufferStorageEXT
#define glGenFramebuffers glGenFramebuffersEXT
#define glBindFramebuffer glBindFramebufferEXT
#define glFramebufferTexture2D glFramebufferTexture2DEXT
#define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define glCheckFramebufferStatus glCheckFramebufferStatusEXT
#define glBindFramebuffer glBindFramebufferEXT
#define glDeleteFramebuffers glDeleteFramebuffersEXT
#endif

// Windows requires windows.h before glext.h
#elif defined(BURGER_WINDOWS)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINDOWS_
#include <windows.h>		// Needed before #include <glext.h> in windows
#endif
#ifndef __glext_h_
#include <glext.h>
#endif
#define glGenerateMipmap glGenerateMipmapEXT

// Android / Ouya / Shield

#elif defined(BURGER_SHIELD)
#ifndef __gl2ext_h_
#include <gles2/gl2ext.h>
#endif
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#endif

//
// Cover values that may not be defined on some platform headers
//

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#endif
#ifndef GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#endif
#ifndef GL_COMPRESSED_TEXTURE_FORMATS_ARB
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#endif
#ifndef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#endif
#ifndef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
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
#ifndef GL_MAX_COLOR_ATTACHMENTS
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT 0x8D00
#endif
#ifndef GL_STENCIL_ATTACHMENT
#define GL_STENCIL_ATTACHMENT 0x8D20
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER 0x8D41
#endif

#endif
