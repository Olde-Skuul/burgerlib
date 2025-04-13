/***************************************

	Display base class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"
#include "brmemoryfunctions.h"
#include "brpalette.h"
#include "brrenderer.h"
#include "brtick.h"

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

/*! ************************************

	\class Burger::Display

	\brief Base class for instantiating a video display and the API to drive it.

	This class manages video displays, resources and APIs to render images.

	Since palettes are tied to a video display, this class will manage
	palettes for palette display contexts. If the display is in true color,
	palette functions will only update internal buffers but will not affect
	actual rendering.

	Classes for specific rendering APIs derive from this base class.

	\sa Burger::Renderer, Burger::Display, Burger::DisplayOpenGL,
		Burger::DisplayDirectX9, Burger::DisplayDirectX11,
		Burger::DisplayOpenGLSoftware16 or Burger::DisplayOpenGLSoftware8

***************************************/

//
// Global display variables
//

Burger::Display::Globals_t Burger::Display::g_Globals;

/*! ************************************

	\struct Burger::Display::Globals_t

	\brief Values to describe the default settings of the display

	This is a description of the default values for the settings of the display
	as set by the user's desktop.

	\sa Display

***************************************/

/*! ************************************

	\enum Burger::Display::eClearBits

	\brief Settings for Clear(uint_t)

	Bitfield for which buffers to clear upon the start of rendering a frame
	buffer

	\sa Clear(uint_t)

***************************************/

/*! ************************************

	\enum Burger::Display::eDepthFunction

	\brief Settings for SetDepthTest(eDepthFunction)

	Enumeration to set the depth test type.

	\sa SetDepthTest(eDepthFunction)

***************************************/

/*! ************************************

	\enum Burger::Display::eCullMode

	\brief Settings for SetCullMode(eCullMode)

	Enumeration to set the polygon culling mode.

	\sa SetCullMode(eCullMode)

***************************************/

/*! ************************************

	\enum Burger::Display::eSourceBlendFactor

	\brief Settings for SetBlendFunction(eSourceBlendFactor)

	Enumeration to set the source pixel blending mode.

	\sa SetBlendFunction(eSourceBlendFactor,eDestinationBlendFactor)

***************************************/

/*! ************************************

	\enum Burger::Display::eDestinationBlendFactor

	\brief Settings for SetBlendFunction(eDestinationBlendFactor)

	Enumeration to set the destination pixel blending mode.

	\sa SetBlendFunction(eSourceBlendFactor,eDestinationBlendFactor)

***************************************/

/*! ************************************

	\enum Burger::Display::ePrimitiveType

	\brief Describe how to render a vertex array.

	Enumeration to determine how to render an array of vertices

	\sa DrawElements(ePrimitiveType) or DrawPrimitive(ePrimitiveType)

***************************************/

/*! ************************************

	\struct Burger::Display::VideoMode_t

	\brief Description of a video card's video mode

	This is a description of a single video mode available in a video card

	\sa Display or Display::VideoCardDescription

***************************************/

/*! ************************************

	\struct Burger::Display::OpenGLVertexInputs_t

	\brief Description of an OpenGL vertex input list

	This is a description of an OpenGL shader's input labels and how
	they map to a vertex buffer's parameter hint

	\sa Display or VertexBuffer

***************************************/

/*! ************************************

	\struct Burger::Display::OpenGLVertexBufferObjectDescription_t

	\brief Description of an OpenGL vertex buffer object

	This is a description of an OpenGL vertex buffer object

	\sa Display or DisplayOpenGL

***************************************/

/*! ************************************

	\class Burger::Display::VideoCardDescription

	\brief Description of a video card's available video modes

	This class contains a description of a video card and a list
	of all the available display modes.

	\sa Display

***************************************/

/*! ************************************

	\brief Standard constructor

	\sa ~VideoCardDescription()

***************************************/

Burger::Display::VideoCardDescription::VideoCardDescription():
	m_Array(),
#if defined(BURGER_MACOSX)
	m_pNSScreen(NULL),
#endif
	m_DeviceName(),
	m_MonitorName(),
	m_uDevNumber(0),
	m_uFlags(0)
{
	m_SystemRect.Clear();
	m_CurrentResolution.Clear();
#if defined(BURGER_WINDOWS)
	MemoryClear(&m_GUID, sizeof(m_GUID));
#endif
}

/*! ************************************

	\brief Standard destructor

	\sa VideoCardDescription or VideoCardDescription()

***************************************/

Burger::Display::VideoCardDescription::~VideoCardDescription() {}

/*! ************************************

	\brief Initialize video globals

	Before an application is allowed to change the display mode, this
	function will query the system what are the current display mode
	so these value can be used as defaults for changing a display mode
	or for the application can be aware of what the user has
	already set the platform to display with.

	\sa InitDefaults(GameApp *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS) || defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
void BURGER_API Burger::Display::InitGlobals(void)
{
	if (!g_Globals.m_bInitialized) {
		g_Globals.m_uDefaultWidth = 0;
		g_Globals.m_uDefaultHeight = 0;
		g_Globals.m_uDefaultDepth = 0;
		g_Globals.m_uDefaultHertz = 0;
		g_Globals.m_uDefaultTotalWidth = 0;
		g_Globals.m_uDefaultTotalHeight = 0;
		g_Globals.m_uDefaultMonitorCount = 0;
		g_Globals.m_bInitialized = TRUE;
	}
}
#endif

/*! ************************************

	\brief Initialize shared variables

	To ease the porting of the Display class to other platforms,
	the initialization of variables that are common to all platforms
	is performed in this function and then platform specific
	variables will be initialized in platform specific code.

	\sa Display(GameApp *) or InitGlobals(void)

***************************************/

