/***************************************

	Font base class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFONT_H__
#define __BRFONT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRENDERER_H__
#include "brrenderer.h"
#endif

/* BEGIN */
namespace Burger {
class Font {
	BURGER_DISABLECOPYCONSTRUCTORS(Font);
public:
	Font(Renderer *pRenderer=NULL);
	virtual ~Font();
	virtual void Draw(const char *pInput,WordPtr uLength);
	virtual Word GetPixelWidth(const char *pInput,WordPtr uLength);
	virtual void DrawChar(Word uLetter);
	BURGER_INLINE void SetRenderer(Renderer *pRenderer) { m_pRenderer = pRenderer; }
	BURGER_INLINE Renderer *GetRenderer(void) const { return m_pRenderer; }
	BURGER_INLINE void SetX(int iX) { m_iX = iX; }
	BURGER_INLINE void SetY(int iY) { m_iY = iY; }
	BURGER_INLINE void SetXY(int iX,int iY) { m_iX = iX; m_iY = iY; }
	BURGER_INLINE int GetX(void) const { return m_iX; }
	BURGER_INLINE int GetY(void) const { return m_iY; }
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE Word GetFirstChar(void) const { return m_uFirst; }
	BURGER_INLINE Word GetCount(void) const { return m_uCount; }
	Word GetPixelWidthChar(Word uLetter);
	Word GetPixelWidthNumber(Int32 iInput);
	Word GetPixelWidthNumber(Word32 uInput);
	Word GetPixelWidthString(const char *pInput);
	void DrawNumber(Int32 iInput);
	void DrawNumber(Word32 uInput);
	void DrawString(const char *pInput);
	void DrawStringCenterX(int iX,int iY,const char *pInput);
	void DrawStringAtXY(int iX,int iY,const char *pInput);
	WordPtr CharsForPixelWidth(const char *pInput,Word uWidth);
//	Word WidthListWidest(struct LinkedList_t *ListPtr);
protected:
	Renderer *m_pRenderer;	///< Rendering context to draw into
	int m_iX;				///< X coord to draw the font
	int m_iY;				///< Y coord to draw the font
	Word m_uHeight;			///< Height of the font in pixels
	Word m_uFirst;			///< First allowable UTF32 code that can be drawn
	Word m_uCount;			///< Number of characters in the font
};
}
/* END */

#endif
