/***************************************

    OpenGL manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayopengl.h"

#if defined(BURGER_OPENGL)
#include "brdebug.h"
#include "brimage.h"
#include "brtextureopengl.h"
#include "brvertexbufferopengl.h"
#include "brnumberto.h"
#include "brmemoryfunctions.h"

// Don't include burger.h
#define __BURGER__
#include "brgl.h"
#include "brglext.h"

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

// Detect if OpenGL or OpenGL ES version 2.0 is available

#if defined(GL_VERSION_2_0)	|| defined(GL_ES_VERSION_2_0)
#define USE_GL2
#endif

//
// OpenGL is a derived class for Windows
// to allow multiple API support. All other
// OpenGL based platforms, this is a base class
//

#if !defined(BURGER_WINDOWS)
#define DisplayOpenGL Display
#define TextureOpenGL Texture
#define VertexBufferOpenGL VertexBuffer
#else
#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayOpenGL,Burger::Display);
#endif
#endif


#if !defined(DOXYGEN)
#define CASE(x) { #x,x }
struct MessageLookup_t {
	const char *m_pName;
	uint_t m_uEnum;
};

// Used by the shader compiler to force a version match
#if defined(BURGER_OPENGL)
static const char g_Version[] = "#version ";

// Defines inserted into the shader for a vertex or fragment shader
static const char g_VertexShader[] =
	"#define VERTEX_SHADER\n"
	"#define PIPED varying\n"
	"#define VERTEX_INPUT attribute\n";
static const char g_VertexShader140[] =
	"#define VERTEX_SHADER\n"
	"#define PIPED out\n"
	"#define VERTEX_INPUT in\n";
static const char g_FragmentShader[] =
	"#define FRAGMENT_SHADER\n"
	"#define PIPED varying\n"
	"#define FRAGCOLOR_USED\n";
static const char g_FragmentShader140[] =
	"#define FRAGMENT_SHADER\n"
	"#define PIPED in\n"
	"#define FRAGCOLOR_USED out vec4 fragColor;\n"
	"#define gl_FragColor fragColor\n"
	"#define texture1D texture\n"
	"#define texture2D texture\n"
	"#define texture3D texture\n"
	"#define textureCube texture\n"
	"#define textureShadow2D texture\n";

static const GLenum s_SourceFunction[] = {
	GL_ZERO,				// SRCBLEND_ZERO
	GL_ONE,					// SRCBLEND_ONE
	GL_SRC_COLOR,			// SRCBLEND_COLOR
	GL_ONE_MINUS_SRC_COLOR,	// SRCBLEND_ONE_MINUS_COLOR
	GL_SRC_ALPHA,			// SRCBLEND_SRC_ALPHA
	GL_ONE_MINUS_SRC_ALPHA,	// SRCBLEND_ONE_MINUS_SRC_ALPHA
	GL_DST_ALPHA,			// SRCBLEND_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,	// SRCBLEND_ONE_MINUS_DST_ALPHA
	GL_SRC_ALPHA_SATURATE	// SRCBLEND_SRC_ALPHA_SATURATE
};
static const GLenum s_DestFunction[] = {
	GL_ZERO,				// DSTBLEND_ZERO
	GL_ONE,					// DSTBLEND_ONE
	GL_DST_COLOR,			// DSTBLEND_COLOR
	GL_ONE_MINUS_DST_COLOR,	// DSTBLEND_ONE_MINUS_COLOR
	GL_DST_ALPHA,			// DSTBLEND_DST_ALPHA
	GL_ONE_MINUS_DST_ALPHA,	// DSTBLEND_ONE_MINUS_DST_ALPHA
	GL_SRC_ALPHA,			// DSTBLEND_SRC_ALPHA
	GL_ONE_MINUS_SRC_ALPHA	// DSTBLEND_ONE_MINUS_SRC_ALPHA
};

static const GLenum s_DepthTest[] = {
	GL_NEVER,		// DEPTHCMP_NEVER
	GL_LESS,		// DEPTHCMP_LESS
	GL_EQUAL,		// DEPTHCMP_EQUAL
	GL_LEQUAL,		// DEPTHCMP_LESSEQUAL
	GL_GREATER,		// DEPTHCMP_GREATER
	GL_NOTEQUAL,	// DEPTHCMP_NOTEQUAL
	GL_GEQUAL,		// DEPTHCMP_GREATEREQUAL
	GL_ALWAYS		// DEPTHCMP_ALWAYS
};

static const GLenum s_Prims[] = {
	GL_POINTS,			// PRIM_POINTS
	GL_LINES,			// PRIM_LINES,
	GL_LINE_STRIP,		// PRIM_LINESTRIP,
	GL_TRIANGLES,		// PRIM_TRIANGLES,
	GL_TRIANGLE_STRIP,	// PRIM_TRIANGLESTRIP,
	GL_TRIANGLE_FAN		// PRIM_TRIANGLEFAN
};

#endif

#endif

/*! ************************************

	\class Burger::DisplayOpenGL
	\brief OpenGL screen setup

	Base class for instantiating a video display using OpenGL

	\sa Burger::RendererBase, Burger::DisplayBase, Burger::Display, Burger::DisplayDirectX9, Burger::DisplayDirectX11

***************************************/

#if !defined(BURGER_OPENGL) || defined(BURGER_LINUX)

/*! ************************************

	\brief Initialize OpenGL

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::~DisplayOpenGL()

***************************************/

Burger::DisplayOpenGL::DisplayOpenGL(Burger::GameApp *pGameApp) :
	m_pCompressedFormats(NULL),
	m_fOpenGLVersion(0.0f),
	m_fShadingLanguageVersion(0.0f),
	m_uCompressedFormatCount(0),
	m_uMaximumVertexAttributes(0),
	m_uMaximumColorAttachments(0),
	m_uActiveTexture(0)
{
	InitDefaults(pGameApp);
}

/*! ************************************

	\brief Start up the OpenGL context

	Base class for instantiating a video display using OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

uint_t Burger::DisplayOpenGL::Init(uint_t,uint_t,uint_t,uint_t)
{
	return 10;
}

/*! ************************************

	\brief Start up the OpenGL context

	Shut down OpenGL

	\sa Burger::DisplayOpenGL::PostShutdown()

***************************************/

void Burger::DisplayOpenGL::Shutdown(void)
{
}

void Burger::DisplayOpenGL::BeginScene(void)
{
}

/*! ************************************

	\brief Update the video display

	Calls SwapBuffers() in OpenGL to draw the rendered scene

	\sa Burger::DisplayOpenGL::PreBeginScene()

***************************************/

void Burger::DisplayOpenGL::EndScene(void)
{
}
#endif

Burger::Texture *Burger::DisplayOpenGL::CreateTextureObject(void)
{
	return new (Alloc(sizeof(TextureOpenGL))) TextureOpenGL;
}

