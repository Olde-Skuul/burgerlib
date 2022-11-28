/***************************************

	Display base class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAY_H__
#define __BRDISPLAY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRRECT_H__
#include "brrect.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __BRARRAY_H__
#include "brarray.h"
#endif

#ifndef __BRTEXTURE_H__
#include "brtexture.h"
#endif

#ifndef __BRVERTEXBUFFER_H__
#include "brvertexbuffer.h"
#endif

#ifndef __BREFFECT_H__
#include "breffect.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_ANDROID) && !defined(__BRANDROIDTYPES_H__)
#include "brandroidtypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {

struct RGBAWord8_t;

class Display: public Base {
	BURGER_DISABLE_COPY(Display);
	BURGER_RTTI_IN_CLASS();

public:
	enum {

		/** The display is in a desktop window, best for debugging */
		INWINDOW = 0x0,
		/** Set if full screen is desired */
		FULLSCREEN = 0x1,
		/** Set if Alt-Enter is allowed to switch from full screen to windowed
		   mode */
		ALLOWFULLSCREENTOGGLE = 0x2,
		/** On desktop platforms, allow the window to be resized */
		ALLOWRESIZING = 0x4,
		/** If Resizing is allowed, maintain the aspect ratio */
		MAINTAIN_ASPECT_RATIO = 0x8,
		/** Enable stencil mode */
		STENCILENABLE = 0x20,
		/** Set if all 256 colors of the palette can be used */
		FULLPALETTEALLOWED = 0x40,
		/** Set if 3D Glasses support is enabled */
		STEREO = 0x80,
		/** Hint that rendering is performed on multiple threads */
		MULTITHREADED = 0x100,
		/** Enable gamma support */
		GAMMAENABLE = 0x200,
		/** Enable dithering */
		DITHERENABLE = 0x400,
		/** Enable interlacing */
		INTERLACEENABLE = 0x800,
		/** Landscape mode */
		LANDSCAPE = 0x0000,
		/** Portrait mode */
		PORTRAIT = 0x1000,
		/** Inverted */
		INVERTED = 0x2000,
		/** Inverted landscape mode */
		LANDSCAPEINVERTED = 0x2000,
		/** Inverted portrait mode */
		PORTRAITINVERTED = 0x3000,
		/** PAL interlace mode */
		PALMODE = 0x4000,
#if defined(_DEBUG) || defined(DOXYGEN)
		/** Default window flags (Debug is in a window, release is full screen)
		 */
		DEFAULTFLAGS = INWINDOW
#else
		/** Default window flags (NDEBUG is full screen) */
		DEFAULTFLAGS = FULLSCREEN
