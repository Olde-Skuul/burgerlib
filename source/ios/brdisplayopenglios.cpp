/***************************************

	OpenGL manager class

	iOS only

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopengl.h"
#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brgameapp.h"

#if !defined(DOXYGEN)
#define GL_GLEXT_PROTOTYPES
#define __BURGER__
#endif

#include "brgl.h"
#include "brglext.h"
#import <UIKit/UIKit.h>

#if !defined(DOXYGEN)

@interface BurgerOpenGLView : UIView<UITextFieldDelegate> {
	CAEAGLLayer *m_pGLLayer;
	EAGLContext *m_pEAGLContext;
}
- (id)initWithFrame:(CGRect)frame parent:(Burger::Display *)pParent;
- (CAEAGLLayer *)getLayer;
- (EAGLContext *)getContext;
@end

@implementation BurgerOpenGLView
- (id)initWithFrame:(CGRect)frame parent:(Burger::Display *)pParent {
	self = [super initWithFrame:frame];
	// Boned?
	if (self != nil) {
		m_pGLLayer = (CAEAGLLayer *)self.layer;
		m_pGLLayer.opaque = YES;
		m_pGLLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithBool: FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		m_pEAGLContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:nil];
		if (!m_pEAGLContext || ![EAGLContext setCurrentContext:m_pEAGLContext]) {
			[self release];
			Burger::Debug::Warning("OpenGL ES 2 not supported");
			self = nil;
		} else {
			if ([self respondsToSelector:@selector(contentScaleFactor)]) {
				self.contentScaleFactor = pParent->GetRetinaScale();
			}
			self.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
			self.autoresizesSubviews = YES;
		}
	}
	return self;
}

- (void)dealloc
{
	if ([EAGLContext currentContext] == m_pEAGLContext) {
		[EAGLContext setCurrentContext:nil];
	}
	[m_pEAGLContext release];
	m_pEAGLContext = NULL;
	[super dealloc];
}

- (void)updateFrame
{
//	Burger::Debug::Message("updateframe\n");
}

- (void)setCurrentContext
{
	[EAGLContext setCurrentContext:m_pEAGLContext];
}

- (void)swapBuffers
{
	/* viewRenderbuffer should always be bound here. Code that binds something
	 else is responsible for rebinding viewRenderbuffer, to reduce
	 duplicate state changes. */
	[m_pEAGLContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)layoutSubviews
{
	[EAGLContext setCurrentContext:m_pEAGLContext];
	[self updateFrame];
}

- (CAEAGLLayer *)getLayer {
	return m_pGLLayer;
}

- (EAGLContext *)getContext {
	return m_pEAGLContext;
}

/***************************************

	This forces the class to be a CAEAGLLayer

***************************************/

+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

@end

/***************************************

	\brief Initialize OpenGL

	Base class for instantiating a video display using OpenGL

	\param pGameApp Pointer to the active game application pointer
	\sa Burger::Display::~Display()

***************************************/

Burger::Display::Display(GameApp *pGameApp) :
	m_pCompressedFormats(NULL),
	m_pEAGLContext(NULL),
	m_uFrontBuffer(0),
	m_uColorRenderBuffer(0),
	m_uDepthRenderBuffer(0),
	m_fOpenGLVersion(0.0f),
	m_fShadingLanguageVersion(0.0f),
	m_uCompressedFormatCount(0),
	m_uMaximumVertexAttributes(0),
	m_uMaximumColorAttachments(0)
{
	InitDefaults(pGameApp);
	// Start by getting the screen size in POINTS
	UIScreen *pMainScreen = [UIScreen mainScreen];

	m_uDisplayWidth = g_Globals.m_uDefaultWidth;
	m_uDisplayHeight = g_Globals.m_uDefaultHeight;

	// Create a default view of the current screen size in points
	CGRect ScreenRect = pMainScreen.bounds;
	BurgerOpenGLView *pView = [[BurgerOpenGLView alloc] initWithFrame:ScreenRect parent:this];
	pGameApp->SetViewController(pGameApp->GetViewController(),pView);
	m_pEAGLContext = [pView getContext];
}

#endif

/*! ************************************

	\brief Shut down OpenGL

	Release all resources allocated by OpenGL

	\sa DisplayOpenGL()

 ***************************************/

Burger::Display::~Display()
{
	if (m_uFrontBuffer) {
		glDeleteFramebuffers(1,&m_uFrontBuffer);
		m_uFrontBuffer = 0;
	}
	if (m_uColorRenderBuffer) {
		glDeleteRenderbuffers(1,&m_uColorRenderBuffer);
		m_uColorRenderBuffer = 0;
	}
	if (m_uDepthRenderBuffer) {
		glDeleteRenderbuffers(1,&m_uDepthRenderBuffer);
		m_uDepthRenderBuffer = 0;
	}
	// Release the OpenGL Context
//	if ([EAGLContext currentContext] == m_pEAGLContext) {
//		[EAGLContext setCurrentContext:nil];
//	}
//	[m_pEAGLContext release];
//	m_pEAGLContext = NULL;
}