Burger::VertexBuffer *Burger::DisplayOpenGL::CreateVertexBufferObject(void)
{
	return new (Alloc(sizeof(VertexBufferOpenGL))) VertexBufferOpenGL;
}

void Burger::DisplayOpenGL::Resize(uint_t uWidth,uint_t uHeight)
{
	SetWidthHeight(uWidth,uHeight);
	SetViewport(0,0,uWidth,uHeight);
}

void Burger::DisplayOpenGL::SetViewport(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight)
{
	glViewport(static_cast<GLint>(uX),static_cast<GLint>(uY),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight));
}

void Burger::DisplayOpenGL::SetScissorRect(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight)
{
	// Note: Flip the Y for OpenGL
	// Also, the starting Y is the BOTTOM of the rect, not the top
	glScissor(static_cast<GLint>(uX),static_cast<GLint>(m_uHeight-(uY+uHeight)),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight));
}

void Burger::DisplayOpenGL::SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha)
{
	glClearColor(fRed,fGreen,fBlue,fAlpha);
}

void Burger::DisplayOpenGL::SetClearDepth(float fDepth)
{
	glClearDepth(fDepth);
}

void Burger::DisplayOpenGL::Clear(uint_t uMask)
{
	GLbitfield uGLMask = 0;
	if (uMask&CLEAR_COLOR) {
		uGLMask = GL_COLOR_BUFFER_BIT;
	}
	if (uMask&CLEAR_DEPTH) {
		uGLMask |= GL_DEPTH_BUFFER_BIT;
	}
	if (uMask&CLEAR_STENCIL) {
		uGLMask |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(uGLMask);
}

void Burger::DisplayOpenGL::Bind(Texture *pTexture,uint_t uIndex)
{
	BURGER_ASSERT(uIndex<BURGER_ARRAYSIZE(m_pBoundTextures));
	m_pBoundTextures[uIndex] = pTexture;

	// Switch to the proper texture unit
	uIndex += GL_TEXTURE0;
	if (uIndex!=m_uActiveTexture) {
		m_uActiveTexture = uIndex;
		glActiveTexture(uIndex);
	}
	// Get the texture ID
	if (!pTexture) {
		glBindTexture(GL_TEXTURE_2D,0);
	} else {
		pTexture->CheckLoad(this);
	}
}

void Burger::DisplayOpenGL::Bind(Effect *pEffect)
{
#if defined(USE_GL2)
	if (!pEffect) {
		glUseProgram(0);
	} else {
		pEffect->CheckLoad(this);
		glUseProgram(pEffect->GetProgramID());
	}
#else
	BURGER_UNUSED(pEffect);
#endif
}

void Burger::DisplayOpenGL::SetBlend(uint_t bEnable)
{
	if (bEnable) {
		glEnable(GL_BLEND);
	} else {
		glDisable(GL_BLEND);
	}
}

void Burger::DisplayOpenGL::SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor)
{
	BURGER_ASSERT(uSourceFactor<BURGER_ARRAYSIZE(s_SourceFunction));
	BURGER_ASSERT(uDestFactor<BURGER_ARRAYSIZE(s_DestFunction));
	glBlendFunc(s_SourceFunction[uSourceFactor],s_DestFunction[uDestFactor]);
}


void Burger::DisplayOpenGL::SetLighting(uint_t bEnable)
{
#if !(defined(BURGER_OPENGLES))
	if (bEnable) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
#else
	BURGER_UNUSED(bEnable);
#endif
}

void Burger::DisplayOpenGL::SetZWrite(uint_t bEnable)
{
	glDepthMask(bEnable!=0);
}

void Burger::DisplayOpenGL::SetDepthTest(eDepthFunction uDepthFunction)
{
	BURGER_ASSERT(uDepthFunction<BURGER_ARRAYSIZE(s_DepthTest));
	glDepthFunc(s_DepthTest[uDepthFunction]);
	if (uDepthFunction==Display::DEPTHCMP_ALWAYS) {
		glDisable(GL_DEPTH_TEST);
	} else {
		glEnable(GL_DEPTH_TEST);
	}
}

void Burger::DisplayOpenGL::SetCullMode(eCullMode uCullMode)
{
	if (uCullMode==CULL_NONE) {
		glDisable(GL_CULL_FACE);
	} else if (uCullMode==CULL_CLOCKWISE) {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	} else if (uCullMode==CULL_COUNTERCLOCKWISE) {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
	}
}

void Burger::DisplayOpenGL::SetScissor(uint_t bEnable)
{
	if (bEnable) {
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}

void Burger::DisplayOpenGL::DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer)
{
	pVertexBuffer->CheckLoad(this);
#if defined(USE_GL2)
	glBindVertexArray(static_cast<VertexBufferOpenGL *>(pVertexBuffer)->GetVertexArrayObject());
	glDrawArrays(s_Prims[uPrimitiveType],0,static_cast<GLsizei>(static_cast<VertexBufferOpenGL *>(pVertexBuffer)->GetArrayEntryCount()));
#else
	BURGER_UNUSED(uPrimitiveType);
	BURGER_UNUSED(pVertexBuffer);
#endif
}

void Burger::DisplayOpenGL::DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer)
{
#if defined(USE_GL2)
	pVertexBuffer->CheckLoad(this);
	glBindVertexArray(static_cast<VertexBufferOpenGL *>(pVertexBuffer)->GetVertexArrayObject());
	glDrawElements(s_Prims[uPrimitiveType],static_cast<GLsizei>(static_cast<VertexBufferOpenGL *>(pVertexBuffer)->GetElementEntryCount()),GL_UNSIGNED_SHORT,0);
#else
	BURGER_UNUSED(uPrimitiveType);
	BURGER_UNUSED(pVertexBuffer);
#endif
}

/*! ************************************

	\fn float Burger::DisplayOpenGL::GetOpenGLVersion(void) const
	\brief Return the version of the OpenGL implementation

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0.0f if OpenGL is not started or a valid version number, example: 4.4f.

***************************************/

/*! ************************************

	\fn float Burger::DisplayOpenGL::GetShadingLanguageVersion(void) const
	\brief Return the version of the OpenGL Shader compiler

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0.0f if OpenGL is not started or a valid version number for the compiler, example: 4.4f.

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayOpenGL::GetCompressedFormatCount(void) const
	\brief Return the number of supported compressed texture formats

	When OpenGL is started, it's queried for the number of compressed texture
	formats it natively supports. This will return the number of formats
	and GetCompressedFormats() will be an array of this size
	with the supported formats.

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return 0 if OpenGL is not started or no texture compression is supported.
	\sa GetCompressedFormats()

***************************************/

