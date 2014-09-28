/***************************************

	OpenGL manager class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRDISPLAYOPENGL_H__
#define __BRDISPLAYOPENGL_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
namespace Burger {
class DisplayOpenGL : public Display {
	Word *m_pCompressedFormats;		///< Pointer to an array of supported OpenGL compressed textures
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	HDC__ *m_pOpenGLDeviceContext;	///< Window to attach the GL context to
	HGLRC__ *m_pOpenGLContext;		///< Current OpenGL context
	Word m_bResolutionChanged;		///< Set to \ref TRUE if the display needs to be restored
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
private:
	NSView *m_pView;
	NSWindowController *m_pWindowController;
	NSOpenGLView *m_pOpenGLView;
	_CGLContextObject *m_pOpenGLContext;
	NSWindow *m_pFullScreenWindow;
#endif
	float m_fOpenGLVersion;
	float m_fShadingLanguageVersion;
	Word m_uCompressedFormatCount;
public:
	DisplayOpenGL(GameApp *pGameApp);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
public:
	BURGER_INLINE float GetOpenGLVersion(void) const { return m_fOpenGLVersion; }
	BURGER_INLINE float GetShadingLanguageVersion(void) const { return m_fShadingLanguageVersion; }
	BURGER_INLINE Word GetCompressedFormatCount(void) const { return m_uCompressedFormatCount; }
	BURGER_INLINE const Word *GetCompressedFormats(void) const { return m_pCompressedFormats; }
	void BURGER_API SetupOpenGL(void);
	static Word BURGER_API LoadShader(Word GLEnum,const char *pShaderCode);
	static const char * BURGER_API GetErrorString(Word uGLErrorEnum);
	static WordPtr BURGER_API GetGLTypeSize(Word uGLTypeEnum);
	static Word BURGER_API PrintGLError(const char *pErrorLocation);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static void BURGER_API WindowsLink(void);
	static void BURGER_API WindowsUnlink(void);
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	BURGER_INLINE NSView *GetView(void) const { return m_pView; }
	BURGER_INLINE NSWindowController *GetWindowController(void) const { return m_pWindowController; }
	BURGER_INLINE NSOpenGLView *GetOpenGLView(void) const { return m_pOpenGLView; }
	BURGER_INLINE _CGLContextObject *GetOpenGLContext(void) const { return m_pOpenGLContext; }
	BURGER_INLINE NSWindow *GetFullScreenWindow(void) const { return m_pFullScreenWindow; }
	BURGER_INLINE void SetFullScreenWindow(NSWindow *pFullScreenWindow) { m_pFullScreenWindow = pFullScreenWindow; }
protected:
	virtual void PreBeginScene(void);
#endif
};
}
/* END */

#endif