void BURGER_API Burger::Display::InitDefaults(GameApp* pGameApp)
{
	m_pGameApp = pGameApp;
	m_pRenderer = NULL;

#if defined(BURGER_WINDOWS)
	MemoryClear(m_WindowPlacement, sizeof(m_WindowPlacement));
#endif

	m_pResize = NULL;
	m_pResizeData = NULL;
	m_pRender = NULL;
	m_pRenderData = NULL;
	m_pRelease = NULL;
	m_pReleaseData = NULL;

	SetWidthHeight(0, 0);
	m_uDepth = 0;
	m_uFlags = 0;
	m_uDisplayWidth = 0;
	m_uDisplayHeight = 0;
	m_uDisplayDepth = 0;

	m_uBorderColor = 0;
	m_uPaletteFadeSpeed = Tick::kTicksPerSecond / 15;

	m_iPauseRenderingCount = 0;
	m_bRenderingPaused = FALSE;
	m_bPaletteDirty = TRUE;
	m_bPaletteVSync = FALSE;

	MemoryClear(m_pBoundTextures, sizeof(m_pBoundTextures));
	MemoryClear(m_Palette, sizeof(m_Palette));
#if defined(BURGER_MACOS)
	m_Palette[0 * 3 + 0] = 255;
	m_Palette[0 * 3 + 1] = 255;
	m_Palette[0 * 3 + 2] = 255;
#else
	m_Palette[255 * 3 + 0] = 255;
	m_Palette[255 * 3 + 1] = 255;
	m_Palette[255 * 3 + 2] = 255;
#endif
	InitGlobals();
}

/*! ************************************

	\brief Set the width and height of the screen

	Sets the width and height in pixels of the display and updates all other
	variables that depend on these values

	\param uWidth New width of the screen in pixels
	\param uHeight New height of the screen in pixels

***************************************/

void BURGER_API Burger::Display::SetWidthHeight(uint_t uWidth, uint_t uHeight)
{
	// Set the initial globals
	m_uWidth = uWidth;
	m_uHeight = uHeight;

	// Convert to floats (Some functions prefer it in float format,
	// so it's converted once at this location for performance)
	float fWidth = static_cast<float>(static_cast<int>(uWidth));
	float fHeight = static_cast<float>(static_cast<int>(uHeight));
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// Both width and height are valid?
	if (uHeight && uWidth) {
		m_fAspectRatioX = fWidth / fHeight;
		m_fAspectRatioY = fHeight / fWidth;
	} else {
		// Set the aspect ratio to 1:1
		// in cases where the screen size is 0,y or x,0
		// This also prevents a divide by zero in the code above
		m_fAspectRatioX = 1.0f;
		m_fAspectRatioY = 1.0f;
	}
}

/*! ************************************

	\brief Default constructor.

	Initializes all of the shared variables and hooks up the Display and the
	Renderer to the GameApp.

	Variables are initialized, but the display is not activated. Call
	Init(uint_t,uint_t,uint_t,uint_t) to activate the display.

	\param pGameApp Pointer to the game application
	\sa Init(uint_t,uint_t,uint_t,uint_t)

***************************************/

#if defined(BURGER_WINDOWS) || \
	!(defined(BURGER_XBOX360) || defined(BURGER_OPENGL)) || defined(DOXYGEN)
Burger::Display::Display(GameApp* pGameApp)
{
	InitDefaults(pGameApp);
}
#endif

/*! ************************************

	\brief Default destructor.

	Shuts down the video context and issues a shutdown to the renderer

	\sa Shutdown() and SetRenderer()

***************************************/

#if defined(BURGER_LINUX) || defined(BURGER_WINDOWS) || \
	!(defined(BURGER_XBOX360) || defined(BURGER_OPENGL)) || defined(DOXYGEN)
Burger::Display::~Display() {}
#endif

/*! ************************************

	\fn uint_t Burger::Display::Init(uint_t uWidth,uint_t uHeight,
		uint_t uDepth,uint_t uFlags)
	\brief Initialize the display

	Set up the video display hardware to the specified mode and depth.
	This calls derived functions and it's the responsibility of the underlying
	class to use the appropriate API such as OpenGL, DirectX or something else
	to perform the display operations and then call the base class to complete
	the setup

	\param uWidth Width of the requested display in pixels
	\param uHeight Height of the requested display in pixels
	\param uDepth Bit depth of the requested display in bits
	\param uFlags Helper flags for hinting at the preferred display
	\return Zero if no error, non-zero if an error has occurred.

	\sa Shutdown() and InitContext()

***************************************/

#if defined(BURGER_WINDOWS) || \
	!(defined(BURGER_XBOX360) || defined(BURGER_OPENGL)) || defined(DOXYGEN)
#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
uint_t Burger::Display::Init(uint_t /* uWidth */, uint_t /* uHeight */,
	uint_t /* uDepth */, uint_t /* uFlags */)
{
	return 10;
}

/*! ************************************

	\brief Shut down the current video display context

	Release all resources and restore the video display to the
	system defaults. This is an internal function and is meant
	to be called as part of a call to Display::Shutdown()
	so the renderer is shut down first before the display

	This code does nothing. It's a placeholder for classes that have no
	need for a shutdown call

	\sa Init(uint_t,uint_t,uint_t,uint_t)

***************************************/

void Burger::Display::Shutdown(void) {}

/*! ************************************

	\fn void Burger::Display::BeginScene(void)
	\brief Prepare the display for rendering.

	This function will call the Renderer::BeginScene() function
	after any operating system calls are issued so the renderer can prepare for
	drawing.

	This must be paired with a subsequent call to Burger::Display::EndScene()

	\sa Burger::Display::EndScene()

***************************************/

/*! ************************************

	\fn void Burger::Display::EndScene(void)
	\brief Render the scene to the display

	Alert the operating system that all rendering calls are complete
	and the hardware is permitted to begin rendering the scene.

	The renderer is issued a call to Renderer::EndScene() before
	the operating system so all rendering calls can be completed before
	the scene is sent to the GPU or other rendering sub-system.

	\sa Burger::Display::BeginScene()

***************************************/

void Burger::Display::EndScene(void) {}

Burger::Texture* Burger::Display::CreateTextureObject(void)
{
	return new (Alloc(sizeof(Texture))) Texture;
}

Burger::VertexBuffer* Burger::Display::CreateVertexBufferObject(void)
{
	return new (Alloc(sizeof(VertexBuffer))) VertexBuffer;
}

void Burger::Display::Resize(uint_t uWidth, uint_t uHeight)
{
	SetWidthHeight(uWidth, uHeight);
}

void Burger::Display::SetViewport(
	uint_t /* uX */, uint_t /* uY */, uint_t /* uWidth */, uint_t /* uHeight */)
{
}

void Burger::Display::SetScissorRect(
	uint_t /* uX */, uint_t /* uY */, uint_t /* uWidth */, uint_t /* uHeight */)
{
}

void Burger::Display::SetClearColor(
	float /* fRed */, float /* fGreen */, float /* fBlue */, float /* fAlpha */)
{
}

void Burger::Display::SetClearDepth(float /* fDepth */) {}

void Burger::Display::Clear(uint_t /* uMask */) {}