/*! ************************************

	\fn const uint_t *Burger::DisplayOpenGL::GetCompressedFormats(void) const
	\brief Return the pointer to an array of supported compressed texture formats

	When OpenGL is started, it's queried for the number of compressed texture
	formats it natively supports and the types are stored in this array.
	This array size can be retrieved with a call to GetCompressedFormatCount().

	\note This value is valid AFTER the OpenGL driver has been
	started up via a call to InitContext().

	\return \ref NULL if OpenGL is not started or no texture compression is supported or an array of GetCompressedFormatCount() in size.
	\sa GetCompressedFormatCount()

***************************************/


/*! ************************************

	\brief Initialize the display for supporting OpenGL

	Once OpenGL is started, this function queries the driver for
	the supported feature list and sets up the rendering status for
	best performance in rendering scenes.

	\sa InitContext()

***************************************/

#if defined(BURGER_OPENGL) && defined(_DEBUG) && !defined(DOXYGEN)

// Data and a function to dump the OpenGL driver data
// for debugging

struct GLStringIndex_t {
	const char *m_pName;		// Printable name of the GL string
	GLenum m_eEnum;				// OpenGL enumeration to check
};

static const GLStringIndex_t g_StringIndexes[] = {
	{"OpenGL version",GL_VERSION},
	{"Vendor",GL_VENDOR},
	{"Renderer",GL_RENDERER},
	{"Extensions",GL_EXTENSIONS}
#if defined(USE_GL2)
	,{"Shader Language Version",GL_SHADING_LANGUAGE_VERSION}
#endif
};

static const GLStringIndex_t g_TextureIndexes[] = {
	{"GL_COMPRESSED_RGB_S3TC_DXT1_EXT",0x83F0},
	{"GL_COMPRESSED_RGBA_S3TC_DXT1_EXT",0x83F1},
	{"GL_COMPRESSED_RGBA_S3TC_DXT3_EXT",0x83F2},
	{"GL_COMPRESSED_RGBA_S3TC_DXT5_EXT",0x83F3},
	{"GL_PALETTE4_RGB8_OES",0x8B90},
	{"GL_PALETTE4_RGBA8_OES",0x8B91},
	{"GL_PALETTE4_R5_G6_B5_OES",0x8B92},
	{"GL_PALETTE4_RGBA4_OES",0x8B93},
	{"GL_PALETTE4_RGB5_A1_OES",0x8B94},
	{"GL_PALETTE8_RGB8_OES",0x8B95},
	{"GL_PALETTE8_RGBA8_OES",0x8B96},
	{"GL_PALETTE8_R5_G6_B5_OES",0x8B97},
	{"GL_PALETTE8_RGBA4_OES",0x8B98},
	{"GL_PALETTE8_RGB5_A1_OES",0x8B99},
	{"GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG",0x8C00},
	{"GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG",0x8C01},
	{"GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG",0x8C02},
	{"GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG",0x8C03},
	{"GL_COMPRESSED_R11_EAC",0x9270},
	{"GL_COMPRESSED_SIGNED_R11_EAC",0x9271},
	{"GL_COMPRESSED_RG11_EAC",0x9272},
	{"GL_COMPRESSED_SIGNED_RG11_EAC",0x9273},
	{"GL_COMPRESSED_RGB8_ETC2",0x9274},
	{"GL_COMPRESSED_SRGB8_ETC2",0x9275},
	{"GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2",0x9276},
	{"GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2",0x9277},
	{"GL_COMPRESSED_RGBA8_ETC2_EAC",0x9278},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC",0x9279},
	{"GL_COMPRESSED_RGBA_ASTC_4x4_KHR",0x93B0},
	{"GL_COMPRESSED_RGBA_ASTC_5x4_KHR",0x93B1},
	{"GL_COMPRESSED_RGBA_ASTC_5x5_KHR",0x93B2},
	{"GL_COMPRESSED_RGBA_ASTC_6x5_KHR",0x93B3},
	{"GL_COMPRESSED_RGBA_ASTC_6x6_KHR",0x93B4},
	{"GL_COMPRESSED_RGBA_ASTC_8x5_KHR",0x93B5},
	{"GL_COMPRESSED_RGBA_ASTC_8x6_KHR",0x93B6},
	{"GL_COMPRESSED_RGBA_ASTC_8x8_KHR",0x93B7},
	{"GL_COMPRESSED_RGBA_ASTC_10x5_KHR",0x93B8},
	{"GL_COMPRESSED_RGBA_ASTC_10x6_KHR",0x93B9},
	{"GL_COMPRESSED_RGBA_ASTC_10x8_KHR",0x93BA},
	{"GL_COMPRESSED_RGBA_ASTC_10x10_KHR",0x93BB},
	{"GL_COMPRESSED_RGBA_ASTC_12x10_KHR",0x93BC},
	{"GL_COMPRESSED_RGBA_ASTC_12x12_KHR",0x93BD},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR",0x93D0},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR",0x93D1},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR",0x93D2},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR",0x93D3},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR",0x93D4},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR",0x93D5},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR",0x93D6},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR",0x93D7},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR",0x93D8},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR",0x93D9},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR",0x93DA},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR",0x93DB},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR",0x93DC},
	{"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR",0x93DD}
};
#endif

void BURGER_API Burger::DisplayOpenGL::SetupOpenGL(void)
{
	const char *pString;

#if defined(_DEBUG)

	// For debug version, dump out OpenGL strings to the console
	// or logfile.txt

	{
		uintptr_t uCount = BURGER_ARRAYSIZE(g_StringIndexes);
		const GLStringIndex_t *pWork = g_StringIndexes;
		do {
			// Get the string for the enumeration
			pString = reinterpret_cast<const char *>(glGetString(pWork->m_eEnum));
			// If supported, print it
			if (pString) {
				Debug::Message("%s = ",pWork->m_pName);
				// Use String() because pResult can be long enough to overrun the buffer
				Debug::PrintString(pString);
				Debug::PrintString("\n");
			}
			++pWork;
		} while (--uCount);
	}
#endif

	//
	// Obtain the version of OpenGL found
	//

	float fVersion = 0.0f;
	pString = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	if (pString) {
		if (!MemoryCompare("OpenGL ES ",pString,10)) {
			pString += 10;
		}
		fVersion = AsciiToFloat(pString);
	}
	m_fOpenGLVersion = fVersion;

	//
	// Obtain the version of the OpenGL shader compiler
	//

	fVersion = 0.0f;
#if defined(USE_GL2)
	pString = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	if (pString) {
		if (!MemoryCompare("OpenGL ES GLSL ES ",pString,18)) {
			pString += 18;
		}
		fVersion = AsciiToFloat(pString);
	}
#endif
	m_fShadingLanguageVersion = fVersion;

	//
	// Obtain the supported compressed texture types
	//

	Free(m_pCompressedFormats);
	m_pCompressedFormats = NULL;
	uint_t uTemp = 0;
	GLint iTemp = 0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB,&iTemp);
#if defined(_DEBUG)
	Debug::Message("GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB = %i\n",iTemp);
