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

/* BEGIN */
namespace Burger {
class DisplayOpenGL : public Display {
public:
	DisplayOpenGL(GameApp *pGameApp);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PostEndScene(void);
public:
	static Word BURGER_API LoadShader(Word GLEnum,const char *pShaderCode);
	static const char * BURGER_API GetErrorString(Word uGLErrorEnum);
	static WordPtr BURGER_API GetGLTypeSize(Word uGLTypeEnum);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static void BURGER_API WindowsLink(void);
	static void BURGER_API WindowsUnlink(void);
private:
	HDC__ *m_pOpenGLDeviceContext;	///< Window to attach the GL context to
	HGLRC__ *m_pOpenGLContext;		///< Current OpenGL context
	Word m_bResolutionChanged;		///< Set to \ref TRUE if the display needs to be restored
#endif
};
}
/* END */

#endif