#endif
	};

	enum eAspectRatio {
		ASPECT_RATIO_UNKNOWN, ///< Undefined aspect ratio
		ASPECT_RATIO_1x1,     ///< Square aspect ratio
		ASPECT_RATIO_4x3,     ///< 4x3 aspect ratio (TV)
		ASPECT_RATIO_16x9,    ///< 16x9 aspect ratio (wide screen)
		ASPECT_RATIO_16x10    ///< 16x10 aspect ratio (laptops)
	};

	enum eClearBits {
		/** Used by Clear(uint_t) to clear the color buffer */
		CLEAR_COLOR = 0x01,
		/** Used by Clear(uint_t) to clear the depth buffer */
		CLEAR_DEPTH = 0x02,
		/** Used by Clear(uint_t) to clear the stencil buffer */
		CLEAR_STENCIL = 0x04
	};

	enum eDepthFunction {
		/** Never render the pixel */
		DEPTHCMP_NEVER,
		/** Render if Z is less than Depth Z */
		DEPTHCMP_LESS,
		/** Render if Z is equal to the Depth Z */
		DEPTHCMP_EQUAL,
		/** Render if Z is less than or equal to the Depth Z */
		DEPTHCMP_LESSEQUAL,
		/** Render if Z is greater than Depth Z */
		DEPTHCMP_GREATER,
		/** Render if Z is not equal to the Depth Z */
		DEPTHCMP_NOTEQUAL,
		/** Render if Z is greater than or equal to the Depth Z */
		DEPTHCMP_GREATEREQUAL,
		/** Always render the pixel (Same as turning off depth testing, allows
		   zwrites) */
		DEPTHCMP_ALWAYS
	};

	enum eCullMode {
		/** Don't cull any polygons */
		CULL_NONE,
		/** Don't render clockwise oriented polygons */
		CULL_CLOCKWISE,
		/** Don't render counter clockwise oriented polygons */
		CULL_COUNTERCLOCKWISE
	};

	enum eSourceBlendFactor {
		/** Source alpha is forced to zero */
		SRCBLEND_ZERO,
		/** Source alpha is forced to one */
		SRCBLEND_ONE,
		/** Source alpha is each color component */
		SRCBLEND_COLOR,
		/** Source alpha is 1-color component */
		SRCBLEND_ONE_MINUS_COLOR,
		/** Source alpha is the alpha component */
		SRCBLEND_SRC_ALPHA,
		/** Source alpha is 1-alpha component */
		SRCBLEND_ONE_MINUS_SRC_ALPHA,
		/** Source alpha is the destination buffer alpha component */
		SRCBLEND_DST_ALPHA,
		/** Source alpha is the destination buffer 1-alpha component */
		SRCBLEND_ONE_MINUS_DST_ALPHA,
		/** Saturate the alpha with 1 */
		SRCBLEND_SRC_ALPHA_SATURATE
	};

	enum eDestinationBlendFactor {
		/** Destination alpha is forced to zero */
		DSTBLEND_ZERO,
		/** Destination alpha is forced to one */
		DSTBLEND_ONE,
		/** Destination alpha is each color component */
		DSTBLEND_COLOR,
		/** Destination alpha is 1-color component */
		DSTBLEND_ONE_MINUS_COLOR,
		/** Destination alpha is the alpha component */
		DSTBLEND_DST_ALPHA,
		/** Destination alpha is 1-alpha component */
		DSTBLEND_ONE_MINUS_DST_ALPHA,
		/** Destination alpha is the source buffer alpha component */
		DSTBLEND_SRC_ALPHA,
		/** Destination alpha is the source buffer 1-alpha component */
		DSTBLEND_ONE_MINUS_SRC_ALPHA
	};

	enum ePrimitiveType {
		/** Draw array as points */
		PRIM_POINTS,
		/** Draw array as line end to end pairs */
		PRIM_LINES,
		/** Draw array as a long line */
		PRIM_LINESTRIP,
		/** Draw array as triangles */
		PRIM_TRIANGLES,
		/** Draw array as triangle strip */
		PRIM_TRIANGLESTRIP,
		/** Draw array as a triangle fan */
		PRIM_TRIANGLEFAN
	};

	struct VideoMode_t {
		enum {
			/** Set if hardware acceleration is available */
			VIDEOMODE_HARDWARE = 0x01,
			/** Set if refresh rate is valid */
			VIDEOMODE_REFRESHVALID = 0x02
		};
		uint_t m_uWidth;  ///< Width of video mode
		uint_t m_uHeight; ///< Height of video mode
		uint_t m_uDepth;  ///< Depth of video mode
		uint_t m_uHertz;  ///< Video scan rate (0 if not supported)
		uint_t m_uFlags;  ///< Flags for special features
	};

	struct OpenGLVertexInputs_t {
		uint_t m_uIndex;     ///< VertexBuffer::eUsage OpenGL BindAttribLocation
		const char* m_pName; ///< Variable name for input in the shader
	};

	struct OpenGLVertexBufferObjectDescription_t {
		/** Pointer to the vertex positions */
		const void* m_pPositions;
		/** Pointer to the vertex normals */
		const void* m_pNormals;
		/** Pointer to the texture UVs */
		const void* m_pTexcoords;
		/** Pointer to the polygon vertex indexes */
		const void* m_pElements;
		/** Size of the positions in bytes */
		uintptr_t m_uPositionSize;
		/** Size of the vertex normals in bytes */
		uintptr_t m_uNormalSize;
		/** Size of the texture UVs in bytes */
		uintptr_t m_uTexcoordSize;
		/** Size of the polygon vertex indexes in bytes */
		uintptr_t m_uElementSize;
		/** Open GL type of positions (GL_FLOAT) */
		uint_t m_ePositionType;
		/** Number of elements per position (3 or 4) */
		uint_t m_uPositionElementCount;
		/** Open GL type of vertex normal (GL_FLOAT) */
		uint_t m_eNormalType;
		/** Number of elements per vertex normal (3 or 4) */
		uint_t m_uNormalElementCount;
		/** Open GL type of texture UVs (GL_FLOAT) */
		uint_t m_eTexcoordType;
		/** Number of elements per uv (2) */
		uint_t m_uTexcoordElementCount;
	};

	class VideoCardDescription {
	public:
		enum {
			/**Set if hardware acceleration is available */
			VIDEOCARD_HARDWARE = 0x01,
			/** Set if this is the primary video display */
			VIDEOCARD_PRIMARY = 0x02
		};
		/** Array of display resolution modes */
		SimpleArray<VideoMode_t> m_Array;

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		/** (Windows only) Device GUID */
		GUID m_GUID;
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
		/** (MacOSX only) NSScreen associated with this card */
		NSScreen* m_pNSScreen;
#endif

		String m_DeviceName;        ///< Name of the device
		String m_MonitorName;       ///< Name of the monitor
		Rect_t m_SystemRect;        ///< Location on the desktop
		Rect_t m_CurrentResolution; ///< Current resolution of the device
		uint_t m_uDevNumber;        ///< Device number
		uint_t m_uFlags;            ///< Flags for special features
		VideoCardDescription();
		~VideoCardDescription();
	};

	/** Callback function prototype for palette fading functions */
	typedef uint_t(BURGER_API* FadeProc)(void* pThis, uint_t uStep);

	/** Callback function prototype for window resizing */
	typedef void(BURGER_API* ResizeProc)(
		void* pThis, uint_t uWidth, uint_t uHeight);

	/** Callback function for rendering the scene */
	typedef void(BURGER_API* RenderProc)(void* pThis);

	/** Callback function for releasing resources on shutdown */
	typedef void(BURGER_API* ReleaseProc)(void* pThis);