#endif
	if (iTemp) {
		GLint iTemp2 = iTemp;
		GLint *pBuffer = static_cast<GLint *>(Alloc(sizeof(GLint)*iTemp2));
		if (pBuffer) {
			glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS_ARB,pBuffer);
			uTemp = static_cast<uint_t>(iTemp);
			m_pCompressedFormats = reinterpret_cast<uint_t *>(pBuffer);
			// Dump the list on debug builds
#if defined(_DEBUG)
			char HexString[16];
			HexString[0] = '0';
			HexString[1] = 'x';
			do {
				uint32_t uValue = static_cast<uint32_t>(pBuffer[0]);
				const char *pFormatName = HexString;
				const GLStringIndex_t *pTextureText = g_TextureIndexes;
				do {
					if (pTextureText->m_eEnum==uValue) {
						pFormatName = pTextureText->m_pName;
						break;
					}
				} while (++pTextureText<&g_TextureIndexes[BURGER_ARRAYSIZE(g_TextureIndexes)]);
				if (pFormatName==HexString) {
					NumberToAsciiHex(HexString+2,uValue,4);
				}
				Debug::Message("OpenGL supported compressed format %s\n",pFormatName);
				++pBuffer;
			} while (--iTemp2);
#endif
		}
	}
	m_uCompressedFormatCount = uTemp;

#if defined(USE_GL2)
	GLint iMaxAttributes = 1;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&iMaxAttributes);
	m_uMaximumVertexAttributes = static_cast<uint_t>(iMaxAttributes);
#endif

#if defined(_DEBUG)
	Debug::Message("m_uMaximumVertexAttributes = %u\n",m_uMaximumVertexAttributes);
#endif

	// If not supported, preflight with 1 attachment
	GLint iMaxColorattachments = 1;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,&iMaxColorattachments);
	m_uMaximumColorAttachments = static_cast<uint_t>(iMaxColorattachments);

#if defined(_DEBUG)
	Debug::Message("m_uMaximumColorAttachments = %u\n",m_uMaximumColorAttachments);
#endif

	m_uActiveTexture = 0;
}

/*! ************************************

	\brief Compile an OpenGL shader

	Given a string that has the source to a shader, compile it
	with OpenGL's GLSL compiler.

	If the source doesn't contain a \#version command as the first line, one
	will be inserted with the version of the current shader compiler.

	A define of \#define VERTEX_SHADER or \#define FRAGMENT_SHADER will be inserted
	into the top of the source to support unified shaders

	\code
	// Magically inserted code
	#if __VERSION__ >=140
	#ifdef VERTEX_SHADER
	#define PIPED out
	#define VERTEX_INPUT in
	#else
	#define PIPED in
	#define VERTEX_INPUT attribute
	#define FRAGCOLOR_USED out vec4 fragColor;
	#define gl_FragColor fragColor
	#define texture2D texture
	#endif
	#else
	#define PIPED varying
	#define FRAGCOLOR_USED
	#endif
	\endcode

	If the shader fails compilation, the error returned by OpenGL will be output
	to Debug::PrintString()

	\param GLEnum OpenGL shader enum GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	\param pShaderCode "C" string of the source code of the shader to compile
	\param uShaderCodeLength Length of the source code string. If zero, pShaderCode is assumed to be zero terminated
	\return Zero if the code can't be compiled, non-zero is a valid OpenGL shader reference

***************************************/

uint_t BURGER_API Burger::DisplayOpenGL::CompileShader(uint_t GLEnum,const char *pShaderCode,uintptr_t uShaderCodeLength) const
{
	// Create a blank shader
	GLuint uShader = 0;
#if defined(USE_GL2)
	// Valid pointer?
	if (pShaderCode) {

		// Determine the length of the code
		if (!uShaderCodeLength) {
			uShaderCodeLength = string_length(pShaderCode);
		}
		// Any code to parse?
		if (uShaderCodeLength) {
			// Create the shader
			uShader = glCreateShader(GLEnum);
			if (uShader) {
				const char *ShaderArray[3];		// Array of shader strings
				GLint ShaderLengthArray[3];
				char VersionString[64];			// Buffer for the optional "#version xxx\n" string
				GLsizei iIndex = 0;

				// Get the actual shader compiler version
				int32_t iVersion = static_cast<int32_t>(GetShadingLanguageVersion()*100.0f);

				// If a version opcode already exists, don't insert one

				if ((uShaderCodeLength<8) || MemoryCompare(g_Version,pShaderCode,8)) {

					// Since the first line isn't #version, create one

					string_copy(VersionString,g_Version);
					NumberToAscii(VersionString+9,iVersion);
					uintptr_t uLength = string_length(VersionString);
					// Append with a \n
					VersionString[uLength] = '\n';
					// Set the string in the array
					ShaderArray[0] = VersionString;
					ShaderLengthArray[0] = static_cast<GLint>(uLength+1);
					iIndex=1;
				} else {
					// Use the version found in the shader to determine the macros to use
					iVersion = AsciiToInteger(pShaderCode+8,iVersion,0,iVersion);
				}

				// Insert a #define for known shader types so
				// unified shaders can compile only what's
				// needed

				const char *pDefines;
				uintptr_t uDefinesLength;
				switch (GLEnum) {
				case GL_VERTEX_SHADER:
					if (iVersion>=140) {
						pDefines = g_VertexShader140;
						uDefinesLength = sizeof(g_VertexShader140)-1;
					} else {
						pDefines = g_VertexShader;
						uDefinesLength = sizeof(g_VertexShader)-1;
					}
					break;
				case GL_FRAGMENT_SHADER:
					if (iVersion>=140) {
						pDefines = g_FragmentShader140;
						uDefinesLength = sizeof(g_FragmentShader140)-1;
					} else {
						pDefines = g_FragmentShader;
						uDefinesLength = sizeof(g_FragmentShader)-1;
					}
					break;
				default:
					pDefines = NULL;
					uDefinesLength = 0;
					break;
				}

				if (pDefines) {
					ShaderArray[iIndex] = pDefines;
					ShaderLengthArray[iIndex] = static_cast<GLint>(uDefinesLength);
					++iIndex;
				}

				// Finally, insert the supplied source code
				ShaderArray[iIndex] = pShaderCode;
				ShaderLengthArray[iIndex] = static_cast<GLint>(uShaderCodeLength);

				// Compile the code
				glShaderSource(uShader,iIndex+1,ShaderArray,ShaderLengthArray);
				glCompileShader(uShader);
				GLint bCompiled = GL_FALSE;
				// Did it compile okay?
				glGetShaderiv(uShader,GL_COMPILE_STATUS,&bCompiled);
				if (bCompiled==GL_FALSE) {
					// Dump out what happened so a programmer
					// can debug the faulty shader
					GLint iLogLength;
					glGetShaderiv(uShader,GL_INFO_LOG_LENGTH,&iLogLength);
					if (iLogLength > 1) {
						// iLogLength includes space for the terminating null at the end of the string
						GLchar *pLog = static_cast<GLchar*>(Alloc(static_cast<uintptr_t>(iLogLength)));
						glGetShaderInfoLog(uShader,iLogLength,&iLogLength,pLog);

						// Note: The log could be so long that it could overflow the
						// Debug::Message buffer (Which would assert)
						// So use Debug::PrintString() to avoid this
						Debug::PrintString("Shader compile log:\n");
						Debug::PrintString(pLog);
						Debug::PrintString("\n");
						Free(pLog);
					}
					glDeleteShader(uShader);
					uShader = 0;
				}
			}
		}
	}
#else
	BURGER_UNUSED(GLEnum);
	BURGER_UNUSED(pShaderCode);
	BURGER_UNUSED(uShaderCodeLength);
#endif
	return uShader;
}


