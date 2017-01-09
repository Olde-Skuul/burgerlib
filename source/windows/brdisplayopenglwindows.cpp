/***************************************

	OpenGL manager class

	Windows only

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopengl.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brgameapp.h"

#if !defined(DOXYGEN)
#define GL_GLEXT_PROTOTYPES
#endif

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <gl.h>
#undef GL_EXT_paletted_texture
#include <glext.h>

#if defined(BURGER_WATCOM)
#pragma library ("opengl32.lib");
#else
#if defined(BURGER_WIN64)
#pragma comment(lib,"opengl64.lib")
#else
#pragma comment(lib,"opengl32.lib")
#endif
#endif

/***************************************

	Scan all of the modes and look for a match.

	Return TRUE if a match was found

***************************************/

#if !defined(DOXYGEN)
static Word ScanVideoModes(DEVMODEW *pOutput,Word uWidth,Word uHeight,Word uDepth)
{
	Burger::MemoryClear(pOutput,sizeof(DEVMODEW));
	DWORD uModeNumber=0;
	Word bFoundOne = FALSE;
	for (;;) {
		DEVMODEW TestMode;
		Burger::MemoryClear(&TestMode,sizeof(TestMode));	// Clear it out
		TestMode.dmSize = sizeof(TestMode);			// Prep the data table size
		// End of the list?
		if (EnumDisplaySettingsW(NULL,uModeNumber,&TestMode)==FALSE) {
			break;
		}

		// See if this is a match
		if ((TestMode.dmBitsPerPel == uDepth) &&		// Match the size?
			(TestMode.dmPelsWidth == uWidth) &&		// Look for the highest frequency
			(TestMode.dmPelsHeight == uHeight) &&
			(TestMode.dmDisplayFrequency > pOutput->dmDisplayFrequency)) {
				// Try setting this mode
				TestMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
				if (ChangeDisplaySettingsExW(NULL,&TestMode,NULL,CDS_TEST,NULL) == DISP_CHANGE_SUCCESSFUL) {
					// this is the best mode so far
					Burger::MemoryCopy(pOutput,&TestMode,sizeof(DEVMODEW));
					bFoundOne = TRUE;
				}
		}
		++uModeNumber;
	}
	return bFoundOne;
}
#endif

/***************************************

	\brief Initialize OpenGL

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::~DisplayOpenGL()

***************************************/

Burger::DisplayOpenGL::DisplayOpenGL(Burger::GameApp *pGameApp) :
	Display(pGameApp),
	m_pCompressedFormats(NULL),
	m_pOpenGLDeviceContext(NULL),
	m_pOpenGLContext(NULL),
	m_bResolutionChanged(FALSE),
	m_fOpenGLVersion(0.0f),
	m_fShadingLanguageVersion(0.0f),
	m_uCompressedFormatCount(0),
	m_uMaximumVertexAttributes(0),
	m_uMaximumColorAttachments(0),
	m_uActiveTexture(0)
{
}

/***************************************

	\brief Start up the OpenGL context

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

Word Burger::DisplayOpenGL::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	// OpenGL allows all 256 palette colors to work FULLPALETTEALLOWED
	// Pass the other flags through

	m_uFlags = (m_uFlags&(~(ALLOWFULLSCREENTOGGLE|ALLOWRESIZING|STEREO|MAINTAIN_ASPECT_RATIO))) | FULLPALETTEALLOWED | (uFlags&(ALLOWFULLSCREENTOGGLE|ALLOWRESIZING|STEREO|MAINTAIN_ASPECT_RATIO));

	// If there's a release function, call it because it's likely that
	// the reset of OpenGL will cause all resources to be destroyed
	if (m_pRelease) {
		m_pRelease(m_pReleaseData);
	}

	// Release the resources

	Shutdown();

	// Initialize the display resolution if
	// it hasn't been set already

	if (!m_uDisplayWidth) {
		m_uDisplayWidth = static_cast<Word>(GetSystemMetrics(SM_CXSCREEN));
		m_uDisplayHeight = static_cast<Word>(GetSystemMetrics(SM_CYSCREEN));
		HDC hHDC = GetDC(NULL);
		m_uDisplayDepth = static_cast<Word>(GetDeviceCaps(hHDC,BITSPIXEL));
		ReleaseDC(NULL,hHDC);
	}

	// Determine the resolution of the screen on power up

	if (!uWidth || !uHeight) {
		// If full screen, just use the video mode
		uWidth = m_uDisplayWidth;
		uHeight = m_uDisplayHeight;
	}

	// Determine the desired display depth

	if (!uDepth) {
		uDepth = m_uDisplayDepth;
	}

	// In the case where a windowed mode is changed in size, force
	// the window to be centered instead of retaining the previous
	// window location

	if (!(uFlags&FULLSCREEN) &&
		!(m_uFlags&FULLSCREEN) &&
		((m_uWidth!=uWidth) || (m_uHeight!=uHeight))) {
		//m_pGameApp->ResetWindowLocation();
	}

	//
	// This is the resolution that will be attempted to for the display
	// to be set.
	//

	SetWidthHeight(uWidth,uHeight);
	m_uDepth = uDepth;

	// Full screen?

	if (uFlags&FULLSCREEN) {
		m_uFlags |= FULLSCREEN;

		// For full screen mode, scan the device to determine if this monitor
		// can play at the requested resolution

		DEVMODEW BestVideoMode;
		if (!ScanVideoModes(&BestVideoMode,m_uWidth,m_uHeight,m_uDepth)) {
			// Try the desktop resolution
			if (!ScanVideoModes(&BestVideoMode,m_uDisplayWidth,m_uDisplayHeight,m_uDisplayDepth)) {
				return 10;			// Nope, failure
			}
		}
		// Found a valid mode, so try to REALLY set it.
		if (ChangeDisplaySettingsExW(NULL,&BestVideoMode,NULL,CDS_FULLSCREEN,NULL) != DISP_CHANGE_SUCCESSFUL) {
			ChangeDisplaySettings(NULL,0);
			return 10;		// Error!!
		}

		// The video mode has been changed. Record the new resolution
		m_uDisplayWidth = BestVideoMode.dmPelsWidth;
		m_uDisplayHeight = BestVideoMode.dmPelsHeight;
		m_uDisplayDepth = BestVideoMode.dmBitsPerPel;
		SetWidthHeight(m_uDisplayWidth,m_uDisplayHeight);
		m_uDepth = m_uDisplayDepth;
		m_bResolutionChanged = TRUE;
	} else {
		m_uFlags &= (~FULLSCREEN);
	}

	// Get the video contexts so drawing can commence
	HWND pWindow = m_pGameApp->GetWindow();

	// For full screen, the window needs to be borderless

	if (m_uFlags&FULLSCREEN) {
		m_pGameApp->SetWindowFullScreen(m_uWidth,m_uHeight);
	} else {
		m_pGameApp->SetWindowSize(m_uWidth,m_uHeight);
	}

	m_pOpenGLDeviceContext = GetDC(pWindow);

	// Determine the true color pixel format the system can use
	// Commented out lines that were storing only zeros since MemoryClear()
	// took care of that already.

	PIXELFORMATDESCRIPTOR PixelFormat;
	MemoryClear(&PixelFormat,sizeof(PixelFormat));
	PixelFormat.nSize = sizeof(PixelFormat);
	PixelFormat.nVersion = 1;					// Don't change.
	PixelFormat.dwFlags = PFD_DRAW_TO_WINDOW	// support window
		| PFD_SUPPORT_OPENGL					// support OpenGL
		| PFD_DOUBLEBUFFER ,					// double buffered
	//PixelFormat.iPixelType = PFD_TYPE_RGBA;		// True color
	PixelFormat.cColorBits = 32;				// True color
	//PixelFormat.cDepthBits = 0;					// No ZBuffer
	//PixelFormat.cStencilBits = 0;
	//PixelFormat.cAlphaBits = 0;
	//PixelFormat.iLayerType = PFD_MAIN_PLANE;	// Main layer
	HGLRC hGLContext = NULL;

	// Get the pixel format
	int iPixelFormat = ChoosePixelFormat(m_pOpenGLDeviceContext,&PixelFormat);
	if (iPixelFormat) {
		// Try setting to this context
		if (SetPixelFormat(m_pOpenGLDeviceContext,iPixelFormat,&PixelFormat) ) {
			hGLContext = wglCreateContext(m_pOpenGLDeviceContext);
			if (hGLContext) {
				if (!wglMakeCurrent(m_pOpenGLDeviceContext,hGLContext)) {
					wglDeleteContext(hGLContext);
					hGLContext = NULL;
				} else {
					m_uDepth = 32;
				}
			}
		}
	}
	
	// Fall back plan, try 24 bit color
	if (!hGLContext) {
		PixelFormat.cColorBits = 24;
		iPixelFormat = ChoosePixelFormat(m_pOpenGLDeviceContext,&PixelFormat);
		if (iPixelFormat) {
			if (SetPixelFormat(m_pOpenGLDeviceContext,iPixelFormat,&PixelFormat)) {
				hGLContext = wglCreateContext(m_pOpenGLDeviceContext);
				if (hGLContext) {
					if (!wglMakeCurrent(m_pOpenGLDeviceContext,hGLContext)) {
						wglDeleteContext(hGLContext);
						hGLContext = NULL;
					} else {
						m_uDepth = 24;
					}
				}
			}
		}
	}

	// Boned?
	if (!hGLContext) {
		// Release the device
		ReleaseDC(m_pGameApp->GetWindow(),m_pOpenGLDeviceContext);
		m_pOpenGLDeviceContext = NULL;
		return 10;
	}

	m_pOpenGLContext = hGLContext;

	// Now that a context has been selected, load OpenGL's functions
	WindowsLink();
	SetupOpenGL();
	return 0;
}

/***************************************

	\brief Start up the OpenGL context

	Shut down OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

void Burger::DisplayOpenGL::Shutdown(void)
{
	// Release everything else
	WindowsUnlink();

	if (m_pOpenGLContext) {
		// Force the system to use the default context, not this one.
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(m_pOpenGLContext);
		m_pOpenGLContext = NULL;
	}

	// Do I have a device?
	if (m_pOpenGLDeviceContext) {
		// Release the device
		ReleaseDC(m_pGameApp->GetWindow(),m_pOpenGLDeviceContext);
		m_pOpenGLDeviceContext = NULL;
	}

	// If the video display resolution was changed, change it back

	if (m_bResolutionChanged) {
		ChangeDisplaySettingsW(NULL,0);		// Switch video mode to default
		m_bResolutionChanged = FALSE;
	}
	Free(m_pCompressedFormats);
	m_pCompressedFormats = NULL;
	m_uCompressedFormatCount = 0;
}

void Burger::DisplayOpenGL::BeginScene(void)
{
}

/***************************************

	\brief Update the video display

	Calls SwapBuffers() in OpenGL to draw the rendered scene

	\sa Burger::DisplayOpenGL::PreBeginScene()

***************************************/