protected:
	struct Globals_t {
		/** Default screen width of the main monitor */
		uint_t m_uDefaultWidth;
		/** Default screen height of the main monitor */
		uint_t m_uDefaultHeight;
		/** Default screen depth of the main monitor */
		uint_t m_uDefaultDepth;
		/** Default screen refresh rate (0 means not applicable) */
		uint_t m_uDefaultHertz;
		/** Default screen width of all monitors */
		uint_t m_uDefaultTotalWidth;
		/** Default screen height of all monitors */
		uint_t m_uDefaultTotalHeight;
		/** Number of monitors attached */
		uint_t m_uDefaultMonitorCount;

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
		/** (Xbox 360 Only) \ref TRUE if wide screen display */
		uint_t m_bIsWidescreen;
		/** (Xbox 360 Only) \ref TRUE if high definition display */
		uint_t m_bIsHiDef;
		/** (Xbox 360 Only) \ref TRUE if interlaced display */
		uint_t m_bIsInterlaced;
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
		/** (iOS Only) Points to Pixels scale factor for retina displays */
		float m_fRetinaScale;
#endif

		uint_t m_bInitialized; ///< Are the globals set?
	};
	/** Global values initialized when the first Display class is created */
	static Globals_t g_Globals;

	GameApp* m_pGameApp;   ///< Pointer to the game application instance
	Renderer* m_pRenderer; ///< Pointer to a renderer

	// Platform specific data

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	uint32_t m_WindowPlacement[11]; ///< WINDOWPLACEMENT record
#endif

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	/** (Xbox 360 Only) Direct 3D device */
	D3DDevice* m_pD3DDevice;
	/** (Xbox 360 Only) 32 bit RGBA color for screen clear */
	uint_t m_uClearColor;
	/** (Xbox 360 Only) ZValue to write for screen clear */
	float m_fClearDepth;
	/** (Xbox 360 Only) \ref TRUE if wide screen display is active */
	uint_t m_bWideScreen;