/*! ************************************

	\brief Compile and link a unified OpenGL shader

	Given a string that has the source to both a vertex and a
	fragment shader, compile them with OpenGL's GLSL compiler
	and link them all together.

	If the shader fails compilation or linking, the error returned
	by OpenGL will be output to Debug::PrintString()

	\param pUnifiedShader "C" string of the source code of the shader to compile
	\param uLength Length of the source code string. If zero, pProgram is assumed to be zero terminated
	\param pPosition Pointer to a "C" string of the label to use to attach to the program the vertex position from the vertex buffer object, set to \ref NULL for no connection.
	\param pNormal Pointer to a "C" string of the label to use to attach to the program the vertex normals from the vertex buffer object, set to \ref NULL for no connection.
	\param pTexcoord Pointer to a "C" string of the label to use to attach to the program the vertex texture coordinates from the vertex buffer object, set to \ref NULL for no connection.
	\return Zero if the code can't be compiled, non-zero is a valid OpenGL shader reference

***************************************/

uint_t BURGER_API Burger::DisplayOpenGL::CompileProgram(const char *pUnifiedShader,uintptr_t uLength,const OpenGLVertexInputs_t *pVertexInputs,const uint_t *pMembers) const
{
	return CompileProgram(pUnifiedShader,uLength,pUnifiedShader,uLength,pVertexInputs,pMembers);
}

uint_t BURGER_API Burger::DisplayOpenGL::CompileProgram(const char *pVertexShader,uintptr_t uVSLength,const char *pPixelShader,uintptr_t uPSLength,const OpenGLVertexInputs_t *pVertexInputs,const uint_t *pMembers) const
{
	GLuint uProgram = 0;
#if defined(USE_GL2)
	// Only if there is source to compile
	if (pVertexShader) {

		// Create a program object
		uProgram = glCreateProgram();
		uint_t bSuccess = FALSE;		// Assume failure

		// If any variable names are passed for position, normal or UVs, bind them
		// for linkage to a vertex buffer object

		if (pVertexInputs) {
			uint_t uIndex = pVertexInputs->m_uIndex;
			if (uIndex!=VertexBuffer::USAGE_END) {
				if (!pMembers) {
					GLuint uGLIndex = 0;
					do {
						glBindAttribLocation(uProgram,uGLIndex,pVertexInputs->m_pName);
						++pVertexInputs;
						++uGLIndex;
					} while (pVertexInputs->m_uIndex!=VertexBuffer::USAGE_END);
				} else {
					GLuint uGLIndex = 0;
					GLuint uGLIndexUsed;
					do {
						const uint_t *pTempMembers = pMembers;
						uint_t uMember = pTempMembers[0];
						uGLIndexUsed = uGLIndex;
						if (uMember!=VertexBuffer::USAGE_END) {
							do {
								if (!((uMember^pVertexInputs->m_uIndex)&VertexBuffer::USAGE_TYPEMASK)) {
									uGLIndexUsed = static_cast<GLuint>(pTempMembers-pMembers);
									break;
								}
								++pTempMembers;
								uMember = pTempMembers[0];
							} while (uMember!=VertexBuffer::USAGE_END);
						}
						glBindAttribLocation(uProgram,uGLIndexUsed,pVertexInputs->m_pName);
						++pVertexInputs;
						++uGLIndex;
					} while (pVertexInputs->m_uIndex!=VertexBuffer::USAGE_END);
				}
			}
		}

		// Compile the vertex shader

		GLuint uVertexShader = CompileShader(GL_VERTEX_SHADER,pVertexShader,uVSLength);
		if (uVertexShader) {
			// Attach the vertex shader to our program
			glAttachShader(uProgram,uVertexShader);
			// Allow the program to be the sole owner of the shader
			glDeleteShader(uVertexShader);

			// Compile the fragment shader
			GLuint uFragmentShader = CompileShader(GL_FRAGMENT_SHADER,pPixelShader,uPSLength);
			if (uFragmentShader) {
				// Attach the fragment shader to our program
				glAttachShader(uProgram,uFragmentShader);
				// Allow the program to be the sole owner of the shader
				glDeleteShader(uFragmentShader);

				// Link everything together!
				glLinkProgram(uProgram);

				// Check for link failure
				GLint iStatus;
				glGetProgramiv(uProgram,GL_LINK_STATUS,&iStatus);
				if (iStatus==GL_FALSE) {

					Debug::PrintString("Failed to link program\n");
					// Print out the log
					GLint iLogLength;
					glGetProgramiv(uProgram,GL_INFO_LOG_LENGTH,&iLogLength);
					if (iLogLength > 1) {
						GLchar *pErrorLog = static_cast<GLchar*>(Alloc(static_cast<uintptr_t>(iLogLength)));
						glGetProgramInfoLog(uProgram,iLogLength,&iLogLength,pErrorLog);
						Debug::PrintString("Program link log:\n");
						Debug::PrintString(pErrorLog);
						Debug::PrintString("\n");
						Free(pErrorLog);
					}

				} else {

					// Verify if it's REALLY okay
					glValidateProgram(uProgram);

					// Is the all clear signaled?
					glGetProgramiv(uProgram,GL_VALIDATE_STATUS,&iStatus);
					if (iStatus==GL_FALSE) {
						// Dump the log for post link validation failures
						GLint iLogLength;
						glGetProgramiv(uProgram,GL_INFO_LOG_LENGTH,&iLogLength);
						Debug::PrintString("Failed to validate program\n");
						if (iLogLength > 1) {
							GLchar *pErrorLog = static_cast<GLchar*>(Alloc(static_cast<uintptr_t>(iLogLength)));
							glGetProgramInfoLog(uProgram, iLogLength, &iLogLength,pErrorLog);
							Debug::PrintString("Program validate log:\n");
							Debug::PrintString(pErrorLog);
							Debug::PrintString("\n");
							Free(pErrorLog);
						}
					} else {
						// All good!!!
						bSuccess = TRUE;
					}
				}
			}
		}
		// On failure, dispose of the program
		// to prevent memory leaks
		if (!bSuccess) {
			glDeleteProgram(uProgram);
			uProgram = 0;
		}
	}
#else
	BURGER_UNUSED(pVertexShader);
	BURGER_UNUSED(uVSLength);
	BURGER_UNUSED(pPixelShader);
	BURGER_UNUSED(uPSLength);
	BURGER_UNUSED(pVertexInputs);
	BURGER_UNUSED(pMembers);
#endif
	return uProgram;
}


