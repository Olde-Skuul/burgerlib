/***************************************

	OpenGL manager class

	MacOSX only

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopengl.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brdebug.h"
#include "brmacosxapp.h"

#if !defined(DOXYGEN)
#define GL_GLEXT_PROTOTYPES
#endif

#include <OpenGL/gl.h>
#undef GL_EXT_paletted_texture
#include <OpenGL/glext.h>

/*! ************************************

	\brief Initialize OpenGL

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::~DisplayOpenGL()

***************************************/

Burger::DisplayOpenGL::DisplayOpenGL(Burger::GameApp *pGameApp) :
	Display(pGameApp,OPENGL)
//	m_pOpenGLDeviceContext(NULL),
//	m_pOpenGLContext(NULL)
{
	SetRenderer(NULL);
}

/*! ************************************

	\brief Start up the OpenGL context

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

Word Burger::DisplayOpenGL::InitContext(void)
{
	PostShutdown();

	// Set the new size of the screen
	Word uWidth = m_uWidth;
	Word uHeight = m_uHeight;
	m_uFlags |= FULLPALETTEALLOWED;

	// Resize the display the window
//	static_cast<WindowsApp *>(m_pGameApp)->SetWindowSize(uWidth,uHeight);

	// Get the video contexts so drawing can commence
//	HWND pWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();
//	m_pOpenGLDeviceContext = GetDC(pWindow);

	// Determine the true color pixel format the system can use
	// Commented out lines that were storing only zeros since MemoryClear()
	// took care of that already.

//	PIXELFORMATDESCRIPTOR PixelFormat;
//	MemoryClear(&PixelFormat,sizeof(PixelFormat));
//	PixelFormat.nSize = sizeof(PixelFormat);
//	PixelFormat.nVersion = 1;					// Don't change.
//	PixelFormat.dwFlags = PFD_DRAW_TO_WINDOW	// support window
//		| PFD_SUPPORT_OPENGL					// support OpenGL
//		| PFD_DOUBLEBUFFER ,					// double buffered
	//PixelFormat.iPixelType = PFD_TYPE_RGBA;		// True color
//	PixelFormat.cColorBits = 32;				// True color
	//PixelFormat.cDepthBits = 0;					// No ZBuffer
	//PixelFormat.cStencilBits = 0;
	//PixelFormat.cAlphaBits = 0;
	//PixelFormat.iLayerType = PFD_MAIN_PLANE;	// Main layer
//	HGLRC hGLContext = NULL;

	// Get the pixel format
//	int iPixelFormat = ChoosePixelFormat(m_pOpenGLDeviceContext,&PixelFormat);
//	if (iPixelFormat) {
		// Try setting to this context
//		if (SetPixelFormat(m_pOpenGLDeviceContext,iPixelFormat,&PixelFormat) ) {
//			hGLContext = wglCreateContext(m_pOpenGLDeviceContext);
//			if (hGLContext) {
//				if (!wglMakeCurrent(m_pOpenGLDeviceContext,hGLContext)) {
//					wglDeleteContext(hGLContext);
//					hGLContext = NULL;
//				} else {
//					m_uDepth = 32;
//				}
//			}
//		}
//	}
	
	// Fall back plan, try 24 bit color
//	if (!hGLContext) {
//		PixelFormat.cColorBits = 24;
//		iPixelFormat = ChoosePixelFormat(m_pOpenGLDeviceContext,&PixelFormat);
//		if (iPixelFormat) {
//			if (SetPixelFormat(m_pOpenGLDeviceContext,iPixelFormat,&PixelFormat)) {
//				hGLContext = wglCreateContext(m_pOpenGLDeviceContext);
//				if (hGLContext) {
//					if (!wglMakeCurrent(m_pOpenGLDeviceContext,hGLContext)) {
//						wglDeleteContext(hGLContext);
//						hGLContext = NULL;
//					} else {
//						m_uDepth = 24;
//					}
//				}
//			}
//		}
//	}
	// Boned?
//	if (!hGLContext) {
		// Release the device
//		ReleaseDC(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),m_pOpenGLDeviceContext);
//		m_pOpenGLDeviceContext = NULL;
//		return 10;
//	}

//	m_pOpenGLContext = hGLContext;

	// Now that a context has been selected, load OpenGL's functions
//	WindowsLink();
	return 0;
}

/*! ************************************

	\brief Start up the OpenGL context

	Shut down OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

void Burger::DisplayOpenGL::PostShutdown(void)
{
	// Release everything else
//	WindowsUnlink();

//	if (m_pOpenGLContext) {
		// Force the system to use the default context, not this one.
//		wglMakeCurrent(NULL,NULL);
//		wglDeleteContext(m_pOpenGLContext);
//		m_pOpenGLContext = NULL;
//	}

	// Do I have a device?
//	if (m_pOpenGLDeviceContext) {
		// Release the device
//		ReleaseDC(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),m_pOpenGLDeviceContext);
//		m_pOpenGLDeviceContext = NULL;
		// Because there was a video context, capture the location
		// of the window, so if the window was re-opened, use it's
		// old location.

//		static_cast<WindowsApp *>(m_pGameApp)->RecordWindowLocation();
//	}
}

/*! ************************************

	\brief Update the video display

	Calls SwapBuffers() in OpenGL to draw the rendered scene

	\sa Burger::DisplayOpenGL::PreBeginScene()

***************************************/

void Burger::DisplayOpenGL::PostEndScene(void)
{
	// Consider it done!
//	SwapBuffers(m_pOpenGLDeviceContext);
//	ValidateRect(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),NULL);
}

#endif