#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	/** (MacOSX Only) Pointer to the main view */
	NSView* m_pView;
	/** (MacOSX Only) Pointer to the window controller */
	NSWindowController* m_pWindowController;
	/** (MacOSX Only) Pointer to the OpenGL view */
	NSOpenGLView* m_pOpenGLView;
	/** (MacOSX Only) Pointer to the OpenGL context */
	_CGLContextObject* m_pOpenGLContext;
	/** (MacOSX Only) Pointer to the main full screen window */
	NSWindow* m_pFullScreenWindow;
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	EAGLContext* m_pEAGLContext; ///< (iOS Only) OpenGL Context
	uint_t m_uFrontBuffer;       ///< (iOS Only) Main render buffer GL name
	uint_t m_uColorRenderBuffer; ///< (iOS Only) Color render buffer GL name
	uint_t m_uDepthRenderBuffer; ///< (iOS Only) Depth buffer for rendering
#endif

// Non windows OpenGL Platforms
#if defined(BURGER_OPENGL) && !defined(BURGER_WINDOWS)
public:
	/** Pointer to an array of supported OpenGL compressed textures */
	uint_t* m_pCompressedFormats;
	/** Numeric value for the version of OpenGL */
	float m_fOpenGLVersion;
	/** Numeric value for the version of the Shader compiler */
	float m_fShadingLanguageVersion;
	/** Number of supported compressed texture formats */
	uint_t m_uCompressedFormatCount;
	/** Maximum number of vertex attributes GL_MAX_VERTEX_ATTRIBS */
	uint_t m_uMaximumVertexAttributes;
	/** Maximum number of color frame attachments GL_MAX_COLOR_ATTACHMENTS */
	uint_t m_uMaximumColorAttachments;
	/** OpenGL glActiveTexture() shadow value */
	uint_t m_uActiveTexture;

protected:
#endif

	// Shared data

	/** Callback if the screen changed sizes */
	ResizeProc m_pResize;
	/** pThis pointer for m_pResize calls */
	void* m_pResizeData;

	/** Callback if the operating system requests a scene draw */
	RenderProc m_pRender;
	/** pThis pointer for m_pRender calls */
	void* m_pRenderData;

	/** Callback if the operating system requests a scene draw */
	ReleaseProc m_pRelease;
	/** pThis pointer for m_pRender calls */
	void* m_pReleaseData;

	/** Bound textures for the current context */
	Texture* m_pBoundTextures[8];

	/** Width in pixels of the display buffer */
	uint_t m_uWidth;
	/** Height in pixels of the display buffer */
	uint_t m_uHeight;
	/** Depth in bits of the display buffer */
	uint_t m_uDepth;
	/** Flags for describing available features */
	uint_t m_uFlags;
	/** Width of the display hardware (Can differ from m_uWidth for pixel
	 * stretching) */
	uint_t m_uDisplayWidth;
	/** Height of the display hardware (Can differ from m_uHeight for pixel
	 * stretching) */
	uint_t m_uDisplayHeight;
	/** Depth of the display hardware (Can differ from m_uDepth for pixel depth
	 * change) */
	uint_t m_uDisplayDepth;
	/** Width of the rendering target as a float */
	float m_fWidth;
	/** Height of the rendering target as a float */
	float m_fHeight;
	/** Width/Height */
	float m_fAspectRatioX;
	/** Height/Width */
	float m_fAspectRatioY;
	/** Hardware border color (MSDOS / Amiga only) */
	uint_t m_uBorderColor;
	/** Speed in 1/60ths of a second for a palette fade */
	uint_t m_uPaletteFadeSpeed;

	int m_iPauseRenderingCount; ///< Rendering pausing reference count
	uint8_t m_bRenderingPaused; ///< If \ref TRUE, rendering is paused
	uint8_t m_bPaletteDirty;    ///< \ref TRUE if the palette buffer was changed
	uint8_t m_bPaletteVSync;    ///< \ref TRUE if palette updates sync to video
	uint8_t m_bNotUsed[1];      ///< Padding
	uint8_t m_Palette[256 * 3]; ///< Palette of 256 RGB values

	static void BURGER_API InitGlobals(void);
	void BURGER_API InitDefaults(GameApp* pGameApp);
	void BURGER_API SetWidthHeight(uint_t uWidth, uint_t uHeight);