//
// Create a vertex array object
//

uint_t BURGER_API Burger::DisplayOpenGL::CreateVertexArrayObject(const OpenGLVertexBufferObjectDescription_t *pDescription) const
{
	GLuint uVertexArrayObjectID = 0;
#if defined(USE_GL2)
	if (pDescription) {

		// Create a vertex array object
		glGenVertexArrays(1,&uVertexArrayObjectID);
		if (uVertexArrayObjectID) {
			uint_t bSuccess = TRUE;		// Assume success!

			glBindVertexArray(uVertexArrayObjectID);
			GLuint uBufferID;

			//
			// Are there vertex positions?
			//

			if (pDescription->m_uPositionSize) {

				// Create a vertex buffer object to store positions
				glGenBuffers(1,&uBufferID);
				GLuint uPosBufferID = uBufferID;
				if (!uPosBufferID) {
					bSuccess = FALSE;
				} else {
					glBindBuffer(GL_ARRAY_BUFFER,uPosBufferID);

					// Allocate and load position data into the Vertex Buffer Object
					glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uPositionSize),pDescription->m_pPositions,static_cast<GLenum>(pDescription->m_pPositions ? GL_STATIC_DRAW : GL_STREAM_DRAW));

					// Enable the position attribute for this Vertex Buffer Object
					glEnableVertexAttribArray(0);

					// Get the size of the position type so we can set the stride properly
					uintptr_t uPositionTypeSize = GetGLTypeSize(pDescription->m_ePositionType);

					// Set up the description of the position array
					glVertexAttribPointer(0,
						static_cast<GLint>(pDescription->m_uPositionElementCount),pDescription->m_ePositionType,
						GL_FALSE,static_cast<GLsizei>(pDescription->m_uPositionElementCount*uPositionTypeSize),NULL);
				}
			}

			//
			// Are there vertex normals?
			//

			if (pDescription->m_uNormalSize) {

				// Create a vertex buffer object to store positions
				glGenBuffers(1,&uBufferID);
				GLuint uNormalBufferID = uBufferID;
				if (!uNormalBufferID) {
					bSuccess = FALSE;
				} else {
					glBindBuffer(GL_ARRAY_BUFFER,uNormalBufferID);

					// Allocate and load position data into the Vertex Buffer Object
					glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uNormalSize),pDescription->m_pNormals,static_cast<GLenum>(pDescription->m_pNormals ? GL_STATIC_DRAW : GL_STREAM_DRAW));

					// Enable the normal attribute for this Vertex Buffer Object
					glEnableVertexAttribArray(1);

					// Get the size of the normal type so we can set the stride properly
					uintptr_t uNormalTypeSize = GetGLTypeSize(pDescription->m_eNormalType);

					// Set up the description of the normal array
					glVertexAttribPointer(1,
						static_cast<GLint>(pDescription->m_uNormalElementCount),pDescription->m_eNormalType,
						GL_FALSE,static_cast<GLsizei>(pDescription->m_uNormalElementCount*uNormalTypeSize),NULL);
				}
			}

			//
			// Are there texture UV coordinates?
			//

			if (pDescription->m_uTexcoordSize) {

				// Create a vertex buffer object to store UV coordinates
				glGenBuffers(1,&uBufferID);
				GLuint uUVBufferID = uBufferID;
				if (!uUVBufferID) {
					bSuccess = FALSE;
				} else {
					glBindBuffer(GL_ARRAY_BUFFER,uUVBufferID);

					// Allocate and load UV coordinates into the Vertex Buffer Object
					glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uTexcoordSize),pDescription->m_pTexcoords,static_cast<GLenum>(pDescription->m_pTexcoords ? GL_STATIC_DRAW : GL_STREAM_DRAW));

					// Enable the UV coordinates attribute for this Vertex Buffer Object
					glEnableVertexAttribArray(2);

					// Get the size of the UV coordinates type so we can set the stride properly
					uintptr_t uUVTypeSize = GetGLTypeSize(pDescription->m_eTexcoordType);

					// Set up the description of the UV coordinates array
					glVertexAttribPointer(2,
						static_cast<GLint>(pDescription->m_uTexcoordElementCount),pDescription->m_eTexcoordType,
						GL_FALSE,static_cast<GLsizei>(pDescription->m_uTexcoordElementCount*uUVTypeSize),NULL);
				}
			}

			if (pDescription->m_uElementSize) {

				// Attach the array of elements to the Vertex Buffer Object
				glGenBuffers(1,&uBufferID);
				GLuint uElementBufferID = uBufferID;
				if (!uElementBufferID) {
					bSuccess = FALSE;
				} else {
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,uElementBufferID);
					// Allocate and load vertex array element data into Vertex Buffer Object
					glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(pDescription->m_uElementSize),pDescription->m_pElements,static_cast<GLenum>(pDescription->m_pElements ? GL_STATIC_DRAW : GL_STREAM_DRAW));
				}
			}

			// Was there a failure of one of the generated arrays?
			if (!bSuccess) {
				DeleteVertexArrayObject(uVertexArrayObjectID);
				uVertexArrayObjectID = 0;
			}
		}
	}
#else
	BURGER_UNUSED(pDescription);
#endif
	return uVertexArrayObjectID;
}

/*! ************************************

	\brief Dispose of a vertex array object

	Dispose of all the vertex objects (GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)
	and the GL_ELEMENT_ARRAY_BUFFER_BINDING object and then
	dispose of the vertex array object.

	\param uVertexArrayObject OpenGL vertex array object (0 does nothing)

***************************************/

void BURGER_API Burger::DisplayOpenGL::DeleteVertexArrayObject(uint_t uVertexArrayObject) const
{
#if defined(USE_GL2)
	if (uVertexArrayObject) {
		// Bind the vertex array object so we can get data from it
		glBindVertexArray(uVertexArrayObject);

		// For every possible attribute set in the vertex array object
		GLint iBufferID;
		GLuint uBufferID;
		GLuint uIndex = 0;
		do {
			// Get the vertex array object set for that attribute
			glGetVertexAttribiv(uIndex,GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,&iBufferID);

			// If there was a vertex array object set...
			if (iBufferID) {
				//...delete the vertex array object
				uBufferID = static_cast<GLuint>(iBufferID);
				glDeleteBuffers(1,&uBufferID);
			}
		} while (++uIndex<m_uMaximumVertexAttributes);

		// Get any element array vertex buffer object set in the vertex array object
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&iBufferID);

		// If there was a element array vertex buffer object set in the vertex array object
		if (iBufferID) {
			//...delete the VBO
			uBufferID = static_cast<GLuint>(iBufferID);
			glDeleteBuffers(1,&uBufferID);
		}
		glBindVertexArray(0);
		// Finally, delete the vertex array object
		uBufferID = static_cast<GLuint>(uVertexArrayObject);
		glDeleteVertexArrays(1,&uBufferID);
	}