void Burger::Display::Bind(Texture* pTexture, uint_t uIndex)
{
	BURGER_ASSERT(uIndex < BURGER_ARRAYSIZE(m_pBoundTextures));
	m_pBoundTextures[uIndex] = pTexture;
}

void Burger::Display::Bind(Effect* /* pEffect */) {}

void Burger::Display::SetBlend(uint_t /* bEnable */) {}

void Burger::Display::SetBlendFunction(eSourceBlendFactor /* uSourceFactor */,
	eDestinationBlendFactor /* uDestFactor */)
{
}

void Burger::Display::SetLighting(uint_t /* bEnable */) {}

void Burger::Display::SetZWrite(uint_t /* bEnable */) {}

void Burger::Display::SetDepthTest(eDepthFunction /* uDepthFunction */) {}

void Burger::Display::SetCullMode(eCullMode /* uCullMode */) {}

void Burger::Display::SetScissor(uint_t /* bEnable */) {}

void Burger::Display::DrawPrimitive(
	ePrimitiveType /* uPrimitiveType */, VertexBuffer* /* pVertexBuffer */)
{
}

void Burger::Display::DrawElements(
	ePrimitiveType /* uPrimitiveType */, VertexBuffer* /* pVertexBuffer */)
{
}

#endif
#endif

/*! ************************************

	\brief Pause or resume rendering

	Set or clear the flag to pause rendering. This is required
	to keep background tasks from attempting to
	use the display device for rendering while it's
	in a transitory state.

	\note Pausing is reference counted, match every call to
		\ref Pause( \ref TRUE) with a call of \ref Pause( \ref FALSE)

	\param bPauseRendering \ref TRUE to pause rendering, \ref FALSE to resume
		rendering.

***************************************/

void BURGER_API Burger::Display::Pause(uint_t bPauseRendering)
{
	int iPauseRenderingCount = m_iPauseRenderingCount;
	if (bPauseRendering) {
		++iPauseRenderingCount;
	} else {
		--iPauseRenderingCount;
	}
	// Make sure it never goes negative
	if (iPauseRenderingCount < 0) {
		iPauseRenderingCount = 0;
	}
	m_iPauseRenderingCount = iPauseRenderingCount;
	m_bRenderingPaused = (iPauseRenderingCount > 0);
}

/*! ************************************

	\brief Create a texture object with wrapping and filters preset

	Create a texture object and set the wrapping and
	filter settings.

	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant
	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTexture(
	Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with buffer and wrapping and filters preset

	Create a texture object and set the wrapping and
	filter settings and create a buffer to hold the pixel map information.

	\param uWidth Width of the texture in pixels
	\param uHeight Height of the texture in pixels
	\param uPixelType Type of pixel data contained in the bitmap
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTexture(uint_t uWidth,
	uint_t uHeight, Image::ePixelTypes uPixelType, Texture::eWrapping uWrapping,
	Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		if (pTexture->GetImage()->Init(uWidth, uHeight, uPixelType)) {
			Delete(pTexture);
			pTexture = NULL;
		} else {
			pTexture->SetWrapping(uWrapping);
			pTexture->SetFilter(uFilter);
		}
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a PNG file.

	\param pFilename Pointer to a "C" string of a Burgerlib path of the texture
		file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTexturePNG(
	const char* pFilename, Texture::eWrapping uWrapping,
	Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadPNG(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and
	filter settings and set it up to obtain the bitmap from a PNG file.

	\param pFilename Pointer to a Burgerlib \ref Filename object with the
		filename
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTexturePNG(
	Filename* pFilename, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadPNG(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and
	filter settings and set it up to obtain the bitmap from a PNG file.

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Chuck ID of the data containing the image file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant
	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTexturePNG(RezFile* pRezFile,
	uint_t uRezNum, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadPNG(pRezFile, uRezNum);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and
	filter settings and set it up to obtain the bitmap from a GIF file.

	\param pFilename Pointer to a "C" string of a Burgerlib path of the texture
		file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureGIF(
	const char* pFilename, Texture::eWrapping uWrapping,
	Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadGIF(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and
	filter settings and set it up to obtain the bitmap from a GIF file.

	\param pFilename Pointer to a Burgerlib \ref Filename object with the
		filename
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureGIF(
	Filename* pFilename, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadGIF(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a GIF file.

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Chuck ID of the data containing the image file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureGIF(RezFile* pRezFile,
	uint_t uRezNum, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadGIF(pRezFile, uRezNum);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a TGA file.

	\param pFilename Pointer to a "C" string of a Burgerlib path of the texture
		file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureTGA(
	const char* pFilename, Texture::eWrapping uWrapping,
	Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadTGA(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a TGA file.

	\param pFilename Pointer to a Burgerlib \ref Filename object with the
		filename
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureTGA(
	Filename* pFilename, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadTGA(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a TGA file.

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Chuck ID of the data containing the image file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant
	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureTGA(RezFile* pRezFile,
	uint_t uRezNum, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadTGA(pRezFile, uRezNum);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a BMP file.

	\param pFilename Pointer to a "C" string of a Burgerlib path of the texture
		file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant
	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureBMP(
	const char* pFilename, Texture::eWrapping uWrapping,
	Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadBMP(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a BMP file.

	\param pFilename Pointer to a Burgerlib \ref Filename object with the
		filename
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureBMP(
	Filename* pFilename, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadBMP(pFilename);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a texture object with wrapping, filters and a texture source

	Create a texture object and set the wrapping and filter settings and set it
	up to obtain the bitmap from a BMP file.

	\param pRezFile Pointer to a Burgerlib RezFile
	\param uRezNum Chuck ID of the data containing the image file
	\param uWrapping Texture wrapping constant
	\param uFilter Texture filter constant

	\return \ref NULL if the object couldn't be created or a valid texture
		object.

	\sa CreateTextureObject()

***************************************/

Burger::Texture* BURGER_API Burger::Display::CreateTextureBMP(RezFile* pRezFile,
	uint_t uRezNum, Texture::eWrapping uWrapping, Texture::eFilter uFilter)
{
	Texture* pTexture = CreateTextureObject();
	if (pTexture) {
		pTexture->SetWrapping(uWrapping);
		pTexture->SetFilter(uFilter);
		pTexture->LoadBMP(pRezFile, uRezNum);
	}
	return pTexture;
}

/*! ************************************

	\brief Create a vertex buffer object with from a vertex description

	Given an Array of Structures description, create a vertex buffer
	that is initialized with the data

	\param pDescription Pointer to a description
	\return \ref NULL if the object couldn't be created or a valid vertex
		object.

	\sa CreateVertexBufferObject()

***************************************/

