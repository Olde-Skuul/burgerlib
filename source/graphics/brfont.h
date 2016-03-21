/***************************************

	Font base class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRRENDERER_H__
#include "brrenderer.h"
#endif

/* BEGIN */
namespace Burger {
class Font : public Base {
	BURGER_RTTI_IN_CLASS();		// Handle RTTI
	BURGER_DISABLECOPYCONSTRUCTORS(Font);
protected:
	Display *m_pDisplay;	///< Rendering context to draw into
	int m_iX;				///< X coord to draw the font
	int m_iY;				///< Y coord to draw the font
	Word m_uHeight;			///< Height of the font in pixels
	Word m_uFirst;			///< First allowable UTF32 code that can be drawn
	Word m_uCount;			///< Number of characters in the font
public:
	Font(Display *pDisplay=NULL);
	virtual ~Font();
	virtual void Draw(const char *pInput,WordPtr uLength);
	virtual Word GetPixelWidth(const char *pInput,WordPtr uLength);
	virtual void DrawChar(Word uLetter);
	BURGER_INLINE void SetDisplay(Display *pDisplay) { m_pDisplay = pDisplay; }
	BURGER_INLINE Display *GetDisplay(void) const { return m_pDisplay; }
	BURGER_INLINE void SetX(int iX) { m_iX = iX; }
	BURGER_INLINE void SetY(int iY) { m_iY = iY; }
	BURGER_INLINE void SetXY(int iX,int iY) { m_iX = iX; m_iY = iY; }
	BURGER_INLINE int GetX(void) const { return m_iX; }
	BURGER_INLINE int GetY(void) const { return m_iY; }
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE Word GetFirstChar(void) const { return m_uFirst; }
	BURGER_INLINE Word GetCount(void) const { return m_uCount; }
	Word BURGER_API GetPixelWidthChar(Word uLetter);
	Word BURGER_API GetPixelWidthNumber(Int32 iInput);
	Word BURGER_API GetPixelWidthNumber(Word32 uInput);
	Word BURGER_API GetPixelWidthString(const char *pInput);
	void BURGER_API DrawNumber(Int32 iInput);
	void BURGER_API DrawNumber(Word32 uInput);
	void BURGER_API DrawString(const char *pInput);
	void BURGER_API DrawStringCenterX(int iX,int iY,const char *pInput);
	void BURGER_API DrawStringAtXY(int iX,int iY,const char *pInput);
	WordPtr BURGER_API CharsForPixelWidth(const char *pInput,Word uWidth);
//	Word WidthListWidest(struct LinkedList_t *ListPtr);
};
}
/* END */

#endif