void Burger::DisplayOpenGL::EndScene(void)
{
	// Consider it done!
	SwapBuffers(m_pOpenGLDeviceContext);
	ValidateRect(m_pGameApp->GetWindow(),NULL);
}

#if !defined(DOXYGEN)
//
// Names of every extended OpenGL function that are not exposed by including OpenGL32.lib
//

static const char *OpenGLNames[277] = {
	// Open GL 2.0
	"glBlendEquationSeparate",
	"glDrawBuffers",
	"glStencilOpSeparate",
	"glStencilFuncSeparate",
	"glStencilMaskSeparate",
	"glAttachShader",
	"glBindAttribLocation",
	"glCompileShader",
	"glCreateProgram",
	"glCreateShader",
	"glDeleteProgram",
	"glDeleteShader",
	"glDetachShader",
	"glDisableVertexAttribArray",
	"glEnableVertexAttribArray",
	"glGetActiveAttrib",
	"glGetActiveUniform",
	"glGetAttachedShaders",
	"glGetAttribLocation",
	"glGetProgramiv",
	"glGetProgramInfoLog",
	"glGetShaderiv",
	"glGetShaderInfoLog",
	"glGetShaderSource",
	"glGetUniformLocation",
	"glGetUniformfv",
	"glGetUniformiv",
	"glGetVertexAttribdv",
	"glGetVertexAttribfv",
	"glGetVertexAttribiv",
	"glGetVertexAttribPointerv",
	"glIsProgram",
	"glIsShader",
	"glLinkProgram",
	"glShaderSource",
	"glUseProgram",
	"glUniform1f",
	"glUniform2f",
	"glUniform3f",
	"glUniform4f",
	"glUniform1i",
	"glUniform2i",
	"glUniform3i",
	"glUniform4i",
	"glUniform1fv",
	"glUniform2fv",
	"glUniform3fv",
	"glUniform4fv",
	"glUniform1iv",
	"glUniform2iv",
	"glUniform3iv",
	"glUniform4iv",
	"glUniformMatrix2fv",
	"glUniformMatrix3fv",
	"glUniformMatrix4fv",
	"glValidateProgram",
	"glVertexAttrib1d",
	"glVertexAttrib1dv",
	"glVertexAttrib1f",
	"glVertexAttrib1fv",
	"glVertexAttrib1s",
	"glVertexAttrib1sv",
	"glVertexAttrib2d",
	"glVertexAttrib2dv",
	"glVertexAttrib2f",
	"glVertexAttrib2fv",
	"glVertexAttrib2s",
	"glVertexAttrib2sv",
	"glVertexAttrib3d",
	"glVertexAttrib3dv",
	"glVertexAttrib3f",
	"glVertexAttrib3fv",
	"glVertexAttrib3s",
	"glVertexAttrib3sv",
	"glVertexAttrib4Nbv",
	"glVertexAttrib4Niv",
	"glVertexAttrib4Nsv",
	"glVertexAttrib4Nub",
	"glVertexAttrib4Nubv",
	"glVertexAttrib4Nuiv",
	"glVertexAttrib4Nusv",
	"glVertexAttrib4bv",
	"glVertexAttrib4d",
	"glVertexAttrib4dv",
	"glVertexAttrib4f",
	"glVertexAttrib4fv",
	"glVertexAttrib4iv",
	"glVertexAttrib4s",
	"glVertexAttrib4sv",
	"glVertexAttrib4ubv",
	"glVertexAttrib4uiv",
	"glVertexAttrib4usv",
	"glVertexAttribPointer",
	// GL_EXT_paletted_texture
	"glColorTableEXT",
	"glGetColorTableEXT",
	"glGetColorTableParameterivEXT",
	"glGetColorTableParameterfvEXT",
	// GL_EXT_framebuffer_object
	"glIsRenderbufferEXT",
	"glBindRenderbufferEXT",
	"glDeleteRenderbuffersEXT",
	"glGenRenderbuffersEXT",
	"glRenderbufferStorageEXT",
	"glGetRenderbufferParameterivEXT",
	"glIsFramebufferEXT",
	"glBindFramebufferEXT",
	"glDeleteFramebuffersEXT",
	"glGenFramebuffersEXT",
	"glCheckFramebufferStatusEXT",
	"glFramebufferTexture1DEXT",
	"glFramebufferTexture2DEXT",
	"glFramebufferTexture3DEXT",
	"glFramebufferRenderbufferEXT",
	"glGetFramebufferAttachmentParameterivEXT",
	"glGenerateMipmapEXT",
	// GL 1.2 Deprecated
	"glColorTable",
	"glColorTableParameterfv",
	"glColorTableParameteriv",
	"glCopyColorTable",
	"glGetColorTable",
	"glGetColorTableParameterfv",
	"glGetColorTableParameteriv",
	"glColorSubTable",
	"glCopyColorSubTable",
	"glConvolutionFilter1D",
	"glConvolutionFilter2D",
	"glConvolutionParameterf",
	"glConvolutionParameterfv",
	"glConvolutionParameteri",
	"glConvolutionParameteriv",
	"glCopyConvolutionFilter1D",
	"glCopyConvolutionFilter2D",
	"glGetConvolutionFilter",
	"glGetConvolutionParameterfv",
	"glGetConvolutionParameteriv",
	"glGetSeparableFilter",
	"glSeparableFilter2D",
	"glGetHistogram",
	"glGetHistogramParameterfv",
	"glGetHistogramParameteriv",
	"glGetMinmax",
	"glGetMinmaxParameterfv",
	"glGetMinmaxParameteriv",
	"glHistogram",
	"glMinmax",
	"glResetHistogram",
	"glResetMinmax",
	// GL 1.3
	"glActiveTexture",
	"glSampleCoverage",
	"glCompressedTexImage3D",
	"glCompressedTexImage2D",
	"glCompressedTexImage1D",
	"glCompressedTexSubImage3D",
	"glCompressedTexSubImage2D",
	"glCompressedTexSubImage1D",
	"glGetCompressedTexImage",
	// GL 1.5
	"glGenQueries",
	"glDeleteQueries",
	"glIsQuery",
	"glBeginQuery",
	"glEndQuery",
	"glGetQueryiv",
	"glGetQueryObjectiv",
	"glGetQueryObjectuiv",
	"glBindBuffer",
	"glDeleteBuffers",
	"glGenBuffers",
	"glIsBuffer",
	"glBufferData",
	"glBufferSubData",
	"glGetBufferSubData",
	"glMapBuffer",
	"glUnmapBuffer",
	"glGetBufferParameteriv",
	"glGetBufferPointerv",
	// GL 3.0
	"glColorMaski",
	"glGetBooleani_v",
	"glGetIntegeri_v",
	"glEnablei",
	"glDisablei",
	"glIsEnabledi",
	"glBeginTransformFeedback",
	"glEndTransformFeedback",
	"glBindBufferRange",
	"glBindBufferBase",
	"glTransformFeedbackVaryings",
	"glGetTransformFeedbackVarying",
	"glClampColor",
	"glBeginConditionalRender",
	"glEndConditionalRender",
	"glVertexAttribIPointer",
	"glGetVertexAttribIiv",
	"glGetVertexAttribIuiv",
	"glVertexAttribI1i",
	"glVertexAttribI2i",
	"glVertexAttribI3i",
	"glVertexAttribI4i",
	"glVertexAttribI1ui",
	"glVertexAttribI2ui",
	"glVertexAttribI3ui",
	"glVertexAttribI4ui",
	"glVertexAttribI1iv",
	"glVertexAttribI2iv",
	"glVertexAttribI3iv",
	"glVertexAttribI4iv",
	"glVertexAttribI1uiv",
	"glVertexAttribI2uiv",
	"glVertexAttribI3uiv",
	"glVertexAttribI4uiv",
	"glVertexAttribI4bv",
	"glVertexAttribI4sv",
	"glVertexAttribI4ubv",
	"glVertexAttribI4usv",
	"glGetUniformuiv",
	"glBindFragDataLocation",
	"glGetFragDataLocation",
	"glUniform1ui",
	"glUniform2ui",
	"glUniform3ui",
	"glUniform4ui",
	"glUniform1uiv",
	"glUniform2uiv",
	"glUniform3uiv",
	"glUniform4uiv",
	"glTexParameterIiv",
	"glTexParameterIuiv",
	"glGetTexParameterIiv",
	"glGetTexParameterIuiv",
	"glClearBufferiv",
	"glClearBufferuiv",
	"glClearBufferfv",
	"glClearBufferfi",
	"glGetStringi",
	"glIsRenderbuffer",
	"glBindRenderbuffer",
	"glDeleteRenderbuffers",
	"glGenRenderbuffers",
	"glRenderbufferStorage",
	"glGetRenderbufferParameteriv",
	"glIsFramebuffer",
	"glBindFramebuffer",
	"glDeleteFramebuffers",
	"glGenFramebuffers",
	"glCheckFramebufferStatus",
	"glFramebufferTexture1D",
	"glFramebufferTexture2D",
	"glFramebufferTexture3D",
	"glFramebufferRenderbuffer",
	"glGetFramebufferAttachmentParameteriv",
	"glGenerateMipmap",
	"glBlitFramebuffer",
	"glRenderbufferStorageMultisample",
	"glFramebufferTextureLayer",
	"glMapBufferRange",
	"glFlushMappedBufferRange",
	"glBindVertexArray",
	"glDeleteVertexArrays",
	"glGenVertexArrays",
	"glIsVertexArray",

	// GL 3.2
	"glDrawElementsBaseVertex",
	"glDrawRangeElementsBaseVertex",
	"glDrawElementsInstancedBaseVertex",
	"glMultiDrawElementsBaseVertex",
	"glProvokingVertex",
	"glFenceSync",
	"glIsSync",
	"glDeleteSync",
	"glClientWaitSync",
	"glWaitSync",
	"glGetInteger64v",
	"glGetSynciv",
	"glGetInteger64i_v",
	"glGetBufferParameteri64v",
	"glFramebufferTexture",
	"glTexImage2DMultisample",
	"glTexImage3DMultisample",
	"glGetMultisamplefv",
	"glSampleMaski"
};


