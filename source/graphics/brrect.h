/***************************************

	Integer Rect handlers

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRECT_H__
#define __BRRECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRPOINT2D_H__
#include "brpoint2d.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
struct Rect_t {
	int m_iLeft;	///< Leftmost x of rectangle
	int m_iTop;		///< Topmost y of rectangle
	int m_iRight;	///< Rightmost x of rectangle
	int m_iBottom;	///< Bottommost y of rectangle
	BURGER_INLINE int GetWidth(void) const { return (m_iRight-m_iLeft); }
	BURGER_INLINE int GetHeight(void) const { return (m_iBottom-m_iTop); }
	BURGER_INLINE int GetLeft(void) const { return m_iLeft; }
	BURGER_INLINE int GetTop(void) const { return m_iTop; }
	BURGER_INLINE int GetRight(void) const { return m_iRight; }
	BURGER_INLINE int GetBottom(void) const { return m_iBottom; }
	BURGER_INLINE void SetWidth(int iWidth) { m_iRight = m_iLeft+iWidth; }
	BURGER_INLINE void SetHeight(int iHeight) { m_iBottom = m_iTop+iHeight; }
	BURGER_INLINE void SetSize(int iWidth,int iHeight) { m_iRight = m_iLeft+iWidth; m_iBottom = m_iTop+iHeight; }
	BURGER_INLINE void SetLeft(int iLeft) { m_iLeft = iLeft; }
	BURGER_INLINE void SetTop(int iTop) { m_iTop = iTop; }
	BURGER_INLINE void SetRight(int iRight) { m_iRight = iRight; }
	BURGER_INLINE void SetBottom(int iBottom) { m_iBottom = iBottom; }
	BURGER_INLINE void Set(int iLeft,int iTop,int iRight,int iBottom) { m_iLeft = iLeft; m_iTop = iTop; m_iRight = iRight; m_iBottom = iBottom; }
	BURGER_INLINE void SetBySize(int iLeft,int iTop,int iWidth,int iHeight) { m_iLeft = iLeft; m_iTop = iTop; m_iRight = iLeft+iWidth; m_iBottom = iTop+iHeight; }
	BURGER_INLINE void Clear(void) { m_iLeft = 0; m_iTop = 0; m_iRight = 0; m_iBottom = 0; }
	BURGER_INLINE void Offset(int iX,int iY) { m_iLeft+=iX; m_iTop+=iY; m_iRight+=iX; m_iBottom+=iY; }
	void MoveTo(int iX,int iY);
	void MoveTo(const Point2D_t *pInput);
	void MoveX(int iX);
	void MoveY(int iY);
	Word IsInside(int iX,int iY) const;
	Word IsInside(const Point2D_t *pPoint) const;
	Word IsInside(const Rect_t *pRect) const;
	void Sort(void);
	void SetSorted(int iLeft,int iTop,int iRight,int iBottom);
	Word IsEmpty(void) const;
	Word IsNotEmpty(void) const;
	Word IsEqual(const Rect_t *pInput) const;
	Word IsNotEqual(const Rect_t *pInput) const;
	BURGER_INLINE Word operator==(const Rect_t &rInput) const { return IsEqual(&rInput); }
	BURGER_INLINE Word operator!=(const Rect_t &rInput) const { return IsNotEqual(&rInput); }
	void Inset(int iX,int iY);
	Word IsIntersecting(const Rect_t *pInput) const;
	Word Intersection(const Rect_t *pInput1,const Rect_t *pInput2);
	void Union(const Rect_t *pInput1,const Rect_t *pInput2);
	void Add(const Point2D_t *pInput);
	void Add(int iX,int iY);
	void Clip(const Rect_t *pInput);
#if defined(BURGER_MACOS) || defined(DOXYGEN)
	void Get(Rect *pOutput) const;
	void Set(const Rect *pInput);
#endif
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	void Get(CGRect *pOutput) const;
	void Set(const CGRect *pInput);
#if (!defined(BURGER_64BITCPU) && defined(BURGER_MACOSX)) || defined(DOXYGEN)
	void Get(_NSRect *pOutput) const;
	void Set(const _NSRect *pInput);
#endif
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void Get(tagRECT *pOutput) const;
	void Set(const tagRECT *pInput);
#endif
};
}
/* END */

#endif

