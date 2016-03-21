/***************************************

	4D fixed point vector manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRFIXEDVECTOR4D_H__
#define __BRFIXEDVECTOR4D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector4D_t {
	Fixed32 x;			///< 32 bit fixed point X value for the 4D Vector
	Fixed32 y;			///< 32 bit fixed point Y value for the 4D Vector
	Fixed32 z;			///< 32 bit fixed point Z value for the 4D Vector
	Fixed32 w;			///< 32 bit fixed point W value for the 4D Vector
	BURGER_INLINE void Zero(void) { x = 0; y = 0; z = 0; w = 0; }
	BURGER_INLINE void One(void) { x = 65536; y = 65536; z = 65536; w = 65536; }
	BURGER_INLINE void Identity(void) { x = 0; y = 0; z = 0; w = FLOATTOFIXED(1.0f); }
	BURGER_INLINE Fixed32 GetX(void) const { return x; }
	BURGER_INLINE Fixed32 GetY(void) const { return y; }
	BURGER_INLINE Fixed32 GetZ(void) const { return z; }
	BURGER_INLINE Fixed32 GetW(void) const { return w; }
	BURGER_INLINE void SetX(Fixed32 fX) { x=fX; }
	BURGER_INLINE void SetY(Fixed32 fY) { y=fY; }
	BURGER_INLINE void SetZ(Fixed32 fZ) { z=fZ; }
	BURGER_INLINE void SetW(Fixed32 fW) { z=fW; }
	BURGER_INLINE void Set(Fixed32 fX,Fixed32 fY,Fixed32 fZ) { x = fX; y = fY; z = fZ; w = FLOATTOFIXED(1.0f); };
	BURGER_INLINE void Set(Fixed32 fX,Fixed32 fY,Fixed32 fZ,Fixed32 fW) { x=fX; y=fY; z=fZ; w=fW; }
	BURGER_INLINE void Set(const FixedVector4D_t *pInput) { *this = *pInput; }
	BURGER_INLINE void Negate(void) { x = -x; y = -y; z = -z; w = -w; }
	BURGER_INLINE void Negate(Fixed32 fX,Fixed32 fY,Fixed32 fZ,Fixed32 fW) { x = -fX; y = -fY; z = -fZ; w = -fW; }
	BURGER_INLINE void Negate(const FixedVector4D_t *pInput) { x = -pInput->x; y = -pInput->y; z = -pInput->z; w = -pInput->w; }
};
extern const FixedVector4D_t s_FixedVector4DZero;
extern const FixedVector4D_t s_FixedVector4DOne;
extern const FixedVector4D_t s_FixedVector4DOneX;
extern const FixedVector4D_t s_FixedVector4DOneY;
extern const FixedVector4D_t s_FixedVector4DOneZ;
extern const FixedVector4D_t s_FixedVector4DOneW;
}
/* END */

#endif