#else
	BURGER_UNUSED(uVertexArrayObject);
#endif
}

/*! ************************************

	\brief Create a render target frame buffer

	Given a size in pixels, create a texture of a specific bit depth and clamp
	type and attach an optional ZBuffer

	This frame buffer can be disposed of with a call to DeleteFrameBufferObject(uint_t)

	\param uWidth Width of the new frame buffer in pixels
	\param uHeight Height of the new frame buffer in pixels
	\param uGLDepth OpenGL type for the pixel type of the color buffer, example GL_RGBA
	\param uGLClamp OpenGL clamp type for the edge of the frame buffer texture, example GL_CLAMP_TO_EDGE
	\param uGLZDepth OpenGL type for the depth buffer, if any. Zero will not generate a depth buffer, GL_DEPTH_COMPONENT16 or equivalent will generate one.
	\return Frame buffer ID or zero on failure
	\sa DeleteFrameBufferObject(uint_t) const

***************************************/

uint_t BURGER_API Burger::DisplayOpenGL::BuildFrameBufferObject(uint_t uWidth,uint_t uHeight,uint_t uGLDepth,uint_t uGLClamp,uint_t uGLZDepth) const
{
	GLuint uFrontBufferObject = 0;
#if defined(USE_GL2)
	// Create the front buffer texture
	GLuint uColorTextureID;
	glGenTextures(1,&uColorTextureID);
	glBindTexture(GL_TEXTURE_2D,uColorTextureID);

	// Set up filter and wrap modes for this texture object
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,static_cast<GLint>(uGLClamp));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,static_cast<GLint>(uGLClamp));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// OpenGLES sucks rocks on performance, so use GL_LINEAR
#if defined(BURGER_OPENGLES)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
#endif

	// Create an uninitialized frame buffer for the requested pixel type
	glTexImage2D(GL_TEXTURE_2D,0,static_cast<GLint>(uGLDepth),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight),0,uGLDepth,GL_UNSIGNED_BYTE,NULL);

	// Create a depth buffer if needed
	GLuint uDepthRenderbuffer = 0;
	if (uGLZDepth) {
		glGenRenderbuffers(1,&uDepthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER,uDepthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER,uGLZDepth,static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight));
	}

	// Create the frame buffer
	glGenFramebuffers(1,&uFrontBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER,uFrontBufferObject);
	// Attach the color texture
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,uColorTextureID,0);

	// Attach the optional depth texture buffer
	if (uDepthRenderbuffer) {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,uDepthRenderbuffer);
	}

	// Is this all kosher? Or are the happy feelings gone?
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// Dispose everything
		DeleteFrameBufferObject(uFrontBufferObject);
		uFrontBufferObject = 0;
	}
#else
	BURGER_UNUSED(uWidth);
	BURGER_UNUSED(uHeight);
	BURGER_UNUSED(uGLDepth);
	BURGER_UNUSED(uGLClamp);
	BURGER_UNUSED(uGLZDepth);
#endif
	// Exit with the frame buffer or zero
	return uFrontBufferObject;
}

/*! ************************************

	\brief Dispose of a frame buffer data object's attachment

	Given an OpenGL frame buffer attachment like GL_DEPTH_ATTACHMENT, delete
	it from the currently bound Framebuffer. It will first query the
	attachment to determine if it's a GL_RENDERBUFFER or a GL_TEXTURE
	and call the appropriate disposal routine

	\param uAttachment OpenGL attachment enumeration
	\sa DeleteFrameBufferObject(uint_t) const

***************************************/

void BURGER_API Burger::DisplayOpenGL::DeleteFrameBufferObjectAttachment(uint_t uAttachment)
{
#if defined(USE_GL2)
	GLint iObjectID;
	// Get the type of frame buffer
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,uAttachment,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,&iObjectID);
	GLuint uObjectID;
	// If it's a render buffer, call glDeleteRenderbuffers()
	if (GL_RENDERBUFFER == iObjectID) {
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,uAttachment,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&iObjectID);
		uObjectID = static_cast<GLuint>(iObjectID);
		glDeleteRenderbuffers(1,&uObjectID);

	// If it's a texture buffer, call glDeleteTextures()
	} else if (GL_TEXTURE == iObjectID) {
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,uAttachment,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&iObjectID);
		uObjectID = static_cast<GLuint>(iObjectID);
		glDeleteTextures(1,&uObjectID);
	}
#else
	BURGER_UNUSED(uAttachment);
#endif
}

/*! ************************************

	\brief Dispose of a frame buffer data object

	Given an OpenGL frame buffer, dispose of it and everything attached to it.

	\param uFrameBufferObject OpenGL frame buffer (Zero does nothing)
	\sa BuildFrameBufferObject(uint_t,uint_t,uint_t,uint_t,uint_t) const or DeleteFrameBufferObjectAttachment(uint_t)

***************************************/

void BURGER_API Burger::DisplayOpenGL::DeleteFrameBufferObject(uint_t uFrameBufferObject) const
{
#if defined(USE_GL2)
	if (uFrameBufferObject) {
		glBindFramebuffer(GL_FRAMEBUFFER,uFrameBufferObject);
		uint_t uCount = m_uMaximumColorAttachments;
		if (uCount) {
			uint_t uColorAttachment = GL_COLOR_ATTACHMENT0;
			do {
				// For every color buffer attached delete the attachment
				DeleteFrameBufferObjectAttachment(uColorAttachment);
				++uColorAttachment;
			} while (--uCount);
		}

		// Delete any depth or stencil buffer attached
		DeleteFrameBufferObjectAttachment(GL_DEPTH_ATTACHMENT);
		DeleteFrameBufferObjectAttachment(GL_STENCIL_ATTACHMENT);
		GLuint uObjectID = static_cast<GLuint>(uFrameBufferObject);
		glDeleteFramebuffers(1,&uObjectID);
	}
#else
	BURGER_UNUSED(uFrameBufferObject);
#endif
}

/*! ************************************

	\brief Create an OpenGL texture

	Given a bit map image, upload it to the OpenGL system while
	trying to retain the format as close as possible.

	Mip Maps are supported.

	\param pImage Pointer to a valid image structure
	\param bGenerateMipMap If \ref TRUE and the image is a single image, alert OpenGL to automatically generate mipmaps
	\return Zero on failure (OpenGL allocation or Image isn't a format that can be uploaded)

***************************************/

