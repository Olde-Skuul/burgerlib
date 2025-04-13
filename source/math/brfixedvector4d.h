/***************************************

    4D fixed point vector manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	fixed16_16_t x;			///< 32 bit fixed point X value for the 4D Vector
	fixed16_16_t y;			///< 32 bit fixed point Y value for the 4D Vector
	fixed16_16_t z;			///< 32 bit fixed point Z value for the 4D Vector
	fixed16_16_t w;			///< 32 bit fixed point W value for the 4D Vector
	BURGER_INLINE void Zero(void) { x = 0; y = 0; z = 0; w = 0; }
	BURGER_INLINE void One(void) { x = 65536; y = 65536; z = 65536; w = 65536; }
	BURGER_INLINE void Identity(void) { x = 0; y = 0; z = 0; w = BURGER_FLOAT_TO_FIXED(1.0f); }
	BURGER_INLINE fixed16_16_t GetX(void) const { return x; }
	BURGER_INLINE fixed16_16_t GetY(void) const { return y; }
	BURGER_INLINE fixed16_16_t GetZ(void) const { return z; }
	BURGER_INLINE fixed16_16_t GetW(void) const { return w; }
	BURGER_INLINE void SetX(fixed16_16_t fX) { x=fX; }
	BURGER_INLINE void SetY(fixed16_16_t fY) { y=fY; }
	BURGER_INLINE void SetZ(fixed16_16_t fZ) { z=fZ; }
	BURGER_INLINE void SetW(fixed16_16_t fW) { z=fW; }
	BURGER_INLINE void Set(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ) { x = fX; y = fY; z = fZ; w = BURGER_FLOAT_TO_FIXED(1.0f); };
	BURGER_INLINE void Set(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ,fixed16_16_t fW) { x=fX; y=fY; z=fZ; w=fW; }
	BURGER_INLINE void Set(const FixedVector4D_t *pInput) { *this = *pInput; }
	BURGER_INLINE void Negate(void) { x = -x; y = -y; z = -z; w = -w; }
	BURGER_INLINE void Negate(fixed16_16_t fX,fixed16_16_t fY,fixed16_16_t fZ,fixed16_16_t fW) { x = -fX; y = -fY; z = -fZ; w = -fW; }
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
