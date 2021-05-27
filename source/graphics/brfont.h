/***************************************

    Font base class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
    BURGER_DISABLE_COPY(Font);
protected:
	Display *m_pDisplay;	///< Rendering context to draw into
	int m_iX;				///< X coord to draw the font
	int m_iY;				///< Y coord to draw the font
	uint_t m_uHeight;			///< Height of the font in pixels
	uint_t m_uFirst;			///< First allowable UTF32 code that can be drawn
	uint_t m_uCount;			///< Number of characters in the font
public:
	Font(Display *pDisplay=NULL);
	virtual ~Font();
	virtual void Draw(const char *pInput,uintptr_t uLength);
	virtual uint_t GetPixelWidth(const char *pInput,uintptr_t uLength);
	virtual void DrawChar(uint_t uLetter);
	BURGER_INLINE void SetDisplay(Display *pDisplay) { m_pDisplay = pDisplay; }
	BURGER_INLINE Display *GetDisplay(void) const { return m_pDisplay; }
	BURGER_INLINE void SetX(int iX) { m_iX = iX; }
	BURGER_INLINE void SetY(int iY) { m_iY = iY; }
	BURGER_INLINE void SetXY(int iX,int iY) { m_iX = iX; m_iY = iY; }
	BURGER_INLINE int GetX(void) const { return m_iX; }
	BURGER_INLINE int GetY(void) const { return m_iY; }
	BURGER_INLINE uint_t GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE uint_t GetFirstChar(void) const { return m_uFirst; }
	BURGER_INLINE uint_t GetCount(void) const { return m_uCount; }
	uint_t BURGER_API GetPixelWidthChar(uint_t uLetter);
	uint_t BURGER_API GetPixelWidthNumber(int32_t iInput);
	uint_t BURGER_API GetPixelWidthNumber(uint32_t uInput);
	uint_t BURGER_API GetPixelWidthString(const char *pInput);
	void BURGER_API DrawNumber(int32_t iInput);
	void BURGER_API DrawNumber(uint32_t uInput);
	void BURGER_API DrawString(const char *pInput);
	void BURGER_API DrawStringCenterX(int iX,int iY,const char *pInput);
	void BURGER_API DrawStringAtXY(int iX,int iY,const char *pInput);
	uintptr_t BURGER_API CharsForPixelWidth(const char *pInput,uint_t uWidth);
//	uint_t WidthListWidest(struct LinkedList_t *ListPtr);
};
}
/* END */

#endif