uint_t BURGER_API Burger::DisplayOpenGL::CreateTextureID(const Image *pImage,uint_t bGenerateMipMap)
{
	GLuint uTextureID = 0;

	Image::ePixelTypes eType = pImage->GetType();
	GLenum iType = GL_UNSIGNED_BYTE;
	GLenum iFormat = 0;
	switch (eType) {
	case Image::PIXELTYPE888:
		iFormat = GL_RGB;
		break;
	case Image::PIXELTYPE8888:
		iFormat = GL_RGBA;
		break;
	default:
		break;
	}

	// Is the format supported?

	if (iFormat) {
		// Start with creating a new texture instance
		glGenTextures(1,&uTextureID);
		// Got a texture?
		if (uTextureID) {
			// Bind it
			glBindTexture(GL_TEXTURE_2D,uTextureID);

			// Set up filter and wrap modes for this texture object
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			GLint iParm = GL_LINEAR;
			// Mip mapped?
			if (bGenerateMipMap || (pImage->GetMipMapCount()>=2)) {
				iParm = GL_LINEAR_MIPMAP_LINEAR;
			}
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,iParm);

			// Bytes are packed together (Needed for RGB format)
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);

			// If the bytes in the image are packed together, then
			// it's a simple upload, otherwise, do it the hard way

			uint_t uMipMap = 0;
			do {
				uint_t uWidth = pImage->GetWidth(uMipMap);
				uint_t uHeight = pImage->GetHeight(uMipMap);
				const uint8_t *pSource = pImage->GetImage(uMipMap);

				if (uMipMap || (pImage->GetSuggestedStride()==pImage->GetStride())) {
					// Allocate and load image data into texture in one go
					glTexImage2D(GL_TEXTURE_2D,static_cast<GLint>(uMipMap),static_cast<GLint>(iFormat),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight),0,iFormat,iType,pSource);
				} else {
					// Allocate the memory for the texture
					glTexImage2D(GL_TEXTURE_2D,0,static_cast<GLint>(iFormat),static_cast<GLsizei>(uWidth),static_cast<GLsizei>(uHeight),0,iFormat,iType,NULL);

					// Upload one line at a time to support stride
					if (uHeight) {
						uint_t uY=0;
						uintptr_t uStride = pImage->GetStride(uMipMap);
						do {
							glTexSubImage2D(GL_TEXTURE_2D,0,0,static_cast<GLsizei>(uY),static_cast<GLsizei>(uWidth),1,iFormat,iType,pSource);
							pSource += uStride;
							++uY;
						} while (--uHeight);
					}
				}
			} while (++uMipMap<pImage->GetMipMapCount());

			// If the texture doesn't have a mip map and one is requested, generate it

#if defined(USE_GL2)
			if (bGenerateMipMap && (pImage->GetMipMapCount()<2)) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}
#endif
		}
	}
	return static_cast<uint_t>(uTextureID);
}

/*! ************************************

	\brief Convert an OpenGL error enumeration into a string

	Given an enum from a call to glGetError(), call this function
	convert the number into a string describing the error.

	\param uGLErrorEnum OpenGL error enum
	\return Pointer to a "C" string with the error message (Don't dispose)

***************************************/

static const MessageLookup_t g_GetErrorString[] = {
	CASE(GL_NO_ERROR),
	CASE(GL_INVALID_VALUE),
	CASE(GL_INVALID_OPERATION),
	CASE(GL_STACK_OVERFLOW),
	CASE(GL_STACK_UNDERFLOW),
	CASE(GL_OUT_OF_MEMORY),
	CASE(GL_INVALID_FRAMEBUFFER_OPERATION),
	CASE(GL_TABLE_TOO_LARGE),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_FORMATS),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT),
	CASE(GL_FRAMEBUFFER_UNSUPPORTED),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER),
	CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
};

const char * BURGER_API Burger::DisplayOpenGL::GetErrorString(uint_t uGLErrorEnum)
{
	const char *pResult = "GL_UNKNOWN_ERROR";
	uintptr_t uCount = BURGER_ARRAYSIZE(g_GetErrorString);
	const MessageLookup_t *pLookup = g_GetErrorString;
	do {
		if (uGLErrorEnum==pLookup->m_uEnum) {
			pResult = pLookup->m_pName;
			break;
		}
		++pLookup;
	} while (--uCount);
	return pResult;
}

/*! ************************************

	\brief Determine an OpenGL type enumeration byte length

	Given an OpenGL data type, such as GL_BYTE, GL_SHORT, or GL_FLOAT,
	return the number of bytes such a data chunk would occupy

	\param uGLTypeEnum OpenGL data type enum
	\return Number of bytes for the type or 0 if unknown

***************************************/

uintptr_t BURGER_API Burger::DisplayOpenGL::GetGLTypeSize(uint_t uGLTypeEnum)
{
	uintptr_t uResult;
	switch (uGLTypeEnum) {
	case GL_BYTE:
		uResult = sizeof(GLbyte);
		break;
	case GL_UNSIGNED_BYTE:
		uResult = sizeof(GLubyte);
		break;
	case GL_SHORT:
		uResult = sizeof(GLshort);
		break;
	case GL_UNSIGNED_SHORT:
		uResult = sizeof(GLushort);
		break;
	case GL_INT:
		uResult = sizeof(GLint);
		break;
	case GL_UNSIGNED_INT:
		uResult = sizeof(GLuint);
		break;
	case GL_FLOAT:
		uResult = sizeof(GLfloat);
		break;
#if defined(GL_2_BYTES)
	case GL_2_BYTES:
		uResult = 2;
		break;
#endif
#if defined(GL_3_BYTES)
	case GL_3_BYTES:
		uResult = 3;
		break;
#endif
#if defined(GL_4_BYTES)
	case GL_4_BYTES:
		uResult = 4;
		break;
#endif
#if defined(GL_DOUBLE)
	case GL_DOUBLE:
		uResult = sizeof(GLdouble);
		break;
#endif
	default:
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Poll OpenGL for errors and print them

	Call glGetError(), and if any errors were found,
	print them using Debug::Warning().

	Used for debugging OpenGL

	\param pErrorLocation Pointer to a string that describes where this
		error condition could be occurring.

	\return \ref TRUE if an error was found, \ref FALSE if not

***************************************/

uint_t BURGER_API Burger::DisplayOpenGL::PrintGLError(const char *pErrorLocation)
{
	uint_t uResult = FALSE;
	GLenum eError = glGetError();
	if (eError != GL_NO_ERROR) {
		do {
			Debug::Message("GLError %s set in location %s\n",GetErrorString(static_cast<uint_t>(eError)),pErrorLocation);
			eError = glGetError();
		} while (eError != GL_NO_ERROR);
		uResult = TRUE;
	}
	return uResult;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayOpenGL::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/
#endif