// This structure MUST match the strings in the table above,
// or BAD THINGS HAPPEN(tm)

struct {
	// Open GL 2.0
	void *glBlendEquationSeparate;
	void *glDrawBuffers;
	void *glStencilOpSeparate;
	void *glStencilFuncSeparate;
	void *glStencilMaskSeparate;
	void *glAttachShader;
	void *glBindAttribLocation;
	void *glCompileShader;
	void *glCreateProgram;
	void *glCreateShader;
	void *glDeleteProgram;
	void *glDeleteShader;
	void *glDetachShader;
	void *glDisableVertexAttribArray;
	void *glEnableVertexAttribArray;
	void *glGetActiveAttrib;
	void *glGetActiveUniform;
	void *glGetAttachedShaders;
	void *glGetAttribLocation;
	void *glGetProgramiv;
	void *glGetProgramInfoLog;
	void *glGetShaderiv;
	void *glGetShaderInfoLog;
	void *glGetShaderSource;
	void *glGetUniformLocation;
	void *glGetUniformfv;
	void *glGetUniformiv;
	void *glGetVertexAttribdv;
	void *glGetVertexAttribfv;
	void *glGetVertexAttribiv;
	void *glGetVertexAttribPointerv;
	void *glIsProgram;
	void *glIsShader;
	void *glLinkProgram;
	void *glShaderSource;
	void *glUseProgram;
	void *glUniform1f;
	void *glUniform2f;
	void *glUniform3f;
	void *glUniform4f;
	void *glUniform1i;
	void *glUniform2i;
	void *glUniform3i;
	void *glUniform4i;
	void *glUniform1fv;
	void *glUniform2fv;
	void *glUniform3fv;
	void *glUniform4fv;
	void *glUniform1iv;
	void *glUniform2iv;
	void *glUniform3iv;
	void *glUniform4iv;
	void *glUniformMatrix2fv;
	void *glUniformMatrix3fv;
	void *glUniformMatrix4fv;
	void *glValidateProgram;
	void *glVertexAttrib1d;
	void *glVertexAttrib1dv;
	void *glVertexAttrib1f;
	void *glVertexAttrib1fv;
	void *glVertexAttrib1s;
	void *glVertexAttrib1sv;
	void *glVertexAttrib2d;
	void *glVertexAttrib2dv;
	void *glVertexAttrib2f;
	void *glVertexAttrib2fv;
	void *glVertexAttrib2s;
	void *glVertexAttrib2sv;
	void *glVertexAttrib3d;
	void *glVertexAttrib3dv;
	void *glVertexAttrib3f;
	void *glVertexAttrib3fv;
	void *glVertexAttrib3s;
	void *glVertexAttrib3sv;
	void *glVertexAttrib4Nbv;
	void *glVertexAttrib4Niv;
	void *glVertexAttrib4Nsv;
	void *glVertexAttrib4Nub;
	void *glVertexAttrib4Nubv;
	void *glVertexAttrib4Nuiv;
	void *glVertexAttrib4Nusv;
	void *glVertexAttrib4bv;
	void *glVertexAttrib4d;
	void *glVertexAttrib4dv;
	void *glVertexAttrib4f;
	void *glVertexAttrib4fv;
	void *glVertexAttrib4iv;
	void *glVertexAttrib4s;
	void *glVertexAttrib4sv;
	void *glVertexAttrib4ubv;
	void *glVertexAttrib4uiv;
	void *glVertexAttrib4usv;
	void *glVertexAttribPointer;
	// GL_EXT_paletted_texture
	void *glColorTableEXT;
	void *glGetColorTableEXT;
	void *glGetColorTableParameterivEXT;
	void *glGetColorTableParameterfvEXT;
	// GL_EXT_framebuffer_object
	void *glIsRenderbufferEXT;
	void *glBindRenderbufferEXT;
	void *glDeleteRenderbuffersEXT;
	void *glGenRenderbuffersEXT;
	void *glRenderbufferStorageEXT;
	void *glGetRenderbufferParameterivEXT;
	void *glIsFramebufferEXT;
	void *glBindFramebufferEXT;
	void *glDeleteFramebuffersEXT;
	void *glGenFramebuffersEXT;
	void *glCheckFramebufferStatusEXT;
	void *glFramebufferTexture1DEXT;
	void *glFramebufferTexture2DEXT;
	void *glFramebufferTexture3DEXT;
	void *glFramebufferRenderbufferEXT;
	void *glGetFramebufferAttachmentParameterivEXT;
	void *glGenerateMipmapEXT;
	// GL 1.2 Deprecated
	void *glColorTable;
	void *glColorTableParameterfv;
	void *glColorTableParameteriv;
	void *glCopyColorTable;
	void *glGetColorTable;
	void *glGetColorTableParameterfv;
	void *glGetColorTableParameteriv;
	void *glColorSubTable;
	void *glCopyColorSubTable;
	void *glConvolutionFilter1D;
	void *glConvolutionFilter2D;
	void *glConvolutionParameterf;
	void *glConvolutionParameterfv;
	void *glConvolutionParameteri;
	void *glConvolutionParameteriv;
	void *glCopyConvolutionFilter1D;
	void *glCopyConvolutionFilter2D;
	void *glGetConvolutionFilter;
	void *glGetConvolutionParameterfv;
	void *glGetConvolutionParameteriv;
	void *glGetSeparableFilter;
	void *glSeparableFilter2D;
	void *glGetHistogram;
	void *glGetHistogramParameterfv;
	void *glGetHistogramParameteriv;
	void *glGetMinmax;
	void *glGetMinmaxParameterfv;
	void *glGetMinmaxParameteriv;
	void *glHistogram;
	void *glMinmax;
	void *glResetHistogram;
	void *glResetMinmax;
	// GL 1.3
	void *glActiveTexture;
	void *glSampleCoverage;
	void *glCompressedTexImage3D;
	void *glCompressedTexImage2D;
	void *glCompressedTexImage1D;
	void *glCompressedTexSubImage3D;
	void *glCompressedTexSubImage2D;
	void *glCompressedTexSubImage1D;
	void *glGetCompressedTexImage;
	// GL 1.5
	void *glGenQueries;
	void *glDeleteQueries;
	void *glIsQuery;
	void *glBeginQuery;
	void *glEndQuery;
	void *glGetQueryiv;
	void *glGetQueryObjectiv;
	void *glGetQueryObjectuiv;
	void *glBindBuffer;
	void *glDeleteBuffers;
	void *glGenBuffers;
	void *glIsBuffer;
	void *glBufferData;
	void *glBufferSubData;
	void *glGetBufferSubData;
	void *glMapBuffer;
	void *glUnmapBuffer;
	void *glGetBufferParameteriv;
	void *glGetBufferPointerv;
	// GL 3.0
	void *glColorMaski;
	void *glGetBooleani_v;
	void *glGetIntegeri_v;
	void *glEnablei;
	void *glDisablei;
	void *glIsEnabledi;
	void *glBeginTransformFeedback;
	void *glEndTransformFeedback;
	void *glBindBufferRange;
	void *glBindBufferBase;
	void *glTransformFeedbackVaryings;
	void *glGetTransformFeedbackVarying;
	void *glClampColor;
	void *glBeginConditionalRender;
	void *glEndConditionalRender;
	void *glVertexAttribIPointer;
	void *glGetVertexAttribIiv;
	void *glGetVertexAttribIuiv;
	void *glVertexAttribI1i;
	void *glVertexAttribI2i;
	void *glVertexAttribI3i;
	void *glVertexAttribI4i;
	void *glVertexAttribI1ui;
	void *glVertexAttribI2ui;
	void *glVertexAttribI3ui;
	void *glVertexAttribI4ui;
	void *glVertexAttribI1iv;
	void *glVertexAttribI2iv;
	void *glVertexAttribI3iv;
	void *glVertexAttribI4iv;
	void *glVertexAttribI1uiv;
	void *glVertexAttribI2uiv;
	void *glVertexAttribI3uiv;
	void *glVertexAttribI4uiv;
	void *glVertexAttribI4bv;
	void *glVertexAttribI4sv;
	void *glVertexAttribI4ubv;
	void *glVertexAttribI4usv;
	void *glGetUniformuiv;
	void *glBindFragDataLocation;
	void *glGetFragDataLocation;
	void *glUniform1ui;
	void *glUniform2ui;
	void *glUniform3ui;
	void *glUniform4ui;
	void *glUniform1uiv;
	void *glUniform2uiv;
	void *glUniform3uiv;
	void *glUniform4uiv;
	void *glTexParameterIiv;
	void *glTexParameterIuiv;
	void *glGetTexParameterIiv;
	void *glGetTexParameterIuiv;
	void *glClearBufferiv;
	void *glClearBufferuiv;
	void *glClearBufferfv;
	void *glClearBufferfi;
	void *glGetStringi;
	void *glIsRenderbuffer;
	void *glBindRenderbuffer;
	void *glDeleteRenderbuffers;
	void *glGenRenderbuffers;
	void *glRenderbufferStorage;
	void *glGetRenderbufferParameteriv;
	void *glIsFramebuffer;
	void *glBindFramebuffer;
	void *glDeleteFramebuffers;
	void *glGenFramebuffers;
	void *glCheckFramebufferStatus;
	void *glFramebufferTexture1D;
	void *glFramebufferTexture2D;
	void *glFramebufferTexture3D;
	void *glFramebufferRenderbuffer;
	void *glGetFramebufferAttachmentParameteriv;
	void *glGenerateMipmap;
	void *glBlitFramebuffer;
	void *glRenderbufferStorageMultisample;
	void *glFramebufferTextureLayer;
	void *glMapBufferRange;
	void *glFlushMappedBufferRange;
	void *glBindVertexArray;
	void *glDeleteVertexArrays;
	void *glGenVertexArrays;
	void *glIsVertexArray;

