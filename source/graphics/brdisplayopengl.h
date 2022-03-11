/***************************************

    OpenGL manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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

#if defined(BURGER_ANDROID) && !defined(__BRANDROIDTYPES_H__)
#include "brandroidtypes.h"
#endif

/* BEGIN */
namespace Burger {
class Image;
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayOpenGL : public Display {
	BURGER_RTTI_IN_CLASS();
protected:
	uint_t *m_pCompressedFormats;		///< Pointer to an array of supported OpenGL compressed textures
	HDC__ *m_pOpenGLDeviceContext;	///< (Windows only) Window to attach the GL context to
	HGLRC__ *m_pOpenGLContext;		///< (Windows only) Current OpenGL context
	uint_t m_bResolutionChanged;		///< (Windows only) Set to \ref TRUE if the display needs to be restored
	float m_fOpenGLVersion;					///< Numeric value for the version of OpenGL
	float m_fShadingLanguageVersion;		///< Numeric value for the version of the Shader compiler
	uint_t m_uCompressedFormatCount;			///< Number of supported compressed texture formats
	uint_t m_uMaximumVertexAttributes;		///< Maximum number of vertex attributes GL_MAX_VERTEX_ATTRIBS
	uint_t m_uMaximumColorAttachments;		///< Maximum number of color frame attachments GL_MAX_COLOR_ATTACHMENTS
	uint_t m_uActiveTexture;					///< OpenGL glActiveTexture() shadow value
public:
	DisplayOpenGL(GameApp *pGameApp);
	uint_t Init(uint_t uWidth,uint_t uHeight,uint_t uDepth=32,uint_t uFlags=DEFAULTFLAGS) BURGER_OVERRIDE;
	void Shutdown(void) BURGER_OVERRIDE;
	void BeginScene(void) BURGER_OVERRIDE;
	void EndScene(void) BURGER_OVERRIDE;
	Texture *CreateTextureObject(void) BURGER_OVERRIDE;
	VertexBuffer *CreateVertexBufferObject(void) BURGER_OVERRIDE;
	void Resize(uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetViewport(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetScissorRect(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha) BURGER_OVERRIDE;
	void SetClearDepth(float fDepth) BURGER_OVERRIDE;
	void Clear(uint_t uMask) BURGER_OVERRIDE;
	void Bind(Texture *pTexture,uint_t uIndex=0) BURGER_OVERRIDE;
	void Bind(Effect *pEffect) BURGER_OVERRIDE;
	void SetBlend(uint_t bEnable) BURGER_OVERRIDE;
	void SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor) BURGER_OVERRIDE;
	void SetLighting(uint_t bEnable) BURGER_OVERRIDE;
	void SetZWrite(uint_t bEnable) BURGER_OVERRIDE;
	void SetDepthTest(eDepthFunction uDepthFunction) BURGER_OVERRIDE;
	void SetCullMode(eCullMode uCullMode) BURGER_OVERRIDE;
	void SetScissor(uint_t bEnable) BURGER_OVERRIDE;
    void DrawPrimitive(ePrimitiveType uPrimitiveType, VertexBuffer* pVertexBuffer) BURGER_OVERRIDE;
	void DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer) BURGER_OVERRIDE;

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
	static void BURGER_API WindowsLink(void);
	static void BURGER_API WindowsUnlink(void);
    static BURGER_INLINE uint_t GetFrontBuffer(void) { return 0; }

};
#endif
}
/* END */

#endif
