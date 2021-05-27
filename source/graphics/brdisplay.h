/***************************************

    Display base class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if defined(BURGER_SHIELD) && !defined(__BRSHIELDTYPES_H__)
#include "brshieldtypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
struct RGBAWord8_t;
class Display : public Base {
    BURGER_DISABLE_COPY(Display);
	BURGER_RTTI_IN_CLASS();
public:
	enum {

		INWINDOW = 0x0,				///< The display is in a desktop window, best for debugging
		FULLSCREEN = 0x1,				///< Set if full screen
		ALLOWFULLSCREENTOGGLE = 0x2,	///< Set if Alt-Enter is allowed to switch from full screen to windowed mode
		ALLOWRESIZING = 0x4,			///< On desktop platforms, allow the window to be resized
		MAINTAIN_ASPECT_RATIO = 0x8,	///< If Resizing is allowed, maintain the aspect ratio
		STENCILENABLE = 0x20,			///< Enable stencil mode
		FULLPALETTEALLOWED = 0x40,	///< Set if all 256 colors of the palette can be used
		STEREO = 0x80,				///< Set if 3D Glasses support is enabled
		MULTITHREADED = 0x100,		///< Hint that rendering is performed on multiple threads
		GAMMAENABLE = 0x200,			///< Enable gamma support
		DITHERENABLE = 0x400,			///< Enable dithering
		INTERLACEENABLE = 0x800,		///< Enable interlacing
		LANDSCAPE = 0x0000,			///< Landscape mode
		PORTRAIT = 0x1000,			///< Portrait mode
		INVERTED = 0x2000,			///< Inverted
		LANDSCAPEINVERTED = 0x2000,	///< Inverted landscape mode
		PORTRAITINVERTED = 0x3000,	///< Inverted portrait mode
		PALMODE = 0x4000,				///< PAL interlace mode
#if defined(_DEBUG) || defined(DOXYGEN)
		DEFAULTFLAGS = INWINDOW		///< Default window flags (Debug is in a window, release is full screen)
#else
		DEFAULTFLAGS = FULLSCREEN	///< Default window flags (NDEBUG is full screen)
#endif
	};

	enum eAspectRatio {
		ASPECT_RATIO_UNKNOWN,		///< Undefined aspect ratio
		ASPECT_RATIO_1x1,			///< Square aspect ratio
		ASPECT_RATIO_4x3,			///< 4x3 aspect ratio (TV)
		ASPECT_RATIO_16x9,			///< 16x9 aspect ratio (wide screen)
		ASPECT_RATIO_16x10			///< 16x10 aspect ratio (laptops)
	};

	enum eClearBits {
		CLEAR_COLOR = 0x01,			///< Used by Clear(uint_t) to clear the color buffer
		CLEAR_DEPTH = 0x02,			///< Used by Clear(uint_t) to clear the depth buffer
		CLEAR_STENCIL = 0x04			///< Used by Clear(uint_t) to clear the stencil buffer
	};

	enum eDepthFunction {
		DEPTHCMP_NEVER,			///< Never render the pixel
		DEPTHCMP_LESS,			///< Render if Z is less than Depth Z
		DEPTHCMP_EQUAL,			///< Render if Z is equal to the Depth Z
		DEPTHCMP_LESSEQUAL,		///< Render if Z is less than or equal to the Depth Z
		DEPTHCMP_GREATER,		///< Render if Z is greater than Depth Z
		DEPTHCMP_NOTEQUAL,		///< Render if Z is not equal to the Depth Z
		DEPTHCMP_GREATEREQUAL,	///< Render if Z is greater than or equal to the Depth Z
		DEPTHCMP_ALWAYS			///< Always render the pixel (Same as turning off depth testing, allows zwrites)
	};

	enum eCullMode {
		CULL_NONE,				///< Don't cull any polygons
		CULL_CLOCKWISE,			///< Don't render clockwise oriented polygons
		CULL_COUNTERCLOCKWISE	///< Don't render counter clockwise oriented polygons
	};

	enum eSourceBlendFactor {
		SRCBLEND_ZERO,					///< Source alpha is forced to zero
		SRCBLEND_ONE,					///< Source alpha is forced to one
		SRCBLEND_COLOR,					///< Source alpha is each color component
		SRCBLEND_ONE_MINUS_COLOR,		///< Source alpha is 1-color component
		SRCBLEND_SRC_ALPHA,				///< Source alpha is the alpha component
		SRCBLEND_ONE_MINUS_SRC_ALPHA,	///< Source alpha is 1-alpha component
		SRCBLEND_DST_ALPHA,				///< Source alpha is the destination buffer alpha component
		SRCBLEND_ONE_MINUS_DST_ALPHA,	///< Source alpha is the destination buffer 1-alpha component
		SRCBLEND_SRC_ALPHA_SATURATE		///< Saturate the alpha with 1
	};

	enum eDestinationBlendFactor {
		DSTBLEND_ZERO,					///< Destination alpha is forced to zero
		DSTBLEND_ONE,					///< Destination alpha is forced to one
		DSTBLEND_COLOR,					///< Destination alpha is each color component
		DSTBLEND_ONE_MINUS_COLOR,		///< Destination alpha is 1-color component
		DSTBLEND_DST_ALPHA,				///< Destination alpha is the alpha component
		DSTBLEND_ONE_MINUS_DST_ALPHA,	///< Destination alpha is 1-alpha component
		DSTBLEND_SRC_ALPHA,				///< Destination alpha is the source buffer alpha component
		DSTBLEND_ONE_MINUS_SRC_ALPHA	///< Destination alpha is the source buffer 1-alpha component
	};

	enum ePrimitiveType {
		PRIM_POINTS,			///< Draw array as points
		PRIM_LINES,				///< Draw array as line end to end pairs
		PRIM_LINESTRIP,			///< Draw array as a long line
		PRIM_TRIANGLES,			///< Draw array as triangles
		PRIM_TRIANGLESTRIP,		///< Draw array as triangle strip
		PRIM_TRIANGLEFAN		///< Draw array as a triangle fan
	};

	struct VideoMode_t {
		enum {
			VIDEOMODE_HARDWARE = 0x01,		///< Set if hardware acceleration is available
			VIDEOMODE_REFRESHVALID = 0x02	///< Set if refresh rate is valid
		};
		uint_t m_uWidth;		///< Width of video mode
		uint_t m_uHeight;		///< Height of video mode
		uint_t m_uDepth;		///< Depth of video mode
		uint_t m_uHertz;		///< Video scan rate (0 if not supported)
		uint_t m_uFlags;		///< Flags for special features
	};

	struct OpenGLVertexInputs_t {
		uint_t m_uIndex;				///< VertexBuffer::eUsage OpenGL BindAttribLocation
		const char *m_pName;		///< Variable name for input in the shader
	};

	struct OpenGLVertexBufferObjectDescription_t {
		const void *m_pPositions;		///< Pointer to the vertex positions
		const void *m_pNormals;			///< Pointer to the vertex normals
		const void *m_pTexcoords;		///< Pointer to the texture UVs
		const void *m_pElements;		///< Pointer to the polygon vertex indexes
		uintptr_t m_uPositionSize;		///< Size of the positions in bytes
		uintptr_t m_uNormalSize;			///< Size of the vertex normals in bytes
		uintptr_t m_uTexcoordSize;		///< Size of the texture UVs in bytes
		uintptr_t m_uElementSize;			///< Size of the polygon vertex indexes in bytes
		uint_t m_ePositionType;			///< Open GL type of positions (GL_FLOAT)
		uint_t m_uPositionElementCount;	///< Number of elements per position (3 or 4)
		uint_t m_eNormalType;				///< Open GL type of vertex normal (GL_FLOAT)
		uint_t m_uNormalElementCount;		///< Number of elements per vertex normal (3 or 4)
		uint_t m_eTexcoordType;			///< Open GL type of texture UVs (GL_FLOAT)
		uint_t m_uTexcoordElementCount;	///< Number of elements per uv (2)
	};

	class VideoCardDescription {
	public:
		enum {
			VIDEOCARD_HARDWARE = 0x01,		///< Set if hardware acceleration is available
			VIDEOCARD_PRIMARY = 0x02		///< Set if this is the primary video display
		};
		SimpleArray<VideoMode_t> m_Array;		///< Array of display resolution modes

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		GUID m_GUID;							///< (Windows only) Device GUID
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
		NSScreen *m_pNSScreen;					///< (MacOSX only) NSScreen associated with this card
#endif

		String m_DeviceName;					///< Name of the device
		String m_MonitorName;					///< Name of the monitor
		Rect_t m_SystemRect;					///< Location on the desktop
		Rect_t m_CurrentResolution;				///< Current resolution of the device
		uint_t m_uDevNumber;						///< Device number
		uint_t m_uFlags;							///< Flags for special features
		VideoCardDescription();
		~VideoCardDescription();
	};

	typedef uint_t(BURGER_API *FadeProc)(void *pThis, uint_t uStep);		///< Callback function prototype for palette fading functions
	typedef void (BURGER_API *ResizeProc)(void *pThis, uint_t uWidth, uint_t uHeight);		///< Callback function prototype for window resizing
	typedef void (BURGER_API *RenderProc)(void *pThis);					///< Callback function for rendering the scene
	typedef void (BURGER_API *ReleaseProc)(void *pThis);				///< Callback function for releasing resources on shutdown

protected:
	struct Globals_t {
		uint_t m_uDefaultWidth;			///< Default screen width of the main monitor
		uint_t m_uDefaultHeight;			///< Default screen height of the main monitor
		uint_t m_uDefaultDepth;			///< Default screen depth of the main monitor
		uint_t m_uDefaultHertz;			///< Default screen refresh rate (0 means not applicable)
		uint_t m_uDefaultTotalWidth;		///< Default screen width of all monitors
		uint_t m_uDefaultTotalHeight;		///< Default screen height of all monitors
		uint_t m_uDefaultMonitorCount;	///< Number of monitors attached

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
		uint_t m_bIsWidescreen;			///< (Xbox 360 Only) \ref TRUE if wide screen display
		uint_t m_bIsHiDef;				///< (Xbox 360 Only) \ref TRUE if high definition display
		uint_t m_bIsInterlaced;			///< (Xbox 360 Only) \ref TRUE if interlaced display
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
		float m_fRetinaScale;			///< (iOS Only) Points to Pixels scale factor for retina displays
#endif

		uint_t m_bInitialized;			///< Are the globals set?
	};
	static Globals_t g_Globals;	///< Global values initialized when the first Display class is created


	GameApp *m_pGameApp;		///< Pointer to the game application instance
	Renderer *m_pRenderer;		///< Pointer to a renderer

// Platform specific data

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	uint32_t m_WindowPlacement[11];		///< WINDOWPLACEMENT record
#endif

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DDevice *m_pD3DDevice;	///< (Xbox 360 Only) Direct 3D device
	uint_t m_uClearColor;			///< (Xbox 360 Only) 32 bit RGBA color for screen clear
	float m_fClearDepth;		///< (Xbox 360 Only) ZValue to write for screen clear
	uint_t m_bWideScreen;			///< (Xbox 360 Only) \ref TRUE if wide screen display is active
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	NSView *m_pView;							///< (MacOSX Only) Pointer to the main view
	NSWindowController *m_pWindowController;	///< (MacOSX Only) Pointer to the window controller
	NSOpenGLView *m_pOpenGLView;				///< (MacOSX Only) Pointer to the OpenGL view
	_CGLContextObject *m_pOpenGLContext;		///< (MacOSX Only) Pointer to the OpenGL context
	NSWindow *m_pFullScreenWindow;				///< (MacOSX Only) Pointer to the main full screen window
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	EAGLContext *m_pEAGLContext;			///< (iOS Only) OpenGL Context
	uint_t m_uFrontBuffer;					///< (iOS Only) Main render buffer GL name
	uint_t m_uColorRenderBuffer;				///< (iOS Only) Color render buffer GL name
	uint_t m_uDepthRenderBuffer;				///< (iOS Only) Depth buffer for rendering
#endif

// Non windows OpenGL Platforms
#if defined(BURGER_OPENGL) && !defined(BURGER_WINDOWS)
public:
	uint_t *m_pCompressedFormats;				///< Pointer to an array of supported OpenGL compressed textures
	float m_fOpenGLVersion;					///< Numeric value for the version of OpenGL
	float m_fShadingLanguageVersion;		///< Numeric value for the version of the Shader compiler
	uint_t m_uCompressedFormatCount;			///< Number of supported compressed texture formats
	uint_t m_uMaximumVertexAttributes;		///< Maximum number of vertex attributes GL_MAX_VERTEX_ATTRIBS
	uint_t m_uMaximumColorAttachments;		///< Maximum number of color frame attachments GL_MAX_COLOR_ATTACHMENTS
	uint_t m_uActiveTexture;					///< OpenGL glActiveTexture() shadow value
protected:
#endif

	// Shared data

	ResizeProc m_pResize;		///< Callback if the screen changed sizes
	void *m_pResizeData;		///< pThis pointer for m_pResize calls

	RenderProc m_pRender;		///< Callback if the operating system requests a scene draw
	void *m_pRenderData;		///< pThis pointer for m_pRender calls

	ReleaseProc m_pRelease;		///< Callback if the operating system requests a scene draw
	void *m_pReleaseData;		///< pThis pointer for m_pRender calls

	Texture *m_pBoundTextures[8];	///< Bound textures for the current context

	uint_t m_uWidth;				///< Width in pixels of the display buffer
	uint_t m_uHeight;				///< Height in pixels of the display buffer
	uint_t m_uDepth;				///< Depth in bits of the display buffer
	uint_t m_uFlags;				///< Flags for describing available features
	uint_t m_uDisplayWidth;		///< Width of the display hardware (Can differ from m_uWidth for pixel stretching)
	uint_t m_uDisplayHeight;		///< Height of the display hardware (Can differ from m_uHeight for pixel stretching)
	uint_t m_uDisplayDepth;		///< Depth of the display hardware (Can differ from m_uDepth for pixel depth change)
	float m_fWidth;				///< Width of the rendering target as a float
	float m_fHeight;			///< Height of the rendering target as a float
	float m_fAspectRatioX;		///< Width/Height
	float m_fAspectRatioY;		///< Height/Width
	uint_t m_uBorderColor;		///< Hardware border color (MSDOS / Amiga only)
	uint_t m_uPaletteFadeSpeed;	///< Speed in 1/60ths of a second for a palette fade

	int m_iPauseRenderingCount;	///< Rendering pausing reference count
	uint8_t m_bRenderingPaused;	///< If \ref TRUE, rendering is paused
	uint8_t m_bPaletteDirty;		///< \ref TRUE if the palette buffer was changed
	uint8_t m_bPaletteVSync;		///< \ref TRUE if palette updates sync to video
	uint8_t m_bNotUsed[1];		///< Padding
	uint8_t m_Palette[256*3];		///< Palette of 256 RGB values
	
	static void BURGER_API InitGlobals(void);
	void BURGER_API InitDefaults(GameApp *pGameApp);
	void BURGER_API SetWidthHeight(uint_t uWidth,uint_t uHeight);
public:
	Display(GameApp *pGameApp);
	virtual ~Display();

// Windows uses Display as a base class, instead of the singular class

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
#define BURGER_VIRTUAL(a,b,c) a b c;
#else
#define BURGER_VIRTUAL(a,b,c) virtual a b c = 0;
#endif
	BURGER_VIRTUAL(uint_t,Init,(uint_t uWidth,uint_t uHeight,uint_t uDepth=32,uint_t uFlags=DEFAULTFLAGS))
	BURGER_VIRTUAL(void,Shutdown,(void))
	BURGER_VIRTUAL(void,BeginScene,(void))
	BURGER_VIRTUAL(void,EndScene,(void))
	BURGER_VIRTUAL(Texture *,CreateTextureObject,(void))
	BURGER_VIRTUAL(VertexBuffer *,CreateVertexBufferObject,(void))
	BURGER_VIRTUAL(void,Resize,(uint_t uWidth,uint_t uHeight))
	BURGER_VIRTUAL(void,SetViewport,(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight))
	BURGER_VIRTUAL(void,SetScissorRect,(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight))
	BURGER_VIRTUAL(void,SetClearColor,(float fRed,float fGreen,float fBlue,float fAlpha))
	BURGER_VIRTUAL(void,SetClearDepth,(float fDepth))
	BURGER_VIRTUAL(void,Clear,(uint_t uMask))
	BURGER_VIRTUAL(void,Bind,(Texture *pTexture,uint_t uIndex = 0))
	BURGER_VIRTUAL(void,Bind,(Effect *pEffect))
	BURGER_VIRTUAL(void,SetBlend,(uint_t bEnable))
	BURGER_VIRTUAL(void,SetBlendFunction,(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor))
	BURGER_VIRTUAL(void,SetLighting,(uint_t bEnable))
	BURGER_VIRTUAL(void,SetZWrite,(uint_t bEnable))
	BURGER_VIRTUAL(void,SetDepthTest,(eDepthFunction uDepthFunction))
	BURGER_VIRTUAL(void,SetCullMode,(eCullMode uCullMode))
	BURGER_VIRTUAL(void,SetScissor,(uint_t bEnable))
	BURGER_VIRTUAL(void,DrawPrimitive,(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer))
	BURGER_VIRTUAL(void,DrawElements,(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer))

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE tagWINDOWPLACEMENT *GetWindowedPlacement(void) { return static_cast<tagWINDOWPLACEMENT *>(static_cast<void *>(m_WindowPlacement)); }
	uint_t BURGER_API HandleMinMax(HWND__ *pWindow,uintptr_t lParam);
	virtual void CheckForWindowSizeChange(void);
	virtual void CheckForWindowChangingMonitors(void);
#endif

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DVertexShader * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	D3DPixelShader * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;
	BURGER_INLINE D3DDevice *GetD3DDevice(void) const { return m_pD3DDevice; }
	void BURGER_API InitState(void);
	BURGER_INLINE uint_t IsWideScreen(void) { return g_Globals.m_bIsWidescreen; }
	BURGER_INLINE uint_t IsHiDef(void) { return g_Globals.m_bIsHiDef; }
	BURGER_INLINE uint_t IsInterlaced(void) { return g_Globals.m_bIsInterlaced; }
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	BURGER_INLINE NSView *GetView(void) const { return m_pView; }
	BURGER_INLINE NSWindowController *GetWindowController(void) const { return m_pWindowController; }
	BURGER_INLINE NSOpenGLView *GetOpenGLView(void) const { return m_pOpenGLView; }
	BURGER_INLINE _CGLContextObject *GetOpenGLContext(void) const { return m_pOpenGLContext; }
	BURGER_INLINE NSWindow *GetFullScreenWindow(void) const { return m_pFullScreenWindow; }
	BURGER_INLINE void SetFullScreenWindow(NSWindow *pFullScreenWindow) { m_pFullScreenWindow = pFullScreenWindow; }
#endif

#if defined(BURGER_IOS) || defined(DOXYGEN)
	BURGER_INLINE EAGLContext *GetGLContext(void) const { return m_pEAGLContext; }
	BURGER_INLINE uint_t GetFrontBuffer(void) const { return m_uFrontBuffer; }
	BURGER_INLINE uint_t GetColorBuffer(void) const { return m_uColorRenderBuffer; }
	BURGER_INLINE uint_t GetDepthBuffer(void) const { return m_uDepthRenderBuffer; }
	BURGER_INLINE static float GetRetinaScale(void) { return g_Globals.m_fRetinaScale; }
#else
    static BURGER_INLINE uint_t GetFrontBuffer(void) { return 0; }
#endif
	
#if defined(BURGER_OPENGL) && !defined(BURGER_WINDOWS)
	BURGER_INLINE float GetOpenGLVersion(void) const { return m_fOpenGLVersion; }
	BURGER_INLINE float GetShadingLanguageVersion(void) const { return m_fShadingLanguageVersion; }
	BURGER_INLINE uint_t GetCompressedFormatCount(void) const { return m_uCompressedFormatCount; }
	BURGER_INLINE const uint_t *GetCompressedFormats(void) const { return m_pCompressedFormats; }
	BURGER_INLINE uint_t GetMaximumVertexAttributes(void) const { return m_uMaximumVertexAttributes; }
	void BURGER_API SetupOpenGL(void);
	uint_t BURGER_API CompileShader(uint_t GLEnum,const char *pShaderCode,uintptr_t uShaderCodeLength=0) const;
	uint_t BURGER_API CompileProgram(const char *pUnifiedShader,uintptr_t uLength,const OpenGLVertexInputs_t *pVertexInputs=NULL,const uint_t *pMembers=NULL) const;
	uint_t BURGER_API CompileProgram(const char *pVertexShader,uintptr_t uVSLength,const char *pPixelShader,uintptr_t uPSLength,const OpenGLVertexInputs_t *pVertexInputs=NULL,const uint_t *pMembers=NULL) const;
	uint_t BURGER_API CreateVertexArrayObject(const OpenGLVertexBufferObjectDescription_t *pDescription) const;
	void BURGER_API DeleteVertexArrayObject(uint_t uVertexArrayObject) const;
	uint_t BURGER_API BuildFrameBufferObject(uint_t uWidth,uint_t uHeight,uint_t uGLDepth,uint_t uGLClamp,uint_t uGLZDepth=0) const;
	static void BURGER_API DeleteFrameBufferObjectAttachment(uint_t uAttachment);
	void BURGER_API DeleteFrameBufferObject(uint_t uFrameBufferObject) const;
	static uint_t BURGER_API CreateTextureID(const Image *pImage,uint_t bGenerateMipMap=FALSE);
	static const char * BURGER_API GetErrorString(uint_t uGLErrorEnum);
	static uintptr_t BURGER_API GetGLTypeSize(uint_t uGLTypeEnum);
	static uint_t BURGER_API PrintGLError(const char *pErrorLocation);
#endif

	void BURGER_API Pause(uint_t bPauseRendering);
	BURGER_INLINE Texture *GetBoundTexture(uint_t uIndex = 0) const { return m_pBoundTextures[uIndex]; }
	Texture * BURGER_API CreateTexture(Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexture(uint_t uWidth,uint_t uHeight,Image::ePixelTypes uPixelType,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(RezFile *pRezFile,uint_t uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(RezFile *pRezFile,uint_t uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(RezFile *pRezFile,uint_t uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(RezFile *pRezFile,uint_t uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	VertexBuffer * BURGER_API CreateVertexBuffer(const VertexBuffer::VertexAoS_t *pDescription);
	static uint_t BURGER_API GetVideoModes(ClassArray<VideoCardDescription> *pOutput);
	void BURGER_API SetPalette(uint_t uStart,uint_t uCount,const uint8_t *pPalette);
	void BURGER_API SetPalette(uint_t uStart,uint_t uCount,const RGBAWord8_t *pPalette);
	void BURGER_API SetBorderColor(uint_t uColor);
	void BURGER_API SetWindowTitle(const char *pTitle);
	void BURGER_API SetPaletteBlack(void);
	void BURGER_API SetPaletteWhite(void);
	void BURGER_API SetPalette(const uint8_t *pPalette);
	void BURGER_API SetPalette(const RGBAWord8_t *pPalette);
	void BURGER_API SetPalette(void **pHandle);
	void BURGER_API SetPalette(RezFile *pRez,uint_t uResID);
	void BURGER_API SetPalette(uint_t uStart,uint_t uCount,RezFile *pRez,uint_t uResID);
	void BURGER_API FadeTo(const uint8_t *pPalette,FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeToBlack(FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeToWhite(FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeTo(RezFile *pRez,uint_t uResID,FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeTo(void **pHandle,FadeProc pProc=NULL,void *pData=NULL);
    static BURGER_INLINE uint_t GetDefaultWidth(void) { return g_Globals.m_uDefaultWidth; }
    static BURGER_INLINE uint_t GetDefaultHeight(void) { return g_Globals.m_uDefaultHeight; }
    static BURGER_INLINE uint_t GetDefaultDepth(void) { return g_Globals.m_uDefaultDepth; }
    static BURGER_INLINE uint_t GetDefaultHertz(void) { return g_Globals.m_uDefaultHertz; }
    static BURGER_INLINE uint_t GetDefaultTotalWidth(void) { return g_Globals.m_uDefaultTotalWidth; }
    static BURGER_INLINE uint_t GetDefaultTotalHeight(void) { return g_Globals.m_uDefaultTotalHeight; }
    static BURGER_INLINE uint_t GetDefaultMonitorCount(void) { return g_Globals.m_uDefaultMonitorCount; }
	BURGER_INLINE GameApp *GetGameApp(void) const { return m_pGameApp; }
	BURGER_INLINE uint_t GetWidth(void) const { return m_uWidth; }
	BURGER_INLINE uint_t GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE uint_t GetDepth(void) const { return m_uDepth; }
	BURGER_INLINE uint_t GetFlags(void) const { return m_uFlags; }
	BURGER_INLINE uint_t GetDisplayWidth(void) const { return m_uDisplayWidth; }
	BURGER_INLINE uint_t GetDisplayHeight(void) const { return m_uDisplayHeight; }
	BURGER_INLINE float GetWidthFloat(void) const { return m_fWidth; }
	BURGER_INLINE float GetHeightFloat(void) const { return m_fHeight; }
	BURGER_INLINE float GetAspectRatioX(void) const { return m_fAspectRatioX; }
	BURGER_INLINE float GetAspectRatioY(void) const { return m_fAspectRatioY; }
	eAspectRatio BURGER_API GetAspectRatio(void) const;
	BURGER_INLINE const uint8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE uint_t GetBorderColor(void) const { return m_uBorderColor; }
	BURGER_INLINE uint_t GetFadeSpeed(void) const { return m_uPaletteFadeSpeed; }
	BURGER_INLINE void SetFadeSpeed(uint_t uPaletteFadeSpeed) { m_uPaletteFadeSpeed = uPaletteFadeSpeed; }
	BURGER_INLINE uint_t GetPaletteVSync(void) const { return m_bPaletteVSync; }
	BURGER_INLINE void SetPaletteVSync(uint_t bPaletteVSync) { m_bPaletteVSync=(bPaletteVSync!=0); }
	BURGER_INLINE void SetResizeCallback(ResizeProc pResize,void *pResizeData) { m_pResize = pResize; m_pResizeData=pResizeData; }
	BURGER_INLINE ResizeProc GetResizeCallback(void) const { return m_pResize; }
	BURGER_INLINE void *GetResizeCallbackData(void) const { return m_pResizeData; }
	BURGER_INLINE void SetRenderCallback(RenderProc pRender,void *pRenderData) { m_pRender = pRender; m_pRenderData=pRenderData; }
	BURGER_INLINE RenderProc GetRenderCallback(void) const { return m_pRender; }
	BURGER_INLINE void *GetRenderCallbackData(void) const { return m_pRenderData; }
	BURGER_INLINE void SetReleaseCallback(ReleaseProc pRelease,void *pReleaseData) { m_pRelease = pRelease; m_pReleaseData=pReleaseData; }
	BURGER_INLINE ReleaseProc GetReleaseCallback(void) const { return m_pRelease; }
	BURGER_INLINE void *GetReleaseCallbackData(void) const { return m_pReleaseData; }
};
#undef BURGER_VIRTUAL
}
/* END */

#endif
