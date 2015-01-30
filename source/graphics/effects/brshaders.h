/***************************************

	Shaders

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRSHADERS_H__
#define __BRSHADERS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
#if defined(BURGER_WINDOWS)
extern const Word8 g_pstexturedx9[];
extern const Word8 g_pstexturecolordx9[];
extern const Word8 g_vs20sprite2ddx9[];
extern const Word8 g_vsstaticpositiondx9[];
#endif
#if defined(BURGER_OPENGL_SUPPORTED)
extern const char g_glDiffuseTexture[];
extern const char g_glPosition[];
extern const char g_glTexcoord[];
extern const char g_glColor[];
extern const char g_glViewProjectionMatrix[];
extern const char g_glXYWidthHeight[];
extern const char g_pstexturegl[];
extern const char g_pstexturecolorgl[];
extern const char g_vs20sprite2dgl[];
extern const char g_vsstaticpositiongl[];
#endif
#if defined(BURGER_XBOX360)
extern const Word32 g_pstexture360[];
extern const Word32 g_pstexturecolor360[];
extern const Word32 g_vs20sprite2d360[];
extern const Word32 g_vsstaticposition360[];
#endif
#if defined(BURGER_VITA)
extern const Word8 g_pstexturevitaimage[];
extern const Word8 g_pstexturecolorvitaimage[];
extern const Word8 g_vs20sprite2dvitaimage[];
extern const Word8 g_vsstaticpositionvitaimage[];
#endif
}
/* END */

#endif