	// GL 3.2
	void *glDrawElementsBaseVertex;
	void *glDrawRangeElementsBaseVertex;
	void *glDrawElementsInstancedBaseVertex;
	void *glMultiDrawElementsBaseVertex;
	void *glProvokingVertex;
	void *glFenceSync;
	void *glIsSync;
	void *glDeleteSync;
	void *glClientWaitSync;
	void *glWaitSync;
	void *glGetInteger64v;
	void *glGetSynciv;
	void *glGetInteger64i_v;
	void *glGetBufferParameteri64v;
	void *glFramebufferTexture;
	void *glTexImage2DMultisample;
	void *glTexImage3DMultisample;
	void *glGetMultisamplefv;
	void *glSampleMaski;
} OpenGLProcPtrs;

#if defined(_DEBUG)
static void CheckGLError(const char *pFunctionName)
{
	GLenum iGLError = glGetError();
	if (iGLError) {
		do {
			Burger::Debug::Message("Function %s returned glGetError(0x%08X)\n",pFunctionName,iGLError);
			iGLError = glGetError();
		} while (iGLError);
	}
}
#else
#define CheckGLError(x)
#endif

// These macros make it easy to create the code to use the above function table

#define VOIDPROC(name) GLAPI void APIENTRY name(void) { static_cast<void(__stdcall *)(void)>(OpenGLProcPtrs.name)(); CheckGLError(#name); }
#define VOIDPROC1(name,a,b) GLAPI void APIENTRY name(a b) { static_cast<void(__stdcall *)(a)>(OpenGLProcPtrs.name)(b); CheckGLError(#name); }
#define VOIDPROC2(name,a,b,c,d) GLAPI void APIENTRY name(a b,c d) { static_cast<void(__stdcall *)(a,c)>(OpenGLProcPtrs.name)(b,d); CheckGLError(#name); }
#define VOIDPROC3(name,a,b,c,d,e,f) GLAPI void APIENTRY name(a b,c d,e f) { static_cast<void(__stdcall *)(a,c,e)>(OpenGLProcPtrs.name)(b,d,f); CheckGLError(#name); }
#define VOIDPROC4(name,a,b,c,d,e,f,g,h) GLAPI void APIENTRY name(a b,c d,e f,g h) { static_cast<void(__stdcall *)(a,c,e,g)>(OpenGLProcPtrs.name)(b,d,f,h); CheckGLError(#name); }
#define VOIDPROC5(name,a,b,c,d,e,f,g,h,i,j) GLAPI void APIENTRY name(a b,c d,e f,g h,i j) { static_cast<void(__stdcall *)(a,c,e,g,i)>(OpenGLProcPtrs.name)(b,d,f,h,j); CheckGLError(#name); }
#define VOIDPROC6(name,a,b,c,d,e,f,g,h,i,j,k,l) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l) { static_cast<void(__stdcall *)(a,c,e,g,i,k)>(OpenGLProcPtrs.name)(b,d,f,h,j,l); CheckGLError(#name); }
#define VOIDPROC7(name,a,b,c,d,e,f,g,h,i,j,k,l,m,n) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l,m n) { static_cast<void(__stdcall *)(a,c,e,g,i,k,m)>(OpenGLProcPtrs.name)(b,d,f,h,j,l,n); CheckGLError(#name); }
#define VOIDPROC8(name,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l,m n,o p) { static_cast<void(__stdcall *)(a,c,e,g,i,k,m,o)>(OpenGLProcPtrs.name)(b,d,f,h,j,l,n,p); CheckGLError(#name); }
#define VOIDPROC9(name,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l,m n,o p,q r) { static_cast<void(__stdcall *)(a,c,e,g,i,k,m,o,q)>(OpenGLProcPtrs.name)(b,d,f,h,j,l,n,p,r); CheckGLError(#name); }
#define VOIDPROC10(name,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l,m n,o p,q r,s t) { static_cast<void(__stdcall *)(a,c,e,g,i,k,m,o,q,s)>(OpenGLProcPtrs.name)(b,d,f,h,j,l,n,p,r,t); CheckGLError(#name); }
#define VOIDPROC11(name,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) GLAPI void APIENTRY name(a b,c d,e f,g h,i j,k l,m n,o p,q r,s t,u v) { static_cast<void(__stdcall *)(a,c,e,g,i,k,m,o,q,s,u)>(OpenGLProcPtrs.name)(b,d,f,h,j,l,n,p,r,t,v); CheckGLError(#name); }
#define PROC(type,name) GLAPI type APIENTRY name(void) { type Result = static_cast<type(__stdcall *)(void)>(OpenGLProcPtrs.name)(); CheckGLError(#name); return Result; }
#define PROC1(type,name,a,b) GLAPI type APIENTRY name(a b) { type Result = static_cast<type(__stdcall *)(a)>(OpenGLProcPtrs.name)(b); CheckGLError(#name); return Result; }
#define PROC2(type,name,a,b,c,d) GLAPI type APIENTRY name(a b,c d) { type Result = static_cast<type(__stdcall *)(a,c)>(OpenGLProcPtrs.name)(b,d); CheckGLError(#name); return Result; }
#define PROC3(type,name,a,b,c,d,e,f) GLAPI type APIENTRY name(a b,c d,e f) { type Result = static_cast<type(__stdcall *)(a,c,e)>(OpenGLProcPtrs.name)(b,d,f); CheckGLError(#name); return Result; }
#define PROC4(type,name,a,b,c,d,e,f,g,h) GLAPI type APIENTRY name(a b,c d,e f,g h) { type Result = static_cast<type(__stdcall *)(a,c,e,g)>(OpenGLProcPtrs.name)(b,d,f,h); CheckGLError(#name); return Result; }

