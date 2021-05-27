/***************************************

    Display base class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSHAPE8BIT_H__
#define __BRSHAPE8BIT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRRENDERER_H__
#include "brrenderer.h"
#endif

/* BEGIN */
namespace Burger {
struct Shape8Bit_t {
	uint16_t m_usWidth;	///< Width of the shape
	uint16_t m_usHeight;	///< Height of the shape
	uint8_t m_Data[1];	///< Raw shape data
	BURGER_INLINE uint_t GetWidth(void) const { return m_usWidth; }
	BURGER_INLINE uint_t GetHeight(void) const { return m_usHeight; }
	BURGER_INLINE const uint8_t *GetPixels(void) const { return m_Data; }
	BURGER_INLINE void Draw(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixels(iX,iY,GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	BURGER_INLINE void DrawMasked(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixelsMasked(iX,iY,GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	static Shape8Bit_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct Shape8BitOffset_t {
	short m_sXOffset;			///< Signed offset for x
	short m_sYOffset;			///< Signed offset for y
	Shape8Bit_t m_Shape8Bit;	///< Shape data
	BURGER_INLINE uint_t GetWidth(void) const { return m_Shape8Bit.GetWidth(); }
	BURGER_INLINE uint_t GetHeight(void) const { return m_Shape8Bit.GetHeight(); }
	BURGER_INLINE const uint8_t *GetPixels(void) const { return m_Shape8Bit.GetPixels(); }
	BURGER_INLINE int GetXOffset(void) const { return m_sXOffset; }
	BURGER_INLINE int GetYOffset(void) const { return m_sYOffset; }
	BURGER_INLINE void Draw(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixels(iX+GetXOffset(),iY+GetYOffset(),GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	BURGER_INLINE void DrawMasked(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixelsMasked(iX+GetXOffset(),iY+GetYOffset(),GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	static Shape8BitOffset_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct Shape8BitArray_t {
	uint32_t m_Index[1];			///< Array of indexes to the shape array
	BURGER_INLINE Shape8Bit_t *GetShape(uint_t uIndex) { return reinterpret_cast<Shape8Bit_t *>(reinterpret_cast<uint8_t *>(this)+m_Index[uIndex]); } 
	BURGER_INLINE const Shape8Bit_t *GetShape(uint_t uIndex) const { return reinterpret_cast<const Shape8Bit_t *>(reinterpret_cast<const uint8_t *>(this)+m_Index[uIndex]); } 
	static Shape8BitArray_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct Shape8BitOffsetArray_t {
	uint32_t m_Index[1];			///< Array of indexes to the shape array
	BURGER_INLINE Shape8BitOffset_t *GetShape(uint_t uIndex) { return reinterpret_cast<Shape8BitOffset_t *>(reinterpret_cast<uint8_t *>(this)+m_Index[uIndex]); } 
	BURGER_INLINE const Shape8BitOffset_t *GetShape(uint_t uIndex) const { return reinterpret_cast<const Shape8BitOffset_t *>(reinterpret_cast<const uint8_t *>(this)+m_Index[uIndex]); } 
	static Shape8BitOffsetArray_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct LWShape_t {
	uint16_t Width;		/* Width of the shape */
	uint16_t Height;		/* Height of the shape */
	uint8_t Data[1];		/* Raw shape data */
	static LWShape_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct LWXShape_t {
	short XOffset;		/* Signed offset for x */
	short YOffset;		/* Signed offset for y */
	LWShape_t Shape;	/* Shape data */
	static LWXShape_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};

struct GfxShape_t {
	uint8_t Palette[768];	/* Palette for the shape */
	LWXShape_t XShape;	/* XShape for the actual data */
	static GfxShape_t * BURGER_API Load(RezFile *pRezFile,uint_t uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,uint_t uRezNum);
};
}
/* END */

#endif
