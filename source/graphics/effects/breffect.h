/***************************************

	Base shader effect class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BREFFECT_H__
#define __BREFFECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRDISPLAYOBJECT_H__
#include "brdisplayobject.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {

class Effect : public DisplayObject {

    BURGER_DISABLE_COPY(Effect);
	BURGER_RTTI_IN_CLASS();

protected:

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DPixelShader *m_pPixelShader;		///< (Xbox360 only) Pixel shader
	D3DVertexShader *m_pVertexShader;	///< (Xbox360 only) Vertex shader
	D3DDevice *m_pDevice;				///< (Xbox360 only) parent device
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_OPENGL) || defined(DOXYGEN)
	union {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		struct {
			IDirect3DPixelShader9 *m_pPixelShader;		///< DirectX9 Pixel shader
			IDirect3DVertexShader9 *m_pVertexShader;	///< DirectX9 Vertex shader
			IDirect3DDevice9 *m_pDevice;				///< DirectX9 parent device
		} m_DX9;		///< DirectX 9 specific data
#endif
		struct {
			Word m_uProgramID;							///< OpenGL Program ID for the compiled shader
		} m_GL;			///< OpenGL specific data
	} m_ShaderData;		///< Union of vertex/pixel shader data
#endif

public:
	Effect();
	virtual ~Effect();
#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
	virtual void Release(Display *pDisplay) BURGER_OVERRIDE;
#endif

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	BURGER_INLINE D3DPixelShader *GetPixelShader360(void) const { return m_pPixelShader; }
	BURGER_INLINE D3DVertexShader *GetVertexShader360(void) const { return m_pVertexShader; }
	BURGER_INLINE D3DDevice *GetDevice360(void) const { return m_pDevice; }
#endif

#if defined(BURGER_OPENGL) || defined(DOXYGEN)
	BURGER_INLINE Word GetProgramID(void) const { return m_ShaderData.m_GL.m_uProgramID; }
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE IDirect3DPixelShader9 *GetPixelShaderDX9(void) const { return m_ShaderData.m_DX9.m_pPixelShader; }
	BURGER_INLINE IDirect3DVertexShader9 *GetVertexShaderDX9(void) const { return m_ShaderData.m_DX9.m_pVertexShader; }
	BURGER_INLINE IDirect3DDevice9 *GetDeviceDX9(void) const { return m_ShaderData.m_DX9.m_pDevice; }
protected:
	void BURGER_API ReleaseGL(void);
	void BURGER_API ReleaseDX9(void);
#endif
};
}
/* END */

#endif