public:
	Display(GameApp* pGameApp);
	virtual ~Display();

	// Windows uses Display as a base class, instead of the singular class

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
#define BURGER_VIRTUAL(a, b, c) a b c;
#else
#define BURGER_VIRTUAL(a, b, c) virtual a b c = 0;
#endif
	BURGER_VIRTUAL(uint_t, Init,
		(uint_t uWidth, uint_t uHeight, uint_t uDepth = 32,
			uint_t uFlags = DEFAULTFLAGS))
	BURGER_VIRTUAL(void, Shutdown, (void))
	BURGER_VIRTUAL(void, BeginScene, (void))
	BURGER_VIRTUAL(void, EndScene, (void))
	BURGER_VIRTUAL(Texture*, CreateTextureObject, (void))
	BURGER_VIRTUAL(VertexBuffer*, CreateVertexBufferObject, (void))
	BURGER_VIRTUAL(void, Resize, (uint_t uWidth, uint_t uHeight))
	BURGER_VIRTUAL(void, SetViewport,
		(uint_t uX, uint_t uY, uint_t uWidth, uint_t uHeight))
	BURGER_VIRTUAL(void, SetScissorRect,
		(uint_t uX, uint_t uY, uint_t uWidth, uint_t uHeight))
	BURGER_VIRTUAL(void, SetClearColor,
		(float fRed, float fGreen, float fBlue, float fAlpha))
	BURGER_VIRTUAL(void, SetClearDepth, (float fDepth))
	BURGER_VIRTUAL(void, Clear, (uint_t uMask))
	BURGER_VIRTUAL(void, Bind, (Texture * pTexture, uint_t uIndex = 0))
	BURGER_VIRTUAL(void, Bind, (Effect * pEffect))
	BURGER_VIRTUAL(void, SetBlend, (uint_t bEnable))
	BURGER_VIRTUAL(void, SetBlendFunction,
		(eSourceBlendFactor uSourceFactor, eDestinationBlendFactor uDestFactor))
	BURGER_VIRTUAL(void, SetLighting, (uint_t bEnable))
	BURGER_VIRTUAL(void, SetZWrite, (uint_t bEnable))
	BURGER_VIRTUAL(void, SetDepthTest, (eDepthFunction uDepthFunction))
	BURGER_VIRTUAL(void, SetCullMode, (eCullMode uCullMode))
	BURGER_VIRTUAL(void, SetScissor, (uint_t bEnable))
	BURGER_VIRTUAL(void, DrawPrimitive,
		(ePrimitiveType uPrimitiveType, VertexBuffer* pVertexBuffer))
	BURGER_VIRTUAL(void, DrawElements,
		(ePrimitiveType uPrimitiveType, VertexBuffer* pVertexBuffer))

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE tagWINDOWPLACEMENT* GetWindowedPlacement(void) BURGER_NOEXCEPT
	{
		return static_cast<tagWINDOWPLACEMENT*>(
			static_cast<void*>(m_WindowPlacement));
	}
	uint_t BURGER_API HandleMinMax(HWND__* pWindow, uintptr_t lParam);
	virtual void CheckForWindowSizeChange(void);
	virtual void CheckForWindowChangingMonitors(void);