Burger::VertexBuffer* BURGER_API Burger::Display::CreateVertexBuffer(
	const VertexBuffer::VertexAoS_t* pDescription)
{
	VertexBuffer* pVertexBuffer = CreateVertexBufferObject();
	if (pVertexBuffer) {
		// Discard on error
		if (pVertexBuffer->LoadData(this, pDescription)) {
			Delete(pVertexBuffer);
			pVertexBuffer = NULL;
		}
	}
	return pVertexBuffer;
}

/*! ************************************

	\brief Get a list of available video modes

	\param pOutput Pointer to array of VideoCardDescription entries
	\return Zero if no error, non-zero on error

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOS) || \
	defined(BURGER_IOS) || defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
uint_t Burger::Display::GetVideoModes(ClassArray<VideoCardDescription>* pOutput)
{
	pOutput->clear();
	return 10;
}
#endif

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplets of Red,Green, and Blue.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pPalette Base pointer to the colors to use in the update in the size
		of uCount*3

	\sa SetPalette(const uint8_t *) or SetPalette(void **)

***************************************/

void BURGER_API Burger::Display::SetPalette(
	uint_t uStart, uint_t uCount, const uint8_t* pPalette)
{
	// Bad?
	if (pPalette && uStart < 256) {
		// Out of range?
		if ((uStart + uCount) >= 257) {
			// Set the limit on the count
			uCount = 256 - uStart;
		}
		// Are colors 0 and 255 reserved?
		if (!(m_uFlags & FULLPALETTEALLOWED)) {

			// Hard code the first and last colors to black and white
			// Note: MacOS has black and white reversed!
#if defined(BURGER_MACOS)
			m_Palette[0 * 3 + 0] = 255;
			m_Palette[0 * 3 + 1] = 255;
			m_Palette[0 * 3 + 2] = 255;
			m_Palette[255 * 3 + 0] = 0;
			m_Palette[255 * 3 + 1] = 0;
			m_Palette[255 * 3 + 2] = 0;
#else
			m_Palette[0 * 3 + 0] = 0;
			m_Palette[0 * 3 + 1] = 0;
			m_Palette[0 * 3 + 2] = 0;
			m_Palette[255 * 3 + 0] = 255;
			m_Palette[255 * 3 + 1] = 255;
			m_Palette[255 * 3 + 2] = 255;
#endif
			if (uCount) {
				// Starting at color #0?
				if (!uStart) {
					// Remove it from the update list
					++uStart;
					--uCount;
					pPalette += 3;
				}
			}

			// Only updating the last color?
			if (uStart >= 255) {
				uCount = 0; // Kill it
			}
			// Is the last color part of the range?
			if (uStart + uCount == 255) {
				--uCount; // Remove it from the end (254 is the highest allowed)
			}
		}
		// If any colors survived the pruning, update them
		if (uCount &&
			(m_bPaletteDirty ||
				MemoryCompare(
					m_Palette + (uStart * 3), pPalette, uCount * 3))) {
			MemoryCopy(m_Palette + (uStart * 3), pPalette,
				uCount * 3); // Update the palette
			m_bPaletteDirty = TRUE;
		}
	}
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of RGBAWord8_t entries.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pPalette Base pointer to an array of RGBAWord8_t colors to use in the
		update in the size of uCount

	\sa SetPalette(const uint8_t *) or SetPalette(void **)

***************************************/

void BURGER_API Burger::Display::SetPalette(
	uint_t uStart, uint_t uCount, const RGBAWord8_t* pPalette)
{
	// Bad?
	if (pPalette && uStart < 256) {
		// Out of range?
		if ((uStart + uCount) >= 257) {
			// Set the limit on the count
			uCount = 256 - uStart;
		}
		// Are colors 0 and 255 reserved?
		if (!(m_uFlags & FULLPALETTEALLOWED)) {

			// Hard code the first and last colors to black and white
			// Note: MacOS has black and white reversed!
#if defined(BURGER_MACOS)
			m_Palette[0 * 3 + 0] = 255;
			m_Palette[0 * 3 + 1] = 255;
			m_Palette[0 * 3 + 2] = 255;
			m_Palette[255 * 3 + 0] = 0;
			m_Palette[255 * 3 + 1] = 0;
			m_Palette[255 * 3 + 2] = 0;
#else
			m_Palette[0 * 3 + 0] = 0;
			m_Palette[0 * 3 + 1] = 0;
			m_Palette[0 * 3 + 2] = 0;
			m_Palette[255 * 3 + 0] = 255;
			m_Palette[255 * 3 + 1] = 255;
			m_Palette[255 * 3 + 2] = 255;
#endif
			if (uCount) {
				// Starting at color #0?
				if (!uStart) {
					// Remove it from the update list
					++uStart;
					--uCount;
					++pPalette;
				}
			}

			// Only updating the last color?
			if (uStart >= 255) {
				uCount = 0; // Kill it
			}
			// Is the last color part of the range?
			if (uStart + uCount == 255) {
				--uCount; // Remove it from the end (254 is the highest allowed)
			}
		}
		// If any colors survived the pruning, update them
		if (uCount) {
			uint8_t* pDest = m_Palette + (uStart * 3);
			// Copy up the colors, ignoring the alpha
			do {
				pDest[0] = pPalette->m_uRed;
				pDest[1] = pPalette->m_uGreen;
				pDest[2] = pPalette->m_uBlue;
				++pPalette;
				pDest += 3;
			} while (--uCount);
			m_bPaletteDirty = TRUE;
		}
	}
}

/*! ************************************

	\brief Update the display border color

	On some platforms, such as MSDOS VGA/MCGA or the Commodore Amiga,
	the display's border color can be set. This function will set
	that color. On all other platforms, the function only logs the
	color change, otherwise it does nothing.

	\note On MSDOS, the EGA value of the border color is 4 bits (0-15).

	\param uColor Border color that is specific to the hardware being updated.

	\sa GetBorderColor()

***************************************/

#if (!defined(BURGER_MSDOS)) || defined(DOXYGEN)
void BURGER_API Burger::Display::SetBorderColor(uint_t uColor)
{
	m_uBorderColor = uColor;
}
#endif

/*! ************************************

	\brief Set the display window title

	On desktop platforms such as Windows or MacOS, the game could be running in
	a desktop window. The window can have a title string, and it can be set
	using this function.

	On consoles and handhelds, this function does nothing.

	\param pTitle UTF-8 string to display for the title bar

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
void BURGER_API Burger::Display::SetWindowTitle(const char* /* pTitle */) {}
#endif

