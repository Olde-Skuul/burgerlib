/***************************************

	Display base class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	BURGER_DISABLECOPYCONSTRUCTORS(Display);
	BURGER_RTTI_IN_CLASS();
public:
	enum {
		INWINDOW=0x0,				///< The display is in a desktop window, best for debugging
		FULLSCREEN=0x1,				///< Set if full screen
		ALLOWFULLSCREENTOGGLE=0x2,	///< Set if Alt-Enter is allowed to switch from full screen to windowed mode
		ALLOWRESIZING=0x4,			///< On desktop platforms, allow the window to be resized
		STENCILENABLE=0x20,			///< Enable stencil mode
		FULLPALETTEALLOWED=0x40,	///< Set if all 256 colors of the palette can be used
		STEREO=0x80,				///< Set if 3D Glasses support is enabled
		MULTITHREADED=0x100,		///< Hint that rendering is performed on multiple threads
		GAMMAENABLE=0x200,			///< Enable gamma support
		DITHERENABLE=0x400,			///< Enable dithering
		INTERLACEENABLE=0x800,		///< Enable interlacing
		LANDSCAPE=0x0000,			///< Landscape mode
		PORTRAIT=0x1000,			///< Portrait mode
		INVERTED=0x2000,			///< Inverted
		LANDSCAPEINVERTED=0x2000,	///< Inverted landscape mode
		PORTRAITINVERTED=0x3000,	///< Inverted portrait mode
		PALMODE=0x4000,				///< PAL interlace mode
#if defined(_DEBUG) || defined(DOXYGEN)
		DEFAULTFLAGS=INWINDOW		///< Default window flags (Debug is in a window, release is full screen)
#else
		DEFAULTFLAGS=FULLSCREEN	///< Default window flags (NDEBUG is full screen)
#endif
	};

	enum eClearBits {
		CLEAR_COLOR=0x01,			///< Used by Clear(Word) to clear the color buffer
		CLEAR_DEPTH=0x02,			///< Used by Clear(Word) to clear the depth buffer
		CLEAR_STENCIL=0x04			///< Used by Clear(Word) to clear the stencil buffer
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
			VIDEOMODE_HARDWARE=0x01,		///< Set if hardware acceleration is available
			VIDEOMODE_REFRESHVALID=0x02		///< Set if refresh rate is valid
		};
		Word m_uWidth;		///< Width of video mode
		Word m_uHeight;		///< Height of video mode
		Word m_uDepth;		///< Depth of video mode
		Word m_uHertz;		///< Video scan rate (0 if not supported)
		Word m_uFlags;		///< Flags for special features
	};

	class VideoCardDescription {
	public:
		enum {
			VIDEOCARD_HARDWARE=0x01,		///< Set if hardware acceleration is available
			VIDEOCARD_PRIMARY=0x02			///< Set if this is the primary video display
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
		Word m_uDevNumber;						///< Device number
		Word m_uFlags;							///< Flags for special features
		VideoCardDescription();
		~VideoCardDescription();
	};
	
	typedef Word (BURGER_API *FadeProc)(void *pThis,Word uStep);		///< Callback function prototype for palette fading functions
	typedef void (BURGER_API *ResizeProc)(void *pThis,Word uWidth,Word uHeight);		///< Callback function prototype for window resizing
	typedef void (BURGER_API *RenderProc)(void *pThis);					///< Callback function for rendering the scene
	typedef void (BURGER_API *ReleaseProc)(void *pThis);				///< Callback function for releasing resources on shutdown

protected:
	struct Globals_t {
		Word m_uDefaultWidth;			///< Default screen width of the main monitor
		Word m_uDefaultHeight;			///< Default screen height of the main monitor
		Word m_uDefaultDepth;			///< Default screen depth of the main monitor
		Word m_uDefaultHertz;			///< Default screen refresh rate (0 means not applicable)
		Word m_uDefaultTotalWidth;		///< Default screen width of all monitors
		Word m_uDefaultTotalHeight;		///< Default screen height of all monitors
		Word m_uDefaultMonitorCount;	///< Number of monitors attached
#if defined(BURGER_XBOX360)
		Word m_bIsWidescreen;			///< (Xbox 360 Only) \ref TRUE if wide screen display
		Word m_bIsHiDef;				///< (Xbox 360 Only) \ref TRUE if high definition display
		Word m_bIsInterlaced;			///< (Xbox 360 Only) \ref TRUE if interlaced display
#endif
#if defined(BURGER_IOS)
		float m_fRetinaScale;			///< (iOS Only) Points to Pixels scale factor for retina displays
#endif
		Word m_bInitialized;			///< Are the globals set?
	};
	static Globals_t g_Globals;	///< Global values initialized when the first Display class is created
	GameApp *m_pGameApp;		///< Pointer to the game application instance
	Renderer *m_pRenderer;		///< Pointer to a renderer

// Platform specific data

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DDevice *m_pD3DDevice;	///< (Xbox 360 Only) Direct 3D device
	Word m_uClearColor;			///< (Xbox 360 Only) 32 bit RGBA color for screen clear
	float m_fClearDepth;		///< (Xbox 360 Only) ZValue to write for screen clear
	Word m_bWideScreen;			///< (Xbox 360 Only) \ref TRUE if wide screen display is active
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
	Word m_uFrontBuffer;					///< (iOS Only) Main render buffer GL name
	Word m_uColorRenderBuffer;				///< (iOS Only) Color render buffer GL name
	Word m_uDepthRenderBuffer;				///< (iOS Only) Depth buffer for rendering
#endif

// Non windows OpenGL Platforms
#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
public:
	static const Word VERTEX_END = BURGER_MAXUINT;
	struct VertexInputs_t {
		Word m_uIndex;				///< OpenGL BindAttribLocation
		const char *m_pName;		///< Variable name for input
	};
	struct VertexBufferObjectDescription_t {
		const void *m_pPositions;	///< Pointer to the vertex positions
		const void *m_pNormals;		///< Pointer to the vertex normals
		const void *m_pTexcoords;	///< Pointer to the texture UVs
		const void *m_pElements;	///< Pointer to the polygon vertex indexes
		WordPtr m_uPositionSize;	///< Size of the positions in bytes
		WordPtr m_uNormalSize;		///< Size of the vertex normals in bytes
		WordPtr m_uTexcoordSize;	///< Size of the texture UVs in bytes
		WordPtr m_uElementSize;		///< Size of the polygon vertex indexes in bytes
		Word m_ePositionType;		///< Open GL type of positions (GL_FLOAT)
		Word m_uPositionElementCount;	///< Number of elements per position (3 or 4)
		Word m_eNormalType;			///< Open GL type of vertex normal (GL_FLOAT)
		Word m_uNormalElementCount;	///< Number of elements per vertex normal (3 or 4)
		Word m_eTexcoordType;		///< Open GL type of texture UVs (GL_FLOAT)
		Word m_uTexcoordElementCount;	///< Number of elements per uv (2)
	};
	Word *m_pCompressedFormats;				///< Pointer to an array of supported OpenGL compressed textures
	float m_fOpenGLVersion;					///< Numeric value for the version of OpenGL
	float m_fShadingLanguageVersion;		///< Numeric value for the version of the Shader compiler
	float m_fAspectRatio;					///< Width/Height
	float m_fWidth;							///< Width of the rendering target as a float
	float m_fHeight;						///< Height of the rendering target as a float
	Word m_uCompressedFormatCount;			///< Number of supported compressed texture formats
	Word m_uMaximumVertexAttributes;		///< Maximum number of vertex attributes GL_MAX_VERTEX_ATTRIBS
	Word m_uMaximumColorAttachments;		///< Maximum number of color frame attachments GL_MAX_COLOR_ATTACHMENTS
	Word m_uActiveTexture;					///< OpenGL glActiveTexture() shadow value
protected:
#endif

	// Shared data

	ResizeProc m_pResize;		///< Callback if the screen changed sizes
	void *m_pResizeData;		///< pThis pointer for m_pResize calls
	RenderProc m_pRender;		///< Callback if the operating system requests a scene draw
	void *m_pRenderData;		///< pThis pointer for m_pRender calls
	ReleaseProc m_pRelease;		///< Callback if the operating system requests a scene draw
	void *m_pReleaseData;		///< pThis pointer for m_pRender calls
	Texture *m_pBoundTextures[8];	///< Bound textures
	Word m_uWidth;				///< Width in pixels of the display buffer
	Word m_uHeight;				///< Height in pixels of the display buffer
	Word m_uDepth;				///< Depth in bits of the display buffer
	Word m_uFlags;				///< Flags for describing available features
	Word m_uDisplayWidth;		///< Width of the display hardware (Can differ from m_uWidth for pixel stretching)
	Word m_uDisplayHeight;		///< Height of the display hardware (Can differ from m_uHeight for pixel stretching)
	Word m_uDisplayDepth;		///< Depth of the display hardware (Can differ from m_uDepth for pixel depth change)
	Word m_uBorderColor;		///< Hardware border color (MSDOS / Amiga only)
	Word m_bPaletteDirty;		///< \ref TRUE if the palette buffer was changed
	Word m_bPaletteVSync;		///< \ref TRUE if palette updates sync to video
	Word m_uPaletteFadeSpeed;	///< Speed in 1/60ths of a second for a palette fade
	Word8 m_Palette[256*3];		///< Palette of 256 RGB values
	
	static void BURGER_API InitGlobals(void);
	void BURGER_API InitDefaults(GameApp *pGameApp);
public:
	Display(GameApp *pGameApp);
	virtual ~Display();

// Windows uses Display as a base class, instead of the singular class

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
#define BURGER_VIRTUAL(a,b,c) a b c;
#else
#define BURGER_VIRTUAL(a,b,c) virtual a b c = 0;
#endif
	BURGER_VIRTUAL(Word,Init,(Word uWidth,Word uHeight,Word uDepth=32,Word uFlags=DEFAULTFLAGS))
	BURGER_VIRTUAL(void,Shutdown,(void))
	BURGER_VIRTUAL(void,BeginScene,(void))
	BURGER_VIRTUAL(void,EndScene,(void))
	BURGER_VIRTUAL(Texture *,CreateTextureObject,(void))
	BURGER_VIRTUAL(VertexBuffer *,CreateVertexBufferObject,(void))
	BURGER_VIRTUAL(void,Resize,(Word uWidth,Word uHeight))
	BURGER_VIRTUAL(void,SetViewport,(Word uX,Word uY,Word uWidth,Word uHeight))
	BURGER_VIRTUAL(void,SetScissorRect,(Word uX,Word uY,Word uWidth,Word uHeight))
	BURGER_VIRTUAL(void,SetClearColor,(float fRed,float fGreen,float fBlue,float fAlpha))
	BURGER_VIRTUAL(void,SetClearDepth,(float fDepth))
	BURGER_VIRTUAL(void,Clear,(Word uMask))
	BURGER_VIRTUAL(void,Bind,(Texture *pTexture,Word uIndex=0))
	BURGER_VIRTUAL(void,SetBlend,(Word bEnable))
	BURGER_VIRTUAL(void,SetBlendFunction,(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor))
	BURGER_VIRTUAL(void,SetLighting,(Word bEnable))
	BURGER_VIRTUAL(void,SetZWrite,(Word bEnable))
	BURGER_VIRTUAL(void,SetDepthTest,(eDepthFunction uDepthFunction))
	BURGER_VIRTUAL(void,SetCullMode,(eCullMode uCullMode))
	BURGER_VIRTUAL(void,SetScissor,(Word bEnable))
	BURGER_VIRTUAL(void,DrawPrimitive,(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer))
	BURGER_VIRTUAL(void,DrawElements,(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer))

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DVertexShader * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	D3DPixelShader * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;
	BURGER_INLINE D3DDevice *GetD3DDevice(void) const { return m_pD3DDevice; }
	void BURGER_API InitState(void);
	BURGER_INLINE Word IsWideScreen(void) { return g_Globals.m_bIsWidescreen; }
	BURGER_INLINE Word IsHiDef(void) { return g_Globals.m_bIsHiDef; }
	BURGER_INLINE Word IsInterlaced(void) { return g_Globals.m_bIsInterlaced; }
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
	BURGER_INLINE Word GetFrontBuffer(void) const { return m_uFrontBuffer; }
	BURGER_INLINE Word GetColorBuffer(void) const { return m_uColorRenderBuffer; }
	BURGER_INLINE Word GetDepthBuffer(void) const { return m_uDepthRenderBuffer; }
	BURGER_INLINE static float GetRetinaScale(void) { return g_Globals.m_fRetinaScale; }
#else
	BURGER_INLINE static Word GetFrontBuffer(void) { return 0; }
#endif

#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
	BURGER_INLINE float GetOpenGLVersion(void) const { return m_fOpenGLVersion; }
	BURGER_INLINE float GetShadingLanguageVersion(void) const { return m_fShadingLanguageVersion; }
	BURGER_INLINE Word GetCompressedFormatCount(void) const { return m_uCompressedFormatCount; }
	BURGER_INLINE const Word *GetCompressedFormats(void) const { return m_pCompressedFormats; }
	BURGER_INLINE Word GetMaximumVertexAttributes(void) const { return m_uMaximumVertexAttributes; }
	void BURGER_API SetupOpenGL(void);
	Word BURGER_API CompileShader(Word GLEnum,const char *pShaderCode,WordPtr uShaderCodeLength=0) const;
	Word BURGER_API CompileProgram(const char *pUnifiedShader,WordPtr uLength,const VertexInputs_t *pVertexInputs=NULL,const Word *pMembers=NULL) const;
	Word BURGER_API CompileProgram(const char *pVertexShader,WordPtr uVSLength,const char *pPixelShader,WordPtr uPSLength,const VertexInputs_t *pVertexInputs=NULL,const Word *pMembers=NULL) const;
	Word BURGER_API CreateVertexArrayObject(const VertexBufferObjectDescription_t *pDescription) const;
	void BURGER_API DeleteVertexArrayObject(Word uVertexArrayObject) const;
	Word BURGER_API BuildFrameBufferObject(Word uWidth,Word uHeight,Word uGLDepth,Word uGLClamp,Word uGLZDepth=0) const;
	static void BURGER_API DeleteFrameBufferObjectAttachment(Word uAttachment);
	void BURGER_API DeleteFrameBufferObject(Word uFrameBufferObject) const;
	static Word BURGER_API CreateTextureID(const Image *pImage,Word bGenerateMipMap=FALSE);
	static const char * BURGER_API GetErrorString(Word uGLErrorEnum);
	static WordPtr BURGER_API GetGLTypeSize(Word uGLTypeEnum);
	static Word BURGER_API PrintGLError(const char *pErrorLocation);
#endif

	BURGER_INLINE Texture *GetBoundTexture(Word uIndex=0) const { return m_pBoundTextures[uIndex]; }
	Texture * BURGER_API CreateTexture(Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexture(Word uWidth,Word uHeight,Image::ePixelTypes uPixelType,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTexturePNG(RezFile *pRezFile,Word uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureGIF(RezFile *pRezFile,Word uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureTGA(RezFile *pRezFile,Word uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(const char *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(Filename *pFilename,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	Texture * BURGER_API CreateTextureBMP(RezFile *pRezFile,Word uRezNum,Texture::eWrapping uWrapping,Texture::eFilter uFilter);
	VertexBuffer * BURGER_API CreateVertexBuffer(const VertexBuffer::VertexAoS_t *pDescription);
	static Word BURGER_API GetVideoModes(ClassArray<VideoCardDescription> *pOutput);
	void BURGER_API SetPalette(Word uStart,Word uCount,const Word8 *pPalette);
	void BURGER_API SetPalette(Word uStart,Word uCount,const RGBAWord8_t *pPalette);
	void BURGER_API SetBorderColor(Word uColor);
	void BURGER_API SetWindowTitle(const char *pTitle);
	void BURGER_API SetPaletteBlack(void);
	void BURGER_API SetPaletteWhite(void);
	void BURGER_API SetPalette(const Word8 *pPalette);
	void BURGER_API SetPalette(const RGBAWord8_t *pPalette);
	void BURGER_API SetPalette(void **pHandle);
	void BURGER_API SetPalette(RezFile *pRez,Word uResID);
	void BURGER_API SetPalette(Word uStart,Word uCount,RezFile *pRez,Word uResID);
	void BURGER_API FadeTo(const Word8 *pPalette,FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeToBlack(FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeToWhite(FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeTo(RezFile *pRez,Word uResID,FadeProc pProc=NULL,void *pData=NULL);
	void BURGER_API FadeTo(void **pHandle,FadeProc pProc=NULL,void *pData=NULL);
	BURGER_INLINE static Word GetDefaultWidth(void) { return g_Globals.m_uDefaultWidth; }
	BURGER_INLINE static Word GetDefaultHeight(void) { return g_Globals.m_uDefaultHeight; }
	BURGER_INLINE static Word GetDefaultDepth(void) { return g_Globals.m_uDefaultDepth; }
	BURGER_INLINE static Word GetDefaultHertz(void) { return g_Globals.m_uDefaultHertz; }
	BURGER_INLINE static Word GetDefaultTotalWidth(void) { return g_Globals.m_uDefaultTotalWidth; }
	BURGER_INLINE static Word GetDefaultTotalHeight(void) { return g_Globals.m_uDefaultTotalHeight; }
	BURGER_INLINE static Word GetDefaultMonitorCount(void) { return g_Globals.m_uDefaultMonitorCount; }
	BURGER_INLINE GameApp *GetGameApp(void) const { return m_pGameApp; }
	BURGER_INLINE Word GetWidth(void) const { return m_uWidth; }
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE Word GetDepth(void) const { return m_uDepth; }
	BURGER_INLINE Word GetFlags(void) const { return m_uFlags; }
	BURGER_INLINE Word GetDisplayWidth(void) const { return m_uDisplayWidth; }
	BURGER_INLINE Word GetDisplayHeight(void) const { return m_uDisplayHeight; }
	BURGER_INLINE const Word8 *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE Word GetBorderColor(void) const { return m_uBorderColor; }
	BURGER_INLINE Word GetFadeSpeed(void) const { return m_uPaletteFadeSpeed; }
	BURGER_INLINE void SetFadeSpeed(Word uPaletteFadeSpeed) { m_uPaletteFadeSpeed = uPaletteFadeSpeed; }
	BURGER_INLINE Word GetPaletteVSync(void) const { return m_bPaletteVSync; }
	BURGER_INLINE void SetPaletteVSync(Word bPaletteVSync) { m_bPaletteVSync=bPaletteVSync; }
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