#endif

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DVertexShader* BURGER_API CreateVertexShader(
		const void* pVertexShaderBinary) const;
	D3DPixelShader* BURGER_API CreatePixelShader(
		const void* pPixelShaderBinary) const;
	BURGER_INLINE D3DDevice* GetD3DDevice(void) const BURGER_NOEXCEPT
	{
		return m_pD3DDevice;
	}
	void BURGER_API InitState(void);
	BURGER_INLINE uint_t IsWideScreen(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_bIsWidescreen;
	}
	BURGER_INLINE uint_t IsHiDef(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_bIsHiDef;
	}
	BURGER_INLINE uint_t IsInterlaced(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_bIsInterlaced;
	}
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	BURGER_INLINE NSView* GetView(void) const BURGER_NOEXCEPT
	{
		return m_pView;
	}
	BURGER_INLINE NSWindowController* GetWindowController(
		void) const BURGER_NOEXCEPT
	{
		return m_pWindowController;
	}
	BURGER_INLINE NSOpenGLView* GetOpenGLView(void) const BURGER_NOEXCEPT
	{
		return m_pOpenGLView;
	}
	BURGER_INLINE _CGLContextObject* GetOpenGLContext(
		void) const BURGER_NOEXCEPT
	{
		return m_pOpenGLContext;
	}
	BURGER_INLINE NSWindow* GetFullScreenWindow(void) const BURGER_NOEXCEPT
	{
		return m_pFullScreenWindow;
	}
	BURGER_INLINE void SetFullScreenWindow(
		NSWindow* pFullScreenWindow) BURGER_NOEXCEPT
	{
		m_pFullScreenWindow = pFullScreenWindow;
	}
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	BURGER_INLINE EAGLContext* GetGLContext(void) const BURGER_NOEXCEPT
	{
		return m_pEAGLContext;
	}
	BURGER_INLINE uint_t GetFrontBuffer(void) const BURGER_NOEXCEPT
	{
		return m_uFrontBuffer;
	}
	BURGER_INLINE uint_t GetColorBuffer(void) const BURGER_NOEXCEPT
	{
		return m_uColorRenderBuffer;
	}
	BURGER_INLINE uint_t GetDepthBuffer(void) const BURGER_NOEXCEPT
	{
		return m_uDepthRenderBuffer;
	}
	BURGER_INLINE static float GetRetinaScale(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_fRetinaScale;
	}
#else
	static BURGER_INLINE uint_t GetFrontBuffer(void) BURGER_NOEXCEPT
	{
		return 0;
	}
#endif

#if defined(BURGER_OPENGL) && !defined(BURGER_WINDOWS)
	BURGER_INLINE float GetOpenGLVersion(void) const BURGER_NOEXCEPT
	{
		return m_fOpenGLVersion;
	}
	BURGER_INLINE float GetShadingLanguageVersion(void) const BURGER_NOEXCEPT
	{
		return m_fShadingLanguageVersion;
	}
	BURGER_INLINE uint_t GetCompressedFormatCount(void) const BURGER_NOEXCEPT
	{
		return m_uCompressedFormatCount;
	}
	BURGER_INLINE const uint_t* GetCompressedFormats(void) const BURGER_NOEXCEPT
	{
		return m_pCompressedFormats;
	}
	BURGER_INLINE uint_t GetMaximumVertexAttributes(void) const BURGER_NOEXCEPT
	{
		return m_uMaximumVertexAttributes;
	}
	void BURGER_API SetupOpenGL(void);
	uint_t BURGER_API CompileShader(uint_t GLEnum, const char* pShaderCode,
		uintptr_t uShaderCodeLength = 0) const;
	uint_t BURGER_API CompileProgram(const char* pUnifiedShader,
		uintptr_t uLength, const OpenGLVertexInputs_t* pVertexInputs = NULL,
		const uint_t* pMembers = NULL) const;
	uint_t BURGER_API CompileProgram(const char* pVertexShader,
		uintptr_t uVSLength, const char* pPixelShader, uintptr_t uPSLength,
		const OpenGLVertexInputs_t* pVertexInputs = NULL,
		const uint_t* pMembers = NULL) const;
	uint_t BURGER_API CreateVertexArrayObject(
		const OpenGLVertexBufferObjectDescription_t* pDescription) const;
	void BURGER_API DeleteVertexArrayObject(uint_t uVertexArrayObject) const;
	uint_t BURGER_API BuildFrameBufferObject(uint_t uWidth, uint_t uHeight,
		uint_t uGLDepth, uint_t uGLClamp, uint_t uGLZDepth = 0) const;
	static void BURGER_API DeleteFrameBufferObjectAttachment(
		uint_t uAttachment);
	void BURGER_API DeleteFrameBufferObject(uint_t uFrameBufferObject) const;
	static uint_t BURGER_API CreateTextureID(
		const Image* pImage, uint_t bGenerateMipMap = FALSE);
	static const char* BURGER_API GetErrorString(uint_t uGLErrorEnum);
	static uintptr_t BURGER_API GetGLTypeSize(uint_t uGLTypeEnum);
	static uint_t BURGER_API PrintGLError(const char* pErrorLocation);
