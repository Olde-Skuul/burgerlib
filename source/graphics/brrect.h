/***************************************

	Integer 2 dimensional Rect handlers

	Copyright (c) 1995-2024 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRECT_H__
#define __BRRECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRPOINT2D_H__
#include "brpoint2d.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
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
struct tagRECT;
namespace Burger {
struct Rect_t {

	/** Leftmost x of rectangle */
	int32_t m_iLeft;
	/** Topmost y of rectangle */
	int32_t m_iTop;
	/** Rightmost x of rectangle */
	int32_t m_iRight;
	/** Bottommost y of rectangle */
	int32_t m_iBottom;

	BURGER_INLINE int32_t GetWidth(void) const BURGER_NOEXCEPT
	{
		return m_iRight - m_iLeft;
	}
	BURGER_INLINE int32_t GetHeight(void) const BURGER_NOEXCEPT
	{
		return m_iBottom - m_iTop;
	}
	BURGER_INLINE int32_t GetLeft(void) const BURGER_NOEXCEPT
	{
		return m_iLeft;
	}
	BURGER_INLINE int32_t GetTop(void) const BURGER_NOEXCEPT
	{
		return m_iTop;
	}
	BURGER_INLINE int32_t GetRight(void) const BURGER_NOEXCEPT
	{
		return m_iRight;
	}
	BURGER_INLINE int32_t GetBottom(void) const BURGER_NOEXCEPT
	{
		return m_iBottom;
	}
	BURGER_INLINE void SetWidth(int32_t iWidth) BURGER_NOEXCEPT
	{
		m_iRight = m_iLeft + iWidth;
	}
	BURGER_INLINE void SetHeight(int32_t iHeight) BURGER_NOEXCEPT
	{
		m_iBottom = m_iTop + iHeight;
	}
	BURGER_INLINE void SetSize(int32_t iWidth, int32_t iHeight) BURGER_NOEXCEPT
	{
		m_iRight = m_iLeft + iWidth;
		m_iBottom = m_iTop + iHeight;
	}
	BURGER_INLINE void SetLeft(int32_t iLeft) BURGER_NOEXCEPT
	{
		m_iLeft = iLeft;
	}
	BURGER_INLINE void SetTop(int32_t iTop) BURGER_NOEXCEPT
	{
		m_iTop = iTop;
	}
	BURGER_INLINE void SetRight(int32_t iRight) BURGER_NOEXCEPT
	{
		m_iRight = iRight;
	}
	BURGER_INLINE void SetBottom(int32_t iBottom) BURGER_NOEXCEPT
	{
		m_iBottom = iBottom;
	}
	BURGER_INLINE void Set(int32_t iLeft, int32_t iTop, int32_t iRight,
		int32_t iBottom) BURGER_NOEXCEPT
	{
		m_iLeft = iLeft;
		m_iTop = iTop;
		m_iRight = iRight;
		m_iBottom = iBottom;
	}
	BURGER_INLINE void SetBySize(int32_t iLeft, int32_t iTop, int32_t iWidth,
		int32_t iHeight) BURGER_NOEXCEPT
	{
		m_iLeft = iLeft;
		m_iTop = iTop;
		m_iRight = iLeft + iWidth;
		m_iBottom = iTop + iHeight;
	}
	BURGER_INLINE void Clear(void) BURGER_NOEXCEPT
	{
		m_iLeft = 0;
		m_iTop = 0;
		m_iRight = 0;
		m_iBottom = 0;
	}
	BURGER_INLINE void Offset(int32_t iX, int32_t iY) BURGER_NOEXCEPT
	{
		m_iLeft += iX;
		m_iTop += iY;
		m_iRight += iX;
		m_iBottom += iY;
	}

	int32_t CenterX(void) const BURGER_NOEXCEPT;
	int32_t CenterY(void) const BURGER_NOEXCEPT;
	void GetCenter(int32_t* pX, int32_t* pY) const BURGER_NOEXCEPT;
	void GetCenter(Point2D_t* pOutput) const BURGER_NOEXCEPT;
	void MoveTo(int32_t iX, int32_t iY) BURGER_NOEXCEPT;
	void MoveTo(const Point2D_t* pInput) BURGER_NOEXCEPT;
	void MoveX(int32_t iX) BURGER_NOEXCEPT;
	void MoveY(int32_t iY) BURGER_NOEXCEPT;
	void MoveWithinRect(const Rect_t* pInput) BURGER_NOEXCEPT;
	void CenterAroundX(int32_t iX) BURGER_NOEXCEPT;
	void CenterAroundX(const Point2D_t* pInput) BURGER_NOEXCEPT;
	void CenterAroundX(const Rect_t* pInput) BURGER_NOEXCEPT;
	void CenterAroundY(int32_t iY) BURGER_NOEXCEPT;
	void CenterAroundY(const Point2D_t* pInput) BURGER_NOEXCEPT;
	void CenterAroundY(const Rect_t* pInput) BURGER_NOEXCEPT;
	void CenterAround(int32_t iX, int32_t iY) BURGER_NOEXCEPT;
	void CenterAround(const Point2D_t* pInput) BURGER_NOEXCEPT;
	void CenterAround(const Rect_t* pInput) BURGER_NOEXCEPT;
	uint_t IsInside(int32_t iX, int32_t iY) const BURGER_NOEXCEPT;
	uint_t IsInside(const Point2D_t* pPoint) const BURGER_NOEXCEPT;
	uint_t IsInside(const Rect_t* pRect) const BURGER_NOEXCEPT;
	void Sort(void) BURGER_NOEXCEPT;
	void SetSorted(int32_t iLeft, int32_t iTop, int32_t iRight,
		int32_t iBottom) BURGER_NOEXCEPT;
	uint_t IsEmpty(void) const BURGER_NOEXCEPT;
	uint_t IsNotEmpty(void) const BURGER_NOEXCEPT;
	uint_t IsEqual(const Rect_t* pInput) const BURGER_NOEXCEPT;
	uint_t IsNotEqual(const Rect_t* pInput) const BURGER_NOEXCEPT;
	BURGER_INLINE uint_t operator==(const Rect_t& rInput) const BURGER_NOEXCEPT
	{
		return IsEqual(&rInput);
	}
	BURGER_INLINE uint_t operator!=(const Rect_t& rInput) const BURGER_NOEXCEPT
	{
		return IsNotEqual(&rInput);
	}
	void Inset(int32_t iX, int32_t iY) BURGER_NOEXCEPT;
	uint_t IsIntersecting(const Rect_t* pInput) const BURGER_NOEXCEPT;
	uint_t Intersection(
		const Rect_t* pInput1, const Rect_t* pInput2) BURGER_NOEXCEPT;
	void Union(const Rect_t* pInput1, const Rect_t* pInput2) BURGER_NOEXCEPT;
	void Add(const Point2D_t* pInput) BURGER_NOEXCEPT;
	void Add(int32_t iX, int32_t iY) BURGER_NOEXCEPT;
	void Clip(const Rect_t* pInput) BURGER_NOEXCEPT;

#if defined(BURGER_MACOS) || defined(DOXYGEN)
	void Get(Rect* pOutput) const BURGER_NOEXCEPT;
	void Set(const Rect* pInput) BURGER_NOEXCEPT;
#endif

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	void Get(CGRect* pOutput) const BURGER_NOEXCEPT;
	void Set(const CGRect* pInput) BURGER_NOEXCEPT;

#if (!defined(BURGER_64BITCPU) && defined(BURGER_MACOSX)) || defined(DOXYGEN)
	void Get(_NSRect* pOutput) const BURGER_NOEXCEPT;
	void Set(const _NSRect* pInput) BURGER_NOEXCEPT;
#endif
#endif

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void Get(tagRECT* pOutput) const BURGER_NOEXCEPT;
	void Set(const tagRECT* pInput) BURGER_NOEXCEPT;
#endif
};

extern eError BURGER_API get(
	Rect* pRect, InputMemoryStream* pInput) BURGER_NOEXCEPT;
extern eError BURGER_API append(
	OutputMemoryStream* pOutput, const Rect* pRect) BURGER_NOEXCEPT;

}
/* END */

#endif
