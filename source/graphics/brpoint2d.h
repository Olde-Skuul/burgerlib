/***************************************

    Integer Point handlers

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPOINT2D_H__
#define __BRPOINT2D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_IOS) && !defined(__BRIOSTYPES_H__)
#include "briostypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

#ifndef __BRFILE_H__
#include "brfile.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {
struct Point2D_t {
	int32_t	m_iX;		///< X coordinate of the point
	int32_t m_iY;		///< Y coordinate of the point
	BURGER_INLINE int32_t GetX(void) const { return m_iX; }
	BURGER_INLINE int32_t GetY(void) const { return m_iY; }
	BURGER_INLINE int32_t *GetXPtr(void) { return &m_iX; }
	BURGER_INLINE int32_t *GetYPtr(void) { return &m_iY; }
	BURGER_INLINE void SetX(int32_t iX) { m_iX = iX; }
	BURGER_INLINE void SetY(int32_t iY) { m_iY = iY; }
	BURGER_INLINE void Set(int32_t iX,int32_t iY) { m_iX = iX; m_iY = iY; }
	BURGER_INLINE void Clear(void) { m_iX = 0; m_iY = 0; }
	uint_t Read(FILE *fp);
	uint_t Write(FILE *fp) const;
	uint_t Read(Burger::File *fp);
	uint_t Write(Burger::File *fp) const;
#if (defined(BURGER_MACOS)) || defined(DOXYGEN)
	void Get(Point *pOutput) const;
	void Set(const Point *pInput);
#endif
#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	void Get(CGPoint *pOutput) const;
	void Set(const CGPoint *pInput);
#if (!defined(BURGER_64BITCPU) && defined(BURGER_MACOSX)) || defined(DOXYGEN)
	void Get(_NSPoint *pOutput) const;
	void Set(const _NSPoint *pInput);
#endif
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void Get(tagPOINT *pOutput) const;
	void Set(const tagPOINT *pInput);
#endif
};
}
/* END */

#endif

