/***************************************

	Integer Point handlers

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	int	m_iX;		///< X coordinate of the point
	int m_iY;		///< Y coordinate of the point
	BURGER_INLINE int GetX(void) const { return m_iX; }
	BURGER_INLINE int GetY(void) const { return m_iY; }
	BURGER_INLINE int *GetXPtr(void) { return &m_iX; }
	BURGER_INLINE int *GetYPtr(void) { return &m_iY; }
	BURGER_INLINE void SetX(int iX) { m_iX = iX; }
	BURGER_INLINE void SetY(int iY) { m_iY = iY; }
	BURGER_INLINE void Set(int iX,int iY) { m_iX = iX; m_iY = iY; }
	BURGER_INLINE void Clear(void) { m_iX = 0; m_iY = 0; }
	Word Read(FILE *fp);
	Word Write(FILE *fp) const;
	Word Read(Burger::File *fp);
	Word Write(Burger::File *fp) const;
#if (defined(BURGER_MAC) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
	void Get(Point *pOutput) const;
	void Set(const Point *pInput);
#endif
#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	void Get(tagPOINT *pOutput) const;
	void Set(const tagPOINT *pInput);
#endif
};
}
/* END */

#endif

