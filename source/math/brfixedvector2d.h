/***************************************

	2D fixed point vector manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFIXEDVECTOR2D_H__
#define __BRFIXEDVECTOR2D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector2D_t {
	Fixed32 x;			///< 32 bit fixed point X value for the 2D Vector
	Fixed32 y;			///< 32 bit fixed point Y value for the 2D Vector
	BURGER_INLINE void Zero(void) { x=0; y=0; }
	BURGER_INLINE void One(void) { x = 65536; y = 65536; }
	BURGER_INLINE Fixed32 GetX(void) const { return x; }
	BURGER_INLINE Fixed32 GetY(void) const { return y; }
	BURGER_INLINE void SetX(Fixed32 fX) { x=fX; }
	BURGER_INLINE void SetY(Fixed32 fY) { y=fY; }
	BURGER_INLINE void Set(Fixed32 fX,Fixed32 fY) { x=fX; y=fY; }
	BURGER_INLINE void Negate(void) { x = -x; y = -y; }
	BURGER_INLINE void Negate(Fixed32 fX,Fixed32 fY) { x = -fX; y = -fY; }
	BURGER_INLINE void Negate(const FixedVector2D_t *pInput) { x = -pInput->x; y = -pInput->y; }

};
}
/* END */

#endif
