/***************************************

	3D fixed point vector manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFIXEDVECTOR3D_H__
#define __BRFIXEDVECTOR3D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector3D_t {
	Fixed32 x;			///< X value for the 3D Vector
	Fixed32 y;			///< Y value for the 3D Vector
	Fixed32 z;			///< Z value for the 3D Vector
	BURGER_INLINE void Zero(void) { x=0; y=0; z=0; }
	BURGER_INLINE void One(void) { x = 65536; y = 65536; z = 65536; }
	BURGER_INLINE Fixed32 GetX(void) const { return x; }
	BURGER_INLINE Fixed32 GetY(void) const { return y; }
	BURGER_INLINE Fixed32 GetZ(void) const { return z; }
	BURGER_INLINE void SetX(Fixed32 fX) { x=fX; }
	BURGER_INLINE void SetY(Fixed32 fY) { y=fY; }
	BURGER_INLINE void SetZ(Fixed32 fZ) { z=fZ; }
	BURGER_INLINE void Set(Fixed32 fX,Fixed32 fY,Fixed32 fZ) { x=fX; y=fY; z=fZ; }
	BURGER_INLINE void Set(const FixedVector3D_t *pInput) { *this = *pInput; }
	void BURGER_API Set(const Vector3D_t *pInput);
	void BURGER_API SetAsInt(const Vector3D_t *pInput);
	void BURGER_API Negate(void);
	void BURGER_API Negate(const FixedVector3D_t *pInput);
	void BURGER_API Add(const FixedVector3D_t *pInput);
	void BURGER_API Add(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2);
	void BURGER_API Sub(const FixedVector3D_t *pInput);
	void BURGER_API Sub(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2);
	Fixed32 BURGER_API Dot(const FixedVector3D_t *pInput) const;
	void BURGER_API Cross(const FixedVector3D_t *pInput1,const FixedVector3D_t *pInput2);
};
extern const FixedVector3D_t s_FixedVector3DZero;
extern const FixedVector3D_t s_FixedVector3DOne;
extern const FixedVector3D_t s_FixedVector3DOneX;
extern const FixedVector3D_t s_FixedVector3DOneY;
extern const FixedVector3D_t s_FixedVector3DOneZ;
}
/* END */

#endif