// These are the fake functions that will execute via a call
// through a manually loaded function pointer

// Open GL 2.0
VOIDPROC2(glBlendEquationSeparate,GLenum,modeRGB,GLenum,modeAlpha)
VOIDPROC2(glDrawBuffers,GLsizei,n,const GLenum *,bufs)
VOIDPROC4(glStencilOpSeparate,GLenum,face,GLenum,sfail,GLenum,dpfail,GLenum,dppass)
VOIDPROC4(glStencilFuncSeparate,GLenum,face,GLenum,func,GLint,ref,GLuint,mask)
VOIDPROC2(glStencilMaskSeparate,GLenum,face,GLuint,mask)
VOIDPROC2(glAttachShader,GLuint,program,GLuint,shader)
VOIDPROC3(glBindAttribLocation,GLuint,program,GLuint,index,const GLchar *,name)
VOIDPROC1(glCompileShader,GLuint,shader)
PROC(GLuint,glCreateProgram)
PROC1(GLuint,glCreateShader,GLenum,type)
VOIDPROC1(glDeleteProgram,GLuint,program)
VOIDPROC1(glDeleteShader,GLuint,shader)
VOIDPROC2(glDetachShader,GLuint,program,GLuint,shader)
VOIDPROC1(glDisableVertexAttribArray,GLuint,index)
VOIDPROC1(glEnableVertexAttribArray,GLuint,index)
VOIDPROC7(glGetActiveAttrib,GLuint,program,GLuint,index,GLsizei,bufSize,GLsizei *,length,GLint *,size,GLenum *,type,GLchar *,name)
VOIDPROC7(glGetActiveUniform,GLuint,program,GLuint,index,GLsizei,bufSize,GLsizei *,length,GLint *,size,GLenum *,type,GLchar *,name)
VOIDPROC4(glGetAttachedShaders,GLuint,program,GLsizei,maxCount,GLsizei *,count,GLuint *,obj)
PROC2(GLint,glGetAttribLocation,GLuint,program,const GLchar *,name)
VOIDPROC3(glGetProgramiv,GLuint,program,GLenum,pname,GLint *,params)
VOIDPROC4(glGetProgramInfoLog,GLuint,program,GLsizei,bufSize,GLsizei *,length,GLchar *,infoLog)
VOIDPROC3(glGetShaderiv,GLuint,shader,GLenum,pname,GLint *,params)
VOIDPROC4(glGetShaderInfoLog,GLuint,shader,GLsizei,bufSize,GLsizei *,length,GLchar *,infoLog)
VOIDPROC4(glGetShaderSource,GLuint,shader,GLsizei,bufSize,GLsizei *,length,GLchar *,source)
PROC2(GLint,glGetUniformLocation,GLuint,program,const GLchar *,name)
VOIDPROC3(glGetUniformfv,GLuint,program,GLint,location,GLfloat *,params)
VOIDPROC3(glGetUniformiv,GLuint,program,GLint,location,GLint *,params)
VOIDPROC3(glGetVertexAttribdv,GLuint,index,GLenum,pname,GLdouble *,params)
VOIDPROC3(glGetVertexAttribfv,GLuint,index,GLenum,pname,GLfloat *,params)
VOIDPROC3(glGetVertexAttribiv,GLuint,index,GLenum,pname,GLint *,params)
VOIDPROC3(glGetVertexAttribPointerv,GLuint,index,GLenum,pname,GLvoid**,pointer)
PROC1(GLboolean,glIsProgram,GLuint,program)
PROC1(GLboolean,glIsShader,GLuint,shader)
VOIDPROC1(glLinkProgram,GLuint,program)
VOIDPROC4(glShaderSource,GLuint,shader,GLsizei,count,const GLchar* const*,string,const GLint *,length)
VOIDPROC1(glUseProgram,GLuint,program)
VOIDPROC2(glUniform1f,GLint,location,GLfloat,v0)
VOIDPROC3(glUniform2f,GLint,location,GLfloat,v0,GLfloat,v1)
VOIDPROC4(glUniform3f,GLint,location,GLfloat,v0,GLfloat,v1,GLfloat,v2)
VOIDPROC5(glUniform4f,GLint,location,GLfloat,v0,GLfloat,v1,GLfloat,v2,GLfloat,v3)
VOIDPROC2(glUniform1i,GLint,location,GLint,v0)
VOIDPROC3(glUniform2i,GLint,location,GLint,v0,GLint,v1)
VOIDPROC4(glUniform3i,GLint,location,GLint,v0,GLint,v1,GLint,v2)
VOIDPROC5(glUniform4i,GLint,location,GLint,v0,GLint,v1,GLint,v2,GLint,v3)
VOIDPROC3(glUniform1fv,GLint,location,GLsizei,count,const GLfloat *,value)
VOIDPROC3(glUniform2fv,GLint,location,GLsizei,count,const GLfloat *,value)
VOIDPROC3(glUniform3fv,GLint,location,GLsizei,count,const GLfloat *,value)
VOIDPROC3(glUniform4fv,GLint,location,GLsizei,count,const GLfloat *,value)
VOIDPROC3(glUniform1iv,GLint,location,GLsizei,count,const GLint *,value)
VOIDPROC3(glUniform2iv,GLint,location,GLsizei,count,const GLint *,value)
VOIDPROC3(glUniform3iv,GLint,location,GLsizei,count,const GLint *,value)
VOIDPROC3(glUniform4iv,GLint,location,GLsizei,count,const GLint *,value)
VOIDPROC4(glUniformMatrix2fv,GLint,location,GLsizei,count,GLboolean,transpose,const GLfloat *,value)
VOIDPROC4(glUniformMatrix3fv,GLint,location,GLsizei,count,GLboolean,transpose,const GLfloat *,value)
VOIDPROC4(glUniformMatrix4fv,GLint,location,GLsizei,count,GLboolean,transpose,const GLfloat *,value)
VOIDPROC1(glValidateProgram,GLuint,program)
VOIDPROC2(glVertexAttrib1d,GLuint,index,GLdouble,x)
VOIDPROC2(glVertexAttrib1dv,GLuint,index,const GLdouble *,v)
VOIDPROC2(glVertexAttrib1f,GLuint,index,GLfloat,x)
VOIDPROC2(glVertexAttrib1fv,GLuint,index,const GLfloat *,v)
VOIDPROC2(glVertexAttrib1s,GLuint,index,GLshort,x)
VOIDPROC2(glVertexAttrib1sv,GLuint,index,const GLshort *,v)
VOIDPROC3(glVertexAttrib2d,GLuint,index,GLdouble,x,GLdouble,y)
VOIDPROC2(glVertexAttrib2dv,GLuint,index,const GLdouble *,v)
VOIDPROC3(glVertexAttrib2f,GLuint,index,GLfloat,x,GLfloat,y)
VOIDPROC2(glVertexAttrib2fv,GLuint,index,const GLfloat *,v)
VOIDPROC3(glVertexAttrib2s,GLuint,index,GLshort,x,GLshort,y)
VOIDPROC2(glVertexAttrib2sv,GLuint,index,const GLshort *,v)
VOIDPROC4(glVertexAttrib3d,GLuint,index,GLdouble,x,GLdouble,y,GLdouble,z)
VOIDPROC2(glVertexAttrib3dv,GLuint,index,const GLdouble *,v)
VOIDPROC4(glVertexAttrib3f,GLuint,index,GLfloat,x,GLfloat,y,GLfloat,z)
VOIDPROC2(glVertexAttrib3fv,GLuint,index,const GLfloat *,v)
VOIDPROC4(glVertexAttrib3s,GLuint,index,GLshort,x,GLshort,y,GLshort,z)
VOIDPROC2(glVertexAttrib3sv,GLuint,index,const GLshort *,v)
VOIDPROC2(glVertexAttrib4Nbv,GLuint,index,const GLbyte *,v)
VOIDPROC2(glVertexAttrib4Niv,GLuint,index,const GLint *,v)
VOIDPROC2(glVertexAttrib4Nsv,GLuint,index,const GLshort *,v)
VOIDPROC5(glVertexAttrib4Nub,GLuint,index,GLubyte,x,GLubyte,y,GLubyte,z,GLubyte,w)
VOIDPROC2(glVertexAttrib4Nubv,GLuint,index,const GLubyte *,v)
VOIDPROC2(glVertexAttrib4Nuiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttrib4Nusv,GLuint,index,const GLushort *,v)
VOIDPROC2(glVertexAttrib4bv,GLuint,index,const GLbyte *,v)
VOIDPROC5(glVertexAttrib4d,GLuint,index,GLdouble,x,GLdouble,y,GLdouble,z,GLdouble,w)
VOIDPROC2(glVertexAttrib4dv,GLuint,index,const GLdouble *,v)
VOIDPROC5(glVertexAttrib4f,GLuint,index,GLfloat,x,GLfloat,y,GLfloat,z,GLfloat,w)
VOIDPROC2(glVertexAttrib4fv,GLuint,index,const GLfloat *,v)
VOIDPROC2(glVertexAttrib4iv,GLuint,index,const GLint *,v)
VOIDPROC5(glVertexAttrib4s,GLuint,index,GLshort,x,GLshort,y,GLshort,z,GLshort,w)
VOIDPROC2(glVertexAttrib4sv,GLuint,index,const GLshort *,v)
VOIDPROC2(glVertexAttrib4ubv,GLuint,index,const GLubyte *,v)
VOIDPROC2(glVertexAttrib4uiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttrib4usv,GLuint,index,const GLushort *,v)
VOIDPROC6(glVertexAttribPointer,GLuint,index,GLint,size,GLenum,type,GLboolean,normalized,GLsizei,stride,const GLvoid *,pointer)
// GL_EXT_paletted_texture
VOIDPROC6(glColorTableEXT,GLenum,target,GLenum,internalFormat,GLsizei,width,GLenum,format,GLenum,type,const GLvoid *,table)
VOIDPROC4(glGetColorTableEXT,GLenum,target,GLenum,format,GLenum,type,GLvoid *,data)
VOIDPROC3(glGetColorTableParameterivEXT,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC3(glGetColorTableParameterfvEXT,GLenum,target,GLenum,pname,GLfloat *,params)
// GL_EXT_framebuffer_object
PROC1(GLboolean,glIsRenderbufferEXT,GLuint,renderbuffer)
VOIDPROC2(glBindRenderbufferEXT,GLenum,target,GLuint,renderbuffer)
VOIDPROC2(glDeleteRenderbuffersEXT,GLsizei,n,const GLuint *,renderbuffers)
VOIDPROC2(glGenRenderbuffersEXT,GLsizei,n,GLuint *,renderbuffers)
VOIDPROC4(glRenderbufferStorageEXT,GLenum,target,GLenum,internalformat,GLsizei,width,GLsizei,height)
VOIDPROC3(glGetRenderbufferParameterivEXT,GLenum,target,GLenum,pname,GLint *,params)
PROC1(GLboolean,glIsFramebufferEXT,GLuint,framebuffer)
VOIDPROC2(glBindFramebufferEXT,GLenum,target,GLuint,framebuffer)
VOIDPROC2(glDeleteFramebuffersEXT,GLsizei,n,const GLuint *,framebuffers)
VOIDPROC2(glGenFramebuffersEXT,GLsizei,n,GLuint *,framebuffers)
PROC1(GLenum,glCheckFramebufferStatusEXT,GLenum,target)
VOIDPROC5(glFramebufferTexture1DEXT,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level)
VOIDPROC5(glFramebufferTexture2DEXT,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level)
VOIDPROC6(glFramebufferTexture3DEXT,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level,GLint,zoffset)
VOIDPROC4(glFramebufferRenderbufferEXT,GLenum,target,GLenum,attachment,GLenum,renderbuffertarget,GLuint,renderbuffer)
VOIDPROC4(glGetFramebufferAttachmentParameterivEXT,GLenum,target,GLenum,attachment,GLenum,pname,GLint *,params)
VOIDPROC1(glGenerateMipmapEXT,GLenum,target)
// GL 1.2 Deprecated
VOIDPROC6(glColorTable,GLenum,target,GLenum,internalformat,GLsizei,width,GLenum,format,GLenum,type,const GLvoid *,table)
VOIDPROC3(glColorTableParameterfv,GLenum,target,GLenum,pname,const GLfloat *,params)
VOIDPROC3(glColorTableParameteriv,GLenum,target,GLenum,pname,const GLint *,params)
VOIDPROC5(glCopyColorTable,GLenum,target,GLenum,internalformat,GLint,x,GLint,y,GLsizei,width)
VOIDPROC4(glGetColorTable,GLenum,target,GLenum,format,GLenum,type,GLvoid *,table)
VOIDPROC3(glGetColorTableParameterfv,GLenum,target,GLenum,pname,GLfloat *,params)
VOIDPROC3(glGetColorTableParameteriv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC6(glColorSubTable,GLenum,target,GLsizei,start,GLsizei,count,GLenum,format,GLenum,type,const GLvoid *,data)
VOIDPROC5(glCopyColorSubTable,GLenum,target,GLsizei,start,GLint,x,GLint,y,GLsizei,width)
VOIDPROC6(glConvolutionFilter1D,GLenum,target,GLenum,internalformat,GLsizei,width,GLenum,format,GLenum,type,const GLvoid *,image)
VOIDPROC7(glConvolutionFilter2D,GLenum,target,GLenum,internalformat,GLsizei,width,GLsizei,height,GLenum,format,GLenum,type,const GLvoid *,image)
VOIDPROC3(glConvolutionParameterf,GLenum,target,GLenum,pname,GLfloat,params)
VOIDPROC3(glConvolutionParameterfv,GLenum,target,GLenum,pname,const GLfloat *,params)
VOIDPROC3(glConvolutionParameteri,GLenum,target,GLenum,pname,GLint,params)
VOIDPROC3(glConvolutionParameteriv,GLenum,target,GLenum,pname,const GLint *,params)
VOIDPROC5(glCopyConvolutionFilter1D,GLenum,target,GLenum,internalformat,GLint,x,GLint,y,GLsizei,width)
VOIDPROC6(glCopyConvolutionFilter2D,GLenum,target,GLenum,internalformat,GLint,x,GLint,y,GLsizei,width,GLsizei,height)
VOIDPROC4(glGetConvolutionFilter,GLenum,target,GLenum,format,GLenum,type,GLvoid *,image)
VOIDPROC3(glGetConvolutionParameterfv,GLenum,target,GLenum,pname,GLfloat *,params)
VOIDPROC3(glGetConvolutionParameteriv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC6(glGetSeparableFilter,GLenum,target,GLenum,format,GLenum,type,GLvoid *,row,GLvoid *,column,GLvoid *,span)
VOIDPROC8(glSeparableFilter2D,GLenum,target,GLenum,internalformat,GLsizei,width,GLsizei,height,GLenum,format,GLenum,type,const GLvoid *,row,const GLvoid *,column)
VOIDPROC5(glGetHistogram,GLenum,target,GLboolean,reset,GLenum,format,GLenum,type,GLvoid *,values)
VOIDPROC3(glGetHistogramParameterfv,GLenum,target,GLenum,pname,GLfloat *,params)
VOIDPROC3(glGetHistogramParameteriv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC5(glGetMinmax,GLenum,target,GLboolean,reset,GLenum,format,GLenum,type,GLvoid *,values)
VOIDPROC3(glGetMinmaxParameterfv,GLenum,target,GLenum,pname,GLfloat *,params)
VOIDPROC3(glGetMinmaxParameteriv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC4(glHistogram,GLenum,target,GLsizei,width,GLenum,internalformat,GLboolean,sink)
VOIDPROC3(glMinmax,GLenum,target,GLenum,internalformat,GLboolean,sink)
VOIDPROC1(glResetHistogram,GLenum,target)
VOIDPROC1(glResetMinmax,GLenum,target)

// GL 1.3
VOIDPROC1(glActiveTexture,GLenum,texture)
VOIDPROC2(glSampleCoverage,GLclampf,value,GLboolean,invert)
VOIDPROC9(glCompressedTexImage3D,GLenum,target,GLint,level,GLenum,internalformat,GLsizei,width,GLsizei,height,GLsizei,depth,GLint,border,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC8(glCompressedTexImage2D,GLenum,target,GLint,level,GLenum,internalformat,GLsizei,width,GLsizei,height,GLint,border,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC7(glCompressedTexImage1D,GLenum,target,GLint,level,GLenum,internalformat,GLsizei,width,GLint,border,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC11(glCompressedTexSubImage3D,GLenum,target,GLint,level,GLint,xoffset,GLint,yoffset,GLint,zoffset,GLsizei,width,GLsizei,height,GLsizei,depth,GLenum,format,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC9(glCompressedTexSubImage2D,GLenum,target,GLint,level,GLint,xoffset,GLint,yoffset,GLsizei,width,GLsizei,height,GLenum,format,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC7(glCompressedTexSubImage1D,GLenum,target,GLint,level,GLint,xoffset,GLsizei,width,GLenum,format,GLsizei,imageSize,const GLvoid *,data)
VOIDPROC3(glGetCompressedTexImage,GLenum,target,GLint,level,GLvoid *,mg)

// GL 1.5
VOIDPROC2(glGenQueries,GLsizei,n,GLuint *,ids)
VOIDPROC2(glDeleteQueries,GLsizei,n,const GLuint *,ids)
PROC1(GLboolean,glIsQuery,GLuint,id)
VOIDPROC2(glBeginQuery,GLenum,target,GLuint,id)
VOIDPROC1(glEndQuery,GLenum,target)
VOIDPROC3(glGetQueryiv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC3(glGetQueryObjectiv,GLuint,id,GLenum,pname,GLint *,params)
VOIDPROC3(glGetQueryObjectuiv,GLuint,id,GLenum,pname,GLuint *,params)
VOIDPROC2(glBindBuffer,GLenum,target,GLuint,buffer)
VOIDPROC2(glDeleteBuffers,GLsizei,n,const GLuint *,buffers)
VOIDPROC2(glGenBuffers,GLsizei,n,GLuint *,buffers)
PROC1(GLboolean,glIsBuffer,GLuint,buffer)
VOIDPROC4(glBufferData,GLenum,target,GLsizeiptr,size,const void *,data,GLenum,usage)
VOIDPROC4(glBufferSubData,GLenum,target,GLintptr,offset,GLsizeiptr,size,const void *,data)
VOIDPROC4(glGetBufferSubData,GLenum,target,GLintptr,offset,GLsizeiptr,size,void *,data)
PROC2(void *,glMapBuffer,GLenum,target,GLenum,access)
PROC1(GLboolean,glUnmapBuffer,GLenum,target)
VOIDPROC3(glGetBufferParameteriv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC3(glGetBufferPointerv,GLenum,target,GLenum,pname,void **,params)

// GL 3.0
VOIDPROC5(glColorMaski,GLuint,index,GLboolean,r,GLboolean,g,GLboolean,b,GLboolean,a)
VOIDPROC3(glGetBooleani_v,GLenum,target,GLuint,index,GLboolean *,data)
VOIDPROC3(glGetIntegeri_v,GLenum,target,GLuint,index,GLint *,data)
VOIDPROC2(glEnablei,GLenum,target,GLuint,index)
VOIDPROC2(glDisablei,GLenum,target,GLuint,index)
PROC2(GLboolean,glIsEnabledi,GLenum,target,GLuint,index)
VOIDPROC1(glBeginTransformFeedback,GLenum,primitiveMode)
VOIDPROC(glEndTransformFeedback)
VOIDPROC5(glBindBufferRange,GLenum,target,GLuint,index,GLuint,buffer,GLintptr,offset,GLsizeiptr,size)
VOIDPROC3(glBindBufferBase,GLenum,target,GLuint,index,GLuint,buffer)
VOIDPROC4(glTransformFeedbackVaryings,GLuint,program,GLsizei,count,const GLchar *const*,varyings,GLenum,bufferMode)
VOIDPROC7(glGetTransformFeedbackVarying,GLuint,program,GLuint,index,GLsizei,bufSize,GLsizei *,length,GLsizei *,size,GLenum *,type,GLchar *,name)
VOIDPROC2(glClampColor,GLenum,target,GLenum,clamp)
VOIDPROC2(glBeginConditionalRender,GLuint,id,GLenum,mode)
VOIDPROC(glEndConditionalRender)
VOIDPROC5(glVertexAttribIPointer,GLuint,index,GLint,size,GLenum,type,GLsizei,stride,const void *,pointer)
VOIDPROC3(glGetVertexAttribIiv,GLuint,index,GLenum,pname,GLint *,params)
VOIDPROC3(glGetVertexAttribIuiv,GLuint,index,GLenum,pname,GLuint *,params)
VOIDPROC2(glVertexAttribI1i,GLuint,index,GLint,x)
VOIDPROC3(glVertexAttribI2i,GLuint,index,GLint,x,GLint,y)
VOIDPROC4(glVertexAttribI3i,GLuint,index,GLint,x,GLint,y,GLint,z)
VOIDPROC5(glVertexAttribI4i,GLuint,index,GLint,x,GLint,y,GLint,z,GLint,w)
VOIDPROC2(glVertexAttribI1ui,GLuint,index,GLuint,x)
VOIDPROC3(glVertexAttribI2ui,GLuint,index,GLuint,x,GLuint,y)
VOIDPROC4(glVertexAttribI3ui,GLuint,index,GLuint,x,GLuint,y,GLuint,z)
VOIDPROC5(glVertexAttribI4ui,GLuint,index,GLuint,x,GLuint,y,GLuint,z,GLuint,w)
VOIDPROC2(glVertexAttribI1iv,GLuint,index,const GLint *,v)
VOIDPROC2(glVertexAttribI2iv,GLuint,index,const GLint *,v)
VOIDPROC2(glVertexAttribI3iv,GLuint,index,const GLint *,v)
VOIDPROC2(glVertexAttribI4iv,GLuint,index,const GLint *,v)
VOIDPROC2(glVertexAttribI1uiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttribI2uiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttribI3uiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttribI4uiv,GLuint,index,const GLuint *,v)
VOIDPROC2(glVertexAttribI4bv,GLuint,index,const GLbyte *,v)
VOIDPROC2(glVertexAttribI4sv,GLuint,index,const GLshort *,v)
VOIDPROC2(glVertexAttribI4ubv,GLuint,index,const GLubyte *,v)
VOIDPROC2(glVertexAttribI4usv,GLuint,index,const GLushort *,v)
VOIDPROC3(glGetUniformuiv,GLuint,program,GLint,location,GLuint *,params)
VOIDPROC3(glBindFragDataLocation,GLuint,program,GLuint,color,const GLchar *,name)
PROC2(GLint,glGetFragDataLocation,GLuint,program,const GLchar *,name)
VOIDPROC2(glUniform1ui,GLint,location,GLuint,v0)
VOIDPROC3(glUniform2ui,GLint,location,GLuint,v0,GLuint,v1)
VOIDPROC4(glUniform3ui,GLint,location,GLuint,v0,GLuint,v1,GLuint,v2)
VOIDPROC5(glUniform4ui,GLint,location,GLuint,v0,GLuint,v1,GLuint,v2,GLuint,v3)
VOIDPROC3(glUniform1uiv,GLint,location,GLsizei,count,const GLuint *,value)
VOIDPROC3(glUniform2uiv,GLint,location,GLsizei,count,const GLuint *,value)
VOIDPROC3(glUniform3uiv,GLint,location,GLsizei,count,const GLuint *,value)
VOIDPROC3(glUniform4uiv,GLint,location,GLsizei,count,const GLuint *,value)
VOIDPROC3(glTexParameterIiv,GLenum,target,GLenum,pname,const GLint *,params)
VOIDPROC3(glTexParameterIuiv,GLenum,target,GLenum,pname,const GLuint *,params)
VOIDPROC3(glGetTexParameterIiv,GLenum,target,GLenum,pname,GLint *,params)
VOIDPROC3(glGetTexParameterIuiv,GLenum,target,GLenum,pname,GLuint *,params)
VOIDPROC3(glClearBufferiv,GLenum,buffer,GLint,drawbuffer,const GLint *,value)
VOIDPROC3(glClearBufferuiv,GLenum,buffer,GLint,drawbuffer,const GLuint *,value)
VOIDPROC3(glClearBufferfv,GLenum,buffer,GLint,drawbuffer,const GLfloat *,value)
VOIDPROC4(glClearBufferfi,GLenum,buffer,GLint,drawbuffer,GLfloat,depth,GLint,stencil)
PROC2(const GLubyte *,glGetStringi,GLenum,name,GLuint,index)
PROC1(GLboolean,glIsRenderbuffer,GLuint,renderbuffer)
VOIDPROC2(glBindRenderbuffer,GLenum,target,GLuint,renderbuffer)
VOIDPROC2(glDeleteRenderbuffers,GLsizei,n,const GLuint *,renderbuffers)
VOIDPROC2(glGenRenderbuffers,GLsizei,n,GLuint *,renderbuffers)
VOIDPROC4(glRenderbufferStorage,GLenum,target,GLenum,internalformat,GLsizei,width,GLsizei,height)
VOIDPROC3(glGetRenderbufferParameteriv,GLenum,target,GLenum,pname,GLint *,params)
PROC1(GLboolean,glIsFramebuffer,GLuint,framebuffer)
VOIDPROC2(glBindFramebuffer,GLenum,target,GLuint,framebuffer)
VOIDPROC2(glDeleteFramebuffers,GLsizei,n,const GLuint *,framebuffers)
VOIDPROC2(glGenFramebuffers,GLsizei,n,GLuint *,framebuffers)
PROC1(GLenum,glCheckFramebufferStatus,GLenum,target)
VOIDPROC5(glFramebufferTexture1D,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level)
VOIDPROC5(glFramebufferTexture2D,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level)
VOIDPROC6(glFramebufferTexture3D,GLenum,target,GLenum,attachment,GLenum,textarget,GLuint,texture,GLint,level,GLint,zoffset)
VOIDPROC4(glFramebufferRenderbuffer,GLenum,target,GLenum,attachment,GLenum,renderbuffertarget,GLuint,renderbuffer)
VOIDPROC4(glGetFramebufferAttachmentParameteriv,GLenum,target,GLenum,attachment,GLenum,pname,GLint *,params)
VOIDPROC1(glGenerateMipmap,GLenum,target)
VOIDPROC10(glBlitFramebuffer,GLint,srcX0,GLint,srcY0,GLint,srcX1,GLint,srcY1,GLint,dstX0,GLint,dstY0,GLint,dstX1,GLint,dstY1,GLbitfield,mask,GLenum,filter)
VOIDPROC5(glRenderbufferStorageMultisample,GLenum,target,GLsizei,samples,GLenum,internalformat,GLsizei,width,GLsizei,height)
VOIDPROC5(glFramebufferTextureLayer,GLenum,target,GLenum,attachment,GLuint,texture,GLint,level,GLint,layer)
PROC4(void *,glMapBufferRange,GLenum,target,GLintptr,offset,GLsizeiptr,length,GLbitfield,access)
VOIDPROC3(glFlushMappedBufferRange,GLenum,target,GLintptr,offset,GLsizeiptr,length)
VOIDPROC1(glBindVertexArray,GLuint,array)
VOIDPROC2(glDeleteVertexArrays,GLsizei,n,const GLuint *,arrays)
VOIDPROC2(glGenVertexArrays,GLsizei,n,GLuint *,arrays)
PROC1(GLboolean,glIsVertexArray,GLuint,array)

// GL 3.2
VOIDPROC5(glDrawElementsBaseVertex,GLenum,mode,GLsizei,count,GLenum,type,const void *,indices,GLint,basevertex)
VOIDPROC7(glDrawRangeElementsBaseVertex,GLenum,mode,GLuint,start,GLuint,end,GLsizei,count,GLenum,type,const void *,indices,GLint,basevertex)
VOIDPROC6(glDrawElementsInstancedBaseVertex,GLenum,mode,GLsizei,count,GLenum,type,const void *,indices,GLsizei,instancecount,GLint,basevertex)
VOIDPROC6(glMultiDrawElementsBaseVertex,GLenum,mode,const GLsizei *,count,GLenum,type,const void *const*,indices,GLsizei,drawcount,const GLint *,basevertex)
VOIDPROC1(glProvokingVertex,GLenum,mode)
PROC2(GLsync,glFenceSync,GLenum,condition,GLbitfield,flags)
PROC1(GLboolean,glIsSync,GLsync,sync)
VOIDPROC1(glDeleteSync,GLsync,sync)
PROC3(GLenum,glClientWaitSync,GLsync,sync,GLbitfield,flags,GLuint64,timeout)
VOIDPROC3(glWaitSync,GLsync,sync,GLbitfield,flags,GLuint64,timeout)
VOIDPROC2(glGetInteger64v,GLenum,pname,GLint64 *,data)
VOIDPROC5(glGetSynciv,GLsync,sync,GLenum,pname,GLsizei,bufSize,GLsizei *,length,GLint *,values)
VOIDPROC3(glGetInteger64i_v,GLenum,target,GLuint,index,GLint64 *,data)
VOIDPROC3(glGetBufferParameteri64v,GLenum,target,GLenum,pname,GLint64 *,params)
VOIDPROC4(glFramebufferTexture,GLenum,target,GLenum,attachment,GLuint,texture,GLint,level)
VOIDPROC6(glTexImage2DMultisample,GLenum,target,GLsizei,samples,GLenum,internalformat,GLsizei,width,GLsizei,height,GLboolean,fixedsamplelocations)
VOIDPROC7(glTexImage3DMultisample,GLenum,target,GLsizei,samples,GLenum,internalformat,GLsizei,width,GLsizei,height,GLsizei,depth,GLboolean,fixedsamplelocations)
VOIDPROC3(glGetMultisamplefv,GLenum,pname,GLuint,index,GLfloat *,val)
VOIDPROC2(glSampleMaski,GLuint,maskNumber,GLbitfield,mask)
#endif

/*! ************************************

	\brief Manually load in all the OpenGL extensions

	Windows doesn't automatically link in all of the OpenGL functions
	when opengl32.lib is linked in. The extensions need to be manually loaded
	via calls to wglGetProcAddress() after an OpenGL pixelformat is selected.

	Call this function immediately after an OpenGL pixelformat is selected
	and then all of the OpenGL extended function will "magically" appear and
	work as if they were directly linked in.

	\windowsonly
	\sa Burger::DisplayOpenGL::WindowsUnlink()
	
***************************************/
 
void BURGER_API Burger::DisplayOpenGL::WindowsLink(void)
{
	const char **ppWork = OpenGLNames;
	void **ppDest = &OpenGLProcPtrs.glBlendEquationSeparate;
	//WordPtr uTest = sizeof(OpenGLProcPtrs)/sizeof(void*);
	Word uCount = BURGER_ARRAYSIZE(OpenGLNames);
	do {
		ppDest[0] = wglGetProcAddress(ppWork[0]);
		++ppWork;
		++ppDest;
	} while (--uCount);
}

/*! ************************************

	\brief Release all of the OpenGL extensions

	When an OpenGL pixelformat is released, call this function
	to prevent accidental calling of any OpenGL functions
	that are connected to a disabled device driver.

	\windowsonly
	\sa Burger::DisplayOpenGL::WindowsLink()
	
***************************************/

void BURGER_API Burger::DisplayOpenGL::WindowsUnlink(void)
{
	const char **ppWork = OpenGLNames;
	void **ppDest = &OpenGLProcPtrs.glBlendEquationSeparate;
	Word uCount = BURGER_ARRAYSIZE(OpenGLNames);
	do {
		ppDest[0] = NULL;
		++ppWork;
		++ppDest;
	} while (--uCount);
}

#endif
