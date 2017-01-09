/***************************************

	Display base class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	Word16 m_usWidth;	///< Width of the shape
	Word16 m_usHeight;	///< Height of the shape
	Word8 m_Data[1];	///< Raw shape data
	BURGER_INLINE Word GetWidth(void) const { return m_usWidth; }
	BURGER_INLINE Word GetHeight(void) const { return m_usHeight; }
	BURGER_INLINE const Word8 *GetPixels(void) const { return m_Data; }
	BURGER_INLINE void Draw(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixels(iX,iY,GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	BURGER_INLINE void DrawMasked(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixelsMasked(iX,iY,GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	static Shape8Bit_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct Shape8BitOffset_t {
	short m_sXOffset;			///< Signed offset for x
	short m_sYOffset;			///< Signed offset for y
	Shape8Bit_t m_Shape8Bit;	///< Shape data
	BURGER_INLINE Word GetWidth(void) const { return m_Shape8Bit.GetWidth(); }
	BURGER_INLINE Word GetHeight(void) const { return m_Shape8Bit.GetHeight(); }
	BURGER_INLINE const Word8 *GetPixels(void) const { return m_Shape8Bit.GetPixels(); }
	BURGER_INLINE int GetXOffset(void) const { return m_sXOffset; }
	BURGER_INLINE int GetYOffset(void) const { return m_sYOffset; }
	BURGER_INLINE void Draw(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixels(iX+GetXOffset(),iY+GetYOffset(),GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	BURGER_INLINE void DrawMasked(Renderer *pRenderer,int iX,int iY) const { pRenderer->Draw8BitPixelsMasked(iX+GetXOffset(),iY+GetYOffset(),GetWidth(),GetHeight(),GetWidth(),GetPixels()); }
	static Shape8BitOffset_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct Shape8BitArray_t {
	Word32 m_Index[1];			///< Array of indexs to the shape array
	BURGER_INLINE Shape8Bit_t *GetShape(Word uIndex) { return reinterpret_cast<Shape8Bit_t *>(reinterpret_cast<Word8 *>(this)+m_Index[uIndex]); } 
	BURGER_INLINE const Shape8Bit_t *GetShape(Word uIndex) const { return reinterpret_cast<const Shape8Bit_t *>(reinterpret_cast<const Word8 *>(this)+m_Index[uIndex]); } 
	static Shape8BitArray_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct Shape8BitOffsetArray_t {
	Word32 m_Index[1];			///< Array of indexs to the shape array
	BURGER_INLINE Shape8BitOffset_t *GetShape(Word uIndex) { return reinterpret_cast<Shape8BitOffset_t *>(reinterpret_cast<Word8 *>(this)+m_Index[uIndex]); } 
	BURGER_INLINE const Shape8BitOffset_t *GetShape(Word uIndex) const { return reinterpret_cast<const Shape8BitOffset_t *>(reinterpret_cast<const Word8 *>(this)+m_Index[uIndex]); } 
	static Shape8BitOffsetArray_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct LWShape_t {
	Word16 Width;		/* Width of the shape */
	Word16 Height;		/* Height of the shape */
	Word8 Data[1];		/* Raw shape data */
	static LWShape_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct LWXShape_t {
	short XOffset;		/* Signed offset for x */
	short YOffset;		/* Signed offset for y */
	LWShape_t Shape;	/* Shape data */
	static LWXShape_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};

struct GfxShape_t {
	Word8 Palette[768];	/* Palette for the shape */
	LWXShape_t XShape;	/* XShape for the actual data */
	static GfxShape_t * BURGER_API Load(RezFile *pRezFile,Word uRezNum);
	static void BURGER_API Preload(RezFile *pRezFile,Word uRezNum);
};
}
/* END */

#endif
