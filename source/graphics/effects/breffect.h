/***************************************

	Base shader effect class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class EffectDX9 : public Base {
	BURGER_RTTI_IN_CLASS();
protected:
	IDirect3DPixelShader9 *m_pPixelShader;		///< DirectX9 Pixel shader
	IDirect3DVertexShader9 *m_pVertexShader;	///< DirectX9 Vertex shader
	IDirect3DDevice9 *m_pDevice;				///< DirectX9 parent device
public:
	EffectDX9();
	virtual ~EffectDX9();
	void BURGER_API SetEffect(void);
};
#endif
class EffectOpenGL : public Base {
	BURGER_RTTI_IN_CLASS();
protected:
	Word m_uEffect;				///< Program ID for the compiled shader
public:
	EffectOpenGL();
	virtual ~EffectOpenGL();
	void BURGER_API SetEffect(void);
};
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
class Effect : public Base {
	BURGER_RTTI_IN_CLASS();
protected:
	D3DPixelShader *m_pPixelShader;		///< (Xbox360 only) Pixel shader
	D3DVertexShader *m_pVertexShader;	///< (Xbox360 only) Vertex shader
	D3DDevice *m_pDevice;				///< (Xbox360 only) parent device
public:
	Effect();
	virtual ~Effect();
	void BURGER_API SetEffect(void);
};
#endif
#if defined(BURGER_WINDOWS)
typedef EffectDX9 Effect;
#elif defined(BURGER_OPENGL_SUPPORTED)
typedef EffectOpenGL Effect;
#endif
}
/* END */

#endif