/*! ************************************

	\brief Set the screen palette to all black

	Set all 8 bit palette color entries to zero (Black)

	\sa SetPalette(const uint8_t *), SetPalette(uint_t,uint_t,const uint8_t *)
		or SetPaletteWhite()

***************************************/

void BURGER_API Burger::Display::SetPaletteBlack(void)
{
	// Perform a compare to the palette to force an update only when changed
	uint8_t TempPalette[sizeof(m_Palette)];
	MemoryClear(m_Palette, sizeof(m_Palette));
	SetPalette(TempPalette);
}

/*! ************************************

	\brief Set the screen palette to all white

	Set all 8 bit palette color entries to 255 (White)

	\sa SetPalette(const uint8_t *), SetPalette(uint_t,uint_t,const uint8_t *)
		or SetPaletteBlack()

***************************************/

void BURGER_API Burger::Display::SetPaletteWhite(void)
{
	uint8_t TempPalette[sizeof(m_Palette)];
	MemoryFill(TempPalette, 255, sizeof(TempPalette));
	SetPalette(TempPalette);
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplets of Red,Green, and Blue.

	\param pPalette Base pointer to the colors to use in the update in the size
		of 256*3 (768)

	\sa SetPalette(void **)

***************************************/

void BURGER_API Burger::Display::SetPalette(const uint8_t* pPalette)
{
	SetPalette(0, 256, pPalette);
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of RGBAWord8_t.

	\param pPalette Base pointer to the array of 256 RGBAWord8_t colors to use
		in the update

	\sa SetPalette(void **)

***************************************/

void BURGER_API Burger::Display::SetPalette(const RGBAWord8_t* pPalette)
{
	SetPalette(0, 256, pPalette);
}

/*! ************************************

	\brief Update the color palette using a memory handle

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplets of Red,Green, and Blue.

	\param pHandle Base handle to the colors to use in the update in the size of
		256*3 (768)

	\sa SetPalette(const uint8_t *)

***************************************/

void BURGER_API Burger::Display::SetPalette(void** pHandle)
{
	const uint8_t* pPalette =
		static_cast<const uint8_t*>(MemoryManagerHandle::lock(pHandle));
	if (pPalette) {
		SetPalette(0, 256, pPalette);
		MemoryManagerHandle::unlock(pHandle);
	}
}

/*! ************************************

	\brief Update the color palette using a resource

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplets of Red,Green, and Blue.

	\param pRez Reference to the resource file
	\param uResID Resource entry the contains the 768 byte palette
	\sa SetPalette(const uint8_t *)

***************************************/

void BURGER_API Burger::Display::SetPalette(RezFile* pRez, uint_t uResID)
{
	const uint8_t* pPalette = static_cast<const uint8_t*>(pRez->Load(uResID));
	if (pPalette) {
		SetPalette(0, 256, pPalette);
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Update a partial color palette using a resource

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplets of Red,Green, and Blue.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pRez Reference to the resource file
	\param uResID Resource entry the contains the palette that's uCount*3 bytes
		in length

	\sa SetPalette(const uint8_t *)

***************************************/

void BURGER_API Burger::Display::SetPalette(
	uint_t uStart, uint_t uCount, RezFile* pRez, uint_t uResID)
{
	const uint8_t* pPalette = static_cast<const uint8_t*>(pRez->Load(uResID));
	if (pPalette) {
		SetPalette(uStart, uCount, pPalette);
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Fade the hardware palette to a palette

	Given an arbitrary palette, and a preset palette speed variable,
	fade from the currently shown palette to the new arbitrary palette
	using smooth steps.

	If the requested palette is the same as Display::m_Palette, this routine
	does nothing.

	Every time the hardware palette is written to, the function pProc()
	is called if the user has set it to a valid function pointer. It is passed a
	0 on startup and the value increments until it reaches 16 when the palette
	is finished. Only parameter 0 and 16 are guaranteed to be called. All other
	values will only be passed once or skipped due to low machine speed.

	\note Running under Windows or BeOS, the thread that calls this routine will
	be put to sleep during time delays between Vertical Blank (VBL) palette
	updates. Keyboard::GetKeyEvent() will only be called no more than the VBL
	rate. All other platforms will wait between VBL's with CPU polling.

	\param pPalette Pointer to the 768 byte palette to fade to.
	\param pProc Function pointer to callback or \ref NULL if callbacks are not
		desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(RezFile *,uint_t,,FadeProc,void *), FadeToWhite(), FadeToBlack(),
		GameApp::Poll(), m_uPaletteFadeSpeed, m_bPaletteVSync, m_Palette,
		Tick::read()

***************************************/

void BURGER_API Burger::Display::FadeTo(
	const uint8_t* pPalette, FadeProc pProc, void* pData)
{
	int DeltaPalette[768];    // Must be SIGNED!
	uint8_t WorkPalette[768]; // Temp palette

	// Same palette?
	if (MemoryCompare(pPalette, m_Palette, 768)) {
		// Save the palette VSync flag
		// Since I am fading, I can wait for VSync
		uint8_t bOldVSync = m_bPaletteVSync;
		m_bPaletteVSync = TRUE;

		// Need to first get the deltas for each color component
		// Since a char is -127 to 128 and the differences are -255 to 255
		// I need the difference table to be an array of int's

		{
			const uint8_t* pOriginal = m_Palette; // Pointer to palette
			int* pDelta = DeltaPalette;           // Pointer to delta
			do {
				uint_t a1 = pOriginal[0]; // Get the values
				uint_t a2 = pPalette[0];
				pDelta[0] = static_cast<int>(a1 - a2); // Calculate the delta
				++pOriginal;
				++pPalette;
				++pDelta;
			} while (pOriginal < &m_Palette[768]);
		}
		// Restore the palette pointer
		pPalette = pPalette - 768;

		{
			// Palette scale temp 0.0-1.0
			fixed16_16_t fScale;
			// Initialize the callback constant
			uint_t LastCall = 0;

			// Get the time base
			uint32_t uMark = Tick::read();
			// Number of ticks to elapse for 16 steps
			uint_t uTotalTicks = 16 * m_uPaletteFadeSpeed;
			do {
				// Yield CPU time if needed
				m_pGameApp->Poll();

				// Get elapsed time
				fScale = static_cast<fixed16_16_t>(Tick::read() - uMark);
				fScale = fScale * (0x10000 / static_cast<fixed16_16_t>(uTotalTicks));
				if (fScale > 0x10000) { // Overflow?
					fScale = 0x10000;   // Cap at 1.0f fixed
				}
				fScale = 0x10000 - fScale;          // Reverse the scale
				uint8_t* pNewPalette = WorkPalette; // Init palette pointers
				int* pDelta = DeltaPalette;
				do {
					// Get a delta
					fixed16_16_t Foo = pDelta[0];
					// Scale the delta by 0.0 to 1.0
					Foo = Foo * fScale;
					// Div by 16 (Signed) (Result is -255 to 255)
					Foo = Foo >> 16;
					pNewPalette[0] = static_cast<uint8_t>(pPalette[0] + Foo);
					++pNewPalette;
					++pPalette;
					++pDelta;
				} while (pNewPalette < &WorkPalette[768]);
				// Restore the pointer
				pPalette = pPalette - 768;
				// Set the new palette
				SetPalette(0, 256, WorkPalette);
				EndScene();

				// Is there a callback?
				if (pProc) {
					// 0-16
					uint_t uCount = static_cast<uint_t>(16 - (fScale >> 12));
					// New value?
					if (uCount > LastCall) {
						LastCall = uCount;
						pProc(pData, uCount); // Call the callback routine
					}
				}
				// All done?
			} while (fScale);
		}
		// Restore the sync value
		m_bPaletteVSync = bOldVSync;
	} else {
		// On occasions where there is no palette change, alert any callback
		// that the stepping concluded
		if (pProc) {
			pProc(pData, 16);
		}
	}
}

/*! ************************************

	\brief Fade the hardware palette to black

	All color entries in the hardware palette are set to black slowly over time.

	\param pProc Function pointer to callback or \ref NULL if callbacks are not
		desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const uint8_t *,FadeProc,void *), SetPaletteBlack(),
		FadeToWhite()

***************************************/

void BURGER_API Burger::Display::FadeToBlack(FadeProc pProc, void* pData)
{
	uint8_t TempPalette[sizeof(m_Palette)];
	MemoryClear(TempPalette, sizeof(TempPalette));
	FadeTo(TempPalette, pProc, pData);
}

/*! ************************************

	\brief Fade the hardware palette to white

	All color entries in the hardware palette are set to white slowly over time.

	\param pProc Function pointer to callback or \ref NULL if callbacks are not
		desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const uint8_t *,FadeProc,void *), SetPaletteWhite(),
		FadeToBlack()

***************************************/

void BURGER_API Burger::Display::FadeToWhite(FadeProc pProc, void* pData)
{
	uint8_t TempPalette[sizeof(m_Palette)];
	MemoryFill(TempPalette, 255, sizeof(TempPalette));
	FadeTo(TempPalette, pProc, pData);
}

/*! ************************************

	\brief Fade the hardware palette to a palette

	All color entries in the hardware palette are set to the new palette slowly
	over time.

	\param pRez Reference to the resource file that has the palette to fade to.
	\param uResID The resource number of the palette
	\param pProc Function pointer to callback or \ref NULL if callbacks are not
		desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const uint8_t *,FadeProc,void *), FadeToWhite(), FadeToBlack()

***************************************/

void BURGER_API Burger::Display::FadeTo(
	RezFile* pRez, uint_t uResID, FadeProc pProc, void* pData)
{
	// Load in the resource file
	const uint8_t* pPalette = static_cast<const uint8_t*>(pRez->Load(uResID));
	if (pPalette) {
		FadeTo(pPalette, pProc, pData);
		// Release the resource data
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Fade the hardware palette to a palette

	All color entries in the hardware palette are set to the new palette slowly
	over time.

	The handle is returned unlocked. Nothing is performed if the handle is
	invalid or purged.

	\param pHandle Handle to the palette to fade to.
	\param pProc Function pointer to callback or \ref NULL if callbacks are not
		desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const uint8_t *,FadeProc,void *), FadeToWhite(), FadeToBlack()

***************************************/

void BURGER_API Burger::Display::FadeTo(
	void** pHandle, FadeProc pProc, void* pData)
{
	// Lock it down
	const uint8_t* pPalette =
		static_cast<const uint8_t*>(MemoryManagerHandle::lock(pHandle));
	if (pPalette) {
		// Perform the fade
		FadeTo(pPalette, pProc, pData);
		// Release the memory
		MemoryManagerHandle::unlock(pHandle);
	}
}

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultWidth(void)
	\brief Get the width of the default monitor

	This is initialized with the size of the user's desktop width from the
	primary display.

	\note This value is only valid after a Display class instance
		was created.

	\return Default monitor width in pixels

	\sa GetDefaultHeight(void), GetDefaultDepth(void) or GetDefaultHertz(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultHeight(void)
	\brief Get the height of the default monitor

	This is initialized with the size of the user's
	desktop height from the primary display.

	\note This value is only valid after a Display class instance
		was created.

	\return Default monitor height in pixels

	\sa GetDefaultWidth(void), GetDefaultDepth(void) or GetDefaultHertz(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultDepth(void)
	\brief Get the pixel depth of the default monitor

	This is initialized with the pixel depth of the user's
	desktop from the primary display.

	\note This value is only valid after a Display class instance
		was created.

	\return Default pixel depth in bits

	\sa GetDefaultWidth(void), GetDefaultHeight(void) or GetDefaultHertz(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultHertz(void)
	\brief Get the refresh rate of the default monitor

	This is initialized with the refresh rate of the user's
	desktop from the primary display.

	\note This value is only valid after a Display class instance
		was created.

	\return Default refresh rate in hertz (Can be zero if not applicable)

	\sa GetDefaultWidth(void), GetDefaultHeight(void) or GetDefaultDepth(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultMonitorCount(void)
	\brief Get the number of active monitors

	This is initialized with the number of monitors used for
	the user's desktop.

	\note This value is only valid after a Display class instance
		was created.

	\return Number of active display monitors used for the desktop.

	\sa GetDefaultWidth(void), GetDefaultHeight(void), GetDefaultDepth(void) or
		GetDefaultHertz(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultTotalWidth(void)
	\brief Get the width of the default monitor

	This is initialized with the size of the user's entire desktop width. It is
	a union of all active monitors.

	\note This value is only valid after a Display class instance
		was created.

	\return Default total width in pixels

	\sa GetDefaultTotalHeight(void), GetDefaultWidth(void) or
		GetDefaultMonitorCount(void)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDefaultTotalHeight(void)
	\brief Get the height of the default monitor

	This is initialized with the size of the user's entire desktop height. It is
	a union of all active monitors.

	\note This value is only valid after a Display class instance
		was created.

	\return Default total height in pixels

	\sa GetDefaultTotalWidth(void), GetDefaultHeight(void) or
		GetDefaultMonitorCount(void)

***************************************/

/*! ************************************

	\fn GameApp * Burger::Display::GetGameApp(void) const
	\brief Get the parent application pointer

	\return Pointer to the parent application
	\sa GetWidth() const, GetHeight() const or GetDepth() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetWidth(void) const
	\brief Get the width in pixels of the display buffer

	\return Width of the display buffer in pixels
	\sa GetHeight() const or GetDepth() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetHeight(void) const
	\brief Get the height in pixels of the display buffer

	\return Height of the display buffer in pixels
	\sa GetWidth() const or GetDepth() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDepth(void) const
	\brief Get the depth in bits of the display buffer

	The display buffer could be 8 for 8 bit palette, 15 for 5:5:5 RGB, 16
	for 5:6:5 RGB or 24 or 32 for hardware rendering

	\return Depth of the display buffer in bits
	\sa GetWidth() const or GetHeight() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetFlags(void) const
	\brief Get the flags associated with this Display class instance

	\return Flags containing the current state of the display system
	\sa GetWidth() const or GetHeight() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDisplayWidth(void) const
	\brief Get the width in pixels of the display hardware

	This differs from GetWidth() in that this is the actual display hardware's
	resolution, which can differ from the resolution of the draw buffer.

	\return Width of the display hardware in pixels
	\sa GetDisplayHeight() const or GetWidth() const

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetDisplayHeight(void) const
	\brief Get the height in pixels of the display hardware

	This differs from GetHeight() in that this is the actual display hardware's
	resolution, which can differ from the resolution of the draw buffer.

	\return Height of the display hardware in pixels
	\sa GetDisplayWidth() const or GetHeight() const

***************************************/

/*! ************************************

	\fn float Burger::Display::GetWidthFloat(void) const
	\brief Get the width in pixels of the display buffer

	\return Width of the display buffer in pixels as a float
	\sa GetHeightFloat() const or GetWidth() const

***************************************/

/*! ************************************

	\fn float Burger::Display::GetHeightFloat(void) const
	\brief Get the height in pixels of the display buffer

	\return Height of the display buffer in pixels as a float
	\sa GetWidthFloat() const or GetHeight() const

***************************************/

/*! ************************************

	\fn float Burger::Display::GetAspectRatioX(void) const
	\brief Get the aspect ratio in the format of width/height

	\return Aspect ratio in the X direction
	\sa GetAspectRatioY() const

***************************************/

/*! ************************************

	\fn float Burger::Display::GetAspectRatioY(void) const
	\brief Get the aspect ratio in the format of height/width

	\return Aspect ratio in the Y direction
	\sa GetAspectRatioX() const

***************************************/

/*! ************************************

	\brief Get the enumeration of the screen aspect ratio

	Convert the aspect ratio values into the closest enumeration of
	a standard aspect ratio.

	\return Enumeration of the screen's current aspect ratio

***************************************/

Burger::Display::eAspectRatio BURGER_API Burger::Display::GetAspectRatio(
	void) const
{
	eAspectRatio uResult = kAspectRatioUnknown;
	float fAspectRatioX = m_fAspectRatioX;
	if (fAspectRatioX >= 1.77f) { // 16/9
		uResult = kAspectRatio16x9;
	} else if (fAspectRatioX >= 1.60f) { // 16/10
		uResult = kAspectRatio16x10;
	} else if (fAspectRatioX >= 1.33f) { // 4/3
		uResult = kAspectRatio4x3;
	} else if (fAspectRatioX == 1.0f) { // 1/1
		uResult = kAspectRatio1x1;
	}
	return uResult;
}

/*! ************************************

	\fn const uint8_t * Burger::Display::GetPalette(void) const
	\brief Get the current palette

	When video is set to an 8 bit mode, a 256 entry color palette is needed
	for the video display to properly show the graphics. A copy of
	the palette is maintained in the class that matches what
	the hardware is currently displaying. For some displays, some
	colors are considered read only. When calling SetPalette(uint_t,uint_t,
	const uint8_t *) it's not guaranteed that all colors will be updated due to
	system reserved colors (For windowed modes). The palette will have the
	reserved colors in it if this is the case.

	\return Pointer to a 768 byte array of Red,Green,Blue color components
	\sa SetPalette()

***************************************/

/*! ************************************

	\fn Burger::Display::GetBorderColor(void) const
	\brief Get the hardware border color

	On some platforms, such as MSDOS VGA/MCGA or the Commodore Amiga,
	the display's border color can be set. This function will return
	that color.

	\note On MSDOS, the EGA value of the border color is 4 bits (0-15).

	\return Value previously set by SetBorderColor(uint_t) or zero if
		uninitialized.

	\sa SetBorderColor(uint_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetFadeSpeed(void) const
	\brief Return the timer constant in Burger::Tick

	When calling the palette fade functions, it will perform the fade evenly
	until this amount of time has elapsed. This is not a "per frame" time. It's
	a total time. If the desired time is one second from start to finish, set
	this value to \ref Tick::kTicksPerSecond

	\return Return the current tick value for a palette fade
	\sa SetFadeSpeed(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Display::SetFadeSpeed(uint_t uPaletteFadeSpeed) const
	\brief Set the timer constant in Burger::Tick

	When calling the palette fade functions, it will perform the fade evenly
	until this amount of time has elapsed. This is not a "per frame" time. It's
	a total time. If the desired time is one second from start to finish, set
	this value to \ref Tick::kTicksPerSecond. Setting this value to zero will disable
	the fade feature and will have palettes update immediately

	\param uPaletteFadeSpeed New time delay in ticks.
	\sa GetFadeSpeed()

***************************************/

/*! ************************************

	\fn uint_t Burger::Display::GetPaletteVSync(void) const
	\brief Return non-zero if palette updates are synced to vertical blank.

	\return Return non-zero if palette updates are synced to vertical blank.

	\sa SetPaletteVSync(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Display::SetPaletteVSync(uint_t bPaletteVSync)
	\brief Set the flag to enable palette updates

	\param bPaletteVSync \ref TRUE to enable vertical blank syncing, \ref FALSE
		to disable it

	\sa GetPaletteVSync() const

***************************************/

/*! ************************************

	\fn void Burger::Display::SetResizeCallback(ResizeProc pResize,
		void *pResizeData)
	\brief Set window resize callback

	Set the function pointer for the callback when the window's size is changed

	\param pResize Pointer to the function or \ref NULL to disable
	\param pResizeData Pointer that is passed as is to the function if it's
		called.

	\sa GetResizeCallback() const or GetResizeCallbackData() const

***************************************/

/*! ************************************

	\fn ResizeProc Burger::Display::GetResizeCallback(void) const
	\brief Return the function pointer for the callback when the window's size
		is changed

	\return Pointer to the function or \ref NULL if disabled

	\sa SetResizeCallback() or GetResizeCallbackData() const

***************************************/

/*! ************************************

	\fn void * Burger::Display::GetResizeCallbackData(void) const
	\brief Return the window resize function

	Return the pointer that's passed to the callback that's used when the
	window's size is changed

	\return Pointer that is passed to the callback

	\sa SetResizeCallback() or GetResizeCallback() const

***************************************/

/*! ************************************

	\fn void Burger::Display::SetRenderCallback(RenderProc pRender,
		void *pRenderData)
	\brief Set window update function

	Set the function pointer for the callback when the window needs to be
	redrawn

	\param pRender Pointer to the function or \ref NULL to disable
	\param pRenderData Pointer that is passed as is to the function if it's
		called.

	\sa GetRenderCallback() const or GetRenderCallbackData() const

***************************************/

/*! ************************************

	\fn RenderProc Burger::Display::GetRenderCallback(void) const
	\brief Set the window updat function

	Return the function pointer for the callback when the window needs to
	be redrawn

	\return Pointer to the function or \ref NULL if disabled
	\sa SetRenderCallback() or GetRenderCallbackData() const

***************************************/

/*! ************************************

	\fn void * Burger::Display::GetRenderCallbackData(void) const
	\brief Get the redraw callback function

	Return the pointer that's passed to the callback that's used when the
	window is redrawn

	\return Pointer that is passed to the callback
	\sa SetRenderCallback() or GetRenderCallback() const

***************************************/

/*! ************************************

	\fn void Burger::Display::SetReleaseCallback(ReleaseProc pRelease,
		void *pReleaseData)
	\brief Set the purge callback

	Set the function pointer for the callback when the renderer needs to purge
	all resources

	\param pRelease Pointer to the function or \ref NULL to disable
	\param pReleaseData Pointer that is passed as is to the function if it's
		called.

	\sa GetReleaseCallback() const or GetReleaseCallbackData() const

***************************************/

/*! ************************************

	\fn ReleaseProc Burger::Display::GetReleaseCallback(void) const
	\brief Purge resource callback

	Return the function pointer for the callback when the renderer needs
	to purge all resources

	\return Pointer to the function or \ref nullptr if disabled
	\sa SetReleaseCallback() or GetReleaseCallbackData() const

***************************************/

/*! ************************************

	\fn void * Burger::Display::GetReleaseCallbackData(void) const
	\brief Return pointer to callback void *.

	Return the pointer that's passed to the callback that's used when the
	renderer

	\return Pointer that is passed to the callback
	\sa SetReleaseCallback() or GetReleaseCallback() const

***************************************/

/*! ************************************

	\brief Calculate the screen aspect ratio.

	Check for an exact aspect ratio match if ``bExactOnly`` is \ref TRUE.
	Otherwise, find the aspect ratio enumeration that's the closest match.
	
	If no match is found, return \ref kAspectRatioUnknown

	\return \ref eAspectRatio enumeration

***************************************/

#if !defined(DOXYGEN)
struct AspectRatioValue_t {
	uint32_t m_uWidth;
	uint32_t m_uHeight;
	uint32_t m_uRatio;
};

// 1x1 = 1.0
// 4x3 = 1.333
// 16x10 = 1.6
// 16x9 = 1.777
// 21x9 = 2.333
// 32x9 = 3.555

static const AspectRatioValue_t g_AspectRatioValues[] = {
	{1, 1, (1U << 16U) / 1U}, {4, 3, (4U << 16U) / 3U},
	{16 / 2, 10 / 2, (8U << 16U) / 5U}, {16, 9, (16U << 16U) / 9U},
	{21 / 3, 9 / 3, (7U << 16U) / 3U}, {32, 9, (32U << 16U) / 9U}};
#endif

Burger::Display::eAspectRatio Burger::Display::get_aspect_ratio(
	uint32_t uWidth, uint32_t uHeight, uint_t bExactOnly) BURGER_NOEXCEPT
{
	// Switch the width and height so the width is always wider
	// Side effect is the ratio is 1.0 or higher.
	if (uWidth < uHeight) {
		const uint32_t uTemp = uWidth;
		uWidth = uHeight;
		uHeight = uTemp;
	}

	// First check for an exact match using a simple test
	const AspectRatioValue_t* pWork = g_AspectRatioValues;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_AspectRatioValues);
	do {
		if ((uWidth * pWork->m_uHeight) == (uHeight * pWork->m_uWidth)) {

			// Match!
			return static_cast<eAspectRatio>(
				(BURGER_ARRAYSIZE(g_AspectRatioValues) - uCount) + 1);
		}
		++pWork;
	} while (--uCount);

	// If only exact matches are allowed, exit now
	if (!bExactOnly) {

		// Test for an inexact match using integer math

		// Get the ratio
		const uint32_t uRatio = (uWidth << 16U) / uHeight;

		pWork = g_AspectRatioValues;
		uCount = BURGER_ARRAYSIZE(g_AspectRatioValues) - 1;
		do {

			// Check if the ratio is between the range
			const uint32_t uUpper = pWork[1].m_uRatio;
			if (uRatio < uUpper) {
				const uint32_t uLower = pWork->m_uRatio;
				if (uRatio >= uLower) {

					// Closer to higher or lower?
					if ((uRatio - uLower) > (uUpper - uRatio)) {
						// "+1"
						--uCount;
					}
					return static_cast<eAspectRatio>(
						BURGER_ARRAYSIZE(g_AspectRatioValues) - uCount);
				}
			}
			++pWork;
		} while (--uCount);

		// Return the largest size
		return kAspectRatio32x9;
	}

	// Unknown since exact match was not found.
	return kAspectRatioUnknown;
}

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Display, Burger::Base);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::Display::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/