/***************************************

	\brief Start up the OpenGL context

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

Word Burger::Display::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	// Set the new size of the screen
	uWidth = m_uDisplayWidth;
	uHeight = m_uDisplayHeight;

	SetWidthHeight(uWidth,uHeight);
	m_uDepth = uDepth;
	m_uFlags = uFlags;

	m_uFlags |= FULLPALETTEALLOWED;

	//
	// Create an auto-release pool for memory clean up
	//

	NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc] init];

	[EAGLContext setCurrentContext:m_pEAGLContext];
	// Create the main rendering buffer
	glGenFramebuffers(1,&m_uFrontBuffer);
	// And the color buffer
	glGenRenderbuffers(1,&m_uColorRenderBuffer);
	// And the Z buffer
	glGenRenderbuffers(1,&m_uDepthRenderBuffer);

	// Bind the frame buffers
	glBindFramebuffer(GL_FRAMEBUFFER,m_uFrontBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,m_uColorRenderBuffer);

	// Get the screen resolution
	[m_pEAGLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:[static_cast<BurgerOpenGLView *>(m_pGameApp->GetView()) getLayer]];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,m_uColorRenderBuffer);
	GLint backingWidth;
	GLint backingHeight;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&backingHeight);

	// Create the Z buffer based on the screen size

	glBindRenderbuffer(GL_RENDERBUFFER,m_uDepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,backingWidth,backingHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,m_uDepthRenderBuffer);
	GLenum uError = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (uError != GL_FRAMEBUFFER_COMPLETE) {
		Debug::Message("Failed to make complete framebuffer object %s",GetErrorString(uError));
		return 10;
	}
	[pPool release];
	SetupOpenGL();
	return 0;
}

/***************************************

	\brief Start up the OpenGL context

	Shut down OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

void Burger::Display::Shutdown(void)
{
	Free(m_pCompressedFormats);
	m_pCompressedFormats = NULL;
	m_uCompressedFormatCount = 0;
}

/***************************************

	\brief Start up the OpenGL rendering

	Start OpenGL for rendering

***************************************/

void Burger::Display::BeginScene(void)
{
	[EAGLContext setCurrentContext:m_pEAGLContext];
	glBindFramebuffer(GL_FRAMEBUFFER,m_uFrontBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,m_uColorRenderBuffer);
}

/***************************************

	\brief Update the video display

	Calls SwapBuffers() in OpenGL to draw the rendered scene

	\sa Burger::DisplayOpenGL::PreBeginScene()

***************************************/

static const GLenum g_DiscardBuffers[]  = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};

void Burger::Display::EndScene(void)
{
	// For iOS, hint to it that the frame buffer is discardable after rendering
	glDiscardFramebuffer(GL_FRAMEBUFFER,2,g_DiscardBuffers);

	// Consider it done!
	// Force update
	[m_pEAGLContext presentRenderbuffer:GL_RENDERBUFFER];
}

/*! ************************************

	\fn EAGLContext *Burger::Display::GetGLContext(void) const
	\brief The currently active OpenGL context

	\iosonly
	\return Pointer to the active OpenGL context

***************************************/

/*! ************************************

	\fn Word Burger::Display::GetFrontBuffer(void) const
	\brief Front buffer index

	When an OpenGL context is created, a front buffer is declared
	and initialized. This index allows user code to access the
	front buffer through this index

	This function returns zero on all platforms except iOS, where it
	returns the created front buffer

	\return Color buffer index obtained from glGenFramebuffers()
	\sa GetDepthBuffer(void) const or GetColorBuffer(void) const

***************************************/

/*! ************************************

	\fn Word Burger::Display::GetColorBuffer(void) const
	\brief Color buffer index

	When an OpenGL context is created, a color buffer is declared
	and initialized. This index allows user code to access the
	color buffer through this index

	\iosonly
	\return Color buffer index obtained from glGenRenderbuffers()
	\sa GetDepthBuffer(void) const or GetFrontBuffer(void) const

***************************************/

/*! ************************************

	\fn Word Burger::Display::GetDepthBuffer(void) const
	\brief Depth buffer index

	When an OpenGL context is created, a depth buffer is declared
	and initialized. This index allows user code to access the
	depth buffer through this index

	\iosonly
	\return Depth buffer index obtained from glGenRenderbuffers()
	\sa GetColorBuffer(void) const or GetFrontBuffer(void) const

***************************************/

/*! ************************************

	\fn float Burger::Display::GetRetinaScale(void) const
	\brief Get the scale factor for the retina display

	Some iOS devices have displays that are higher resolution than
	the "Point" system they present as. Older systems use a 1:1
	ratio for points to pixels where more recent devices are 2:1
	or higher. This value represents the scale from Points to
	Pixels

	\iosonly
	\return Scale factor for Points to Pixels (Usually 1.0f or 2.0f)

***************************************/

#endif