#endif

	void BURGER_API Pause(uint_t bPauseRendering);
	BURGER_INLINE Texture* GetBoundTexture(
		uint_t uIndex = 0) const BURGER_NOEXCEPT
	{
		return m_pBoundTextures[uIndex];
	}
	Texture* BURGER_API CreateTexture(
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTexture(uint_t uWidth, uint_t uHeight,
		Image::ePixelTypes uPixelType, Texture::eWrapping uWrapping,
		Texture::eFilter uFilter);
	Texture* BURGER_API CreateTexturePNG(const char* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTexturePNG(Filename* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTexturePNG(RezFile* pRezFile, uint_t uRezNum,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureGIF(const char* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureGIF(Filename* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureGIF(RezFile* pRezFile, uint_t uRezNum,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureTGA(const char* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureTGA(Filename* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureTGA(RezFile* pRezFile, uint_t uRezNum,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureBMP(const char* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureBMP(Filename* pFilename,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	Texture* BURGER_API CreateTextureBMP(RezFile* pRezFile, uint_t uRezNum,
		Texture::eWrapping uWrapping, Texture::eFilter uFilter);
	VertexBuffer* BURGER_API CreateVertexBuffer(
		const VertexBuffer::VertexAoS_t* pDescription);
	static uint_t BURGER_API GetVideoModes(
		ClassArray<VideoCardDescription>* pOutput);
	void BURGER_API SetPalette(
		uint_t uStart, uint_t uCount, const uint8_t* pPalette);
	void BURGER_API SetPalette(
		uint_t uStart, uint_t uCount, const RGBAWord8_t* pPalette);
	void BURGER_API SetBorderColor(uint_t uColor);
	void BURGER_API SetWindowTitle(const char* pTitle);
	void BURGER_API SetPaletteBlack(void);
	void BURGER_API SetPaletteWhite(void);
	void BURGER_API SetPalette(const uint8_t* pPalette);
	void BURGER_API SetPalette(const RGBAWord8_t* pPalette);
	void BURGER_API SetPalette(void** pHandle);
	void BURGER_API SetPalette(RezFile* pRez, uint_t uResID);
	void BURGER_API SetPalette(
		uint_t uStart, uint_t uCount, RezFile* pRez, uint_t uResID);
	void BURGER_API FadeTo(const uint8_t* pPalette, FadeProc pProc = nullptr,
		void* pData = nullptr);
	void BURGER_API FadeToBlack(
		FadeProc pProc = nullptr, void* pData = nullptr);
	void BURGER_API FadeToWhite(
		FadeProc pProc = nullptr, void* pData = nullptr);
	void BURGER_API FadeTo(RezFile* pRez, uint_t uResID,
		FadeProc pProc = nullptr, void* pData = nullptr);
	void BURGER_API FadeTo(
		void** pHandle, FadeProc pProc = nullptr, void* pData = nullptr);
	static BURGER_INLINE uint_t GetDefaultWidth(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultWidth;
	}
	static BURGER_INLINE uint_t GetDefaultHeight(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultHeight;
	}
	static BURGER_INLINE uint_t GetDefaultDepth(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultDepth;
	}
	static BURGER_INLINE uint_t GetDefaultHertz(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultHertz;
	}
	static BURGER_INLINE uint_t GetDefaultTotalWidth(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultTotalWidth;
	}
	static BURGER_INLINE uint_t GetDefaultTotalHeight(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultTotalHeight;
	}
	static BURGER_INLINE uint_t GetDefaultMonitorCount(void) BURGER_NOEXCEPT
	{
		return g_Globals.m_uDefaultMonitorCount;
	}
	BURGER_INLINE GameApp* GetGameApp(void) const BURGER_NOEXCEPT
	{
		return m_pGameApp;
	}
	BURGER_INLINE uint_t GetWidth(void) const BURGER_NOEXCEPT
	{
		return m_uWidth;
	}
	BURGER_INLINE uint_t GetHeight(void) const BURGER_NOEXCEPT
	{
		return m_uHeight;
	}
	BURGER_INLINE uint_t GetDepth(void) const BURGER_NOEXCEPT
	{
		return m_uDepth;
	}
	BURGER_INLINE uint_t GetFlags(void) const BURGER_NOEXCEPT
	{
		return m_uFlags;
	}
	BURGER_INLINE uint_t GetDisplayWidth(void) const BURGER_NOEXCEPT
	{
		return m_uDisplayWidth;
	}
	BURGER_INLINE uint_t GetDisplayHeight(void) const BURGER_NOEXCEPT
	{
		return m_uDisplayHeight;
	}
	BURGER_INLINE float GetWidthFloat(void) const BURGER_NOEXCEPT
	{
		return m_fWidth;
	}
	BURGER_INLINE float GetHeightFloat(void) const BURGER_NOEXCEPT
	{
		return m_fHeight;
	}
	BURGER_INLINE float GetAspectRatioX(void) const BURGER_NOEXCEPT
	{
		return m_fAspectRatioX;
	}
	BURGER_INLINE float GetAspectRatioY(void) const BURGER_NOEXCEPT
	{
		return m_fAspectRatioY;
	}
	eAspectRatio BURGER_API GetAspectRatio(void) const;
	BURGER_INLINE const uint8_t* GetPalette(void) const BURGER_NOEXCEPT
	{
		return m_Palette;
	}
	BURGER_INLINE uint_t GetBorderColor(void) const BURGER_NOEXCEPT
	{
		return m_uBorderColor;
	}
	BURGER_INLINE uint_t GetFadeSpeed(void) const BURGER_NOEXCEPT
	{
		return m_uPaletteFadeSpeed;
	}
	BURGER_INLINE void SetFadeSpeed(uint_t uPaletteFadeSpeed) BURGER_NOEXCEPT
	{
		m_uPaletteFadeSpeed = uPaletteFadeSpeed;
	}
	BURGER_INLINE uint_t GetPaletteVSync(void) const BURGER_NOEXCEPT
	{
		return m_bPaletteVSync;
	}
	BURGER_INLINE void SetPaletteVSync(uint_t bPaletteVSync) BURGER_NOEXCEPT
	{
		m_bPaletteVSync = (bPaletteVSync != 0);
	}
	BURGER_INLINE void SetResizeCallback(
		ResizeProc pResize, void* pResizeData) BURGER_NOEXCEPT
	{
		m_pResize = pResize;
		m_pResizeData = pResizeData;
	}
	BURGER_INLINE ResizeProc GetResizeCallback(void) const BURGER_NOEXCEPT
	{
		return m_pResize;
	}
	BURGER_INLINE void* GetResizeCallbackData(void) const BURGER_NOEXCEPT
	{
		return m_pResizeData;
	}
	BURGER_INLINE void SetRenderCallback(
		RenderProc pRender, void* pRenderData) BURGER_NOEXCEPT
	{
		m_pRender = pRender;
		m_pRenderData = pRenderData;
	}
	BURGER_INLINE RenderProc GetRenderCallback(void) const BURGER_NOEXCEPT
	{
		return m_pRender;
	}
	BURGER_INLINE void* GetRenderCallbackData(void) const BURGER_NOEXCEPT
	{
		return m_pRenderData;
	}
	BURGER_INLINE void SetReleaseCallback(
		ReleaseProc pRelease, void* pReleaseData) BURGER_NOEXCEPT
	{
		m_pRelease = pRelease;
		m_pReleaseData = pReleaseData;
	}
	BURGER_INLINE ReleaseProc GetReleaseCallback(void) const BURGER_NOEXCEPT
	{
		return m_pRelease;
	}
	BURGER_INLINE void* GetReleaseCallbackData(void) const BURGER_NOEXCEPT
	{
		return m_pReleaseData;
	}
};
#undef BURGER_VIRTUAL
}
/* END */

#endif
